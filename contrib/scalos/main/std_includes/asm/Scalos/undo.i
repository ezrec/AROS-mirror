	IFND	SCA_UNDO_I
SCA_UNDO_I     SET     1
**
**	$VER: undo.i 41.1 (23 Oct 2009 21:31:48)
**
**	File format for scalos_pattern preferences
**
**	(C) Copyright 2009 The Scalos Team
**	All Rights Reserved
**

;---------------------------------------------------------------------------

	IFND UTILITY_TAGITEM_I
	INCLUDE "utility/tagitem.i"
	ENDC

;---------------------------------------------------------------------------

;enum ScalosUndoType

UNDO_Snapshot		equ	0
UNDO_Unsnapshot		equ	1
UNDO_Cleanup		equ	2
UNDO_Copy		equ	3
UNDO_Move		equ	4
UNDO_CreateLink		equ	5
UNDO_ChangeIconPos	equ	6
UNDO_Rename		equ	7
UNDO_Relabel		equ	8
UNDO_Delete		equ	9
UNDO_Leaveout		equ	10
UNDO_PutAway		equ	11
UNDO_NewDrawer		equ	12
UNDO_SizeWindow		equ     13
UNDO_SetProtection	equ	14
UNDO_SetComment		equ	15
UNDO_SetToolTypes	equ	16
UNDO_ChangeIconObject	equ	17
UNDO_CloseWindow	equ	18

;enum ScalosUndoTags
UNDOTAG_TagBase		equ     TAG_USER+1287

UNDOTAG_UndoMultiStep	equ     UNDOTAG_TagBase+1
UNDOTAG_IconNode	equ	UNDOTAG_TagBase+1	; struct ScaIconNode *
UNDOTAG_IconList	equ	UNDOTAG_TagBase+2	; struct ScaIconNode *
UNDOTAG_IconDirLock	equ	UNDOTAG_TagBase+3	; BPTR
UNDOTAG_CopySrcDirLock	equ	UNDOTAG_TagBase+4	; BPTR
UNDOTAG_CopyDestDirLock	equ	UNDOTAG_TagBase+5	; BPTR
UNDOTAG_CopySrcName	equ	UNDOTAG_TagBase+6	; CONST_STRPTR
UNDOTAG_CopyDestName	equ	UNDOTAG_TagBase+7	; CONST_STRPTR
UNDOTAG_IconPosX	equ	UNDOTAG_TagBase+8	; LONG
UNDOTAG_IconPosY	equ	UNDOTAG_TagBase+9	; LONG
UNDOTAG_OldIconPosX	equ	UNDOTAG_TagBase+10	; LONG
UNDOTAG_OldIconPosY	equ	UNDOTAG_TagBase+11	; LONG
UNDOTAG_WindowTask	equ	UNDOTAG_TagBase+12	; struct internalScaWindowTask *
UNDOTAG_CleanupMode	equ	UNDOTAG_TagBase+13	; ULONG
UNDOTAG_UndoHook	equ	UNDOTAG_TagBase+14	; struct Hook *
UNDOTAG_RedoHook	equ	UNDOTAG_TagBase+15	; struct Hook *
UNDOTAG_DisposeHook	equ	UNDOTAG_TagBase+16	; struct Hook *
UNDOTAG_SaveIcon	equ	UNDOTAG_TagBase+17	; ULONG
UNDOTAG_CustomAddHook	equ	UNDOTAG_TagBase+18	; struct Hook *
UNDOTAG_WbArg		equ	UNDOTAG_TagBase+19	; struct WBArg *
UNDOTAG_OldWindowLeft	equ	UNDOTAG_TagBase+20	; LONG
UNDOTAG_OldWindowTop	equ	UNDOTAG_TagBase+21	; LONG
UNDOTAG_OldWindowWidth	equ	UNDOTAG_TagBase+22	; ULONG
UNDOTAG_OldWindowHeight	equ	UNDOTAG_TagBase+23	; ULONG
UNDOTAG_NewWindowLeft	equ	UNDOTAG_TagBase+24	; LONG
UNDOTAG_NewWindowTop    equ     UNDOTAG_TagBase+25 	; LONG
UNDOTAG_NewWindowWidth	equ	UNDOTAG_TagBase+26	; ULONG
UNDOTAG_NewWindowHeight	equ	UNDOTAG_TagBase+27	; ULONG
UNDOTAG_OldWindowVirtX	equ	UNDOTAG_TagBase+28	; LONG
UNDOTAG_OldWindowVirtY	equ	UNDOTAG_TagBase+29	; LONG
UNDOTAG_NewWindowVirtX	equ	UNDOTAG_TagBase+30	; LONG
UNDOTAG_NewWindowVirtY	equ	UNDOTAG_TagBase+31	; LONG
UNDOTAG_CreateIcon	equ	UNDOTAG_TagBase+32	; LONG
UNDOTAG_IconName	equ	UNDOTAG_TagBase+33	; CONST_STRPTR
UNDOTAG_OldProtection	equ	UNDOTAG_TagBase+34	; ULONG
UNDOTAG_NewProtection	equ	UNDOTAG_TagBase+35	; ULONG
UNDOTAG_OldComment	equ	UNDOTAG_TagBase+36	; CONST_STRPTR
UNDOTAG_NewComment	equ	UNDOTAG_TagBase+37	; CONST_STRPTR
UNDOTAG_OldToolTypes	equ	UNDOTAG_TagBase+38	; const STRPTR *
UNDOTAG_NewToolTypes	equ	UNDOTAG_TagBase+39	; const STRPTR *
UNDOTAG_OldIconObject	equ	UNDOTAG_TagBase+40	; Object *
UNDOTAG_NewIconObject	equ	UNDOTAG_TagBase+41	; Object *

;enum ScalosUndoCleanupMode
CLEANUP_Default		equ	0
CLEANUP_ByName		equ	1
CLEANUP_ByDate		equ	2
CLEANUP_BySize		equ	3
CLEANUP_ByType		equ	4

  STRUCTURE	UndoCleanupIconEntry,0
	APTR	ucin_IconNode		;const struct ScaIconNode *ucin_IconNode;
	LONG 	ucin_Left		;original x position of icon
	LONG 	ucin_Top		;original y position of icon
	LABEL	ucin_SIZEOF

  STRUCTURE	UndoCopyMoveEventData,0
	APTR 	ucmed_srcDirName	;STRPTR
	APTR 	ucmed_destDirName	;STRPTR
	APTR 	ucmed_srcName		;STRPTR
	APTR 	ucmed_destName		;STRPTR
	LABEL	ucmed_SIZEOF

  STRUCTURE	UndoIconEventData,0
	APTR 	uid_DirName		;STRPTR
	APTR 	uid_IconName		;STRPTR
	LABEL	uid_SIZEOF

  STRUCTURE	UndoCleanupData,0
	ULONG	ucd_CleanupMode		;enum ScalosUndoCleanupMode
	APTR	ucd_Icons		;struct UndoCleanupIconEntry *
	ULONG 	ucd_IconCount		;number of entries in ucd_Icons
	APTR	ucd_WindowTask		;struct ScaWindowTask *
	APTR 	ucd_WindowTitle		;STRPTR - a copy of the window title of ucd_WindowTask
	LABEL	ucd_SIZEOF

  STRUCTURE 	UndoSnaphotIconData,0
	APTR 	usid_DirName		;STRPTR
	APTR	usid_IconName		;STRPTR
	APTR	usid_IconObj		;Object * - Iconobject BEFORE snapshot/unsnaphot
	ULONG 	usid_SaveIcon
	LABEL	usid_SIZEOF

  STRUCTURE 	UndoSizeWindowData,0
	APTR	uswd_WindowTask		;struct ScaWindowTask *
	APTR 	uswd_WindowTitle	;STRPTR - a copy of the window title of ucd_WindowTask
	LONG 	uswd_OldLeft
	LONG 	uswd_OldTop
	ULONG 	uswd_OldWidth
	ULONG 	uswd_OldHeight
	LONG 	uswd_NewLeft
	LONG 	uswd_NewTop
	ULONG 	uswd_NewWidth
	ULONG 	uswd_NewHeight
	LONG 	uswd_OldVirtX
	LONG 	uswd_OldVirtY
	LONG 	uswd_NewVirtX
	LONG 	uswd_NewVirtY
	LABEL	uswd_SIZEOF

  STRUCTURE 	UndoNewDrawerData,0
	APTR 	und_DirName		; STRPTR
	APTR 	und_srcName		; STRPTR
	ULONG	und_CreateIcon
	LABEL 	und_SIZEOF

  STRUCTURE 	UndoSetProtectionData,0
	APTR 	uspd_DirName		; STRPTR
	APTR 	uspd_IconName		; STRPTR
	ULONG 	uspd_OldProtection
	ULONG 	uspd_NewProtection
	LABEL	uspd_SIZEOF

  STRUCTURE 	UndoSetCommentData,0
	APTR 	uscd_DirName		; STRPTR
	APTR 	uscd_IconName		; STRPTR
	APTR	uscd_OldComment		; STRPTR
	APTR	uscd_NewComment		; STRPTR
	LABEL	uscd_SIZEOF

  STRUCTURE 	UndoSetToolTypesData,0
	APTR 	ustd_DirName		; STRPTR
	APTR 	ustd_IconName		; STRPTR
	APTR	ustd_OldToolTypes	; STRPTR *
	APTR	ustd_NewToolTypes	; STRPTR *
	LABEL	ustd_SIZEOF

  STRUCTURE 	UndoChangeIconObjectData,0
	APTR	uciod_DirName		; STRPTR
	APTR 	uciod_IconName		; STRPTR
	APTR	uciod_OldIconObject     ; Object *
	APTR	uciod_NewIconObject	; Object *
	LABEL	uciod_SIZEOF

  STRUCTURE 	UndoCloseWindowData,0
	APTR	ucwd_DirName		; STRPTR
	APTR	ucwd_WindowTitle	; STRPTR
	LONG 	ucwd_Left;
	LONG 	ucwd_Top;
	ULONG 	ucwd_Width
	ULONG 	ucwd_Height
	LONG 	ucwd_VirtX
	LONG 	ucwd_VirtY
	UWORD 	ucwd_ViewAll
	UBYTE 	ucwd_Viewmodes
	LABEL	ucwd_SIZEOF

; uev_Data_SIZEOF is the largest of all sizes of the sub-structures
; UndoCopyMoveEventData, UndoIconEventData, UndoCleanupData, and UndoSnaphotIconData

uev_Data_SIZEOF	set     ucmed_SIZEOF
	IFGT    usid_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     usid_SIZEOF
	ENDIF
	IFGT    ucd_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     ucd_SIZEOF
	ENDIF
	IFGT    ucin_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     ucin_SIZEOF
	ENDIF
	IFGT    uid_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     uid_SIZEOF
	ENDIF
	IFGT    uswd_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     uswd_SIZEOF
	ENDIF
	IFGT    und_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     und_SIZEOF
	ENDIF
	IFGT    uspd_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     uspd_SIZEOF
	ENDIF
	IFGT    uscd_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     uscd_SIZEOF
	ENDIF
	IFGT    ustd_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     ustd_SIZEOF
	ENDIF
	IFGT    uciod_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     uciod_SIZEOF
	ENDIF
	IFGT    ucwd_SIZEOF-uev_Data_SIZEOF
uev_Data_SIZEOF	set     ucwd_SIZEOF
	ENDIF

  STRUCTURE	UndoEvent,0
	STRUCT	uev_Node,LN_SIZE
	ALIGNLONG	
	ULONG	uev_Type		;enum ScalosUndoType

	APTR	uev_UndoHook		;struct Hook *
	APTR	uev_RedoHook		;struct Hook *
	APTR	uev_DisposeHook		;struct Hook *
	APTR	uev_CustomAddHook	;struct Hook *

	LONG uev_OldPosX
	LONG uev_OldPosY
	LONG uev_NewPosX
	LONG uev_NewPosY

	STRUCT  uev_Data,uev_Data_SIZEOF

	APTR	uev_UndoStep		;struct UndoStep *

	ULONG	uev_DescrMsgIDSingle
	ULONG 	uev_DescrMsgIDMultiple

	APTR 	uev_DescrObjName	;STRPTR *

	LABEL	uev_SIZEOF

;---------------------------------------------------------------------------

	ENDC	; SCA_UNDO_I
