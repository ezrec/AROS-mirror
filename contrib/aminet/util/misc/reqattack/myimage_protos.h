#ifndef MYIMAGE_PROTOS_H
#define MYIMAGE_PROTOS_H

#ifndef MYIMAGE_H
#include "myimage.h"
#endif

#ifndef REQLIST_H
#include "reqlist.h"
#endif

void InitMyImage(void);
void CleanupMyImage(void);

BOOL MakeMyImage(struct ReqNode *reqnode,struct MyImage *im,struct MyImageSpec *spec,LONG bgpen,BOOL DoMask);
void AllocMyImageHelpBM(struct MyImage *im,struct BitMap *blitbm);
void SetupMyImageHelpBM(struct MyImage *im,struct RastPort *rp,WORD x,WORD y);
void FreeMyImageMask(struct MyImage *im);
void FreeMyImage(struct MyImage *im);
void DrawMyImage(struct RastPort *rp,struct MyImage *im,WORD x,WORD y,WORD frame);

#endif

