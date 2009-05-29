#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "tree.h"
#include "template.h"

int op_equal (exp_node *right, exp_node *left) {

	if (right->type == BOOL && left->type == BOOL) {
		return (right->value.lnum == left->value.lnum);
	}

	if (right->type == LONG && left->type == LONG) {
		return (right->value.lnum == left->value.lnum);
	}
	
	if (right->type == STR && left->type == STR) {
		return (strcmp (right->value.str, left->value.str) == 0);
	}

	return -1;
}

int op_not_equal (exp_node *right, exp_node *left) {

	if (right->type == BOOL && left->type == BOOL) {
		return (right->value.lnum != left->value.lnum);
	}

	if (right->type == LONG && left->type == LONG) {
		return (right->value.lnum != left->value.lnum);
	}
	
	if (right->type == STR && left->type != STR) {
		return (strcmp (right->value.str, left->value.str) == 0);
	}

	return -1;
}

int op_and (exp_node *right, exp_node *left) {

	if (right->type == BOOL && left->type == BOOL) {
		return (right->value.lnum && left->value.lnum);
	}

	if (right->type == LONG && left->type == LONG) {
		return (right->value.lnum && left->value.lnum);
	}

	return -1;
}

int op_or (exp_node *right, exp_node *left) {

	if (right->type == BOOL && left->type == BOOL) {
		return (right->value.lnum && left->value.lnum);
	}

	if (right->type == LONG && left->type == LONG) {
		return (right->value.lnum && left->value.lnum);
	}

	return -1;
}

int op_less (exp_node *right, exp_node *left) {
	return -1;
}

int op_less_equal (exp_node *right, exp_node *left) {
	return -1;
}

int op_more (exp_node *right, exp_node *left) {
	return -1;
}

int op_more_equal (exp_node *right, exp_node *left) {
	return -1;
}

int evaluate (exp_node *n) {

	exp_node *result;

	result = eval_expression (n->right, n->left, n->type);

	printf ("result == [%d]", result->value.lnum);

	return result->value.lnum;
}

exp_node *eval_expression (exp_node *right, exp_node *left, int op) {

	exp_node *n;
	
	n = malloc (sizeof(exp_node));
	n->type = 0;

	switch (op) {

	case AND:
		n->value.lnum = op_and (right, left);
		n->type = BOOL;
		//printf ("and result == [%d]\n", n->value.lnum);
		break;
		
	case OR:
		n->value.lnum = op_or (right, left);
		n->type = BOOL;
		//printf ("or result == [%d]\n", n->value.lnum);
		break;
	
	case LESS:
		break;
	
	case LESS_EQUAL:
		break;
		
	case MORE:
		break;
		
	case MORE_EQUAL:
		break;	
		
	case NOT_EQUAL:
		n->value.lnum = op_equal (right, left);
		n->type = BOOL;
		//printf ("not equal result == [%d]\n", n->value.lnum);
		break;
		
	case EQUAL:
		n->value.lnum = op_equal (right, left);
		n->type = BOOL;
		//printf ("equal result == [%d]\n", n->value.lnum);
		break;
	case 0:
		printf ("zero");
		break;
	}

	/*
	if (right != NULL) {
		if (right->type == STR) {
			free (right->value.str);
		}
		free (right);
	}
	
	if (left != NULL) {
		if (left->type == STR) {
			free (left->value.str);
		}
		free (left);
	}
	*/

	return n;
}

exp_node *get_value_for_term (struct _context *c, node *n) {

	exp_node *exp;

	exp = malloc (sizeof(exp_node));
	exp->type = 0;
	
	if (n->type == VARIABLE) {
		exp->value.str = strdup (template_get_variable (c, n->value.str));
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

exp_node *create_exp_node (exp_node *left, exp_node *right, int op) {

	exp_node *n;
	
	n = malloc (sizeof(exp_node));
	n->type = op;

	n->left = left;
	n->right = right;

	return n;
}

exp_node *create_exp_node_for_term (node *n) {

	exp_node *exp;

	exp = malloc (sizeof(exp_node));
	exp->type = 0;

	if (n->type == VARIABLE) {
		exp->value.str = strdup (n->value.str);
		exp->type = VAR;

		free (n->value.str);
	} else if (n->type == VALUE) {
		if (n->value_type == STR) {
			exp->value.str = strdup (n->value.str);
			exp->value_type = STR;
			free (n->value.str);
		} else if (n->value_type == LONG) {
			exp->value.lnum = n->value.lnum;
			exp->value_type = LONG;
		} else if (n->value_type == DOUBLE) {
			exp->value.dnum = n->value.dnum;
			exp->value_type = DOUBLE;
		}
	}

	free (n);

	return exp;

}