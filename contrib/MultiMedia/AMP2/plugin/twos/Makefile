#
# Makefile for TWOS plugin (020111)
#

CC=pgcc -V 2.95.3 -warpup
CFLAGS=-Wall -I../../common -O3 -fomit-frame-pointer -ffast-math -mcpu=604e
LDFLAGS=$(CFLAGS) -Wl,--warn-common
EXE=TWOS.AMP

# Objects

OBJS = ../../common/plugin.o \
main.o

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

main.o : main.c ../../common/plugin.h

# Rules

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

