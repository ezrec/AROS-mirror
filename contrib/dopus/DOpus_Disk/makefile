OBJS = diskop.o data.o lsprintf.o format.o strings.o diskcopy.o install.o

All : DOpus_Disk

DOpus_Disk : $(OBJS)
	slink with <<
define __ctype=___ctype
from lib:cres.o $(OBJS)
to DOpus_Disk
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

