
all : apache.conf .stamp
	@true

.stamp : HTMLcolors.py HTMLgen.py aros.rc gensite imgsize.py \
	    $(wildcard news/*) links.py
	@python gensite
	@touch $@

apache.conf : genconf apache.conf.in site.conf
	./genconf apache.conf.in > $@

site.conf :
	@echo "Copy site.conf.in to site.conf and modify site.conf"
	@echo "to suit your setup."
	@exit 1

compileall:
	@python -c "import compileall; compileall.compile_dir('.',0)"

