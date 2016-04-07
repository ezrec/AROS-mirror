#ifndef _CLASS_PROTOS_H
#define _CLASS_PROTOS_H

#include <intuition/classusr.h>

/* init.c */
SAVEDS ASM IPTR query ( REG (d0 )LONG which );
void ASM freeBase ( REG (a0 )struct UrltextBase *base );
BOOL ASM initBase ( REG (a0 )struct UrltextBase *base );

/* utils.c */
char ASM getKeyChar ( REG (a0 )CONST_STRPTR string );
Object *ASM keyCheckmark ( REG (a0 )CONST_STRPTR key , REG (d0 )ULONG help );
Object *ASM keyPopPen ( REG (d0 )ULONG title , REG (d1 )CONST_STRPTR key , REG (d2 )ULONG help );
Object *ASM urltext ( REG (a0 )STRPTR url , REG (a1 )STRPTR text );
#ifndef __AROS__
int STDARGS snprintf ( char *buf , int size , char *fmt , ...);
#endif

/* loc.c */
CONST_STRPTR ASM getString ( REG (d0 )ULONG id );

/* mcp.c */
BOOL ASM initMCP ( REG (a0 )struct UrltextBase *base );

#endif /* _CLASS_PROTOS_H */
