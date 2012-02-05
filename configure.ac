#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([hacf_apache_module], [0.9.2], [hacf@hobbiton.be])
AC_CONFIG_AUX_DIR([autotools/aux])
AC_CONFIG_MACRO_DIR([autotools/m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_CONFIG_SRCDIR([src/hacf_apache_module.c])
AC_CONFIG_HEADER([config.h])

# Argument checks
AC_ARG_WITH([apxs], 
  [AS_HELP_STRING([--with-apxs=PATH],[Absolute path to the APache eXtenSion tool apxs])],
  [AC_CHECK_FILE([${with_apxs}],[APXS="${with_apxs}"],[AC_MSG_ERROR([${with_apxs} not found])] )]
)

# Checks for programs.
AC_PROG_CC
AC_CHECK_PROGS([APXS],[apxs],[no])
if test "x${APXS}" = xno; then
  AC_MSG_ERROR([apxs not found])
fi
CPPFLAGS="-I`${APXS} -q INCLUDEDIR` -I`${APXS} -q APR_INCLUDEDIR`"
apache_exp_libdir=`${APXS} -q exp_libdir`
AC_CHECK_FILE([${apache_exp_libdir}],[LDFLAGS="-L${apache_exp_libdir}"],[AC_MSG_WARN([${apache_exp_libdir} not found])] )

# Checks for libraries.
AC_SEARCH_LIBS([apr_palloc], [apr-0 apr-1], [], [AC_MSG_ERROR([apr_palloc not found])])

# Checks for header files.
AC_CHECK_HEADER([apr_pools.h], [], [AC_MSG_ERROR([apr_pools.h not found, use --with-apache=PATH to point to the apache server installation])])
AC_CHECK_HEADER([httpd.h], [], [AC_MSG_ERROR([httpd.h not found, use --with-apxs=PATH to point to apxs of the Apache server you want to use])])
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit memset strcasecmp strdup])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT
