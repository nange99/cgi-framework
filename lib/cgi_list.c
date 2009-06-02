#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/list.h"
#include "util/data.h"
#include "cgi_servlet.h"
#include "cgi_servlet_private.h"

list *cgi_create_list () {
	list *l;
	
	l = malloc (sizeof (list));
	
	INIT_LIST_HEAD (&l->list);
	
	return l;
}

void cgi_destroy_list (list *l) {
	list *tmp;
	struct list_head *pos, *q;
	
	list_for_each_safe(pos, q, &l->list) {
		tmp = list_entry(pos, list, list);
		list_del(pos);
		destroy_data(tmp->data);
		free (tmp);
	}
	return;
}

list * _cgi_list_init (void *value, parameter_type t) {
	list *new;
	new = malloc (sizeof (list));
	
	new->data = malloc (sizeof (data));
	
	switch (t) {
	case CGI_STRING:
		new->data->value.u_str = strdup ((char *) value);
		new->data->type = STRING;
		break;
	case CGI_INT:
		new->data->value.u_int = (int) value;
		new->data->type = INTEGER;
		break;
	case CGI_DOUBLE:
		break;
	case CGI_LIST:
		break;
	case CGI_ARRAY:
		break;
	}
	
	return new;
}

int cgi_list_append (list *l, void *value, parameter_type t) {

	list *new;
	
	new = _cgi_list_init (value, t);
	
	list_add_tail (&(new->list), &(l->list));
	
	return 1;
}

int cgi_list_preppend (list *l, void *value, parameter_type t) {
	list *new;
	
	new = _cgi_list_init (value, t);
	
	list_add (&(new->list), &(l->list));
	
	return 1;
}

int cgi_list_insert_after (list *l, int index, void *value, parameter_type t) {
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

void *cgi_list_get (list *l, int index, int *type) {
	int i = 0 ;
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
		
	switch (tmp->data->type) {
	case INTEGER:
		type = CGI_INT;
		return (void *) (tmp->data->value.u_int);
	case STRING:
		type = CGI_STRING;
		return (void *) (tmp->data->value.u_str);
	}

}

int cgi_list_remove (list *l, int index) {
	int i = 0 ;
	list *tmp;
	struct list_head *pos, *q;
	
	list_for_each_safe(pos, q, &l->list) {
		if (i == index) {
			tmp = list_entry (pos, list, list);
			list_del(pos);
			destroy_data(tmp->data);
			free (tmp);
			return 1;
		}
		i++;
	}
	return 0;
}

int cgi_list_size (list *l) {
	int i = 0 ;
	struct list_head *pos;
	
	list_for_each (pos, &l->list) {
		i++;
	}
	
	return i;
}