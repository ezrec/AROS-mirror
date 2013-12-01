;APS00000000000000000000000000000000000000000000000000000000000000000000000000000000
	IFND LISTTREE_MCC_I
LISTTREE_MCC_I SET 1

	IFND LIBRARIES_MUI_H
	INCLUDE "mui_asmone.i"
	ENDC


*** MUI Defines ***/

MUIC_Listtree MACRO
		dc.b		"Listtree.mcc",0
      even
      ENDM

; don't know what this is doing, well, obviously its creating
; a new mui object, with the type set to the one for the listtree
; but I don't know how to do this in asm
;ListtreeObject MACRO
;		MUI_NewObject(MUIC_Listtree
;      ENDM



*** Methods ***/

MUIM_Listtree_Close                EQU $8002001f
MUIM_Listtree_Exchange             EQU $80020008
MUIM_Listtree_FindName             EQU $8002003c
MUIM_Listtree_GetEntry             EQU $8002002b
MUIM_Listtree_GetNr                EQU $8002000e
MUIM_Listtree_Insert               EQU $80020011
MUIM_Listtree_Move                 EQU $80020009
MUIM_Listtree_Open                 EQU $8002001e
MUIM_Listtree_Remove               EQU $80020012
MUIM_Listtree_Rename               EQU $8002000c
MUIM_Listtree_SetDropMark          EQU $8002004c
MUIM_Listtree_Sort                 EQU $80020029
MUIM_Listtree_TestPos              EQU $8002004b

*** Method structs ***/

 STRUCTURE MUIP_Listtree_Close,0 
	ULONG MUIP_Listtree_Close_MethodID;
	APTR  MUIP_Listtree_Close_ListNode;
	APTR  MUIP_Listtree_Close_TreeNode;
	ULONG MUIP_Listtree_Close_Flags;
	LABEL	MUIP_Listtree_CloseSIZEOF

 STRUCTURE MUIP_Listtree_Exchange,0 
	ULONG MUIP_Listtree_Exchange_MethodID;
	APTR  MUIP_Listtree_Exchange_ListNode1;
	APTR  MUIP_Listtree_Exchange_TreeNode1;
	APTR  MUIP_Listtree_Exchange_ListNode2;
	APTR  MUIP_Listtree_Exchange_TreeNode2;
	ULONG MUIP_Listtree_Exchange_Flags;
	LABEL	MUIP_Listtree_ExchangeSIZEOF

 STRUCTURE MUIP_Listtree_FindName,0
	ULONG MUIP_Listtree_FindName_MethodID;
	APTR  MUIP_Listtree_FindName_ListNode;
	UBYTE  MUIP_Listtree_FindName_Name;
	ULONG MUIP_Listtree_FindName_Flags;
	LABEL	MUIP_Listtree_FindNameSIZEOF

 STRUCTURE MUIP_Listtree_GetEntry,0
	ULONG MUIP_Listtree_GetEntry_MethodID;
	APTR  MUIP_Listtree_GetEntry_Node;
	LONG  MUIP_Listtree_GetEntry_Position;
	ULONG MUIP_Listtree_GetEntry_Flags;
	LABEL	MUIP_Listtree_GetEntrySIZEOF

 STRUCTURE MUIP_Listtree_GetNr,0
	ULONG MUIP_Listtree_GetNr_MethodID;
	APTR  MUIP_Listtree_GetNr_TreeNode;
	ULONG MUIP_Listtree_GetNr_Flags;
	LABEL	MUIP_Listtree_GetNrSIZEOF

 STRUCTURE MUIP_Listtree_Insert,0
	ULONG MUIP_Listtree_Insert_MethodID;
	UBYTE  MUIP_Listtree_Insert_Name;
	APTR  MUIP_Listtree_Insert_User;
	APTR  MUIP_Listtree_Insert_ListNode;
	APTR  MUIP_Listtree_Insert_PrevNode;
	ULONG MUIP_Listtree_Insert_Flags;
	LABEL	MUIP_Listtree_InsertSIZEOF

 STRUCTURE MUIP_Listtree_Move,0 
	ULONG MUIP_Listtree_Move_MethodID;
	APTR  MUIP_Listtree_Move_OldListNode;
	APTR  MUIP_Listtree_Move_OldTreeNode;
	APTR  MUIP_Listtree_Move_NewListNode;
	APTR  MUIP_Listtree_Move_NewTreeNode;
	ULONG MUIP_Listtree_Move_Flags;
	LABEL	MUIP_Listtree_MoveSIZEOF

 STRUCTURE MUIP_Listtree_Open,0
	ULONG MUIP_Listtree_Open_MethodID;
	APTR  MUIP_Listtree_Open_ListNode;
	APTR  MUIP_Listtree_Open_TreeNode;
	ULONG MUIP_Listtree_Open_Flags;
	LABEL	MUIP_Listtree_OpenSIZEOF

 STRUCTURE MUIP_Listtree_Remove,0 
	ULONG MUIP_Listtree_Remove_MethodID;
	APTR  MUIP_Listtree_Remove_ListNode;
	APTR  MUIP_Listtree_Remove_TreeNode;
	ULONG MUIP_Listtree_Remove_Flags;
	LABEL	MUIP_Listtree_RemoveSIZEOF

 STRUCTURE MUIP_Listtree_Rename,0
	ULONG MUIP_Listtree_Rename_MethodID;
	APTR  MUIP_Listtree_Rename_TreeNode;
	UBYTE  MUIP_Listtree_Rename_NewName;
	ULONG MUIP_Listtree_Rename_Flags;
	LABEL	MUIP_Listtree_RenameSIZEOF

 STRUCTURE MUIP_Listtree_SetDropMark,0
	ULONG MUIP_Listtree_SetDropMark_MethodID;
	LONG  MUIP_Listtree_SetDropMark_Entry;
	ULONG MUIP_Listtree_SetDropMark_Values;
	LABEL	MUIP_Listtree_SetDropMarkSIZEOF

 STRUCTURE MUIP_Listtree_Sort,0
	ULONG MUIP_Listtree_Sort_MethodID;
	APTR  MUIP_Listtree_Sort_ListNode;
	ULONG MUIP_Listtree_Sort_Flags;
	LABEL	MUIP_Listtree_SortSIZEOF

 STRUCTURE MUIP_Listtree_TestPos,0
	ULONG MUIP_Listtree_TestPos_MethodID;
	LONG  MUIP_Listtree_TestPos_X;
	LONG  MUIP_Listtree_TestPos_Y;
	APTR  MUIP_Listtree_TestPos_Result;
	LABEL	MUIP_Listtree_TestPosSIZEOF


*** Special method values ***/

MUIV_Listtree_Close_ListNode_Root        EQU   0
MUIV_Listtree_Close_ListNode_Parent      EQU  -1
MUIV_Listtree_Close_ListNode_Active      EQU  -2

MUIV_Listtree_Close_TreeNode_Head        EQU   0
MUIV_Listtree_Close_TreeNode_Tail        EQU  -1
MUIV_Listtree_Close_TreeNode_Active      EQU  -2
MUIV_Listtree_Close_TreeNode_All         EQU  -3

MUIV_Listtree_Exchange_ListNode1_Root    EQU   0
MUIV_Listtree_Exchange_ListNode1_Active  EQU  -2

MUIV_Listtree_Exchange_TreeNode1_Head    EQU   0
MUIV_Listtree_Exchange_TreeNode1_Tail    EQU  -1
MUIV_Listtree_Exchange_TreeNode1_Active  EQU  -2

MUIV_Listtree_Exchange_ListNode2_Root    EQU   0
MUIV_Listtree_Exchange_ListNode2_Active  EQU  -2

MUIV_Listtree_Exchange_TreeNode2_Head    EQU   0
MUIV_Listtree_Exchange_TreeNode2_Tail    EQU  -1
MUIV_Listtree_Exchange_TreeNode2_Active  EQU  -2
MUIV_Listtree_Exchange_TreeNode2_Up      EQU  -5
MUIV_Listtree_Exchange_TreeNode2_Down    EQU  -6

MUIV_Listtree_FindName_ListNode_Root     EQU   0
MUIV_Listtree_FindName_ListNode_Active   EQU  -2

MUIV_Listtree_GetEntry_ListNode_Root     EQU   0
MUIV_Listtree_GetEntry_ListNode_Active   EQU  -2

MUIV_Listtree_GetEntry_Position_Head     EQU   0
MUIV_Listtree_GetEntry_Position_Tail     EQU  -1
MUIV_Listtree_GetEntry_Position_Active   EQU  -2
MUIV_Listtree_GetEntry_Position_Next     EQU  -3
MUIV_Listtree_GetEntry_Position_Previous EQU  -4
MUIV_Listtree_GetEntry_Position_Parent   EQU  -5

MUIV_Listtree_GetNr_TreeNode_Active      EQU  -2

MUIV_Listtree_Insert_ListNode_Root       EQU   0
MUIV_Listtree_Insert_ListNode_Active     EQU  -2

MUIV_Listtree_Insert_PrevNode_Head       EQU   0
MUIV_Listtree_Insert_PrevNode_Tail       EQU  -1
MUIV_Listtree_Insert_PrevNode_Active     EQU  -2
MUIV_Listtree_Insert_PrevNode_Sorted     EQU  -4

MUIV_Listtree_Move_OldListNode_Root      EQU   0
MUIV_Listtree_Move_OldListNode_Active    EQU  -2

MUIV_Listtree_Move_OldTreeNode_Head      EQU   0
MUIV_Listtree_Move_OldTreeNode_Tail      EQU  -1
MUIV_Listtree_Move_OldTreeNode_Active    EQU  -2

MUIV_Listtree_Move_NewListNode_Root      EQU   0
MUIV_Listtree_Move_NewListNode_Active    EQU  -2

MUIV_Listtree_Move_NewTreeNode_Head      EQU   0
MUIV_Listtree_Move_NewTreeNode_Tail      EQU  -1
MUIV_Listtree_Move_NewTreeNode_Active    EQU  -2
MUIV_Listtree_Move_NewTreeNode_Sorted    EQU  -4

MUIV_Listtree_Open_ListNode_Root         EQU   0
MUIV_Listtree_Open_ListNode_Parent       EQU  -1
MUIV_Listtree_Open_ListNode_Active       EQU  -2
MUIV_Listtree_Open_TreeNode_Head         EQU   0
MUIV_Listtree_Open_TreeNode_Tail         EQU  -1
MUIV_Listtree_Open_TreeNode_Active       EQU  -2
MUIV_Listtree_Open_TreeNode_All          EQU  -3

MUIV_Listtree_Remove_ListNode_Root       EQU   0
MUIV_Listtree_Remove_ListNode_Active     EQU  -2
MUIV_Listtree_Remove_TreeNode_Head       EQU   0
MUIV_Listtree_Remove_TreeNode_Tail       EQU  -1
MUIV_Listtree_Remove_TreeNode_Active     EQU  -2
MUIV_Listtree_Remove_TreeNode_All        EQU  -3

MUIV_Listtree_Rename_TreeNode_Active     EQU  -2

MUIV_Listtree_SetDropMark_Entry_None     EQU  -1

MUIV_Listtree_SetDropMark_Values_None    EQU   0
MUIV_Listtree_SetDropMark_Values_Above   EQU   1
MUIV_Listtree_SetDropMark_Values_Below   EQU   2
MUIV_Listtree_SetDropMark_Values_Onto    EQU   3
MUIV_Listtree_SetDropMark_Values_Sorted  EQU   4

MUIV_Listtree_Sort_ListNode_Root         EQU   0
MUIV_Listtree_Sort_ListNode_Active       EQU  -2

MUIV_Listtree_TestPos_Result_Flags_None  EQU   0
MUIV_Listtree_TestPos_Result_Flags_Above EQU   1
MUIV_Listtree_TestPos_Result_Flags_Below EQU   2
MUIV_Listtree_TestPos_Result_Flags_Onto  EQU   3
MUIV_Listtree_TestPos_Result_Flags_Sorted EQU   4


*** Special method flags ***/

MUIV_Listtree_Close_Flags_Nr             EQU (1<<15)
MUIV_Listtree_Close_Flags_Visible        EQU (1<<14)

MUIV_Listtree_FindName_Flags_SameLevel   EQU (1<<15)
MUIV_Listtree_FindName_Flags_Visible     EQU (1<<14)

MUIV_Listtree_GetEntry_Flags_SameLevel   EQU (1<<15)
MUIV_Listtree_GetEntry_Flags_Visible     EQU (1<<14)

MUIV_Listtree_GetNr_Flags_ListEmpty      EQU (1<<12)
MUIV_Listtree_GetNr_Flags_CountList      EQU (1<<13)
MUIV_Listtree_GetNr_Flags_CountLevel     EQU (1<<14)
MUIV_Listtree_GetNr_Flags_CountAll       EQU (1<<15)

MUIV_Listtree_Insert_Flags_Nr            EQU (1<<15)
MUIV_Listtree_Insert_Flags_Visible       EQU (1<<14)
MUIV_Listtree_Insert_Flags_Active        EQU (1<<13)
MUIV_Listtree_Insert_Flags_NextNode      EQU (1<<12)

MUIV_Listtree_Move_Flags_Nr              EQU (1<<15)
MUIV_Listtree_Move_Flags_Visible         EQU (1<<14)

MUIV_Listtree_Open_Flags_Nr              EQU (1<<15)
MUIV_Listtree_Open_Flags_Visible         EQU (1<<14)

MUIV_Listtree_Remove_Flags_Nr            EQU (1<<15)
MUIV_Listtree_Remove_Flags_Visible       EQU (1<<14)

MUIV_Listtree_Rename_Flags_User          EQU (1<<8)
MUIV_Listtree_Rename_Flags_NoRefresh     EQU (1<<9)

MUIV_Listtree_Sort_Flags_Nr              EQU (1<<15)
MUIV_Listtree_Sort_Flags_Visible         EQU (1<<14)



*** Attributes ***/

MUIA_Listtree_Active               EQU $80020020
MUIA_Listtree_CloseHook            EQU $80020033
MUIA_Listtree_ConstructHook        EQU $80020016
MUIA_Listtree_DestructHook         EQU $80020017
MUIA_Listtree_DisplayHook          EQU $80020018
MUIA_Listtree_DoubleClick          EQU $8002000d
MUIA_Listtree_DragDropSort         EQU $80020031
MUIA_Listtree_DuplicateNodeName    EQU $8002003d
MUIA_Listtree_EmptyNodes           EQU $80020030
MUIA_Listtree_Format               EQU $80020014
MUIA_Listtree_MultiSelect          EQU $800200c3
MUIA_Listtree_NList                EQU $800200c4
MUIA_Listtree_OpenHook             EQU $80020032
MUIA_Listtree_Quiet                EQU $8002000a
MUIA_Listtree_SortHook             EQU $80020010
MUIA_Listtree_Title                EQU $80020015
MUIA_Listtree_TreeColumn           EQU $80020013

*** Special attribute values ***/

MUIV_Listtree_Active_Off           EQU   0

MUIV_Listtree_ConstructHook_String EQU  -1

MUIV_Listtree_DestructHook_String  EQU  -1

MUIV_Listtree_DisplayHook_Default  EQU  -1

MUIV_Listtree_DoubleClick_Off      EQU  -1
MUIV_Listtree_DoubleClick_All      EQU  -2
MUIV_Listtree_DoubleClick_Tree     EQU  -3

MUIV_Listtree_SortHook_Head        EQU   0
MUIV_Listtree_SortHook_Tail        EQU  -1
MUIV_Listtree_SortHook_LeavesTop   EQU  -2
MUIV_Listtree_SortHook_LeavesMixed EQU  -3
MUIV_Listtree_SortHook_LeavesBottom EQU  -4



*** Structures, Flags & Values ***/

 STRUCTURE MUIS_Listtree_TreeNode,0
	LONG  tn_Private1;
	LONG  tn_Private2;
	UBYTE  tn_Name;
	UWORD tn_Flags;
	APTR  tn_User;
	LABEL	MUIS_Listtree_TreeNodeSIZEOF

 STRUCTURE MUIS_Listtree_TestPos_Result,0
	APTR  tpr_TreeNode;
	UWORD tpr_Flags;
	LONG  tpr_ListEntry;
	UWORD tpr_ListFlags;
	LABEL	MUIS_Listtree_TestPos_ResultSIZEOF


TNF_OPEN   EQU (1<<00)
TNF_LIST   EQU (1<<01)
TNF_FROZEN EQU (1<<02)
TNF_NOSIGN EQU (1<<03)




*** Configs ***/



	ENDC * LISTTREE_MCC_I */

* PrMake.rexx 0.10 (16.2.1996) Copyright 1995 kmel, Klaus Melchior */

