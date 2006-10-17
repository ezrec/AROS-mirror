#
# GCC makefile
#

SHELL = /bin/sh
CC = gcc

CFLAGS = -O2 -fstrength-reduce -fomit-frame-pointer -I../common/
# -DWITH_DEBUG
LDFLAGS = -noixemul -nostartfiles

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#

NListviews.mcp: library.o NListviews.o
	$(CC) $(LDFLAGS) $^ -o $@

library.o: library.c ../common/mccheader.c NListviews_mcp.h private.h icon.bh rev.h

NListviews.o: NListviews.c NListviews_mcp.h private.h ../common/mcc_common.h rev.h

#

.PHONY : clean
clean:
	rm -f *.o

.PHONY : install
install:
	cp NListviews.mcp MUI:libs/mui/

.PHONY : uninstall
uninstall:
	rm -f MUI:libs/mui/NListviews.mcp
