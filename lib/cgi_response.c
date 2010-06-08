#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/hashtable.h"
#include "util/list.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_priv.h"

int cgi_response_add_parameter(struct response *resp,
                               const char *key,
                               void *value,
                               cgi_object_type type)
{

	cgi_object *o;

	o = malloc(sizeof(cgi_object));

	o->type = type;
	switch (type) {
	case CGI_STRING:
		o->value.u_str = strdup(value);
		break;
	case CGI_INTEGER:
		o->value.u_int = (int) value;
		break;
	case CGI_FLOAT:
		o->value.u_double = *(double *) value;
		break;
	case CGI_LIST:
		o->value.u_hash = value;
		break;
	case CGI_TABLE:
		break;
	case CGI_COOKIES:
		return 0;
		break;
	}

	htable_insert(resp->parameters, (char *) key, o);

	return 1;
}

void cgi_response_set_html(struct response *resp, const char *file)
{
	resp->html = strdup(file);
	return;
}

int cgi_response_set_content_type(struct response *resp)
{
	return 1;
}

int cgi_response_add_header(struct response *resp)
{

	return 1;
}

void cgi_response_free(struct response *resp)
{

	destroy_htable(resp->parameters);
	destroy_htable(resp->headers);

	if (resp->html != NULL)
		free(resp->html);

	free(resp);
}
