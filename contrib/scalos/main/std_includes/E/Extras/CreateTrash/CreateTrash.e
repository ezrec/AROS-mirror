OPT PREPROCESS, REG=5

MODULE  'workbench/startup','workbench/workbench','dos/dos','*mod',
	'tools/file', 'muimaster','libraries/mui', 'mui/betterString_mcc',
	'amigalib/boopsi','libraries/gadtools','utility/tagitem','icon'

OBJECT ttype;name,next:PTR TO ttype; ENDOBJECT
OBJECT disk;  size, used, free, blksize  ; ENDOBJECT

OBJECT file
  name, doname
  infodata:infodata
  type
ENDOBJECT

ENUM ER_MUILIB=1,ER_APP,ERR_ICON

DEF	str[500]:STRING,file:file,debug=0,myname,ent=0,app=0,win=0,running=TRUE,
	result,sigs,tir,appicon:PTR TO diskobject,ttir,bt_Rapport,wb:PTR TO wbstartup,
	args:PTR TO wbarg,rdargs,myargs:PTR TO LONG, marg0:PTR TO LONG,textbuffer[80]:STRING,path[600]:STRING,
	name[600]:STRING,corb,var,bt_Quit,c=0,gr_Virtual

PROC main() HANDLE 

  IF (muimasterbase:=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))=NIL THEN Raise(ER_MUILIB)
  IF (iconbase:=OpenLibrary('icon.library',0))=NIL THEN Raise(ERR_ICON)

  app := ApplicationObject,
    MUIA_Application_Title      , 'CreateTrash',
    MUIA_Application_Version    , '$VER: CreateTrash 1.1 (24.07.05) © Jean-Marie COAT - The Scalos Team',
    MUIA_Application_Copyright  , 'COAT J_Marie, 2003-2005',
    MUIA_Application_Author     , 'COAT J_Marie, ALIAS JMC ',
    MUIA_Application_Description, 'Create trashcans for SCALOS.',
    MUIA_Application_Base       , 'CREATETRASH',
    MUIA_Application_DiskObject, appicon:= GetDiskObject('PROGDIR:CreateTrash'),

    SubWindow, win:= WindowObject,
      -> MUIA_Window_ID, "MAIN",
      MUIA_Window_UseBottomBorderScroller, 1,
      MUIA_Window_SizeGadget, FALSE,
      -> MUIA_Window_SizeRight, TRUE,
      MUIA_Window_DragBar, FALSE,
      MUIA_Window_DepthGadget, FALSE,
      MUIA_Window_CloseGadget, FALSE,
      MUIA_Window_Width, MUIV_Window_Width_Screen(35),

      WindowContents, VGroup,
	Child, TextObject,
              MUIA_Text_Contents, '\ebCreateTrash \e31.1\en\e1 \ei(c)2003-2005 JMC - The Scalos Team\en\nModule for \ebSCALOS\en',
              MUIA_Text_PreParse , '\ec',
          End,
	  Child, HGroup,	
	  	Child, GroupObject,
	  		Child, gr_Virtual := ScrollgroupObject,
				MUIA_Scrollgroup_VertBar, NIL,
				MUIA_Scrollgroup_HorizBar, NIL,
				MUIA_Scrollgroup_FreeHoriz, 1,
				MUIA_Scrollgroup_FreeVert, 0,
				MUIA_Scrollgroup_UseWinBorder, 1,
				MUIA_Scrollgroup_Contents,
				VirtgroupObject,
					Child, HGroup, MUIA_Weight, 75,
	  					Child, Label2('\ebPath :'),
          					Child, tir:=TextObject, TextFrame,
							MUIA_Background , MUII_TextBack,
              						MUIA_Text_PreParse , '\ec\eb' ,
              						MUIA_Text_Contents, '',
          					End,
					End,
				End,
			End,
		End,
		Child, HGroup, MUIA_Weight, 25,
			Child, Label2('\ebType :'),
			Child, ttir:=TextObject, TextFrame,
				MUIA_Background , MUII_TextBack ,
				MUIA_Text_Contents, '',
				MUIA_Text_PreParse , '\ec\eb\e3' ,
			End,
		End,
	End,

	Child, HGroup,MUIA_Frame, MUIV_Frame_Text,
	  Child, Label2('\ebName of trashcan :'),
          Child, corb:=BetterStringObject, TextFrame,
		MUIA_ExportID, 1,
		MUIA_String_Format , MUIV_String_Format_Center ,
          End,
	End,
	Child,HGroup, MUIA_Frame, MUIV_Frame_Text,
          Child, bt_Rapport:=SimpleButton(' _Create '),
	  Child, HSpace(0),
          Child, bt_Quit:=SimpleButton(' _Quit '),
	End,
	End,
->>>
      End,
    End

  IF app=NIL THEN Raise(ER_APP)

  doMethodA(win,[MUIM_Notify,MUIA_Window_CloseRequest,MUI_TRUE,app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit])

  doMethodA(bt_Rapport,[MUIM_Notify,MUIA_Pressed,0,app,2,MUIM_Application_ReturnID,"trah"])

  doMethodA(corb,[MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,app,2,MUIM_Application_ReturnID,"trah"])

  doMethodA(bt_Quit,[MUIM_Notify,MUIA_Pressed,0,app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit])

  doMethodA(app,[MUIM_Application_Load,MUIV_Application_Load_ENV])

 getargs()

IF wb.numargs>1
	IF ent
  		set(tir,MUIA_Text_Contents,file.name)
		set(win,MUIA_Window_Open,MUI_TRUE)
		set(win,MUIA_Window_ActiveObject, corb)
  	ELSE
		Mui_RequestA(app,NIL,0,'CreateTrash : Error !',' \ebOkay ','\eb\s\en\n is wrong object type !',{str})
		running:=FALSE
  	ENDIF
ELSE
	Mui_RequestA(app,NIL,0,'CreateTrash : Error !',' \ebOkay ','\ecNo argument loaded !\n\ebScalos Menu configuration\en :\nLoad \ebCreateTrash\en then, pess \ebARGS WB\en\nand select \ebWorkbench\en.\n\nYou can insert \ebCreateTrash\en in :\n - PopupMenu: Disks\n - PopupMenu: Windows\n - PopupMenu: Drawers.',NIL)
	senv()
	running:=FALSE
ENDIF

    WHILE running
      result:= doMethodA(app,[MUIM_Application_Input,{sigs}])

        SELECT result
		CASE MUIV_Application_ReturnID_Quit
			senv()
			running:=FALSE

		CASE "trah"
			trash()
         ENDSELECT

      IF (running AND sigs) THEN Wait(sigs)
    ENDWHILE
  FreeDiskObject(appicon)
EXCEPT DO
  IF app THEN Mui_DisposeObject(app)
  IF muimasterbase THEN CloseLibrary(muimasterbase)
  IF iconbase THEN CloseLibrary(iconbase)
  
  SELECT exception
    CASE ER_MUILIB
      WriteF('Failed to open \s.\n',MUIMASTER_NAME)
      CleanUp(20)
    CASE ERR_ICON
      WriteF('Failed to open icon.library')
      CleanUp(20)
    CASE ER_APP
      WriteF('Failed to create application !\n')
      CleanUp(20)
  ENDSELECT
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
    			FreeArgs(rdargs)
    		ENDIF
	ELSEIF wb:=wbmessage
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
    	diskobjects()
ENDPROC

PROC diskobjects()
DEF do:diskobject,type

  IF do:=GetDiskObjectNew(file.name)
  type:=do.type
  file.type:=type
  IF (file.type=WBTOOL)
	ent:=0
        set(ttir,MUIA_Text_Contents,'Tool')
  ELSEIF (file.type=WBPROJECT)
	ent:=0
        set(ttir,MUIA_Text_Contents,'Project')
  ELSEIF (file.type=WBDRAWER)
        set(ttir,MUIA_Text_Contents,'Drawer')
        StrCopy(str,file.name)
	IF str[(StrLen(str)-1)]=":" THEN set(ttir,MUIA_Text_Contents,'Assign')
        file.name:=str
	ent:=1
  ELSEIF (file.type=WBDISK)
        set(ttir,MUIA_Text_Contents,'Disk')
	ent:=1
  ELSEIF (file.type=WBGARBAGE)
        set(ttir,MUIA_Text_Contents,'Trashcan')
        StrCopy(str,file.name)
        file.name:=str
	ent:=1
  ENDIF
  FreeDiskObject(do)
ENDIF
ENDPROC

PROC getprogname()
DEF l,d,f[56]:STRING
IF l:=GetProgramDir()
d:=String(256)
NameFromLock(l,d,256)
GetProgramName(f,56)
AddPart(d,f,256)
SetStr(d,StrLen(d))
ENDIF
ENDPROC d

PROC loadfile(prog,file=0); DEF str[600]:STRING
StringF(str,'STACK 10000\nrun <>nil: "\s"',prog)
IF file THEN StringF(str,'\s "\s"',str,file)
IF debug THEN WriteF('\s\n',str)
Execute(str,0,stdout)
ENDPROC

PROC trash()
DEF root_lock,old_lock,disk_obj,trash_lock,bcomp[500]:STRING,lockicon

StrCopy(path,str,ALL)
get(corb,MUIA_String_Contents,{var})
IF StrLen(var)<>0
	name:=var
	StringF(bcomp,'\s.info',var)
	   root_lock := Lock(path, SHARED_LOCK )
	    IF(root_lock <> 0)
		old_lock := CurrentDir(root_lock)
		trash_lock := CreateDir(name)
		IF(trash_lock <> 0 )
			UnLock(trash_lock)
		ELSE
			faute()
		ENDIF
		IF (lockicon := Lock(bcomp,SHARED_LOCK))
			UnLock(lockicon)
			Mui_RequestA(app,win,0,'CreateTrash : Error !',' \ebOkay','Icon : \eb\s\en already exists !',{bcomp})
		ELSE
			disk_obj := GetDefDiskObject(WBGARBAGE)
			IF(disk_obj <> 0 )
				IF(PutDiskObject(name,disk_obj))
					FreeDiskObject(disk_obj)
				ELSE
					faute()
		    		ENDIF
		ENDIF
		CurrentDir(old_lock)
		UnLock(root_lock)
		senv()
		running:=FALSE
	    ENDIF
  	ENDIF
ELSE
	Mui_RequestA(app,win,0,'CreateTrash : Error !',' \ebOkay ','\ecTextfield is empty !\nEnter a name for new trashcan to create in:\n\eb\s\en.', {path})
ENDIF

ENDPROC

PROC senv()
	doMethodA(app,[MUIM_Application_Save,MUIV_Application_Save_ENV])
ENDPROC

PROC faute()
	Fault(IoErr(),NIL,textbuffer,80)
	Mui_RequestA(app, win, 0, ' CreateTrash : Error !', ' \ebOkay ','ErrorCode : \d\nError : \s', [IoErr(),textbuffer])
ENDPROC
