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
	cgi_servlet_private.h

libcgiservlet_la_LIBADD = \
	./util/libutil.la \
    ./json/libjson.la \
	./template/libtemplate.la

libcgiservlet_la_includedir=$(includedir)/libcgiservlet
libcgiservlet_la_include_HEADERS = cgi_servlet.h \
									cgi_object.h

AM_CFLAGS = \
	-I$(top_srcdir)/lib/util \
	-I$(top_srcdir)/lib/template
