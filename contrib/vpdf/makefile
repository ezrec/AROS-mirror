# "show" Makefile

CXX=ppc-morphos-g++-4
CC=ppc-morphos-gcc-4
LD=ppc-morphos-gcc-4
SHELL = /bin/sh

JPEGSHARED = ../../libs/jpeg6b/lib/
FREETYPESHARED  = ../../libs/freetype/library/lib/
ROOTPATH=../../../
APPLICATION    = VPDF
CATALOGS       = polski
CATALOGDIR     = MorphOS/Locale/Catalogs
NEWCATCOMP = $(ROOTPATH)morphoswb/development/tools/catcomp/catcomp.pl

VERBOSE ?= 0
ifneq ($(BUILD_QUIET_COMPILE), y)
	VERBOSE = 0
endif

ifeq ($(wildcard ../showgirls/History),)
   INMORPHOS=n
else
   INMORPHOS=y
endif

ifeq ($(VERBOSE), 0)
	OUT   = @
	ECHO  = @echo
	QUIET = >/dev/null
else
	OUT   = @
	ECHO  = @echo >/dev/null
	QUIET =
endif

#CPPFLAGS = -O3 -Wall -mfused-madd -mmultiple -mcpu=750 -ffast-math -fomit-frame-pointer -noixemul -Isystem -Lsystem -Ifx -Lfx -Imcc -Lmcc -I. -Llib -Ilibjpeg -Llibjpeg -Llibpng -Lexif -Llibz
#CPPFLAGS = -O0 -Wall -mfused-madd -mmultiple -ffast-math -fomit-frame-pointer -noixemul -Isystem -Lsystem -Imcc -Lmcc -I. -Llib

#FONTCONFIGLIB = -lfontconfig
FONTCONFIGINC = -I../../libs/fontconfig/MorphOS/include

CPPFLAGS = -g -O2 -Wno-write-strings -Wall -mfused-madd -mmultiple -mcpu=750 -noixemul -fno-inline-functions -Isystem -Lsystem -Imcc -Lmcc -I. -Llib -Ipoppler/poppler/ -Ipoppler
#CPPFLAGS = -D__MORPHOS_SHAREDLIBS -O2 -Wall -mfused-madd -mmultiple -mcpu=750 -noixemul -fno-inline-functions -Isystem -Lsystem -Ifx -Lfx -Imcc -Lmcc -I. -Llib -Llibpng -Llibjpeg -Lexif -Llibz -Lalbum

ifeq ($(INMORPHOS),y)
   CPPFLAGS += -I../showgirls -I../showgirls/system -I../../libs/cairo/cairo-src/MorphOS/include/cairo -I../../libs/cairo/cairo-src/MorphOS/include -I../../../include  -I../../libs/freetype/library/include ${FONTCONFIGINC} -I../../libs/freetype/include
   CPPFLAGS += -L$(JPEGSHARED) -L$(FREETYPESHARED)
else
   # For building with the public SDK
   CPPFLAGS += -Iinclude -Iinclude/cairo -Lsystem
endif

#selection between shared libs and staticly linnked
OBJS = vpdf.o poppler_io.o poppler.o window.o application.o logger.o settings.o fontcache.o arrowstring_class.o  pagenumberclass.o printer.o locale.o

#selection between shared libs and staticly linnked
CAIRO_LIBS = ${FONTCONFIGLIB} -lcairo
POPPLER_LIBS = -lpoppler -lgoo -lfofi
#LIBS_SHARED = -L../poppler-0.11.1/poppler/.libs/ -lpoppler-cairo -lfontconfig -lcairo -lpoppler -lfontconfig -lfreetype-2.3.8 -lexpat -L../showgirls/system -L../showgirls/mcc -L../lairecvs/morphos/morphoswb/apps/showgirls/system -lsystem -lmuiclasses -lsystem -lmoto -L. -ljpeg_shared -lpng_shared -I../showgirls  -ldebug -lsyscall -ljpeg_shared -lpng_shared -liconv
#LIBS_SHARED = -L../poppler-0.11.1/poppler/.libs/ $(CAIRO_LIBS) -lpoppler -lfontconfig -lfreetypeauto -lexpat -L../showgirls/system -L../showgirls/mcc -L../lairecvs/morphos/morphoswb/apps/showgirls/system -lsystem -lmuiclasses -lsystem -lmoto -L. -ljpeg_shared -lpng_shared -I../showgirls  -ldebug -lsyscall -ljpeg_shared -lpng_shared -liconv
LIBS_SHARED = -Lpoppler/poppler/ -Lpoppler/fofi/ -Lpoppler/goo $(CAIRO_LIBS) -L../showgirls/system -L../showgirls/mcc -L../../libs/fontconfig/MorphOS/lib/libnix -L../../libs/cairo/cairo-src/MorphOS/lib/libnix -L../../libs/freetype/library/lib -L. -I../showgirls -lmuiclasses -lsystem -ldebug
LIBS=$(LIBS_SHARED) $(POPPLER_LIBS) $(CAIRO_LIBS) -lsystem -lfreetypeauto -ljpeg_shared -lstdc++ -lm -ldebug

.PHONY: all install install-iso clean

all: libs version.h  $(OBJS)
	make -C poppler
	make -C mcc
	$(LD) $(CPPFLAGS) -o vpdf.db $(OBJS) $(LIBS)
	ppc-morphos-strip vpdf.db --remove-section=.comment --strip-unneeded -o vpdf
	chmod u+x vpdf

fontcache: fontcache.c
	ppc-morphos-gcc-4.4.4 $(CPPFLAGS) -DSTANDALONE -c fontcache.c
	ppc-morphos-gcc-4.4.4 $(CPPFLAGS) -o fontcache fontcache.o $(LIBS_SHARED) -lfreetypeauto

test: test.o
	ppc-morphos-gcc-4.4.4 $(CPPFLAGS) -o test.exe test.o $(LIBS)

libs:
ifeq ($(INMORPHOS),y)
	make -C ../../libs/cairo
	make -C ../../libs/pixman
	make -C ../../libs/freetype
	make -C ../../libs/jpeg6b
endif

ifeq ($(INMORPHOS),n)
LIBSYSTEMOBJS = $(addprefix system/,altivec.o directory.o dlist.o file.o functions.o init.o memory.o timer.o)

system/libsystem.a: CC=ppc-morphos-gcc
system/libsystem.a: CPPFLAGS=-O2 -Wall -fvec -I. -Iinclude
system/libsystem.a: $(LIBSYSTEMOBJS)
	rm -f $@
	ppc-morphos-ar rcs $@ $^

all: system/libsystem.a
endif

VERSIONDATE	 = `date +%d.%m.%y`
VERSION  = 1
REVISION = 1

version.h: FORCE
	@echo 'updating version.h'
	@echo '#define VERSION  "'${VERSION}'"'  >>version.h.new
	@echo '#define REVISION "'${REVISION}'"' >>version.h.new
	@echo '#define VERSIONDATE  "'${VERSIONDATE}'"'  >>version.h.new
	@# only update when required prevent senseless rebuilds
	@cmp -s version.h version.h.new 2>/dev/null || mv version.h.new version.h
	@rm -f version.h.new


.c.o:
	$(CC) $(CPPFLAGS) -c $< -o $@
.SUFFIXES: .cpp .c .o

clean:
	make -C mcc clean
	make -C poppler clean
	rm -f *.o VPDF VPDF.db VPDF_strings.h

install-iso: all
	mkdir -p $(ISOPATH)Applications/VPDF
	mkdir -p $(ISOPATH)Applications/VPDF/Fonts
	cp -r fonts/* $(ISOPATH)Applications/VPDF/Fonts
	cp vpdf $(ISOPATH)Applications/VPDF/VPDF
	cp vpdf.info $(ISOPATH)Applications/VPDF/VPDF.info
	for i in `find $(ISOPATH)Applications/VPDF/ -name "CVS"`; do rm -rf $$i; done

source: $(APPLICATION)_strings.h
	(cd .. && tar -cf $(SOURCEPATH)vpdf.tar vpdf)

	(cd ../showgirls/system && tar --transform "s,^,vpdf/system/," -rf $(SOURCEPATH)vpdf.tar altivec.h chunky.h data.h directory.h dlist.h file.h functions.h gentexture.h loadpng.h loadsunrast.h memory.h system.h timer.h)
	(cd ../showgirls/system && tar --transform "s,^,vpdf/system/," -rf $(SOURCEPATH)vpdf.tar altivec.c directory.c dlist.c file.c functions.c init.c memory.c timer.c)

source-postgg:
	(cd ../../libs/cairo/cairo-src/MorphOS && make headers lib/libnix/libcairo.a)
	(cd ../../libs/freetype/library && make sdk)
	(cd ../../libs/jpeg6b && make lib/libjpeg_shared.a)

	(cd ../../development/os-include && tar --transform "s,^,vpdf/include/," -rf $(SOURCEPATH)vpdf.tar private/vapor/vapor.h)
	(cd ../../libs/cairo/cairo-src/MorphOS && tar --transform "s,^,vpdf/," -rf $(SOURCEPATH)vpdf.tar include)
	(cd ../../libs/freetype && tar --transform "s,^,vpdf/," -rf $(SOURCEPATH)vpdf.tar include)
	(cd ../../libs/freetype/library && tar --transform "s,^,vpdf/," -rf $(SOURCEPATH)vpdf.tar include)
	(cd ../../development/include && tar --transform "s,^,vpdf/include/," -rf $(SOURCEPATH)vpdf.tar fontconfig)

	(cd ../../libs/cairo/cairo-src/MorphOS/lib/libnix && tar --transform "s,^,vpdf/lib/," -rf $(SOURCEPATH)vpdf.tar libcairo.a)
	(cd ../../libs/freetype/library/lib && tar --transform "s,^,vpdf/lib/," -rf $(SOURCEPATH)vpdf.tar libfreetypeauto.a)
	(cd ../../libs/jpeg6b/lib && tar --transform "s,^,vpdf/lib/," -rf $(SOURCEPATH)vpdf.tar libjpeg_shared.a)

dump:
	objdump --section-headers --all-headers --reloc --disassemble-all --line-numbers vpdf.db >ram:zr.dump

dump-fontcache:
	objdump --section-headers --all-headers --reloc --disassemble-all fontcache >ram:fontcache.dump

dumptest:
	objdump --section-headers --all-headers --reloc --disassemble-all test.exe >ram:test.dump


bz2: all
	rm -f vpdf.bz2
	bzip2 -k vpdf

FORCE:

vpdf.o:		    vpdf.c mcc/classes.h poppler.h version.h locale.h VPDF_strings.h
application.o:	application.c application.h poppler.h
logger.o:		logger.c mcc/classes.h application.h
settings.o:		settings.c mcc/classes.h settings.h
poppler.o:		poppler.cpp poppler.h
poppler_io.o:	poppler_io.cpp poppler_io.h
fontcache.o:	fontcache.c fcstr.c fontcache.h
printer.o:      printer.c mcc/classes.h VPDF_strings.h
pagenumberclass.o: pagenumberclass.c pagenumberclass.h
arrowstring_class.o: arrowstring_class.c arrowstring_class.h
printer.o: 		printer.c VPDF_strings.h


#  locale
#
$(APPLICATION)_strings.h: $(APPLICATION).cd
ifeq ($(INMORPHOS),y)
	$(ECHO) "    $(COL_INFO)[CATCOMP]$(COL_END) $@"
	$(OUT)$(NEWCATCOMP) --no-code --no-block -cd $(APPLICATION).cd --cfile $@
endif

.PHONY: catalogs
catalogs: $(patsubst %,catalogs/%/$(APPLICATION).catalog,$(CATALOGS))

%.catalog: %.ct $(APPLICATION).cd
	$(ECHO) "    $(COL_INFO)[CATCOMP]$(COL_END) $@"
	$(OUT)$(NEWCATCOMP) -u -cd $(APPLICATION).cd -ct $< -o $@


