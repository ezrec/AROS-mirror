
/*
** $Id$
**
** © 1999 Marcin Orlowski <carlos@amiga.com.pl>
*/


/* mccheader.c */

ULONG __stdargs DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...);
extern struct Library *MUIMasterBase;



/* popph_misc.c */

int _strlen( char *str, char stop );
char *_strchr( char *buf, char key );
ULONG xget( Object *obj, int attr);


/* popph_clipboard.c */

int _WriteClip( char *data, int len, int clip_device );


/* popphs.c */

//ULONG PPHS_Dispatcher();

ULONG ASM SAVEDS PPHS_Dispatcher(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg);
//ULONG __asm __saveds PPHS_Dispatcher(register __a0 struct IClass *cl, register __a2 Object *obj, register __a1 Msg msg);

