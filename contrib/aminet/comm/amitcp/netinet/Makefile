# $Id$
# 
# Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>,
#                    Helsinki University of Technology, Finland.
#                    All rights reserved.
#
# Smakefile for AmiTCP/IP Project
#
# Created      : Mon Mar  8 03:15:42 1993 ppessi
# Last modified: Sat Mar 26 12:16:21 1994 too
# 
# HISTORY
# $Log$
# Revision 1.1  2001/12/25 22:28:13  henrik
# amitcp
#
# Revision 3.13  1994/04/22  13:53:20  jraja
# Minor changes
#
# Revision 3.12  1994/03/26  10:18:02  too
# Added kern/accesscontrol.[cho]
#
# Revision 3.11  1994/02/27  00:04:05  ppessi
# Added autodoc generation rules
#
# Revision 3.10  1994/02/18  06:20:15  jraja
# Updated SCOPTIONS for SAS/C 6.51.
#
# Revision 3.9  1994/02/15  20:47:53  jraja
# Removed SCOPTIONS from MISC sources, added SCOPTIONS dependency to the
# amitcp.gst rule.
#
# Revision 3.8  1994/02/03  19:12:31  ppessi
# Changed the default options slightly
#
# Revision 3.7  1994/02/03  04:41:17  ppessi
# Added net/sana2config.[hc]
# Changed the SCOPTIONS mechanism
#
# Revision 3.6  1994/01/23  22:10:49  jraja
# Added net/sana2perror.c, removed special rule for it.
#
# Revision 3.5  1994/01/12  07:13:36  jraja
# Moved sys/cdefs.h to the netinclude.
#
# Revision 3.4  1994/01/09  21:17:52  too
# Added amiga_errlists.c to API_C files
#
# Revision 3.3  1994/01/09  11:55:39  jraja
# Added one space between file names...
#
# Revision 3.2  1994/01/06  13:33:35  too
# Added amiga_sendrecv.[co] and sockargs.h to API files
#
# Revision 1.37  1993/11/11  23:48:08  jraja
# Changed assebler include dir to INCLUDE: (SAS/C standard).
# Added in_cksum.asm and mbuf.i to MISC.
#
# Revision 1.36  1993/10/21  01:58:48  ppessi
# Using optimized in_cksum() from netinet/in_cksum.asm
#
# Revision 1.35  1993/10/14  00:07:22  ppessi
# Added new target, install.
#
# Revision 1.34  1993/10/11  01:41:35  jraja
# Changed /src to /amitcp to accomodate the new directory structure.
#

MAKE = make
CC= gcc -noixemul
LD= ld
AS= gas

RM= rm
INSTALL= cp -r
MKDIR = mkdir

OPTIMIZATION-OPTION = -O2
BASEREL-OPTION = -fbaserel
DEBUG-OPTION = -g
#VERBOSITY = --verbose -Wall
VERBOSITY =  -Wall -Wno-unused -Wno-uninitialized
OBJECT-ONLY = -c

CFLAGS= $(OPTIMIZATION-OPTION) $(BASEREL-OPTION) $(DEBUG-OPTION)\
	 $(VERBOSITY) $(OBJECT-ONLY) 

#CFLAGS= -O0  -g -c
#CFLAGS= -O2 -resident -c
# -include all_includes.h 
# as a desparate measure
AFLAGS= -I .

IDIRS=   -I /netinclude -I /netinclude/conf -I /netinclude/protos 

DEFS= -DRCS_ID_C=// -DAMITCP -DKERNEL \
   -DDEBUG -DNDEBUG -DARP_DEBUG -DTCPDEBUG \
   -DICMPPRINTFS -DSOCKBUF_DEBUG \
   -DUSE_ALIGNED_COPIES -DDIRECTED_BROADCAST

#SCOPTIONS= NoCheckAbort NoStackCheck StructureEquivalence \
#  StringMerge StringsConst StringSection=Near \
#  NoMultipleIncludes NoErrorSource NoVersion NoErrorHighlight \
#  SmallCode SmallData \
#  Map MapHunk MapSymbols MapLib MapXReference \
#  LinkerOptions=Plain NoIcons Batch \
#  OptInl OptimizerInlineLocal OptimizerTime OptimizerScheduler \
#  OptComp=5 OptDep=5 OptRDep=5 \
#  $(IDIRS) $(DEFS) \
#  GlobalSymbolTable=amitcp.gst \
#  Ignore=306 Ignore=304 Ignore=308

#
# System headers
#
SYS_H= \
   sys/synch.h sys/uio.h \
   sys/kernel.h sys/malloc.h sys/mbuf.h sys/queue.h \
   sys/socketvar.h sys/domain.h sys/protosw.h \
   sys/systm.h 

PROTOS_H= \
   protos/kern/amiga_api_protos.h   protos/kern/amiga_main_protos.h \
   protos/kern/amiga_select_protos.h protos/kern/amiga_time_protos.h \
   protos/kern/amiga_userlib_protos.h protos/kern/kern_malloc_protos.h \
   protos/kern/kern_synch_protos.h protos/kern/subr_prf_protos.h \
   protos/kern/uipc_domain_protos.h \
   protos/kern/uipc_socket2_protos.h protos/kern/uipc_socket_protos.h \
   protos/net/if_loop_protos.h protos/net/if_protos.h \
   protos/net/radix_protos.h protos/net/raw_cb_protos.h \
   protos/net/raw_usrreq_protos.h protos/net/rtsock_protos.h \
   protos/netinet/in_cksum_protos.h protos/netinet/in_pcb_protos.h \
   protos/netinet/in_proto_protos.h protos/netinet/in_protos.h \
   protos/netinet/ip_icmp_protos.h protos/netinet/ip_input_protos.h \
   protos/netinet/ip_output_protos.h protos/netinet/raw_ip_protos.h \
   protos/netinet/tcp_debug_protos.h protos/netinet/tcp_input_protos.h \
   protos/netinet/tcp_output_protos.h protos/netinet/tcp_subr_protos.h \
   protos/netinet/tcp_timer_protos.h protos/netinet/tcp_usrreq_protos.h \
   protos/netinet/udp_usrreq_protos.h

# netinet -- internet protocols (ip, icmp, tcp, udp, arp)
NETINET_C= \
   in.c in_cksum.c in_pcb.c in_proto.c \
   ip_icmp.c \
   ip_input.c ip_output.c raw_ip.c \
   tcp_debug.c tcp_input.c tcp_output.c \
   tcp_subr.c tcp_timer.c tcp_usrreq.c \
   udp_usrreq.c

NETINET_O= \
   in.o in_cksum.o in_pcb.o in_proto.o \
   ip_icmp.o \
   ip_input.o ip_output.o raw_ip.o \
   tcp_debug.o tcp_input.o tcp_output.o \
   tcp_subr.o tcp_timer.o tcp_usrreq.o \
   udp_usrreq.o 

NETINET_H= \
   in_pcb.h in_var.h icmp_var.h  \
   tcpip.h tcp_debug.h tcp_fsm.h \
   tcp_seq.h tcp_timer.h tcp_var.h \
   udp_var.h 

# Misc headers
#
MISC_H=\
   all_includes.h \
   conf/conf.h conf/rcs.h

#
# Misc sources
#
#MISC= \
#  all_includes.c GNUmakefile Smakefile GCCOPTS \
#  kern/variables.src kern/config_var.awk \
#  netinet/in_cksum.asm sys/mbuf.i


CSRCS=   $(NETINET_C) 
OBJS=    $(NETINET_O) 
SRCS= $(CSRCS) $(SYS_H) $(NETINET_H)\
   $(PROTOS_H)
LIBSRC= 
LIBS=  # LIB netlib:sana2.lib

all: netinet


netinet:  $(NETINET_O)

netinet/in_cksum.o: netinet/in_cksum.asm
	$(AS) $(AFLAGS) netinet/in_cksum.asm -o$@

#install: all 
#  $(INSTALL) amitcp $(DEST)
#  $(INSTALL) bsdsocket.doc $(DEST)doc

#$(DEST)doc:
#  -$(MKDIR) $@

clean:
	-$(RM) *.o 

veryclean: clean

   
date:
	date

TAGS:
	etags $(CSRCS) 

.c.o:
	$(CC) $(CFLAGS) $(IDIRS) $(DEFS) $*.c

#.c.s:
#  $(CC) $(CFLAGS) $*.c DISASM=$@

# DO NOT DELETE THIS LINE -- make depend depends on it.
