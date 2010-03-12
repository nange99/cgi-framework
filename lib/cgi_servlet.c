#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util/hashtable.h"
#include "util/list.h"
#include "cgi_log.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"
#include "template/template.h"

int cgi_servlet_init (struct config *conf, struct url_mapping *map[], int map_length, struct filter_mapping *filters[]) {
	int r;

	struct request *req;
	struct response *resp;

	log_init(0);
	log_verbose (1);

	log_info ("begin cgi_servlet\n");

	req = malloc (sizeof (struct request));
	resp = malloc (sizeof (struct response));

	req->parameters = create_htable (17);
	resp->parameters = create_htable (17);

	resp->headers = create_htable (13);
	resp->html = NULL;

	r = process_request (req);

	/*printf("method: %s\n", req.method);*/

	/*r = do_filters (&req, &resp);*/

	r = do_handler (map, map_length, req, resp);

	if (r < 0) {
		goto cleanup;
	}
	
	draw_page (req, resp);
	
cleanup:
	cgi_request_free (req);
	cgi_response_free (resp);

	log_info ("end cgi_servlet\n");

	return 0;
}

int process_request (struct request *req) {

	req->url = NULL;

	if (getenv ("REQUEST_METHOD") != NULL) {
		strcpy (req->method, getenv ("REQUEST_METHOD"));
		log_debug ("got method = [%s]\n", req->method);
	}

	if (getenv ("PATH_INFO") != NULL) {
		char *path;
		path = getenv ("PATH_INFO");

		path = strchr (path+1, '/');

		req->url = malloc ((strlen (path) + 1) * sizeof(char));
		if (req->url == NULL) {
			// FIXME: no memory, bailout!
		}


		strcpy (req->url, path);
	}

	if (getenv ("QUERY_STRING") != NULL) {

		int content_length = 0;

		if (getenv ("CONTENT_LENGTH") != NULL) {
			content_length = atoi (getenv ("CONTENT_LENGTH"));
		}

		parse_data_string (req, getenv ("QUERY_STRING"), content_length);
	}

	if (strcmp (req->method, "POST") == 0) {
		int content_length;
		int read;
		char *content_data;

		log_debug ("parsing post...\n");

		if (getenv ("CONTENT_LENGTH") == NULL) {
			return 0;
		}

		content_length = atoi (getenv ("CONTENT_LENGTH"));

		log_debug ("content_length = %d\n", content_length);

		content_data = malloc ((content_length + 1) * sizeof(char));
		if (content_data == NULL) {
			// FIXME: no memory, bailout!
		}

		read = fread (content_data, content_length, 1, stdin);
		content_data[content_length] = '\0';

		log_debug ("got content = [%s]\n", content_data);

		parse_data_string (req, content_data, content_length);

		free (content_data);
	}

	return 0;
}

char *cgi_url_decode (char *str) {
	int i;
	char *result, *tmp;

	result = malloc ( (strlen (str) + 1) * sizeof (char));
	memset (result, '\0', strlen (str) + 1);


	tmp = result;
	while (str != NULL && *str != '\0') {
		if (*str == '%' && *(str + 1) != '\0' && *(str + 2) != '\0') {
			for (i = 0, str++; i < 2; i++, str++) {
				(*tmp) += ((isdigit(*str) ?  *str - '0' : ((tolower(*str) - 'a')) + 10) * ( i ? 1 : 16));
			}
			tmp++;
		} else if (*str == '+') {
			*tmp++ = ' ';
			str++;
		} else {
			*tmp++ = *str++;
		}
	}
	return result;
}

char *cgi_url_encode (char *str) {

	static char safechars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,;:[]/\\=+";
	char *result, *tmp;
	int length;

	length = strlen (str) + 1;

	result = malloc ( length * sizeof (char));
	memset (result, '\0', length );

	tmp = result;

	while (*str != '\0') {
		if (strchr (safechars, *str) != NULL) {
			*tmp++ = *str++;
		} else {
			char buf [4];
			char *pbuf;
			int current_length;
			int i;

			length += 3;
			current_length = tmp - result;

			result = realloc (result, (length) * sizeof (char));

			tmp = result + current_length;

			sprintf (buf, "%%%02x", (int) *str);

			pbuf = buf;
			for (i = 0; i < 3; i++) {
				*tmp++ = *pbuf++;
			}

			*str++;
		}
	}
	*tmp = '\0';

	return result;
}

int parse_data_string (struct request *req, char *string, int length) {

	/* name=asdf&check[1]=on&check[3]=on;session=A1s2d3F4 */

	const char delim = '=';
	const char sep1 = '&';
	const char sep2 = ';';
	int str_length, position;
	char *aux, *tmp;
	char *key, *value;
	cgi_object *d;

	str_length = strlen (string);
	aux = string;
	while (*string) {
		position = 0;
		while (*aux && (*aux != delim)) {
			position++;
			aux++;
		}

		if (*aux) {
			aux++;
		}

		key = malloc ((position + 1) * sizeof(char));
		if (key == NULL) {
			// FIXME: no memory, bailout!
		}
		strncpy (key, string, position);
		key[position] = '\0';

		log_debug ("[%s] => ", key);

		string = aux;
		position = 0;
		while (*aux && (*aux != sep1) && (*aux != sep2)) {
			position++;
			aux++;
		}

		if (*aux) {
			aux++;
		}

		value = malloc ((position + 1) * sizeof(char));
		if (value == NULL) {
			// FIXME: no memory, bailout!
		}
		strncpy (value, string, position);
		value[position] = '\0';

		tmp = cgi_url_decode (value);

		if (tmp != NULL) {
			free (value);
		}

		value = tmp;

		log_debug ("[%s]\n", value);

		d = malloc (sizeof (cgi_object));

		d->value.u_str = value;
		d->type = CGI_STRING;

		htable_insert (req->parameters, key, d);

		free (key);

		string = aux;
	}

	return 0;
}

int do_filters (struct request *req, struct response *resp) {
	return 0;
}

int do_handler (struct url_mapping *map[], int map_length, struct request *req, struct response *resp) {

	int i;
	struct url_mapping *current, *tmp;

	if (req->url == NULL) {
		return 0;
	}

	current = NULL;

	for (i = 0; i < map_length; i++) {
		tmp = (struct url_mapping *) &map[i * 2];
		if (strcmp (req->url, tmp->url) == 0) {
			current = tmp;
			break;
		}
	}

	if (current == NULL) {
		log_warn ("*** could not find a handler for %s\n", req->url);
		return -1;
	}
	
	current->handler (req, resp);
	
	return 0;
}

int print_headers (struct response *resp) {

	cgi_object *o;

	o = htable_lookup (resp->headers, "cookie");

	if (o != NULL)
		printf ("%s", o->value.u_str);

	printf ("Content-Type: text/html\r\n\r\n");

	return 1;
}

int draw_page (struct request *req, struct response *resp) {

	print_headers (resp);

	template_draw (resp->html, req->parameters, resp->parameters);

	return 0;
}
