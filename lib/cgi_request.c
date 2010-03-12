#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/hashtable.h"
#include "util/list.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"

char *cgi_request_get_parameter (struct request *req, char *name)
{

	cgi_object *o;

	o = htable_lookup (req->parameters, name);

	if (o != NULL)
		return o->value.u_str;

	return NULL;
}

void cgi_request_free (struct request *req)
{

	destroy_htable (req->parameters);

	if (req->url != NULL)
		free (req->url);

	free (req);
}
