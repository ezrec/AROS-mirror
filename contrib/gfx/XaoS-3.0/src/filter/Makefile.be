CC     = mwcc
CFLAGS = -I- -I. -I../include -O7 -fp_contract on -inline auto
LIBS   = 
LFLAGS = 
AR     = ar
RANLIB = : No ranlib necessary for

SRCS = image.c \
       palette.c \
       random.c \
       filter.c

OBJS = $(SRCS:.c=.o)

TLIB = ../lib/libfilter.a


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
