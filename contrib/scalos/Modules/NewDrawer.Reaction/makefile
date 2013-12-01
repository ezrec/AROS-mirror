# Makefile for NewDrawer.module (Reaction)
# using GNU make and SAS/C
# $Date$

#####################################################################

CHEADERS	= 
CC		= sc
CFLAGS		= optimize nostackcheck nochkabort debug=s \
		  idlen=128 strmer nover streq data=far \
		  idir=sc:include/ \
		  idir=include: idir=//include
AS		= phxass
AFLAGS		= QUIET m=68020 linedebug opt=NRQB i=include:
LD		= slink
LNFLAGS		= quiet batch noicons stripdebug
LNDBFLAGS 	= quiet batch noicons addsym
LIBS		= LIB:sc.lib LIB:debug.lib LIB:amiga.lib \
		  LIB:reaction.lib
CATCOMP		= catcomp
CSTARTUP	=
OBJDIR		= .sasobj

#####################################################################

.SUFFIXES: .asm

#####################################################################

NAME	 = NewDrawer.module
DBGNAME	 = $(NAME).debug
CATCOMPH = NewDrawerStrings.h
CATCOMPI = NewDrawerStrings.i

#####################################################################

all:	$(NAME) \
	$(DBGNAME)
# install
# clean

#####################################################################

ASRCS =		NDStrings.asm
CSRCS =		NewDrawer.c

#####################################################################

XOBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(CSRCS:%.c=$(OBJDIR)/%.o)
OBJS = $(XOBJS) NewDrawerGUI.o

#####################################################################

$(NAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $@ LIB $(LIBS) $(LNFLAGS)

$(DBGNAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $@ LIB $(LIBS) $(LNDBFLAGS)

#####################################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#####################################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $< objectname $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#####################################################################

$(CATCOMPH) $(CATCOMPI) : NewDrawerGUI.cd
	@printf '\033[32mMake CatComp header: \033[31m\033[1m$(CATCOMPH) $@ \033[32mfrom \033[31m$<\033[0m\n'
	@$(CATCOMP) $< cfile=$(CATCOMPH) asmfile=$(CATCOMPI) NOCODE XDEF

$(OBJDIR)/NDStrings.o : NewDrawerStrings.i

$(OBJDIR)/NewDrawer.o : NewDrawer.c NewDrawerGUI.h NewDrawerStrings.h

#####################################################################

$(NAME) $(DBGNAME) : $(OBJS)

#####################################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(NAME) \033[0mto  \033[31m\033[1mScalos:modules/ \033[0m\n'
	@copy $(NAME) Scalos:modules/

#####################################################################

# A little something to clean it all up
clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(XOBJS) $(NAME) $(DBGNAME) \
	  $(CATCOMPH) $(CATCOMPI)
	@printf '\033[0m'

#####################################################################
