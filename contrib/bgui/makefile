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
# Revision 41.10  1998/02/25 21:12:36  mlemos
# Bumping to 41.10
#
# Revision 1.1  1998/02/25 17:09:03  mlemos
# Ian sources
#
#
#

ver=41
type=_2
cpu=68000
o=$(type).o
link=slink quiet sc sd nd noicons maxhunk 32000 prelink from

includes=include:bgui/bgui.h include:bgui/bgui_image.h include:bgui/bgui_asl.h include:bgui/bgui_cx.h \
   include:bgui/bgui_obsolete.h include:bgui/bgui_macros.h include:clib/bgui_protos.h include:fd/bgui_lib.fd

all: $(includes) bin/bgui.library bin/KillBGUI
   bin/KillBGUI
   copy bin/bgui.library libs:bgui.library
   @echo "SMake complete."

bin/MakeProto: MakeProto.c
	sc RESETOPTIONS NOICONS LINK ObjectName=o/ MakeProto.c ProgramName=bin/MakeProto
	delete bin/MakeProto.lnk

bin/KillBGUI: KillBGUI.c
	sc RESETOPTIONS NOICONS LINK DEFINE __USE_SYSBASE ObjectName=o/ KillBGUI.c ProgramName=bin/KillBGUI
	delete bin/KillBGUI.lnk

o/class-protos.h: $(proto_src) bin/MakeProto
   bin/MakeProto \#?.c \#?.asm header=$@

include:bgui/bgui.h:            include/bgui.h
include:bgui/bgui_image.h:      include/bgui_image.h
include:bgui/bgui_asl.h:        include/bgui_asl.h
include:bgui/bgui_cx.h:         include/bgui_cx.h
include:bgui/bgui_obsolete.h:   include/bgui_obsolete.h
include:bgui/bgui_macros.h:     include/bgui_macros.h
include:clib/bgui_protos.h:     include/bgui_protos.h
include:pragmas/bgui_pragmas.h: include/bgui_pragmas.h

include:fd/bgui_lib.fd:         include/bgui_lib.fd
   copy >NIL: include/bgui_lib.fd include:fd/bgui_lib.fd

clean: 
	delete $(includes) libs:bgui.library bin/bgui.library bin/KillBGUI bin/MakeProto o/\#? 

.c.o:
   -list >ENV:sizeold $@ lformat="%l"
   echo "Compiling $*.c..."
   execute <<
      if $(type) eq _1
         sc $*.c objname $@ cpu=68020 define=ENHANCED
      endif
      if $(type) eq _2
         sc $*.c objname $@ opt verbose
      endif
      if $(type) eq _3
         sc $*.c objname $@ opt verbose opttime cpu=68020 define=ENHANCED
         copy $@ o/$>_1.o
      endif
   <
   -list >ENV:sizenew $@ lformat="%l"
   echo "$@: $$sizenew bytes, `eval $$sizenew-$$sizeold` change.*n"
   
.asm.o:
   echo "Assembling $*.asm..."
   sc assembler=$*.asm ObjectName=$@

.h.h:
   copy >NIL: $*.h to $@


o/bgui.library_rev.h: bgui.library_rev.rev
   execute >NIL: <<
   copy bgui.library_rev.rev o/
   cd o
   /bin/bumprev >NIL: $(ver) bgui.library
   <

bin/bgui.library: o/class-protos.h o/libtag.o t:main$(o) t:gfx$(o) t:boopsi$(o) o/bgui.library_rev.h
bin/bgui.library: t:imageclasses$(o) t:gadgetclasses$(o) t:groupclasses$(o) t:aslclasses$(o)
   execute >NIL: <<
   if $(type) eq _2
      sc >NIL: ver.c objname=o/ver.o nolink opt
   else
      sc >NIL: ver.c objname=o/ver.o nolink opt cpu=68020 define=ENHANCED
   endif
   <
   echo "Linking $@..."
   -list >ENV:sizeold $@ lformat="%l"
   slink quiet lib lib:sc.lib lib:amiga.lib from o/libtag.o o/ver.o t:main$(o) t:gfx$(o) \
      t:boopsi$(o) t:groupclasses$(o) t:imageclasses$(o) t:gadgetclasses$(o) \
      t:aslclasses$(o) to $@
   -list >ENV:sizenew $@ lformat="%l"
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
   o/misc$(o) o/bgui_locale$(o) o/miscasm$(o)

boopsi_o=o/classes$(o) o/dgm$(o) o/rootclass$(o)

gfx_o=o/gfx$(o) o/blitter$(o) o/textclass$(o) o/strformat$(o)

group_o=o/windowclass$(o) o/groupclass$(o) o/spacingclass$(o) o/pageclass$(o) \
   o/viewclass$(o) o/externalclass$(o)

image_o=o/frameclass$(o) o/labelclass$(o) o/separatorclass$(o) o/vectorclass$(o) o/systemiclass$(o)

gadget_o=o/baseclass$(o) o/areaclass$(o) o/buttonclass$(o) \
   o/checkboxclass$(o) o/commodityclass$(o) o/cycleclass$(o) o/indicatorclass$(o) \
   o/infoclass$(o) o/listclass$(o) o/mxclass$(o) o/progressclass$(o) o/propclass$(o) \
   o/sliderclass$(o) o/stringclass$(o) o/radiobuttonclass$(o)

asl_o=o/aslreqclass$(o) o/filereqclass$(o) o/fontreqclass$(o) o/screenreqclass$(o)


t:main$(o): $(main_o)
   echo "Linking $@..."
   $(link) $(main_o) to $@

t:boopsi$(o): $(boopsi_o)
   echo "Linking $@..."
   $(link) $(boopsi_o) to $@

t:gfx$(o): $(gfx_o)
   echo "Linking $@..."
   $(link) $(gfx_o) to $@

t:groupclasses$(o): $(group_o)
   echo "Linking $@..."
   $(link) $(group_o) to $@

t:imageclasses$(o): $(image_o)
   echo "Linking $@..."
   $(link) $(image_o) to $@
      
t:gadgetclasses$(o): $(gadget_o)
   echo "Linking $@..."
   $(link) $(gadget_o) to $@

t:aslclasses$(o): $(asl_o)
   echo "Linking $@..."
   $(link) $(asl_o) to $@


o/libtag.o: libtag.asm

o/lib$(o): lib.c o/bgui.library_rev.h
o/libfunc$(o): libfunc.c
o/task$(o): task.c
o/request$(o): request.c
o/misc$(o): misc.c
o/bgui_locale$(o): bgui_locale.c
o/miscasm$(o): miscasm.asm

o/classes$(o): classes.c
o/dgm$(o): dgm.c
o/rootclass$(o): rootclass.c

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

proto_src=miscasm.asm libfunc.c request.c task.c gfx.c blitter.c misc.c classes.c dgm.c \
textclass.c strformat.c bgui_locale.c lib.c baseclass.c
