# Top-level makefile for Mesa

# Mesa 3-D graphics library
# Version:  3.0
# Copyright (C) 1995-1998  Brian Paul
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


# $Id$

# $Log$
# Revision 1.1  2005/01/11 14:58:29  NicJA
# AROSMesa 3.0
#
# - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
# - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
# - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
#
# Revision 3.24  1998/09/01 03:14:15  brianp
# added *.bat to tar file list
#
# Revision 3.23  1998/08/05 01:27:47  brianp
# updated OS/2 config (Alexander Mai)
#
# Revision 3.22  1998/08/02 15:51:00  brianp
# added CONFORM file
#
# Revision 3.21  1998/08/01 03:58:11  brianp
# added hpux10-gcc-sl and irix6-gcc-n32-sl configs (Albert De Knuydt)
#
# Revision 3.20  1998/07/30 23:51:47  brianp
# updated BeOS configs (Ed Silva)
#
# Revision 3.19  1998/07/26 17:28:55  brianp
# small change to tar file list
#
# Revision 3.18  1998/07/26 01:18:07  brianp
# added irix5-gcc config
#
# Revision 3.17  1998/07/26 01:15:04  brianp
# moved glut.h to demos tar file instead of lib tar file
#
# Revision 3.16  1998/07/08 02:03:16  brianp
# added linux-386-pthread-shared config
#
# Revision 3.15  1998/07/08 01:38:27  brianp
# added linux-ggi config
#
# Revision 3.14  1998/06/22 01:55:44  brianp
# removed obsolete mswindows stuff
#
# Revision 3.13  1998/06/22 01:53:58  brianp
# added linux-386-glide-mits config
#
# Revision 3.12  1998/06/19 03:18:39  brianp
# minor clean-up
#
# Revision 3.11  1998/06/13 16:03:00  brianp
# clean-up of tar file list
#
# Revision 3.10  1998/06/01 00:56:00  brianp
# updated NeXT and OpenStep configs
#
# Revision 3.9  1998/06/01 00:43:44  brianp
# added linux-sparc config from Timothy Small
#
# Revision 3.8  1998/05/31 23:34:38  brianp
# updated OS/2 support from Alexander Mai
#
# Revision 3.7  1998/04/22 00:49:42  brianp
# small changes to tar file lists
#
# Revision 3.6  1998/04/20 23:53:54  brianp
# added DavidB's 3Dfx v0.25 driver changes
#
# Revision 3.5  1998/04/18 05:01:33  brianp
# IRIX: make lib32 and lib64 directories
# added linux-sparc5-elf
#
# Revision 3.4  1998/03/24 00:52:28  brianp
# added openstep-dynlib config
#
# Revision 3.3  1998/03/11 03:13:00  brianp
# added linux-ppc-so config
#
# Revision 3.2  1998/03/10 02:10:46  brianp
# minor changes
#
# Revision 3.1  1998/02/21 01:41:10  brianp
# added sunos4-gcc-x11r6-sl
# more reorganization
#
# Revision 3.0  1998/02/14 17:43:55  brianp
# initial rev
#


SHELL = /bin/sh


# To add a new configuration for your system add it to the list below
# then update the Make-config file.



default:
	@echo "Type one of the following:"
	@echo "  make aix                  for IBM RS/6000 with AIX"
	@echo "  make aix-sl               for IBM RS/6000, make shared libs"
	@echo "  make amiwin               for Amiga with SAS/C and AmiWin"
	@echo "  make amix                 for Amiga 3000 UX  SVR4 v2.1 systems"
	@echo "  make beos-ppc             for BeOS with R3 or later"
	@echo "  make beos-x86             for BeOS with R3 or later"
	@echo "  make beos-glide           for BeOS with R3 or later, 3Dfx Glide driver"
	@echo "  make bsdos                for BSD/OS from BSDI using GCC"
	@echo "  make cygnus               for Win95/NT using Cygnus-Win32"
	@echo "  make cygnus-linux         for Win95/NT using Cygnus-Win32 under Linux"
	@echo "  make dgux                 for Data General"
	@echo "  make freebsd              for FreeBSD systems with GCC"
	@echo "  make freebsd-386          for FreeBSD systems with GCC, w/ Intel assembly"
	@echo "  make gcc                  for a generic system with GCC"
	@echo "  make hpux9                for HP systems with HPUX 9.x"
	@echo "  make hpux9-sl             for HP systems with HPUX 9.x, make shared libs"
	@echo "  make hpux9-gcc            for HP systems with HPUX 9.x using GCC"
	@echo "  make hpux9-gcc-sl         for HP systems with HPUX 9.x, GCC, make shared libs"
	@echo "  make hpux10               for HP systems with HPUX 10.x"
	@echo "  make hpux10-gcc           for HP systems with HPUX 10.x w/ GCC"
	@echo "  make hpux10-gcc-sl        for HP systems with HPUX 10.x w/ GCC, shared libs"
	@echo "  make irix4                for SGI systems with IRIX 4.x"
	@echo "  make irix5                for SGI systems with IRIX 5.x"
	@echo "  make irix5-gcc            for SGI systems with IRIX 5.x using GCC"
	@echo "  make irix5-dso            for SGI systems with IRIX 5.x, make DSOs"
	@echo "  make irix6-32             for SGI systems with IRIX 6.x, make 32-bit libs"
	@echo "  make irix6-32-dso         for SGI systems with IRIX 6.x, make 32-bit DSOs"
	@echo "  make irix6-n32            for SGI systems with IRIX 6.x, make n32-bit libs"
	@echo "  make irix6-n32-dso        for SGI systems with IRIX 6.x, make n32-bit DSOs"
	@echo "  make irix6-gcc-n32-sl     for SGI systems with IRIX 6.x, GCC, make n32 DSOs"
	@echo "  make irix6-64             for SGI systems with IRIX 6.x, make 64-bit libs"
	@echo "  make irix6-64-dso         for SGI systems with IRIX 6.x, make 64-bit DSOs"

	@echo "  make linux                for Linux systems, make static .a libs"
	@echo "  make linux-elf            for Linux systems, make ELF shared libs"
	@echo "  make linux-386            for Linux w/ Intel assembly"
	@echo "  make linux-386-elf        for Linux w/ Intel assembly, make ELF shared libs"
	@echo "  make linux-386-pthread    for Linux w/ Intel assembly and linuxthreads"
	@echo "  make linux-386-pthread-shared  for Linux w/ Intel assembly and linuxthreads"
	@echo "  make linux-ggi            for Linux systems with libggi"
	@echo "  make linux-alpha          for Linux on Alpha systems"
	@echo "  make linux-alpha-elf      for Linux on Alpha systems, make ELF shared libs"
	@echo "  make linux-ppc            for Linux on PowerPC systems"
	@echo "  make linux-ppc-so         for Linux on PowerPC systems, make shared libs"
	@echo "  make linux-glide          for Linux w/ 3Dfx Glide driver"
	@echo "  make linux-386-glide      for Linux w/ 3Dfx Glide driver, Intel assembly"
	@echo "  make linux-386-glide-mits for Linux w/ 3Dfx Glide driver, Intel assembly, MITS"
	@echo "  make linux-386-opt-glide  for Linux with 3Dfx Voodoo1 for GLQuake"
	@echo "  make linux-386-opt-V2-glide  for Linux with 3Dfx Voodoo2 for GLQuake"
	@echo "  make linux-sparc          for Linux on Sparc systems"
	@echo "  make linux-sparc5-elf     for Sparc5 systems, make ELF shared libs"
	@echo "  make lynxos               for LynxOS systems with GCC"
	@echo "  make macintosh            for Macintosh"
	@echo "  make machten-2.2          for Macs w/ MachTen 2.2 (68k w/ FPU)"
	@echo "  make machten-4.0          for Macs w/ MachTen 4.0.1 or newer with GNU make"
	@echo "  make mklinux              for Linux on Power Macintosh"
	@echo "  make netbsd               for NetBSD 1.0 systems with GCC"
	@echo "  make next                 for NeXT systems with NEXTSTEP 3.3"
	@echo "  make next-x86-x11         for NeXT on Intel x86 with X11"
	@echo "  make next-x11             for NeXT with X11"
	@echo "  make openbsd              for OpenBSD systems"
	@echo "  make openstep             for NeXT systems with OPENSTEP 4.0"
	@echo "  make openstep-dynlib      for NeXT systems with OPENSTEP 4.0, shared libs"
	@echo "  make os2-x11              for OS/2 with XFree86"
	@echo "  make osf1                 for DEC Alpha systems with OSF/1"
	@echo "  make osf1-sl              for DEC Alpha systems with OSF/1, make shared libs"
	@echo "  make qnx                  for QNX V4 systems with Watcom compiler"
	@echo "  make sco                  for SCO Unix systems with ODT"
	@echo "  make solaris-x86          for PCs with Solaris"
	@echo "  make solaris-x86-gcc      for PCs with Solaris using GCC"
#   @echo "  make solaris-gcc          for Solaris 2 systems with GCC"
	@echo "  make sunos4               for Suns with SunOS 4.x"
	@echo "  make sunos4-sl            for Suns with SunOS 4.x, make shared libs"
	@echo "  make sunos4-gcc           for Suns with SunOS 4.x and GCC"
	@echo "  make sunos4-gcc-sl        for Suns with SunOS 4.x, GCC, make shared libs"
	@echo "  make sunos5               for Suns with SunOS 5.x"
	@echo "  make sunos5-sl            for Suns with SunOS 5.x, make shared libs"
	@echo "  make sunos5-ultra         for Sun UltraSPARCs with SunOS 5.x"
	@echo "  make sunos5-ultra-sl      for Sun UltraSPARCs with SunOS 5.x, make shared libs"
	@echo "  make sunos5-thread        for Suns with SunOS 5.x, using Solaris threads"
	@echo "  make sunos5-pthread       for Suns with SunOS 5.[56] using POSIX threads"
	@echo "  make sunos5-gcc           for Suns with SunOS 5.x and GCC"
	@echo "  make sunos5-gcc-sl        for Suns with SunOS 5.x, GCC, make shared libs"
	@echo "  make sunos5-gcc-thread    for Suns with SunOS 5.x and GCC, using Solaris threads"
	@echo "  make sunos5-gcc-pthread   for Suns with SunOS 5.[56] and GCC, using POSIX threads"
	@echo "  make sunos5-x11r6-gcc-sl  for Suns with X11R6, GCC, make shared libs"
	@echo "  make sunSolaris-CC        for Solaris using C++ compiler"
	@echo "  make ultrix-gcc           for DEC systems with Ultrix and GCC"
	@echo "  make unicos               for Cray C90 (and other?) systems"
	@echo "  make unixware             for PCs running UnixWare"
	@echo "  make unixware-shared      for PCs running UnixWare, shared libs"
	@echo "  make vistra               for Stardent Vistra systems"
	@echo "  make clean                remove .o files"
	@echo "  make realclean            remove .o, library and executable files"



aix aix-sl amix bsdos dgux freebsd freebsd-386 gcc \
hpux9 hpux9-gcc hpux9-sl hpux9-gcc-sl hpux10 hpux10-gcc hpux10-gcc-sl \
irix-debug irix4 irix5 irix5-gcc irix5-dso irix6-32 \
irix6-32-dso \
linux linux-debug linux-elf \
linux-glide linux-386-glide linux-386-opt-glide \
linux-386-opt-V2-glide linux-386-glide-mits \
linux-386 linux-386-elf \
linux-386-pthread linux-386-pthread-shared \
linux-alpha linux-alpha-elf \
linux-ppc linux-ppc-so \
linux-sparc5-elf \
lynxos machten-2.2 machten-4.0 \
mklinux netbsd next-x86-x11 next-x11 osf1 osf1-sl openbsd qnx sco \
solaris-x86 solaris-x86-gcc sunSolaris-CC \
sunos4 sunos4-sl sunos4-gcc sunos4-gcc-sl sunos4-gcc-x11r6-sl \
sunos5 sunos5-sl sunos5-ultra sunos5-ultra-sl sunos5-gcc sunos5-gcc-sl \
sunos5-thread sunos5-pthread sunos5-gcc-thread sunos5-gcc-pthread \
sunos5-x11r6-gcc-sl ultrix-gcc unicos unixware vistra:
	-mkdir lib
	touch src/depend
	touch src-glu/depend
	if [ -d src-glut ] ; then touch src-glut/depend ; fi
	cd src ; $(MAKE) $@
	cd src-glu ; $(MAKE) $@
	if [ -d src-glut ] ; then cd src-glut ; $(MAKE) $@ ; fi
	if [ -d demos ]    ; then cd demos    ; $(MAKE) $@ ; fi
	if [ -d xdemos ]   ; then cd xdemos   ; $(MAKE) $@ ; fi
	if [ -d samples ]  ; then cd samples  ; $(MAKE) $@ ; fi
	if [ -d book ]     ; then cd book     ; $(MAKE) $@ ; fi


irix6-n32 irix6-n32-dso irix6-gcc-n32-sl:
	-mkdir lib32
	touch src/depend
	touch src-glu/depend
	if [ -d src-glut ] ; then touch src-glut/depend ; fi
	cd src ; $(MAKE) $@
	cd src-glu ; $(MAKE) $@
	if [ -d src-glut ] ; then cd src-glut ; $(MAKE) $@ ; fi
	if [ -d demos ]    ; then cd demos    ; $(MAKE) $@ ; fi
	if [ -d xdemos ]   ; then cd xdemos   ; $(MAKE) $@ ; fi
	if [ -d samples ]  ; then cd samples  ; $(MAKE) $@ ; fi
	if [ -d book ]     ; then cd book     ; $(MAKE) $@ ; fi


irix6-64 irix6-64-dso:
	-mkdir lib64
	touch src/depend
	touch src-glu/depend
	if [ -d src-glut ] ; then touch src-glut/depend ; fi
	cd src ; $(MAKE) $@
	cd src-glu ; $(MAKE) $@
	if [ -d src-glut ] ; then cd src-glut ; $(MAKE) $@ ; fi
	if [ -d demos ]    ; then cd demos    ; $(MAKE) $@ ; fi
	if [ -d xdemos ]  ; then cd xdemos  ; $(MAKE) $@ ; fi
	if [ -d samples ] ; then cd samples ; $(MAKE) $@ ; fi
	if [ -d book ]    ; then cd book    ; $(MAKE) $@ ; fi


amiwin:
	mklib.amiwin

beos-ppc beos-x86 beos-glide:
	-mkdir lib
	touch src/depend
	touch src-glu/depend
	cd src ; $(MAKE) -f Makefile.BeOS $@
	cd src-glu ; $(MAKE) -f Makefile.BeOS $@
	if [ -d BeOS ]    ; then cd BeOS    ; $(MAKE) -f Makefile.BeOS $@ ; fi

cygnus cygnus-linux:
	-mkdir lib
	touch src/depend
	touch src-glu/depend
	cd src ; $(MAKE) $@
	cd src-glu ; $(MAKE) $@
	cd src-glut ; $(MAKE) $@
	cd demos ; $(MAKE) $@
	if [ -d xdemos ]  ; then cd xdemos  ; $(MAKE) $@ ; fi

macintosh:
	@echo "See the README file for Macintosh intallation information"

next:
	-mkdir lib
	cd src ; $(MAKE) $@
	cd src-glu ; $(MAKE) $@

openstep:
	-mkdir lib
	cd src ; $(MAKE) $@
	cd src-glu ; $(MAKE) $@
	cd OpenStep/MesaView; gnumake

openstep-dylib:
	-mkdir lib
	cd src ; $(MAKE) -f Makefile.OpenStep $@
	cd src-glu ; $(MAKE) $@
	cd OpenStep/MesaView; gnumake

os2-x11:
	if not EXIST .\lib md lib
	touch src/depend
	touch src-glu/depend
	if exist src-glut touch src-glut/depend
	cd src     & make $@
	cd src-glu & make $@
	if exist src-glut  cd src-glut & make $@
	if exist demos     cd demos    & make $@
	if exist xdemos    cd xdemos   & make $@
	if exist samples   cd samples  & make $@
	if exist book      cd book     & make $@



linux-ggi:
	-mkdir lib
	touch src/depend
	touch src-glu/depend
	if [ -d src-glut ] ; then touch src-glut/depend ; fi
	if [ -d ggi ] ; then touch ggi/depend ; fi
	cd src ; $(MAKE) $@
	cd src/GGI/default ; $(MAKE)
	cd src-glu ; $(MAKE) $@
#	if [ -d src-glut ] ; then cd src-glut ; $(MAKE) $@ ; fi
	if [ -d ggi ]      ; then cd ggi      ; $(MAKE) $@ ; fi
	if [ -d ggi ]      ; then cd ggi/demos; $(MAKE)    ; fi
	if [ -d demos ]    ; then cd demos    ; $(MAKE) $@ ; fi
	if [ -d xdemos ]   ; then cd xdemos   ; $(MAKE) $@ ; fi
	if [ -d samples ]  ; then cd samples  ; $(MAKE) $@ ; fi
	if [ -d book ]     ; then cd book     ; $(MAKE) $@ ; fi

# if you change GGI_DEST please change it in ggimesa.conf, too.
GGI_DEST=/usr/local/lib/ggi/mesa/default

linux-ggi-install:
	install -d $(GGI_DEST)
	install -m 0755 src/GGI/default/*.so $(GGI_DEST)
	install -m 0644 src/GGI/ggimesa.conf /etc/ggi
	if [ -z "`grep ggimesa /etc/ggi/libggi.conf`" ]; then \
	echo ".include /etc/ggi/ggimesa.conf" >> /etc/ggi/libggi.conf ; \
	fi


# Remove .o files, emacs backup files, etc.
clean:
	-rm -f include/*~
	-rm -f include/GL/*~
	-rm -f src/*.o src/*~ src/*.a src/*/*.o src/*/*~
	-rm -f src-glu/*.o src-glu/*~ src-glu/*.a
	-rm -f src-glut/*.o
	-rm -f demos/*.o
	-rm -f book/*.o book/*~
	-rm -f xdemos/*.o xdemos/*~
	-rm -f samples/*.o samples/*~

# Remove everthing that can be remade
realclean: clean
	-rm -f lib/*
	cd demos   && $(MAKE) realclean || true
	cd xdemos  && $(MAKE) realclean || true
	cd book    && $(MAKE) realclean || true
	cd samples && $(MAKE) realclean || true



DIRECTORY = Mesa-3.0
LIB_NAME = MesaLib-3.0
DEMO_NAME = MesaDemos-3.0


LIB_FILES =	\
	$(DIRECTORY)/README*			\
	$(DIRECTORY)/IAFA-PACKAGE		\
	$(DIRECTORY)/LICENSE			\
	$(DIRECTORY)/VERSIONS			\
	$(DIRECTORY)/RELNOTES			\
	$(DIRECTORY)/FUTURE			\
	$(DIRECTORY)/CONFORM			\
	$(DIRECTORY)/Makefile*			\
	$(DIRECTORY)/Make-config		\
	$(DIRECTORY)/mklib.*			\
	$(DIRECTORY)/mklib-emx.cmd		\
	$(DIRECTORY)/*.BAT			\
	$(DIRECTORY)/*.bat			\
	$(DIRECTORY)/descrip.mms		\
	$(DIRECTORY)/mms-config			\
	$(DIRECTORY)/xlib.opt			\
	$(DIRECTORY)/STARTUP.MK			\
	$(DIRECTORY)/mesawin32.mak		\
	$(DIRECTORY)/Names.win			\
	$(DIRECTORY)/win32-openstep.sh		\
	$(DIRECTORY)/include/GL/dosmesa.h	\
	$(DIRECTORY)/include/GL/foomesa.h	\
	$(DIRECTORY)/include/GL/fxmesa.h	\
	$(DIRECTORY)/include/GL/ggimesa.h	\
	$(DIRECTORY)/include/GL/gl.h		\
	$(DIRECTORY)/include/GL/gl_mangle.h	\
	$(DIRECTORY)/include/GL/glu.h		\
	$(DIRECTORY)/include/GL/glu_mangle.h	\
	$(DIRECTORY)/include/GL/glx.h		\
	$(DIRECTORY)/include/GL/glx_mangle.h	\
	$(DIRECTORY)/include/GL/mglmesa.h	\
	$(DIRECTORY)/include/GL/osmesa.h	\
	$(DIRECTORY)/include/GL/osmesa3.h	\
	$(DIRECTORY)/include/GL/svgamesa.h	\
	$(DIRECTORY)/include/GL/wmesa.h		\
	$(DIRECTORY)/include/GL/xmesa.h		\
	$(DIRECTORY)/src/Makefile*		\
	$(DIRECTORY)/src/descrip.mms		\
	$(DIRECTORY)/src/mms_depend		\
	$(DIRECTORY)/src/*.def			\
	$(DIRECTORY)/src/*.rsp			\
	$(DIRECTORY)/src/depend			\
	$(DIRECTORY)/src/*.[chS]		\
	$(DIRECTORY)/src/DOS/*			\
	$(DIRECTORY)/src/FX/*.[ch]		\
	$(DIRECTORY)/src/FX/*.def		\
	$(DIRECTORY)/src/MGL/*			\
	$(DIRECTORY)/src/OSmesa/*.[ch]		\
	$(DIRECTORY)/src/S3/*			\
	$(DIRECTORY)/src/SVGA/*.[ch]		\
	$(DIRECTORY)/src/GGI/*.[ch]		\
	$(DIRECTORY)/src/GGI/ggimesa.conf	\
	$(DIRECTORY)/src/GGI/default/*.c	\
	$(DIRECTORY)/src/GGI/default/Makefile	\
	$(DIRECTORY)/src/Windows/*.[ch]		\
	$(DIRECTORY)/src/Windows/*.def		\
	$(DIRECTORY)/src/X/*.[ch]		\
	$(DIRECTORY)/src-glu/README[12]		\
	$(DIRECTORY)/src-glu/Makefile*		\
	$(DIRECTORY)/src-glu/descrip.mms	\
	$(DIRECTORY)/src-glu/mms_depend		\
	$(DIRECTORY)/src-glu/*.def		\
	$(DIRECTORY)/src-glu/*.rsp		\
	$(DIRECTORY)/src-glu/depend		\
	$(DIRECTORY)/src-glu/*.[ch]		\
	$(DIRECTORY)/widgets-mesa		\
	$(DIRECTORY)/widgets-sgi		\
	$(DIRECTORY)/util/README		\
	$(DIRECTORY)/util/*.c			\
	$(DIRECTORY)/BeOS			\
	$(DIRECTORY)/OpenStep			\
	$(DIRECTORY)/win32


DEMO_FILES =	\
	$(DIRECTORY)/include/GL/glut.h		\
	$(DIRECTORY)/src-glut/Makefile*		\
	$(DIRECTORY)/src-glut/depend		\
	$(DIRECTORY)/src-glut/*def		\
	$(DIRECTORY)/src-glut/descrip.mms	\
	$(DIRECTORY)/src-glut/mms_depend	\
	$(DIRECTORY)/src-glut/*.[ch]		\
	$(DIRECTORY)/demos/Makefile*		\
	$(DIRECTORY)/demos/descrip.mms		\
	$(DIRECTORY)/demos/*.[ch]		\
	$(DIRECTORY)/demos/*.rgb		\
	$(DIRECTORY)/demos/*.dat		\
	$(DIRECTORY)/xdemos/Makefile*		\
	$(DIRECTORY)/xdemos/descrip.mms		\
	$(DIRECTORY)/xdemos/*.[cf]		\
	$(DIRECTORY)/book/Makefile*		\
	$(DIRECTORY)/book/README		\
	$(DIRECTORY)/book/*.[ch]		\
	$(DIRECTORY)/samples/Makefile*		\
	$(DIRECTORY)/samples/README		\
	$(DIRECTORY)/samples/*.c		\
	$(DIRECTORY)/3Dfx			\
	$(DIRECTORY)/mtdemos			\
	$(DIRECTORY)/ggi


lib_tar:
	cd .. ; \
	tar -cvf $(LIB_NAME).tar $(LIB_FILES) ; \
	gzip $(LIB_NAME).tar ; \
	mv $(LIB_NAME).tar.gz $(DIRECTORY)

demo_tar:
	cd .. ; \
	tar -cvf $(DEMO_NAME).tar $(DEMO_FILES) ; \
	gzip $(DEMO_NAME).tar ; \
	mv $(DEMO_NAME).tar.gz $(DIRECTORY)

lib_zip:
	-rm $(LIB_NAME).zip
	cd .. ; \
	zip -r $(LIB_NAME).zip $(LIB_FILES) ; \
	mv $(LIB_NAME).zip $(DIRECTORY)

demo_zip:
	-rm $(DEMO_NAME).zip
	cd .. ; \
	zip -r $(DEMO_NAME).zip $(DEMO_FILES) ; \
	mv $(DEMO_NAME).zip $(DIRECTORY)



SRC_FILES =	\
	src/Makefile*		\
	src/depend		\
	src/*.[ch]		\
	include/GL/*.h

srctar:
	tar -cvf src.tar $(SRC_FILES) ; \
	gzip src.tar

srctar.zip:
	-rm src.zip
	zip -r src.zip $(SRC_FILES) ; \
