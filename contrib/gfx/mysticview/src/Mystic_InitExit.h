#ifndef MYSTIC_INITEXIT_H
#define	MYSTIC_INITEXIT_H 1

/*********************************************************************
----------------------------------------------------------------------

	MysticView 
	InitExit definitions

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#include <libraries/commodities.h>

#include "Mystic_Global.h"
#include "Mystic_Icons.h"

/*--------------------------------------------------------------------

	prototypes

--------------------------------------------------------------------*/

/*
 *		mysticview
 */

struct mview
{
	struct SignalSemaphore semaphore;

	PICTURE *logopic;
	PICTURE *buttonpic;
	PICTURE *animpic;
	
	struct mvscreen *screen;
	struct mvwindow *window;

	struct FileRequester *freq;				// main file requester
	struct FileRequester *destfreq;			// copy/move to path file requester
	struct FileRequester *listfreq;			// list file requester
	struct FileRequester *startpicfreq;		// list file requester
	struct FileRequester *presetfreq;
	struct FileRequester *savefreq;
	struct FileRequester *copyfreq;
	struct FileRequester *renamefreq;
	
	
	APTR abouttask;

	struct ScreenModeRequester *modereq;
	
	char *progfilename;
	
	struct NewMenu *copynewmenuarray;
	struct NewMenu *movenewmenuarray;
	struct NewMenu *presetmenuarray;
	struct NewMenu *pubscreenmenuarray;

	struct MenuItem *copypathmenuitems;
	struct MenuItem *movepathmenuitems;
	struct MenuItem *presetmenuitems;
	struct MenuItem *pubscreenmenuitems;

	struct NewBroker *newbroker;
	CxObj *broker;
	struct MsgPort *brokermsgport;

	char **pubscreenlist;

	struct MsgPort *appmsgport;
	ULONG appSignal;
	
	struct DiskObject *diskobject;
	struct AppIcon *appicon;
	struct AppMenuItem *appitem;
	
	struct DiskObject *iconifyicon;


	struct IconThumbnail *appiconthumbnail;
};


#include "Mystic_Settings.h"

extern struct mview *CreateMView(struct mainsettings *mvs, char *progfilename);
extern void DeleteMView(struct mview *mv);

#endif
