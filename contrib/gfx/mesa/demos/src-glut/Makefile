# $Id$

# Makefile for GLUT 3.6
#
# NOTICE:  The OpenGL Utility Toolkit (GLUT) distribution contains source
# code published in a book titled "Programming OpenGL for the X Window
# System" (ISBN: 0-201-48359-9) published by Addison-Wesley.  The
# programs and associated files contained in the distribution were
# developed by Mark J. Kilgard and are Copyright 1994, 1995, 1996 by Mark
# J. Kilgard (unless otherwise noted).  The programs are not in the
# public domain, but they are freely distributable without licensing
# fees.  These programs are provided without guarantee or warrantee
# expressed or implied.
#
# GLUT source included with Mesa with permission from Mark Kilgard.


# $Log$
# Revision 1.1  2005/01/11 14:58:28  NicJA
# AROSMesa 3.0
#
# - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
# - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
# - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
#
# Revision 1.1  1997/12/09 01:51:55  brianp
# Initial revision
#


##### MACROS #####

GLUT_MAJOR = 3
GLUT_MINOR = 6

VPATH = RCS

INCDIR = ../include
LIBDIR = ../lib

SOURCES = \
	glut_8x13.c \
	glut_9x15.c \
	glut_bitmap.c \
	glut_bwidth.c \
	glut_cindex.c \
	glut_cmap.c \
	glut_cursor.c \
	glut_dials.c \
	glut_dstr.c \
	glut_event.c \
	glut_ext.c \
	glut_fullscrn.c \
	glut_get.c \
	glut_hel10.c \
	glut_hel12.c \
	glut_hel18.c \
	glut_init.c \
	glut_input.c \
	glut_menu.c \
	glut_mesa.c \
	glut_modifier.c \
	glut_mroman.c \
	glut_overlay.c \
	glut_roman.c \
	glut_shapes.c \
	glut_space.c \
	glut_stroke.c \
	glut_swap.c \
	glut_swidth.c \
	glut_tablet.c \
	glut_teapot.c \
	glut_tr10.c \
	glut_tr24.c \
	glut_util.c \
	glut_vidresize.c \
	glut_warp.c \
	glut_win.c \
	glut_winmisc.c \
	layerutil.c


OBJECTS = $(SOURCES:.c=.o)



##### RULES #####

.c.o:
	$(CC) -c -I$(INCDIR) $(CFLAGS) $<



##### TARGETS #####

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

targets: $(LIBDIR)/$(GLUT_LIB)

# Make the library
$(LIBDIR)/$(GLUT_LIB): $(OBJECTS)
	$(MAKELIB) $(GLUT_LIB) $(GLUT_MAJOR) $(GLUT_MINOR) $(OBJECTS)
	mv $(GLUT_LIB)* $(LIBDIR)

include ../Make-config

include depend



#
# Run 'make dep' to update the dependencies if you change what's included
# by any source file.
# 
dep: $(SOURCES)
	makedepend -fdepend -Y -I../include $(SOURCES)
