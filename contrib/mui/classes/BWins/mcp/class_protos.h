#ifndef _CLASS_PROTOS_H
#define _CLASS_PROTOS_H

/* init.c */
ULONG ASM SAVEDS query ( REG (d0 )LONG which );
void ASM freeBase ( void );
BOOL ASM initBase ( void );

/* snprintf.c */
#ifndef __AROS__
int STDARGS snprintf ( char *buf , int size , char *fmt , ...);
#endif

/* loc.c */
void ASM initStrings ( void );
CONST_STRPTR ASM getString ( REG (d0 )LONG id );
ULONG ASM getKeyChar ( REG (a0 )STRPTR string );

/* shape.c */
BOOL ASM initShape ( void );

/* class.c */
BOOL ASM initMCP ( void );

#endif /* _CLASS_PROTOS_H */
