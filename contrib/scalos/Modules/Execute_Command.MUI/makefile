# Makefile for Execute_Command.module (MUI)
# using GNU make and SAS/C
# $Date$

#####################################################################

TOPLEVEL	= /
SUBDIRMAKE 	= $(MAKE) -s -C 
CHEADERS	= 
CC		= sc
CFLAGS		= optimize nostackcheck nochkabort debug=s \
		  idlen=128 strmer nover streq data=far \
		  ignore=217 idir=sc:include/ \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  idir=include: idir=//include
AS		= phxass
AFLAGS		= QUIET m=68020 linedebug opt=NRQB i=include:
LD		= slink
LNFLAGS		= quiet batch noicons stripdebug
LNDBFLAGS 	= quiet batch noicons addsym
LIBS		= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
CATCOMP		= catcomp
FLEXCAT		= FlexCat
CSTARTUP	= LIB:c.o
OBJDIR		= .sasobj

SCALOS_LOCALE	= $(OBJDIR)/Execute_Command_Locale.h

#####################################################################

.SUFFIXES: .asm

#####################################################################

NAME		= .bin_os3/Execute_Command.module
DBGNAME		= $(NAME).debug
CATCOMPH	= $(SCALOS_LOCALE)
CAT_FILE	= Scalos/Execute_Command.catalog
DESTCAT		= Locale:Catalogs
ALLCATS 	= Catalogs/deutsch/$(CAT_FILE) \
		  Catalogs/français/$(CAT_FILE)

#####################################################################

all:	$(NAME) \
	$(DBGNAME) \
	allcatalogs
# install
# clean

#####################################################################

# make all Scalos .catalogs
allcatalogs:
	@$(SUBDIRMAKE) Catalogs/deutsch/Scalos
	@$(SUBDIRMAKE) Catalogs/français/Scalos

#####################################################################

CSRCS =		Execute_Command.c

#####################################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#####################################################################

XOBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(CSRCS:%.c=$(OBJDIR)/%.o)
OBJS = $(XOBJS)

#####################################################################

$(NAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(CSTARTUP) $(OBJS) TO $@ LIB $(LIBS) $(LNFLAGS)

$(DBGNAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(CSTARTUP) $(OBJS) TO $@ LIB $(LIBS) $(LNDBFLAGS)

#####################################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $< objectname $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#####################################################################

$(CATCOMPH) : Execute_Command.cd
	@printf '\033[32mMake CatComp header: \033[31m\033[1m$(CATCOMPH) \033[32mfrom \033[31m$<\033[0m\n'
	@$(FLEXCAT) $< $@=$(TOPLEVEL)/CatComp_h.sd

$(OBJDIR)/Execute_Command.o : Execute_Command.c Execute_Command.h $(CATCOMPH)

#####################################################################

$(NAME) $(DBGNAME) : $(OBJS)

#####################################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(NAME) \033[0mto  \033[31m\033[1mScalos:modules/ \033[0m\n'
	@copy $(NAME) Scalos:modules/
	@avail flush
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@copy "catalogs/deutsch/$(CAT_FILE)"  	"$(DESTCAT)/Deutsch/Scalos/" clone
	-@copy "catalogs/français/$(CAT_FILE)" 	"$(DESTCAT)/français/Scalos/" clone

#####################################################################

# A little something to clean it all up
clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	-@delete $(XOBJS) $(NAME) $(DBGNAME) $(CATCOMPH)
	@printf '\033[0m'

#####################################################################
