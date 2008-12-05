#
# GCC makefile
#

SHELL = /bin/sh
CC = gcc

CFLAGS =  -fomit-frame-pointer -I../common/
OPTFLAGS = -O3 -finline-functions -fstrength-reduce
#DBGFLAGS = -g -DWITH_DEBUG

LDFLAGS = -noixemul -nostartfiles

%.o: %.c
	$(CC) $(OPTFLAGS) $(CFLAGS) $(DBGFLAGS) -c $< -o $@

#

all: ./NList_mcc.h NList.mcc

#

NList.mcc: library.o NList_mcc.o NList_mcc0.o NList_mcc1.o NList_mcc2.o NList_mcc3.o NList_mcc4.o NList_mcc5.o NList_mcc6.o move.o cbio.o NList_func.o NList_func2.o NList_func3.o NList_func4.o NList_grp.o NList_img.o NList_img2.o
	$(CC) $(LDFLAGS) $^ -o $@

library.o: library.c ../common/mccheader.c NList_mcc.h private.h

#

./NList_mcc.h: ./NList_mcc.h.orj
	cp -f $^ $@

.PHONY : clean
clean:
	rm -f *.o

.PHONY : install
install:
	cp NList.mcc MUI:libs/mui/

.PHONY : uninstall
uninstall:
	rm -f MUI:libs/mui/NList.mcc
