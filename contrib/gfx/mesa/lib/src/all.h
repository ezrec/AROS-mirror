/* $Id$ */

/*
 * Mesa 3-D graphics library
 * Version:  3.0
 * Copyright (C) 1995-1998  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log$
 * Revision 1.1  2005/01/11 14:58:30  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.1  1998/07/08 00:39:38  brianp
 * added zoom.h and quads.h
 *
 * Revision 3.0  1998/01/31 20:44:03  brianp
 * initial rev
 *
 */


/* The purpose of this file is to collect all the header files that Mesa
 * uses into a single header so that we can get new compilers that support
 * pre-compiled headers to compile much faster.
 * All we do is list all the internal headers used by Mesa in this one
 * main header file, and most compilers will pre-compile all these headers
 * and use them over and over again for each source module. This makes a
 * big difference for Win32 support, because the <windows.h> headers take
 * a *long* time to compile.
 */


#ifndef SRC_ALL_H
#define SRC_ALL_H


#ifndef PC_HEADER
  This is an error.  all.h should be included only if PC_HEADER is defined.
#endif


#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "GL/gl.h"
#include "GL/osmesa.h"
#include "accum.h"
#include "alpha.h"
#include "alphabuf.h"
#include "api.h"
#include "asm-386.h"
#include "attrib.h"
#include "bitmap.h"
#include "blend.h"
#include "clip.h"
#include "colortab.h"
#include "context.h"
#include "config.h"
#include "copypix.h"
#include "dd.h"
#include "depth.h"
#include "dlist.h"
#include "drawpix.h"
#include "enable.h"
#include "eval.h"
#include "feedback.h"
#include "fixed.h"
#include "fog.h"
#include "get.h"
#include "hash.h"
#include "image.h"
#include "light.h"
#include "lines.h"
#include "logic.h"
#include "macros.h"
#include "masking.h"
#include "matrix.h"
#include "misc.h"
#include "mmath.h"
#include "pb.h"
#include "pixel.h"
#include "pointers.h"
#include "points.h"
#include "polygon.h"
#include "quads.h"
#include "rastpos.h"
#include "readpix.h"
#include "rect.h"
#include "scissor.h"
#include "shade.h"
#include "span.h"
#include "stencil.h"
#include "teximage.h"
#include "texobj.h"
#include "texstate.h"
#include "texture.h"
#include "triangle.h"
#include "types.h"
#include "varray.h"
#include "vb.h"
#include "vbfill.h"
#include "vbrender.h"
#include "vbxform.h"
#include "winpos.h"
#include "xform.h"
#include "zoom.h"

#endif /*SRC_ALL_H*/
