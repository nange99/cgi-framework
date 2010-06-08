#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include "../util/hashtable.h"
#include "../cgi_object.h"
#include "node.h"

typedef struct _context {
	void *scanner;
	htable *request;
	htable *response;
	htable *variables;
	node *root;
} context;

context * template_context_alloc (htable *req, htable *resp);
int template_context_destroy (context *c);

node *template_parse_include (context *c, char *filename);

void template_print (node *n, context *c);

int template_draw (char *filename, htable *req, htable *resp);

cgi_object *template_get_variable (context *c, char *variable);
int template_register_variable (context *c, const char *variable, void *v, int type);
int template_register_variable_data (context *c, const char *variable, cgi_object *o);
int template_update_variable (context *c, const char *variable, void *v, int type);
int template_update_variable_data (context *c, const char *variable, cgi_object *o);

int template_register_update_variable_data (context *c, char *variable, cgi_object *o);

int template_unregister_variable (context *c, const char *variable);
int template_unregister_free_variable (context *c, const char *variable);

extern int template_parse (context *c);

#endif
