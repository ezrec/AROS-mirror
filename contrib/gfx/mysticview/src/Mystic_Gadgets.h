#ifndef _MADRID_FAKEGADGETS
#define _MADRID_FAKEGADGETS

/*********************************************************************
----------------------------------------------------------------------

	Madrid fake gadgets

----------------------------------------------------------------------
*********************************************************************/

#include <exec/lists.h>
#include <guigfx/guigfx.h>


struct FakeGadgetList
{
	struct List list;
	int numgadgets;
	
	int x, y;
	int width, height;
	
	struct Window *window;
	struct RastPort *rp;

	struct FakeGadget *rollovergadget;
	struct FakeGadget *pressedgadget;
	
};


struct FakeGadget
{
	struct Node node;
	BOOL inserted;

	ULONG id;
	ULONG type;
	BOOL togglestatus;
	int status;
	BOOL visible;

	int x, y;				// inserted from layout
	
	int hborder;
	int vborder;

	int width, height;


	LONG pen_inactive;
	LONG pen_rollover;
	LONG pen_pressed;
	LONG pen_active;
	LONG pen_activerollover;
	LONG pen_activepressed;

//	struct BitMap *bitmap;

	ULONG picx_inactive;
	ULONG picx_rollover;
	ULONG picx_pressed;
	ULONG picx_active;
	ULONG picx_activerollover;
	ULONG picx_activepressed;

	ULONG picy_inactive;
	ULONG picy_rollover;
	ULONG picy_pressed;
	ULONG picy_active;
	ULONG picy_activerollover;
	ULONG picy_activepressed;
};


#define	FGDT_DUMMY 					(TAG_USER + 111)
#define FGDT_HBORDER 				(FGDT_DUMMY+1)
#define FGDT_VBORDER 				(FGDT_DUMMY+2)
#define FGDT_WIDTH 					(FGDT_DUMMY+3)
#define FGDT_HEIGHT	 				(FGDT_DUMMY+4)
#define FGDT_INACTIVEPEN 			(FGDT_DUMMY+5)
#define FGDT_PRESSEDPEN 			(FGDT_DUMMY+6)
#define FGDT_ROLLOVERPEN 			(FGDT_DUMMY+7)
#define FGDT_ACTIVEPEN 				(FGDT_DUMMY+8)
#define FGDT_ACTIVEROLLOVERPEN 		(FGDT_DUMMY+9)
#define FGDT_ACTIVEPRESSEDPEN 		(FGDT_DUMMY+10)
#define	FGDT_BITMAP					(FGDT_DUMMY+11)

#define	FGDT_PICX_INACTIVE			(FGDT_DUMMY+12)
#define	FGDT_PICX_PRESSED			(FGDT_DUMMY+13)
#define	FGDT_PICX_ROLLOVER			(FGDT_DUMMY+14)
#define	FGDT_PICX_ACTIVE			(FGDT_DUMMY+15)
#define	FGDT_PICX_ACTIVEROLLOVER	(FGDT_DUMMY+16)
#define	FGDT_PICX_ACTIVEPRESSED		(FGDT_DUMMY+17)

#define	FGDT_PICY_INACTIVE			(FGDT_DUMMY+18)
#define	FGDT_PICY_PRESSED			(FGDT_DUMMY+19)
#define	FGDT_PICY_ROLLOVER			(FGDT_DUMMY+20)
#define	FGDT_PICY_ACTIVE			(FGDT_DUMMY+21)
#define	FGDT_PICY_ACTIVEROLLOVER	(FGDT_DUMMY+22)
#define	FGDT_PICY_ACTIVEPRESSED		(FGDT_DUMMY+23)

enum ORIENTATION
{
	ORIENTATION_RIGHT,
	ORIENTATION_DOWN
};

enum FGADTYPE
{
	FGADTYPE_PUSH,
	FGADTYPE_TOGGLE
};

enum FGADSTATUS
{
	FGSTATUS_INACTIVE,
	FGSTATUS_PRESSED,
	FGSTATUS_ROLLOVER,
};


#define INVALID_FAKEGADGET		(0xffffffff)

void DeleteFakeGadgetList(struct FakeGadgetList *fgl);
struct FakeGadgetList * STDARGS CreateFakeGadgetList(struct Window *win, ...);
void RemFakeGadget(struct FakeGadget *fg);
void DeleteFakeGadget(struct FakeGadget *fg);
struct FakeGadget * STDARGS CreateFakeGadget(ULONG id, ULONG type, ...);
void AddFakeGadget(struct FakeGadgetList *fgl, struct FakeGadget *fg);
void GetLayoutData(struct FakeGadgetList *fgl, int orientation, int *minwidth, int *minheight);
BOOL LayoutFakeGadgetList(struct FakeGadgetList *fgl, int orientation, int width, int height, int x, int y);
void DrawFakeGadgetList(struct FakeGadgetList *fgl, struct BitMap *bm);
ULONG HandleGadgetListEvent(struct FakeGadgetList *fgl, struct IntuiMessage *imsg, struct BitMap *bm);
void DrawFakeGadget(struct Window *win, struct FakeGadget *fg, int x, int y, struct BitMap *bitmap);

#endif
