#ifndef REQLIST_H
#define REQLIST_H

#ifndef MYIMAGE_H
#include "myimage.h"
#endif

#ifndef MYGADGET_H
#include "mygadget.h"
#endif

#ifndef MYDOPUS_H
#include "mydopus.h"
#endif

#ifndef CONFIG_H
#include "config.h"
#endif

#ifndef REQATTACK_H
#include "reqattack.h"
#endif

struct ReqNode
{
	struct Node	 		node;
	struct Window		*win;
	struct Screen		*scr;
	struct ViewPort	 	*vp;
	struct ColorMap 	*cm;
	struct DrawInfo		*dri;
	struct TextFont		*font;
	char				*title;
	char 				*bodytext;
	char				*buttontext;
	struct IntuiText	*body;
	struct MyGadget	 	*buttongadgets;
	struct Gadget		*draggad;
	struct MyImage		reqimage;
	struct MyImageSpec	*reqimagespec;
	struct MyImage		backfillimage;
	struct MyImageSpec	*textbackfillimagespec;
	struct MyImage		textbackfillimage;
	struct MyImageSpec	*backfillimagespec;
	struct MyImage		buttonbackfillimage;
	struct MyImageSpec	*buttonbackfillimagespec;
	struct MyImage		*actbackfillimage;
	struct Hook			backfillhook;
	ULONG				soundhandle;
	ULONG				customidcmp;
	ULONG				flags;
	WORD				num_buttongadgets;
	WORD 				hilightpen;
	WORD 				activegad;
	
	/* here's the sizes of layout parts...*/
	WORD				buttonwidth;
	WORD				buttonheight;
	WORD				buttonx;
	WORD				buttony;
	WORD				maxbwidth;
	WORD				maxbheight;
	WORD				buttonextrawidth;
	WORD				buttonextraheight;

	WORD				bodywidth;
	WORD				bodyheight;
	WORD				bodyx;
	WORD				bodyy;
	
	WORD				textheight;
	WORD				textwidth;
	
	BOOL 				rtg;
	struct config		cfg;
	struct backconfig	bcfg;
	BOOL				fontok;	/*J adds*/
	struct TextFont	 	*buttonfont;
	BOOL				buttonfontok;
	struct PopupMenu	*popupmenu;
	BOOL	 			dissound;
	struct AppIcon		*appicon;
	struct DiskObject	*appicondiskobject;
	struct Image		*iconifyimage;
	struct Gadget		*iconifygadget;
	struct Image		*menuimage;
	struct Gadget		*menugadget;
	struct DeveloperAttack *devattack;
};

struct ReqNodeExt
{
	struct MyGadget 	*extragadgets;
	ULONG				numextragadgets;
	ULONG 				*callback;
	APTR				callbackdata;
	struct MyBuffer		*textbuffer;
	char 				*checkmark;
	short				*checkmarkshort;
	ULONG				*buttoncodes;
};

#define REQF_INLIST				 1
#define REQF_FREEBODY			 2
#define REQF_POPBACKSCREEN		 4
#define REQF_FREEHILIGHTPEN	 8
#define REQF_LOGOANIMATED      16

#endif

