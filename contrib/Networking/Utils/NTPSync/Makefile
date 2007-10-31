CCOS4	= vc +aosppc
CCMOS	= vc +morphos
CC68K	= vc +aos68k
COPTS	= -O1
AMITCP  = -I:SDK/AmiTCP/netinclude
ROADSHOW= -I:SDK/OS4/Include/netinclude
LDFLAGS	= -lauto -lamiga


all: NTPsync_os4 NTPsync_mos NTPsync_68k

NTPsync_os4: NTPsync.c
	$(CCOS4) -o $@ -D__USE_INLINE__ $(COPTS) $(ROADSHOW) NTPsync.c $(LDFLAGS)

NTPsync_mos: NTPsync.c
	$(CCMOS) -o $@ $(COPTS) $(AMITCP) NTPsync.c $(LDFLAGS)

NTPsync_68k: NTPsync.c
	$(CC68K) -o $@ $(COPTS) $(AMITCP) NTPsync.c $(LDFLAGS)

clean:
	@delete force quiet NTPsync_os4 NTPsync_mos NTPsync_68k
