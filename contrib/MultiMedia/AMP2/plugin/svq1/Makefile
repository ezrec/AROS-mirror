#
# Makefile for SVQ1 plugin (020831)
#

CC=pgcc -V 2.95.3 -warpup
CFLAGS=-Wall -I../../common -O3 -fomit-frame-pointer -ffast-math -mcpu=604e
LDFLAGS=$(CFLAGS) -Wl,--warn-common
EXE=SVQ1.AMP

# Objects

OBJS = ../../common/plugin.o \
svq1.o

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

svq1.o : svq1.c ../../common/plugin.h

# Rules

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

