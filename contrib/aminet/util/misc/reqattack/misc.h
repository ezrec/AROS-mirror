#ifndef MISC_H
#define MISC_H

#ifndef REQLIST_H
#include "reqlist.h"
#endif

void InitMisc(void);
void CleanupMisc(void);
APTR MiscAllocVec(ULONG size);
void MiscFreeVec(APTR mem);
void CalcITextSize(struct ReqNode *reqnode,struct IntuiText *it,WORD *width,WORD *height);
void MyPrintIText(struct ReqNode *reqnode,struct RastPort *rp,struct IntuiText *it,WORD dx,WORD dy,WORD itextwidth);
BOOL KeyGadget(struct Window *win,struct Gadget *gad,WORD icode,BOOL rawkey);
void VisibleScreenSize(struct Screen *scr,WORD *width,WORD *height);
void DrawFrame(struct ReqNode *reqnode,struct RastPort *rp,WORD x1,WORD y1,WORD x2,WORD y2,WORD pen1,WORD pen2,WORD pen3,WORD pen4,WORD bgpen, BYTE gadq);

char *GetTaskName(struct ReqNode *reqnode);
char *GetProcName(struct ReqNode *reqnode);
char *GetProgName(struct ReqNode *reqnode);
char *GetProgName2(struct ReqNode *reqnode);
char *GetScreenTitle(struct ReqNode *reqnode);
char *GetReqTitle(struct ReqNode *reqnode);
char *GetReqText(struct ReqNode *reqnode);
LONG GetScreenDepth(struct ReqNode *reqnode);
LONG GetScreenWidth(struct ReqNode *reqnode);
LONG GetScreenHeight(struct ReqNode *reqnode);
LONG GetVisibleWidth(struct ReqNode *reqnode);
LONG GetVisibleHeight(struct ReqNode *reqnode);
LONG GetResolutionX(struct ReqNode *reqnode);
LONG GetResolutionY(struct ReqNode *reqnode);
LONG GetAspect(struct ReqNode *reqnode);
LONG GetNumButtons(struct ReqNode *reqnode);

#endif

