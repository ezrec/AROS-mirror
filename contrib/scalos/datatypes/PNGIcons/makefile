# makefile for PNG icon datatype
# using GNU make and SAS/C
# $Date$
# $Revision$
# $Id$
#####################################################################

ASRCS =
CSRCS =		PNGIconDt-classic.c \
		PNGIconDt.c \
		LoadPngIcon.c \
		GetPNGPicture.c \
		SavePngIcon.c \

#####################################################################

AS		= phxass
SPLAT		= sc:c/splat
LD		= slink
CC		= sc
MKDIR   	= mkdir -p #makedir force
LIBS		= 	//SAS-lib/png.lib \
			//SAS-lib/z.lib \
			//SAS-lib/snprintf.lib \
			LIB:mempools.lib \
			LIB:scm.lib \
			LIB:sc.lib \
			LIB:debug.lib \
			LIB:amiga.lib
PRECOMP 	= Include:all.gst
DT_DIR		= scalos:IconDatatypes/datatypes
OBJDIR		= .sasobj

.SUFFIXES: .asm

#############################################################

.PHONY: clean install nodebug

#####################################################################

CFLAGS		= optimize nostackcheck nochkabort strcons debug=s \
		  IGNORE=73 strmer nover streq \
		  idir=sc:include/ \
		  idir=include: \
		  idir=//include
AFLAGS		= QUIET m=68020 linedebug opt=NRQB i=include:
LNFLAGS		= quiet batch noicons stripdebug
LNDBFLAGS 	= quiet batch noicons addsym
CSTARTUP	=

#####################################################################

OBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(CSRCS:%.c=$(OBJDIR)/%.o)

#####################################################################

DTNAME		= .bin_os3/pngiconobject.datatype
DTDBGNAME	= $(DTNAME).debug

#####################################################################

all:	$(DTNAME) \
	$(DTDBGNAME)

#####################################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#####################################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $< objectname $@

$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#####################################################################

$(DTNAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $(DTNAME) LIB $(LIBS) $(LNFLAGS)

$(DTDBGNAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $(DTDBGNAME) LIB $(LIBS) $(LNDBFLAGS)

#####################################################################

$(OBJDIR)/PNGIconDt-classic.o : PNGIconDt.c PngIconDt.h PNGIconProto.h
$(OBJDIR)/PNGIconDt.o : PNGIconDt.c PngIconDt.h PNGIconProto.h
$(OBJDIR)/LoadPngIcon.o : LoadPngIcon.c PngIconDt.h PNGIconProto.h
$(OBJDIR)/SavePngIcon.o : SavePngIcon.c PngIconDt.h PNGIconProto.h

#####################################################################

install:
	@printf '\033[32mFlushing memory...\033[0m\n'
	avail flush
	@printf '\033[32mInstall: \033[31m\033[1m$(DTNAME)\033[0m\n'
	-@$(MKDIR) $(DT_DIR)
	copy $(DTNAME) $(DT_DIR)

#####################################################################

clean:
	@printf '\033[32mCleanup...\033[0m\n'
	-@delete $(OBJS) $(DTNAME) $(DTNAME).debug

#############################################################

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################

