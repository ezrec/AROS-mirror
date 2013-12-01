# makefile für Scalos Pattern.prefsplugin
# $Date$
# $Revision$
#############################################################

SDPATH		= //

SUBDIRMAKE 	= $(MAKE) -s -C 
FLEXCAT		= FlexCat
CATCOMP 	= CatComp
LD 		= slink
CC 		= sc
SPLAT		= sc:c/splat

LIBS 		= LIB:scm.lib LIB:sc.lib LIB:mempools.lib \
		  LIB:debug.lib LIB:amiga.lib
LDFLAGS		= quiet batch noicons 
PRECOMP 	= Include:all.gst
ECHO		= echo
OBJDIR		= .sasobj
BINDIR		= .bin_os3

COMMON_DIR		= ../../../common/Plugin
BITMAPMCC_DIR		= ../../../common/BitMapMCC
BACKFILLMCC_DIR		= ../../../common/BackfillMCC
DATATYPESMCC_DIR	= ../../../common/DataTypesMCC

# search ScalosPattern.cd for Prefs/Pattern/catalogs/ 
PREFSPATH	= ../../../Prefs/Pattern

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################

CSRCS = 	$(COMMON_DIR)/plugin-classic.c \
		PatternPrefs.c \
                $(BACKFILLMCC_DIR)/Backfill.c \
		$(BITMAPMCC_DIR)/BitMapMCC.c \
                $(DATATYPESMCC_DIR)/DataTypesMCC.c \
		DoLoadDT.c

#############################################################

.PHONY: clean install nodebug

#############################################################

CFLAGS 		= optimize nostackcheck nover dbg=s DATA=far \
		  idlen=64 ignore=306 \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  idir=///include \
		  idir=$(subst ../,/,$(PREFSPATH)) \
		  idir=$(subst ../,/,$(BACKFILLMCC_DIR)) \
                  idir=$(subst ../,/,$(BITMAPMCC_DIR)) \
                  idir=$(subst ../,/,$(DATATYPESMCC_DIR)) \
		  idir=///common/Plugin
LNFLAGS 	= quiet batch noicons stripdebug SD
LNDBFLAGS 	= quiet batch noicons addsym SD

#############################################################

PLUGIN		= Pattern.prefsplugin
PLUGINDBG	= $(PLUGIN).debug
CAT_FILE	= Scalos/ScalosPattern.catalog
DESTTOOL	= Scalos:Prefs/
DESTCAT		= Locale:Catalogs
SRCCAT		= $(subst ../,/,$(PREFSPATH))
SCALOS_LOCALE	= $(OBJDIR)/ScalosPattern_locale.h
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

$(OBJDIR)/BitMapMCC.o  : $(BITMAPMCC_DIR)/BitMapMCC.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/Backfill.o  : $(BACKFILLMCC_DIR)/Backfill.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/DataTypesMCC.o  : $(DATATYPESMCC_DIR)/DataTypesMCC.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-common.c \
	plugin_data.h $(COMMON_DIR)/plugin.h

$(OBJDIR)/BitMapMCC.o  : $(BITMAPMCC_DIR)/BitMapMCC.c \
	$(BITMAPMCC_DIR)/BitMapMCC.h debug.h

$(OBJDIR)/Backfill.o : $(BACKFILLMCC_DIR)/Backfill.c \
	$(BACKFILLMCC_DIR)/Backfill.h $(BITMAPMCC_DIR)/BitMapMCC.h

$(OBJDIR)/DataTypesMCC.o : $(DATATYPESMCC_DIR)/DataTypesMCC.h

$(OBJDIR)/PatternPrefs.o : PatternPrefs.h \
		PatternPrefsImage.h \
		plugin_data.h \
                $(COMMON_DIR)/plugin.h \
                $(BACKFILLMCC_DIR)/Backfill.h \
        	$(BITMAPMCC_DIR)/BitMapMCC.h \
                $(DATATYPESMCC_DIR)/DataTypesMCC.h \
                $(CATCOMPHEADER)

#############################################################

PatternPrefs.c : $(SCALOS_LOCALE)

$(CATCOMPHEADER) : $(PREFSPATH)/ScalosPattern.cd
	@printf '\033[32mMake CatComp header: \033[31m\033[1m$@ \033[32mfrom \033[31m$<\033[0m\n'
	$(FLEXCAT) $(subst ../,/,$<) $@=$(SDPATH)/CatComp_h.sd

##############################################################################

$(BINDIR)/$(PLUGIN) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(OBJS) lib $(LIBS) $(LDFLAGS) STRIPDEBUG

$(BINDIR)/$(PLUGINDBG) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(OBJS) lib $(LIBS) $(LDFLAGS) ADDSYM

#############################################################

# copy all generated files to their final destinations
install:
	@avail flush
	@printf '\033[32mInstall: \033[31m\033[1m$(PLUGIN)\033[0m\n'
	@copy $(BINDIR)/$(PLUGIN) "$(DESTTOOL)"
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

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################

