
#include "../common/nsmtracker.h"
#include "../common/visual_proc.h"


int GFX_CreateVisual(struct Tracker_Windows *tvisual){return 0;}
int GFX_ShutDownVisual(struct Tracker_Windows *tvisual){return 0;}

bool GFX_SelectEditFont(struct Tracker_Windows *tvisual){return true;}

void GFX_Line(struct Tracker_Windows *tvisual,int color,int x,int y,int x2,int y2){return ;}
void GFX_All_Line(struct Tracker_Windows *tvisual,int color,int x,int y,int x2,int y2){return ;}
void GFX_Box(struct Tracker_Windows *tvisual,int color,int x,int y,int x2,int y2){return ;}
void GFX_FilledBox(struct Tracker_Windows *tvisual,int color,int x,int y,int x2,int y2){return ;}
void GFX_Slider_FilledBox(struct Tracker_Windows *tvisual,int color,int x,int y,int x2,int y2){return ;}
void GFX_All_FilledBox(struct Tracker_Windows *tvisual,int color,int x,int y,int x2,int y2){return ;}

void GFX_Text(
	struct Tracker_Windows *tvisual,
	int color,
	char *text,
	int x,
	int y
){return ;}

void GFX_Text_noborder(
	struct Tracker_Windows *tvisual,
	int color,
	char *text,
	int x,
	int y
){return ;}

void GFX_InvertText(
	struct Tracker_Windows *tvisual,
	int color,
	char *text,
	int x,
	int y
){return ;}


void GFX_InvertTextNoText(
	struct Tracker_Windows *tvisual,
	int color,
	int x,
	int y
){return ;}

void GFX_InitDrawCurrentLine(
	struct Tracker_Windows *tvisual,
	int x, int y, int x2, int y2
){return ;}
void GFX_InitDrawCurrentLine2(
	struct Tracker_Windows *tvisual,
	int x, int y, int x2, int y2
){return ;}

void GFX_DrawCursorPos(
	struct Tracker_Windows *tvisual,
	int fx, int fy, int fx2, int fy2,
	int x, int y, int x2, int y2
){return ;}

void GFX_DrawTrackBorder(
	struct Tracker_Windows *tvisual,
	int x, int y, int y2
){return ;}

void GFX_SetWindowTitle(struct Tracker_Windows *tvisual,char *title){return ;}

void GFX_Scroll(
	struct Tracker_Windows *tvisual,
	int dx,int dy,
	int x,int y,
	int x2,int y2
){return ;}

void GFX_ScrollDown(
	struct Tracker_Windows *tvisual,
	int dx,int dy,
	int x,int y,
	int x2,int y2
){return ;}

void GFX_ClearWindow(struct Tracker_Windows *tvisual){return ;}

int GFX_ResizeWindow(struct Tracker_Windows *tvisual,int x,int y){return 0;}

void SetNormalPointer(struct Tracker_Windows *tvisual){return ;}
void SetResizePointer(struct Tracker_Windows *tvisual){return ;}

ReqType GFX_OpenReq(struct Tracker_Windows *tvisual,int width,int height,char *title){return NULL;}
void GFX_CloseReq(struct Tracker_Windows *tvisual,ReqType reqtype){return ;}

int GFX_GetInteger(struct Tracker_Windows *tvisual,ReqType reqtype,char *text,int min,int max){return 0;}

float GFX_GetFloat(struct Tracker_Windows *tvisual,ReqType reqtype,char *text,float min,float max){return 0.0f;}

char *GFX_GetString(struct Tracker_Windows *tvisual,ReqType reqtype,char *text){return "null";}

int GFX_Menu(
	struct Tracker_Windows *tvisual,
	ReqType reqtype,
	char *seltext,
	int num_sel,
	char **menutext
){return 0;}

char *GFX_GetLoadFileName(
	struct Tracker_Windows *tvisual,
	ReqType reqtype,
	char *seltext,
	char *dir
){return NULL;}

char *GFX_GetSaveFileName(
	struct Tracker_Windows *tvisual,
	ReqType reqtype,
	char *seltext,
	char *dir
){return NULL;}
