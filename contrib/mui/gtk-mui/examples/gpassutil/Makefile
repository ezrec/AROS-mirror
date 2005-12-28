#
# small Makefile to build gpassutil for AROS
#
# CC must be AROS cross compiler
#
# Oliver Brunner 2005
#
# $Id: Makefile,v 1.1 2005/12/04 00:50:21 o1i Exp $
#
TARGET=gpass
CFLAGS=-I../.. -I../../include -I../../classes 


OBJS = \
	main.o \
	support.o \
	interface.o \
	callbacks.o 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c

all: $(OBJS) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -L ../.. -lgtk-mui -larosc -lmui -larossupport -lamiga 

clean: 
	rm -f $(OBJS) $(TARGET)

