#
# Makefile for ChocolateCastle
#

CC     = gcc
CFLAGS = -W -Wall -noixemul -O2 -Wpointer-arith -Wno-parentheses -fno-strict-aliasing
BUILD = $(shell svnversion)
CFLAGS += -D__NOLIBBASE__ -DUSE_INLINE_STDARG
CFLAGS += -D__SVNVERSION__=\"$(BUILD)\" -Ilibvstring/
LDFLAGS = -Llibvstring/
STRIP = ppc-morphos-strip

#########################################################################

OBJS  = main.o generator.o castleapp.o support.o muigenerator.o methodlist.o
OBJS += editor.o reggenerator.o methodeditor.o logo.o locale.o
LIBS = -lvstring
EXE = ChocolateCastle

#########################################################################

.PHONY: all clean libvstring catalogs

all: startmsg libvstring program debug catalogs
	@echo "Build finished."
	@list $(EXE) lformat "%n %l %t"

dep:
	$(CC) -MM *.c

clean:
	rm -f *.o $(OUTPUT) $(OUTPUT).db
	make -C libvstring/ clean

startmsg:
	@echo "Starting build $(BUILD)."

program: $(OBJS)
	@echo "Linking release executable..."
	@$(CC) $(CFLAGS) -s $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXE).tmp
	@$(STRIP) --strip-unneeded --remove-section .comment $(EXE).tmp -o $(EXE)
	@delete $(EXE).tmp QUIET
	@protect $(EXE) +E

debug: $(OBJS)
	@echo "Linking debug executable..."
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $(EXE).db
	@protect $(EXE).db +E

libvstring:
	make -C libvstring/

catalogs: catalogsmsg polski
	@echo "Done."

catalogsmsg:
	@echo "Compiling catalogs:"

%.o:
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o $@ $<

castleapp.o: castleapp.c support.h methodlist.h generator.h castleapp.h \
 muigenerator.h reggenerator.h locale.h
editor.o: editor.c support.h methodlist.h generator.h editor.h locale.h
generator.o: generator.c generator.h support.h methodlist.h locale.h
locale.o: locale.c locale.h
logo.o: logo.c
main.o: main.c support.h methodlist.h generator.h castleapp.h \
 muigenerator.h reggenerator.h editor.h methodeditor.h logo.h locale.h
methodeditor.o: methodeditor.c methodeditor.h editor.h methodlist.h \
 support.h generator.h locale.h
methodlist.o: methodlist.c methodlist.h support.h generator.h locale.h
muigenerator.o: muigenerator.c muigenerator.h generator.h support.h \
 methodlist.h methodeditor.h editor.h locale.h
reggenerator.o: reggenerator.c reggenerator.h generator.h support.h \
 methodlist.h methodeditor.h editor.h locale.h
support.o: support.c support.h methodlist.h generator.h locale.h

polski: catalogs/ChocolateCastle.cd catalogs/polski.ct
	@echo " " $@
	@mkdir -p catalogs/$@
	@catcomp.pl -cd $< -ct catalogs/$@.ct -o catalogs/$@/ChocolateCastle.catalog

