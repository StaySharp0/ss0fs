## Process this file with automake to produce Makefile.in

sbin_PROGRAMS = ss0msd

ss0msd_SOURCES = master_svc.c
ss0msd_LDADD   = ../../libs/svc/libss0svc.a \
								 ../../libs/common/libss0common.a
ss0msd_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) \
									-I$(top_srcdir)/include

MAINTAINERCLEANFILES = Makefile.in
