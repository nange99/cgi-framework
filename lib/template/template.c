#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "tree.h"
#include "template.h"
#include "template_scanner.h"
#include "template_parser.h"

data *template_get_variable (context *c, char *variable) {

	char *v;
	data *d;

	v = variable;
	
	if (strncmp (v, "request", 7) == 0) {
		v += 8;
		d = htable_lookup(c->request, v);
		
		if (d != NULL)
			return d;
	} else if (strncmp (v, "response", 8) == 0) {
		v += 9;
		d = htable_lookup(c->response, v);
		
		if (d != NULL)
			return d;
	} else {
		d = htable_lookup(c->variables, v);

		if (d != NULL) {
			return d;
		}
	}
	
	return NULL;
}

int template_register_variable_data (context *c, char *variable, data *d) {
	
	return htable_insert (c->variables, variable, d);
}

int template_register_variable (context *c, char *variable, void *v, int type) {

	data *d;
	
	d = malloc (sizeof (data));

	if (type == STR) {
		d->value.u_str = strdup ((char *) v);
		d->type = STRING;
	}

	if (type == LONG) {
		d->value.u_int = (int) v;
		d->type = INTEGER;
	}
	
	return htable_insert (c->variables, variable, d);
}

int template_register_update_variable_data (context *c, char *variable, data *d) {
	data *tmp;

	tmp = htable_lookup (c->variables, variable);
	
	if (tmp == NULL) {
		htable_insert (c->variables, variable, d);
	} else {
		htable_update (c->variables, variable, d);
	}
}

int template_update_variable_data (context *c, char *variable, data *d) {

	return htable_update (c->variables, variable, d);
}

int template_update_variable (context *c, char *variable, void *v, int type) {

	data *d;

	d = htable_lookup (c->variables, variable);

	if (type == LONG && d->type == INTEGER) {
		d->value.u_int = (int) v;
	}
	
	return 1;
}

int template_unregister_free_variable (context *c, char *variable) {

	htable_remove (c->variables, variable);
	return 1;
}

int template_unregister_variable (context *c, char *variable) {
	
	htable_remove_entry (c->variables, variable);
	return 1;
}

void template_print (node *n, context *c) {
	
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

context * template_context_alloc (htable *req, htable *resp) {

	context *c;

	c = malloc (sizeof(context));
	if (c == NULL) {
		printf ("no memory");
		return NULL;
	}

	c->root = malloc (sizeof (node));
	c->root->type = ROOT;
	c->root->children = NULL;
	c->root->children_count = 0;

	c->request = req;
	c->response = resp;
	c->variables = create_htable (17);

	return c;
}

int template_context_destroy (context *c) {
	
	destroy_htable (c->variables);
	destroy_tree (c->root);
	
	free (c);
	return 1;
}

node *template_parse_include (context *c, char *filename) {

	node *n;
	FILE *f;
	context *local;
	
	f = fopen (filename, "r");

	if (f == NULL) {
		// printf ("include not found. [%s]", filename);
		return NULL;
	}
	
	local = template_context_alloc (c->request, c->response);
	destroy_htable (local->variables);
	local->variables = c->variables;

	template_lex_init (&(local->scanner));
	template_set_in (f, local->scanner);

	template_parse (local);

	template_lex_destroy (local->scanner);

	local->root->destroy = destroy_tree;

	n = local->root;

	free (local);
	fclose (f);

	return n;
}

int template_draw (char *filename, htable *req, htable *resp) {

	context *c;
	FILE *f;

	f = fopen (filename, "r");

	if (f == NULL) {
		printf ("could not open template file... [%s]\n", filename);
		return 0;
	}

	c = template_context_alloc (req, resp);
	
	template_lex_init (&(c->scanner));
	template_set_in (f, c->scanner);
	
	if (template_parse (c) == 0) {
		template_print (c->root, c);
	}

	template_lex_destroy (c->scanner);

	template_context_destroy (c);
	fclose (f);
	
	return 1;
}

/*
int main (int argc, char *argv[]) {

	context *c;
	FILE *f;
	data *d1;

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

	d1 = malloc (sizeof (data));
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
*/
