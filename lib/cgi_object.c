/*
 * cgi_object.c
 *
 *  Created on: Nov 1, 2009
 *      Author: pedrokiefer
 */
#include <stdio.h>
#include <stdlib.h>
#include "cgi_object.h"

void cgi_object_destroy (cgi_object *o)
{
	if (o == NULL)
		return;

	switch (o->type) {
	case CGI_STRING:
		free (o->value.u_str);
		break;
	case CGI_LIST:
		cgi_list_destroy (o->value.u_hash);
		break;
	}

	free (o);
}
