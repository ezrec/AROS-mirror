TOP = ../../../../../..

include $(TOP)/config/make.cfg

LOCALINCLUDES = -I. -I../Include

DEFINES = -D_AROS
OPTIMIZE = -O2

aros/%_aros.c : %.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

%.o : aros/%_aros.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

test/%.o : test/%.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(STARTUP) $(LOCALINCLUDES) -c $^ -o $@

API_SOURCE = \
	LT_Activate \
	LT_Build \
	LT_CreateHandle \
	LT_DeleteHandle \
	LT_DisposeMenu \
	LT_FindMenuCommand \
	LT_GetAttributes \
	LT_GetCode \
	LT_GetMenuItem \
	LT_HandleInput \
	LT_IMsg \
	LT_LabelWidth \
	LT_LayoutMenus \
	LT_LevelWidth \
	LT_LockWindow \
	LT_MenuControlTagList \
	LT_New \
	LT_NewMenuTagList \
	LT_NewMenuTemplate \
	LT_PressButton \
	LT_Rebuild \
	LT_Refresh \
	LT_SetAttributes \
	LT_ShowWindow \
	LT_UpdateStrings

UFC_SOURCE = \
	LTP_ImageClass \
	LTP_PopupClass \
	LTP_HexEditRoutine \
	LTP_DefaultHistoryHook \
	LTP_DefaultEditRoutine \
	LTP_BackFillRoutine

OTHERS = gtlayout_lib

INTERMEDIATES = $(foreach f,$(API_SOURCE) $(UFC_SOURCE),aros/$(f)_aros.c)

FILES = \
	LTP_AddGadgets \
	LTP_AddHistory \
	LTP_AdjustItemPosition \
	LTP_AdjustMenuPosition \
	LTP_Atol \
	LTP_BitMap \
	LTP_BlinkButton \
	LTP_CheckGlyph \
	LTP_Clone \
	LTP_ConvertNum \
	LTP_CorrectItemList \
	LTP_CreateExtraObject \
	LTP_CreateGadgets \
	LTP_CreateMenuTagList \
	LTP_CreateMenuTemplate \
	LTP_CreateObjectNode \
	LTP_Delay \
	LTP_DeleteObjectNode \
	LTP_DetermineSize \
	LTP_Draw \
	LTP_DrawBackFore \
	LTP_DrawBox \
	LTP_DrawGauge \
	LTP_DrawGroup \
	LTP_DrawIncrementer \
	LTP_DrawPalette \
	LTP_DrawPicker \
	LTP_DrawPrevNext \
	LTP_DrawTapeButton \
	LTP_FillMenu \
	LTP_Find \
	LTP_FixExtraLabel \
	LTP_GetCommandWidth \
	LTP_GetDisplayClip \
	LTP_GlyphSetup \
	LTP_HandleHistory \
	LTP_InitIText \
	LTP_LayoutGadgets \
	LTP_LayoutGroup \
	LTP_LayoutMenu \
	LTP_LevelClass \
	LTP_MakeItem \
	LTP_MakeMenu \
	LTP_Memory \
	LTP_MoveToWindow \
	LTP_NewMenu \
	LTP_PasswordEditRoutine \
	LTP_PlaceGroups \
	LTP_PrintBoxLine \
	LTP_PrintLabel \
	LTP_RenderArrow \
	LTP_RenderCircle \
	LTP_Rescale \
	LTP_ResetGroups \
	LTP_RPortAttrs \
	LTP_SearchKeys \
	LTP_SelectKeys \
	LTP_ShrinkMenu \
	LTP_SizeDimensions \
	LTP_Spread \
	LTP_SPrintf \
	LTP_Storage \
	LTP_TabClass \
	LT_InitExit

TESTFILES = test/gui test/Tools

OBJS = $(foreach f,$(API_SOURCE) $(UFC_SOURCE) $(FILES),$(f).o)
TESTOBJS = test/gui.o test/Tools.o

#First all intermediate files MUST be created so there is a 'defines' file!
all: gtlayout.library test_aros

gtlayout.library : $(INTERMEDIATES) $(OBJS) gtlayout_lib.o gtlayout_data.o
	$(CC) -nostartfiles -nostdlib -Wl,-i \
		gtlayout_lib.o gtlayout_data.o $(OBJS) \
		-static -i -L$(LIBDIR) -lamiga -larosc -o $@

test_aros: $(TESTOBJS)
	$(CC) -nostartfiles -nostdlib -Wl,-r\
		-static $(TESTOBJS) -L$(LIBDIR) -lamiga -larosc -o $@
 
clean:
	$(RM) $(INTERMEDIATES) $(OBJS) gtlayout.library aros/gtlayout_defines_aros.h

