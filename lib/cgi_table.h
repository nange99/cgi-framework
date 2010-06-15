/*
 * cgi_table.h
 *
 *  Created on: Jun 15, 2010
 *      Author: pedro
 */

#ifndef CGI_TABLE_H_
#define CGI_TABLE_H_

typedef struct _cgi_table cgi_table;

cgi_table *cgi_table_create(int cols, ...);
void cgi_table_add_row(cgi_table *t);
int cgi_table_add_value(cgi_table *t,
                        const char *key,
                        void *value,
                        cgi_object_type type);
void cgi_table_destroy(cgi_table *t);

#endif /* CGI_TABLE_H_ */
