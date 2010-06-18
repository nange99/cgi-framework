/*
 * cgi_upload.c
 *
 *  Created on: Jun 17, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "cgi_log.h"
#include "util/list.h"
#include "util/hashtable.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_priv.h"
#include "cgi_upload.h"

#define CGI_DEBUG_FILE_UPLOAD 1

#define MAX_SIZE_ANONNAME 33

static char upload_err_str[7][43] = {
	"File upload successful",
	"Uploaded file exceeded upload_max_filesize",
	"Uploaded file exceeded MAX_FILE_SIZE",
	"Partially uploaded",
	"No file uploaded",
	"Missing /tmp or similar directory",
	"Failed to write file to disk"
};

char *cgi_upload_get_error_str(enum upload_err err)
{
	return (char *)&upload_err_str[err];
}

void cgi_upload_file_destroy(cgi_file *f)
{
	if (f == NULL)
		return;

	if (f->filename)
		free(f->filename);

	if (f->tmp_filename)
		free(f->tmp_filename);

	if (f->type)
		free(f->type);

	free(f);
}

cgi_file *cgi_upload_get_file(struct request *req, const char *key)
{
	struct _cgi_object *o;

	o = (struct _cgi_object *) htable_lookup(req->files, (char *)key);

	return (cgi_file *)o->value.u_hash;
}

static int cgi_upload_add_file (struct request *req, const char *field, struct _cgi_file *file)
{
	cgi_object *o;
	o = malloc(sizeof(cgi_object));

	if (o == NULL)
		return -1;

	o->type = CGI_FILE;
	o->value.u_hash = file;

	htable_insert(req->files, (char *)field, o);
	return 0;
}

/* This is heavily based on Apache's libapreq and PHP5 RFC1867 implementation */
#define FILLUNIT (1024 * 5)

typedef struct {
	/* read buffer */
	char *buffer;
	char *buf_begin;
	int bufsize;
	int bytes_in_buffer;

	/* boundary info */
	char *boundary;
	char *boundary_next;
	int boundary_next_len;
} multipart_buffer;

typedef struct {
	struct list_head list;
	char *key;
	char *value;
} mime_headers;

static mime_headers *mime_header_init(void)
{
	mime_headers *m;

	m = malloc(sizeof(mime_headers));

	INIT_LIST_HEAD(&m->list);

	return m;
}

static int mime_header_count(mime_headers *h)
{
	mime_headers *entry;
	struct list_head *pos;
	int i;

	list_for_each (pos, &h->list) {
		entry = list_entry(pos, mime_headers, list);
		if (entry != NULL)
			i++;
	}

	return i;
}

static char *mime_get_hdr_value(mime_headers *header, char *key)
{
	mime_headers *entry;
	struct list_head *pos;

	if (key == NULL) {
		return NULL;
	}

	list_for_each (pos, &header->list) {
		entry = list_entry(pos, mime_headers, list);
		if (!strcasecmp(entry->key, key)) {
			return entry->value;
		}
	}

	return NULL;
}

/* Free header entry */
static void free_mime_hdr_entry(mime_headers *h)
{
	if (h->key) {
		free(h->key);
	}
	if (h->value) {
		free(h->value);
	}
}

static void mime_header_list_destroy(mime_headers *h)
{
	mime_headers *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &h->list) {
		tmp = list_entry(pos, mime_headers, list);
		list_del(pos);
		free_mime_hdr_entry(tmp);
		free(tmp);
	}

	free(h);
}

static void mime_header_list_clean(mime_headers *h)
{
	mime_headers *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &h->list) {
		tmp = list_entry(pos, mime_headers, list);
		list_del(pos);
		free_mime_hdr_entry(tmp);
		free(tmp);
	}
	return;
}

/*
 fill up the buffer with client data.
 returns number of bytes added to buffer.
 */
static int fill_buffer(multipart_buffer *self)
{
	int bytes_to_read, total_read = 0, actual_read = 0;

	/* shift the existing data if necessary */
	if (self->bytes_in_buffer > 0 && self->buf_begin != self->buffer) {
		memmove(self->buffer, self->buf_begin, self->bytes_in_buffer);
	}

	self->buf_begin = self->buffer;

	/* calculate the free space in the buffer */
	bytes_to_read = self->bufsize - self->bytes_in_buffer;

	/* read the required number of bytes */
	while (bytes_to_read > 0) {

		char *buf = self->buffer + self->bytes_in_buffer;

		//actual_read = sapi_module.read_post(buf, bytes_to_read);
		actual_read = read(fileno(stdin), buf, bytes_to_read);

		/* update the buffer length */
		if (actual_read > 0) {
			self->bytes_in_buffer += actual_read;
			total_read += actual_read;
			bytes_to_read -= actual_read;
		} else {
			break;
		}
	}

	return total_read;
}

/* eof if we are out of bytes, or if we hit the final boundary */
static int multipart_buffer_eof(multipart_buffer *self)
{
	if ((self->bytes_in_buffer == 0 && fill_buffer(self) < 1)) {
		return 1;
	} else {
		return 0;
	}
}

/* create new multipart_buffer structure */
static multipart_buffer *multipart_buffer_new(char *boundary, int boundary_len)
{
	multipart_buffer *self = (multipart_buffer *) calloc(1, sizeof(multipart_buffer));

	int minsize = boundary_len + 6;
	if (minsize < FILLUNIT)
		minsize = FILLUNIT;

	self->buffer = (char *) calloc(1, minsize + 1);
	self->bufsize = minsize;

	self->boundary = (char *) calloc(1, boundary_len + 3);
	sprintf(self->boundary, "--%s", boundary);

	self->boundary_next = (char *) calloc(1, boundary_len + 4);
	sprintf(self->boundary_next, "\n--%s", boundary);
	self->boundary_next_len = boundary_len + 3;

	self->buf_begin = self->buffer;
	self->bytes_in_buffer = 0;

	return self;
}

/*
 gets the next CRLF terminated line from the input buffer.
 if it doesn't find a CRLF, and the buffer isn't completely full, returns
 NULL; otherwise, returns the beginning of the null-terminated line,
 minus the CRLF.

 note that we really just look for LF terminated lines. this works
 around a bug in internet explorer for the macintosh which sends mime
 boundaries that are only LF terminated when you use an image submit
 button in a multipart/form-data form.
 */
static char *next_line(multipart_buffer *self)
{
	/* look for LF in the data */
	char* line = self->buf_begin;
	char* ptr = memchr(self->buf_begin, '\n', self->bytes_in_buffer);

	if (ptr) { /* LF found */

		/* terminate the string, remove CRLF */
		if ((ptr - line) > 0 && *(ptr - 1) == '\r') {
			*(ptr - 1) = 0;
		} else {
			*ptr = 0;
		}

		/* bump the pointer */
		self->buf_begin = ptr + 1;
		self->bytes_in_buffer -= (self->buf_begin - line);

	} else { /* no LF found */

		/* buffer isn't completely full, fail */
		if (self->bytes_in_buffer < self->bufsize) {
			return NULL;
		}
		/* return entire buffer as a partial line */
		line[self->bufsize] = 0;
		self->buf_begin = ptr;
		self->bytes_in_buffer = 0;
	}

	return line;
}

/* returns the next CRLF terminated line from the client */
static char *get_line(multipart_buffer *self)
{
	char* ptr = next_line(self);

	if (!ptr) {
		fill_buffer(self);
		ptr = next_line(self);
	}

	return ptr;
}

/* finds a boundary */
static int find_boundary(multipart_buffer *self, char *boundary)
{
	char *line;

	/* loop thru lines */
	while ((line = get_line(self))) {
		/* finished if we found the boundary */
		if (!strcmp(line, boundary)) {
			return 1;
		}
	}

	/* didn't find the boundary */
	return 0;
}

/* parse headers */
static int multipart_buffer_headers(multipart_buffer *self, mime_headers *header)
{
	char *line;
	mime_headers *prev_entry, *entry;
	int prev_len, cur_len;

	/* didn't find boundary, abort */
	if (!find_boundary(self, self->boundary)) {
		return 0;
	}

	/* get lines of text, or CRLF_CRLF */

	while ((line = get_line(self)) && strlen(line) > 0) {
		/* add header to table */

		char *key = line;
		char *value = NULL;

		/* space in the beginning means same header */
		if (!isspace(line[0])) {
			value = strchr(line, ':');
		}

		if (value) {
			*value = 0;
			do {
				value++;
			} while (isspace(*value));

			entry = mime_header_init();

			entry->value = strdup(value);
			entry->key = strdup(key);

		} else if (mime_header_count(header)) { /* If no ':' on the line, add to previous line */

			prev_len = strlen(prev_entry->value);
			cur_len = strlen(line);

			entry = mime_header_init();

			entry->value = malloc(prev_len + cur_len + 1);
			memcpy(entry->value, prev_entry->value, prev_len);
			memcpy(entry->value + prev_len, line, cur_len);
			entry->value[cur_len + prev_len] = '\0';

			entry->key = strdup(prev_entry->key);

			list_del(&prev_entry->list);
			free_mime_hdr_entry(prev_entry);
			free(prev_entry);
		} else {
			continue;
		}

		list_add_tail(&(entry->list), &(header->list));
		prev_entry = entry;
	}

	return 1;
}

static char *cgi_ap_getword(char **line, char stop)
{
	char *pos = *line, quote;
	char *res;

	while (*pos && *pos != stop) {

		if ((quote = *pos) == '"' || quote == '\'') {
			++pos;
			while (*pos && *pos != quote) {
				if (*pos == '\\' && pos[1] && pos[1] == quote) {
					pos += 2;
				} else {
					++pos;
				}
			}
			if (*pos) {
				++pos;
			}
		} else
			++pos;

	}
	if (*pos == '\0') {
		res = strdup(*line);
		*line += strlen(*line);
		return res;
	}

	res = strndup(*line, pos - *line);

	while (*pos == stop) {
		++pos;
	}

	*line = pos;
	return res;
}

static char *substring_conf(char *start, int len, char quote)
{
	char *result = malloc(len + 2);
	char *resp = result;
	int i;

	for (i = 0; i < len; ++i) {
		if (start[i] == '\\' && (start[i + 1] == '\\' || (quote
		                && start[i + 1] == quote))) {
			*resp++ = start[++i];
		} else {
			*resp++ = start[i];
		}
	}

	*resp++ = '\0';
	return result;
}

static char *cgi_ap_getword_conf(char **line)
{
	char *str = *line, *strend, *res, quote;

	while (*str && isspace(*str)) {
		++str;
	}

	if (!*str) {
		*line = str;
		return strdup("");
	}

	if ((quote = *str) == '"' || quote == '\'') {
		strend = str + 1;
		look_for_quote: while (*strend && *strend != quote) {
			if (*strend == '\\' && strend[1] && strend[1] == quote) {
				strend += 2;
			} else {
				++strend;
			}
		}
		if (*strend && *strend == quote) {
			char p = *(strend + 1);
			if (p != '\r' && p != '\n' && p != '\0') {
				strend++;
				goto look_for_quote;
			}
		}

		res = substring_conf(str + 1, strend - str - 1, quote);

		if (*strend == quote) {
			++strend;
		}

	} else {

		strend = str;
		while (*strend && !isspace(*strend)) {
			++strend;
		}
		res = substring_conf(str, strend - str, 0);
	}

	while (*strend && isspace(*strend)) {
		++strend;
	}

	*line = strend;
	return res;
}

/*
 search for a string in a fixed-length byte string.
 if partial is true, partial matches are allowed at the end of the buffer.
 returns NULL if not found, or a pointer to the start of the first match.
 */
static void *cgi_ap_memstr(char *haystack,
                           int haystacklen,
                           char *needle,
                           int needlen,
                           int partial)
{
	int len = haystacklen;
	char *ptr = haystack;

	/* iterate through first character matches */
	while ((ptr = memchr(ptr, needle[0], len))) {

		/* calculate length after match */
		len = haystacklen - (ptr - (char *) haystack);

		/* done if matches up to capacity of buffer */
		if (memcmp(needle, ptr, needlen < len ? needlen : len) == 0
		                && (partial || len >= needlen)) {
			break;
		}

		/* next character */
		ptr++;
		len--;
	}

	return ptr;
}

/* read until a boundary condition */
static int multipart_buffer_read(multipart_buffer *self,
                                 char *buf,
                                 int bytes,
                                 int *end)
{
	int len, max;
	char *bound;

	/* fill buffer if needed */
	if (bytes > self->bytes_in_buffer) {
		fill_buffer(self);
	}

	/* look for a potential boundary match, only read data up to that point */
	if ((bound = cgi_ap_memstr(self->buf_begin, self->bytes_in_buffer,
	                self->boundary_next, self->boundary_next_len, 1))) {
		max = bound - self->buf_begin;
		if (end && cgi_ap_memstr(self->buf_begin,
		                self->bytes_in_buffer, self->boundary_next,
		                self->boundary_next_len, 0)) {
			*end = 1;
		}
	} else {
		max = self->bytes_in_buffer;
	}

	/* maximum number of bytes we are reading */
	len = max < bytes - 1 ? max : bytes - 1;

	/* if we read any data... */
	if (len > 0) {

		/* copy the data */
		memcpy(buf, self->buf_begin, len);
		buf[len] = 0;

		if (bound && len > 0 && buf[len - 1] == '\r') {
			buf[--len] = 0;
		}

		/* update the buffer */
		self->bytes_in_buffer -= len;
		self->buf_begin += len;
	}

	return len;
}

/*
 XXX: this is horrible memory-usage-wise, but we only expect
 to do this on small pieces of form data.
 */
static char *multipart_buffer_read_body(multipart_buffer *self)
{
	char buf[FILLUNIT], *out = NULL;
	int total_bytes = 0, read_bytes = 0;

	while ((read_bytes
	                = multipart_buffer_read(self, buf, sizeof(buf), NULL))) {
		out = realloc(out, total_bytes + read_bytes + 1);
		memcpy(out + total_bytes, buf, read_bytes);
		total_bytes += read_bytes;
	}

	if (out)
		out[total_bytes] = '\0';

	return out;
}

int cgi_open_temp_file (const char *dir, const char *pfx, char **path)
{
	int fd;
	char *opened_path;

	if (!pfx) {
		pfx = "tmp.";
	}
	if (!path) {
		*path = NULL;
	}

	opened_path = malloc (256);
	if (opened_path == NULL)
		return -1;

	snprintf(opened_path, 256, "%s/%sXXXXXX", dir, pfx);

	fd = mkstemp(opened_path);

	if (fd == -1 || !path) {
		free (path);
	} else {
		*path = opened_path;
	}

	return fd;
}

#define SAFE_RETURN { \
        if (lbuf) free(lbuf); \
        if (abuf) free(abuf); \
        if (array_index) free(array_index); \
        mime_header_list_destroy(header); \
        if (mbuff->boundary_next) free(mbuff->boundary_next); \
        if (mbuff->boundary) free(mbuff->boundary); \
        if (mbuff->buffer) free(mbuff->buffer); \
        if (mbuff) free(mbuff); \
        return; }

void rfc1867_post_handler(struct request *req,
                          int content_length,
                          const char *content_type)
{
	char *boundary, *s = NULL, *boundary_end = NULL, *array_index = NULL;
	char *temp_filename = NULL, *lbuf = NULL, *abuf = NULL;
	int boundary_len = 0, total_bytes = 0, cancel_upload = 0;
	int max_file_size = 0, skip_upload = 0, anonindex = 0, is_anonymous;

	multipart_buffer *mbuff;
	int fd = -1;
	mime_headers *header;

	if (content_length > CGI_MAX_POST_SIZE) {
		log_debug("[error] exceeds max length\n");
		cgi_error(req,
		          CGI_WARNING,
		          "POST Content-Length of %ld bytes exceeds the limit of %ld bytes",
		          content_length, CGI_MAX_POST_SIZE);
		return;
	}

	/* Get the boundary */
	boundary = strstr(content_type, "boundary");
	if (!boundary || !(boundary = strchr(boundary, '='))) {
		log_debug("[error] missing boundary\n");
		cgi_error(req, CGI_WARNING,
		                "Missing boundary in multipart/form-data POST data");
		return;
	}

	boundary++;
	boundary_len = strlen(boundary);

	if (boundary[0] == '"') {
		boundary++;
		boundary_end = strchr(boundary, '"');
		if (!boundary_end) {
			log_debug("[error] invalid boundary\n");
			cgi_error(req, CGI_WARNING,
			                "Invalid boundary in multipart/form-data POST data");
			return;
		}
	} else {
		/* search for the end of the boundary */
		boundary_end = strchr(boundary, ',');
	}
	if (boundary_end) {
		boundary_end[0] = '\0';
		boundary_len = boundary_end - boundary;
	}

	/* Initialize the buffer */
	if (!(mbuff = multipart_buffer_new(boundary, boundary_len))) {
		log_debug("[error] no input buffer\n");
		cgi_error(req, CGI_WARNING,
		                "Unable to initialize the input buffer");
		return;
	}

	/* Initialize $_FILES[] */
	header = malloc (sizeof(mime_headers));
	INIT_LIST_HEAD(&header->list);

	while (!multipart_buffer_eof(mbuff)) {
		char buff[FILLUNIT];
		char *cd = NULL, *param = NULL, *filename = NULL, *tmp = NULL;
		int blen = 0, wlen = 0;

		mime_header_list_clean(header);

		if (!multipart_buffer_headers(mbuff, header)) {
			SAFE_RETURN;
		}

		if ((cd = mime_get_hdr_value(header, "Content-Disposition"))) {
			char *pair = NULL;
			int end = 0;

			while (isspace(*cd)) {
				++cd;
			}

			while (*cd && (pair = cgi_ap_getword(&cd, ';'))) {
				char *key = NULL, *word = pair;

				while (isspace(*cd)) {
					++cd;
				}

				if (strchr(pair, '=')) {
					key = cgi_ap_getword(&pair, '=');

					if (!strcasecmp(key, "name")) {
						if (param) {
							free(param);
						}
						param = cgi_ap_getword_conf(&pair);
					} else if (!strcasecmp(key, "filename")) {
						if (filename) {
							free(filename);
						}
						filename = cgi_ap_getword_conf(&pair);
					}
				}
				if (key) {
					free(key);
				}
				free(word);
			}

			/* Normal form variable, safe to read all data into memory */
			if (!filename && param) {
				cgi_object *d;
				char *value = multipart_buffer_read_body(mbuff);

				if (!value) {
					value = strdup("");
				}

				log_debug("have a parameter [%s] => [%s]\n", param, value);

				d = malloc(sizeof(cgi_object));
				d->value.u_str = cgi_url_decode(value);
				d->type = CGI_STRING;

				htable_insert(req->parameters, param, d);

				if (!strcasecmp(param, "MAX_FILE_SIZE")) {
					max_file_size = atol(value);
				}

				free(param);
				free(value);
				continue;
			}

			/* Return with an error if the posted data is garbled */
			if (!param && !filename) {
				cgi_error(req, CGI_WARNING,
				                "File Upload Mime headers garbled");
				SAFE_RETURN;
			}

			if (!param) {
				is_anonymous = 1;
				param = malloc(MAX_SIZE_ANONNAME);
				snprintf(param, MAX_SIZE_ANONNAME, "%u", anonindex++);
			} else {
				is_anonymous = 0;
			}

			/* New Rule: never repair potential malicious user input */
			if (!skip_upload) {
				char *tmp = param;
				long c = 0;

				while (*tmp) {
					if (*tmp == '[') {
						c++;
					} else if (*tmp == ']') {
						c--;
						if (tmp[1] && tmp[1] != '[') {
							skip_upload = 1;
							break;
						}
					}
					if (c < 0) {
						skip_upload = 1;
						break;
					}
					tmp++;
				}
			}

			total_bytes = cancel_upload = 0;

			if (!skip_upload) {
				/* Handle file */
				fd = cgi_open_temp_file(UPLOAD_TMP_DIR, "cgi", &temp_filename);
				if (fd == -1) {
					cgi_error(req, CGI_WARNING,
					                "File upload error - unable to create a temporary file");
					cancel_upload = UPLOAD_ERROR_E;
				}
			}
			if (skip_upload) {
				free(param);
				free(filename);
				continue;
			}

			if (strlen(filename) == 0) {
#if CGI_DEBUG_FILE_UPLOAD
				cgi_error(req, CGI_NOTICE, "No file uploaded");
#endif
				cancel_upload = UPLOAD_ERROR_D;
			}

			end = 0;
			while (!cancel_upload && (blen = multipart_buffer_read(mbuff, buff, sizeof(buff), &end))) {
				if (total_bytes > CGI_MAX_POST_SIZE) {
#if CGI_DEBUG_FILE_UPLOAD
					cgi_error(req, CGI_NOTICE, "upload_max_filesize of %ld bytes exceeded - file [%s=%s] not saved", CGI_MAX_POST_SIZE, param, filename);
#endif
					cancel_upload = UPLOAD_ERROR_A;
				} else if (max_file_size && (total_bytes > max_file_size)) {
#if CGI_DEBUG_FILE_UPLOAD
					cgi_error(req, CGI_NOTICE, "MAX_FILE_SIZE of %ld bytes exceeded - file [%s=%s] not saved", max_file_size, param, filename);
#endif
					cancel_upload = UPLOAD_ERROR_B;
				} else if (blen > 0) {
					wlen = write(fd, buff, blen);

					if (wlen < blen) {
#if CGI_DEBUG_FILE_UPLOAD
						cgi_error(req, CGI_NOTICE, "Only %d bytes were written, expected to write %d", wlen, blen);
#endif
						cancel_upload = UPLOAD_ERROR_F;
					} else {
						total_bytes += wlen;
					}
				}
			}
			if (fd != -1) { /* may not be initialized if file could not be created */
				close(fd);
			}
			if (!cancel_upload && !end) {
#if CGI_DEBUG_FILE_UPLOAD
				cgi_error(req, CGI_NOTICE, "Missing mime boundary at the end of the data for file %s", strlen(filename) > 0 ? filename : "");
#endif
				cancel_upload = UPLOAD_ERROR_C;
			}
#if CGI_DEBUG_FILE_UPLOAD
			if(strlen(filename) > 0 && total_bytes == 0 && !cancel_upload) {
				cgi_error(req, CGI_WARNING, "Uploaded file size 0 - file [%s=%s] not saved", param, filename);
				cancel_upload = 5;
			}
#endif

			if (cancel_upload) {
				if (temp_filename) {
					if (cancel_upload != UPLOAD_ERROR_E) { /* file creation failed */
						unlink(temp_filename);
					}
					free(temp_filename);
				}
				temp_filename = NULL;
			}

			{
				cgi_file *f;

				f = malloc(sizeof(struct _cgi_file));

				/* The \ check should technically be needed for win32 systems only where
				 * it is a valid path separator. However, IE in all it's wisdom always sends
				 * the full path of the file on the user's file system. Fix it.
				 */
				s = strrchr(filename, '\\');
				if ((tmp = strrchr(filename, '/')) > s) {
					s = tmp;
				}

				if (s && s > filename) {
					f->filename = strdup(s+1);
					log_debug ("s+1 = %s\n", f->filename);
				} else {
					f->filename = strdup(filename);
					log_debug ("filename = %s\n", f->filename);
				}

				free(filename);
				s = NULL;

				/* Possible Content-Type: */
				if (cancel_upload || !(cd = mime_get_hdr_value(header, "Content-Type"))) {
					cd = "";
				} else {
					/* fix for Opera 6.01 */
					s = strchr(cd, ';');
					if (s != NULL) {
						*s = '\0';
					}
				}

				f->type = strdup(cd);
				log_debug ("type = %s\n", f->type);

				/* Restore Content-Type Header */
				if (s != NULL) {
					*s = ';';
				}
				s = "";

				if (temp_filename == NULL) {
					f->tmp_filename = strdup("");
				} else {
					f->tmp_filename = strdup(temp_filename);
					free(temp_filename);
				}
				log_debug ("tmp_filename = %s\n", f->tmp_filename);

				f->error = cancel_upload;
				log_debug ("error = %d\n", f->error);

				if (cancel_upload) {
					f->size = 0;
				} else {
					f->size = total_bytes;
				}

				log_debug ("size = %ld", f->size);
				log_debug ("param = %s", param);

				cgi_upload_add_file (req, param, f);
				free(param);
			}
		}
	}

	SAFE_RETURN;
}

