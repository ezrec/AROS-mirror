# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# Contributed by beverly@datacube.com (Beverly Brown)

# ** DO NOT EDIT **



# TARGTYPE "Win32 (x86) Console Application" 0x0103



!IF "$(CFG)" == ""

CFG=Win32 Debug

!MESSAGE No configuration specified.  Defaulting to Win32 Debug.

!ENDIF 



!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"

!MESSAGE Invalid configuration "$(CFG)" specified.

!MESSAGE You can specify a configuration when running NMAKE on this makefile

!MESSAGE by defining the macro CFG on the command line.  For example:

!MESSAGE 

!MESSAGE NMAKE /f "indent.mak" CFG="Win32 Debug"

!MESSAGE 

!MESSAGE Possible choices for configuration are:

!MESSAGE 

!MESSAGE "Win32 Release" (based on "Win32 (x86) Console Application")

!MESSAGE "Win32 Debug" (based on "Win32 (x86) Console Application")

!MESSAGE 

!ERROR An invalid configuration is specified.

!ENDIF 



################################################################################

# Begin Project

# PROP Target_Last_Scanned "Win32 Debug"

CPP=cl.exe

RSC=rc.exe



!IF  "$(CFG)" == "Win32 Release"



# PROP BASE Use_MFC 0

# PROP BASE Use_Debug_Libraries 0

# PROP BASE Output_Dir "WinRel"

# PROP BASE Intermediate_Dir "WinRel"

# PROP Use_MFC 0

# PROP Use_Debug_Libraries 0

# PROP Output_Dir "WinRel"

# PROP Intermediate_Dir "WinRel"

OUTDIR=.\WinRel

INTDIR=.\WinRel



ALL : $(OUTDIR)/"indent.exe" $(OUTDIR)/"indent.bsc"



$(OUTDIR) : 

    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)



# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"
/FR /c

# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /FR /c

CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\

 /FR$(INTDIR)/ /Fp$(OUTDIR)/"indent.pch" /Fo$(INTDIR)/ /c 

CPP_OBJS=.\WinRel/

# ADD BASE RSC /l 0x409 /d "NDEBUG"

# ADD RSC /l 0x409 /d "NDEBUG"

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo

BSC32_FLAGS=/nologo /o$(OUTDIR)/"indent.bsc" 

BSC32_SBRS= \

	$(INTDIR)/"indent.sbr" \

	$(INTDIR)/"lexi.sbr" \

	$(INTDIR)/"args.sbr" \

	$(INTDIR)/"comments.sbr" \

	$(INTDIR)/"io.sbr" \

	$(INTDIR)/"parse.sbr" \

	$(INTDIR)/"globs.sbr" \

	$(INTDIR)/"backup.sbr" \

	$(INTDIR)/"memcpy.sbr"



$(OUTDIR)/"indent.bsc" : $(OUTDIR)  $(BSC32_SBRS)

    $(BSC32) @<<

  $(BSC32_FLAGS) $(BSC32_SBRS)

<<



LINK32=link.exe

# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib
comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib
odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386

# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib
advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib
odbccp32.lib /NOLOGO /SUBSYSTEM:console /MACHINE:I386

LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\

 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\

 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:no\

 /PDB:$(OUTDIR)/"indent.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"indent.exe" 

DEF_FILE=

LINK32_OBJS= \

	$(INTDIR)/"indent.obj" \

	$(INTDIR)/"lexi.obj" \

	$(INTDIR)/"args.obj" \

	$(INTDIR)/"comments.obj" \

	$(INTDIR)/"io.obj" \

	$(INTDIR)/"parse.obj" \

	$(INTDIR)/"globs.obj" \

	$(INTDIR)/"backup.obj" \

	$(INTDIR)/"memcpy.obj"



$(OUTDIR)/"indent.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)

    $(LINK32) @<<

  $(LINK32_FLAGS) $(LINK32_OBJS)

<<



!ELSEIF  "$(CFG)" == "Win32 Debug"



# PROP BASE Use_MFC 0

# PROP BASE Use_Debug_Libraries 1

# PROP BASE Output_Dir "WinDebug"

# PROP BASE Intermediate_Dir "WinDebug"

# PROP Use_MFC 0

# PROP Use_Debug_Libraries 1

# PROP Output_Dir "WinDebug"

# PROP Intermediate_Dir "WinDebug"

OUTDIR=.\WinDebug

INTDIR=.\WinDebug



ALL : $(OUTDIR)/"indent.exe" $(OUTDIR)/"indent.bsc"



$(OUTDIR) : 

    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)



# ADD BASE CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D
"_CONSOLE" /FR /c

# ADD CPP /nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"
/FR /c

CPP_PROJ=/nologo /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\

 /FR$(INTDIR)/ /Fp$(OUTDIR)/"indent.pch" /Fo$(INTDIR)/
/Fd$(OUTDIR)/"indent.pdb"\

 /c 

CPP_OBJS=.\WinDebug/

# ADD BASE RSC /l 0x409 /d "_DEBUG"

# ADD RSC /l 0x409 /d "_DEBUG"

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo

BSC32_FLAGS=/nologo /o$(OUTDIR)/"indent.bsc" 

BSC32_SBRS= \

	$(INTDIR)/"indent.sbr" \

	$(INTDIR)/"lexi.sbr" \

	$(INTDIR)/"args.sbr" \

	$(INTDIR)/"comments.sbr" \

	$(INTDIR)/"io.sbr" \

	$(INTDIR)/"parse.sbr" \

	$(INTDIR)/"globs.sbr" \

	$(INTDIR)/"backup.sbr" \

	$(INTDIR)/"memcpy.sbr"



$(OUTDIR)/"indent.bsc" : $(OUTDIR)  $(BSC32_SBRS)

    $(BSC32) @<<

  $(BSC32_FLAGS) $(BSC32_SBRS)

<<



LINK32=link.exe

# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib
comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib
odbc32.lib odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386

# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib
advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib
odbccp32.lib /NOLOGO /SUBSYSTEM:console /DEBUG /MACHINE:I386

LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\

 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\

 odbccp32.lib /NOLOGO /SUBSYSTEM:console /INCREMENTAL:yes\

 /PDB:$(OUTDIR)/"indent.pdb" /DEBUG /MACHINE:I386 /OUT:$(OUTDIR)/"indent.exe" 

DEF_FILE=

LINK32_OBJS= \

	$(INTDIR)/"indent.obj" \

	$(INTDIR)/"lexi.obj" \

	$(INTDIR)/"args.obj" \

	$(INTDIR)/"comments.obj" \

	$(INTDIR)/"io.obj" \

	$(INTDIR)/"parse.obj" \

	$(INTDIR)/"globs.obj" \

	$(INTDIR)/"backup.obj" \

	$(INTDIR)/"memcpy.obj"



$(OUTDIR)/"indent.exe" : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)

    $(LINK32) @<<

  $(LINK32_FLAGS) $(LINK32_OBJS)

<<



!ENDIF 



.c{$(CPP_OBJS)}.obj:

   $(CPP) $(CPP_PROJ) $<  



.cpp{$(CPP_OBJS)}.obj:

   $(CPP) $(CPP_PROJ) $<  



.cxx{$(CPP_OBJS)}.obj:

   $(CPP) $(CPP_PROJ) $<  



################################################################################

# Begin Group "Source Files"



################################################################################

# Begin Source File



SOURCE=.\indent.c

DEP_INDEN=\

	".\sys.h"\

	".\indent.h"



$(INTDIR)/"indent.obj" :  $(SOURCE)  $(DEP_INDEN) $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\lexi.c



$(INTDIR)/"lexi.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\args.c



$(INTDIR)/"args.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\comments.c



$(INTDIR)/"comments.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\io.c



$(INTDIR)/"io.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\parse.c



$(INTDIR)/"parse.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\globs.c



$(INTDIR)/"globs.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\backup.c



$(INTDIR)/"backup.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\memcpy.c



$(INTDIR)/"memcpy.obj" :  $(SOURCE)  $(INTDIR)



# End Source File

################################################################################

# Begin Source File



SOURCE=.\backup.h

# End Source File

################################################################################

# Begin Source File



SOURCE=.\version.h

# End Source File

# End Group

# End Project

################################################################################
