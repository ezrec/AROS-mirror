#ifndef CONFIG_H
#define CONFIG_H

#define ALIGN_TOP    -1
#define ALIGN_LEFT   -1
#define ALIGN_BOTTOM  1
#define ALIGN_RIGHT   1
#define ALIGN_CENTER  0
#define ALIGN_SPREAD  2

#define BALIGN_LEFT  -1
#define BALIGN_TOP    0
#define BALIGN_RIGHT  1
#define BALIGN_BOTTOM 2

#define RPOS_NORMAL   0
#define RPOS_CENTER   1
#define RPOS_MOUSE    2

#define BITMAPF_DISPLAYABLE 1
#define BITMAPF_MINPLANES	 2
#define BITMAPF_FRIEND		 4
#define BITMAPF_INTERLEAVED 8

#define CMD_SET       1
#define CMD_IF        2
#define CMD_ELSE      3
#define CMD_ELSEIF    4
#define CMD_STOP_		 5

#define INTERNAL_LOGO_INFO			((char *)0x1)
#define INTERNAL_LOGO_ASK			((char *)0x3)
#define INTERNAL_LOGO_MULTIASK	((char *)0x5)
#define INTERNAL_IMAGE_OK			((char *)0x7)
#define INTERNAL_IMAGE_CANCEL		((char *)0x9)
#define INTERNAL_IMAGE_THIRD		((char *)0xB)
#define IMAGE_OFF						((char *)0xD)

#define REQOFF_OFF 		-1
#define REQOFF_ICONIFY 	-2

enum
{
	RPEN_GADGETHILIGHT,
	RPEN_PATTERN1,
	RPEN_PATTERN2,
	RPEN_TEXTBOXBG,
	RPEN_TEXTBOXFG,
	RPEN_TEXTBOXSHADOW,
	RPEN_OTEXTBOXEDGE1,
	RPEN_OTEXTBOXEDGE2,
	RPEN_ITEXTBOXEDGE1,
	RPEN_ITEXTBOXEDGE2,
	RPEN_BUTTONBG,
	RPEN_BUTTONBGSEL,
	RPEN_BUTTONBGHI,
	RPEN_BUTTONFG,
	RPEN_BUTTONFGSEL,
	RPEN_BUTTONFGHI,
	RPEN_OBUTTONEDGE1,
	RPEN_OBUTTONEDGE2,
	RPEN_IBUTTONEDGE1,
	RPEN_IBUTTONEDGE2,
	RPEN_OBUTTONEDGE1SEL,
	RPEN_OBUTTONEDGE2SEL,
	RPEN_IBUTTONEDGE1SEL,
	RPEN_IBUTTONEDGE2SEL,
	RPEN_OBUTTONEDGE1HI,
	RPEN_OBUTTONEDGE2HI,
	RPEN_IBUTTONEDGE1HI,
	RPEN_IBUTTONEDGE2HI,
	
	NUM_RPENS
};

#define RCOLOR_COLMASK  0x00FFFFFF
#define RCOLOR_TYPEMASK 0xFF000000

#define RCOLOR_RGB		0x00000000
#define RCOLOR_IGNORE   0x01000000
#define RCOLOR_DIRECT   0x02000000
#define RCOLOR_DRIPEN	0xFF000000

#define RPENF_ALLOCED 1

struct ReqPen
{
	LONG	color;
	LONG  pen;
	UBYTE flags;
};

#define PEN(x)		 	(reqnode->cfg.pens[x].pen)
#define PENOK(x)	 	(reqnode->cfg.pens[x].color != RCOLOR_IGNORE)
#define DIRECTPEN(x) (((reqnode->cfg.pens[x].color) & RCOLOR_TYPEMASK) == RCOLOR_DIRECT)
#define ORGBPEN(x)   (reqnode->cfg.pens[x].color)

struct config
{
	char *infologo;
	char *asklogo;
	char *multiasklogo;
	char *okimage;
	char *cancelimage;
	char *thirdimage;
	char *logo;
	char *backfill;
	char *textbackground;
	char *buttonbackfill;
	UBYTE okkey;
	UBYTE cancelkey;
	UBYTE runkey;
	UBYTE editkey;
	UBYTE iconifykey;

	char *soundport;
	char *sound;
	char *oksound;
	char *cancelsound;
	char *othersound1;
	char *othersound2;
	char *othersound3;
		
	char *textfont;
	WORD textfontsize;
	char *buttonfont;
	WORD buttonfontsize;

	struct ReqPen pens[NUM_RPENS];

	WORD imagecaches;
	BYTE bitmapflags;
	
	UBYTE keyqualifier;
	BYTE cooldragging;
	
	BYTE outerspacingx;
	BYTE outerspacingy;
	BYTE innerspacingx;
	BYTE innerspacingy;
	BYTE bodygadgetspacing;
	BYTE buttonspacing;
	BYTE bodylogospacing;
	BYTE logoalign;
	BYTE bodyxalign;
	BYTE bodyyalign;
	BYTE reqtextalign;
	BYTE buttonalign;
	BYTE buttonyalign;
	BYTE hithickx;
	BYTE hithicky;
	BYTE reqpos;
	BYTE nobuttonimages;
	BYTE nologo;
	BYTE samebuttonwidth;
	BYTE buttontextstyle;
	BYTE buttontextselstyle;
	BYTE buttontexthistyle;
	BYTE shiftbuttontext;
	BYTE buttonispacingx;
	BYTE buttonispacingy;
	BYTE buttonispacingy2;
	BYTE buttonextrawidth;
	BYTE bimagetextspacing;
	BYTE functionkeys;
	BYTE donotpatch;
	BYTE roundbuttons; /*j*/
	BYTE reqoff;
	BYTE logoalign2;
	BYTE previewmode;
	char *info;
	char *reqoffexec;
};

struct backconfig
{
	char soundport[256];
	char oksound[256];
	char cancelsound[256];
	char othersound1[256];
	char othersound2[256];
	char othersound3[256];
	char info[256];
	
};

struct configcmd
{
	struct MinNode node;

	WORD cmd;
 
	union
	{
		struct
		{
			LONG what;
			LONG data;
		} set;
		struct
		{
			WORD check;
			WORD  op;
			LONG data;
			struct MinList then;
		} _if;
		struct
		{
			struct MinList this;
		} _else;
	} par;
};

#define CONFIGAGE_ALL 0xFFFF

struct configmem
{
	struct MinNode node;
	ULONG size;
	UWORD age;
};

extern ULONG configmask;
extern struct config defaultconfig;

#endif

