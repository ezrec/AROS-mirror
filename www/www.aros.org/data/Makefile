
all : .stamp
	@true

.stamp : HTMLcolors.py HTMLgen.py aros.rc gensite imgsize.py \
	    $(wildcard news/*) links.py
	@python gensite
	@touch $@

compileall:
	@python -c "import compileall; compileall.compile_dir('.',0)"

