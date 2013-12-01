# MakeFile for Format_Disk.module (Gadtools) module.
# $Date$
# $Revision$
#############################################################

.PHONY: dirs clean install nodebug

#############################################################

TOPLEVEL	= /
SUBDIRMAKE 	= $(MAKE) -s -C 
CATCOMP 	= CatComp
FLEXCAT		= FlexCat
CC 		= sc
SPLAT 		= sc:c/splat

CFLAGS		= optimize nostackcheck nochkabort debug=s idlen=128 \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  strmer nover streq \
		  idir=sc:include/ \
		  idir=include: \
		  idir=//include

LIBS	 	= lib:sc.lib LIB:debug.lib lib:amiga.lib
CSTARTUP 	= LIB:c.o

LINK		= SLINK
LFLAGS		= SC SD
LNFLAGS		= quiet batch noicons stripdebug
LNDBFLAGS	= quiet batch noicons addsym

OBJDIR		= .sasobj
MKDIR		= mkdir -p
DESTTOOL	= Scalos:modules/

SCALOS_LOCALE	= $(OBJDIR)/FormatDisk_Locale.h

#############################################################

.SUFFIXES: .asm .cd

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#############################################################

NAME    	= .bin_os3/Format_Disk.module
DBGNAME 	= $(NAME).debug
CATCOMPHEADER	= $(SCALOS_LOCALE)
CAT_FILE	= Scalos/FormatDisk.catalog
DESTCAT		= Locale:Catalogs
ALLCATS 	= Catalogs/français/$(CAT_FILE) \
		  Catalogs/deutsch/$(CAT_FILE)


#############################################################

all: 	dirs \
	$(NAME) \
	$(DBGNAME) \
	allcatalogs

#############################################################

# make all Scalos .catalogs
allcatalogs:
	@$(SUBDIRMAKE) Catalogs/français/Scalos
	@$(SUBDIRMAKE) Catalogs/deutsch/Scalos

#############################################################

SRCS =	NSD_64bit.c			\
	Device_Handler.c 		\
	FullFormat.c			\
	FormatVolume.c			\
	ParseArgs.c			\
	Format.c			\
	GetInputFromWindow.c		\
	GUI.c				\

#############################################################

define SRCS_LIST_TO_OBJS
	$(addprefix $(OBJDIR)/, $(addsuffix .o, $(foreach file, $(SRCS), \
	$(basename $(notdir $(file))))))
endef
OBJS = $(SRCS_LIST_TO_OBJS)

#############################################################

$(CATCOMPHEADER) : FormatDisk.cd
	@printf '\033[32mMake Catcomp header: \033[31m\033[1m$@ \033[32mfrom \033[31m$<\033[0m\n'
	@$(FLEXCAT) $< $@=$(TOPLEVEL)/CatComp_h.sd

#############################################################

$(NAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LINK) FROM $(CSTARTUP) $(OBJS) TO $@ LIB $(LIBS) $(LNFLAGS)

$(DBGNAME) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LINK) FROM $(CSTARTUP) $(OBJS) TO $@ LIB $(LIBS) $(LNDBFLAGS)

#############################################################

$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

#############################################################

# commands generated a catalog (.catalog) from a catalog translation (.ct) file.
.ct.catalog:
	$(CATCOMP) $*.cd $< CATALOG $*.catalog VB=1

#############################################################

$(OBJDIR)/NSD_64bit.o 		: NSD_64bit.c
$(OBJDIR)/Device_Handler.o 	: Device_Handler.c Format.h
$(OBJDIR)/FullFormat.o 		: FullFormat.c Format.h $(SCALOS_LOCALE)
$(OBJDIR)/FormatVolume.o 	: FormatVolume.c Format.h $(SCALOS_LOCALE)
$(OBJDIR)/ParseArgs.o 		: ParseArgs.c Format.h GUI.h
$(OBJDIR)/Format.o 		: Format.c Format.h $(SCALOS_LOCALE) GUI.h
$(OBJDIR)/GetInputFromWindow.o 	: GetInputFromWindow.c Format.h GUI.h
$(OBJDIR)/GUI.o 		: GUI.c $(SCALOS_LOCALE) Format.h GUI.h

#############################################################


dirs:
	@$(MKDIR) $(OBJDIR)

#############################################################

install:
	@printf '\033[32mInstall: \033[31m\033[1m$(NAME) \033[32mto \033[31m\033[1m$(DESTTOOL) \033[0m\n'
	@copy $(NAME) $(DESTTOOL) clone
	@copy $(NAME).info $(DESTTOOL) clone
	@printf '\033[32mFlushing memory\033[0m\n'
	@avail flush
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@copy 'catalogs/deutsch/$(CAT_FILE)'  	'$(DESTCAT)/Deutsch/Scalos/' clone
	-@copy 'catalogs/français/$(CAT_FILE)' 	'$(DESTCAT)/français/Scalos/' clone

#############################################################

clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	-@delete $(OBJS) $(CATCOMPHEADER) $(NAME) $(DBGNAME)
	@printf '\033[0m'

#############################################################

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################
