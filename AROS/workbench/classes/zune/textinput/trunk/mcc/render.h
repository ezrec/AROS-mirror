/*
** $Id$
*/

struct clipbuff {
	struct Rectangle oldrect;
	APTR oldnext;
	APTR mcliphandle;
	int use;
	int dummy;
};

#ifndef MBX
/* textfit.a */
extern __asm int patextfit( register __a1 UBYTE *fa, register __a0 STRPTR text, register __d0 ULONG textlen, register __d1 UWORD pixelsize );
extern __asm int patextlen( register __a1 UBYTE *fa, register __a0 STRPTR text, register __d0 ULONG textlen );
#endif

BOOL isstyle( struct Data *data, char *ch );
//char styletotoken( struct Data *data, char ch );
int codetostyle( struct Data *data, char code, char *buffer );
//int parsestyles( struct Data *data, char *start, char *end, struct style *style, ULONG defbgpen, ULONG deffgpen );
//char *stripstyles( struct Data *data, char *text, int *textlen );
int textlen( struct Data *data, char *text, int len );
int findcharoffset( struct Data *data, char *text, int textlen, int pixel, int *lineend );
//void __inline setabpendrmd( struct RastPort *rp, ULONG fgpen, ULONG bgpen, ULONG drmd );
//void textextent( struct Data *data, struct RastPort *rp, char *str, int len, struct TextExtent *te );
void FillBack( struct Data *data, Object *obj, LONG left, LONG top, LONG right, LONG bottom );
//void TextFillBack( struct Data *data, Object *obj, char *str, LONG len, int pen );
//static void mytext( struct Data *data, Object *obj, char *str, LONG len, ULONG styles, ULONG fgpen, ULONG bgpen );
void text( struct Data *data, Object *obj, char *str, LONG len, int penmode );
void smarttext( struct Data *data, Object *obj, char *str, LONG len );
int underlinesegment( struct Data *data, Object *obj, struct segment *seg, char *line, int lineno, ULONG pen );
//static int underlinemisspells( struct Data *data, Object *obj, int lineno );
//static int underlineurls( struct Data *data, Object *obj, int lineno );
void drawpart( struct RastPort *rp, APTR obj, struct Data *data, char *start, char *end );
void drawline( struct RastPort *rp, APTR obj, struct Data *data, char *line, int lineno );
void drawlinefromcursor( struct RastPort *rp, APTR obj, struct Data *data, char *line, int lineno, int cleareol );
void quickaddclip( APTR obj, ULONG x, ULONG y, ULONG w, ULONG h, struct clipbuff *bh );
void quickremclip( APTR obj, struct clipbuff *bh );
void drawcursor( struct RastPort *rp, APTR obj, struct Data *data, BOOL kill );
int getlinexoffset( Object *, struct Data *, char * );
#ifdef MBX
ULONG getcharkern( struct TextFont *tf, char *chr);
#endif
