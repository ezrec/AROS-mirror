OPT PREPROCESS

MODULE  'workbench/startup','workbench/workbench','dos/datetime','dos/dos','utility/hooks','*getpath',
	'tools/file', 'muimaster','libraries/mui', 'mui/betterString_mcc','amigalib/boopsi','libraries/gadtools',
	'utility/tagitem','tools/installhook','icon','libraries/asl','timer','devices/timer','exec/io','*VERSTAG',
	'locale','libraries/locale','*Scalos_Protect_Locale','*ProtModule','scalos','scalos/scalos41.6','*GetIconObject'

#define prefile 'ENV:Scalos/Protect_Module.prefs'

OBJECT ttype
	name
	next:PTR TO ttype
ENDOBJECT

OBJECT disk
	size
	used
	free
	blksize
ENDOBJECT

OBJECT file
	name
	doname
	iname
	pname
	fp
	comment
	protection
	numblocks, size
	datestamp:datestamp
	infodata:infodata
	tt:ttype
	numtt
	nt:ttype
	nic
	type
	noicon
	stack
	deftool
	not
	nod
	pat
ENDOBJECT

ENUM ER_MUILIB=1,ID_CHANGES,ER_ICONLIB,ER_APP

DEF	ex_dirs=0,ex_files=0,ex_size=0,str[500]:STRING,file:file,debug=0,myname,ent=0,
	app=0,win=0,running=TRUE,result,sigs,tir,appMsgHook:hook,log,
	bufname[600]:STRING, appicon:PTR TO diskobject,ttir,lock=NIL,aslobj,bt_Rapport,arret=0,debut=1

DEF	wb:PTR TO wbstartup, args:PTR TO wbarg, rdargs,etatlog,	myargs:PTR TO LONG, marg0:PTR TO LONG,
	c=0, tirv,vuetir,filev

DEF	tr:PTR TO timerequest, at:PTR TO timeval, bb:PTR TO timeval,btime[20]:STRING,temps,m=0,
	textbuffer[80]:STRING,endgui,etatendgui,vuetime,cat:PTR TO catalog_Scalos_Protect,bt_p

DEF	ph,ps,pp,pa,pr,pw,pe,pd
DEF	ah,as,ap,aa,ar,aw,ae,ad

DEF	var,buffer[600]:STRING

PROC o_pref()
DEF bufo[2]:STRING,bufo2[2]:STRING,n,outfile=NIL,line[1024]:ARRAY OF CHAR
	n:=0
	IF outfile:=Open(prefile,OLDFILE)
        	WHILE (Fgets(outfile, line, 3))
        		n++
        		IF n=1
        			StrCopy(bufo,line,StrLen(line)-1)
				-> WriteF('n[\d] bufo = \s\n', n, bufo)
			ENDIF
		ENDWHILE

        	IF StrCmp(bufo,'0')=TRUE
        		set(endgui,MUIA_Selected,0)
        	ELSEIF StrCmp(bufo,'1')=TRUE
        		set(endgui,MUIA_Selected,1)
		ENDIF

		Close(outfile)
        ENDIF
ENDPROC

PROC s_pref()
DEF bufs[3]:STRING,outfile=NIL

	get(endgui,MUIA_Selected,{etatendgui})
	outfile:=Open(prefile,NEWFILE)
	StringF(bufs,'\d\n',etatendgui)
	Fputs(outfile,bufs)
	Close(outfile)
ENDPROC

PROC main() HANDLE 
DEF	strprotinfo,lockinfo=NIL,wintitre[100]:STRING

  IF (muimasterbase:=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))=NIL THEN Raise(ER_MUILIB)
  IF (iconbase:=OpenLibrary('icon.library',0))=NIL THEN Raise(ER_ICONLIB)
  IF (scalosbase:=OpenLibrary('scalos.library',40))=NIL THEN Raise('pas de scalos!')

  localebase := OpenLibrary( 'locale.library' , 0 )
  NEW cat.create()
  cat.open( NIL , NIL )

  installhook(appMsgHook,{appMsgFunc})

  StringF(wintitre,'\s \s',VERSTRING, __TARGET__)

  app := ApplicationObject,
    MUIA_Application_Title      , 'Protect.module',
    MUIA_Application_Version    , VERSDATE,
    MUIA_Application_Copyright  , 'COAT J_Marie, 2004',
    MUIA_Application_Author     , 'COAT J_Marie, ALIAS JMC ',
    MUIA_Application_Description, 'Set protection bits of drawers/files for SCALOS',
    MUIA_Application_Base       , 'Protect.module',
    MUIA_Application_DiskObject, get_MyDiskObject(appicon),
    SubWindow, win:= WindowObject,
      MUIA_Window_ID, "MAIN",
      MUIA_Window_ScreenTitle, VERSTRING,
      MUIA_Window_AppWindow, MUI_TRUE,
      MUIA_Window_Title, wintitre,
      MUIA_Window_AppWindow, MUI_TRUE,
      WindowContents, VGroup,
	Child,HGroup,
	  Child, Label2((cat.msg_Entry.getstr() )),
          Child, PopaslObject,
                MUIA_Popstring_String, tir:=BetterStringObject, TextFrame,
		MUIA_String_Format , MUIV_String_Format_Center ,
              End,
              MUIA_Popstring_Button, aslobj:=PopButton(MUII_PopFile),
              ASLFR_TITLETEXT, (cat.msg_Asltext.getstr() ),
          End,
	  Child, Label2((cat.msg_Type.getstr() )),
          Child, ttir:=TextObject, TextFrame,
              MUIA_Background , MUII_ListBack ,
              MUIA_Text_Contents, '',
              MUIA_Text_PreParse , '\ec\eb\e3' ,
	      MUIA_Weight, 40,
          End,
        End,
	Child, ColGroup(8), GroupFrameT( (cat.msg_GroupProtect.getstr() ) ),
           	Child, ph := makeIMG(ah, (cat.msg_HelpTextHide.getstr() ) ),
                Child, proTxtObject( (cat.msg_TextHide.getstr() ) ),

                Child, ps := makeIMG(as, (cat.msg_HelpTextScript.getstr() ) ),
                Child, proTxtObject( (cat.msg_TextScript.getstr() ) ),

                Child, pp := makeIMG(ap, (cat.msg_HelpTextPure.getstr() ) ),
                Child, proTxtObject( (cat.msg_TextPure.getstr() ) ),

                Child, pa := makeIMG(aa, (cat.msg_HelpTextArchive.getstr() ) ),
                Child, proTxtObject( (cat.msg_TextArchive.getstr() ) ),

                Child, pr := makeIMG(ar, (cat.msg_HelpTextRead.getstr() ) ),
                Child, proTxtObject( (cat.msg_TextRead.getstr() ) ),

                Child, pw := makeIMG(aw, (cat.msg_HelpTextWrite.getstr() ) ),
                Child, proTxtObject( (cat.msg_TextWrite.getstr() ) ),

		Child, pe := makeIMG(ae, (cat.msg_HelpTextExecute.getstr() ) ),
       		Child, proTxtObject( (cat.msg_TextExecute.getstr() ) ),

       		Child, pd := makeIMG(ad, (cat.msg_HelpTextDelete.getstr() ) ),
       		Child, proTxtObject( (cat.msg_TextDelete.getstr() ) ),
	End,
	Child, Mui_MakeObjectA(MUIO_HBar,[2]),

	Child,HGroup,MUIA_Frame ,MUIV_Frame_Text ,
	     	Child, RectangleObject, 
	     	End,
          	Child,Label1((cat.msg_All.getstr() )),
                	MUIA_Font, MUIV_Font_Normal,
	  	Child, log:=ImageObject, MUIA_ShortHelp, (cat.msg_HelpAll.getstr() ),
	     		ImageButtonFrame,
              		MUIA_FixWidth, 4,
              		MUIA_FixHeight, 8,
	      		MUIA_InputMode, MUIV_InputMode_Toggle,
	      		MUIA_Image_Spec, MUII_CheckMark,
	      		MUIA_Image_FreeVert, TRUE,
	      		MUIA_Selected, FALSE,
	      		MUIA_Background, MUII_ButtonBack,
	      		MUIA_ShowSelState, FALSE,
		End,
	   	Child, RectangleObject, 
	   	End,
          	Child,Label1((cat.msg_Quit.getstr() )),
                	MUIA_Font, MUIV_Font_Normal,
	  	Child, endgui:=ImageObject, MUIA_ShortHelp, (cat.msg_HelpQuit.getstr() ),
	     		ImageButtonFrame,
              		MUIA_FixWidth, 4,
              		MUIA_FixHeight, 8,
	      		MUIA_InputMode, MUIV_InputMode_Toggle,
	      		MUIA_Image_Spec, MUII_CheckMark,
	      		MUIA_Image_FreeVert, TRUE,
	      		MUIA_Selected, FALSE,
	      		MUIA_Background, MUII_ButtonBack,
	      		MUIA_ShowSelState, FALSE,
	     	End,
	     	Child, RectangleObject, 
	     	End,
 		Child, bt_p:=SimpleButton((cat.msg_Protect.getstr() )),
		Child, HGroup,
	  		Child, HSpace(1),
          		Child, bt_Rapport:=TextObject,   MUIA_ShortHelp, (cat.msg_HelpBtAbort.getstr() ),
              			MUIA_Frame, MUIV_Frame_ImageButton,
	      			MUIA_Background, MUII_HSHINEBACK,
	      			MUIA_InputMode, MUIV_InputMode_RelVerify,
              			MUIA_Text_Contents, (cat.msg_BtAbort.getstr() ),
              			MUIA_Text_PreParse , '\eb\ec' ,
	      			MUIA_Weight, 80,
            		End,
	    		Child, RectangleObject, 
	    		End,

			Child, vuetime := GroupObject,
	   			Child, HGroup,
	     				Child, RectangleObject, 
	     				End,
             				Child, Label2((cat.msg_Time.getstr() )),
             				Child, temps:= TextObject, TextFrame,
              					MUIA_Background, MUII_TextBack,
              					MUIA_Text_PreParse , '\ec' ,
              					MUIA_FixWidth, 50,
	     				End,
           			End,
         		End,
		End,
	End,

	Child, Mui_MakeObjectA(MUIO_HBar,[2]),

	Child, vuetir := GroupObject, GroupFrameT( (cat.msg_GroupEntry.getstr() ) ), MUIA_ShortHelp, (cat.msg_HelpGroupEntry.getstr() ),
          	MUIA_Frame ,MUIV_Frame_Text ,
	   	Child, HGroup,
            		Child, HSpace(1),
	    		Child, Label2((cat.msg_LabelDrawer.getstr() )),
            		Child, tirv:=TextObject, TextFrame,
              			MUIA_Background , MUII_TextBack, -> MUII_ListBack,
              			MUIA_Text_Contents, '',
              			MUIA_Text_PreParse , '\eb' ,
            		End,
		End,
	   	Child, HGroup,
            		Child, HSpace(1),
	    		Child, Label2((cat.msg_LabelFile.getstr() )),
            		Child, filev:=TextObject, TextFrame,
              			MUIA_Background , MUII_TextBack, -> MUII_ListBack,
              			MUIA_Text_Contents, '',
              			MUIA_Text_PreParse , '\e3' ,
            		End,
	    		Child, HSpace(1),
		End,
        End,
->>>
       End,
     End,
    End

  IF app=NIL THEN Raise(ER_APP)

  doMethodA(win,[MUIM_Notify,MUIA_Window_CloseRequest,MUI_TRUE,app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit])

  doMethodA(win,[MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,tir,3,MUIM_CallHook,appMsgHook,MUIV_TriggerValue])

  doMethodA(tir,[MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,app,2,MUIM_Application_ReturnID,"init"])

  doMethodA(log,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,
	app,3, MUIM_WriteLong,MUIV_TriggerValue,{etatlog}])

  doMethodA(log,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,
	app,3, MUIM_Application_ReturnID,ID_CHANGES])

  doMethodA(endgui,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,
	app,3, MUIM_WriteLong,MUIV_TriggerValue,{etatendgui}])

  doMethodA(ph,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(ps,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(pp,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(pa,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(pr,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(pw,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(pe,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])
  doMethodA(pd,[MUIM_Notify,MUIA_Selected, MUIV_EveryTime,app,3, MUIM_Application_ReturnID,ID_CHANGES])


  doMethodA(bt_Rapport,[MUIM_Notify,MUIA_Pressed,0,app,3,MUIM_WriteLong,1,{arret}])

  doMethodA(bt_p, [MUIM_Notify,MUIA_Pressed,0,app,2,MUIM_Application_ReturnID,"use"])

  doMethodA(win,[MUIM_Notify,MUIA_Window_CloseRequest,MUI_TRUE, app,3,MUIM_WriteLong,1,{arret}])

  doMethodA(win,[MUIM_Window_SetCycleChain, tir, ph,ps,pp,pa,pr,pw,pe,pd, log, endgui, NIL])

  set(app,MUIA_Application_DropObject,win)

  set(bt_Rapport,MUIA_ShowMe,0)
  set(vuetir,MUIA_ShowMe,0)
  set(vuetime,MUIA_ShowMe,0)

 o_pref()

 getargs()


 IF StrLen(str) = 0
	set(app,MUIA_Application_Iconified,TRUE)
	set(win,MUIA_Window_Open,MUI_TRUE)
  ELSEIF StrLen(str) >0
	set(win,MUIA_Window_Open,MUI_TRUE)
	set(tir,MUIA_String_Contents,file.name)
  ENDIF


    WHILE running
      result:= doMethodA(app,[MUIM_Application_Input,{sigs}])

        SELECT result
		CASE MUIV_Application_ReturnID_Quit
			s_pref()
			running:=FALSE

		CASE "init"
			getinit()
			changebtapply()

		CASE "use"
			get(tir,MUIA_String_Contents,{var})
			IF StrLen(var) <> 0
				set(bt_p,MUIA_Disabled,1)
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
    					diskobjects()
                        		IF ent AND (etatlog=1)
						examine()
                        		ELSEIF (ent=0) AND (etatlog=0)
						protectfile(file.name)
					ENDIF
					strprotinfo:=String(StrLen(buffer)+5)
					StrCopy(strprotinfo,buffer,ALL)
					StrAdd(strprotinfo,'.info')
					-> WriteF('\s\n',strprotinfo)
					IF (lockinfo := Lock(strprotinfo, ACCESS_READ))
						file.name:=strprotinfo
						protectfile(file.name)
						UnLock(lockinfo)

					ENDIF


				ENDIF
				IF lock THEN UnLock(lock)
				IF (etatendgui=1)
					s_pref()
					running:=FALSE
				ELSE
					file.name:=NIL
					file.pname:=NIL
					var:=NIL
					getinit()
					changebtapply()
				ENDIF
				s_pref()
				-> set(bt_p,MUIA_Disabled,0)
			ELSE
				set(ttir,MUIA_Text_Contents, (cat.msg_TextNothing.getstr() ) )
				set(win,MUIA_Window_ActiveObject,tir)
			ENDIF

		CASE ID_CHANGES
			changebtapply()
         ENDSELECT

      IF (running AND sigs) THEN Wait(sigs)
    ENDWHILE
  FreeDiskObject(appicon)

EXCEPT DO
  IF app THEN Mui_DisposeObject(app)
  IF muimasterbase THEN CloseLibrary(muimasterbase)
  IF iconbase THEN CloseLibrary(iconbase)
  IF scalosbase THEN CloseLibrary(scalosbase)

  cat.close()
  IF localebase THEN CloseLibrary( localebase )
  SELECT exception
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

->**************
PROC getinit()
	get(tir,MUIA_String_Contents,{var})
	IF StrLen(var) <> 0
               	IF (lock := Lock(var, ACCESS_READ)) = 0
			faute()
			mreq(win, (cat.msg_TitleErrorReq.getstr() ) , (cat.msg_BtOkReq.getstr() ), (cat.msg_ErrorReq.getstr() ),[var,IoErr(),textbuffer])
			set(ttir,MUIA_Text_Contents, (cat.msg_NoFound.getstr() ))
               	ELSE
			StrCopy(buffer,var)
                       	IF buffer[StrLen(buffer)-1]="/"
				 SetStr(buffer,StrLen(buffer)-1)
			ENDIF

               		file.name:=buffer
			StrCopy(str,buffer)

			file.type:=-10
			debut:=1
			getprotect()
    			diskobjects()
			debut:=0
		ENDIF
		IF lock THEN UnLock(lock)
	ELSE
		set(win,MUIA_Window_ActiveObject,tir)
	ENDIF
ENDPROC

PROC examine()
DEF lock=0,fib:fileinfoblock

 m:=0
 set(vuetime,MUIA_ShowMe,1)
 doMethodA(aslobj,[MUIM_Set,MUIA_Disabled,TRUE])
 set(bt_Rapport,MUIA_ShowMe,1)
 set(vuetir,MUIA_ShowMe,1)
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
 NEW tr, at, bb
 IF OpenDevice('timer.device', UNIT_MICROHZ, tr, 0)=0  ->>> TIMER  
  	timerbase:=tr.io.device
  	GetSysTime(at)
 ENDIF
 getdir()
 set(vuetime,MUIA_ShowMe,0)
 doMethodA(aslobj,[MUIM_Set,MUIA_Disabled,FALSE])
ENDPROC

PROC getdir()
	exdir(file.name)
	IF arret
 		set(bt_Rapport,MUIA_ShowMe,0)
 		set(vuetir,MUIA_ShowMe,0)
		set(vuetime,MUIA_ShowMe,0)
 		set(aslobj,MUIA_Disabled,FALSE)
 	ELSE
 		set(app,MUIA_Application_Iconified, FALSE)
		set(bt_Rapport,MUIA_ShowMe,0)
 		set(vuetir,MUIA_ShowMe,0)
 		Dispose(at)
 		Dispose(bb)
 		CloseDevice(tr)
 	ENDIF
ENDPROC

PROC exdir(dir)
DEF fib:PTR TO fileinfoblock,dirlock=NIL,path[256]:STRING,files[256]:STRING,olddir = NIL

IF(dirlock := Lock(dir,SHARED_LOCK))
	olddir := CurrentDir(dirlock)
        fib := AllocDosObject(DOS_FIB, NIL)
        IF (Examine(dirlock, fib))
       		WHILE ExNext(dirlock, fib) AND (arret=0)
        		IF(fib.entrytype=-3) -> Fichier
				file.pname:=fib.filename
				StrCopy(files,fib.filename,ALL)
        			set(filev,MUIA_Text_Contents,files)
				doMethodA(app,[MUIM_Application_InputBuffered])
                		protectfile(file.pname)
				doMethodA(app,[MUIM_Application_InputBuffered])
        			ex_files++
        			ex_size:=ex_size+fib.size

        		ELSE
				NameFromLock(dirlock,path,StrMax(path))
        			-> StrCopy(path,fib.filename)
        			set(tirv,MUIA_Text_Contents,path)
				file.pname:=fib.filename
				protectfile(file.pname)
				doMethodA(app,[MUIM_Application_InputBuffered])
        			exdir(fib.filename)
				doMethodA(app,[MUIM_Application_InputBuffered])
        		ENDIF
				GetSysTime(bb)
				SubTime(bb,at)
				IF bb.secs>59
					Dispose(bb)
					Dispose(at)
					m:=m+1
	        			GetSysTime(at)
					GetSysTime(bb)
					SubTime(bb,at)
				ENDIF
			get_time()
      		ENDWHILE
    	ENDIF
  	UnLock(dirlock)
  ENDIF
  CurrentDir(olddir)
  FreeDosObject(DOS_FIB, fib)
ENDPROC

->**************

PROC get_time()
	IF m<10
		IF bb.secs<10
			StringF(btime,'0\d:0\d:\d[02]', m,bb.secs, bb.micro)
		ELSE
			StringF(btime,'0\d:\d:\d[02]', m,bb.secs, bb.micro)
		ENDIF
	ELSE
		StringF(btime,'\d:\d:\d[02]', m,bb.secs, bb.micro)
	ENDIF
        set(temps,MUIA_Text_Contents,btime)
ENDPROC

PROC getargs()
  IF wbmessage=NIL
  myname:=getprogname()
  myargs:=[0]
    IF rdargs:=ReadArgs('Files/A/M',myargs,NIL)
    marg0:=myargs[0]
      WHILE marg0[]
      StrCopy(str,marg0[])
        IF str[StrLen(str)-1]="/"
         str[StrLen(str)-1]:=0
        ENDIF
      IF c=0 THEN file.name:=news(str) ELSE loadfile(myname,str)
      marg0[]++
      c++
      ENDWHILE
    ent:=1
    ENDIF
  ELSE						/* from wb */
    wb:=wbmessage
   args:=wb.arglist
    NameFromLock(args[0].lock,str,256)
    IF StrLen(args[0].name)>0 THEN AddPart(str, args[0].name,256)
    myname:=news(str)
    ent:=0
    IF wb.numargs>=2
      IF wb.numargs>2
        FOR rdargs:=2 TO wb.numargs-1
        NameFromLock(args[rdargs].lock,str,256)
        IF StrLen(args[rdargs].name)>0 THEN AddPart(str, args[rdargs].name,256)
        loadfile(myname,str)
        ENDFOR
      ENDIF
    NameFromLock(args[1].lock,str,256)
    IF StrLen(args[1].name)>0 THEN AddPart(str, args[1].name,256)
    file.name:=news(str)
    ENDIF
  ENDIF
  file.type:=-10
  StringF(str,'\s',file.name)
  getprotect()
  diskobjects()
  debut:=0

ENDPROC

PROC diskobjects()
DEF	dlock=NIL, info:PTR TO fileinfoblock

  info:=AllocDosObject(DOS_FIB, NIL)
  IF dlock:=Lock(file.name,-2)
   	Examine(dlock, info)
	IF info.direntrytype=-3
		ent:=0
		set(log,MUIA_Disabled,1)
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeProject.getstr() ))
	ELSEIF info.direntrytype=1
		ent:=1
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeDisk.getstr() ))
		set(log,MUIA_Selected,1)
		StrCopy(str,file.name)
	ELSEIF info.direntrytype=2
		ent:=1
  		IF (debut=1) THEN initfile(file.name) ELSE comfile(file.name)
 		set(log,MUIA_Disabled,0)
        	set(ttir,MUIA_Text_Contents,(cat.msg_TypeDrawer.getstr() ))
        	StrCopy(str,file.name)
		IF str[(StrLen(str)-1)]=":" THEN set(ttir,MUIA_Text_Contents,(cat.msg_TypeAssign.getstr() ))
		IF str[(StrLen(str)-1)]<>":" THEN
        	StrAdd(str,'/')
        	file.name:=str
       	ENDIF

	-> WriteF('type = \d\n',info.direntrytype)

    	UnLock(dlock)
  ENDIF
  FreeDosObject(DOS_FIB, info)
ENDPROC

->*********************

PROC initfile(fichier)
DEF fib:fileinfoblock,lock=0

 IF lock:=Lock(fichier,-2)
   Examine(lock,fib)
   file.numblocks:=fib.numblocks
   file.size:=fib.size
   getprotect()
   file.comment:=fib.comment
   UnLock(lock)
 ELSE
   faute()
   mreq(win,(cat.msg_TitleErrorReq.getstr() ), (cat.msg_BtOkReq.getstr() ), (cat.msg_ErrorReq.getstr() ) ,[fichier,IoErr(),textbuffer])
 ENDIF
ENDPROC

PROC comfile(fichier)
DEF fib:fileinfoblock,lock=0

 IF lock:=Lock(fichier,-2)
   Examine(lock,fib)
   file.pname:=fichier
   protectfile(fichier)
   UnLock(lock)
 ENDIF
ENDPROC

->*********************

PROC faute()
	Fault(IoErr(),NIL,textbuffer,80)
ENDPROC

PROC mreq(obj,tobj,tbt,tmreq,tamp)
	Mui_RequestA(app, obj, 0, tobj, tbt, tmreq, tamp)
ENDPROC

PROC appMsgFunc(hook,obj,x:PTR TO LONG)
  DEF ap:PTR TO wbarg,
      amsg:PTR TO appmessage,
      i,
      buf[256]:STRING,b,bufapp[256]:STRING

  amsg:=x[]
  b:=buf;i:=0
  ap:=amsg.arglist
  WHILE (i<amsg.numargs)
    NameFromLock(ap.lock,buf,StrMax(buf))
    AddPart(buf,ap.name,StrMax(buf))
    file.name:=b
    StringF(bufapp,'\s',b)
    loadfile(myname,buf)
    ap++
    i++
  ENDWHILE
ENDPROC


PROC loadfile(prog,file=0); DEF str[600]:STRING
StringF(str,'STACK 10000\nrun <>nil: "\s"',prog)
IF file THEN StringF(str,'\s "\s"',str,file)
IF debug THEN WriteF('\s\n',str)
Execute(str,0,stdout)
ENDPROC


PROC getprotect()
DEF lock=0,fib:fileinfoblock

 file.pname:=file.name
 IF lock:=Lock(file.name,-2)
 	Examine(lock,fib)
 	file.protection:=fib.protection
 	CopyMem(fib.datestamp,file.datestamp,SIZEOF datestamp)
 	ah := IF file.protection AND FIBF_HIDDEN THEN 1 ELSE 0
 	as := IF file.protection AND FIBF_SCRIPT THEN 1 ELSE 0
 	ap := IF file.protection AND FIBF_PURE THEN 1 ELSE 0
 	aa := IF file.protection AND FIBF_ARCHIVE THEN 1 ELSE 0
 	ar := IF file.protection AND FIBF_READ THEN 0 ELSE 1
 	aw := IF file.protection AND FIBF_WRITE THEN 0 ELSE 1
 	ae := IF file.protection AND FIBF_EXECUTE THEN 0 ELSE 1
 	ad := IF file.protection AND FIBF_DELETE THEN 0 ELSE 1
	IF (debut=1)
		set(ph,MUIA_Selected,ah)
		set(ps,MUIA_Selected,as)
		set(pp,MUIA_Selected,ap)
		set(pa,MUIA_Selected,aa)
		set(pr,MUIA_Selected,ar)
		set(pw,MUIA_Selected,aw)
		set(pe,MUIA_Selected,ae)
		set(pd,MUIA_Selected,ad)
	ENDIF
 	UnLock(lock)
 ENDIF

ENDPROC

PROC changebtapply()
DEF	vh,vs,vp,va,vr,vw,ve,vd

  IF file.pname<>0
  	get(ph,MUIA_Selected,{vh})
  	get(ps,MUIA_Selected,{vs})
  	get(pp,MUIA_Selected,{vp})
  	get(pa,MUIA_Selected,{va})
  	get(pr,MUIA_Selected,{vr})
  	get(pw,MUIA_Selected,{vw})
  	get(pe,MUIA_Selected,{ve})
  	get(pd,MUIA_Selected,{vd})
    	IF (vh<>ah) OR (vs<>as) OR (vp<>ap) OR (va<>aa) OR (vr<>ar) OR (vw<>aw) OR (ve<>ae) OR (vd<>ad)
		set(bt_p,MUIA_Disabled,0)
	ELSE
		IF etatlog THEN set(bt_p,MUIA_Disabled,0) ELSE set(bt_p,MUIA_Disabled,1)
	ENDIF
  ENDIF
ENDPROC

PROC protectfile(fichier)
DEF	nh=NIL,ns=NIL,np=NIL,na=NIL,nr=NIL,nw=NIL,ne=NIL,nd=NIL,nprot=0,pt=NIL


  	get(ph,MUIA_Selected,{nh})
  	get(ps,MUIA_Selected,{ns})
  	get(pp,MUIA_Selected,{np})
  	get(pa,MUIA_Selected,{na})
  	get(pr,MUIA_Selected,{nr})
  	get(pw,MUIA_Selected,{nw})
  	get(pe,MUIA_Selected,{ne})
  	get(pd,MUIA_Selected,{nd})

    	IF nh=1	THEN 	nprot:=nprot+FIBF_HIDDEN
    	IF ns=1	THEN 	nprot:=nprot+FIBF_SCRIPT
    	IF np=1	THEN 	nprot:=nprot+FIBF_PURE
    	IF na=1	THEN 	nprot:=nprot+FIBF_ARCHIVE
    	IF nr=0 THEN 	nprot:=nprot+FIBF_READ
    	IF nw=0 THEN 	nprot:=nprot+FIBF_WRITE
    	IF ne=0 THEN 	nprot:=nprot+FIBF_EXECUTE
    	IF nd=0 THEN 	nprot:=nprot+FIBF_DELETE
    	pt:=SetProtection(fichier,nprot)


	-> WriteF('\s:\tH=\d S=\d P=\d A=\d R=\d W=\d E= \d D=\d\n',fichier,nh,ns,np,na,nr,nw,ne,nd)

	IF pt=FALSE
		faute()
		mreq(win, (cat.msg_TitleErrorReq.getstr() ) , (cat.msg_BtOkReq.getstr() ), (cat.msg_ErrorReq.getstr() ),[fichier,IoErr(),textbuffer])
	ENDIF
ENDPROC

PROC proTxtObject(s) IS TextObject,
          MUIA_Text_Contents,s,
          End

PROC makeIMG(sel,help) IS ImageObject,
	ImageButtonFrame,
              	/* MUIA_FixWidth, 4,
              	MUIA_FixHeight, 8, */
	      	MUIA_InputMode, MUIV_InputMode_Toggle,
	      	MUIA_Image_Spec, MUII_CheckMark,
	      	MUIA_Image_FreeVert, TRUE,
	      	MUIA_Selected, sel,
	      	MUIA_Background, MUII_ButtonBack,
	      	MUIA_ShowSelState, FALSE,
		MUIA_ShortHelp, help,
	      End

CHAR VERSTAG, __TARGET__, 0
