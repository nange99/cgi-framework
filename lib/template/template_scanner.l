%{
#include <stdlib.h>
#include "template.h"
#include "template_parser.h"

#define YY_EXTRA_TYPE context*
#define YY_USER_ACTION yylloc->first_line = yylineno;
%}

%option reentrant
%option stack
%option prefix="template_"
%option bison-bridge
%option bison-locations
%option never-interactive
%option noyywrap
%option yylineno
%option nounput
%option noinput
%option outfile="lex.yy.c"

LNUM    [0-9]+
DNUM    ([0-9]*"."[0-9]+)|([0-9]+"."[0-9]*)
EXPONENT_DNUM   (({LNUM}|{DNUM})[eE][+-]?{LNUM})
LABEL   [a-zA-Z_\x7f-\xff\/\.][a-zA-Z0-9_\x7f-\xff\/\.]*
WHITESPACE [ \n\r\t]+
LINEBREAK [\n\r]+
TABS_AND_SPACES [ \t]*
ANY_CHAR (.|[\n])*
NEWLINE ("\r"|"\n"|"\r\n")

%x IN_SCRIPT IN_QUOTE
%%
	char *string_buf_ptr;

<INITIAL>"<?"    { yy_push_state (IN_SCRIPT, yyscanner); }

<IN_SCRIPT>{
	"include"       	{ return T_INCLUDE; }
	"echo"          	{ return T_ECHO; }
	"if"            	{ return T_IF; }
	"elseif"        	{ return T_ELSE_IF; }
	"else"          	{ return T_ELSE; }
	"endif"         	{ return T_END_IF; }
	"foreach"       	{ return T_FOREACH; }
	"endforeach"    	{ return T_END_FOREACH; }
	"in"            	{ return T_IN; }
	"then"          	{ return T_THEN; }
	"=="            	{ return T_EQUAL; }
	"!="            	{ return T_NOT_EQUAL; }
	"<="            	{ return T_LESS_OR_EQUAL; }
	">="            	{ return T_MORE_OR_EQUAL; }
	"<"             	{ return T_LESS; }
	">"             	{ return T_MORE; }
	"||"            	{ return T_OR; }
	"&&"            	{ return T_AND; }
	{LNUM}			{ yylval->lnumber = atoi (yytext); return T_LNUMBER;}
	{DNUM}|{EXPONENT_DNUM}	{ yylval->dnumber = atof (yytext); return T_DNUMBER;}
	{LABEL}((\.)?{LABEL})*	{ yylval->text = strdup (yytext); return T_VARIABLE; }
	\"			{ 
		string_buf_ptr = malloc (sizeof(char));
		yy_push_state (IN_QUOTE, yyscanner); 
	}
	"("			{ return '('; }
	")"			{ return ')'; }
	{TABS_AND_SPACES}	;
	{WHITESPACE}		;
	"?>"({LINEBREAK}*)	{ yy_pop_state (yyscanner); }
}

<IN_QUOTE>\"		{ 
	yylval->text = strdup(string_buf_ptr);
	yy_pop_state (yyscanner);
	free (string_buf_ptr);
	return T_STRING;
}
<IN_QUOTE>({LABEL}|{TABS_AND_SPACES})((\.)?{LABEL}|{TABS_AND_SPACES})* 	{
	string_buf_ptr = realloc (string_buf_ptr, sizeof(char)* (yyleng + 1));
	strncpy (string_buf_ptr, yytext, yyleng);
	string_buf_ptr[yyleng] = '\0';
}

<INITIAL>(([^<]|"<"[^?%s<]){1,400})|"<s"|"<"	{ 
	yylval->text = strdup (yytext); 
	return T_INLINE_HTML; 
}

<INITIAL>{TABS_AND_SPACES}	;
<INITIAL>{WHITESPACE}		;

%%
