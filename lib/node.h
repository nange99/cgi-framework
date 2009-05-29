#ifndef _NODE_H
#define _NODE_H

enum node_type {
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
};

enum value_type {
	LONG = 1,
	DOUBLE,
	STR,
	BOOL,
	VAR
};

enum op_type {
	EQUAL = 1,
	NOT_EQUAL,
	MORE,
	MORE_EQUAL,
	LESS,
	LESS_EQUAL,
	OR,
	AND
};

struct _context;

typedef struct _exp_node {
	int type;
	union {
		int lnum;
		double dnum;
		char *str;
	} value;
	int value_type;
	struct _exp_node *right;
	struct _exp_node *left;
} exp_node;

typedef struct _node {
	int type;
	union {
		int lnum;
		double dnum;
		char *str;
		exp_node *exp;
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