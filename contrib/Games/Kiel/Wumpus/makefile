TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larosc -lamiga -laros
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean


all:	setup Wumpus

setup:
	@$(MKDIR) $(BINDIR)/Games

Wumpus: 	Wumpus.o $(DEPLIBS)
	@$(ECHO) "Linking Wumpus..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) Wumpus.o $(LIBS)


clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Wumpus.o: 
	@$(ECHO) "Compiling Wumpus..."
	@$(CC) $(CFLAGS) $(INCL) -c Wumpus.c
