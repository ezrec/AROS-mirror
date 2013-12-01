# makefile for SAS/C
# $Date$

#############################################################

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################

CC      	= sc
CFLAGS  	= optimize nostackcheck nover dbg=s DATA=far \
		  idlen=64 idir=///include
LD      	= slink
ECHO		= echo
LDFLAGS 	= quiet batch noicons SD
LIBS  		= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
ECHO		= echo
OBJDIR		= .sasobj
COMMON_DIR 	= ../../../common/Plugin

#############################################################

PLUGNAME = title_envvar.plugin

#############################################################

all:	$(PLUGNAME) $(PLUGNAME).debug
# install
# clean

#############################################################

CSRCS 	=	$(COMMON_DIR)/plugin-classic.c \
		libfuncs.c

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

XSRCS = $(notdir $(CSRCS))
OBJS = $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

$(OBJDIR)/plugin-classic.o  : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

#############################################################

# CLI command used when linking the final executable
$(PLUGNAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(OBJS) lib $(LIBS) $(LDFLAGS) STRIPDEBUG

$(PLUGNAME).debug : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(OBJS) lib $(LIBS) $(LDFLAGS) ADDSYM

#############################################################

$(OBJDIR)/plugin-classic.o : plugin_data.h \
	$(COMMON_DIR)/plugin.h $(COMMON_DIR)/plugin-common.c


#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(PLUGNAME)\033[0m\n'
	@copy $(PLUGNAME) Scalos:Plugins/OOP clone

#############################################################


# A little something to clean it all up
clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	-@delete $(OBJS) $(PLUGNAME) $(PLUGNAME).debug
	@printf '\033[0m'

#############################################################
