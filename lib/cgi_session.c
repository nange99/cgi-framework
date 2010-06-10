/*
 * cgi_session.c
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "util/sha1.h"
#include "json/json.h"
#include "cgi_servlet.h"
#include "cgi_servlet_priv.h"
#include "cgi_cookie.h"
#include "cgi_session.h"
#include "cgi_session_priv.h"

#define SESSION_PATH	"/tmp"

struct _session {
	char *id;
	char *filename;
	int initialized;
	int headers;
	struct json_object *json;
};

static char *_session_create_id(void)
{
	sha_ctx hashctx;
	char buf[58];
	unsigned char sha1[20];
	char *remote_addr = NULL;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	if (getenv("REMOTE_ADDR") != NULL) {
		remote_addr = getenv("REMOTE_ADDR");
	}

	snprintf((char * )&buf, 58, "%.15s%ld%ldcgif", remote_addr ? remote_addr : "", tv.tv_sec, (long int)tv.tv_usec);

	sha1_init(&hashctx);
	sha1_update(&hashctx, (unsigned char *) buf, strlen(buf));
	sha1_final(sha1, &hashctx);

	return strdup(sha1_to_hex(sha1));
}

static int _session_create_file(char *fname)
{
	int f;

	f = open(fname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (!f) {
		return -1;
	}
	close(f);

	return 0;
}

static struct _session *_session_init(struct request *req)
{
	struct _session *s;
	int len;

	s = malloc(sizeof(struct _session));

	if (s == NULL)
		return 0;

	s->id = _session_create_id();

	len = strlen(s->id) + strlen(SESSION_PATH) + 2;
	s->filename = malloc(len * sizeof(char));
	if (s->filename == NULL)
		return 0;

	snprintf(s->filename, len, "%s/%s", SESSION_PATH, s->id);

	if (_session_create_file(s->filename) < 0)
		return 0;

	s->json = json_object_new_object();

	cgi_cookie_add(req, SESSION_COOKIE, s->id, 0, 0, 0, 0, 0);

	s->initialized = 1;

	return s;
}

/**
 * Check if session exists
 *
 * Public function usable by external programs
 * using this library.
 *
 * @param req
 * @return 1 if session exists, 0 otherwise.
 */
int cgi_session_exists(struct request *req)
{
	if (req->session != NULL)
		return 1;

	return 0;
}

/**
 * Initializes a session;
 *
 * @param req
 * @return
 */
int cgi_session_init(struct request *req)
{
	struct _session *s;

	if (req->session != NULL) {
		s = (struct _session *) req->session;
		if (s->initialized) {
			return 0;
		}
	}

	/* creates a new session */
	s = _session_init(req);
	req->session = s;
	return 0;
}

/**
 * Destroy the current session
 *
 * @param req
 */
void cgi_session_destroy(struct request *req)
{
	struct _session *s;

	s = (struct _session *) req->session;

	remove(s->filename);

	cgi_cookie_remove(req, SESSION_COOKIE);
}

char *cgi_session_get_value(struct request *req, const char *name)
{
	struct json_object *json;
	struct json_object *obj;

	json = ((struct _session *)req->session)->json;

	obj = json_object_object_get(json, name);

	if (obj == NULL)
		return NULL;

	return (char *)json_object_get_string(obj);
}

int cgi_session_has_value(struct request *req, const char *name)
{
	struct json_object *json;
	struct json_object *obj;

	json = ((struct _session *)req->session)->json;

	obj = json_object_object_get(json, name);

	if (obj == NULL)
		return 0;

	return 1;
}

int cgi_session_add_value(struct request *req,
                          const char *name,
                          void *value,
                          cgi_object_type type)
{
	struct json_object *json;

	json = ((struct _session *)req->session)->json;

	switch (type) {
	case CGI_STRING:
		json_object_object_add(json, name,
		                json_object_new_string(value));
		break;
	case CGI_INTEGER:
		json_object_object_add(json, name,
				json_object_new_int((int) value));
		break;
	case CGI_FLOAT:
		json_object_object_add(json, name,
				json_object_new_double(*(double *) value));
		break;
	case CGI_LIST:
	case CGI_TABLE:
	case CGI_COOKIES:
		break;
	}

	return 1;
}

/*
 * Private Functions
 */

/**
 * Frees the session
 * @param req
 * @return
 */
int cgi_session_free(struct request *req)
{
	struct _session *s;

	s = (struct _session *) req->session;

	if (s == NULL)
		return -1;

	json_object_to_file(s->filename, s->json);
	json_object_put(s->json);

	free(s->id);
	free(s->filename);
	free(s);

	req->session = NULL;

	return 0;
}

/**
 * Have a established session?
 *
 * Private function used inside cgi_servlet_init
 *
 * @param req
 * @return
 */
int cgi_session_try_init(struct request *req)
{
	char *path;
	char *sid;
	int len;
	int fp;
	struct _session *s;

	if (cgi_cookie_get_value(req, SESSION_COOKIE) == NULL) {
		req->session = NULL;
		return 0;
	}

	sid = cgi_cookie_get_value(req, SESSION_COOKIE);

	/* Does not accept an empty cookie */
	if (strlen(sid) == 0) {
		req->session = NULL;
		return 0;
	}

	len = strlen(sid) + strlen(SESSION_PATH) + 2;
	path = malloc(len * sizeof(char));
	if (path == NULL)
		return 1;

	snprintf(path, len, "%s/%s", SESSION_PATH, sid);

	errno = 0;
	fp = open(path, O_RDWR);
	if (errno == ENOENT) {
		/* file doesn't exist */
		req->session = NULL;
		free(path);
		return 1;
	}

	s = malloc(sizeof(struct _session));

	if (s == NULL)
		return 0;

	s->id = strdup(sid);
	s->filename = strdup(path);
	s->initialized = 1;

	s->json = json_object_from_file(s->filename);

	if (s->json == NULL) {
		printf ("we don't have the file yet... should create a json object!\n"); /* TODO */
	}

	req->session = s;

	free(path);

	return 0;
}
