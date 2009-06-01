#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"

int cgi_response_add_parameter(struct response *resp, char *key, void *value, int type) {

	data *d;

	d = malloc (sizeof (data));
	
	switch (type) {

	case STRING:
		d->value.u_str = cgi_url_encode (value);
		d->type = STRING;

		break;	
	}

	htable_insert (resp->parameters, key, d);

	return 1;
}

int cgi_response_set_content_type (struct response *resp) {
	
}

int cgi_response_add_cookie (struct response *resp, char *name, char *value, char *max_age, char *path, char *domain, int secure) {

	int len;
	char *cookie;
	data *d;

	cookie = malloc ((20 + strlen (name) + strlen (value)) * sizeof(char));

	if (cookie == NULL) {
		// FIXME: no memory, bailout;
		return 0;
	}
	
	len = sprintf (cookie, "Set-Cookie: %s=%s;", name, value);

	if (max_age) {
		cookie = realloc (cookie, (len + strlen (max_age) + 12) * sizeof(char));
		if (cookie == NULL) {
			return 0;
		}
		len = sprintf (cookie, "%s Max-age=%s;", cookie, max_age);
	}

	if (path) {
		cookie = realloc (cookie, (len + strlen (path) + 12) * sizeof(char));
		if (cookie == NULL) {
			return 0;
		}
		len = sprintf (cookie, "%s Path=%s;", cookie, path);
	}

	if (domain) {
		cookie = realloc (cookie, (len + strlen (domain) + 12) * sizeof(char));
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


	d = malloc (sizeof (data));

	d->value.u_str = cookie;
	d->type = STRING;

	htable_insert (resp->headers, "cookie", d);
	
	return 0;
}

int cgi_response_add_header (struct response *resp) {

}

void cgi_response_free (struct response *resp) {

	destroy_htable (resp->parameters);
	destroy_htable (resp->headers);
	
	if (resp->html != NULL)
		free (resp->html);
	
	free (resp);
}