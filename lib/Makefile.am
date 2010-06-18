SUBDIRS = \
	util \
    json \
	template 

lib_LTLIBRARIES = \
	libcgiservlet.la

libcgiservlet_la_SOURCES = \
	cgi_servlet.c     \
	cgi_servlet.h     \
	cgi_request.c \
	cgi_response.c \
	cgi_session.c \
	cgi_session.h \
	cgi_cookie.c \
	cgi_cookie.h \
	cgi_object.c \
	cgi_log.c \
	cgi_list.c \
	cgi_table.c \
	cgi_servlet_priv.h

libcgiservlet_la_LIBADD = \
	./util/libutil.la \
    ./json/libjson.la \
	./template/libtemplate.la

libcgiservlet_la_includedir=$(includedir)/libcgiservlet
libcgiservlet_utils_includedir=$(includedir)/libcgiservlet/util
libcgiservlet_la_include_HEADERS = cgi_servlet.h \
			 	   cgi_object.h \
			 	   cgi_session.h \
			 	   cgi_list.h \
			 	   cgi_table.h \
			 	   cgi_cookie.h

libcgiservlet_utils_include_HEADERS = util/sha1.h \
                                      util/list.h

AM_CFLAGS = \
	-I$(top_srcdir)/lib/util \
	-I$(top_srcdir)/lib/template
