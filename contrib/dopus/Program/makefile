CC = m68k-amigaos-gcc

OBJS = strings.o about.o arbiter.o archive.o assembly.o buffers.o\
complete.o custom.o cycling.o data.o dirtree.o doerror.o doidcmp.o\
dopusfunctions.o getusage.o icondata.o iffload.o launchexternal.o main.o\
main1.o main2.o main3.o main4.o main5.o main6.o main7.o main8.o main9.o\
main10.o main11.o main12.o main13.o main14.o main15.o main16.o main17.o\
main18.o main19.o main20.o main21.o main22.o parentmulti.o protectdata.o\
remember.o renamedata.o rexx.o screendata.o searchdata.o select.o\
selectdata.o tasks.o view.o popupmenu.o makelinkdata.o chipimage.o

OBJS_DEBUG = $(patsubst %.o,%DBG.o, $(OBJS))

CCOPTS = -m68020 -fbaserel
INC = -I../include
LIBS = -L./ -lm -lpm -lpchg -lasync -ldebug
ASM = Work:Dev/PhxAss/PhxAss
ASMOPTS = INCPATH Include: MACHINE=68020 SYMDEBUG SD 4
FLEXCAT = Work:Dev/FlexCat/FlexCat

APPNAME = ../../DirectoryOpus

All : $(APPNAME).debug $(APPNAME)

$(APPNAME) : $(OBJS)
	$(CC) -o $(APPNAME) $(OBJS) $(CCOPTS) -noixemul -detach $(LIBS) -s

$(APPNAME).debug : $(OBJS_DEBUG)
	$(CC) -o $(APPNAME).debug $(OBJS_DEBUG) $(CCOPTS) -noixemul -detach $(LIBS)
# -s

strings.c: ../catalogs/DOpus4.cd dopusstrings.h strings_c.sd
	$(FLEXCAT) /catalogs/DOpus4.cd strings.c=strings_c.sd

dopusstrings.h: ../catalogs/DOpus4.cd dopusstrings_h.sd
	$(FLEXCAT) /catalogs/DOpus4.cd dopusstrings.h=dopusstrings_h.sd

%.o: %.c
	@touch about.c
	$(CC) -c $*.c $(CCOPTS) -O3 -fomit-frame-pointer $(INC) -DWITHPCHG -Wall

%DBG.o: %.c
	$(CC) -o $*DBG.o -c $*.c $(CCOPTS) -O3 -fomit-frame-pointer $(INC) -DWITHPCHG -DDEBUG -Wall

chipimage.o: chipimage.asm
	$(ASM) $*.asm $(ASMOPTS)

chipimageDBG.o: chipimage.o
	cp chipimage.o chipimageDBG.o
