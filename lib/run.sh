#!/bin/bash
bison -r all -d template_parser.y
flex -o template_scanner.yy.c template_scanner.l
gcc -g template_scanner.yy.c template_parser.tab.c template.c eval.c tree.c \
util/data.c util/hash.c util/hashtable.c util/list.c -o teste -lfl -ly -Wall
