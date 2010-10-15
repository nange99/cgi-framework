/*
 * cgi_upload.h
 *
 *  Created on: Jun 17, 2010
 *      Author: Pedro Kiefer (pkiefer@pd3.com.br)
 */

#ifndef CGI_UPLOAD_H_
#define CGI_UPLOAD_H_

#define MULTIPART_CONTENT_TYPE "multipart/form-data"
#define UPLOAD_TMP_DIR "/tmp"
#define CGI_MAX_POST_SIZE (16 * 1024 * 1024)	/* 16MB */

enum upload_err {
	UPLOAD_ERROR_OK,
	UPLOAD_ERROR_A,
	UPLOAD_ERROR_B,
	UPLOAD_ERROR_C,
	UPLOAD_ERROR_D,
	UPLOAD_ERROR_E,
	UPLOAD_ERROR_F
};

typedef struct _cgi_file {
	enum upload_err error;
	char *tmp_filename;
	char *filename;
	char *type;
	long size;
} cgi_file;

void cgi_upload_file_destroy(cgi_file *f);

cgi_file *cgi_upload_get_file(struct request *req, const char *key);

char *cgi_upload_get_error_str(enum upload_err err);

void rfc1867_post_handler(struct request *req,
                          int content_length,
                          const char *content_type);

#endif /* CGI_UPLOAD_H_ */
