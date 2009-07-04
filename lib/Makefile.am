SUBDIRS = \
	util \
	template

lib_LTLIBRARIES = \
	libcgiservlet.la

libcgiservlet_la_SOURCES = \
	cgi_servlet.c     \
	cgi_servlet.h     \
	cgi_request.c \
	cgi_response.c \
	cgi_list.c \
	cgi_servlet_private.h

libcgiservlet_la_LIBADD = \
	./util/libutil.la \
	./template/libtemplate.la

libcgiservlet_la_includedir=$(includedir)/libcgiservlet
libcgiservlet_la_include_HEADERS = cgi_servlet.h

AM_CFLAGS = \
	-I$(top_srcdir)/lib/util \
	-I$(top_srcdir)/lib/template