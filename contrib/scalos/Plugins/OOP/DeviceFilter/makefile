# makefile for devicefilter.plugin
# $Date$
# $Revision$
# using SAS/C

#############################################################

.PHONY: clean install nodebug

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################


CHEADERS 	= devicefilter.h plugin_data.h $(COMMON_DIR)/plugin.h
CC      	= sc
CFLAGS  	= nostkchk nochkabort strcons strmer opt dbg=ff nover \
		  streq data=far idir=sc:include/ idir=include: \
		  idlen=64 idir=$(subst ../,/,$(COMMON_DIR)) idir=///include
SPLAT		= sc:c/splat
AS      	= phxass
AFLAGS  	= quiet m=68020 linedebug opt=NRQB i=include:
LD      	= slink
ECHO		= echo
LDFLAGS 	= quiet batch noicons sd
LDLIBS  	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
OBJDIR		= .sasobj
COMMON_DIR 	= ../../../common/Plugin

#############################################################

PLUGNAME = devicefilter.plugin

#############################################################

all:	$(PLUGNAME) $(PLUGNAME).debug
# install
# clean

#############################################################

CSRCS 	= 	$(COMMON_DIR)/plugin-classic.c \
		devicefilter.c

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

XSRCS = $(notdir $(CSRCS))
OBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

$(OBJDIR)/devicefilter.o: $(CHEADERS)
$(OBJDIR)/plugin-classic.o: $(COMMON_DIR)/plugin-classic.c \
	$(COMMON_DIR)/plugin-common.c $(CHEADERS)

#############################################################

$(PLUGNAME): $(OBJS) 
	@printf '\033[32mLink: \033[1m\033[31m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $@ LIB $(LDLIBS) $(DBFLAG) $(LDFLAGS) stripdebug

$(PLUGNAME).debug: $(OBJS) 
	@printf '\033[32mLink: \033[1m\033[31m$@\033[0m\n'
	@$(LD) FROM $(OBJS) TO $@ LIB $(LDLIBS) $(DBFLAG) $(LDFLAGS) addsym

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[1m\033[31m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble: \033[1m\033[31m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

$(OBJDIR)/%.o : %.asm
	@printf '\033[32mAssemble: \033[1m\033[31m$<\033[0m\n'
	@$(AS) $(AFLAGS) $< to $@

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(PLUGNAME)\033[0m\n'
	@copy $(PLUGNAME) Scalos:Plugins/OOP clone

#############################################################

# A little something to clean it all up
clean:
	@printf '\033[32mDelete: \033[31m\033[1m'
	@delete $(OBJS) $(PLUGNAME) $(PLUGNAME).debug
	@printf '\033[0m'

#############################################################

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################
