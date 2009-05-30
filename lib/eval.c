#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "tree.h"
#include "template.h"

expr_node *create_expr_node_for_variable (struct _context *c, char *variable) {
	expr_node *tmp;
	data *d;
	
	tmp = malloc (sizeof (expr_node));
	tmp->op = NONE;
	
	d = template_get_variable (c, variable);
	
	if (d == NULL) {
		free (tmp);
		return NULL;
	}
	
	switch (d->type) {
	case STR:
		tmp->value.str = d->value.u_str;
		tmp->type = STR;
		break;
	case INTEGER:
		tmp->value.lnum = d->value.u_int;
		tmp->type = LONG;
		break;
	}
	
	return tmp;
}

int eval_op_equal (struct _context *c, expr_node *r, expr_node *l) {

	if (r->type == BOOL && l->type == BOOL) {
		return (r->value.lnum == l->value.lnum);
	}

	if (r->type == LONG && l->type == LONG) {
		return (r->value.lnum == l->value.lnum);
	}
	
	if (r->type == STR && l->type == STR) {
		return (strcmp (r->value.str, l->value.str) == 0);
	}
	
	if (r->type == VAR && l->type != VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, r->value.str);
		if (tmp != NULL) {
			result = eval_op_equal (c, tmp, l);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
	if (r->type != VAR && l->type == VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, l->value.str);
		if (tmp != NULL) {
			result = eval_op_equal (c, r, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}

	return -1;
}

int eval_op_not_equal (struct _context *c, expr_node *r, expr_node *l) {

	if (r->type == BOOL && l->type == BOOL) {
		return (r->value.lnum != l->value.lnum);
	}

	if (r->type == LONG && l->type == LONG) {
		return (r->value.lnum != l->value.lnum);
	}
	
	if (r->type == STR && l->type != STR) {
		return (strcmp (r->value.str, l->value.str) != 0);
	}

	if (r->type == VAR && l->type != VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, r->value.str);
		if (tmp != NULL) {
			result = eval_op_not_equal (c, tmp, l);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
	if (r->type != VAR && l->type == VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, l->value.str);
		if (tmp != NULL) {
			result = eval_op_not_equal (c, r, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}

	return -1;
}

int eval_op_less (struct _context *c, expr_node *right, expr_node *left) {
	return -1;
}

int eval_op_less_equal (struct _context *c, expr_node *right, expr_node *left) {
	return -1;
}

int eval_op_more (struct _context *c, expr_node *right, expr_node *left) {
	return -1;
}

int eval_op_more_equal (struct _context *c, expr_node *right, expr_node *left) {
	return -1;
}

int eval_expression (struct _context *c, expr_node *n) {
	switch (n->op) {
	case OR:
		return (eval_expression (c, n->right) || eval_expression (c, n->left));
		
	case AND:
		return (eval_expression (c, n->right) && eval_expression (c, n->left));
		
	case EQUAL:
		return eval_op_equal (c, n->right, n->left);
	case NOT_EQUAL:
		return eval_op_not_equal (c, n->right, n->left);
	case LESS:
		break;
	case LESS_EQUAL:
		break;
	case MORE:
		break;
	case MORE_EQUAL:
		break;
	case NONE:
		printf ("NONE!!!");
		break;
	}
	return -1;
}

expr_node *get_value_for_term (struct _context *c, node *n) {

	expr_node *exp;

	exp = malloc (sizeof(expr_node));
	exp->type = 0;
	
	if (n->type == VARIABLE) {
		exp->value.str = strdup ((char *)template_get_variable (c, n->value.str));
		exp->type = STR;

		//printf ("[%s]\n", exp->value.str);
		free (n->value.str);
		
	} else if (n->type == VALUE) {
		if (n->value_type == STR) {
			exp->value.str = strdup (n->value.str);
			exp->type = STR;

			free (n->value.str);
		} else if (n->value_type == LONG) {
			exp->value.lnum = n->value.lnum;
			exp->type = LONG;
		}
	}

	free (n);
	
	return exp;
}

expr_node *create_expr_node (expr_node *left, expr_node *right, op_type op) {

	expr_node *n;
	
	n = malloc (sizeof(expr_node));
	n->op = op;

	n->left = left;
	n->right = right;

	return n;
}

expr_node *create_expr_node_for_term (node *n) {

	expr_node *exp;

	exp = malloc (sizeof(expr_node));
	exp->type = 0;

	if (n->type == VARIABLE) {
		exp->value.str = strdup (n->value.str);
		exp->type = VAR;

		free (n->value.str);
	} else if (n->type == VALUE) {
		if (n->value_type == STR) {
			exp->value.str = strdup (n->value.str);
			exp->type = STR;
			free (n->value.str);
		} else if (n->value_type == LONG) {
			exp->value.lnum = n->value.lnum;
			exp->type = LONG;
		} else if (n->value_type == DOUBLE) {
			exp->value.dnum = n->value.dnum;
			exp->type = DOUBLE;
		}
	}

	free (n);

	return exp;

}