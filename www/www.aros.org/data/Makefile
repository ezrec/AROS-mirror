
-include .Makefile.inc

# Tools
SED = sed
RM = rm -f
CP = cp
NOP = true # A command which does nothing
ALLOW_READ = chmod a+r
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
	    config.py \
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

$(HTMLDIR)/%_size.png : %.size size2plot
	$(SED) -e "s;@OUTPUTFILE@;$@;" \
            -e "s;@SIZEFILE@;$<;" \
	    -e "s;@XAXIS@;$$(python -c "import time; start=time.time()-3600*24*90; print time.strftime ('%d.%m.%Y', time.localtime(start));");" \
            size2plot > size2plot.gp
	$(GNUPLOT) size2plot.gp
	$(CONVERT) $@ $(basename $@).jpg
	@$(RM) size2plot.gp
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
