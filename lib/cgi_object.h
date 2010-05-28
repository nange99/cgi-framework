/*
 * cgi_object.h
 *
 *  Created on: Nov 1, 2009
 *      Author: pedrokiefer
 */

#ifndef CGI_OBJECT_H_
#define CGI_OBJECT_H_

typedef enum cgi_object_type {
	CGI_STRING, CGI_INTEGER, CGI_FLOAT, CGI_LIST, CGI_TABLE, CGI_COOKIES
} cgi_object_type;

typedef struct {
	enum cgi_object_type type;
	union {
		int u_int;
		double u_double;
		char *u_str;
		void *u_hash;
	} value;
} cgi_object;

cgi_object *cgi_object_init();
void cgi_object_destroy (cgi_object *o);

#endif /* CGI_OBJECT_H_ */
