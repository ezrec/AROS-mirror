#
# @(#) $Header$
#
# BGUI library
# makefile
# SAS C MakeFile for the bgui.library
#
# (C) Copyright 1998 Manuel Lemos.
# (C) Copyright 1996-1997 Ian J. Einman.
# (C) Copyright 1993-1996 Jaba Development.
# (C) Copyright 1993-1996 Jan van den Baard.
# All Rights Reserved.
#
# $Log$
# Revision 42.0  2000/05/09 22:09:38  mlemos
# Bumped to revision 42.0 before handing BGUI to AROS team
#
# Revision 41.11  2000/05/09 19:54:43  mlemos
# Merged with the branch Manuel_Lemos_fixes.
#
# Revision 41.10.2.49  2000/05/09 04:59:18  mlemos
# Made the LICENSE text be included in the distribution archives.
#
# Revision 41.10.2.48  2000/05/04 04:40:45  mlemos
# Removed the outdate time limit for the debug library version.
# Added the Bar and Layout group gadgets classes to the build.
#
# Revision 41.10.2.47  2000/04/21 05:34:33  mlemos
# Postponed the outdating date for the debugging version.
# Changed the name of the branch to export to generate the source archive.
# Fixed the rules for generating library class compiled object from source.
#
# Revision 41.10.2.46  2000/02/27 19:36:04  mlemos
# Made the library revision file be copied from bgui.library_rev.h to
# work around a Y2K bug in bumprev.
#
# Revision 41.10.2.45  2000/02/27 17:57:31  mlemos
# Added the test11 to the build.
#
# Revision 41.10.2.44  2000/01/30 20:42:04  mlemos
# Postponed the debug version expiry date.
# Added the test13 program to the build.
#
# Revision 41.10.2.43  1999/08/17 19:08:05  mlemos
# Changed the name of the script that adds an assigned directory to LIBS:  to
# find BGUI libraries to MakeAssign.
#
# Revision 41.10.2.42  1999/08/13 17:24:00  mlemos
# Made the bgui_treeview.h and the respective stub include be installed in
# include:bgui and include:libraries respectively.
#
# Revision 41.10.2.41  1999/08/13 04:35:00  mlemos
# Made the bgui.h and bgui_macros.h be installed in include:bgui and the
# respective stubs be installed in include:libraries.
# Fixed the install location of the bgui_treeview.h include.
#
# Revision 41.10.2.40  1999/08/08 23:08:30  mlemos
# Added the target for test10 program.
#
# Revision 41.10.2.39  1999/07/30 22:12:26  mlemos
# Added support to build the TextField example.
#
# Revision 41.10.2.38  1999/07/24 17:21:53  mlemos
# Removed the activitydata.rexx script from the RexxBGUI library distribution
# archive.
#
# Revision 41.10.2.37  1999/07/23 06:08:54  mlemos
# Added the Script protection bit to the Assign scripts that are generated
# for the distribution archives.
#
# Revision 41.10.2.36  1999/07/04 05:25:21  mlemos
# Restored the line that made the library be installed in the default target.
#
# Revision 41.10.2.35  1999/07/03 20:03:00  mlemos
# Ensured that the revision information is up-to-date when the distribution
# archives are built.
# Replaced and tab characters in the makefile by spaces.
#
# Revision 41.10.2.34  1999/07/03 19:36:46  mlemos
# Added the rexxbgui.library to the build, distribution and clean targets.
# Removed needless QUIET options to some delete statements.
#
# Revision 41.10.2.33  1999/06/01 03:55:07  mlemos
# Ensured that the temporary directory with the source code extracted from
# the repository is removed after creating the BGUI source archive.
# Made the cleaning of the BGUI documentation directory only cleans the
# generated HTML files.
#
# Revision 41.10.2.32  1999/05/31 03:32:10  mlemos
# Fixed the location of the icons directory relative to the Autodocs
# directory.
# Fixed the file name of the archive of the AmigaGuide documentation to be
# deleted before being generated.
#
# Revision 41.10.2.31  1999/05/31 03:08:49  mlemos
# Added the extra gadgets directories to the build.
# Added the generation Autodocs for the the PopButton and Palette class to
# the build.
# Added the generation of documentation in HTML and AmigaGuide formats to the
# build.
# Added an Assign script to the users distribution archive to make sure LIBS:
# finds the libraries of BGUI and extra gadgets.
# Ensured that the generated HTML files are removed by the clean target.
#
# Revision 41.10.2.30  1999/02/19 04:51:01  mlemos
# Added the StormC BGUI pragma file to the distribution.
# Ensured that the fd, the inline and the pragma directories existe before
# any files are installed there.
#
# Revision 41.10.2.29  1999/02/18 23:14:50  mlemos
# Postponed the debug build outdate time.
# Added inline/bgui.h file to the distribution.
#
# Revision 41.10.2.28  1998/12/26 21:47:26  nobody
# Corrected the dependencies to build the CAD program.
#
# Revision 41.10.2.27  1998/12/20 01:39:52  mlemos
# Added a target to build test program 9.
#
# Revision 41.10.2.26  1998/12/08 03:56:27  mlemos
# Added a target to build test program 8.
# Made the BGUIPrefs program be renamed to BGUI.
#
# Revision 41.10.2.25  1998/12/06 21:34:25  mlemos
# Added a target to build test 7 program.
#
# Revision 41.10.2.24  1998/11/30 00:19:32  mlemos
# Added support to produce three kinds of build processes for the
# bgui.library:  standard, enhanced and debug.
# Made the distribution process split developer material into separate
# archives.
# Added icons to the relevant directories and programs shipped in the
# distribution archives.
#
# Revision 41.10.2.23  1998/11/23 15:05:08  mlemos
# Added needed dependencies of some targets on installed include files.
# Made the default build be compatible with OS 2 under MC68000.
#
# Revision 41.10.2.22  1998/11/16 19:58:24  mlemos
# Added a target to build test 6 program.
#
# Revision 41.10.2.21  1998/10/18 18:23:44  mlemos
# Added a target to build the ViewGroup example program.
#
# Revision 41.10.2.20  1998/10/12 01:28:04  mlemos
# Added a target to build the ARexx Demo program.
# Added the bgui_arexx.h include file to the distribution.
#
# Revision 41.10.2.19  1998/10/01 04:59:09  mlemos
# Added stkext.asm to the list of files that define automatic function
# prototypes.
#
# Revision 41.10.2.18  1998/10/01 04:52:32  mlemos
# Made the CAD program linking command take all objects explicitly.
# Added a target to generate the AutoDoc files.
# Added the Autodoc files to the distribution.
# Uncommented commands to clean up that were inadvertdly left commented.
#
# Revision 41.10.2.17  1998/09/20 23:57:36  mlemos
# Added targets for BGUIPrefs and Tap programs.
# Added BGUIPrefs and Tap programs to the distribution.
# Supressed most of the irrelevant output of the programs building the
# different targets.
#
# Revision 41.10.2.16  1998/09/19 04:01:08  mlemos
# Added targets to build the CAD program into the distribution.
#
# Revision 41.10.2.15  1998/09/13 04:32:49  mlemos
# Removed space characters in excess at the end of the file lines.
# Make the destribuition target create the source code archive.
# Added intermediate targets to the distribution target, so that if it is
# interrupted in the middle it will not rebuild the lha archives again.
#
# Revision 41.10.2.14  1998/09/12 02:20:09  mlemos
# Changed the outdate build date.
# Added a target to build the stack extension code.
#
# Revision 41.10.2.13  1998/08/27 04:05:16  mlemos
# Added a target to build the distribution files.
#
# Revision 41.10.2.12  1998/07/05 19:41:47  mlemos
# Added targets to build test4 and test5 programs.
#
# Revision 41.10.2.11  1998/06/21 19:28:24  mlemos
# Added the target for test3 program.
# Added an option to debugging build to outdate it after a given date.
#
# Revision 41.10.2.10  1998/04/27 00:14:44  mlemos
# Added Test2 target.
#
# Revision 41.10.2.9  1998/03/02 03:21:32  mlemos
# Added option to not perform stack checking in the example programs.
# Added dependency to FieldList example from the FieldList.h header.
#
# Revision 41.10.2.8  1998/03/01 15:47:44  mlemos
# Fixed example targets build statements.
#
# Revision 41.10.2.7  1998/02/28 17:54:17  mlemos
# Split clean target in several delete statment lines.
#
# Revision 41.10.2.6  1998/02/28 17:50:56  mlemos
# Added support to build examples.
#
# Revision 41.10.2.5  1998/02/28 02:47:24  mlemos
# Added DEFINE to enable debugging support.
#
# Revision 41.10.2.4  1998/02/26 22:41:59  mlemos
# Added message to bgui.library target action.
# Deleted test1 linker temporary file.
# Re-added strip debug linker options to pre linked modules.
# Made BOOPSI modules be linked with debug information.
#
# Revision 41.10.2.3  1998/02/26 19:09:57  mlemos
# Corrected libs:bgui.library dependencies
#
# Revision 41.10.2.2  1998/02/26 18:42:48  mlemos
# Fixed fd file target directory
#
# Revision 41.10.2.1  1998/02/26 18:30:31  mlemos
# Changed building options to produce a version of the library for debugging.
# Added missing includes and corrected their paths.
# Added a test directory with test program.
#
# Revision 41.10  1998/02/25 21:12:36  mlemos
# Bumping to 41.10
#
# Revision 1.1  1998/02/25 17:09:03  mlemos
# Ian sources
#
#
#

ver=41
o=_$(BUILD_TYPE).o
link=slink QUIET SMALLCODE ADDSYM STRIPDEBUG NODEBUG NOICONS MAXHUNK 32000 PRELINK FROM
DEBUG_OPTIONS=DEBUG=FULLFLUSH DEFINE DEBUG_BGUI
STANDARD_OPTIONS=NODEBUG OPT
ENHANCED_OPTIONS=NODEBUG OPT OPTTIME CPU=68020 DEFINE=ENHANCED

PRAGMA_DIRECTORY=include:pragma
INLINE_DIRECTORY=include:inline
FD_DIRECTORY=include:fd

includes= \
include:libraries/bgui.h \
include:bgui/bgui.h \
include:bgui/bgui_image.h \
include:bgui/bgui_asl.h \
include:bgui/bgui_arexx.h \
include:bgui/bgui_cx.h \
include:bgui/bgui_obsolete.h \
include:bgui/bgui_prefs.h \
include:libraries/bgui_macros.h \
include:bgui/bgui_macros.h \
include:libraries/bgui_treeview.h \
include:bgui/bgui_treeview.h \
include:libraries/bgui_bar.h \
include:bgui/bgui_bar.h \
include:libraries/bgui_layoutgroup.h \
include:bgui/bgui_layoutgroup.h \
include:proto/bgui.h \
include:clib/bgui_protos.h \
include:pragmas/bgui_pragmas.h \
$(PRAGMA_DIRECTORY)/bgui_lib.h \
$(FD_DIRECTORY)/bgui_lib.fd \
$(INLINE_DIRECTORY)/bgui.h

build_and_install: debug_build
   @smake -s -f makefile BUILD_TYPE=D install
   @echo "bgui.library is built and installed in libs:"

debug_build:
   @echo "Making BGUI version for debugging..."
   @smake -s -f makefile BUILD_TYPE=D build

debug_install:
   @echo "Installing BGUI version for debugging..."
   @smake -s -f makefile BUILD_TYPE=D install

standard_build:
   @echo "Making BGUI standard version..."
   @smake -s -f makefile BUILD_TYPE=S build

enhanced_build:
   @echo "Making BGUI enhanced version..."
   @smake -s -f makefile BUILD_TYPE=E build

build: bin/bgui.library.$(BUILD_TYPE) test examples tools

tools= \
bin/CAD \
bin/BGUI \
bin/KillBGUI \
bin/Tap

tools: $(includes) $(tools)

install: bin/KillBGUI bin/bgui.library.$(BUILD_TYPE)
   @echo "Installing BGUI library..."
   @bin/KillBGUI
   @copy bin/bgui.library.$(BUILD_TYPE) libs:bgui.library

libs:bgui.library: 
   @smake -s -f makefile debug_install

PROGRAM_OPTIONS = RESETOPTIONS NOVER NOICONS LINK DEBUG=FULLFLUSH ERRORREXX NOSTACKCHECK ObjectName=o/

bin/MakeProto: MakeProto.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/KillBGUI: KillBGUI.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) DEFINE __USE_SYSBASE $? ProgramName=$@
   -@delete QUIET $@.lnk

KillBGUI: bin/KillBGUI
   @smake -s -f makefile BUILD_TYPE=D bin/KillBGUI

bin/test1: test/test1.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test2: test/test2.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test3: test/test3.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test4: test/test4.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test5: test/test5.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test6: test/test6.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test7: test/test7.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test8: test/test8.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test9: test/test9.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/test10: test/test10.e
   @echo Making $@...
   @ec QUIET $*
   @copy $* $@
   @Delete QUIET $*

bin/test11: test/test11.e
   @echo Making $@...
   @ec QUIET $*
   @copy $* $@
   @Delete QUIET $*

bin/test13: test/test13.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

test= \
bin/test1 \
bin/test2 \
bin/test3 \
bin/test4 \
bin/test5 \
bin/test6 \
bin/test7 \
bin/test8 \
bin/test9 \
bin/test10 \
bin/test11 \
bin/test13

test: $(includes) $(test)

examples= \
bin/AddButtons \
bin/ColorWheel \
bin/TextField \
bin/Area \
bin/ARexxDemo \
bin/BGUIDemo \
bin/Colors \
bin/CXDemo \
bin/Useless \
bin/StringHook \
bin/MultiSelect \
bin/IDCMPHook \
bin/Images \
bin/FieldList \
bin/Backdrop \
bin/MultiFont \
bin/FileRequester \
bin/List \
bin/MultiColumn \
bin/ViewGroup

examples: $(includes) $(examples)

example:
   @smake -s -f makefile BUILD_TYPE=D examples

gadget_directories= \
gadgets/PopButton \
gadgets/Palette \
gadgets/TreeView \
gadgets/Bar \
gadgets/LayoutGroup

o/cad_main.o: cad/cad_main.c CAD/cad.h 

o/cad_file.o: cad/cad_file.c CAD/cad.h

o/cad_support.o: cad/cad_support.c CAD/cad.h

bin/CAD: $(includes) o/cad_main.o o/cad_file.o o/cad_support.o
   @echo Making $@...
   @slink NOICONS SMALLCODE SMALLDATA ADDSYM STRIPDEBUG NODEBUG NOICONS QUIET FROM LIB:c.o o/cad_main.o o/cad_file.o o/cad_support.o LIB:debug.lib lib lib:sc.lib lib:amiga.lib to $@
   -@delete QUIET $@.lnk

CAD_PROGRAM: $(includes) cad/cad_main.c cad/cad_file.c cad/cad_support.c CAD/cad.h
   @smake -s -f makefile BUILD_TYPE=D bin/CAD

.cad.doc: libs:bgui.library bin/CAD
   @bin/CAD $*.cad TO $@

docs= \
CAD/doc/aslreqclass.doc \
CAD/doc/baseclass.doc \
CAD/doc/bgui.doc \
CAD/doc/buttonclass.doc \
CAD/doc/checkboxclass.doc \
CAD/doc/commodityclass.doc \
CAD/doc/cycleclass.doc \
CAD/doc/externalclass.doc \
CAD/doc/filereqclass.doc \
CAD/doc/frameclass.doc \
CAD/doc/groupclass.doc \
CAD/doc/indicatorclass.doc \
CAD/doc/infoclass.doc \
CAD/doc/labelclass.doc \
CAD/doc/listviewclass.doc \
CAD/doc/methods.doc \
CAD/doc/mxclass.doc \
CAD/doc/pageclass.doc \
CAD/doc/progressclass.doc \
CAD/doc/propclass.doc \
CAD/doc/separatorclass.doc \
CAD/doc/sliderclass.doc \
CAD/doc/stringclass.doc \
CAD/doc/vectorclass.doc \
CAD/doc/windowclass.doc \
CAD/doc/popbuttonclass.doc \
CAD/doc/paletteclass.doc

doc: $(docs)

bin/BGUI: $(includes) Prefs/BGUIPrefs.c Prefs/BGUIPrefs_gui.c Prefs/BGUIPrefs_iff.c Prefs/BGUIPrefs.h Prefs/BGUIPrefs_rev.h
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) Prefs/BGUIPrefs.c ProgramName=$@
   -@delete QUIET $@.lnk

BGUI: bin/BGUI
   @smake -s -f makefile BUILD_TYPE=D bin/BGUI


bin/Tap: Tap.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

Tap: bin/Tap
   @smake -s -f makefile BUILD_TYPE=D bin/Tap

extra_gadgets:
   @execute >NIL: <<
      cd gadgets/PopButton
      smake -s
      cd //gadgets/Palette
      smake -s
      cd //gadgets/TreeView
      smake -s
      cd //gadgets/Bar
      smake -s
      cd //gadgets/LayoutGroup
      smake -s
   <

t:build_source_archive:
   @execute >NIL: <<
      if exists t:bgui
         echo Deleting t:bgui directory...
         delete QUIET t:bgui all
      endif
   <
   @echo Extracting current source tree...
   @execute >NIL: <<
      cd t:
      cvs export -r HEAD bgui
   <
   @echo Making the distribution lha source archive...
   @execute >NIL: <<
      if exists t:bgui_src.lha
         echo Deleting t:bgui_src.lha archive...
         delete QUIET t:bgui_src.lha
      endif
      cd t:
      lha -r a bgui_src.lha bgui
   <
   @copy t:bgui_src.lha website/bin/bgui_src.lha
   @delete QUIET t:bgui_src.lha
   @delete QUIET t:bgui ALL
   @echo ok >t:build_source_archive

t:build_developers_archive: update_revision debug_build doc html_documentation amigaguide_documentation
   @execute >NIL: <<
      if exists t:bgui
         echo Deleting t:bgui directory...
         delete QUIET t:bgui all
      endif
      if exists t:BGUI.info
         echo Deleting t:BGUI.info directory icon...
         delete QUIET t:BGUI.info
      endif
   <
   @makedir t:bgui
   @echo Copying BGUI directory icon...
   @copy icons/def_Drawer.info to t:BGUI.info
   @echo Copying BGUI LICENSE document...
   @copy LICENSE to t:bgui/LICENSE
   @copy icons/def_doc.info t:bgui/LICENSE.info quiet
   @makedir t:bgui/libs
   @echo Copying BGUI library...
   @slink from bin/bgui.library.D to t:bgui/libs/bgui.library NODEBUG NOICONS QUIET
   @makedir t:bgui/Tools
   @copy icons/def_Drawer.info to t:bgui/Tools.info
   @echo Copying CAD utility program...
   @slink from bin/CAD to t:bgui/tools/CAD NODEBUG NOICONS QUIET
   @copy icons/def_Tool.info to t:bgui/tools/CAD.info
   @echo Copying Tap program...
   @slink from bin/Tap to t:bgui/tools/Tap NODEBUG NOICONS QUIET
   @copy icons/def_Tool.info to t:bgui/tools/Tap.info
   @echo Copying KillBGUI program...
   @slink from bin/KillBGUI to t:bgui/tools/KillBGUI NODEBUG NOICONS QUIET
   @copy icons/def_Tool.info to t:bgui/tools/KillBGUI.info
   @echo Copying BGUIBuilder program...
   @slink from bin/BGUIBuilder to t:bgui/tools/BGUIBuilder NODEBUG NOICONS QUIET
   @copy icons/def_Tool.info t:bgui/tools/BGUIBuilder.info quiet
   @echo Copying include files...
   @makedir t:bgui/include
   @makedir t:bgui/include/libraries
   @copy include/libraries_bgui.h to t:bgui/include/libraries/bgui.h
   @copy icons/def_h.info to t:bgui/include/libraries/bgui.h.info
   @makedir t:bgui/include/bgui
   @copy include/bgui.h to t:bgui/include/bgui/bgui.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui.h.info
   @copy include/bgui_image.h to t:bgui/include/bgui/bgui_image.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_image.h.info
   @copy include/bgui_asl.h to t:bgui/include/bgui/bgui_asl.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_asl.h.info
   @copy include/bgui_arexx.h to t:bgui/include/bgui/bgui_arexx.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_arexx.h.info
   @copy include/bgui_cx.h to t:bgui/include/bgui/bgui_cx.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_cx.h.info
   @copy include/bgui_obsolete.h to t:bgui/include/bgui/bgui_obsolete.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_obsolete.h.info
   @copy include/libraries_bgui_macros.h to t:bgui/include/libraries/bgui_macros.h
   @copy icons/def_h.info to t:bgui/include/libraries/bgui_macros.h.info
   @copy include/bgui_macros.h to t:bgui/include/bgui/bgui_macros.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_macros.h.info
   @copy gadgets/TreeView/include/TreeViewClass.h to t:bgui/include/bgui/bgui_treeview.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_treeview.h.info
   @copy include/libraries_bgui_treeview.h to t:bgui/include/libraries/bgui_treeview.h
   @copy icons/def_h.info to t:bgui/include/libraries/bgui_treeview.h.info
   @copy gadgets/Bar/include/BarClass.h to t:bgui/include/bgui/bgui_bar.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_bar.h.info
   @copy include/libraries_bgui_bar.h to t:bgui/include/libraries/bgui_bar.h
   @copy icons/def_h.info to t:bgui/include/libraries/bgui_bar.h.info
   @copy gadgets/LayoutGroup/include/LayoutGroupClass.h to t:bgui/include/bgui/bgui_layoutgroup.h
   @copy icons/def_h.info to t:bgui/include/bgui/bgui_layoutgroup.h.info
   @copy include/libraries_bgui_layoutgroup.h to t:bgui/include/libraries/bgui_layoutgroup.h
   @copy icons/def_h.info to t:bgui/include/libraries/bgui_layoutgroup.h.info
   @makedir t:bgui/include/proto
   @copy include/bgui_proto.h to t:bgui/include/proto/bgui.h
   @copy icons/def_h.info to t:bgui/include/proto/bgui.h.info
   @makedir t:bgui/include/clib
   @copy include/bgui_protos.h to t:bgui/include/clib/bgui_protos.h
   @copy icons/def_h.info to t:bgui/include/clib/bgui_protos.h.info
   @makedir t:bgui/include/pragmas
   @copy include/bgui_pragmas.h to t:bgui/include/pragmas/bgui_pragmas.h
   @copy icons/def_h.info to t:bgui/include/pragmas/bgui_pragmas.h.info
   @makedir t:bgui/include/pragma
   @copy include/pragma/bgui_lib.h to t:bgui/include/pragma/bgui_lib.h
   @copy icons/def_h.info to t:bgui/include/pragma/bgui_lib.h.info
   @makedir t:bgui/include/fd
   @copy include/bgui_lib.fd to t:bgui/include/fd/bgui_lib.fd
   @copy icons/def_fd.info to t:bgui/include/fd/bgui_lib.fd.info
   @makedir t:bgui/include/inline
   @copy include/inline/bgui.h to t:bgui/include/inline/bgui.h
   @copy icons/def_h.info to t:bgui/include/inline/bgui.h.info
   @makedir t:bgui/AutoDoc
   @copy icons/def_Drawer.info to t:bgui/AutoDoc.info
   @execute >NIL: <<
      echo Copying AutoDoc files...
      foreach f in CAD/doc/*.doc
         copy $$f t:bgui/AutoDoc quiet
         copy //icons/def_doc.info t:bgui/AutoDoc/$$f.info quiet
      end
   <
   @execute >NIL: <<
      echo Making the developers distribution lha archive...
      if exists t:bgui_dev.lha
         echo Deleting t:bgui_dev.lha archive...
         delete QUIET t:bgui_dev.lha
      endif
      cd t:
      lha -r a bgui_dev.lha bgui BGUI.info
   <
   @copy t:bgui_dev.lha website/bin/bgui_dev.lha
   @delete QUIET t:bgui_dev.lha
   @delete QUIET t:BGUI.info
   @delete QUIET t:bgui all
   @echo ok >t:build_developers_archive

html_documentation: CAD_PROGRAM
   @execute >NIL: <<
      echo Removing prior HTML documentation files...
      if exists website/bgui_documentation
         delete QUIET website/bgui_documentation/~(.cvsignore|CVS|.htaccess)
      endif
      echo Copying BGUI LICENSE document...
      copy LICENSE to website/bgui_documentation/LICENSE
      copy icons/def_doc.info website/bgui_documentation/LICENSE quiet
      echo Generating HTML documentation files...
      echo >website/bgui_documentation/index.html "<HTML>*n<HEAD>*n<TITLE>BGUI Documentation - Table of contents</TITLE>*n</HEAD>*n<BODY>*n<H1><CENTER>BGUI Documentation</CENTER></H1>*n<HR>*n<UL>*n<H2>Table of contents</H2>"
      foreach f IN CAD/doc/*.cad SORT
         set html `get f.BASE`
         bin/CAD CAD/doc/$$f TO website/bgui_documentation/$$html.html HTML
         echo >>website/bgui_documentation/index.html "<LI><A HREF=$$html.html>$$html</A></LI>"
      end
      echo >>website/bgui_documentation/index.html "</UL>*n<HR>*n</BODY>*n</HTML>*n";
      echo Generating HTML documentation archive...
      cd website
      Delete bin/bgui_html_documentation.lha
      lha a bin/bgui_html_documentation.lha bgui_documentation/~(.cvsignore|CVS|.htaccess)
   <

amigaguide_documentation: CAD_PROGRAM
   @execute >NIL: <<
      echo Removing prior AmigaGuide documentation files...
      if exists t:bgui_documentation
         delete QUIET t:bgui_documentation all
      endif
      makedir t:bgui_documentation
      echo Copying BGUI LICENSE document...
      copy LICENSE to t:bgui_documentation/LICENSE
      copy icons/def_doc.info t:bgui_documentation/LICENSE.info quiet
      echo Generating AmigaGuide documentation files...
      echo >t:bgui_documentation/index.guide "@database*n@node *"Main*" link *"BGUI Documentation*"*nTABLE OF CONTENTS*n"
      foreach f IN CAD/doc/*.cad SORT
         set guide `get f.BASE`
         bin/CAD CAD/doc/$$f TO t:bgui_documentation/$$guide.guide GUIDE
         echo >>t:bgui_documentation/index.guide "   @{*"$$guide*" link *"$$guide.guide/main*" 0}"
      end
      echo >>t:bgui_documentation/index.guide "@endnode"
      echo Generating AmigaGuide documentation archive...
      Delete website/bin/bgui_guide_documentation.lha
      lha a website/bin/bgui_guide_documentation.lha t:bgui_documentation
      echo Removing generated AmigaGuide documentation files...
      if exists t:bgui_documentation
         delete QUIET t:bgui_documentation all
      endif
   <

update_revision:
   -@Delete o/bgui.library_rev.h

t:build_users_archive: update_revision enhanced_build standard_build doc extra_gadgets
   @execute >NIL: <<
      if exists t:bgui
         echo Deleting t:bgui directory...
         delete QUIET t:bgui all
      endif
      if exists t:BGUI.info
         echo Deleting t:BGUI.info directory icon...
         delete QUIET t:BGUI.info
      endif
   <
   @makedir t:bgui
   @echo Copying BGUI directory icon...
   @copy icons/def_Drawer.info to t:BGUI.info
   @echo Copying BGUI LICENSE document...
   @copy LICENSE to t:bgui/LICENSE
   @copy icons/def_doc.info t:bgui/LICENSE.info quiet
   @makedir t:bgui/libs
   @echo Copying Standard version of BGUI library...
   @slink from bin/bgui.library.S to t:bgui/libs/bgui.library NODEBUG NOICONS QUIET
   @echo Copying Enhanced version of BGUI library...
   @slink from bin/bgui.library.E to t:bgui/libs/bgui.library_e NODEBUG NOICONS QUIET
   @makedir t:bgui/libs/gadgets
   @echo Copying extra gadgets libraries...
   @slink from gadgets/bgui_popbutton.gadget to t:bgui/libs/gadgets/bgui_popbutton.gadget NODEBUG NOICONS QUIET
   @slink from gadgets/bgui_palette.gadget to t:bgui/libs/gadgets/bgui_palette.gadget NODEBUG NOICONS QUIET
   @slink from gadgets/bgui_treeview.gadget to t:bgui/libs/gadgets/bgui_treeview.gadget NODEBUG NOICONS QUIET
   @slink from gadgets/bgui_bar.gadget to t:bgui/libs/gadgets/bgui_bar.gadget NODEBUG NOICONS QUIET
   @slink from gadgets/bgui_layoutgroup.gadget to t:bgui/libs/gadgets/bgui_layoutgroup.gadget NODEBUG NOICONS QUIET
   @echo "Assign LIBS: libs ADD" to t:bgui/MakeAssign
   @protect t:bgui/MakeAssign +s
   @copy icons/def_Script.info to t:bgui/MakeAssign.info
   @makedir t:bgui/Examples
   @copy icons/def_Drawer.info to t:bgui/Examples.info
   @makedir t:bgui/Examples/Source
   @copy icons/def_Drawer.info to t:bgui/Examples/Source.info
   @execute >NIL: <<
      echo Copying source and program example files...
      foreach f in $(examples)
         copy /examples/$$f.c t:bgui/Examples/Source quiet
         copy /icons/def_c.info t:bgui/Examples/Source/$$f.c.info quiet
         slink from $$f to t:bgui/Examples/$$f NODEBUG NOICONS QUIET
         copy /icons/def_Example.info t:bgui/Examples/$$f.info quiet
      end
   <
   @execute >NIL: <<
      echo Copying extra gadgets demo program and example source files...
      foreach f in $(gadget_directories)
         set demo Demo
         copy $$f/$$f$$demo.c t:bgui/Examples/Source quiet
         copy /icons/def_c.info t:bgui/Examples/Source/$$f$$demo.c.info quiet
         slink /bin/$$f$$demo to t:bgui/Examples/$$f$$demo NODEBUG NOICONS QUIET
         copy /icons/def_Example.info t:bgui/Examples/$$f$$demo.info quiet
      end
   <
   @echo Copying example ARexx files...
   @copy examples/demo.rexx t:bgui/Examples/ quiet
   @copy icons/def_rexx.info t:bgui/Examples/demo.rexx.info quiet
   @echo Copying example include files...
   @copy examples/DemoCode.h examples/FieldList.h t:bgui/Examples/Source quiet
   @copy icons/def_h.info t:bgui/Examples/Source/DemoCode.h.info quiet
   @copy icons/def_h.info t:bgui/Examples/Source/FieldList.h.info quiet
   @echo Copying BGUIPrefs program...
   @slink from bin/BGUI to t:bgui/BGUI NODEBUG NOICONS QUIET
   @copy icons/def_Tool.info t:bgui/BGUI.info quiet
   @execute >NIL: <<
      echo Making the users distribution lha archive...
      if exists t:bgui.lha
         echo Deleting t:bgui.lha archive...
         delete QUIET t:bgui.lha
      endif
      cd t:
      lha -r a bgui.lha bgui BGUI.info
   <
   @copy t:bgui.lha website/bin/bgui.lha
   @delete QUIET t:bgui.lha
   @delete QUIET t:BGUI.info
   @delete QUIET t:bgui all
   @echo ok >t:build_users_archive

rexx_examples= \
RexxBGUI/arrowcontrol.rexx \
RexxBGUI/conditional.rexx \
RexxBGUI/dirlist.rexx \
RexxBGUI/iconbutton.rexx \
RexxBGUI/imagebutton.rexx \
RexxBGUI/list.rexx \
RexxBGUI/mclist.rexx \
RexxBGUI/menu.rexx \
RexxBGUI/multiselect.rexx \
RexxBGUI/progress.rexx \
RexxBGUI/selectline.rexx \
RexxBGUI/testbgui.rexx \
RexxBGUI/testbgui2.rexx \
RexxBGUI/testlistview.rexx \
RexxBGUI/testpages.rexx \
RexxBGUI/testpageslv.rexx \
RexxBGUI/testreq.rexx \
RexxBGUI/testver3.rexx

aweb_rexx_examples= \
RexxBGUI/bguimodes.awebrx \
RexxBGUI/bguishowdir.awebrx

RexxBGUI/rexxbgui.library:
   @execute >NIL: <<
      cd RexxBGUI
      smake -f .build all
   <

t:build_arexx_library_archive: RexxBGUI/rexxbgui.library
   @execute >NIL: <<
      if exists t:bgui
         echo Deleting t:bgui directory...
         delete QUIET t:bgui all
      endif
      if exists t:BGUI.info
         echo Deleting t:BGUI.info directory icon...
         delete QUIET t:BGUI.info
      endif
   <
   @makedir t:bgui
   @echo Copying BGUI directory icon...
   @copy icons/def_Drawer.info to t:BGUI.info
   @echo Copying BGUI LICENSE document...
   @copy LICENSE to t:bgui/LICENSE
   @copy icons/def_doc.info t:bgui/LICENSE.info quiet
   @makedir t:bgui/libs
   @echo Copying RexxBGUI library...
   @slink from RexxBGUI/rexxbgui.library to t:bgui/libs/rexxbgui.library NODEBUG NOICONS QUIET
   @echo "Assign LIBS: libs ADD" to t:bgui/MakeAssign
   @protect t:bgui/MakeAssign +s
   @copy icons/def_Script.info to t:bgui/MakeAssign.info
   @makedir t:bgui/Examples
   @copy icons/def_Drawer.info to t:bgui/Examples.info
   @makedir t:bgui/Examples/ARexx
   @copy icons/def_Drawer.info to t:bgui/Examples/ARexx.info
   @execute >NIL: <<
      echo Copying BGUI ARexx example files...
      foreach f in $(rexx_examples)
         copy /RexxBGUI/$$f t:bgui/Examples/ARexx quiet
         copy /icons/def_rexx.info t:bgui/Examples/ARexx/$$f.info quiet
      end
   <
   @execute >NIL: <<
      echo Copying AWeb BGUI ARexx example files...
      foreach f in $(aweb_rexx_examples)
         copy /RexxBGUI/$$f t:bgui/Examples/ARexx quiet
         copy /icons/def_rexx.info t:bgui/Examples/ARexx/$$f.info quiet
      end
   <
   @makedir t:bgui/Tools
   @copy icons/def_Drawer.info to t:bgui/Tools.info
   @echo Copying MakeImage utility program...
   @slink from RexxBGUI/MakeImage to t:bgui/tools/MakeImage NODEBUG NOICONS QUIET
   @copy icons/def_Tool.info to t:bgui/tools/MakeImage.info
   @makedir t:bgui/AutoDoc
   @copy icons/def_Drawer.info to t:bgui/AutoDoc.info
   @copy RexxBGUI/RexxBGUI.doc to t:bgui/AutoDoc/RexxBGUI.doc
   @copy icons/def_doc.info to t:bgui/AutoDoc/RexxBGUI.doc.info
   @copy RexxBGUI/Functions.doc to t:bgui/AutoDoc/RexxFunctions.doc
   @copy icons/def_doc.info to t:bgui/AutoDoc/RexxFunctions.doc.info
   @execute >NIL: <<
      echo Making the Rexx BGUI distribution lha archive...
      if exists t:bgui_rexx.lha
         echo Deleting t:bgui_rexx.lha archive...
         delete QUIET t:bgui_rexx.lha
      endif
      cd t:
      lha -r a bgui_rexx.lha bgui BGUI.info
   <
   @copy t:bgui_rexx.lha website/bin/bgui_rexx.lha
   @delete QUIET t:bgui_rexx.lha
   @delete QUIET t:BGUI.info
   @delete QUIET t:bgui all
   @echo ok >t:build_users_archive

dist: t:build_users_archive t:build_developers_archive t:build_source_archive t:build_arexx_library_archive
   @echo Making the website distribution archive...
   @execute >NIL: <<
      cd website
      Apache:cgi-bin/php3.cgi -q dist.php
   <
   @delete QUIET t:build_developers_archive t:build_users_archive t:build_source_archive t:build_arexx_library_archive
   @echo Done.


bin/AddButtons: examples/AddButtons.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/ColorWheel: examples/ColorWheel.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/TextField: examples/TextField.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/Area: examples/Area.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/ARexxDemo: examples/ARexxDemo.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/BGUIDemo: examples/BGUIDemo.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/Colors: examples/Colors.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/CXDemo: examples/CXDemo.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/Useless: examples/Useless.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/StringHook: examples/StringHook.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/MultiSelect: examples/MultiSelect.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/IDCMPHook: examples/IDCMPHook.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/Images: examples/Images.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/FieldList: examples/FieldList.c examples/FieldList.h
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) DEFINE DEBUG_BGUI LIBRARY LIB:debug.lib examples/FieldList.c ProgramName=$@
   -@delete QUIET $@.lnk

bin/Backdrop: examples/Backdrop.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/MultiFont: examples/MultiFont.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/FileRequester: examples/FileRequester.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/List: examples/List.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/MultiColumn: examples/MultiColumn.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

bin/ViewGroup: examples/ViewGroup.c
   @echo Making $@...
   @sc $(PROGRAM_OPTIONS) $? ProgramName=$@
   -@delete QUIET $@.lnk

o/class-protos.h: $(proto_src) bin/MakeProto
   @echo Making $@...
   @bin/MakeProto \#?.c \#?.asm header=$@

$(PRAGMA_DIRECTORY) $(INLINE_DIRECTORY) $(FD_DIRECTORY):
   makedir $@

include:libraries/bgui.h:             include/libraries_bgui.h
include:bgui/bgui.h:                  include/bgui.h
include:bgui/bgui_image.h:            include/bgui_image.h
include:bgui/bgui_asl.h:              include/bgui_asl.h
include:bgui/bgui_arexx.h:            include/bgui_arexx.h
include:bgui/bgui_cx.h:               include/bgui_cx.h
include:bgui/bgui_obsolete.h:         include/bgui_obsolete.h
include:bgui/bgui_prefs.h:            include/bgui_prefs.h
include:libraries/bgui_macros.h:      include/libraries_bgui_macros.h
include:bgui/bgui_macros.h:           include/bgui_macros.h
include:libraries/bgui_treeview.h:    include/libraries_bgui_treeview.h
include:bgui/bgui_treeview.h:         gadgets/TreeView/include/TreeViewClass.h
include:libraries/bgui_bar.h:         include/libraries_bgui_bar.h
include:bgui/bgui_bar.h:              gadgets/Bar/include/BarClass.h
include:libraries/bgui_layoutgroup.h: include/libraries_bgui_layoutgroup.h
include:bgui/bgui_layoutgroup.h:      gadgets/LayoutGroup/include/LayoutGroupClass.h
include:proto/bgui.h:                 include/bgui_proto.h
include:clib/bgui_protos.h:           include/bgui_protos.h
include:pragmas/bgui_pragmas.h:       include/bgui_pragmas.h
$(PRAGMA_DIRECTORY)/bgui_lib.h:       $(PRAGMA_DIRECTORY) include/pragma/bgui_lib.h 
$(INLINE_DIRECTORY)/bgui.h:           $(INLINE_DIRECTORY) include/inline/bgui.h
$(FD_DIRECTORY)/bgui_lib.fd:          $(FD_DIRECTORY) include/bgui_lib.fd
   @echo Installing $@...
   @copy >NIL: include/bgui_lib.fd $(FD_DIRECTORY)/bgui_lib.fd

clean:
   @echo "Deleting installed includes..."
   -@delete QUIET $(includes)
   @echo "Deleting generated AutoDoc files ..."
   -@delete QUIET CAD/doc/\#?.doc
   @echo "Deleting generated HTML files ..."
   -delete QUIET website/bgui_documentation/~(.cvsignore|CVS|.htaccess)
   @echo "Deleting example programs..."
   -@delete QUIET $(examples)
   @echo "Deleting test programs..."
   -@delete QUIET $(test)
   @echo "Deleting utility programs..."
   -@delete QUIET bin/KillBGUI bin/MakeProto bin/CAD bin/BGUI bin/Tap
   @echo "Deleting BGUI library..."
   -@delete QUIET libs:bgui.library bin/bgui.library.(D|S|E)
   @echo "Deleting intermediate files..."
   -@delete QUIET o/~(dummy.h|CVS) bin/\#?.lnk
   @echo "Clean rexxbgui.library and intermediate files..."
   @execute >NIL: <<
      cd RexxBGUI
      smake -f .build clean
   <
   @echo "Done."

.c.o:
   @execute <<
      if exists $@
         list >ENV:sizeold $@ lformat="%l"
     else
        echo 0 >ENV:sizeold
     endif
   <
   @echo "Compiling $*.c..."
   @execute <<
      if $(BUILD_TYPE) eq D
         sc $< objname $@ $(DEBUG_OPTIONS)
      endif
      if $(BUILD_TYPE) eq S
         sc $< objname $@ $(STANDARD_OPTIONS)
      endif
      if $(BUILD_TYPE) eq E
         sc $< objname $@ $(ENHANCED_OPTIONS)
      endif
   <
   -@list >ENV:sizenew $@ lformat="%l"
   @echo "$@: $$sizenew bytes, `eval $$sizenew-$$sizeold` change.*n"

.asm.o:
   @echo "Assembling $*.asm..."
   @sc NOVER assembler=$*.asm ObjectName=$@

.h.h:
   @echo Installing $@...
   @copy >NIL: $*.h to $@


o/bgui.library_rev.h: bgui.library_rev.h
   @echo Bumping the revision of $@...
   @copy $? TO $@

bin/bgui.library.$(BUILD_TYPE): $(includes) o/class-protos.h o/libtag.o o/plmain$(o) o/plgfx$(o) o/plboopsi$(o) o/bgui.library_rev.h
bin/bgui.library.$(BUILD_TYPE): o/plimageclasses$(o) o/plgadgetclasses$(o) o/plgroupclasses$(o) o/plaslclasses$(o)
   @execute >NIL: <<
   if $(BUILD_TYPE) eq D
      sc >NIL: ver.c objname=o/ver.o nolink $(DEBUG_OPTIONS)
   endif
   if $(BUILD_TYPE) eq S
      sc >NIL: ver.c objname=o/ver.o nolink $(STANDARD_OPTIONS)
   endif
   if $(BUILD_TYPE) eq E
      sc >NIL: ver.c objname=o/ver.o nolink $(ENHANCED_OPTIONS)
   endif
   <
   @echo "Linking $@..."
   @execute <<
      if exists $@
         list >ENV:sizeold $@ lformat="%l"
     else
        echo 0 >ENV:sizeold
     endif
   <
   @slink NOICONS quiet addsym from o/libtag.o o/ver.o lib o/plmain$(o) lib o/plgfx$(o) \
      lib o/plboopsi$(o) lib o/plimageclasses$(o) o/plgadgetclasses$(o) \
      lib o/plaslclasses$(o) lib o/plgroupclasses$(o) LIB:debug.lib lib lib:sc.lib lib:amiga.lib to $@
   -@list >ENV:sizenew $@ lformat="%l"
   -@list $@ nohead lformat="Final %n size: %l bytes, `eval $$sizenew-$$sizeold` change.*n"

bumprev:
   execute <<
   cd o
   if exists bgui.library_rev.rev
      protect bgui.library_rev.rev rwd
   endif
   bin/bumprev >NIL: $(ver) bgui.library
   protect bgui.library_rev.rev r
   cd /
   <


main_o=o/lib$(o) o/libfunc$(o) o/task$(o) o/request$(o) \
   o/misc$(o) o/bgui_locale$(o) o/miscasm$(o) o/stkext$(o)

boopsi_o=o/classes$(o) o/dgm$(o) o/rootclass$(o) o/arexxclass$(o)

gfx_o=o/gfx$(o) o/blitter$(o) o/textclass$(o) o/strformat$(o)

group_o=o/windowclass$(o) o/groupclass$(o) o/spacingclass$(o) o/pageclass$(o) \
   o/viewclass$(o) o/externalclass$(o)

image_o=o/frameclass$(o) o/labelclass$(o) o/separatorclass$(o) o/vectorclass$(o) o/systemiclass$(o)

gadget_o=o/baseclass$(o) o/areaclass$(o) o/buttonclass$(o) \
   o/checkboxclass$(o) o/commodityclass$(o) o/cycleclass$(o) o/indicatorclass$(o) \
   o/infoclass$(o) o/listclass$(o) o/mxclass$(o) o/progressclass$(o) o/propclass$(o) \
   o/sliderclass$(o) o/stringclass$(o) o/radiobuttonclass$(o)

asl_o=o/aslreqclass$(o) o/filereqclass$(o) o/fontreqclass$(o) o/screenreqclass$(o)


o/plmain$(o): $(main_o)
   @echo "Linking $@..."
   -@delete QUIET $@
   @oml >NIL: $@ add $(main_o)
#   $(link) $(main_o) to $@

o/plboopsi$(o): $(boopsi_o)
   @echo "Linking $@..."
   -@delete QUIET $@
   @oml >NIL: $@ add $(boopsi_o)
#   $(link) $(boopsi_o) to $@

o/plgfx$(o): $(gfx_o)
   @echo "Linking $@..."
   -@delete QUIET $@
   @oml >NIL: $@ add $(gfx_o)
#   $(link) $(gfx_o) to $@

o/plgroupclasses$(o): $(group_o)
   @echo "Linking $@..."
   -@delete QUIET $@
   @oml >NIL: $@ add $(group_o)
#   $(link) $(group_o) to $@

o/plimageclasses$(o): $(image_o)
   @echo "Linking $@..."
   -@delete QUIET $@
   @oml >NIL: $@ add $(image_o)
#   $(link) $(image_o) to $@

o/plgadgetclasses$(o): $(gadget_o)
   @echo "Linking $@..."
   -@delete QUIET $@
   @oml >NIL: $@ add $(gadget_o)
#   $(link) $(gadget_o) to $@

o/plaslclasses$(o): $(asl_o)
   @echo "Linking $@..."
   @$(link) $(asl_o) to $@


o/libtag.o: libtag.asm

o/lib$(o): lib.c o/bgui.library_rev.h
o/libfunc$(o): libfunc.c
o/task$(o): task.c
o/request$(o): request.c
o/misc$(o): misc.c
o/bgui_locale$(o): bgui_locale.c
o/miscasm$(o): miscasm.asm
o/stkext$(o): stkext.asm

o/classes$(o): classes.c
o/dgm$(o): dgm.c
o/rootclass$(o): rootclass.c
o/arexxclass$(o): arexxclass.c

o/gfx$(o): gfx.c
o/blitter$(o): blitter.c
o/textclass$(o): textclass.c
o/strformat$(o): strformat.c

o/windowclass$(o): windowclass.c
o/groupclass$(o): groupclass.c
o/spacingclass$(o): spacingclass.c
o/pageclass$(o): pageclass.c
o/viewclass$(o): viewclass.c
o/externalclass$(o): externalclass.c

o/frameclass$(o): frameclass.c
o/labelclass$(o): labelclass.c
o/separatorclass$(o): separatorclass.c
o/vectorclass$(o): vectorclass.c
o/systemiclass$(o): systemiclass.c

o/baseclass$(o): baseclass.c
o/areaclass$(o): areaclass.c
o/buttonclass$(o): buttonclass.c
o/checkboxclass$(o): checkboxclass.c
o/commodityclass$(o): commodityclass.c
o/cycleclass$(o): cycleclass.c
o/indicatorclass$(o): indicatorclass.c
o/infoclass$(o): infoclass.c
o/listclass$(o): listclass.c
o/mxclass$(o): mxclass.c
o/progressclass$(o): progressclass.c
o/propclass$(o): propclass.c
o/sliderclass$(o): sliderclass.c
o/stringclass$(o): stringclass.c
o/radiobuttonclass$(o): radiobuttonclass.c

o/aslreqclass$(o): aslreqclass.c
o/filereqclass$(o): filereqclass.c
o/fontreqclass$(o): fontreqclass.c
o/screenreqclass$(o): screenreqclass.c

proto_src=miscasm.asm stkext.asm libfunc.c request.c task.c gfx.c blitter.c misc.c classes.c dgm.c \
textclass.c strformat.c bgui_locale.c lib.c baseclass.c
