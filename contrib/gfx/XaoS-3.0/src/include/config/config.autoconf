#ifndef CONFIG_H
#define CONFIG_H
#define HOMEDIR
#define CONFIGFILE ".XaoSrc"
/*#define I_WAS_HERE */      /*uncoment this to disable changing of parameters by atoconf */

#define FPOINT_TYPE  long double
				       /*floating point math type on computers
				          with medium speed floating point math should   
				          use float instead */
#include <aconfig.h>
#define USE_STDIO
#if !defined(HAVE_LONG_DOUBLE)&&!defined(I_WAS_HERE)
#undef FPOINT_TYPE
#define FPOINT_TYPE double
#endif
#define CONST const
#define INLINE inline
#ifdef __GNUC__
#define myfabs(x) fabs(x)
#ifndef __STRICT_ANSI__
#define CONSTF __attribute__ ((const))
#ifdef __i386__
#ifndef NOREGISTERS
#define REGISTERS(n) __attribute__ ((regparm(n)))
#else
#define REGISTERS(n) 
#endif
#else
#define REGISTERS(n) 
#endif
#else
/*#warning Remove -ansi switch from your Makefile to get better code.*/
#define CONSTF
#define REGISTERS(n)
#endif
#else
#define myfabs(x) ((x)>0?(x):-(x))
#define CONSTF
#define REGISTERS(n)
#endif
#ifdef HAVE_MOUSEMASK
#define NCURSESMOUSE
#endif
#ifndef HAVE_LIMITS_H
#define INT_MAX 2127423647
#endif
#ifdef SVGA_DRIVER
#define DESTICKY
#endif
#ifdef X11_DRIVER
#define SFIXEDCOLOR
#define STRUECOLOR
#define SHICOLOR
#define SREALCOLOR
#define SMBITMAPS
#define SUPPORTMISSORDERED
#endif
#ifdef SVGA_DRIVER
#undef SREALCOLOR
#define SREALCOLOR
#undef SHICOLOR
#define SHICOLOR
#undef STRUECOLOR
#define STRUECOLOR
#define STRUECOLOR24
#undef SMBITMAPS
#define SMBITMAPS
#undef SUPPORTMISSORDERED
#define SUPPORTMISSORDERED
#endif
#endif				/*CONFIG_H */
