echo off
rem EGCS-Mingw32 build of Mesa 3-D Graphics Library
rem Paul Garceau, August 26, 1998
rem
rem bat file uses Make 3.76.1
rem touch command requires "touch" to be somewhere on your system path
rem
touch src/depend
touch src-glu/depend
rem touch src-glut/depend

rem Create a lib directory/folder
rem
md lib

rem Generate wing32.dll for the sake of Mesa build
cd src\windows
rem
rem Create a .a lib file
rem
dlltool --def wing32.def --output-lib wing32.a
rem
rem Create a .dll file (wing32.dll); EGCS-Mingw32 compiler used
rem
gcc -mdll -o wing32.dll wing32.a -WI,temp.exp
rem
rem wing32.dll is now created -- move .a and .dll to
rem lib dir
mv wing32.dll wing32.a c:\mesa-3.0\lib
rem files moved

rem Return to mesa-3.0 'root' directory
cd c:\mesa-3.0

rem Now begins the build of mesa-2.6 libs for EGCS/Mingw32

rem  Build libMesaGL.a
rem
make -w --directory=c:\mesa-3.0\src -f makefile.nt4

rem  Build libMesaGLU.a
rem
make -w --directory=c:\mesa-3.0\src-glu -f makefile.nt4

rem  Optional libraries:
rem The following libraries, apparently, are optional
rem for Mesa and should only be uncommented if you have the
rem necessary support already available on your machine
rem
rem         Mesaaux.a lib for aux extension
rem make -w --directory=c:\mesa-3.0\src-aux -f makefile.nt4

rem         libglut.a for glut extension of Mesa:
rem make -w --directory=c:\mesa-3.0\src-glut -f makefile.nt4
rem
rem         libMesatk.a for tk extensions of Mesa
rem make -w --directory=c:\mesa-3.0\src-tk -f makefile.nt4
rem
rem Clean up the object files floating around out there...
rem comment the next lines depending on which collection of
rem Mesa libs you built
rem
del src\*.o
del src-glu\*.o
rem del src-aux\*.o
rem del src-glut\*.o
