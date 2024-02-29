#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([linx ss0fs], [0.0.0], [staysharp0@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIRS([aclocal])

AM_INIT_AUTOMAKE
AR_FLAGS="cr"
AC_SUBST(AR_FLAGS)

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T

# Checks for library functions.
AC_CHECK_FUNCS([mkdir socket strrchr])

AC_CONFIG_FILES([Makefile
                 admin/Makefile
                 include/Makefile
                 libs/Makefile
                 libs/common/Makefile
                 libs/svc/Makefile
                 master/Makefile
                 storage/Makefile])
AC_OUTPUT
