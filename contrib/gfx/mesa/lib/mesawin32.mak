
# Be sure to modify the definitions in this file to agree with your
# systems installation.
#
#  NOTE: be sure that the install directories use '\' not '/' for paths.

# MSVC install directories
#
LIBINSTALL     = $(MSDEVDIR)\..\VC98\LIB
INCLUDEINSTALL = $(MSDEVDIR)\..\VC98\INCLUDE

# win32 dll directory
#
!IF "$(OS)" == "Windows_NT"
DLLINSTALL     = $(WINDIR)\SYSTEM32
!ELSE
DLLINSTALL     = $(WINDIR)\SYSTEM
!ENDIF

# Microsoft OpenGL libraries
#
GLU	= glu32.lib
OPENGL	= opengl32.lib
GLUT	= $(TOP)\lib\glut\glut32.lib
MESALIB = opengl32.lib
MESADLL = opengl32.dll

# common definitions used by all makefiles
CFLAGS	= $(cflags) $(cdebug) $(EXTRACFLAGS) -I$(TOP)\include
LIBS	= $(lflags) $(ldebug) $(EXTRALIBS) $(GLUT) $(GLU) $(OPENGL) $(guilibs)
EXES	= $(SRCS:.c=.exe) $(CPPSRCS:.cpp=.exe)

!IFNDEF NODEBUG
lcommon = /NODEFAULTLIB /INCREMENTAL:NO /DEBUG /NOLOGO
!ENDIF

# default rule
default	: $(EXES)

# cleanup rules
clean	::
	@del /f *.obj
	@del /f *.pdb
	@del /f *.ilk
	@del /f *.ncb
	@del /f *~
	@del /f *.exp

clobber	:: clean
	@del /f *.exe
	@del /f *.dll
	@del /f *.lib
	-@del /f $(LDIRT)

# inference rules
$(EXES)	: $*.obj $(DEPLIBS)
	echo $@
        $(link) -out:$@ $** $(LIBS)
.c.obj	:
	$(CC) $(CFLAGS) $<
.cpp.obj :
	$(CC) $(CFLAGS) $<
