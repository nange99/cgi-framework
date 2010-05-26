/*
 * cgi_session.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_SESSION_H_
#define CGI_SESSION_H_

#include "cgi_servlet_private.h"

#define SESSION_COOKIE	"CGISID"

struct _session {
	char *id;
	int initialized;
	int headers;
};

char *_session_create_id();

int cgi_session_init (struct request *req);
int cgi_session_destroy (struct request *req);

#endif /* CGI_SESSION_H_ */
