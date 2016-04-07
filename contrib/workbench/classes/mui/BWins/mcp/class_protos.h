#ifndef _CLASS_PROTOS_H
#define _CLASS_PROTOS_H

/* init.c */
IPTR ASM SAVEDS query ( REG (d0 )LONG which );
void ASM freeBase ( void );
BOOL ASM initBase ( void );

/* snprintf.c */
#ifdef __AROS__
#include <stdio.h>
#else
int STDARGS snprintf ( char *buf , int size , char *fmt , ...);
#endif

/* loc.c */
void ASM initStrings ( void );
CONST_STRPTR ASM getString ( REG (d0 )LONG id );
char ASM getKeyChar ( REG (a0 )CONST_STRPTR string );

/* shape.c */
BOOL ASM initShape ( void );

/* class.c */
BOOL ASM initMCP ( void );

#endif /* _CLASS_PROTOS_H */
