#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../util/hashtable.h"
#include "tree.h"
#include "template.h"
#include "template_scanner.h"
#include "template_parser.h"
#include "../cgi_object.h"

struct _error_list _template_err;

cgi_object *template_get_variable(context *c, char *variable)
{

	char *v;
	cgi_object *obj;

	v = variable;

	if (strncmp(v, "request", 7) == 0) {
		v += 8;
		obj = htable_lookup(c->request, v);

		if (obj != NULL)
			return obj;

	} else if (strncmp(v, "response", 8) == 0) {
		v += 9;
		obj = htable_lookup(c->response, v);

		if (obj != NULL)
			return obj;

	} else if (strncmp(v, "row", 3) == 0) {

		if (c->cur_row == NULL)
			return NULL;

		v += 4;
		obj = htable_lookup(c->cur_row, v);

		if (obj != NULL)
			return obj;

	} else {
		obj = htable_lookup(c->variables, v);

		if (obj != NULL)
			return obj;
	}

	return NULL;
}

int template_register_variable_data(context *c,
                                    const char *variable,
                                    cgi_object *d)
{

	return htable_insert(c->variables, (char *) variable, d);
}

int template_register_variable(context *c,
                               const char *variable,
                               void *v,
                               int type)
{

	cgi_object *d;

	d = malloc(sizeof(cgi_object));

	if (type == STR) {
		d->value.u_str = strdup((char *) v);
		d->type = CGI_STRING;
	}

	if (type == LONG) {
		d->value.u_int = (int) v;
		d->type = CGI_INTEGER;
	}

	return htable_insert(c->variables, (char *) variable, d);
}

int template_register_update_variable_data(context *c,
                                           char *variable,
                                           cgi_object *d)
{
	cgi_object *tmp;
	int ret;

	tmp = htable_lookup(c->variables, variable);

	if (tmp == NULL) {
		ret = htable_insert(c->variables, variable, d);
	} else {
		ret = htable_update(c->variables, variable, d);
	}
	return ret;
}

int template_update_variable_data(context *c,
                                  const char *variable,
                                  cgi_object *d)
{

	return htable_update(c->variables, (char *) variable, d);
}

int template_update_variable(context *c,
                             const char *variable,
                             void *v,
                             int type)
{

	cgi_object *d;

	d = htable_lookup(c->variables, (char *) variable);

	if (type == LONG && d->type == CGI_INTEGER) {
		d->value.u_int = (int) v;
	}

	return 1;
}

int template_unregister_free_variable(context *c, const char *variable)
{

	htable_remove(c->variables, (char *) variable);
	return 1;
}

int template_unregister_variable(context *c, const char *variable)
{

	htable_remove_entry(c->variables, (char *) variable);
	return 1;
}

void template_print(node *n, context *c)
{

	int i;
	node *tmp;

	if (n->type == ROOT) {
		if (n->children_count > 0) {
			for (i = 0; i < n->children_count; i++) {
				tmp = n->children[i];
				tmp->print(tmp, c);
			}
		}
	}
}

context * template_context_alloc(htable *req, htable *resp, char *filename)
{

	context *c;

	c = malloc(sizeof(context));
	if (c == NULL) {
		printf("no memory");
		return NULL;
	}

	c->root = malloc(sizeof(node));
	c->root->type = ROOT;
	c->root->children = NULL;
	c->root->children_count = 0;

	c->filename = strdup(filename);
	c->request = req;
	c->response = resp;
	c->variables = create_htable(17);
	c->cur_row = NULL;

	return c;
}

int template_context_destroy(context *c)
{
	destroy_htable(c->variables);
	destroy_tree(c->root);

	free(c->filename);
	free(c);
	return 1;
}

node *template_parse_include(context *c, char *filename)
{

	node *n;
	FILE *f;
	context *local;

	f = fopen(filename, "r");

	if (f == NULL) {
		return NULL;
	}

	local = template_context_alloc(c->request, c->response, filename);
	destroy_htable(local->variables);
	local->variables = c->variables;

	template_lex_init(&(local->scanner));
	template_set_in(f, local->scanner);

	template_parse(local);

	template_lex_destroy(local->scanner);

	local->root->destroy = destroy_tree;

	n = local->root;

	free(local);
	fclose(f);

	return n;
}

int template_draw(char *filename, htable *req, htable *resp)
{

	context *c;
	FILE *f;
	struct _error_list *tmp;
	struct list_head *pos, *q;

	f = fopen(filename, "r");

	if (f == NULL) {
		printf("could not open template file... [%s]\n", filename);
		return 0;
	}

	c = template_context_alloc(req, resp, filename);

	INIT_LIST_HEAD(&_template_err.errlist);

	template_lex_init(&(c->scanner));
	template_set_in(f, c->scanner);

	if (template_parse(c) == 0) {
		template_print(c->root, c);
	} else {
		template_send_error();
	}

	list_for_each_safe(pos, q, &_template_err.errlist) {
		tmp = list_entry(pos, struct _error_list, errlist);
		list_del(pos);
		if (tmp->errmsg)
			free(tmp->errmsg);
		free(tmp);
	}

	template_lex_destroy(c->scanner);

	template_context_destroy(c);
	fclose(f);

	return 1;
}

void template_send_error(void)
{
	struct _error_list *tmp = NULL;
	struct list_head *pos;

	printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
	printf("<html>\n");
	printf("<head>\n");
	printf("<title>CGI-Framework Error</title>\n");
	printf("<style type=\"text/css\">\n");
	printf("pre {\n");
	printf("color: red;\n");
	printf("}\n");
	printf("</style>\n");
	printf("</head>\n");
	printf("<body>\n");
	printf("<h1>Error</h1>\n");
	printf("<p>An error happend! Please contact the webmaster of this site and inform the folowing message:</p>\n");
	printf("<pre>\n");

	list_for_each (pos, &_template_err.errlist) {
		tmp = list_entry(pos, struct _error_list, errlist);
		printf("%s\n", tmp->errmsg);
	}

	printf("</pre>\n");
	printf("</body>\n");
	printf("</html>\n\n");

}

void template_append_error(context *c, int linenum, const char *err)
{
	struct _error_list *new;
	int size;
	new = malloc(sizeof(struct _error_list));

	if (c == NULL) {
		new->errmsg = strdup(err);
	} else {
		size = (strlen(err) + strlen(c->filename) + 20) * sizeof(char);

		if (size >= UINT_MAX) {
			free(new);
			return;
		}

		new->errmsg = malloc (size);
		snprintf(new->errmsg, size, "<b>%s</b>\nLine %d: %s", c->filename, linenum, err);
	}

	list_add_tail(&(new->errlist), &(_template_err.errlist));
}

#if 0
int main (int argc, char *argv[]) {

	context *c;
	FILE *f;
	cgi_object *d1;

	f = fopen ("teste.html", "r");

	if (f == NULL) {
		return 1;
	}

	c = malloc (sizeof (context));
	if (c == NULL) {
		printf ("no memory!\n");
		return 1;
	}

	c->root = malloc (sizeof (node));
	c->root->type = ROOT;
	c->root->children = NULL;
	c->root->children_count = 0;

	c->request = create_htable (3);
	c->response = create_htable (3);
	c->variables = create_htable (3);

	d1 = malloc (sizeof (cgi_object));
	d1->value.u_str = strdup ("teste");
	d1->type = STRING;

	htable_insert (c->request, "pedro", d1);

	template_lex_init (&(c->scanner));
	template_set_in (f, c->scanner);

	if (template_parse (c) == 0) {
		template_print (c->root, c);
	}

	//print_tree (c->root, 0);

	destroy_htable (c->request);
	destroy_htable (c->response);
	destroy_htable (c->variables);
	destroy_tree (c->root);

	template_lex_destroy (c->scanner);

	free (c);

	fclose (f);

	return 0;
}
#endif
