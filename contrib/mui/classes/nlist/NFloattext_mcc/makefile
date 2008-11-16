#
# GCC makefile
#

SHELL = /bin/sh
CC = gcc

CFLAGS = -O2 -fstrength-reduce -fomit-frame-pointer -I../common/
#-DWITH_DEBUG
LDFLAGS = -noixemul -nostartfiles

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#

NFloattext.mcc: library.o NFloattext.o
	$(CC) $(LDFLAGS) $^ -o $@

library.o: library.c ../common/mccheader.c NFloattext_mcc.h private.h rev.h

NFloattext.o: NFloattext.c NFloattext_mcc.h private.h ../common/mcc_common.h rev.h

#

.PHONY : clean
clean:
	rm -f *.o

.PHONY : install
install:
	cp NFloattext.mcc MUI:libs/mui/

.PHONY : uninstall
uninstall:
	rm -f MUI:libs/mui/NFloattext.mcc
