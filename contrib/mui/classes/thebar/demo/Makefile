#/***************************************************************************
#
# TheBar.mcc - Next Generation Toolbar MUI Custom Class
# Copyright (C) 2003-2005 Alfonso Ranieri
# Copyright (C) 2005-2007 by TheBar.mcc Open Source Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# TheBar class Support Site:  http://www.sf.net/projects/thebar
#
# $Id$
#
#***************************************************************************/

###########################################################################
# This makefile is a very generic one. It tries to identify both, the host
# and the target operating system for which YAM should be compiled.
# However, this auto-detection can be easily overridden by directly
# specifying an option on the commandline while calling 'make'.
#
# Example:
#
# # to explicitly compile for AmigaOS3
# > make OS=os3
#
# # to compile for AmigaOS4 but with debugging
# > make OS=os4 DEBUG=
#

#############################################
# find out the HOST operating system
# on which this makefile is run
HOST ?= $(shell uname)
ifeq ($(HOST), AmigaOS)
  ifeq ($(shell uname -m), powerpc)
    HOST = AmigaOS4
  endif
  ifeq ($(shell uname -m), ppc)
    HOST = AmigaOS4
  endif
endif

# if no host is identifed (no uname tool)
# we assume a AmigaOS build
ifeq ($(HOST),)
  HOST = AmigaOS
endif

#############################################
# now we find out the target OS for
# which we are going to compile YAM in case
# the caller didn't yet define OS himself
ifndef (OS)
  ifeq ($(HOST), AmigaOS4)
    OS = os4
  else
  ifeq ($(HOST), AmigaOS)
    OS = os3
  else
  ifeq ($(HOST), MorphOS)
    OS = mos
  else
  ifeq ($(HOST), AROS)
    # now we find out which CPU system aros will be used
    ifeq ($(shell uname -m), powerpc)
      OS = aros-ppc
    endif
    ifeq ($(shell uname -m), ppc)
      OS = aros-ppc
    endif
    ifeq ($(shell uname -m), i386)
      OS = aros-i386
    endif
    ifeq ($(shell uname -m), i686)
      OS = aros-i686
    endif
    ifeq ($(shell uname -m), x86_64)
      OS = aros-x86_64
    endif
  else
    OS = os4
  endif
  endif
  endif
  endif
endif

#############################################
# define common commands we use in this
# makefile. Please note that each of them
# might be overridden on the commandline.

# common commands
FLEX    = flex
FC      = flexcat
EXPR    = expr
DATE    = date
RM      = delete force
RMDIR   = delete force all
MKDIR   = makedir
CHMOD   = protect FLAGS=rwed
CP      = copy
CC      = gcc
STRIP   = strip
OBJDUMP = objdump

# path definitions
CDUP  = /
CDTHIS=

# override some variables for non-native builds (cross-compiler)
ifneq ($(HOST), AmigaOS)
ifneq ($(HOST), AmigaOS4)
ifneq ($(HOST), MorphOS)

  # when we end up here this is either a unix or Aros host
  # so lets use unix kind of commands
  RM    = rm -f
  RMDIR = rm -rf
  MKDIR = mkdir -p
  CHMOD = chmod 755
  CP    = cp -f

  CDUP  = ../
  CDTHIS= ./

endif
endif
endif

###########################################################################
# CPU and DEBUG can be defined outside, defaults to above
# using e.g. "make DEBUG= CPU=-mcpu=603e" produces optimized non-debug
# PPC-603e version
#
# OPTFLAGS are disabled by DEBUG normally!
#
# ignored warnings are:
# none - because we want to compile with -Wall all the time

# Common Directories
OBJDIR = .obj_$(OS)
BINDIR = bin_$(OS)

# target definition
DEMO1TARGET  = $(BINDIR)/demo1

DEMO2TARGET  = $(BINDIR)/demo2

DEMO3TARGET  = $(BINDIR)/demo3

DEMO4TARGET  = $(BINDIR)/demo4

DEMO5TARGET  = $(BINDIR)/demo5

DEMO6TARGET  = $(BINDIR)/demo6

DEMO7TARGET  = $(BINDIR)/demo7

DEMO8TARGET  = $(BINDIR)/demo8

DEMO9TARGET  = $(BINDIR)/demo9

DEMO10TARGET = $(BINDIR)/demo10

# Common compiler/linker flags
WARN     = -W -Wall -Wwrite-strings -Wpointer-arith -Wsign-compare
OPTFLAGS = -O3 -fomit-frame-pointer
DEBUG    = -DDEBUG -O0
DEBUGSYM = -g -gstabs
CFLAGS   = -I. -I../include $(CPU) $(WARN) $(OPTFLAGS) \
           $(DEBUG) $(DEBUGSYM) -c
LDFLAGS  = $(CPU) $(DEBUGSYM)
LDLIBS   =

# different options per target OS
ifeq ($(OS), os4)

  ##############################
  # AmigaOS4

  # Compiler/link/strip commands
  CC      = ppc-amigaos-gcc
  STRIP   = ppc-amigaos-strip
  OBJDUMP = ppc-amigaos-objdump

  # Compiler/Linker flags
  CRT      = newlib
  CPU      = -mcpu=powerpc
  WARN     += -Wdeclaration-after-statement -Wdisabled-optimization
  CFLAGS   += -mcrt=$(CRT) -D__USE_INLINE__ -D__NEW_TIMEVAL_DEFINITION_USED__ \
              -DNO_PPCINLINE_STDARG -Wa,-mregnames
  LDFLAGS  += -mcrt=$(CRT)

else
ifeq ($(OS), os3)

  ##############################
  # AmigaOS3

  # Compiler/link/strip commands
  CC      = m68k-amigaos-gcc
  STRIP   = m68k-amigaos-strip
  OBJDUMP = m68k-amigaos-objdump

  # Compiler/Linker flags
  CPU	    = -m68020-60 -msoft-float
  CFLAGS  += -noixemul -DNO_INLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug -lmui

else
ifeq ($(OS), mos)

  ##############################
  # MorphOS

  # Compiler/link/strip commands
  CC      = ppc-morphos-gcc
  STRIP   = ppc-morphos-strip
  OBJDUMP = ppc-morphos-objdump

  # Compiler/Linker flags
  CPU     = -mcpu=powerpc
  CFLAGS  += -noixemul -DNO_PPCINLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug

else
ifeq ($(OS), aros-i386)

  ##############################
  # AROS (i386)

  ifneq ($(HOST), AROS)
    CC      = i386-aros-gcc
    STRIP   = i386-aros-strip
    OBJDUMP = i386-aros-objdump
  endif

  # Compiler/Linker flags
  CFLAGS += -Wno-pointer-sign -DNO_INLINE_STDARG
  LDLIBS += -larossupport -lamiga -lmui

else
ifeq ($(OS), aros-ppc)

  ##############################
  # AROS (PPC)

  ifneq ($(HOST), AROS)
    CC      = ppc-aros-gcc
    STRIP   = ppc-aros-strip
    OBJDUMP = ppc-aros-objdump
  endif

  # Compiler/Linker flags
  CFLAGS += -Wno-pointer-sign -DNO_INLINE_STDARG
  LDLIBS += -larossupport -lamiga -lmui

else
ifeq ($(OS), aros-x86_64)

  ##############################
  # AROS (x86_64)

  ifneq ($(HOST), AROS)
    CC      = x86_64-aros-gcc
    STRIP   = x86_64-aros-strip
    OBJDUMP = x86_64-aros-objdump
  endif

  # Compiler/Linker flags
  CFLAGS += -Wno-pointer-sign -DNO_INLINE_STDARG
  LDLIBS += -larossupport -lamiga -lmui

endif
endif
endif
endif
endif
endif

###########################################################################
# Here starts all stuff that is common for all target platforms and
# hosts.

DEMO1OBJS  = $(OBJDIR)/demo1.o $(OBJDIR)/vastubs.o

DEMO2OBJS  = $(OBJDIR)/demo2.o $(OBJDIR)/vastubs.o

DEMO3OBJS  = $(OBJDIR)/demo3.o $(OBJDIR)/vastubs.o

DEMO4OBJS  = $(OBJDIR)/demo4.o $(OBJDIR)/vastubs.o

DEMO5OBJS  = $(OBJDIR)/demo5.o $(OBJDIR)/vastubs.o

DEMO6OBJS  = $(OBJDIR)/demo6.o $(OBJDIR)/vastubs.o

DEMO7OBJS  = $(OBJDIR)/demo7.o $(OBJDIR)/vastubs.o

DEMO8OBJS  = $(OBJDIR)/demo8.o $(OBJDIR)/vastubs.o

DEMO9OBJS  = $(OBJDIR)/demo9.o $(OBJDIR)/vastubs.o

DEMO10OBJS = $(OBJDIR)/demo10.o $(OBJDIR)/vastubs.o

#

all: $(BINDIR) $(OBJDIR) $(DEMO1TARGET) $(DEMO2TARGET) $(DEMO3TARGET) $(DEMO4TARGET) $(DEMO5TARGET) $(DEMO6TARGET) $(DEMO7TARGET) $(DEMO8TARGET) $(DEMO9TARGET) $(DEMO10TARGET)

# for making a release we compile ALL target with no debug
release:
	@echo "  CC $<"
	make OS=os4 clean
	make OS=os4 DEBUG=
	@echo "  CC $<"
	make OS=os3 clean
	make OS=os3 DEBUG=
	@echo "  CC $<"
	make OS=mos clean
	make OS=mos DEBUG=
	@echo "  CC $<"
	make OS=aros-i386 clean
	make OS=aros-i386 DEBUG=
	@echo "  CC $<"
	make OS=aros-ppc clean
	make OS=aros-ppc DEBUG=
	@echo "  CC $<"
	make OS=aros-x86_64 clean
	make OS=aros-x86_64 DEBUG=

# make the object directories
$(OBJDIR):
	@echo "  MK $@"
	@$(MKDIR) $(OBJDIR)


# make the binary directories
$(BINDIR):
	@echo "  MK $@"
	@$(MKDIR) $(BINDIR)

#

$(OBJDIR)/%.o: %.c
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $@

#

$(DEMO1TARGET): $(DEMO1OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO1OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO2TARGET): $(DEMO2OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO2OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO3TARGET): $(DEMO3OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO3OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO4TARGET): $(DEMO4OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO4OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO5TARGET): $(DEMO5OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO5OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO6TARGET): $(DEMO6OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO6OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO7TARGET): $(DEMO7OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO7OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO8TARGET): $(DEMO8OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO8OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO9TARGET): $(DEMO9OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO9OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(DEMO10TARGET): $(DEMO10OBJS)
	@echo "  LD $@.debug"
	@$(CC) $(LDFLAGS) -o $@.debug $(DEMO10OBJS) $(LDLIBS) -Wl,-Map,$@.map
	@echo "  LD $@"
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

#

.PHONY: clean
clean:
	-$(RM) $(DEMO1OBJS) $(DEMO1TARGET) $(DEMO1TARGET).debug $(DEMO1TARGET).map
	-$(RM) $(DEMO2OBJS) $(DEMO2TARGET) $(DEMO2TARGET).debug $(DEMO2TARGET).map
	-$(RM) $(DEMO3OBJS) $(DEMO3TARGET) $(DEMO3TARGET).debug $(DEMO3TARGET).map
	-$(RM) $(DEMO4OBJS) $(DEMO4TARGET) $(DEMO4TARGET).debug $(DEMO4TARGET).map
	-$(RM) $(DEMO5OBJS) $(DEMO5TARGET) $(DEMO5TARGET).debug $(DEMO5TARGET).map
	-$(RM) $(DEMO6OBJS) $(DEMO6TARGET) $(DEMO6TARGET).debug $(DEMO6TARGET).map
	-$(RM) $(DEMO7OBJS) $(DEMO7TARGET) $(DEMO7TARGET).debug $(DEMO7TARGET).map
	-$(RM) $(DEMO8OBJS) $(DEMO8TARGET) $(DEMO8TARGET).debug $(DEMO8TARGET).map
	-$(RM) $(DEMO9OBJS) $(DEMO9TARGET) $(DEMO9TARGET).debug $(DEMO9TARGET).map
	-$(RM) $(DEMO10OBJS) $(DEMO10TARGET) $(DEMO10TARGET).debug $(DEMO10TARGET).map

.PHONY: distclean
distclean: clean
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)
