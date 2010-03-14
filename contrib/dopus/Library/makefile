ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = INCPATH Include: M=68020 SYMDEBUG

CC = m68k-amigaos-gcc
CCOPTS = -m68020 -fbaserel

OBJS = dopuslib.o borders.o dates.o dopus_stuff.o dospaths.o dos_exec.o \
filereq.o gui.o imagery.o language.o listview.o misc.o requesters.o \
simplerequest.o strings.o wildcard.o menus.o functions.o data.o

OBJS_DEBUG = $(patsubst %.o, %.odebug, $(OBJS))

FLEXCAT = Work:Dev/FlexCat/FlexCat

APPNAME = ../../dopus.library

All : $(APPNAME) $(APPNAME).debug

$(APPNAME): $(OBJS)
	$(CC) -o $(APPNAME) -nostartfiles $(OBJS) -noixemul $(CCOPTS) -lamiga -s

$(APPNAME).debug : $(OBJS_DEBUG)
	$(CC) -o $(APPNAME).debug -nostartfiles $(OBJS_DEBUG) -noixemul $(CCOPTS) -lamiga -ldebug
# -s

dopuslib.h: ../catalogs/dopus4_lib.cd dopuslib_h.sd
	$(FLEXCAT) /catalogs/dopus4_lib.cd dopuslib.h=dopuslib_h.sd

language.c: ../catalogs/dopus4_lib.cd dopuslib.h language_c.sd
	$(FLEXCAT) /catalogs/dopus4_lib.cd language.c=language_c.sd

%.o: %.c dopuslib.h
	$(CC) -c $*.c $(CCOPTS) -I../include -Os -fomit-frame-pointer -msmall-code -Wall

%.odebug: %.c dopuslib.h
	$(CC) -o $*.odebug -c $*.c $(CCOPTS) -I../include -Os -fomit-frame-pointer -msmall-code -Wall -DDEBUG

data.o: data.a
	$(ASM) data.a $(ASMOPTS)

data.odebug: data.o
	cp data.o data.odebug

.a.o:
	$(ASM) $*.a $(ASMOPTS)
	hunk2aout >NIL: $*.o
	delete $*.o QUIET
	rename obj.0000.* $*.o
