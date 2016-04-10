CC = gcc
STRIP = strip
CHMOD = chmod
CP = cp
RM = rm

CFLAGS = -O2 -Wall -noixemul
LFLAGS = -lm -noixemul
CDEFS = \
	-D__AMIGADATE__=\"$(shell date "+%d.%m.%Y")\" \
	-D__YEAR__=\"$(shell date "+%Y")\" \
	-DNO_PPCINLINE_STDARG

EXE = MinAD

OBJ = \
	startup.o \
	app.o \
	misc.o \
	main.o \
	listtree.o \
	string.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@.debug $(LFLAGS)
	$(STRIP) --strip-all --strip-unneeded --remove-section .comment $@.debug -o $@
	$(CHMOD) +x $(EXE)

.c.o:
	$(CC) $(CDEFS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(OBJ) $(EXE) $(EXE).debug

#---------------------------------------------------------------------------

DIR_OS3 = 0-REL:project/build/m68k-amigaos/$(EXE)
DIR_MOS = 0-REL:project/build/ppc-morphos/$(EXE)
DIR_SRC = 0-REL:project/source/$(EXE)

copy_cls:
	$(CP) -f ChangeLog $(DIR_OS3)/Docs
	$(CP) -f ChangeLog $(DIR_MOS)/Docs
	$(CP) -f ChangeLog $(DIR_SRC)/Docs

copy_os3:
	$(CP) -f $(EXE) $(DIR_OS3)

copy_mos:
	$(CP) -f $(EXE) $(DIR_MOS)

copy_src:
	$(RM) -f $(DIR_SRC)/Source/*.c $(DIR_SRC)/Source/*.h
	$(CP) -f *.c *.h makefile makefile_os3 $(DIR_SRC)/Source

