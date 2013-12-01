OPT PREPROCESS, LARGE, STACK=32000

MODULE  'workbench/startup','workbench/workbench','dos/datetime','dos/dos','utility/hooks','utility',
	'tools/file', 'muimaster','libraries/mui', 'mui/betterString_mcc','libraries/gadtools',
	'utility/tagitem','tools/installhook','icon','libraries/asl','locale','libraries/locale','ASL',
	'*Scalos_Comment_Locale','*VERSTAG','mui/nlist_mcc','mui/nlistview_mcc','mui/lamp_mcc',
	'dos/datetime','intuition/classusr','intuition/classes','amigalib/boopsi','*getpath','*GeticonObject',
	'scalos/scalos41.6','scalos'

#define prefile 'ENV:Scalos/Comment_Module.prefs'

OBJECT disk
	size
        used
        free
        blksize
ENDOBJECT

OBJECT file
  name
  pname
  comment
  protection
  numblocks
  size
  datestamp:datestamp
  infodata:infodata
  type
  stack
ENDOBJECT

OBJECT oListData
   nom     : PTR TO CHAR
   comm    : PTR TO CHAR
   taille  : PTR TO CHAR
   date    : PTR TO CHAR
   heure   : PTR TO CHAR
   chemin  : PTR TO CHAR
ENDOBJECT

ENUM ER_MUILIB=1,ER_LIB,ER_ICONLIB,ER_APP,MN_DIR, OP_DIR, ID_REM,ID_COMALL,ID_COM, ID_ASL, ID_ASLD, ID_SAVEOBJS,ID_ABOUT, ID_NUMBER

DEF	ex_dirs=0,ex_files=0,ex_size=0,ex_blocs=0,str[500]:STRING,file:file,debug=0,ent=0,
	app=0,win=0,running=TRUE,result,sigs,tir,appMsgHook:hook,log,win2, ntitretxt, nb,
	bufname[600]:STRING, appicon:PTR TO diskobject,ttir,lock=NIL,aslobj,aslobjd,arret=0,debut=1

DEF	wb:PTR TO wbstartup, args:PTR TO wbarg, rdargs,etatlog,	outfile,line[1024]:ARRAY OF CHAR,n=0,ncomment=NIL,
	strcomment, num_comment, textbuffer[80]:STRING,endgui,etatendgui,cat:PTR TO catalog_Scalos_Comment,vlist

DEF	bufstring
DEF	bufnom[256]:STRING,buftaille[20]:STRING,bufstamp[20]:STRING,bufheure[20]:STRING,
	bufchemin[256]:STRING,bufcomm[200]:STRING,numlist,cySort,new,dt:datetime,id,
	day[50]:ARRAY,date[50]:ARRAY,time[50]:ARRAY,menusize, menuopen, menufiletypes,gr_list,str_menu,
	nfiles=0,ndirs=0,size=0,exdirs=0,exfiles=0,nonenum=0,ninit=0,path[500]:STRING,
	var,buffer[600]:STRING,bt_Arret,bt_ComAll,bt_Com,bt_Rem,lamp,numobj=0, txt_size,
	txt_path,win3,lastcom=0,always=0,gr_path,txt_abort,txt_Num, visible=0,title1[100]:STRING,obj_Found=FALSE

DEF hook_lv_display:hook,
    hook_lv_construct:hook,
    hook_lv_destruct:hook,
    hook_lv_compare:hook,
    hook_lv_compare2:hook,
    contextmenu_hook:hook,
    stringMsgHook:hook


PROC opref()
DEF bufo[2]:STRING,bufo2[2]:STRING
	n:=0
	IF outfile:=Open(prefile,OLDFILE)
        	WHILE (Fgets(outfile, line, 3))
        		n++
        		IF n=1
        			StrCopy(bufo,line,StrLen(line)-1)
				-> WriteF('n[\d] bufo = \s\n', n, bufo)
        		ELSEIF n=2
        			StrCopy(bufo2,line,StrLen(line)-1)
				-> WriteF('n[\d] bufo2 = \s\n', n, bufo2)
			ENDIF
		ENDWHILE

        	IF StrCmp(bufo,'0')=TRUE
        		set(log,MUIA_Selected,0)
        	ELSEIF StrCmp(bufo,'1')=TRUE
        		set(log,MUIA_Selected,1)
		ENDIF

        	IF StrCmp(bufo2,'0')=TRUE
        		set(endgui,MUIA_Selected,0)
        	ELSEIF StrCmp(bufo2,'1')=TRUE
        		set(endgui,MUIA_Selected,1)
			-> WriteF('Endgui selected = \d\n',Val(bufo2))
		ENDIF
		Close(outfile)
        ELSE
		EasyRequestArgs(NIL, [20, 0, (cat.msg_TitleErrorReq.getstr() ) , (cat.msg_PrefNoFoundReq.getstr() ), (cat.msg_BtOkReq.getstr() )], 0, [prefile])
        ENDIF

ENDPROC

PROC pref()
DEF bufs[3]:STRING
	get(log,MUIA_Selected,{etatlog})
	get(endgui,MUIA_Selected,{etatendgui})
	outfile:=Open(prefile,NEWFILE)
	StringF(bufs,'\d\n',etatlog)
	Fputs(outfile,bufs)
	StringF(bufs,'\d\n',etatendgui)
	Fputs(outfile,bufs)
	Close(outfile)
ENDPROC

PROC main() HANDLE 
 DEF np=0,ncible:PTR TO oListData,nl=0,menu,jmc[600]:STRING,bufsetting[3]:STRING

  IF (scalosbase := OpenLibrary(SCALOSNAME,SCALOSREVISION)) = NIL THEN Raise(ER_LIB)
  IF (muimasterbase:=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))=NIL THEN Raise(ER_MUILIB)
  IF (iconbase:=OpenLibrary('icon.library',0))=NIL THEN Raise(ER_ICONLIB)
  IF (utilitybase:=OpenLibrary('utility.library',0))=NIL THEN Raise('Failed to open utility.library !')

  localebase := OpenLibrary( 'locale.library' , 0 )
  NEW cat.create()
  cat.open( NIL , NIL )

	StringF(title1,'Scalos comment module \s',__TARGET__)

	installhook(hook_lv_display, {hooklvdisplay})
	installhook(hook_lv_construct, {hooklvconstruct})
	installhook(hook_lv_destruct, {hooklvdestruct})
	installhook(hook_lv_compare, {hooklvcompare})
	installhook(hook_lv_compare2, {hooklvcompare2})
	installhook(appMsgHook,{appMsgFunc})
	installhook(contextmenu_hook, {contextmenu_func})
	installhook(stringMsgHook,{stringMsgFunc})

	menufiletypes:=MenustripObject,
		Child, MenuObject,
			MUIA_Menu_Title, bufnom,
			Child, menusize:=MenuitemObject,
				MUIA_Menuitem_Title, (cat.msg_GetSize.getstr() ),
				MUIA_UserData, MN_DIR,
			End,
			Child, menuopen:=MenuitemObject,
				MUIA_Menuitem_Title, (cat.msg_OpenDir.getstr() ),
				MUIA_UserData, OP_DIR,
			End,
		End,
	End

menu := [NM_TITLE,0, (cat.msg_MenuProject.getstr() ), 0 ,0,0,0,
          NM_ITEM ,0, NM_BARLABEL, 0 ,0,0,0,
	  NM_ITEM ,0, NM_BARLABEL, 0 ,0,0,0,
          NM_ITEM ,0, (cat.msg_MenuItemAbout.getstr() ), (cat.msg_MenuItemAboutKey.getstr() ),0,0,ID_ABOUT,
          NM_ITEM ,0, NM_BARLABEL, 0 ,0,0,0,
          NM_ITEM ,0, (cat.msg_MenuItemSettings.getstr() ), (cat.msg_MenuItemSettingsKey.getstr() ),0,0,ID_SAVEOBJS,
          NM_ITEM ,0, NM_BARLABEL, 0 ,0,0,0,
          NM_ITEM ,0, (cat.msg_MenuItemQuit.getstr() ), (cat.msg_MenuItemQuitKey.getstr() ),0,0, MUIV_Application_ReturnID_Quit,
          NM_ITEM ,0, NM_BARLABEL, 0 ,0,0,0,
          NM_ITEM ,0, NM_BARLABEL, 0 ,0,0,0,
          NM_END  ,0, NIL        , 0 ,0,0,0]:newmenu

  app := ApplicationObject,
    MUIA_Application_Title      , 'Scalos Comment',
    MUIA_Application_Version    , VERSDATE,
    MUIA_Application_Copyright  , 'J_Marie COAT, ©2005-2006',
    MUIA_Application_Author     , 'J_Marie COAT, ALIAS JMC ',
    MUIA_Application_Description, 'Comment objects under SCALOS',
    MUIA_Application_Base       , 'SCALOS_COMMENT',
    MUIA_Application_Menustrip  ,  Mui_MakeObjectA(MUIO_MenustripNM,[menu,0]),
    -> MUIA_Application_DiskObject, appicon:= GetDiskObject('PROGDIR:Comment.module'),
    MUIA_Application_DiskObject, get_MyDiskObject(appicon),

->> ***************************

	SubWindow, win2:= WindowObject,
      		MUIA_Window_Width, MUIV_Window_Width_Screen(30),
      		MUIA_Window_SizeGadget, FALSE,
      		MUIA_Window_DragBar, FALSE,
      		MUIA_Window_DepthGadget, FALSE,
      		MUIA_Window_CloseGadget, FALSE,
      		WindowContents , VGroup,
        		Child, makeTXT(100,MUIV_Frame_Text,MUII_TextBack,NIL, VERSTRING ,NIL,'\ec\eb',NIL),
			Child, HGroup,
           			Child, ntitretxt:=makeTXT(100,NIL,NIL,NIL,'',NIL,'\e3\ec',NIL),
            			Child, nb:=makeTXT(60,NIL,NIL,NIL,'',NIL,'\eb',NIL),
	   		End,
		End,
	End,

	SubWindow, win3:= WindowObject,
      		MUIA_Window_Width, MUIV_Window_Width_Screen(45),
		MUIA_Window_Title, cat.msg_WinInfos.getstr(),
      		WindowContents , VGroup,
        		Child, txt_size:=makeTXT(100,MUIV_Frame_Text,MUII_TextBack,NIL, '',NIL,'\ec',NIL),
			Child, gr_path:=HGroup,
				Child, Label2( cat.msg_Path.getstr() ),
        			Child, txt_path:=makeTXT(100,MUIV_Frame_Text,MUII_TextBack,NIL, '',NIL,'\ec',NIL),
			End,
       			Child, txt_abort:=makeTXT(100,MUIV_Frame_Text,MUII_TextBack,NIL, '',NIL,'\ec',NIL),
		End,
	End,

->> *****************************

    SubWindow, win:= WindowObject,
      MUIA_Window_ID, "COMM",
      MUIA_Window_AppWindow, MUI_TRUE,
      MUIA_Window_Title, title1,
      MUIA_Window_AppWindow, MUI_TRUE,
      WindowContents, VGroup,
	Child,HGroup,
	  Child, lamp:=LampObject,
		MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
	  End,
	  Child, Label2((cat.msg_Entry.getstr() )),
	  Child, str_menu:= GroupObject,MUIA_ContextMenu, menufiletypes,
		Child, HGroup, MUIA_Group_Spacing, 0,
          		Child, tir:=BetterStringObject, StringFrame,
				MUIA_String_Format , MUIV_String_Format_Center ,
              		End,
          		Child, aslobj:= makeIMG(NIL, NIL, NIL, MUIV_InputMode_RelVerify,MUII_PopFile,0,1,0),
              		Child, aslobjd:= makeIMG(NIL, NIL, NIL, MUIV_InputMode_RelVerify,MUII_PopDrawer,0,1,1),
	  	End,

	  End,
	
	  Child, Label2((cat.msg_Type.getstr() )),
          Child, ttir:=makeTXT(40,MUIV_Frame_Text,MUII_TextBack,NIL,'',NIL,'\ec\eb\e3',NIL),
        End,

	Child, VGroup,
		Child,HGroup,
	  		Child, Label2((cat.msg_Comment.getstr() )),
          		Child, strcomment:=BetterStringObject, TextFrame,
				MUIA_String_Format, MUIV_String_Format_Center,
				MUIA_String_MaxLen, 81,
	  		End,
          		Child, num_comment:=makeTXT(10,MUIV_Frame_Text,MUII_TextBack,NIL,'',NIL,'\ec\eb\e3',NIL),
        	End,
		Child, gr_list:=GroupObject,MUIA_ContextMenu, menufiletypes, MUIA_ShowMe,0,
			Child, vlist:=makeLV(NIL, TRUE, TRUE, MUIV_Frame_InputList, MUIV_NList_MultiSelect_Default, NIL),
			Child, HGroup,
				Child, bt_ComAll:=SimpleButton(cat.msg_BTCommAll.getstr() ),
				Child, bt_Com:=SimpleButton(cat.msg_BTCommSel.getstr() ),
				Child, bt_Rem:=SimpleButton(cat.msg_BTRemEntry.getstr() ),
				Child, bt_Arret:=SimpleButton(cat.msg_BtAbort.getstr() ),
			End,
		End,
	End,

/*** HGROUP MUII_CheckMark ************************************************************************************/

	 Child, HGroup, MUIA_Frame ,MUIV_Frame_Text ,
	  Child, RectangleObject, 
	    End,
          Child,Label1((cat.msg_All.getstr() )),
                MUIA_Font, MUIV_Font_Normal,
	  Child, log:=makeIMG(NIL, NIL, cat.msg_HelpAll.getstr(), MUIV_InputMode_Toggle,MUII_CheckMark,0,0,0),
	   Child, RectangleObject, 
	   End,
          Child,Label1((cat.msg_Quit.getstr() )),
                MUIA_Font, MUIV_Font_Normal,
	  Child, endgui:=makeIMG(NIL, NIL, cat.msg_HelpQuit.getstr(), MUIV_InputMode_Toggle,MUII_CheckMark,0,0,0),
	     Child, RectangleObject, 
	     End,
          Child,Label1((cat.msg_EntriesNumber.getstr() )),
                MUIA_Font, MUIV_Font_Normal,
          Child, txt_Num:=makeTXT(40,MUIV_Frame_Text,MUII_TextBack,NIL,'',NIL,'\ec\eb\e3', cat.msg_EntriesNumberHelp.getstr() ),
	End,

/***************************************************************************************************************/

->
        End,
      End,
    End

  IF app=NIL THEN Raise(ER_APP)

  doMethodA(win,	[MUIM_Notify,MUIA_Window_CloseRequest,MUI_TRUE,app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit])
  doMethodA(win,	[MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,vlist,3,MUIM_CallHook,appMsgHook,MUIV_TriggerValue])
  doMethodA(win3,	[MUIM_Notify,MUIA_Window_CloseRequest,MUI_TRUE,win3,3,MUIM_Set,MUIA_Window_Open, 0])
  doMethodA(strcomment,	[MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,app,2,MUIM_Application_ReturnID, ID_COM])
  doMethodA(tir,	[MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,app,2,MUIM_Application_ReturnID,"init"])
  doMethodA(gr_list, 	[MUIM_Notify, MUIA_ContextMenuTrigger,MUIV_EveryTime,app,3,MUIM_CallHook,contextmenu_hook,MUIV_TriggerValue])
  doMethodA(str_menu, 	[MUIM_Notify, MUIA_ContextMenuTrigger,MUIV_EveryTime,app,3,MUIM_CallHook,contextmenu_hook,MUIV_TriggerValue])

  doMethodA(strcomment,	[MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, num_comment, 3, MUIM_CallHook, stringMsgHook, MUIV_TriggerValue])

  doMethodA(vlist,	[MUIM_Notify,MUIA_NList_Active,MUIV_EveryTime,app,2,MUIM_Application_ReturnID,"disp"])
  doMethodA(bt_Arret,  	[MUIM_Notify,MUIA_Pressed,0,app,3,MUIM_WriteLong,1,{arret}])

  retidp(bt_Rem,   	ID_REM)
  retidp(bt_ComAll,	ID_COMALL)
  retidp(bt_Com,   	ID_COM)
  retidp(aslobj,   	ID_ASL)
  retidp(aslobjd,   	ID_ASLD)

  doMethodA(log,	[MUIM_Notify,MUIA_Selected, MUIV_EveryTime, app,3, MUIM_WriteLong,MUIV_TriggerValue,{etatlog}])
  doMethodA(endgui,	[MUIM_Notify,MUIA_Selected, MUIV_EveryTime, app,3, MUIM_WriteLong,MUIV_TriggerValue,{etatendgui}])
  doMethodA(win,	[MUIM_Notify,MUIA_Window_CloseRequest,MUI_TRUE, app,3,MUIM_WriteLong,1,{arret}])
  doMethodA(win,	[MUIM_Window_SetCycleChain, tir, strcomment, log, endgui, NIL])

  set(bt_Arret, 	MUIA_Disabled, 1)
  set(app,	MUIA_Application_DropObject,win)

  getargs()

   IF numobj = 0
	set(app,MUIA_Application_Iconified,MUI_TRUE)
	set(win,MUIA_Window_Open,MUI_TRUE)
	opref()
  ELSEIF numobj > 0
	set(win,MUIA_Window_Open,MUI_TRUE)
	opref()
	set(vlist, MUIA_NList_Active,0)
  ENDIF

    WHILE running AND (CheckSignal(SIGBREAKF_CTRL_C)=0)
      result:= doMethodA(app,[MUIM_Application_Input,{sigs}])

        SELECT result
		CASE MUIV_Application_ReturnID_Quit
			arret:=1
			pref()
			running:=FALSE

                CASE ID_ABOUT
			StringF(jmc,'\eb\ecApplication \s for Scalos\en\n\euWritten in E\en\n\e3\s\e1\en\n\ei\s \en\nCompiled on \s \s with: ECX: \d',__TARGET__, VERSTRING, COPYRIGHT, __DATE__, __TIME__, ECX_VERSION)
			Mui_RequestA(app,win,0,'A propos...', (cat.msg_BtOkReq.getstr() ), jmc, NIL)

                CASE ID_SAVEOBJS
			get(log,MUIA_Selected,{etatlog})
			get(endgui,MUIA_Selected,{etatendgui})
			outfile:=Open('ENVARC:Scalos/Comment_Module.prefs',NEWFILE)
			StringF(bufsetting,'\d\n',etatlog)
			Fputs(outfile,bufsetting)
			StringF(bufsetting,'\d\n',etatendgui)
			Fputs(outfile,bufsetting)
			Close(outfile)

		CASE ID_REM
			 removesel(vlist,MUIV_NList_Remove_Selected)
			 vnumlist(vlist)
			 -> WriteF('numlist = \d\n',numlist)
			 IF (numlist = 0)
				running:=FALSE
			 ENDIF

		CASE ID_COMALL
			vnumlist(vlist)
			nonenum:=0
			arret:=0
			lastcom:=0
			always:=0
			np:=1
			nl:=0
			get(log,MUIA_Selected, {etatlog})
			doMethodA(aslobj,[MUIM_Set,MUIA_Disabled,TRUE])
			IF numlist > 1
				set(bt_Arret, MUIA_Disabled, 0)

				doMethodA(vlist,[MUIM_NList_Select,MUIV_NList_Select_All,MUIV_NList_Select_On, NIL ])

				commentlist(vlist)

				set(bt_Arret, MUIA_Disabled, 1)
			ELSEIF numlist = 1
				np:=0
				set(vlist,MUIA_NList_Active,np)
				doMethodA(vlist,[MUIM_NList_GetEntry,np,{ncible}])
				proc_Analyse(ncible)
			ENDIF

			doMethodA(aslobj,[MUIM_Set,MUIA_Disabled,FALSE])
			IF (etatendgui=1)
				doMethodA(vlist,[MUIM_NList_Clear])
				pref()
				running:=FALSE
			ENDIF

		CASE ID_COM
			vnumlist(vlist)
			nonenum:=0
			arret:=0
			always:=0
			get(log,MUIA_Selected, {etatlog})
			set(bt_Arret, MUIA_Disabled, 0)
			doMethodA(aslobj,[MUIM_Set,MUIA_Disabled,TRUE])
			commentlist(vlist)
			set(bt_Arret, MUIA_Disabled, 1)
			doMethodA(aslobj,[MUIM_Set,MUIA_Disabled,FALSE])

		CASE ID_ASL
			asl()

		CASE ID_ASLD
			asld()

		CASE "disp"
			get(vlist,MUIA_NList_EntryClick,{visible})
			display()

		CASE "init"
			get(tir,MUIA_String_Contents,{var})
			IF StrLen(var) <> 0
                        	IF (lock := Lock(var, ACCESS_READ)) = 0
					faute()
					mreq(win, (cat.msg_TitleErrorReq.getstr() ) , (cat.msg_BtOkReq.getstr() ), (cat.msg_ErrorReq.getstr() ),[var,IoErr(),textbuffer])
					set(ttir,MUIA_Text_Contents, (cat.msg_NoFound.getstr() ))
					UnLock(lock)
                        	ELSE
					StrCopy(buffer,var)
                                	IF buffer[StrLen(buffer)-1]="/"
					 SetStr(buffer,StrLen(buffer)-1)
					ENDIF

                        		file.name:=buffer
					StrCopy(str,buffer)

					file.type:=-10
					debut:=1
    					diskobjects()
					countobj()
				 	-> set(gr_list,MUIA_ShowMe,1)
				ENDIF
			ELSE
				set(win,MUIA_Window_ActiveObject,tir)
			ENDIF
         ENDSELECT

      IF (running AND sigs) THEN Wait(sigs)
    ENDWHILE
  FreeDiskObject(appicon)

EXCEPT DO
  IF app THEN Mui_DisposeObject(app)
  IF muimasterbase THEN CloseLibrary(muimasterbase)
  IF iconbase THEN CloseLibrary(iconbase)
  IF utilitybase THEN CloseLibrary(utilitybase)
  IF scalosbase THEN CloseLibrary(scalosbase)
  cat.close()
  IF localebase THEN CloseLibrary( localebase )
  SELECT exception
    CASE ER_LIB
	WriteF((cat.msg_ScaLibError.getstr() ) ,SCALOSNAME, SCALOSREVISION)
	CleanUp(20)

    CASE ER_MUILIB
      	WriteF((cat.msg_MuiError.getstr() ) ,MUIMASTER_NAME)
      	CleanUp(20)

    CASE ER_ICONLIB
      	WriteF((cat.msg_IconLibError.getstr() ))
      	CleanUp(20)

    CASE ER_APP
      	WriteF((cat.msg_AppError.getstr() ))
      	CleanUp(20)
      
  ENDSELECT
ENDPROC 0

-> --------------------------------------

PROC p_WinTitle(titre)
	set(win,MUIA_Window_Title, titre)
ENDPROC

->*****************

PROC vid()
	id:=MUIV_NList_NextSelected_Start
ENDPROC

-> *********************************************************************

PROC asl()
DEF 	multifiles:PTR TO wbarg, frtags=NIL :PTR TO LONG,
	fr = NIL : PTR TO filerequester,flen, n, currentdir[300]:STRING, newentry

frtags :=  [ASLFR_TITLETEXT,   (cat.msg_Asltext.getstr() ),
           ASLFR_INITIALDRAWER, '',
           ASLFR_INITIALFILE, '',
	   ASLFR_DOMULTISELECT, multifiles,
           TAG_DONE] : tagitem

IF aslbase := OpenLibrary('asl.library', 37)
   	IF fr := AllocAslRequest (ASL_FILEREQUEST, frtags)
     		IF AslRequest (fr, NIL)
      			flen   := FileLength (fr.drawer)
         		IF (multifiles)
            			n:=0
       				IF lock:=Lock(fr.drawer,-2)
            				NameFromLock(lock,currentdir,300)
            				AddPart(currentdir,'',300)
            				multifiles:=fr.arglist
					UnLock(lock)
            				FOR n:=1 TO fr.numargs
		 				StringF(str,'\s\s', currentdir,multifiles[].name)
    						countobj()
						get(vlist,MUIA_NList_InsertPosition,{newentry})
		 				multifiles++
            				ENDFOR
					vnumlist(vlist)
					IF (numlist > 1) THEN set(gr_list,MUIA_ShowMe,1)
					set(vlist,MUIA_NList_Active, newentry)
					display()
	    			ENDIF
			ENDIF
     		ENDIF
      		FreeAslRequest (fr)
    	ENDIF
ENDIF
ENDPROC

PROC asld()
DEF 	frtags=NIL :PTR TO LONG,fr = NIL : PTR TO filerequester,flen, currentdir[300]:STRING, newentry

frtags :=  [ASLFR_TITLETEXT,   (cat.msg_AsltextDir.getstr() ),
           ASLFR_INITIALDRAWER, '',
	   ASLFR_DRAWERSONLY, ASLFR_FLAGS1,
           TAG_DONE] : tagitem

IF aslbase := OpenLibrary('asl.library', 37)
   	IF fr := AllocAslRequest (ASL_FILEREQUEST, frtags)
     		IF AslRequest (fr, NIL)
      			flen   := FileLength (fr.drawer)
       				IF lock:=Lock(fr.drawer,-2)
            				NameFromLock(lock,currentdir,300)
            				AddPart(currentdir,'',300)
					UnLock(lock)
	 				StringF(str,'\s', currentdir)
					countobj()
					get(vlist,MUIA_NList_InsertPosition,{newentry})
					vnumlist(vlist)
					IF (numlist > 1) THEN set(gr_list,MUIA_ShowMe,1)
					set(vlist,MUIA_NList_Active, newentry)
					display()
	    			ENDIF
     		ENDIF
      		FreeAslRequest (fr)
    	ENDIF
ENDIF
ENDPROC

PROC diskobjects1()
DEF	dlock=NIL, info:PTR TO fileinfoblock

  info:=AllocDosObject(DOS_FIB, NIL)
  IF dlock:=Lock(file.name,-2)
   	Examine(dlock, info)
	IF info.direntrytype<0
		ent:=0
	ELSE
		ent:=1
       	ENDIF

	IF (info.direntrytype=2) AND (obj_Found=FALSE)
 		set(log,MUIA_Disabled,0)
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeDrawer.getstr() ))
        	StrCopy(str,file.name)
		IF str[(StrLen(str)-1)]=":" THEN set(ttir,MUIA_Text_Contents,(cat.msg_TypeAssign.getstr() ))
		IF str[(StrLen(str)-1)]<>":" THEN
        	StrAdd(str,'/')
        	file.name:=str
	ELSEIF info.direntrytype=1
 		set(log,MUIA_Disabled,0)
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeDisk.getstr() ))
		StrCopy(str,file.name)
	ENDIF

	-> WriteF('TYPE = \d - \s\n', info.direntrytype, file.name)
    	UnLock(dlock)
  ENDIF
  FreeDosObject(DOS_FIB, info)
ENDPROC
-> ******************************************************************************************************
PROC diskobjects()
DEF do:PTR TO diskobject,type

obj_Found:=FALSE

IF do:=GetDiskObjectNew(file.name)
  	type:=do.type
  	file.type:=type
  	IF (file.type=WBTOOL)
		ent:=0
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeTool.getstr() ))
 		set(log,MUIA_Disabled,1)
  	ELSEIF (file.type=WBPROJECT)
		ent:=0
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeProject.getstr() ))
 		set(log,MUIA_Disabled,1)
  	ELSEIF (file.type=WBGARBAGE)
		obj_Found:=TRUE
 		set(log,MUIA_Disabled,0)
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeGarbage.getstr() ))
        	StrCopy(str,file.name)
        	StrAdd(str,'/')
        	file.name:=str
		ent:=1
  	ENDIF

  	FreeDiskObject(do)
	diskobjects1()
	set(win,MUIA_Window_ActiveObject,strcomment)
ENDIF
ENDPROC

PROC initfile(fichier)
DEF fib:fileinfoblock,lock=0
 IF lock:=Lock(fichier,-2)
   Examine(lock,fib)
   file.numblocks:=fib.numblocks
   file.size:=fib.size
   file.comment:=fib.comment
   set(strcomment,MUIA_String_Contents,file.comment)
   UnLock(lock)
 ELSE
   faute()
   mreq(win,(cat.msg_TitleErrorReq.getstr() ), (cat.msg_BtOkReq.getstr() ), (cat.msg_ErrorReq.getstr() ) ,[fichier,IoErr(),textbuffer])
 ENDIF
ENDPROC


PROC removesel(obj,tag:PTR TO LONG)
	doMethodA(obj,[MUIM_NList_Remove,tag])
ENDPROC

PROC retidp(btobj,rid)
	doMethodA(btobj,[MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, rid])
ENDPROC

/*******************************************************************
 ******************** MUIV_Lamp_Color_Processing *******************/

PROC proc_StartProcess()
	set(lamp, MUIA_Lamp_Color, MUIV_Lamp_Color_Processing)
	set(win, MUIA_Window_Title, cat.msg_WinAnalysing.getstr() )
ENDPROC

/*********************** MUIV_Lamp_Color_Off ************************/

PROC proc_EndProcess()
	set(lamp, MUIA_Lamp_Color, MUIV_Lamp_Color_Off)
	set(win, MUIA_Window_Title,'Comment.module')
ENDPROC

/*********************************************************************
 ********************************************************************/


PROC commentlist(obj)
DEF	npc=0,ent:PTR TO oListData

	IF (numlist=1)
		set(obj,MUIA_NList_Active,0)
		doMethodA(obj,[MUIM_NList_GetEntry,0,{ent}])
		proc_Analyse(ent)
		init_Selected(0,ent)
		set(obj,MUIA_NList_Active,0)
		
	ELSE
		set(obj,MUIA_NList_Active,MUIV_NList_Active_Off)
		vid()
		set(obj,MUIA_NList_Quiet,1)
		REPEAT
			refresh()
			doMethodA(obj,[MUIM_NList_NextSelected,{id}])
			doMethodA(obj,[MUIM_NList_GetEntry,id,{ent}])
			IF (id<>-1) AND (arret=0)
				npc++
				proc_Analyse(ent)
				init_Selected(id,ent)
				lastcom:=npc
				doMethodA(obj,[MUIM_NList_Select,id,MUIV_NList_Select_Off,NIL])
			ENDIF
		UNTIL(id=MUIV_NList_NextSelected_End)
		set(vlist,MUIA_NList_Active,visible)

		/* WriteF('commentlist() : visible     = \d\n', visible)
		WriteF('commentlist() : npc         = \d\n', npc)
		WriteF('commentlist() : visible+npc = \d\n',visible+npc) */

		set(obj,MUIA_NList_Quiet,0)
	ENDIF
ENDPROC

PROC proc_Analyse(cible:PTR TO oListData)
DEF	tempnom

	tempnom:=String(StrLen(cible.nom))
	StrCopy(tempnom,cible.nom,ALL)
	IF StrCmp(tempnom,'\e3',2) THEN MidStr(tempnom,tempnom,2,StrLen(tempnom)-2)
	IF StrCmp(tempnom,bufchemin,ALL)=TRUE
		set(tir,MUIA_String_Contents,bufchemin)
		StrCopy(str,cible.chemin,ALL)
	ELSE
		StringF(str,'\s\s',cible.chemin,tempnom)
		set(tir,MUIA_String_Contents,str)
	ENDIF
	commenter()
ENDPROC

PROC commenter()
DEF askreq=0

	get(tir,MUIA_String_Contents,{var})
	IF StrLen(var) <> 0
               	IF (lock := Lock(var, ACCESS_READ)) = 0
			faute()
			mreq(win,(cat.msg_TitleErrorReq.getstr() ) , (cat.msg_BtOkReq.getstr() ), (cat.msg_ErrorReq.getstr() ) ,[var,IoErr(),textbuffer])
			set(ttir,MUIA_Text_Contents,(cat.msg_NoFound.getstr() ))
               	ELSE
			StrCopy(buffer,var)
                       	IF buffer[StrLen(buffer)-1]="/"
				 SetStr(buffer,StrLen(buffer)-1)
			ENDIF

               		file.name:=buffer
			StrCopy(str,buffer)

			file.type:=-10

	        	get(strcomment,MUIA_String_Contents,{ncomment})
			-> WriteF('ncomment=<\s>', ncomment)

			IF StrLen(ncomment)=0 THEN ncomment:=''

			debut:=0
			diskobjects()

			proc_StartProcess()

               		IF ent
				comment(str,ncomment)
				-> getpathdir(str)
				-> intypes(str)
				vnumlist(vlist)
				IF (numlist > 1) AND (always=0)

					get(log,MUIA_Selected, {etatlog})
					IF (etatlog=1)
						-> askreq := Mui_RequestA(app,win,0, 'confirmation' , 'Oui | Non | Oui pour tous | Jamais' , 'Commenter tous les éléments présents\ndans : \s', {str})
						askreq := Mui_RequestA(app,win,0, (cat.msg_TitleConfirmReq.getstr() ), (cat.msg_BTConfirmReq.getstr() ), (cat.msg_BodyConfirmReq.getstr() ), {str})
						IF askreq = 0 THEN set(log,MUIA_Selected, 0)
						IF askreq = 1 THEN etatlog:=1
						IF askreq = 2 THEN etatlog:=0
						IF askreq = 3 THEN always:=1
					ENDIF
				ENDIF

				IF (etatlog=1) THEN examine()
			ELSE
				comment(str,ncomment)
				-> getpathdir(str)
				-> intypes(str)
			ENDIF

			proc_EndProcess()

			initfile(str) ->file.name

			IF (etatendgui=1)
				pref()
				running:=FALSE
			ENDIF
		ENDIF
		IF lock THEN UnLock(lock)
	ELSE
		set(win,MUIA_Window_ActiveObject,tir)
	ENDIF
ENDPROC

/*************************************************************************/

PROC init_Selected(num:LONG,cible:PTR TO oListData)
DEF tempnom
		tempnom:=String(StrLen(cible.nom)+1)
		StrCopy(tempnom,cible.nom,ALL)
		StrCopy(bufchemin,cible.chemin,ALL)

		IF StrCmp(tempnom,'\e3',2) THEN MidStr(tempnom,tempnom,2,StrLen(tempnom)-2)
		IF StrCmp(tempnom,bufchemin,ALL)=TRUE
			set(tir,MUIA_String_Contents,bufchemin)
			StrCopy(str,cible.chemin,ALL)
		ELSE
			StringF(str,'\s\s',cible.chemin,tempnom)
			set(tir,MUIA_String_Contents,str)
		ENDIF
		doMethodA(vlist,	[MUIM_NList_Remove, num])
		getpathdir(str)
		refresh()
		intypes(str)
		refresh()
ENDPROC
-> *****************

PROC examine()
DEF lock=0,fib:fileinfoblock

 ex_dirs:=0
 ex_files:=0
 IF debug THEN WriteF('')
 StrCopy(bufname,'Examen de : \eb')
 StrAdd(bufname,file.name)
 StrAdd(bufname,' \en...')

 file.pname:=file.name
 IF lock:=Lock(file.name,-2)
 	Examine(lock,fib)
 	file.protection:=fib.protection
 	CopyMem(fib.datestamp,file.datestamp,SIZEOF datestamp)
   	IF fib.direntrytype<0
   		file.numblocks:=fib.numblocks
   		file.size:=fib.size
   	ENDIF

 	-> file.comment:=news(fib.comment)

 	UnLock(lock)
 ELSE
 	file.pname:=0
 ENDIF
 arret:=0
 refresh()
 getdir()
 refresh()
ENDPROC

PROC getdir()
	refresh()
	exdir(file.name)
	refresh()
	IF arret
 		set(aslobj,MUIA_Disabled,FALSE)
 	ELSE
 		set(app,MUIA_Application_Iconified, FALSE)
 	ENDIF
ENDPROC

PROC exdir(dir)
DEF fib:PTR TO fileinfoblock,dirlock=NIL,olddir = NIL

IF(dirlock := Lock(dir,SHARED_LOCK))
	olddir := CurrentDir(dirlock)
        fib := AllocDosObject(DOS_FIB, NIL)
        IF (Examine(dirlock, fib))
		ex_blocs:=ex_blocs+fib.numblocks
       		WHILE ExNext(dirlock, fib) AND (arret=0)
        		IF(fib.entrytype<0) -> Fichier
                		comment(fib.filename,ncomment)
        			ex_files++
        			ex_size:=ex_size+fib.size

        		ELSE -> Répertoire

                		comment(fib.filename,ncomment)
				refresh()
        			exdir(fib.filename)
				refresh()
        		ENDIF
      		ENDWHILE
    	ENDIF
  	UnLock(dirlock)
  ENDIF
  CurrentDir(olddir)
  FreeDosObject(DOS_FIB, fib)
ENDPROC

->**************

PROC getargs()
DEF bnum[10]:STRING

	IF wbmessage<>NIL				/* from wb */
    		wb:=wbmessage
   		args:=wb.arglist

    		NameFromLock(args[0].lock,str,256)

    		IF StrLen(args[0].name)>0 THEN AddPart(str, args[0].name,256)

    		IF wb.numargs > 1
			IF wb.numargs >= 2
				IF wb.numargs > 2 THEN set(win2,MUIA_Window_Open,1)
        			FOR rdargs:=1 TO wb.numargs-1
					refresh()
        				NameFromLock(args[rdargs].lock,str,256)
					IF StrLen(args[rdargs].name)>0 THEN AddPart(str, args[rdargs].name,256)
					numobj++
					StringF(bnum,'\d',numobj)
					set(ntitretxt, MUIA_Text_Contents,args[rdargs].name)
					set(nb, MUIA_Text_Contents,bnum)
					-> WriteF('Name: <\s> [\d]\n',args[rdargs].name, numobj)
					countobj()
                        		UnLock(args[rdargs].lock)
                        		args[rdargs].lock := NIL

        			ENDFOR
				IF wb.numargs > 2 THEN set(win2,MUIA_Window_Open,0)
			ENDIF
    			file.name:=str
  			file.type:=-10
  			StringF(str,'\s',file.name)
  			diskobjects()
			initfile(file.name)
			vnumlist(vlist)
			IF numlist > 1 THEN set(gr_list,MUIA_ShowMe,1)
			set(vlist,MUIA_NList_Active,0)

		ENDIF
		debut:=0
	ELSE
		Raise(WriteF('\s\n\s\n\e[1mSorry, no WB arguments found! \e[0m\n',VERSTRING, COPYRIGHT))
	ENDIF
ENDPROC

PROC getpathdir(fichier)
DEF newlock2,lock=NIL,p
	IF fichier[StrLen(fichier)-1]<>":"
		IF(lock := Lock(fichier, SHARED_LOCK)) <> 0
			IF (newlock2:=ParentDir(lock)) <> 0
				p:=String(500)
				NameFromLock(newlock2,p,500)
				StrCopy(path,p,ALL)
				UnLock(newlock2)
			ENDIF
			UnLock(lock)
		ENDIF
	ELSE
		StrCopy(path,fichier,ALL)
	ENDIF
	-> WriteF('getpathdir(\s) = \s\n',fichier, path)
ENDPROC

PROC countobj()
DEF 	numobj=0, testicon,lockicon=NIL,lock=NIL,info:fileinfoblock,check=FALSE

	-> WriteF('* START :countobj()\n')

	IF lock:=Lock(str,-2)

		-> WriteF('* STR: countobj()\ntesticon=\s\n',str)

		Examine(lock, info)
		IF info.direntrytype<0
			nfiles++
		ELSE
			ndirs++
		ENDIF
		testicon:=String(StrLen(str)+5)
        	IF str[StrLen(str)-1]="/"
			str[StrLen(str)-1]:=0
		ENDIF
		UnLock(lock)
		IF str[StrLen(str)-1]<>":"
			StringF(testicon,'\s.info',str)
		ENDIF
	ENDIF
			-> diskobjects()

	IF (arret=0)
		getpathdir(str)
		check:=checkentry(str)
		IF check = 0
			intypes(str)
		-> ELSE
		->	mreq(NIL, (cat.msg_TitleErrorReq.getstr() ) ,(cat.msg_BtOkReq.getstr() ) ,(cat.msg_EntryExists.getstr() ) ,[str])
		ENDIF

		IF StrLen(testicon) > 0
			IF lockicon:=Lock(testicon,-2)
				UnLock(lockicon)
				-> WriteF('* TESTICON: countobj()\ntesticon=\s\n',testicon)
				getpathdir(testicon)
				check:=checkentry(testicon)
				IF check = 0
					intypes(testicon)
					nfiles++
				-> ELSE
				->	mreq(NIL, (cat.msg_TitleErrorReq.getstr() ) ,(cat.msg_BtOkReq.getstr() ) ,(cat.msg_EntryExists.getstr() ) ,[testicon])
				ENDIF
			ENDIF
		ENDIF
	ENDIF
	numobj++

	-> WriteF('* END :countobj()\n')
ENDPROC

PROC intypes(ftypesdir)
  DEF 	info:fileinfoblock,s,h,lock=0

  StrCopy(bufchemin,path,ALL)
  IF bufchemin[StrLen(bufchemin)-1]<>":" THEN StrAdd(bufchemin,'/')
  IF lock:=Lock(ftypesdir,-2)

	-> WriteF('* intypes(\s)\nbufchemin =\s\n* END :intypes()\n',ftypesdir,bufchemin)

   	Examine(lock, info)
       	StringF(bufcomm,'\ec\ei\s',info.comment)

	CopyMemQuick(info.datestamp,dt.stamp,SIZEOF datestamp)
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
	n++

	IF info.direntrytype<0
		StrCopy(bufnom,info.filename)
		StringF(buftaille,'\d',info.size)
	ELSE
		IF ftypesdir[StrLen(ftypesdir)-1]=":"
			StringF(bufnom,'\e3\s',ftypesdir)
			StringF(buftaille,'\ecI[6:26]')
		ELSE
			StringF(bufnom,'\e3\s',info.filename)
			StringF(buftaille,'\ecI[6:22]')
		ENDIF
       	ENDIF

	new:=[bufnom,bufcomm,buftaille,s,h,bufchemin]:oListData
	doMethodA(vlist,[MUIM_NList_InsertSingle,new,MUIV_NList_Insert_Sorted])

    	UnLock(lock)
  ENDIF
ENDPROC

PROC checkentry(newbuf)
DEF identry:PTR TO oListData,idnum=0,testbuf[500]:STRING,testnom

	vnumlist(vlist)
	FOR idnum:= 0 TO numlist-1
		doMethodA(vlist,[MUIM_NList_GetEntry,idnum,{identry}])
		testnom:=String(StrLen(identry.nom))
		StrCopy(testnom,identry.nom,ALL)
		IF StrCmp(testnom,'\e3',2) THEN MidStr(testnom,testnom,2,StrLen(testnom)-2)
		IF newbuf[StrLen(newbuf)-1]=":"
			StringF(testbuf,'\s',testnom)
		ELSE
			StringF(testbuf,'\s\s',identry.chemin,testnom)
		ENDIF
		-> IF StrCmp(testbuf,'\e3',2) THEN MidStr(testbuf,testbuf,2,StrLen(testbuf)-2)
		-> WriteF('\s\n\s\n',newbuf,testbuf)
		IF Stricmp(newbuf,testbuf) = 0
			-> WriteF('Stricmp = 0 ->  \s\n\s\n',newbuf,testbuf)
			RETURN TRUE
		ENDIF
	ENDFOR
	RETURN FALSE
ENDPROC

PROC vnumlist(obj)
	get(obj,MUIA_NList_Entries,{numlist})
ENDPROC

PROC comment(ent,com)
DEF c
   c:=SetComment(ent,com)
   IF c=FALSE
	faute()
	mreq(win, (cat.msg_TitleErrorReq.getstr() ) ,(cat.msg_BtOkReq.getstr() ) ,(cat.msg_ErrorReq.getstr() ),[ent,IoErr(),textbuffer])
   ENDIF
ENDPROC

->*********************

PROC faute()
	Fault(IoErr(),NIL,textbuffer,80)
ENDPROC

PROC mreq(obj,tobj,tbt:PTR TO LONG,tmreq,tamp)
	Mui_RequestA(app, obj, 0, tobj, tbt, tmreq, tamp)
ENDPROC

/*************************************************************************************/

PROC stringMsgFunc(hook,obj,msg)
DEF	string:LONG,long=0,buf_Com[4]:STRING
	get(strcomment, MUIA_String_Contents,{string})
	IF StrLen(string) > 0
		long:=StrLen(string)
		StringF(buf_Com,'\d',long)
		set(obj, MUIA_Text_Contents, buf_Com)
	ELSE
		set(obj, MUIA_Text_Contents, '0')
	ENDIF
ENDPROC

/*************************************************************************************/

PROC appMsgFunc(hook, obj, x:PTR TO LONG)
DEF	ap:PTR TO wbarg, amsg:PTR TO appmessage, i, buf[300]:STRING, b,
	bufapp[300]:STRING, lnew

	debut:=1
  	amsg:=x[]
  	b:=buf;i:=0
  	ap:=amsg.arglist
	set(obj,MUIA_NList_Quiet,1)
  	WHILE (i<amsg.numargs)
    		NameFromLock(ap.lock,buf,StrMax(buf))
    		AddPart(buf,ap.name,StrMax(buf))
    		file.name:=b
    		StringF(bufapp,'\s',b)
    		StrCopy(str,b,ALL)
    		countobj()
    		ap++
    		i++
		get(obj,MUIA_NList_InsertPosition,{lnew})
  	ENDWHILE
	vnumlist(obj)
 	IF (numlist > 1) THEN set(gr_list,MUIA_ShowMe,1)
	-> set(obj, MUIA_String_Contents,str)
	set(obj,MUIA_NList_Quiet,0)
	set(obj,MUIA_NList_Active, lnew)
	display()
	set(win, MUIA_Window_Activate, TRUE)
ENDPROC 

/*************************************************************************************/
/******************************* HOOKLVDISPLAY ***************************************/

PROC hooklvdisplay(hook, array:PTR TO LONG, entry:PTR TO oListData)

	IF entry=0                 -> Titles
		array[0]:= cat.msg_LName.getstr()
		array[1]:= cat.msg_LComment.getstr()
		array[2]:= cat.msg_LSize.getstr()
		array[3]:= cat.msg_LDate.getstr()
		array[4]:= cat.msg_LTime.getstr()
		array[5]:= cat.msg_LPath.getstr()

	ELSE                       -> normal entries
		array[0]:=entry.nom
		array[1]:=entry.comm
		array[2]:=entry.taille
		array[3]:=entry.date
		array[4]:=entry.heure
		array[5]:=entry.chemin

		get(vlist,MUIA_NList_Entries,{numlist})
		viewnum()
	   ENDIF
ENDPROC

/*************** Affichage du nombre d'entrées **************************************/

PROC viewnum()
DEF str_num[80]:STRING
	StringF(str_num,'\d',numlist)
	set(txt_Num, MUIA_Text_Contents, str_num)

ENDPROC

/************************************************************************************/

PROC checkname(taille,nom,comm)
	IF StrCmp(taille,'\ecI[6:22]',ALL)=TRUE OR StrCmp(taille,'\ecI[6:26]',ALL)=TRUE
		MidStr(nom,nom,2,StrLen(nom)-2)
		set(menusize,MUIA_Menuitem_Enabled,1)
		set(menuopen,MUIA_Menuitem_Enabled,1)
	ELSE
		set(menusize,MUIA_Menuitem_Enabled,0)
		set(menuopen,MUIA_Menuitem_Enabled,0)
	ENDIF
	IF StrCmp(comm,'\ec\ei',4) THEN MidStr(comm,comm,4,StrLen(comm)-4)
ENDPROC

/*************************************************************************************/

PROC display()
DEF fent:PTR TO oListData

	doMethodA(vlist,[MUIM_NList_GetEntry,MUIV_NList_GetEntry_Active,{fent}])

	StrCopy(bufchemin, fent.chemin, ALL)
	StrCopy(bufnom,    fent.nom,    ALL)
	StrCopy(bufcomm,   fent.comm,   ALL)
	StrCopy(buftaille, fent.taille, ALL)
	StrCopy(bufstamp, fent.date,   ALL)
	StrCopy(bufheure, fent.heure,  ALL)

	checkname(buftaille,bufnom,bufcomm)

	bufstring:=String(StrLen(bufchemin)+StrLen(bufnom))
	StrCopy(bufstring,bufchemin,ALL)
	IF bufchemin[StrLen(bufchemin)-1]=":" AND bufnom[StrLen(bufnom)-1]=":"
		StrCopy(bufstring,bufchemin,ALL)
	ELSE
		StrAdd(bufstring,bufnom)
	ENDIF
	set(tir,MUIA_String_Contents,bufstring)

	-> get(strcomment,MUIA_String_Contents,{ncomment})

	IF StrLen(bufcomm)=0 THEN set(strcomment,MUIA_String_Contents,'') ELSE set(strcomment,MUIA_String_Contents,bufcomm)
	file.name:=bufstring
	file.type:=-10
	diskobjects()
ENDPROC

/*********************************  HOOKLVCONSTRUCT  *****************************/

PROC hooklvconstruct(hook, pool, entry:PTR TO oListData)
DEF  new=NIL:PTR TO oListData
	IF (new:=AllocPooled(pool, SIZEOF oListData))
		new.nom:=AllocPooled(pool, StrLen(entry.nom)+1)
		AstrCopy(new.nom, entry.nom)
		new.taille:=AllocPooled(pool, StrLen(entry.taille)+1)
		AstrCopy(new.taille, entry.taille)
		new.date:=AllocPooled(pool, StrLen(entry.date)+1)
		AstrCopy(new.date, entry.date)
		new.heure:=AllocPooled(pool, StrLen(entry.heure)+1)
		AstrCopy(new.heure, entry.heure)
		new.chemin:=AllocPooled(pool, StrLen(entry.chemin)+1)
		AstrCopy(new.chemin, entry.chemin)
		new.comm:=AllocPooled(pool, StrLen(entry.comm)+1)
		AstrCopy(new.comm, entry.comm)
	ENDIF
ENDPROC new

/*********************************  HOOKLVDESTRUCT  *****************************/

PROC hooklvdestruct(hook, pool, entry:PTR TO oListData)
	IF entry.nom    THEN FreePooled(pool, entry.nom, StrLen(entry.nom)+1)
	IF entry.taille THEN FreePooled(pool, entry.taille, StrLen(entry.taille)+1)
	IF entry.date   THEN FreePooled(pool, entry.date, StrLen(entry.date)+1)
	IF entry.heure  THEN FreePooled(pool, entry.heure, StrLen(entry.heure)+1)
	IF entry.chemin THEN FreePooled(pool, entry.chemin, StrLen(entry.chemin)+1)
	IF entry.comm   THEN FreePooled(pool, entry.comm, StrLen(entry.comm)+1)
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
		CASE  2;   RETURN OstrCmp(o1.comm, o2.comm);
		CASE  3;   RETURN OstrCmp(o1.taille, o2.taille);
		CASE  4;   RETURN OstrCmp(o1.date, o2.date);
		CASE  5;   RETURN OstrCmp(o1.heure, o2.heure);
		CASE  6;   RETURN OstrCmp(o1.chemin, o2.chemin);
	ENDSELECT
ENDPROC

/*********************************  HOOKLVCOMPARE2  *****************************/

PROC hooklvcompare2(hook, obj, msg:PTR TO nlist_comparemessage)
DEF 	scp1:PTR TO oListData, scp2:PTR TO oListData
	scp1:=msg.entry1
	scp2:=msg.entry2

RETURN 	Stricmp(scp1.nom,scp2.nom)
       	Stricmp(scp1.comm,scp2.comm)
       	Stricmp(scp1.taille,scp2.taille)
       	Stricmp(scp1.date,scp2.date)
       	Stricmp(scp1.heure,scp2.heure)
       	Stricmp(scp1.chemin,scp2.chemin)

ENDPROC

/******************************** CONTEXTMENU ********************************/

PROC contextmenu_dispatcher(hook, cl:PTR TO iclass,obj:PTR TO object,msg:PTR TO msg)
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
DEF 	o:PTR TO object, buffertemp[256]:STRING,buffertemp2[500]:STRING,dirscan,opened=0,defbytes,
	defkbytes,defmbytes,result = RETURN_OK, strblocs[50]:STRING

	defbytes:=String(StrLen(cat.msg_Bytes.getstr() )+4)
	StringF(defbytes,' \e2\s', cat.msg_Bytes.getstr() )

	defkbytes:=String(StrLen(cat.msg_KBytes.getstr() )+4)
	StringF(defkbytes,' \e2\s', cat.msg_KBytes.getstr() )

	defmbytes:=String(StrLen(cat.msg_MBytes.getstr() )+4)
	StringF(defmbytes,' \e2\s', cat.msg_MBytes.getstr() )

	o:=params[0]
	userdata:=muiUserData(o)

	SELECT userdata
	CASE OP_DIR
		ScA_OpenIconWindow([SCA_Path, bufstring,
                	       SCA_ViewModes, 0,
                	       SCA_ShowAllMode, 0,
                	       TAG_END])
	CASE MN_DIR
		get(win3,MUIA_Window_Open,{opened})
		IF opened=1 THEN set(win3,MUIA_Window_Open,0)

		size:=0
		exfiles:=0
		exdirs:=0
		ex_blocs:=1
		nonenum:=0
		set(txt_abort, MUIA_ShowMe,0)
		arret:=0

		dirscan:=String(StrLen(bufnom))
		StringF(dirscan,'\s',bufnom)

		doMethodA(app,	[MUIM_MultiSet, MUIA_Disabled, TRUE,
			vlist, bt_ComAll, bt_Com, bt_Rem, log, endgui, NIL])

		set(menusize,MUIA_Menuitem_Enabled,0)

		set(bt_Arret, MUIA_Disabled,0)

 		proc_StartProcess()
		refresh()
		-> WriteF('GetSize: \s\n',bufstring)
		result:=count(bufstring)
		refresh()
		proc_EndProcess()

	IF (result = RETURN_OK)
		StringF(buffertemp,v2s(size,'\e3', defbytes))
		StringF(strblocs, cat.msg_Blocks.getstr(), ex_blocs)

		IF size >= 1024 THEN StringF(buffertemp,'\e3\s',v2s(Div(size,1024),'', defkbytes))
		IF size >= 1048576 THEN StringF(buffertemp,'\e3\s\e2 - \e3\s\e2',buffertemp,v2s(Div(Div(size,1024),1024),'', defmbytes))
		StringF(buffertemp2,'\e2[\e3\s\e2] \e2\s:\en \e3\d\e2 \s:\en \e3\d\e2 \s: \en \e3\d\e2 \s:\en \s \s',dirscan,cat.msg_Drawers.getstr(),exdirs,cat.msg_Files.getstr(),exfiles,cat.msg_Objects.getstr(),exdirs+exfiles,cat.msg_Size.getstr(),buffertemp,strblocs)

		get(win3,MUIA_Window_Open,{opened})
		IF opened=0 THEN set(win3,MUIA_Window_Open,1)

		set(txt_size,MUIA_Text_Contents,buffertemp2)

		IF StrCmp(bufchemin,bufnom,ALL)=TRUE
			set(txt_path, MUIA_Text_Contents, '')
			set(gr_path, MUIA_ShowMe,0)
		ELSE
			set(txt_path, MUIA_Text_Contents, bufchemin)
			set(gr_path, MUIA_ShowMe,1)
		ENDIF

		IF (arret=1)
			set(txt_abort,MUIA_Text_Contents, cat.msg_InComplete.getstr() )
			refresh()
			set(txt_abort, MUIA_ShowMe,1)
			refresh()
		ENDIF
	ENDIF
		doMethodA(app,	[MUIM_MultiSet, MUIA_Disabled, FALSE,
			vlist, bt_ComAll, bt_Com, bt_Rem, log, endgui, NIL])

		set(menusize,MUIA_Menuitem_Enabled,1)

		set(bt_Arret, MUIA_Disabled,1)
	ENDSELECT

	StringF(title1,'Scalos comment module \s',__TARGET__)
	p_WinTitle(title1)
ENDPROC

/************************** HOOKS FIN ******************************************/

PROC count(dirname)
DEF 	fib:fileinfoblock, dirlock = NIL, olddir = NIL

    IF(dirlock := Lock(dirname, ACCESS_READ))<>0
        olddir := CurrentDir(dirlock)
        IF(Examine(dirlock, fib))
       		WHILE ExNext(dirlock, fib) AND (arret=0)
			ex_blocs:=ex_blocs+fib.numblocks
			refresh()
	               	IF(fib.entrytype>0)
				exdirs++
				size:=size+fib.size
				ninit++
				refresh()
				count(fib.filename)
				refresh()

			ELSE
				size:=size+fib.size
				exfiles++
				ninit++
			ENDIF
		ENDWHILE
	ENDIF

	CurrentDir(olddir)
        UnLock(dirlock)
    ELSE
	faute()
	mreq(win, (cat.msg_TitleErrorReq.getstr() ) ,(cat.msg_BtOkReq.getstr() ) ,(cat.msg_ErrorReq.getstr() ),[dirname,IoErr(),textbuffer])
	RETURN RETURN_ERROR
    ENDIF
ENDPROC

PROC v2s(v,pref=0,post=0); DEF s:PTR TO LONG,l=0
IF pref THEN l:=StrLen(pref)
IF post THEN l:=l+StrLen(post)
l:=l+20
s:=String(l)
StringF(s,'\d',v)

checkcom(s)

IF pref THEN StringF(s,'\s\s',pref,s)
IF post THEN StringF(s,'\s\s',s,post)
ENDPROC s

PROC checkcom(s); DEF ts,l,p,len
 FOR len:=15 TO 3 STEP -3
 l:=StrLen(s)
 p:=l-len
   IF p>0
   ts:=String(l); StrCopy(ts,s)
   StrCopy(s,ts,p)
   StrAdd(s,'.')
   MidStr(ts,ts,p,ALL)
   StringF(s,'\s\s',s,ts)
   ENDIF
 ENDFOR
ENDPROC

PROC refresh()
	doMethodA(app,[MUIM_Application_InputBuffered])
ENDPROC

PROC makeLV(fixh,ajusth,ttxt,tf,msel,h) IS NListviewObject,
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
		MUIA_ContextMenu, NIL,
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

PROC makeIMG(id,cr,help,mode,spec,sel,show,freeh) IS ImageObject,
	ImageButtonFrame,
		MUIA_ExportID, id,
		MUIA_Font, MUIV_Font_Button,
	      	MUIA_InputMode, mode,
	      	MUIA_Image_Spec, spec,
	      	MUIA_Image_FreeVert, TRUE,
		MUIA_Image_FreeHoriz, freeh,
		MUIA_FixWidthTxt, 'M',
	      	MUIA_Selected, sel,
	      	MUIA_Background, MUII_ButtonBack,
	      	MUIA_ShowSelState, show,
	      	MUIA_ControlChar, cr,
		MUIA_ShortHelp, help,
	      End

CHAR VERSTAG, ' Compiled with: ', COMPILER_STRING, 0
