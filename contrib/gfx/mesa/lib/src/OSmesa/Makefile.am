## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

if HAVE_OSMESA
lib_LTLIBRARIES = libMesaOS.la
endif

libMesaOS_la_SOURCES = osmesa.c
libMesaOS_la_LDFLAGS = -version-info @LIBMESA_VERSION_INFO@


include $(top_srcdir)/common_rules.make
