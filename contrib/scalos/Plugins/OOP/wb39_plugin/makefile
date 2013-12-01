# makefile for several Scalos plugins
# $Date$
# using GNU make and SAS/C 6.58


#############################################################

SUBDIRMAKE 	= $(MAKE) -s -C 
CC 		= sc
AS		= phxass
PRECOMP 	= INCLUDE:All.gst
LD      	= slink
LDFLAGS 	= quiet batch noicons sd
LDLIBS  	= LIB:sc.lib LIB:debug.lib LIB:amiga.lib
OBJDIR		= .sasobj
DESTPLUG	= Scalos:Plugins/OOP
COMMON_DIR 	= ../../../common/Plugin

.SUFFIXES: .plugin .plugin.debug

#############################################################

SUBDIRS		= persist \
		  volumegauge \
		  wbrexx \

#############################################################

# Optimizer Flags
# Ignore Note 306: local function inlined: "InsertMH"
OPT_FLG 	= OPTIMIZE OPTINLOCAL OPTTIME OPTSCHED IGNORE=306,308 ERROR=87
OPT_FLG2 	= NOOPTIMIZE IGNORE=306,308 ERROR=87

ifdef DEBUG
	CFLAGS		= nostkchk nochkabort dbg=ff nover gst=$(PRECOMP) idlen=64 \
			idir=///include \
			idir=volumegauge \
                  	idir=$(subst ../,/,$(DATATYPESMCC_DIR))

	CFLAGS2		= nostkchk nochkabort dbg=ff nover gst=$(PRECOMP) idlen=64 \
			idir=///include \
			idir=volumegauge \
                  	idir=$(subst ../,/,$(DATATYPESMCC_DIR))
	CSTARTUP	= LIB:c.o
else
	CFLAGS		= nostkchk nochkabort $(OPT_FLG) dbg=f def=NDEBUG \
			  def=NODEBUG nover gst=$(PRECOMP) idlen=64 \
			idir=///include \
			idir=volumegauge \
                  	idir=$(subst ../,/,$(DATATYPESMCC_DIR))
	CFLAGS2		= nostkchk nochkabort $(OPT_FLG2) dbg=f def=NDEBUG \
			  def=NODEBUG nover gst=$(PRECOMP) idlen=64 \
			idir=///include \
			idir=volumegauge \
                  	idir=$(subst ../,/,$(DATATYPESMCC_DIR))
	CSTARTUP 	= LIB:cback.o
endif

AFLAGS		= quiet I=sc:Assembler_Headers

#############################################################

# Files for wb39.plugin
WB39NAME = 	wb39.plugin

WB39CSRCS = 	$(COMMON_DIR)/plugin-classic.c \
		wb39.c \
		AppWindow.c \
		WorkbenchControl.c \
		volumegauge/VolumeGauge.c \
		AppIcons.c \
		Scalos_Helper.c \

XSRCS = $(notdir $(WB39CSRCS))
WB39OBJS= $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

LIBS	= LIB:mempools.lib LIB:sc.lib LIB:debug.lib LIB:amiga.lib

#############################################################

all:	$(WB39NAME) \
	$(WB39NAME).debug \
	allsubdirs \
	test
# install
# clean
# $(DEFICONSNAME) $(DEFICONSNAME).debug

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/VolumeGauge.o : volumegauge/VolumeGauge.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

$(OBJDIR)/wb39.o : wb39.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS2) $< objectname $@

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c \
	$(COMMON_DIR)/plugin-common.c $(COMMON_DIR)/plugin.h plugin_data.h

$(OBJDIR)/wb39.o AppWindow.o WorkbenchControl.o VolumeGauge.o \
	AppIcons.o Scalos_Helper.o : wb39.h

$(OBJDIR)/wb39.o VolumeGauge.o vg_plugin.o : volumegauge/VolumeGauge.h

#############################################################

$(WB39NAME): $(WB39OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(WB39OBJS) TO $(WB39NAME) lib $(LIBS) $(LDFLAGS) STRIPDEBUG 

$(WB39NAME).debug: $(WB39OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(WB39OBJS) TO $(WB39NAME).debug lib $(LIBS) $(LDFLAGS) ADDSYM

#############################################################

test: $(OBJDIR)/test.o
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM LIB:c.o $(OBJDIR)/test.o TO $@ LIB $(LIBS) $(LDFLAGS) $(DBFLAG)

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(WB39NAME).\033[0m\n'
	@copy $(WB39NAME) $(DESTPLUG) clone
	-@$(foreach cat,$(SUBDIRS),$(SUBDIRMAKE) $(cat) install;)

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	-@delete $(WB39OBJS) \
		$(OBJDIR)/test.o \
		test \
		$(WB39NAME) \
		$(WB39NAME).debug
	@printf '\033[0m'
	-@$(foreach cat,$(SUBDIRS),$(SUBDIRMAKE) $(cat) clean;)

#############################################################

# make all subdirectories
allsubdirs:
	-@$(foreach cat,$(SUBDIRS),$(SUBDIRMAKE) $(cat);)

#############################################################

