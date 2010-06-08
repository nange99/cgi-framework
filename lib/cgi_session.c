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

#define SESSION_PATH	"/var/tmp"

static char *_session_create_id()
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
		return 0;
	}
	close(f);

	return 1;
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

	_session_create_file(s->filename);

	cgi_cookie_add(req, SESSION_COOKIE, s->id, 0, 0, 0, 0);

	s->initialized = 1;

	return s;
}

/**
 * Have a established session?
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

	len = strlen(sid) + strlen(SESSION_PATH) + 2;
	path = malloc(len * sizeof(char));
	if (path == NULL)
		return 1;

	snprintf(path, len, "%s/%s", SESSION_PATH, sid);

	errno = 0;
	fp = open(path, O_RDWR);
	if (errno == ENOENT) {
		/* file doesn't exist */
		s = _session_init(req);
		req->session = s;

		free(path);

		return 1;
	}

	s = malloc(sizeof(struct _session));

	if (s == NULL)
		return 0;

	s->id = strdup(sid);
	s->filename = strdup(path);
	s->initialized = 1;

	req->session = s;

	free(path);

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

int cgi_session_free(struct request *req)
{
	struct _session *s;

	s = (struct _session *)req->session;

	if (s == NULL)
		return -1;

	free(s->id);
	free(s->filename);
	free(s);

	req->session = NULL;

	return 0;
}

void cgi_session_destroy(struct request *req)
{
	struct _session *s;

	s = (struct _session *)req->session;

	remove (s->filename);

	cgi_cookie_remove(req, SESSION_COOKIE);
}
