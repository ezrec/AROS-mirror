/*********************************************************************
----------------------------------------------------------------------

	MysticView
	data

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#ifndef MYSTICVIEW_H
#define MYSTICVIEW_H


#include <exec/types.h>
#include <guigfx/guigfx.h>


enum MVSTATUS
{
	STATUS_WORKING,
	STATUS_REOPENWINDOW,
	STATUS_REOPENALL,
	STATUS_QUIT,
	STATUS_SLEEP
};




struct viewdata
{
	float zoom;					// zoom
	float xpos, ypos;			// position
	int angle;
	
	PICTURE *showpic;			// current picture
	
	char **startfilepatternlist;		// current list of filenames/patterns

	char *pathname;
	char *filename;
	BOOL isdirectory;
	
	char *formatname;

	char statustext[1200];
	char picturetext[1000];

	//--------------------------------------------

	float oldzoom;
	float oldxpos, oldypos;
	int oldangle;

	//--------------------------------------------

	struct SignalSemaphore rastlock;

	UBYTE	picturevisiblesignal;

	struct trigger *trigger;			// nextpic trigger

	struct picloader *picloader;
	struct picview *view;

	struct pichandler *pichandler;

	//--------------------------------------------

	ULONG tagbuffer[40];
	
	float	destzoom, destxpos, destypos;
	
};



#define GAD_OPEN			0
#define GAD_RESTART			1
#define GAD_PREV			2
#define GAD_NEXT			3
#define GAD_SLIDE			4
#define GAD_LOOP			5
#define GAD_DISPLAYMODE		6
#define GAD_RESET			7
#define GAD_NUM				8

#define BUTTON_HSPACE	4
#define BUTTON_VSPACE	2


struct MVButtons
{
	APTR pic;
	APTR animpic;
	
	APTR psm;
	APTR drawhandle;
	ULONG width, height;

	struct BitMap *bitmap;
	struct BitMap *animbitmap;

	ULONG gadgetwidth, gadgetheight;
	struct FakeGadgetList *gadgetlist;
	struct FakeGadget *gadgets[GAD_NUM];
	
	int animframe;
};


#include "Mystic_InitExit.h"
#include "Mystic_Settings.h"

void DeleteViewPic(struct viewdata *viewdata, PICTURE *defaultpic, struct mainsettings *settings);
void GetCurrentPicture(struct mview *mv, struct MVButtons *buttons, struct viewdata *viewdata, 
	struct mainsettings *settings, PICTURE *defaultpic);
void SetViewText(struct viewdata *viewdata, char *text);
char *SetPicInfoText(struct viewdata *viewdata, PICTURE *pic, struct mainsettings *settings);

#endif
