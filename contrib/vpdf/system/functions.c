
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <math.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "functions.h"
//#include	  "altivec.h"
//#include	  <altivec.h>

//#define ENABLE_16BIT
#if 0
void bcopy( const void *s1, void *s2, size_t n )
{
    unsigned long m;

	if ( !n ) return;

    if ( s2 < s1 )
    {
        if ( ( long ) s1 & 1 )
        {
			char *cs1 = (char*)s1;
			char *cs2 = (char*)s2;
			*cs2++ = *cs1++;
			s1 = cs1;
			s2 = cs2;
            n--;
        }

        if ( !( ( long ) s2 & 1 ) )
        {
            m = n / sizeof( long );
            n &= sizeof( long ) - 1;

            for ( ; m; m-- )
            {
				long *ls1 = (long*)s1;
				long *ls2 = (long*)s2;
				*ls2++ = *ls1++;
				s1 = ls1;
				s2 = ls2;
            }
        }

        for ( ; n; n-- )
        {
			char *cs1 = (char*)s1;
			char *cs2 = (char*)s2;
			*cs2++ = *cs1++;
			s1 = cs1;
			s2 = cs2;
        }
    }
    else
    {
		s1 = (char*)s1 + n;
		s2 = (char*)s2 + n;

        if ( ( long ) s1 & 1 )
        {
			char *cs1 = (char*)s1;
			char *cs2 = (char*)s2;
			*--cs2 = *--cs1;
			s1 = cs1;
			s2 = cs2;
            n--;
        }

        if ( !( ( long ) s2 & 1 ) )
        {
            m = n / sizeof( long );
            n &= sizeof( long ) - 1;

            for ( ; m; m-- )
            {
				long *ls1 = (long*)s1;
				long *ls2 = (long*)s2;
				*--ls2 = *--ls1;
				s1 = ls1;
				s2 = ls2;
            }
        }

        for ( ; n; n-- )
        {
			char *cs1 = (char*)s1;
			char *cs2 = (char*)s2;
			*--cs2 = *--cs1;
			s1 = cs1;
			s2 = cs2;
        }
    }
}
#endif
#if 0
void *memcpy( void *s1, const void *s2, size_t n )
{
	bcopy( s2, s1, n );

	return s1;
}

void *memmove( void *s1, const void *s2, size_t n )
{
    bcopy( s2, s1, n );

    return s1;
}
#endif

#if 0

/* left here only for historical reasons:) */

void *mymemset( void *s, int c, size_t n )
{
	unsigned char *p = s;

    if ( n != 0 )
    {
		/* if more than 8 bytes to fill (to make it worth the extra effort */

		if ( n > 8 )
		{
			unsigned int lc = c | ( c << 8 ) | ( c << 16 ) | ( c << 24 );
			unsigned int *lp;
			int ln;

			/* align */

			if ( (unsigned int)p & 3 )
			{
				*p++ = c;
				n--;
			}
			if ( (unsigned int)p & 3 )
			{
				*p++ = c;
				n--;
			}
			if ( (unsigned int)p & 3 )
			{
				*p++ = c;
				n--;
			}

			/* fill longwords */

			lp = p;
			ln = n >> 2;

			do
			{
				*lp++ = lc;
			}
			while ( --ln != 0 );

			n = n & 3;
			p = lp;
		}

		/* fill remaining parts */

		if ( n > 0 )
		{
			do
			{
				*p++ = c;
			}
			while ( --n != 0 );
		}
	}

    return s;
}

#endif

/// Some color usefull conversion functions (sources!=destination)

// Source: 24bit RGB

#ifdef ENABLE_16BIT
void convertRGB888_to_RGB565( 	  unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned short *d;

	p = (unsigned char*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	r = (*p++) >> 3;
		int	g = (*p++) >> 2;
		int	b = (*p++) >> 3;

		*d++ = ( r << 11 ) | ( g << 5 ) | b;
    }
}
#endif

void convertRGB888_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int n;
    unsigned char *p;
	unsigned int *d;

	p = (unsigned char*)src;
	d = (unsigned int*)dst;
    n = xsize * ysize;

	if ( !n )
		return;

	do
    {
		unsigned	int	r = *p++;
		unsigned	int	g = *p++;
		unsigned	int	b = *p++;

		*d++ = 0xff000000 | ( r << 16 ) | ( g << 8 ) | b;
	} while ( --n );
}

void convertRGB888_to_RGBA8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int n;
    unsigned char *p;
	unsigned int *d;

	p = (unsigned char*)src;
	d = (unsigned int*)dst;
    n = xsize * ysize;

    if ( !n )
		return;

	do
    {
		unsigned	int	r = *p++;
		unsigned	int	g = *p++;
		unsigned	int	b = *p++;

		*d++ = 0xff | ( r << 24 ) | ( g << 16 ) | ( b << 8 );
	} while (--n);
}

#if 0
static int expandRGB888_to_ARGB8888_av( unsigned char *src, unsigned char *dst,
								int n )
{
	int i;
    unsigned char *p;
	unsigned char *d;

	vector unsigned char vfill = { 0xff, 0, 0, 0, 0xff, 0, 0, 0, 0xff, 0, 0, 0, 0xff, 0, 0, 0 };
	vector unsigned char perm1 = { 16, 0, 1, 2, 16, 3, 4, 5, 16, 6, 7, 8, 16, 9, 10, 11 };	/* no OR needed */
	vector unsigned char perm2 = { 16, 12, 13, 14, 16, 15, 16, 17, 16, 18, 19, 20, 16, 21, 22, 23 }; /* need to OR to insert filler */
	vector unsigned char perm3 = { 16, 24, 25, 26, 16, 27, 28, 29, 16, 30, 31, 0, 16, 1, 2, 3 }; /* need to OR to insert filler */
	vector unsigned char perm4 = { 16, 4, 5, 6, 16, 7, 8, 9, 16, 10, 11, 12, 16, 13, 14, 15 }; /* no OR needec */

	/* limit n to multiply of 16 */

	i = ( n >> 4 ) << 4;
	
	p = (unsigned char*)( src + i * 3 - 48 );
	d = (unsigned char*)( dst + i * 4 - 64 );

	if	( ( (unsigned int)p & 0xf ) != 0 || ( (unsigned int)d & 0xf ) != 0  || n != i )
		return 0;

	n = i >> 4;

    for ( i = 0; i < n; i++ )
    {
		vector unsigned char vp0 = vec_ld( 0, p );	 /* rgbr gbrg brgb rgbr */
		vector unsigned char vp1 = vec_ld( 16, p );	 /* gbrg brgb rgbr gbrg */
		vector unsigned char vp2 = vec_ld( 32, p );	 /* brgb rgbr gbrg brgb */

		/* expanding with permute function */

		vector unsigned char vexp2 = vec_perm( vp0, vp1, perm2 );
		vector unsigned char vexp3 = vec_perm( vp2, vp1, perm3 );
		vector unsigned char vexp1 = vec_perm( vp0, vfill, perm1 );
		vector unsigned char vexp4 = vec_perm( vp2, vfill, perm4 );

		vexp2 = vec_or( vexp2, vfill );
		vexp3 = vec_or( vexp3, vfill );

		vec_st( vexp1, 0, d );
		vec_st( vexp2, 16, d );
		vec_st( vexp3, 32, d );
		vec_st( vexp4, 48, d );

		p-=48;
		d-=64;
    }

	return	n << 4;
}

#endif

void expandRGB888_to_ARGB8888( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int n;
    unsigned char *p;
	unsigned int *d;
	int	done = 0;

    n = xsize * ysize;
	#if 0
	if	( hasAltivec )
	{
		done = expandRGB888_to_ARGB8888_av( src, dst, n );

		/* calculate if there is anything else to expand */

		if	( done == n )
			return;
	}
	#endif
    p = (unsigned char*)( src + n * 3 - 3 );
	d = (unsigned int*)( dst + n * 4 - 4 );
	n -= done;

    if ( !n )
		return;

	do
    {
		unsigned	int	r = p[ 0 ];
		unsigned	int	g = p[ 1 ];
		unsigned	int	b = p[ 2 ];

		d[ 0 ] = 0xff000000 | ( r << 16 ) | ( g << 8 ) | b;

		p-=3;
		d-=1;
	}while(--n);
}

void expandL8_to_ARGB8888( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned int *d;

    n = xsize * ysize;
	p = (unsigned char*)( src + n - 1 );
	d = (unsigned int*)( dst + n * 4 - 4 );

    for ( i = 0; i < n; i++ )
    {
		unsigned	int	l = p[ 0 ];

		d[ 0 ] = 0xff000000 | ( l << 16 ) | ( l << 8 ) | l;

		p-=1;
		d-=1;
    }
}


void expandRGB888_to_RGBA8888( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int n;
    unsigned char *p;
	unsigned int *d;

    n = xsize * ysize;
	p = (unsigned char*)( src + n * 3 - 3 );
	d = (unsigned int*)( dst + n * 4 - 4 );

    if ( !n )
		return;

	do
    {
		unsigned	int	r = p[ 0 ];
		unsigned	int	g = p[ 1 ];
		unsigned	int	b = p[ 2 ];

		d[ 0 ] = 0xff | ( r << 24 ) | ( g << 16 ) | ( b << 8 );

		p-=3;
		d-=1;
	} while(--n);
}

void convertRGB888_to_L8( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	r = *p++;
		int	g = *p++;
		int	b = *p++;


		*d++ = (r + g + b) / 3 ;
    }
}

void convertRGB888_to_A8( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	r = *p++;
		int	g = *p++;
		int	b = *p++;


		*d++ = (r + g + b) / 3 ;
    }
}

// Source:32bit RGBA
#ifdef ENABLE_16BIT
void convertRGBA8888_to_RGBA4444( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned int *p;
	unsigned short *d;

	p = (unsigned int*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned int rgba = *p++;
		int	r = ( rgba & 0xf0000000 ) >> 16;
		int	g = ( rgba & 0x00f00000 ) >> 12;
		int	b = ( rgba & 0x0000f000 ) >> 8;
		int	a = ( rgba & 0x000000f0 ) >> 4;

		*d++ = r | g | b | a;
    }
}

void convertRGBA8888_to_ARGB4444( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned int *p;
	unsigned short *d;

	p = (unsigned int*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned int rgba = *p++;
		int	r = ( rgba & 0xf0000000 ) >> 20;
		int	g = ( rgba & 0x00f00000 ) >> 16;
		int	b = ( rgba & 0x0000f000 ) >> 12;
		int	a = ( rgba & 0x000000f0 ) << 8;

		*d++ = a | r | g | b;
    }

}

void convertRGB888_to_ARGB4444(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned char *p;
	unsigned short *d;

	p = (unsigned char*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	r = ( *p++ ) >> 4;
		int	g = ( *p++ ) >> 4;
		int	b = ( *p++ ) >> 4;

		*d++ = ( 0xf << 12 ) | ( r << 8 ) | ( g << 4 ) | b;
    }

}

void convertARGB8888_to_ARGB4444( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;

	unsigned int *p;
	unsigned short *d;

	p = (unsigned int*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned int argb = *p++;
		int	a = ( argb & 0xf0000000 ) >> 16;
		int	r = ( argb & 0x00f00000 ) >> 12;
		int	g = ( argb & 0x0000f000 ) >> 8;
		int	b = ( argb & 0x000000f0 ) >> 4;

		*d++ = a | r | g | b;
    }

}
#endif

void convertRGBA8888_to_RGB888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

	if ( !n )
		return;

	do
    {
		int	r = *p++;
		int	g = *p++;
		int	b = *p++;
		p++;

		*d++ = r;
		*d++ = g;
		*d++ = b;

	}while (--n);
}

void convertARGB8888_to_RGB888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    if ( !n )
		return;

	do
    {
		int	r = p[ 1 ];
		int	g = p[ 2 ];
		int	b = p[ 3 ];

		*d++ = r;
		*d++ = g;
		*d++ = b;

		p+=4;

	}while(--n);
}

#ifdef ENABLE_16BIT
void convertRGBA8888_to_RGB565(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned short *d;

	p = (unsigned char*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	r = *p++;
		int	g = *p++;
		int	b = *p++;
		p++;

		r = r>>3;
		g = g>>2;
		b = b>>3;

		*d++ = (r<<11) | (g<<5) | b;

    }
}
#endif

void convertRGBA8888_to_ARGB8888( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int	n;
	unsigned int *p;
	unsigned int *d;

	p = (unsigned int*)src;
	d = (unsigned int*)dst;
	n = xsize*ysize;

    if ( !n )
		return;

	do
	{
		unsigned int rgba = *p++;
		unsigned int r = (rgba & 0xff000000);
		unsigned int g = (rgba & 0x00ff0000);
		unsigned int b = (rgba & 0x0000ff00);
		unsigned int a = (rgba & 0x000000ff);

		*d++ = (a<<24) | (r>>8) | (g>>8) | (b>>8);
	}while(--n);
}

void convertARGB8888_to_RGBA8888( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int	n;
	unsigned int *p;
	unsigned int *d;

	p = (unsigned int*)src;
	d = (unsigned int*)dst;
	n = xsize*ysize;

    if ( !n )
		return;

	do
	{
		unsigned int rgba = *p++;
		unsigned int a = (rgba & 0xff000000);
		unsigned int r = (rgba & 0x00ff0000);
		unsigned int g = (rgba & 0x0000ff00);
		unsigned int b = (rgba & 0x000000ff);

		*d++ = (r<<8) | (g<<8) | (b<<8) | (a>>24);
	}while(--n);
}

void convertARGB0888_to_ARGB8888( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	int	n;
	unsigned int *p;
	unsigned int *d;

	p = (unsigned int*)src;
	d = (unsigned int*)dst;
	n = xsize*ysize;

    if ( !n )
		return;

	do
	{
		unsigned int argb = *p++;
		argb = (unsigned int)( 0xff << 24 ) | argb;

		*d++ = argb;
	}while(--n);
}

void convertRGBA8888_to_A8(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	a = p[3];

		*d++ = a;

		p += 4;
    }
}

void convertRGBA8888_to_L8(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	r = *p++;
		int	g = *p++;
		int	b = *p++;
		p++;

		*d++ = ( r + g + b ) / 3;
    }
}

void convertARGB8888_to_L8(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned char *d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int a = *p++;
		int	r = *p++;
		int	g = *p++;
		int	b = *p++;

		*d++ = ( r + g + b ) / 3;
    }
}

// Source: A8/L8

void convertA8_to_RGBA8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned char	*p;
	unsigned char	*d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	a = *p++;

		*d++ = 255;
		*d++ = 255;
		*d++ = 255;
		*d++ = a;

    }
}

void convertA8_to_RGB888( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned char	*p;
	unsigned char	*d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
/*		int	a = *p++;*/

		*d++ = 255;
		*d++ = 255;
		*d++ = 255;

    }
}


void convertL8_to_RGB888( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned char	*p;
	unsigned char	*d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	l = *p++;

		*d++ = l;
		*d++ = l;
		*d++ = l;

    }
}

void convertL8_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned char	*p;
	unsigned int	*d;

	p = (unsigned char*)src;
	d = (unsigned int*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	l = *p++;

		*d++ = 0xff000000 | ( l << 16 ) | ( l << 8 ) | l;
    }
}



void convertA8_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned char	*p;
	unsigned char	*d;

	p = (unsigned char*)src;
	d = (unsigned char*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	a = *p++;

		*d++ = a;
		*d++ = 255;
		*d++ = 255;
		*d++ = 255;
    }
}

#ifdef ENABLE_16BIT
void convertA8_to_ARGB4444(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned short *d;

	p = (unsigned char*)src;
	d = (unsigned short*)dst;
    n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	a = (*p++) >> 4;

		*d++ = (a<<12) | 0x0fff;

    }
}

void convertL8_to_RGB565( 	   unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
    unsigned char *p;
	unsigned short *d;

	p = (unsigned char*)src;
	d = (unsigned short*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		int	l = *p++;

		*d++ = ((l>>3)<<11) | ((l>>2)<<5) | (l>>3);
    }
}
#endif

/*
	Note: When we are extending the range, we can't use simple shifts. Here is an explanation:

	Let's say we have 4bit value 0xf (15 = max. we want to extend it into 8 bits. with
	a simple shift it would be 0xf0 (240) which is not the maximum value the component
	can have. we have to use multiplication to do this:

	int	mul4_8	= ( 255 / 15 ) * 65536;
	int	new = ( old * mul4_8 ) >> 16;

	this way from 0xf we will get 0xff which is a correct result. Here are some precomputed
	scaling factors:

	mul4_8 = 1114112
	mul5_8 = 539086
	mul1_8 = 16711680
*/

#define	mul1_8  16711680
#define	mul4_8  1114112
#define	mul5_8  539086
#define mul1_4  291
#define mul4_5  135441
#define mul4_6  275251
#define	mul5_6  133186
#define mul6_8  265264

#ifdef ENABLE_16BIT
void convertARGB4444_to_RGB888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned char *d;

	p = (unsigned short*)src;
	d = (unsigned char*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		unsigned int r = ( pixel16 & 0x0f00 ) >> 8;
		unsigned int g = ( pixel16 & 0x00f0 ) >> 4;
		unsigned int b = ( pixel16 & 0x000f );

		r = ( r * mul4_8 ) >> 16;
		g = ( g * mul4_8 ) >> 16;
		b = ( b * mul4_8 ) >> 16;

		*d++ = r;
		*d++ = g;
		*d++ = b;

    }
}

void convertARGB4444_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned int	*d;

	p = (unsigned short*)src;
	d = (unsigned int*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		unsigned int a = ( pixel16 & 0xf000 ) >> 12;
		unsigned int r = ( pixel16 & 0x0f00 ) >> 8;
		unsigned int g = ( pixel16 & 0x00f0 ) >> 4;
		unsigned int b = ( pixel16 & 0x000f );

		a = ( a * mul4_8 ) >> 16;
        r = ( r * mul4_8 ) >> 16;
		g = ( g * mul4_8 ) >> 16;
		b = ( b * mul4_8 ) >> 16;

		*d++ = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b ;

    }
}

void convertARGB4444_to_RGB565(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned short *d;

	p = (unsigned short*)src;
	d = (unsigned short*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		unsigned int r = ( pixel16 & 0x0f00 ) >> 8;
		unsigned int g = ( pixel16 & 0x00f0 ) >> 4;
		unsigned int b = ( pixel16 & 0x000f );

		r = ( r * mul4_5 ) >> 16;
		g = ( g * mul4_6 ) >> 16;
		b = ( b * mul4_5 ) >> 16;

		*d++ = ( r << 11 ) | ( g << 5 ) | b ;

    }
}

#define	MASKA	0x8000
#define	MASKR	0x7c00
#define	MASKG	0x03e0
#define	MASKB	0x001f

void convertARGB1555_to_RGB888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned char *d;

	p = (unsigned short*)src;
	d = (unsigned char*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		int	r = ( pixel16 & MASKR ) >> 10;
		int	g = ( pixel16 & MASKG ) >> 5;
		int	b = ( pixel16 & MASKB );

		r = ( r * mul5_8 ) >> 16;
		g = ( g * mul5_8 ) >> 16;
		b = ( b * mul5_8 ) >> 16;

		*d++ = r;
		*d++ = g;
		*d++ = b;

    }
}

void convertARGB1555_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned int *d;

	p = (unsigned short*)src;
	d = (unsigned int*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		unsigned int a = ( pixel16 & MASKA ) >> 15;
		unsigned int r = ( pixel16 & MASKR ) >> 10;
		unsigned int g = ( pixel16 & MASKG ) >> 5;
		unsigned int b = ( pixel16 & MASKB );

		a = ( a * mul1_8 ) >> 16;
		r = ( r * mul5_8 ) >> 16;
		g = ( g * mul5_8 ) >> 16;
		b = ( b * mul5_8 ) >> 16;

		*d++ = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b ;

    }
}

void convertARGB1555_to_RGB565(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned short *d;

	p = (unsigned short*)src;
	d = (unsigned short*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		unsigned int r = ( pixel16 & MASKR ) >> 10;
		unsigned int g = ( pixel16 & MASKG ) >> 5;
		unsigned int b = ( pixel16 & MASKB );

		g = ( g * mul5_6 ) >> 16;

		*d++ = ( r << 11 ) | ( g << 5 ) | b ;

    }
}

#undef MASKA
#undef MASKR
#undef MASKG
#undef MASKB

#define MASKR	0xf800
#define MASKG	0x7e0
#define MASKB	0x1f

void convertRGB565_to_RGB888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned char *d;

	p = (unsigned short*)src;
	d = (unsigned char*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
		unsigned int r = ( pixel16 & MASKR) >> 11;
		unsigned int g = ( pixel16 & MASKG ) >> 5;
		unsigned int b = ( pixel16 & MASKB );

		r = ( r * mul5_8 ) >> 16;
		g = ( g * mul6_8 ) >> 16;
		b = ( b * mul5_8 ) >> 16;

		*d++ = r;
		*d++ = g;
		*d++ = b;

    }
}

void convertRGB565_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
    int i, n;
	unsigned short *p;
	unsigned int	*d;

	p = (unsigned short*)src;
	d = (unsigned int*)dst;
	n = xsize * ysize;

    for ( i = 0; i < n; i++ )
    {
		unsigned short	pixel16 = *p++;
        unsigned int r = ( pixel16 & MASKR) >> 11;
		unsigned int g = ( pixel16 & MASKG ) >> 5;
		unsigned int b = ( pixel16 & MASKB );

		r = ( r * mul5_8 ) >> 16;
		g = ( g * mul6_8 ) >> 16;
		b = ( b * mul5_8 ) >> 16;

		*d++ = ( 0xff << 24 ) | ( r << 16 ) | ( g << 8 ) | b ;

    }
}

void convertRGB565_to_RGB565(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	memcpy( dst, src, xsize * ysize * 2 );
}
#endif

void convertRGB888_to_RGB888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	memcpy( dst, src, xsize * ysize * 3 );
}

void convertARGB8888_to_ARGB8888(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	memcpy( dst, src, xsize * ysize * 4 );
}

void convertL8_to_L8(      unsigned char *src, unsigned char *dst,
									int xsize, int ysize )
{
	memcpy( dst, src, xsize * ysize  );
}

#undef	MASKR
#undef	MASKG
#undef	MASKB

/* YUV color functions */

/*
	Formula to convert from RGB to YUV values:

    Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16
	U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128
	V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128

	Formula to convert from YUV to RGB values:

	Scalar version:

    C = Y - 16
	D = U - 128
	E = V - 128


	R = clip(( 298 * C           + 409 * E + 128) >> 8)
	G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
	B = clip(( 298 * C + 516 * D           + 128) >> 8)

	Vector version:

	(reg0)

	R1 = clip(( 298 * C1 + 0   * D1 + 409 * E1 + 128) >> 8)
	G1 = clip(( 298 * C1 - 100 * D1 - 208 * E1 + 128) >> 8)
	B1 = clip(( 298 * C1 + 516 * D1 + 0   * E1 + 128) >> 8)
	R2 = clip(( 298 * C2 + 0   * D1 + 409 * E1 + 128) >> 8)

	(reg1)

	G2 = clip(( 298 * C2 - 100 * D1 - 208 * E1 + 128) >> 8)
	B2 = clip(( 298 * C2 + 516 * D1 + 0   * E1 + 128) >> 8)
	R3 = clip(( 298 * C3 + 0   * D2 + 409 * E2 + 128) >> 8)
	G3 = clip(( 298 * C3 - 100 * D2 - 208 * E2 + 128) >> 8)

	(reg2)

	B3 = clip(( 298 * C3 + 516 * D2 + 0   * E2 + 128) >> 8)
	R4 = clip(( 298 * C4 + 0   * D2 + 409 * E2 + 128) >> 8)
	G4 = clip(( 298 * C4 - 100 * D2 - 208 * E2 + 128) >> 8)
	B4 = clip(( 298 * C4 + 516 * D2 + 0   * E2 + 128) >> 8)

	C/D/E are 32bit values. It means that there at 4 of them
	in one vector register.



*/

static inline int yuvclip( int a )
{
	if ( a < 0 )
	{
		//printf("%d\n",a);
		return	0;
	}
	if ( a > 255 )
	{
	//	  printf("%d\n",a);
		return	255;
	}

	return a;
}

void convertRGB888_to_YUV422( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int i, j;
    unsigned char *p;
	unsigned short *d;

	if ( xsize == 0 || ysize == 0 )
		return;

	/* YUV 4:2:2 has main block containing 2 pixels. first Y and U and second Y and V */

	for ( j = 0; j < ysize; j++ )
	{
		p = (unsigned char*)src + j * xsize * 3;
		d = (unsigned short*)dst + j * xsize;

		i = xsize>>1;

		do
		{
			int	y, u, v;

			int	r = *p++;
			int	g = *p++;
			int	b = *p++;

			/* first pixel */

			y = ( (  66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
			u = ( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
			v = ( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

			/* store first pixel */

			*d++ = ( y << 8 ) | u;

			r = *p++;
			g = *p++;
			b = *p++;

			/* second pixel */

			y = ( (  66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
			//u = ( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
			//v = ( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

			/* store second pixel */

			*d++ = ( y << 8 ) | v;
		}while(--i);
	}
}

/*
 * custom version doesn't comply strictly to conversion rules. It shoul hovewer produce
 * same result when coverted to and from YUV format.
 */

void convertRGB888_to_YUV422_custom( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int i, j;
    unsigned char *p;
	unsigned short *d;

	if ( xsize == 0 || ysize == 0 )
		return;

	/* YUV 4:2:2 has main block containing 2 pixels. first Y and U and second Y and V */

	for ( j = 0; j < ysize; j++ )
	{
		p = (unsigned char*)src + j * xsize * 3;
		d = (unsigned short*)dst + j * xsize;

		i = xsize>>1;

		do
		{
			int	y, u, v;

			int	r = *p++;
			int	g = *p++;
			int	b = *p++;

			/* first pixel */

			y = ( (  66 * r + 129 * g +  25 * b ) >> 8);
			u = ( ( -38 * r -  74 * g + 112 * b ) >> 8) + 128;
			v = ( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128;

			/* store first pixel */

			*d++ = ( y << 8 ) | u;

			r = *p++;
			g = *p++;
			b = *p++;

			/* second pixel */

			y = ( (  66 * r + 129 * g +  25 * b ) >> 8);
			//u = ( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
			//v = ( ( 112 * r -  94 * g -  18 * b ) >> 8) + 128;

			/* store second pixel */

			*d++ = ( y << 8 ) | v;
		}while(--i);
	}
}

#if 0
static void convertYUV422_to_RGB888_av(unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int i, j;
	unsigned char	*p;
	unsigned short	*_d;

	static	unsigned char	lookup[ 2048 ];
	static	int	initialized = 0;

	if	( !initialized )
	{

		for	(i=0;i<2048;i++)
		{
			int	a = i - 512;

			if	( a < 0 )
				a = 0;
			if	( a > 255 )
				a = 255;

			lookup[ i ] = a;
		}

		initialized = 1;
	}

	#define	yuvclip(a) (lookup[ (a) + 512 ])

	/* YUV 4:2:2 has main block containing 2 pixels. first Y and U and second Y and V */

	for ( j = 0; j < ysize; j++ )
	{
		vector int v128 = { 128, 128, 128, 128 };
		vector int vzero = { 0, 0, 0, 0 };
		vector int v298 = { 298, 298, 298, 298 };
		vector int vUMul1 = { 0, -100, 516, 0 };
		vector int vUMul2 = { -100, 516, 0, -100 };
		vector int vUMul3 = { 516, 0, -100, 516 };
		vector int vVMul1 = { 409, 208, 0, 409 };
		vector int vVMul2 = { 208, 0, 409, 208 };
		vector int vVMul3 = { 0, 409, 208, 0 };
		vector int vshift = { 8, 8, 8, 8 };
		vector int v255 = { 255, 255, 255, 255 };
		vector int vswap = vec_lvsl(8, (int*)0);

		p = (unsigned char*)src + j * xsize * 2;
		_d = (unsigned short*)(dst + j * xsize * 3);

		/* vectorized version processes 24 pixels in one go! */

		for	( i = 0 ; i < (xsize>>1) ; i++ )
		{
			vector int vp1;
			vector int vy1y1y1y2, vy2y2y3y3, vy3y4y4y4;
			vector int vu1u1u1u1, vu1u1u2u2, vu2u2u2u2;
			vector int vv1v1v1v1, vv1v1v2v2, vv2v2v2v2;

			vector int vy5y5y5y6, vy6y6y7y7, vy7y8y8y8;
			vector int vu3u3u3u3, vu3u3u4u4, vu4u4u5u5;
			vector int vv3v3v3v3, vv4v4v4v4, vv4v4v4v4;

			vector int vres1, vres2, vres3, vres4, vres5, vres6;
			vector int vfin1;

			/* load yuv pixels (y1u1y2v1 y3u2y4v2 y5u3y6v3 y7u4y8v4) */

			vp1 = vec_ld( 0, ( unsigned int* )p );

			/* permute data into right places in right registers */

			{
				/* y1y1y1y2 - each 32bit */
				vector unsigned char perm1 = { 0, 16, 16, 16, 0, 16, 16, 16, 0, 16, 16, 16, 2, 16, 16, 16 };
				/* u1u1u1u1 */
				vector unsigned char perm2 = { 1, 16, 16, 16, 1, 16, 16, 16, 1, 16, 16, 16, 1, 16, 16, 16 };
				/* v1v1v1v1 */
				vector unsigned char perm3 = { 3, 16, 16, 16, 3, 16, 16, 16, 3, 16, 16, 16, 3, 16, 16, 16 };
				/* y2y2y3y3 - each 32bit. */
				vector unsigned char perm4 = { 2, 16, 16, 16, 2, 16, 16, 16, 4, 16, 16, 16, 4, 16, 16, 16 };
				/* u1u1u2u2 */
				vector unsigned char perm5 = { 1, 16, 16, 16, 1, 16, 16, 16, 5, 16, 16, 16, 5, 16, 16, 16 };
				/* v1v1v2v2 */
				vector unsigned char perm6 = { 3, 16, 16, 16, 3, 16, 16, 16, 7, 16, 16, 16, 7, 16, 16, 16 };
				/* y3y4y4y4 - each 32bit. */
				vector unsigned char perm7 = { 2, 16, 16, 16, 2, 16, 16, 16, 4, 16, 16, 16, 4, 16, 16, 16 };
				/* u2u2u2u2 */
				vector unsigned char perm8 = { 5, 16, 16, 16, 5, 16, 16, 16, 5, 16, 16, 16, 5, 16, 16, 16 };
				/* v2v2v2v2 */
				vector unsigned char perm9 = { 7, 16, 16, 16, 7, 16, 16, 16, 7, 16, 16, 16, 7, 16, 16, 16 };

				vy1y1y1y2 = vec_perm( vp1, vzero, perm1 );
				vu1u1u1u1 = vec_perm( vp1, vzero, perm2 );
				vv1v1v1v1 = vec_perm( vp1, vzero, perm3 );
				
                vy2y2y3y3 = vec_perm( vp1, vzero, perm4 );
				vu1u1u2u2 = vec_perm( vp1, vzero, perm5 );
				vy1v1v2v2 = vec_perm( vp1, vzero, perm6 );
				
                vy2y2y3y3 = vec_perm( vp1, vzero, perm7 );
				vu2u2u2u2 = vec_perm( vp1, vzero, perm8 );
				vy2v2v2v2 = vec_perm( vp1, vzero, perm9 );

				/* swap the source so we can reuse permute params */

				vp1 = vec_perm( vp1, vp1, vswap );

				vy5y5y5y6 = vec_perm( vp1, vzero, perm1 );
				vu3u3u3u3 = vec_perm( vp1, vzero, perm2 );
				vv3v3v3v3 = vec_perm( vp1, vzero, perm3 );

				vy6y6y7y7 = vec_perm( vp1, vzero, perm4 );
				vu3u3u4u4 = vec_perm( vp1, vzero, perm5 );
				vy3v3v4v4 = vec_perm( vp1, vzero, perm6 );

				vy7y8y8y8 = vec_perm( vp1, vzero, perm7 );
				vu4u4u4u4 = vec_perm( vp1, vzero, perm8 );
				vy4v4v4v4 = vec_perm( vp1, vzero, perm9 );
			}


			/* calculate first 2 vectors */

			vres1 = vec_madd( vy1y1y1y2, v298, v128 );
			vres1 = vec_madd( vu1u1u1u1, vUMul1, vres1 );
			vres1 = vec_madd( vv1v1v1v1, vVMul1, vres1 );
			vres2 = vec_madd( vy2y2y3y3, v298, v128 );
			vres2 = vec_madd( vu1u1u2u2, vUMul2, vres1 );
			vres2 = vec_madd( vv1v1v2v2, vVMul2, vres1 );

			/* pack (will shift 8 right and clamp to 16bit) first 2 pixels */

			vfin1 = vec_packs( vres1, vres2 );	/* r0g0b0r0 g0b0r0g0 */

			/* calculate second 2 vectors */

			vres1 = vec_madd( vy3y4y4y4, v298, v128 );
			vres1 = vec_madd( vu2u2u2u2, vUMul3, vres1 );
			vres1 = vec_madd( vv2v2v2v2, vVMul3, vres1 );
			vres2 = vec_madd( vy5y5y5y6, v298, v128 );
			vres2 = vec_madd( vu2u2u3u3, vUMul1, vres1 );
			vres2 = vec_madd( vv2v2v3v3, vVMul1, vres1 );

            /* pack (will shift 8 right and clamp to 16bit) first 2 pixels */

			vfin2 = vec_packs( vres1, vres2 );	/* b0r0g0b0 r0g0b0r0 */

			p += 4;
		}
	}
}
#endif

static unsigned char yuvlookup[2048];

void ConversionInit(void)
{
	int i;
	for	(i=0; i<2048; i++)
	{
		int	a = i - 512;

		if (a < 0)
			a = 0;
		if (a > 255)
			a = 255;

		yuvlookup[i] = a;
	}
}

void convertYUV422_to_RGB888( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int i, j;
	unsigned char *p;
	unsigned short *_d;
	unsigned char *lookup = yuvlookup;

	if ( xsize == 0 || ysize == 0 )
		return;

	#if 0
	if	( hasAltivec )
		return convertYUV422_to_RGB888_av( src, dst, xsize, ysize );
	#endif

	#define	yuvclip(a) (lookup[ (a) + 512 ])

	/* YUV 4:2:2 has main block containing 2 pixels. first Y and U and second Y and V */

	for ( j = 0; j < ysize; j++ )
	{
		p = (unsigned char*)src + j * xsize * 2;
		_d = (unsigned short*)(dst + j * xsize * 3);

		i = xsize >> 1;
		do
		{
			int	r, g, b;

			unsigned int y1uy2v = *(unsigned int*)p;
			int	y1	= ( y1uy2v & 0xff000000 ) >> 24;//*p++;
			int	u	= ( y1uy2v & 0xff0000 ) >> 16;//*p++;
			int	y2	= ( y1uy2v & 0xff00 ) >> 8;//*p++;
			int	v	= ( y1uy2v & 0xff );//*p++;

			int	c1 = ( y1 - 16 ) * 298;
			int	c2 = ( y2 - 16 ) * 298;
			int	d = u - 128;
			int	e = v - 128;
			int	de = - 100 * d - 200 * e + 128;

			int	l1;

			d = 516 * d + 128;
			e = 409 * e + 128;

			/* first pixel */

			r = yuvclip( ( c1 + e                      ) >> 8 );
			g = yuvclip( ( c1 + de                     ) >> 8 );
			b = yuvclip( ( c1 + d                      ) >> 8 );

			/* store first pixel */

			//*_d++ = r;
			//*_d++ = g;
			//*_d++ = b;
			l1 = (r<<24)|(g<<16)|(b<<8);
			/* second pixel */

			r = yuvclip( ( c2 + e                      ) >> 8 );
			g = yuvclip( ( c2 + de                     ) >> 8 );
			l1|=r;
			b = yuvclip( ( c2 + d                      ) >> 8 );

			/* store second pixel */

			//*_d++ = r;
			//*_d++ = g;
			*(unsigned int*)_d = l1;
			_d+=2;
			*_d++ = (g<<8|b);

			p += 4;
		}while(--i);

		if ( xsize > 1 && ( xsize & 1 ) )
		{
			unsigned char *d = ( unsigned char* )_d;
			d[ 0 ] = d[ -3 ];
			d[ 1 ] = d[ -2 ];
			d[ 2 ] = d[ -1 ];
		}
	}
}

void convertYUV422_to_RGB888_custom( unsigned char *src, unsigned char *dst,
								int xsize, int ysize )
{
	int i, j;
	unsigned char *p;
	unsigned short *_d;
	unsigned char *lookup = yuvlookup;

	if ( xsize == 0 || ysize == 0 )
		return;

	#define	yuvclip(a) (lookup[ (a) + 512 ])

	/* YUV 4:2:2 has main block containing 2 pixels. first Y and U and second Y and V */

	for ( j = 0; j < ysize; j++ )
	{
		p = (unsigned char*)src + j * xsize * 2;
		_d = (unsigned short*)(dst + j * xsize * 3);

		i = xsize >> 1;
		do
		{
			int	r, g, b;

			unsigned int y1uy2v = *(unsigned int*)p;
			int	y1	= ( y1uy2v & 0xff000000 ) >> 24;//*p++;
			int	u	= ( y1uy2v & 0xff0000 ) >> 16;//*p++;
			int	y2	= ( y1uy2v & 0xff00 ) >> 8;//*p++;
			int	v	= ( y1uy2v & 0xff );//*p++;

			int	c1 = y1 * 298;
			int	c2 = y2 * 298;
			int	d = u - 128;
			int	e = v - 128;
			int	de = - 100 * d - 200 * e;

			int	l1;

			d = 516 * d;
			e = 409 * e;

			/* first pixel */

			r = yuvclip( ( c1 + e                      ) >> 8 );
			g = yuvclip( ( c1 + de                     ) >> 8 );
			b = yuvclip( ( c1 + d                      ) >> 8 );

			/* store first pixel */

			//*_d++ = r;
			//*_d++ = g;
			//*_d++ = b;
			l1 = (r<<24)|(g<<16)|(b<<8);
			/* second pixel */

			r = yuvclip( ( c2 + e                      ) >> 8 );
			g = yuvclip( ( c2 + de                     ) >> 8 );
			l1|=r;
			b = yuvclip( ( c2 + d                      ) >> 8 );

			/* store second pixel */

			//*_d++ = r;
			//*_d++ = g;
			*(unsigned int*)_d = l1;
			_d+=2;
			*_d++ = (g<<8|b);

			p += 4;
		}while(--i);

		if ( xsize > 1 && ( xsize & 1 ) )
		{
			unsigned char *d = ( unsigned char* )_d;
			d[ 0 ] = d[ -3 ];
			d[ 1 ] = d[ -2 ];
			d[ 2 ] = d[ -1 ];
		}
	}
}

void convertYUV422_to_ARGB8888_custom(unsigned char *src, unsigned char *dst, int xsize, int ysize)
{
	int i, j;
	unsigned char *p;
	unsigned int *_d;
	unsigned char *lookup = yuvlookup;

	if ( xsize == 0 || ysize == 0 )
		return;

	#define	yuvclip(a) (lookup[ (a) + 512 ])

	/* YUV 4:2:2 has main block containing 2 pixels. first Y and U and second Y and V */

	for ( j = 0; j < ysize; j++ )
	{
		p = (unsigned char*)src + j * xsize * 2;
		_d = (unsigned int*)(dst + j * xsize * 4);

		i = xsize >> 1;
		do
		{
			int	r, g, b;

			unsigned int y1uy2v = *(unsigned int*)p;
			int	y1 = (y1uy2v & 0xff000000) >> 24;//*p++;
			int	u  = (y1uy2v & 0xff0000) >> 16;//*p++;
			int	y2 = (y1uy2v & 0xff00) >> 8;//*p++;
			int	v  = (y1uy2v & 0xff);//*p++;

			int	c1 = y1 * 298;
			int	c2 = y2 * 298;
			int	d = u - 128;
			int	e = v - 128;
			int	de = - 100 * d - 200 * e;

			d = 516 * d;
			e = 409 * e;

			/* first pixel */
			r = yuvclip( ( c1 + e                      ) >> 8 );
			g = yuvclip( ( c1 + de                     ) >> 8 );
			b = yuvclip( ( c1 + d                      ) >> 8 );

			/* store first pixel */
			*_d++ = 0xff000000 | (r<<16) | (g<<8) | (b);

			/* second pixel */
			r = yuvclip( ( c2 + e                      ) >> 8 );
			g = yuvclip( ( c2 + de                     ) >> 8 );
			b = yuvclip( ( c2 + d                      ) >> 8 );

			/* store second pixel */
			*_d++ = 0xff000000 | (r<<16) | (g<<8) | (b);
			p += 4;
		}while(--i);

		if (xsize > 1 && (xsize & 1))
			_d[0] = _d[-1];
	}
}

////

char *strduplicate( char *str )
{
	char *newstr;
	int len;

	if (str == NULL)
        return NULL;

	len = strlen(str);
	newstr = malloc(len + 1);
	if (newstr == NULL)
        return NULL;

	memcpy(newstr, str, len + 1);
    return newstr;
}

char *strPathOnly(char *str)
{
	char *newStr = NULL;
	struct Process *this = (struct Process*)FindTask(NULL);
	struct Window *oldwindowptr = this->pr_WindowPtr;
	this->pr_WindowPtr = (struct Window*)-1;

	if (str != NULL)
	{
		char *file = FilePart(str);

		/* first try to lock and check if maybe it's already only a path without a file */

		if (strIsDir(str))
		{
			newStr =  strdup(str);
		}
		else if (file != NULL)
		{
			/* try to resolve device name (if it's a device). max device length = 30 characters */

			char *tempStr = malloc(file - str + 1 + 32);
			if (tempStr != NULL)
			{
				BPTR lock;

				stccpy(tempStr, str, file - str + 1);
				lock = Lock(tempStr, ACCESS_READ);

				if (lock != NULL)
				{
					if (NameFromLock(lock, tempStr, file - str + 1 + 32))
						newStr = strdup(tempStr);

					UnLock(lock);
				}

				free(tempStr);
			}

			/* try simple aproach */

			if (newStr == NULL)
			{
				newStr = malloc(file - str + 1);
				if (newStr != NULL)
					stccpy(newStr, str, file - str + 1);
			}
		}
		else
		{
			newStr = strdup(str);
		}
	}

	this->pr_WindowPtr = oldwindowptr;

	return newStr;
}

char *strSetExtension(char *str, char *ext)
{
	if (str != NULL && ext != NULL)
	{
		/* look for last '.' */

		int	pos = strlen(str) - 1;
		char *newStr;

		while(pos >= 0 && str[ pos ] != '.')
			pos--;

		if (pos < 0)
		{
			/* couldn't find '.' */

			return strdup(str);
		}

		newStr = (char*)calloc(1, pos + strlen(ext) + 1 + 1);
		stccpy(newStr, str, pos + 1);

		/* place new extension in place */

		strcat(newStr, ".");
		strcat(newStr, ext);
		return newStr;
	}

	return NULL;
}

char *strReplaceExt(char *oldstring, char *oldext, char *newext)
{
	char *newstring = NULL;

	/* search for the old extension position in the string */

	int	offset = 0;
	int	lastoffset = -1;
	int	done = 0;

	do
	{
		char *found = strstr(oldstring + offset, oldext);

		if (found != NULL)
		{
			lastoffset = found - (oldstring + offset);
			offset = lastoffset + 1;
		}
        else
		{
			done = 1;
		}

	} while (!done);

	if (lastoffset == -1)
	{
		/* return unmodified string's copy */

		return strdup(oldstring);
	}
	else
	{
		//printf("Found extension at position %d (string:%s, extension: %s)\n",lastoffset, oldstring, oldext );

		/* alloc memory for the new string */

		newstring = malloc(lastoffset + strlen(newext) + 1);
		if (newstring == NULL)
			return NULL;

		/* create new string */

		stccpy(newstring, oldstring, lastoffset + 1);
		strcat(newstring, newext);

		/* done */

		return newstring;
	}
}

/* This one converts \'s from PC filepaths into / (Amiga style) */

void strFixPath(char *string)
{
	int	i;

	if (string == NULL)
		return;

	/* fix EOL marker */

	i = strlen(string);
	if (i > 1)
	{
		if (string[i -2] == 0x0d && string[ i-1 ] == 0x0a)
		{
			string[i-2]=0;
			string[i-1]=0;
		}
		else if	(string[i-1] == '\n')
		{
			string[i-1]=0;
			string[i-1]=0;
		}
	}

	/* change dir separator */

	for	(i=0;i<strlen(string);i++)
	{
		if (string[i]=='\\')
			string[i]='/';
	}
}

int strMatch(char *name, char *pattern)
{
	int	rc = FALSE;

	if (name != NULL && pattern != NULL)
	{
		char statbuf[512];	  /* To remove memory allocation in most cases */
		char *dynbuf = NULL;
		char *buf;
		int len = strlen(pattern) * 2 + 2;

		if (len > sizeof(statbuf))
			buf = dynbuf = malloc(len);
		else
			buf = statbuf;

		if (buf != NULL)
		{
			if (ParsePatternNoCase(pattern, buf, len) != -1)
			{
				if (MatchPatternNoCase(buf, (STRPTR) name))
					rc = TRUE;
			}
		}

		free(dynbuf);
	}

	return rc;
}

char *strReplace(char *oldname, char *newname)
{
	free(oldname);
	return strdup(newname);
}

/// SQRT function

/* fsqrt.c
 *
 * A fast square root program adapted from the code of
 * Paul Lalonde and Robert Dawson in Graphics Gems I.
 * The format of IEEE double precision floating point numbers is:
 *
 * SEEEEEEEEEEEMMMM MMMMMMMMMMMMMMMM MMMMMMMMMMMMMMMM MMMMMMMMMMMMMMMM
 *
 * S = Sign bit for whole number
 * E = Exponent bit (exponent in excess 1023 form)
 * M = Mantissa bit
 */

/* SQRT_TAB_SIZE - the size of the lookup table - must be a power of four.
 */

#define SQRT_TAB_SIZE	1024

/* MANT_SHIFTS is the number of shifts to move mantissa into position.
 * If you quadruple the table size subtract two from this constant,
 * if you quarter the table size then add two.
 * Valid values are: (16384, 7) (4096, 9) (1024, 11) (256, 13)
 */

#define MANT_SHIFTS   11

#define EXP_BIAS   1023       /* Exponents are always positive     */
#define EXP_SHIFTS 20         /* Shifs exponent to least sig. bits */
#define EXP_LSB    0x00100000 /* 1 << EXP_SHIFTS                   */
#define MANT_MASK  0x000FFFFF /* Mask to extract mantissa          */

static	int *sqrt_tab = NULL;

union doubleintfuck
{
	double f;
	unsigned int fi;
};

void	InitFSQRT( void )
{
    printf( "[FSQRT]: Initializing table...\n" );
    sqrt_tab = calloc( sizeof( int ), SQRT_TAB_SIZE );

    {
        int i;
		union doubleintfuck di;

        // SAS/C note: Don't change this into single loop because it causes
        // optimization errors.

        for ( i = 0; i < ( SQRT_TAB_SIZE / 2 ); i++ )
        {
			di.f = 0;  /* Clears least sig part */
			di.fi = ( i << MANT_SHIFTS ) | ( EXP_BIAS << EXP_SHIFTS );
			di.f = sqrt(di. f );
			sqrt_tab[ i ] = di.fi & MANT_MASK;
        }

        for ( i = 0; i < ( SQRT_TAB_SIZE / 2 ); i++ )
        {
            di.f = 0;    /* Clears least sig part */
			di.fi = ( i << MANT_SHIFTS ) | ( ( EXP_BIAS + 1 ) << EXP_SHIFTS );
			di.f = sqrt( di.f );
			sqrt_tab[ i + ( SQRT_TAB_SIZE / 2 ) ] = di.fi & MANT_MASK;
        }
    }

}


double	fsqrt( double f )
{
	unsigned int e;
	unsigned int fint;
	union doubleintfuck di;

	if ( f == 0.0 )
		return ( 0.0 );

	di.f = f;
	fint = di.fi;

	e = ( fint >> EXP_SHIFTS ) - EXP_BIAS;
	fint &= MANT_MASK;
	if ( e & 1 )
		fint |= EXP_LSB;

	e >>= 1;
	di.fi = ( sqrt_tab[ fint >> MANT_SHIFTS ] ) |
	        ( ( e + EXP_BIAS ) << EXP_SHIFTS );
	return ( di.f );
}

////

/// FRAND function

float frand(float max)
{
	float t = (float)rand();
	t = t / (float)RAND_MAX;
    t = t * max;
    return t;
}

////

/// FindParam function

static int __argc;
static char	**__argv;

void ParamsInit(int argc, char *argv[])
{
	__argc = argc;
	__argv = argv;
}

int	FindParam(char *param)
{
    int	i;

	for (i=1; i<__argc; i++)
    {
		if (stricmp(__argv[i], param) == 0)
            return 1;
    }

    return 0;
}

////

/// fClamp(val)

float fClamp(float a)
{
	if (a < 0.0f)
		return 0.0f;
	if (a > 1.0f)
		return 1.0f;

	return a;
}

////

void scaleCoordsWithAspect( int width, int height, int destWidth, int destHeight, int	 *newWidth, int	  *newHeight )
{
    /* calculate new dimensions with preserved aspect */

	{
		/* calculate dimensions for fitting width */

		float width1 = destWidth;
		float height1 = (float)height / width * destWidth;

		/* calculate dimensions for fitting height */

		float width2 = (float)width / height * destHeight;
		float height2 = destHeight;

		/* check which fit into the area */

		if ( width1 <= destWidth && height1 <= destHeight )
		{
			/* use first set */
			*newWidth = max( 1, (int)width1 );
			*newHeight = max( 1, (int)height1 );
		}
		else
		{
			/* use second set */
			*newWidth = max( 1, (int)width2 );
			*newHeight = max( 1, (int)height2 );
		}
	}
}

int strIsDir(char *path)
{
	BPTR lock = Lock(path, ACCESS_READ);
	if (lock != NULL)
	{
		struct FileInfoBlock fib;
		if (Examine(lock, &fib))
		{
			UnLock(lock);
			return fib.fib_DirEntryType > 0;
		}
	}
	return FALSE;
}
