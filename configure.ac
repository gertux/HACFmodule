#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([hacf_apache_module], [0.9.1], [hacf@hobbiton.be])
AC_CONFIG_AUX_DIR([autotools/aux])
AC_CONFIG_MACRO_DIR([autotools/m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_CONFIG_SRCDIR([src/hacf.c])
AC_CONFIG_HEADER([config.h])

# Argument checks
AC_ARG_WITH([apache], 
  [AS_HELP_STRING([--with-apache=PATH],[Absolute path to the Apache server installation])],
  [CPPFLAGS="-I${with_apache}/include ${CPPFLAGS}"
   LDFLAGS="-L${with_apache}/lib ${LDFLAGS}"
   APXS="${with_apache}/bin/apxs"
  ]
)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Apache Checks
AC_CHECK_HEADER([httpd.h], [], [AC_MSG_ERROR([httpd.h not found, use --with-apache=PATH to point to the apache server installation])])

# APR Checks
AC_CHECK_LIB([apr-0], [apr_palloc], [], [AC_MSG_ERROR([apr_palloc not found in libapr-0])])
AC_CHECK_HEADER([apr_pools.h], [], [AC_MSG_ERROR([apr_pools.h not found, use --with-apache=PATH to point to the apache server installation])])
AC_CHECK_PROGS([APXS],[apxs],[AC_MSG_ERROR([apxs not found in ${PATH}])])

# Checks for header files.
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
