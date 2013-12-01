->******************************************************************************
-> $VER: Cat_Protect_Module.e 30.05.2004 (c) 2004 By COAT Jean-Marie ALIAS JMC
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

EXPORT OBJECT catalog_Scalos_Protect
	PUBLIC
                msg_Entry		:	PTR TO fc_type
                msg_Asltext		:	PTR TO fc_type
                msg_Type		:	PTR TO fc_type
		msg_Protect		:	PTR TO fc_type
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

		msg_TextHide		:	PTR TO fc_type
		msg_TextScript		:	PTR TO fc_type
		msg_TextPure		:	PTR TO fc_type
		msg_TextArchive		:	PTR TO fc_type
		msg_TextRead		:	PTR TO fc_type
		msg_TextWrite		:	PTR TO fc_type
		msg_TextExecute		:	PTR TO fc_type
		msg_TextDelete		:	PTR TO fc_type

		msg_GroupProtect	:	PTR TO fc_type
		msg_TextNothing		:	PTR TO fc_type
		msg_HelpTextHide	:	PTR TO fc_type
		msg_HelpTextScript	:	PTR TO fc_type
		msg_HelpTextPure	:	PTR TO fc_type
		msg_HelpTextArchive	:	PTR TO fc_type
		msg_HelpTextRead	:	PTR TO fc_type
		msg_HelpTextWrite	:	PTR TO fc_type
		msg_HelpTextExecute	:	PTR TO fc_type
		msg_HelpTextDelete	:	PTR TO fc_type

ENDOBJECT

->*****
->** Global variables
->*****
DEF cat_Scalos_Protect : PTR TO catalog

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
	RETURN ( IF cat_Scalos_Protect THEN GetCatalogStr( cat_Scalos_Protect , self.id , self.str ) ELSE self.str )
ENDPROC

->*****
->** Creation procedure for catalog_Scalos_Protect object
->*****
PROC create() OF catalog_Scalos_Protect

	DEF fct : PTR TO fc_type

	cat_Scalos_Protect := NIL

	self.msg_Entry	 	:= NEW fct.create( 0 , 'Entry :' )
	self.msg_Asltext 	:= NEW fct.create( 1 , 'Please, select a drawer or file...' )
	self.msg_Type	 	:= NEW fct.create( 2 , 'Type:' )
	self.msg_Protect 	:= NEW fct.create( 3 , '_Apply' )
	self.msg_All	 	:= NEW fct.create( 4 , 'All' )
	self.msg_HelpAll 	:= NEW fct.create( 5 , 'If selected, Protect will be apply\nto drawer and its contents.' )
	self.msg_Quit	 	:= NEW fct.create( 6 , 'Quit' )
	self.msg_HelpQuit	:= NEW fct.create( 7 , 'If selected, apply to quit Protect.module after\nto finish Protect operation.' )
	self.msg_Time		:= NEW fct.create( 8 , 'Time:' )
	self.msg_HelpBtAbort	:= NEW fct.create( 9 , 'Abort operation.' )
	self.msg_BtAbort	:= NEW fct.create( 10 , 'Cancel' )
	self.msg_GroupEntry	:= NEW fct.create( 11 , 'Current entry' )
	self.msg_HelpGroupEntry	:= NEW fct.create( 12 , 'Current drawer and file analysing' )
	self.msg_LabelDrawer	:= NEW fct.create( 13 , 'Drawer:' )
	self.msg_LabelFile	:= NEW fct.create( 14 , 'File:' )
	self.msg_ErrorReq	:= NEW fct.create( 15 , 'Entry : %s !\nErrorCode: %ld, %s' )
	self.msg_BtOkReq	:= NEW fct.create( 16 , 'Okay' )
	self.msg_NoFound	:= NEW fct.create( 17 , 'No found!' )
	self.msg_MuiError	:= NEW fct.create( 18 , 'Failed to open "%s".\n' )
	self.msg_IconLibError	:= NEW fct.create( 19 , 'Failed to open icon.library!\n' )
	self.msg_AppError	:= NEW fct.create( 20 , 'Failed to created application!\n' )
	self.msg_TypeTool	:= NEW fct.create( 21 , 'Tool' )
	self.msg_TypeProject	:= NEW fct.create( 22 , 'Project' )
	self.msg_TypeDrawer	:= NEW fct.create( 23 , 'Drawer' )
	self.msg_TypeAssign	:= NEW fct.create( 24 , 'Assign' )
	self.msg_TypeDisk	:= NEW fct.create( 25 , 'Disk' )
	self.msg_TypeGarbage	:= NEW fct.create( 26 , 'Trashcan' )
	self.msg_TitleErrorReq	:= NEW fct.create( 27 , 'Protect.module: Error' )
	self.msg_PrefNoFoundReq	:= NEW fct.create( 28 , 'Prefs file:\n"%s"\nno found!\nUse default settings...' )
	self.msg_TextHide	:= NEW fct.create( 29 , 'Hide' )
	self.msg_TextScript	:= NEW fct.create( 30 , 'Script' )
	self.msg_TextPure	:= NEW fct.create( 31 , 'Pure' )
	self.msg_TextArchive	:= NEW fct.create( 32 , 'Archive' )
	self.msg_TextRead	:= NEW fct.create( 33 , 'Read' )
	self.msg_TextWrite	:= NEW fct.create( 34 , 'Write' )
	self.msg_TextExecute	:= NEW fct.create( 35 , 'Execute' )
	self.msg_TextDelete	:= NEW fct.create( 36 , 'Delete' )
	self.msg_GroupProtect	:= NEW fct.create( 37 , ' Protection bits ' )
	self.msg_TextNothing	:= NEW fct.create( 38 , 'Nothing to do!' )

	self.msg_HelpTextHide	:= NEW fct.create( 39 , 'The file is hidden.' )
	self.msg_HelpTextScript	:= NEW fct.create( 40 , 'The file is a script.' )
	self.msg_HelpTextPure	:= NEW fct.create( 41 , 'The file is a pure command\nand can be made resident.' )
	self.msg_HelpTextArchive:= NEW fct.create( 42 , 'The file has been archived.' )
	self.msg_HelpTextRead	:= NEW fct.create( 43 , 'The file can be read.' )
	self.msg_HelpTextWrite	:= NEW fct.create( 44 , 'The file can be written to (altered).' )
	self.msg_HelpTextExecute:= NEW fct.create( 45 , 'The file is executable (a program).' )
	self.msg_HelpTextDelete	:= NEW fct.create( 46 , 'The file or directory can be deleted.\n(Files within a delete-protected directory can still be deleted.)' )


ENDPROC

->*****
->** Opening catalog procedure (exported)
->*****
PROC open( loc : PTR TO locale , language : PTR TO CHAR ) OF catalog_Scalos_Protect

	DEF tag , tagarg

	self.close()

	IF ( localebase AND ( cat_Scalos_Protect = NIL ) )

		IF language

			tag := OC_LANGUAGE
			tagarg := language

		ELSE

			tag:= TAG_IGNORE

		ENDIF

		cat_Scalos_Protect := OpenCatalogA( loc , 'Scalos/Scalos_Protect.catalog' ,
								[	OC_BUILTINLANGUAGE , 'english' ,
									tag , tagarg ,
									OC_VERSION , 0 ,
									TAG_DONE	] )

	ENDIF

ENDPROC

->*****
->** Closing catalog procedure
->*****
PROC close() OF catalog_Scalos_Protect

	IF localebase THEN CloseCatalog( cat_Scalos_Protect )
	cat_Scalos_Protect := NIL

ENDPROC
