OBJ1 = dummy.o string.o misc.o render.o buffers.o extensions.o extedit.o matchix.o textfit.o
OBJ1_020 = dummy.o string_020.o misc_020.o render_020.o buffers_020.o extensions_020.o extedit_020.o matchix.o textfit.o
OBJ2 = scroll.o
OBJ3 = stringprefs.o

all: Textinput.mcc.020 Textinput.mcc TextInputscroll.mcc TextInput.mcp

Textinput.mcc: $(OBJ1)
	slink <with <
from $(OBJ1)
to Textinput.mcc.debug
sc sd
with lib:utillib.with
lib lib:sc.lib lib:debug.lib lib:small.lib
map string.map hsflxo swidth 32
<
	-delete Textinput.mcc
	slink from Textinput.mcc.debug to Textinput.mcc nd
	-flushlib Textinput.mcc
#	-copy Textinput.mcc mui:libs/mui/

Textinput.mcc.020: $(OBJ1_020)
	slink <with <
from $(OBJ1_020)
to Textinput.mcc.020.debug
sc sd nd
with lib:utillib.with
lib lib:sc.lib lib:debug.lib lib:small.lib
map string.map hsflxo swidth 32
<
	-delete Textinput.mcc
	slink from Textinput.mcc.020.debug to Textinput.mcc nd
	-flushlib Textinput.mcc
	-delete Textinput.mcc.020
	rename Textinput.mcc Textinput.mcc.020
	-copy Textinput.mcc.020 mui:libs/mui/Textinput.mcc

Textinputscroll.mcc: $(OBJ2)
	slink <with <
from $(OBJ2)
to Textinputscroll.mcc.debug
sc sd addsym
lib lib:sc.lib lib:debug.lib lib:small.lib
map scroll.map hsflxo swidth 32
<
	slink Textinputscroll.mcc.debug to Textinputscroll.mcc nd
	-flushlib Textinputscroll.mcc
	-copy Textinputscroll.mcc mui:libs/mui/

Textinput.mcp: $(OBJ3)
	slink <with <
from $(OBJ3)
to Textinput.mcp
sc sd nd
with lib:utillib.with
lib lib:sc.lib lib:debug.lib lib:small.lib
map stringprefs.map hsflxo swidth 32
<
	-flushlib Textinput.mcp
	-copy Textinput.mcp mui:libs/mui/

rev.h: .revinfo
	/rev/rev QUIET

string.o: string.c scl.gst structs.h misc.h render.h buffers.h extensions.h rev.h mccheader.h textinput_cfg.h textinput_mcc.h textinput_cat.h
	sc string.c

misc.o: misc.c scl.gst structs.h misc.h
	sc misc.c

render.o: render.c scl.gst structs.h misc.h render.h textinput_mcc.h
	sc render.c

buffers.o: buffers.c scl.gst structs.h misc.h
	sc buffers.c

extensions.o: extensions.c scl.gst structs.h misc.h textinput_mcc.h
	sc extensions.c

extedit.o: extedit.c scl.gst
	sc extedit.c

string_020.o: string.c scl.gst structs.h misc.h render.h buffers.h extensions.h rev.h mccheader.h textinput_cfg.h textinput_mcc.h textinput_cat.h
	sc CPU=68020 string.c OBJNAME=string_020.o

misc_020.o: misc.c scl.gst structs.h misc.h
	sc CPU=68020 misc.c OBJNAME=misc_020.o

render_020.o: render.c scl.gst structs.h misc.h render.h textinput_mcc.h
	sc render.c CPU=68020 OBJNAME=render_020.o

buffers_020.o: buffers.c scl.gst structs.h misc.h
	sc buffers.c CPU=68020 OBJNAME=buffers_020.o

extensions_020.o: extensions.c scl.gst structs.h misc.h textinput_mcc.h
	sc extensions.c CPU=68020 OBJNAME=extensions_020.o

extedit_020.o: extedit.c scl.gst
	sc extedit.c CPU=68020 OBJNAME=extedit_020.o

scroll.o: scroll.c scl.gst rev.h mccheader.h textinput_cfg.h textinput_mcc.h
	sc scroll.c

stringprefs.o: stringprefs.c scl.gst rev.h mccheader.h textinput_cfg.h textinput_mcc.h textinput_cat.h
	sc stringprefs.c

matchix.o: matchix.s
	asm matchix.s

dummy.o: dummy.s
	asm dummy.s

menuproc.o: menuproc.c
	sc menuproc.c

scl.gst: mystring.h
	sc makegst=scl.gst gst.c

textinput_cat.h: textinput.cd
	catcomp textinput.cd cfile textinput_cat.h

textfit.o: textfit.s
	asm textfit.s

#
#
#
demo: demo.c
	sc nogst nolibcode link demo.c map

arc: demo textinput.mcc textinputscroll.mcc textinput.mcp
	-makedir >NIL: MCC_Textinput MCC_Textinput/Demos MCC_Textinput/Developer MCC_Textinput/Developer/C MCC_Textinput/Developer/C/Examples MCC_Textinput/Developer/C/Include MCC_Textinput/Developer/C/Include/MUI MCC_Textinput/Libs MCC_Textinput/MUI
	copy demo MCC_Textinput/Demos/Textinput-Demo
	copy demo.c MCC_Textinput/Developer/C/Examples/Textinput-Demo.c
	copy textinput_mcc.h MCC_Textinput/Developer/C/Include/MUI
	copy \#?.mcc \#?.020 \#?.mcp MCC_Textinput/MUI
	copy /vat/vapor_toolkit.library MCC_Textinput/Libs/
	copy Install_Textinput Install_Textinput.info MCC_Textinput
	copy libs:vapor_toolkit.library MCC_Textinput/Libs/
	copy textinput.readme MCC_Textinput/MCC_Textinput.readme
	copy Demos.info Developer.info Install_Textinput.info MUI.info MCC_Textinput.ReadMe.info MCC_Textinput/
	lzx -r -x -a -9 a ram:MCC_Textinput`type .revinfo` MCC_Textinput/\#? MCC_Textinput.info

#
# Own personalized targets
#
zapinstall: all
	copy Textinput.mcc to MUI:Libs/MUI
	-flushlib Textinput.mcc

