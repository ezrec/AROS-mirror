OPT MODULE
OPT PREPROCESS
OPT STACK=36000

MODULE 'libraries/mui','muimaster','*GetTCP','debug','*TCPHeaders','*Scalos_GetHidden_Locale','locale','libraries/locale'

EXPORT OBJECT updObject
	app:PTR TO LONG
	win:PTR TO LONG
	name:PTR TO CHAR
	deft:PTR TO CHAR
	ver:PTR TO CHAR

	msg_chk
	msg_yesno
	msg_nopen
	msg_unfound
	msg_latest
	msg_better
	msg_found
	msg_gads
	msg_aborted
	msg_error
	msg_complete
ENDOBJECT

DEF	cat:PTR TO catalog_Scalos_GetHidden

EXPORT PROC update(up:PTR TO updObject) HANDLE;
DEF v,c=0,s,ts,look=1,over,path,b_str[4096]:STRING,search, t_Log, dbug=0,file_name[256]:STRING,file_Log[256]:STRING

	localebase := OpenLibrary( 'locale.library' , 0 )
	NEW cat.create()
	cat.open( NIL , NIL )

	IF (up.app=0) OR (up.win=0) OR (up.name=0) OR (up.deft=0) OR (up.ver=0) THEN
	Raise(cat.msg_MissedSomething.getstr())

	s:=String(500) ; ts:=String(256)

	set(up.win,MUIA_Window_Sleep,1)

	IF up.msg_chk		= 0 THEN up.msg_chk:=	(cat.msg_CheckOnLine.getstr() )							-> news('Update\en\e1\nCheck online for newer Version\n\n(Must have an active internet connection)')

	IF up.msg_yesno		= 0 THEN up.msg_yesno:=	(cat.msg_CheckYesOrNo.getstr() )						-> news('\eb_Yes\en|_No')

	IF up.msg_nopen		= 0 THEN up.msg_nopen:=	(cat.msg_NotOpen.getstr() )							-> news('Failed to Open')

	IF up.msg_unfound	= 0 THEN up.msg_unfound:= StringF(file_Log, cat.msg_TheUnFound.getstr(), FilePart(TCP_LOG_UPDATE))	-> 'was not found in "\eb\s\en" database ?\nPlease Email the author about this', FilePart(TCP_LOG_UPDATE))

	IF up.msg_latest	= 0 THEN up.msg_latest:= (cat.msg_TheLatest.getstr() )							-> news('You have the latest version :)')

	IF up.msg_better	= 0 THEN up.msg_better:= (cat.msg_TheBetter.getstr() )							-> news('Your Version seems to be better than mine ?')

	IF up.msg_found		= 0 THEN up.msg_found:= (cat.msg_TheFound.getstr() )							-> news('Upgrade Found :o)')

	IF up.msg_gads		= 0 THEN up.msg_gads:= (cat.msg_TheGads.getstr() )							-> news('\eb_Download|_Abort')

	IF up.msg_aborted	= 0 THEN up.msg_aborted:= (cat.msg_TheAborted.getstr() )						-> news('Process was aborted')

	IF up.msg_error		= 0 THEN up.msg_error:= (cat.msg_TheUpdateError.getstr() )						-> news('Update error!')

	IF up.msg_complete	= 0 THEN up.msg_complete:= StringF(file_name, cat.msg_TheCompleted.getstr(), FilePart(TCP_NEW_UPDATE)) 	-> 'Download Completed..\n\nNew archive "\eb\s\en" is in Ram:', FilePart(TCP_NEW_UPDATE))


	StrCopy(b_str,up.ver)
	StrCopy(b_str,b_str,InStr(b_str,' ',6))
	UpperStr(b_str)
	search:=news(b_str)

	-> WriteF('(1) search=[\s]\n',search)

	StringF(b_str,'\ec\eu\eb\s \s',up.name,up.msg_chk)

	IF muireq(up.deft,b_str,up.msg_yesno,up.app,up.win)=0
		Raise(up.msg_aborted)
		-> cat.close()
	ENDIF

	t_Log := String(300)	->StrLen(FilePart(TCP_LOG_UPDATE)) +2)

	StringF(t_Log,'T:\s', FilePart(TCP_LOG_UPDATE))

	-> WriteF('(2) t_Log=[\s]\n',t_Log)

	IF dbug THEN kPrintF('update()/t_Log=<\s>\n', [t_Log])

	IF FileLength(t_Log)<0 THEN
	IF (v:=httpGET(TCP_LOG_UPDATE , 'T:'))<>0 THEN
	Raise(v)

	IF (c:=Open(t_Log, OLDFILE))=0 THEN Raise(StringF(b_str,'\s \s',up.msg_nopen, t_Log))

	WHILE (ReadStr(c,s)<>-1) AND (look<>0)

		-> WriteF('(3) s=[\s]\n',s)

		StrCopy(ts,s,InStr(s,')')+1); UpperStr(ts)

		-> WriteF('(4) UpperStr(ts)=[\s]\n',ts)

		IF InStr(ts,search)=0
			MidStr(ts,s,InStr(s,')')+2,ALL); path:=news(ts)
			StrCopy(ts,s,InStr(s,')')+1); over:=news(ts)
			look:=0

			-> WriteF('(5) path=[\s] - over=[\s]\n',path,over)

		ENDIF
	ENDWHILE

	IF dbug THEN kPrintF('update()/path = <\s> - ts = <\s>\n', [path, ts])

	-> WriteF('(6) path=[\s] ts=[\s]\n',path,ts)

	IF look=1 THEN Raise(StringF(b_str,'\eb\s\en \s',up.name,up.msg_unfound))

	IF dbug THEN kPrintF('update()/OstCmp:\nover   = \s\nup.ver = \s\n', [over,up.ver])

	v:=OstrCmp(up.ver,over)

	SELECT v
		CASE 0
			IF dbug THEN kPrintF('update()/RESULT = \d\n', [v])
			muireq(up.deft,up.msg_latest, cat.msg_ReqOkay.getstr() ,up.app,up.win)
		CASE -1
			IF dbug THEN kPrintF('update()/RESULT = \d\n', [v])
			muireq(up.deft,up.msg_better, cat.msg_ReqOkay.getstr() ,up.app,up.win)
		CASE 1
			IF dbug THEN kPrintF('update()/RESULT = \d\n', [v])
			StringF(b_str, cat.msg_CompareVersions.getstr() ,up.name,up.msg_found,up.ver,over)
			IF muireq(up.deft,b_str,up.msg_gads,up.app,up.win)=0 THEN
			Raise(up.msg_aborted)

			IF dbug THEN kPrintF('update()/v:=httpGET(\s,"Ram:")\n', [TCP_NEW_UPDATE])

			IF (v:=httpGET(TCP_NEW_UPDATE,'Ram:'))<>0 THEN Raise(v)
			muireq(up.deft,up.msg_complete, cat.msg_ReqOkay.getstr() ,up.app,up.win)
			Raise(0)
	ENDSELECT

EXCEPT DO
	IF c<>0 THEN Close(c)
	-> DisposeLink(t_Log)
	-> DisposeLink(s)
	-> DisposeLink(ts)
	IF exception THEN muireq(up.deft,StringF(b_str, cat.msg_ReqReason.getstr() ,up.name,up.msg_error,exception), cat.msg_ReqOkay.getstr() ,up.app,up.win)

	set(up.win,MUIA_Window_Sleep,0)

	IF FileLength(t_Log)>0 THEN DeleteFile(t_Log)
ENDPROC

PROC news(os)
DEF ns
	ns:=String(StrLen(os))
	StrCopy(ns,os)
ENDPROC ns

PROC muireq(ttl=0,bod=0,gads=0,app=0,win=0,ag=0,openLibrary=0) HANDLE
DEF	title:PTR TO CHAR,body:PTR TO CHAR,gadgets:PTR TO CHAR,args:PTR TO CHAR,answer

	title	:= IF ttl=0  THEN (cat.msg_ReqTitle.getstr())   ELSE ttl
	body	:= IF bod=0  THEN (cat.msg_ReqBody.getstr())    ELSE bod
	args	:= IF ag=0   THEN 0 ELSE ag
	gadgets	:= IF gads=0 THEN (cat.msg_ReqGadgets.getstr()) ELSE gads

	IF openLibrary THEN
  	IF (muimasterbase:=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN))=NIL THEN
	Raise( WriteF(cat.msg_ErrorMuimaster.getstr() , MUIMASTER_NAME)  )

	answer:=Mui_RequestA(app,
        		win,
        		0,title,gadgets,body,args)

EXCEPT DO
	IF openLibrary THEN
	IF muimasterbase THEN
	CloseLibrary(muimasterbase)
ENDPROC answer
