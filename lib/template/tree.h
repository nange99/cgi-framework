#ifndef _TREE_H
#define _TREE_H

#include "node.h"

/* leaf node */
node *create_value_node (int type, void *value);
node *create_variable_node (char *name);
node *create_html_node (char *html);

/* */
node *create_include_node (struct _context *c, node *n);
node *create_echo_node (node *n);

node *create_if_node (expr_node *exp, node *block, node *e);
node *create_elseif_node (expr_node *exp, node *block, node *e);
node *create_else_node (node *block);
node *create_foreach_node (node *var, node *items, node *block);

node *add_chunk_node (node *cur, node *n);

/* destructors */
int destroy_value (node *n);
int destroy_variable_node (node *n);
int destroy_html (node *n);

int destroy_include (node *n);
int destroy_echo (node *n);

int destroy_if (node *n);
int destroy_elseif (node *n);
int destroy_else (node *n);
int destroy_foreach (node *n);

int destroy_tree (node *n);

/* recursive printer */
int print_value (node *n, struct _context *c);
int print_variable_node (node *n, struct _context *c);
int print_html (node *n, struct _context *c);

int print_include (node *n, struct _context *c);
int print_echo (node *n, struct _context *c);

int print_if (node *n, struct _context *c);
int print_elseif (node *n, struct _context *c);
int print_else (node *n, struct _context *c);
int print_foreach_list (cgi_list *l, node *var, node *block, struct _context *c);
int print_foreach (node *n, struct _context *c);

/* debug */
void debug_print_tree (node *n, int level);

#endif
