#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/hashtable.h"
#include "util/list.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_priv.h"
#include "cgi_cookie_priv.h"
#include "cgi_session_priv.h"

int cgi_request_is_post(struct request *req)
{
	if (strncmp("POST", req->method, 4) == 0)
		return 1;

	return 0;
}

int cgi_request_is_get(struct request *req)
{
	if (strncmp("GET", req->method, 3) == 0)
		return 1;

	return 0;
}


char *cgi_request_get_parameter(struct request *req, const char *name)
{

	cgi_object *o;

	o = htable_lookup(req->parameters, (char *) name);

	if (o != NULL)
		return o->value.u_str;

	return NULL;
}

void cgi_request_free(struct request *req)
{
	cgi_cookie_destroy(req);
	cgi_session_free(req);

	destroy_htable(req->headers);
	destroy_htable(req->parameters);
	destroy_htable(req->files);

	if (req->url != NULL)
		free(req->url);

	free(req);
}
