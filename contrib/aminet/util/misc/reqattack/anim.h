#ifndef ANIM_H
#define ANIM_H

#ifndef MYIMAGE_H
#include "myimage.h"
#endif

#define ANIMM_START 1
#define ANIMM_STOP  2

#define ANIMMR_OK   0
#define ANIMMR_FAIL 1

#define SIGF_ROGER SIGBREAKF_CTRL_D

#define TICS_PER_SEC 50

#define TIC_MICRO_DELAY(x) (1000000L * (x) / TICS_PER_SEC)
#define TIC_SEC_DELAY(x) ((x) / 50)

struct AnimMsg
{
	struct Message msg;
	struct RastPort *rp;
	struct MyImage *im;
	WORD x,y;
	WORD code;
};

struct AnimNode
{
	struct Node node;
	struct RastPort *rp;
	struct MyImage *im;
	WORD destx,desty;
	WORD activeframe;
	WORD frameinc;
	WORD tick;
};

void InitAnim(void);
void CleanupAnim(void);
WORD SendAnimMsg(WORD code,struct RastPort *rp,struct MyImage *im,WORD x,WORD y);
void AnimTask(void);

#endif

