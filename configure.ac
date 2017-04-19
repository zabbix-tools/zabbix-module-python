AC_INIT([zabbix-module-python], [1.0.0], [ryan@cavaliercoder.com])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability foreign])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

# init libtool
# need > v1.4 to drop 'lib' prefix from linker output
LT_PREREQ([1.4.0]) 
LT_INIT

AC_PROG_CC
AC_PROG_CC_STDC
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# check for stdlib header files
AC_HEADER_STDC

# check for python
AX_PYTHON_DEVEL

# Checking for Zabbix headers
AX_CHECK_ZABBIX
if test ! "x$found_zabbix" = "xyes"; then
  AC_MSG_ERROR([Zabbix headers not found])
fi

dnl
dnl configure zabbix config path
dnl
ZABBIX_CONFIG_PATH="${sysconfdir}/zabbix"
AC_ARG_WITH([zabbix-conf],
  AS_HELP_STRING([--with-zabbix-conf=DIR],
    [use Zabbix configuration path @<:@SYSCONFDIR/zabbix@:>@]
  ),
  [ZABBIX_CONFIG_PATH="$withval"]
)

AC_SUBST([ZABBIX_CONFIG_PATH])

dnl
dnl configure embedded module load path
dnl
ZABBIX_PYTHON_MODULE_PATH="${libdir}/python${PYTHON_VERSION}"
AC_ARG_WITH([python-modules],
  AS_HELP_STRING([--with-python-modules=DIR],
    [use embedded Python module path @<:@LIBDIR/zabbix/modules/python@:>@]
  ),
  [ZABBIX_PYTHON_MODULE_PATH="$withval"]
)

AC_DEFINE_UNQUOTED([ZABBIX_PYTHON_MODULE_PATH],"${ZABBIX_PYTHON_MODULE_PATH}",
  [define Python embedded module path])
AC_SUBST([ZABBIX_PYTHON_MODULE_PATH])

dnl
dnl output
dnl
AC_CONFIG_FILES([
 Makefile
 src/modules/python/Makefile
 python/Makefile
])
AC_OUTPUT

echo "
Configuration:

  Detected OS:           ${host_os}
  Module install path:   ${libdir}

  Python version:        ${PYTHON_VERSION}
  Python package path:   ${PYTHON_SITE_PKG}
  Python module path:    ${ZABBIX_PYTHON_MODULE_PATH}

  Compiler:              ${CC}
  Compiler flags:        ${CFLAGS}

  Library-specific flags:"

if test "x$ZABBIX_CPPFLAGS" != "x"; then
  echo "    zabbix:              ${ZABBIX_CPPFLAGS}"
fi

if test "x$PYTHON_CPPFLAGS" != "x"; then
  echo "    python:              ${PYTHON_CPPFLAGS}"
fi

echo
echo "************************************************************"
echo "*                 Now run '${am_make} install'                   *"
echo "*                                                          *"
echo "*            Thank you for using libzbxpython!             *"
echo "*               <http://cavaliercoder.com>                 *"
echo "************************************************************"
echo
