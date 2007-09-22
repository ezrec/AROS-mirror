CC     = mwcc
CFLAGS = -I- -I. -I../include -O7 -fp_contract on -inline auto
LIBS   = 
LFLAGS = 
AR     = ar
RANLIB = : No ranlib necessary for

SRCS = \
       png.c \
       catalog.c \
       be_thread.c \
       xstdio.c \
       timers.c 

OBJS = $(SRCS:.c=.o)

TLIB = ../lib/libutil.a


all:	$(TLIB)

$(TLIB):$(OBJS)
	rm -f $@
	$(AR) rc $@ $^
	$(RANLIB) $@ 

clean:
	rm -f $(TLIB)
	rm -f *.[oas]
	rm -f *~
	rm -f core 

distclean:	clean
