# Makefile for demo programs

# Mesa 3-D graphics library
# Version:  2.0
# Copyright (C) 1995-1996  Brian Paul
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


# $Id$

# $Log$
# Revision 1.1  2003/08/09 00:23:34  chodorowski
# Initial revision
#


##### MACROS #####

INCDIR = ../include

GL_LIBS = -L../lib -lMesaaux -lMesatk -lMesaGLU -lMesaGL -lm $(XLIBS)

LIB_DEP = ../lib/$(GL_LIB) ../lib/$(GLU_LIB) ../lib/$(TK_LIB) ../lib/$(AUX_LIB)

ifdef AMIGA
PROGS = bounce gamma gears isosurf osdemo spin test0 texobj reflect winpos
else
PROGS = bounce gamma gears glxdemo glxpixmap isosurf offset osdemo \
	spin test0 texobj xdemo reflect winpos
endif

### TODO: finish new shadow demo


##### RULES #####

.SUFFIXES:
.SUFFIXES: .c

.c: $(LIB_DEP)
	$(CC) -I$(INCDIR) $(CFLAGS) $< $(GL_LIBS) -o $@



##### TARGETS #####

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

realclean:
	-rm $(PROGS)
	-rm *.o *~

targets: $(PROGS)

include ../Make-config

