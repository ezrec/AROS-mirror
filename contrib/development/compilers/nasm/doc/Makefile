# Generated automatically from Makefile.in by configure.
# $Id$
#
# UNIX Makefile for NASM documentation
#

srcdir		= .
prefix		= /usr
exec_prefix	= ${prefix}
bindir		= ${exec_prefix}/bin
mandir		= ${prefix}/man
docdir		= $(prefix)/doc/nasm
infodir		= ${prefix}/info

INSTALL		= /usr/bin/install -c
INSTALL_PROGRAM	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644

PERL		= perl
MAKEINFO	= makeinfo

SRCS		= nasmdoc.src
OUT		= nasm.info

all: $(OUT)

.SUFFIXES: .src .texi .info .ps .rtf .hpj .dvi .ps .txt .pl

# Consider html, txt and src output a side effect
.src.texi:
	mkdir -p html
	$(PERL) ./rdsrc.pl < $<
	mv -f *.html html

nasm.info: nasmdoc.texi
	mkdir -p info
	$(MAKEINFO) $<
	mv -f *.info *.info-* info

clean:
	-rm -f *.rtf *.hpj *.texi *.ph *.gid

spotless: clean
	-rm -rf html info *.hlp *.txt *.ps

install: all
	$(INSTALL_DATA) info/* $(INSTALLROOT)$(infodir)
	mkdir -p $(INSTALLROOT)$(docdir)/html
	$(INSTALL_DATA) html/* $(INSTALLROOT)$(docdir)/html
	$(INSTALL_DATA) nasmdoc.ps nasmdoc.txt $(INSTALLROOT)$(docdir)
