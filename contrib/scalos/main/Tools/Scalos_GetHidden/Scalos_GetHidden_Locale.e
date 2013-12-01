->*************************************************************************
-> $VER: cat_Scalos_GetHidden.e 26.05.2009 (c) 2008-2009 By COAT Jean-Marie
->*************************************************************************

OPT MODULE, PREPROCESS


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

EXPORT OBJECT catalog_Scalos_GetHidden
	PUBLIC
                msg_Searching 		:	PTR TO fc_type
                msg_RemoveHidden 	:	PTR TO fc_type
                msg_Abort	 	:	PTR TO fc_type
		msg_Dirs	 	:	PTR TO fc_type
		msg_Files	 	:	PTR TO fc_type
		msg_Total	 	:	PTR TO fc_type
		msg_NumberDirs 		:	PTR TO fc_type
		msg_NumberFiles		:	PTR TO fc_type
		msg_NumberTotal		:	PTR TO fc_type
		msg_AnyHiddenFound	:	PTR TO fc_type
		msg_UserAborted		:	PTR TO fc_type
		msg_Completed		:	PTR TO fc_type
		msg_WaitHiddenRemoved	:	PTR TO fc_type
		msg_HiddenRemoved	:	PTR TO fc_type
		msg_ScalosFailed	:	PTR TO fc_type
		msg_ErrorMuimaster	:	PTR TO fc_type
		msg_ErrorIconLib	:	PTR TO fc_type
		msg_ErrorApp		:	PTR TO fc_type
		msg_IoErrorFile		:	PTR TO fc_type
		msg_IoErrorFullPath	:	PTR TO fc_type
		msg_TitleName		:	PTR TO fc_type
		msg_TitleProtect	:	PTR TO fc_type
		msg_TitleSise		:	PTR TO fc_type
		msg_TitleDate		:	PTR TO fc_type
		msg_TitleTime		:	PTR TO fc_type
		msg_TitlePath		:	PTR TO fc_type
		msg_MenuOpenDir		:	PTR TO fc_type
		msg_ArgNoFound		: 	PTR TO fc_type
		msg_EntryName		: 	PTR TO fc_type
ENDOBJECT

->*****
->** Global variables
->*****
DEF cat_Scalos_GetHidden : PTR TO catalog

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
	RETURN ( IF cat_Scalos_GetHidden THEN GetCatalogStr( cat_Scalos_GetHidden , self.id , self.str ) ELSE self.str )
ENDPROC

->*****
->** Creation procedure for catalog_Scalos_GetHidden object
->*****
PROC create() OF catalog_Scalos_GetHidden

	DEF fct : PTR TO fc_type

	cat_Scalos_GetHidden := NIL

	self.msg_Searching 		:= NEW fct.create( 0 ,  'Please wait, searching hidden protection...' )
	self.msg_RemoveHidden 		:= NEW fct.create( 1 ,  'Remove hidden protection...' )
	self.msg_Abort	 		:= NEW fct.create( 2 ,  'Abort' )
	self.msg_Dirs	 		:= NEW fct.create( 3 ,  'Dirs:' )
	self.msg_Files	 		:= NEW fct.create( 4 ,  'Files:' )
	self.msg_Total	 		:= NEW fct.create( 5 ,  'Total:' )
	self.msg_NumberDirs 		:= NEW fct.create( 6 ,  'Number of dirs found.' )
	self.msg_NumberFiles		:= NEW fct.create( 7 ,  'Number of files found.' )
	self.msg_NumberTotal		:= NEW fct.create( 8 ,  'Total number of objects found.' )
	self.msg_AnyHiddenFound		:= NEW fct.create( 9 ,  'Any hidden protection bit found!' )
	self.msg_UserAborted		:= NEW fct.create( 10 , 'Aborted by user!' )
	self.msg_Completed		:= NEW fct.create( 11 , 'Completed!' )
	self.msg_WaitHiddenRemoved	:= NEW fct.create( 12 , 'Please wait, remove hidden protection...' )
	self.msg_HiddenRemoved		:= NEW fct.create( 13 , 'Hidden protection bit removed! ' )
	self.msg_ScalosFailed		:= NEW fct.create( 14 , 'Can\at open %s.%s' )
	self.msg_ErrorMuimaster		:= NEW fct.create( 15 , 'Can\at open %s!' )
	self.msg_ErrorIconLib		:= NEW fct.create( 16 , 'Can\at open icon.library!' )
	self.msg_ErrorApp		:= NEW fct.create( 17 , 'Can\at create application!' )
	self.msg_IoErrorFile		:= NEW fct.create( 18 , '*** WARNING ***\n%s\n\tErrorCode: %d - %s\n***************\n' )
	self.msg_IoErrorFullPath	:= NEW fct.create( 19 , '*** WARNING ***\n%s\n\tErrorCode: %d - %s\n***************\n' )
	self.msg_TitleName		:= NEW fct.create( 20 , 'Name' )
	self.msg_TitleProtect		:= NEW fct.create( 21 , 'Protection' )
	self.msg_TitleSise		:= NEW fct.create( 22 , 'Size' )
	self.msg_TitleDate		:= NEW fct.create( 23 , 'Date' )
	self.msg_TitleTime		:= NEW fct.create( 24 , 'Time' )
	self.msg_TitlePath		:= NEW fct.create( 25 , 'Path' )
	self.msg_MenuOpenDir		:= NEW fct.create( 26 , 'Open parent dir' )
	self.msg_ArgNoFound		:= NEW fct.create( 27 , 'Argument no found!' )
	self.msg_EntryName		:= NEW fct.create( 28 , '** Entry name:' )
ENDPROC

->*****
->** Opening catalog procedure (exported)
->*****
PROC open( loc : PTR TO locale , language : PTR TO CHAR ) OF catalog_Scalos_GetHidden

	DEF tag , tagarg

	self.close()

	IF ( localebase AND ( cat_Scalos_GetHidden = NIL ) )

		IF language

			tag := OC_LANGUAGE
			tagarg := language

		ELSE

			tag:= TAG_IGNORE

		ENDIF

		cat_Scalos_GetHidden := OpenCatalogA( loc , 'Scalos/GetHidden.catalog' ,
								[	OC_BUILTINLANGUAGE , 'english' ,
									tag , tagarg ,
									OC_VERSION , 0 ,
									TAG_DONE	] )

	ENDIF

ENDPROC

->*****
->** Closing catalog procedure
->*****
PROC close() OF catalog_Scalos_GetHidden

	IF localebase THEN CloseCatalog( cat_Scalos_GetHidden )
	cat_Scalos_GetHidden := NIL

ENDPROC
