#ifndef _CGI_SERVLET_H
#define _CGI_SERVLET_H

#include "cgi_object.h"

/* user of this API shouldn't bother with the implementation details */
struct request;
struct response;

struct url_mapping {
	char *url;
	int (*handler)(struct request *req, struct response *resp);
};

struct filter_mapping {
	char *url;
	int (*handler)(struct request *req, struct response *resp);
};

struct config {
	char *default_error_html;
};

int cgi_servlet_init(struct config *conf,
                     struct url_mapping *map[],
                     int map_length,
                     struct filter_mapping *filters[]);

char *cgi_url_decode(char *str);
char *cgi_url_encode(char *str);

char *cgi_request_get_parameter(struct request *req, char *name);

void cgi_response_set_html(struct response *resp, char *file);
int cgi_response_add_parameter(struct response *resp,
                               char *key,
                               void *value,
                               cgi_object_type type);

int cgi_response_set_content_type(struct response *resp);
int cgi_response_add_cookie(struct response *resp,
                            char *name,
                            char *value,
                            char *max_age,
                            char *path,
                            char *domain,
                            int secure);
int cgi_response_add_header(struct response *resp);

/* list */
typedef struct _list list;

list *cgi_list_create();
void cgi_list_destroy(list *l);

int cgi_list_append(list *l, void *value, cgi_object_type t);
int cgi_list_preppend(list *l, void *value, cgi_object_type t);
int cgi_list_insert_after(list *l, int index, void *value, cgi_object_type t);
void *cgi_list_get(list *l, int index, int *type);
int cgi_list_remove(list *l, int index);
int cgi_list_size(list *l);

#endif
