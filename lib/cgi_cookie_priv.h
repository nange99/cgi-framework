/*
 * cgi_cookie_priv.h
 *
 *  Created on: Jun 8, 2010
 *      Author: pedro
 */

#ifndef CGI_COOKIE_PRIV_H_
#define CGI_COOKIE_PRIV_H_

void cgi_cookie_init(struct request *req);
void cgi_cookie_destroy(struct request *req);
void cgi_cookie_print_headers(struct request *req);

#endif /* CGI_COOKIE_PRIV_H_ */
