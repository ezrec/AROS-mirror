# makefile für Scalos IconObject library (C-Version)
# $Date$
# $Revision$
# $Id$
#############################################################

CSRCS 	= IconObject.c \
	  IconObject-classic.c
ASRCS 	= 

AS 	= phxAss
SPLAT		= sc:c/splat
LINK 	= slink
CC	= sc
LIBS 	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
PRECOMP = Include:all.gst
OBJDIR	= .sasobj

#############################################################

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################

.PHONY: clean install nodebug

#############################################################

CFLAGS 		= optimize nostackcheck nover dbg=s DATA=far idir=//include
LNFLAGS		= quiet batch noicons stripdebug SD
LNDBFLAGS 	= quiet batch noicons addsym SD

ifdef L
AFLAGS	= QUIET DS NOEXE opt=NRQB LIST=* XREFS linedebug I=SC:Assembler_Headers
#AFLAGS = -l -t -iINCLUDE:
else
AFLAGS	= QUIET DS opt=NRQB NOEXE linedebug I=SC:Assembler_Headers
#AFLAGS = -t -iINCLUDE:
endif

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#############################################################

OBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(CSRCS:%.c=$(OBJDIR)/%.o)

#############################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $* objectname $@

$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

############################################################

NAME    = .bin_os3/iconobject.library
DBGNAME = $(NAME).debug

#############################################################

all:	$(NAME) $(DBGNAME)

#############################################################

$(OBJDIR)/IconObject.o : iconnode.h iconobject_base.h
$(OBJDIR)/IconObject-classic.o : iconnode.h iconobject_base.h

#############################################################

$(NAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LINK) to $@ FROM $(OBJS) lib $(LIBS) $(LNFLAGS)

$(DBGNAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LINK) to $@ FROM $(OBJS) lib $(LIBS) $(LNDBFLAGS)

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(NAME)\033[0m to \033[1mLIBS: \033[0m\n'
	@copy $(NAME) LIBS: clone

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(NAME) $(DBGNAME) $(OBJS)
	@printf '\033[0m'

#############################################################

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################
