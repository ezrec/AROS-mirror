TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larosc -lamiga -laros
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean


all:	setup Mine

setup:
	@$(MKDIR) $(BINDIR)/Games

Mine: 	Mine.o $(DEPLIBS)
	@$(ECHO) "Linking Mine..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) Mine.o $(LIBS)


clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Mine.o: 
	@$(ECHO) "Compiling Mine..."
	@$(CC) $(CFLAGS) $(INCL) -c Mine.c
