->******************************************************************************
-> $VER: Cat_Comment_Module.e 30.05.2004 (c) 2004 By COAT Jean-Marie ALIAS JMC
->******************************************************************************

OPT MODULE


->*****
->** External modules
->*****
MODULE 'locale' , 'libraries/locale'
MODULE 'utility/tagitem'

->*****
->** Object definitions
->*****
EXPORT OBJECT fc_type
	PRIVATE
		id	:	LONG
		str	:	LONG
ENDOBJECT

EXPORT OBJECT catalog_Scalos_Comment
	PUBLIC
                msg_Entry		:	PTR TO fc_type
                msg_Asltext		:	PTR TO fc_type
                msg_Type		:	PTR TO fc_type
		msg_Comment		:	PTR TO fc_type
		msg_All			:	PTR TO fc_type
		msg_HelpAll		:	PTR TO fc_type
		msg_Quit		:	PTR TO fc_type
		msg_HelpQuit		:	PTR TO fc_type
		msg_Time		:	PTR TO fc_type
		msg_HelpBtAbort		:	PTR TO fc_type
		msg_BtAbort		:	PTR TO fc_type
		msg_GroupEntry		:	PTR TO fc_type
		msg_HelpGroupEntry	:	PTR TO fc_type
		msg_LabelDrawer		:	PTR TO fc_type
		msg_LabelFile		:	PTR TO fc_type
		msg_ErrorReq		:	PTR TO fc_type
		msg_BtOkReq		:	PTR TO fc_type
		msg_NoFound		:	PTR TO fc_type
		msg_MuiError		:	PTR TO fc_type
		msg_IconLibError	:	PTR TO fc_type
		msg_AppError		:	PTR TO fc_type
		msg_TypeTool		:	PTR TO fc_type
		msg_TypeProject		:	PTR TO fc_type
		msg_TypeDrawer		:	PTR TO fc_type
		msg_TypeAssign		:	PTR TO fc_type
		msg_TypeDisk		:	PTR TO fc_type
		msg_TypeGarbage		:	PTR TO fc_type
		msg_TitleErrorReq	:	PTR TO fc_type
		msg_PrefNoFoundReq	:	PTR TO fc_type
		msg_EntryExists		:	PTR TO fc_type
		msg_Path		:	PTR TO fc_type
		msg_WinAnalysing	:	PTR TO fc_type
		msg_WinInfos		:	PTR TO fc_type
		msg_Objects		:	PTR TO fc_type
		msg_Drawers		:	PTR TO fc_type
		msg_Files		:	PTR TO fc_type
		msg_Size		:	PTR TO fc_type
		msg_Bytes		:	PTR TO fc_type
		msg_KBytes		:	PTR TO fc_type
		msg_MBytes		:	PTR TO fc_type
		msg_GetSize		:	PTR TO fc_type
		msg_LName		:	PTR TO fc_type
		msg_LComment		:	PTR TO fc_type
		msg_LSize		:	PTR TO fc_type
		msg_LDate		:	PTR TO fc_type
		msg_LTime		:	PTR TO fc_type
		msg_LPath		:	PTR TO fc_type
		msg_InComplete		:	PTR TO fc_type
		msg_BTCommAll		:	PTR TO fc_type
		msg_BTCommSel		:	PTR TO fc_type
		msg_BTRemEntry		:	PTR TO fc_type
		msg_TitleConfirmReq	:	PTR TO fc_type
		msg_BTConfirmReq	:	PTR TO fc_type
		msg_BodyConfirmReq	:	PTR TO fc_type
		msg_EntriesNumber	:	PTR TO fc_type
		msg_EntriesNumberHelp	:	PTR TO fc_type
		msg_MenuProject		:	PTR TO fc_type
		msg_MenuItemAbout	:	PTR TO fc_type
		msg_MenuItemSettings	:	PTR TO fc_type
		msg_MenuItemQuit	:	PTR TO fc_type
		msg_OpenDir		:	PTR TO fc_type
		msg_About		:	PTR TO fc_type
		msg_ReqAboutTitle	:	PTR TO fc_type
		msg_ScaLibError		:	PTR TO fc_type
		msg_MenuItemAboutKey	:	PTR TO fc_type
		msg_MenuItemSettingsKey	:	PTR TO fc_type
		msg_MenuItemQuitKey	:	PTR TO fc_type
		msg_AsltextDir		:	PTR TO fc_type
		msg_Blocks		:	PTR TO fc_type

ENDOBJECT

->*****
->** Global variables
->*****
DEF cat_Scalos_Comment : PTR TO catalog

->*****
->** Creation procedure for fc_type object
->*****
PROC create( id , str : PTR TO CHAR ) OF fc_type

	self.id := id
	self.str := str

ENDPROC

->*****
->** Procedure which returns the correct string according to the catalog
->*****

PROC getstr() OF fc_type
	RETURN ( IF cat_Scalos_Comment THEN GetCatalogStr( cat_Scalos_Comment , self.id , self.str ) ELSE self.str )
ENDPROC

->*****
->** Creation procedure for catalog_Scalos_Comment object
->*****
PROC create() OF catalog_Scalos_Comment

	DEF fct : PTR TO fc_type

	cat_Scalos_Comment := NIL

	self.msg_Entry	 		:= NEW fct.create( 0 , 'Entry :' )
	self.msg_Asltext 		:= NEW fct.create( 1 , 'Please, select one or several files...' )
	self.msg_Type	 		:= NEW fct.create( 2 , 'Type:' )
	self.msg_Comment 		:= NEW fct.create( 3 , 'Comment:' )
	self.msg_All	 		:= NEW fct.create( 4 , 'All' )
	self.msg_HelpAll 		:= NEW fct.create( 5 , 'If selected, comment will be apply\nto drawer and its contents.' )
	self.msg_Quit	 		:= NEW fct.create( 6 , 'Quit' )
	self.msg_HelpQuit		:= NEW fct.create( 7 , 'If selected, apply to quit Comment.module after\nto finish comment operation.' )
	self.msg_Time			:= NEW fct.create( 8 , 'Time:' )
	self.msg_HelpBtAbort		:= NEW fct.create( 9 , 'Abort operation.' )
	self.msg_BtAbort		:= NEW fct.create( 10 , '_Abort' )
	self.msg_GroupEntry		:= NEW fct.create( 11 , 'Current entry' )
	self.msg_HelpGroupEntry		:= NEW fct.create( 12 , 'Current drawer and file analysing' )
	self.msg_LabelDrawer		:= NEW fct.create( 13 , 'Drawer:' )
	self.msg_LabelFile		:= NEW fct.create( 14 , 'File:' )
	self.msg_ErrorReq		:= NEW fct.create( 15 , 'Entry : %s !\nErrorCode: %ld, %s' )
	self.msg_BtOkReq		:= NEW fct.create( 16 , 'Okay' )
	self.msg_NoFound		:= NEW fct.create( 17 , 'No found!' )
	self.msg_MuiError		:= NEW fct.create( 18 , 'Failed to open "%s".\n' )
	self.msg_IconLibError		:= NEW fct.create( 19 , 'Failed to open icon.library!\n' )
	self.msg_AppError		:= NEW fct.create( 20 , 'Failed to created application!\n' )
	self.msg_TypeTool		:= NEW fct.create( 21 , 'Tool' )
	self.msg_TypeProject		:= NEW fct.create( 22 , 'Project' )
	self.msg_TypeDrawer		:= NEW fct.create( 23 , 'Drawer' )
	self.msg_TypeAssign		:= NEW fct.create( 24 , 'Assign' )
	self.msg_TypeDisk		:= NEW fct.create( 25 , 'Disk' )
	self.msg_TypeGarbage		:= NEW fct.create( 26 , 'Trashcan' )
	self.msg_TitleErrorReq		:= NEW fct.create( 27 , 'Comment.module: Error' )
	self.msg_PrefNoFoundReq		:= NEW fct.create( 28 , 'Prefs file:\n"%s"\nno found!\nUse default settings...' )
	self.msg_EntryExists		:= NEW fct.create( 29 , 'Entry:\n"\eb%s\en"\nis already present!' )
	self.msg_Path			:= NEW fct.create( 30 , 'Path:' )
	self.msg_WinAnalysing		:= NEW fct.create( 31 , 'Comment.module in progress...' )
	self.msg_WinInfos		:= NEW fct.create( 32 , 'Comment.module - Informations' )
	self.msg_Objects		:= NEW fct.create( 33 , 'Object(s)' )
	self.msg_Drawers		:= NEW fct.create( 34 , 'Drawer(s)' )
	self.msg_Files			:= NEW fct.create( 35 , 'File(s)' )
	self.msg_Size			:= NEW fct.create( 36 , 'Size' )
	self.msg_Bytes			:= NEW fct.create( 37 , 'Bytes' )
	self.msg_KBytes			:= NEW fct.create( 38 , 'KBytes' )
	self.msg_MBytes			:= NEW fct.create( 39 , 'MBytes' )
	self.msg_GetSize		:= NEW fct.create( 40 , 'Get size' )
	self.msg_LName			:= NEW fct.create( 41 , 'Name' )
	self.msg_LComment		:= NEW fct.create( 42 , 'Comment' )
	self.msg_LSize			:= NEW fct.create( 43 , 'Size' )
	self.msg_LDate			:= NEW fct.create( 44 , 'Date' )
	self.msg_LTime			:= NEW fct.create( 45 , 'Time' )
	self.msg_LPath			:= NEW fct.create( 46 , 'Path' )
	self.msg_InComplete		:= NEW fct.create( 47 , 'Incomplete!' )
	self.msg_BTCommAll		:= NEW fct.create( 48 , '_Comment all' )
	self.msg_BTCommSel		:= NEW fct.create( 49 , 'Comment _selected' )
	self.msg_BTRemEntry		:= NEW fct.create( 50 , '_Remove entry' )
	self.msg_TitleConfirmReq	:= NEW fct.create( 51 , 'Comment.module: Confirmation' )
	self.msg_BTConfirmReq		:= NEW fct.create( 52 , ' Yes | No | Yes for all | Never ' )
	self.msg_BodyConfirmReq		:= NEW fct.create( 53 , 'Comment all objects presents\nin : %s' )
	self.msg_EntriesNumber		:= NEW fct.create( 54 , 'Entries number:' )
	self.msg_EntriesNumberHelp	:= NEW fct.create( 55 , 'This is the number of entries currently\npresents into the list.' )
	self.msg_MenuProject		:= NEW fct.create( 56 , 'Project' )
	self.msg_MenuItemAbout		:= NEW fct.create( 57 , 'About...' )
	self.msg_MenuItemSettings	:= NEW fct.create( 58 , 'Save settings' )
	self.msg_MenuItemQuit		:= NEW fct.create( 59 , 'Quit' )
	self.msg_OpenDir		:= NEW fct.create( 60 , 'Open path' )
	self.msg_About			:= NEW fct.create( 61 , 'Comment module for Scalos\nWritten in E\n%s\n%s' )
	self.msg_ReqAboutTitle		:= NEW fct.create( 62 , 'About Comment.module...' )
	self.msg_ScaLibError		:= NEW fct.create( 63 , 'Failed to open "%s" %s.\n' )
	self.msg_MenuItemAboutKey	:= NEW fct.create( 64 , 'A' )
	self.msg_MenuItemSettingsKey	:= NEW fct.create( 65 , 'S' )
	self.msg_MenuItemQuitKey	:= NEW fct.create( 66 , 'Q' )
	self.msg_AsltextDir 		:= NEW fct.create( 67 , 'Please, select a drawer or disk...' )
	self.msg_Blocks			:= NEW fct.create( 68 , '(%ld Blocks)')

ENDPROC

->*****
->** Opening catalog procedure (exported)
->*****
PROC open( loc : PTR TO locale , language : PTR TO CHAR ) OF catalog_Scalos_Comment

	DEF tag , tagarg

	self.close()

	IF ( localebase AND ( cat_Scalos_Comment = NIL ) )

		IF language

			tag := OC_LANGUAGE
			tagarg := language

		ELSE

			tag:= TAG_IGNORE

		ENDIF

		cat_Scalos_Comment := OpenCatalogA( loc , 'Scalos/Scalos_Comment.catalog' ,
								[	OC_BUILTINLANGUAGE , 'english' ,
									tag , tagarg ,
									OC_VERSION , 0 ,
									TAG_DONE	] )

	ENDIF

ENDPROC

->*****
->** Closing catalog procedure
->*****
PROC close() OF catalog_Scalos_Comment

	IF localebase THEN CloseCatalog( cat_Scalos_Comment )
	cat_Scalos_Comment := NIL

ENDPROC
