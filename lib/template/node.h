#ifndef _NODE_H
#define _NODE_H

typedef enum {
	HTML = 1,
	VALUE,
	VARIABLE,
	INCLUDE,
	ECHO,
	IF,
	ELSEIF,
	ELSE,
	FOREACH,
	CHUNK,
	ROOT,
	EXP
} node_type;

typedef enum {
	LONG = 1,
	DOUBLE,
	STR,
	BOOL,
	VAR,
	OP
} value_type;

typedef enum {
	EQUAL = 1,
	NOT_EQUAL,
	MORE,
	MORE_EQUAL,
	LESS,
	LESS_EQUAL,
	OR,
	AND,
	NONE
} op_type;

struct _context;

typedef struct _expr_node {
	op_type op;
	union {
		int lnum;
		double dnum;
		char *str;
	} value;
	value_type type;
	int (*destroy) (struct _expr_node *node);
	struct _expr_node *right;
	struct _expr_node *left;
} expr_node;

typedef struct _node {
	node_type type;
	union {
		int lnum;
		double dnum;
		char *str;
		expr_node *exp;
	} value;
	int value_type;
	int children_count;
	int op_type;
	int (*print) (struct _node *node, struct _context *c);
	int (*destroy) (struct _node *node);
	struct _node *parent;
	struct _node **children;
} node;

#endif
