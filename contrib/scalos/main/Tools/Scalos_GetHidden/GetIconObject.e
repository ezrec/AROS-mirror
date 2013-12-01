OPT MODULE, PREPROCESS

MODULE 'workbench/startup','workbench/workbench','icon','debug','jmc/MyDebugOnOff'

-> Switch: Debug output line: module JMC/MyDebugOnOff.m.
-> #define d1(x)
-> #define d2(x)	x

#define THE_FILE 'GetIconObject.e'

EXPORT PROC get_MyDiskObject(dobj=NIL:PTR TO diskobject,dbg=NIL:PTR TO LONG)
DEF wb:PTR TO wbstartup,wba:PTR TO wbarg, debug_lock

	IF (iconbase:=OpenLibrary('icon.library',37))<>0
		IF wbmessage
			wb:=wbmessage
			wba:=wb.arglist
			IF (wba.lock>0) AND (wba.name>0)
				debug_lock:=String(512)
				NameFromLock(wba.lock, debug_lock, StrMax(debug_lock))
            			AddPart(debug_lock,wba.name, StrMax(debug_lock))

				IF (dbg=TRUE)
					d2(kPrintF('\s/get_MyDiskObject()/ICON NAME FROM WB: \s\n', [THE_FILE, debug_lock]))
				ELSE
					d1(kPrintF('\s/get_MyDiskObject()/ICON NAME FROM WB: \s\n', [THE_FILE, debug_lock]))
				ENDIF
			ENDIF
			IF (dobj:=GetDiskObject(debug_lock))=0
				dobj:=GetDefDiskObject(WBTOOL)
			ENDIF
		ELSE
			debug_lock:=getprogname()

			IF (dbg=TRUE)
				d2(kPrintF('\s/get_MyDiskObject()/ICON NAME FROM CLI: \s\n', [THE_FILE, debug_lock]))
			ELSE
				d1(kPrintF('\s/get_MyDiskObject()/ICON NAME FROM CLI: \s\n', [THE_FILE, debug_lock]))
			ENDIF

			IF (dobj:=GetDiskObject(debug_lock))=0
				dobj:=GetDefDiskObject(WBTOOL)
			ENDIF
		ENDIF
		CloseLibrary(iconbase)
	ENDIF
ENDPROC dobj

EXPORT PROC getprogname()
DEF l,d,f[128]:STRING
	IF l:=GetProgramDir()
		d:=String(500)
		NameFromLock(l,d,500)
		GetProgramName(f,128)
		AddPart(d,f,500)
		SetStr(d,StrLen(d))
	ENDIF
ENDPROC d

