/*
** $Id$
*/

#include "mystring.h"
#include "structs.h"
#include "misc.h"

#ifndef MBX
struct IClass *notifyclass;
struct MinList fontcachelist;

struct MinList pixlist = {0,0,0};
struct SignalSemaphore pixsem;
#endif

LONG xget(Object *obj,ULONG attribute)
{
	LONG x=0;
	get(obj,attribute,&x);
	return(x);
}

Object *getvirtparent( Object *obj )
{
	if( !_isinvirtual( obj ) )
		return( NULL );
	return( _parent( obj ) );
}

void getvirtoff( Object *obj, ULONG *left, ULONG *top )
{
	Object *parent;
	if( !( parent = getvirtparent( obj ) ) )
	{
		left = top = 0;
		return;
	}
	*left = xget( parent, MUIA_Virtgroup_Left );
	*top = xget( parent, MUIA_Virtgroup_Top );
}

void getvirtrect( Object *obj, struct Rectangle *rect )
{
	Object *parent;
	rect->rect_MinX = rect->rect_MaxX = rect->rect_MinY = rect->rect_MaxY = 0;
	if( !_isinvirtual( obj ) )
		return;
	parent = _parent( obj );
	if( !parent )
		return;
	rect->rect_MaxX=rect->rect_MaxY=69;
	DoMethod( parent, MUIM_Virtgroup_GetClip, rect );
//	kprintf("getvirtrect: (%ld,%ld),(%ld,%ld), _vtop( obj )==%ld\n",rect->rect_MinX,rect->rect_MinY,rect->rect_MaxX,rect->rect_MaxY,_vtop( obj ) );
}

BOOL isinobject( Object *obj, int x, int y, BOOL f )
{
//	struct Rectangle vrect;
	BOOL rc;
//	getvirtrect( obj, &vrect );
	rc = f ? _between(_mleft(obj),x,_mright(obj)) && _between(_mtop(obj),y,_mbottom(obj)) : _between(_mleft(obj),x,_mright(obj)) && _between(_mtop(obj),y,_mbottom(obj));
	return( rc );
}

int updatemark( struct Data *data, char *rmarkstart, char *rmarkend )
{
	data->oldcursorx = data->cursorx;
	data->oldcursory = data->cursory;
	data->oldcursorp = data->cursorp;
	data->oldpxo = data->pxo;
	data->oldcellsize = data->cellsize;
	data->rmarkstart = rmarkstart;
	data->rmarkend = rmarkend;
	data->oldmarkstart = data->markstart;
	data->oldmarkend = data->markend;
	data->markstart = STRMIN( data->rmarkstart, data->rmarkend );
	data->markend = STRMAX( data->rmarkstart, data->rmarkend );
	return( data->markstart != data->oldmarkstart || data->markend != data->oldmarkend );
}

int resetmarkmode( struct Data *data )
{
	if( data->markmode )
	{
		data->markmode = FALSE;
		data->markmethod = 0;
		updatemark( data, NOMARK, NOMARK );
		return( TRUE );
	}
	return( FALSE );
}

char *getcursorp( struct Data *data, int x, int y )
{
	return( getline( data, y ) + x );
}

void findcursorp( struct Data *data )
{
	data->cursorp = getcursorp( data, data->cursorx, data->cursory );
}

void findcursorxy( struct Data *data, char *pos )
{
	findxyp( data, pos, &data->cursorx, &data->cursory, NULL );
	data->cursorp = pos;
}

void findxyp( struct Data *data, char *pos, int *x, int *y, char **line )
{
	char *p = data->buffer;
	*x = *y = 0;
	if( line )
		*line = p;
	while( p < pos )
	{
		(*x)++;
		if( *p++ == '\n' )
		{
			if( line )
				*line = p;
			*x = 0;
			(*y)++;
		}
	}
}

char * __inline getline( struct Data *data, int l )
{
	char *p = data->buffer;

	while( l )
	{
		while( *p && *p != '\n' )
			p++;
		if( !*p )
			break;
		p++;
		l--;
	}
	return( p );
}

char *__inline getlinestart( struct Data *data, char *line )
{
	while( line > data->buffer && line[ -1 ] != '\n' )
		line--;
	return( line );
}

char *__inline getlineend( struct Data *data, char *p )
{
	char *le;
	for( le = p; *le && *le != '\n'; le++ );
	if( le != p )
		le--;
	return( le );
}

int __inline getlinelenline( char *line )
{
	int l;
	for( l = 0; line[ l ] && line[ l ] != '\n'; l++ );
	return( l );
}

int __inline getlinelen( struct Data *data, int l )
{
	char *p = getline( data, l );
	int len = 0;

	while( *p && *p != '\n' )
		len++, p++;

	return( len );
}

char *__inline getwordstart( struct Data *data, char *p )
{
	char *ws;
	for( ws = p; ws > data->buffer && ( isalnum( (int)*ws ) || *ws == '\'' ); ws-- );
	if( ws != p && !isalnum( (int)*ws ) && *ws != '\'' )
		ws++;
	return( ws );
}

char *__inline getwordend( struct Data *data, char *p )
{
	char *we;
	for( we = p; *we && ( isalnum( (int)*we ) || *we == '\'' ); we++ );
	if( we != p )
		we--;
	return( we );
}

char *__inline getnumstart( struct Data *data, char *p )
{
	char *ws;
	for( ws = p; ws > data->buffer && tisnum( *ws ); ws-- );
	if( ws != p && !tisnum( *ws ) )
		ws++;
	if( ws > data->buffer && ws[ -1 ] == '-' )
		ws--;
	return( ws );
}

char *__inline getnumend( struct Data *data, char *p )
{
	char *we = p;
	if( *we == '-' )
		we++;
	while( *we && tisnum( *we ) )
		we++;
	if( we != p )
		we--;
	return( we );
}

#ifndef MBX
//	sprintf() replacement
UWORD fmtfunc[] = { 0x16c0, 0x4e75 };
void __stdargs sprintf( char *to, char *fmt, ... )
{
	RawDoFmt( fmt, &fmt + 1, (APTR)fmtfunc, to );
}
#endif

long myatoi( char *string )
{
	long v=0;
	if (!string)
		return 0;
	while (*string)
	{
		if (*string>='0' || *string<='9')
		{
			v*=10;
			v+=*string-'0';
		}
		string++;
	}
	return( v );
}

#ifndef MBX
struct MenuItem *FindCommandKeyInMenuItem(struct MenuItem *mi,BYTE key, int *itemcount, int *subcount )
{
	*itemcount = 0;
	if( subcount )
		*subcount = NOSUB;
	for(;mi;mi=mi->NextItem,*itemcount = *itemcount+1)
	{
		if ( ( mi->Flags & COMMSEQ ) && ( tolower( mi->Command ) == key ) )
		{
			return( mi );
		}
		else if( mi->SubItem && subcount )
		{
			struct MenuItem *rc;
			if (rc = FindCommandKeyInMenuItem(mi->SubItem,key,subcount,NULL))
				return(rc);
		}
	}
	return(NULL);
}

struct MenuItem *FindCommandKeyInMenu(struct Menu *m,BYTE key,UWORD *menunum)
{
	struct MenuItem *rc;
	int menucount = 0, itemcount, subcount;

	for (;m;m=m->NextMenu,menucount++)
	{
		if (rc=FindCommandKeyInMenuItem(m->FirstItem,key,&itemcount,&subcount))
		{
			*menunum = FULLMENUNUM( menucount, itemcount, subcount );
			return(rc);
		}
	}
	return(NULL);
}
#endif


#if 0 // not needed atm for MBX

#ifdef ParseIX
#undef ParseIX
#endif
#define ParseIX localParseIX

/* constants for InputXpression.ix_QualSame */
#define IXSYM_SHIFT 1	/* left- and right- shift are equivalent     */
#define IXSYM_CAPS  2	/* either shift or caps lock are equivalent  */
#define IXSYM_ALT   4	/* left- and right- alt are equivalent	     */

#define IXSYM_SHIFTMASK (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)
#define IXSYM_CAPSMASK	(IXSYM_SHIFTMASK | IEQUALIFIER_CAPSLOCK)
#define IXSYM_ALTMASK	(IEQUALIFIER_LALT | IEQUALIFIER_RALT)

#define IX_NORMALQUALS	0x7FFF	 /* avoid RELATIVEMOUSE */


typedef struct paType
{
    STRPTR name;
    UDWORD  value;
} pix_S;


pix_S pix_Class[]=
    {
	{"DISKINSERTED",
	IECLASS_DISKINSERTED},

	{"DISKREMOVED",
	IECLASS_DISKREMOVED},

	{"EVENT",
	IECLASS_EVENT},

	{"NEWPREFS",
	IECLASS_NEWPREFS},

	{"POINTERPOS",
	IECLASS_POINTERPOS},

	{"RAWKEY",
	IECLASS_RAWKEY},

	{"RAWMOUSE",
	IECLASS_RAWMOUSE},

	{"TIMER",
	IECLASS_TIMER},

	{NULL,
	0}
    };

pix_S pix_IEvent[]=
    {
	{"CAPSLOCK",
	IEQUALIFIER_CAPSLOCK},

	{"CAPS_LOCK",
	IEQUALIFIER_CAPSLOCK},

	{"CONTROL",
	IEQUALIFIER_CONTROL},

	{"CTRL",
	IEQUALIFIER_CONTROL},

	{"LALT",
	IEQUALIFIER_LALT},

	{"LAMIGA",
	IEQUALIFIER_LCOMMAND},

	{"LBUTTON",
	IEQUALIFIER_LEFTBUTTON},

	{"LCOMMAND",
	IEQUALIFIER_LCOMMAND},

	{"LEFTBUTTON",
	IEQUALIFIER_LEFTBUTTON},

	{"LEFT_ALT",
	IEQUALIFIER_LALT},

	{"LEFT_AMIGA",
	IEQUALIFIER_LCOMMAND},

	{"LEFT_BUTTON",
	IEQUALIFIER_LEFTBUTTON},

	{"LEFT_COMMAND",
	IEQUALIFIER_LCOMMAND},

	{"LEFT_SHIFT",
	IEQUALIFIER_LSHIFT},

	{"LSHIFT",
	IEQUALIFIER_LSHIFT},

	{"MBUTTON",
	IEQUALIFIER_MIDBUTTON},

	{"MIDBUTTON",
	IEQUALIFIER_MIDBUTTON},

	{"MIDDLEBUTTON",
	IEQUALIFIER_MIDBUTTON},

	{"MIDDLE_BUTTON",
	IEQUALIFIER_MIDBUTTON},

	{"NUMERICPAD",
	IEQUALIFIER_NUMERICPAD},

	{"NUMERIC_PAD",
	IEQUALIFIER_NUMERICPAD},

	{"NUMPAD",
	IEQUALIFIER_NUMERICPAD},

	{"NUM_PAD",
	IEQUALIFIER_NUMERICPAD},

	{"RALT",
	IEQUALIFIER_RALT},

	{"RAMIGA",
	IEQUALIFIER_RCOMMAND},

	{"RBUTTON",
	IEQUALIFIER_RBUTTON},

	{"RCOMMAND",
	IEQUALIFIER_RCOMMAND},

	{"RELATIVEMOUSE",
	IEQUALIFIER_RELATIVEMOUSE},

	{"REPEAT",
	IEQUALIFIER_REPEAT},

	{"RIGHTBUTTON",
	IEQUALIFIER_RBUTTON},

	{"RIGHT_ALT",
	IEQUALIFIER_RALT},

	{"RIGHT_AMIGA",
	IEQUALIFIER_RCOMMAND},

	{"RIGHT_BUTTON",
	IEQUALIFIER_RBUTTON},

	{"RIGHT_COMMAND",
	IEQUALIFIER_RCOMMAND},

	{"RIGHT_SHIFT",
	IEQUALIFIER_RSHIFT},

	{"RSHIFT",
	IEQUALIFIER_RSHIFT},

	{NULL,
	0}
    };

pix_S pix_Synonyms[]=
    {
	{"ALT",
	IXSYM_ALT},

	{"CAPS",
	IXSYM_CAPS},

	{"SHIFT",
	IXSYM_SHIFT},

	{NULL,
	0}
    };

pix_S pix_Upstroke[]=
    {
	{"UPSTROKE",
	0x1},

	{NULL,
	0}
    };


pix_S pix_Highmap[]=
    {
	{"BACKSPACE",
	0x41},

	{"BREAK",
	0x6e},

	{"COMMA",
	0x38},

	{"CURSOR_DOWN",
	0x4d},

	{"CURSOR_LEFT",
	0x4f},

	{"CURSOR_RIGHT",
	0x4e},

	{"CURSOR_UP",
	0x4c},

	{"DEL",
	0x46},

	{"DELETE",
	0x46},

	{"DOWN",
	0x4d},

	{"END",
	0x71},

	{"ENTER",
	0x43},

	{"ESC",
	0x45},

	{"ESCAPE",
	0x45},

	{"F1",
	0x50},

	{"F10",
	0x59},

	{"F11",
	0x4b},

	{"F12",
	0x6f},

	{"F2",
	0x51},

	{"F3",
	0x52},

	{"F4",
	0x53},

	{"F5",
	0x54},

	{"F6",
	0x55},

	{"F7",
	0x56},

	{"F8",
	0x57},

	{"F9",
	0x58},

	{"HELP",
	0x5f},

	{"HOME",
	0x70},

	{"INSERT",
	0x47},

	{"LEFT",
	0x4f},

	{"PAGE_DOWN",
	0x49},

	{"PAGE_UP",
	0x48},

	{"PAUSE",
	0x6e},

	{"RETURN",
	0x44},

	{"RIGHT",
	0x4e},

	{"SPACE",
	0x40},

	{"SPACEBAR",
	0x40},

	{"TAB",
	0x42},

	{"UP",
	0x4C},

/*	{NULL,
	0}
    };

pix_S pix_Extra[]=
    {
*/
	{"MOUSE_LEFTPRESS",
	0x68},

	{"MOUSE_MIDDLEPRESS",
	0x6a},

	{"MOUSE_RIGHTPRESS",
	0x69},

	{"(",
	0x5a},

	{")",
	0x5b},

	{"*",
	0x5d},

	{"+",
	0x5e},

	{"-",
	0x4a},

	{".",
	0x3c},

	{"/",
	0x5c},

	{"0",
	0x0f},

	{"1",
	0x1d},

	{"2",
	0x1e},

	{"3",
	0x1f},

	{"4",
	0x2d},

	{"5",
	0x2e},

	{"6",
	0x2f},

	{"7",
	0x3d},

	{"8",
	0x3e},

	{"9",
	0x3f},

	{NULL,
	0}
    };


BOOL IsSeparator(char a)
{
	if (a==' ' || a=='\n' || a=='\t' || a==',' || a=='\0')
		return TRUE;
	else
		return FALSE;
}


VOID GetNext(STRPTR *str)
{
	while(!(IsSeparator(**str)))
		(*str)++;

	while(IsSeparator(**str) && !(**str=='\0'))
		(*str)++;
}


BOOL pMatch (pix_S words[], STRPTR string, LONG *v, BOOL *dash)
{
	STRPTR nstr = string;
	int    i;

	if(*dash)
	{
		if(*nstr == '-')
		{
			nstr++;
			*dash = TRUE;
		}
		else
			*dash = FALSE;
	}

	for(i = 0; words[i].name != NULL; i++)
	{
		if(strnicmp(nstr, words[i].name, strlen(words[i].name)) == 0)
		{
			*v = words[i].value;
			return TRUE;
		}
	}

	return FALSE;
}

LONG ParseIX(STRPTR desc,IX *ix)
{
	LONG val;
	BOOL dash, upstrdash = TRUE;
	BOOL upstroke = FALSE;
	struct InputEvent event;

	/* Set as standard if no class is specified in the description */
	ix->ix_Class = IECLASS_RAWKEY;
	ix->ix_Code = 0;
	ix->ix_CodeMask = 0xFFFF;
	ix->ix_Qualifier = 0;
	//ix->ix_QualMask = IX_NORMALQUALS & ~(IEQUALIFIER_INTERRUPT | IEQUALIFIER_MULTIBROADCAST);
	ix->ix_QualMask = IX_NORMALQUALS;
  	ix->ix_QualSame = 0;

	if(desc == NULL)
	{
		ix->ix_Code = 0xFFFF;
		return -2;
	}

	while(IsSeparator(*desc))
		desc++;

	dash = FALSE;

	if(pMatch(pix_Class, desc, &val, &dash))
	{
		ix->ix_Class = val;
		GetNext(&desc);
	}

	while(1)
	{
		dash = TRUE;

		if(pMatch(pix_IEvent, desc, &val, &dash))
		{
			if(dash)
				ix->ix_QualMask &= ~val;
			else
				ix->ix_Qualifier |= val;

			GetNext(&desc);
		}
		else
		{
			dash = TRUE;

			if(pMatch(pix_Synonyms, desc, &val, &dash))
			{
				ix->ix_QualSame |= val;

				if(dash)
				{
					switch(val)
					{
						case IXSYM_SHIFT:
							ix->ix_QualMask &= ~IXSYM_SHIFTMASK;
							break;

						case IXSYM_ALT:
							ix->ix_QualMask &= ~IXSYM_ALTMASK;
							break;

						case IXSYM_CAPS:
							ix->ix_QualMask &= ~IXSYM_CAPSMASK;
							break;
					}
				}
				else
				{
					switch(val)
					{
						case IXSYM_SHIFT:
							ix->ix_Qualifier |= IXSYM_SHIFTMASK;
							break;

						case IXSYM_ALT:
							ix->ix_Qualifier |= IXSYM_ALTMASK;
							break;

						case IXSYM_CAPS:
							ix->ix_Qualifier |= IXSYM_CAPSMASK;
							break;
					}
				}

				GetNext(&desc);
			}
			else
				break;
		}
	}

	if(pMatch(pix_Upstroke, desc, &val, &upstrdash))
	{
		upstroke = TRUE;
		GetNext(&desc);
	}

	dash = FALSE;

	if(pMatch(pix_Highmap, desc, &val, &dash))
	{
		ix->ix_Code = val;
	}
	else
	{
		if (*desc != '\0')
		{
/*
			if (InvertKeyMap(*desc, &event, NULL))
				ix->ix_Code = event.ie_Code;
*/
		}
	}

	if (upstroke)
	{
		if(upstrdash)
			ix->ix_CodeMask &= ~IECODE_UP_PREFIX;
		else
			ix->ix_Code |= IECODE_UP_PREFIX;
	}

	while (!(IsSeparator(*desc)))
		desc++;

	if (*desc == '\0')
		return 0;
	else
		return -1;
}


BOOL MatchIX(struct InputEvent *event,IX *ix)
{
	UWORD temp = 0;
	UWORD qual;

	if(ix->ix_Class == IECLASS_NULL)
		return TRUE;

	if(event->ie_Class != ix->ix_Class)
		return FALSE;

	//    kprintf("Code: ie %i ix: %i\n", event->ie_Code, ix->ix_Code);

	if(((ix->ix_Code ^ event->ie_Code) & ix->ix_CodeMask) != 0)
		return FALSE;

	if((qual = ix->ix_QualSame) != 0)
	{
		if((qual & ~IXSYM_SHIFT) != 0)
		{
			if((event->ie_Qualifier & IXSYM_SHIFTMASK) != 0)
				temp |= IXSYM_SHIFTMASK;
		}

		if((qual & ~IXSYM_CAPS) != 0)
		{
			if((event->ie_Qualifier & IXSYM_CAPSMASK) != 0)
				temp |= IXSYM_CAPSMASK;
		}

		if((qual & ~IXSYM_ALT) != 0)
		{
			if((event->ie_Qualifier & IXSYM_ALTMASK) != 0)
				temp |= IXSYM_ALTMASK;
		}
	}

	if(((temp ^ ix->ix_Qualifier) & ix->ix_QualMask) == 0)
		return TRUE;
	else
		return FALSE;
}

#endif // MBX


#ifndef MBX

BOOL myParseIX( STRPTR keyspec, IX *ix )
{
	LONG res = ParseIX( keyspec, ix );

	if( !res )
	{
		if( CxBase->lib_Version <= 37 )
		{
			if( strstr(keyspec,"-upstroke") )
				ix->ix_CodeMask &= ~128;
		}
	}

	return((BOOL)res);
}


IX *parseix( STRPTR keyspec )
{
	struct parsedix *pix;

	ObtainSemaphoreShared( &pixsem );
	pix = (struct parsedix *)FindName( (struct List *)&pixlist, keyspec );
	ReleaseSemaphore( &pixsem );

	if( !pix )
	{
		pix = AllocVec( sizeof( *pix ) + strlen( keyspec ) + 1, MEMF_CLEAR );
		pix->n.ln_Name = (char *)( pix + 1 );
		strcpy( pix->n.ln_Name, keyspec );
		pix->ix.ix_Class = 255;
		myParseIX( keyspec, &pix->ix );
		ObtainSemaphore( &pixsem );
		AddTail( (struct List *)&pixlist, (struct Node *)pix );
		ReleaseSemaphore( &pixsem );

/*
#define DUMP(x) kprintf( #x "=%lx/%ld\n", pix->ix.x, pix->ix.x );

		DUMP( ix_Version )
		DUMP( ix_Class )
		DUMP( ix_Code )
		DUMP( ix_CodeMask )
		DUMP( ix_Qualifier )
		DUMP( ix_QualMask )
		DUMP( ix_QualSame )
*/
	}

	return( &pix->ix );
}

#endif


#ifndef MBX

void makefontarray( struct TextFont *tf, UBYTE *fa )
{
	int c;
	UWORD *space = tf->tf_CharSpace, *kern = tf->tf_CharKern;

	if( !( tf->tf_Flags & FPF_PROPORTIONAL ) )
	{
		memset( fa, tf->tf_XSize, 256 );
		return;
	}

	for( c = 0; c < 256; c++ )
	{
		int ch;

		if( c < tf->tf_LoChar || c > tf->tf_HiChar )
		{
			ch = tf->tf_HiChar - tf->tf_LoChar + 1;
		}
		else
		{
			ch = c - tf->tf_LoChar;
		}
		fa[ c ] = kern[ ch ] + space[ ch ];
	}
}

struct fontcache *fc_find( struct TextFont *tf )
{
	struct fontcache *n;

	ObtainSemaphore( &pixsem );
	for( n = FIRSTNODE( &fontcachelist ); NEXTNODE( n ); n = NEXTNODE( n ) )
	{
		if( n->tf == tf )
		{
			n->usecount++;
			ReleaseSemaphore( &pixsem );
			return( n );
		}
	}

	n = AllocMem( sizeof( *n ), MEMF_PUBLIC );
	makefontarray( tf, n->fwarray );
	n->usecount = 1;
	n->tf = tf;
	ADDTAIL( &fontcachelist, n );

	ReleaseSemaphore( &pixsem );
	return( n );
}

void fc_free( struct fontcache *fc )
{
	ObtainSemaphore( &pixsem );
	if( !--fc->usecount )
	{
		REMOVE( fc );
		FreeMem( fc, sizeof( *fc ) );
	}
	ReleaseSemaphore( &pixsem );
}

#endif // MBX



#if 0
static int myMatchIMsgIX( struct IntuiMessage *imsg, IX *ix )
{
	struct InputEvent ie;
	extern int __asm myMatchIX( register __a0 struct InputEvent *ie, register __a1 IX *ix );

	switch (imsg->Class)
	{
		case IDCMP_RAWKEY      : ie.ie_Class = IECLASS_RAWKEY  ; break;
		case IDCMP_MOUSEBUTTONS: ie.ie_Class = IECLASS_RAWMOUSE; break;
		default                : ie.ie_Class = IECLASS_NULL    ; break;
	}
	ie.ie_Code               = imsg->Code;
	ie.ie_Qualifier          = imsg->Qualifier;

	return( myMatchIX( &ie, ix ) );
}
#endif

#ifndef MBX
UBYTE isalnumtable[ 256 ];
#endif

void makealnumtable( void )
{
#ifndef MBX
	memset( &isalnumtable[ '0' ], 0xff, 10 );
	memset( &isalnumtable[ 'a' ], 0xff, 26 );
	memset( &isalnumtable[ 'A' ], 0xff, 26 );
	memset( &isalnumtable[ 192 ], 0xff, 64 );
#endif
}

BOOL __inline tisnum( char ch )
{
	return( (BOOL)(ch >= '0' && ch <= '9') );
}

#ifndef MBX
APTR __stdargs DoSuperNew( struct IClass *class, APTR obj, ULONG tag1, ... )
{
	return( (APTR)DoSuperMethod( class, obj, OM_NEW, &tag1, NULL ) );
}
#endif

void __stdargs tnotify( APTR obj, ... )
{
	CoerceMethod( notifyclass, obj, OM_SET, &obj + 1 );
}

int writeclip( char *data, int len )
{
	int rc = FALSE;

	#ifndef MBX

	struct IFFHandle *iffh;
	APTR cliphandle;
	int c;

	for( c = 0; c < len; c++ )
	{
		if( !data[ c ] )
			break;
	}
	len = c;

	if( iffh = AllocIFF() )
	{
		if( cliphandle = OpenClipboard( CLIPDEVICE ) )
		{
			iffh->iff_Stream = (ULONG)cliphandle;
			InitIFFasClip( iffh );

			if( !OpenIFF( iffh, IFFF_WRITE ) )
			{
				PushChunk( iffh, MAKE_ID( 'F','T','X','T' ), MAKE_ID( 'F','O','R','M' ), IFFSIZE_UNKNOWN );
				PushChunk( iffh, MAKE_ID( 'F','T','X','T' ), MAKE_ID( 'C','H','R','S' ), IFFSIZE_UNKNOWN );

				WriteChunkBytes( iffh, data, len );

				PopChunk( iffh );
				PopChunk( iffh );

				CloseIFF( iffh );

				rc = TRUE;
			}
			CloseClipboard( cliphandle );
		}
		FreeIFF( iffh );
	}

	#endif

	return( rc );
}
