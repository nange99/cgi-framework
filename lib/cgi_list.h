/*
 * cgi_list.h
 *
 *  Created on: Jun 8, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_LIST_H_
#define CGI_LIST_H_

typedef struct _list list;

list *cgi_list_create();
void cgi_list_destroy(list *l);

int cgi_list_append(list *l, void *value, cgi_object_type t);
int cgi_list_preppend(list *l, void *value, cgi_object_type t);
int cgi_list_insert_after(list *l, int index, void *value, cgi_object_type t);
void *cgi_list_get(list *l, int index, int *type);
int cgi_list_remove(list *l, int index);
int cgi_list_size(list *l);

#endif /* CGI_LIST_H_ */
