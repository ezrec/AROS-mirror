#
# Makefile for libvstring
#

# Commons

CFLAGS = -W -Wall -O2 -Wpointer-arith -Wno-parentheses
CFLAGS += -fno-strict-aliasing -fno-builtin
OBJS  = fmtlen.o fmtnew.o fmtput.o freestrarray.o strarraylen.o strcopy.o strlen.o \
 strnew.o vfmtnew.o strncopy.o fmtnput.o
DOC = libvstring.guide
LIB = libvstring.a

################################################################################

.PHONY: all clean install install-iso doc

all: mos

install: mos

install-iso: mos

clean:
	rm -vf $(OBJS) $(LIB) $(DOC)

mos: CC = ppc-morphos-gcc
mos: AR = ppc-morphos-ar
mos: CFLAGS += -DUSE_INLINE_STDARG
mos: $(LIB)

sdk:
	@echo "*** No autodocs here."

################################################################################

$(LIB): $(OBJS)
	@echo "Library merging..."
	@$(AR) rs $@ $(OBJS)
	@List "#?.a" LFORMAT "%n %l"

%.o: %.c libvstring.h
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o $@ $<
