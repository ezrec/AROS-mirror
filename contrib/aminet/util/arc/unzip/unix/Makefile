# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
guidedir = ${prefix}/guide
htmldir = ${prefix}/html
dvidir = ${prefix}/dvi
psdir = ${prefix}/ps
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

pkgdatadir = $(datadir)/unzip
pkglibdir = $(libdir)/unzip
pkgincludedir = $(includedir)/unzip

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = true
PRE_INSTALL = true
POST_INSTALL = true
NORMAL_UNINSTALL = true
PRE_UNINSTALL = true
POST_UNINSTALL = true
CC = gcc
LN_S = ln -s
MAKEINFO = makeinfo
PACKAGE = unzip
VERSION = 5.12

man_MANS = unzip.1 funzip.1 unzipsfx.1 zipinfo.1

EXTRA_DIST = unix.c Contents Makefile.unix
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
MANS = unzip.1 funzip.1 unzipsfx.1 zipinfo.1

NROFF = nroff
DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST) \
		$(INFOS) $(GUIDES) $(HTMLS) $(MANS)
TAR = tar
GZIP = --best
default: all

.SUFFIXES:
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --geek --include-deps unix/Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status

install-man: $(MANS)
	$(NORMAL_INSTALL)
	$(mkinstalldirs) $(mandir)/man1
	@sect=1;				\
	inst=`echo "unzip" | sed '$(transform)'`.1; \
	if test -f $(srcdir)/unzip.1; then file=$(srcdir)/unzip.1; \
	else file=unzip.1; fi; \
	echo " $(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst"; \
	$(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst
	@sect=1;				\
	inst=`echo "funzip" | sed '$(transform)'`.1; \
	if test -f $(srcdir)/funzip.1; then file=$(srcdir)/funzip.1; \
	else file=funzip.1; fi; \
	echo " $(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst"; \
	$(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst
	@sect=1;				\
	inst=`echo "unzipsfx" | sed '$(transform)'`.1; \
	if test -f $(srcdir)/unzipsfx.1; then file=$(srcdir)/unzipsfx.1; \
	else file=unzipsfx.1; fi; \
	echo " $(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst"; \
	$(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst
	@sect=1;				\
	inst=`echo "zipinfo" | sed '$(transform)'`.1; \
	if test -f $(srcdir)/zipinfo.1; then file=$(srcdir)/zipinfo.1; \
	else file=zipinfo.1; fi; \
	echo " $(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst"; \
	$(INSTALL_DATA) $$file $(mandir)/man$$sect/$$inst

uninstall-man:
	$(NORMAL_UNINSTALL)
	inst=`echo "unzip" | sed '$(transform)'`.1; \
	rm -f $(mandir)/man1/$$inst
	inst=`echo "funzip" | sed '$(transform)'`.1; \
	rm -f $(mandir)/man1/$$inst
	inst=`echo "unzipsfx" | sed '$(transform)'`.1; \
	rm -f $(mandir)/man1/$$inst
	inst=`echo "zipinfo" | sed '$(transform)'`.1; \
	rm -f $(mandir)/man1/$$inst

tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = unix

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  if test -f $$file; then d=.; else d=$(srcdir); fi; \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
info:
guide:
html:
ps:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: 
	@$(NORMAL_INSTALL)

install-data: install-man
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-man

all: Makefile $(MANS)

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(mandir)/man1


mostlyclean-generic:
	test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	rm -f Makefile $(DISTCLEANFILES)
	rm -f config.cache config.log stamp-h stamp-h[0-9]*
	test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean

distclean:  distclean-generic clean
	rm -f config.status

maintainer-clean:  maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: default install-man uninstall-man tags distdir info guide html \
ps dvi installcheck install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
