

.PHONY: all debug clean do_make do_makedb install

all: build/config.status do_make

build/config.status: configure
	mkdir -p build
	(cd build; ../configure --enable-opi --enable-wordlist --host=ppc-morphos)
	make -C build depend

build/Makefile: build/config.status Makefile.in
	(cd build; ./config.status --file=Makefile)

build/aconf.h: build/config.status aconf.h.in
	(cd build; ./config.status --file=aconf.h)

do_make: build/Makefile build/aconf.h
	make -C build


debug: bdebug/config.status do_makedb

bdebug/config.status: configure
	mkdir -p bdebug
	(cd bdebug; ../configure --enable-opi --enable-wordlist --enable-memdebug)
	make -C bdebug depend

bdebug/Makefile: bdebug/config.status Makefile.in
	(cd bdebug; ./config.status --file=Makefile)

bdebug/aconf.h: bdebug/config.status aconf.h.in
	(cd bdebug; ./config.status --file=aconf.h)

do_makedb: bdebug/Makefile bdebug/aconf.h
	make -C bdebug

clean:
	rm -rf build bdebug
	rm -f $(patsubst %,catalogs/%/$(APPLICATION).catalog,$(BUILT_CATALOGS))


install: all
	mkdir -p /sys/Applications/Apdf
	mkdir -p /sys/Applications/Apdf/Fonts
#	mkdir -p /sys/Applications/Apdf/Catalogs
#	cp -r catalogs/* /sys/Applications/Apdf/Catalogs
	cp -r addons/fonts/* /sys/Applications/Apdf/Fonts
	cp -r addons/Apdf.config /sys/Applications/Apdf
	cp build/apdf/Apdf /sys/Applications/Apdf
	cp build/server/Apdf_604e.module /sys/Applications/Apdf
	cp apdf/Apdf.info /sys/Applications/Apdf
	cp COPYING /sys/Applications/Apdf
	cp doc/Apdf.guide /sys/Applications/Apdf
	cp doc/ATO.readme /sys/Applications/Apdf
	cp doc/Apdf.guide.info /sys/Applications/Apdf/COPYING.info
	cp doc/Apdf.guide.info /sys/Applications/Apdf/ATO.readme.info
	cp doc/Apdf.guide.info /sys/Applications/Apdf/Apdf.guide.info
	cp doc/Apdf.guide.info /sys/Applications/Apdf/test.pdf.info
	cp misc/test.pdf /sys/Applications/Apdf/
	cp misc/test.pdf.info /sys/Applications/Apdf/
	for i in `find /sys/Applications/Apdf/ -name "CVS"`; do rm -rf $$i; done
#	for i in `find /sys/Applications/Apdf/Catalogs -name "*.ct"`; do rm -f $$i; done


install-iso: all
	mkdir -p $(ISOPATH)Applications/Apdf
	mkdir -p $(ISOPATH)Applications/Apdf/Fonts
#	mkdir -p $(ISOPATH)Applications/Apdf/Catalogs
#	cp -r catalogs/* $(ISOPATH)Applications/Apdf/Catalogs
	cp -r addons/fonts/* $(ISOPATH)Applications/Apdf/Fonts
	cp -r addons/Apdf.config $(ISOPATH)Applications/Apdf
	cp build/apdf/Apdf $(ISOPATH)Applications/Apdf
	cp build/server/Apdf_604e.module $(ISOPATH)Applications/Apdf
	cp apdf/Apdf.info $(ISOPATH)Applications/Apdf
	cp COPYING $(ISOPATH)Applications/Apdf
	cp doc/Apdf.guide $(ISOPATH)Applications/Apdf
	cp doc/ATO.readme $(ISOPATH)Applications/Apdf
	cp doc/Apdf.guide.info $(ISOPATH)Applications/Apdf/COPYING.info
	cp doc/Apdf.guide.info $(ISOPATH)Applications/Apdf/ATO.readme.info
	cp doc/Apdf.guide.info $(ISOPATH)Applications/Apdf/Apdf.guide.info
	cp misc/test.pdf $(ISOPATH)Applications/Apdf
	cp misc/test.pdf.info $(ISOPATH)Applications/Apdf
	for i in `find $(ISOPATH)Applications/Apdf/ -name "CVS"`; do rm -rf $$i; done
#	for i in `find $(ISOPATH)Applications/Apdf/Catalogs -name "*.ct"`; do rm -f $$i; done

source:
	(cd .. && tar -cf $(SOURCEPATH)apdf.tar apdf)

# dynamically built catalogs (should be all of them really)

APPLICATION    = Apdf
BUILT_CATALOGS = 

ROOTPATH       = ../../../
NEWCATCOMP     = $(ROOTPATH)morphoswb/development/tools/catcomp/catcomp.pl

.PHONY: dummy_target catalogs
dummy_target: 
	@$(MAKE) --no-print-directory $(APPLICATION) catalogs

catalogs: $(patsubst %,catalogs/%/$(APPLICATION).catalog,$(BUILT_CATALOGS))

%.catalog: %.ct apdf/apdf.cd
	@echo "    [CATCOMP] $@"
	@$(NEWCATCOMP) -u -cd apdf/apdf.cd -ct $< -o $@
