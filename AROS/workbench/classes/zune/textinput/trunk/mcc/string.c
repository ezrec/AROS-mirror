/*
** $Id$
*/

#include "mystring.h"
#include "structs.h"
#include "textinput_mcc.h"
#include "textinput_cfg.h"
#include "misc.h"
#include "render.h"
#include "buffers.h"

#ifdef MBX

void findurls(struct Data *data)
{
}

AUDIOBASE;

#else

#include "extensions.h"
#include "extedit.h"
#include "rev.h"
#include "cat.h"
#include "cmanager.h"
#include "cmanager_protos.h"
#include "newmouse.h"

#ifdef _M68020
#define UserLibID "$VER: Textinput.mcc.020 " LVERTAG " © 1997-" COMPILEYEAR " Oliver Wagner <owagner@vapor.com>, All Rights Reserved\r\n"
#else
#define UserLibID "$VER: Textinput.mcc " LVERTAG " © 1997-" COMPILEYEAR " Oliver Wagner <owagner@vapor.com>, All Rights Reserved\r\n"
#endif

#define CLASS "Textinput.mcc"
#define SUPERCLASS MUIC_Area
#define _Dispatcher dispfunc
#define MASTERVERSION 18
#define ClassInit
#define ClassExit

#include "mccheader.h"

//#define DEBUGUNDO

//
// Globals
//
struct Library *VATBase;
struct Library *RexxSysBase, *KeymapBase, *DiskfontBase, *CxBase, *TimerBase, *IFFParseBase, *LayersBase;
#if INCLUDE_VERSION < 44
struct Library *LocaleBase;
#else
struct LocaleBase *LocaleBase;
#endif
struct Library *CyberGfxBase, *CManagerBase;
struct Catalog *catalog;

static char classinsertpath[ 200 ];

#endif // !MBX


//#define ASYNCSPELL

#define CHECKEDIT if(data->noedit){DisplayBeep(_screen(obj));return(-1);}

#define DOCOLOURS (data->styles==MUIV_Textinput_Styles_IRC||data->styles==MUIV_Textinput_Styles_Email||data->styles==MUIV_Textinput_Styles_HTML)

#define BSETTAG(t) (t->ti_Data?TRUE:FALSE)

static void stopblink( APTR obj, struct Data *data )
{
	if( data->blinkihnactive )
	{
		DoMethod(_app(obj),MUIM_Application_RemInputHandler,&data->ihn);
		data->blinkihnactive = FALSE;
	}
}

static void startblink( APTR obj, struct Data *data )
{
	stopblink( obj, data );
	if( data->blinkrate )
	{
		data->ihn.ihn_Object = obj;
		data->ihn.ihn_Millis = data->blinkrate * 50;
		data->ihn.ihn_Flags = MUIIHNF_TIMER;
		data->ihn.ihn_Method = MUIM_Textinput_Blink;
		DoMethod( _app( obj ), MUIM_Application_AddInputHandler, &data->ihn );
		data->blinkihnactive = TRUE;
	}
}

static void procset( APTR obj, APTR cl, struct Data *data, struct TagItem *tags );

DECCONST
{
	struct Data *data;
	struct TagItem *tag, *tagp;

	obj = (APTR) DoSuperNew( cl, obj,
		MUIA_FillArea, FALSE,
		InnerSpacing( 0, 0 ),
		TAG_MORE, msg->ops_AttrList
	);
	if( !obj )
		return( 0 );

/*
#ifdef MBX
// it doesnt work with fontaine prop fonts on mbx yet
// we test this on userdata 42
if (muiUserData(obj)!=42)
	set(obj,MUIA_Font,MUIV_Font_Fixed);
#endif
*/

	data = INST_DATA( cl, obj );

	data->popupstate = -1;

	tagp = msg->ops_AttrList;
	while((tag = NextTagItem( &tagp )))
	{
		switch( tag->ti_Tag )
		{
			case MUIA_Textinput_Format:
			case MUIA_String_Format:
				data->format = tag->ti_Data;
				break;

			case MUIA_Textinput_Multiline:
				data->multiline = BSETTAG( tag );
				break;

			case MUIA_Textinput_MaxLen:
			case MUIA_String_MaxLen:
				data->maxlen = tag->ti_Data;
				break;

			case MUIA_Textinput_MaxLines:
				data->maxlines = tag->ti_Data;
				break;

			case MUIA_Font:
				if( tag->ti_Data != MUIV_Font_Inherit )
					data->fontset = TRUE;
				break;

			case MUIA_Textinput_DefaultPopup:
				data->popupstate = BSETTAG( tag );
				break;

			case MUIA_Textinput_MinVersion:
				if( tag->ti_Data > VERSION )
				{
					#ifndef MBX
					MUI_Request( NULL, NULL, 0, "Textinput Class", "OK",GS( MINVER ), tag->ti_Data, VERSION);
					#endif
					goto problem;
				}
				break;

			case MUIA_Textinput_NoInput:
				data->noinput = BSETTAG( tag );
				break;
		}
	}

	data->font = _font( obj );
	
	if( !data->maxlen )
		data->maxlen = data->multiline ? 2047 : 79;

	if( data->multiline || data->noinput )
		data->autoexpand = TRUE;

	if( !data->multiline )
		data->setvmax = TRUE;

	if( data->noinput )
	{
		if( !data->multiline )
		{
			data->setmin = TRUE;
			data->setvmin = TRUE;
		}
		data->clickableurls = TRUE;
		data->patternback = TRUE;
		data->styles = MUIV_Textinput_Styles_MUI;
	}
	else
	{
//		data->spellcheck = TRUE; //!!!!!!TEMP!
		data->styles = MUIV_Textinput_Styles_None;
		data->maxundobytes = 2048;
	}

	// !!!!!!TEMP!!!!!!!!!
	data->tabs = MUIV_Textinput_Tabs_Spaces;
	data->tablen = 4;

	data->smooth = 0;

	NEWLIST( &data->undolist );
	NEWLIST( &data->redolist );

	if( !( data->pool = CreatePool( 0, 256, 128 ) ) )
		goto problem;
	if( !( data->buffer = AllocVecPooled( data->pool, data->maxlen + 1 ) ) )
		goto problem;
	data->buffer[0] = 0;

	data->cursorp = data->buffer;

	muiAreaData( obj )->mad_Flags |= MADF_KNOWSACTIVE;

	data->markstart = data->rmarkstart = data->markend = data->rmarkend = NOMARK;

	strcpy( data->tmpextension, "titmp" );

	data->wordwrap = 70;
	if( data->multiline )
		data->dowordwrap = TRUE;

	data->minnumval = MININT;
	data->maxnumval = MAXINT;

	procset( obj, cl, data, msg->ops_AttrList );

#ifdef ASYNCSPELL
	openrexxport( data );
#endif

	return( (ULONG)obj );

	problem:
	if( data->pool )
	{
		if( data->buffer )
			FreeVecPooled( data->pool, data->buffer );
		DeletePool( data->pool );
	}
	return( NULL );
}

DECDEST
{
	GETDATA;

	#ifndef MBX
	if( data->eeh )
	{
		ee_free( data->eeh );
		data->eeh = NULL;
		DeleteFile( data->tmpfilename );
		data->externaledit = FALSE;
	}
	#endif

#ifdef ASYNCSPELL
	closerexxport( data );
#endif

	if (data->undobuffer)
		FreeVec(data->undobuffer);

	if( data->pool )
		DeletePool( data->pool );

	return( DOSUPER );
}

static struct viewurl *urlselect( struct Data *data, int x, int y )
{
	char *line = getline( data, y );
	struct viewurl *vu;
	char *p = line + x;
	int currpos = p - data->buffer;
	for( vu = data->urls; vu; vu = vu->next )
	{
		if( ( vu->word.xstart <= currpos && vu->word.xstart + vu->word.xsize >= currpos ) )
		{
			vu->selected = TRUE;
			return( vu );
		}
	}
	return( NULL );
}

static struct viewurl *urldeselect( struct Data *data )
{
	struct viewurl *vu = data->urls;
	while( vu )
	{
		if( vu->selected )
		{
			vu->selected = FALSE;
			return( vu );
		}
		vu = vu->next;
	}
	return( NULL );
}

static void findcursorpos( APTR obj, struct Data *data, int *pxo, int *cellsize )
{
	char *line;

	line = getline( data, data->cursory );

	if( data->cursorx )
		*pxo = textlen( data, line, data->cursorx );
	else
		*pxo = 0;

	if( data->fontfixed )
	{
		*cellsize = data->fontfixed;
	}
	else
	{
		#ifdef MBX
		char *str = line + data->cursorx;
//		*cellsize = data->font->tf_Width[ch];
		if (!*str || *str == '\n')
			str = "n";
		*cellsize = TextWidth(data->font, str, CHARLEN(*str));
		data->negativekerning = NEGKERN( str );

		#else

		int ch = data->issecret ? SECRETCHAR : line[data->cursorx];

		if (!ch || ch=='\n')
			ch = 'n';

		if( ch < data->font->tf_LoChar || ch > data->font->tf_HiChar )
			ch = data->font->tf_HiChar + 1;
		ch -= data->font->tf_LoChar;

		/* bogus crap ?!?
		if( ch && ch != '\n' )
			ch = ( ch > data->font->tf_HiChar ) ? ( data->font->tf_HiChar + 1 ) : (int)(ch) - (int)data->font->tf_LoChar;
		else
			ch = (int)'n' - (int)data->font->tf_LoChar;
		if( ch < 0 )
			ch = data->font->tf_HiChar - data->font->tf_LoChar + 1;
		*/

		*cellsize = ((UWORD *)data->font->tf_CharSpace)[ch] + ((WORD *)data->font->tf_CharKern)[ch];
		data->negativekerning = NEGKERN( (char *)&ch );
		#endif
	}
}

static int getxycharpos( struct RastPort *rp, APTR obj, struct Data *data, int mx, int my, int *line, int *xo, int *outside )
{
	char *lb;
	int ll, lo;
	int outofbounds = FALSE;

	if( outside )
		*outside = FALSE;

	// first, determine line
	my -= _mtop( obj ) + YSPACING + data->ycenter - data->yoffset + data->topline * data->lineheight;
	if( my < 0 )
	{
		*line = max( 0, data->topline - 1 + my / data->lineheight );
		outofbounds = TRUE;
	}
	else if( my >= ( data->visiblelines * data->lineheight ) )
	{
		my -= data->visiblelines * data->lineheight;
		*line = max( 0, min( data->linenum - 1, data->topline + data->visiblelines + my / data->lineheight ) );
		outofbounds = TRUE;
	}
	else
	{
		my /= data->lineheight;
		my += data->topline;
		if( my > data->linenum - 1)
		{
			if( outside )
				*outside = TRUE;
			my = data->linenum - 1;
		}
		*line = max( 0, my );
	}

	lb = getline( data, *line );
	ll = getlinelenline( lb );
	lo = getlinexoffset( obj, data, lb );

	if( mx < _mleft( obj ) + XSPACING )
	{
		mx -= _mleft( obj ) + XSPACING;
		outofbounds = TRUE;
		*xo = findcharoffset( data, lb, ll, data->xoffset + mx - lo, outside );
	}
	else if( mx > _mright( obj ) - XSPACING )
	{
		mx -= _mright( obj ) - XSPACING;
		outofbounds = TRUE;
		*xo = findcharoffset( data, lb, ll, _mwidth( obj ) - XSPACING * 2 + data->xoffset + mx - lo, outside );
	}
	else
	{
		mx -= _mleft( obj ) + XSPACING - data->xoffset;
		*xo = findcharoffset( data, lb, ll, mx - 1 - lo, outside );
	}

	return( !outofbounds );
}

static void doinform( struct Data *data, APTR obj )
{
	if( data->scrollobj )
	{
		DoMethod( data->scrollobj, MUIM_Textinputscroll_Inform, 
			data->xoffset, data->topline * data->lineheight,
			data->maxxsize, data->linenum * data->lineheight,
			_mwidth( obj ) - XSPACING * 2, data->visiblelines * data->lineheight,
			data->noedit || data->noinput
		);
	}
}

static void docheckinform( struct Data *data, APTR obj )
{
	if( data->needsinform && data->scrollobj )
	{
		data->needsinform = FALSE;

		if( data->old_xoffset != data->xoffset ||
			data->old_topline != data->topline ||
			data->old_maxx != data->maxxsize ||
			data->old_linenum != data->linenum )
		{
			data->old_xoffset = data->xoffset;
			data->old_topline = data->topline;
			data->old_maxx = data->maxxsize;
			data->old_linenum = data->linenum;
			doinform( data, obj );
		}
	}
	else
		data->old_linenum = data->linenum;
}

void endchange( APTR obj, struct Data *data, ULONG excludeattr )
{
	data->changed = TRUE;
	findurls( data );
	tnotify( obj,
		MUIA_Textinput_Contents, data->buffer,
		MUIA_String_Contents, data->buffer,
		MUIA_Text_Contents, data->buffer,
		MUIA_Textinput_Changed, TRUE,
		TAG_DONE
	);
}

static void calcmaxvals( struct Data *data, APTR obj )
{
	char *l, *ll;

	data->linenum = 0;
	if( muiRenderInfo( obj ) )
		data->maxxsize = _mwidth( obj ) - XSPACING * 2;
	else
		data->maxxsize = 0;
	data->maxlinewidth = 0;

	for( l = data->buffer; ; l = ll )
	{
		ll = l;

		//Printf( "chk %s\n", ll );

		while( *ll && *ll != '\n' )
			INCSTRPTR(ll);

		data->linenum++;
		data->maxlinewidth = max( data->maxlinewidth, textlen( data, l, ll - l ) );
		data->maxxsize = max( data->maxxsize, data->maxlinewidth );

		if (!*ll)
			break;

		INCSTRPTR(ll);
	}

	//Printf( "linenum %ld\n", data->linenum );

	if( data->linenum != data->old_linenum )
	{
		tnotify( obj, MUIA_Textinput_Lines, data->linenum, TAG_DONE );
	}

	data->needsinform = TRUE;
}

static void calcmaxvalline( struct Data *data, APTR obj, int line )
{
	char *lp = getline( data, line );
	int len = getlinelenline( lp );
	int newmax = textlen( data, lp, len );

	if( newmax > data->maxlinewidth )
		data->maxlinewidth = newmax;

	if( newmax > data->maxxsize )
	{
		data->maxxsize = newmax;
		data->needsinform = TRUE;
	}
}

static int checkcursorvisible( struct Data *data, APTR obj )
{
	int didscroll = FALSE;

	if( data->cursory * data->lineheight < data->yoffset )
	{
		data->topline = data->cursory;
		data->yoffset = data->topline * data->lineheight;
		didscroll = TRUE;
	}
	else if( data->cursory >= ( data->topline + data->visiblelines ) )
	{
		data->topline = data->cursory - data->visiblelines + 1;
		data->yoffset = data->topline * data->lineheight;
		didscroll = TRUE;
	}

	findcursorpos( obj, data, &data->pxo, &data->cellsize );

	if( data->pxo + getlinexoffset( obj, data, data->cursorp ) < data->xoffset )
	{
		data->xoffset = data->pxo;
		didscroll = TRUE;
	}
	else if( ( data->pxo + getlinexoffset( obj, data, data->cursorp ) + data->cellsize - 1 ) >= ( _mwidth( obj ) - XSPACING * 2 + data->xoffset ) )
	{
		data->xoffset = ( data->pxo + getlinexoffset( obj, data, data->cursorp ) + data->cellsize ) - ( _mwidth( obj ) - XSPACING * 2 );
		didscroll = TRUE;
	}

	if( didscroll )
	{
		data->needsinform = TRUE;
	}

	if( didscroll )
		data->redrawmode = RDO_SCROLL;

	return( didscroll );
}

DECMMETHOD( Draw )
{
	GETDATA;
	struct RastPort *rp;
	struct clipbuff cb;

	DOSUPER;

#ifdef MBX
	msg-> flags = MADF_DRAWOBJECT; // FIXME TOFIX !!!
#endif

	rp = _rp( obj );
	SetFont( rp, data->font );

#ifdef USEWRITEMASK
	if( !data->patternback )
	{
		if( CyberGfxBase || GFX39 )
			SetMaxPen( rp, data->maxpen );
		else
			SetWrMsk( rp, data->maxpen );
	}
#endif

	if( msg->flags & MADF_DRAWOBJECT )
	{
		FillBack( data, obj, 0, 0, _mwidth( obj ) - 1, _mheight( obj ) - 1 );
		msg->flags |= MADF_DRAWUPDATE;
		data->redrawmode = RDO_CONTENTS;
	}

	if( data->redrawmode == RDO_SCROLL )
	{
		FillBack( data, obj, 0, 0, _mwidth( obj ) - 1, YSPACING ); // Top border
		FillBack( data, obj, 0, _mheight( obj ) - YSPACING - 1, _mwidth( obj ) - 1, _mheight( obj ) - 1 ); // Bottom border
		FillBack( data, obj, 0, 0, XSPACING, _mheight( obj ) - 1 ); // Left border
		FillBack( data, obj, _mwidth( obj ) - XSPACING - 1, 0, _mwidth( obj ) - 1, _mheight( obj ) - 1 ); // Right border
		data->redrawmode = RDO_CONTENTS;
	}

	if( data->redrawmode == RDO_FROMCURSOR || data->redrawmode == RDO_FROMCURSOR_PREV )
	{
		if( data->format != MUIV_Textinput_Format_Left || data->spellcheck || PARSEURLS )
			data->redrawmode = RDO_CURRENT;
	}

	if( data->redrawmode != RDO_NONE )
	{
		quickaddclip( obj, _mleft( obj ) + XSPACING, _mtop( obj ) + YSPACING, _mwidth( obj ) - XSPACING * 2, _mheight( obj ) - YSPACING * 2, &cb );
		switch( data->redrawmode )
		{
			case RDO_MOVECURSOR:
				drawcursor( rp, obj, data, TRUE );
				data->cursorstate = FALSE;
			case RDO_CURSOR:
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_MOVEMARK:
				drawcursor( rp, obj, data, TRUE );
				data->cursorstate = FALSE;
				if( data->oldmarkstart == NOMARK || data->oldmarkend == NOMARK )
				{
					if( data->markstart != NOMARK && data->markend != NOMARK )
						drawpart( rp, obj, data, data->markstart, data->markend );
				}
				else if( data->markstart == NOMARK || data->markend == NOMARK )
				{
					drawpart( rp, obj, data, data->oldmarkstart, data->oldmarkend );
				}
				else if( data->markstart > data->oldmarkend || data->markend < data->oldmarkstart )
				{
					drawpart( rp, obj, data, data->oldmarkstart, data->oldmarkend );
					drawpart( rp, obj, data, data->markstart, data->markend );
				}
				else
				{
					if( data->markstart != data->oldmarkstart )
						drawpart( rp, obj, data, STRMIN( data->markstart, data->oldmarkstart ), STRMAX( data->markstart, data->oldmarkstart ) - 1 );
					if( data->markend != data->oldmarkend )
						drawpart( rp, obj, data, STRMIN( data->markend, data->oldmarkend ) + 1, STRMAX( data->markend, data->oldmarkend ) );
				}
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_URL:
				if( PARSEURLS )
				{
					char *line;
					int x, lineno;
					findxyp( data, data->redrawurl->word.xstart + data->buffer, &x, &lineno, &line );
					underlinesegment( data, obj, &data->redrawurl->word, line, lineno, data->redrawurl->selected ? data->pens[ pen_s_url ] : data->pens[ pen_u_url ] );
				}
				break;

			case RDO_MISSPELL:
				if( data->spellcheck )
				{
					char *line;
					int x, lineno;
					findxyp( data, data->redrawmisspell->word.xstart + data->buffer, &x, &lineno, &line );
					underlinesegment( data, obj, &data->redrawmisspell->word, line, lineno, data->pens[ pen_u_misspell ] );
				}
				break;

			case RDO_CURRENT:
				drawline( rp, obj, data, getline( data, data->cursory ), data->cursory - data->topline );
				data->cursorstate = FALSE;
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_FROMCURSOR:
				drawlinefromcursor( rp, obj, data, data->cursorp, data->cursory - data->topline, TRUE );
				data->cursorstate = FALSE;
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_FROMCURSOR_PREV:
				drawlinefromcursor( rp, obj, data, data->cursorp - 1, data->cursory - data->topline, FALSE );
				data->cursorstate = FALSE;
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_CURRENT_PREV:
				drawline( rp, obj, data, getline( data, data->cursory - 1 ), data->cursory - data->topline - 1 );
				drawline( rp, obj, data, getline( data, data->cursory ), data->cursory - data->topline );
				data->cursorstate = FALSE;
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_CURRENT_NEXT:
				drawline( rp, obj, data, getline( data, data->cursory + 1 ), data->cursory - data->topline + 1 );
				drawline( rp, obj, data, getline( data, data->cursory ), data->cursory - data->topline );
				data->cursorstate = FALSE;
				drawcursor( rp, obj, data, FALSE );
				break;

			case RDO_CONTENTS:
				// draw contents
				if( data->multiline )
				{
					int c;
					int start = data->smooth && ( data->ycenter ) && data->topline ? -1 : 0;
//					int end = data->smooth && ( data->ycenter ) && ( data->topline + data->visiblelines < data->linenum ) ? data->visiblelines + 1 : data->visiblelines;
					int end = data->visiblelines + 2;
					int topoff = data->smooth && data->topline ? 0 : data->ycenter;
					int bottomoff = data->smooth && ( data->topline + data->visiblelines < data->linenum ) ? 0 : data->ycenter;
					if( topoff )
						FillBack( data, obj, XSPACING, YSPACING, _mwidth( obj ) - 1 - XSPACING, YSPACING + topoff );
					if( bottomoff )
						FillBack( data, obj, XSPACING, _mheight( obj ) - YSPACING - bottomoff - 1, _mwidth( obj ) - 1 - XSPACING, _mheight( obj ) - 1 - YSPACING );
					for( c = start; c < end; c++ )
						drawline( rp, obj, data, getline( data, data->topline + c ), c );
				}
				else
				{
					drawline( rp, obj, data, data->buffer, 0 );
				}
				if( data->isactive )
				{
					data->cursorstate = FALSE;
					drawcursor( rp, obj, data, FALSE );
				}
				break;
		}
		quickremclip( obj, &cb );
	}
	data->redrawmode = RDO_NONE;
	SetDrMd( rp, JAM1 );
	SetSoftStyle( rp, 0, FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC );
	SetBPen( rp, 0 );
#ifdef USEWRITEMASK
	if( !data->patternback )
	{
		if( CyberGfxBase || GFX39 )
			SetMaxPen( rp, ( 1L << rp->BitMap->Depth ) - 1 );
		else
			SetWrMsk( rp, 0xff );
	}
#endif

	if( _isdisabled( obj ) )
	{
		#ifdef MBX
		MUIG_DrawDisablePattern(muiRenderInfo(obj),_left(obj),_top(obj),_width(obj),_height(obj));
		#else
		static UWORD dr[] = { 0x4444, 0x1111 };
		SetAfPt( rp, dr, 1 );
		SetAPen( rp, _pens(obj)[MPEN_SHADOW]);
		RectFill( rp, _mleft( obj ), _mtop( obj ), _mright( obj ), _mbottom( obj ) );
		SetAfPt( rp, NULL, -1 );
		#endif
	}

	return( 0 );
}

static void dowordwrap( struct Data *data, char *startline )
{
	int x;

	if( !data->multiline || !data->dowordwrap )
		return;

	/* !!!!!!!!!!! Not undoable yet !!!!!!!!!!!!!!! */

	for( x = 0; ; x += CHARLEN(startline[x]) )
	{
		if( startline[x]=='\n' || !startline[x])
			return;
		if( !isspace(startline[x]))
			break;
	}

	// first, replace all blanks
	for( x = 0; x < data->wordwrap; x += CHARLEN(startline[x]) )
	{
		if(startline[x]=='\n')
		{
			if( !startline[ x + 1 ] || isspace( startline[ x + 1 ] ) || startline[ x + 1 ] == '>' )
				break;
			startline[x]=' ';
		}
	}
	// now go backwards until finding a space

	for( ;x > 0; x-- )
	{
		if( isspace( startline[ x ] ) )
		{
			startline[ x ] = '\n';
			dowordwrap( data, &startline[ x + 1 ] );
			return;
		}
	}
	// we can do nothing, really
}

#ifdef MBX
static ULONG handlechar( Class *cl, Object *obj, struct Data *data, STRPTR chstr, int quiet)
#define ch (*chstr)
#else
static ULONG handlechar( APTR cl, APTR obj, struct Data *data, int chint, int quiet )
#endif
{
	int rdm = data->negativekerning ? RDO_CURRENT : RDO_FROMCURSOR_PREV;
#ifndef MBX
	char ch = (char)chint;
#endif

	data->oldpxo = data->pxo;

	if( ch == 10 )
		return( 0 );
	else if( ch == 9 )
		ch = ' ';

#ifdef MBX
  	if( AudioBase )
  	 	SystemBeep( SYS_CLICK );
#endif

	if( ch == 13 && !data->multiline )
	{
		if( data->isnumeric )
		{
			long v = myatoi( data->buffer );

			if( v < data->minnumval )
			{
				set( obj, MUIA_Textinput_Integer, data->minnumval );
				DisplayBeep( _screen( obj ) );
				return( 0 );
			}
			else if( v > data->maxnumval )
			{
				set( obj, MUIA_Textinput_Integer, data->maxnumval );
				DisplayBeep( _screen( obj ) );
				return( 0 );
			}
		}

		DoMethod( obj, MUIM_Textinput_Acknowledge, data->buffer );
		if( !data->remainactive )
		{
			data->goinactive = TRUE;
		}
		return( 0 );
	}

	if( ( data->rejectchars && strchr( data->rejectchars, ch ) ) || ( data->acceptchars && !strchr( data->acceptchars, ch ) ) )
	{
		DisplayBeep( _screen( obj ) );
		return( FALSE );
	}

	if( data->isnumeric )
	{
		char *accept = "-0123456789";
		if( data->minnumval >= 0 )
			accept++;
		if( !strchr( accept, ch ) || ( ch == '-' && data->cursorx ) )
		{
			DisplayBeep( _screen( obj ) );
			return( FALSE );
		}
	}

	if( data->markmode == 3 && isstyle( data, &ch ) )
	{
		if( ( data->styles == MUIV_Textinput_Styles_IRC && ch == '\017' && *data->markstart == ch ) || ( *data->markstart == ch && *data->markend == ch ) )
		{
			if( data->markstart == data->markend || ( data->styles == MUIV_Textinput_Styles_IRC && *data->markstart == '\017' ) )
			{
				doaction( data, ACTION_DELETE, data->markstart, 1, NULL, 0 );
				if( data->markstart == data->markend )
					resetmarkmode( data );
				else
				{
					updatemark( data, data->markstart, data->markend - 1 );
					data->cursorp = STRMIN( data->cursorp, data->markend );
				}
			}
			else
			{
				doaction( data, ACTION_UNSURROUND, data->markstart, 1, NULL, data->markend - data->markstart - 1 );
				if( data->markstart == data->markend - 1 )
				{
					data->cursorp = data->markstart;
					resetmarkmode( data );
				}
				else
				{
					updatemark( data, data->markstart, data->markend - 2 );
					data->cursorp = STRMIN( data->cursorp, data->markend );
				}
			}
		}
		else
		{
			char *markend = *data->markend ? data->markend : data->markend - 1;
			if( markend < data->markstart || ( data->styles == MUIV_Textinput_Styles_IRC && ch == '\017' ) )
			{
#ifdef MBX
				if( !doaction( data, ACTION_INSERT, data->markstart, 0, chstr, CHARLEN( *chstr ) ) )
#else
				char chc = ch;
				if( !doaction( data, ACTION_INSERT, data->markstart, 0, &chc, 1 ) )
#endif
				{
					DisplayBeep( _screen( obj ) );
					return( FALSE );
				}
				if( data->cursorp == data->markend )
					INCSTRPTR(data->cursorp);
				updatemark( data, data->markstart, data->cursorp );
			}
			else
			{
#ifdef MBX
				if( !doaction( data, ACTION_SURROUND, data->markstart, markend - data->markstart + 1, chstr, CHARLEN( *chstr ) ) )
#else
				char chc = ch;
				if( !doaction( data, ACTION_SURROUND, data->markstart, markend - data->markstart + 1, &chc, 1 ) )
#endif

				{
					DisplayBeep( _screen( obj ) );
					return( FALSE );
				}
				if( data->cursorp == data->markend )
					data->cursorp += 2;
				updatemark( data, data->markstart, markend + 2 );
			}
		}
		findcursorxy( data, data->cursorp );
		data->redrawmode = RDO_CONTENTS;
		if( !quiet )
		{
			if( muiRenderInfo( obj ) )
				calcmaxvals( data, obj );
			checkcursorvisible( data, obj );
		}
	}
	else if(ch==13)
	{
		if( data->markmode == 3 )
		{
			if( !doaction( data, ACTION_REPLACE, data->markstart, data->markend - data->markstart + 1, "\n", 1 ) )
			{
				DisplayBeep( _screen( obj ) );
				return( FALSE );
			}
			findcursorxy( data, data->markstart );
			resetmarkmode( data );
		}
		else if( !doaction( data, ACTION_INSERT, data->cursorp, 0, "\n", 1 ) )
		{
			DisplayBeep( _screen( obj ) );
			return( FALSE );
		}
		INCSTRPTR(data->cursorp);
		data->cursorx = 0;
		data->cursory++;
		data->redrawmode = RDO_CONTENTS;
		if( !quiet )
		{
			if( muiRenderInfo( obj ) )
				calcmaxvals( data, obj );
			checkcursorvisible( data, obj );
		}
		else
			data->linenum++;
	}
	else
	{
#ifndef MBX
		char chc = ch;
#endif
		if( data->markmode == 3 )
		{
#ifdef MBX
			if( !doaction( data, ACTION_REPLACE, data->markstart, data->markend - data->markstart + 1, chstr, CHARLEN( *chstr ) ) )
#else
			if( !doaction( data, ACTION_REPLACE, data->markstart, data->markend - data->markstart + 1, &chc, 1 ) )
#endif
			{
				DisplayBeep( _screen( obj ) );
				return( FALSE );
			}
			rdm = RDO_CONTENTS;
			findcursorxy( data, data->markstart );
			checkcursorvisible( data, obj );
			resetmarkmode( data );
		}
#ifdef MBX
		else if( !doaction( data, ACTION_INSERT, data->cursorp, 0, chstr, CHARLEN( *chstr ) ) )
#else
		else if( !doaction( data, ACTION_INSERT, data->cursorp, 0, &chc, 1 ) )
#endif
		{
			DisplayBeep( _screen( obj ) );
			return( FALSE );
		}
		data->cursorx += CHARLEN(*data->cursorp);
		INCSTRPTR(data->cursorp);

		if( data->multiline && data->dowordwrap )
		{
			char *p = data->cursorp;
			int l = data->cursorx;

			while( *p && *p != '\n' )
				p++, l++;

			if( l > data->wordwrap )
			{
				dowordwrap( data, getline( data, data->cursory ) );
				findcursorxy( data, data->cursorp );
				data->redrawmode = RDO_CONTENTS;
				if( muiRenderInfo( obj ) )
				{
					calcmaxvals( data, obj );
					checkcursorvisible( data, obj );
				}
				endchange( obj, data, 0 );
				return( TRUE );
			}
		}
		if( !quiet )
		{
			if( muiRenderInfo( obj ) )
				calcmaxvalline( data, obj, data->cursory );
			data->redrawmode = rdm;
			checkcursorvisible( data, obj );
		}
	}

#ifndef MBX
	if( data->spellcheck && !isalnum( ch ) && ch != '\'' )
	{
		char *spellbuf;
		char *ws = getwordstart( data, data->cursorp - 2 );
		if( ws < data->cursorp - 1 )
		{
			char *checkcmd = "quickcheck ";
			if( spellbuf = AllocVec( data->cursorp - ws + strlen( checkcmd ) + 2, 0L ) )
			{
				strcpy( spellbuf, checkcmd );
				strncat( spellbuf, ws, data->cursorp - ws - 1 );
#ifdef ASYNCSPELL
				sendrexxcmd( data, "IRexxSpell", spellbuf, spellbuf, ws, obj );
#else
				openrexxport( data );
				if( sendrexxcmd( data, "IRexxSpell", spellbuf, spellbuf, ws, obj ) )
				{
					WaitPort( data->rexxPort );
					disprexxport( data );
				}
				closerexxport( data );
#endif
			}
		}
	}
#endif

	if( !quiet )
		endchange( obj, data, 0 );

	return( TRUE );
}
#ifdef MBX
#undef ch
#endif

int extendmark( struct Data *data )
{
	char *cursorp = data->cursorp;
	char *ws, *we;

	data->rmarkend = cursorp;

	if( data->markmethod == MARKMETHOD_CHAR )
	{
		ws = data->rmarkstart;
		we = cursorp;
	}
	else if( data->markmethod == MARKMETHOD_ALL )
	{
		ws = data->buffer;
		we = strchr( data->buffer, 0 );
	}
	else
	{
		char *min = STRMIN( data->rmarkstart, data->rmarkend );
		char *max = STRMAX( data->rmarkstart, data->rmarkend );
		if( data->markmethod == MARKMETHOD_WORD )
		{
			ws = getwordstart( data, min );
			we = getwordend( data, max );
		}
		else if( data->markmethod == MARKMETHOD_LINE )
		{
			ws = getlinestart( data, min );
			we = getlineend( data, max );
		}
		else
		{
			resetmarkmode( data );
			return( TRUE );
		}
//		findcursorxy( data, we );
	}

	if( data->noinput && !*we )
	{
		if( we <= data->buffer )
			return( FALSE );
		if( --we < ws )
			return( FALSE );
	}

	data->oldmarkstart = data->markstart;
	data->oldmarkend = data->markend;
	data->markstart = STRMIN( ws, we );
	data->markend = STRMAX( ws, we );
	return( data->markstart != data->oldmarkstart || data->markend != data->oldmarkend );
}

void updatecursor( struct Data *data, Object *obj, int newcursorx, int newcursory )
{
	int redraw = FALSE, killoldmark = FALSE;
	data->redrawmode = RDO_MOVECURSOR;
	data->oldcursorx = data->cursorx;
	data->oldcursory = data->cursory;
	data->oldcursorp = data->cursorp;
	data->oldpxo = data->pxo;
	data->oldcellsize = data->cellsize;
	data->cursorx = newcursorx;
	data->cursory = newcursory;
	findcursorp( data );

	if( data->resetmarkoncursor && data->markmode )
	{
		DoMethod( _app( obj ), MUIM_Application_PushMethod, obj, 3,
			MUIM_Set, MUIA_Textinput_MarkEnd, -1
		);
		DoMethod( _app( obj ), MUIM_Application_PushMethod, obj, 3,
			MUIM_Set, MUIA_Textinput_MarkStart, -1
		);
	}


	if( data->markmode == 1 && data->oldmarkstart != NOMARK && data->oldmarkend != NOMARK )
	{
		data->redrawmode = RDO_MOVEMARK;
		redraw = TRUE;
		killoldmark = TRUE;
	}
	else if( data->markmode == 2 || data->markmode == 3 )
	{
		if( extendmark( data ) )
		{
			data->redrawmode = RDO_MOVEMARK;
			redraw = TRUE;
		}
	}
	if( data->oldcursorx != data->cursorx || data->oldcursory != data->cursory )
	{
		redraw = TRUE;
		checkcursorvisible( data, obj );
	}
	if( redraw )
	{
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	if( killoldmark )
		data->oldmarkstart = data->oldmarkend = NOMARK;
	docheckinform( data, obj );
}

static int processkey( APTR cl, APTR obj, struct Data *data, struct IntuiMessage *im )
{
	char buffer[ 32 ];
#ifndef MBX
	struct InputEvent ie;
#endif
	int rc = 0;
	int c, rv = 0;
#ifdef USE_SETTASKPRI
	int oldpri;
#endif

#ifndef MBX // !!!

	memset( &ie, 0, sizeof( ie ) );
	ie.ie_Class = IECLASS_RAWKEY;
	ie.ie_Code = im->Code;
	ie.ie_Qualifier = im->Qualifier;
	ie.ie_EventAddress = (APTR *) *((ULONG *)im->IAddress);

	if( ie.ie_Qualifier & IEQUALIFIER_CONTROL )
		rc = codetostyle( data, ie.ie_Code, buffer );

	if( !rc )
		rc = MapRawKey( &ie, buffer, sizeof( buffer ), 0 );

	if( DoMethod( obj, MUIM_Textinput_TranslateEvent, &ie, buffer, &rc ) )
		return( FALSE );

	if( rc < 1 )
		return( FALSE ); // Still eat the event (key up, most likely)

	if( data->noedit )
	{
		DisplayBeep( _screen( obj ) );
		return( TRUE );
	}

	#ifdef USE_SETTASKPRI
	oldpri = SetTaskPri( FindTask( NULL ), 19 );
	#endif

#else
	strcpy(buffer,im->im_UTF8);
	rc = strlen(buffer);
#endif

	for( c = 0; c < rc; c += CHARLEN(buffer[c]) )
	{
		int ch = buffer[ c ];
		//if( handlechar( cl, obj, data, ch == 10 ? 13 : ch, FALSE ) )
#ifdef MBX
		char *cr = "\x0D";
		if( DoMethod( obj, MUIM_Textinput_HandleChar, ch == 10 ? cr : buffer + c, FALSE ) )
#else
		if( DoMethod( obj, MUIM_Textinput_HandleChar, ch == 10 ? 13 : ch, FALSE ) )
#endif
			rv = TRUE;
	}

#ifdef MBX
	if (!rv)
	{
		char *cr = "\x0D";
		if (im->im_Code==IECODE_ENTER)
			if( DoMethod( obj, MUIM_Textinput_HandleChar, cr, FALSE ) )
				rv = TRUE;
	}
#endif

	if( rv )
		MUI_Redraw( obj, MADF_DRAWUPDATE );

	docheckinform( data, obj );

#ifdef USE_SETTASKPRI
	SetTaskPri( FindTask( NULL ), oldpri );
#endif

	return( TRUE );
}

#ifdef MBX
static int DoubleClick( ULONG lastsecs, ULONG lastmics, ULONG thissecs, ULONG thismics )
{
	lastmics /= 1000;
	thismics /= 1000;

	lastsecs = ( lastsecs % 1000 ) * 1000 + lastmics;
	thissecs = ( thissecs % 1000 ) * 1000 + thismics;

	// Double Click time fixed at 1/2 second

	if( thissecs - 5000 < lastsecs )
		return( TRUE );
	else
		return( FALSE );
}
#endif

DECMMETHOD( HandleEvent )
{
	GETDATA;
	int intuiticks = FALSE;

	if( _isdisabled( obj ) || !data->isshown )
		return( 0 );

	#ifdef MBX
	switch( msg->muikey )
	{
		case MUIKEY_UP     : return(0);
		case MUIKEY_DOWN   : return(0);
		case MUIKEY_PRESS  : return(0);
		case MUIKEY_RELEASE: return(0);
	}
	if (msg->imsg)
	{
		switch (msg->imsg->im_Code)
		{
			case IECODE_MENU: return(0);
			case IECODE_EXIT: return(0);
			case IECODE_PADLEFT: msg->imsg->im_Code=IECODE_CURSORLEFT; break;
			case IECODE_PADRIGHT: msg->imsg->im_Code=IECODE_CURSORRIGHT; break;
		}
	}
	#endif

	if( data->attachedlist )
	{
		switch( msg->muikey )
		{
			case MUIKEY_PRESS:
			case MUIKEY_RELEASE:
			case MUIKEY_UP:
			case MUIKEY_DOWN:
			case MUIKEY_PAGEUP:
			case MUIKEY_PAGEDOWN:
			case MUIKEY_TOP:
			case MUIKEY_BOTTOM:
				DoMethod( data->attachedlist, (((struct Library*)MUIMasterBase)->lib_Version >= 20 ) ? MUIM_HandleEvent : MUIM_HandleInput, NULL, msg->muikey );
				break;
		}
	}

	if( msg->muikey == MUIKEY_HELP )
		return( 0 );

	if( msg->imsg )
	{
		int mx = msg->imsg->im_MouseX, my = msg->imsg->im_MouseY;

		switch( msg->imsg->im_Class )
		{
			case IDCMP_INACTIVEWINDOW:
				if( data->markmode && data->noinput )
				{
					DoMethod( _win( obj ), MUIM_Window_RemEventHandler, &data->ehn );
					data->ehn.ehn_Events &= ~( IDCMP_MOUSEMOVE | IDCMP_INTUITICKS );
					DoMethod( _win( obj ), MUIM_Window_AddEventHandler, &data->ehn );
					set( obj, MUIA_ContextMenu, data->popupmenu );
					resetmarkmode( data );
				}
				urldeselect( data );
#if 0
				if( data->isactive )
				{
					stopblink( obj, data );
					data->redrawmode = RDO_SCROLL;
					MUI_Redraw( obj, MADF_DRAWUPDATE );
					SetTaskPri( FindTask( 0 ), data->oldpri );
				}
				break;
#endif

			case IDCMP_ACTIVEWINDOW:
				if( data->isactive )
				{
					data->redrawmode = RDO_SCROLL;
					MUI_Redraw( obj, MADF_DRAWUPDATE );
					startblink( obj, data );
				}
				break;

			case IDCMP_MOUSEBUTTONS:
				if( msg->imsg->im_Code == SELECTDOWN )
				{
					if( !data->noinput )
					{
						if( !data->isactive )
						{
							if( isinobject( obj, mx, my, FALSE ) )
							{
								set( _win( obj ), MUIA_Window_ActiveObject, obj );
							}
						}
						else
						{
							if( !isinobject( obj, mx, my, FALSE ) && !data->remainactive )
							{
								set( _win( obj ), MUIA_Window_ActiveObject, NULL );
							}
						}
					}
					if( isinobject( obj, mx, my, TRUE ) )
					{
						int nx, ny, outside;
						if( getxycharpos( _rp( obj ), obj, data, mx, my, &ny, &nx, PARSEURLS ? &outside : NULL ) )
						{
							char *p = getcursorp( data, nx, ny );
							int restart = ( msg->imsg->im_Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT ) );
							if( data->issecret || data->nocopy )
							{
								updatecursor( data, obj, nx, ny );
								return( MUI_EventHandlerRC_Eat );
							}
							if( restart )
							{
								if( data->markmode != 3 )
									data->rmarkend = data->rmarkstart = data->cursorp;
							}
#ifndef MBX
							if( DoubleClick( data->lastsecs, data->lastmics, msg->imsg->Seconds, msg->imsg->Micros ) && max( data->downx, mx ) - min( data->downx, mx ) < 3 && max( data->downy, my ) - min( data->downy, my ) < 3 )
#else
							if( DoubleClick( data->lastsecs, data->lastmics, msg->imsg->im_TimeStamp.tv_Micros / 1000000, msg->imsg->im_TimeStamp.tv_Micros % 1000000 ) && max( data->downx, mx ) - min( data->downx, mx ) < 3 && max( data->downy, my ) - min( data->downy, my ) < 3 )
#endif
							{
								if( !restart )
									data->rmarkend = data->rmarkstart = p;
								data->markmode = 2;
								DoMethod( obj, MUIM_Textinput_DoubleClick, mx, my, data->dblclickcount++ );
							}
							else
							{
								data->dblclickcount = 0;
								data->downx = mx;
								data->downy = my;
								if( restart )
								{
									data->markmode = 2;
									if( abs( p - data->rmarkstart ) < abs( p - data->rmarkend ) )
										data->rmarkstart = data->rmarkend;
									updatemark( data, data->rmarkstart, p );
									data->redrawmode = RDO_MOVEMARK;
									MUI_Redraw( obj, MADF_DRAWUPDATE );
								}
								else
								{
									data->markmethod = 0;
									if( PARSEURLS && !outside && ( data->redrawurl = urlselect( data, nx, ny ) ) )
									{
										data->redrawmode = RDO_URL;
										MUI_Redraw( obj, MADF_DRAWUPDATE );
									}
									data->oldmarkstart = data->markstart;
									data->oldmarkend = data->markend;
									data->markmode = 1;
									data->rmarkstart = data->rmarkend = NOMARK;
									data->markstart = data->markend = NOMARK;
								}
							}
							DoMethod( _win( obj ), MUIM_Window_RemEventHandler, &data->ehn );
							data->ehn.ehn_Events |= IDCMP_MOUSEMOVE | IDCMP_INTUITICKS;
							DoMethod( _win( obj ), MUIM_Window_AddEventHandler, &data->ehn );
							set( obj, MUIA_ContextMenu, NULL );
							updatecursor( data, obj, nx, ny );
#ifndef MBX
							data->lastsecs = msg->imsg->Seconds;
							data->lastmics = msg->imsg->Micros;
#else
							data->lastsecs = msg->imsg->im_TimeStamp.tv_Micros / 1000000;
							data->lastmics = msg->imsg->im_TimeStamp.tv_Micros % 1000000;
#endif
						}
						return( MUI_EventHandlerRC_Eat );
					}
				}
				else if( msg->imsg->im_Code == SELECTUP || msg->imsg->im_Code == MENUDOWN )
				{
					int abort = msg->imsg->im_Code == MENUDOWN;
					struct viewurl *vu;
					if( data->markmode )
					{
						DoMethod( _win( obj ), MUIM_Window_RemEventHandler, &data->ehn );
						data->ehn.ehn_Events &= ~( IDCMP_MOUSEMOVE | IDCMP_INTUITICKS );
						DoMethod( _win( obj ), MUIM_Window_AddEventHandler, &data->ehn );
						set( obj, MUIA_ContextMenu, data->popupmenu );
						if( data->markmode == 1 || ( data->markmode == 2 && abort ) )
						{
							int aborted = data->markmode == 2;
							resetmarkmode( data );
							if( aborted )
							{
								data->redrawmode = RDO_MOVEMARK;
								MUI_Redraw( obj, MADF_DRAWUPDATE );
							}
						}
						else
						{
							if( data->markstart != NOMARK && data->markend != NOMARK )
							{
								data->markmode = 3;
								if( data->noinput )
									DoMethod( obj, MUIM_Textinput_DoCopy );
							}
						}
					}
					if((vu = urldeselect( data )))
					{
						data->redrawmode = RDO_URL;
						data->redrawurl = vu;
						MUI_Redraw( obj, MADF_DRAWUPDATE );
						if( !abort )
							DoMethod( obj, MUIM_Textinput_HandleURL, vu->url );
					}
				}
				break;

			case IDCMP_INTUITICKS:
				intuiticks = TRUE;
/*
				if( data->markmode && data->markmode < 3 )
				{
					//Printf( "t %ld v %ld l %ld\n", data->topline, data->visiblelines, data->linenum );
					if( mx < _mleft( obj ) && data->xoffset )
					{
						data->xoffset--;
					}
					else if( mx > _mright( obj ) )
					{
						if( data->xoffset < ( data->maxxsize - ( _mwidth( obj ) - XSPACING * 2 ) ) )
							data->xoffset++;
					}

					if( my < _mtop( obj ) && data->topline )
					{
						data->topline--;
					}
					else if( my > _mbottom( obj ) && ( data->topline + data->visiblelines < ( data->linenum - 1 ) ) )
					{
						//PutStr( "+++\n" );
						data->topline++;
					}
				}
*/
				// fallthrough
			case IDCMP_MOUSEMOVE:
				if( data->markmode )
				{
					int nx, ny;
					_window( obj )->ww_Flags |= WFLG_RMBTRAP;

					if( data->markmode == 1 )
					{
						struct viewurl *vu;
						if( max( data->downx, mx ) - min( data->downx, mx ) < 3 &&
							max( data->downy, my ) - min( data->downy, my ) < 3 )
						break;

						data->rmarkend = data->rmarkstart = data->cursorp;
						data->markmode = 2;
						if((vu = urldeselect( data )))
						{
							data->redrawmode = RDO_URL;
							data->redrawurl = vu;
							MUI_Redraw( obj, MADF_DRAWUPDATE );
						}
					}
					if( getxycharpos( _rp( obj ), obj, data, mx, my, &ny, &nx, NULL ) || intuiticks )
					{
						if( data->noinput )
						{
							char *p = getcursorp( data, nx, ny );
							if( !*p )
							{
								if( data->buffer < p )
									findxyp( data, p - 1, &nx, &ny, NULL );
								else
									break;
							}
						}
						updatecursor( data, obj, nx, ny );
					}
				}
				break;

			case IDCMP_RAWKEY:
				if( data->isactive && !data->noinput )
				{
#ifndef MBX
					int c;
					struct InputEvent ie;
					extern int __asm myMatchIX( register __a0 struct InputEvent *ie, register __a1 IX *ix );

					if( msg->imsg->im_Code & IECODE_UP_PREFIX )
					{
						if( data->goinactive )
						{
							set( _win( obj ), MUIA_Window_ActiveObject, data->advanceoncr ? MUIV_Window_ActiveObject_Next : MUIV_Window_ActiveObject_None );
							data->goinactive = FALSE;
							return( MUI_EventHandlerRC_Eat );
						}
					}


					if( msg->imsg->im_Code == NM_WHEEL_UP )
					{
						if( data->topline )
						{
							set( obj, MUIA_Textinput_TopLine, data->topline - 1 );
							doinform( data, obj );
						}
						return( MUI_EventHandlerRC_Eat );
					}
					else if( msg->imsg->im_Code == NM_WHEEL_DOWN )
					{
						if( data->topline + data->visiblelines < ( data->linenum - 1 ) )
						{
							set( obj, MUIA_Textinput_TopLine, data->topline + 1 );
							doinform( data, obj );
						}
						return( MUI_EventHandlerRC_Eat );
					}
					else if( msg->imsg->im_Code == NM_WHEEL_LEFT )
					{
						set( obj, MUIA_Textinput_LeftOffset, max( 0, data->xoffset - 20 ) );
						doinform( data, obj );
						return( MUI_EventHandlerRC_Eat );
					}
					else if( msg->imsg->im_Code == NM_WHEEL_RIGHT )
					{
						if( ( data->xoffset + 20 ) < ( data->maxxsize - ( _mwidth( obj ) - XSPACING * 2 ) ) )
							set( obj, MUIA_Textinput_LeftOffset, data->xoffset + 20 );
						doinform( data, obj );
						return( MUI_EventHandlerRC_Eat );
					}
#endif

					#ifdef MBX

					{
						struct GuessMe
						{
							ULONG quali;
							ULONG code;
							ULONG method;
							int multiline_only;
						};
						static struct GuessMe guessme[] =
						{
							{ 0,IECODE_CURSORLEFT ,MUIM_Textinput_DoLeft, 0  },
							{ 0,IECODE_CURSORRIGHT,MUIM_Textinput_DoRight, 0 },
							{ 0,IECODE_CURSORUP   ,MUIM_Textinput_DoUp, 1    },
							{ 0,IECODE_CURSORDOWN ,MUIM_Textinput_DoDown, 1  },
							{ 0,IECODE_PAGEUP     ,MUIM_Textinput_DoPageUp, 1 },
							{ 0,IECODE_PAGEDOWN   ,MUIM_Textinput_DoPageDown, 1 },
							{ 0,IECODE_DELETE     ,MUIM_Textinput_DoDel, 0 },
							{ 0,IECODE_BACKSPACE  ,MUIM_Textinput_DoBS, 0 },
							{ 0,IECODE_TAB        ,MUIM_Textinput_DoTab, 0 },
							{ 0,0,0 }
						};
						struct GuessMe *gm;
						for (gm=guessme;gm->method;gm++)
						{
							if (msg->imsg->im_Code==gm->code)
							{
								if( gm->multiline_only && !data->multiline )
									return( 0 ); // Let other handlers take up this event
								DoMethod(obj,gm->method);
#ifdef MBX
							  	if( AudioBase )
							  	 	SystemBeep( SYS_CLICK );
#endif
								return(MUI_EventHandlerRC_Eat);
							}
						}
					}

					#else // !MBX

					//memset( &ie, 0, sizeof( ie ) );
					ie.ie_Class = IECLASS_RAWKEY;
					ie.ie_Code = msg->imsg->im_Code;
					#define QUALS (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT|IEQUALIFIER_CONTROL|IEQUALIFIER_LALT|IEQUALIFIER_RALT|IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|IEQUALIFIER_NUMERICPAD|IEQUALIFIER_REPEAT|IEQUALIFIER_INTERRUPT|IEQUALIFIER_MULTIBROADCAST)
					ie.ie_Qualifier = msg->imsg->im_Qualifier & QUALS;
					for( c = 0; c < data->numkeys; c++ )
					{
						if( myMatchIX( &ie, data->keyspecs[ c ].ix ) )
						{
							static ULONG keymethods[] = {
								MUIM_Textinput_DoRevert,
								MUIM_Textinput_DoDelLine,
								MUIM_Textinput_DoMarkStart,
								MUIM_Textinput_DoMarkAll,
								MUIM_Textinput_DoCut,
								MUIM_Textinput_DoCopy,
								MUIM_Textinput_DoPaste,
								MUIM_Textinput_ExternalEdit,
								MUIM_Textinput_DoToggleWordwrap,
								MUIM_Textinput_DoLeft,
								MUIM_Textinput_DoRight,
								MUIM_Textinput_DoUp,
								MUIM_Textinput_DoDown,
								MUIM_Textinput_DoPageUp,
								MUIM_Textinput_DoPageDown,
								MUIM_Textinput_DoTop,
								MUIM_Textinput_DoBottom,
								MUIM_Textinput_DoLineStart,
								MUIM_Textinput_DoLineEnd,
								MUIM_Textinput_DoPrevWord,
								MUIM_Textinput_DoNextWord,
								MUIM_Textinput_DoPopup,
								MUIM_Textinput_DoDel,
								MUIM_Textinput_DoDelEOL,
								MUIM_Textinput_DoBS,
								MUIM_Textinput_DoBSSOL,
								0, 0, // tab/tab
								MUIM_Textinput_DoDelWord,
								MUIM_Textinput_DoBSWord,
								MUIM_Textinput_DoInsertFile,
								MUIM_Textinput_DoToggleCase,
								MUIM_Textinput_DoToggleCaseEOW,
								MUIM_Textinput_DoIncrementDec,
								MUIM_Textinput_DoDecrementDec,
								MUIM_Textinput_DoUndo,
								MUIM_Textinput_DoRedo,
								MUIM_Textinput_DoTab,
								MUIM_Textinput_DoNextGadget,
								MUIM_Textinput_DoSetBookmark1,
								MUIM_Textinput_DoSetBookmark2,
								MUIM_Textinput_DoSetBookmark3,
								MUIM_Textinput_DoGotoBookmark1,
								MUIM_Textinput_DoGotoBookmark2,
								MUIM_Textinput_DoGotoBookmark3,
								MUIM_Textinput_DoCutLine,
								MUIM_Textinput_DoCopyCut
							};
							if( keymethods[ data->keyspecs[ c ].action ] )
								DoMethod( obj, keymethods[ data->keyspecs[ c ].action ] );
							return( MUI_EventHandlerRC_Eat );
						}
					}
					#endif // MBX

#ifndef MBX
					if( ( msg->imsg->im_Qualifier & IEQUALIFIER_RCOMMAND ) && data->oldmenustrip )
					{
						struct MenuItem *mit;
						char key;

						// check for a menu item...

						if( MapRawKey( &ie, &key, 1, 0 ) == 1 )
						{
							UWORD menunum;

							key = tolower( key );

							mit = FindCommandKeyInMenu( data->oldmenustrip, key, &menunum );
							if( mit )
							{
								APTR muiobj = GTMENUITEM_USERDATA( mit );

								if( mit->Flags & MENUTOGGLE )
									mit->Flags ^= CHECKED;
								else if( mit->Flags & CHECKIT )
									mit->Flags |= CHECKED;

								if( !_window( obj )->MenuStrip )
								{
									ResetMenuStrip( _window( obj ), data->oldmenustrip );
								}
								data->oldmenustrip = NULL;

								//menuproc( _window( obj ), mit, menunum );
								if( muiobj )
								{
									DoMethod( _app( obj ), MUIM_Application_ReturnID, muiUserData( muiobj ) );

									DoMethod( _app( obj ), MUIM_Application_PushMethod,
										_app( obj ), 3, MUIM_Set, MUIA_Application_MenuAction, muiUserData(muiobj )
									);
									DoMethod( _app( obj ), MUIM_Application_PushMethod,
										_win( obj ), 3, MUIM_Set, MUIA_Window_MenuAction, muiUserData(muiobj )
									);
									set( muiobj, MUIA_Menuitem_Trigger, mit );

									//set( _app(obj), MUIA_Application_MenuAction, muiUserData(muiobj ) );
									//set( _win( obj ), MUIA_Window_MenuAction, muiUserData( muiobj ) );
								}

								return( MUI_EventHandlerRC_Eat );
							}
						}
					}
#endif

					switch( msg->imsg->im_Code )
					{
#ifndef MBX
						case 0x66:
						case 0x67:
							if( !data->oldmenustrip )
							{
								data->oldmenustrip = _window( obj )->MenuStrip;
								ClearMenuStrip( _window( obj ) );
							}
							break;

						case 0x66 | 128:
						case 0x67 | 128:
							if( data->oldmenustrip )
							{
								if( !_window( obj )->MenuStrip )
								{
									ResetMenuStrip( _window( obj ), data->oldmenustrip );
								}
								data->oldmenustrip = NULL;
							}
							break;

						case 0x4c:
						case 0x4d:
						case 0x4e:
						case 0x4f:
						case 0x46:
						case 0x5f:
						case 0x45:
						case 0x41:
						case 0x42:
							break;
#endif

						default:
							if( !( msg->imsg->im_Qualifier & ( IEQUALIFIER_RCOMMAND | IEQUALIFIER_LCOMMAND ) ) )
								if( processkey( cl, obj, data, msg->imsg ) )
									return( MUI_EventHandlerRC_Eat );
							break;
					}
					return( 0 );
				}
#ifndef MBX
				else if( data->controlchar )
				{
					char buffer[ 16 ];
					struct InputEvent ie;
					int rc;

					memset( &ie, 0, sizeof( ie ) );
					ie.ie_Class = IECLASS_RAWKEY;
					ie.ie_Code = msg->imsg->im_Code;
					ie.ie_Qualifier = msg->imsg->im_Qualifier;
					ie.ie_EventAddress = (APTR *) *((ULONG *)msg->imsg->IAddress);

					rc = MapRawKey( &ie, buffer, sizeof( buffer ), 0 );

					if( rc == 1 && buffer[ 0 ] == data->controlchar )
					{
						set( _win( obj ), MUIA_Window_ActiveObject, obj );
					}
				}
#endif

				break;
		}
	}

	return( 0 );
}

static void procset( APTR obj, APTR cl, struct Data *data, struct TagItem *tags )
{
	struct TagItem *tag;
	int needredraw = FALSE;
	int checkvals = FALSE;

	while((tag = NextTagItem( &tags )))
	{
		switch( tag->ti_Tag )
		{
			case MUIA_Textinput_NoCopy:
				data->nocopy = tag->ti_Data;
				break;

			case MUIA_Textinput_SuggestParse:
				data->suggestparse = tag->ti_Data;
				break;

			case MUIA_Textinput_ProhibitParse:
				data->prohibitparse = tag->ti_Data;
				break;

			case MUIA_Textinput_Font:
				data->fonttype = tag->ti_Data;
				break;

			case MUIA_Textinput_Bookmark1:
				data->bookmarks[ 0 ] = tag->ti_Data;
				break;

			case MUIA_Textinput_Bookmark2:
				data->bookmarks[ 1 ] = tag->ti_Data;
				break;

			case MUIA_Textinput_Bookmark3:
				data->bookmarks[ 2 ] = tag->ti_Data;
				break;

			case MUIA_Textinput_HandleURLHook:
				data->handleurlhook = (struct Hook *)tag->ti_Data;
				break;

			case MUIA_Textinput_PreParse:
			case MUIA_Text_PreParse:
				if( tag->ti_Data )
				{
					char *preparse = (char *)tag->ti_Data;
					int len = strlen( preparse );
					if( len > data->preparsemaxlen )
					{
						if( data->preparse )
							FreeVecPooled( data->pool, data->preparse );
						if((data->preparse = AllocVecPooled( data->pool, len + 33 )))
							data->preparsemaxlen = len + 32;
					}
					if( data->preparse )
						strcpy( data->preparse, preparse );
				}
				else if( data->preparse )
				{
					*data->preparse = 0;
				}
				break;

			case MUIA_Textinput_SetMin:
			case MUIA_Text_SetMin:
				data->setmin = BSETTAG( tag );
				break;

			case MUIA_Textinput_SetMax:
			case MUIA_Text_SetMax:
				data->setmax = BSETTAG( tag );
				break;

			case MUIA_Textinput_SetVMin:
				data->setvmin = BSETTAG( tag );
				break;

			case MUIA_Textinput_SetVMax:
			case MUIA_Text_SetVMax:
				data->setvmax = BSETTAG( tag );
				break;

			case MUIA_Textinput_Styles:
				data->styles = tag->ti_Data;
				break;

			case MUIA_Textinput_Contents:
			case MUIA_String_Contents:
			case MUIA_Text_Contents:
				{
					char *s = (char *)tag->ti_Data;
					struct actionnode *n;
					int len;
					int stripnl = FALSE;
					if( !s )
						s = "";
					else if( !data->multiline && data->noinput )
					{
						char *nl;
						if((nl = strchr( s, '\n' )))
						{
							if( nl [ 1 ] )
								data->multiline = TRUE;
							else
								stripnl = TRUE;
						}
					}
					len = strlen( s );

					/* !!!!!!!! Not yet undoable, so purge the undo/redo lists for now !!!!!!!!!! */

					while((n = REMHEAD( &data->undolist )))
						freeaction( data, n );
					while((n = REMHEAD( &data->redolist )))
						freeaction( data, n );

//					doaction( data, ACTION_REPLACE | ACTIONF_UNCONDITIONAL | ACTIONF_STRIPCR, data->buffer, -1, s, -1 );

					dobuffersize( data, len );
					stccpy( data->buffer, s, data->maxlen + 1 );
					if( stripnl && len <= data->maxlen + 1 )
						data->buffer[ len - 1 ] = 0;
					for( s = data->buffer; *s; )
					{
						if( *s == '\r' )
							strcpy( s, s + 1 );
						else
							s++;
					}
					//dowordwrap( data, data->buffer );
					findcursorxy( data, data->isactive && !data->noinput ? (char *)strchr( data->buffer, 0 ) : data->buffer );
					resetmarkmode( data );
					data->xoffset = 0;
					checkvals = TRUE;
					data->redrawmode = RDO_SCROLL;
					needredraw = MADF_DRAWUPDATE;
					data->changed = TRUE;
					findurls( data );
					tnotify( obj,
						( tag->ti_Tag == MUIA_String_Contents ) ? MUIA_Textinput_Contents : MUIA_String_Contents, data->buffer,
						( tag->ti_Tag == MUIA_Text_Contents ) ? MUIA_Textinput_Contents : MUIA_Text_Contents, data->buffer,
						MUIA_Textinput_Changed, TRUE,
						TAG_DONE
					);
				}
				break;

			case MUIA_Textinput_Integer:
			case MUIA_String_Integer:
			case MUIA_Numeric_Value:
				{
					char buffer[ 32 ];
					sprintf( buffer, "%ld", tag->ti_Data );
					doaction( data, ACTION_REPLACE | ACTIONF_UNCONDITIONAL, data->buffer, -1, buffer, -1 );
					//dowordwrap( data, data->buffer );
					findcursorxy( data, data->isactive && !data->noinput ? (char *)strchr( data->buffer, NULL ) : data->buffer );
					resetmarkmode( data );
					data->xoffset = 0;
					checkvals = TRUE;
					data->redrawmode = RDO_SCROLL;
					needredraw = MADF_DRAWUPDATE;
					endchange( obj, data, 0 );
				}
				break;

			case MUIA_Textinput_LeftOffset:
				data->xoffset = tag->ti_Data;
				data->redrawmode = RDO_SCROLL;
				needredraw = MADF_DRAWUPDATE;
				break;

			case MUIA_Textinput_TopOffset:
				{
					int off = tag->ti_Data + tag->ti_Data % ( data->smooth ? min( data->smooth, data->lineheight ) : data->lineheight );
					data->yoffset = off;
					data->topline = off / data->lineheight;
					data->redrawmode = RDO_SCROLL;
					needredraw = MADF_DRAWUPDATE;
//					tnotify( obj, MUIA_Textinput_TopLine, data->topline, TAG_DONE );
				}
				break;

			case MUIA_Textinput_TopLine:
				data->topline = tag->ti_Data;
				data->yoffset = tag->ti_Data * data->lineheight;
				data->redrawmode = RDO_SCROLL;
				needredraw = MADF_DRAWUPDATE;
//				tnotify( obj, MUIA_Textinput_TopOffset, data->yoffset, TAG_DONE );
				break;

			case MUIA_Textinput_TSCO:
				data->scrollobj = (APTR)tag->ti_Data;
				break;

			case MUIA_Textinput_AutoExpand:
				data->autoexpand = BSETTAG( tag );
				break;

			case MUIA_Textinput_AdvanceOnCR:
			case MUIA_String_AdvanceOnCR:
				data->advanceoncr = BSETTAG( tag );
				break;

			case MUIA_Textinput_Blinkrate:
				data->blinkrate = tag->ti_Data;
				data->redrawmode = RDO_SCROLL;
				needredraw = MADF_DRAWUPDATE;
				break;

			case MUIA_Textinput_WordWrap:
				data->wordwrap = tag->ti_Data;
				data->dowordwrap = data->wordwrap ? TRUE : FALSE;
				break;

			case MUIA_Textinput_Cursorstyle:
				data->cursorstyle = tag->ti_Data;
				data->redrawmode = RDO_SCROLL;
				needredraw = MADF_DRAWUPDATE;
				break;

			case MUIA_Textinput_CursorSize:
				data->cursorsize = tag->ti_Data;
				data->redrawmode = RDO_SCROLL;
				needredraw = MADF_DRAWUPDATE;
				break;

			case MUIA_Textinput_TmpExtension:
				{
					STRPTR s = (STRPTR)tag->ti_Data;
					while( *s == '.' )
						s++;
					stccpy( data->tmpextension, s, sizeof( data->tmpextension ) );
				}
				break;

			case MUIA_Textinput_Quiet:
				data->quiet = BSETTAG( tag );
				if( !data->quiet )
				{
					docheckinform( data, obj );
					data->redrawmode = RDO_SCROLL;
					needredraw = MADF_DRAWUPDATE;
				}
				break;

			case MUIA_Textinput_AttachedList:
			case MUIA_String_AttachedList:
				data->attachedlist = (APTR)tag->ti_Data;
				break;

			case MUIA_String_Popup:
				data->attachedpop = (APTR)tag->ti_Data;
				break;

			case MUIA_Textinput_RemainActive:
				data->remainactive = BSETTAG( tag );
				break;

			case MUIA_Textinput_IsNumeric:
				data->isnumeric = BSETTAG( tag );
				break;

			case MUIA_Textinput_MinVal:
			case MUIA_Numeric_Min:
				data->minnumval = tag->ti_Data;
				break;

			case MUIA_Textinput_MaxVal:
			case MUIA_Numeric_Max:
				data->maxnumval = tag->ti_Data;
				break;

			case MUIA_Textinput_AcceptChars:
			case MUIA_String_Accept:
				data->acceptchars = (char *)tag->ti_Data;
				break;

			case MUIA_Textinput_RejectChars:
			case MUIA_String_Reject:
				data->rejectchars = (char *)tag->ti_Data;
				break;

			case MUIA_Textinput_Secret:
			case MUIA_String_Secret:
				data->issecret = BSETTAG( tag );
				break;

			case MUIA_Textinput_Editable:
				data->noedit = !tag->ti_Data;
				break;

			case MUIA_Textinput_CursorPos:
			case MUIA_String_BufferPos:
				data->cursorp = data->buffer + min( tag->ti_Data, strlen( data->buffer ) );
				findcursorxy( data, data->cursorp );
				checkvals = TRUE;
				break;

			case MUIA_Textinput_MarkStart:
				if( (int)tag->ti_Data < 0 )
				{
					resetmarkmode( data );
				}
				else
				{
					char *rmarkstart = data->buffer + min( tag->ti_Data, strlen( data->buffer ) );
					updatemark( data, rmarkstart, data->rmarkend == NOMARK ? rmarkstart : data->rmarkend );
					data->markmode = 3;
				}
				if( !( needredraw && data->redrawmode == RDO_SCROLL ) )
				{
					data->redrawmode = RDO_MOVEMARK;
					needredraw = MADF_DRAWUPDATE;
				}
				break;

			case MUIA_Textinput_MarkEnd:
				if( (int)tag->ti_Data < 0 )
				{
					resetmarkmode( data );
				}
				else
				{
					char *rmarkend = data->buffer + min( tag->ti_Data, strlen( data->buffer ) );
					updatemark( data, data->rmarkstart == NOMARK ? rmarkend : data->rmarkstart, rmarkend );
					data->markmode = 3;
					checkvals = TRUE;
				}
				if( !( needredraw && data->redrawmode == RDO_SCROLL ) )
				{
					data->redrawmode = RDO_MOVEMARK;
					needredraw = MADF_DRAWUPDATE;
				}
				break;

			case MUIA_ControlChar:
				data->controlchar = (UBYTE)tag->ti_Data;
				break;

			case MUIA_Textinput_ResetMarkOnCursor:
				data->resetmarkoncursor = tag->ti_Data ? TRUE : FALSE;
				break;

			case MUIA_Textinput_NoExtraSpacing:
				data->noExtraSpacing = tag->ti_Data ? TRUE : FALSE;
				break;				

			case MUIA_Textinput_MaxLen:
			case MUIA_String_MaxLen:
				data->maxlen = tag->ti_Data;
				break;

		}
	}

	if( checkvals )
	{
		if( muiRenderInfo( obj ) )
		{
			calcmaxvals( data, obj );
			if( checkcursorvisible( data, obj ) )
			{
				data->redrawmode = RDO_SCROLL;
				needredraw = MADF_DRAWUPDATE;
			}
			docheckinform( data, obj );
		}
	}
	if( needredraw )
		MUI_Redraw( obj, needredraw );
}

DECSET
{
	GETDATA;
	procset( obj, cl, data, msg->ops_AttrList );
	return( DOSUPER );
}

DECMMETHOD( AskMinMax )
{
	GETDATA;
	struct MUI_MinMax *mmi;

	DOSUPER;

	mmi = msg->MinMaxInfo;

	if( data->setvmin )
		mmi->MinHeight += data->linenum * data->font->tf_YSize + YSPACING * 2;
	else
		mmi->MinHeight += data->font->tf_YSize + YSPACING * 2;

	if( data->setmin )
		mmi->MinWidth += data->maxlinewidth + XSPACING * 2;
	else
	{
		if( data->noinput )
			mmi->MinWidth += XSPACING * 2;
		else
			mmi->MinWidth += data->font->tf_XSize * 4 + XSPACING * 2;
	}

	if( data->setmax )
		mmi->MaxWidth += data->maxlinewidth + XSPACING * 2;
	else
		mmi->MaxWidth += MUI_MAXMAX;

	if( data->setvmax )
		mmi->MaxHeight = mmi->MinHeight;
	else
		mmi->MaxHeight += MUI_MAXMAX;

	mmi->DefWidth = max( mmi->DefWidth, mmi->MinWidth );
	mmi->DefHeight = max( mmi->DefHeight, mmi->MinHeight );

	return( 0 );
}

DECMMETHOD( Show )
{
	GETDATA;
	int c;

	DOSUPER;

	if( data->multiline )
		data->visiblelines = ( _mheight( obj ) - ( YSPACING * 2 ) ) / data->lineheight;
	else
		data->visiblelines = 1;

	data->ycenter = ( _mheight( obj ) - ( YSPACING * 2 ) - ( data->visiblelines * data->lineheight ) ) / 2;

	calcmaxvals( data, obj );

	data->topline = max( 0, min( data->topline, data->linenum - data->visiblelines ) );
	data->yoffset = data->topline * data->lineheight;
	data->xoffset = max( 0, min( data->xoffset, data->maxxsize - ( _mwidth( obj ) - XSPACING * 2 ) ) );

	checkcursorvisible( data, obj );

	doinform( data, obj );

	data->isshown = TRUE;

	data->maxpen = 0;

	for( c = 0; c < 13; c++ )
	{
		struct MUI_PenSpec *ps;
		static char *defstr[ 13 ] = { "m5", "m2", "m5", "m2", "m2", "m5", "m7", "m6", "m2", "m5", "m5", "m6", "m5" };
		static ULONG defid[ 13 ] = { MUICFG_StringColor, MUICFG_StringBack, MUICFG_StringActiveColor, MUICFG_StringActiveBack, MUICFG_StringActiveBack, MUICFG_StringActiveColor, MUICFG_ActiveObjectPen, MUICFG_StringColor, MUICFG_StringActiveBack, MUICFG_StringActiveColor, MUICFG_StringActiveColor, MUICFG_StringColor, MUICFG_StringActiveColor };
		static ULONG defcfg[ 13 ] = { MUICFG_Textinput_Pens_Inactive_Foreground, MUICFG_Textinput_Pens_Inactive_Background, MUICFG_Textinput_Pens_Active_Foreground, MUICFG_Textinput_Pens_Active_Background, MUICFG_Textinput_Pens_Marked_Foreground, MUICFG_Textinput_Pens_Marked_Background, MUICFG_Textinput_Pens_Cursor_Foreground, MUICFG_Textinput_Pens_Cursor_Background, MUICFG_Textinput_Pens_Style_Foreground, MUICFG_Textinput_Pens_Style_Background, MUICFG_Textinput_Pens_URL_Underline, MUICFG_Textinput_Pens_URL_SelectedUnderline, MUICFG_Textinput_Pens_Misspell_Underline };
#ifndef MBX
		struct MUI_PenSpec pensp;
#endif
		ps = NULL;
		DoMethod( obj, MUIM_GetConfigItem, defcfg[ c ], &ps );
		if( !ps )
		{
			#ifndef MBX
			if( c == pen_f_cursor )
			{
				sprintf( pensp.buf, "p%ld", ~data->pens[ pen_f_active ] );
				ps = &pensp;
			}
			else if( c == pen_b_cursor )
			{
				sprintf( pensp.buf, "p%ld", ~data->pens[ pen_b_active ] );
				ps = &pensp;
			}
			else if( c == pen_f_style || c == pen_s_url )
			{
				sprintf( pensp.buf, "p%ld", data->pens[ pen_b_cursor ] );
				ps = &pensp;
			}
			else if( c == pen_b_style )
			{
				sprintf( pensp.buf, "p%ld", data->pens[ pen_f_cursor ] );
				ps = &pensp;
			}
			else if( c == pen_u_url || c == pen_u_misspell )
			{
				sprintf( pensp.buf, "p%ld", data->pens[ pen_f_active ] );
				ps = &pensp;
			}
			else
			#endif
			{
				DoMethod( obj, MUIM_GetConfigItem, defid[ c ], &ps );
				if( !ps )
					ps = (struct MUI_PenSpec *)defstr[ c ];
			}
		}

		//#ifdef MBX
		//data->pens[ c ] = MUI_ObtainPen( muiRenderInfo( obj ), (struct MUI_PenSpec *)defstr[c], 0 );
		//#else
		data->pens[ c ] = MUI_ObtainPen( muiRenderInfo( obj ), ps, 0 );
		//#endif
		data->maxpen |= MUIPEN( data->pens[ c ] );
	}

	data->pens[ pen_f_pattern ] = MUI_ObtainPen( muiRenderInfo( obj ), (struct MUI_PenSpec *)"m5", 0 );
	data->maxpen |= MUIPEN( data->pens[ 13 ] );
	for( c = 2; c <= 9; c++ )
		data->maxpen |= _dri( obj )->dri_Pens[ c ];

	if( DOCOLOURS )
	{
		for( c = 0; c < 16; c++ )
		{
			static ULONG colmap[3*16] = {
				224<<24, 224<<24, 224<<24,	// White
				  0    ,   0    ,   0    ,	// Black
				  0    ,   0<<24, 127<<24,	// Dark Blue
				  0    , 147<<24,   0    ,	// Dark Green
				255<<24,   0<<24,   0    ,	// Red
				127<<24,   0<<24,   0    ,	// Dark Red
				156<<24,   0<<24, 156<<24,	// Dark Purple
				252<<24, 127<<24,   0    ,	// Orange
				255<<24, 255<<24,   0    ,	// Yellow
				  0<<24, 252<<24,   0<<24,	// Green
				  0<<24, 147<<24, 147<<24,	// Cyan (Dark)
				  0<<24, 255<<24, 255<<24,	// Cyan
				  0    ,   0    , 252<<24,	// Blue
				255<<24,   0<<24, 255<<24,	// Purple
				127<<24, 127<<24, 127<<24,	// Gray
				210<<24, 210<<24, 210<<24	// Light Gray
			};

			struct MUI_PenSpec ps;
			sprintf( (APTR)&ps, "r%08lx,%08lx,%08lx", colmap[ c * 3 + 0 ], colmap[ c * 3 + 1 ], colmap[ c * 3 + 2 ] );
			data->colpens[ c ] = MUI_ObtainPen( muiRenderInfo( obj ), &ps, 0 );
			data->maxpen |= MUIPEN( data->colpens[ c ] );
		}
		data->do_colors = TRUE;
	}

	return( TRUE );
}

DECMMETHOD( Hide )
{
	GETDATA;
	int c;
	for( c = 0; c < 14; c++ )
	{
		MUI_ReleasePen( muiRenderInfo( obj ), data->pens[ c ] );
	}
	if( data->do_colors )
	{
		for( c = 0; c < 16; c++ )
			MUI_ReleasePen( muiRenderInfo( obj ), data->colpens[ c ] );
		data->do_colors = FALSE;
	}
	data->isshown = FALSE;
	return( DOSUPER );
}

#define NUMDEFKEYS 44
#ifndef MBX
static struct textinput_keydef defkeydefs[ NUMDEFKEYS + 1 ] = {
	{0, "ramiga q"},
	{1, "-repeat ramiga k"},
	{2, "ramiga b"},
	{3, "ramiga a"},
	{4, "ramiga x"},
	{5, "ramiga c"},
	{6, "-repeat ramiga v"},
	{7, "ramiga e"},
	{8, "ramiga w"},
	{9, "-repeat left"},
	{10, "-repeat right"},
	{11, "-repeat up"},
	{12, "-repeat down"},
	{13, "-repeat shift up"},
	{14, "-repeat shift down"},
	{15, "control up"},
	{16, "control down"},
	{17, "-repeat shift left"},
	{18, "-repeat shift right"},
	{19, "-repeat control left"},
	{20, "-repeat control right"},
	{21, "control p"},
	{22, "-repeat delete"},
	{23, "shift delete"},
	{24, "-repeat backspace"},
	{25, "shift backspace"},

	// 26 & 27 removed

	{28, "-repeat control delete"},
	{29, "-repeat control backspace"},

	{30, "ramiga shift o"},
	{31, "-repeat ramiga g"},
	{32, "-repeat ramiga shift g"},
	{33, "-repeat ramiga i"},
	{34, "-repeat ramiga d"},
	{35, "-repeat ramiga u"},
	{36, "-repeat ramiga shift u"},
	{37, "-repeat tab"},
	{38, "alt tab"},
	{39, "rshift ramiga 1"},
	{40, "rshift ramiga 2"},
	{41, "rshift ramiga 3"},
	{42, "ramiga 1"},
	{43, "ramiga 2"},
	{44, "ramiga 3"},
	{45, "-repeat shift ramiga k"},
	{-1, ""}
};
#endif

static ULONG __inline getci( Object *obj, ULONG ci, ULONG def )
{
	ULONG *vp = NULL;
	DoMethod( obj, MUIM_GetConfigItem, ci, &vp );
	if( vp )
		return( *vp );
	return( def );
}

DECMMETHOD( Setup )
{
	GETDATA;
	ULONG *vp;
	int dopopup = FALSE;
#ifndef MBX
	int c;

	if( !data->fontset )
	{
		char *s = 0;
		DoMethod( obj, MUIM_GetConfigItem, data->fonttype ? MUICFG_Textinput_FixedFont : MUICFG_Textinput_Font, &s );
		if( s && *s )
		{
			char fontspec[ 40 ], *p;
			struct TextAttr ta;
			stccpy( fontspec, s, 30 );
			p = strchr( fontspec, '/' );
			if( p )
			{
				*p++ = 0;
				ta.ta_YSize = myatoi( p );
				strcat( fontspec, ".font" );
			}
			else
				ta.ta_YSize = 8;

			ta.ta_Style = 0;
			ta.ta_Flags = 0;
			ta.ta_Name = fontspec;

			data->closefont = OpenDiskFont( &ta );

			if( data->closefont )
			{
				SetSuperAttrs( cl, obj, MUIA_Font, data->closefont, TAG_DONE );
			}
		}
		else
			SetSuperAttrs( cl, obj, MUIA_Font, MUIV_Font_Inherit );
	}
#endif

	if( !DOSUPER )
		return( 0 );

	data->font = _font( obj );
	#ifndef MBX
	data->farray = fc_find( data->font );
	#endif
	data->lineheight = data->font->tf_YSize;

	data->ehn.ehn_Object = obj;
	//data->ehn.ehn_Class = cl;
	data->ehn.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW;
#ifndef MUI_EHF_GUIMODE // TOFIX!!
#define MUI_EHF_GUIMODE (1<<1)
#endif
	data->ehn.ehn_Flags = MUI_EHF_GUIMODE;
	DoMethod( _win( obj ), MUIM_Window_AddEventHandler, &data->ehn );

	#ifdef MBX
	data->blinkrate = 0;
	#else
	data->blinkrate = getci( obj, MUICFG_Textinput_Blinkrate, 0 );
	#endif
	data->cursorstyle = getci( obj, MUICFG_Textinput_Cursorstyle, 0 );
	data->cursorsize = getci( obj, MUICFG_Textinput_Cursorstyle, 1 );
	data->findurls = getci( obj, data->noinput ? MUICFG_Textinput_FindURLNoInput : MUICFG_Textinput_FindURLInput, FALSE );
	data->maxundobytes = getci( obj, data->multiline ? MUICFG_Textinput_UndoBytesMulti : MUICFG_Textinput_UndoBytesSingle, data->multiline ? 2000 : 500 );
	data->maxundolevels = getci( obj, data->multiline ? MUICFG_Textinput_UndoLevelsMulti : MUICFG_Textinput_UndoLevelsSingle, data->multiline ? 200 : 50 );

	data->emailstyles = getci( obj, MUICFG_Textinput_HiliteQuotes, 1 );

#ifndef MBX
	vp = NULL;
	DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_KeyCount, &vp );
	if( vp )
	{
		data->numkeys = min( MAXKEYS, (int)*vp );
		for( c = 0; c < data->numkeys; c++ )
		{
			struct textinput_keydef *kd = NULL;

			DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_KeyBase + c, &kd );
			if( !kd )
				continue;

			data->keyspecs[ c ].action = kd->action;
			data->keyspecs[ c ].ix = parseix( kd->key );
		}
	}
	else
	{
		data->numkeys = NUMDEFKEYS;
		for( c = 0; c < NUMDEFKEYS; c++ )
		{
			data->keyspecs[ c ].action = defkeydefs[ c ].action;
			data->keyspecs[ c ].ix = parseix( defkeydefs[ c ].key );
		}
	}
#endif

	#ifndef MBX
	if( data->eeh )
		ee_setobj( data->eeh, obj );
	#endif

	vp = 0;
	DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_WordWrapOn, &vp );
	if( vp )
		data->dowordwrap = *vp ? TRUE : FALSE;

	vp = 0;
	DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_WordWrapAt, &vp );
	if( vp )
		data->wordwrap = *vp;

	if( data->font->tf_Flags & FPF_PROPORTIONAL )
		data->fontfixed = 0;
	else
		data->fontfixed = data->font->tf_XSize;

	findcursorpos( obj, data, &data->pxo, &data->cellsize );

#ifdef USE_SETTASKPRI
	data->oldpri = FindTask( 0 )->tc_Node.ln_Pri;
#endif

	if( data->popupstate < 0 )
	{
		vp = NULL;
		DoMethod( obj, MUIM_GetConfigItem, data->multiline ? MUICFG_Textinput_PopupMulti : MUICFG_Textinput_PopupSingle, &vp );
		if( vp )
			dopopup = *vp;
	}
	else
		dopopup = data->popupstate;

	#ifndef MBX
	if( dopopup && !data->noinput )
	{
		static struct NewMenu nm_multi[] = {
			{ NM_TITLE, "Menu", 0, 0, 0, 0 },
			{ NM_ITEM, "Cut", 0, 0, 0, (APTR)1 },
			{ NM_ITEM, "Copy", 0, 0, 0, (APTR)2 },
			{ NM_ITEM, "Paste", 0, 0, 0, (APTR)3 },
			{ NM_ITEM, NM_BARLABEL },
			{ NM_ITEM, "External edit...", 0, 0, 0, (APTR)10 },
			{ NM_ITEM, "Insert text file...", 0, 0, 0, (APTR)12 },
			{ NM_ITEM, NM_BARLABEL },
			{ NM_ITEM, "Word wrap?", 0, CHECKIT | MENUTOGGLE, 0, (APTR)11 },
			{ NM_ITEM, NM_BARLABEL },
			{ NM_ITEM, "Contact Manager", 0, 0, 0, (APTR)49 },
			{ NM_SUB,  "Name" , 0, 0, 0, (APTR)50 },
			{ NM_SUB,  "Last Name" , 0, 0, 0, (APTR)51 },
			{ NM_SUB,  "Address" , 0, 0, 0, (APTR)52 },
			{ NM_SUB,  "City" , 0, 0, 0, (APTR)53 },
			{ NM_SUB,  "Country" , 0, 0, 0, (APTR)54 },
			{ NM_SUB,  "Comment" , 0, 0, 0, (APTR)55 },
			{ NM_SUB,  "Alias" , 0, 0, 0, (APTR)56 },
			{ NM_SUB,  "Phone" , 0, 0, 0, (APTR)57 },
			{ NM_SUB,  "Mobile" , 0, 0, 0, (APTR)58 },
			{ NM_SUB,  "Fax" , 0, 0, 0, (APTR)59 },
			{ NM_SUB,  "EMail" , 0, 0, 0, (APTR)60 },
			{ NM_ITEM, "Style", 0, 0, 0, (APTR)61 },
			{ NM_SUB,  "Bold", 0, 0, 0, (APTR)62 },
			{ NM_SUB,  "Underline", 0, 0, 0, (APTR)63 },
			{ NM_SUB,  "Inverse", 0, 0, 0, (APTR)64 },
			{ NM_SUB,  "Reset", 0, 0, 0, (APTR)65 },
			{ NM_SUB,  "Beep", 0, 0, 0, (APTR)66 },
			NM_END
		};
		static struct NewMenu nm_single[] = {
			{ NM_TITLE, "Menu", 0, 0, 0, 0 },
			{ NM_ITEM, "Cut", 0, 0, 0, (APTR)1 },
			{ NM_ITEM, "Copy", 0, 0, 0, (APTR)2 },
			{ NM_ITEM, "Paste", 0, 0, 0, (APTR)3 },
			{ NM_ITEM, NM_BARLABEL },
			{ NM_ITEM, "Contact Manager", 0, 0, 0, (APTR)49 },
			{ NM_SUB,  "Name" , 0, 0, 0, (APTR)50 },
			{ NM_SUB,  "Last Name" , 0, 0, 0, (APTR)51 },
			{ NM_SUB,  "Address" , 0, 0, 0, (APTR)52 },
			{ NM_SUB,  "City" , 0, 0, 0, (APTR)53 },
			{ NM_SUB,  "Country" , 0, 0, 0, (APTR)54 },
			{ NM_SUB,  "Comment" , 0, 0, 0, (APTR)55 },
			{ NM_SUB,  "Alias" , 0, 0, 0, (APTR)56 },
			{ NM_SUB,  "Phone" , 0, 0, 0, (APTR)57 },
			{ NM_SUB,  "Mobile" , 0, 0, 0, (APTR)58 },
			{ NM_SUB,  "Fax" , 0, 0, 0, (APTR)59 },
			{ NM_SUB,  "EMail" , 0, 0, 0, (APTR)60 },
			{ NM_ITEM, "Style", 0, 0, 0, (APTR)61 },
			{ NM_SUB,  "Bold", 0, 0, 0, (APTR)62 },
			{ NM_SUB,  "Underline", 0, 0, 0, (APTR)63 },
			{ NM_SUB,  "Inverse", 0, 0, 0, (APTR)64 },
			{ NM_SUB,  "Reset", 0, 0, 0, (APTR)65 },
			{ NM_SUB,  "Beep", 0, 0, 0, (APTR)66 },
			NM_END
		};
		data->popupmenu = MUI_MakeObject( MUIO_MenustripNM, data->multiline ? nm_multi : nm_single );
		set( obj, MUIA_ContextMenu, data->popupmenu );
	}
	#endif

	calcmaxvals( data, obj );

	#ifndef MBX
	if( data->rexxPortBits )
	{
		data->rexxihn.ihn_Object = obj;
		data->rexxihn.ihn_Signals = data->rexxPortBits;
		data->rexxihn.ihn_Flags = 0;
		data->rexxihn.ihn_Method = MUIM_Textinput_HandleRexxSignal;
		DoMethod( _app( obj ), MUIM_Application_AddInputHandler, &data->rexxihn );
	}
	#endif

	data->quickrender = data->fontfixed && !data->styles;

	#ifndef MBX
	if( data->spellcheck )
		ispell_run();
	#endif

	return( TRUE );
}

DECMMETHOD( Cleanup )
{
	GETDATA;

	#ifndef MBX
	if( data->popupmenu )
	{
		set( obj, MUIA_ContextMenu, NULL );
		MUI_DisposeObject( data->popupmenu );
		data->popupmenu = NULL;
	}
	#endif

	#ifndef MBX
	if( data->eeh )
	{
		ee_setobj( data->eeh, NULL );
		DoMethod( _app( obj ), MUIM_Application_KillPushMethod, obj );
	}
	#endif

	data->isactive = FALSE;

	stopblink( obj, data );

	DoMethod( _win( obj ), MUIM_Window_RemEventHandler, &data->ehn );

	if( data->closefont )
	{
		CloseFont( data->closefont );
		data->closefont = NULL;
	}

	#ifndef MBX
	fc_free( data->farray );
	#endif

#ifdef USE_SETTASKPRI
	SetTaskPri( FindTask( 0 ), data->oldpri );
#endif

	#ifndef MBX
	if( data->rexxPortBits )
		DoMethod( _app( obj ), MUIM_Application_RemInputHandler, &data->rexxihn );
	#endif

	return( DOSUPER );
}

DECMMETHOD( Enable )
{
	GETDATA;
	if( data->popupmenu )
		set( obj, MUIA_ContextMenu, NULL );
	if( data->isactive )
		startblink( obj, data );
	return( DOSUPER );
}

DECMMETHOD( Disable )
{
	GETDATA;
	if( data->popupmenu )
		set( obj, MUIA_ContextMenu, data->popupmenu );
	if( data->isactive )
		stopblink( obj, data );
	return( DOSUPER );
}

DECMMETHOD( GoActive )
{
	GETDATA;
	int size;
#ifdef USE_SETTASKPRI
	struct Task *me = FindTask( 0 );
#endif

	data->isactive = TRUE;

 	// store undo data
	size = strlen( data->buffer ) + 1;

	if (data->undobuffer && size>data->undobuffersize)
	{
		FreeVec(data->undobuffer);
		data->undobuffer = NULL;
	}

	if (!data->undobuffer)
	{
		data->undobuffersize = size + 256; // alloc some more bytes to avoid constan reallocation on growing strings
		data->undobuffer = AllocVec(data->undobuffersize,MEMF_CLEAR);
	}

	if (data->undobuffer)
		strcpy(data->undobuffer,data->buffer);

	data->changed = FALSE;

	if( !data->isshown || !muiRenderInfo( obj ) || !( _window( obj )->ww_Flags & WFLG_WINDOWACTIVE ) )
		return( 0 );

#ifdef USE_SETTASKPRI
	SetTaskPri( me, data->oldpri + 2 );
#endif

	data->cursorstate = FALSE;
	data->redrawmode = RDO_SCROLL;
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	startblink( obj, data );

	return( 0 );
}

DECMMETHOD( GoInactive )
{
	GETDATA;
#ifdef USE_SETTASKPRI
	struct Task *me = FindTask( 0 );
#endif

	stopblink( obj, data );

	if( data->noinput )
		resetmarkmode( data );

	data->isactive = FALSE;
	data->redrawmode = RDO_SCROLL;
	MUI_Redraw( obj, MADF_DRAWUPDATE );

#ifdef USE_SETTASKPRI
	SetTaskPri( me, data->oldpri );
#endif

	return( 0 );
}

DECMMETHOD( Import )
{
//	GETDATA;
	ULONG id;
	char *contents;
	if((id = muiNotifyData( obj )->mnd_ObjectID))
	{
		if((contents = (char *)DoMethod( msg->dataspace, MUIM_Dataspace_Find, id )))
			set( obj, MUIA_Textinput_Contents, contents );
	}
	return( 0 );
}

DECMMETHOD( Export )
{
	GETDATA;
	ULONG id;
	if((id = muiNotifyData( obj )->mnd_ObjectID))
		DoMethod( msg->dataspace, MUIM_Dataspace_Add, data->buffer, strlen( data->buffer ) + 1, id );
	return( 0 );
}

DECGET
{
	GETDATA;

	switch( msg->opg_AttrID )
	{
		case MUIA_Textinput_NoCopy:
			*msg->opg_Storage = data->nocopy;
			return( TRUE );

		case MUIA_Textinput_Font:
			*msg->opg_Storage = data->fonttype;
			return( TRUE );

		case MUIA_Textinput_Bookmark1:
			*msg->opg_Storage = (ULONG)data->bookmarks[ 0 ];
			return( TRUE );

		case MUIA_Textinput_Bookmark2:
			*msg->opg_Storage = (ULONG)data->bookmarks[ 1 ];
			return( TRUE );

		case MUIA_Textinput_Bookmark3:
			*msg->opg_Storage = (ULONG)data->bookmarks[ 2 ];
			return( TRUE );

		case MUIA_Textinput_HandleURLHook:
			*msg->opg_Storage = (ULONG)data->handleurlhook;
			return( TRUE );

		case MUIA_Textinput_PreParse:
		case MUIA_Text_PreParse:
			*msg->opg_Storage = (ULONG)data->preparse;
			return( TRUE );

		case MUIA_Textinput_Format:
		case MUIA_String_Format:
			*msg->opg_Storage = data->format;
			return( TRUE );

		case MUIA_Textinput_Multiline:
			*msg->opg_Storage = data->multiline;
			return( TRUE );

		case MUIA_Textinput_SetMin:
		case MUIA_Text_SetMin:
			*msg->opg_Storage = data->setmin;
			return( TRUE );

		case MUIA_Textinput_SetMax:
		case MUIA_Text_SetMax:
			*msg->opg_Storage = data->setmax;
			return( TRUE );

		case MUIA_Textinput_SetVMin:
			*msg->opg_Storage = data->setvmin;
			return( TRUE );

		case MUIA_Textinput_SetVMax:
		case MUIA_Text_SetVMax:
			*msg->opg_Storage = data->setvmax;
			return( TRUE );

		case MUIA_Textinput_Styles:
			*msg->opg_Storage = data->styles;
			return( TRUE );

		case MUIA_Textinput_MaxLen:
		case MUIA_String_MaxLen:
			*msg->opg_Storage = data->maxlen;
			return( TRUE );

		case MUIA_Textinput_MaxLines:
			*msg->opg_Storage = data->maxlines;
			return( TRUE );

		case MUIA_Textinput_AutoExpand:
			*msg->opg_Storage = data->autoexpand;
			return( TRUE );

		case MUIA_Textinput_LeftOffset:
			*msg->opg_Storage = data->xoffset;
			return( TRUE );

		case MUIA_Textinput_TopOffset:
			*msg->opg_Storage = data->yoffset;
			return( TRUE );

		case MUIA_Textinput_TopLine:
			*msg->opg_Storage = data->topline;
			return( TRUE );

		case MUIA_Textinput_TSCO:
			*msg->opg_Storage = (ULONG)data->scrollobj;
			return( TRUE );

		case MUIA_Textinput_Blinkrate:
			*msg->opg_Storage = data->blinkrate;
			return( TRUE );

		case MUIA_Textinput_Cursorstyle:
			*msg->opg_Storage = data->cursorstyle;
			return( TRUE );

		case MUIA_Textinput_CursorSize:
			*msg->opg_Storage = data->cursorsize;
			return( TRUE );

		case MUIA_Textinput_AdvanceOnCR:
		case MUIA_String_AdvanceOnCR:
			*msg->opg_Storage = data->advanceoncr;
			return( TRUE );

		case MUIA_Textinput_TmpExtension:
			*msg->opg_Storage = (ULONG)data->tmpextension;
			return( TRUE );

		case MUIA_Textinput_Quiet:
			*msg->opg_Storage = data->quiet;
			return( TRUE );

		case MUIA_Textinput_WordWrap:
			*msg->opg_Storage = data->wordwrap;
			return( TRUE );

		case MUIA_Textinput_IsNumeric:
			*msg->opg_Storage = data->isnumeric;
			return( TRUE );

		case MUIA_Textinput_MinVal:
		case MUIA_Numeric_Min:
			*msg->opg_Storage = data->minnumval;
			return( TRUE );

		case MUIA_Textinput_MaxVal:
		case MUIA_Numeric_Max:
			*msg->opg_Storage = data->maxnumval;
			return( TRUE );

		case MUIA_Textinput_AcceptChars:
		case MUIA_String_Accept:
			*msg->opg_Storage = (ULONG)data->acceptchars;
			return( TRUE );

		case MUIA_Textinput_RejectChars:
		case MUIA_String_Reject:
			*msg->opg_Storage = (ULONG)data->rejectchars;
			return( TRUE );

		case MUIA_Textinput_AttachedList:
		case MUIA_String_AttachedList:
			*msg->opg_Storage = (ULONG)data->attachedlist;
			return( TRUE );

		case MUIA_Textinput_RemainActive:
			*msg->opg_Storage = data->remainactive;
			return( TRUE );

		case MUIA_Textinput_Secret:
		case MUIA_String_Secret:
			*msg->opg_Storage = data->issecret;
			return( TRUE );

		case MUIA_Textinput_Lines:
			*msg->opg_Storage = data->linenum;
			return( TRUE );

		case MUIA_Textinput_Editable:
			*msg->opg_Storage = !data->noedit;
			return( TRUE );

		case MUIA_Textinput_Contents:
		case MUIA_String_Contents:
		case MUIA_Text_Contents:
			*msg->opg_Storage = (ULONG)data->buffer;
			return( TRUE );

		case MUIA_Textinput_Integer:
		case MUIA_String_Integer:
		case MUIA_Numeric_Value:
			*msg->opg_Storage = max( myatoi( data->buffer ), data->minnumval );
			*msg->opg_Storage = min( *msg->opg_Storage, data->maxnumval );
			return( TRUE );

		case MUIA_Version:
			*msg->opg_Storage = VERSION;
			return( TRUE );

		case MUIA_Revision:
			*msg->opg_Storage = REVISION;
			return( TRUE );

#ifndef MBX
		case MUIA_Textinput_DefKeys:
			*msg->opg_Storage = (ULONG)defkeydefs;
			return( TRUE );
#endif

		case MUIA_Textinput_Changed:
			*msg->opg_Storage = (ULONG)data->changed;
			return( TRUE );

		case MUIA_Textinput_CursorPos:
			*msg->opg_Storage = (ULONG)data->cursorp - (ULONG)data->buffer;
			return( TRUE );

		case MUIA_Textinput_MarkStart:
			*msg->opg_Storage = ( data->markstart == NOMARK ) ? (ULONG)NOMARK : (ULONG)( data->markstart - data->buffer );
			return( TRUE );

		case MUIA_Textinput_MarkEnd:
			*msg->opg_Storage = ( data->markend == NOMARK ) ? (ULONG)NOMARK : (ULONG)( data->markend - data->buffer );
			return( TRUE );

		case MUIA_Textinput_NoInput:
			*msg->opg_Storage = (ULONG)data->noinput;
			return( TRUE );

		case MUIA_Textinput_ResetMarkOnCursor:
			*msg->opg_Storage = (ULONG)data->resetmarkoncursor;
			return( TRUE );
		case MUIA_Textinput_NoExtraSpacing:
			*msg->opg_Storage = (ULONG)data->noExtraSpacing;
			return( TRUE );			
	}
	return( DOSUPER );
}

DECMMETHOD( Textinput_Blink )
{
	GETDATA;

	data->cursorstate = !data->cursorstate;
	data->redrawmode = RDO_CURSOR;
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	return( 0 );
}

DECMMETHOD( Textinput_SaveToFile )
{
#ifndef MBX
	GETDATA;
	BPTR f;

	f = Open( msg->filename, MODE_NEWFILE );
	if( !f )
		return( FALSE );

	Write( f, data->buffer, strlen( data->buffer ) );
	Close( f );
#endif
	return( TRUE );
}

DECMMETHOD( Textinput_LoadFromFile )
{
#ifndef MBX
	GETDATA;
	BPTR f;
	int len;

	f = Open( msg->filename, MODE_OLDFILE );
	if( !f )
		return( FALSE );

	Seek( f, 0, OFFSET_END );
	len = Seek( f, 0, OFFSET_BEGINNING );

	if( !dobuffersize( data, len + 1 ) )
		len = data->maxlen;

	Read( f, data->buffer, len );
	Close( f );
	data->buffer[ len ] = 0;
	//dowordwrap( data, data->buffer );

	data->cursorx = data->cursory = 0;
	resetmarkmode( data );
	data->redrawmode = RDO_CONTENTS;
	if( muiRenderInfo( obj ) )
	{
		calcmaxvals( data, obj );
		checkcursorvisible( data, obj );
		docheckinform( data, obj );
	}
	findcursorp( data );
	endchange( obj, data, 0 );
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	return( (ULONG)len );
#else
	return(0);
#endif
}

DECMMETHOD( Textinput_ExternalEdit )
{
	#ifndef MBX

	GETDATA;
	static int exportcnt;
	ULONG *sync = NULL;

	if( data->issecret || data->noedit )
	{
		DisplayBeep( _screen( obj ) );
		return( 0 );
	}

	DoMethod( obj, MUIM_GetConfigItem, MUICFG_Textinput_EditSync, &sync );

	if( data->externaledit )
	{
		if( sync && *sync )
		{
			return( FALSE );
		}
		else
		{
			ee_restart( data->eeh );
			return( TRUE );
		}
	}

	sprintf( data->tmpfilename, "T:%lx-%lx.%s", FindTask( NULL ), exportcnt++, data->tmpextension );
	if( !DoMethod( obj, MUIM_Textinput_SaveToFile, data->tmpfilename ) )
		return( FALSE );

	data->eeh = ee_add( data->tmpfilename, obj, sync ? *sync : 0 );
	if( !data->eeh )
	{
		DeleteFile( data->tmpfilename );
		return( FALSE );
	}

	data->externaledit = TRUE;
	if( sync && *sync )
		set( data->scrollobj ? data->scrollobj : obj, MUIA_Disabled, TRUE );

	return( TRUE );

	#else

	return(FALSE);

	#endif
}

DECMMETHOD( Textinput_ExternalEditDone )
{
	#ifndef MBX

	GETDATA;

	if( data->externaledit )
	{
		if( msg->changed == 2 )
		{
			DoMethod( obj, MUIM_Textinput_LoadFromFile, data->tmpfilename );
			return( 0 );
		}

		ee_free( data->eeh );
		data->eeh = NULL;
		data->externaledit = FALSE;

		if( msg->changed == TRUE )
			DoMethod( obj, MUIM_Textinput_LoadFromFile, data->tmpfilename );
		else if( msg->changed == -1 )
		{
			if( MUI_Request( _app( obj ), _win( obj ), 0, "Textinput Class", "*\033bOpen config|OK",
				"\033cCalling the external editor has \033bfailed!\033n\n"
				"Make sure that you have specified the path\n"
				"correctly in the Textinput section of MUI prefs.\n\n"
				"Note also that the call needs to be \033bsynchronous\033n!\n"
				"If you use an Editor or frontend which normally\n"
				"detaches itself from the shell, you have to specify\n"
				"an option to avoid this (e.g. 'ed -sticky' for CED)"
			))
				DoMethod( _app( obj ), MUIM_Application_OpenConfigWindow );
		}
		DeleteFile( data->tmpfilename );

		set( data->scrollobj ? data->scrollobj : obj, MUIA_Disabled, FALSE );

		DoMethod( _win( obj ), MUIM_Window_ScreenToFront );
		set( _win( obj ), MUIA_Window_Activate, TRUE );
	}

	#endif

	return( NULL );
}

DECMMETHOD( Textinput_DoMarkAll )
{
	GETDATA;

	char *end = strchr( data->buffer, NULL );
	if( data->noinput )
	{
		if( end <= data->buffer )
			return( 0 );
		end--;
	}
	updatemark( data, data->buffer, end );
	data->markmode = 3;
	data->cursory = data->linenum - 1;
	data->cursorx = getlinelen( data, data->cursory );
	findcursorp( data );
	data->redrawmode = RDO_MOVEMARK;
	checkcursorvisible( data, obj );
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	return( 0 );
}

DECMMETHOD( Textinput_DoMarkStart )
{
	GETDATA;

	data->redrawmode = RDO_MOVEMARK;
	if( data->markmode )
	{
		resetmarkmode( data );
	}
	else
	{
		data->markmode = 3;
		updatemark( data, data->cursorp, data->cursorp );
		checkcursorvisible( data, obj );
	}

	MUI_Redraw( obj, MADF_DRAWUPDATE );

	return( 0 );
}

DECMMETHOD( Textinput_AppendText )
{
	GETDATA;
	int len = msg->len;

	if( len < 0 )
		len = strlen( msg->text );

	if( !len )
		return( TRUE );

	if( !doaction( data, ACTION_INSERT, strchr( data->buffer, 0 ), 0, msg->text, len ) )
		return( FALSE );

	if( muiRenderInfo( obj ) )
		calcmaxvals( data, obj );

	if( !data->quiet )
	{
		data->redrawmode = RDO_CONTENTS;
		MUI_Redraw( obj, MADF_DRAWUPDATE );
		docheckinform( data, obj );
	}

	endchange( obj, data, 0 );

	return( TRUE );
}

static ULONG insertstring( Class *cl, Object *obj, struct Data *data, char *s, int rc )
{
	int wascut = FALSE;

	if( rc < 0 )
		rc = strlen( s );

	if( data->isnumeric )
	{
		char *start = data->markmode == 3 ? data->markstart : data->cursorp;
		char *p = s;
		if( *p == '-' && ( start != data->buffer || data->minnumval >= 0 ) )
		{
			return( FALSE );
		}
		while( *p )
		{
			if( !tisnum( *p++ ) )
				return( FALSE );
		}
	}

	if( !data->multiline )
	{
		int c;

		for( c = 0; c < rc; c++ )
		{
			if( s[ c ] == '\n' || s[ c ] == '\r' )
				break;
		}
		if( c != rc )
		{
			wascut = rc;
			rc = c;
		}
	}

	if( data->markmode == 3 )
	{
		if( !doaction( data, ACTION_REPLACE, data->markstart, data->markend - data->markstart + 1, s, rc ) )
		{
			DisplayBeep( _screen( obj ) );
			return( FALSE );
		}

		findcursorxy( data, data->markstart );
		resetmarkmode( data );
	}
	else if( !doaction( data, ACTION_INSERT, data->cursorp, 0, s, rc ) )
	{
		DisplayBeep( _screen( obj ) );
		return( FALSE );
	}
	//dowordwrap( data, getlinestart( data, data->cursorp ) );
	data->cursorp += rc;
	findcursorxy( data, data->cursorp );
	data->redrawmode = RDO_CONTENTS;
	if( muiRenderInfo( obj ) )
	{
		calcmaxvals( data, obj );
		checkcursorvisible( data, obj );
	}
	docheckinform( data, obj );
	endchange( obj, data, 0 );
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	if( wascut )
	{
		if( !data->multiline && !data->remainactive )
			return( TRUE );

		DoMethod( obj, MUIM_Textinput_Acknowledge, data->buffer );
		data->cursorp = data->buffer;
		doaction( data, ACTION_DELETE, data->buffer, -1, NULL, 0 );
		s += rc + 1;
		rc = wascut - rc - 1;
		if( rc > 0 )
			return( insertstring( cl, obj, data, s, rc ) );
		else
		{
			findcursorxy( data, data->cursorp );

			data->redrawmode = RDO_CONTENTS;
			if( muiRenderInfo( obj ) )
			{
				calcmaxvals( data, obj );
				checkcursorvisible( data, obj );
			}
			MUI_Redraw( obj, MADF_DRAWUPDATE );
			docheckinform( data, obj );
		}
	}
	return( TRUE );
}

DECMMETHOD( Textinput_DoPaste )
{
	ULONG rc = 0;

	#ifndef MBX

	GETDATA;
	struct IFFHandle *iffh;
	APTR cliphandle;

	CHECKEDIT;

	if( iffh = AllocIFF() )
	{
		if( cliphandle = OpenClipboard( CLIPDEVICE ) )
		{
			iffh->iff_Stream = (ULONG)cliphandle;
			InitIFFasClip( iffh );

			if( !OpenIFF( iffh, IFFF_READ ) )
			{
				struct StoredProperty *sp;

				PropChunk( iffh, MAKE_ID('F','T','X','T'), MAKE_ID('C','H','R','S') );
				StopOnExit( iffh, MAKE_ID('F','T','X','T'), MAKE_ID('C','H','R','S') );
				for(;;)
				{
					int error = ParseIFF( iffh, IFFPARSE_SCAN );
					if( error != IFFERR_EOC )
						break;
					sp = FindProp( iffh, MAKE_ID('F','T','X','T'), MAKE_ID('C','H','R','S') );
					if( sp && sp->sp_Size )
					{
						STRPTR s = sp->sp_Data;
						rc += sp->sp_Size;
						insertstring( cl, obj, data, s, sp->sp_Size );
					}
				}
				CloseIFF( iffh );
			}
			CloseClipboard( cliphandle );
		}
		FreeIFF( iffh );
	}

	#endif // MBX

	return( rc );
}

DECMMETHOD( Textinput_DoCopy )
{
	GETDATA;
	ULONG rc = 0;

	if( data->issecret || data->nocopy )
	{
		DisplayBeep( _screen( obj ) );
		return( 0 );
	}

	if( data->markmode == 3 )
	{
		//if( !*data->markend )
		//	data->markend--;

		rc = writeclip( data->markstart, data->markend - data->markstart + 1 );

		resetmarkmode( data );
		data->redrawmode = RDO_MOVEMARK;
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else
		rc = writeclip( data->buffer, strlen( data->buffer ) );

	return( rc );
}


ULONG docut( Object *obj, struct Data *data, BOOL alwayscopy )
{
	ULONG rc = 0;

	if( data->issecret || data->noedit || data->nocopy )
	{
		DisplayBeep( _screen( obj ) );
		return( 0 );
	}

	if( data->markmode != 3 )
	{
		if( !data->multiline )
		{
			if( alwayscopy )
				rc = writeclip( data->buffer, strlen( data->buffer ) );
			set( obj, MUIA_Textinput_Contents, "" );
		}
		return( rc );
	}

	rc = writeclip( data->markstart, data->markend - data->markstart + 1 - ( *data->markend ? 0 : 1 ) );

	doaction( data, ACTION_DELETE, data->markstart, data->markend - data->markstart + 1, NULL, 0 );

	findcursorxy( data, data->markstart );
	resetmarkmode( data );
	data->redrawmode = RDO_CONTENTS;
	checkcursorvisible( data, obj );
	if( muiRenderInfo( obj ) )
		calcmaxvals( data, obj );
	docheckinform( data, obj );
	endchange( obj, data, 0 );
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	return( rc );
}

DECMMETHOD( Textinput_DoCut )
{
	GETDATA;
	return docut( obj, data, FALSE );
}

DECMMETHOD( Textinput_DoCopyCut )
{
	GETDATA;
	return docut( obj, data, TRUE );
}



ULONG cutline( Object *obj, struct Data *data, ULONG clip )
{
	char *start = data->cursorp, *end = data->cursorp;

	CHECKEDIT;

	/*if( data->issecret )
	{
		DisplayBeep( _screen( obj ) );
		return( 0 );
	}*/

	while( start > data->buffer && start[ -1 ] != '\n' )
		start--;
	while( *end && *end != '\n' )
		end++;

	if( clip )
		clip = writeclip( start, end - start + 1 );

	doaction( data, ACTION_DELETE, start, end - start + 1, NULL, 0 );

	findcursorxy( data, start );
	resetmarkmode( data );
	data->redrawmode = RDO_CONTENTS;
	checkcursorvisible( data, obj );
	if( muiRenderInfo( obj ) )
		calcmaxvals( data, obj );
	docheckinform( data, obj );
	endchange( obj, data, 0 );
	MUI_Redraw( obj, MADF_DRAWUPDATE );

	return( clip );
}

DECMMETHOD( Textinput_DoDelLine )
{
	GETDATA;
	return( cutline( obj, data, FALSE ) );
}

DECMMETHOD( Textinput_DoCutLine )
{
	GETDATA;
	return( cutline( obj, data, TRUE ) );
}

DECMMETHOD( ContextMenuChoice )
{
	GETDATA;
	switch( muiUserData( msg->item ) )
	{
		case 1:
			DoMethod( obj, MUIM_Textinput_DoCut );
			break;
		case 2:
			DoMethod( obj, MUIM_Textinput_DoCopy );
			break;
		case 3:
			DoMethod( obj, MUIM_Textinput_DoPaste );
			break;
		case 10:
			DoMethod( obj, MUIM_Textinput_ExternalEdit );
			break;
		case 12:
			DoMethod( obj, MUIM_Textinput_DoInsertFile );
			break;

		case 11:
			{
				ULONG v;
				get( msg->item, MUIA_Menuitem_Checked, &v );
				data->dowordwrap = v ? TRUE : FALSE;
			}
			break;

		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
			{
				int id = muiUserData( msg->item );
#ifdef MBX
				if( DoMethod( obj, MUIM_Textinput_HandleChar, id == 62 ? "\002" : id == 63 ? "\037" : id == 64 ? "\026" : id == 65 ? "\017" : "\007", FALSE ) )
#else
				if( DoMethod( obj, MUIM_Textinput_HandleChar, id == 62 ? '\002' : id == 63 ? '\037' : id == 64 ? '\026' : id == 65 ? '\017' : '\007', FALSE ) )
#endif
					MUI_Redraw( obj, MADF_DRAWUPDATE );
			}
	}

	#ifndef MBX
	if( CManagerBase && muiUserData( msg->item ) >= 50 && muiUserData( msg->item ) <= 60 )
	{
		struct CMUser *cmu = cm_getentry();

		if( cmu )
		{
			char *p = 0;

			switch( muiUserData( msg->item ) )
			{
				#define RE(n,x) case n: p=cmu->x;break

				RE( 50, Name );
				RE( 51, LastName );
				RE( 52, Address );
				RE( 53, City );
				RE( 54, Country );
				RE( 55, Comment );
				RE( 56, Alias );
				RE( 57, Phone );
				RE( 58, Mobile );
				RE( 59, Fax );
				RE( 60, EMail );
			}

			if( p )
				DoMethod( obj, MUIM_Textinput_InsertText, p, strlen( p ) );
		}
	}
	#endif

	return( 0 );
}

DECMMETHOD( ContextMenuBuild )
{
	#ifndef MBX

	GETDATA;

	if( data->popupmenu )
	{
		int i;
		APTR o = (APTR)DoMethod( data->popupmenu, MUIM_FindUData, 11 );
		if( o )
		{
			SetAttrs( o,
				MUIA_NoNotify, TRUE,
				MUIA_Menuitem_Checked, data->dowordwrap,
				MUIA_Menuitem_Enabled, !data->noedit,
				TAG_DONE
			);
		}
		o = (APTR)DoMethod( data->popupmenu, MUIM_FindUData, 1 );
		if( o )
		{
			SetAttrs( o,
				MUIA_Menuitem_Enabled, !data->noedit,
				TAG_DONE
			);
		}
		o = (APTR)DoMethod( data->popupmenu, MUIM_FindUData, 10 );
		if( o )
		{
			SetAttrs( o,
				MUIA_Menuitem_Enabled, !data->noedit,
				TAG_DONE
 			);
		}
		o = (APTR)DoMethod( data->popupmenu, MUIM_FindUData, 12 );
		if( o )
		{
			SetAttrs( o,
				MUIA_Menuitem_Enabled, !data->noedit,
				TAG_DONE
			);
		}

		#ifndef MBX
		for( i=49; i<=60; i++ )
		{
			o = (APTR)DoMethod( data->popupmenu, MUIM_FindUData, i );
			if( o )
				SetAttrs( o, MUIA_Menuitem_Enabled, CManagerBase ? TRUE : FALSE, TAG_DONE );
		}
		#endif

		for( i = 61; i <= 66; i++ )
		{
			o = (APTR)DoMethod( data->popupmenu, MUIM_FindUData, i );
			if( o )
				SetAttrs( o, MUIA_Menuitem_Enabled, data->styles == MUIV_Textinput_Styles_IRC ? TRUE : FALSE, TAG_DONE );
		}
	}

	#endif

	return( DOSUPER );
}

DECMMETHOD( Textinput_DoToggleWordwrap )
{
	GETDATA;

	data->dowordwrap = !data->dowordwrap;

	return( 0 );
}

DECMMETHOD( Textinput_Acknowledge )
{
	tnotify( obj,
		MUIA_Textinput_Acknowledge, msg->contents,
		MUIA_String_Acknowledge, msg->contents,
		TAG_DONE
	);
	return( 0 );
}

DECMMETHOD( Textinput_TranslateEvent )
{
	return( 0 );
}

DECMMETHOD( Textinput_InsertText )
{
	GETDATA;

	insertstring( cl, obj, data, msg->text, msg->len );

	return( 0 );
}

DECMMETHOD( Textinput_DoLeft )
{
	GETDATA;
	if( data->cursorx )
	{
		updatecursor( data, obj, data->cursorx - PREVCHARLEN(data->buffer + data->cursorx), data->cursory );
	}
	else if( data->multiline && data->cursory )
	{
		// FIXME - UTF8
		updatecursor( data, obj, getlinelen( data, data->cursory - 1), data->cursory - 1);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoRight )
{
	GETDATA;
	if( data->cursorx < getlinelen( data, data->cursory ) )
	{
		updatecursor(data,obj,data->cursorx + CHARLEN(data->buffer[data->cursorx]),data->cursory);
	}
	else if( data->multiline && data->cursory < ( data->linenum - 1 ) )
	{
		updatecursor(data,obj,0,data->cursory+1);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoUp )
{
	GETDATA;

	if( data->cursory )
	{
		updatecursor(data,obj,min(getlinelen(data,data->cursory-1),data->cursorx),data->cursory-1);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoDown )
{
	GETDATA;
	if( data->cursory < ( data->linenum - 1 ) )
	{
		updatecursor( data, obj, min( getlinelen( data, data->cursory + 1), data->cursorx ), data->cursory + 1);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoPrevWord )
{
	GETDATA;
	if( data->cursorx )
	{
		int cursorx=data->cursorx;
		char *cursorp=data->cursorp;
		int hf = 0;

		while( cursorx && isalnum( (int)cursorp[-1] ) )
			cursorx--, cursorp--, hf = TRUE;

		if( !hf )
		{
			while( cursorx && !isalnum( (int)cursorp[-1] ) )
				cursorx--, cursorp--;

			while( cursorx && isalnum( (int)cursorp[-1] ) )
				cursorx--, cursorp--;

		}
		updatecursor(data,obj,cursorx,data->cursory);
		return( 0 );
	}

	return( DoMethod( obj, MUIM_Textinput_DoLeft ) );
}

DECMMETHOD( Textinput_DoNextWord )
{
	GETDATA;
	if( *data->cursorp != '\n' )
	{
		int cursorx=data->cursorx;
		char *cursorp=data->cursorp;
		while( isalnum( (int)*cursorp ) )
			cursorx++, cursorp++;
		while( *cursorp && *cursorp != '\n' && !isalnum( (int)*cursorp ) )
			cursorx++, cursorp++;
		updatecursor(data,obj,cursorx,data->cursory);
		return( 0 );
	}
	return( DoMethod( obj, MUIM_Textinput_DoRight ) );
}

DECMMETHOD( Textinput_DoLineStart )
{
	GETDATA;
	if( data->cursorx )
	{
		updatecursor( data, obj, 0, data->cursory );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoLineEnd )
{
	GETDATA;

	if( data->cursorx < getlinelen( data, data->cursory ) )
	{
		updatecursor( data, obj, getlinelen( data, data->cursory ), data->cursory);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoTop )
{
	GETDATA;

	if( data->cursory )
	{
		updatecursor( data, obj, min( getlinelen( data, 0 ), data->cursorx ), 0);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoBottom )
{
	GETDATA;

	if( data->cursory < ( data->linenum - 1 ) )
	{
		updatecursor( data, obj, min( getlinelen( data, data->linenum - 1), data->cursorx ), data->linenum - 1 );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoPageUp )
{
	GETDATA;

	if( data->cursory )
	{
		int cursory=data->cursory - min( data->visiblelines, data->cursory );
		updatecursor( data, obj, min( getlinelen( data, cursory ), data->cursorx ), cursory);
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoPageDown )
{
	GETDATA;

	if( data->cursory < ( data->linenum - 1 ) )
	{
		int cursory = data->cursory + max( 0, min( data->visiblelines, data->linenum - data->cursory - 1 ) );
		updatecursor( data, obj, min( getlinelen( data, cursory ), data->cursorx ), cursory );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoPopup )
{
	GETDATA;

	if( data->attachedpop )
	{
		DoMethod( data->attachedpop, MUIM_Popstring_Open );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoDel )
{
	GETDATA;

	CHECKEDIT;

	if( data->markmode == 3 )
	{
		doaction( data, ACTION_DELETE, data->markstart, data->markend - data->markstart + 1, NULL, 0 );
		findcursorxy( data, data->markstart );
		resetmarkmode( data );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CONTENTS;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else if( *data->cursorp && *data->cursorp != '\n' )
	{
		data->redrawmode = NEGKERN( data->cursorp ) ? RDO_CURRENT : RDO_FROMCURSOR;
		doaction( data, ACTION_DELETE, data->cursorp, 1, NULL, 0 );
		checkcursorvisible( data, obj );
		data->oldpxo = data->pxo;
		if( muiRenderInfo( obj ) )
			calcmaxvalline( data, obj, data->cursory );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else if( data->multiline && data->cursory < ( data->linenum - 1 ) )
	{
		doaction( data, ACTION_DELETE, data->cursorp, 1, NULL, 0 );
		data->linenum--;
		//dowordwrap( data, getlinestart( data, data->cursorp ) );
		findcursorp( data );
		data->redrawmode = RDO_CONTENTS;
		checkcursorvisible( data, obj );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}

	return( 0 );
}

DECMMETHOD( Textinput_DoDelEOL )
{
	GETDATA;

	CHECKEDIT;

	if( data->markmode == 3 )
	{
		return( DoMethod( obj, MUIM_Textinput_DoDel ) );
	}

	if( *data->cursorp && *data->cursorp != '\n' )
	{
		char *s = data->cursorp;
		while( *s && *s != '\n' )
			s++;
		data->redrawmode = NEGKERN( data->cursorp ) ? RDO_CURRENT : RDO_FROMCURSOR;
		doaction( data, ACTION_DELETE, data->cursorp, s - data->cursorp, NULL, 0 );
		//dowordwrap( data, getlinestart( data, data->cursorp ) );
		checkcursorvisible( data, obj );
		data->oldpxo = data->pxo;
		if( muiRenderInfo( obj ) )
			calcmaxvalline( data, obj, data->cursory );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}

	return( 0 );
}

DECMMETHOD( Textinput_DoBS )
{
	GETDATA;

	CHECKEDIT;

	if( data->markmode == 3 )
	{
		doaction( data, ACTION_DELETE, data->markstart, data->markend - data->markstart + 1, NULL, 0 );
		findcursorxy( data, data->markstart );
		resetmarkmode( data );
		calcmaxvals( data, obj );
		findcursorp( data );
		data->redrawmode = RDO_CONTENTS;
		checkcursorvisible( data, obj );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else if( data->cursorx )
	{
		ULONG prevcharlen = PREVCHARLEN(data->cursorp);
		data->redrawmode = NEGKERN( data->cursorp - prevcharlen) ? RDO_CURRENT : RDO_FROMCURSOR;
		doaction( data, ACTION_DELETE, data->cursorp - prevcharlen, prevcharlen, NULL, 0 );
		data->cursorx -= prevcharlen;
		findcursorp( data );
		checkcursorvisible( data, obj );
		data->oldpxo = data->pxo;
		if( muiRenderInfo( obj ) )
			calcmaxvalline( data, obj, data->cursory );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else if( data->multiline && data->cursory )
	{
		data->cursorx = getlinelen( data, data->cursory - 1 );
		data->cursory--;
		doaction( data, ACTION_DELETE, data->cursorp - 1, 1, NULL, 0 );
		data->linenum--;
		findcursorp( data );
		data->redrawmode = RDO_CONTENTS;
		checkcursorvisible( data, obj );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}

	return( 0 );
}

DECMMETHOD( Textinput_DoBSSOL )
{
	GETDATA;

	CHECKEDIT;

	if( data->markmode == 3 )
	{
		return( DoMethod( obj, MUIM_Textinput_DoBS ) );
	}
	else if( data->cursorx )
	{
		doaction( data, ACTION_DELETE, data->cursorp - data->cursorx, data->cursorx, NULL, 0 );
		data->cursorx = 0;
		findcursorp( data );
		checkcursorvisible( data, obj );
		data->redrawmode = RDO_CURRENT;
		if( muiRenderInfo( obj ) )
			calcmaxvalline( data, obj, data->cursory );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoBSWord )
{
	GETDATA;

	CHECKEDIT;

	if( data->markmode == 3 )
	{
		return( DoMethod( obj, MUIM_Textinput_DoBS ) );
	}
	else
	{
		char *p = data->cursorp;
		if( data->cursorx == 0 )
		{
			if( p > data->buffer )
				p--;
			else
				return( 0 );
			data->redrawmode = RDO_CONTENTS;
		}
		else
		{
			if( p > data->buffer )
				p--;
			while( p > data->buffer && *p != '\n' && isspace( *p ) )
				p--;
			while( p > data->buffer && !isspace( *p ) )
				p--;
			if( p > data->buffer || *p == '\n' )
				p++;
			if( p >= data->cursorp )
				return( 0 );
			data->redrawmode = RDO_FROMCURSOR;
		}
		data->redrawmode = ( data->redrawmode == RDO_FROMCURSOR && NEGKERN( p ) ) ? RDO_CURRENT : data->redrawmode;
		doaction( data, ACTION_DELETE, p, data->cursorp - p, NULL, 0 );
		data->cursorp = p;
		findcursorxy( data, data->cursorp );
		checkcursorvisible( data, obj );
		data->oldpxo = data->pxo;
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoDelWord )
{
	GETDATA;

	CHECKEDIT;

	if( data->markmode == 3 )
	{
		return( DoMethod( obj, MUIM_Textinput_DoBS ) );
	}
	else
	{
		char *p = data->cursorp;

		if( *p == '\n' )
		{
			p++;
			data->redrawmode = RDO_CONTENTS;
		}
		else
		{
			while( *p && !isspace( *p ) )
				p++;
			while( *p && *p != '\n' && isspace( *p ) )
				p++;

			if( p <= data->cursorp )
				return( 0 );
			data->redrawmode = RDO_FROMCURSOR;
		}
		data->redrawmode = ( data->redrawmode == RDO_FROMCURSOR && NEGKERN( data->cursorp ) ) ? RDO_CURRENT : data->redrawmode;
		doaction( data, ACTION_DELETE, data->cursorp, p - data->cursorp, NULL, 0 );
		findcursorxy( data, data->cursorp );
		checkcursorvisible( data, obj );
		data->oldpxo = data->pxo;
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		docheckinform( data, obj );
		endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoRevert )
{
	GETDATA;

	if( !data->changed )
		return( 0 );

	if( data->undobuffer )
	{
		SetAttrs( obj,
			MUIA_Textinput_Contents, data->undobuffer,
			TAG_DONE
		);
	}
	else
		DisplayBeep( _screen( obj ) );

	return( 0 );
}

DECMMETHOD( Textinput_DoubleClick )
{
	GETDATA;

	data->markmethod = msg->cnt + 1;
	if( ( data->multiline && data->dblclickcount > 3 ) || ( !data->multiline && data->dblclickcount > 2 ) )
		data->markmethod = data->dblclickcount = 0;

	return( 0 );
}

DECMMETHOD( Textinput_InsertFromFile )
{
#ifndef MBX
	GETDATA;
	char buff[ 512 ];
	BPTR f;
	int rc;

	f = Open( msg->filename, MODE_OLDFILE );
	if( !f )
		return( FALSE );

	while( ( rc = Read( f, buff, sizeof( buff ) ) )	> 0 )
		insertstring( cl, obj, data, buff, rc ); 

	Close( f );
#endif
	return( TRUE );
}

DECMMETHOD( Textinput_DoInsertFile )
{
	ULONG rc = FALSE;

#ifndef MBX

	struct FileRequester *fr;
	char path[ 256 ];

	fr = MUI_AllocAslRequestTags( ASL_FileRequest,
		ASLFR_Screen, _screen( obj ),
		ASLFR_TitleText, "Insert text file...",
		ASLFR_RejectIcons, TRUE,
		ASLFR_InitialDrawer, classinsertpath,
		TAG_DONE
	);

	if( fr )
	{
		set( _app( obj ), MUIA_Application_Sleep, TRUE );
		DoMethod( _app( obj ), MUIM_Application_CheckRefresh );
		if( MUI_AslRequestTags( fr, TAG_DONE ) )
		{
			DoMethod( _app( obj ), MUIM_Application_CheckRefresh );
			strcpy( path, fr->fr_Drawer );
			AddPart( path, fr->fr_File, sizeof( path ) );
			rc = DoMethod( obj, MUIM_Textinput_InsertFromFile, path );
		}
		set( _app( obj ), MUIA_Application_Sleep, FALSE );
		stccpy( classinsertpath, fr->fr_Drawer, sizeof( classinsertpath ) );

		MUI_FreeAslRequest( fr );
	}

#endif // MBX

	return( rc );
}

char togglecase( struct Data *data, char *p )
{
	char other = ToUpper( *p );
	if( other != *p )
	{
		doaction( data, ACTION_REPLACE, p, 1, &other, 1 );
		return( other );
	}
	other = ToLower( *p );
	if( other != *p )
	{
		doaction( data, ACTION_REPLACE, p, 1, &other, 1 );
		return( other );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoToggleCase )
{
	GETDATA;

	if( data->markmode == 3 )
	{
		char *p;
		int changed = FALSE;
		for( p = data->markstart; p <= data->markend; p++ )
		{
			changed = togglecase( data, p ) || changed;
		}
		findcursorxy( data, *data->markend ? data->markend + 1 : data->markend );
		resetmarkmode( data );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CONTENTS;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );

		if( changed )
			endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else if( *data->cursorp )
	{
		int changed = togglecase( data, data->cursorp );
		findcursorxy( data, data->cursorp + 1 );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CURRENT;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );

		if( changed )
		{
			endchange( obj, data, 0 );
		}
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	return( 0 );
}

DECMMETHOD( Textinput_DoToggleCaseEOW )
{
	GETDATA;

	if( data->markmode == 3 )
	{
		char *p;
		int changed = FALSE;
		for( p = data->markstart; p <= data->markend; p++ )
		{
			changed = togglecase( data, p ) || changed;
		}
		findcursorxy( data, *data->markend ? data->markend + 1 : data->markend );
		resetmarkmode( data );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CONTENTS;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );

		if( changed )
			endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else if( *data->cursorp )
	{
		int changed = FALSE;
		char *end = getwordend( data, data->cursorp );
		while( data->cursorp <= end )
			changed = togglecase( data, data->cursorp++ ) || changed;
		findcursorxy( data, data->cursorp );
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CURRENT;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );

		if( changed )
			endchange( obj, data, 0 );
		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	return( 0 );
}

int changedec( APTR obj, struct Data *data, int change )
{
	char numstr[14];
	int num;

	if( data->markmode == 3 && !data->isnumeric )
	{
		char *p = data->markstart;
		int len = data->markend - data->markstart + 1;
		int off;
		if( len > 10 )
			return( 0 );
		if( *p == '-' && p < data->markend )
			p++;
		while( p < data->markend )
		{
			if( !tisnum( *p++ ) )
				return( 0 );
		}
		strncpy( numstr, data->markstart, len );
		numstr[ len ] = 0;
		num = myatoi( numstr ) + change;
		sprintf( numstr, "%ld", num );
		if( !doaction( data, ACTION_REPLACE, data->markstart, data->markend - data->markstart + 1, numstr, -1 ) )
		{
			DisplayBeep( _screen( obj ) );
			return( 0 );
		}
		off = strlen( numstr ) - len;
		if( off )
		{
			updatemark( data, data->markstart, data->markend + off );
			findcursorxy( data, STRMAX( data->markstart, data->cursorp + off ) );
		}
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CURRENT;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );

		endchange( obj, data, 0 );

		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	else
	{
		char *p = data->cursorp;
		int len, off;
		if( !*p )
		{
			if( p > data->buffer )
				p--;
			else
				return( 0 );
		}
		if( !strchr( "-0123456789", *p ) )
			return( 0 );
		p = getnumstart( data, p );
		len = getnumend( data, p ) - p + 1;
		if( len > 10 )
			return( 0 );
		strncpy( numstr, p, len );
		numstr[ len ] = 0;
		num = myatoi( numstr ) + change;
		if( data->isnumeric && ( ( num < 0 && p != data->buffer ) || num < data->minnumval || num > data->maxnumval ) )
		{
			DisplayBeep( _screen( obj ) );
			return( FALSE );
		}
		sprintf( numstr, "%ld", num );
		if( !doaction( data, ACTION_REPLACE, p, len, numstr, -1 ) )
		{
			DisplayBeep( _screen( obj ) );
			return( 0 );
		}
		off = strlen( numstr ) - len;
		if( off )
		{
			if( data->markmode == 3 )
				updatemark( data, data->markstart, data->markend + off );
			findcursorxy( data, STRMAX( data->cursorp + off, p ) );
		}
		if( muiRenderInfo( obj ) )
			calcmaxvals( data, obj );
		data->redrawmode = RDO_CURRENT;
		checkcursorvisible( data, obj );
		docheckinform( data, obj );

		endchange( obj, data, 0 );

		MUI_Redraw( obj, MADF_DRAWUPDATE );
	}
	return( 1 );
}

DECMMETHOD( Textinput_DoIncrementDec )
{
	GETDATA;

	changedec( obj, data, 1 );

	return( 0 );
}

DECMMETHOD( Textinput_DoDecrementDec )
{
	GETDATA;

	changedec( obj, data, -1 );

	return( 0 );
}

ULONG invertfirstnode( Object *obj, struct Data *data, struct MinList *list )
{
	struct actionnode *n;
	char *rcp;
	n = FIRSTNODE( list );
	if( NEXTNODE( n ) )
	{
		if((rcp = undoaction( data, n )))
		{
			resetmarkmode( data );
			findcursorxy( data, rcp );
			data->redrawmode = RDO_CONTENTS;
			if( muiRenderInfo( obj ) )
				calcmaxvals( data, obj );
			checkcursorvisible( data, obj );
			docheckinform( data, obj );
			endchange( obj, data, 0 );
			MUI_Redraw( obj, MADF_DRAWUPDATE );
			return( 0 );
		}
	}
	DisplayBeep( _screen( obj ) );
	return( 0 );
}

DECMMETHOD( Textinput_DoUndo )
{
	GETDATA;
	return( invertfirstnode( obj, data, &data->undolist ) );
}

DECMMETHOD( Textinput_DoRedo )
{
	GETDATA;
	return( invertfirstnode( obj, data, &data->redolist ) );
}

DECMMETHOD( Textinput_DoTab )
{
/*
	GETDATA;
	int spaces;
	char *rcp;
	char ch;
	if( data->markmode || data->tabs == MUIV_Textinput_Tabs_Ignore || !data->tablen )
		return( 0 ); // TEMP!!!!!!!!! Should do nextgadget?
	ch = data->tabs == MUIV_Textinput_Tabs_Spaces ? ' ' : '\t';
	if( data->isnumeric || ( data->rejectchars && strchr( data->rejectchars, ch ) ) || ( data->acceptchars && !strchr( data->acceptchars, ch ) ) )
	{
		DisplayBeep( _screen( obj ) );
		return( 0 );
	}
	spaces = data->cursorx % data->tablen;
	if( !spaces )
		spaces = data->tablen;
	if( !( rcp = doaction( data, ACTION_INSERT, data->cursorp, 0, "                ", spaces ) ) )
	{
		DisplayBeep( _screen( obj ) );
		return( 0 );
	}
	findcursorxy( data, rcp );
	if( !data->quiet )
	{
		calcmaxvals( data, obj );
		checkcursorvisible( data, obj );
	}
	MUI_Redraw( obj, MADF_DRAWOBJECT );
*/
	return( 0 );
}

DECMMETHOD( Textinput_DoNextGadget )
{
	set( _win( obj ), MUIA_Window_ActiveObject, MUIV_Window_ActiveObject_Next );
	return( 0 );
}

DECMMETHOD( Textinput_DoSetBookmark1 )
{
	GETDATA;
	data->bookmarks[ 0 ] = data->cursorp - data->buffer;
	tnotify( obj, MUIA_Textinput_Bookmark1, data->bookmarks[ 0 ], TAG_DONE );
	return( 0 );
}

DECMMETHOD( Textinput_DoSetBookmark2 )
{
	GETDATA;
	data->bookmarks[ 1 ] = data->cursorp - data->buffer;
	tnotify( obj, MUIA_Textinput_Bookmark2, data->bookmarks[ 1 ], TAG_DONE );
	return( 0 );
}

DECMMETHOD( Textinput_DoSetBookmark3 )
{
	GETDATA;
	data->bookmarks[ 2 ] = data->cursorp - data->buffer;
	tnotify( obj, MUIA_Textinput_Bookmark3, data->bookmarks[ 2 ], TAG_DONE );
	return( 0 );
}

void gotobm( struct Data *data, Object *obj, int bm )
{
	char *newp = data->buffer + min( data->bookmarks[ bm ], strlen( data->buffer ) );
	if( newp != data->cursorp )
	{
		int x, y;
		findxyp( data, newp, &x, &y, NULL );
		updatecursor( data, obj, x, y );
	}
}

DECMMETHOD( Textinput_DoGotoBookmark1 )
{
	GETDATA;
	gotobm( data, obj, 0 );
	return( 0 );
}

DECMMETHOD( Textinput_DoGotoBookmark2 )
{
	GETDATA;
	gotobm( data, obj, 1 );
	return( 0 );
}

DECMMETHOD( Textinput_DoGotoBookmark3 )
{
	GETDATA;
	gotobm( data, obj, 2 );
	return( 0 );
}

DECMMETHOD( Textinput_HandleChar )
{
	GETDATA;

	return( handlechar( cl, obj, data, msg->ch, msg->quiet ) );
}

DECMMETHOD( Textinput_HandleURL )
{
	#ifndef MBX
	GETDATA;

	if( !( data->handleurlhook && CallHook( data->handleurlhook, obj, msg->url ) ) )
		VAT_ShowURL( msg->url, NULL, _app( obj ) );
	#endif

	return( TRUE );
}

DECMMETHOD( Textinput_HandleRexxSignal )
{
	#ifndef MBX
	GETDATA;

	return( disprexxport( data ) );
	#else
	return(0);
	#endif
}

DECMMETHOD( Textinput_HandleMisspell )
{
	GETDATA;
	char *ws = msg->pos;
	int len;
	if( !data->spellcheck )
		return( 0 );
	len = strlen( (char *)msg->word );
	if( ws >= data->buffer && ws + len <= (char *)strchr( data->buffer, 0 ) && !strncmp( ws, msg->word, len ) )
	{
		struct misspell *ms = AllocPooled( data->pool, sizeof( *ms ) + strlen( msg->word ) + 1 );
		if( ms )
		{
			int oldredrawmode = data->redrawmode;
			strcpy( ms->misspell, msg->word );
			ms->word.xstart = ws - data->buffer;
			ms->word.xsize = len;
			ms->next = data->misspells;
			data->misspells = ms;
			data->redrawmisspell = ms;
			data->redrawmode = RDO_MISSPELL;
			MUI_Redraw( obj, MADF_DRAWUPDATE );
			data->redrawmode = oldredrawmode;
		}
		return( 1 );
	}
	return( 0 );
}

#ifdef MBX
DISPATCHERNAME_BEGIN(Textinput)
#else
__asm __saveds ULONG dispfunc(register __a0 struct IClass *cl,register __a2 Object *obj,register __a1 Msg msg)
#endif
{
	switch( msg->MethodID )
	{
		DEFMETHOD( OM_NEW )
		DEFMETHOD( OM_DISPOSE )
		DEFMETHOD( OM_SET )
		DEFMETHOD( OM_GET )
		DEFMMETHOD( Draw )
		DEFMMETHOD( HandleEvent )
		DEFMMETHOD( AskMinMax )
		DEFMMETHOD( Show )
		DEFMMETHOD( Setup )
		DEFMMETHOD( Cleanup )
		DEFMMETHOD( Hide )
		DEFMMETHOD( GoActive )
		DEFMMETHOD( GoInactive )
		DEFMMETHOD( Disable )
		DEFMMETHOD( Enable )
		DEFMMETHOD( Import )
		DEFMMETHOD( Export )
		DEFMMETHOD( ContextMenuChoice )
		DEFMMETHOD( ContextMenuBuild )
		DEFMMETHOD( Textinput_Blink )
		DEFMMETHOD( Textinput_SaveToFile )
		DEFMMETHOD( Textinput_LoadFromFile )
		DEFMMETHOD( Textinput_ExternalEdit )
		DEFMMETHOD( Textinput_ExternalEditDone )
		DEFMMETHOD( Textinput_HandleChar )
		DEFMMETHOD( Textinput_DoMarkAll )
		DEFMMETHOD( Textinput_DoMarkStart )
		DEFMMETHOD( Textinput_DoPaste )
		DEFMMETHOD( Textinput_DoCopy )
		DEFMMETHOD( Textinput_DoCut )
		DEFMMETHOD( Textinput_DoCutLine )
		DEFMMETHOD( Textinput_DoRevert )
		DEFMMETHOD( Textinput_DoToggleWordwrap )
		DEFMMETHOD( Textinput_AppendText )
		DEFMMETHOD( Textinput_InsertText )
		DEFMMETHOD( Textinput_Acknowledge )
		DEFMMETHOD( Textinput_TranslateEvent )
		DEFMMETHOD( Textinput_DoLeft )
		DEFMMETHOD( Textinput_DoRight )
		DEFMMETHOD( Textinput_DoUp )
		DEFMMETHOD( Textinput_DoDown )
		DEFMMETHOD( Textinput_DoPageUp )
		DEFMMETHOD( Textinput_DoPageDown )
		DEFMMETHOD( Textinput_DoTop )
		DEFMMETHOD( Textinput_DoBottom )
		DEFMMETHOD( Textinput_DoLineStart )
		DEFMMETHOD( Textinput_DoLineEnd )
		DEFMMETHOD( Textinput_DoNextWord )
		DEFMMETHOD( Textinput_DoPrevWord )
		DEFMMETHOD( Textinput_DoPopup )
		DEFMMETHOD( Textinput_DoDel )
		DEFMMETHOD( Textinput_DoDelWord )
		DEFMMETHOD( Textinput_DoDelEOL )
		DEFMMETHOD( Textinput_DoBS )
		DEFMMETHOD( Textinput_DoBSWord )
		DEFMMETHOD( Textinput_DoBSSOL )
		DEFMMETHOD( Textinput_DoInsertFile )
		DEFMMETHOD( Textinput_DoToggleCase )
		DEFMMETHOD( Textinput_DoToggleCaseEOW )
		DEFMMETHOD( Textinput_DoIncrementDec )
		DEFMMETHOD( Textinput_DoDecrementDec )
		DEFMMETHOD( Textinput_DoUndo )
		DEFMMETHOD( Textinput_DoRedo )
		DEFMMETHOD( Textinput_DoTab )
		DEFMMETHOD( Textinput_DoNextGadget )
		DEFMMETHOD( Textinput_DoSetBookmark1 )
		DEFMMETHOD( Textinput_DoSetBookmark2 )
		DEFMMETHOD( Textinput_DoSetBookmark3 )
		DEFMMETHOD( Textinput_DoGotoBookmark1 )
		DEFMMETHOD( Textinput_DoGotoBookmark2 )
		DEFMMETHOD( Textinput_DoGotoBookmark3 )
		DEFMMETHOD( Textinput_DoDelLine )
		DEFMMETHOD( Textinput_DoCopyCut )

		DEFMMETHOD( Textinput_InsertFromFile )
		DEFMMETHOD( Textinput_DoubleClick )
		DEFMMETHOD( Textinput_HandleURL )
		DEFMMETHOD( Textinput_HandleRexxSignal )
		DEFMMETHOD( Textinput_HandleMisspell )
	}
	return( DOSUPER );
}
#ifdef MBX
DISPATCHER_END
#endif

#ifdef _PROFILE
extern void __asm _STD_150_Sprof( void );
extern void __asm _STI_150_Sprof( void );
#endif


#ifdef MBX

ULONG TextinputGetSize(VOID)
{
	return(sizeof(struct Data));
}

#endif



BOOL ClassInitFunc( struct Library *base )
{
	makealnumtable();

#ifndef MBX
	memset( &pixsem, 0, sizeof( pixsem ) );
	InitSemaphore( &pixsem );
	NewList( (struct List *)&pixlist );

	NewList( (struct List *)&fontcachelist );

	if( !( VATBase = OpenLibrary( "vapor_toolkit.library", 0 ) ) )
		return( FALSE );
	if( !( DiskfontBase = VAT_OpenLibraryCode( VATOC_DISKFONT ) ) )
		return( FALSE );
	if( !( KeymapBase = VAT_OpenLibrary( "keymap.library", 37 ) ) )
		return( FALSE );
	if( !( CxBase = VAT_OpenLibraryCode( VATOC_COMMODITIES ) ) )
		return( FALSE );
	if( !( IFFParseBase = VAT_OpenLibraryCode( VATOC_IFFPARSE ) ) )
		return( FALSE );
	if( !( LayersBase = VAT_OpenLibrary( "layers.library", 37 ) ) )
		return( FALSE );
	if( !( RexxSysBase = VAT_OpenLibraryCode( VATOC_REXXSYS ) ) )
		return( FALSE );

	CyberGfxBase = OpenLibrary( "cybergraphics.library", 1 );

	CManagerBase = OpenLibrary( "CManager.library", 1 );

	TimerBase = (struct Library *)FindName( &SysBase->DeviceList, "timer.device" );

	#if INCLUDE_VERSION < 44
	LocaleBase = OpenLibrary( "locale.library", 38 );
	#else
	LocaleBase = (struct LocaleBase *)OpenLibrary( "locale.library", 38 );
	#endif
	if( LocaleBase )
	{
		catalog = OpenCatalog( NULL, "Textinput_mcc.catalog",
			OC_BuiltInLanguage, "english",
			TAG_DONE
		);
	}

	notifyclass = MUI_GetClass( MUIC_Notify );

	if( MUIMasterBase->lib_Version >= 20 )
		ThisClass->mcc_Class->cl_ID = "Textinput.mcc";
#else
	AudioBase = (AudioData_p)OpenModule( AUDIONAME, AUDIOVERSION );
#endif // MBX

#ifdef _PROFILE
	_STI_150_Sprof();
#endif

	return( TRUE );
}

VOID ClassExitFunc( struct Library *base )
{
#ifndef MBX
	struct Node *n;
#endif

#ifdef _PROFILE
	_STD_150_Sprof();
#endif

	//menuproc_exit();

#ifndef MBX
	if (pixlist.mlh_Head)
	{
		while( n = RemHead( (struct List *)&pixlist ) )
			FreeVec( n );

		pixlist.mlh_Head = NULL;
	}

	while( n = RemHead( (struct List *)&fontcachelist ) )
		FreeVec( n );

	if( LocaleBase )
	{
		CloseCatalog( catalog );
		catalog = NULL;
		CloseLibrary( LocaleBase );
		LocaleBase = NULL;
	}
	CloseLibrary( RexxSysBase );
	CloseLibrary( CxBase );
	CloseLibrary( KeymapBase );
	CloseLibrary( DiskfontBase );
	CloseLibrary( IFFParseBase );
	CloseLibrary( LayersBase );
	CloseLibrary( CyberGfxBase );
	CloseLibrary( CManagerBase );
	CloseLibrary( VATBase );
#else
	CloseModule( AudioBase );

#endif
}
