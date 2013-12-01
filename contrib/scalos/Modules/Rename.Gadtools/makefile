# Makefile for Rename.module (MUI)
# using GNU make and SAS/C
# $Date$

#####################################################################

CSRCS =		Rename.c

#####################################################################

CC		= sc
CFLAGS		= optimize nostackcheck nochkabort debug=s \
		  idlen=128 strmer nover streq idir=sc:include/ idir=include: idir=//include
AS		= phxass
AFLAGS		= QUIET m=68020 linedebug opt=NRQB i=include:
LD		= slink
LNFLAGS		= quiet batch noicons stripdebug
LNDBFLAGS 	= quiet batch noicons addsym
LIBS		= //SAS-lib/z.lib LIB:sc.lib LIB:debug.lib LIB:amiga.lib
CSTARTUP	= LIB:c.o
OBJDIR		= .sasobj

#####################################################################

.SUFFIXES: .asm

#####################################################################

NAME		= Rename.module
DBGNAME		= $(NAME).debug

#####################################################################

all:	$(NAME) $(DBGNAME)
# install clean

#####################################################################

OBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(CSRCS:%.c=$(OBJDIR)/%.o)

#####################################################################

$(NAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(CSTARTUP) $(OBJS) TO $@ LIB $(LIBS) $(LNFLAGS)

$(DBGNAME): $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(CSTARTUP) $(OBJS) TO $@ LIB $(LIBS) $(LNDBFLAGS)

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

$(NAME) $(DBGNAME) : $(OBJS)

#####################################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(NAME) \033[0mto \033[31m\033[1mScalos:modules/ \033[0m\n'
	copy $(NAME) Scalos:modules/

#####################################################################

# A little something to clean it all up
clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(OBJS) $(NAME) $(DBGNAME)
	@printf '\033[0m'

#####################################################################

