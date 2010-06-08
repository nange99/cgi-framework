/*
 * cgi_session.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_SESSION_H_
#define CGI_SESSION_H_

#include "cgi_servlet.h"

#define SESSION_COOKIE	"sid"

struct _session {
	char *id;
	char *filename;
	int initialized;
	int headers;
};

/* private */
int cgi_session_try_init(struct request *req);
int cgi_session_free (struct request *req);

/* public */
int cgi_session_init (struct request *req);
void cgi_session_destroy(struct request *req);

#endif /* CGI_SESSION_H_ */
