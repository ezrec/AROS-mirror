TOP=../../..

include $(TOP)/config/make.cfg

STARTUP = $(LIBDIR)/startup.o
LIBS = -L$(LIBDIR) -larosc -lamiga -laros
DEPLIBS = $(STARTUP) $(LIBDIR)/libarosc.a \
	$(LIBDIR)/libamiga.a $(LIBDIR)/libaros.a
CFLAGS = $(USER_DEFINES) -O2 -Wunused -Wall $(SPECIAL_CFLAGS) $(INCLUDES)

.PHONY : all
.PHONY : clean


all:	setup Jump JumpEd

setup:
	@$(MKDIR) $(BINDIR)/Games

Jump: 	Jump.o $(DEPLIBS)
	@$(ECHO) "Linking Jump..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) Jump.o $(LIBS)

JumpEd: JumpEd.o $(DEPLIBS)
	@$(ECHO) "Linking JumpEd..."
	@$(CC) $(ILDFLAGS) -o $(BINDIR)/Games/$@ $(STARTUP) JumpEd.o $(LIBS)


clean:
	@$(ECHO) "Cleaning Source..."
	@$(RM) -f *.o

Jump.o: 
	@$(ECHO) "Compiling Jump..."
	@$(CC) $(CFLAGS) $(INCL) -c Jump.c

JumpEd.o:
	@$(ECHO) "Compiling JumpEd..."
	@$(CC) $(CFLAGS) $(INCL) -c JumpEd.c
