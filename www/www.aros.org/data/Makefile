
BASEDIR=$(shell cd .. ; pwd)
-include .Makefile.inc

# Tools
SED = sed
RM = rm -f
CP = cp
NOP = true # A command which does nothing
ALLOW_READ = chmod -R a+rX,go-w
MKDIR = mkdir -p

# Collect files
DEVPICS := $(subst $(BINDIR)/developers/,$(HTMLDIR)/pics/developers/,$(wildcard $(BINDIR)/developers/*.*))
SCREENSHOTS := $(subst $(BINDIR)/screenshots/,$(SCREENSHOTDIR)/,$(wildcard $(BINDIR)/screenshots/*.* $(BINDIR)/screenshots/[0-9]*/*.* $(BINDIR)/screenshots/[0-9]*/README))
SCREENSHOTDIRS := $(subst $(BINDIR)/screenshots/,$(SCREENSHOTDIR)/,$(wildcard $(BINDIR)/screenshots/[0-9]*))

all : pics makedirs check-html .stamp apache.conf
	@$(NOP)

# These two rules will make sure that gensite is run if one of the
# files gensite creates is missing.
check-html : $(HTMLDIR)/documentation.html $(HTMLDIR)/download.html \
	    $(HTMLDIR)/index.html $(HTMLDIR)/links.html \
	    $(HTMLDIR)/oldnews.html $(HTMLDIR)/screenshots.html \
	    $(HTMLDIR)/snapshots.html $(HTMLDIR)/status.html

$(HTMLDIR)/%.html :
	@echo "$@ is missing."
	$(RM) .stamp

# Run gensite if .stamp is missing or if one of the source files
# is newer.
.stamp : HTMLcolors.py HTMLgen.py aros.rc gensite imgsize.py \
	    config.py htmlautodocs.py mainlinks.py news.py status.py \
	    util.py code2html.py faq2html.py \
	    contents2html.py xml2html.py \
	    $(AROSDIR)/docs/src/contents.py \
	    $(AROSDIR)/docs/src/xmlsupport.py \
	    $(AROSDIR)/docs/src/abbreviations.py \
	    $(AROSDIR)/docs/src/credits.py \
	    $(AROSDIR)/docs/src/genadocs.py \
	    $(AROSDIR)/docs/src/archtool.py \
	    $(AROSDIR)/docs/src/faq.py \
	    $(AROSDIR)/docs/src/jobs.py \
	    $(wildcard $(AROSDIR)/docs/src/*.xml) \
	    $(wildcard $(AROSDIR)/docs/faq/[0-9]*) \
	    $(wildcard news/*) links.py $(DEVPICS) $(SCREENSHOTS)
	@python gensite
	@touch $@

apache.conf : genconf apache.conf.in site.conf
	./genconf apache.conf.in $@
	@echo "Please read the instructions at the beginning of apache.conf"
	@echo "to change the config of Apache accordingly."

site.conf : site.conf.in
	@if [ ! -f "$@" ]; then \
	    echo "Copy site.conf.in to site.conf and modify site.conf" ; \
	    echo "to suit your setup." ; \
	else \
	    echo "site.conf.in has changed. Please update site.conf." ; \
	fi
	@exit 1

ifdef GNUPLOT
pics : $(HTMLDIR)/aros_size.png $(HTMLDIR)/contrib_size.png

$(HTMLDIR)/%_size.png : %.size size2plot.py
	python size2plot.py $(subst _size.png,,$@) $<
ifdef CONVERT
	$(CONVERT) $@ $(basename $@).jpg
	$(CONVERT) $(subst _size.png,_allsize.png,$@) $(subst _size.png,_allsize.jpg,$@)
endif
else
pics :
	@$(NOP)
endif

makedirs :
	@$(MKDIR) $(HTMLDIR)/pics/developers $(HTMLDIR)/pics/screenshots \
	    $(SCREENSHOTDIRS)

$(HTMLDIR)/pics/developers/% : $(BINDIR)/developers/%
	$(CP) $< $@
	$(ALLOW_READ) $@

$(HTMLDIR)/pics/screenshots/% : $(BINDIR)/screenshots/%
	$(CP) $< $@
	$(ALLOW_READ) $@

compileall:
	@python -c "import compileall; compileall.compile_dir('.',0)"

clean :
	$(RM) size2plot.gp $(HTMLDIR)/plot.pbm apache.conf.in \
	    *.pyc .Makefile.inc config.py

.Makefile.inc : site.conf
	$(SED) -e 's;\$$\([a-zA-Z_]*\);$$(\1);' site.conf > $@

config.py : site.conf
	$(SED) -e 's;\([a-zA-Z_]*=\)\(.*$$\);\1"\2";' site.conf > $@

lxr :
	echo $(BASEDIR)
	$(RM) -rf $(BASEDIR)/lxr/source
	$(MKDIR) $(BASEDIR)/lxr/source
	$(CP) -r $(AROSDIR)/bin/*/AROS/Include $(BASEDIR)/lxr/source/include
	$(CP) -r $(AROSDIR)/config $(AROSDIR)/rom $(AROSDIR)/workbench \
		$(BASEDIR)/lxr/source
	$(ALLOW_READ) $(BASEDIR)/lxr/source
	find $(BASEDIR)/lxr/source -name CVS -prune -exec $(RM) -rf "{}" \;
	cd $(BASEDIR)/lxr ; ./bin/genxref $(BASEDIR)/lxr/source
	$(ALLOW_READ) $(BASEDIR)/lxr/fileidx $(BASEDIR)/lxr/xref
