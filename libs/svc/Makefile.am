## Process this file with automake to produce Makefile.in

ARFLAGS = cr

noinst_LIBRARIES     = libss0svc.a
libss0svc_a_SOURCES  = svc.c event.c control.c
libss0svc_a_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) \
											 -I$(top_srcdir)/include

MAINTAINERCLEANFILES = Makefile.in
