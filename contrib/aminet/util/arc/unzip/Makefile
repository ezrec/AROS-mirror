# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.2 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.


SHELL = /bin/sh

srcdir = .
top_srcdir = .
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

top_builddir = .

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

AUTOMAKE_OPTIONS = no-dependencies 1.1g

SUBDIRS = amiga atari human68k mac msdos nt os2 ram tops20 unix vms

EXTRA_DIST = BUGS ChangeLog.amiga History.512 ToDo Where ZipPorts \
	Contents Makefile.gccamiga README.amiga

bin_PROGRAMS = unzip funzip unzipsfx

gccAmigaOS_special = -DUNIX -DBSD -DTERMIOS -UAMIGA -DCHECK_EOF

DEFS = -DHAVE_CONFIG_H $(gccAmigaOS_special)
INCLUDES = -I$(srcdir) -I.

unzip_SOURCES = unzip.c crypt.c envargs.c explode.c extract.c \
	file_io.c inflate.c match.c unreduce.c unshrink.c zipinfo.c unix.c

funzip_SOURCES = # funzip.c crypt_.c inflate_.c
funzip_DEPENDENCIES = funzip.o crypt_.o inflate_.o
funzip_LDADD = funzip.o crypt_.o inflate_.o

unzipsfx_SOURCES = # unzipsfx.c crypt.c extract_.c file_io.c inflate.c match.c unix_.c
unzipsfx_DEPENDENCIES = unzipsfx.o crypt.o extract_.o file_io.o inflate.o match.o unix_.o
unzipsfx_LDADD = unzipsfx.o crypt.o extract_.o file_io.o inflate.o match.o unix_.o
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES = 
PROGRAMS =  $(bin_PROGRAMS)

CPPFLAGS = 
LDFLAGS = 
LIBS = -lbsd 
unzip_OBJECTS =  unzip.o crypt.o envargs.o explode.o extract.o file_io.o \
inflate.o match.o unreduce.o unshrink.o zipinfo.o unix.o
unzip_LDADD = $(LDADD)
unzip_LDFLAGS = 
funzip_OBJECTS = 
funzip_LDFLAGS = 
unzipsfx_OBJECTS = 
unzipsfx_LDFLAGS = 
CFLAGS = -g -O2
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  README COPYING ChangeLog INSTALL Makefile.am Makefile.in \
TODO acconfig.h aclocal.m4 config.h.in configure configure.in \
install-sh missing mkinstalldirs stamp-h.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST) \
		$(INFOS) $(GUIDES) $(HTMLS) $(MANS)
TAR = tar
GZIP = --best
SOURCES = $(unzip_SOURCES) $(funzip_SOURCES) $(unzipsfx_SOURCES)
OBJECTS = $(unzip_OBJECTS) $(funzip_OBJECTS) $(unzipsfx_OBJECTS)

default: all

.SUFFIXES:
.SUFFIXES: .c .o
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --geek --include-deps Makefile

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

$(ACLOCAL_M4):  configure.in 
	cd $(srcdir) && $(ACLOCAL)

config.status: $(srcdir)/configure
	$(SHELL) ./config.status --recheck
$(srcdir)/configure: $(srcdir)/configure.in $(ACLOCAL_M4) $(CONFIGURE_DEPENDENCIES)
	cd $(srcdir) && $(AUTOCONF)

config.h: stamp-h
stamp-h: $(srcdir)/config.h.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES= CONFIG_HEADERS=config.h \
	     $(SHELL) ./config.status
	@echo timestamp > stamp-h
$(srcdir)/config.h.in: $(srcdir)/stamp-h.in
$(srcdir)/stamp-h.in: $(top_srcdir)/configure.in $(ACLOCAL_M4) acconfig.h
	cd $(top_srcdir) && $(AUTOHEADER)
	@echo timestamp > $(srcdir)/stamp-h.in

mostlyclean-hdr:

clean-hdr:

distclean-hdr:
	rm -f config.h

maintainer-clean-hdr:

mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "  $(INSTALL_PROGRAM) $$p $(bindir)/`echo $$p|sed '$(transform)'`"; \
	     $(INSTALL_PROGRAM) $$p $(bindir)/`echo $$p|sed '$(transform)'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(bindir)/`echo $$p|sed '$(transform)'`; \
	done

.c.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	rm -f *.o core

clean-compile:

distclean-compile:
	rm -f *.tab.c

maintainer-clean-compile:

unzip: $(unzip_OBJECTS) $(unzip_DEPENDENCIES)
	@rm -f unzip
	$(LINK) $(unzip_LDFLAGS) $(unzip_OBJECTS) $(unzip_LDADD) $(LIBS)

funzip: $(funzip_OBJECTS) $(funzip_DEPENDENCIES)
	@rm -f funzip
	$(LINK) $(funzip_LDFLAGS) $(funzip_OBJECTS) $(funzip_LDADD) $(LIBS)

unzipsfx: $(unzipsfx_OBJECTS) $(unzipsfx_DEPENDENCIES)
	@rm -f unzipsfx
	$(LINK) $(unzipsfx_LDFLAGS) $(unzipsfx_OBJECTS) $(unzipsfx_LDADD) $(LIBS)

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.



all-recursive install-data-recursive install-exec-recursive \
installdirs-recursive install-recursive uninstall-recursive  \
check-recursive installcheck-recursive info-recursive dvi-recursive \
guide-recursive ps-recursive html-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	for subdir in $(SUBDIRS); do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	rev=''; for subdir in $(SUBDIRS); do rev="$$rev $$subdir"; done; \
	for subdir in $$rev; do \
	  target=`echo $@ | sed s/-recursive//`; \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) tags); \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES)
	here=`pwd` && cd $(srcdir) && mkid -f$$here/ID $(SOURCES) $(HEADERS)

TAGS: tags-recursive $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES)
	tags=; \
	here=`pwd`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test -f $$subdir/TAGS && tags="$$tags -i $$here/$$subdir/TAGS"; \
	done; \
	test -z "$(ETAGS_ARGS)config.h.in$(SOURCES)$(HEADERS)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags config.h.in $(SOURCES) $(HEADERS) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	rm -f TAGS ID

maintainer-clean-tags:

distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	rm -rf $(distdir)
	GZIP=$(GZIP) $(TAR) zxf $(distdir).tar.gz
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	dc_install_base=`cd $(distdir)/=inst && pwd`; \
	cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix=$$dc_install_base \
	  && $(MAKE) \
	  && $(MAKE) dvi \
	  && $(MAKE) check \
	  && $(MAKE) install \
	  && $(MAKE) installcheck \
	  && $(MAKE) dist
	rm -rf $(distdir)
	@echo "========================"; \
	echo "$(distdir).tar.gz is ready for distribution"; \
	echo "========================"
dist: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP) $(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -rf $(distdir)
dist-all: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP) $(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -rf $(distdir)
distdir: $(DISTFILES)
	rm -rf $(distdir)
	mkdir $(distdir)
	-chmod 777 $(distdir)
	@for file in $(DISTFILES); do \
	  if test -f $$file; then d=.; else d=$(srcdir); fi; \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done
	for subdir in $(SUBDIRS); do		\
	  test -d $(distdir)/$$subdir		\
	  || mkdir $(distdir)/$$subdir		\
	  || exit 1;				\
	  chmod 777 $(distdir)/$$subdir;	\
	  (cd $$subdir && $(MAKE) top_distdir=../$(top_distdir) distdir=../$(distdir)/$$subdir distdir) \
	    || exit 1; \
	done
info: info-recursive
guide: guide-recursive
html: html-recursive
ps: ps-recursive
dvi: dvi-recursive
check: all-am
	$(MAKE) check-recursive
installcheck: installcheck-recursive
all-recursive-am: config.h
	$(MAKE) all-recursive

all-am: Makefile $(PROGRAMS) config.h

install-exec-am: install-binPROGRAMS

uninstall-am: uninstall-binPROGRAMS

install-exec: install-exec-recursive install-exec-am
	@$(NORMAL_INSTALL)

install-data: install-data-recursive
	@$(NORMAL_INSTALL)

install: install-recursive install-exec-am
	@:

uninstall: uninstall-recursive uninstall-am

all: all-recursive-am all-am

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs: installdirs-recursive
	$(mkinstalldirs)  $(bindir)


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
mostlyclean-am:  mostlyclean-hdr mostlyclean-binPROGRAMS \
		mostlyclean-compile mostlyclean-tags \
		mostlyclean-generic

clean-am:  clean-hdr clean-binPROGRAMS clean-compile clean-tags \
		clean-generic mostlyclean-am

distclean-am:  distclean-hdr distclean-binPROGRAMS distclean-compile \
		distclean-tags distclean-generic clean-am

maintainer-clean-am:  maintainer-clean-hdr maintainer-clean-binPROGRAMS \
		maintainer-clean-compile maintainer-clean-tags \
		maintainer-clean-generic distclean-am

mostlyclean:  mostlyclean-recursive mostlyclean-am

clean:  clean-recursive clean-am

distclean:  distclean-recursive distclean-am
	rm -f config.status

maintainer-clean:  maintainer-clean-recursive maintainer-clean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f config.status

.PHONY: default mostlyclean-hdr distclean-hdr clean-hdr \
maintainer-clean-hdr mostlyclean-binPROGRAMS distclean-binPROGRAMS \
clean-binPROGRAMS maintainer-clean-binPROGRAMS uninstall-binPROGRAMS \
install-binPROGRAMS mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile install-data-recursive \
uninstall-data-recursive install-exec-recursive \
uninstall-exec-recursive installdirs-recursive uninstalldirs-recursive \
all-recursive check-recursive installcheck-recursive info-recursive \
guide-recursive html-recursive ps-recursive dvi-recursive \
mostlyclean-recursive distclean-recursive clean-recursive \
maintainer-clean-recursive tags tags-recursive mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info guide html \
ps dvi installcheck all-recursive-am all-am install-exec-am \
uninstall-am install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean

	file_io.o inflate.o match.o unreduce.o unshrink.o zipinfo.o unix.o
	file_io.o inflate.o match.o unreduce.o unshrink.o zipinfo.o unix.o

unzip.o : $(srcdir)/unzip.c $(srcdir)/unzip.h $(srcdir)/crypt.h $(srcdir)/version.h config.h

crypt.o : $(srcdir)/crypt.c $(srcdir)/unzip.h $(srcdir)/zip.h $(srcdir)/crypt.h config.h

envargs.o : $(srcdir)/envargs.c $(srcdir)/unzip.h config.h

explode.o : $(srcdir)/explode.c $(srcdir)/unzip.h config.h

extract.o : $(srcdir)/extract.c $(srcdir)/unzip.h $(srcdir)/crypt.h config.h

file_io.o : $(srcdir)/file_io.c $(srcdir)/unzip.h $(srcdir)/crypt.h $(srcdir)/tables.h config.h

inflate.o : $(srcdir)/inflate.c $(srcdir)/inflate.h $(srcdir)/unzip.h config.h

match.o : $(srcdir)/match.c $(srcdir)/unzip.h config.h

unreduce.o : $(srcdir)/unreduce.c $(srcdir)/unzip.h config.h

unshrink.o : $(srcdir)/unshrink.c $(srcdir)/unzip.h config.h

zipinfo.o : $(srcdir)/zipinfo.c $(srcdir)/unzip.h config.h

unix.o : $(srcdir)/unix/unix.c $(srcdir)/unzip.h config.h
	$(COMPILE) -c $(srcdir)/unix/unix.c -o unix.o

crypt_.o : $(srcdir)/crypt.c $(srcdir)/unzip.h $(srcdir)/zip.h $(srcdir)/crypt.h config.h
	$(COMPILE) -DFUNZIP -c $(srcdir)/crypt.c -o crypt_.o

inflate_.o : $(srcdir)/inflate.c $(srcdir)/inflate.h $(srcdir)/unzip.h $(srcdir)/crypt.h config.h
	$(COMPILE) -DFUNZIP -c $(srcdir)/inflate.c -o inflate_.o

unzipsfx.o : $(srcdir)/unzip.c $(srcdir)/unzip.h $(srcdir)/crypt.h $(srcdir)/version.h config.h
	$(COMPILE) -DSFX -c $(srcdir)/unzip.c -o unzipsfx.o

extract_.o : $(srcdir)/extract.c $(srcdir)/unzip.h $(srcdir)/crypt.h config.h
	$(COMPILE) -DSFX -c $(srcdir)/extract.c -o extract_.o

unix_.o : $(srcdir)/unix/unix.c $(srcdir)/unzip.h config.h
	$(COMPILE) -DSFX -c $(srcdir)/unix/unix.c -o unix_.o

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
