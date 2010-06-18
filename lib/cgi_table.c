/*
 * cgi_table.c
 *
 *  Created on: Jun 10, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "util/hashtable.h"
#include "util/list.h"
#include "cgi_object.h"
#include "cgi_list.h"
#include "cgi_table_priv.h"
#include "cgi_table.h"

struct _cgi_table_header *_cgi_table_header_init(void)
{
	struct _cgi_table_header *th;

	th = malloc (sizeof(struct _cgi_table_header));
	if (th == NULL)
		return NULL;

	INIT_LIST_HEAD(&th->list);

	return th;
}

struct _cgi_table_row *_cgi_table_row_init(int cols, int init_htable)
{
	struct _cgi_table_row *tr;

	tr = malloc(sizeof(struct _cgi_table_row));
	if(tr == NULL)
		return NULL;

	INIT_LIST_HEAD(&tr->list);

	if (init_htable)
		tr->data = create_htable(cols);

	return tr;
}

cgi_table *_cgi_table_init(int cols)
{
	cgi_table *t;

	t = malloc(sizeof(struct _cgi_table));

	if (t == NULL)
		return NULL; /*TODO handle out of memory */

	t->headers = _cgi_table_header_init();
	if (t->headers == NULL)
		goto err1;

	t->rows = _cgi_table_row_init(cols, 0);
	if (t->rows == NULL)
		goto err2;

	t->cols = cols;
	t->cur_row = NULL;

	return t;
err2:
	free(t->headers);
err1:
	free(t);
	return NULL;
}

void _cgi_table_add_header(cgi_table *t, char *header)
{
	struct _cgi_table_header *new;
	new = _cgi_table_header_init();

	new->header = strdup(header);

	list_add_tail(&(new->list), &(t->headers->list));
}

int _cgi_table_allow_header(cgi_table *t, char *key)
{
	int allow = 0;
	struct _cgi_table_header *tr;
	struct list_head *pos;

	list_for_each (pos, &t->headers->list) {
		tr = list_entry(pos, struct _cgi_table_header, list);
		if (strncmp(tr->header, key, strlen(key)) == 0) {
			allow = 1;
			break;
		}
	}

	return allow;
}

cgi_table *cgi_table_create(int cols, ...)
{
	cgi_table *t;
	va_list args;
	int i;

	if (cols < 0)
		return NULL;

	t = _cgi_table_init(cols);

	va_start(args, cols);
	for (i = 0; i < cols; i++) {
		char *colname = va_arg (args, char *);
		_cgi_table_add_header(t, colname);
	}
	va_end(args);

	return t;
}

void cgi_table_add_row(cgi_table *t)
{
	struct _cgi_table_row *new;
	new = _cgi_table_row_init(t->cols, 1);

	t->cur_row = new;

	list_add_tail(&(new->list), &(t->rows->list));
}

int cgi_table_add_value(cgi_table *t,
                        const char *key,
                        void *value,
                        cgi_object_type type)
{
	htable *row_data;
	cgi_object *o;
	int ret;

	if (t->cur_row == NULL)
		return 1;

	row_data = t->cur_row->data;

	if (!_cgi_table_allow_header(t, (char *) key)) {
		return 2;
	}

	o = malloc(sizeof(cgi_object));
	o->type = type;

	switch (type) {
	case CGI_STRING:
		o->value.u_str = strdup(value);
		break;
	case CGI_INTEGER:
		o->value.u_int = (int) value;
		break;
	case CGI_FLOAT:
		o->value.u_double = *(double *) value;
		break;
	case CGI_LIST:
	case CGI_TABLE:
	case CGI_FILE:
	case CGI_COOKIES:
		free(o);
		return 0;
		break;
	}

	ret = htable_insert(row_data, (char *) key, o);

	if (ret == 2) {
		/* key already exists, should we update? */
		return 3;
	}

	return 0;
}

void _cgi_table_free_headers (cgi_table *t)
{
	struct _cgi_table_header *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &t->headers->list) {
		tmp = list_entry(pos, struct _cgi_table_header, list);
		list_del(pos);
		free(tmp->header);
		free(tmp);
	}

	free (t->headers);
	return;
}

void _cgi_table_free_rows (cgi_table *t)
{
	struct _cgi_table_row *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &t->rows->list) {
		tmp = list_entry(pos, struct _cgi_table_row, list);
		list_del(pos);
		destroy_htable(tmp->data);
		free(tmp);
	}

	free (t->rows);
	return;
}

void cgi_table_destroy(cgi_table *t)
{
	_cgi_table_free_headers(t);
	_cgi_table_free_rows(t);

	free (t);

	return;
}
