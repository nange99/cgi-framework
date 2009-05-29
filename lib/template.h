#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include "util/hashtable.h"
#include "cgi_servlet.h"
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

int template_draw (char *filename, struct request *req, struct response *resp);

char *template_get_variable (context *c, char *variable);

int template_register_variable (context *c, char *variable, void *v, int type);
int template_update_variable (context *c, char *variable, void *v, int type);


#endif