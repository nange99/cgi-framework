#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"


char *cgi_request_get_parameter (struct request *req, char *name) {

	data *d;

	d = htable_lookup (req->parameters, name);

	if (d != NULL)
		return d->value.u_str;

	return NULL;
}

void cgi_request_free (struct request *req) {

	destroy_htable (req->parameters);

	if (req->url != NULL)
		free (req->url);

	free (req);
}
