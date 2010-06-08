/*
 * cgi_session.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_SESSION_H_
#define CGI_SESSION_H_

#include "cgi_servlet.h"

/* public */
int cgi_session_init(struct request *req);
void cgi_session_destroy(struct request *req);
int cgi_session_exists(struct request *req);

#endif /* CGI_SESSION_H_ */
