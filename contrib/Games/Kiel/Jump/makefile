CURDIR=apps/Games/Jump
TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larossupport -lamiga -larosc
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused -Wall $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean

all:	setup Jump JumpEd

setup:
	@$(MKDIR) $(BINDIR)/Games

Jump: 	Jump.o $(DEPLIBS)
	@$(ECHO) "Linking $@..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) $@.o $(LIBS)
	@$(ECHO) "Checking $@..."
	@$(CHECKUDS) $(BINDIR)/Games/$@

JumpEd: JumpEd.o $(DEPLIBS)
	@$(ECHO) "Linking $@..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) $@.o $(LIBS)
	@$(ECHO) "Checking $@..."
	@$(CHECKUDS) $(BINDIR)/Games/$@

clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Jump.o: Jump.c ../prec.c Jump.h JumpFile.h
	@$(ECHO) "Compiling Jump..."
	@$(CC) $(CFLAGS) $(INCL) -c Jump.c

JumpEd.o: JumpEd.c ../prec.c JumpEd.h JumpEdFile.h
	@$(ECHO) "Compiling JumpEd..."
	@$(CC) $(CFLAGS) $(INCL) -c JumpEd.c
