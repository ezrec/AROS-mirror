#include <aros/oldprograms.h>
#include "wtime.h"


long _stack=4000;
long _priority=0;
long _BackGroundIO=0;
char *_procname="worldtime";

struct TimeZone TimeZones[84];

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *Mesg;
struct timerequest timereq;
struct MsgPort *timerport;
struct TextFont *myfont;

struct TextAttr topaz8 =
{
    "topaz.font", 8, 0, 0
};

struct NewWindow
	win={
		0,0,640,200,-1,-1,CLOSEWINDOW|MOUSEBUTTONS,
		WINDOWCLOSE|WINDOWDEPTH|WINDOWDRAG|RMBTRAP|ACTIVATE,
		NULL,NULL,(STRPTR)"WorldTime v1.31 by Jonathan Potter, 1991",
		NULL,NULL,0,0,0,0,WBENCHSCREEN},
	iconwin={
		100,0,388,0,-1,-1,CLOSEWINDOW|MOUSEBUTTONS,
		WINDOWCLOSE|WINDOWDEPTH|WINDOWDRAG|RMBTRAP,
		NULL,NULL,NULL,NULL,NULL,
		0,0,0,0,WBENCHSCREEN};

char yourplace[20],otherplace[20],defotherplace[20];
float yourtime,othertime,defothertime;

char
	*months[12]={
		"January","February","March","April","May","June",
		"July","August","September","October","November","December"},
	*days[7]={
		"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
	*shortmonths[12]={
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
int wdays[12]={
	31,28,31,30,31,30,31,31,30,31,30,31};
int lastmin,timeoff,lastday;
int todaycol,yesterdaycol,tomorrowcol,clockcol,citycol;
