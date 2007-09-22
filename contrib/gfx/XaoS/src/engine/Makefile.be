CC     = mwcc
CFLAGS = -I- -I. -I../include -O7 -fp_contract on -inline auto -sym full
LIBS   = 
LFLAGS = 
AR     = ar
RANLIB = : No ranlib necessary for

SRCS = formulas.c \
       fractal.c  \
       btrace.c \
       palettef.c \
       emboss.c \
       star.c \
       dither.c \
       edge.c \
       edge2.c \
       rotate.c \
       zoom.c \
       blur.c \
       interlace.c \
       itersmall.c \
       stereogram.c \
       subwindow.c \
       plane.c \
       julia.c \
       i386.c

OBJS = $(SRCS:.c=.o)

TLIB = ../lib/libengine.a


all:	$(TLIB)

$(TLIB):$(OBJS)
	rm -f $@
	$(AR) rc $@ $^
	$(RANLIB) $@ 

formulas.o: docalc.c


clean:
	rm -f $(TLIB)
	rm -f *.[oas]
	rm -f *~
	rm -f core 

distclean:	clean
