
#----------------DEFININTIONS----------------------

# If you don`t have "rad:", or "rad:" is too small. Just change
# "T" to something else (but not "ram:").
# This is the only place "rad:" is defined.

T=rad:

VERSION=0.55
VERSION_LETTER=
DISKVERSION=0.51

PLAYERPRI=40

FROMMAKEFILE=thesmakefile.smk
TOMAKEFILE=$(T)smakefile.smk
MAKEFILE=smakefile.smk

CC=sc
LINKER=slink

AD=Amiga/
CD=common/


#-----------------COMPILING-----------------------------

all: $(TOMAKEFILE)
	smake -f $(TOMAKEFILE)

clean:
	echo >m.o
   delete $(CD)*.o $(CD)new/*.o $(AD)plug-ins/*.o \
          *.o radium0.* $(AD)instrprop/*.o $(AD)*.o \
          mmd2loader/*.o

	echo >m.info
   delete $(CD)*.info $(CD)new/*.info $(AD)plug-ins/*.info \
          *.info $(AD)instrprop/*.info $(AD)*.info \
          mmd2loader/*.info

	echo >m.lnk
   delete $(CD)*.lnk $(CD)new/*.lnk $(AD)plug-ins/*.lnk \
          *.lnk $(AD)instrprop/*.lnk $(AD)*.lnk \
          mmd2loader/*.lnk

	echo >m.dis
   delete $(CD)*.dis $(CD)new/*.dis $(AD)plug-ins/*.dis \
          *.dis $(AD)instrprop/*.dis $(AD)*.dis \
          mmd2loader/*.dis


newfile:
	delete $(T)radplace.m
	smake all

$(TOMAKEFILE): $(FROMMAKEFILE) $(MAKEFILE)
	echo "T=$(T)" >$(TOMAKEFILE)
	echo "VERSION=$(VERSION)" >>$(TOMAKEFILE)
	echo "VERSION_LETTER=$(VERSION_LETTER)" >>$(TOMAKEFILE)
	echo "DISKVERSION=$(DISKVERSION)" >>$(TOMAKEFILE)
	echo "PLAYERPRI=$(PLAYERPRI)" >>$(TOMAKEFILE)
	echo "MAKEFILE=$(TOMAKEFILE)" >>$(TOMAKEFILE)
	echo "CC=$(CC)" >>$(TOMAKEFILE)
	echo "LINKER=$(LINKER)" >>$(TOMAKEFILE)
	echo "AD=$(AD)" >>$(TOMAKEFILE)
	echo "CD=$(CD)" >>$(TOMAKEFILE)
	type $(FROMMAKEFILE) >>$(TOMAKEFILE)

