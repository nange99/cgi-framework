/*
 * cgi_cookie.h
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_COOKIE_H_
#define CGI_COOKIE_H_

#include "util/list.h"
#include "cgi_servlet_private.h"

struct _cookie {
	struct list_head list;
	char *name;
	char *value;
	char *max_age;
	char *path;
	char *domain;
	int secure;
	int send;
};

char *cgi_cookie_get_value (struct request *req, const char *name);
void cgi_cookie_remove(struct request *req, const char *name);
void cgi_cookie_add(struct request *req,
                          const char *name,
                          char *value,
                          char *max_age,
                          char *path,
                          char *domain,
                          int secure);

#endif /* CGI_COOKIE_H_ */
