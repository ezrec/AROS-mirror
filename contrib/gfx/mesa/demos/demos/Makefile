# Makefile for demo programs

# Mesa 3-D graphics library
# Version:  2.3
# Copyright (C) 1995-1997  Brian Paul
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



##### MACROS #####

INCDIR = ../include

GL_LIBS = -L../lib -lMesaaux -lMesatk -lMesaGLU -lMesaGL -lm $(XLIBS)

LIB_DEP = ../lib/$(GL_LIB) ../lib/$(GLU_LIB) ../lib/$(TK_LIB) ../lib/$(AUX_LIB)

PROGS = bounce gamma gears glxdemo glxpixmap isosurf morph3d offset osdemo \
	reflect spin stex3d test0 texobj winpos xdemo

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

# execute all programs
exec: $(PROGS)
	@for prog in $(PROGS) ;			\
	do					\
		echo -n "Running $$prog ..." ;	\
		$$prog ;			\
		echo ;				\
	done


include ../Make-config

