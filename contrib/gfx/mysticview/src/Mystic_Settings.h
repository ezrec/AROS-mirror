#ifndef MYSTIC_SETTINGS_H
#define	MYSTIC_SETTINGS_H 1

/*********************************************************************
----------------------------------------------------------------------

	MysticView 
	Settings

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#include "Mystic_Global.h"


/*--------------------------------------------------------------------

	main settings structure

--------------------------------------------------------------------*/

struct mainsettings
{
	struct SignalSemaphore semaphore;

	BOOL recursive;				// scan directories recursively?
	BOOL requestfile;			// requester at startup

	WORD winleft;
	WORD wintop;
	WORD winwidth;
	WORD winheight;
	WORD winopenmode;
	WORD winsizemode;
	BOOL borderless;
	WORD refreshmode;
	WORD displaymode;
	BOOL previewmode;

	char *pubscreen;
	UWORD depth;
	ULONG precision;
	BOOL hamscreen;
	WORD screenaspectx, screenaspecty;
	WORD scrwidth, scrheight;
	ULONG modeID;

	ULONG bgcolor;
	ULONG textcolor;
	ULONG markcolor;
	
	UWORD hstype;
	int dither;
	BOOL staticpalette;

	BOOL slideshow;	
	int slidedelay;
	BOOL loop;
	BOOL autoquit;
	BOOL autohide;
	
	BOOL grid;
	BOOL picinfo;
	ULONG dithermode;
	char *startpic;
	int sortmode;
	BOOL reverse;
	
	double zoomstep;
	
	BOOL markcenter;
	int scropenmode;

	int cx_priority;
	char *hotkey;
	BOOL cx_popup;

	int rotatestep;
	int confirmlevel;
	int iconifymode;
	char *appiconname;
	
	int	thumbsize;
	int thumbnumcolors;
	BOOL createthumbnails;
	
	char *rejectpattern;
	
	BOOL resetdisplaysettings;
	
	int leftmouseaction;
	
	char *fontspec;
	
	BOOL autodisplaymode;

	BOOL autocrop;
	
	BOOL asyncscanning;
	
	int bufferpercent;

	BOOL filterpictures;
	
	BOOL hideonclose;
	
	BOOL autostop;
	
	BOOL autoclear;
	
	BOOL showarrows;
	
	BOOL hideonescape;
	
	BOOL showpip;

	BOOL smoothdisplay;	

	BOOL simplescanning;
	
	char *buttonpic;
	char *animpic;

	BOOL showbuttons;
	
	char *defaulttool;
	
	ULONG realbgcolor;
	
	char *picformat;

	BOOL appendicons;

	BOOL browsemode;
};


/*--------------------------------------------------------------------

	path settings structure

--------------------------------------------------------------------*/

struct pathsettings
{
	struct SignalSemaphore semaphore;

	char *startpath;
	char *destpath[10];
	char *listpath;
	char *startpic;
	char *presetpath;
	char *savepath;
	char *copypath;

	char *preset[10];

	BOOL autosavesettings;
};


#include "MysticView.h"
#include "Mystic_InitExit.h"

/*--------------------------------------------------------------------

	prototypes

--------------------------------------------------------------------*/

extern struct mainsettings * STDARGS CreateMainSettings(char **ttypes, struct mainsettings *oldsettings, ...);
extern void DeleteMainSettings(struct mainsettings *settings);

extern struct pathsettings * STDARGS CreatePathSettings(char **ttypes, struct pathsettings *oldsettings, ...);
extern void DeletePathSettings(struct pathsettings *settings);

extern BOOL GetBooleanSetting(char **ttypes, char *name, BOOL def);

extern char **GetToolTypes(char *filename);
extern BOOL PutToolTypes(char *filename, char **tt);
extern char **SetToolType(char **ttypes, char *entry, char *value);


extern BOOL SaveDefaultSettings(struct mainsettings *settings, char *filename);
extern struct mainsettings *LoadDefaultSettings(char *filename);


extern BOOL SavePreset(struct mview *mv, struct mainsettings *settings, char *filename);
extern struct mainsettings *LoadPreset(char *filename, struct mainsettings *oldsettings);


extern BOOL SavePathSettings(struct pathsettings *pathsettings, char *pathname);
extern struct pathsettings *LoadPathSettings(char *pathname, struct pathsettings *oldsettings);


extern BOOL SetPrefsNode(struct List *list, char *setting, char *value, BOOL enabled);


#endif

