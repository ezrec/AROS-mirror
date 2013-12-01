# makefile for Scalos volumegauge.plugin
# $Date$
# using GNU make and SAS/C 6.58


#############################################################

CC 		= sc
AS		= phxass
PRECOMP 	= INCLUDE:All.gst
LD      	= slink
LDFLAGS 	= quiet batch noicons sd
LDLIBS  	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
OBJDIR		= .sasobj
DESTPLUG	= Scalos:Plugins/OOP
COMMON_DIR 	= ../../../../common/Plugin

.SUFFIXES: .plugin .plugin.debug

#############################################################

# Optimizer Flags
# Ignore Note 306: local function inlined: "InsertMH"
OPT_FLG 	= OPTIMIZE OPTINLOCAL OPTTIME OPTSCHED IGNORE=306,308 ERROR=87
OPT_FLG2 	= NOOPTIMIZE IGNORE=306,308 ERROR=87

ifdef DEBUG
	CFLAGS		= nostkchk nochkabort dbg=ff nover gst=$(PRECOMP) idlen=64 \
			idir=////include \
                  	idir=$(subst ../,/,$(COMMON_DIR))
	CSTARTUP	= LIB:c.o
else
	CFLAGS		= nostkchk nochkabort $(OPT_FLG) dbg=f def=NDEBUG \
			  def=NODEBUG nover gst=$(PRECOMP) idlen=64 \
			idir=////include \
                  	idir=$(subst ../,/,$(COMMON_DIR))
	CSTARTUP 	= LIB:cback.o
endif

AFLAGS		= quiet I=sc:Assembler_Headers

#############################################################

# Files for volumegauge.plugin
VOLGAUGENAME = volumegauge.plugin

VOLGAUGECSRCS =	$(COMMON_DIR)/plugin-classic.c \
		vg_plugin.c \
		VolumeGauge.c

XSRCS = $(notdir $(VOLGAUGECSRCS))
VOLGAUGEOBJS= $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

LIBS	= LIB:mempools.lib LIB:sc.lib LIB:debug.lib LIB:amiga.lib

#############################################################

all:	$(VOLGAUGENAME) \
	$(VOLGAUGENAME).debug \
# install
# clean

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c \
	$(COMMON_DIR)/plugin-common.c $(COMMON_DIR)/plugin.h plugin_data.h

$(OBJDIR)/persist.o : Persist.c Persist.h plugin_data.h

#############################################################

$(VOLGAUGENAME): $(VOLGAUGEOBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(VOLGAUGEOBJS) TO $(VOLGAUGENAME) lib $(LIBS) $(LDFLAGS) STRIPDEBUG 

$(VOLGAUGENAME).debug: $(VOLGAUGEOBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(VOLGAUGEOBJS) TO $(VOLGAUGENAME).debug lib $(LIBS) $(LDFLAGS) ADDSYM

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(VOLGAUGENAME).\033[0m\n'
	@copy $(VOLGAUGENAME) $(DESTPLUG) clone

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	-@delete $(OBJS) \
		$(VOLGAUGENAME) \
		$(VOLGAUGENAME).debug
	@printf '\033[0m'

#############################################################
