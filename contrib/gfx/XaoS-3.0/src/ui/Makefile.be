CC     = mwcc
CFLAGS = -I- -I. -I../include -O7 -fp_contract on
LIBS   = 
LFLAGS = 
AR     = ar
RANLIB = : No ranlib necessary for

SRCS = autopilot.c \
       drivers.c \
       uinew.c \
       ui_helper.c \
       play.c \
       keywords.c \
       save.c \
       param.c

OBJS = $(SRCS:.c=.o)

TLIB = ../lib/libui.a


all:	$(TLIB)

$(TLIB):$(OBJS)
	rm -f $@
	$(AR) rc $@ $^
	$(RANLIB) $@ 

keywords.c: keywords.gperf
	gperf -t -p -D -C keywords.gperf > keywords.c

clean:
	rm -f $(TLIB)
	rm -f *.[oas]
	rm -f *~
	rm -f core 

distclean:	clean
