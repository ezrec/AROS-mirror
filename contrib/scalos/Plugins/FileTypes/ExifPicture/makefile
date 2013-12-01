# makefile für Scalos exifpicture.plugin
# $Date$
# $Revision$
##############################################################################

.SUFFIXES: .plugin .plugin.debug .cd

#############################################################

SUBDIRMAKE 	= $(MAKE) -s -C 
#SUBDIRMAKE 	= smakedir

PRECOMP 	= Include:all.gst

SDPATH		= //

CC      	= sc
CFLAGS  	= nostackcheck nochkabort strmer dbg=l nover streq data=far \
		  idlen=64 Math=s \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  idir=///include \
		  idir=///common/Plugin

#############################################################

OPT_FLG 	= OPTIMIZE OPTINLOCAL OPTTIME OPTSCHED IGNORE=306 IGNORE=308 IGNORE=304
COPTS 		= NOWVRET NOVERSION nostackcheck idlen=128 idir=///include
OPTIONS 	=  $(OPT_FLG) Math=s DATA=FAR DEBUG=L $(COPTS) def=NDEBUG

#############################################################

AS      	= phxass
AFLAGS  	= quiet noexe m=68020 opt=NRQB i=sc:Assembler_Headers/
LD      	= slink
LDFLAGS 	= quiet batch noicons sd
LDLIBS  	= LIB:scm.lib LIB:sc.lib LIB:debug.lib LIB:amiga.lib
ASTARTUP	=
#CSTARTUP 	= LIB:c.o

OBJDIR		= .sasobj
CATCOMP 	= CatComp
FLEXCAT		= FlexCat

COMMON_DIR	= ../../../common/Plugin

#############################################################

CSRCS  		= $(COMMON_DIR)/plugin-classic.c \
		  ExifPicture.c

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

XSRCS = $(notdir $(CSRCS))
OBJS = $(XSRCS:%.c=$(OBJDIR)/$(notdir %).o)

#############################################################

PLUGIN		= exifpicture.plugin
SCALOS_LOCALE	= $(OBJDIR)/ExifPicture_locale.h
CATCOMPHEADER	= $(SCALOS_LOCALE)
CAT_FILE	= Scalos/ExifPicturePlugin.catalog
DESTTOOL	= Scalos:plugins/FileTypes/
DESTCAT		= Locale:Catalogs
ALLCATS 	= Catalogs/deutsch/$(CAT_FILE) \
		  Catalogs/français/$(CAT_FILE)

#############################################################

All: 	$(PLUGIN) \
	$(PLUGIN).debug \
    	allcatalogs
# install
# clean

#############################################################

# commands generated a catalog (.catalog) from a catalog translation (.ct) file.
.ct.catalog:
	$(CATCOMP) $*.cd $< CATALOG $*.catalog VB=1

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-classic.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

#############################################################

$(OBJDIR)/plugin-classic.o : $(COMMON_DIR)/plugin-common.c \
	plugin_data.h ExifPicture_base.h $(COMMON_DIR)/plugin.h
$(OBJDIR)/ExifPicture.o : ExifPicture.h ExifPicture_base.h $(SCALOS_LOCALE)

#############################################################

$(CATCOMPHEADER) : ExifPicturePlugin.cd
	@printf '\033[32mMake CatComp header: \033[31m\033[1m$@ \033[32mfrom \033[31m$<\033[0m\n'
	$(FLEXCAT) $(subst ../,/,$<) $@=$(SDPATH)/CatComp_h.sd

$(PLUGIN) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(CSTARTUP) $(OBJS) lib $(LDLIBS) $(LDFLAGS) stripdebug

$(PLUGIN).debug : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) to $@ FROM $(CSTARTUP) $(OBJS) lib $(LDLIBS) $(LDFLAGS) addsym

#############################################################

# A little something to clean it all up
clean:
	@printf '\033[32mDelete: \033[31m\033[1m'
	@delete $(OBJS) $(PLUGIN) $(PLUGIN).debug $(ALLCATS) $(CATCOMPHEADER)
	@printf '\033[0m'

#############################################################

# copy all generated file to their final destinations
install:
	@printf '\033[32mInstall: \033[31m\033[1m$(PLUGIN)\033[0m\n'
	@copy $(PLUGIN) $(DESTTOOL) clone
	@printf '\033[32mFlushing memory\033[0m\n'
	@avail flush
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@copy 'catalogs/deutsch/$(CAT_FILE)'  	'$(DESTCAT)/Deutsch/Scalos/' clone
	-@copy 'catalogs/français/$(CAT_FILE)' 	'$(DESTCAT)/français/Scalos/' clone

#############################################################

# make all Scalos preferences .catalogs
allcatalogs:
	@$(SUBDIRMAKE) Catalogs/deutsch/Scalos
	@$(SUBDIRMAKE) Catalogs/français/Scalos
#	@$(SUBDIRMAKE) Catalogs/sample/Scalos

