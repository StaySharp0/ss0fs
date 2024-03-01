## Process this file with automake to produce Makefile.in

sbin_PROGRAMS = ss0adm

ss0adm_SOURCES = admin_clnt.c transport.c
ss0adm_LDADD   = ../libs/common/libss0common.a
ss0adm_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) \
									-I$(top_srcdir)/include

MAINTAINERCLEANFILES = Makefile.in
