#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/cgi_servlet.h"
#include "../lib/cgi_list.h"
#include "../lib/cgi_cookie.h"
#include "../lib/cgi_session.h"

int handle_test (struct request *req, struct response *resp);
int handle_list (struct request *req, struct response *resp);
int handle_expr (struct request *req, struct response *resp);
int handle_asdf (struct request *req, struct response *resp);

int main(int argc, char *argv[])
{

	struct url_mapping map[] = {
		{.url = "/do.test", .handler = handle_test},
		{.url = "/do.expr", .handler = handle_expr},
		{.url = "/do.list", .handler = handle_list},
		{.url = "/do.qwer", .handler = handle_asdf},
		{.url = "/do.zxcv", .handler = handle_list}
	};

	struct config conf = {
		NULL
		/*default_error_html = "error.html";*/
	};

	cgi_servlet_init (&conf, &map, 5, NULL);

	return 0;
}

int handle_test (struct request *req, struct response *resp) {

	double d;
	char *str;

	d = 3.1415;

	if (cgi_request_get_parameter (req, "detail") != NULL) {
		cgi_response_set_html (resp, "html/detail.html");
	} else {
		cgi_response_set_html (resp, "html/teste.html");

		cgi_response_add_parameter (resp, "teste", (char *)"valor de teste$!", CGI_STRING);
		cgi_response_add_parameter (resp, "number", (void *)3222, CGI_INTEGER);
		cgi_response_add_parameter (resp, "pi", (double *) &d, CGI_FLOAT);
	}

	if (cgi_session_exists(req)) {
		str = cgi_session_get_value(req, "user");

		if (str != NULL)
			cgi_response_add_parameter (resp, "user", str, CGI_STRING);
	}

	cgi_cookie_add (req, "nome", "valor", NULL, NULL, NULL, 0);

	return 1;
}

int handle_list (struct request *req, struct response *resp) {

	int i;
	cgi_list *l;
	char *v;

	cgi_session_init (req);

	cgi_session_add_value(req, "user", (char *)"zemeyer", CGI_STRING);

	l = cgi_list_create ();

	for (i = 0; i < 5; i++) {
		v = malloc (sizeof (char) * 10);
		sprintf (v, "valor %d", i);
		cgi_list_append (l, (void *)v, CGI_STRING);
		free (v);
	}

	cgi_list_remove (l, 3);

	/*
	valor = cgi_list_get (l, 2, &type);

	if (valor != NULL) {
		printf ("type = [%d] ==> valor = [%s]", type, (char *) valor);
	}

	printf ("list size = [%d]", cgi_list_size (l));

	printf ("list size = [%d]", cgi_list_size (l));
	*/

	cgi_response_add_parameter (resp, "list", l, CGI_LIST);

	cgi_response_set_html (resp, "html/list.html");

	return 1;
}

int handle_expr (struct request *req, struct response *resp) {

	cgi_response_add_parameter (resp, "a", (void *) 1, CGI_INTEGER);
	cgi_response_add_parameter (resp, "b", (void *) 1, CGI_INTEGER);
	cgi_response_add_parameter (resp, "d", (void *) 10, CGI_INTEGER);

	cgi_response_set_html (resp, "html/expr.html");

	return 1;
}

int handle_asdf (struct request *req, struct response *resp) {

	cgi_response_set_html (resp, "html/asdf.html");

	return 1;
}
