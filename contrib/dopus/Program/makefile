OBJS = about.o arbiter.o main.o main1.o main2.o main3.o main4.o main5.o main6.o main7.o\
main8.o main9.o main10.o main11.o main12.o main13.o main14.o main15.o main16.o\
main17.o main18.o main19.o main20.o main21.o main22.o\
buffers.o chipimage.o custom.o cycling.o data.o doerror.o doidcmp.o getusage.o\
icondata.o iffload.o rexx.o protectdata.o renamedata.o select.o selectdata.o\
strings.o tasks.o view.o assembly.o screendata.o dopusfunctions.o\
launchexternal.o searchdata.o complete.o remember.o parentmulti.o\
dirtree.o

All : DOpus DOpusRT

DOpus : $(OBJS)
	slink with <<
define __ctype=___ctype
#define ___main=___tinymain
from lib:cback.o $(OBJS)
to DirectoryOpus
lib lib:scm.lib lib:sc.lib lib:amiga.lib pchgr.lib asyncio.lib
noicons
maxhunk 51200
sd
nodebug
<

DOpusRT : DOpusRT.o dopusrtasm.o
	slink with <<
define __ctype=___ctype
from lib:cres.o DOpusRT.o DOpusRTAsm.o
to DOpusRT
lib lib:sc.lib lib:amiga.lib
noicons
sc sd nd
<

.c.o:
	sc $*.c
.asm.o:
	sc:c/asm -isc:include $*.asm
