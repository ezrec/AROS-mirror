#
# Makefile for MPEGVX-LO plugin (020111)
#

CC=pgcc -V 2.95.3 -warpup
CFLAGS=-Wall -I../../common -O3 -fomit-frame-pointer -ffast-math -mcpu=604e
LDFLAGS=$(CFLAGS) -Wl,--warn-common
EXE=MPEGVX-LO.AMP

# Objects

OBJS = ../../common/plugin.o \
slice.o idct.o motion_comp.o decode.o header.o mpeg2dec.o

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

decode.o : decode.c ../../common/inttypes.h ../../common/plugin.h mpeg2.h mpeg2_internal.h video_out.h
header.o : header.c ../../common/inttypes.h mpeg2_internal.h
idct.o : idct.c ../../common/inttypes.h mpeg2_internal.h
motion_comp.o : motion_comp.c ../../common/inttypes.h mpeg2_internal.h
mpeg2dec.o : mpeg2dec.c ../../common/inttypes.h ../../common/plugin.h mpeg2.h mpeg2_internal.h video_out.h
slice.o : slice.c ../../common/inttypes.h mpeg2_internal.h vlc.h

# Rules

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

