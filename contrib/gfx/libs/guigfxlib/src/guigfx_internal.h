#ifndef GUIGFX_INTERNAL_H
#define GUIGFX_INTERNAL_H	1


#include "exec/exec.h"
#include "graphics/view.h"
#include "utility/tagitem.h"


/****************************************************************************

	debugging

****************************************************************************/

#ifdef DEBUG
#define DB(x)	x
#else
#define DB(x)	;
#endif

#ifdef __MORPHOS__
void dprintf(char *, ... );
#undef kprintf
#define kprintf dprintf
#elif defined(__AROS__)
#include <aros/debug.h>
#else
VOID kprintf(STRPTR,...);
#endif

/****************************************************************************

	Structure definitions

****************************************************************************/

typedef APTR PALETTE;

struct SharedHistogram
{
	struct SignalSemaphore semaphore;
	ULONG count;
	APTR histogram;
};

typedef struct Picture
{
	struct	SignalSemaphore semaphore;
	UBYTE	*array;					/* Daten */
	UWORD	width, height;			/* Breite/Höhe in Pixeln */
	UWORD	aspectx, aspecty;		/* x:y Pixelaspekt */
	PALETTE	palette;				/* Palette im render.library Format */
	ULONG	pixelformat;			/* Pixelformat */


	BOOL	alphapresent;			/* alpha-channel vorhanden */
	UBYTE	*alphaarray;			/* separater (ausgelagerter) alpha-channel */

	struct	SharedHistogram *histogram;

	UWORD	hstype;					/* palette und histogramm */

	BOOL	owner;					/* zeigt an, daß das Array auch gelöscht werden muß */
	ULONG	maxbytes;				/* nur bei owner==FALSE - Größe des Speicherbereichs */

	struct	DrawHandle *directdraw;	/* auf dieses Drawhandle kann direkt gezeichnet werden */


	APTR	scaleengine;
	WORD	scalesourcewidth, scalesourceheight;
	WORD	scaledestwidth, scaledestheight;
	ULONG	scalepixelformat;
	BOOL	scalecoords;

} PIC;


typedef struct ColorHandle
{
	struct	Node node;						/* Verkettung in der colorlist der PenShareMap */
	ULONG	numpixels;						/* Anzahl repräsentierter Pixel */
	UWORD	weight;							/* Gewichtung */
	struct	PenShareMap *psm;				/* zugehörige Pensharemap */
	struct	SharedHistogram *histogram;		/* SharedHistogram */

} COLORHANDLE;


typedef struct RastHandle
{
	struct	RastPort *rp;
	UWORD	drawmode;
	BOOL	truecolor;
	struct	RastPort *temprp;
	UWORD	colormode;

} RASTHANDLE;


typedef struct DrawHandle
{
	struct	RastHandle *rasthandle;	/* RastHandle mit Informationen zum Rastport */

	struct	PenShareMap *psm;		/* die zugehörige PenShareMap */

	struct	ColorMap *colormap;		/* Colormap, von der angefordert wurde */
	ULONG	precision;				/* precision, mit der angefordert wurde */

	PALETTE	mainpalette;
	UWORD	mainnumcolors;
	UBYTE	mainpentab[256];

//	APTR	basepalette;
//	UWORD	basenumcolors;
//	UBYTE	basepentab[256];

	struct	SignalSemaphore mapsemaphore;	//	semaphore für die mapengine
	APTR	mapengine;						// mapengine für eine realpalette

	PALETTE	realpalette;			/* die tatsächlichen Farben auf dem Screen */

	UWORD	dithermode;
	UWORD	ditheramount;
	BOOL	autodither;
	UWORD	ditherthreshold;

	ULONG	modeID;					/* optional: screen's modeID */

	ULONG	bgcolor;
	LONG	bgpen;

} DRAWHANDLE;


typedef struct PenShareMap
{
	UWORD	hstype;				/* globale Einstellungen */

	struct	SignalSemaphore semaphore;	/* Schutz für die folgenden Daten */

	APTR	histogram;			/* globales Histogramm der PenShareMap */
	struct	List colorlist;			/* Liste der Colorhandles */
	UWORD	numcolorhandles;		/* Anzahl Colorhandles in der Liste */
	BOOL	modified;			/* die Liste wurde verändert */

} PSM;



typedef struct DirectDrawHandle
{
	DRAWHANDLE	*dh;				// das entsprechende Drawhandle
	int sourcewidth, sourceheight;
	int destwidth, destheight;

	APTR scaleengine;				// scaling-engine

	APTR scalebuffer;				// Buffer für skalierte Daten
	APTR mapbuffer;					// if present, it's more effective to map before scale

	int	drawmode;			

	int pixelformat;				// source
	
} DIRECTDRAWHANDLE;

#define	DDMODE_DRAW				0
#define	DDMODE_SCALEDRAW		1
#define	DDMODE_MAPDRAW			2
#define	DDMODE_MAPSCALEDRAW		3
#define	DDMODE_SCALEMAPDRAW		4



/****************************************************************************

	miscellaneous

****************************************************************************/

typedef struct TagItem * TAGLIST;

#define	PIXELSIZE(a) ((a) == PIXFMT_CHUNKY_CLUT ? sizeof(UBYTE) : ((a) == PIXFMT_0RGB_32 ? sizeof(ULONG) : 3))

#define RED_RGB32(a) ((((a) & 0xff0000)<<8) + ((a) & 0xff0000) + (((a) & 0xff0000)>>8) + (((a) & 0xff0000)>>16))
#define GREEN_RGB32(a) ((((a) & 0x00ff00)<<16) + (((a) & 0x00ff00)<<8) + ((a) & 0x00ff00) + (((a) & 0x00ff00)>>8))
#define BLUE_RGB32(a) ((((a) & 0x0000ff)<<24) + (((a) & 0x0000ff)<<16) + (((a) & 0x0000ff)<<8) + ((a) & 0x0000ff))


/****************************************************************************

	constants

****************************************************************************/

#define DRAWMODE_WRITEPIXELARRAY		0
#define DRAWMODE_WRITECHUNKYPIXELS		1
#define DRAWMODE_CYBERGFX				2
#define DRAWMODE_BITMAP					3

#define THRESHOLD_SPAREMODE				20

#define	DEFAULT_AUTODITHER_THRESHOLD	250
#define	DEFAULT_AUTODITHER				TRUE
#define	DEFAULT_DITHERMODE				DITHERMODE_EDD
#define	DEFAULT_DITHERAMOUNT			40

#define	DEFAULT_RMHTYPE					RMHTYPE_POOL	/* !!!!! */

#define	DEFAULT_HSTYPE					HSTYPE_12BIT_TURBO
#define	DEFAULT_PICTURE_HSTYPE			DEFAULT_HSTYPE
#define	DEFAULT_PALETTE_HSTYPE			DEFAULT_HSTYPE
#define	DEFAULT_PICTURE_INTERLEAVE		7

#define	DEFAULT_RGBWEIGHT				0x010101

#define	HSTYPE_UNDEFINED				0

#define CYBERGFX_VERSION				40
#define	RENDER_VERSION					30
#define DATATYPES_VERSION				39

#define	DEFAULT_BGCOLOR					0x000000

#endif
