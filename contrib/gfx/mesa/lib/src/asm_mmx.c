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
 * Revision 1.1  1998/03/27 04:40:07  brianp
 * Initial revision
 *
 */


#ifdef MMX


void gl_mmx_blend_transparency( GLcontext *ctx, GLuint n, const GLubyte mask[],
                                GLubyte rgba[][4], const GLubyte dest[][4] )
{

  long long SUBMASK  =0x00ff000000ff0000LL;
  long long HIGHMASK =0xFFFFFFFF00000000LL;
  long long LOWMASK  =0x00000000FFFFFFFFLL;
  
   GLuint i;

   ASSERT(ctx->Color.BlendEquation==GL_FUNC_ADD_EXT);
   ASSERT(ctx->Color.BlendSrc==GL_SRC_ALPHA);
   ASSERT(ctx->Color.BlendDst==GL_ONE_MINUS_SRC_ALPHA);

   if(((unsigned long)rgba&4 + (unsigned long)dest&4 ) == 8) {
     GLint t = rgba[0][ACOMP];  /* t in [0,255] */
     GLint s = 255 - t;
     GLint r = (rgba[0][RCOMP] * t + dest[0][RCOMP] * s) >> 8;
     GLint g = (rgba[0][GCOMP] * t + dest[0][GCOMP] * s) >> 8;
     GLint b = (rgba[0][BCOMP] * t + dest[0][BCOMP] * s) >> 8;
     GLint a = (rgba[0][ACOMP] * t + dest[0][ACOMP] * s) >> 8;
     rgba[0][RCOMP] = r;
     rgba[0][GCOMP] = g;
     rgba[0][BCOMP] = b;
     rgba[0][ACOMP] = a;

     mask++;
     rgba++;
     dest++;
     n--;
     /*puts("odd adress!");*/
   }
   
   if(n) {
     for (i=0;i<n/2;i++) {
       if (mask[i*2]) {

	 __asm__ (
		  "movq (%0), %%mm4\n\t"  /* mm4 = rgba[i] rgba[i+1]*/
		  "pxor %%mm5,%%mm5\n\t"

		  "movq %%mm4, %%mm1\n\t"

		  "movq (%1), %%mm7\n\t"  /* mm7 = dest[i] dest[i+1] */

		  "punpcklbw %%mm5, %%mm1\n\t" /* mm1 = rgba[i] */

		  "movq %%mm7, %%mm6\n\t"		  
		  
		  "movq %%mm1, %%mm0\n\t"

		  "punpcklbw %%mm5, %%mm6\n\t" /* mm6 = dest[i] */

		  "movq %%mm1, %%mm2\n\t"      /* mm2 = rgba[i]  */

		  "psrlq  $48, %%mm0\n\t"        /* mm0 = mm0 & 0xFF  */
		  
		  "punpckhbw %%mm5, %%mm4\n\t" /* mm4 = rgba[i+1] */

		  "packssdw  %%mm0, %%mm0\n\t" /* mm0 = 0 t 0 t  */
		  
		  "movq %%mm0, %%mm3\n\t"		  

		  "punpckhbw %%mm5, %%mm7\n\t" /* mm7 = dest[i+1] */

		  "psllq $16, %%mm3\n\t"        /* mm3 = t 0 t 0  */

		  "por %3, %%mm0\n\t"        /* mm0 = 255 t 255 t  */	  

		  "punpcklwd %%mm6, %%mm1\n\t" /* mm1 = rgba[i][RCOMP] dest[i][RCOMP] rgba[i][GCOMP] dest[i][GCOMP] */

		  "psubw %%mm3, %%mm0\n\t"     /* mm0 = s[i]           t[i]           s[i]           t[i]*/
		  
		  "punpckhwd %%mm6, %%mm2\n\t" /* mm2 = rgba[i][BCOMP] dest[i][BCOMP] rgba[i][ACOMP] dest[i][ACOMP] */

		  "movq %%mm4, %%mm3\n\t"
		  		  		  
		  "psrlq  $48, %%mm3\n\t"        /* mm3 = mm3 >> 48 */

		  "packssdw  %%mm3, %%mm3\n\t" /* mm3 = 0 t 0 t */
		  
		  "movq %%mm3, %%mm6\n\t"
		  
		  "por %3, %%mm3\n\t"        /* mm3 = 255 t 255 t */
		  
		  "psllq $16, %%mm6\n\t"        /* mm3 = t  0  t   0 */
		  
		  "psubw %%mm6, %%mm3\n\t"     /* mm3 = s[i+1]           t[i+1]           s[i+1]           t[i+1] */
		  
		  "movq %%mm4, %%mm5\n\t"      /* mm5 = rgba[i+1] */

		  "punpcklwd %%mm7, %%mm4\n\t" /* mm4 = rgba[i+1][RCOMP] dest[i+1][RCOMP] rgba[i+1][GCOMP] dest[i+1][GCOMP] */
		  "punpckhwd %%mm7, %%mm5\n\t" /* mm5 = rgba[i+1][BCOMP] dest[i+1][BCOMP] rgba[i+1][ACOMP] dest[i+1][ACOMP] */
		  
		  "pmaddwd %%mm0, %%mm1\n\t"

		  "pmaddwd %%mm3, %%mm4\n\t"

		  "pmaddwd %%mm0, %%mm2\n\t"

		  "pmaddwd %%mm3, %%mm5\n\t"
		  
		  "psrld $8, %%mm1\n\t"
		  "psrld $8, %%mm2\n\t"

		  "psrld $8, %%mm4\n\t"
		  "packssdw %%mm2, %%mm1\n\t"  /* mm2 = rgba[i] */

		  "psrld $8, %%mm5\n\t"		  
		  "packuswb %%mm1, %%mm1\n\t"

		  "packssdw %%mm5, %%mm4\n\t"  /* mm5 = rgba[i+1] */
		  "pand %4, %%mm1\n\t"		  
		  
		  "packuswb %%mm4, %%mm4\n\t" 
		  "pand %2, %%mm4\n\t"
		  
		  "por %%mm1, %%mm4\n\t"  /* mm2 = rgba[i] rgba[i+1] */
		  "movq %%mm4, (%0)\n\t"
		  
		  : /* no output */
		  : "r" (rgba[i*2]),  "r" (dest[i*2]), "m" (HIGHMASK), "m" (SUBMASK), "m" (LOWMASK)
		  : "memory" );
	 
       }
     }
     __asm__("emms":::"memory");  
   }

   if(n&1) {
     GLint t = rgba[n-1][ACOMP];  /* t in [0,255] */
     GLint s = 255 - t;
     GLint r = (rgba[n-1][RCOMP] * t + dest[n-1][RCOMP] * s) >> 8;
     GLint g = (rgba[n-1][GCOMP] * t + dest[n-1][GCOMP] * s) >> 8;
     GLint b = (rgba[n-1][BCOMP] * t + dest[n-1][BCOMP] * s) >> 8;
     GLint a = (rgba[n-1][ACOMP] * t + dest[n-1][ACOMP] * s) >> 8;
     rgba[n-1][RCOMP] = r;
     rgba[n-1][GCOMP] = g;
     rgba[n-1][BCOMP] = b;
     rgba[n-1][ACOMP] = a;

     /*puts("odd n!");*/
   }
}


#else


/* some compilers don't accept sources files without code */
void gl_mmx_dummy_function(void)
{
}

#endif
