/**/
Parse arg ver .
If ver = '' Then
 Do
    Say 'Must supply version: eg 20'
    Exit
 End
SRC = 'c:\regina'
'cd wcc'
'mkdir dist'
'cd dist'
'copy ..\regina.exe'
'copy ..\rexx.exe'
'copy ..\regina.dll'
'copy ..\regina.lib'
'copy ..\test1.dll'
'copy ..\test2.dll'
'copy' SRC || '\COPYING-LIB'
'copy' SRC || '\rexxsaa.h'
'copy' SRC || '\README.08?'
'copy' SRC || '\README.2*'
'copy' SRC || '\BUGS'
'copy' SRC || '\README.WCC README'
'copy' SRC || '\file_id.diz.os2 file_id.diz'
'mkdir demo'
'copy' SRC || '\demo\*.rexx demo'
'del rex??os2.zip'
'zip -r rx' || ver || 'os2 *'
'cd ..\..'
