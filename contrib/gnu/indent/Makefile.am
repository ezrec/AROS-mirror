## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = no-texinfo.tex

bin_PROGRAMS = indent
indent_SOURCES = args.c comments.c indent.c lexi.c parse.c backup.c globs.c io.c
noinst_HEADERS = backup.h indent.h sys.h lexi.h args.h comments.h globs.h io.h parse.h

info_TEXINFOS = indent.texinfo
man_MANS = indent.1

BUILT_SOURCES = indent.1 README README.vc++ ChangeLog gperf.c gperf-cc.c indent.html config.h.vc++ config.h.vms
EXTRA_DIST = $(BUILT_SOURCES) indent.gperf indent-cc.gperf ChangeLog-1990 ChangeLog-1998 indent.dsp README.VMS make-decc.com \
             aclocal/UTIMBUF.m4 gnuc-make.com indent.mak Makefile.mingw32 vaxc-make.com wildexp.c

lexi.o: $(srcdir)/lexi.c $(srcdir)/sys.h config.h $(srcdir)/indent.h @MAINTAINER_MODE_TRUE@ gperf.c gperf-cc.c
	$(COMPILE) -c $(srcdir)/lexi.c

gperf.c: @MAINTAINER_MODE_TRUE@ indent.gperf
	gperf -D -c -l -p -t -T -g -j1 -o -K rwd -N is_reserved indent.gperf > gperf.c

gperf-cc.c: @MAINTAINER_MODE_TRUE@ indent-cc.gperf
	gperf -D -c -l -p -t -T -g -j1 -o -K rwd -N is_reserved_cc -H hash_cc indent-cc.gperf > gperf-cc.c

DISTCLEANFILES=config.cache config.log config.h

# --------------- Maintainer's Section

MAINTAINERCLEANFILES=aclocal.m4 Makefile.in config.h.in stamp-h.in configure \
	ChangeLog texinfo2man indent.1 gperf.c gperf-cc.c \
	indent-1.spec index.html cvslog-*.html indent.html
ACLOCAL=aclocal -I aclocal
aclocal.m4: $(shell ls aclocal/*.m4)

## Use -g when in maintainer mode
if MAINTAINER_MODE
CFLAGS=-g
else
CFLAGS=@CFLAGS@
endif

## Bug in automake: Can't use `if MAINTAINER_MODE'
@MAINTAINER_MODE_TRUE@include $(srcdir)/maintMakefile

if MAINTAINER_MODE
README.vc++: Makefile.am README.vc++.in config.status
	sed -e 's%!VERSION!%'`grep 's%@VERSION' config.status | cut -d% -f3`'%g' README.vc++.in > README.vc++

config.h.vc++: Makefile.am config.h.vc++.in config.status
	sed -e 's%!VERSION!%'`grep 's%@VERSION' config.status | cut -d% -f3`'%g' config.h.vc++.in > config.h.vc++

config.h.vms: Makefile.am config.h.vms.in config.status
	sed -e 's%!VERSION!%'`grep 's%@VERSION' config.status | cut -d% -f3`'%g' config.h.vms.in > config.h.vms
endif

## Can't put this in the maintMakefile for some reason.
maintainer-clean-local:
	echo "all:" > Makefile
	echo -e "\t$(ACLOCAL)" >> Makefile
	echo -e "\tautoheader" >> Makefile
	echo -e "\tautoconf" >> Makefile
	echo -e "\tautomake --foreign" >> Makefile
	echo -e "\tconfigure --enable-maintainer-mode --prefix=/home/carlo" >> Makefile
