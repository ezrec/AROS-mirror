OBJECTS = main.o main1.o main2.o main3.o main4.o main5.o\
screen.o screen_arrows.o screen_colors.o screen_fonts.o screen_palette.o \
screen_sliders.o screen_screenmode.o \
main7.o main8.o main9.o data.o gadgets.o lsprintf.o strings.o paint_mode.o

All : ConfigOpus

ConfigOpus : $(OBJECTS)
	slink with <<
define __ctype=___ctype
from lib:cres.o $(OBJECTS)
to ConfigOpus
lib lib:scm.lib lib:sc.lib lib:amiga.lib
noicons
maxhunk 51200
sd
nodebug
<

.c.o:
	sc $*.c
.asm.o:
	sc:c/asm -isc:include $*.asm

