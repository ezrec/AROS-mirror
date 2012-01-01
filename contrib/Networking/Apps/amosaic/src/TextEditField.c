#include "includes.h"
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/dos.h>
#include <proto/muimaster.h>

#include <graphics/gfxmacros.h>

#include <exec/memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TextEditField.h"
// #include "demo.h"
static const char VersionTag[] = "\0$VER: TextEditField.c 1.0 (12.06.95)";

#define MUIA_Prop_Delta                 0x8042c5f4 /* is. LONG              */
#ifndef MUIA_Prop_DeltaFactor
#define MUIA_Prop_DeltaFactor           0x80427c5e /* isg LONG              */
#endif

#define PreAlloc 256 /* Preallocate 256 bytes */

extern struct Library *MUIMasterBase;
//struct MsgPort *FileNotifyPort;
extern struct MsgPort *FileNotifyPort;

struct EditFuncMsg
{
	struct IClass *cl;
	Object *obj;
};

void fail(APTR,APTR);
#ifndef __AROS__
long kprintf(char *,...);
#endif

HOOKPROTO(EditFunc, IPTR, APTR object, struct EditFuncMsg *msg);

#define IEQUALIFIER_SHIFT (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)
#define IEQUALIFIER_ALT (IEQUALIFIER_LALT | IEQUALIFIER_RALT)

#define HIKEY_SHIFT	(0x100)
#define HIKEY_ALT	(0x200)
#define HIKEY_CTRL	(0x400)

MakeStaticHook(EditHook, EditFunc);

/*
 * Class data
 */

static Class *TextFieldCl;

struct TextEditFieldClData
{
	Object *TextField;
	Object *VBar;
	Object *HBar;
	Object *Space;
};

struct TextFieldClData
{
	Object *VBar;
	Object *HBar;

	LONG x_offset;
	LONG y_offset;
#if 0
	LONG c_width;		/* In chars */
	LONG c_height;		/* In chars */
#endif
	LONG width;			/* In pixels */
	LONG height;		/* In pixels */
	LONG lines;			/* # of lines in the text */
	LONG longest_line;
	LONG text_width;	/* Max chars/line *tf_XSize */
	LONG text_height;	/* Lines *tf_YSize */
	char *text;			/* The actual text */
	char **linearray; 	/* For fast search in the text */
	char *end_text;  	/* The last char +1 in the text */
	LONG cursor_x;		/* Chars offset from left text left */
	LONG cursor_y;		/* Chars offset from top text top */
	LONG s_cursor_x;	/* Offset from left border*/
	LONG s_cursor_y;	/* Offset from top border */
	LONG active;
	UWORD font_width,font_height; /* For fast access */
	struct NotifyRequest FileNotify;
	LONG notifying;
	LONG alloced_size;	/* Allocated space for text */
	LONG text_size;		/* The size of the text */
	LONG go_active;		/* Signal the gadget to go active 1= inactivate, 2= activate */
	LONG visible_lines;
	LONG visible_columns;
	WORD p_l,p_r,p_t,p_b; /* The parrent group size */
	Object *parent;
	LONG icr_left,icr_top,icr_width,icr_height;
	LONG icr_ncx,icr_ncy;
	LONG FileNo;
	char *FileName;
};

/*
 * Misc routines
 */

static LONG LineLength(char *str)
{
	char *l=str;
	for(;*str!='\n' && *str!=0;str++);
	return (LONG)(str-l);
}

static LONG LongestLine(struct TextFieldClData *inst)
{
	char **lines;
	LONG len,longlen=0;

	for(lines=inst->linearray;*lines;lines++){
		if((len=LineLength(*lines))>longlen)
			longlen=len;
		}
	return longlen;
}

static ULONG CountLines(char *str)
{
	ULONG i=1;
	for(;*str;str++)
		if(*str=='\n')
			i++;
	return i;
}

static void ParseText(struct TextFieldClData *inst)
{
	char **line,*text;
	BOOL newline;
	LONG longline;

	inst->lines=CountLines(inst->text);
	if(inst->linearray) FreeVec(inst->linearray);
	inst->linearray=(char **)AllocVec((inst->lines+25)<<2,MEMF_CLEAR);
	inst->longest_line=0;
	longline=0;
	line=inst->linearray;
	newline=TRUE;
	for(text=inst->text;*text;text++){
		if(newline){
			newline=FALSE;
			*line++=text;
			if(inst->longest_line<longline) inst->longest_line=longline;
			longline=0;
			}
		if(*text=='\n') newline=TRUE;
		longline++;
		}
	if(newline){
		*line++=text;
		if(inst->longest_line<longline) inst->longest_line=longline;
		}
	inst->end_text=text;
	*line=NULL;
}

/*
 * Cursor handling
 */

static void ShowC (struct RastPort *rp,long col,long x,long y,BOOL active,long width,long height)
{
	static UWORD mask1[] = {0x5555,0xAAAA};
	static UWORD mask2[] = {0xAAAA,0x5555}; /* Depending on the y pos */
	BYTE DrMode;
	DrMode=rp->DrawMode;
	SetABPenDrMd(rp,col,0,COMPLEMENT);
	if(!active){
		if((x ^ y)&1)
			{SetAfPt(rp,(UWORD *)&mask1,1);}
		else
			{SetAfPt(rp,(UWORD *)&mask2,1);}
		}
	RectFill(rp,x,y,x+width-1,y+height-1);
	SetDrMd(rp,DrMode);
	if(!active) SetAfPt(rp,NULL,0);
}

static void ClipCursor(struct TextFieldClData *inst,Object *obj)
{
	if(inst->icr_ncx*inst->font_width<inst->x_offset){ /* Go left */

		inst->icr_left=_mleft(obj);
		inst->icr_top=_mtop(obj);
		inst->icr_width=_mwidth(obj);
		inst->icr_height=_mheight(obj);

		inst->x_offset = inst->icr_ncx*inst->font_width;
		nnset(inst->HBar,MUIA_Prop_First,inst->x_offset);
		}
	if(inst->icr_ncy*inst->font_height<inst->y_offset){ /* Go up */

		inst->icr_left=_mleft(obj);
		inst->icr_top=_mtop(obj);
		inst->icr_width=_mwidth(obj);
		inst->icr_height=_mheight(obj);

		inst->y_offset = inst->icr_ncy*inst->font_height;
		nnset(inst->VBar,MUIA_Prop_First,inst->y_offset);
		}
	if(((inst->icr_ncx+1)*inst->font_width)>inst->x_offset+_mwidth(obj)){
		/* Go right */
		inst->icr_left=_mleft(obj);
		inst->icr_top=_mtop(obj);
		inst->icr_width=_mwidth(obj);
		inst->icr_height=_mheight(obj);

		inst->x_offset = ((inst->icr_ncx+1)*inst->font_width)-_mwidth(obj);
		nnset(inst->HBar,MUIA_Prop_First,inst->x_offset);
		}
	if(((inst->icr_ncy+1)*inst->font_height)>inst->y_offset+_mheight(obj)){
		/* Go down */

		inst->icr_left=_mleft(obj);
		inst->icr_top=_mtop(obj);
		inst->icr_width=_mwidth(obj);
		inst->icr_height=_mheight(obj);

		inst->y_offset = ((inst->icr_ncy+1)*inst->font_height)-_mheight(obj);
		nnset(inst->VBar,MUIA_Prop_First,inst->y_offset);
		}	
}

static void ClipCursor2(struct TextFieldClData *inst,Object *obj)
{
	inst->icr_ncx=inst->cursor_x;
	inst->icr_ncy=inst->cursor_y;

	if(inst->cursor_x*inst->font_width<inst->x_offset)
		inst->icr_ncx=(inst->x_offset+inst->font_width-1)/inst->font_width;
	if(inst->cursor_y*inst->font_height<inst->y_offset)
		inst->icr_ncy=(inst->y_offset+inst->font_height-1)/inst->font_height;
	if(((inst->cursor_x+1)*inst->font_width)>inst->x_offset+_mwidth(obj))
		inst->icr_ncx=(inst->x_offset+_mwidth(obj))/inst->font_width-1;
	if(((inst->cursor_y+1)*inst->font_height)>inst->y_offset+_mheight(obj))
		inst->icr_ncy=(inst->y_offset+_mheight(obj))/inst->font_height-1;
}
static void ShowCursor (struct TextFieldClData *inst,
						Object *obj)
{
	ShowC(_rp(obj),_dri(obj)->dri_Pens[HIGHLIGHTTEXTPEN],
			_mleft(obj)+inst->s_cursor_x,_mtop(obj)+inst->s_cursor_y,inst->active&1,inst->font_width,inst->font_height);
}

static void HideCursor (struct TextFieldClData *inst,
						Object *obj)
{
	ShowC(_rp(obj),_dri(obj)->dri_Pens[HIGHLIGHTTEXTPEN],
			_mleft(obj)+inst->s_cursor_x,_mtop(obj)+inst->s_cursor_y,inst->active&1,inst->font_width,inst->font_height);
}

/*
 * Refresh routines
 */

static void RefreshTextArea(struct Window *window,
							struct TextFieldClData *inst,
							Object *obj,
                            long offs_x,long offs_y,
                            long x,long y,
                            long width,long height)
{
	LONG line,charpos,dx,dy,nchars,nwidth,ypos,nlines,nheight,rnchars;
	LONG endcolumn;
	char *linetext,*rtext;
	BYTE DrMode;
	APTR cliphandle;

	DrMode=_rp(obj)->DrawMode;
	SetFont(_rp(obj),_font(obj));

	cliphandle=MUI_AddClipping(muiRenderInfo(obj),x,y,width,height);
	endcolumn=x+width-1;

	line	= offs_y/inst->font_height;
//	kprintf("Line: %ld\n",line);
	if(line>inst->lines) line=inst->lines;
	dy		= offs_y % inst->font_height;
	ypos	= y-dy;
	nlines	= 1;
	if(dy) nlines++;
	nheight	= height-dy;
	if(nheight>0)
		nlines+=nheight/inst->font_height;
	if(nheight % inst->font_height)
		nlines++;

	SetABPenDrMd(_rp(obj),1,0,JAM2);
/*	kprintf("Lines %ld\n",nlines); */

	while(nlines--){
		charpos	= offs_x/inst->font_width;
		dx		= offs_x % inst->font_width;
		linetext=inst->linearray[line];
		if(linetext){
//			kprintf("Linetext %08lx\n",linetext);
			for(;*linetext!='\n' && *linetext!='\000' && charpos;linetext++,charpos--);
			nchars=1;
			if(dx) nchars++;
			nwidth=width-dx;
			if(nwidth>0)
				nchars+=nwidth/inst->font_width;
			if(nwidth % inst->font_width)
				nchars++;

			for(rnchars=0,rtext=linetext;nchars-- && *rtext!='\000' && *rtext!='\n';rnchars++,rtext++);

			Move(_rp(obj),x-dx,ypos+_font(obj)->tf_Baseline);
			Text(_rp(obj),linetext,rnchars);
			if(_rp(obj)->cp_x<endcolumn){
				SetAPen(_rp(obj),0);
				RectFill(_rp(obj),_rp(obj)->cp_x,ypos,endcolumn,_rp(obj)->cp_y+_font(obj)->tf_YSize-1);
				SetAPen(_rp(obj),1);
				}
			ypos+=_font(obj)->tf_YSize;
			line++;
			}
		}
	SetDrMd(_rp(obj),DrMode);
	MUI_RemoveClipping(muiRenderInfo(obj),cliphandle);
}

static UBYTE chartable[256]={ /* To filter 'illegal' chars */
0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0A,0x20,0x20,0x0A,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x20,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF};

static void SetText(struct TextFieldClData *inst,char *text)
{
	LONG length=strlen(text);
	APTR oldmem=inst->text;
	char *p1,*p2;

//	kprintf("(Re)allocating\n");

	inst->text=AllocVec(length+PreAlloc,0L);
	for(p1=text,p2=inst->text;*p1;*p2++=chartable[(UBYTE)*p1++]);
	*p2=0;
	inst->text_size=length;
	inst->alloced_size=length+PreAlloc;
	if(oldmem) FreeVec(oldmem);
}

//static long cnt;

/* Return true if redraw is needed */

BOOL InsertChar(Object *obj,struct TextFieldClData *inst,long ascii)
{
	char *textpos,*currentpos,**lines=inst->linearray,*tpos;
	long cursor_x,cursor_y;

	LONG i;

//	kprintf("Getting char %c at %ld\n",(char)ascii,cnt);
//	return;

	cursor_x=inst->cursor_x;
	cursor_y=inst->cursor_y;

	if((i=LineLength(lines[inst->cursor_y]))<inst->cursor_x){
		inst->icr_ncx=i;
		inst->icr_ncy=inst->cursor_y;
		cursor_x=i;
		} /* After this cursor_x may have changed */

	textpos=(char *)(lines[cursor_y]+cursor_x);

//	kprintf("Keycode: %ld\n",ascii);

	if((ascii>31) && (ascii!=127)){ /* Normal chars */
		for(currentpos=inst->end_text,tpos=inst->end_text+1;currentpos>=textpos;*tpos--=*currentpos--);
		for(i=cursor_y+1;lines[i];i++)
			lines[i]++;
		*++currentpos=ascii;
		inst->end_text++;

		for(;*currentpos!=0 && *currentpos!='\n';currentpos++);
		currentpos=(char *)((long)currentpos-(long)lines[cursor_y]+1); /* = line length */
		if((long)currentpos>inst->longest_line){
			inst->longest_line=(long)currentpos;
			set(inst->HBar,MUIA_Prop_Entries,inst->longest_line*inst->font_width);
			}

		inst->icr_ncx=cursor_x+1;

		inst->text_size++;

		if((inst->text_size+10)>inst->alloced_size){ /* Exceeding buffer */
			SetText(inst,inst->text);
			ParseText(inst);
			}

//		RefreshTextArea(_window(obj),inst,obj,inst->x_offset,inst->font_height*cursor_y,_mleft(obj),_mtop(obj)+inst->font_height*cursor_y-inst->y_offset,_mwidth(obj),inst->font_height);
		inst->icr_left=_mleft(obj);
		inst->icr_top=_mtop(obj)+inst->font_height*cursor_y-inst->y_offset;
		inst->icr_width=_mwidth(obj);
		inst->icr_height=inst->font_height;
		return TRUE;
		}
	else{ /* Ctrl chars (return del etc) */
		if(ascii==13){ /* Return */
			LONG len;
			for(currentpos=inst->end_text,tpos=inst->end_text+1;currentpos>=textpos;*tpos--=*currentpos--);

			*++currentpos='\n';

			inst->icr_ncy=cursor_y+1;
			inst->icr_ncx=0;
			inst->lines++;

			inst->text_size++;

			len=inst->longest_line;
			if((inst->text_size+10)>inst->alloced_size){ /* Exceeding buffer */
				SetText(inst,inst->text);
				ParseText(inst);
				}
			else
				ParseText(inst);

			if(len>inst->longest_line)
				set(inst->HBar,MUIA_Prop_Entries,inst->longest_line*inst->font_width);
			set(inst->VBar,MUIA_Prop_Entries,inst->lines*inst->font_height);

			inst->icr_left=_mleft(obj);
			inst->icr_top=_mtop(obj);
			inst->icr_width=_mwidth(obj);
			inst->icr_height=_mheight(obj);
			return TRUE;
			}
		else if(ascii==8){ /* Backspace */
			if(cursor_x>0){
				inst->icr_ncx=cursor_x-1;
				textpos--;
				ascii=127;
				}
			else if(cursor_y>0){
				textpos--;
				ascii=127;
				inst->icr_ncx=(LONG)(inst->linearray[cursor_y]-inst->linearray[cursor_y-1])-1;
				inst->icr_ncy=cursor_y-1;
				}
			}
		if(ascii==127){ /* Delete  (or a modified Backspace) */ 
			LONG len;
			if(*textpos!='\n'){ /* If _not_ deleting a \n */
				for(currentpos=textpos,tpos=currentpos+1;currentpos<inst->end_text;*currentpos++=*tpos++);
				for(i=cursor_y+1;lines[i];i++)
					lines[i]--;
				len=LineLength(inst->linearray[cursor_y])+1;

				if((len+1)==inst->longest_line){
					len=LongestLine(inst)+1;
					if(len<inst->longest_line){
						inst->longest_line=len;
						set(inst->HBar,MUIA_Prop_Entries,inst->longest_line*inst->font_width);
						}
					}

				inst->icr_left=_mleft(obj);
				inst->icr_top=_mtop(obj)+inst->font_height*cursor_y-inst->y_offset;
				inst->icr_width=_mwidth(obj);
				inst->icr_height=inst->font_height;

				return TRUE;
				}
			else{ /* If deleting a \n */
				for(currentpos=textpos,tpos=currentpos+1;currentpos<inst->end_text;*currentpos++=*tpos++);
				ParseText(inst);
				set(inst->VBar,MUIA_Prop_Entries,inst->lines*inst->font_height);
				inst->icr_left=_mleft(obj);
				inst->icr_top=_mtop(obj);
				inst->icr_width=_mwidth(obj);
				inst->icr_height=_mheight(obj);
				return TRUE;
				}
			}
		}
	return FALSE;
}

BOOL HandleSpecialKeys(struct TextFieldClData *inst,long key)
{
#if 0
					case 0x3E: /* Keypad 8 (up) */
					case 0x1E: /* Keypad 2 (down) */
					case 0x2D: /* Keypad 4 (left) */
					case 0x2F: /* Keypad 6 (right) */
					case 0x1F: /* Keypad 3 (PgDn) */
					case (0x1E | HIKEY_SHIFT): /* Shift Keypad 2 (down) */
					case (CURSORDOWN | HIKEY_SHIFT):
					case 0x40: /* Space */
					case 0x3F: /* Keypad 9 (PgUp) */
					case (0x3E | HIKEY_SHIFT): /* Shift Keypad 8 (up) */
					case (CURSORUP | HIKEY_SHIFT):
					case 0x41: /* Backspace */
					case (CURSORLEFT | HIKEY_SHIFT):
					case (0x2D | HIKEY_SHIFT): /* Shift Keypad 4 (left) */
					case (0x2F | HIKEY_SHIFT): /* Shift Keypad 6 (right) */
					case (CURSORRIGHT | HIKEY_SHIFT):
					case 0x3D: /* Keypad 7 (Home) */
					case (0x3E | HIKEY_CTRL): /* Ctrl Keypad 8 (up) */
					case (CURSORUP | HIKEY_CTRL):
					case 0x1D: /* Keypad 1 (End) */
					case (0x1E | HIKEY_CTRL): /* Ctrl Keypad 2 (down) */
					case (CURSORDOWN | HIKEY_CTRL):
					case (0x2D | HIKEY_CTRL): /* Ctrl Keypad 4 (left) */
					case (CURSORLEFT | HIKEY_CTRL):
					case (0x2F | HIKEY_CTRL): /* Ctrl Keypad 6 (right) */
					case (CURSORRIGHT | HIKEY_CTRL):
#endif

	switch(key){
		case CURSORLEFT:
			if(inst->cursor_x>0){
				inst->icr_ncx=inst->cursor_x-1;
				inst->icr_ncy=inst->cursor_y;
				}
			else if(inst->cursor_y>0){
				inst->icr_ncx=(LONG)(inst->linearray[inst->cursor_y]-inst->linearray[inst->cursor_y-1])-1;
				inst->icr_ncy=inst->cursor_y-1;
				}
			return TRUE;
			break;
		case CURSORRIGHT:{
			char ch= *(inst->linearray[inst->cursor_y]+inst->cursor_x);
			if(ch=='\n'){
				inst->icr_ncy=inst->cursor_y+1;
				inst->icr_ncx=0;
				return TRUE;
				}
			else if(ch==0);
			else{
				inst->icr_ncy=inst->cursor_y;
				inst->icr_ncx=inst->cursor_x+1;
				return TRUE;
				}
			}
			break;
		case CURSORUP:
			if(inst->cursor_y>0){
				LONG len;
				inst->icr_ncy=inst->cursor_y-1;
				len=LineLength(inst->linearray[inst->icr_ncy]);
				if (len<inst->cursor_x) inst->icr_ncx=len;
				else inst->icr_ncx=inst->cursor_x;
				return TRUE;
			}
			break;
		case CURSORDOWN:
			if(inst->linearray[inst->cursor_y+1]){
				LONG len;
				inst->icr_ncy=inst->cursor_y+1;
				len=LineLength(inst->linearray[inst->icr_ncy]);
				if (len<inst->cursor_x) inst->icr_ncx=len;
				else inst->icr_ncx=inst->cursor_x;
				return TRUE;
			}
			break;
		case (CURSORDOWN | HIKEY_SHIFT):{
			LONG len;
			inst->icr_ncy=inst->cursor_y+(inst->visible_lines*3)/4;
			if(inst->icr_ncy>=inst->lines)
				inst->icr_ncy=inst->lines-1;

			len=LineLength(inst->linearray[inst->icr_ncy]);
				if (len<inst->cursor_x) inst->icr_ncx=len;
				else inst->icr_ncx=inst->cursor_x;

			return TRUE;}
			break;
		case (CURSORUP | HIKEY_SHIFT):{
			LONG len;
			inst->icr_ncy=inst->cursor_y-(inst->visible_lines*3)/4;
			if(inst->icr_ncy<0) inst->icr_ncy=0;

			len=LineLength(inst->linearray[inst->icr_ncy]);
				if (len<inst->cursor_x) inst->icr_ncx=len;
				else inst->icr_ncx=inst->cursor_x;
			return TRUE;}
			break;
		case (CURSORLEFT | HIKEY_SHIFT):
			inst->icr_ncx=0;
			inst->icr_ncy=inst->cursor_y;
			return TRUE;
			break;
		case (CURSORRIGHT | HIKEY_SHIFT):
			inst->icr_ncx=LineLength(inst->linearray[inst->cursor_y]);
			inst->icr_ncy=inst->cursor_y;
			return TRUE;
			break;
/*		case (CURSORUP | HIKEY_CTRL):
			inst->new_cursor_x=inst->cursor_x;
			inst->new_cursor_y=0;
			return TRUE;
			break;
		case (CURSORDOWN | HIKEY_CTRL):
			inst->new_cursor_x=inst->cursor_x;
			inst->new_cursor_y=inst->lines-1;
			return TRUE;
			break;
		case (CURSORLEFT | HIKEY_CTRL):
		case (CURSORRIGHT | HIKEY_CTRL):
			break; */
		}
	return FALSE;
}

/*
 * TextEditField methods
 */

static IPTR mTextEditFieldNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct TextEditFieldClData *inst;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
		return(0);

	inst = INST_DATA(cl,obj);

	inst->VBar = ScrollbarObject, End;
	inst->HBar = ScrollbarObject, MUIA_Group_Horiz, TRUE, End;
	inst->Space =SimpleButton("E");

	inst->TextField = NewObject(TextFieldCl,NULL,
				TextFrame,
				MUIA_Background, MUII_TextBack,
				MUIA_Font,MUIV_Font_Fixed,
				MUIA_TEF_VBar,inst->VBar,
				MUIA_TEF_HBar,inst->HBar,
				MUIA_TEF_Parent,obj,
				TAG_MORE,((struct opSet *)msg)->ops_AttrList,
				TAG_DONE);

	if (!inst->TextField || !inst->VBar || !inst->HBar || !inst->Space)
	{
		if(inst->TextField)
			MUI_DisposeObject(inst->TextField);
		if(inst->VBar)
			MUI_DisposeObject(inst->VBar);
		if(inst->HBar)
			MUI_DisposeObject(inst->HBar);
		if(inst->Space)
			MUI_DisposeObject(inst->Space);
		CoerceMethod(cl,obj,OM_DISPOSE);
		return(0);
	}

	DoMethod(inst->VBar,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,inst->TextField,3,MUIM_Set,MUIA_TEF_FirstY,MUIV_TriggerValue);
	DoMethod(inst->HBar,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,inst->TextField,3,MUIM_Set,MUIA_TEF_FirstX,MUIV_TriggerValue);
	DoMethod(inst->TextField,MUIM_Notify,MUIA_TEF_FirstY,MUIV_EveryTime,inst->VBar,3,MUIM_Set,MUIA_Prop_First,MUIV_TriggerValue);
	DoMethod(inst->TextField,MUIM_Notify,MUIA_TEF_FirstX,MUIV_EveryTime,inst->HBar,3,MUIM_Set,MUIA_Prop_First,MUIV_TriggerValue);
	DoMethod(inst->Space,MUIM_Notify,MUIA_Pressed,FALSE,inst->Space,4,MUIM_CallHook,&EditHook,TextFieldCl,inst->TextField);

	DoMethod(obj,OM_ADDMEMBER,inst->TextField);
	DoMethod(obj,OM_ADDMEMBER,inst->VBar);
	DoMethod(obj,OM_ADDMEMBER,inst->HBar);
	DoMethod(obj,OM_ADDMEMBER,inst->Space);
	set(obj,MUIA_Group_Columns,2);
	set(obj,MUIA_Group_Spacing,1);



	msg->MethodID = OM_SET; 	/* a little trick to parse taglist */
	DoMethodA(obj, (Msg) msg);     	/* and set attributes with OM_SET. */
	msg->MethodID = OM_NEW; 	/* restore method id		   */

	return((IPTR)obj);
}

static IPTR mTextEditFieldDispose(struct IClass *cl,Object *obj,Msg msg)
{
	return(DoSuperMethodA(cl,obj,msg));
}

static IPTR mTextEditFieldShow(struct IClass *cl,Object *obj,Msg msg)
{
//	ULONG res;
	struct TextEditFieldClData *inst=INST_DATA(cl,obj);
	struct TextFieldClData *inst2=INST_DATA(TextFieldCl,inst->TextField);

	DoSuperMethodA(cl,obj,(Msg)msg);
//	if(DoSuperMethodA(cl,obj,(Msg)msg)){
/*		SetAttrs(inst->TextField,MUIA_TEF_ParentL,_mleft(obj),
								MUIA_TEF_ParentR,_mright(obj),
								MUIA_TEF_ParentT,_mtop(obj),
								MUIA_TEF_ParentB,_mbottom(obj),TAG_DONE); */
		inst2->p_l=_mleft(obj); /* Faster, but uglier! */
		inst2->p_r=_mright(obj);
		inst2->p_t=_mtop(obj);
		inst2->p_b=_mbottom(obj);

//		}
//	return res;
	return TRUE;
}

static IPTR mTextEditFieldSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct TagItem *tags;
	struct TagItem *tag;

	for (tags=msg->ops_AttrList;tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{
			case 0: break;
		}
	}
	return(DoSuperMethodA(cl,obj,(Msg)msg));
}


static IPTR mTextEditFieldGet(struct IClass *cl,Object *obj,Msg msg)
{
//	struct TextFieldClData *inst = INST_DATA(cl,obj);

	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case 0: break;
	}

	return(DoSuperMethodA(cl,obj,msg));
}

DISPATCHER(TextEditFieldDispatcher)
{
	switch (msg->MethodID)
	{
		case OM_NEW			: return(mTextEditFieldNew     (cl,obj,(APTR)msg));
		case OM_DISPOSE 	: return(mTextEditFieldDispose (cl,obj,(APTR)msg));
		case OM_GET			: return(mTextEditFieldGet     (cl,obj,(APTR)msg));
		case OM_SET			: return(mTextEditFieldSet     (cl,obj,(APTR)msg));
		case MUIM_Show		: return(mTextEditFieldShow    (cl,obj,(APTR)msg));
		case MUIM_Notify	: if(((struct MUIP_Notify *)msg)->TrigAttr==MUIA_TEF_Active) return DoMethodA(((struct TextEditFieldClData *)INST_DATA(cl,obj))->TextField,msg);
								else return DoSuperMethodA(cl,obj,msg);
	}

	return(DoSuperMethodA(cl,obj,msg));
}

/*
 * The text area class
 */

static IPTR mTextFieldNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct TextFieldClData *inst;
	struct TagItem *tags;
	struct TagItem *tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg)))
		return(0);

	inst = INST_DATA(cl,obj);

	inst->icr_top=0x80000000;

	for (tags=msg->ops_AttrList;tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{
			case MUIA_TEF_VBar:   inst->VBar=(Object *)tag->ti_Data; break;
			case MUIA_TEF_HBar:   inst->HBar=(Object *)tag->ti_Data; break;
			case MUIA_TEF_Width:  inst->width=tag->ti_Data; tag->ti_Tag=TAG_IGNORE; break;
			case MUIA_TEF_Height: inst->height=tag->ti_Data; tag->ti_Tag=TAG_IGNORE; break;
			case MUIA_TEF_Active: inst->active=tag->ti_Data; tag->ti_Tag=TAG_IGNORE; break;
			case MUIA_TEF_Text:   SetText(inst,(char *)tag->ti_Data); tag->ti_Tag=TAG_IGNORE; break;
			case MUIA_TEF_Parent: inst->parent=(Object *)tag->ti_Data; tag->ti_Tag=TAG_IGNORE; break;
		}
	}

//	kprintf("Text: %s\n",inst->text);

	ParseText(inst);

	return((IPTR)obj);
}

static IPTR mTextFieldDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct TextFieldClData *inst=INST_DATA(cl,obj);

	if(inst->notifying){
		EndNotify(&inst->FileNotify);
		if(inst->FileName){
			DeleteFile(inst->FileName);
			free(inst->FileName);
			}
		}
	if(inst->text)
		FreeVec(inst->text);
	if(inst->linearray)
		FreeVec(inst->linearray);
	return(DoSuperMethodA(cl,obj,msg));
}


static IPTR mTextFieldSet(struct IClass *cl,Object *obj,struct opSet *msg)
{
	struct TextFieldClData *inst = INST_DATA(cl,obj);
	struct TagItem *tags;
	struct TagItem *tag;

	for (tags=msg->ops_AttrList;tag=NextTagItem(&tags);)
	{
		switch (tag->ti_Tag)
		{
			case MUIA_TEF_VBar:   inst->VBar=(Object *)tag->ti_Data; break;
			case MUIA_TEF_HBar:   inst->HBar=(Object *)tag->ti_Data; break;
/*			case MUIA_TEF_FirstX: inst->new_x_offset=tag->ti_Data; inst->new_dir=1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
			case MUIA_TEF_FirstY: inst->new_y_offset=tag->ti_Data; inst->new_dir=2; MUI_Redraw(obj,MADF_DRAWUPDATE); break; */

			case MUIA_TEF_FirstX: inst->x_offset=tag->ti_Data; 
									inst->icr_left=_mleft(obj);
									inst->icr_top=_mtop(obj);
									inst->icr_width=_mwidth(obj);
									inst->icr_height=_mheight(obj);
									ClipCursor2(inst,obj);
									MUI_Redraw(obj,MADF_DRAWUPDATE);
									break;
			case MUIA_TEF_FirstY: inst->y_offset=tag->ti_Data;
									inst->icr_left=_mleft(obj);
									inst->icr_top=_mtop(obj);
									inst->icr_width=_mwidth(obj);
										inst->icr_height=_mheight(obj);
									ClipCursor2(inst,obj);
									MUI_Redraw(obj,MADF_DRAWUPDATE);
									break;

			case MUIA_TEF_Width:  inst->width=tag->ti_Data; break;
			case MUIA_TEF_Height: inst->height=tag->ti_Data; break;
			case MUIA_TEF_Active: inst->go_active=tag->ti_Data+1; MUI_Redraw(obj,MADF_DRAWUPDATE); break;
			case MUIA_TEF_Text:   SetText(inst,(char *)tag->ti_Data);
									ParseText(inst);

									set(inst->VBar,MUIA_Prop_Entries,inst->lines*inst->font_height);
									set(inst->VBar,MUIA_Prop_Visible,_mheight(obj));
									set(inst->HBar,MUIA_Prop_Entries,inst->longest_line*inst->font_width);
									set(inst->HBar,MUIA_Prop_Visible,_width(obj));

									MUI_Redraw(obj,MADF_DRAWOBJECT);
									break;




			case MUIA_TEF_ParentL: inst->p_l=tag->ti_Data; break;
			case MUIA_TEF_ParentR: inst->p_r=tag->ti_Data; break;
			case MUIA_TEF_ParentT: inst->p_t=tag->ti_Data; break;
			case MUIA_TEF_ParentB: inst->p_b=tag->ti_Data; break;
		}
	}
	return(DoSuperMethodA(cl,obj,(Msg)msg));
}


static IPTR mTextFieldGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct TextFieldClData *inst = INST_DATA(cl,obj);
	
	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case MUIA_TEF_Text: 	*(((struct opGet *)msg)->opg_Storage) = (IPTR)(inst->text); break;
		case MUIA_TEF_Active:	*(((struct opGet *)msg)->opg_Storage) = (IPTR)(inst->active); break;
	}

	return(DoSuperMethodA(cl,obj,msg));
}

static SAVEDS IPTR TextFieldAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct TextFieldClData *inst = INST_DATA(cl,obj);
	DoSuperMethodA(cl,obj,(Msg)msg);

	msg->MinMaxInfo->MinWidth  += 16;
	msg->MinMaxInfo->DefWidth  += inst->width*inst->font_width;
	msg->MinMaxInfo->MaxWidth  += inst->width*inst->font_width;
//	msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;

	msg->MinMaxInfo->MinHeight += 16;
	msg->MinMaxInfo->DefHeight += inst->height*inst->font_height;
	msg->MinMaxInfo->MaxHeight += inst->height*inst->font_height;
//	msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

//	kprintf("Height :%ld\n",inst->height);

	return(0);
}

static SAVEDS IPTR TextFieldDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct TextFieldClData *inst=INST_DATA(cl,obj);
//	LONG delta,tmp;
	DoSuperMethodA(cl,obj,(Msg)msg);

	if (msg->flags & MADF_DRAWOBJECT){
//		HideCursor(inst,obj); /* The cursor is erased after DoSuperMethodA */
		RefreshTextArea(_window(obj),INST_DATA(cl,obj),obj,inst->x_offset,inst->y_offset,_mleft(obj),_mtop(obj),_mwidth(obj),_mheight(obj));
		ShowCursor(inst,obj);
		}
//	if (MADF_DRAWUPDATE){
//		if(!(inst->cursor_state&2))
//			HideCursor(inst,obj);
//		}
	if (msg->flags & MADF_DRAWUPDATE){
		HideCursor(inst,obj);

//		kprintf("Offset x: %ld y: %ld Top: %ld Left: %ld Width: %ld Height: %ld\n",inst->x_offset,inst->y_offset,inst->icr_left,inst->icr_top,inst->icr_width,inst->icr_height);

		if(inst->icr_top!=0x80000000){
			RefreshTextArea(_window(obj),INST_DATA(cl,obj),obj,inst->x_offset+inst->icr_left-_mleft(obj),inst->y_offset+inst->icr_top-_mtop(obj),inst->icr_left,inst->icr_top,inst->icr_width,inst->icr_height);
			inst->cursor_x=inst->icr_ncx;
			inst->cursor_y=inst->icr_ncy;

			inst->s_cursor_x=inst->cursor_x*inst->font_width-inst->x_offset;
			inst->s_cursor_y=inst->cursor_y*inst->font_height-inst->y_offset;

			inst->icr_top=0x80000000;
			}
		else
			{
			inst->cursor_x=inst->icr_ncx;
			inst->cursor_y=inst->icr_ncy;

			inst->s_cursor_x=inst->cursor_x*inst->font_width-inst->x_offset;
			inst->s_cursor_y=inst->cursor_y*inst->font_height-inst->y_offset;
			}



		inst->active=inst->go_active-1;

/*		if (msg->flags & MADF_DRAWOBJECT || msg->flags & MADF_DRAWUPDATE){
			ShowCursor(inst,obj);
//			ShowCursor(inst,obj,TRUE);
			} */
		ShowCursor(inst,obj);



/*		if(inst->new_dir==1){
			delta=inst->new_x_offset-inst->x_offset;
			if(abs(delta)<_mwidth(obj))
				ScrollRaster(_rp(obj),delta,0,_mleft(obj),_mtop(obj),_mright(obj),_mbottom(obj));
			else
				delta=-_mwidth(obj);
			inst->x_offset=inst->new_x_offset;

			if(delta<0)
				RefreshTextArea(_window(obj),INST_DATA(cl,obj),obj,inst->x_offset,inst->y_offset,_mleft(obj),_mtop(obj),-delta,_mheight(obj));
			else			
				RefreshTextArea(_window(obj),INST_DATA(cl,obj),obj,inst->x_offset+_mwidth(obj)-delta,inst->y_offset,_mright(obj)-delta+1,_mtop(obj),delta,_mheight(obj));
			inst->new_dir=0;
			}
		else if(inst->new_dir==2){
			delta=inst->new_y_offset-inst->y_offset;
			if(abs(delta)<_mheight(obj))
				ScrollRaster(_rp(obj),0,delta,_mleft(obj),_mtop(obj),_mright(obj),_mbottom(obj));
			else
				delta=-_mheight(obj);
			inst->y_offset=inst->new_y_offset;
			if(delta<0)
				RefreshTextArea(_window(obj),INST_DATA(cl,obj),obj,inst->x_offset,inst->y_offset,_mleft(obj),_mtop(obj),_mwidth(obj),-delta);
			else			
				RefreshTextArea(_window(obj),INST_DATA(cl,obj),obj,inst->x_offset,inst->y_offset+_mheight(obj)-delta,_mleft(obj),_mbottom(obj)-delta+1,_mwidth(obj),delta);
			inst->new_dir=0;
			} */

/*		if(inst->new_cursor_x>=0){
			LONG len;
			if(inst->new_cursor_y>=inst->lines)
				inst->new_cursor_y=inst->lines-1;
			if(inst->new_cursor_x>(len=LineLength(inst->linearray[inst->new_cursor_y])))
				inst->new_cursor_x=len;

			inst->cursor_x=inst->new_cursor_x;
			inst->cursor_y=inst->new_cursor_y;
		
			inst->new_cursor_x=-1;
		} */
	}

	return 0;
}

static IPTR TextFieldShow(struct IClass *cl,Object *obj,struct MUIP_Show *msg)
{
	struct TextFieldClData *inst=INST_DATA(cl,obj);
	DoSuperMethodA(cl,obj,(Msg)msg);

//	kprintf("TextField: Show VBar %08lx\n",inst->VBar);

	set(inst->VBar,MUIA_Prop_Entries,inst->lines*inst->font_height);
	set(inst->VBar,MUIA_Prop_Visible,_mheight(obj));
	set(inst->HBar,MUIA_Prop_Entries,inst->longest_line*inst->font_width);
	set(inst->HBar,MUIA_Prop_Visible,_mwidth(obj));

	inst->visible_lines=_mheight(obj)/inst->font_height;
	inst->visible_columns=_mwidth(obj)/inst->font_width;

	return TRUE;
}

static SAVEDS IPTR TextFieldSetup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg){
	struct TextFieldClData *inst=INST_DATA(cl,obj);

	if (!(DoSuperMethodA(cl,obj,(Msg)msg)))
		return(FALSE);
	
	inst->font_width  = _font(obj)->tf_XSize;
	inst->font_height = _font(obj)->tf_YSize;

	set(inst->VBar,MUIA_Prop_DeltaFactor,inst->font_height);
	set(inst->HBar,MUIA_Prop_DeltaFactor,inst->font_width);

//	kprintf("Font width: %ld height: %ld\n",inst->font_width,inst->font_height);

	MUI_RequestIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_VANILLAKEY);
	return TRUE;
}

static SAVEDS IPTR TextFieldCleanup(struct IClass *cl,Object *obj,struct MUIP_Setup *msg){
	MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_VANILLAKEY);
	return(DoSuperMethodA(cl,obj,(Msg)msg));
}

SAVEDS IPTR TextFieldHandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
	#define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))
	#define _isinarea(x,y) (_between(inst->p_l,(x),inst->p_r) && _between(inst->p_t,(y),inst->p_b))

	struct TextFieldClData *inst = INST_DATA(cl,obj);

	DoSuperMethodA(cl,obj,(Msg)obj);

	if (msg->imsg)
	{
		switch (msg->imsg->Class)
		{
			case IDCMP_RAWKEY:
				if(inst->active){
					long key;
					key=msg->imsg->Code;
					if(msg->imsg->Qualifier & IEQUALIFIER_SHIFT)
						key |= HIKEY_SHIFT;
					if(msg->imsg->Qualifier & IEQUALIFIER_ALT)
						key |= HIKEY_ALT;
					if(msg->imsg->Qualifier & IEQUALIFIER_CONTROL)
						key |= HIKEY_CTRL;
					if(HandleSpecialKeys(inst,key)){
						ClipCursor(inst,obj);
						MUI_Redraw(obj,MADF_DRAWUPDATE);
						}
					}
				break;
			case IDCMP_VANILLAKEY:
				if(inst->active){
					if(InsertChar(obj,inst,msg->imsg->Code)){
						ClipCursor(inst,obj);
						MUI_Redraw(obj,MADF_DRAWUPDATE);
						}
					}
				break;
			case IDCMP_MOUSEBUTTONS:
				if (msg->imsg->Code==SELECTDOWN){
					LONG i;
					if (_isinarea(msg->imsg->MouseX,msg->imsg->MouseY)){
						if(!inst->active) set(inst->parent,MUIA_TEF_Active,TRUE);
						if(_isinobject(msg->imsg->MouseX,msg->imsg->MouseY)){
							inst->icr_ncx=(msg->imsg->MouseX-_mleft(obj)+inst->x_offset) / inst->font_width;
							inst->icr_ncy=(msg->imsg->MouseY-_mtop(obj)+inst->y_offset) / inst->font_height;
							MUI_RequestIDCMP(obj,IDCMP_MOUSEMOVE);
							if(inst->icr_ncy>=inst->lines)
								inst->icr_ncy=inst->lines-1;
							if((i=LineLength(inst->linearray[inst->icr_ncy]))<inst->icr_ncx)
								inst->icr_ncx=i;
							ClipCursor(inst,obj);
							MUI_Redraw(obj,MADF_DRAWUPDATE);
							}
						}
						else if(inst->active) set(inst->parent,MUIA_TEF_Active,FALSE);
					}
				else if (msg->imsg->Code==SELECTUP)
					MUI_RejectIDCMP(obj,IDCMP_MOUSEMOVE);
				break;

			case IDCMP_MOUSEMOVE:
				if (_isinobject(msg->imsg->MouseX,msg->imsg->MouseY)){
					LONG i;
					inst->icr_ncx=(msg->imsg->MouseX-_mleft(obj)+inst->x_offset) / inst->font_width;
					inst->icr_ncy=(msg->imsg->MouseY-_mtop(obj)+inst->y_offset) / inst->font_height;
					if(inst->icr_ncy>=inst->lines)
						inst->icr_ncy=inst->lines-1;
					if((i=LineLength(inst->linearray[inst->icr_ncy]))<inst->icr_ncx)
						inst->icr_ncx=i;
					ClipCursor(inst,obj);
					MUI_Redraw(obj,MADF_DRAWUPDATE);
					}
			break;
		}
	}
	return(0);
}

SAVEDS IPTR TextFieldFileChange(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
	struct TextFieldClData *inst=INST_DATA(cl,obj);
	APTR fh;
	APTR mem;
	IPTR size;
//	kprintf("Getting filenotification\n");
	if(inst->FileName)
		if(fh=fopen(inst->FileName,"r")){
			fseek(fh,0,SEEK_END);
			size=ftell(fh);
			fseek(fh,0,SEEK_SET);
			mem=AllocVec(size+1,MEMF_CLEAR);
			fread(mem,1,size,fh);
			fclose(fh);
			SetText(inst,mem);
			ParseText(inst);
			FreeVec(mem);

			set(inst->VBar,MUIA_Prop_Entries,inst->lines*inst->font_height);
			set(inst->VBar,MUIA_Prop_Visible,_mheight(obj));
			set(inst->HBar,MUIA_Prop_Entries,inst->longest_line*inst->font_width);
			set(inst->HBar,MUIA_Prop_Visible,_width(obj));

			MUI_Redraw(obj,MADF_DRAWOBJECT);
			}
	return 0;
}

DISPATCHER(TextFieldDispatcher)
{
	switch (msg->MethodID)
	{
		case MUIM_HandleInput: return(TextFieldHandleInput(cl,obj,(APTR)msg));
		case OM_NEW		     : return(mTextFieldNew       (cl,obj,(APTR)msg));
		case OM_DISPOSE 	 : return(mTextFieldDispose   (cl,obj,(APTR)msg));
		case OM_GET		     : return(mTextFieldGet       (cl,obj,(APTR)msg));
		case OM_SET		     : return(mTextFieldSet       (cl,obj,(APTR)msg));
		case MUIM_AskMinMax  : return(TextFieldAskMinMax  (cl,obj,(APTR)msg));
		case MUIM_Draw       : return(TextFieldDraw       (cl,obj,(APTR)msg));
		case MUIM_Show       : return(TextFieldShow       (cl,obj,(APTR)msg));
		case MUIM_Setup      : return(TextFieldSetup      (cl,obj,(APTR)msg));
		case MUIM_Cleanup    : return(TextFieldCleanup    (cl,obj,(APTR)msg));
		case MUIM_TEF_FileChange : return(TextFieldFileChange    (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,msg));
}

Class *TextEditFieldClInit(void)
{
  APTR SuperClass;
  Class *cl;

  if (!(SuperClass = MUI_GetClass(MUIC_Group)))
    fail(NULL, "Superclass for TextEditField class not found.");
  if (!(cl = MakeClass(NULL, NULL, SuperClass, sizeof(struct TextEditFieldClData), 0)))
    {
      MUI_FreeClass(SuperClass);
      fail(NULL, "Failed to create TextEditField class.");
    }
  cl->cl_Dispatcher.h_Entry = ENTRY(TextEditFieldDispatcher);
  cl->cl_Dispatcher.h_SubEntry = NULL;
  cl->cl_Dispatcher.h_Data = NULL;

  if (!(SuperClass = MUI_GetClass(MUIC_Area)))
   {
    fail(NULL, "Superclass for TextEditField class not found.");
   }
  if (!(TextFieldCl = MakeClass(NULL, NULL, SuperClass, sizeof(struct TextFieldClData), 0)))
    {
      MUI_FreeClass(SuperClass);
      FreeClass(cl);
      fail(NULL, "Failed to create TextEditField class.");
    }
  TextFieldCl->cl_Dispatcher.h_Entry = ENTRY(TextFieldDispatcher);
  TextFieldCl->cl_Dispatcher.h_SubEntry = NULL;
  TextFieldCl->cl_Dispatcher.h_Data = NULL;

  return cl;
}

BOOL TextEditFieldClFree(Class *cl)
{
 FreeClass(TextFieldCl);
 FreeClass(cl);
 return 0L;
}


HOOKPROTO(EditFunc, IPTR, APTR object, struct EditFuncMsg *msg)
{
	char buffer[256],buffer2[256]="run ";
	APTR fh;
	long res = TRUE;

	struct TextFieldClData *inst=INST_DATA(msg->cl,msg->obj);

	if(!inst->FileName){
		inst->FileName=malloc(128);

		do{
			sprintf(inst->FileName,"t:TextEditFieldTemp.%d",(int)inst->FileNo);
			if(fh=fopen(inst->FileName,"r")){
				fclose(fh);
				inst->FileNo++;
				}
			}while(fh);
		}

	if(fh=fopen(inst->FileName,"w")){
		fputs(inst->text,fh);
		fclose(fh);
//		kprintf("String %s\n",inst->text);

		if(!inst->notifying){
			memset(&inst->FileNotify,0,sizeof(struct NotifyRequest));
			inst->FileNotify.nr_Name=inst->FileName;
			inst->FileNotify.nr_UserData=(IPTR)msg->obj; /* So that main knows what to call */
			inst->FileNotify.nr_Flags=NRF_SEND_MESSAGE;
			inst->FileNotify.nr_stuff.nr_Msg.nr_Port=FileNotifyPort;
			res=StartNotify(&inst->FileNotify);
//			kprintf("Starting notify. Result %ld\n",res);
			inst->notifying=1;
			}


		GetVar("editor",(char *)&buffer,256,0);
		if(*buffer==0x0)
			strcpy(buffer,"c:ed");
		strcat(buffer2,buffer);
		strcat(buffer2," ");
		strcat(buffer2,inst->FileName);

		Execute(buffer2,BNULL,BNULL);
	}
	return res;
}
