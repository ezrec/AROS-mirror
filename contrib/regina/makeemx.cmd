/**/
Parse arg ver .
If ver = '' Then
 Do
    Say 'Must supply version: eg 20'
    Exit
 End
SRC = Translate(Value('REGINA_SRCDIR',,'OS2ENVIRONMENT'),'\','/')
'cd emx'
'mkdir dist'
'cd dist'
'copy ..\regina.exe'
'copy ..\rexx.exe'
'copy ..\rxstack.exe'
'copy ..\rxqueue.exe'
'copy ..\regina.dll'
'copy ..\rexx.a'
'copy ..\rexx.lib'
'copy ..\regina.a'
'copy ..\regina.lib'
'copy ..\test1.dll'
'copy ..\test2.dll'
'copy' SRC || '\COPYING-LIB'
'copy' SRC || '\rexxsaa.h'
'copy' SRC || '\README.2*'
'copy' SRC || '\BUGS'
'copy' SRC || '\README.EMX README'
'copy' SRC || '\file_id.diz.emx file_id.diz'
'mkdir demo'
'copy' SRC || '\demo\*.rexx demo'
'del rex??emx.zip'
'zip -r rex' || ver || 'emx *'
'cd ..\..'
