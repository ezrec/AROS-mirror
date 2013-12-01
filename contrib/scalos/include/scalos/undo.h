#ifndef SCALOS_UNDO_H
#define SCALOS_UNDO_H
/*
**  $VER: undo.h 41.1 (23 Mar 2009 21:05:36)
**
**	scalosgfx.library include
**
**  (C) Copyright 2009 The Scalos Team
**  All Rights Reserved
*/

#ifndef	 EXEC_TYPES_H
#include <exec/types.h>
#endif	//EXEC_TYPES_H

#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif	//UTILITY_TAGITEM_H

//----------------------------------------------------------------------------

enum ScalosUndoType
	{
	UNDO_Snapshot,			// UNDOTAG_IconDirLock, UNDOTAG_SaveIcon, UNDOTAG_IconNode
	UNDO_Unsnapshot,                // UNDOTAG_IconDirLock, UNDOTAG_SaveIcon, UNDOTAG_IconNode
	UNDO_Cleanup,			// UNDOTAG_CleanupMode, UNDOTAG_WindowTask, UNDOTAG_IconList
	UNDO_Copy,			// UNDOTAG_CopySrcDirLock, UNDOTAG_CopyDestDirLock, UNDOTAG_CopySrcName, UNDOTAG_CopyDestName
	UNDO_Move,                      // UNDOTAG_CopySrcDirLock, UNDOTAG_CopyDestDirLock, UNDOTAG_CopySrcName, UNDOTAG_CopyDestName
	UNDO_CreateLink,                // UNDOTAG_CopySrcDirLock, UNDOTAG_CopyDestDirLock, UNDOTAG_CopySrcName, UNDOTAG_CopyDestName
	UNDO_ChangeIconPos,		// UNDOTAG_IconDirLock, UNDOTAG_IconNode,
	UNDO_Rename,			// UNDOTAG_CopySrcDirLock, UNDOTAG_CopySrcName, UNDOTAG_CopyDestName
	UNDO_Relabel,			// UNDOTAG_CopySrcName, UNDOTAG_CopyDestName
	UNDO_Delete,                    // UNDOTAG_CopySrcDirLock, UNDOTAG_CopySrcName, UNDOTAG_CopyDestName
	UNDO_Leaveout,                  // UNDOTAG_IconDirLock, UNDOTAG_IconName
	UNDO_PutAway,                   // UNDOTAG_IconDirLock, UNDOTAG_IconName
	UNDO_NewDrawer,			// UNDOTAG_IconDirLock, UNDOTAG_IconName, UNDOTAG_CreateIcon
	UNDO_SizeWindow,		// UNDOTAG_WindowTask, UNDOTAG_OldWindowLeft, UNDOTAG_OldWindowTop, UNDOTAG_OldWindowWidth, UNDOTAG_OldWindowHeight, UNDOTAG_OldWindowVirtX, UNDOTAG_OldWindowVirtY,UNDOTAG_NewWindowLeft,UNDOTAG_NewWindowTop,UNDOTAG_NewWindowWidth,UNDOTAG_NewWindowHeight,UNDOTAG_NewWindowVirtX,UNDOTAG_NewWindowVirtY
	UNDO_SetProtection,             // UNDOTAG_IconDirLock, UNDOTAG_IconName, UNDOTAG_OldProtection, UNDOTAG_NewProtection
	UNDO_SetComment,		// UNDOTAG_IconDirLock, UNDOTAG_IconName, UNDOTAG_OldComment, UNDOTAG_NewComment
	UNDO_SetToolTypes,		// UNDOTAG_IconDirLock, UNDOTAG_IconName, UNDOTAG_OldToolTypes, UNDOTAG_NewToolTypes
	UNDO_ChangeIconObject,		// UNDOTAG_IconDirLock, UNDOTAG_IconName, UNDOTAG_OldIconObject, UNDOTAG_NewIconObject
	UNDO_CloseWindow,		// UNDOTAG_WindowTask
	};

enum ScalosUndoTags
	{
	UNDOTAG_UndoMultiStep = (int) (TAG_USER + 1287),
	UNDOTAG_IconNode,		// struct ScaIconNode *
	UNDOTAG_IconList,		// struct ScaIconNode *
	UNDOTAG_IconDirLock,		// BPTR
	UNDOTAG_CopySrcDirLock,		// BPTR
	UNDOTAG_CopyDestDirLock,	// BPTR
	UNDOTAG_CopySrcName,		// CONST_STRPTR
	UNDOTAG_CopyDestName,		// CONST_STRPTR
	UNDOTAG_IconPosX,		// LONG
	UNDOTAG_IconPosY,		// LONG
	UNDOTAG_OldIconPosX,		// LONG
	UNDOTAG_OldIconPosY,		// LONG
	UNDOTAG_WindowTask,		// struct internalScaWindowTask *
	UNDOTAG_CleanupMode,		// ULONG
	UNDOTAG_UndoHook,		// struct Hook *
	UNDOTAG_RedoHook,		// struct Hook *
	UNDOTAG_DisposeHook,		// struct Hook *
	UNDOTAG_SaveIcon,		// ULONG
	UNDOTAG_CustomAddHook,		// struct Hook *
	UNDOTAG_WbArg,			// struct WBArg *
	UNDOTAG_OldWindowLeft,		// LONG
	UNDOTAG_OldWindowTop,           // LONG
	UNDOTAG_OldWindowWidth,		// ULONG
	UNDOTAG_OldWindowHeight,	// ULONG
	UNDOTAG_NewWindowLeft,		// LONG
	UNDOTAG_NewWindowTop,           // LONG
	UNDOTAG_NewWindowWidth,		// ULONG
	UNDOTAG_NewWindowHeight,	// ULONG
	UNDOTAG_OldWindowVirtX,		// LONG
	UNDOTAG_OldWindowVirtY,		// LONG
	UNDOTAG_NewWindowVirtX,		// LONG
	UNDOTAG_NewWindowVirtY,		// LONG
	UNDOTAG_CreateIcon,		// LONG
	UNDOTAG_IconName,		// CONST_STRPTR
	UNDOTAG_OldProtection,		// ULONG
	UNDOTAG_NewProtection,		// ULONG
	UNDOTAG_OldComment,		// CONST_STRPTR
	UNDOTAG_NewComment,		// CONST_STRPTR
	UNDOTAG_OldToolTypes,		// const STRPTR *
	UNDOTAG_NewToolTypes,		// const STRPTR *
	UNDOTAG_OldIconObject,		// Object *
	UNDOTAG_NewIconObject,		// Object *
	};

enum ScalosUndoCleanupMode
	{
	CLEANUP_Default,
	CLEANUP_ByName,
	CLEANUP_ByDate,
	CLEANUP_BySize,
	CLEANUP_ByType,
	};

struct UndoCleanupIconEntry
	{
	const struct ScaIconNode *ucin_IconNode;
	LONG ucin_Left;			// original x position of icon
	LONG ucin_Top;			// original y position of icon
	};

struct UndoEvent
	{
	struct Node uev_Node;
	enum ScalosUndoType uev_Type;

	struct Hook *uev_UndoHook;
	struct Hook *uev_RedoHook;
	struct Hook *uev_DisposeHook;
	struct Hook *uev_CustomAddHook;

	LONG uev_OldPosX;
	LONG uev_OldPosY;
	LONG uev_NewPosX;
	LONG uev_NewPosY;
	
	union UndoEventData
		{
		APTR uev_PrivateData;
		struct UndoCopyMoveEventData
			{
			// we do not keep Locks to src/dest directories here since it wouldn't be nice to block rename/delete of referenced objects!
			STRPTR ucmed_srcDirName;
			STRPTR ucmed_destDirName;
			STRPTR ucmed_srcName;
			STRPTR ucmed_destName;
			} uev_CopyMoveData;
		struct UndoNewDrawerData
			{
			// we do not keep Locks to src directory here since it wouldn't be nice to block rename/delete of referenced objects!
			STRPTR und_DirName;
			STRPTR und_srcName;
			BOOL und_CreateIcon;
			} uev_NewDrawerData;
		struct UndoIconEventData
			{
			// we do not keep Locks to icon directory here since it wouldn't be nice to block rename/delete of referenced objects!
			STRPTR uid_DirName;
			STRPTR uid_IconName;
			} uev_IconData;
		struct UndoCleanupData
			{
			enum ScalosUndoCleanupMode ucd_CleanupMode;
			struct UndoCleanupIconEntry *ucd_Icons;
			ULONG ucd_IconCount;		// number of entries in ucd_Icons
			struct ScaWindowTask *ucd_WindowTask;
			STRPTR ucd_WindowTitle;		// a copy of the window title of ucd_WindowTask
			} uev_CleanupData;
		struct UndoSnaphotIconData
			{
			STRPTR usid_DirName;
			STRPTR usid_IconName;
			Object *usid_IconObj;		// Iconobject BEFORE snapshot/unsnaphot
			ULONG usid_SaveIcon;
			} uev_SnapshotData;
		struct UndoSizeWindowData
			{
			struct ScaWindowTask *uswd_WindowTask;
			STRPTR uswd_WindowTitle;	// a copy of the window title of ucd_WindowTask
			LONG uswd_OldLeft;
			LONG uswd_OldTop;
			ULONG uswd_OldWidth;
			ULONG uswd_OldHeight;
			LONG uswd_NewLeft;
			LONG uswd_NewTop;
			ULONG uswd_NewWidth;
			ULONG uswd_NewHeight;
			LONG uswd_OldVirtX;
			LONG uswd_OldVirtY;
			LONG uswd_NewVirtX;
			LONG uswd_NewVirtY;
			} uev_SizeWindowData;
		struct UndoSetProtectionData
			{
			STRPTR uspd_DirName;
			STRPTR uspd_IconName;
			ULONG uspd_OldProtection;
			ULONG uspd_NewProtection;
			} uev_SetProtectionData;
		struct UndoSetCommentData
			{
			STRPTR uscd_DirName;
			STRPTR uscd_IconName;
			STRPTR uscd_OldComment;
			STRPTR uscd_NewComment;
			} uev_SetCommentData;
		struct UndoSetToolTypesData
			{
			STRPTR ustd_DirName;
			STRPTR ustd_IconName;
			STRPTR *ustd_OldToolTypes;
			STRPTR *ustd_NewToolTypes;
			} uev_SetToolTypesData;
		struct UndoChangeIconObjectData
			{
			STRPTR uciod_DirName;
			STRPTR uciod_IconName;
			Object *uciod_OldIconObject;
			Object *uciod_NewIconObject;
			} uev_ChangeIconObjectData;
		struct UndoCloseWindowData
			{
			STRPTR ucwd_DirName;
			STRPTR ucwd_WindowTitle;
			LONG ucwd_Left;
			LONG ucwd_Top;
			ULONG ucwd_Width;
			ULONG ucwd_Height;
			LONG ucwd_VirtX;
			LONG ucwd_VirtY;
			UWORD ucwd_ViewAll;
			UBYTE ucwd_Viewmodes;
			} uev_CloseWindowData;
		} uev_Data;

	struct UndoStep *uev_UndoStep;

	ULONG uev_DescrMsgIDSingle;
	ULONG uev_DescrMsgIDMultiple;
	STRPTR *uev_DescrObjName;
	};

/*----------------------------------------------------------------------------------*/

#endif /* SCALOS_UNDO_H */
