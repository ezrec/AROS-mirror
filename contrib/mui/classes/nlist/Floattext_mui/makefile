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

Floattext.mui: library.o Floattext.o
	$(CC) $(LDFLAGS) $^ -o $@

library.o: library.c ../common/mccheader.c Floattext_mui.h private.h rev.h

Floattext.o: Floattext.c Floattext_mui.h private.h ../common/mcc_common.h rev.h

#

.PHONY : clean
clean:
	rm -f *.o

.PHONY : install
install:
	cp Floattext.mui MUI:libs/mui/

.PHONY : uninstall
uninstall:
	rm -f MUI:libs/mui/Floattext.mui
