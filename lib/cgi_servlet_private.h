#ifndef _CGI_SERVLET_PRIVATE_H
#define _CGI_SERVLET_PRIVATE_H

#include "cgi_object.h"
#include "util/hashtable.h"
#include "util/list.h"

#include "cgi_servlet.h"

struct request {
	char *url;
	char method [10];
	htable *parameters;
};

struct response {
	int status;
	htable *headers;
	htable *parameters;
	char *html;
};

struct _list {
	struct list_head list;
	cgi_object *data;
};

int parse_data_string (struct request *req, char *string, int length);

int do_filters (struct request *req, struct response *resp);
int do_handler (struct url_mapping *map[], int map_length, struct request *req, struct response *resp);

int print_headers (struct response *resp);

int draw_page (struct request *req, struct response *resp);

int process_request (struct request *req);

void cgi_request_free (struct request *req);
void cgi_response_free (struct response *resp);

#endif
