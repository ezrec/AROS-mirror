# Top-level makefile for BREXX V2
#
# BREXX interpreter
# Version:  2.0
# Copyright (C) 1991-1998  Vassilis Vlachoudis (V.Vlachoudis@cern.ch)
#
# This interpreter is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

############# INCLUDE #############

include ../make_cnf

############# MACROS ##############

CCOPTIONS = $(CFLAGS) $(GREEK) $(MSDOS) $(RMLAST) -I$(INCDIR)

SOURCES = \
	$(MAIN) \
	rexx.c \
	compile.c \
	expr.c \
	template.c \
	interpre.c \
	nextsymb.c \
	rexxfunc.c \
	builtin.c \
	rxstr.c \
	rxconv.c \
	rxmath.c \
	rxword.c \
	rxfiles.c \
	address.c \
	variable.c \
	trace.c \
	error.c \
	stack.c \
	dqueue.c \
	bintree.c \
	bmem.c \
	$(EXTRA)

OBJECTS = $(SOURCES:.c=.o)

############# RULES ###############

.c.o:
	$(CC) -c $(CCOPTIONS) $<

############# TARGETS #############

default:
	@echo "Specify a target configuration"

$(BINDIR)/$(REXX_EXE):	$(OBJECTS)
	$(CC) -o $@ $(CCOPTIONS) \
		$(OBJECTS) \
		-L$(LIBDIR) -llstr -lm

clean:
	rm -f *.o *.obj

targets: $(BINDIR)/$(REXX_EXE)

