#ifndef _EVAL_H
#define _EVAL_H

#include "node.h"

exp_node *eval_expression (exp_node *right, exp_node *left, int op);
exp_node *get_value_for_term (struct _context *c, node *n);

#endif