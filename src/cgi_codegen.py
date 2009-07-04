import os

class cgilib:
	def __init__ (self):
		self.handlers = []
	def set_project_name (self, name):
		self.name = name
	def add_handler (self, url, name, html):
		tmp = {'url': url, 'name': name, 'html': html}
		self.handlers.append (tmp)
	def create_html (self, name, html):
		htmlContent = """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>"""+ name +"""</title>
</head>
<body>
</body>
</html>	
"""
		f = open (self.name + "/html/" + html, "wb")
		f.write (htmlContent)
		f.close ()
	def create_mapping (self, url, name):
		return "{\""+url+"\", handle_"+name+"}"
	def create_method_prototype (self, name):
		return "int handle_" + name + " (struct request *req, struct response *resp);"
	def create_method (self, name, html):
		return "int handle_" + name + """ (struct request *req, struct response *resp)
{
	cgi_response_set_html (resp, "html/"""+html+"""");
	return 1;
}

"""
	def define_includes (self):
		return """#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgi_servlet.h>
#include """+ "\"" + self.name +""".h"
"""
	def create_main (self):
		mapping = ',\n\t\t'.join([self.create_mapping(handler['url'], handler['name']) for handler in self.handlers])
		main = """int main (int argc, char *argv[])
{

	struct url_mapping map[] = { 
		""" + mapping + """
	};

	struct config conf = {
		NULL
		/*default_error_html = "error.html";*/
	};

	cgi_servlet_init (&conf, &map, """+ str(len (self.handlers)) +""", NULL);

	return 0;
}

"""
		methods = ''.join ([self.create_method (handler['name'], handler['html']) for handler in self.handlers])
		mainContent = ''.join ([self.define_includes(), "\n", main, methods])
		
		f = open (self.name + "/" + self.name + ".c", "wb")
		f.write (mainContent)
		f.close()
	def create_file_structure (self):
		if os.path.isdir (self.name) <> True: 
			os.mkdir (self.name)
		
		if os.path.isdir (self.name + "/html") <> True:
			os.mkdir (self.name + "/html")
	def create_include (self):
		prototypes = '\n'.join ([self.create_method_prototype (handler['name']) for handler in self.handlers])
		include_name = "_" + self.name.upper () + "_H"
		includeContent = """#ifndef """+include_name+"""
#define """+include_name+"""

""" + prototypes + """
#endif
"""
		f = open (self.name + "/" + self.name + ".h", "wb")
		f.write (includeContent)
		f.close()
	def create_makefile (self):
		makefileAm = """
bin_PROGRAMS = """+ self.name +"""

"""+ self.name +"""_SOURCES = \\
	""" + self.name + """.c \\
	""" + self.name + """.h

INCLUDES = $(DEPS_CFLAGS)
LIBS = $(DEPS_LIBS)
"""
		configAc = """#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT('"""+self.name+"""', 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(DEPS, libcgiservlet >= 0.1)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
"""
		autogen = """
#!/bin/sh
echo "Running aclocal ..."
aclocal
echo "Running autoheader ..."
autoheader
echo "Running automake --gnu ..."
automake --add-missing --foreign
echo "Running autoconf ..."
autoconf
"""
		f = open (self.name + "/Makefile.am", "wb")
		f.write (makefileAm)
		f.close()
		f = open (self.name + "/configure.ac", "wb")
		f.write (configAc)
		f.close()
		f = open (self.name + "/autogen.sh", "wb")
		f.write (autogen)
		f.close()
		os.chmod (self.name + "/autogen.sh", 0755)
	def generate (self):
		self.create_file_structure()
		self.create_include ()
		self.create_main ()
		self.create_makefile ()
		for handler in self.handlers:
			self.create_html (handler['name'], handler['html'])

c = cgilib()
c.set_project_name ("test1")
c.add_handler ("/do.list", "list_files", "list.html")
c.add_handler ("/do.config", "configure", "config.html")
c.generate()