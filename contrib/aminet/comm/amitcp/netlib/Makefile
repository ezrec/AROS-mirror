# $Id$
#
# Smakefile for AmiTCP/IP network support library 
#
# Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
#		   Helsinki University of Technology, Finland.
#		   All rights reserved. 
#
# Created      : Sat Mar 20 02:44:57 1993 ppessi
# Last modified: Sat Apr  2 14:57:09 1994 jraja
#
# $Log$
# Revision 1.1  2001/12/25 22:28:13  henrik
# amitcp
#
# Revision 3.12  1994/04/12  22:05:32  jraja
# Added module set_socket_stdio.c
#
# Revision 3.9  1994/04/02  12:01:36  jraja
# Removed getherrno.c
#
# Revision 3.8  1994/03/29  12:56:35  ppessi
# Added usergroup interface files, more unix IO files,
# circumvented "autodoc" argument count restrictions
#
# Revision 3.7  1994/03/24  16:33:42  jraja
# added many files, removed short int libraries (ppessi).
#
# Revision 3.6  1994/03/22  09:09:05  jraja
# Added unix io style modules: _allocufb.c, _chkufb.c, _close.c, _lseek.c,
# _open.c, _read.c, _write.c, fhopen.c, ioctl.c, iomode.c, isatty.c and
# stat.c.
#
# Revision 3.5  1994/02/27  15:03:31  jraja
# Added getherrno.c.
#
# Revision 3.4  1994/02/25  15:03:41  ppessi
# changed the install rules
#
# Revision 3.3  1994/02/16  08:02:36  jraja
# Added herror.*, added stubs.*o and perror.*o to the proper places.
#
# Revision 3.2  1994/02/03  19:21:15  ppessi
# Removed far, small and debugging libraries from normal installation.
#
# Revision 3.1  1994/01/24  00:34:26  jraja
# Removed errlst.c and gethostname.c,
# added strerror.c and syslog.c
#
# Revision 1.17  1994/01/21  12:02:11  ppessi
# Removed old link library get*ent() stuff. Added rcmd()
#
# Revision 1.16  1993/11/21  13:25:44  jraja
# Commented autodoc installation, since the docs are incomplete.
#
# Revision 1.15  1993/10/20  05:42:26  ppessi
# Added autodoc extraction.
#
# Revision 1.14  1993/10/18  06:13:15  jraja
# Added timerinit.c and gettimeofday.c.
#
# Revision 1.13  1993/10/15  01:14:47  ppessi
# Changed compilation mechanism, added debugging libraries.
#
# Revision 1.12  1993/10/14  00:05:35  ppessi
# SCOPTIONS is now created automatically from Smakefile
# Added user database handling functions.
# Added a new `install' target.
#
# ...
#
# Revision 1.1  93/03/22  03:23:54  03:23:54  ppessi (Pekka Pessi)
# Initial revision
#


MAKE = make

RM= rm
RM_RECURSIVE= rm -r
MKDIR= mkdir
CP= cp

CC= gcc -noixemul
LD= ld
AR= ar
AR_FLAGS= rs



OPTIMIZATION-OPTION = -O2
BASEREL-OPTION = -fbaserel
DEBUG-OPTION = -g
#VERBOSITY = --verbose -Wall
VERBOSITY = -Wall -Wno-unused -Wno-uninitialized
OBJECT-ONLY = -c

CFLAGS= $(OPTIMIZATION-OPTION) $(BASEREL-OPTION) $(DEBUG-OPTION)\
	 $(VERBOSITY) $(OBJECT-ONLY) 



#CFLAGS= -c

#OFLAGS= -O2 -fbaserel 
#GFLAGS= -O0 -g

INCLUDE_DIRS= -I .. -I ../conf

# We define SOCKETBASE_IN_CONTEXT for kernel usage.  Check <inline/socket.h>.
# same for USERGROUPBASE_IN_CONTEXT

DEFS= -D"RCS_ID_C=//" -DAMITCP=1 -DNETLIB=1 \
      -D__USE_SYSBASE=1 -DUSE_BUILTIN_MATH=1 -DAMITCP_NEW_NAMES\
	-DSOCKETBASE_IN_CONTEXT -DUSERGROUPBASE_IN_CONTEXT

 
#SCOPTIONS= parm=BOTH Nostackcheck \
#	Stringmerge Noerrorsource Nomultipleincludes Structureequivalence \
#	noversion noerrorhighlight IGNORE=224 IGNORE=92 \
#	optinl optinlocal opttime optcomp=5 optdep=5 optrdep=5 \
#	 $(IDIRS) $(DEFS) debug=line

#
# Note: sources with leading underscore "_" are specific with SAS C UFB 
# 
# As the standard autodoc extractor is broken (it allows only for 32 arguments)
# don't add sources with autodocs to the end of macro
#
#DOCSRC= autoinit.c gettimeofday.c herror.c \
#	lineread.c perror.c printfault.c rcmd.c strerror.c \
#	syslog.c timerinit.c dostat.c utime.c chmod.c _dup.c _dup2.c \
#	chown.c printuserfault.c init_usergroup.c \
#	popen.c sleep.c usleep.c \
#	init_inet_daemon.c set_socket_stdio.c serveraccept.c

#NETLIBSRC= strerror.c autoinit.c dummy.c lineread.c \
#        getopt.c printfault.c stubs.c perror.c herror.c \
#        timerinit.c gettimeofday.c rcmd.c syslog.c \
#        getpid.c chmod.c chown.c utime.c popen.c sleep.c usleep.c \
#	printuserfault.c init_usergroup.c setegid.c seteuid.c \
#	_dup.c _dup2.c stat.c _fstat.c fib.c dostat.c access.c \
#        _allocufb.c _chkufb.c _close.c _lseek.c _open.c _read.c \
#        _write.c fhopen.c ioctl.c iomode.c isatty.c \
#	init_inet_daemon.c set_socket_stdio.c serveraccept.c


NETLIBSRC= strerror.c lineread.c \
        printfault.c stubs.c \
        syslog.c \
        printuserfault.c \
	set_socket_stdio.c serveraccept.c



SANA2LIBSRC= sana2perror.c sana2errlist.c sana2printfault.c

#DOCS= netlib.doc sana2lib.doc

LIBS= libnet.a libsana2.a

DLIBS= gnet.lib gsana2.lib

NETOBJ= strerror.o lineread.o \
        printfault.o stubs.o \
        syslog.o \
	printuserfault.o \
	set_socket_stdio.o serveraccept.o

#NETOBJ= strerror.o autoinit.o dummy.o lineread.o \
#        getopt.o printfault.o stubs.o perror.o herror.o \
#        timerinit.o gettimeofday.o rcmd.o syslog.o \
#        getpid.o chmod.o chown.o utime.o popen.o sleep.o usleep.o \
#	printuserfault.o init_usergroup.o setegid.o seteuid.o \
#	_dup.o _dup2.o stat.o _fstat.o fib.o dostat.o access.o \
#        _allocufb.o _chkufb.o _close.o _lseek.o _open.o _read.o \
#        _write.o fhopen.o ioctl.o iomode.o isatty.o \
#	init_inet_daemon.o set_socket_stdio.o serveraccept.o

SANA2OBJ=   sana2perror.o   sana2errlist.o   sana2printfault.o

#GNETOBJ= strerror.go autoinit.go dummy.go lineread.go \
#        getopt.go printfault.go stubs.go perror.go herror.go \
#        timerinit.go gettimeofday.go rcmd.go syslog.go \
#        getpid.go chmod.go chown.go utime.go popen.go sleep.go usleep.go \
#	printuserfault.go init_usergroup.go setegid.go seteuid.go \
#	_dup.go _dup2.go stat.go _fstat.go fib.go dostat.go access.go \
#        _allocufb.go _chkufb.go _close.go _lseek.go _open.go _read.go \
#        _write.go fhopen.go ioctl.go iomode.go isatty.go \
#	init_inet_daemon.go set_socket_stdio.go serveraccept.go


#GSANA2OBJ=   sana2perror.go   sana2errlist.go   sana2printfault.go

.SUFFIXES: .c .o  
#
# Generic rules
#
.c.o:
	$(CC) $(CFLAGS) $(OFLAGS) $(DEFS) $(INCLUDE_DIRS) $*.c
#.c.go:
#	$(CC) $(CFLAGS) $(GFLAGS) $(DEFS) $(INCLUDE_DIRS) $*.c OBJNAME=$*.go

normal: $(LIBS)

all: normal 

#debug:  $(DLIBS)

netlib:
	-$(MKDIR) $@
	

#SCOPTIONS: Smakefile
#	copy to $@ <from < $(SCOPTIONS) <

#
# Network libraries
#
libnet.a: $(NETOBJ)
	-$(RM) $@
	$(AR) $(AR_FLAGS) $@ $(NETOBJ) 

#
# Sana2.lib, convenience functions for Sana2 utilities
#
libsana2.a: $(SANA2OBJ)
	-$(RM) $@
	$(AR) $(AR_FLAGS) $@ $(SANA2OBJ)

#
# Debugging libraries
#
#gnet.lib: $(GNETOBJ)
#	-$(RM) $@
#	$(AR) $(AR_FLAGS) $@ $(GNETOBJ)


#gsana2.lib: $(GSANA2OBJ)
#	-$(RM) $@
#	$(AR) $(AR_FLAGS) $@ $(GSANA2OBJ)

clean:
	-$(RM) \*.(lib|o|nbo|so|go|gnbo|gso)

# DO NOT DELETE THIS LINE -- make depend depends on it.




