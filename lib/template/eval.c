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
	case STRING:
		tmp->value.str = d->value.u_str;
		tmp->type = STR;
		break;
	case INTEGER:
		tmp->value.lnum = d->value.u_int;
		tmp->type = LONG;
		break;
	case FLOAT:
		tmp->value.dnum = d->value.u_double;
		tmp->type = DOUBLE;
		break;
	default:
		tmp->type = -1;
	}
	
	return tmp;
}

int eval_op_equal (struct _context *c, expr_node *r, expr_node *l) {

	if (r->type == OP) {
		int result;
		expr_node *tmp;

		tmp = malloc (sizeof (expr_node));
		tmp->op = NONE;
		tmp->type = BOOL;
		tmp->value.lnum = eval_expression (c, r);
		
		result = eval_op_equal (c, tmp, l);
		
		free (tmp);
		return result;
	}
	
	if (l->type == OP) {
		int result;
		expr_node *tmp;

		tmp = malloc (sizeof (expr_node));
		tmp->op = NONE;
		tmp->type = BOOL;
		tmp->value.lnum = eval_expression (c, l);
		
		result = eval_op_equal (c, r, tmp);
		
		free (tmp);
		return result;
	}

	if ( (r->type == BOOL || r->type == LONG) && (l->type == BOOL || l->type == LONG)) {
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
	
	if (r->type == VAR && l->type == VAR) {
		int result;
		expr_node *rtmp;
		expr_node *ltmp;
		
		rtmp = create_expr_node_for_variable (c, r->value.str);
		ltmp = create_expr_node_for_variable (c, l->value.str);
		if (rtmp != NULL && ltmp != NULL) {
			result = eval_op_equal (c, rtmp, ltmp);
		
			free (ltmp);
			free (rtmp);
			return result;
		}
		return -1;
	}

	return -1;
}

int eval_op_not_equal (struct _context *c, expr_node *r, expr_node *l) {

	if (r->type == OP) {
		int result;
		expr_node *tmp;

		tmp = malloc (sizeof (expr_node));
		tmp->op = NONE;
		tmp->type = BOOL;
		tmp->value.lnum = eval_expression (c, r);
		
		result = eval_op_not_equal (c, tmp, l);
		
		free (tmp);
		return result;
	}
	
	if (l->type == OP) {
		int result;
		expr_node *tmp;

		tmp = malloc (sizeof (expr_node));
		tmp->op = NONE;
		tmp->type = BOOL;
		tmp->value.lnum = eval_expression (c, l);
		
		result = eval_op_not_equal (c, r, tmp);
		
		free (tmp);
		return result;
	}

	if ( (r->type == BOOL || r->type == LONG) && (l->type == BOOL || l->type == LONG)) {
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
	
	if (r->type == VAR && l->type == VAR) {
		int result;
		expr_node *rtmp;
		expr_node *ltmp;
		
		rtmp = create_expr_node_for_variable (c, r->value.str);
		ltmp = create_expr_node_for_variable (c, l->value.str);
		if (rtmp != NULL && ltmp != NULL) {
			result = eval_op_not_equal (c, rtmp, ltmp);
		
			free (ltmp);
			free (rtmp);
			return result;
		}
		return -1;
	}

	return -1;
}

int eval_op_less (struct _context *c, expr_node *r, expr_node *l) {
	
	if (r->type == LONG && l->type == LONG) {
		return (r->value.lnum < l->value.lnum);
	}
	
	if (r->type == STR && l->type != STR) {
		return (strlen (r->value.str) < strlen(l->value.str));
	}

	if (r->type == VAR && l->type != VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, r->value.str);
		if (tmp != NULL) {
			result = eval_op_less (c, tmp, l);
		
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
			result = eval_op_less (c, r, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
	if (r->type == VAR && l->type == VAR) {
		int result;
		expr_node *rtmp;
		expr_node *ltmp;
		
		rtmp = create_expr_node_for_variable (c, r->value.str);
		ltmp = create_expr_node_for_variable (c, l->value.str);
		if (rtmp != NULL && ltmp != NULL) {
			result = eval_op_less (c, rtmp, ltmp);
		
			free (ltmp);
			free (rtmp);
			return result;
		}
		return -1;
	}
	
	return -1;
}

int eval_op_less_equal (struct _context *c, expr_node *r, expr_node *l) {
	
	if (r->type == LONG && l->type == LONG) {
		return (r->value.lnum <= l->value.lnum);
	}
	
	if (r->type == STR && l->type != STR) {
		return (strlen (r->value.str) <= strlen(l->value.str));
	}

	if (r->type == VAR && l->type != VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, r->value.str);
		if (tmp != NULL) {
			result = eval_op_less_equal (c, tmp, l);
		
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
			result = eval_op_less_equal (c, r, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
	if (r->type == VAR && l->type == VAR) {
		int result;
		expr_node *rtmp;
		expr_node *ltmp;
		
		rtmp = create_expr_node_for_variable (c, r->value.str);
		ltmp = create_expr_node_for_variable (c, l->value.str);
		if (rtmp != NULL && ltmp != NULL) {
			result = eval_op_less_equal (c, rtmp, ltmp);
		
			free (ltmp);
			free (rtmp);
			return result;
		}
		return -1;
	}
	
	return -1;
}

int eval_op_more (struct _context *c, expr_node *r, expr_node *l) {
	if (r->type == LONG && l->type == LONG) {
		return (r->value.lnum > l->value.lnum);
	}
	
	if (r->type == STR && l->type != STR) {
		return (strlen (r->value.str) > strlen(l->value.str));
	}

	if (r->type == VAR && l->type != VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, r->value.str);
		if (tmp != NULL) {
			result = eval_op_more (c, tmp, l);
		
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
			result = eval_op_more (c, r, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
	if (r->type == VAR && l->type == VAR) {
		int result;
		expr_node *rtmp;
		expr_node *ltmp;
		
		rtmp = create_expr_node_for_variable (c, r->value.str);
		ltmp = create_expr_node_for_variable (c, l->value.str);
		if (rtmp != NULL && ltmp != NULL) {
			result = eval_op_more (c, rtmp, ltmp);
		
			free (ltmp);
			free (rtmp);
			return result;
		}
		return -1;
	}
	
	return -1;
}

int eval_op_more_equal (struct _context *c, expr_node *r, expr_node *l) {
	if (r->type == LONG && l->type == LONG) {
		return (r->value.lnum >= l->value.lnum);
	}
	
	if (r->type == STR && l->type != STR) {
		return (strlen (r->value.str) >= strlen(l->value.str));
	}

	if (r->type == VAR && l->type != VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, r->value.str);
		if (tmp != NULL) {
			result = eval_op_more_equal (c, tmp, l);
		
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
			result = eval_op_more_equal (c, r, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
	if (r->type == VAR && l->type == VAR) {
		int result;
		expr_node *rtmp;
		expr_node *ltmp;
		
		rtmp = create_expr_node_for_variable (c, r->value.str);
		ltmp = create_expr_node_for_variable (c, l->value.str);
		if (rtmp != NULL && ltmp != NULL) {
			result = eval_op_more_equal (c, rtmp, ltmp);
		
			free (ltmp);
			free (rtmp);
			return result;
		}
		return -1;
	}
	
	return -1;
}

int eval_op_none (struct _context *c, expr_node *n) {
	if (n->type == LONG) {
		return n->value.lnum > 0 ? 1 : 0;
	}
	
	if (n->type == STR) {
		return strlen (n->value.str) > 0 ? 1 : 0 ;
	}
	
	if (n->type == VAR) {
		int result;
		expr_node *tmp;
		
		tmp = create_expr_node_for_variable (c, n->value.str);
		if (tmp != NULL) {
			result = eval_op_none (c, tmp);
		
			free (tmp);
			return result;
		}
		return -1;
	}
	
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
		return eval_op_less (c, n->right, n->left);
		
	case LESS_EQUAL:
		return eval_op_less_equal (c, n->right, n->left);
		
	case MORE:
		return eval_op_more (c, n->right, n->left);
		
	case MORE_EQUAL:
		return eval_op_more_equal (c, n->right, n->left);
		
	case NONE:
		return eval_op_none (c, n);
	}
	return -1;
}

int destroy_expr (expr_node *n) {
	
	expr_node *tmp;
	
	if (n->left != NULL) {
		tmp = n->left;
		tmp->destroy (tmp);
	}
	
	
	if (n->right != NULL) {
		tmp = n->right;
		tmp->destroy (tmp);
	}
	
	if (n->op == NONE) {
		if (n->type == VAR) {
			free (n->value.str);
		} else if (n->type == STR) {
			free (n->value.str);
		}
	}
	
	free (n);
	return 1;
}

expr_node *create_expr_node (expr_node *right, expr_node *left, op_type op) {

	expr_node *n;
	
	n = malloc (sizeof(expr_node));
	n->op = op;
	n->type = OP;

	n->destroy = destroy_expr;

	n->left = left;
	n->right = right;

	return n;
}

expr_node *create_expr_node_for_term (node *n) {

	expr_node *exp;

	exp = malloc (sizeof(expr_node));
	exp->destroy = destroy_expr;

	exp->op = NONE;
	exp->left = NULL;
	exp->right = NULL;
	
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
