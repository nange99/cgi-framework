SUBDIRS = \
	util

AM_YFLAGS = -d 

lib_LTLIBRARIES = \
	libcgiservlet.la

libcgiservlet_la_SOURCES = \
	template_parser.y \
	template_scanner.l \
	eval.c \
	eval.h \
	tree.c \
	tree.h \
	node.h \
	template.c \
	template.h \
	cgi_servlet.c     \
	cgi_servlet.h     \
	cgi_request.c \
	cgi_response.c \
	cgi_servlet_private.h

libcgiservlet_la_LIBADD = \
	./util/libutil.la

AM_CFLAGS = \
	-I$(top_srcdir)/lib/util