CURDIR=apps/Games/Wumpus
TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larossupport -lamiga -larosc
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused -Wall $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean

all:	setup Wumpus

setup:
	@$(MKDIR) $(BINDIR)/Games

Wumpus:	Wumpus.o $(DEPLIBS)
	@$(ECHO) "Linking $@..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) $@.o $(LIBS)
	@$(ECHO) "Checking $@..."
	@$(CHECKUDS) $(BINDIR)/Games/$@

clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Wumpus.o: Wumpus.c ../prec.c WumpusIncl.h
	@$(ECHO) "Compiling Wumpus..."
	@$(CC) $(CFLAGS) $(INCL) -c Wumpus.c
