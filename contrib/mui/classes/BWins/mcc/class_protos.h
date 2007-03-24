#ifndef _CLASS_PROTOS_H
#define _CLASS_PROTOS_H

/* init.c */
ULONG ASM SAVEDS query ( REG (d0 )LONG which );
void ASM freeBase ( void );
BOOL ASM initBase ( void );

/* utils.c */
ULONG STDARGS DoSuperNew ( struct IClass *cl , Object *obj , ULONG tag1 , ...);

/* loc.c */
void ASM localizeMenus ( REG (a0) struct NewMenu *menu );
void ASM initStrings ( void );
STRPTR ASM getString ( REG (d0 )ULONG id );

/* sgad.c */
BOOL ASM initSGad ( void );

/* contents.c */
BOOL ASM initContents ( void );

/* boopsi.c */
BOOL ASM initBoopsi ( void );

/* class.c */
BOOL ASM initMCC ( void );

#endif /* _CLASS_PROTOS_H */
