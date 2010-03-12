#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/hashtable.h"
#include "util/list.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"

int cgi_response_add_parameter (struct response *resp,
                                char *key,
                                void *value,
                                cgi_object_type type)
{

	cgi_object *o;

	o = malloc (sizeof(cgi_object));

	o->type = type;
	switch (type) {
	case CGI_STRING:
		o->value.u_str = strdup (value);
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
	}

	htable_insert (resp->parameters, key, o);

	return 1;
}

void cgi_response_set_html (struct response *resp, char *file)
{
	resp->html = strdup (file);
	return;
}

int cgi_response_set_content_type (struct response *resp)
{
	return 1;
}

int cgi_response_add_cookie (struct response *resp,
                             char *name,
                             char *value,
                             char *max_age,
                             char *path,
                             char *domain,
                             int secure)
{

	int len;
	char *cookie;
	cgi_object *o;

	cookie = malloc ((20 + strlen (name) + strlen (value)) * sizeof(char));

	if (cookie == NULL) {
		// FIXME: no memory, bailout;
		return 0;
	}

	len = sprintf (cookie, "Set-Cookie: %s=%s;", name, value);

	if (max_age) {
		cookie = realloc (cookie, (len + strlen (max_age) + 12)
		                * sizeof(char));
		if (cookie == NULL) {
			return 0;
		}
		len = sprintf (cookie, "%s Max-age=%s;", cookie, max_age);
	}

	if (path) {
		cookie = realloc (cookie, (len + strlen (path) + 12)
		                * sizeof(char));
		if (cookie == NULL) {
			return 0;
		}
		len = sprintf (cookie, "%s Path=%s;", cookie, path);
	}

	if (domain) {
		cookie = realloc (cookie, (len + strlen (domain) + 12)
		                * sizeof(char));
		if (cookie == NULL) {
			return 0;
		}
		len = sprintf (cookie, "%s Domain=%s;", cookie, domain);
	}

	if (secure) {
		cookie = realloc (cookie, (len + 12) * sizeof(char));
		if (cookie == NULL) {
			return 0;
		}
		len = sprintf (cookie, "%s Secure", cookie);
	}

	cookie = realloc (cookie, (len + 3) * sizeof(char));
	if (cookie == NULL) {
		return 0;
	}
	sprintf (cookie, "%s\r\n", cookie);

	o = malloc (sizeof(cgi_object));

	o->value.u_str = cookie;
	o->type = CGI_STRING;

	htable_insert (resp->headers, "cookie", o);

	return 1;
}

int cgi_response_add_header (struct response *resp)
{

	return 1;
}

void cgi_response_free (struct response *resp)
{

	destroy_htable (resp->parameters);
	destroy_htable (resp->headers);

	if (resp->html != NULL)
		free (resp->html);

	free (resp);
}
