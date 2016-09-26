AC_INIT([libzbxpython], [1.0.0], [ryan@cavaliercoder.com])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability foreign])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR
AC_PROG_LIBTOOL # seem to need this on CentOS 5 (libtool v1.5)
LT_INIT
AC_PROG_CC
AC_PROG_CC_STDC
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# check for stdlib header files
AC_HEADER_STDC

# with argument for python module load path
ZABBIX_MODULE_PATH="/var/lib/zabbix/modules/python"
AC_ARG_WITH([module-path],
  AS_HELP_STRING([--with-module-path=@<:@ARG@:>@],
    [use Zabbix module path @<:@default=$ZABBIX_MODULE_PATH@:>@, optionally specify path to Zabbix agent modules]
  ),
  [ZABBIX_MODULE_PATH="$withval"]
)

AC_DEFINE_UNQUOTED([ZABBIX_MODULE_PATH], ["$ZABBIX_MODULE_PATH"],
  [Define Zabbix agent module load path])

# check for pythong
AX_PYTHON_DEVEL

# Checking for Zabbix headers
AX_LIB_ZABBIX
if test ! "x$found_zabbix" = "xyes"; then
  AC_MSG_ERROR([Zabbix headers not found])
fi

# output
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
