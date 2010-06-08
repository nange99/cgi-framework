/*
 * cgi_cookie.c
 *
 *  Created on: Mar 16, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/list.h"
#include "cgi_servlet.h"
#include "cgi_servlet_priv.h"
#include "cgi_list.h"
#include "cgi_cookie.h"
#include "cgi_cookie_priv.h"

struct _cookie {
	struct list_head list;
	char *name;
	char *value;
	char *max_age;
	char *path;
	char *domain;
	int secure;
	int send;
};

char _cgi_cookie_str[] = "cookies";

static void _cgi_cookie_process_request(struct request *req)
{
	cgi_object *olist;
	int position;
	char *cookie_str, *aux;
	char *tmp;
	struct _cookie *cookies;
	struct _cookie *c;

	if (getenv("HTTP_COOKIE") == NULL)
		return;

	olist = htable_lookup(req->headers, (char *) &_cgi_cookie_str);

	if (olist == NULL)
		return;

	cookies = (struct _cookie *)olist->value.u_hash;

	cookie_str = getenv("HTTP_COOKIE");
	aux = cookie_str;

	printf("cookie_str=[%s]\n", cookie_str);

	while (cookie_str) {
		position = 0;

		c = malloc(sizeof(struct _cookie));
		if (!c)
			return;

		while (*aux++ != '=')
			position++;

		c->name = malloc(position + 1);
		if (!c->name)
			return;

		strncpy(c->name, cookie_str, position);
		c->name[position] = '\0';

		position = 0;
		cookie_str = aux;
		if ((strchr(cookie_str, ';')) == NULL) {
			aux = NULL;
			position = strlen(cookie_str);
		} else {
			while (*aux++ != ';')
				position++;
			/* blank space after ';' */
			aux++;
		}

		c->value = malloc(position + 1);
		if (!c->value)
			return;

		strncpy(c->value, cookie_str, position);
		c->value[position] = '\0';

		tmp = cgi_url_decode(c->value);

		if (tmp != NULL) {
			free(c->value);
		}

		c->value = tmp;

		c->max_age = NULL;
		c->domain = NULL;
		c->path = NULL;
		c->secure = 0;
		c->send = 0;

		list_add_tail (&(c->list), &(cookies->list));
		cookie_str = aux;
	}

}

void cgi_cookie_init(struct request *req)
{
	cgi_object *obj;
	struct _cookie *cookies;

	cookies = malloc(sizeof(struct _cookie));

	if (cookies == NULL)
		return; /* TODO: handle mem error */

	INIT_LIST_HEAD (&cookies->list);

	obj = cgi_object_init();

	obj->value.u_hash = cookies;
	obj->type = CGI_COOKIES;

	htable_insert(req->headers, (char *) &_cgi_cookie_str, obj);

	_cgi_cookie_process_request(req);
}

static void _cookie_free (struct _cookie *c)
{
	free(c->name);
	free(c->value);

	if (c->max_age != NULL)
		free(c->max_age);

	if (c->path != NULL)
		free(c->path);

	if (c->domain != NULL)
		free(c->domain);

	free(c);
}

void cgi_cookie_destroy(struct request *req)
{
	struct list_head *pos, *q;
	cgi_object *olist;
	struct _cookie *cookies;
	struct _cookie *tmp;

	olist = htable_lookup(req->headers, (char *) &_cgi_cookie_str);

	if (olist == NULL)
		return;

	cookies = (struct _cookie *)olist->value.u_hash;

	list_for_each_safe(pos, q, &cookies->list) {
		tmp = list_entry(pos, struct _cookie, list);
		list_del (pos);
		_cookie_free (tmp);
	}

	free(olist->value.u_hash);
	olist->value.u_hash = NULL;

	htable_remove(req->headers, (char *) &_cgi_cookie_str);
	return;
}

static struct _cookie *_cgi_cookie_find_by_name(cgi_list *cookies, const char *name)
{
	struct _cookie *tmp = NULL;
	struct list_head *pos;

	list_for_each (pos, &cookies->list) {
		tmp = list_entry (pos, struct _cookie, list);
		if (strncmp(name, tmp->name, strlen(name)) == 0) {
			break;
		} else {
			tmp = NULL;
		}
	}

	return tmp;
}

char *cgi_cookie_get_value(struct request *req, const char *name)
{
	cgi_object *olist;
	struct _cookie *cookie;

	olist = htable_lookup(req->headers, (char *) &_cgi_cookie_str);

	if (olist == NULL)
		return NULL;

	cookie = _cgi_cookie_find_by_name(olist->value.u_hash, name);

	if (cookie != NULL)
		return cookie->value;

	return NULL;
}

void cgi_cookie_add(struct request *req,
                          const char *name,
                          const char *value,
                          const char *max_age,
                          const char *path,
                          const char *domain,
                          int secure)
{
	cgi_object *olist;
	struct _cookie *cookie;
	struct _cookie *cookies;

	olist = htable_lookup(req->headers, (char *) &_cgi_cookie_str);

	if (olist == NULL)
		return;

	cookies = (struct _cookie *)olist->value.u_hash;

	cookie = malloc(sizeof(struct _cookie));

	if (cookie == NULL)
		return;

	cookie->name = strdup (name);
	cookie->value = strdup (value);

	if (max_age) {
		cookie->max_age = strdup (max_age);
	} else {
		cookie->max_age = NULL;
	}

	if (path) {
		cookie->path = strdup (path);
	} else {
		cookie->path = NULL;
	}

	if (domain) {
		cookie->domain = strdup (domain);
	} else {
		cookie->domain = NULL;
	}

	cookie->secure = secure;
	cookie->send = 1;

	list_add_tail (&(cookie->list), &(cookies->list));
}
void cgi_cookie_remove(struct request *req, const char *name)
{
	cgi_object *olist;
	struct _cookie *c;

	olist = htable_lookup(req->headers, (char *) &_cgi_cookie_str);

	if (olist == NULL)
		return;

	c = _cgi_cookie_find_by_name(olist->value.u_hash, name);

	free(c->value);
	c->value = strdup("");
	/* make sure this cookie is in the past */
	c->max_age = strdup("1234567890");
	c->send = 1;

	return;
}

static void _cookie_print(struct _cookie *c)
{

	if (c == NULL)
		return;

	if (!c->send)
		return;

	printf("Set-Cookie: %s=%s;", c->name, c->value);

	if (c->max_age != NULL) {
		printf(" Max-age=%s;", c->max_age);
	}

	if (c->path != NULL) {
		printf(" Path=%s;", c->path);
	}

	if (c->domain != NULL) {
		printf(" Domain=%s;", c->domain);
	}

	if (c->secure) {
		printf(" Secure");
	}

	printf("\r\n");
}

void cgi_cookie_print_headers(struct request *req)
{
	cgi_object *olist;
	struct list_head *pos;
	struct _cookie *cookies;
	struct _cookie *cookie = NULL;

	olist = htable_lookup(req->headers, (char *) &_cgi_cookie_str);

	if (olist == NULL)
		return;

	cookies = (struct _cookie *)olist->value.u_hash;

	list_for_each (pos, &cookies->list) {
		cookie = list_entry (pos, struct _cookie, list);
		_cookie_print(cookie);
	}
}
