.asm.o:
	a68k $(AFLAGS) $<

AFLAGS = -q -iinclude:

OBJS =	AttemptDevUnit.o AvailDevUnit.o FreeDevUnit.o Glue.o \
	LockDevUnit.o LT_Activate.o LT_Add.o LT_AddA.o \
	LT_BeginRefresh.o LT_Build.o LT_BuildA.o LT_CreateHandle.o \
	LT_CreateHandleTagList.o LT_CreateHandleTags.o LT_DeleteHandle.o LT_DeleteWindowLock.o \
	LT_DisposeMenu.o LT_EndGroup.o LT_EndRefresh.o LT_FindMenuCommand.o \
	LT_Fixed2String.o LT_FixedMult.o LT_GetAttributes.o LT_GetAttributesA.o \
	LT_GetCode.o LT_GetIMsg.o LT_GetMenuItem.o LT_HandleInput.o \
	LT_LabelChars.o LT_LabelWidth.o LT_Layout.o LT_LayoutA.o \
	LT_LayoutMenus.o LT_LayoutMenusA.o LT_LevelWidth.o LT_LockWindow.o \
	LT_MenuControlTagList.o LT_MenuControlTags.o LT_New.o LT_NewA.o \
	LT_NewLevelWidth.o LT_NewMenuTagList.o LT_NewMenuTags.o LT_NewMenuTemplate.o \
	LT_PressButton.o LT_Rebuild.o LT_RebuildTagList.o LT_RebuildTags.o \
	LT_Refresh.o LT_ReplyIMsg.o LT_SetAttributes.o LT_SetAttributesA.o \
	LT_ShowWindow.o LT_String2Fixed.o LT_UnlockWindow.o LT_UpdateStrings.o LVO.o \
	NameDevUnit.o QuickSort.o StackCall.o XEmulatorCleanup.o \
	XEmulatorClearConsole.o XEmulatorCloseConsole.o XEmulatorGetFreeMacroKeys.o XEmulatorHostMon.o \
	XEmulatorInfo.o XEmulatorMacroKeyFilter.o XEmulatorOpenConsole.o XEmulatorOptions.o \
	XEmulatorPreferences.o XEmulatorResetCharset.o XEmulatorResetConsole.o XEmulatorResetTextStyles.o \
	XEmulatorSetup.o XEmulatorSignal.o XEmulatorUserMon.o XEmulatorWrite.o \
	XProtocolCleanup.o XProtocolHostMon.o XProtocolReceive.o XProtocolSend.o \
	XProtocolSetup.o XProtocolUserMon.o

all: glue.lib

glue.lib: $(OBJS)
	join $(OBJS) to $@

clean:
	delete quiet $(OBJS) glue.lib
