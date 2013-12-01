/* OpenDrawer for Scalos by: Jean-Marie COAT
 * Need: Scalos.library 40.34
 * Original source by: Eric Sauvageau
*/

/*
 * Arguments:
 *
 * PATH - The path to open
 * VIEWMODE -  The viewing mode: 0 = Default, 1 = Icon, 2 = Name, 3 = Size, 4 = Date, 5 = Time, 6 = Comment, 7 = Access
 * SALL- Switch, use if you want to display all files or as default or only icons.
 *
*/
OPT PREPROCESS, POWERPC, MORPHOS

MODULE 'scalos/scalos40.34','scalos', 'utility/tagitem'
MODULE 'dos','dos/dos'
MODULE 'exec/execbase','exec/types','exec/libraries','exec/lists','exec/nodes','*VERSTAG'

DEF ver,rev,badversion=0,badrevision=0

PROC main() HANDLE
DEF rdargs=NIL
DEF argarray:PTR TO LONG
DEF strversion[6]:STRING
DEF buf_args[264]:STRING

StringF(buf_args,'\n\e[1m \s \e[0m\n\nViewModes: VIEWBYDEFAULT=0 / ICON=1 / NAME=2 / SIZE=3 / DATE=4 / TIME=5 / COMMENT=6 / ACCESS=7 \nSALL=ShowAllMode: SHOWDEFAULT=0 / SHOWONLYICONS=1 / SHOWALL=2\nICONIFY=I\n\nPATH/A,VM=VIEWMODE/K/N,SALL/K/N,I/S',VERSTRING)

StringF(strversion,'\d.\d', SCALOSVERSION, SCALOSREVISION)

argarray := [0,0,0,0]

-> Read arguments
-> IF (rdargs := ReadArgs('\n\e[1m OpenDrawer \e[2m40.34 \e[31m(05.06.06)\e[0m © COPYRIGHT 2002-2006 \e[3m Jean-marie COAT \e[0m\n\nViewModes: VIEWBYDEFAULT=0 / ICON=1 / NAME=2 / SIZE=3 / DATE=4 / TIME=5 / COMMENT=6 / ACCESS=7 \nSALL=ShowAllMode: SHOWDEFAULT=0 / SHOWONLYICONS=1 / SHOWALL=2\nICONIFY=I\n\nPATH/A,VM=VIEWMODE/K/N,SALL/K/N,I/S',argarray,VERSTRING)) = NIL THEN Raise("ARG")

IF (rdargs := ReadArgs(buf_args ,argarray,NIL)) = NIL THEN Raise("ARG")

-> Open Scalos.library

checklib('scalos.library')

-> WriteF('Ver/Rev = \d.\d\nScalosVersion/Revision = \d.\d\n\n',ver,rev,SCALOSVERSION,SCALOSREVISION)

IF (scalosbase := OpenLibrary(SCALOSNAME,SCALOSREVISION)) = NIL THEN Raise("LIB")

IF (scalosbase := OpenLibrary(SCALOSNAME,SCALOSREVISION)) <> NIL AND badrevision THEN Raise("BREV")

IF argarray[1]
   IF (Long(argarray[1]) > 6) OR (Long(argarray[1]) < 0) THEN Raise("ARG1")
   -> IF Long(argarray[1]) = 1 THEN Raise("ARGN")
   IF (Long(argarray[2]) > 2) OR (Long(argarray[2]) < 0) THEN Raise("ARG2")
ENDIF

IF ScA_OpenIconWindow([SCA_Path, argarray[0],
                       SCA_ViewModes, (IF argarray[1] THEN Long(argarray[1]) ELSE 0),
                       SCA_ShowAllMode, (IF argarray[2] THEN Long(argarray[2]) ELSE 0),
                       SCA_Iconify, (IF argarray[3] THEN TRUE ELSE FALSE),
                       TAG_END]) = FALSE THEN Raise("OIW")
                   

EXCEPT DO
   SELECT exception
      CASE "LIB" ; WriteF('Can\at open "\s" version \s! Scalos must be run.\n\n',SCALOSNAME,strversion)
      CASE "BREV" ; WriteF('Sorry, can\at open "\s" version "\s"!\nFound "\s" \d.\d\n\n',SCALOSNAME,strversion,SCALOSNAME,ver,rev)
      CASE "OIW" ; WriteF('\nCan\at open:\n "\s".\n\n',argarray[0])
      CASE "ARG" ; WriteF('\nInvalid arguments.\nType: Opendrawer \e[1m\e[32m? \e[0mfor \e[1mhelp \e[0m!\n\n')
      CASE "ARG1" ; WriteF('\n\e[1m\e[32m\d \e[0m: Bad number of \e[1mViewMode \e[0m!\n\e[1mViewMode \e[0mmust be between \e[32m\e[1m0 \e[0mand \e[1m\e[32m6 \e[0m.\n\n',Long(argarray[1]))
      CASE "ARG2" ; WriteF('\n\e[1m\e[32m\d \e[0m: Bad number of \e[1mShowAllMode\e[0m!\n\e[1mShowAllMode \e[0mmust be between \e[32m\e[1m0 \e[0mand \e[1m\e[32m2 \e[0m.\n\n',Long(argarray[2]))
      CASE "ARGN" ; WriteF('\n\e[1mViewMode \e[0mmust be between \e[32m\e[1m0 \e[0mand \e[1m\e[32m7 \e[0m except number \e[1m\e[32m1 \e[0m!\n\n')
   ENDSELECT

   CloseLibrary(scalosbase)  -> NIL is a valid input for CloseLibrary()

   IF rdargs THEN FreeArgs(rdargs)

   IF exception THEN CleanUp(20)
ENDPROC 0

PROC checklib(scabase)
  DEF exec:PTR TO execbase,base:PTR TO lib
    exec := execbase

    IF base := FindName(exec.liblist, scabase)
	ver:=base.version
	rev:=base.revision
	IF ver < 40 THEN badversion:=1
	IF ver = 40 AND rev < 34 THEN badrevision:=1
	IF ver > 40 THEN badrevision:=0

    ENDIF

ENDPROC

-> CHAR '$VER:OpenDrawer 40.34 (05.06.06) © COPYRIGHT 2002-2006 Jean-marie COAT - PPC version - Compiled with ECX 50 release 1.5',0

CHAR VERSTAG
