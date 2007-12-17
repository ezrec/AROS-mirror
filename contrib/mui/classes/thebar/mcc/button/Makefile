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
# $Id: Makefile 115 2007-10-31 17:42:31Z damato $
#
#***************************************************************************/

###########################################################################
# This makefile is a very generic one. It tries to identify both, the host
# and the target operating system for which TheBar should be compiled.
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
ifndef (HOST)
  HOST = $(shell uname)
  ifeq ($(HOST), AmigaOS)
    ifeq ($(shell uname -m), powerpc)
      HOST = AmigaOS4
    endif
    ifeq ($(shell uname -m), ppc)
      HOST = AmigaOS4
    endif
  endif
endif

#############################################
# now we find out the target OS for
# which we are going to compile TheBar in case
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
  ifeq ($(HOST), Aros)
    OS = aros
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
FLEX  = flex
FC    = flexcat
EXPR  = expr
CHMOD = chmod
SED   = sed
DATE  = date
RM    = rm -f
RMDIR = rm -rf
MKDIR = mkdir

# override commands for native builds
ifeq ($(HOST), AmigaOS4)
  # AmigaOS4 host
  RM    = delete force
  RMDIR = delete force all
  MKDIR = makedir
  DATE  = gdate
else
ifeq ($(HOST), AmigaOS)
  # AmigaOS3 host
  RM    = delete force
  RMDIR = delete force all
  MKDIR = makedir
else
ifeq ($(HOST), MorphOS)
  # MorphOS host
  RM    = delete force
  RMDIR = delete force all
  MKDIR = makedir
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
MCCTARGET = $(BINDIR)/TheButton.mcc

# Common compiler/linker flags
WARN     = -W -Wall -Wwrite-strings
OPTFLAGS = -O3 -fomit-frame-pointer -funroll-loops
DEBUG    = -DDEBUG -O0
DEBUGSYM = -gstabs
CFLAGS   = -I. -I../../mcp -I../../include $(CPU) $(WARN) $(OPTFLAGS) \
           $(DEBUG) $(DEBUGSYM) -c
LDFLAGS  = $(CPU) $(DEBUGSYM) -nostartfiles
LDLIBS   = -lm

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
  CFLAGS   += -mcrt=$(CRT) -D__USE_INLINE__ -D__NEW_TIMEVAL_DEFINITION_USED__ \
              -DNO_PPCINLINE_STDARG -Wa,-mregnames
  LDFLAGS  += -mcrt=$(CRT)

  # additional object files required
  M68KSTUBS = $(OBJDIR)/mccclass_68k.o

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
ifeq ($(OS), aros)

  ##############################
  # AROS

  # Compiler/link/strip commands
  CC      = i686-aros-gcc
  STRIP   = i686-aros-strip
  OBJDUMP = i686-aros-objdump

  # Compiler/Linker flags
  CPU     =

endif
endif
endif
endif

###########################################################################
# Here starts all stuff that is common for all target platforms and
# hosts.

MCCOBJS = $(OBJDIR)/library.o \
          $(OBJDIR)/brc1.o \
          $(OBJDIR)/build.o \
          $(OBJDIR)/class.o \
          $(OBJDIR)/scale.o \
          $(OBJDIR)/utils.o \
          $(OBJDIR)/Debug.o

#

all: $(BINDIR) $(OBJDIR) $(MCCTARGET)

# make the object directories
$(OBJDIR):
	@printf '\033[33mGenerating $@ directory\033[0m\n'
	@$(MKDIR) $(OBJDIR)


# make the binary directories
$(BINDIR):
	@printf '\033[33mGenerating $@ directory\033[0m\n'
	@$(MKDIR) $(BINDIR)

#

$(OBJDIR)/%.o: %.c
	@printf '\033[32mCompiling $<\033[0m\n'
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/mccclass_68k.o: ../../include/mccclass_68k.c
	@printf '\033[32mCompiling $<\033[0m\n'
	@$(CC) $(CFLAGS) $< -o $@

#

$(MCCTARGET): $(M68KSTUBS) $(MCCOBJS)
	@printf '\033[32mLinking \033[1m$@\033[0m\n'
	@$(CC) $(LDFLAGS) -o $@.debug $(MCCOBJS) $(M68KSTUBS) $(LDLIBS) -Wl,-Map,$@.map
	@$(STRIP) --preserve-dates -R.comment -R.sdata2 -S -o $@ $@.debug

$(OBJDIR)/library.o: library.c ../../include/mccinit.c \
  ../../include/mui/TheBar_mcc.h private.h rev.h

## DEPENDENCY GENERATION ##############

Makefile.depend: ;
	@printf "\033[33mWARNING: Makefile.depend missing. Please run 'make depend'\033[0m\n"

.PHONY: depend
depend:
	@printf '\033[33mGenerating Makefile.depend\033[0m\n'
	@echo '# AUTOGENERATED! DO NOT EDIT!!!' >Makefile.depend
	@$(CC) $(CFLAGS) $(MCCOBJS:$(OBJDIR)/%.o=./%.c) -MM >>Makefile.depend
	@$(SED) -i 's,^\([^$$].*\)\.o: ,$$\(OBJDIR\)/\1.o: ,g' Makefile.depend
	@echo '# AUTOGENERATED! DO NOT EDIT!!!' >>Makefile.depend

#

.PHONY: clean
clean:
	-$(RM) $(MCCTARGET) $(MCCTARGET).debug $(MCCTARGET).map
	-$(RM) $(MCCOBJS) $(M68KSTUBS)

.PHONY: distclean
distclean: clean
	-$(RMDIR) $(OBJDIR)
	-$(RMDIR) $(BINDIR)

## DEPENDENCY INCLUDE #################

-include Makefile.depend
