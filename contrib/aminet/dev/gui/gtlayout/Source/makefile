TOP = ../../../../../..

include $(TOP)/config/make.cfg

LOCALINCLUDES = -I. -I../Include

DEFINES = -D_AROS
OPTIMIZE = -O2

aros/LT_Activate_aros.c : LT_Activate.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_Build_aros.c : LT_Build.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_CreateHandle_aros.c : LT_CreateHandle.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_DeleteHandle_aros.c : LT_DeleteHandle.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_DisposeMenu_aros.c : LT_DisposeMenu.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_FindMenuCommand_aros.c : LT_FindMenuCommand.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_GetAttributes_aros.c : LT_GetAttributes.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_GetCode_aros.c : LT_GetCode.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_GetMenuItem_aros.c : LT_GetMenuItem.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_HandleInput_aros.c : LT_HandleInput.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_IMsg_aros.c : LT_IMsg.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_LabelWidth_aros.c : LT_LabelWidth.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_LayoutMenus_aros.c : LT_LayoutMenus.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_LevelWidth_aros.c : LT_LevelWidth.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_LockWindow_aros.c : LT_LockWindow.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_MenuControlTagList_aros.c : LT_MenuControlTagList.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_New_aros.c : LT_New.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_NewMenuTagList_aros.c : LT_NewMenuTagList.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_NewMenuTemplate_aros.c : LT_NewMenuTemplate.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_PressButton_aros.c : LT_PressButton.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_Rebuild_aros.c : LT_Rebuild.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_Refresh_aros.c : LT_Refresh.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_SetAttributes_aros.c : LT_SetAttributes.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_ShowWindow_aros.c : LT_ShowWindow.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

aros/LT_UpdateStrings_aros.c : LT_UpdateStrings.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h


LT_Activate.o : aros/LT_Activate_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_Build.o : aros/LT_Build_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_CreateHandle.o : aros/LT_CreateHandle_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_DeleteHandle.o : aros/LT_DeleteHandle_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_DisposeMenu.o : aros/LT_DisposeMenu_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_FindMenuCommand.o : aros/LT_FindMenuCommand_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_GetAttributes.o : aros/LT_GetAttributes_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_GetCode.o : aros/LT_GetCode_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_GetMenuItem.o : aros/LT_GetMenuItem_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_HandleInput.o : aros/LT_HandleInput_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_IMsg.o : aros/LT_IMsg_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_LabelWidth.o : aros/LT_LabelWidth_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_LayoutMenus.o : aros/LT_LayoutMenus_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_LevelWidth.o : aros/LT_LevelWidth_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_LockWindow.o : aros/LT_LockWindow_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_MenuControlTagList.o : aros/LT_MenuControlTagList_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_New.o : aros/LT_New_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_NewMenuTagList.o : aros/LT_NewMenuTagList_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_NewMenuTemplate.o : aros/LT_NewMenuTemplate_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_PressButton.o : aros/LT_PressButton_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_Rebuild.o : aros/LT_Rebuild_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_Refresh.o : aros/LT_Refresh_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_SetAttributes.o : aros/LT_SetAttributes_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_ShowWindow.o : aros/LT_ShowWindow_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

LT_UpdateStrings.o : aros/LT_UpdateStrings_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

API_SOURCE = LT_Activate LT_Build LT_CreateHandle LT_DeleteHandle \
	LT_DisposeMenu LT_FindMenuCommand LT_GetAttributes \
	LT_GetCode LT_GetMenuItem LT_HandleInput LT_IMsg \
	LT_LabelWidth LT_LayoutMenus LT_LevelWidth \
	LT_LockWindow LT_MenuControlTagList LT_New LT_NewMenuTagList \
	LT_NewMenuTemplate LT_PressButton LT_Rebuild LT_Refresh \
	LT_SetAttributes LT_ShowWindow LT_UpdateStrings


aros/LTP_ImageClass_aros.c : LTP_ImageClass.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

LTP_ImageClass.o : aros/LTP_ImageClass_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

aros/LTP_PopupClass_aros.c : LTP_PopupClass.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

LTP_PopupClass.o : aros/LTP_PopupClass_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

aros/LTP_HexEditRoutine_aros.c : LTP_HexEditRoutine.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

LTP_HexEditRoutine.o : aros/LTP_HexEditRoutine_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

aros/LTP_DefaultHistoryHook_aros.c : LTP_DefaultHistoryHook.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

LTP_DefaultHistoryHook.o : aros/LTP_DefaultHistoryHook_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

aros/LTP_DefaultEditRoutine_aros.c : LTP_DefaultEditRoutine.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

LTP_DefaultEditRoutine.o : aros/LTP_DefaultEditRoutine_aros.c
	$(CC)  $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

aros/LTP_BackFillRoutine_aros.c : LTP_BackFillRoutine.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

LTP_BackFillRoutine.o : aros/LTP_BackFillRoutine_aros.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h



UFC_SOURCE = LTP_ImageClass LTP_PopupClass LTP_HexEditRoutine \
	LTP_DefaultHistoryHook LTP_DefaultEditRoutine LTP_BackFillRoutine

aros/gtlayout_lib_aros.c : gtlayout_lib.c
	$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h

gtlayout_lib.o : aros/gtlayout_lib_aros.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

OTHERS = gtlayout_lib


INTERMEDIATES = $(foreach f,$(API_SOURCE) $(UFC_SOURCE),aros/$(f)_aros.c)

%.o : %.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@ -include aros/arosspecific.h

FILES =		LTP_AddGadgets LTP_AddHistory LTP_AdjustItemPosition \
		LTP_AdjustMenuPosition LTP_Atol \
		LTP_BitMap LTP_BlinkButton LTP_CheckGlyph \
		LTP_Clone LTP_ConvertNum LTP_CorrectItemList \
		LTP_CreateExtraObject LTP_CreateGadgets LTP_CreateMenuTagList \
		LTP_CreateMenuTemplate LTP_CreateObjectNode \
		LTP_Delay LTP_DeleteObjectNode \
		LTP_DetermineSize LTP_Draw LTP_DrawBackFore \
		LTP_DrawBox LTP_DrawGauge LTP_DrawGroup \
		LTP_DrawIncrementer LTP_DrawPalette LTP_DrawPicker \
		LTP_DrawPrevNext LTP_DrawTapeButton LTP_FillMenu \
		LTP_Find LTP_FixExtraLabel  \
		LTP_GetCommandWidth LTP_GetDisplayClip LTP_GlyphSetup \
		LTP_HandleHistory \
		LTP_InitIText LTP_LayoutGadgets LTP_LayoutGroup \
		LTP_LayoutMenu LTP_LevelClass \
		LTP_MakeItem LTP_MakeMenu LTP_Memory \
		LTP_MoveToWindow LTP_NewMenu LTP_PasswordEditRoutine \
		LTP_PlaceGroups LTP_PrintBoxLine \
		LTP_PrintLabel LTP_RenderArrow LTP_RenderCircle \
		LTP_Rescale LTP_ResetGroups LTP_RPortAttrs \
		LTP_SearchKeys LTP_SelectKeys LTP_ShrinkMenu \
		LTP_SizeDimensions LTP_Spread LTP_SPrintf \
		LTP_Storage LTP_TabClass LT_InitExit

OBJS = $(foreach f,$(API_SOURCE) $(UFC_SOURCE) $(FILES),$(f).o)

#First all intermediate files MUST be create so there is a 'defines' file!
all: gtlayout.library

gtlayout.library : $(INTERMEDIATES) $(OBJS) gtlayout_lib.o gtlayout_data.o
	$(CC) -nostartfiles -nostdlib -Wl,-i \
		gtlayout_lib.o gtlayout_data.o $(OBJS) \
		-static -i -L$(LIBDIR) -lamiga -larosc -o $@

clean:
	$(RM) $(INTERMEDIATES) $(OBJS) gtlayout.library aros/gtlayout_defines_aros.h
