/*
 * cgi_object.c
 *
 *  Created on: Nov 1, 2009
 *      Author: pedrokiefer
 */
#include <stdio.h>
#include <stdlib.h>
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_list.h"
#include "cgi_table.h"

cgi_object *cgi_object_init(void)
{
	cgi_object *o;

	o = malloc(sizeof(cgi_object));

	return o;
}

void cgi_object_destroy(cgi_object *o)
{
	if (o == NULL)
		return;

	switch (o->type) {
	case CGI_STRING:
		free(o->value.u_str);
		break;
	case CGI_LIST:
		cgi_list_destroy(o->value.u_hash);
		break;
	case CGI_TABLE:
		cgi_table_destroy(o->value.u_hash);
		break;
	case CGI_FLOAT:
	case CGI_INTEGER:
	case CGI_COOKIES:
	default:
		break;
	}

	free(o);
}
