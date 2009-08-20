/*
 * DOS/DJGPP Mesa Utility Toolkit
 * Version:  1.0
 *
 * Copyright (C) 2005  Daniel Borca   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * DANIEL BORCA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#if defined (__AROS__)
#include <GL/glut.h>
#endif

#include "arosmesaglut_intern.h"

extern struct AROSMesaGLUTBase *AMGLInternalBase;

IPTR
_glut_font (IPTR font)
{
#if defined(DEBUG_AROSMESAGLUT)
D(bug("[AMGLUT] In _glut_font(font = %d:%x)\n", font, font));
#endif
   switch ((int)font) {
      case GLUT_STROKE_ROMAN_ID:
         return AMGLInternalBase->_glutStrokeRoman;
      case GLUT_STROKE_MONO_ROMAN_ID:
         return AMGLInternalBase->_glutStrokeMonoRoman;
      case GLUT_BITMAP_9_BY_15_ID:
         return AMGLInternalBase->_glutBitmap9By15;
      case GLUT_BITMAP_8_BY_13_ID:
         return AMGLInternalBase->_glutBitmap8By13;
      case GLUT_BITMAP_TIMES_ROMAN_10_ID:
         return AMGLInternalBase->_glutBitmapTimesRoman10;
      case GLUT_BITMAP_TIMES_ROMAN_24_ID:
         return AMGLInternalBase->_glutBitmapTimesRoman24;
      case GLUT_BITMAP_HELVETICA_10_ID:
         return AMGLInternalBase->_glutBitmapHelvetica10;
      case GLUT_BITMAP_HELVETICA_12_ID:
         return AMGLInternalBase->_glutBitmapHelvetica12;
      case GLUT_BITMAP_HELVETICA_18_ID:
         return AMGLInternalBase->_glutBitmapHelvetica18;
      default:
         if ((font == AMGLInternalBase->_glutStrokeRoman) ||
             (font == AMGLInternalBase->_glutStrokeMonoRoman) ||
             (font == AMGLInternalBase->_glutBitmap9By15) ||
             (font == AMGLInternalBase->_glutBitmap8By13) ||
             (font == AMGLInternalBase->_glutBitmapTimesRoman10) ||
             (font == AMGLInternalBase->_glutBitmapTimesRoman24) ||
             (font == AMGLInternalBase->_glutBitmapHelvetica10) ||
             (font == AMGLInternalBase->_glutBitmapHelvetica12) ||
             (font == AMGLInternalBase->_glutBitmapHelvetica18)) {
            return font;
         }
         _glut_fatal("bad font!");
   }
   return (IPTR)NULL;
}
