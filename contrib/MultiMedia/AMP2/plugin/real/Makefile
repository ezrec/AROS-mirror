#
# Makefile for REAL plugin (020902)
#

CC=pgcc -V 2.95.3 -warpup
CFLAGS=-Wall -I../../common -O3 -fomit-frame-pointer -ffast-math -mcpu=604e -DMSB_FIRST
LDFLAGS=$(CFLAGS) -Wl,--warn-common
EXE=REAL.AMP

# Objects

OBJS = ../../common/plugin.o \
loadelf.o real.o

all: $(EXE)

clean:
	rm -f *.o $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LDFLAGS)
	elf2exe2 $(EXE) $(EXE)
	copy $(EXE) Work2:AMP2/Plugin/$(EXE)

# Dependencies, common

../../common/plugin.o : ../../common/plugin.c ../../common/plugin.h

# Dependencies, local

real.o : real.c ../../common/plugin.h

# Rules

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

