HTMLDIR = ../www

# Tools
SED = sed
RM = rm
GNUPLOT = gnuplot # gnuplot
CONVERT = convert # ImageMagick

all : apache.conf pics .stamp
	@true

.stamp : HTMLcolors.py HTMLgen.py aros.rc gensite imgsize.py \
	    $(wildcard news/*) links.py
	@HTMLDIR=$(HTMLDIR) python gensite
	@touch $@

apache.conf : genconf apache.conf.in site.conf
	./genconf apache.conf.in > $@

site.conf :
	@echo "Copy site.conf.in to site.conf and modify site.conf"
	@echo "to suit your setup."
	@exit 1

pics : $(HTMLDIR)/aros_size.png $(HTMLDIR)/contrib_size.png

$(HTMLDIR)/%_size.png : %.size size2plot
	$(SED) -e "s;@HTMLDIR@;$(HTMLDIR);" \
            -e "s;@SIZEFILE@;$<;" \
	    -e "s;@XAXIS@;$$(python -c "import time; start=time.time()-3600*24*90; print time.strftime ('%d.%m.%Y', time.localtime(start));");" \
            size2plot > size2plot.gp
	$(GNUPLOT) size2plot.gp
	$(CONVERT) $(HTMLDIR)/plot.pbm $@
	$(CONVERT) $(HTMLDIR)/plot.pbm $(basename $@).jpg
	@$(RM) -f size2plot.gp $(HTMLDIR)/plot.pbm

compileall:
	@python -c "import compileall; compileall.compile_dir('.',0)"

