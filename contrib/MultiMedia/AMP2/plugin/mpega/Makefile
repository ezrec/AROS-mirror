#
# Makefile for MPEGA plugin (020111)
#

CC=pgcc -V 2.95.3 -warpup
CFLAGS=-Wall -I../../common -O3 -fomit-frame-pointer -ffast-math -mcpu=604e
LDFLAGS=$(CFLAGS) -Wl,--warn-common
EXE=MPEGAX.AMP

# Objects

OBJS = ../../common/plugin.o \
layer1.o layer2.o layer3.o decode.o decode_2to1.o decode_4to1.o dct64.o tabinit.o mpg123.o

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

dct64.o : dct64.c mpg123.h
decode.o : decode.c mpg123.h
decode_2to1.o : decode_2to1.c mpg123.h
decode_4to1.o : decode_4to1.c mpg123.h
layer1.o : layer1.c mpg123.h getbits.h
layer2.o : layer2.c mpg123.h tables.h getbits.h
layer3.o : layer3.c mpg123.h huffman.h getbits.h get1bit.h
mpg123.o : mpg123.c mpg123.h getbits.h ../../common/plugin.h
tabinit.o : tabinit.c mpg123.h

# Rules

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

