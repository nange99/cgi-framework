#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/cgi_servlet.h"
#include "../lib/cgi_list.h"
#include "../lib/cgi_cookie.h"
#include "../lib/cgi_session.h"
#include "../lib/cgi_table.h"
#include "../lib/cgi_upload.h"

int handle_test(struct request *req, struct response *resp);
int handle_list(struct request *req, struct response *resp);
int handle_expr(struct request *req, struct response *resp);
int handle_table(struct request *req, struct response *resp);
int handle_fileupload(struct request *req, struct response *resp);

int main(int argc, char *argv[])
{

	struct url_mapping map[] = {
	                { .url = "/do.test", .handler = handle_test },
	                { .url = "/do.expr", .handler = handle_expr },
	                { .url = "/do.list", .handler = handle_list },
	                { .url = "/do.table", .handler = handle_table },
	                { .url = "/do.upload", .handler = handle_fileupload } };

	struct config conf = { NULL
	/*default_error_html = "error.html";*/
	};

	cgi_servlet_init(&conf, (struct url_mapping **)&map, 5, NULL);

	return 0;
}

int handle_test(struct request *req, struct response *resp)
{

	double d;
	char *str;

	d = 3.1415;

	if (cgi_request_get_parameter(req, "detail") != NULL) {
		cgi_response_set_html(resp, "html/detail.html");
	} else {
		cgi_response_set_html(resp, "html/error.html");

		cgi_response_add_parameter(resp, "teste",
		                (char *) "valor de teste$!", CGI_STRING);
		cgi_response_add_parameter(resp, "number", (void *) 3222,
		                CGI_INTEGER);
		cgi_response_add_parameter(resp, "pi", (double *) &d, CGI_FLOAT);
	}

	if (cgi_session_exists(req)) {
		str = cgi_session_get_value(req, "user");

		if (str != NULL)
			cgi_response_add_parameter(resp, "user", str,
			                CGI_STRING);
	}

	cgi_cookie_add(req, "nome", "valor", NULL, NULL, NULL, NULL, 0);

	return 1;
}

int handle_list(struct request *req, struct response *resp)
{

	int i;
	cgi_list *l;
	char *v;

	cgi_session_init(req);

	cgi_session_add_value(req, "user", (char *) "zemeyer", CGI_STRING);

	l = cgi_list_create();

	for (i = 0; i < 5; i++) {
		v = malloc(sizeof(char) * 10);
		sprintf(v, "valor %d", i);
		cgi_list_append(l, (void *) v, CGI_STRING);
		free(v);
	}

	cgi_list_remove(l, 3);

	/*
	 valor = cgi_list_get (l, 2, &type);

	 if (valor != NULL) {
	 printf ("type = [%d] ==> valor = [%s]", type, (char *) valor);
	 }

	 printf ("list size = [%d]", cgi_list_size (l));

	 printf ("list size = [%d]", cgi_list_size (l));
	 */

	cgi_response_add_parameter(resp, "list", l, CGI_LIST);

	cgi_response_set_html(resp, "html/list.html");

	return 1;
}

int handle_expr(struct request *req, struct response *resp)
{

	cgi_response_add_parameter(resp, "a", (void *) 1, CGI_INTEGER);
	cgi_response_add_parameter(resp, "b", (void *) 1, CGI_INTEGER);
	cgi_response_add_parameter(resp, "d", (void *) 10, CGI_INTEGER);

	cgi_response_set_html(resp, "html/expr.html");

	return 1;
}

int handle_table(struct request *req, struct response *resp)
{
	cgi_table *t;

	t = cgi_table_create(5, "teste_1", "teste_2", "teste_3", "teste_4", "teste_5");

	if (cgi_table_add_value(t, "teste_1", (char *) "value1", CGI_STRING)) {
		printf ("should fail!\n");
	}

	cgi_table_add_row(t);

	if (cgi_table_add_value(t, "teste_1", (char *) "primeiro valor", CGI_STRING)) {
		printf ("should not fail!\n");
	} else {
		printf ("inserted\n");
	}

	if (cgi_table_add_value(t, "asdf", (char *) "value1", CGI_STRING)) {
		printf ("should fail too!\n");
	}

	cgi_table_add_row(t);
	cgi_table_add_value(t, "teste_1", (char *) "segundo valor", CGI_STRING);
	cgi_table_add_value(t, "teste_2", (char *) "terceiro valor", CGI_STRING);

	cgi_response_add_parameter(resp, "table", (void *)t, CGI_TABLE);
	cgi_response_set_html(resp, "html/table.html");

	return 1;
}

int handle_fileupload(struct request *req, struct response *resp)
{
	cgi_file *f;

	f = cgi_upload_get_file(req, "upfile");

	if (f != NULL) {
		cgi_response_add_parameter(resp, "filename", f->filename, CGI_STRING);
		cgi_response_add_parameter(resp, "tmp_filename", f->tmp_filename, CGI_STRING);
		cgi_response_add_parameter(resp, "err_msg", cgi_upload_get_error_str(f->error), CGI_STRING);
	} else {
		cgi_response_add_parameter(resp, "nofile", (void *) 1, CGI_INTEGER);
	}

	cgi_response_set_html(resp, "html/upload.html");

	return 1;
}
