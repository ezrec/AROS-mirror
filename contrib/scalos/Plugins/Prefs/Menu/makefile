# makefile für Scalos Menu.prefsplugin
# $Date$
# $Revision$
#############################################################

SDPATH		= //

SUBDIRMAKE 	= $(MAKE) -s -C
FLEXCAT		= FlexCat
CATCOMP 	= CatComp
FLEXCAT		= FlexCat
AS 		= phxAss
LD		= slink
CC 		= sc
ECHO		= echo
LIBS 		= LIB:sc.lib \
		LIB:debug.lib \
		LIB:mempools.lib \
		///SAS-lib/snprintf.lib \
		LIB:amiga.lib
LDFLAGS		= quiet batch noicons 
PRECOMP 	= Include:all.gst
OBJDIR		= .sasobj
BINDIR		= .bin_os3
PREFSPATH	= ../../../Prefs/Menu
DATATYPESMCC_DIR = ../../../common/DataTypesMCC
COMMON_DIR	= ../../../common/Plugin

.SUFFIXES: .asm

#############################################################

CSRCS = 	$(COMMON_DIR)/plugin-classic.c \
		MenuPrefs.c \
                DefaultMenu.c \
		$(DATATYPESMCC_DIR)/DataTypesMCC.c \

#############################################################

PLUGIN 		= Menu.prefsplugin
PLUGINDBG	= $(PLUGIN).debug
CAT_FILE	= Scalos/ScalosMenu.catalog
DESTTOOL	= Scalos:Prefs/
DESTCAT		= Locale:Catalogs
SRCCAT		= $(subst ../,/,$(PREFSPATH))
SCALOS_LOCALE	= $(OBJDIR)/ScalosMenu_locale.h
CATCOMPHEADER	= $(SCALOS_LOCALE)

CATS		= dansk \
		  deutsch \
		  español \
		  français \
		  italiano \
		  svenska \
		  ÃeÓtina

ALLCATS		= $(foreach cat,$(CATS),$(SRCCAT)/catalogs/$(cat)/$(CAT_FILE))

#############################################################

CFLAGS 		= optimize nostackcheck nover dbg=s DATA=far \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  idlen=64 idir=///include \
                  idir=$(subst ../,/,$(DATATYPESMCC_DIR)) \
                  idir=$(subst ../,/,$(PREFSPATH)) \
		  idir=///common/Plugin
AFLAGS 		= quiet DS opt=NRQB NOEXE linedebug I=SC:Assembler_Headers
#AFLAGS 	= -t -iINCLUDE:


#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

XSRCS = $(notdir $(CSRCS))
OBJS = $(ASRCS:%.asm=$(OBJDIR)/%.o) $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

All:	$(BINDIR)/$(PLUGIN) \
	$(BINDIR)/$(PLUGINDBG) \
	allcatalogs
# install
# clean

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/DataTypesMCC.o : $(DATATYPESMCC_DIR)/DataTypesMCC.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-common.c \
	plugin_data.h $(COMMON_DIR)/plugin.h

$(OBJDIR)/DataTypesMCC.o: $(DATATYPESMCC_DIR)/DataTypesMCC.c \
	$(DATATYPESMCC_DIR)/DataTypesMCC.h

$(OBJDIR)/MenuPrefs.o : MenuPrefs.h MenuPrefsImage.h \
	$(SCALOS_LOCALE) plugin_data.h $(COMMON_DIR)/plugin.h

$(OBJDIR)/DefaultMenu.o : DefaultMenu.c MenuPrefs.h \
        $(SCALOS_LOCALE)

#############################################################

$(CATCOMPHEADER) : $(PREFSPATH)/ScalosMenu.cd
	@printf '\033[32mMake CatComp header: \033[31m\033[1m$@ \033[32mfrom \033[31m$<\033[0m\n'
	$(FLEXCAT) $(subst ../,/,$<) $@=$(SDPATH)/CatComp_h.sd

$(BINDIR)/$(PLUGIN) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(OBJS) lib $(LIBS) $(LDFLAGS) STRIPDEBUG

$(BINDIR)/$(PLUGINDBG) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(OBJS) lib $(LIBS) $(LDFLAGS) ADDSYM

#############################################################

install:
	@avail flush
	@printf '\033[32mInstall: \033[31m\033[1m$(PLUGIN)\033[0m\n'
	@copy $(BINDIR)/$(PLUGIN) $(DESTTOOL)
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@$(foreach cat,$(CATS),copy "$(SRCCAT)/catalogs/$(cat)/$(CAT_FILE)" "$(DESTCAT)/$(cat)/Scalos" clone;)

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	-@delete $(BINDIR)/$(PLUGIN) $(BINDIR)/$(PLUGINDBG) $(OBJS) $(subst ../,/,$(CATCOMPHEADER)) $(ALLCATS)
	@printf '\033[0m'

#############################################################

# make all Scalos preferences .catalogs
allcatalogs:
	-@$(foreach cat,$(CATS),$(SUBDIRMAKE) $(PREFSPATH)/catalogs/$(cat)/Scalos;)

#############################################################

