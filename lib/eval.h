#ifndef _EVAL_H
#define _EVAL_H

#include "node.h"

exp_node *create_exp_node (exp_node *left, exp_node *right, int op);
exp_node *create_exp_node_for_term (node *n);

exp_node *eval_expression (exp_node *right, exp_node *left, int op);

exp_node *get_value_for_term (struct _context *c, node *n);

int evaluate (exp_node *n);

#endif