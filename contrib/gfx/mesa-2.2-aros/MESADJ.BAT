rem build a demo file with djgpp
gcc %1.c -o%1 -DDOSVGA -I..\include ..\src-glu\dosglub.a ..\src-aux\dosaux.a ..\src-tk\tkdos.a ..\src\dosmesa.a
