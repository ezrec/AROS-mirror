# Top-level makefile for BREXX V2
#
# BREXX interpreter
# Version:  2.0
# Copyright (C) 1991-1999  Vassilis Vlachoudis (V.Vlachoudis@cern.ch)
#
# This interpreter is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#
# To add a new configuration for your system add it to the list below
# then update the make_cnf file.

RXVERSION = brexx203

TARGETS =	aix aix_debug \
		alpha alpha_debug \
		amiga \
		dos32 \
		gcc gcc_debug \
		hpux hpux_debug \
		hpux-gcc hpux-gcc_debug \
		irix irix_debug \
		linux linux_debug \
		linux_noalign linux_noalign_debug \
		test test_debug

default:
	@echo "BREXX V2 makefile"
	@echo "Type one of the following:"
	@echo "  make aix                  for IBM RS/6000 with AIX"
	@echo "  make alpha                for Digital's Alpha"
	@echo "  make amiga                for an AmigaOS application"
	@echo "  make bc                   for a Borland C++, DOS application"
	@echo "  make bc86                 for a Borland C++, 8086 code"
	@echo "  make dos32                for a 32-bit DOS application"
	@echo "  make gcc                  for a generic system with GCC"
	@echo "  make hpux                 for HP systems with HPUX *"
	@echo "  make hpux-gcc             for HP systems with HPUX using GCC"
	@echo "  make irix                 for SGI systems with IRIX"
	@echo "  make linux                for Linux systems with GCC *"
	@echo "  make linux_noalign        for Linux systems with GCC,"
	@echo "  make macintosh            for a Mac application"
	@echo "  make mswindows            for Microsoft Windows"
	@echo "  make test                 for testing/experimenting"
	@echo "  make tar                  to prepare a .tar.gz file"
	@echo "  make clean"
	@echo "(*) USE THIS ONE!"
	@echo "For a DEBUG version type one of the above with suffix _debug"
	@echo "i.e.:  make linux_debug"

$(TARGETS):
	cd lstring ; $(MAKE) $@
	cd src ; $(MAKE) $@

bc:
	cd lstring
	make -f makefile.bc
	cd ../src
	make -f makefile.bc
	cd ..

bc_debug:
	cd lstring
	make -f makefile.bc -D__DEBUG__
	cd ../src
	make -f makefile.bc -D__DEBUG__
	cd ..

bc86:
	cd lstring
	make -f makefile.bc -D__8086__
	cd ../src
	make -f makefile.bc -D__8086__
	cd ..

macintosh:
	@echo "See the README file for Macintosh intallation information"

mswindows:
	@echo "See the windows/README.WIN for installation information"

# Remove .o files, instr.cnt and profile out files
clean:
	rm -f lib/*.a
	cd lstring ; $(MAKE) $@
	cd src ; $(MAKE) $@
	rm bin/rexx

tar:
	touch $(RXVERSION).tgz
	rm $(RXVERSION).tgz
	cd .. ; \
	tar -cvf $(RXVERSION).tar $(RXVERSION) ; \
	gzip $(RXVERSION).tar ; \
	mv $(RXVERSION).tar.gz brexx/$(RXVERSION).tgz
