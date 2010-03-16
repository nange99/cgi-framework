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
#include "util/sha1.h"
#include "cgi_cookie.h"
#include "cgi_session.h"

char *_session_create_id ()
{
	sha_ctx hashctx;
	char buf[58];
	unsigned char sha1[20];
	char *remote_addr = NULL;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	if (getenv ("REMOTE_ADDR") != NULL) {
		remote_addr = getenv ("REMOTE_ADDR");
	}

	snprintf ( (char * )&buf, 58, "%.15s%ld%ldcgif", remote_addr ? remote_addr : "", tv.tv_sec, (long int)tv.tv_usec);

	sha1_init (&hashctx);
	sha1_update (&hashctx, (unsigned char *) buf, strlen(buf));
	sha1_final (sha1, &hashctx);

	return sha1_to_hex (sha1);
}

int cgi_session_init (struct request *req)
{
	char *sid;
	struct _session *s;

	if (req->session != NULL) {
		s = (struct _session *)req->session;
		if (s->initialized) {
			return 0;
		}
	}

	if (cgi_cookie_get_value (req, SESSION_COOKIE) != NULL) {
		sid = cgi_cookie_get_value (req, SESSION_COOKIE);

	} else {
		sid = _session_create_id();
		printf ("session id: %s\n", sid);

		s = malloc (sizeof (struct _session));

		s->id = sid;
		s->initialized = 1;

		req->session = s;

		return 1;
	}

}

int cgi_session_destroy (struct request *req)
{

	return 0;
}
