OBJS = print.o printreq.o doprint.o data.o printdir.o lsprintf.o strings.o

All : DOpus_Print

DOpus_Print : $(OBJS)
	slink with <<
define __ctype=___ctype
from lib:cres.o $(OBJS)
to DOpus_Print
lib lib:scm.lib lib:sc.lib lib:amiga.lib
noicons
maxhunk 51200
sd
nodebug
<

.c.o:
	sc $*.c
.asm.o:
	sc:c/asm -isc:include $*.asm

