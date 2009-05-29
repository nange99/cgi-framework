#ifndef _CGI_SERVLET_PRIVATE_H
#define _CGI_SERVLET_PRIVATE_H

#include "cgi_servlet.h"

int parse_data_string (struct request *req, char *string, int length);

int do_filters (struct request *req, struct response *resp);
int do_handler (struct url_mapping *map[], int map_length, struct request *req, struct response *resp);

int print_headers (struct response *resp);

int draw_page (struct request *req, struct response *resp);

int process_request (struct request *req);

void cgi_request_free (struct request *req);
void cgi_response_free (struct response *resp);

#endif