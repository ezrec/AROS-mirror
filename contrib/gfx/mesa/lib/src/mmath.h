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
 * Revision 1.1  2005/01/11 14:58:31  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.1  1998/04/18 05:00:14  brianp
 * added FLOAT_COLOR_TO_UBYTE_COLOR macro
 *
 * Revision 3.0  1998/01/31 20:59:27  brianp
 * initial rev
 *
 */


/*
 * Faster arithmetic functions.  If the FAST_MATH preprocessor symbol is
 * defined on the command line (-DFAST_MATH) then we'll use some (hopefully)
 * faster functions for sqrt(), etc.
 */


#ifndef MMATH_H
#define MMATH_H


#include <math.h>



/*
 * Set the x86 FPU control word to less precision for more speed:
 */
#if defined(__linux__) && defined(__i386__) && defined(FAST_MATH)
#include <i386/fpu_control.h>
#define START_FAST_MATH  __setfpucw(_FPU_SINGLE | _FPU_MASK_IM | _FPU_MASK_DM \
            | _FPU_MASK_ZM | _FPU_MASK_OM | _FPU_MASK_UM | _FPU_MASK_PM);
#define END_FAST_MATH  __setfpucw(_FPU_EXTENDED | _FPU_MASK_IM | _FPU_MASK_DM \
            | _FPU_MASK_ZM | _FPU_MASK_OM | _FPU_MASK_UM | _FPU_MASK_PM);
#else
#define START_FAST_MATH
#define END_FAST_MATH
#endif



/*
 * Float -> Int conversion
 */

#if defined(USE_X86_ASM)
static __inline__ int FloatToInt(float f)
{
   int r;
   __asm__ ("fistpl %0" : "=m" (r) : "t" (f) : "st");
   return r;
}
#else
#define FloatToInt(F) ((int) (F))
#endif



/*
 * Square root
 */

extern float gl_sqrt(float x);

#ifdef FAST_MATH
#  define GL_SQRT(X)  gl_sqrt(X)
#else
#  define GL_SQRT(X)  sqrt(X)
#endif



/*
 * Normalize a 3-element vector to unit length.
 */
#define NORMALIZE_3FV( V )				\
{							\
   GLfloat len;						\
   len = GL_SQRT(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);	\
   if (len>0.0001F) {					\
      len = 1.0F / len;					\
      V[0] *= len;					\
      V[1] *= len;					\
      V[2] *= len;					\
   }							\
}



/*
 * Optimization for:
 * GLfloat f;
 * GLubyte b = FloatToInt(CLAMP(f, 0, 1) * 255)
 */

#if defined(__i386__)
#define USE_IEEE
#endif

#if defined(USE_IEEE) && !defined(DEBUG)

#define IEEE_ONE 0x3f7f0000
#define FLOAT_COLOR_TO_UBYTE_COLOR(b, f)			\
	{							\
	   GLfloat tmp = f + 32768.0F;				\
	   b = ((*(GLuint *)&f >= IEEE_ONE)			\
	       ? (*(GLint *)&f < 0) ? (GLubyte)0 : (GLubyte)255	\
	       : (GLubyte)*(GLuint *)&tmp);			\
	}

#else

#define FLOAT_COLOR_TO_UBYTE_COLOR(b, f)			\
	b = FloatToInt(CLAMP(f, 0.0F, 1.0F) * 255.0F)

#endif



extern void gl_init_math(void);


#endif
