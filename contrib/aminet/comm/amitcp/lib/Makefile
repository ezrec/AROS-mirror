# Generated automatically from Makefile.in by configure.
srcdir =	.

SHELL =		/bin/sh

# Make a linker library with stubs, if for some reason one can't use
# inline headers. This requires two arguments on "make" commandline:
#
# INCBASE: base name of includes (muimaster for MUI etc).
# LIBBASE: base name of library (mui for MUI etc).

all:		lib$(LIBBASE).a libb/lib$(LIBBASE).a
		$(RM) $(INCBASE).c
		$(RM) $(INCBASE).h

lib$(LIBBASE).a:	$(INCBASE).asm
		$(AWK) -f $(srcdir)/splitasm.awk $(INCBASE).asm
		$(CC) -c *.s
		$(RM) *.s $(INCBASE).asm lib$(LIBBASE).a
		$(AR) r lib$(LIBBASE).a *.o
		$(RANLIB) lib$(LIBBASE).a
		$(RM) *.o

libb/lib$(LIBBASE).a:	$(INCBASE)b.asm
		$(AWK) -f $(srcdir)/splitasm.awk $(INCBASE)b.asm
		$(CC) -c *.s
		if [ -d libb ] ; then true ; else mkdir libb; fi
		$(RM) *.s $(INCBASE)b.asm libb/lib$(LIBBASE).a
		$(AR) r libb/lib$(LIBBASE).a *.o
		$(RANLIB) libb/lib$(LIBBASE).a
		$(RM) *.o

$(INCBASE).asm:	$(INCBASE).c
		$(CC) -O2 -fomit-frame-pointer -S $(INCBASE).c -o $(INCBASE).asm

$(INCBASE)b.asm:	$(INCBASE).c
		$(CC) -O2 -fomit-frame-pointer -fbaserel -S $(INCBASE).c -o $(INCBASE)b.asm

$(INCBASE).c:   $(INCBASE).h
		$(RM) $(INCBASE).c
		grep '#include' $(OS_INC_DIR)/clib/$(INCBASE)_protos.h >$(INCBASE).c
		if [ -f $(srcdir)/../include/proto-src/$(INCBASE).h ]; then \
			grep '#include' $(srcdir)/../include/proto-src/$(INCBASE).h \
			| grep -v '#include.*<clib/' | grep -v '#include.*<inline/' \
			| grep -v '#include.*<proto/' >>$(INCBASE).c; \
		else true; \
		fi
		echo '#include "$(INCBASE).h"' >>$(INCBASE).c

$(INCBASE).h:
		../fd2inline --stubs $(FD_DIR)/$(INCBASE)_lib.fd $(OS_INC_DIR)/clib/$(INCBASE)_protos.h -o $(INCBASE).h

clean:
		$(RM) *.asm *.c *.h *.o *.s

clobber:	clean
		$(RM) *.a libb
