## Process this file with automake to produce Makefile.in

sbin_PROGRAMS = ss0ssd

ss0ssd_SOURCES = storage_svc.c
ss0ssd_LDADD   = ../../libs/svc/libss0svc.a
ss0ssd_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) \
									-I$(top_srcdir)/include

MAINTAINERCLEANFILES = Makefile.in
