%{
#include <stdio.h>
#include "template.h"
#include "eval.h"
#include "tree.h"


#define YYERROR_VERBOSE 1

#define scanner context->scanner

int template_lex ();
int template_error ();
%}

%pure_parser
%name-prefix="template_"
%locations
%defines
/* %error-verbose */
%parse-param { context *context }
%lex-param { void *scanner  }

%union {
	char *text;
	long int lnumber;
	double dnumber;
	struct _node *node;
	struct _expr_node *snode;
}

%left T_INCLUDE
%left T_OR
%left T_AND
%nonassoc T_EQUAL T_NOT_EQUAL 
%nonassoc T_LESS T_LESS_OR_EQUAL T_MORE T_MORE_OR_EQUAL 
%token T_IF
%left T_ELSE_IF
%left T_ELSE
%left T_END_IF
%token T_ECHO
%token T_FOREACH
%token T_END_FOREACH
%token T_IN
%token T_THEN
%token <text> T_INLINE_HTML
%token <text> T_VARIABLE 
%token <lnumber> T_LNUMBER
%token <dnumber> T_DNUMBER
%token <text> T_STRING

%type <node> block
%type <node> chunks
%type <node> chunk
%type <node> statements
%type <node> atomdir
%type <snode> expr
%type <node> term
%type <node> foreach
%type <node> condition
%type <node> else

%%

block:
    chunks			{ add_chunk_node (context->root, $1); }
    |				{ $$ = NULL; }
    ;

chunks:
    chunks chunk		{ $$ = add_chunk_node ($$, $2);}
    | chunk			{ }
    ;

chunk:
    statements			
    ;

statements:
    atomdir
    | condition
    | foreach
    ;

atomdir:
    T_ECHO term 		{ $$ = create_echo_node ($2); }
    | T_INCLUDE term 		{ $$ = create_include_node (context, $2); }
    | T_INLINE_HTML		{ $$ = create_html_node ($1); }
    ;

condition:
    T_IF expr T_THEN chunks else T_END_IF	{ $$ = create_if_node ($2, $4, $5); }
    | T_IF expr error	{ template_append_error(NULL, 0, "Malformed if clause."); YYERROR; }
    ;

else:
    T_ELSE_IF expr T_THEN chunks else		{ $$ = create_elseif_node ($2, $4, $5); }
    | T_ELSE chunks				{ $$ = create_else_node ($2); }
    | /* empty */				{ $$ = NULL; }
    ;

foreach:
    T_FOREACH T_VARIABLE T_IN T_VARIABLE chunks T_END_FOREACH
				{ $$ = create_foreach_node (create_variable_node($2), create_variable_node($4), $5); }
    ;

term:
    T_STRING				{ $$ = create_value_node (STR, $1); }
    | T_VARIABLE			{ $$ = create_variable_node ($1); } 
    | T_LNUMBER				{ $$ = create_value_node (LONG, (void *) $1); }
    | T_DNUMBER				{ $$ = create_value_node (DOUBLE, (double *) &$1); }
    ;

expr:
    expr T_MORE_OR_EQUAL expr		{ $$ = create_expr_node ($1, $3, MORE_EQUAL); }
    | expr T_LESS_OR_EQUAL expr		{ $$ = create_expr_node ($1, $3, LESS_EQUAL); }
    | expr T_EQUAL expr			{ $$ = create_expr_node ($1, $3, EQUAL); }
    | expr T_NOT_EQUAL expr		{ $$ = create_expr_node ($1, $3, NOT_EQUAL); }
    | expr T_MORE expr			{ $$ = create_expr_node ($1, $3, MORE); }
    | expr T_LESS expr			{ $$ = create_expr_node ($1, $3, LESS); }
    | expr T_OR expr			{ $$ = create_expr_node ($1, $3, OR); }
    | expr T_AND expr			{ $$ = create_expr_node ($1, $3, AND); }
    | '(' expr ')'			{ $$ = $2; }
    | '(' expr error	{ template_append_error(NULL, 0, "Unmatched parenthesis."); YYERROR; }
    | term				{ $$ = create_expr_node_for_term ($1); }
    | term error		{ template_append_error(NULL, 0, "Invalid expression."); YYERROR; }
    ;

%%

int template_error (YYLTYPE *locp, context *c, const char* err)
{
	template_append_error(c, locp->first_line, (char *)err);
	return 1;
}