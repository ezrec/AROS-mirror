#ifndef MYGADGET_PROTOS_H
#define MYGADGET_PROTOS_H

BOOL MakeMyGadget(struct ReqNode *reqnode,struct MyGadget *mygad,char *text,WORD left,WORD top,WORD width,WORD height,WORD id);
void FreeMyGadget(struct Window *win,struct MyGadget *mygad);

#endif

