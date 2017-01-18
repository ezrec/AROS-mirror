CC = i386-aros-gcc
CFLAGS = -Wall -Wno-pointer-sign -g -O0
LDFLAGS =

PROGS = asl dos_readargs_help graphics_bitmap helloworld intuition_appscreen intuition_events \
dos_file exec_rawdofmt graphics_font icon_change intuition_customscreen intuition_refresh \
dos_readargs graphics_area graphics_simple icon_start intuition_easyreq

all : $(PROGS)

asl : asl.c
dos_file : dos_file.c
dos_readargs : dos_readargs.c
dos_readargs_help : dos_readargs_help.c
exec_rawdofmt : exec_rawdofmt.c
graphics_area : graphics_area.c
graphics_bitmap : graphics_bitmap.c
graphics_font : graphics_font.c
graphics_simple : graphics_simple.c
helloworld : helloworld.c
icon_change : icon_change.c
icon_start : icon_start.c
intuition_appscreen : intuition_appscreen.c
intuition_customscreen : intuition_customscreen.c
intuition_easyreq : intuition_easyreq.c
intuition_events : intuition_events.c
intuition_refresh : intuition_refresh.c

clean:
	rm -f $(PROGS)
