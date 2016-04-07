#ifndef _CLASS_PROTOS_H
#define _CLASS_PROTOS_H

/* init.c */
APTR ASM SAVEDS query ( REG (d0 )LONG which );
void ASM freeBase ( void );
BOOL ASM initBase ( void );

/* utils.c */
IPTR STDARGS DoSuperNew ( struct IClass *cl , Object *obj , Tag tag1 , ...);

/* loc.c */
void ASM localizeMenus ( REG (a0) struct NewMenu *menu );
void ASM initStrings ( void );
CONST_STRPTR ASM getString ( REG (d0 )ULONG id );

/* sgad.c */
BOOL ASM initSGad ( void );

/* contents.c */
BOOL ASM initContents ( void );

/* boopsi.c */
BOOL ASM initBoopsi ( void );

/* class.c */
BOOL ASM initMCC ( void );

#endif /* _CLASS_PROTOS_H */
