/******************************************************/
/*                                                    */
/* declare.h: declaration-symbols (UCHAR, ULONG, ...) */
/*                                                    */
/******************************************************/

#ifndef __declare_h
#define __declare_h

#ifdef AMIGA
#include <exec/types.h>

#if defined __AROS__
typedef unsigned short USHORT;
typedef short          SHORT;
#endif

#else /* AMIGA */

typedef unsigned short USHORT;
typedef short          SHORT ;
typedef unsigned short UWORD ;
typedef short          WORD  ;
typedef unsigned long  ULONG ;
typedef long           LONG  ;

#endif  /* !AMIGA */

typedef unsigned char  UCHAR ;
typedef char           CHAR  ;
typedef unsigned       UINT  ;
typedef int            INT   ;

#endif /* __declare_h */

