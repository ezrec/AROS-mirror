TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larosc -lamiga -laros
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused -Wall $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean


all:	setup Quad

setup:
	@$(MKDIR) $(BINDIR)/Games

Quad: 	Quad.o $(DEPLIBS)
	@$(ECHO) "Linking Quad..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) Quad.o $(LIBS)


clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Quad.o: 
	@$(ECHO) "Compiling Quad..."
	@$(CC) $(CFLAGS) $(INCL) -c Quad.c
