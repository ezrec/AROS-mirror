CC     = mwcc
CFLAGS = -I- -I. -Iinclude -O7 -fp_contract on -inline auto
LIBS   = -lpng -lz
LFLAGS = 
AR     = ar
RANLIB = : No ranlib necessary for

TLIBS= \
       lib/libui.a \
       lib/libengine.a \
       lib/libutil.a \
       lib/libfilter.a \
       lib/libuibe.a

DIRS= \
       filter \
       engine \
       ui \
       util \
       ui/ui-drv/BeOS

PROG= ../bin/xaos
OBJS = $(SRCS:.c=.o)
RESOURCES = ui/ui-drv/BeOS/XaoS.rsrc

TLIB = 


all default:	$(PROG)

../bin:
	mkdir $@

libs:
	@for name in $(DIRS) ; do $(MAKE) -C $$name -f Makefile.be all; done

$(PROG): ../bin include/config.h include/aconfig.h libs $(RESOURCES)
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ -map $@.xMAP $(TLIBS) $(LIBS)
	copyres $(RESOURCES) $@

include/config.h:	include/config/config.be
	cp include/config/config.be include/config.h
include/aconfig.h:	include/config/aconfig.be
	cp include/config/aconfig.be include/aconfig.h

clean:
	@for name in $(DIRS) ; do $(MAKE) -C $$name -f Makefile.be $@; done
	rm -f include/config.h include/aconfig.h
	rm -f $(PROG) $(PROG).xMAP

distclean:	clean
