CURDIR=apps/Games/Quad
TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larossupport -lamiga -larosc
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused -Wall $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean

all:	setup Quad

setup:
	@$(MKDIR) $(BINDIR)/Games

Quad: 	Quad.o $(DEPLIBS)
	@$(ECHO) "Linking $@..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) $@.o $(LIBS)
	@$(ECHO) "Checking $@..."
	@$(CHECKUDS) $(BINDIR)/Games/$@

clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Quad.o: Quad.c ../prec.c Quad.h QuadIncl.h
	@$(ECHO) "Compiling Quad..."
	@$(CC) $(CFLAGS) $(INCL) -c Quad.c
