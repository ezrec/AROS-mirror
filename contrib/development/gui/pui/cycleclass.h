#define CYCLEIMAGEWIDTH 19
#define ARROWWIDTH 7
#define ARROWHEIGHT 4

#define WINMAP_WINDOW 0
#define WINMAP_BITMAP 1

// it is possible to scroll faster through the
// popup after TURBOCOUNTDOWN intuiticks
// (= 1/10 sec) by moving around the mouse
// (to force mousemove events to be sent to
// the gadget)

#define TURBOCOUNTDOWN 5

struct cycleitem
{
	char				*string;
	WORD				charlen;
	WORD				pixellen;
};

struct cycledata
{
	struct MinList		itemlist;
	struct cycleitem	*itemmemory;
	struct RastPort	*rp;
	struct RastPort	clonerp;
	char					**entries;
	APTR					winmap;		/* either window or bitmap, depending on popup_uselayer */
	WORD					itemheight;
	WORD					itemwidth;
	WORD					menuleft;
	WORD					menutop;
	WORD					menuwidth;
	WORD					menuheight;
	WORD					numitems;
	WORD					active;
	WORD					visible;
	WORD					top;
	WORD					selected;
	WORD					menux1;
	WORD					menuy1;
	WORD					menux2;
	WORD					menuy2;
	WORD					layerx1;
	WORD					layery1;
	WORD					layerx2;
	WORD					layery2;
	BYTE					borderleft;
	BYTE					borderright;
	BYTE					bordertop;
	BYTE					borderbottom;
	BYTE					maypopup;
	BYTE					popup;
	BYTE					winmaptype;
	BYTE					uparrowblack;
	BYTE					downarrowblack;
	BYTE					sentgadgetup;
	BYTE					turbocountdown;
};

