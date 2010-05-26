/*
 * cgi_cookie.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_COOKIE_H_
#define CGI_COOKIE_H_

#include "cgi_servlet_private.h"

char *cgi_cookie_get_value (struct request *req, const char *name);
void cgi_cookie_add_value (struct request *req, const char *name, char *value);

#endif /* CGI_COOKIE_H_ */
