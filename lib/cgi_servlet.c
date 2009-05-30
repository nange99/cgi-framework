#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"
#include "template.h"

int cgi_servlet_init (struct config *conf, struct url_mapping *map[], int map_length, struct filter_mapping *filters[]) {
	int r;
	
	struct request *req;
	struct response *resp;

	req = malloc (sizeof (struct request));
	resp = malloc (sizeof (struct response));
	
	req->parameters = create_htable (17);
	resp->parameters = create_htable (17);
	
	resp->headers = create_htable (13);
	resp->html = NULL;
	
	r = process_request (req);
	
	/*printf("method: %s\n", req.method);*/

	/*r = do_filters (&req, &resp);*/

	r = do_handler (map, map_length, req, resp);

	draw_page (req, resp);

	cgi_request_free (req);
	cgi_response_free (resp);
	
	return 0;
}

int process_request (struct request *req) {

	req->url = NULL;
	
	if (getenv ("REQUEST_METHOD") != NULL) {
		strcpy (req->method, getenv ("REQUEST_METHOD"));
	}

	if (getenv ("PATH_INFO") != NULL) {
		char *path;
		path = getenv ("PATH_INFO");

		path = strchr (path+1, '/');

		req->url = malloc ((strlen (path) + 1) * sizeof(char));
		if (req->url == NULL) {
			// FIXME: no memory, bailout!
		}
			

		strcpy (req->url, path);
	}

	if (getenv ("QUERY_STRING") != NULL) {

		int content_length = 0;

		if (getenv ("CONTENT_LENGTH") != NULL) {
			content_length = atoi (getenv ("CONTENT_LENGTH"));
		}
		
		parse_data_string (req, getenv ("QUERY_STRING"), content_length);
	}

	if (strcmp (req->method, "POST")) {
		int content_length;
		char *content_data;
		
		if (getenv ("CONTENT_LENGTH") == NULL) {
			return 0;
		}

		content_length = atoi (getenv ("CONTENT_LENGTH"));

		content_data = malloc ((content_length + 1) * sizeof(char));
		if (content_data == NULL) {
			// FIXME: no memory, bailout!
		}
		
		
	}
	
	return 0;
}

int parse_data_string (struct request *req, char *string, int length) {

	/* name=asdf&check[1]=on&check[3]=on;session=A1s2d3F4 */
	
	const char delim = '=';
	const char sep1 = '&';
	const char sep2 = ';';
	int str_length, position;
	char *aux;
	char *key, *value;
	data *d;

	str_length = strlen (string);
	aux = string;
	while (*string) {
		position = 0;
		while (*aux && (*aux != delim)) {
			position++;
			aux++;
		}

		if (*aux) {
			aux++;
		}

		key = malloc ((position + 1) * sizeof(char));
		if (key == NULL) {
			// FIXME: no memory, bailout!
		}
		strncpy (key, string, position);
		key[position] = '\0';
		
		printf ("[%s] => ", key);

		string = aux;
		position = 0;
		while (*aux && (*aux != sep1) && (*aux != sep2)) {
			position++;
			aux++;
		}

		if (*aux) {
			aux++;
		}

		value = malloc ((position + 1) * sizeof(char));
		if (value == NULL) {
			// FIXME: no memory, bailout!
		}
		strncpy (value, string, position);
		value[position] = '\0';
		
		printf ("[%s]\n", value);

		d = malloc (sizeof (data));

		d->value.u_str = value;
		d->type = STRING;

		htable_insert (req->parameters, key, d);

		free (key);
		
		string = aux;
	}
	
	return 0;
}

int do_filters (struct request *req, struct response *resp) {
	return 0;
}

int do_handler (struct url_mapping *map[], int map_length, struct request *req, struct response *resp) {

	int i;
	struct url_mapping *current, *tmp;

	if (req->url == NULL) {
		return 0;
	}
	
	current = NULL;
	
	for (i = 0; i < map_length; i++) {
		tmp = (struct url_mapping *) &map[i * 2];
		if (strcmp (req->url, tmp->url) == 0) {
			current = tmp;
			break;
		}
	}

	if (current != NULL) {
		current->handler (req, resp);
	}
	
	return 0;
}

int print_headers (struct response *resp) {

	data *d;

	d = htable_lookup (resp->headers, "cookie");

	if (d != NULL)
		printf ("%s", d->value.u_str);
	
	printf ("Content-Type: text/html\r\n\r\n");
	
	return 1;
}

int draw_page (struct request *req, struct response *resp) {
	
	print_headers (resp);

	template_draw (resp->html, req->parameters, resp->parameters);
	
	return 0;
}