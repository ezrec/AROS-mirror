OBJECTS = dopuslib.o data.o \
          borders.o dates.o dopus_stuff.o dospaths.o dos_exec.o filereq.o \
          graphics.o gui.o imagery.o language.o \
          listview.o misc.o requesters.o simplerequest.o strings.o wildcard.o \
          menus.o functions2.o functions.o

all: dopuspragmas.h dopus.library

dopuspragmas.h: dopus_lib.fd
	fd2pragma dopus_lib.fd dopuspragmas.h

dopus.library: $(OBJECTS)
	slink with <<
define __ctype=___ctype
from $(OBJECTS)
to dopus.library
lib lib:scm.lib lib:sc.lib
noicons
maxhunk 51200
sc sd
nodebug
<

.c.o:
	sc $*.c
.a.o:
	sc:c/asm -isc:include $*.a
