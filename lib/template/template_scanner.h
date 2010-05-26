#ifndef _TEMPLATE_SCANNER_H
#define _TEMPLATE_SCANNER_H

#include <stdlib.h>

int template_lex_init (void *);
int template_set_in (FILE *, void *);
int template_lex_destroy (void *);

#endif