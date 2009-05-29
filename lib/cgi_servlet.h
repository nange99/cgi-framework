#ifndef _CGI_SERVLET_H
#define _CGI_SERVLET_H

#include "util/data.h"
#include "util/hashtable.h"
#include "util/list.h"

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

struct url_mapping {
	char *url;
	int (*handler) (struct request *req, struct response *resp);
};

struct filter_mapping {
	char *url;
	int (*handler) (struct request *req, struct response *resp);
};

struct config {
	char *default_error_html;
};

int cgi_servlet_init (struct config *conf, struct url_mapping *map[], int map_length, struct filter_mapping *filters[]);

char *cgi_request_get_parameter (struct request *req, char *name);

int cgi_response_add_parameter_int (struct response *resp, char *key, int value);
int cgi_response_add_parameter_str (struct response *resp, char *key, char *value);

int cgi_response_set_content_type (struct response *resp);
int cgi_response_add_cookie (struct response *resp, char *name, char *value, char *max_age, char *path, char *domain, int secure);
int cgi_response_add_header (struct response *resp);


#endif