#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/cgi_servlet.h"

int handle_list (struct request *req, struct response *resp);
int handle_asdf (struct request *req, struct response *resp);

int main(int argc, char *argv[]) 
{

	struct url_mapping map[] = { 
		{"/do.list", handle_list},
		{"/do.asdf", handle_asdf},
		{"/do.login", handle_list},
		{"/do.qwer", handle_asdf},
		{"/do.zxcv", handle_list}
	};

	struct config conf = {
		NULL
		/*default_error_html = "error.html";*/
	};

	cgi_servlet_init (&conf, &map, 5, NULL);

}

int handle_list (struct request *req, struct response *resp) {

	double d;

	d = 3.1415;
	
	if (cgi_request_get_parameter (req, "detail") != NULL) {
		cgi_response_set_html (resp, "html/detail.html");
	} else {
		cgi_response_set_html (resp, "html/teste.html");

		cgi_response_add_parameter (resp, "teste", "valor de teste$!", CGI_STRING);
		cgi_response_add_parameter (resp, "number", (void *)3222, CGI_INT);
		cgi_response_add_parameter (resp, "pi", (double *) &d, CGI_DOUBLE);	
	}

	cgi_response_add_cookie (resp, "nome", "valor", NULL, NULL, NULL, 0);
	
	return 0;
}

int handle_asdf (struct request *req, struct response *resp) {

	printf ("handle_asdf( )\n");

	cgi_response_set_html (resp, "html/asdf.html");
	
	return 0;
}
