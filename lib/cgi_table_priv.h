/*
 * cgi_table_priv.h
 *
 *  Created on: Jun 15, 2010
 *      Author: pedro
 */

#ifndef CGI_TABLE_PRIV_H_
#define CGI_TABLE_PRIV_H_

struct _cgi_table_header {
	struct list_head list;
	char *header;
};

struct _cgi_table_row {
	struct list_head list;
	htable *data;
};

struct _cgi_table {
	struct _cgi_table_header *headers;
	struct _cgi_table_row *rows;
	int cols;
	struct _cgi_table_row *cur_row;
};

#endif /* CGI_TABLE_PRIV_H_ */
