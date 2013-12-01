/* OpenDrawer.e E source by: Jean-Marie COAT <agalliance@free.fr>
 * This version need at least: Scalos.library 40.34
 * $VER: OpenDrawer 40.36 (03.07.06)
 * Original source (version: 0.1) by: Eric Sauvageau
*/

/*
 * Arguments:
 *
 * PATH		The path to open. 	NOTE: Since version 40.36, if a file is specified as path, its parent dir will be used.
 * VIEWMODE	The viewing mode: 	0 = Default, 1 = Icon, 2 = Name, 3 = Size, 4 = Date, 5 = Time, 6 = Comment, 7 = Access.
 * SALL		Use if you want to display all files or as default or only icons.
 * ICONIFY	Switch, the window(argument PATH) will be iconified.
 *
*/

OPT PREPROCESS

MODULE 'scalos/scalos41.6','scalos', 'utility/tagitem'
MODULE 'dos','dos/dos'
MODULE 'exec/execbase','exec/types','exec/libraries','exec/lists','exec/nodes'
MODULE '*VERSTAG','debug'

-> Switch: Debug output line.
#define d1(x)
#define d2(x)	x

-> Globals definitipons.
DEF ver, rev, badversion=0, badrevision=0

PROC main() HANDLE
-> main(): Locals definitions.
DEF	rdargs=NIL
DEF	argarray:PTR TO LONG
DEF	strversion[6]:STRING
DEF	path[512]:STRING
DEF	lock_Path=NIL
DEF	lock_Parent=NIL
DEF	info:fileinfoblock
DEF	buf_Args=NIL

buf_Args:=String(StrLen(VERSTRING) + StrLen(COPYRIGHT) + 232)

-> Write into a buffer internals Scalos version and revision
-> to inform user if a error is occurred. Execptions "LIB" and "BREV".
StringF(strversion,'\d.\d', SCALOSVERSION, SCALOSREVISION)

StringF(buf_Args,'\n\e[1m \s \e[1m \e[31m \s \e[0m\n\nViewModes: VIEWBYDEFAULT=0 / ICON=1 / NAME=2 / SIZE=3 / DATE=4 / TIME=5 / COMMENT=6 / ACCESS=7 \nSALL=ShowAllMode: SHOWDEFAULT=0 / SHOWONLYICONS=1 / SHOWALL=2\nICONIFY=I\n\nPATH/A,VM=VIEWMODE/K/N,SALL/K/N,I/S',VERSTRING, COPYRIGHT)

argarray := [0,0,0,0]

-> Read arguments
-> IF (rdargs := ReadArgs('\n\e[1m OpenDrawer 41.6 \e[31m(03.07.06)\e[0m © 2002-2006 \e[3m Jean-marie COAT <agalliance@wanadoo.fr>\e[0m\n\nViewModes: VIEWBYDEFAULT=0 / ICON=1 / NAME=2 / SIZE=3 / DATE=4 / TIME=5 / COMMENT=6 / ACCESS=7 \nSALL=ShowAllMode: SHOWDEFAULT=0 / SHOWONLYICONS=1 / SHOWALL=2\nICONIFY=I\n\nPATH/A,VM=VIEWMODE/K/N,SALL/K/N,I/S', argarray, NIL)) = NIL THEN Raise("ARG")

IF (rdargs := ReadArgs(buf_Args, argarray, NIL)) = NIL THEN Raise("ARG")



-> Open Scalos.library
IF (scalosbase := OpenLibrary(SCALOSNAME,SCALOSREVISION)) <> NIL
	checklib('scalos.library') -> check scalos.libary version and revision.

	d1(kPrintF('scalos.libary Ver/Rev = \d.\d\nMin Scalos Version/Revision requiered = \d.\d\n\n', [ver,rev,SCALOSVERSION,SCALOSREVISION]))

	IF badrevision THEN Raise("BREV")
ELSE
	Raise("LIB")
ENDIF

d1(kPrintF(IF scalosbase THEN 'scalosbase opened.\n' ELSE 'scalosbase not opened!\n', [NIL]))

IF argarray[1]
   IF (Long(argarray[1]) > 7) OR (Long(argarray[1]) < 0) THEN Raise("ARG1")
   IF (Long(argarray[2]) > 2) OR (Long(argarray[2]) < 0) THEN Raise("ARG2")
ENDIF

-> If first argument isn't a drawer or a volume, opendrawer will use its parent path. 
IF lock_Path:=Lock(argarray[0], -2)
   	Examine(lock_Path, info)
	IF info.direntrytype<0

		d1(kPrintF('Path is not drawer or volume:\n<\s>\nType = \d\n', [argarray[0], info.direntrytype]))

		lock_Parent:=ParentDir(lock_Path)
		NameFromLock(lock_Parent, path, 512)
		argarray[0]:=path

		d1(kPrintF('Lock to its parent:\n<\s>\n', [path]))

		UnLock(lock_Parent)
	ENDIF
	UnLock(lock_Path)
ENDIF


IF ScA_OpenIconWindow([SCA_Path, argarray[0],
                       SCA_ViewModes, (IF argarray[1] THEN Long(argarray[1]) ELSE 0),
                       SCA_ShowAllMode, (IF argarray[2] THEN Long(argarray[2]) ELSE 0),
                       SCA_Iconify, (IF argarray[3] THEN TRUE ELSE FALSE),
                       TAG_END]) = FALSE THEN Raise("OIW")

EXCEPT DO
   SELECT exception
      CASE "LIB" ;	WriteF('Can\at open "\s" version \s! Scalos must be run.\n\n',SCALOSNAME,strversion)
      CASE "BREV" ;	WriteF('Sorry, can\at open "\s" version "\s"!\nFound "\s" \d.\d\n\n',SCALOSNAME,strversion,SCALOSNAME,ver,rev)
      CASE "OIW" ;	WriteF('\nCan\at open:\n "\s" or its parent!\n\n',argarray[0])
      CASE "ARG" ;	WriteF('\nInvalid arguments.\nType: Opendrawer \e[1m\e[32m? \e[0mfor \e[1mhelp \e[0m!\n\n')
      CASE "ARG1" ;	WriteF('\n\e[1m\e[32m\d \e[0m: Bad number of \e[1mViewMode \e[0m!\n\e[1mViewMode \e[0mmust be between \e[32m\e[1m0 \e[0mand \e[1m\e[32m6 \e[0m.\n\n',Long(argarray[1]))
      CASE "ARG2" ;	WriteF('\n\e[1m\e[32m\d \e[0m: Bad number of \e[1mShowAllMode\e[0m!\n\e[1mShowAllMode \e[0mmust be between \e[32m\e[1m0 \e[0mand \e[1m\e[32m2 \e[0m.\n\n',Long(argarray[2]))
      CASE "ARGN" ;	WriteF('\n\e[1mViewMode \e[0mmust be between \e[32m\e[1m0 \e[0mand \e[1m\e[32m7 \e[0m except number \e[1m\e[32m1 \e[0m!\n\n')
   ENDSELECT

   CloseLibrary(scalosbase)  -> NIL is a valid input for CloseLibrary()

   IF rdargs THEN FreeArgs(rdargs)
   IF buf_Args THEN DisposeLink(buf_Args)
   IF exception THEN CleanUp(20)
ENDPROC

PROC checklib(scabase)
  DEF exec:PTR TO execbase, base:lib
    exec := execbase

    IF base := FindName(exec.liblist, scabase)
	ver:=base.version
	rev:=base.revision
	IF ver < 40 THEN badversion:=1
	IF ver = 40 AND rev < 34 THEN badrevision:=1
	IF ver > 40 THEN badrevision:=0

    ENDIF

ENDPROC

CHAR 0, VERSTAG, ' Compiled with : ', COMPILER_STRING, 0

