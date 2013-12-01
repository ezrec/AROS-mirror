# makefile für Scalos FileTypesPrefs.prefsplugin
# $Date$
# $Revision$
#############################################################

SDPATH		= //

SUBDIRMAKE 	= $(MAKE) -s -C 
FLEXCAT		= FlexCat
CATCOMP 	= CatComp
AS		= phxAss
LD 		= slink
CC 		= sc
ECHO		= echo
LIBS 		= LIB:scm.lib \
		LIB:sc.lib \
		LIB:mempools.lib \
		///SAS-lib/snprintf.lib \
		LIB:debug.lib \
		LIB:amiga.lib
LDFLAGS		= quiet batch noicons 
PRECOMP 	= Include:all.gst
OBJDIR		= .sasobj
BINDIR		= .bin_os3
PREFSPATH	= ../../../Prefs/FileTypes
FONTSAMPLE_DIR	= ../../../common/FontSampleMCC
ICONOBJMCC_DIR	= ../../../common/IconobjectMCC
DATATYPESMCC_DIR = ../../../common/DataTypesMCC
COMMON_DIR	= ../../../common/Plugin

.SUFFIXES: .s .asm .plugin .plugin.debug

#############################################################

CSRCS 	= 	$(COMMON_DIR)/plugin-classic.c \
		FileTypesPrefs.c \
		ReadFtPrefs.c \
		WriteFtPrefs.c \
		DefIcons.c \
		DefIconsPrefs.c \
                DefaultDefIconsPrefs.c \
		$(FONTSAMPLE_DIR)/FontSampleMCC.c \
		$(ICONOBJMCC_DIR)/IconobjectMCC.c \
		$(DATATYPESMCC_DIR)/DataTypesMCC.c \

#############################################################

CFLAGS 		= optimize nostackcheck nover dbg=s \
		  data=FAR code=far \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  idlen=64 idir=///include idir=///common/FontSampleMCC \
		  idir=$(subst ../,/,$(PREFSPATH)) \
		  idir=$(subst ../,/,$(ICONOBJMCC_DIR)) \
                  idir=$(subst ../,/,$(DATATYPESMCC_DIR)) \
		  idir=///common/Plugin
AFLAGS 		= QUIET DS opt=NRQB NOEXE linedebug I=SC:Assembler_Headers
#AFLAGS 	= -t -iINCLUDE:

#############################################################

PLUGIN		= FileTypes.prefsplugin
PLUGINDBG	= $(PLUGIN).debug
CAT_FILE	= Scalos/ScalosFileTypes.catalog
DESTTOOL	= Scalos:Prefs/
DESTCAT		= Locale:Catalogs
SRCCAT		= $(subst ../,/,$(PREFSPATH))
SCALOS_LOCALE	= $(OBJDIR)/ScalosFileTypes_locale.h
CATCOMPHEADER	= $(SCALOS_LOCALE)
CATS		= deutsch \
		  français

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

$(OBJDIR)/DataTypesMCC.o : $(DATATYPESMCC_DIR)/DataTypesMCC.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/FontSampleMCC.o : $(FONTSAMPLE_DIR)/FontSampleMCC.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/IconobjectMCC.o : $(ICONOBJMCC_DIR)/IconobjectMCC.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $< objectname $@

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-common.c \
	plugin_data.h $(COMMON_DIR)/plugin.h

$(OBJDIR)/FileTypesPrefs.o : FileTypesPrefs.h FileTypesPrefs_proto.h \
	$(ICONOBJMCC_DIR)/IconobjectMCC.h \
	FileTypesPrefsImage.h plugin_data.h $(COMMON_DIR)/plugin.h \
      	$(SCALOS_LOCALE) debug.h

$(OBJDIR)/ReadFtPrefs.o : ReadFtPrefs.c FileTypesPrefs.h \
	FileTypesPrefs_proto.h debug.h

$(OBJDIR)/WriteFtPrefs.o : WriteFtPrefs.c FileTypesPrefs.h \
	FileTypesPrefs_proto.h debug.h

$(OBJDIR)/DefIcons.o : DefIcons.c FileTypesPrefs.h \
	FileTypesPrefs_proto.h debug.h

$(OBJDIR)/DefIconsPrefs.o : DefIconsPrefs.c FileTypesPrefs.h \
	FileTypesPrefs_proto.h debug.h \
        $(SCALOS_LOCALE)

$(OBJDIR)/DefaultDefIconsPrefs.o : DefaultDefIconsPrefs.c

$(OBJDIR)/DataTypesMCC.o: $(DATATYPESMCC_DIR)/DataTypesMCC.c \
	$(DATATYPESMCC_DIR)/DataTypesMCC.h

$(OBJDIR)/FontSampleMCC.o : $(FONTSAMPLE_DIR)/FontSampleMCC.c \
	$(FONTSAMPLE_DIR)/FontSampleMCC.h

$(OBJDIR)/IconobjectMCC.o  : $(ICONOBJMCC_DIR)/IconobjectMCC.c \
	$(ICONOBJMCC_DIR)/IconobjectMCC.h debug.h

#############################################################

$(CATCOMPHEADER) : $(PREFSPATH)/ScalosFileTypes.cd
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
	@copy $(BINDIR)/$(PLUGIN) "$(DESTTOOL)"
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@$(foreach cat,$(CATS),copy "$(SRCCAT)/catalogs/$(cat)/$(CAT_FILE)" "$(DESTCAT)/$(cat)/Scalos" clone;)

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(BINDIR)/$(PLUGIN) $(BINDIR)/$(PLUGINDBG) $(OBJS) $(subst ../,/,$(CATCOMPHEADER)) $(ALLCATS)
	@printf '\033[0m'

#############################################################

# make all Scalos preferences .catalogs
allcatalogs:
	-@$(foreach cat,$(CATS),$(SUBDIRMAKE) $(PREFSPATH)/catalogs/$(cat)/Scalos;)


#############################################################

