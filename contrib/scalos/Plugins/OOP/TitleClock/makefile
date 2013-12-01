# makefile for TitleClock.plugin
# $Date$
# $Revision$
# $Id$
# using SAS/C

#############################################################

.SUFFIXES: .asm .plugin .plugin.debug

#############################################################

CHEADERS	= libbase.h libfuncs.h plugin_data.h $(COMMON_DIR)/plugin.h
CC      	= sc
CFLAGS  	= nostkchk nochkabort strcons strmer opt dbg=ff nover \
		  streq data=far idlen=64 idir=include: idir=$(subst ../,/,$(COMMON_DIR)) \
		  idir=///include
LD      	= slink
ECHO		= echo
LDFLAGS 	= QUIET NOICONS BATCH
LDLIBS  	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
OBJDIR		= .sasobj
COMMON_DIR 	= ../../../common/Plugin
PREFS_DIR	= prefs

#############################################################

PLUGNAME 	= title_clock.plugin

#############################################################

all: 	$(PLUGNAME) $(PLUGNAME).debug
# install
# clean

#############################################################

CSRCS	=	$(COMMON_DIR)/plugin-classic.c \
 		libfuncs.c \
		$(PREFS_DIR)/prefs_file.c

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

XSRCS = $(notdir $(CSRCS))
OBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

$(OBJDIR)/title_clock.o: $(CHEADERS)
$(OBJDIR)/plugin-classic.o: $(COMMON_DIR)/plugin-classic.c $(CHEADERS) 

#############################################################

$(PLUGNAME) : $(OBJS) 
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $@ LIB $(LDLIBS) $(LDFLAGS) STRIPDEBUG

$(PLUGNAME).debug : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $@ LIB $(LDLIBS) $(LDFLAGS) ADDSYM

#############################################################

$(OBJDIR)/plugin-classic.o  : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/prefs_file.o : $(PREFS_DIR)/prefs_file.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $< objectname $@

#############################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#############################################################

$(OBJDIR)/libfuncs.o	: $(CHEADERS)
$(OBJDIR)/prefs_file.o : $(PREFS_DIR)/prefs_file.h
$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-common.c $(CHEADERS)

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(PLUGNAME)\033[0m\n'
	@copy $(PLUGNAME) Scalos:Plugins/OOP clone

#############################################################

# A little something to clean it all up
clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(OBJS) $(PLUGNAME) $(PLUGNAME).debug
	@printf '\033[0m'
