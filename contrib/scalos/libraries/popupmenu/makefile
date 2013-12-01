# makefile für Scalos popupmenu library
# Copyright Henrik Isaksson <henrik@isaksson.tk>
# $Date$
# $Revision$
#
#############################################################

CSRCS 	=  	popupmenu-classic.c	\
		pm.c			\
		pmgraph.c		\
		pminit.c		\
		window.c		\
                pmimages.c		\
		pmfind.c		\
		pmshadow.c		\
		pmtopography.c		\
		pmdlist.c		\
		pmlayout.c		\
		pmdrawshadow.c		\
		pmtags.c		\
		pmcreate.c		\
		pmmem.c			\
                pmprefs.c		\
                pminput.c		\
                pmversion.c		\
		iff.c			\

ASRCS 	= 

SPLAT	= sc:c/splat
LINK 	= slink
CC	= sc
LIBS 	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
PRECOMP = Include:all.gst
OBJDIR	= .sasobj

#############################################################

.SUFFIXES: plugin .plugin.debug

#############################################################

CFLAGS 		= optimize nostackcheck nover dbg=s DATA=far idlen=64 idir=//include
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

############################################################

NAME    = .bin_os3/popupmenu.library
DBGNAME = $(NAME).debug

#############################################################

all:	$(NAME) $(DBGNAME)

#############################################################

pmpriv.h : compiler.h popupmenu.h pmtypes.h pmmem.h \
	pmshadow.h pmtopography.h pminput.h \
        pmfind.h pminternals.h pmgraph.h pmimages.h

$(OBJDIR)/popupmenu-classic.o 	: pmpriv.h
$(OBJDIR)/pm.o 		: pmpriv.h
$(OBJDIR)/pmgraph.o 		: pmpriv.h
$(OBJDIR)/pminit.o 		: pmpriv.h
$(OBJDIR)/window.o 		: pmpriv.h
$(OBJDIR)/pmfind.o 		: pmpriv.h
$(OBJDIR)/pmshadow.o 		: pmpriv.h
$(OBJDIR)/pmtopography.o 	: pmpriv.h
$(OBJDIR)/pmdlist.o 		: pmpriv.h
$(OBJDIR)/pmlayout.o 		: pmpriv.h
$(OBJDIR)/pmdrawshadow.o 	: pmpriv.h
$(OBJDIR)/pmtags.c 		: pmpriv.h
$(OBJDIR)/pmcreate.o 		: pmpriv.h
$(OBJDIR)/pmmem.o 		: pmpriv.h
$(OBJDIR)/pmprefs.o 		: pmpriv.h
$(OBJDIR)/pminput.o 		: pmpriv.h
$(OBJDIR)/pmverson.o 		: pmpriv.h
$(OBJDIR)/iff.o 		: pmpriv.h

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
