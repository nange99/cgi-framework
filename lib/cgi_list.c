#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/list.h"
#include "cgi_object.h"
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"
#include "cgi_list.h"

list *cgi_list_create ()
{
	list *l;

	l = malloc (sizeof(list));

	INIT_LIST_HEAD (&l->list);

	return l;
}

void cgi_list_destroy (list *l)
{
	list *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &l->list) {
		tmp = list_entry(pos, list, list);
		list_del (pos);
		cgi_object_destroy (tmp->data);
		free (tmp);
	}

	free (l);
	return;
}

list * _cgi_list_init (void *value, cgi_object_type t)
{
	list *new;
	new = malloc (sizeof(list));

	new->data = malloc (sizeof(cgi_object));

	new->data->type = t;
	switch (t) {
	case CGI_STRING:
		new->data->value.u_str = strdup ((char *) value);
		break;
	case CGI_INTEGER:
		new->data->value.u_int = (int) value;
		break;
	case CGI_FLOAT:
		break;
	case CGI_LIST:
		break;
	case CGI_TABLE:
		break;
	}

	return new;
}

int cgi_list_append (list *l, void *value, cgi_object_type t)
{

	list *new;

	new = _cgi_list_init (value, t);

	list_add_tail (&(new->list), &(l->list));

	return 1;
}

int cgi_list_preppend (list *l, void *value, cgi_object_type t)
{
	list *new;

	new = _cgi_list_init (value, t);

	list_add (&(new->list), &(l->list));

	return 1;
}

int cgi_list_insert_after (list *l, int index, void *value, cgi_object_type t)
{
	int i = 0;
	list *new, *tmp;
	struct list_head *pos;

	list_for_each (pos, &l->list) {
		if (i == index) {
			tmp = list_entry(pos, list, list);
			break;
		}
		i++;
	}

	if (tmp == NULL)
		return 0;

	new = _cgi_list_init (value, t);
	list_add (&(new->list), &(tmp->list));

	return 1;
}

void *cgi_list_get (list *l, int index, int *type)
{
	int i = 0;
	list *tmp;
	struct list_head *pos;

	list_for_each (pos, &l->list) {
		if (i == index) {
			tmp = list_entry (pos, list, list);
			break;
		}
		i++;
	}

	if (tmp == NULL)
		return 0;

	type = (int *) tmp->data->type;
	switch (tmp->data->type) {
	case CGI_INTEGER:
		return (void *) (tmp->data->value.u_int);
	case CGI_STRING:
		return (void *) (tmp->data->value.u_str);
	default:
		return NULL;
	}

	return NULL;
}

int cgi_list_remove (list *l, int index)
{
	int i = 0;
	list *tmp;
	struct list_head *pos, *q;

	list_for_each_safe(pos, q, &l->list) {
		if (i == index) {
			tmp = list_entry (pos, list, list);
			list_del (pos);
			cgi_object_destroy (tmp->data);
			free (tmp);
			return 1;
		}
		i++;
	}
	return 0;
}

int cgi_list_size (list *l)
{
	int i = 0;
	struct list_head *pos;

	list_for_each (pos, &l->list) {
		i++;
	}

	return i;
}
