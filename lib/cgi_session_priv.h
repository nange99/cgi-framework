/*
 * cgi_session_priv.h
 *
 *  Created on: Jun 8, 2010
 *      Author: pedro
 */

#ifndef CGI_SESSION_PRIV_H_
#define CGI_SESSION_PRIV_H_

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

#endif /* CGI_SESSION_PRIV_H_ */
