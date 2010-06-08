/*
 * cgi_cookie.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_COOKIE_H_
#define CGI_COOKIE_H_

#include "cgi_servlet.h"

char *cgi_cookie_get_value (struct request *req, const char *name);
void cgi_cookie_remove(struct request *req, const char *name);
void cgi_cookie_add(struct request *req,
                          const char *name,
                          const char *value,
                          const char *max_age,
                          const char *path,
                          const char *domain,
                          int secure);

#endif /* CGI_COOKIE_H_ */
