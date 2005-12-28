
TARGET=eurocalc
CFLAGS = -O3 -Wall -I../.. -I../../include -I../../classes


EURO_OBJS = \
	main.o \
	support.o \
	interface.o \
	callbacks.o 

all: $(OBJS) $(EURO_OBJS)
	$(CC) $(EURO_OBJS) $(OBJS) -o $(TARGET) -L../.. -lgtk-mui -larosc -lmui -larossupport -lamiga 

clean: 
	rm -f $(EURO_OBJS) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $*.c


