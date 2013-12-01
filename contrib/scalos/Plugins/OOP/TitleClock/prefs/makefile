# Makefile for title_clock_prefs

HEADERS = title_clock_prefs.h ../plugin_api.h prefs_file.h screen.h tcp_locale.h cc_locale.h interface.h misc.h requesters.h saveicon.h
CC      = vc
CFLAGS  = -+ -c -O3 -Iinclude: -D__VBCC__
LD      = vlink
LDFLAGS = -Lvlib: -lamiga -lvc -lextra -ldebug
STARTUP = vlib:startup.o

OBJS    = title_clock_prefs.o prefs_file.o screen.o tcp_locale.o interface.o misc.o requesters.o saveicon.o

all: title_clock_prefs

title_clock_prefs:  $(OBJS)
	$(LD) $(STARTUP) $^ $(LDFLAGS) -o $@.debug
	$(LD) $(STARTUP) $^ $(LDFLAGS) -s -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

title_clock_prefs.o: title_clock_prefs.c $(HEADERS)
prefs_file.o: prefs_file.c prefs_file.h
screen.o: screen.c
tcp_locale.o: tcp_locale.c tcp_locale.h cc_locale.h
interface.o: interface.c $(HEADERS)
misc.o: misc.c
requesters.o: requesters.c
saveicon.o: saveicon.c

clean:
	delete title_clock_prefs title_clock_prefs.debug $(OBJS) >NIL:

