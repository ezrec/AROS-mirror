/* mklib-emx.cmd */

version = "19980802"

TRUE=1
FALSE=0
quiet = TRUE
/* DEBUG = TRUE */
DEBUG = FALSE
 
Parse Arg param Major Minor Objects

say "mklib-emx version "version

Parse Var param base '.' extension
_extension=Strip(Translate(extension))

select
  when _extension = "" then
    do
    DynaLibrary = Base".dll"
    StatLibrary = Base".a"
    end
  when _extension = "A" then
    do
    DynaLibrary = Base".dll"
    StatLibrary = Base".a"
    end
  when _extension = "LIB" then
    do
    DynaLibrary = Base".dll"
    StatLibrary = Base".lib"
    end
  otherwise
    do
    say 'error. unknown extension'
    exit
    end
  end /* select */

if DEBUG then
  do
  say "DynaLibrary: "DynaLibrary
  say "StatLibrary: "StatLibrary
  Say "Major: "Major
  Say "Minor: "Minor
  Say "Objects: "Objects
  end

DEFFile = Base".def"
olddef = Stream(DEFFile, 'C', 'QUERY EXISTS')
if olddef <> "" then
  do
  tmpfile = SysTempFileName('.\mklib-emx.???')
  call ExecCmd("mv "olddef" "tmpfile)
  say "Renaming old "DEFFile" to "tmpfile
  end

call LineOut DEFFile ,,1
call LineOut DEFFile,  'LIBRARY "'Base'"'
call LineOut DEFFile,  'DESCRIPTION "X11R6 Xfree86 'DynaLibrary' for OS/2 EMX09c VERSION=1.2.7"'
call LineOut DEFFile,  'CODE'
call LineOut DEFFile,  '   PRELOAD'
call LineOut DEFFile,  'DATA'
call LineOut DEFFile,  '   MULTIPLE NONSHARED'
call LineOut DEFFile,  'STACKSIZE 65536'
call LineOut DEFFile,  'EXPORTS'

call LineOut DEFFile 

call CreateSymbolList

DLLFLAGS = "-Zdll -Zmt -Zcrtdll"
DLLLibs = "-lXi -lXmu -lXext -lXt -lX11 -lSM -lICE"

if Translate(Base) = "MESAGL" then
  DLLGLLIBS = ""
if Translate(Base) = "MESAGLU" then
  DLLGLLIBS = "-L..\lib -lMesaGL"
if Translate(Base) = "GLUT" then
  DLLGLLIBS = "-L..\lib -lMesaGL -lMesaGLU"

call ExecCmd("gcc -o "DynaLibrary" "DLLFLAGS" "Objects" "DEFFile" "DLLGLLIBS" "DLLLibs)

call ExecCmd("emximp -o "StatLibrary" "DEFFile)

call ExecCmd("mv "DynaLibrary" ..\lib")
call ExecCmd("cp "Base".def ..\lib")

exit

/* Small procedures */

CreateSymbolList:
Olist = Objects
call ExecCmd("emxexp "OList" >>"DEFFile)
return

ExecCmd: PROCEDURE EXPOSE quiet
/* Execute a command properly and return it's return value */
Parse Arg cmdstring
if quiet then
  Address CMD "@ "cmdstring
else
  Address CMD cmdstring
return rc
