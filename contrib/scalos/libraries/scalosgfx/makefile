# makefile für Scalos scalosgfx.library
# $Date$
# $Revision$
# $Id$
############################################################

CSRCS 	=	scalosgfx-classic.c \
		scalosgfx.c \
                argb.c \
                blit.c \
                Dither.c \
		Render.c \
                BitMapScale.c \


ASRCS 	= 

SPLAT	= sc:c/splat
LINK 	= slink
CC	= sc
LIBS 	= LIB:scmnb.lib LIB:scnb.lib LIB:debug.lib LIB:amiga.lib
PRECOMP = Include:all.gst
OBJDIR	= .sasobj

#############################################################

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################

CFLAGS 		= optimize nochkabort nostackcheck nover \
		  dbg=f DATA=far code=far \
		  ignore=306 idlen=64 idir=//include
LNFLAGS		= quiet batch noicons stripdebug 
LNDBFLAGS 	= quiet batch noicons addsym

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#############################################################

OBJS = $(CSRCS:%.c=$(OBJDIR)/%.o)

#############################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $* objectname $@

############################################################

NAME    = .bin_os3/scalosgfx.library
DBGNAME = $(NAME).debug

#############################################################

all:	$(NAME) $(DBGNAME)
# install
# clean

#############################################################

$(OBJDIR)/scalosgfx-classic.o 	: scalosgfx_base.h scalosgfx.h
$(OBJDIR)/scalosgfx.o 		: scalosgfx_base.h scalosgfx.h
$(OBJDIR)/argb.o 		: scalosgfx_base.h scalosgfx.h
$(OBJDIR)/blit.o 		: scalosgfx_base.h scalosgfx.h
$(OBJDIR)/Dither.o 		: scalosgfx_base.h scalosgfx.h
$(OBJDIR)/Render.o 		: scalosgfx_base.h scalosgfx.h
$(OBJDIR)/BitMapScale.o	: scalosgfx_base.h scalosgfx.h

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
	@avail flush

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(NAME) $(DBGNAME) $(OBJS)
	@printf '\033[0m'

#############################################################

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################
