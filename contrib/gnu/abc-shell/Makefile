#
# Makefile for Amiga Bourne Compatible Shell.
#

.PHONY: all clean debug

ifndef (CRT)
CRT = clib2
endif

DEBUG = -DNDEBUG

CC = ppc-amigaos-gcc
STRIP = ppc-amigaos-strip -R.comment $@

CFLAGS = -mcrt=$(CRT) -Wall -Wwrite-strings -O2 $(DEBUG)
LDFLAGS = -mcrt=$(CRT)
CPPFLAGS = -mcrt=$(CRT) -DAMIGA -DPOSIXLY_CORRECT -DHISTORY -DAUTOINIT -D__STDC_VERSION__=199901L

ifeq ($(CRT), newlib)
LIBS = -lunix
else
LIBS = -lnet -lm -lunix
endif

SRCS = amigaos.c alloc.c c_ksh.c c_sh.c c_test.c environ.c eval.c exec.c \
	expr.c history.c io.c jobs.c lex.c main.c misc.c missing.c path.c \
	shf.c sigact.c syn.c table.c trap.c tree.c tty.c var.c version.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: sh
	@echo done.

debug: DEBUG = -ggdb
debug: STRIP = 
debug: all

sh: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
	$(STRIP)

%.o : %.c
	$(CC) -MM -MP $(INCDIRS) $< >$*.d
	$(CC) -c $(CPPFLAGS) -I. $(CFLAGS) $<

clean:
	rm -f sh $(OBJS) $(DEPS) *~

-include $(SRCS:.c=.d)
