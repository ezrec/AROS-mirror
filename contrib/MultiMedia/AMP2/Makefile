#
# Makefile for AMP2 (030415)
#

CC=pgcc -V 2.95.3 -warpup

CFLAGS=-Wall -O3 -fomit-frame-pointer -ffast-math -mcpu=604e \
-DWORDS_BIGENDIAN -Iparser/dvd/ -Iparser/dvd/dvdnav/ # FIXME: remove include paths

LDFLAGS=$(CFLAGS) -Wl,--warn-common
LIBS=-lamiaudio -lz
VPATH=/ade/ppc-amigaos/lib/

EXE=AMP

# Objects

AMIGAOS=amigaos/aga.o amigaos/cgfx.o amigaos/video.o amigaos/req.o amigaos/font.o

COMMON=common/amp.o common/find_plugin.o common/core.o common/ampio.o

MAIN=main/main.o main/prefs.o main/plugin.o main/types.o main/keyfile.o main/buffer.o \
main/version.o main/mpegio.o main/mount.o

REFRESH=refresh/refresh.o refresh/osd.o refresh/subtitle.o \
refresh/yuv2ham6.o refresh/yuv2ham8.o refresh/yuv2aga.o refresh/yuv2rgb.o \
refresh/yuv2gray.o refresh/yuv2lut.o refresh/yuv2win.o refresh/yuv2overlay.o \
refresh/argb2ham6.o refresh/argb2ham8.o refresh/argb2aga.o refresh/argb2rgb.o \
refresh/argb2gray.o refresh/argb2lut.o refresh/argb2win.o refresh/argb2overlay.o \
refresh/lut2aga.o refresh/lut2rgb.o refresh/lut2gray.o refresh/lut2lut.o \
refresh/lut2win.o refresh/lut2overlay.o

DVD=parser/dvd/dvdnav/decoder.o parser/dvd/dvdnav/dvdnav.o parser/dvd/dvdnav/vm.o \
parser/dvd/dvdnav/vmcmd.o parser/dvd/dvdnav/read_cache.o parser/dvd/dvdnav/navigation.o \
parser/dvd/dvdnav/highlight.o parser/dvd/dvdnav/searching.o parser/dvd/dvdnav/settings.o \
parser/dvd/dvdread/ifo_read.o parser/dvd/dvdread/nav_read.o parser/dvd/dvdread/dvd_udf.o \
parser/dvd/dvdread/dvd_reader.o \
parser/dvd/dvdcss/decss.o parser/dvd/dvdcss/dvd.o parser/dvd/dvdcss/css-auth.o \
parser/dvd/dvdcss/findkey.o \
parser/dvd/dvd_main.o parser/dvd/spu.o

PARSER=parser/rm/rm.o parser/avi/avi.o parser/stream/stream.o parser/mpeg/mpeg.o \
parser/vcd/vcd.o \
parser/qt/quicktime.o parser/qt/atom.o parser/qt/mdat.o parser/qt/moov.o parser/qt/mvhd.o \
parser/qt/matrix.o parser/qt/trak.o parser/qt/stco.o parser/qt/stsz.o parser/qt/stsc.o \
parser/qt/mdia.o parser/qt/tkhd.o parser/qt/edts.o parser/qt/minf.o parser/qt/hdlr.o \
parser/qt/mdhd.o parser/qt/vmhd.o parser/qt/smhd.o parser/qt/dinf.o parser/qt/stbl.o \
parser/qt/stsd.o parser/qt/stts.o parser/qt/stss.o parser/qt/udta.o parser/qt/ctab.o \
parser/qt/elst.o parser/qt/dref.o parser/qt/stsdtable.o parser/qt/util.o parser/qt/qt.o

OBJS=$(MAIN) $(AMIGAOS) $(COMMON) $(REFRESH) $(DVD) $(PARSER)

# source files
SRCFILES=$(OBJS:.o=.c)

# make rules
all: default
ifeq (_depend,$(wildcard _depend))
default: $(EXE)
else
default: depend
endif

clean:
	rm -f amigaos/*.o parser/*/*.o parser/*/*/*.o $(COMMON) main/*.o refresh/*.o $(EXE) _depend

$(EXE): $(OBJS) $(LIBS)
	$(CC) -o $(EXE) $(OBJS) $(LDFLAGS) $(LIBS)
	elf2exe2 $(EXE) $(EXE)
	copy $(EXE) Work2:AMP2/$(EXE)

# Rules

$(AMIGAOS) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

$(PARSER) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

$(COMMON) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

$(MAIN) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

$(REFRESH) : %.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

$(DVD) : %.o : %.c
	$(CC) $(CFLAGS) -DNDEBUG -o $@ -c $*.c

depend:
	$(CC) $(CFLAGS) -MM $(SRCFILES) > _depend

# Dependancy data
ifeq (_depend,$(wildcard _depend))
include _depend
endif

