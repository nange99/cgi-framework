#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "eval.h"
#include "template.h"
#include "../cgi_servlet_private.h"
#include "../cgi_list.h"
#include "../util/list.h"

int destroy_value (node *n) {

	if (n->value_type == STR) {
		free (n->value.str);
	}

	free (n);
	return 1;
}

node *create_value_node (int type, void *value) {

	node *n;

	n = malloc (sizeof (node));

	n->children = NULL;
	n->children_count = 0;
	n->type = VALUE;

	n->destroy = destroy_value;

	switch (type) {

	case STR:
		n->value.str = value;
		n->value_type = type;
		break;
	case LONG:
		n->value.lnum = (long int) value;
		n->value_type = type;
		break;
	case DOUBLE:
		n->value.dnum = *(double *)value;
		n->value_type = type;
		break;
	default:
		break;
	}

	return n;
}

int destroy_variable_node (node *n) {
	free (n->value.str);
	free (n);
	return 1;
}

node *create_variable_node (char *name) {
	node *n;

	n = malloc (sizeof (node));

	n->children = NULL;
	n->children_count = 0;
	n->type = VARIABLE;
	n->value.str = name;

	n->destroy = destroy_variable_node;

	return n;
}

int destroy_html (node *n) {
	int i;
	node *tmp;

	if (n->children_count > 0) {
		for (i = 0; i < n->children_count; i++) {
			tmp = n->children[i];
			if (tmp != NULL)
				tmp->destroy(tmp);
		}
		free (n->children);
	}

	free (n->value.str);
	free (n);
	return 1;
}

int print_html (node *n, struct  _context *c) {
	int i;
	node *tmp;

	printf ("%s", n->value.str);

	if (n->children_count > 0) {
		for (i = 0; i < n->children_count; i++) {
			tmp = n->children[i];
			tmp->print(tmp, c);
		}
	}
	return 1;
}

node *create_html_node (char *html) {

	node *n;

	n = malloc (sizeof(node));
	n->children_count = 0;
	n->children = NULL;
	n->type = HTML;

	n->value.str = html;

	n->destroy = destroy_html;
	n->print = print_html;

	return n;
}

int destroy_include (node *n) {
	node *tmp;

	tmp = n->children[0];

	tmp->destroy (tmp);

	if (n->children[1] != NULL) {
		tmp = n->children[1];
		tmp->destroy (tmp);
	}

	free (n->children);
	free (n);
	return 1;
}

int print_include (node *n, struct _context *c) {

	node *tmp;
	node *root;
	int i;

	root = n->children [1];

	if (root == NULL)
		return -1;

	if (root->children_count > 0) {
		for (i = 0; i < root->children_count; i++) {
			tmp = root->children[i];
			tmp->print(tmp, c);
		}
	}
	return 1;
}

node *create_include_node (struct _context *c, node *n) {
	char *filename;
	node *nn;

	nn = malloc (sizeof (node));

	nn->type = INCLUDE;

	if (n->type == VARIABLE) {

	} else if (n->type == VALUE) {
		filename = strdup (n->value.str);
	}

	nn->children = calloc (2, sizeof (node));
	nn->children [0] = n;
	nn->children [1] = (node *)template_parse_include (c, filename);;
	nn->children_count = 2;

	nn->destroy = destroy_include;
	nn->print = print_include;

	n->parent = nn;

	free (filename);

	return nn;
}

int destroy_echo (node *n) {

	node *tmp;

	tmp = n->children[0];

	tmp->destroy(tmp);

	free (n->children);
	free (n);
	return 1;
}

int print_echo (node *n, struct _context *c) {
	node *tmp;
	cgi_object *value;
	tmp = n->children[0];

	if (tmp->type == VARIABLE) {
		value = template_get_variable (c, tmp->value.str);

		if (value == NULL)
			return -1;

		if (value->type == CGI_STRING) {
			printf ("%s", value->value.u_str);
		} else if (value->type == CGI_INTEGER) {
			printf ("%d", value->value.u_int);
		} else if (value->type == CGI_FLOAT) {
			printf ("%f", value->value.u_double);
		}
	} else if (tmp->type == VALUE) {
		printf ("%s", tmp->value.str);
	}
	return 1;
}

node *create_echo_node (node *n) {
	node *nn;

	nn = malloc (sizeof (node));

	nn->type = ECHO;

	nn->children = calloc (1, sizeof (node));
	nn->children [0] = n;
	nn->children_count = 1;

	nn->destroy = destroy_echo;
	nn->print = print_echo;

	n->parent = nn;

	return nn;
}

int destroy_if (node *n) {

	int i;
	node *tmp;
	expr_node *exp;

	tmp = n->children[0];

	tmp->destroy (tmp);

	if (n->children[1] != NULL) {
		tmp = n->children[1];
		tmp->destroy (tmp);
	}

	if (n->children_count > 2) {
		for (i = 2; i < n->children_count; i++) {
			tmp = n->children [i];
			tmp->destroy (tmp);
		}
	}

	exp = n->value.exp;
	exp->destroy (exp);

	free (n->children);
	free (n);
	return 1;
}

int print_if (node *n, struct _context *c) {

	int i, result;
	node *tmp;
	expr_node *exp;

	exp = n->value.exp;

	result = eval_expression (c, exp);

	if (result == 1) {
		tmp = n->children [0];
		tmp->print (tmp, c);
	} else {
		if (n->children [1] != NULL) {
			tmp = n->children [1];
			tmp->print (tmp, c);
		}
	}

	if (n->children_count > 2) {
		for (i = 2; i < n->children_count; i++) {
			tmp = n->children [i];
			tmp->print (tmp, c);
		}
	}

	return 1;
}

node *create_if_node (expr_node *exp, node *block, node *e) {

	node *nn;

	nn = malloc (sizeof (node));

	nn->type = IF;
	nn->children = calloc (2, sizeof(node));
	nn->value.exp = exp;
	nn->children[0] = block;
	nn->children[1] = e;

	nn->children_count = 2;

	nn->destroy = destroy_if;
	nn->print = print_if;

	if (block != NULL)
		block->parent = nn;

	if (e != NULL)
		e->parent = nn;

	return nn;

}

int destroy_elseif (node *n) {
	int i;
	node *tmp;
	expr_node *exp;

	tmp = n->children[0];

	tmp->destroy (tmp);

	if (n->children[1] != NULL) {
		tmp = n->children[1];
		tmp->destroy (tmp);
	}

	if (n->children_count > 2) {
		for (i = 2; i < n->children_count; i++) {
			tmp = n->children [i];
			tmp->destroy (tmp);
		}
	}

	exp = n->value.exp;
	exp->destroy (exp);

	free (n->children);
	free (n);
	return 1;
}

int print_elseif (node *n, struct _context *c) {
	int result;
	node *tmp;
	expr_node *exp;

	exp = n->value.exp;

	result = eval_expression (c, exp);

	if (result == 1) {
		tmp = n->children [0];
		tmp->print (tmp, c);
	} else {
		if (n->children [1] != NULL) {
			tmp = n->children [1];
			tmp->print (tmp, c);
		}
	}
	return 1;
}

node *create_elseif_node (expr_node *exp, node *block, node *e) {

	node *nn;

	nn = malloc (sizeof (node));

	nn->type = ELSEIF;
	nn->children = calloc (2, sizeof(node));
	nn->value.exp = exp;
	nn->children[0] = block;
	nn->children[1] = e;

	nn->children_count = 2;

	nn->destroy = destroy_elseif;
	nn->print = print_elseif;

	if (block != NULL)
		block->parent = nn;

	if (e != NULL)
		e->parent = nn;

	return nn;
}

int destroy_else (node *n) {
	node *tmp;

	tmp = n->children[0];

	tmp->destroy (tmp);

	free (n->children);
	free (n);
	return 1;
}

int print_else (node *n, struct _context *c) {

	node *tmp;

	tmp = n->children [0];
	tmp->print (tmp, c);

	return 1;
}

node *create_else_node (node *block) {

	node *nn;

	nn = malloc (sizeof (node));

	nn->type = ELSE;

	nn->children = calloc (1, sizeof (node));
	nn->children[0] = block;
	nn->children_count = 1;

	nn->destroy = destroy_else;
	nn->print = print_else;

	block->parent = nn;

	return nn;
}

int destroy_foreach (node *n) {
	node *tmp;

	tmp = n->children[0];
	tmp->destroy (tmp);

	tmp = n->children[1];
	tmp->destroy (tmp);

	tmp = n->children[2];
	tmp->destroy (tmp);

	free (n->children);
	free (n);
	return 1;
}

int print_foreach_list (cgi_list *l, node *var, node *block, struct _context *c) {
	int i = 0;
	cgi_list *tmp;
	struct list_head *pos;
	char *varname;

	varname = var->value.str;

	pos = (&l->list)->next;
	tmp = list_entry (pos, cgi_list, list);

	template_register_variable (c, "count_", 0, LONG);
	template_register_variable (c, "odd_", 0, LONG);

	list_for_each (pos, &l->list) {
		tmp = list_entry(pos, cgi_list, list);

		template_register_update_variable_data (c, varname, tmp->data);

		block->print (block, c);

		template_update_variable (c, "count_", (void *) (i+1), LONG);
		template_update_variable (c, "odd_", (void *) ((i+1)%2), LONG);
		i++;
	}

	template_unregister_variable (c, varname);

	template_unregister_free_variable (c, "count_");
	template_unregister_free_variable (c, "odd_");

	return 1;
}

int print_foreach (node *n, struct _context *c) {
	node *items;
	cgi_object *value;

	items = n->children[1];
	value = template_get_variable (c, items->value.str);

	if (value == NULL)
		return 0;

	if (value->type == CGI_LIST) {
		return print_foreach_list ( (cgi_list *) value->value.u_hash, n->children[0], n->children[2], c);
	}

	/*
	template_register_variable (c, "count_", 0, LONG);
	template_register_variable (c, "odd_", 0, LONG);

	for (i = 0; i < 10; i++) {
		block->print (block, c);
		template_update_variable (c, "count_", (void *) (i+1), LONG);
		template_update_variable (c, "odd_", (void *) ((i+1)%2), LONG);
	}
	*/
	return 1;
}

node *create_foreach_node (node *var, node *items, node *block) {

	node *nn;

	nn = malloc (sizeof (node));

	nn->type = FOREACH;

	nn->children = calloc (3, sizeof (node));
	nn->children[0] = var;
	nn->children[1] = items;
	nn->children[2] = block;
	nn->children_count = 3;

	nn->destroy = destroy_foreach;
	nn->print = print_foreach;

	return nn;

}

node *add_chunk_node (node *cur, node *n) {

	node *nn;
	node **new_children;
	int children_count = 0;
	int i;

	new_children = NULL;

	if (cur != NULL) {
		nn = cur;
	} else {
		nn = malloc (sizeof (node));
		nn->type = CHUNK;
		nn->children_count = 0;
		nn->children = NULL;
	}

	if (nn->children_count > 0) {
		children_count = nn->children_count;
	}

	new_children = calloc (children_count + 1, sizeof(node));

	for (i = 0; i < children_count; i++ ) {
		new_children[i] = nn->children[i];
	}

	new_children[children_count] = n;

	children_count++;

	if (nn->children != NULL)
		free (nn->children);

	nn->children = new_children;
	nn->children_count = children_count;

	n->parent = nn;

	return nn;
}

int destroy_tree (node *n) {

	int i = 0;
	node *tmp;

	if (n->children_count > 0) {
		for (i = 0; i < n->children_count; i++) {
			tmp = n->children[i];
			if (tmp->destroy != NULL)
				tmp->destroy (tmp);
		}
	}

	free (n->children);
	free (n);

	return 0;
}

void debug_print_tree (node *n, int level) {

	int i = 0;
	char *tabs;
	node *tmp;

	if (n == NULL) {
		return;
	}

	tabs = malloc (sizeof(char) * (level+2));
	while (i<level) {
		strcat (tabs, "\t");
		i++;
	}

	printf ("%slevel:%d -> type: %d op_type=[%d] \n", tabs, level, n->type, n->op_type);

	//printf ("%s%s", tabs, n->value.str != NULL ? n->value.str : "-");

	if (n->children_count > 0) {
		level++;
		for (i = 0; i < n->children_count; i++) {
			tmp = n->children[i];
			debug_print_tree (tmp, level);
		}
	}
}

