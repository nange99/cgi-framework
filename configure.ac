dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([cgi_servlet],[0.1])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_ENABLE(gcc-warnings,
[  --enable-gcc-warnings   turn on lots of GCC warnings (not recommended)],
[case "${enableval}" in
   yes|no) ;;
   *)      AC_MSG_ERROR([bad value ${enableval} for gcc-warnings option]) ;;
 esac],
	      [enableval=no])
if test "${enableval}" = yes; then

MAYBE_WARN="-Wall -Wextra -std=c99 -pedantic \
-Wsign-compare -Werror-implicit-function-declaration \
-Wpointer-arith -Wwrite-strings -Wstrict-prototypes \
-Wmissing-prototypes -Wmissing-declarations -Wnested-externs \
-Wpacked -Wswitch-enum -Wmissing-format-attribute \
-Wstrict-aliasing=2 -Winit-self  \
-Wdeclaration-after-statement -Wold-style-definition \
-Wno-missing-field-initializers -Wno-unused-parameter \
 -Wno-long-long -Winline"

#MAYBE_WARN="$MAYBE_WARN -Wunsafe-loop-optimizations -Wno-attributes"
MAYBE_WARN="$MAYBE_WARN -fno-strict-aliasing -fno-common"

MAYBE_WARN="$MAYBE_WARN -Wp,-D_FORTIFY_SOURCE=2"

CFLAGS="$CFLAGS $MAYBE_WARN"
  AC_DEFINE([lint], 1, [Define to 1 if the compiler is checking for lint.])
fi

AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_CC
AC_PROG_CC
AC_DIAGNOSE([obsolete],[AM_PROG_CC_STDC:
	your code should no longer depend upon `am_cv_prog_cc_stdc', but upon
	`ac_cv_prog_cc_stdc'.  Remove this warning and the assignment when
	you adjust the code.  You can also remove the above call to
	AC_PROG_CC if you already called it elsewhere.])
am_cv_prog_cc_stdc=$ac_cv_prog_cc_stdc

AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_HEADER_STDC

YFLAGS="-d"

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h stddef.h stdarg.h stdlib.h string.h [sys/param.h] syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_VPRINTF
AC_FUNC_MEMCMP
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strchr strdup strerror tzset strndup strerror vsnprintf vasprintf open vsyslog strncasecmp])

LT_INIT

SHAVE_INIT([m4], [enable]) # dnl Make the output pretty

AC_CONFIG_FILES([m4/shave
		 m4/shave-libtool
		 Makefile
		 libcgiservlet.pc
		 lib/Makefile
         lib/util/Makefile
         lib/json/Makefile
		 lib/template/Makefile
         test/Makefile])
AC_OUTPUT

