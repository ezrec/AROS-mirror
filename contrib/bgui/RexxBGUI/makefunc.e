
MODULE 'dos/dostags'

CONST ZERO=0

ENUM ERR_NONE,ERR_ARGS,ERR_OPEN

RAISE ERR_ARGS IF ReadArgs()=NIL,
			ERR_OPEN IF Open()=ZERO

PROC main() HANDLE
	DEF rda=NIL,in=ZERO,out=ZERO,str[257]:STRING,q
	DEF a[2]:ARRAY OF LONG
	rda:=ReadArgs('FROM/A,TO/A',a,NIL)
	in:=Open(a[0],OLDFILE)
	out:=Open('t:makefunc.temp',NEWFILE)
	WHILE Fgets(in,str,256)
		IF StrCmp(str,'\t\t/* ',STRLEN)
			IF q:=InStr(str,'=')+1 THEN Fputs(out,str+q)
		ENDIF
	ENDWHILE
	Close(out); out:=ZERO
	out:=Open('t:makefunc.header',NEWFILE)
	Fputs(out,'\e[1mFunctions in alphabetical order\e[22m\n\n')
	Close(out); out:=ZERO
	StringF(str,'Sort t:makefunc.temp t:makefunc.sorted\n'+
		'Join t:makefunc.header t:makefunc.sorted AS \s',a[1])
	SystemTagList(str,NIL)
	DeleteFile('t:makefunc.temp')
	DeleteFile('t:makefunc.header')
	DeleteFile('t:makefunc.sorted')
EXCEPT DO
	Close(out)
	Close(in)
	IF rda THEN FreeArgs(rda)
	SELECT exception
		CASE ERR_ARGS; PrintFault(IoErr(),'MakeFunc cannot read args')
		CASE ERR_OPEN; PrintFault(IoErr(),'MakeFunc cannot open file')
	ENDSELECT
ENDPROC
