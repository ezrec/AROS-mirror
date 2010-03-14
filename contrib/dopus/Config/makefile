ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = M=68020 SYMDEBUG SC SD 4

CC = m68k-amigaos-gcc
CCOPTS = -m68020 -resident
FLEXCAT = Work:Dev/FlexCat/FlexCat

OBJS = main.o main1.o main2.o main3.o main4.o main5.o\
screen.o screen_arrows.o screen_colors.o screen_fonts.o screen_palette.o \
screen_sliders.o screen_screenmode.o \
main7.o main8.o main9.o data.o gadgets.o lsprintf.o strings.o paint_mode.o\
chipdata.o

OBJS_DEBUG = $(patsubst %.o, %.odebug, $(OBJS))

APPNAME = ../../ConfigOpus

All : $(APPNAME) $(APPNAME).debug

$(APPNAME) : $(OBJS)
	$(CC) -o $(APPNAME) $(OBJS) -noixemul $(CCOPTS) -ldebug -s

$(APPNAME).debug : $(OBJS_DEBUG)
	$(CC) -o $(APPNAME).debug $(OBJS_DEBUG) -noixemul $(CCOPTS) -ldebug
# -s

configstrings.h: ../catalogs/DOpus4_Config.cd configstrings_h.sd
	$(FLEXCAT) /catalogs/DOpus4_Config.cd configstrings.h=configstrings_h.sd

strings.c: ../catalogs/DOpus4_Config.cd configstrings.h strings_c.sd
	$(FLEXCAT) /catalogs/DOpus4_Config.cd strings.c=strings_c.sd

%.o: %.c
	$(CC) -c $*.c $(CCOPTS) -I../include -msmall-code -fomit-frame-pointer -O3 -Wall

%.odebug: %.c
	$(CC) -o $*.odebug -c $*.c $(CCOPTS) -I../include -msmall-code -fomit-frame-pointer -O3 -Wall -DDEBUG

lsprintf.o: lsprintf.asm
	$(ASM) $*.asm $(ASMOPTS)
	@hunk2aout >NIL: $*.o
	@delete $*.o QUIET
	@rename obj.0000.* $*.o

lsprintf.odebug: lsprintf.o
	cp lsprintf.o lsprintf.odebug

chipdata.o: chipdata.asm
	$(ASM) $*.asm $(ASMOPTS)

chipdata.odebug: chipdata.o
	cp chipdata.o chipdata.odebug
