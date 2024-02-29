#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([linx ss0fs], [0.0.0], [staysharp0@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AR_FLAGS="cr"
AC_SUBST(AR_FLAGS)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T

# Checks for library functions.
AC_CHECK_FUNCS([mkdir socket strrchr])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 libs/Makefile
                 libs/svc/Makefile
                 src/Makefile
                 src/admin/Makefile
                 src/master/Makefile
                 src/storage/Makefile])
AC_OUTPUT
