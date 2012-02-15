/*
** $Id$
*/

#include "mystring.h"
#include "structs.h"
#include "misc.h"
#include "render.h"
#include "textinput_mcc.h"

//#define USEWRITEMASK

extern struct Library *CyberGfxBase;

#ifdef MBX
ULONG getcharkern(struct TextFont *tf, char *chr)
{
	UDWORD kern = 0;
	GetCharMetrics( tf, chr, GCMTAG_KERNING, &kern, TAG_DONE);
	return kern;
}
#endif

static ULONG getcharwidth(struct TextFont *tf, char *chr)
{
#ifdef MBX
	UDWORD width;
	GetCharMetrics( tf, chr, GCMTAG_WIDTH, &width, TAG_DONE);
	return width;
#else
	char ch = *chr;
	if( ch < tf->tf_LoChar || ch > tf->tf_HiChar )
		ch = tf->tf_HiChar + 1;
	ch -= tf->tf_LoChar;
	return (ULONG)( ( WORD * )tf->tf_CharSpace )[ ch ] + ( ( WORD * )tf->tf_CharKern )[ ch ];
#endif
}

static int expandcalcbuf( struct Data *data, int newlen )
{
	if( data->calcbuflen < newlen )
	{
		char *calcbuf;
		if((calcbuf = AllocPooled( data->pool, max( newlen, 512 ) )))
		{
			if( data->calcbuf )
				FreePooled( data->pool, data->calcbuf, data->calcbuflen );
			data->calcbuf = calcbuf;
			data->calcbuflen = max( newlen, 512 );
		}
	}
	return( data->calcbuflen );
}

static int __inline getformat( char *ls, int *format )
{
	if( *ls == '\033' )
	{
		switch( ls[ 1 ] )
		{
			case 'c':
				*format = MUIV_Textinput_Format_Centre;
				return( TRUE );
			case 'r':
				*format = MUIV_Textinput_Format_Right;
				return( TRUE );
			case 'l':
				*format = MUIV_Textinput_Format_Left;
				return( TRUE );
		}
	}
	return( FALSE );
}

static int __inline getcharnegkern( struct Data *data, char *ch )
{
	char *c = *ch && *ch != '\n' ? ch : " ";
	return( min( KERN( c ), 0 ) );
}

int getlinexoffset( Object *obj, struct Data *data, char *p )
{
	int pixels = 0, kern = 0;
	int format = data->format;
	p = getlinestart( data, p );
	kern = getcharnegkern( data, p );
	if( data->styles == MUIV_Textinput_Styles_MUI )
	{
		char *ls = p;
		for( ;; )
		{
			ls = getlinestart( data, ls );
			if( getformat( ls, &format ) )
				break;
			if( ls == data->buffer )
			{
				if( data->preparse )
					getformat( data->preparse, &format );
				break;
			}
			ls--;
		}
	}
	if( format == MUIV_String_Format_Left )
		return( -kern );
	if( *p )
		pixels = textlen( data, p, getlinelenline( p ) );
	if( format == MUIV_String_Format_Right )
		return( data->maxxsize - pixels - kern );
	return( ( data->maxxsize - pixels ) / 2 - kern );
}

int getlineyoffset( Object *obj, struct Data *data, int lineno )
{
	int yoff = data->ycenter + data->lineheight * lineno;
	if( data->smooth )
		yoff -= data->yoffset - data->topline * data->lineheight;
	return( yoff );
}

BOOL isstyle( struct Data *data, char *ch )
{
	if( data->styles == MUIV_Textinput_Styles_MUI )
		return( (BOOL)( *ch == '\33' ) );
	if( data->styles == MUIV_Textinput_Styles_IRC )
		return( (BOOL)( strchr( "\002\003\007\017\026\037", *ch ) ? TRUE : FALSE ) );
	if( data->styles == MUIV_Textinput_Styles_Email )
	{
		return( (BOOL)( strchr( &">_*#"[ data->emailstyles ? 0 : 1 ] , *ch ) ? TRUE : FALSE ) );
	}
	if( data->styles == MUIV_Textinput_Styles_HTML )
		return( (BOOL)( strchr( "<>\"&;", *ch ) ? TRUE : FALSE ) );
	return( FALSE );
}

static BOOL __inline isquietstyle( struct Data *data, char *ch )
{
	if( data->styles == MUIV_Textinput_Styles_Email && data->emailstyles && *ch == '>' )
		return( TRUE );
	if( data->styles == MUIV_Textinput_Styles_HTML )
		return( TRUE );
	return( FALSE );
}

static BOOL __inline isloudstyle( struct Data *data, char *ch )
{
	if( !isstyle( data, ch ) )
		return( FALSE );
	return( (BOOL)!isquietstyle( data, ch ) );
}

static char *styletotoken( struct Data *data, char *ch )
{
	if( data->styles == MUIV_Textinput_Styles_IRC )
	{
		switch( *ch )
		{
			case '\002':
				return( "B" );
			case '\007':
				return( "*" );
			case '\037':
				return( "U" );
			case '\026':
				return( "I" );
			case '\017':
				return( "R" );
			case '\003':
				return( "C" );
		}
	}
	return( ch );
}

int codetostyle( struct Data *data, char code, char *buffer )
{
	if( data->styles == MUIV_Textinput_Styles_IRC )
	{
		int ch = 0;
		switch( code )
		{
			case 0x35:
				ch = '\002';
				break;
			case 0x34:
				ch = '\026';
				break;
			case 0x16:
				ch = '\037';
				break;
			case 0x11:
				ch = '\017';
				break;
			case 0x24:
				ch = '\007';
				break;
			case 0x27:
				ch = '\003';
				break;
		}
		if( ch )
		{
			*buffer = ch;
			return( 1 );
		}
	}
	return( 0 );
}

int parsestyles( Object *obj, struct Data *data, char *start, char *end, struct style *style, ULONG defbgpen, ULONG deffgpen )
{
	int stylechars = 0;
	if( !data->styles )
		return( 0 );
	if( data->styles == MUIV_Textinput_Styles_MUI )
	{
		while( start < end )
		{
			if( *start++ != '\33' || style->disable )
				continue;
			if( *start >= '2' && *start <= '9' )
			{
				style->fgpen = _dri( obj )->dri_Pens[ *start++ - '0' ];
			}
			else switch( *start++ )
			{
				case '0':
					style->fgpen = deffgpen;
					break;
				case '-':
					style->disable = TRUE;
					break;
				case 'u':
					style->styles ^= 2;
					break;
				case 'b':
					style->styles ^= 1;
					break;
				case 'i':
					style->styles ^= 4;
					break;
				case 'n':
					style->styles = 0;
					break;
				case 'c':
					// Centre
				case 'r':
					// Right
				case 'l':
					// Left
				case 'I':
					// Image
					break;

			}
			if( start > end )
				stylechars = start - end;
		}
	}
	else if( data->styles == MUIV_Textinput_Styles_IRC )
	{
		while( start < end )
		{
			switch( *start++ )
			{
				case '\002':
					style->styles ^= 1;
					break;
				case '\037':
					style->styles ^= 2;
					break;
				case '\026':
					style->styles ^= 8;
					break;

				case '\017':	// Styles reset
					style->styles = 0;
					style->bgpen = defbgpen;
					style->fgpen = deffgpen;
					break;

				case '\003':
					style->bgpen = defbgpen;

					if( isdigit( *start ) )
					{
						LONG col = *start++-'0';
						if( isdigit( *start ) )
							col = (10*col)+*start++-'0';

						if( data->do_colors )
							style->fgpen = data->colpens[ col & 15];
						else
							style->fgpen = deffgpen;

						//Printf( "front col %ld\n", col );
						if( *start == ',' )
						{
							start++;
							if( isdigit( *start ) )
							{
								col = *start++-'0';
								if( isdigit( *start ) )
									col = (10*col)+*start++-'0';
								if( data->do_colors )
									style->bgpen = data->colpens[ col & 15 ];
								//Printf( "back col %ld\n", col );
							}
							else
								start--; // don't count comma if no valid bg spec
						}
					}
					else
						style->fgpen = deffgpen;

					if( start > end )
						stylechars = start - end;

					break;
			}
		}
	}
	else if( data->styles == MUIV_Textinput_Styles_Email )
	{
		while( start < end )
		{
			switch( *start++ )
			{
				case '_':
					style->styles ^= 2;
					break;
				case '*':
					style->styles ^= 1;
					break;
				case '#':
					style->styles ^= 16;
					if( style->styles & 16 )
					{
						style->lastbgpen = style->bgpen;
						style->lastfgpen = style->fgpen;
						style->fgpen = data->colpens[ 4 ];
						style->bgpen = data->colpens[ 5 ];
					}
					else
					{
						style->bgpen = style->lastbgpen;
						style->fgpen = style->lastfgpen;
					}
					break;
				case '\n':
					style->fgpen = deffgpen;
					style->bgpen = defbgpen;
					style->styles = 0;
					break;
				case '>':
					if( data->emailstyles )
					{
						char *p;
						int level = 1;
						for( p = start - 1;p >= data->buffer; p-- )
						{
							if( *p == '\n' )
								break;
							if( *p == '>' )
								level++;
							else if( !isspace( *p ) )
							{
								level = 0;
								break;
							}
						}
						if( level )
						{
							style->fgpen = data->colpens[ level % 2 ? 1 : 1 ];
							style->bgpen = data->colpens[ level % 2 ? 10 : 14 ];
						}
						else
						break;
					}
			}
		}
	}
	else if( data->styles == MUIV_Textinput_Styles_HTML )
	{
		while( start < end )
		{
			switch( *start++ )
			{
				case '<':
					if( !style->html_isintag && !style->html_isinquote )
					{
						style->html_isincomment = !strncmp( start, "!--", 3 );
						style->fgpen = data->colpens[ style->html_isincomment ? 12 : 2 ];
						style->html_isintag = TRUE;
						style->usefortoken = TRUE;
					}
					break;
				case '>':
					if( !style->html_isinquote )
					{
						style->usefortoken = FALSE;
						if (style->html_isincomment)
						{
							if (start - 3 >= data->buffer && *(start - 3) == '-' && *(start - 2) == '-')
							{
								style->html_isincomment = style->html_isintag = FALSE;
								style->fgpen = data->colpens[ 2 ];
							}
						}
						else
						{
							style->html_isintag = FALSE;
							style->html_isinquote = FALSE;
							style->fgpen = deffgpen;
						}
						style->usefortoken = FALSE;
					}
					break;
				case '"':
					if( style->html_isintag )
					{
						style->fgpen = data->colpens[ style->html_isinquote ? ( style->html_isincomment ? 12 : 2 ) : 6 ];
						//style->usefortoken = !style->html_isintag;
						style->usefortoken = !style->html_isinquote;
						style->html_isinquote ^= 1;
					}
					break;
				case '&':
					if( !style->html_isintag )
					{
						style->fgpen = data->colpens[ 5 ];
						style->html_isinentity = TRUE;
						style->usefortoken = TRUE;
					}
					break;
				case ';':
					if( !style->html_isintag && style->html_isinentity )
					{
						style->fgpen = deffgpen;
						style->html_isinentity = FALSE;
						style->usefortoken = FALSE;
					}
			}
		}
	}
	return( stylechars );
}

char *stripstyles( struct Data *data, char *text, int *textlen )
{
	int c;
	int len = *textlen;
	if( !data->styles )
		return( text );
	if( expandcalcbuf( data, len ) < len )
		return( text );
	for( c = 0; len > 0; len-- )
	{
		if( !data->noinput )
		{
			data->calcbuf[ c++ ] = *styletotoken( data, text++ );
		}
		else if( !isloudstyle( data, text ) )
		{
			data->calcbuf[ c++ ] = *text++;
		}
		else if( data->styles == MUIV_Textinput_Styles_IRC )
		{
			if( *text != '\003' )
				text++;
			else
			{
				text++;
				if( isdigit( *text ) )
				{
					text++, len--;
					if( isdigit( *text ) )
						text++, len--;

					if( *text == ',' )
					{
						text++, len--;
						if( isdigit( *text ) )
						{
							text++, len--;
							if( isdigit( *text ) )
								text++, len--;
						}
						else
							text--;	// don't count the comma if there's no valid bg color spec
					}
				}
			}
		}
		else if( data->styles == MUIV_Textinput_Styles_MUI )
		{
			text += 2;
			len--;
		}
	}
	*textlen = c;
	return( data->calcbuf );
}

#ifdef MBX // !!!
int textfit( struct TextFont *tf, STRPTR text, ULONG textlen, UWORD pixelsize )
{
/*
	struct RastPort rp;
	struct RastPort fuck[5];
	struct TextExtent ex;
	Forbid();
	InitRastPort(&rp,RPTAG_TEXTFONT,tf,TAG_DONE);
	Permit();
	return(TextFit(&rp,text,textlen,&ex,NULL,1,pixelsize,tf->tf_YSize));
*/
	return(GuessFit(tf,text,textlen,pixelsize,tf->tf_YSize));
};

int mbxpatextlen( struct TextFont *tf, STRPTR text, ULONG textlen )
{
/*
	struct RastPort rp;
	struct RastPort fuck[5];
	Forbid();
	InitRastPort(&rp,RPTAG_TEXTFONT,tf,TAG_DONE);
	Permit();
	return(TextLength(&rp,text,textlen));
*/
	return(TextWidth(tf,text,textlen));
}

#endif

int textlen( struct Data *data, char *text, int len )
{
	if( data->issecret )
	{
		return( (int)patextlen( data->farray->fwarray, SECRETSTR, 1 ) * len );
	}
	text = stripstyles( data, text, &len );
	return( (int)patextlen( data->farray->fwarray, text, len ) );
}

int findcharoffset( struct Data *data, char *text, int textlen, int pixel, int *lineend )
{
	struct TextFont *tf = data->font;
	int isprop = ( tf->tf_Flags & FPF_PROPORTIONAL );
	char *p = text;
	int off = 0;

	if( lineend )
		*lineend = FALSE;

	while( p < text + textlen )
	{
		char *ch = data->issecret? SECRETSTR : p;
		p += CHARLEN(*p);

		if( data->styles && isloudstyle( data, ch ) )
		{
			if( !data->noinput )
			{
				ch = styletotoken( data, ch );
			}
			else
			{
				if( data->styles == MUIV_Textinput_Styles_IRC )
				{
					if( *ch == '\003' )
					{
						off++;
						if( isdigit( *p ) )
						{
							p++, off++;
							if( isdigit( *p ) )
								p++, off++;

							if( *p == ',' )
							{
								p++, off++;
								if( isdigit( *p ) )
								{
									p++, off++;
									if( isdigit( *p ) )
										p++, off++;
								}
								else
									p--, off--;
							}
						}
					}
					else
						off++;
				}
				else
				{
					off+=2;
					p++;
				}
				continue;
			}
		}

		if( isprop )
			pixel -= getcharwidth(tf, ch);
		else
			pixel -= tf->tf_XSize;

		if( pixel < 0 )
		{
			if( off > textlen )
				break;
			return( off );
		}

		off++;
	}

	if( lineend )
		*lineend = TRUE;

	return( min( off, textlen ) );
}

static void __inline setabpendrmd( struct RastPort *rp, ULONG fgpen, ULONG bgpen, ULONG drmd )
{
	#ifndef MBX
	if( GFX39 )
	{
		SetABPenDrMd( rp, fgpen, bgpen, drmd );
	}
	else
	#endif
	{
		SetAPen( rp, fgpen );
		if( drmd & JAM2 )
			SetBPen( rp, bgpen );
		SetDrMd( rp, drmd );
	}
}

void textextent( struct Data *data, struct RastPort *rp, char *str, int len, struct TextExtent *te )
{
	str = stripstyles( data, str, &len );
	TextExtent( rp, str, len, te );
}

void FillBack( struct Data *data, Object *obj, LONG left, LONG top, LONG right, LONG bottom )
{
#ifdef USEWRITEMASK
	if( !data->patternback )
	{
		if( CyberGfxBase || GFX39 )
			SetMaxPen( _rp( obj ), ( 1L << _rp( obj )->BitMap->Depth ) - 1 );
		else
			SetWrMsk( _rp( obj ), 0xff );
	}
#endif
	if( data->patternback )
	{
		ULONG vleft = 0, vtop = 0;
		getvirtoff( obj, &vleft, &vtop );
		setabpendrmd( _rp( obj ), 0, 0, JAM1 );
		DoMethod( obj, MUIM_DrawBackground, left + _mleft( obj ), top + _mtop( obj ), right - left + 1, bottom - top + 1, _mleft( obj ) + left + vleft + data->xoffset, _mtop( obj ) + top + vtop + data->yoffset, 0 );
	}
	else
	{
		SetAPen( _rp( obj ), MUIPEN( data->pens[ data->isactive ? pen_b_active : pen_b_normal ] ) );
		RectFill( _rp( obj ), left + _mleft( obj ), top + _mtop( obj ), right + _mleft( obj ), bottom + _mtop( obj ) );
	}
#ifdef USEWRITEMASK
	if( !data->patternback )
	{
		if( CyberGfxBase || GFX39 )
			SetMaxPen( _rp( obj ), data->maxpen );
		else
			SetWrMsk( _rp( obj ), data->maxpen );
	}
#endif
}

void TextFillBack( struct Data *data, Object *obj, char *str, LONG len, int pen )
{
	struct TextExtent te;
	LONG left, top;
	textextent( data, _rp( obj ), str, len, &te);
/*
	if( _rp( obj )->AlgoStyle & FSF_BOLD && _rp( obj )->Font->tf_BoldSmear )
	{
		te.te_Extent.rect_MaxX = max( te.te_Extent.rect_MaxX - 1, 0 );
	}
*/
	left = _rp( obj )->cp_x + te.te_Extent.rect_MinX;
	top = _rp( obj )->cp_y + te.te_Extent.rect_MinY;
	if( pen == -1 )
	{
		ULONG vleft = 0, vtop = 0;
		getvirtoff( obj, &vleft, &vtop );
		setabpendrmd( _rp( obj ), 0, 0, JAM1 );
		DoMethod( obj, MUIM_DrawBackground, left, top, te.te_Extent.rect_MaxX - te.te_Extent.rect_MinX + 1, te.te_Extent.rect_MaxY - te.te_Extent.rect_MinY + 1, left + vleft + data->xoffset, top + vtop + data->yoffset, 0 );
	}
	else
	{
		SetAPen( _rp( obj ), MUIPEN( pen ) );
		RectFill( _rp( obj ), left, top, _rp( obj )->cp_x + te.te_Extent.rect_MaxX, _rp( obj )->cp_y + te.te_Extent.rect_MaxY );
#ifdef MBX
	SetAlphaChannel(_rp(obj), left, top, _rp( obj )->cp_x + te.te_Extent.rect_MaxX, _rp( obj )->cp_y + te.te_Extent.rect_MaxY, 0xc0);
#endif

	}
}

static void mytext( struct Data *data, Object *obj, char *str, LONG len, ULONG styles, ULONG fgpen, ULONG bgpen )
{
	struct RastPort *rp = _rp( obj );
	ULONG softstyle = 0;
	ULONG drmd = 0;
//	UWORD dr[] = { 0x4444, 0x1111 };
//	WORD cp_x = rp->cp_x, cp_y = rp->cp_y;
	if( styles & 1 )
		softstyle |= FSF_BOLD;
	if( styles & 2 )
		softstyle |= FSF_UNDERLINED;
	if( styles & 4 )
		softstyle |= FSF_ITALIC;
	if( styles & 8 )
		drmd = INVERSVID;
	SetSoftStyle( rp, softstyle, FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC );
	if( bgpen == -1 || (styles & 16 ) || ( ( styles & 1 ) && !( styles & 8 ) ) )
	{
		drmd |= JAM1;
		SetDrMd( rp, drmd );
		TextFillBack( data, obj, str, len, bgpen );
		SetAPen( rp, MUIPEN( fgpen ) );
	}
	else
	{
		drmd |= JAM2;
		setabpendrmd( rp, fgpen, bgpen, drmd );
	}
	Text( rp, str, len );
/*
	// Simple outline
	if( styles & 16 )
	{
		WORD new_cp_x = rp->cp_x;
		WORD xend = new_cp_x - 1;
		WORD y = cp_y - data->font->tf_Baseline;
		SetDrMd( rp, JAM1 );
		Move( rp, cp_x, y );
		Draw( rp, cp_x, y + data->lineheight - 1 );
		Draw( rp, xend, y + data->lineheight - 1 );
		Draw( rp, xend, y );
		Draw( rp, cp_x, y );
		Move( rp, new_cp_x, cp_y );
	}
*/
}

#define STYLETEXT( p, len ) mytext( data, obj, p, len, 0, stylefgpen, stylebgpen )
#define NORMALTEXT( p, len, style ) mytext( data, obj, p, len, (style)->styles, cursormode ? deffgpen : (style)->fgpen, cursormode ? defbgpen : (style)->bgpen )

void text( struct Data *data, Object *obj, char *str, LONG len, int penmode )
{
	struct RastPort *rp = _rp( obj );
	WORD cpx = rp->cp_x, cpy = rp->cp_y;
	int markmode = penmode == PENMODE_MARK;
	int cursormode = penmode == PENMODE_CURSOR;
	struct style style;
	ULONG deffgpen, defbgpen, stylefgpen, stylebgpen;
	if( cursormode )
	{
		deffgpen = data->pens[ pen_f_cursor ];
		defbgpen = data->pens[ pen_b_cursor ];
		stylefgpen = deffgpen;
		stylebgpen = defbgpen;
	}
	else if( markmode && !data->do_colors )
	{
		deffgpen = data->pens[ pen_f_mark ];
		defbgpen = data->pens[ pen_b_mark ];
		stylefgpen = data->pens[ pen_b_style ];
		stylebgpen = data->pens[ pen_f_style ];
	}
	else
	{
		deffgpen = data->isactive ? data->pens[ pen_f_active ] : data->pens[ pen_f_normal ];
		if( data->patternback && !markmode )
		{
			defbgpen = -1;
			deffgpen = data->pens[ pen_f_pattern ];
		}
		else
			defbgpen = data->isactive ? data->pens[ pen_b_active ] : data->pens[ pen_b_normal ];
		stylefgpen = data->pens[ pen_f_style ];
		stylebgpen = data->pens[ pen_b_style ];
	}
	memset( &style, 0, sizeof( style ) );
	style.fgpen = deffgpen;
	style.bgpen = defbgpen;
	if( data->styles )
	{
		char *p = str, *p2;
		int stylechars = 0;
		if( data->preparse && data->noinput )
		{
			parsestyles( obj, data, data->preparse, strchr( data->preparse, 0 ), &style, defbgpen, deffgpen );
		}
		if( str > data->buffer && str < (char *)strchr( data->buffer, NULL ) )
		{
			char *p2 = data->buffer;
			stylechars = min( parsestyles( obj, data, p2, str, &style, defbgpen, deffgpen ), len );
		}
		if( stylechars )
		{
			if( !data->noinput )
				STYLETEXT( p, stylechars );
			p += stylechars;
		}
		while( *p && p <= str + len )
		{
			struct style oldstyle;
			int quietstyle;
			p2 = p;
			while( *p2 && p2 < str + len && !isstyle( data, p2 ) )
				p2++;
			if( p2 > p )
			{
				NORMALTEXT( p, p2 - p, &style );
			}
			if( !*p2 || p2 == str + len )
				break;
			p = p2;
			quietstyle = isquietstyle( data, p );
			memcpy( &oldstyle, &style, sizeof( struct style ) );
			p2 += 1 + parsestyles( obj, data, p2, p2 + 1, &style, defbgpen, deffgpen );
			if( !data->noinput || quietstyle )
			{
				char *ch = styletotoken( data, p++ );
				if( quietstyle )
					NORMALTEXT( ch, 1, style.usefortoken ? &style : &oldstyle );
				else
					STYLETEXT( ch, 1 );
			}
			if( !data->noinput )
			{
				if( p2 > str + len )
					p2 = str + len;
				if( p2 > p )
				{
					if( quietstyle )
						NORMALTEXT( p, p2 - p, &style );
					else
						STYLETEXT( p, p2 - p );
				}
			}
			p = p2;
		}
	}
	else
	{
		NORMALTEXT( str, len, &style );
	}
	if( data->do_colors && markmode )
	{
		struct TextExtent te;
		int xs, xe;
		textextent( data, rp, str, len, &te );
		xs = cpx + te.te_Extent.rect_MinX;
		xe = cpx + te.te_Extent.rect_MaxX + 1;
		ClipBlit( rp , xs, cpy - data->font->tf_Baseline, rp, xs, cpy - data->font->tf_Baseline, xe - xs, data->lineheight, 0x50 );
	}
}

void smarttext( struct Data *data, Object *obj, char *str, LONG len )
{
	if( !len )
		return;
	if( data->markmode > 1 )
	{
		if( data->markstart >= str && data->markend <= str + len - 1 )
		{
			// partially marked
			text( data, obj, str, data->markstart - str, PENMODE_NORMAL );
			text( data, obj, data->markstart, data->markend - data->markstart + 1, PENMODE_MARK );
			text( data, obj, data->markend + 1, len + str - data->markend - 1, PENMODE_NORMAL );
		}
		else if( data->markstart > str && data->markstart <=/*-=*/ ( str + len - 1 ) )
		{
			text( data, obj, str, data->markstart - str, PENMODE_NORMAL );
			text( data, obj, data->markstart, len - ( data->markstart - str ), PENMODE_MARK );
		}
		else if( data->markend >= str && data->markend < ( str + len - 1 ) )
		{
			text( data, obj, str, data->markend - str + 1, PENMODE_MARK );
			text( data, obj, data->markend + 1, len - ( data->markend - str ) - 1, PENMODE_NORMAL );
		}
		else if( data->markstart <= str && data->markend >= str + len - 1 )
		{
			// completely marked
			text( data, obj, str, len, PENMODE_MARK );
		}
		else
		{
			// completely unmarked
			text( data, obj, str, len, PENMODE_NORMAL );
		}
	}
	else
	{
		// completely unmarked
		if( data->issecret )
		{
			char bf[ 256 ];

			len = min( len, sizeof( bf ) );
			memset( bf, SECRETCHAR, len );
			text( data, obj, bf, len, PENMODE_NORMAL );
		}
		else
		{
			text( data, obj, str, len, PENMODE_NORMAL );
		}
	}
}

int underlinesegment( struct Data *data, Object *obj, struct segment *seg, char *line, int lineno, ULONG pen )
{
	int currpos = line - data->buffer, currendpos = currpos + getlinelenline( line );
	struct RastPort *rp = _rp( obj );
	if( lineno >= data->topline && lineno < data->topline + data->visiblelines && ( ( seg->xstart >= currpos && seg->xstart <= currendpos ) || ( seg->xstart + seg->xsize >= currpos && seg->xstart + seg->xsize <= currendpos ) ) )
	{
		char *ustr = data->buffer + max( currpos, seg->xstart );
		int ulen = min( currendpos - currpos, seg->xsize );
		int bl = _mtop( obj ) + YSPACING + data->ycenter + data->lineheight * ( lineno - data->topline ) + data->font->tf_YSize - 1;
		int xp = _mleft( obj ) + XSPACING - data->xoffset + getlinexoffset( obj, data, line ) + textlen( data, line, ustr - line );
		SetDrMd( rp, JAM1 );
		rp->LinePtrn = 0xcccc;
		SetAPen( rp, MUIPEN( pen ) );
		Move( rp, xp, bl );
		Draw( rp, xp + textlen( data, ustr, ulen ), bl );
		rp->LinePtrn = 0xffff;
		return( TRUE );
	}
	return( FALSE );
}

static int underlinemisspells( struct Data *data, Object *obj, int lineno )
{
	char *line = getline( data, lineno );
	struct misspell *ms, *lastms = NULL, *nextms;
	int gotone = FALSE;
	for( ms = data->misspells; ms; ms = nextms )
	{
		char *ws;
		int len;
		ws = ms->word.xstart + data->buffer;
		len = ms->word.xsize;
		nextms = ms->next;
		if( ws == getwordstart( data, ws ) && getwordend( data, ws ) - ws + 1 == len && ws >= data->buffer && ws + len <= (char *)strchr( data->buffer, 0 ) && !strncmp( ws, ms->misspell, len ) )
		{
			if( underlinesegment( data, obj, &ms->word, line, lineno, data->pens[ pen_u_misspell ] ) )
				gotone = TRUE;
			lastms = ms;
		}
		else
		{
			if( lastms )
				lastms->next = ms->next;
			else
				data->misspells = ms->next;
			FreePooled( data->pool, ms, sizeof( *ms ) + strlen( ms->misspell ) + 1 );
		}
	}
	return( gotone );
}

static int underlineurls( struct Data *data, Object *obj, int lineno )
{
	char *line = getline( data, lineno );
	struct viewurl *vu;
	int gotone = FALSE;
	for( vu = data->urls; vu; vu = vu->next )
	{
		if( underlinesegment( data, obj, &vu->word, line, lineno, vu->selected ? data->pens[ pen_s_url ] : data->pens[ pen_u_url ] ) )
			gotone = TRUE;
	}
	return( gotone );
}

void drawpart( struct RastPort *rp, APTR obj, struct Data *data, char *start, char *end )
{
	int x, y, llen, offset = 0;
	char *line;
	findxyp( data, start, &x, &y, &line );
	if( data->quickrender )
	{
		if( line < start )
			offset = textlen( data, line, x );
	}
	else
		start = getlinestart( data, start );
	while( start <= end )
	{
		llen = getlinelenline( start );
		if( data->quickrender )
			llen = min( llen, end - start + 1 );
		if( llen > 0 && y >= data->topline && y < data->topline + data->visiblelines )
		{
			Move( rp, _mleft( obj ) + XSPACING - data->xoffset + getlinexoffset( obj, data, start ) + offset, _mtop( obj ) + YSPACING + data->font->tf_Baseline + getlineyoffset( obj, data, y - data->topline ) );
			smarttext( data, obj, start, llen );
			if( PARSEURLS )
				underlineurls( data, obj, y );
			if( data->spellcheck )
				underlinemisspells( data, obj, y );
		}
		offset = 0;
		start += llen + 1;
		y++;
	}
}

void drawline( struct RastPort *rp, APTR obj, struct Data *data, char *line, int lineno )
{
	int llen;
	int linexoffset = getlinexoffset( obj, data, line ), lineyoffset = getlineyoffset( obj, data, lineno );

	for( llen = 0; line[ llen ] && line[ llen ] != '\n'; llen++ );

	Move( rp, _mleft( obj ) + XSPACING - data->xoffset + linexoffset, _mtop( obj ) + YSPACING + data->font->tf_Baseline + lineyoffset );

	smarttext( data, obj, line, llen );

	if( linexoffset + getcharnegkern( data, line ) > data->xoffset )
	{
		FillBack( data, obj, XSPACING, YSPACING + data->ycenter + lineno * data->lineheight, linexoffset + getcharnegkern( data, line ) - data->xoffset + 1, YSPACING + lineyoffset + data->lineheight - 1 );
	}
	if( max( rp->cp_x, _mleft( obj ) + XSPACING ) < _mright( obj ) )
		FillBack( data, obj, max( rp->cp_x - _mleft( obj ), XSPACING ), YSPACING + lineyoffset, _mwidth( obj ) - 1, YSPACING + lineyoffset + data->lineheight - 1 );
	if( PARSEURLS )
		underlineurls( data, obj, lineno + data->topline );
	if( data->spellcheck )
		underlinemisspells( data, obj, lineno + data->topline );
}

void drawlinefromcursor( struct RastPort *rp, APTR obj, struct Data *data, char *line, int lineno, int cleareol )
{
	int llen = getlinelenline( line );

	Move( rp, _mleft( obj ) + XSPACING - data->xoffset + getlinexoffset( obj, data, line ) + data->oldpxo, _mtop( obj ) + YSPACING + data->font->tf_Baseline + getlineyoffset( obj, data, lineno ) );
	if( llen )
	{
		smarttext( data, obj, line, llen );
	}
	if( cleareol && max( rp->cp_x, _mleft( obj ) + XSPACING ) < _mright( obj ) )
		FillBack( data, obj, max( rp->cp_x - _mleft( obj ), XSPACING ), YSPACING + getlineyoffset( obj, data, lineno ), _mwidth( obj ) - 1, YSPACING + getlineyoffset( obj, data, lineno ) + data->lineheight - 1 );
	if( PARSEURLS )
		underlineurls( data, obj, lineno + data->topline );
	if( data->spellcheck )
		underlinemisspells( data, obj, lineno + data->topline );
}

void quickaddclip( APTR obj, ULONG x, ULONG y, ULONG w, ULONG h, struct clipbuff *bh )
{
#ifndef MBX
	struct Layer *l = _rp( obj )->Layer;

	if( !l || !_window( obj ) )
		goto give_up;

	LockLayer( 0, l );
	if( l->ClipRect && !l->ClipRect->lobs )
	{
		LONG sx = x + _window( obj )->LeftEdge, sy = y + _window( obj )->TopEdge;
		LONG mx = sx + w - 1, my = sy + h - 1;
		struct Rectangle *r = &l->ClipRect->bounds;

		//Printf( "%ld/%ld %ld/%ld %ld/%ld %ld/%ld\n", r->rect_MinX, sx, r->rect_MaxX, mx, r->rect_MinY, sy, r->rect_MaxY, my );*/

		if( r->rect_MinX <= sx && r->rect_MaxX >= mx && r->rect_MinY <= sy && r->rect_MaxY >= my )
		{
			bh->oldrect = *r;
			bh->oldnext = l->ClipRect->Next;

			l->ClipRect->Next = NULL;
			r->rect_MinX = sx;
			r->rect_MaxX = mx;
			r->rect_MinY = sy;
			r->rect_MaxY = my;

			bh->use = TRUE;

			//PutStr( "quick!\n" );

			return;
		}
	}
	UnlockLayer( l );

give_up:
#endif
	bh->use = FALSE;
	bh->mcliphandle = MUI_AddClipping( muiRenderInfo( obj ), x, y, w, h );
}

void quickremclip( APTR obj, struct clipbuff *bh )
{
#ifndef MBX
	if( bh->use )
	{
		struct Layer *l = _rp( obj )->Layer;
		l->ClipRect->bounds = bh->oldrect;
		l->ClipRect->Next = bh->oldnext;
		UnlockLayer( l );
	}
	else
#endif
	{
		MUI_RemoveClipping( muiRenderInfo( obj ), bh->mcliphandle );
	}
}

void drawcursor( struct RastPort *rp, APTR obj, struct Data *data, BOOL kill )
{
	int pxo, cellsize, cursory;
	char *cursorp;
	int xs, ys, linexoffset;
	struct clipbuff cb;
	int cs;
	BOOL realchar;
	BOOL marked;

	if( kill )
	{
		pxo = data->oldpxo;
		cellsize = data->oldcellsize;
		cursory = data->oldcursory;
		cursorp = data->oldcursorp;
	}
	else
	{
		pxo = data->pxo;
		cellsize = data->cellsize;
		cursory = data->cursory;
		cursorp = data->cursorp;
	}
	realchar = *cursorp && *cursorp != '\n';
	marked = data->markstart <= cursorp && data->markend >= cursorp;

	if( data->noinput )
	{
		if( realchar )
			return;
		data->cursorstate = TRUE;
	}

/*
	if( kill && data->cursorstate && realchar )
		return; // cursor is already in killed state, no need to render
*/

	if( cursory < data->topline || cursory >= ( data->topline + data->visiblelines ) )
		return; // cursor not inside a visible line

	linexoffset = getlinexoffset( obj, data, cursorp ) + pxo - data->xoffset;
	if( linexoffset < 0 || linexoffset > _mwidth( obj ) - cellsize - XSPACING * 2 )
		return;

	//findcursorpos( rp, obj, data, &pxo, &cellsize );

	xs = _mleft( obj ) + XSPACING + linexoffset;
	ys = _mtop ( obj ) + YSPACING + getlineyoffset( obj, data, cursory - data->topline );

	if( data->cursorstate || kill )
	{
		if( marked && !( kill && !realchar ) )
			cs = PENMODE_MARK;
		else
			cs = PENMODE_NORMAL;
	}
	else
		cs = PENMODE_CURSOR;

	Move( rp, xs, ys + data->font->tf_Baseline );

	if( !data->cursorstyle )
		quickaddclip( obj, xs, ys, cellsize, data->lineheight, &cb );
	else if( data->cursorstyle == 1 )
		quickaddclip( obj, xs, ys, min( data->cursorsize, cellsize ), data->lineheight, &cb );
	else
		quickaddclip( obj, xs, ys + data->lineheight - min( data->cursorsize, data->lineheight ), cellsize, min( data->cursorsize, data->lineheight ), &cb );

	if( data->issecret && realchar )
		text( data, obj, SECRETSTR, CHARLEN(*SECRETSTR), cs );
	else if( realchar )
		text( data, obj, realchar ? cursorp : " ", CHARLEN(realchar ? *cursorp : ' '), cs);
	else
	{
		int bg;
		// Quick cursor rendition

		if( cs == PENMODE_CURSOR )
			bg = data->pens[ pen_b_cursor ];
		else
			bg = data->isactive ? data->pens[ pen_b_active ] : data->pens[ pen_b_normal ];

		setabpendrmd( _rp( obj ), bg, 0, JAM1 );
		RectFill( _rp( obj ), xs, ys, xs + cellsize - 1, ys + data->lineheight - 1 );
	}

	if( PARSEURLS )
		underlineurls( data, obj, cursory );
	if( data->spellcheck )
		underlinemisspells( data, obj, cursory );

	quickremclip( obj, &cb );

}
