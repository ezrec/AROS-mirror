#
# Makefile for A52 plugin (020111)
#

CC=pgcc -V 2.95.3 -warpup
CFLAGS=-Wall -I../../common -O3 -fomit-frame-pointer -ffast-math -mcpu=604e
LDFLAGS=$(CFLAGS) -Wl,--warn-common
EXE=A52.AMP

# Objects

OBJS = ../../common/plugin.o \
bitstream.o imdct.o bit_allocate.o parse.o downmix.o a52dec.o

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

a52dec.o : a52dec.c ../../common/inttypes.h ../../common/plugin.h a52.h
bitstream.o : bitstream.c ../../common/inttypes.h a52.h a52_internal.h bitstream.h
bit_allocate.o : bit_allocate.c ../../common/inttypes.h a52.h a52_internal.h
downmix.o : downmix.c ../../common/inttypes.h a52.h a52_internal.h
imdct.o : imdct.c ../../common/inttypes.h a52.h a52_internal.h
parse.o : parse.c ../../common/inttypes.h a52.h a52_internal.h bitstream.h tables.h

# Rules

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

