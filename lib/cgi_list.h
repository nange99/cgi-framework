/*
 * cgi_list.h
 *
 *  Created on: Jun 8, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_LIST_H_
#define CGI_LIST_H_

typedef struct _list cgi_list;

cgi_list *cgi_list_create(void);
void cgi_list_destroy(cgi_list *l);

int cgi_list_append(cgi_list *l, void *value, cgi_object_type t);
int cgi_list_preppend(cgi_list *l, void *value, cgi_object_type t);
int cgi_list_insert_after(cgi_list *l,
                          int index,
                          void *value,
                          cgi_object_type t);
void *cgi_list_get(cgi_list *l, int index, int *type);
int cgi_list_remove(cgi_list *l, int index);
int cgi_list_size(cgi_list *l);

#endif /* CGI_LIST_H_ */
