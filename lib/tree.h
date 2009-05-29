#ifndef _TREE_H
#define _TREE_H

#include "node.h"

/* leaf node */
node *create_value_node (int type, void *value);
node *create_variable_node (char *name);
node *create_html_node (char *html);

/* */
node *create_include_node (node *n);
node *create_echo_node (node *n);

node *create_if_node (exp_node *exp, node *block, node *e);
node *create_elseif_node (exp_node *exp, node *block, node *e);
node *create_else_node (node *block);
node *create_foreach_node (node *var, node *items, node *block);
node *create_exp_node (node *left, node *right, int op_type);

node *add_chunk_node (node *cur, node *n);

void destroy_tree (node *n);
void print_tree (node *n, int level);

#endif