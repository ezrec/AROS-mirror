/*
** $Id$
*/

#define _isinobject(x,y) (_between(_left(obj),(x),_right(obj)) && _between(_top(obj),(y),_bottom(obj)))
#define _isinobjectf(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))
#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define GFX39 (GfxBase->LibNode.lib_Version>=39)
#define STRMIN(a,b) (char *)min((int)a,(int)b)
#define STRMAX(a,b) (char *)max((int)a,(int)b)
#ifndef MBX
#undef isalpha
#define isalpha(ch) isalnumtable[ch]
#undef isalnum
#define isalnum(ch) isalnumtable[ch]
#endif
#define CLIPDEVICE  0

struct parsedix {
	struct Node n;
	IX ix;
};

struct fontcache {
	struct MinNode n;
	struct TextFont *tf;
	int usecount;
   UBYTE fwarray[ 256 ];
};

#ifndef MBX
extern UBYTE isalnumtable[ 256 ];
#endif
#ifdef MBX
#define notifyclass NotifyClass
#else
extern struct IClass *notifyclass;
extern struct MinList fontcachelist;
#endif

extern struct MinList pixlist;
extern struct SignalSemaphore pixsem;

void getvirtoff( Object *, ULONG *, ULONG * );
BOOL isinobject( Object *,int, int, BOOL );
long myatoi( char * );
struct MenuItem *FindCommandKeyInMenuItem(struct MenuItem *mi,BYTE key, int *itemcount, int *subcount );
#ifndef MBX
struct MenuItem *FindCommandKeyInMenu(struct Menu *m,BYTE key,UWORD *menunum);
#endif
BOOL myParseIX( STRPTR, IX * );
IX *parseix( STRPTR );
void makefontarray( struct TextFont *, UBYTE * );
struct fontcache *fc_find( struct TextFont * );
void fc_free( struct fontcache * );
int myMatchIMsgIX( struct IntuiMessage *, IX * );
void makealnumtable( void );
BOOL tisnum( char );
#ifndef MBX
APTR __stdargs DoSuperNew( Class *, Object *, ULONG, ... );
#endif
void __stdargs tnotify( APTR, ... );
int writeclip( char *, int );
int getlinelenline( char * );
int getlinelen( struct Data *, int );
char *getline( struct Data *, int );
char *getlinestart( struct Data *, char * );
char *getlineend( struct Data *, char * );
char *getwordstart( struct Data *, char * );
char *getwordend( struct Data *, char * );
char *getnumstart( struct Data *, char * );
char *getnumend( struct Data *, char * );
char *getcursorp( struct Data *, int, int );
void findcursorp( struct Data * );
void findcursorxy( struct Data *, char * );
void findxyp( struct Data *, char *, int *, int *, char ** );
int updatemark( struct Data *, char *, char * );
int resetmarkmode( struct Data * );

#ifndef MBX
void __stdargs sprintf( char *, char *, ... );
#endif
