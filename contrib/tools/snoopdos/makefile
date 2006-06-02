###############################################################################
#
#       Makefile for OS4 using GNU Make and GCC                         vi:ts=4
#
#       SnoopDosPPC 3.8 Copyright (c) Eddy Carroll, 1994. Freely distributable.
#
###############################################################################

CROSS   = ppc-amigaos-
RM      = rm

CC      = $(CROSS)gcc

NAME    = SnoopDos

CFLAGS  = -D__USE_INLINE__ -DFlush=FFlush $(PROFOPT) $(DEBUG)
AFLAGS  = -Wa,-mregnames
LIBS    = 

#
#       Default rules
#
.c.o:
	$(CC) $(CFLAGS) $*.c -c -o $@

.a.o:
	$(CC) $(AFLAGS) underscore $*.a    # Prefix all symbols with C-style _

.s.o:
	$(CC) $(AFLAGS) $*.s -c -o $@

.cd.h:
	catcomp $*.cd cfile $*.h

#
# Makefile dependencies
#

all: $(NAME) TestCalls

OBJS    = buffer.o snoopdos.o language.o settings.o   \
          mainwin.o subwin.o miscwin.o  hotkey.o amigaos4.o patches_ppc.o patchcode_ppc.o #patchcode.o $(PROFILE)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LIBS)

clean:
	$(RM) $(OBJS)

snooptext.h:    snooptext.cd
patchcode.o:    patchcode.s
buffer.o:       buffer.c        snooptext.h     #snoopdos.h
snoopdos.o:     snoopdos.c      snooptext.h     snoopdos.h
language.o:     language.c      snooptext.h     #snoopdos.h
#windows.o:     windows.c       snooptext.h     #snoopdos.h
miscwin.o:      miscwin.c       snooptext.h     #snoopdos.h     gui.h
mainwin.o:      mainwin.c       snooptext.h     #snoopdos.h     gui.h
subwin.o:       subwin.c        snooptext.h     #snoopdos.h     gui.h
settings.o:     settings.c      snooptext.h     #snoopdos.h
hotkey.o:       hotkey.c        snooptext.h     icon.h  #snoopdos.h
patches.o:      patches.c       snooptext.h     #snoopdos.h
amigaos4.o:     amigaos4.c
patches_ppc.o:  patches_ppc.c   snoopdos.h
patchcode_ppc.o: patchcode_ppc.s

TestCalls:      testcalls.o
	$(CC)  testcalls.o -o TestCalls -lauto

testcalls.o:    testcalls.c


blank:
	catcomp snooptext.cd ctfile snooptext.ct-blank

english:
	catcomp snooptext.cd translation snoop-english.ct catalog catalogs/english/snoopdos.catalog nooptim

