# makefile for complex-number calculator, simple version

CC=	lc
OPTS=	-cs -fi -O
OBJS=	complex.o tree.o memory.o cmath.o math.o symbol.o function.o \
	init.o command.o
LIBS=	myumain.o LIB:gnu.lib LIB:lcmieee.lib LIB:lc.lib
LOPTS=	SC SD ND LIB $(LIBS) VERBOSE

icalc:	$(OBJS)
	blink from LIB:c.o $(OBJS) TO icalc $(LOPTS)

complex.o:	complex.c
	$(CC) $(OPTS) complex.c

complex.c:	complex.y
	bison -d complex.y
	cp complex.tab.c complex.c

tree.o: tree.c complex.tab.h
	$(CC) $(OPTS) tree.c

memory.o: memory.c
	$(CC) $(OPTS) memory.c

cmath.o: cmath.c
	$(CC) $(OPTS) cmath.c

math.o: math.c
	$(CC) $(OPTS) math.c

symbol.o: symbol.c complex.tab.h
	$(CC) $(OPTS) symbol.c

function.o: function.c
	$(CC) $(OPTS) function.c

init.o: init.c complex.tab.h
	$(CC) $(OPTS) init.c

command.o: command.c complex.tab.h
	$(CC) $(OPTS) command.c
