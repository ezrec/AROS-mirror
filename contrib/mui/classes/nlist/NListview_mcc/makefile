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

NListview.mcc: library.o NListview.o
	$(CC) $(LDFLAGS) $^ -o $@

library.o: library.c ../common/mccheader.c NListview_mcc.h private.h rev.h

NListview.o: NListview.c NListview_mcc.h private.h ../common/mcc_common.h rev.h

#

.PHONY : clean
clean:
	rm -f *.o

.PHONY : install
install:
	cp NListview.mcc MUI:libs/mui/

.PHONY : uninstall
uninstall:
	rm -f MUI:libs/mui/NListview.mcc
