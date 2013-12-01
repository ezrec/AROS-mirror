# makefile for Scalos +jl+
# $Date$
# $Revision$
# using SAS/C

#############################################################

SUBDIRMAKE  = $(MAKE) -s -C 

#############################################################

.PHONY: clean install nodebug

#############################################################

SVNVERSION	= $(shell svnversion -n)
CATCOMP 	= CatComp
FLEXCAT		= FlexCat
CHEADERS 	= scalos_structures.h variables.h functions.h debug.h Semaphores.h wrappers.h \
		  $(INT64_DIR)/int64.h ../include/scalos/scalos.h ../include/scalosdebug.h
LOCALE_HEADERS 	= locale.h $(CATHEADER) # Scalos_Locale.h 

CC      	= sc
CFLAGS  	= nostkchk nochkabort strcons strmer opt dbg=f nover streq data=far \
		  idir=/include \
		  idir=$(subst ../,/,$(MCPGFX_DIR)) \
		  idir=$(subst ../,/,$(INT64_DIR)) \
		  idir=$(subst ../,/,$(FS_DIR)) \
		  idir=$(subst ../,/,$(MALLOC_DIR)) \
                  idlen=64 \
                  define SVN_VERSION=$(SVNVERSION) \
                  define SCALOSLOCALE=$(SCALOS_LOCALE) \
		  ERROR=87,88 IGNORE=73,306,316 
#define DEBUG_SEMAPHORES=1
SPLAT		= sc:c/splat
AS      	= phxass
AFLAGS  	= quiet m=68020 linedebug opt=NRQB i=sc:Assembler_Headers/
LD      	= slink
LDFLAGS 	= NOICONS BATCH QUIET
LDLIBS  	= LIB:scmnb.lib \
		/SAS-lib/png.lib \
		/SAS-lib/zf.lib \
		/SAS-lib/snprintf.lib \
		LIB:scnb.lib \
                LIB:mempools.lib \
		LIB:debug.lib \
                LIB:amiga.lib
ECHO		= echo
CSTARTUP 	= .startup/cbacknb.o
OBJDIR	       	= .sasobj
MCPGFX_DIR	= ../common/McpGfx
INT64_DIR	= ../common/Int64
FS_DIR		= ../common/Fs
MALLOC_DIR	= ../common/malloc

SCALOS_LOCALE	= $(OBJDIR)/Scalos_Locale.h

#############################################################

$(OBJDIR)::
	@[ -d $(OBJDIR) ] || mkdir $(OBJDIR) > /dev/null 2>&1

#############################################################

APP		= .bin_os3/Scalos
APP_DBG		= .bin_os3/Scalos.debug
CATHEADER	= $(SCALOS_LOCALE)
MSG_NAMES 	= MsgIdNames.h
DESTCAT		= Locale:Catalogs
CAT_FILE	= Scalos/Scalos.catalog

CATS		= dansk \
		  deutsch \
		  español \
		  français \
		  Polski \
		  ÃeÓtina

ALLCATS 	= $(foreach cat,$(CATS),catalogs/$(cat)/$(CAT_FILE))

#############################################################

# The default target (all) and what to really build
all: 	$(APP) \
	$(APP_DBG) \
	allcatalogs
	@$(SUBDIRMAKE) Tools

################################################################################

# make all Scalos .catalogs
allcatalogs:
	-@$(foreach cat,$(CATS),$(SUBDIRMAKE) catalogs/$(cat)/Scalos;)

#############################################################

SRCS  =	about.c			\
	AppMenu.c		\
	AutoUpdate.c		\
	Backdrop.c		\
	BackFill.c		\
        BTree.c			\
	ButtonGadgetClass.c	\
	ChildProcess.c		\
	Class.c			\
	cleanup.c		\
	CLIStart.c		\
        ControlBar.c		\
	crc32.c			\
        CycleGadgetClass.c	\
	DefIcons.c		\
	DeviceWindowClass.c	\
	DevListClass.c		\
        $(INT64_DIR)/Dos64.c	\
	DoubleClick.c		\
	DragDrop.c		\
	DragDropBobs.c		\
	DrawIcon.c		\
	DropMarks.c		\
        dtimage.c		\
        DtImageClass.c		\
	FileCommands.c		\
	FileTransClass.c	\
	FileTypes.c		\
	FontUtil.c		\
        FrameImageClass.c	\
	$(FS_DIR)/FsAbstraction.c \
	Functions.c		\
	GadgetBarClass.c	\
	GadgetBarImageClass.c	\
	GadgetBarTextClass.c	\
	GaugeGadgetClass.c	\
        HistoryGadgetClass.c	\
	IconifyClass.c		\
	IconImageClass.c	\
	IconWindowClass.c	\
	IconWindow_MouseMove.c	\
	idcmp.c			\
	InputHandler.c		\
	$(INT64_DIR)/int64.c	\
	Lasso.c			\
	LocaleStrings.c		\
	MainWait.c		\
	$(MALLOC_DIR)/malloc.c	\
	Memory.c		\
	Menu.c			\
	MenuCommand.c		\
	Messages.c		\
	MouseIcon.c		\
	OpenDrawerByName.c	\
	Patches.c		\
	Patches-classic.c       \
	Pattern.c		\
	PopupMenus.c		\
	PopOpenWindows.c	\
	Prefs.c			\
	Rename.c		\
	Request.c		\
	RootClass.c		\
	ScaLibrary.c		\
	ScaLibrary-classic.c	\
	scalos.c		\
	ScalosInit.c		\
	ScalosLocale.c		\
	ScalosVersion.c	        \
	Scalos_Cx.c		\
	$(MCPGFX_DIR)/ScalosMcpGfx.c	\
	ScanDir.c		\
	ScanDirText.c		\
	Semaphores.c		\
        SeparatorGadgetClass.c	\
	Shortcuts.c		\
	Splash.c		\
        StatusBar.c		\
	StringGadgetClass.c	\
	Subroutines.c		\
	TextIconClass.c		\
	TextIconHighlightClass.c	\
	TextWindowClass.c	\
	TitleClass.c		\
        ThumbnailCache.c	\
	Thumbnails.c		\
	ToolTip.c		\
	ToolTypes.c		\
	TTLayout.c		\
	Undo.c			\
	wbl.c			\
	WBStartup.c		\
	Window.c		\
	WindowClass.c		\
	windowtask.c		\

#############################################################

define SRCS_LIST_TO_OBJS
	$(addprefix $(OBJDIR)/, $(addsuffix .o, $(foreach file, $(SRCS), \
	$(basename $(notdir $(file))))))
endef
OBJS = $(SRCS_LIST_TO_OBJS)

#############################################################

# CLI command used when linking the final executable
$(APP) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(CSTARTUP) $(OBJS) LIB $(LDLIBS) TO $@ $(LDFLAGS) STRIPDEBUG


# new 20010410 : build separate executable "Scalos.debug" with all debug information
$(APP_DBG) : $(OBJS)
	@printf '\033[32mLink: \033[31m\033[1m$@\033[0m\n'
	@$(LD) FROM $(CSTARTUP) $(OBJS) LIB $(LDLIBS) TO $@ $(LDFLAGS) ADDSYM

#############################################################

# CLI command used when compiling. This is generic and will be called for all the
# dependancies (C source files) that need compiling.
$(OBJDIR)/%.o : %.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $*.c objectname $@

$(OBJDIR)/ScalosMcpGfx.o: $(MCPGFX_DIR)/ScalosMcpGfx.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/int64.o: $(INT64_DIR)/int64.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/Dos64.o: $(INT64_DIR)/Dos64.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/FsAbstraction.o: $(FS_DIR)/FsAbstraction.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

$(OBJDIR)/malloc.o: $(MALLOC_DIR)/malloc.c
	@printf '\033[32mCompile: \033[31m\033[1m$<\033[0m\n'
	@$(CC) $(CFLAGS) $(subst ../,/,$<) objectname $@

# commands generated a catalog (.catalog) from a catalog translation (.ct) file.
.ct.catalog:
	$(CATCOMP) scalos.cd $< CATALOG $*.catalog VB=1

# Commands for assembling. Called for all the targets which have an assembly language (.s)
# file as the main source file.
$(OBJDIR)/%.o : %.s
	@printf '\033[32mAssemble: \033[31m\033[1m$<\033[0m\n'
	@$(AS) $(AFLAGS) $<

#############################################################

# A list of all project dependancies. Since the list of header files is so small, they
# are just put in after the source files instead of defining another variable to hold them
# all.

scalos_structures.h 			: BTree.h Semaphores.h

$(OBJDIR)/about.o 			: about.c about.h DtImageClass.h TTLayout.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/AppMenu.o			: AppMenu.c $(CHEADERS)
$(OBJDIR)/argb.o			: argb.c $(CHEADERS)
$(OBJDIR)/AutoUpdate.o			: AutoUpdate.c $(CHEADERS)
$(OBJDIR)/Backdrop.o			: Backdrop.c $(CHEADERS)
$(OBJDIR)/BackFill.o			: BackFill.c $(CHEADERS)
$(OBJDIR)/BTree.o			: BTree.c BTree.h $(CHEADERS)
$(OBJDIR)/ButtonGadgetClass.o		: ButtonGadgetClass.c $(CHEADERS)
$(OBJDIR)/ChildProcess.o		: ChildProcess.c $(CHEADERS)
$(OBJDIR)/Class.o			: Class.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/cleanup.o			: cleanup.c $(CHEADERS)
$(OBJDIR)/CLIStart.o			: CLIStart.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/ControlBar.o			: ControlBar.c $(CHEADERS) $(LOCALE_HEADERS) DtImageClass.h HistoryGadgetClass.h CycleGadgetClass.h FrameImageClass.h
$(OBJDIR)/crc32.o			: crc32.c $(CHEADERS)
$(OBJDIR)/CycleGadgetClass.o		: CycleGadgetClass.c $(CHEADERS) DtImageClass.h CycleGadgetClass.h FrameImageClass.h
$(OBJDIR)/DefIcons.o			: DefIcons.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/DeviceWindowClass.o		: DeviceWindowClass.c ../include/DefIcons.h $(LOCALE_HEADERS) $(CHEADERS)
$(OBJDIR)/DevListClass.o		: DevListClass.c $(CHEADERS)
$(OBJDIR)/Dos64.o			: $(INT64_DIR)/Dos64.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/DoubleClick.o			: DoubleClick.c ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/DragDrop.o			: DragDrop.c DtImageClass.h ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/DragDropBobs.o		: DragDropBobs.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/DropMarks.o			: DropMarks.c $(CHEADERS)
$(OBJDIR)/DrawIcon.o			: DrawIcon.c $(CHEADERS)
$(OBJDIR)/dtimage.o 			: dtimage.c $(CHEADERS)
$(OBJDIR)/DtImageClass.o 		: DtImageClass.c DtImageClass.h $(CHEADERS)
$(OBJDIR)/FileCommands.o		: FileCommands.c $(CHEADERS) FileTransClass.h $(LOCALE_HEADERS)
$(OBJDIR)/FileTransClass.o		: FileTransClass.c $(CHEADERS) FileTransClass.h $(LOCALE_HEADERS)
$(OBJDIR)/FileTypes.o			: FileTypes.c TTLayout.h MsgIdNames.h PenNames.h DtImageClass.h ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/FontUtil.o			: FontUtil.c $(CHEADERS)
$(OBJDIR)/FrameImageClass.o		: FrameImageClass.c $(CHEADERS) FrameImageClass.h
$(OBJDIR)/FsAbstraction.o		: $(FS_DIR)/FsAbstraction.c $(CHEADERS)
$(OBJDIR)/Functions.o			: Functions.c $(CHEADERS)
$(OBJDIR)/GadgetBarClass.o		: GadgetBarClass.c $(CHEADERS)
$(OBJDIR)/GadgetBarImageClass.o		: GadgetBarImageClass.c $(CHEADERS)
$(OBJDIR)/GadgetBarTextClass.o		: GadgetBarTextClass.c $(CHEADERS)
$(OBJDIR)/GaugeGadgetClass.o		: GaugeGadgetClass.c $(CHEADERS) GaugeGadgetClass.h
$(OBJDIR)/HistoryGadgetClass.o		: HistoryGadgetClass.c $(CHEADERS) DtImageClass.h HistoryGadgetClass.h
$(OBJDIR)/IconifyClass.o		: IconifyClass.c $(CHEADERS)
$(OBJDIR)/IconImageClass.o		: IconImageClass.c $(CHEADERS) IconImageClass.h
$(OBJDIR)/IconWindowClass.o		: IconWindowClass.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/IconWindow_MouseMove.o	: IconWindow_MouseMove.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/idcmp.o			: idcmp.c CycleGadgetClass.h IconImageClass.h $(CHEADERS)
$(OBJDIR)/InputHandler.o		: InputHandler.c $(CHEADERS)
$(OBJDIR)/int64.o			: $(INT64_DIR)/int64.c $(CHEADERS)
$(OBJDIR)/ScalosMcpGfx.o		: $(MCPGFX_DIR)/ScalosMcpGfx.c $(CHEADERS)
$(OBJDIR)/Lasso.o			: Lasso.c $(CHEADERS)
$(OBJDIR)/LocaleStrings.o		: LocaleStrings.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/MainWait.o			: MainWait.c $(CHEADERS)
$(OBJDIR)/malloc.o			: $(MALLOC_DIR)/malloc.c $(CHEADERS)
$(OBJDIR)/Memory.o			: Memory.c $(CHEADERS)
$(OBJDIR)/Menu.o			: Menu.c DtImageClass.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/MenuCommand.o			: MenuCommand.c ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/Messages.o			: Messages.c ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/MouseIcon.o			: MouseIcon.c $(CHEADERS)
$(OBJDIR)/OpenDrawerByName.o		: OpenDrawerByName.c $(CHEADERS)
$(OBJDIR)/Patches.o			: Patches.c Patches.h $(CHEADERS)
$(OBJDIR)/Patches-classic.o		: Patches-classic.c Patches.h $(CHEADERS)
$(OBJDIR)/Pattern.o			: Pattern.c $(CHEADERS)
$(OBJDIR)/PopOpenWindows.o		: PopOpenWindows.c $(CHEADERS)
$(OBJDIR)/PopupMenus.o			: PopupMenus.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/Prefs.o			: Prefs.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/Rename.o			: Rename.c $(CHEADERS)
$(OBJDIR)/Request.o			: Request.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/RootClass.o			: RootClass.c $(CHEADERS)
$(OBJDIR)/ScaLibrary.o			: ScaLibrary.c Scalos_rev.h ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/ScaLibrary-classic.o		: ScaLibrary-classic.c Scalos_rev.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/scalos.o			: scalos.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/ScalosInit.o			: ScalosInit.c Scalos_rev.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/ScalosLocale.o		: ScalosLocale.c $(CHEADERS)
$(OBJDIR)/ScalosVersion.o		: ScalosVersion.c Scalos_rev.h FORCE
$(OBJDIR)/Scalos_Cx.o			: Scalos_Cx.c $(CHEADERS)
$(OBJDIR)/ScanDir.o			: ScanDir.c ../include/DefIcons.h $(LOCALE_HEADERS) $(CHEADERS)
$(OBJDIR)/ScanDirText.o			: ScanDirText.c ../include/DefIcons.h $(LOCALE_HEADERS) $(CHEADERS)
$(OBJDIR)/Semaphores.o			: Semaphores.c $(CHEADERS)
$(OBJDIR)/SeparatorGadgetClass.o	: SeparatorGadgetClass.c $(CHEADERS)
$(OBJDIR)/Shortcuts.o			: Shortcuts.c $(CHEADERS)
$(OBJDIR)/Splash.o			: Splash.c TTLayout.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/StatusBar.o			: StatusBar.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/StringGadgetClass.o		: StringGadgetClass.c $(CHEADERS)
$(OBJDIR)/Subroutines.o			: Subroutines.c $(CHEADERS)
$(OBJDIR)/TextIconClass.o		: TextIconClass.c TextIconHighlightClass.h ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/TextIconHighlightClass.o	: TextIconHighlightClass.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/TextWindowClass.o		: TextWindowClass.c ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/TitleClass.o			: TitleClass.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/ThumbnailCache.o		: ThumbnailCache.c $(CHEADERS)
$(OBJDIR)/Thumbnails.o			: Thumbnails.c $(CHEADERS)
$(OBJDIR)/ToolTip.o			: ToolTip.c TTLayout.h ../include/DefIcons.h $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/ToolTypes.o			: ToolTypes.c $(CHEADERS)
$(OBJDIR)/TTLayout.o			: TTLayout.c TTLayout.h ../include/DefIcons.h $(CHEADERS)
$(OBJDIR)/Undo.o			: Undo.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/wbl.o				: wbl.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/WBStartup.o			: WBStartup.c $(CHEADERS) $(LOCALE_HEADERS)
$(OBJDIR)/Window.o			: Window.c $(CHEADERS) IconImageClass.h
$(OBJDIR)/WindowClass.o			: WindowClass.c $(CHEADERS)
$(OBJDIR)/windowtask.o			: windowtask.c $(CHEADERS) $(LOCALE_HEADERS)

FORCE:

#############################################################

# generate locale include files
$(CATHEADER) : Scalos.cd
	@printf '\033[32mCatComp header: \033[31m\033[1m$@ \033[32mfrom \033[31m$<\033[0m\n'
	@$(FLEXCAT) $< $@=$(TOPLEVEL)/CatComp_h.sd


$(MSG_NAMES) : Scalos.cd
	@printf '\033[32mGenMsgIdNames: \033[31m\033[1m$@ \033[32mfrom \033[31m$<\033[0m\n'
	@$(FLEXCAT) $< $@=$(TOPLEVEL)/msgidnames.sd

#############################################################

# install Scalos executable in Scalos:
install:
	@printf '\033[32mInstall: \033[31m\033[1m$(APP)\033[0m\n'
	@copy $(APP) Scalos: clone
	@printf '\033[32mInstall: \033[31m\033[1m$(CAT_FILE)\033[0m\n'
	-@$(foreach cat,$(CATS),copy "catalogs/$(cat)/$(CAT_FILE)" "$(DESTCAT)/$(cat)/Scalos" clone;)

#############################################################

# A little something to clean it all up
clean:
	@printf '\033[32mCleanup: \033[31m\033[1m'
	@delete $(OBJS) $(APP) $(APP_DBG) $(MSG_NAMES) $(ALLCATS) $(CATHEADER) $(MSG_NAMES)
	@printf '\033[0m'

#############################################################

nodebug:
	-@$(SPLAT) -s -o "d2(" "d1(" "#?.c"

#############################################################
