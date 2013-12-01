OPT PREPROCESS, STACK=32000, LARGE

MODULE	'dos/dos', 'dos/dosextens', 'dos/datetime', 'amigalib/boopsi','tools/installhook', 'workbench/startup',

	'workbench/workbench', 'utility/hooks', 'utility', 'utility/tagitem', 'libraries/gadtools',

	'muimaster', 'libraries/mui', 'mui/nlistview_mcc', 'mui/nlist_mcc', 'debug',

	'mui/lamp_mcc', 'icon', '*MyDebugOnOff', '*VERSTAGECX','debug', '*getpath', '*GeticonObject',

	'scalos/scalos41.6','scalos', 'intuition/classusr', 'intuition/classes', 'amigalib/boopsi',

	'*Scalos_GetHidden_Locale','locale','libraries/locale','*GetTCP','*OnlineUpdate','exec/memory','exec/nodes',

	'tools/boopsi'


#define FIBF_HIDDEN Shl(1,7)

#define THE_FILE 'GeHidden.e'

OBJECT oListData
   nom     : PTR TO CHAR
   protect : PTR TO CHAR
   taille  : PTR TO CHAR
   ladate  : PTR TO CHAR
   heure   : PTR TO CHAR
   chemin  : PTR TO CHAR
ENDOBJECT

ENUM ER_NONE, ER_MUILIB, ER_LIB, ER_ICONLIB, ER_APP, UN_HIDE, OP_DIR, ABOUT_MUI, ABOUT, UPDATE

DEF	textbuffer[120]:STRING,
	list_Prot,
	bufnom[110]:STRING,
	buftaille[20]:STRING,
	bufstamp[20]:STRING,
	bufheure[20]:STRING,
	path[256]:STRING,
	bufchemin[256]:STRING,
	bufprotect[10]:STRING,
	numlist,
	new,
	dt:datetime,
	day[50]:ARRAY,
	date[50]:ARRAY,
	time[50]:ARRAY,
	txt_Num,
	nfiles=0,
	ndirs=0,
	txt_Dirs,
	txt_Files,
	txt_Splash,
	arret=0,
	bt_Arret,
	set_UnHide=FALSE,
	app=0,
	win=0,
	sigs,
	result,
	running=TRUE,
	bufstring=NIL,
	id,
	lastcom=0,
	numobj=0,
	cySort,
	buf_NumDirs, buf_NumFiles, txt_HeadSplash, bt_UnHide,
	icone,
	lamp,
	gr_list,
	menufiletypes,
	menuopen,
	cat:PTR TO catalog_Scalos_GetHidden,
	buf_Search[300]:STRING,wb_message=0,opened=FALSE,txt_DirSplash,
	up=0

DEF hook_lv_display:hook,
    hook_lv_construct:hook,
    hook_lv_destruct:hook,
    hook_lv_compare:hook,
    hook_lv_compare2:hook,
    contextmenu_hook:hook,
    appMsgHook:hook

PROC main() HANDLE
DEF	ent:PTR TO oListData,npc=0, appicon:PTR TO diskobject,wintitre[100]:STRING,tcp_version=NIL,menu,jmc=NIL

	IF (scalosbase := OpenLibrary(SCALOSNAME,SCALOSREVISION)) = NIL THEN Raise(ER_LIB)
	IF (muimasterbase:=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))=NIL THEN Raise(ER_MUILIB)
	IF (iconbase:=OpenLibrary('icon.library',0))=NIL THEN Raise(ER_ICONLIB)
	IF (utilitybase:=OpenLibrary('utility.library',0))=NIL THEN Raise('Failed to open utility.library !')

	localebase := OpenLibrary( 'locale.library' , 0 )
	NEW cat.create()
	cat.open( NIL , NIL )

	installhook(hook_lv_display, {hooklvdisplay})
	installhook(hook_lv_construct, {hooklvconstruct})
	installhook(hook_lv_destruct, {hooklvdestruct})
	installhook(hook_lv_compare, {hooklvcompare})
	installhook(hook_lv_compare2, {hooklvcompare2})
	installhook(contextmenu_hook, {contextmenu_func})
	installhook(appMsgHook,{appMsgFunc})

	tcp_version	:= String(EstrLen(VERSTRING)+6)
	StringF(tcp_version, '$VER: \s', VERSTRING)
	-> SetStr(tcp_version,StrLen(tcp_version)-22)

	menu:=[ NM_TITLE,	0, (cat.msg_MenuProject.getstr() )	, 0 ,0,0,0,
   		NM_ITEM,	0, NM_BARLABEL				, 0 ,0,0,0,
   		NM_ITEM,	0, (cat.msg_MenuAbout.getstr() )	, 'A' ,0,0, ABOUT,
   		NM_ITEM,	0, (cat.msg_MenuAboutMui.getstr() )	, 'M' ,0,0, ABOUT_MUI,
		NM_ITEM,	0, NM_BARLABEL				, 0 ,0,0,0,
   		NM_ITEM,	0, (cat.msg_MenuUpdate.getstr() )	, 'G' ,0,0, UPDATE,
   		NM_ITEM,	0, (cat.msg_MenuQuit.getstr() )		, 'Q' ,0,0, MUIV_Application_ReturnID_Quit,
		NM_END, 	0, NIL					, 0 ,0,0,0]:newmenu

	menufiletypes:=MenustripObject,
		Child, MenuObject,
			MUIA_Menu_Title, bufnom,
			Child, menuopen:=MenuitemObject,
				MUIA_Menuitem_Title, (cat.msg_MenuOpenDir.getstr() ),
				MUIA_Menuitem_Enabled, 0,
				MUIA_UserData, OP_DIR,
			End,
		End,
	End

	StringF(wintitre,'\s \s',VERSTRING, __TARGET__)

	app := ApplicationObject,
		MUIA_Application_Title      , 'GetHidden.module',
		MUIA_Application_Version    , VERSTRING,
		MUIA_Application_Copyright  , COPYRIGHT,
		MUIA_Application_Author     , 'Jean_Marie COAT',
		MUIA_Application_Description, 'UnSet Hidden protection of drawers/files for SCALOS',
		MUIA_Application_Base       , 'GetHidden.module',
		MUIA_Application_DiskObject, get_MyDiskObject(appicon),
		MUIA_Application_Menustrip,    Mui_MakeObjectA(MUIO_MenustripNM,[menu,0]),

-> ***************************

		SubWindow, win:= WindowObject,
			MUIA_Window_Title, wintitre,
			MUIA_Window_ID, "MAIN",
			MUIA_Window_ScreenTitle, VERSTAG,
			MUIA_Window_Width,  MUIV_Window_Width_Screen(25),
			MUIA_Window_Height, MUIV_Window_Height_Screen(25),
			MUIA_Window_AppWindow, 1,
			WindowContents, VGroup,
				Child, txt_HeadSplash:=TextObject,
					MUIA_Text_PreParse,'\ec',
					MUIA_Text_Contents, (cat.msg_Searching.getstr() ),
				End,

				Child, txt_DirSplash:=TextObject,	-> +++
					MUIA_Text_PreParse,'\el',
					MUIA_Text_Contents, '',
				End,

				Child, HGroup,
					Child, txt_Splash:=TextObject, TextFrame,
						MUIA_Text_PreParse,'\el',
						MUIA_Text_Contents,'',
					End,
	  				Child, lamp:=LampObject,
						MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
					End,
				End,

				Child, gr_list:=GroupObject,MUIA_ContextMenu, menufiletypes,
					Child, list_Prot :=makeLV(NIL, TRUE, TRUE, MUIV_Frame_InputList, MUIV_NList_MultiSelect_Default, NIL, NIL),
				End,
				Child, VGroup,
					Child, HGroup,
						Child, bt_UnHide:=SimpleButton(cat.msg_RemoveHidden.getstr() ),
						Child, bt_Arret:=SimpleButton(cat.msg_Abort.getstr() ),
					End,
					Child, ColGroup(3),
						Child, HGroup,
							Child, Label2(cat.msg_Dirs.getstr() ),
							Child, txt_Dirs  :=makeTXT(40,MUIV_Frame_Text,MUII_TextBack,NIL,'0',NIL,'\ec\eb\e3', cat.msg_NumberDirs.getstr() ),
						End,
						Child, HGroup,
							Child, Label2(cat.msg_Files.getstr() ),
							Child, txt_Files :=makeTXT(40,MUIV_Frame_Text,MUII_TextBack,NIL,'0',NIL,'\ec\eb\e3', cat.msg_NumberFiles.getstr() ),
						End,
						Child, HGroup,
							Child, Label2(cat.msg_Total.getstr() ),
							Child, txt_Num   :=makeTXT(40,MUIV_Frame_Text,MUII_TextBack,NIL,'0',NIL,'\ec\eb\e3', cat.msg_NumberTotal.getstr() ),
						End,
					End,
				End,
			End,
		End,

-> ***************************

	End

	IF (app=NIL) THEN Raise(ER_APP)

	doMethodA(win,		[MUIM_Notify, MUIA_Window_CloseRequest, MUI_TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit])
	doMethodA(win,		[MUIM_Notify, MUIA_Window_CloseRequest, MUI_TRUE, app, 3, MUIM_WriteLong, 1, {arret}])
	doMethodA(bt_Arret,  	[MUIM_Notify, MUIA_Pressed, 0, app, 3, MUIM_WriteLong, 1, {arret}])
	doMethodA(bt_UnHide,  	[MUIM_Notify, MUIA_Pressed, 0, app, 2, MUIM_Application_ReturnID, UN_HIDE])

	doMethodA(gr_list, 	[MUIM_Notify, MUIA_ContextMenuTrigger, MUIV_EveryTime, app, 3, MUIM_CallHook, contextmenu_hook, MUIV_TriggerValue])
	doMethodA(win,		[MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,list_Prot,3,MUIM_CallHook,appMsgHook,MUIV_TriggerValue])

	-> doMethodA(win,		[MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,list_Prot,3,MUIM_WriteLong, 1, {set_AppWin}])

	set(bt_UnHide, MUIA_Disabled, 1)

	getargs()

	up:=NEW [app,
		win,
		'GetHidden',
		'GetHidden update',
		tcp_version,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL,
		NIL]:updObject

	get(app,MUIA_Application_Iconified,{icone})
	IF icone THEN set(app,MUIA_Application_Iconified, FALSE)

/******************************************************************************************************
	get(win, MUIA_Window_Open, {opened})
	IF opened
		set(bt_Arret, MUIA_Disabled, 1)
		set(bt_UnHide, MUIA_Disabled, 0)

		IF (set_UnHide)
			proc_CountDirFiles()
		ELSE
			set(txt_Splash,     MUIA_Text_Contents, cat.msg_AnyHiddenFound.getstr() )
			set(bt_UnHide, MUIA_Disabled, 1)
		ENDIF
		IF (arret=1) THEN set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_UserAborted.getstr() ) ELSE set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_Completed.getstr() )
	ENDIF
*******************************************************************************************************/


	WHILE running
		result:= doMethodA(app,[MUIM_Application_Input,{sigs}])
		SELECT result
			CASE MUIV_Application_ReturnID_Quit
				running:=FALSE

			CASE UN_HIDE
				set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_WaitHiddenRemoved.getstr() )
				arret:=0
				vnumlist(list_Prot)
				set(list_Prot,  MUIA_NList_Quiet,1)
				set(bt_Arret,   MUIA_Disabled, 0)
				set(bt_UnHide,  MUIA_Disabled, 1)

				IF (numlist=1)
					set(list_Prot, MUIA_NList_Active,0)
					doMethodA(list_Prot, [MUIM_NList_GetEntry,0,{ent}])
					proc_Analyse(ent)
					init_Selected(0,ent)
					set(list_Prot,MUIA_NList_Active,0)
		
				ELSE
					set(list_Prot,MUIA_NList_Active,MUIV_NList_Active_Off)
					vid()
					proc_StartProcess()
					REPEAT
						doMethodA(list_Prot, [MUIM_NList_NextSelected,{id}])
						doMethodA(list_Prot, [MUIM_NList_GetEntry,id,{ent}])
						IF (id<>-1) AND (arret=0)
							npc++
							proc_Analyse(ent)
							init_Selected(id,ent)
							lastcom:=npc
						ENDIF
					UNTIL(id=MUIV_NList_NextSelected_End)
					proc_EndProcess()
				ENDIF

				set(bt_Arret,   MUIA_Disabled, 1)
				set(bt_UnHide,  MUIA_Disabled, 0)
				set(list_Prot, MUIA_NList_Quiet,0)
				set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_HiddenRemoved.getstr() )

			CASE UPDATE
				update(up)

	                CASE ABOUT
				jmc:=String(StrLen(VERSTRING)+StrLen(COPYRIGHT)+StrLen(COMPILER_STRING)+50)
				StringF(jmc,'\ec\s\n\s\nCompiled with ECX \e3\d\e2 Version: \e3\s\en', VERSTRING, COPYRIGHT, ECX_VERSION, __TARGET__)
				Mui_RequestA(app,win,0, cat.msg_ReqBodyHidden.getstr() , cat.msg_ReqOkay.getstr() , jmc, NIL)

	                CASE ABOUT_MUI
        	            	domethod(app,[MUIM_Application_AboutMUI,NIL])

			DEFAULT
			 	set(bt_Arret, MUIA_Disabled, 1)	-> +++
				vnumlist(list_Prot)
				IF numlist > 0 THEN set(bt_UnHide,  MUIA_Disabled, 0)
				-> set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_Completed.getstr() )

		ENDSELECT

		IF (running AND sigs) THEN Wait(sigs)
	ENDWHILE

	FreeDiskObject(appicon)

	EXCEPT DO
		IF app 		 THEN Mui_DisposeObject(app)
		IF scalosbase	 THEN CloseLibrary(scalosbase)
		IF utilitybase	 THEN CloseLibrary(utilitybase)
		IF iconbase 	 THEN CloseLibrary(iconbase)
		IF muimasterbase THEN CloseLibrary(muimasterbase)
		IF up  THEN END up
		IF tcp_version <> NIL
			DisposeLink(tcp_version)
			tcp_version:=NIL
		ENDIF
		IF jmc <> NIL
			DisposeLink(jmc)
			jmc:=NIL
		ENDIF
		cat.close()
		IF localebase THEN CloseLibrary( localebase )
  		SELECT exception
			CASE ER_LIB
				WriteF(cat.msg_ScalosFailed.getstr() ,SCALOSNAME, SCALOSREVISION)
				CleanUp(20)
			CASE ER_MUILIB
				WriteF(cat.msg_ErrorMuimaster.getstr() , MUIMASTER_NAME)
				CleanUp(20)

			CASE ER_ICONLIB
				WriteF(cat.msg_ErrorIconLib.getstr() )
				CleanUp(20)

			CASE ER_APP
				WriteF(cat.msg_ErrorApp.getstr() )
				CleanUp(20)

		ENDSELECT
ENDPROC 0

->*************

PROC proc_CountDirFiles()

	get(list_Prot,MUIA_NList_Entries,{numlist})
	viewnum()
	checkentry()
	IF (ndirs > 0)
		buf_NumDirs := String(10)
		StringF(buf_NumDirs,'\d', ndirs)
		set(txt_Dirs,  MUIA_Text_Contents, buf_NumDirs)
		DisposeLink(buf_NumDirs)
	ELSE
		 set(txt_Dirs,  MUIA_Text_Contents, '0')
	ENDIF

	IF (nfiles > 0)
		buf_NumFiles := String(10)
		StringF(buf_NumFiles,'\d', nfiles)
		set(txt_Files, MUIA_Text_Contents, buf_NumFiles)
		DisposeLink(buf_NumFiles)
	ELSE
		set(txt_Files, MUIA_Text_Contents, '0')
	ENDIF
ENDPROC

->***************

PROC checkentry()
DEF identry:PTR TO oListData,idnum=0,testnom

	ndirs  :=0
	nfiles :=0
	vnumlist(list_Prot)
	FOR idnum:= 0 TO numlist-1
		doMethodA(list_Prot ,[MUIM_NList_GetEntry,idnum,{identry}])
		testnom:=String(StrLen(identry.nom)+2)
		StrCopy(testnom,identry.nom,ALL)
		IF StrCmp(testnom,'\e3',2) THEN ndirs++ ELSE nfiles++
		DisposeLink(testnom)
	ENDFOR
ENDPROC

->**********************
PROC vid()
	id:=MUIV_NList_NextSelected_Start
ENDPROC

/*******************************************************************
 ******************** MUIV_Lamp_Color_Processing *******************/

PROC proc_StartProcess()
	set(lamp, MUIA_Lamp_Color, MUIV_Lamp_Color_Processing)
ENDPROC

/*********************** MUIV_Lamp_Color_Off ************************/

PROC proc_EndProcess()
	set(lamp, MUIA_Lamp_Color, MUIV_Lamp_Color_Off)
ENDPROC

/*********************** MUIV_Lamp_Color_Off ************************/

PROC proc_OkProcess()
	set(lamp, MUIA_Lamp_Color, MUIV_Lamp_Color_Ok)
ENDPROC

/*********************************************************************
 ********************************************************************/
->****************

PROC proc_Analyse(cible:PTR TO oListData)
DEF	tempnom,str[512]:STRING

	IF (arret=0)
		tempnom:=String(StrLen(cible.nom))
		StrCopy(tempnom,cible.nom,ALL)
		IF StrCmp(tempnom,'\e3',2) THEN MidStr(tempnom,tempnom,2,StrLen(tempnom)-2)
		StringF(str,'\s\s',cible.chemin,tempnom)

		refresh()

		setprotect(str)
		vnumlist(list_Prot)

		refresh()
		DisposeLink(tempnom)
	ENDIF
ENDPROC

->***************

PROC init_Selected(num:LONG,cible:PTR TO oListData)

DEF tempnom,str[512]:STRING
		tempnom:=String(StrLen(cible.nom)+1)
		StrCopy(tempnom,cible.nom,ALL)
		StrCopy(bufchemin,cible.chemin,ALL)

		IF StrCmp(tempnom,'\e3',2) THEN MidStr(tempnom,tempnom,2,StrLen(tempnom)-2)
		IF StrCmp(tempnom,bufchemin,ALL)=TRUE
			StrCopy(str,cible.chemin,ALL)
		ELSE
			StringF(str,'\s\s',cible.chemin,tempnom)
		ENDIF
		doMethodA(list_Prot, [MUIM_NList_Remove, num])

		d1(kPrintF('\s/init_Selected(()/str=\s numlist=\d num=\d cible - \s\n', [THE_FILE, str, numlist,num,cible.nom]))

		proc_CountDirFiles()

		getpathdir(str)

		refresh()

		check_HiddenBit(str)

		refresh()

		DisposeLink(tempnom)
ENDPROC

->***************

PROC getargs()
DEF 	the_File[300]:STRING, rdargs, myargs:PTR TO LONG, wb:PTR TO wbstartup, args:PTR TO wbarg,buf_Args[256]:STRING

wb_message:=1
	IF wbmessage<>NIL				/* from wb */
    		wb:=wbmessage
   		args:=wb.arglist
    		NameFromLock(args[0].lock,the_File,StrMax(the_File))
    		IF StrLen(args[0].name)>0 THEN AddPart(the_File, args[0].name,StrMax(the_File))
    		IF wb.numargs > 1
			IF wb.numargs >= 2

				set(win, MUIA_Window_Open, MUI_TRUE)

        			FOR rdargs := 1 TO wb.numargs-1
        				NameFromLock(args[rdargs].lock,the_File,StrMax(the_File))
					IF StrLen(args[rdargs].name)>0 THEN AddPart(the_File, args[rdargs].name,StrMax(the_File))
					AddPart(the_File,'', StrMax(the_File))
					numobj++

					d1(kPrintF('\s/getargs()/wb.numargs=[\d] the_File=<\s>\n', [THE_FILE, wb.numargs,the_File]))

					refresh()

					-> WriteF('\s\n',the_File)

					-> StringF(buf_Search, (cat.msg_Searching.getstr() ),the_File)

					-> set(txt_HeadSplash, MUIA_Text_Contents, buf_Search)

					-> WriteF('\s\n',buf_Search)

					inspect(the_File)
					refresh()

                        		UnLock(args[rdargs].lock)
                        		args[rdargs].lock := NIL
        			ENDFOR
			ENDIF
		ELSE
			Raise(WriteF(cat.msg_ArgNoFound.getstr() ))
		ENDIF

		d1(kPrintF('\s/getargs()/wb.numargs=[\d] numobj=[\d] - the_File=\s\n', [THE_FILE, wb.numargs, numobj, the_File]))

	ELSE
		myargs:=[0]

		StringF(buf_Args,'\s\nPATH/A',VERSTAG)

		IF (rdargs:=ReadArgs(buf_Args ,myargs,0))>0
			IF myargs[0]
				StrCopy(the_File,myargs[0],ALL)
			ENDIF
			IF rdargs THEN FreeArgs(rdargs)
		ENDIF

		IF StrLen(the_File) > 0 THEN set(win, MUIA_Window_Open, MUI_TRUE) ELSE Raise(WriteF( cat.msg_NoWBargsFound.getstr() ))

		refresh()
		inspect_cli(the_File)
		refresh()
	ENDIF

	IF (numlist > 0) THEN set(menuopen, MUIA_Menuitem_Enabled,1) ELSE set(menuopen, MUIA_Menuitem_Enabled,0)

	d1(kPrintF('\s/proc_Analyse()/numlist=\d \n', [THE_FILE, numlist]))

	set(bt_Arret, MUIA_Disabled, 1)
wb_message:=0
ENDPROC

PROC inspect(the_File)
DEF	fib:PTR TO fileinfoblock, isfile=FALSE, my_File, s_lock

	set(bt_Arret, MUIA_Disabled, 0)

	StringF(buf_Search, (cat.msg_Searching.getstr() ),the_File)
	set(txt_HeadSplash, MUIA_Text_Contents, buf_Search)

	fib := AllocDosObject(DOS_FIB, NIL)

	IF s_lock:=Lock(the_File, ACCESS_READ)
        	Examine(s_lock, fib)
		my_File := String(StrLen(the_File)+2)
		NameFromLock(s_lock,my_File, StrMax(my_File))

		set(txt_Splash, MUIA_Text_Contents, my_File)

        	IF(fib.entrytype<0) -> Fichier
			isfile:=TRUE
		ENDIF

		d1(kPrintF('\s/inspect(\s)/fib.entrytype=\d isfile=\d\n', [THE_FILE, the_File, fib.entrytype, isfile]))

		-> WriteF('\s: Please wait...\n',VERSTRING)

		IF (isfile)
			-> WriteF('*** Analyze file: "\s"\n', the_File)
        		getprotect(my_File)
		ELSE
			-> WriteF('*** Analyze content of: "\s"\n', the_File)
			getprotect(my_File)
			refresh()
			exdir(my_File)
			refresh()

		ENDIF
		UnLock(s_lock)
		DisposeLink(my_File)
		proc_EndProcess()

		get(win, MUIA_Window_Open, {opened})		-> +++
		IF opened
			IF (arret=1) THEN set(bt_Arret, MUIA_Disabled, 1)
			set(bt_UnHide, MUIA_Disabled, 0)

			IF (set_UnHide)
				proc_CountDirFiles()
			ELSE
				set(txt_Splash,     MUIA_Text_Contents, cat.msg_AnyHiddenFound.getstr() )
				set(bt_UnHide, MUIA_Disabled, 1)
			ENDIF
			IF (arret=1) THEN set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_UserAborted.getstr() ) ELSE set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_Completed.getstr() )
		ENDIF
	ENDIF

	FreeDosObject(DOS_FIB, fib)
ENDPROC

PROC inspect_cli(the_File)
DEF	s_lock, fib:PTR TO fileinfoblock, isfile=FALSE, my_File

	fib := AllocDosObject(DOS_FIB, NIL)

	IF s_lock:=Lock(the_File, ACCESS_READ)
        	Examine(s_lock, fib)
		my_File := String(StrLen(the_File)+256)
		NameFromLock(s_lock,my_File, StrMax(my_File))

		set(txt_Splash, MUIA_Text_Contents, my_File)

        	IF(fib.entrytype<0) -> Fichier
			isfile:=TRUE
		ENDIF

		d1(kPrintF('\s/inspect(\s)/fib.entrytype=\d isfile=\d\n', [THE_FILE, the_File, fib.entrytype, isfile]))

		-> WriteF('\s: Please wait...\n',VERSTRING)
		IF (isfile)
			-> WriteF('*** Analyze file: "\s"\n', the_File)
        		getprotect(my_File)
		ELSE
			-> WriteF('*** Analyze content of: "\s"\n', the_File)
			getprotect(my_File)
			exdir(my_File)

		ENDIF

		-> WriteF('\n*** ANALYZE COMPLETED ! ***\n')
		UnLock(s_lock)
		DisposeLink(my_File)
		proc_EndProcess()
	ELSE
		Fault(IoErr(),NIL,textbuffer,120)
		WriteF(cat.msg_IoErrorFile.getstr(), the_File, IoErr(), textbuffer)
		CleanUp(20)
	 ENDIF
	FreeDosObject(DOS_FIB, fib)
ENDPROC

PROC exdir(dir)
DEF fib:PTR TO fileinfoblock,dirlock=NIL,olddir = NIL,buf_File[300]:STRING

       	fib := AllocDosObject(DOS_FIB, NIL)

	IF dirlock := Lock(dir,-2)
		olddir := CurrentDir(dirlock)

        	IF (Examine(dirlock, fib))
       			WHILE ExNext(dirlock, fib) AND (arret=0) AND (CheckSignal(SIGBREAKF_CTRL_C)=0)

			NameFromLock(dirlock,buf_File, StrMax(buf_File))
			-> IF StrLen(fib.filename)>0 THEN AddPart(buf_File, fib.filename,StrMax(buf_File))
			AddPart(buf_File,'', StrMax(buf_File))

			StringF(buf_Search, (cat.msg_Txt_DirSplash.getstr() ),buf_File)
			set(txt_DirSplash, MUIA_Text_Contents, buf_Search)

				set(txt_Splash, MUIA_Text_Contents, fib.filename)
	       			IF(fib.entrytype<0) -> Fichier
					refresh()
                			getprotect(fib.filename)
					refresh()
        			ELSE					-> Répertoire
					refresh()
					getprotect(fib.filename)
					refresh()
        				exdir(fib.filename)
					refresh()
        			ENDIF
				IF CheckSignal(SIGBREAKF_CTRL_C)<>0 THEN arret:=1
      			ENDWHILE
    		ENDIF
		UnLock(dirlock)
	  	CurrentDir(olddir)

	ENDIF

	FreeDosObject(DOS_FIB, fib)
ENDPROC

->----------------------------------------------------

PROC refresh()
	doMethodA(app,[MUIM_Application_InputBuffered])
ENDPROC

->**************

PROC check_HiddenBit(tampon)
DEF	fib:PTR TO fileinfoblock,lock_file=NIL, ah=0, as=0, ap=0, aa=0, ar=0, aw=0, ae=0, ad=0
DEF	name, full_Path[512]:STRING, buf_Prot[9]:STRING

	StringF(buf_Prot,'')
	name:=String(StrLen(tampon))
	StrCopy(name, tampon, ALL)

       	fib := AllocDosObject(DOS_FIB, NIL)

	IF lock_file:=Lock(name, -2)

 		Examine(lock_file,fib)

		NameFromLock(lock_file, full_Path, StrMax(full_Path))
		-> IF fib.entrytype = 2
		-> 	AddPart(full_Path, '', '')
		-> ENDIF

		d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - full_Path=<\s>\n', [THE_FILE, fib.entrytype,full_Path]))

 		ah := IF fib.protection AND FIBF_HIDDEN		THEN 1 ELSE 0
		as := IF fib.protection AND FIBF_SCRIPT 	THEN 1 ELSE 0
 		ap := IF fib.protection AND FIBF_PURE		THEN 1 ELSE 0
 		aa := IF fib.protection AND FIBF_ARCHIVE	THEN 1 ELSE 0
 		ar := IF fib.protection AND FIBF_READ 		THEN 0 ELSE 1
 		aw := IF fib.protection AND FIBF_WRITE 		THEN 0 ELSE 1
 		ae := IF fib.protection AND FIBF_EXECUTE 	THEN 0 ELSE 1
 		ad := IF fib.protection AND FIBF_DELETE 	THEN 0 ELSE 1


		d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - Name=<\s> HIDDEN=\d SCRIPT=\d PURE =\d ARCHIVE=\d READ=\d WRITE=\d EXECUTE=\d DELETE=\d - set_UnHide = \d\n', [THE_FILE, fib.entrytype,FilePart(full_Path),ah,as,ap,aa,ar,aw,ae,ad,set_UnHide]))


		proc_Installbits(buf_Prot, ah, as, ap, aa, aw, ar, ae, ad)


		d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - Name=<\s> New protection = \s\n', [THE_FILE, fib.entrytype,FilePart(full_Path), buf_Prot]))

		d1(kPrintF('\s/getprotect(\s)\n', [THE_FILE, name]))


		Delay(10)
		countobj(name, buf_Prot)
		-> set(txt_Splash, MUIA_Text_Contents, name)

		UnLock(lock_file)

		DisposeLink(name)
	ENDIF
	FreeDosObject(DOS_FIB, fib)
ENDPROC

->**************

PROC getprotect(file)
DEF	fib:PTR TO fileinfoblock ,lock_file
DEF	ah=0, as=0, ap=0, aa=0, ar=0, aw=0, ae=0, ad=0
DEF	name, full_Path[512]:STRING, buf_Prot[8]:STRING, s, h

	StringF(buf_Prot,'')
	name:=String(StrLen(file))
	StrCopy(name, file, ALL)

       	fib := AllocDosObject(DOS_FIB, NIL)

	IF lock_file:=Lock(name, -2)

 		Examine(lock_file,fib)

		NameFromLock(lock_file, full_Path, StrMax(full_Path))

		-> IF fib.entrytype = 2
		-> 	AddPart(full_Path, '', '')
		-> ENDIF

		d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - full_Path=<\s>\n', [THE_FILE, fib.entrytype,full_Path]))

 
 		ah := IF fib.protection AND FIBF_HIDDEN THEN 1 ELSE 0

		IF (ah=1)
			proc_OkProcess()
			set_UnHide:=TRUE
			as := IF fib.protection AND FIBF_SCRIPT 	THEN 1 ELSE 0
 			ap := IF fib.protection AND FIBF_PURE		THEN 1 ELSE 0
 			aa := IF fib.protection AND FIBF_ARCHIVE	THEN 1 ELSE 0
 			ar := IF fib.protection AND FIBF_READ 		THEN 0 ELSE 1
 			aw := IF fib.protection AND FIBF_WRITE 		THEN 0 ELSE 1
 			ae := IF fib.protection AND FIBF_EXECUTE 	THEN 0 ELSE 1
 			ad := IF fib.protection AND FIBF_DELETE 	THEN 0 ELSE 1


			d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - Name=<\s> HIDDEN=\d SCRIPT=\d PURE =\d ARCHIVE=\d READ=\d WRITE=\d EXECUTE=\d DELETE=\d - set_UnHide = \d\n', [THE_FILE, fib.entrytype,FilePart(full_Path),ah,as,ap,aa,ar,aw,ae,ad,set_UnHide]))


			proc_Installbits(buf_Prot, ah, as, ap, aa, aw, ar, ae, ad)


			d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - Name=<\s> New protection = \s\n', [THE_FILE, fib.entrytype,FilePart(full_Path), buf_Prot]))

			d1(kPrintF('\s/getprotect(\s)\n', [THE_FILE, name]))

			-> countobj(name, buf_Prot)

			getpathdir(name)
			-> intypes(name,list_Prot,buf_Prot)

-> ------- intypes(ftypesdir, obj, protection) --------

			StrCopy(bufchemin,path,ALL)
			IF bufchemin[StrLen(bufchemin)-1]<>":" THEN StrAdd(bufchemin,'/')

			CopyMem(fib.datestamp,dt.stamp,SIZEOF datestamp)
			dt.format:=FORMAT_DOS
			dt.flags:=DTF_FUTURE
			dt.strday:=day
			dt.strdate:=date
			dt.strtime:=time
			IF DateToStr(dt)
				s:=String(100)
				h:=String(100)
				StringF(s,'\s \s',day,date)
				StringF(h,'\s',time)
			ENDIF
			IF fib.direntrytype<0
				StrCopy(bufnom,fib.filename)
				StringF(buftaille,'\d',fib.size)
			ELSE
				IF name[StrLen(name)-1]=":"
					StringF(bufnom,'\e3\s',name)
					StringF(buftaille,'\ecI[6:26]')
				ELSE
					StringF(bufnom,'\e3\s',fib.filename)
					StringF(buftaille,'\ecI[6:22]')
				ENDIF
	       		ENDIF

			new:=[bufnom,buf_Prot,buftaille,s,h,bufchemin]:oListData
			doMethodA(list_Prot, [MUIM_NList_InsertSingle,new,MUIV_NList_Insert_Sorted])

			refresh()
			proc_CountDirFiles()
			refresh()

			DisposeLink(s)
			DisposeLink(h)

			d1(kPrintF('\s/intypes() bufnom = \s protection = \s\n buftaille = \s S = \s H = \s\n bufchemin = \s\n', [THE_FILE, bufnom, protection, buftaille, s, h, bufchemin]))

-> ---------------

			-> set(txt_Splash, MUIA_Text_Contents, name)

		ELSE
			proc_StartProcess()
		ENDIF
		UnLock(lock_file)

		DisposeLink(name)
	ENDIF

	FreeDosObject(DOS_FIB, fib)
ENDPROC

PROC proc_Installbits(buffer_Prot, hide, script, pure, archive, read, write, execute, delete)

	StringF(buffer_Prot,'')

	IF hide=1	THEN StrAdd(buffer_Prot,'H') ELSE StrAdd(buffer_Prot,'-')

	IF script=1	THEN StrAdd(buffer_Prot,'S') ELSE StrAdd(buffer_Prot,'-')

	IF pure=1	THEN StrAdd(buffer_Prot,'P') ELSE StrAdd(buffer_Prot,'-')

	IF archive=1	THEN StrAdd(buffer_Prot,'A') ELSE StrAdd(buffer_Prot,'-')

	IF read=0	THEN StrAdd(buffer_Prot,'-') ELSE StrAdd(buffer_Prot,'R')

	IF write=0 	THEN StrAdd(buffer_Prot,'-') ELSE StrAdd(buffer_Prot,'W')

	IF execute=0	THEN StrAdd(buffer_Prot,'-') ELSE StrAdd(buffer_Prot,'E')

	IF delete=0	THEN StrAdd(buffer_Prot,'-') ELSE StrAdd(buffer_Prot,'D')
ENDPROC

PROC setprotect(file)
DEF	fib:PTR TO fileinfoblock,lock_file=NIL
DEF	ah=0, as=0, ap=0, aa=0, ar=0, aw=0, ae=0, ad=0
DEF	nprot=NIL, pt=FALSE, name, full_Path[512]:STRING

	name:=String(StrLen(file))
	StrCopy(name, file, ALL)

       	fib := AllocDosObject(DOS_FIB, NIL)

	IF lock_file:=Lock(name, -2)

 		Examine(lock_file,fib)

		NameFromLock(lock_file, full_Path, StrMax(full_Path))
		-> IF fib.entrytype = 2
		-> 	AddPart(full_Path, '', '')
		-> ENDIF

		d1(kPrintF('\s/getprotect()/fib.entrytype=[\d] - full_Path=<\s>\n', [THE_FILE, fib.entrytype,full_Path]))

		ah := IF fib.protection AND FIBF_HIDDEN 	THEN 1 ELSE 0
		as := IF fib.protection AND FIBF_SCRIPT 	THEN 1 ELSE 0
 		ap := IF fib.protection AND FIBF_PURE		THEN 1 ELSE 0
		aa := IF fib.protection AND FIBF_ARCHIVE	THEN 1 ELSE 0
 		ar := IF fib.protection AND FIBF_READ 		THEN 0 ELSE 1
 		aw := IF fib.protection AND FIBF_WRITE 		THEN 0 ELSE 1
 		ae := IF fib.protection AND FIBF_EXECUTE 	THEN 0 ELSE 1
 		ad := IF fib.protection AND FIBF_DELETE 	THEN 0 ELSE 1

		d1(kPrintF('\s/setprotect()/fib.entrytype=[\d] - Name=<\s> HIDDEN=\d SCRIPT=\d PURE =\d ARCHIVE=\d READ=\d WRITE=\d EXECUTE=\d DELETE=\d - set_UnHide = \d\n', [THE_FILE, fib.entrytype,FilePart(full_Path),ah,as,ap,aa,ar,aw,ae,ad,set_UnHide]))

	    	IF ah=1 THEN nprot:=(fib.protection - FIBF_HIDDEN)

	    	IF as=1 THEN nprot:= nprot + FIBF_SCRIPT

	    	IF ap=1 THEN nprot:= nprot + FIBF_PURE

	    	IF aa=1 THEN nprot:= nprot - FIBF_ARCHIVE

	    	IF ar=0 THEN nprot:= nprot + FIBF_READ

	    	IF aw=0 THEN nprot:= nprot + FIBF_WRITE

	    	IF ae=0 THEN nprot:= nprot + FIBF_EXECUTE

	    	IF ad=0 THEN nprot:= nprot + FIBF_DELETE

		d1(kPrintF('\s/setprotect()/fib.entrytype=[\d] - Name=<\s> nprot = \z\h[8]\n', [THE_FILE, fib.entrytype,FilePart(full_Path),nprot]))


		set(txt_Splash, MUIA_Text_Contents, name)

	    	pt:=SetProtection(name, nprot)

		d1(kPrintF('\s/getprotect(\s)\n', [THE_FILE, name]))

		IF pt:=FALSE
		 	Fault(IoErr(),NIL,textbuffer,120)
			WriteF(cat.msg_IoErrorFullPath.getstr() , full_Path, IoErr(), textbuffer)
		ENDIF
		nprot:=NIL
		UnLock(lock_file)

	ENDIF
	DisposeLink(name)

	FreeDosObject(DOS_FIB, fib)

ENDPROC

->*********************

PROC countobj(str,bprotection)
	d1(kPrintF('\s/countobj(\s)\n', [THE_FILE, str]))

	getpathdir(str)
	intypes(str,list_Prot,bprotection)

	d1(kPrintF('\n\s/countobj() files=\d dirs = \d numobj = \d\n', [THE_FILE, nfiles, ndirs, numobj]))
ENDPROC

->*********************

PROC intypes(ftypesdir, obj, protection)
  DEF 	fib:PTR TO fileinfoblock,s,h, lock=NIL

  StrCopy(bufchemin,path,ALL)
  IF bufchemin[StrLen(bufchemin)-1]<>":" THEN StrAdd(bufchemin,'/')

  fib := AllocDosObject(DOS_FIB, NIL)

  IF lock:=Lock(ftypesdir,-2)

   	Examine(lock, fib)

	CopyMem(fib.datestamp,dt.stamp,SIZEOF datestamp)
	dt.format:=FORMAT_DOS
	dt.flags:=DTF_FUTURE
	dt.strday:=day
	dt.strdate:=date
	dt.strtime:=time
	IF DateToStr(dt)
		s:=String(100)
		h:=String(100)
		StringF(s,'\s \s',day,date)
		StringF(h,'\s',time)
	ENDIF
	IF fib.direntrytype<0
		StrCopy(bufnom,fib.filename)
		StringF(buftaille,'\d',fib.size)
	ELSE
		IF ftypesdir[StrLen(ftypesdir)-1]=":"
			StringF(bufnom,'\e3\s',ftypesdir)
			StringF(buftaille,'\ecI[6:26]')
		ELSE
			StringF(bufnom,'\e3\s',fib.filename)
			StringF(buftaille,'\ecI[6:22]')
		ENDIF
       	ENDIF

	new:=[bufnom,protection,buftaille,s,h,bufchemin]:oListData
	doMethodA(obj,[MUIM_NList_InsertSingle,new,MUIV_NList_Insert_Sorted])

	DisposeLink(s)
	DisposeLink(h)

	d1(kPrintF('\s/intypes() bufnom = \s protection = \s\n buftaille = \s S = \s H = \s\n bufchemin = \s\n', [THE_FILE, bufnom, protection, buftaille, s, h, bufchemin]))

    	UnLock(lock)
  ENDIF

  FreeDosObject(DOS_FIB, fib)
ENDPROC

->**********************

PROC getpathdir(fichier)
DEF newlock2,lock=NIL,p
	IF fichier[StrLen(fichier)-1]<>":"
		IF(lock := Lock(fichier, SHARED_LOCK)) <> 0
			IF (newlock2:=ParentDir(lock)) <> 0
				p:=String(500)
				NameFromLock(newlock2,p,500)
				StrCopy(path,p,ALL)
				UnLock(newlock2)
				DisposeLink(p)
			ENDIF
			UnLock(lock)
		ENDIF
	ELSE
		StrCopy(path,fichier,ALL)
	ENDIF
	-> WriteF('getpathdir(\s) = \s\n',fichier, path)
ENDPROC

/************************************************************************************/

PROC vnumlist(obj)
	get(obj,MUIA_NList_Entries,{numlist})
ENDPROC

/************************************************************************************/

/******************************** CONTEXTMENU ********************************/

PROC contextmenu_dispatcher(hook, cl:PTR TO iclass, obj:PTR TO object,msg:PTR TO msg)
DEF state,member
	IF msg.methodid = MUIM_ContextMenuChoice
		get(obj,MUIA_Group_ChildList,{state})
		NextObject({state})
		WHILE (member:=NextObject({state}))
			doMethodA(member,msg)
		ENDWHILE
		RETURN NIL
	ENDIF
ENDPROC doSuperMethodA(cl,obj,msg)

/*****************************************************************************/

-> params[0] = the MenuitemObject the user selected in the context menu
PROC contextmenu_func(hook, userdata, params:PTR TO LONG)
DEF 	o:PTR TO object, act_Open=NIL:PTR TO oListData

	o:=params[0]
	userdata:=muiUserData(o)

	SELECT userdata

		CASE OP_DIR
			IF (numlist=1)
				act_Open.chemin := bufchemin
				open_Dir(act_Open.chemin)
			ELSE
				vid()
				REPEAT
					refresh()
					doMethodA(list_Prot, [MUIM_NList_NextSelected,{id}])
					doMethodA(list_Prot, [MUIM_NList_GetEntry,id,{act_Open}])
					IF (id<>-1) AND (arret=0)
						open_Dir(act_Open.chemin)
					ENDIF
				UNTIL(id=MUIV_NList_NextSelected_End)
			ENDIF

			d1(kPrintF('\s/contextmenu_func()/ id=\d act_Open.chemin = \s\n', [THE_FILE, id, act_Open.chemin]))

	ENDSELECT
ENDPROC

/************************** ScA_OpenIconWindow() ************************************/

PROC open_Dir(the_Path)
	ScA_OpenIconWindow([SCA_Path, the_Path,
		SCA_ViewModes, 0,
		SCA_ShowAllMode, 0,
		TAG_END])
ENDPROC

/*************************************************************************************/
/******************************* HOOKLVDISPLAY ***************************************/

PROC hooklvdisplay(hook, array:PTR TO LONG, entry:PTR TO oListData)
DEF	s_Disk=0, s_Dir=0, aide_Nom, aide_Protect, aide_Size, aide_Date, aide_Heure, aide_Chemin

	IF entry=0                 	-> Titles
		array[0]:= (cat.msg_TitleName.getstr() )
		array[1]:= (cat.msg_TitleProtect.getstr() )
		array[2]:= (cat.msg_TitleSise.getstr() )
		array[3]:= (cat.msg_TitleDate.getstr() )
		array[4]:= (cat.msg_TitleTime.getstr() )
		array[5]:= (cat.msg_TitlePath.getstr() )
	ELSE                       	-> normal entries
		array[0]:=entry.nom
		array[1]:=entry.protect
		array[2]:=entry.taille
		array[3]:=entry.ladate
		array[4]:=entry.heure
		array[5]:=entry.chemin

		-> get(list_Prot,MUIA_NList_Entries,{numlist})
		-> viewnum()

		aide_Nom	:= (cat.msg_BubbleName.getstr() )	-> 'Name:'
		aide_Protect	:= (cat.msg_BubbleProtect.getstr() )	-> 'Access: '
		aide_Size	:= (cat.msg_BubbleSize.getstr() )	-> 'Size:   '
		aide_Date	:= (cat.msg_BubbleDate.getstr() )	-> 'Date:   '
		aide_Heure	:= (cat.msg_BubbleTime.getstr() )	-> 'Time:   '
		aide_Chemin	:= (cat.msg_BubblePath.getstr() )	-> 'Path:   '

		StrCopy(bufnom,		array[0], ALL)
		StrCopy(bufprotect,	array[1], ALL)
		StrCopy(buftaille,	array[2], ALL)
		StrCopy(bufstamp,	array[3], ALL)
		StrCopy(bufheure,	array[4], ALL)
		StrCopy(bufchemin,	array[5], ALL)

		IF StrCmp(bufnom,'\e3',2)=TRUE 		THEN MidStr(bufnom,bufnom,2,StrLen(bufnom)-2)
		-> IF StrCmp(bufprotect,'H',1)=FALSE	THEN set(bt_UnHide, MUIA_Disabled,1) ELSE set(bt_UnHide, MUIA_Disabled,0)


		s_Disk := InStr(buftaille, '6:26', 0)
		IF s_Disk <> -1 THEN StrCopy(buftaille, cat.msg_BubbleVolume.getstr() )	-> 'Volume'

		s_Dir := InStr(buftaille, '6:22', 0)
		IF s_Dir <> -1 THEN StrCopy(buftaille, cat.msg_BubbleDir.getstr() )	-> 'Directory'

		d1(kPrintF('\s/hooklvdisplay()/buftaille = \s - s_Disk=\d s_Dir =\d\n', [THE_FILE, buftaille, s_Disk, s_Dir]))

		bufstring:= String(StrLen(bufchemin) + StrLen(bufnom) + StrLen(bufprotect) + StrLen(buftaille) + StrLen(bufstamp) + StrLen(bufheure) + StrLen(aide_Nom) + StrLen(aide_Protect) + StrLen(aide_Size) + StrLen(aide_Date) + StrLen(aide_Heure) + StrLen(aide_Chemin) + 70)

	        StringF(bufstring,'\s\eb\s\en\n\s\ei\s\en\n\s\eb\s\en\n\s\s\n\s\s\n\s\s', aide_Nom, bufnom, aide_Chemin, bufchemin, aide_Protect, bufprotect, aide_Size, buftaille, aide_Date, bufstamp, aide_Heure, bufheure)

		set(list_Prot, MUIA_ShortHelp, bufstring)
	ENDIF
ENDPROC

/*************** Affichage du nombre d'entrées **************************************/

PROC viewnum()
DEF str_num[80]:STRING
	StringF(str_num,'\d',numlist)
	set(txt_Num, MUIA_Text_Contents, str_num)

ENDPROC

/*********************************  HOOKLVCONSTRUCT  *****************************/

PROC hooklvconstruct(hook, pool, entry:PTR TO oListData)
DEF  new=NIL:PTR TO oListData
	IF (new:=AllocPooled(pool, SIZEOF oListData))
		new.nom:=AllocPooled(pool, StrLen(entry.nom)+1)
		AstrCopy(new.nom, entry.nom)

		new.protect:=AllocPooled(pool, StrLen(entry.protect)+1)
		AstrCopy(new.protect, entry.protect)

		new.taille:=AllocPooled(pool, StrLen(entry.taille)+1)
		AstrCopy(new.taille, entry.taille)

		new.ladate:=AllocPooled(pool, StrLen(entry.ladate)+1)
		AstrCopy(new.ladate, entry.ladate)

		new.heure:=AllocPooled(pool, StrLen(entry.heure)+1)
		AstrCopy(new.heure, entry.heure)

		new.chemin:=AllocPooled(pool, StrLen(entry.chemin)+1)
		AstrCopy(new.chemin, entry.chemin)
	ENDIF
ENDPROC new

/*********************************  HOOKLVDESTRUCT  *****************************/

PROC hooklvdestruct(hook, pool, entry:PTR TO oListData)
	IF entry.nom     THEN FreePooled(pool, entry.nom, StrLen(entry.nom)+1)
	IF entry.protect THEN FreePooled(pool, entry.protect, StrLen(entry.protect)+1)
	IF entry.taille  THEN FreePooled(pool, entry.taille, StrLen(entry.taille)+1)
	IF entry.ladate    THEN FreePooled(pool, entry.ladate, StrLen(entry.ladate)+1)
	IF entry.heure   THEN FreePooled(pool, entry.heure, StrLen(entry.heure)+1)
	IF entry.chemin  THEN FreePooled(pool, entry.chemin, StrLen(entry.chemin)+1)
	FreePooled(pool, entry, SIZEOF oListData)
ENDPROC

/*********************************  HOOKLVCOMPARE  ******************************/

-> does not work correctly for NList
PROC hooklvcompare(hook, o1:PTR TO oListData, o2:PTR TO oListData)
DEF mode

	get(cySort,MUIA_Cycle_Active,{mode})
	SELECT mode
		CASE  0;   RETURN 1;
		CASE  1;   RETURN OstrCmp(o2.nom, o2.nom);
		CASE  2;   RETURN OstrCmp(o1.protect, o2.protect);
		CASE  3;   RETURN OstrCmp(o1.taille, o2.taille);
		CASE  4;   RETURN OstrCmp(o1.ladate, o2.ladate);
		CASE  5;   RETURN OstrCmp(o1.heure, o2.heure);
		CASE  6;   RETURN OstrCmp(o1.chemin, o2.chemin);
	ENDSELECT
ENDPROC

/*********************************  HOOKLVCOMPARE2  *****************************/

PROC hooklvcompare2(hook, entry, msg:PTR TO nlist_comparemessage)
DEF 	scp1:PTR TO oListData,scp2:PTR TO oListData
	scp1:=msg.entry1
	scp2:=msg.entry2

RETURN 	Stricmp(scp1.nom,scp2.nom)
       	Stricmp(scp1.protect,scp2.protect)
       	Stricmp(scp1.taille,scp2.taille)
       	Stricmp(scp1.ladate,scp2.ladate)
       	Stricmp(scp1.heure,scp2.heure)
       	Stricmp(scp1.chemin,scp2.chemin)

ENDPROC


/**********************************************************************************/


PROC appMsgFunc(hook, obj, x:PTR TO LONG)
DEF	ap:PTR TO wbarg, amsg:PTR TO appmessage, i, buf[300]:STRING, b,
	bufapp[300]:STRING, str[300]:STRING,buf_ApName[256]:STRING

IF wb_message=0
	set_UnHide:=FALSE
	set(bt_Arret, MUIA_Disabled, 0)
	set(bt_UnHide, MUIA_Disabled, 1)
	doMethodA(obj, [MUIM_NList_Clear])
	set(txt_Files, MUIA_Text_Contents, '0')
	set(txt_Dirs, MUIA_Text_Contents,  '0')
	set(txt_Num, MUIA_Text_Contents, '0')

  	amsg:=x[]
  	b:=buf;i:=0
  	ap:=amsg.arglist

	arret:=0
  	WHILE (i<amsg.numargs) AND (arret=0)
    		NameFromLock(ap.lock,buf,StrMax(buf))
    		AddPart(buf,ap.name,StrMax(buf))
    		StringF(bufapp,'\s',b)
    		StrCopy(str,b,ALL)
		StringF(buf_Search, cat.msg_Searching.getstr() ,str)
		set(txt_HeadSplash, MUIA_Text_Contents, buf_Search)
		refresh()
    		inspect(str)

		StringF(buf_ApName,'"\s" \s',bufapp, cat.msg_Completed.getstr() )	-> +++
		set(txt_HeadSplash, MUIA_Text_Contents, buf_ApName)			-> +++

		refresh()
    		ap++
    		i++
  	ENDWHILE

	vnumlist(obj)

	IF (set_UnHide)
		proc_CountDirFiles()
	ELSE
		set(txt_Splash,     MUIA_Text_Contents, cat.msg_AnyHiddenFound.getstr() )
		set(bt_UnHide, MUIA_Disabled, 1)
		viewnum()
	ENDIF

	get(app,MUIA_Application_Iconified,{icone})
	IF icone THEN set(app,MUIA_Application_Iconified, FALSE)

	IF (arret=1) THEN set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_UserAborted.getstr() ) /* ELSE set(txt_HeadSplash, MUIA_Text_Contents, cat.msg_Completed.getstr() ) */

	doMethodA(obj, [MUIM_NList_Redraw,MUIV_NList_Redraw_All]) -> ou MUIV_NList_Redraw_Title

ENDIF
ENDPROC 

/**********************************************************************************/

PROC makeLV(fixh,ajusth,ttxt,tf,msel,h,context) IS NListviewObject,
		MUIA_Height, fixh,
		MUIA_NList_AdjustHeight, ajusth,
                MUIA_CycleChain,MUI_TRUE,
                MUIA_NListview_NList, NListObject,
		MUIA_NList_Title, ttxt,
                MUIA_Frame, tf,
		MUIA_NList_MultiSelect, msel,
		MUIA_ShortHelp, h,
		MUIA_NList_Format, 'BAR,BAR,BAR,BAR,BAR,BAR',
                MUIA_NList_CompareHook, hook_lv_compare,
                MUIA_NList_CompareHook2, hook_lv_compare2,
                MUIA_NList_ConstructHook, hook_lv_construct,
                MUIA_NList_DestructHook, hook_lv_destruct,
                MUIA_NList_DisplayHook, hook_lv_display,
		MUIA_ContextMenu, context,
              End,
            End

PROC makeTXT(w,tf,bk,type,tx,c,j,h) IS TextObject,
	       	MUIA_Weight, w,
              	MUIA_Frame, tf,
	      	MUIA_Background , bk,
		MUIA_InputMode, type,
              	MUIA_Text_Contents, tx,
		MUIA_ControlChar, c,
              	MUIA_Text_PreParse , j ,
	        MUIA_ShortHelp, h,
	End


CHAR VERSTAG, ' - Compiled with: ', COMPILER_STRING
