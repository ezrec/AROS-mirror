###############################################################################
# The following macros should be defined before this script is
# invoked:
#
# TOP              The toplevel directory of the source tree.  This is the
#                  directory that contains this "Makefile.in" and the
#                  "configure.in" script.
#
# BCC              C Compiler and options for use in building executables that
#                  will run on the platform that is doing the build.
#
# USLEEP           If the target operating system supports the "usleep()" system
#                  call, then define the HAVE_USLEEP macro for all C modules.
#
# THREADSAFE       If you want the SQLite library to be safe for use within a 
#                  multi-threaded program, then define the following macro
#                  appropriately:
#
# THREADLIB        Specify any extra linker options needed to make the library
#                  thread safe
#
# OPTS             Extra compiler command-line options.
#
# EXE              The suffix to add to executable files.  ".exe" for windows
#                  and "" for Unix.
#
# TCC              C Compiler and options for use in building executables that 
#                  will run on the target platform.  This is usually the same
#                  as BCC, unless you are cross-compiling.
#
# AR               Tools used to build a static library.
# RANLIB
#
# TCL_FLAGS        Extra compiler options needed for programs that use the
#                  TCL library.
#
# LIBTCL           Linker options needed to link against the TCL library.
#
# READLINE_FLAGS   Compiler options needed for programs that use the
#                  readline() library.
#
# LIBREADLINE      Linker options needed by programs using readline() must
#                  link against.
#
# ENCODING         "UTF8" or "ISO8859"
#
# Once the macros above are defined, the rest of this make script will
# build the SQLite library and testing tools.
################################################################################

# This is how we compile
#
TCCX = $(TCC) $(OPTS) $(THREADSAFE) $(USLEEP) -I. -I$(TOP)/src


# Source code to the test files.
#
TESTSRC = \
  $(TOP)/src/btree.c \
  $(TOP)/src/date.c \
  $(TOP)/src/func.c \
  $(TOP)/src/os_unix.c \
  $(TOP)/src/os_win.c \
  $(TOP)/src/pager.c \
  $(TOP)/src/pragma.c \
  $(TOP)/src/printf.c \
  $(TOP)/src/test1.c \
  $(TOP)/src/test2.c \
  $(TOP)/src/test3.c \
  $(TOP)/src/test4.c \
  $(TOP)/src/test5.c \
  $(TOP)/src/utf.c \
  $(TOP)/src/util.c \
  $(TOP)/src/vdbe.c \
  $(TOP)/src/md5.c

# Header files used by the VDBE submodule
#
VDBEHDR = \
   $(HDR) \
   $(TOP)/src/vdbeInt.h

# This is the default Makefile target.  The objects listed here
# are what get build when you type just "make" with no arguments.
#
#all:	sqlite3.h libsqlite3.a sqlite3$(EXE)

# Generate the file "last_change" which contains the date of change
# of the most recently modified source code file
#
last_change:	$(FILES)
	cat `for x in $(FILES); do echo "$x".c; done` | grep '$$Id: ' | sort +4 | tail -1 \
          | awk '{print $$5,$$6}' >last_change

libsqlite3.a:	$(LIBOBJ)
	$(AR) libsqlite3.a $(LIBOBJ)
	$(RANLIB) libsqlite3.a

sqlite3$(EXE):	$(TOP)/src/shell.c libsqlite3.a sqlite3.h
	$(TCCX) $(READLINE_FLAGS) -o sqlite3$(EXE) $(TOP)/src/shell.c \
		libsqlite3.a $(LIBREADLINE) $(THREADLIB)

objects: $(LIBOBJ_ORIG)

# This target creates a directory named "tsrc" and fills it with
# copies of all of the C source code and header files needed to
# build on the target system.  Some of the C source code and header
# files are automatically generated.  This target takes care of
# all that automatic generation.
#
target_source:	$(SRC) $(VDBEHDR) opcodes.c keywordhash.h
	rm -rf tsrc
	mkdir tsrc
	cp $(SRC) $(VDBEHDR) tsrc
	rm tsrc/sqlite.h.in tsrc/parse.y
	cp parse.c opcodes.c keywordhash.h tsrc
	cp $(TOP)/sqlite3.def tsrc

testfixture$(EXE):	$(TOP)/src/tclsqlite.c libsqlite3.a $(TESTSRC)
	$(TCCX) $(TCL_FLAGS) -DTCLSH=1 -DSQLITE_TEST=1 -o testfixture$(EXE) \
		$(TESTSRC) $(TOP)/src/tclsqlite.c \
		libsqlite3.a $(LIBTCL) $(THREADLIB)

crashtest:	$(TOP)/src/tclsqlite.c libsqlite3.a $(TESTSRC) $(TOP)/src/os_test.c
	$(TCCX) $(TCL_FLAGS) -DOS_TEST=1 -DTCLSH=1 -DSQLITE_TEST=1 \
		-o crashtest \
		$(TESTSRC) $(TOP)/src/os_test.c $(TOP)/src/tclsqlite.c \
		libsqlite3.a $(LIBTCL) $(THREADLIB)

fulltest:	testfixture$(EXE) sqlite3$(EXE) crashtest
	./testfixture$(EXE) $(TOP)/test/all.test

test:	testfixture$(EXE) sqlite3$(EXE)
	./testfixture$(EXE) $(TOP)/test/quick.test

sqlite3_analyzer$(EXE):	$(TOP)/src/tclsqlite.c libsqlite3.a $(TESTSRC) \
			$(TOP)/tool/spaceanal.tcl
	sed \
	  -e '/^#/d' \
	  -e 's,\\,\\\\,g' \
	  -e 's,",\\",g' \
	  -e 's,^,",' \
	  -e 's,$$,\\n",' \
	  $(TOP)/tool/spaceanal.tcl >spaceanal_tcl.h
	$(TCCX) $(TCL_FLAGS) -DTCLSH=2 -DSQLITE_TEST=1 -o \
 		sqlite3_analyzer$(EXE) $(TESTSRC) $(TOP)/src/tclsqlite.c \
		libsqlite3.a $(LIBTCL) $(THREADLIB)

# Standard install and cleanup targets
#
install:	sqlite3 libsqlite3.a sqlite3.h
	mv sqlite3 /usr/bin
	mv libsqlite3.a /usr/lib
	mv sqlite3.h /usr/include

cleanup:
	rm -f *.o sqlite3 libsqlite3.a sqlite3.h opcodes.*
	rm -f lemon lempar.c parse.* sqlite*.tar.gz mkkeywordhash keywordhash.h
	rm -f $(PUBLISH)
	rm -f *.da *.bb *.bbg gmon.out
	rm -rf tsrc
