#ifndef _EVAL_H
#define _EVAL_H

#include "node.h"

expr_node *create_expr_node (expr_node *left, expr_node *right, op_type op);
expr_node *create_expr_node_for_term (node *n);

expr_node *create_expr_node_for_variable (struct _context *c, char *variable);

int destroy_expr (expr_node *n);

int eval_op_equal (struct _context *c, expr_node *r, expr_node *l);
int eval_op_not_equal (struct _context *c, expr_node *r, expr_node *l);
int eval_op_less (struct _context *c, expr_node *right, expr_node *left); 
int eval_op_less_equal (struct _context *c, expr_node *right, expr_node *left);
int eval_op_more (struct _context *c, expr_node *right, expr_node *left);
int eval_op_more_equal (struct _context *c, expr_node *right, expr_node *left);

int eval_op_none (struct _context *c, expr_node *n);

int eval_expression (struct _context *c, expr_node *n);

expr_node *get_value_for_term (struct _context *c, node *n);

int evaluate (expr_node *n);

#endif