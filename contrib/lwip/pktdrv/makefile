CFLAGS=-g -Wall -O2
CC = ../../../bin/linux-i386/tools/i386-linux-aros-gcc
libpktdrv.a: pktdrv.o
	ar rs libpktdrv.a pktdrv.o

pktdrv.o: pktdrv.c pktdrv.h
	$(CC) $(CFLAGS) -c pktdrv.c

test: libpktdrv.a test.o
	$(CC) $(CFLAGS) -o test.exe test.o -lpktdrv -L.

test.o: test.c pktdrv.h
	$(CC) $(CFLAGS) -c test.c

clean:
	rm -f *.o
	rm -f *.exe
	rm -f pktasm.h
	rm -f libpktdrv.a
