OBJS = data.o main.o doiconinfo.o lsprintf.o strings.o

All : DOpus_Icon

DOpus_Icon : $(OBJS)
	slink with <<
define __ctype=___ctype
from lib:cres.o $(OBJS)
to DOpus_Icon
lib lib:scm.lib lib:sc.lib
noicons
maxhunk 51200
sd
nodebug
<

.c.o:
	sc $*.c
.asm.o:
	sc:c/asm -isc:include $*.asm

