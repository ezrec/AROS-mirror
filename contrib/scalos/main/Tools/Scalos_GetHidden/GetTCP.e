OPT PREPROCESS
OPT MODULE
OPT STACK=36000

MODULE 'muimaster','libraries/mui','libraries/muip',
       'mui/muicustomclass','amigalib/boopsi','libraries/gadtools',
       'intuition/classes','intuition/classusr',
       'utility/tagitem',
       'intuition/screens','intuition/intuition','debug',
	'*Scalos_GetHidden_Locale','locale','libraries/locale'

ENUM	MUIV_Application_Abort=1

DEF	cat:PTR TO catalog_Scalos_GetHidden

EXPORT PROC httpGET(url,dst,args=0:PTR TO LONG,openMUI=0) HANDLE
DEF	app=0, window, sigs=0, running=TRUE, result, gob,
	str, v, deftitle, abort, ts, server, fpart, fname, dest, bufNUM=1024, gui=1, beep=0, message_abort,
	infile=0, outfile=0, max=0, type, len=0, size=0, afile=0, flines=0, eof=0, buf, dbug=0,arret=0 -> , busy

	str:=String(4096)

	localebase := OpenLibrary( 'locale.library' , 0 )
	NEW cat.create()
	cat.open( NIL , NIL )

	message_abort := cat.msg_DownloadAbortedByUser.getstr() -> 'Download aborted by user!'

	IF args<>0
		FOR v:=0 TO ListLen(args)-1
			StrCopy(str,ListItem(args,v))
			IF (InStr(str,'BUFFERS=')=0) OR (InStr(str,'BUF=')=0)
				MidStr(str,str,InStr(str,'=')+1,ALL)
				bufNUM:=Val(str)
			ELSEIF (StrCmp(str,'NOGUI')) OR (StrCmp(str,'NOG'))
				gui:=0
			ELSEIF StrCmp(str,'BEEP')
				beep:=1
			ENDIF
		ENDFOR
	ENDIF

	buf:=String(bufNUM+1)

	deftitle:='httpGET 1.0'
	dest:=String(StrLen(dst)+StrLen(FilePart(url))+1); StrCopy(dest,dst)

	IF dbug THEN kPrintF('TCPGet2()/httpGET(\s,\s,args=0:PTR TO LONG,\d)\n', [url, dst, openMUI])
	IF dbug THEN kPrintF('TCPGet2()/dest=<\s>\n', [dest])

	IF gui
		IF openMUI THEN
		IF (muimasterbase:=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))=NIL THEN	Raise(WriteF(cat.msg_ErrorMuimaster.getstr() , MUIMASTER_NAME) )

		app:=ApplicationObject,
			MUIA_Application_Title      , deftitle,
			MUIA_Application_Version    , 'HTCPGET 1.1',
			MUIA_Application_Copyright  , '©2004-2009, Jean-Marie COAT',
			MUIA_Application_Author     , 'Jean-Marie COAT',
			MUIA_Application_Description, 'Uses HTTP "GET" to retrieve files',
			MUIA_Application_Base       , 'GETHIDDEN_GETTCP',
			SubWindow, window:= WindowObject,
				MUIA_Window_Title    , news(FilePart(url)),
				MUIA_Window_ID       , "HTCP",
				MUIA_Window_Width,400,
				WindowContents, VGroup,
					Child, VGroup, GroupFrame,
						MUIA_FrameTitle, cat.msg_FrameTitle.getstr(),
						Child, gob := GaugeObject, GaugeFrame,
							MUIA_Gauge_Current,100,
							MUIA_Gauge_Max,100,
							MUIA_Gauge_Horiz,1,
							MUIA_Gauge_InfoText, cat.msg_Initialising.getstr(),
						End,
						Child, ScaleObject,
							MUIA_Scale_Horiz, MUI_TRUE,
						End,
					End,
					-> Child, busy:= BusyBar, MUIA_ShowMe, 1,
					Child, abort := SimpleButton( cat.msg_BtAbort.getstr() ),
				End,
			End,
		End

		IF (app=NIL) THEN Raise(cat.msg_ErrorApp.getstr() )

		doMethodA(window,	[MUIM_Notify, MUIA_Window_CloseRequest, MUI_TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit])

		doMethodA(abort,	[MUIM_Notify, MUIA_Pressed, 0, app, 2, MUIM_Application_ReturnID, MUIV_Application_Abort])

		set(window,MUIA_Window_Open, MUI_TRUE)
	ENDIF

	StrCopy(str,url)
	ts:=String(StrLen(str))

	IF InStr(str,'http://')=0 THEN MidStr(str,str,7,ALL)

	StrCopy(ts,str,InStr(str,'/'));
	server:=news(StringF(ts,'tcp:\s/80',ts))

	IF dbug THEN kPrintF('TCPGet2()/ts = <\s> - server = \s\n', [ts, server])

	MidStr(str,str,StrLen(server)+1,ALL);

	fpart:=news(str)
	fname:=news(FilePart(fpart))

	IF dbug THEN kPrintF('TCPGet2()/fpart = \s\n', [fpart])
	IF dbug THEN kPrintF('TCPGet2()/dest = \s\n', [fname])

	StringF(dest,'\s\s\s',dest,IF (dest[StrLen(dest)-1]<>":") AND (dest[StrLen(dest)-1]<>"/") THEN '/' ELSE '',fname)

	IF dbug THEN kPrintF('TCPGet2()/dest = \s\n', [dest])

	IF gui
		set(gob,MUIA_Gauge_Current,0)
		setinf(gob,StringF(str, cat.msg_Connecting.getstr() ,server))
		IF dbug THEN kPrintF('TCPGet2()/Connecting to \s...\n', [server])
	ENDIF

	IF (infile:=Open(server,OLDFILE))=0
		-> set(busy, MUIA_ShowMe,0)
		Raise(StringF(str, cat.msg_TCPError.getstr() ,server))
	ENDIF

	IF gui
		setinf(gob,StringF(str, cat.msg_GettingFile.getstr() , FilePart(url)))
		IF dbug THEN kPrintF('TCPGet2()/PASS1: str = "\s"\n', [str])
	ENDIF

	StringF(str,'GET \s HTTP/1.0\b\n',url)

	setinf(gob,'')

	IF dbug THEN kPrintF('TCPGet2()/PASS2: write(infile,"\s")\n', [str])

	write(infile,str)
	Write(infile,'\b\n',STRLEN)

	IF (outfile:=Open(dest,NEWFILE))=0
		Close(infile);
		Raise(StringF(str, cat.msg_FileOpenError.getstr() ,dest))
	ENDIF

	size:=0; afile:=0 ; flines:=0; eof:=1

	WHILE (infile<>0) AND (CtrlC()=0) AND (running<>0)
		IF (afile=0) AND (infile<>0) AND (CtrlC()=0)
			eof:=ReadStr(infile,str)
			IF eof=-1 THEN Raise( cat.msg_FileEndedUnexpectedly.getstr() )
			UpperStr(trimcr(str))
			IF InStr(str,'CONTENT-LENGTH:')=0
				MidStr(str,str,16,ALL)
				max:=Val(str)
				IF gui THEN set(gob,MUIA_Gauge_Max,max)
			ELSEIF InStr(str,'CONTENT-TYPE:')=0
				MidStr(str,str,14,ALL)
				type:=news(str)
				StringF(str,'MIME Type = \s',type)
				IF gui THEN setinf(gob,str)
			ELSEIF str[]=0
				afile:=1
			ENDIF

		ELSEIF (infile<>0) AND (CtrlC()=0) AND (running<>0)
			eof:=Fread(infile,buf,1,bufNUM)
			IF eof>0
				len := Fwrite(outfile,buf,1,eof)
				size:=size+len
				IF (size>0) AND (gui<>0) THEN updategauge(gob, Min(size,max),max,fname)
			ENDIF
			IF eof<1 THEN running:=0
		ELSE
			running:=0
		ENDIF

		IF gui
			checkresult:
				result:=doMethodA(app,[MUIM_Application_NewInput,{sigs}])
				IF result=MUIV_Application_Abort
					setinf(gob,message_abort)
					running:=0
					IF outfile THEN Close(outfile)
					IF infile THEN Close(infile)
					outfile:=0
					infile:=0
					arret:=1
					cat.close()
				ELSEIF result=MUIV_Application_ReturnID_Quit
					running:=0
					IF outfile THEN Close(outfile)
					IF infile THEN Close(infile)
					outfile:=0
					infile:=0
					cat.close()
				ENDIF
				IF result<>0 THEN JUMP checkresult
		ENDIF
	ENDWHILE

	EXCEPT DO
		IF outfile THEN Close(outfile)
		IF infile THEN Close(infile)
		-> DisposeLink(str)
		-> DisposeLink(buf)
		-> DisposeLink(dest)

	IF gui
		IF app THEN Mui_DisposeObject(app)
		IF openMUI THEN
			IF muimasterbase THEN CloseLibrary(muimasterbase)
		ENDIF
		IF beep THEN DisplayBeep(0)
		IF arret
			RETURN message_abort
		ENDIF
ENDPROC exception

PROC news(s)
DEF 	ns
	ns:=String(StrLen(s))
	StrCopy(ns,s)
ENDPROC ns

PROC setinf(obj,info) IS set(obj,MUIA_Gauge_InfoText,info)

PROC updategauge(obj,size,max,name)
DEF	s[200]:STRING
	StringF(s, cat.msg_TextGauge.getstr() ,name,size,max,Div(size*100,max))
	set(obj,MUIA_Gauge_InfoText,s)
	set(obj,MUIA_Gauge_Current,size)
ENDPROC

PROC write(fh,s) IS Write(fh,s,StrLen(s))
PROC trimcr(s)
	IF s[StrLen(s)-1]="" THEN s[StrLen(s)-1]:=0
ENDPROC s

