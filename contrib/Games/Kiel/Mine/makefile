CURDIR=apps/Games/Mine
TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larosc -lamiga -larossupport -lamiga
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused -Wall $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean

all:	setup Mine

setup:
	@$(MKDIR) $(BINDIR)/Games

Mine:	Mine.o $(DEPLIBS)
	@$(ECHO) "Linking Mine..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) Mine.o $(LIBS)
	@$(ECHO) "Checking Mine..."
	@$(CHECKUDS) $(BINDIR)/Games/$@


clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Mine.o: Mine.c ../prec.c MineDatei.h MineSpiel.h Mineincl.h
	@$(ECHO) "Compiling Mine..."
	@$(CC) $(CFLAGS) $(INCL) -c Mine.c
