#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <proto/dos.h>

#ifdef __MAXON__
#include <pragma/intuition_lib.h>
#else
#include <proto/intuition.h>
#endif

#include "global.h"
#include "reqlist.h"
#include "myimage.h"
#include "myimageclass.h"
#include "mygadget.h"
#include "mygadget_protos.h"

#include "stdio.h"

BOOL MakeMyGadget(struct ReqNode *reqnode,struct MyGadget *mygad,char *text,WORD left,WORD top,WORD width,WORD height,WORD id)
{
	BOOL rc=FALSE;
	
	if ((mygad->im = NewObject(MyImageClass,0,
				SYSIA_DrawInfo,(LONG)reqnode->dri,
				IA_Data,MYIMAGEOK(&mygad->myim) ? (LONG)&mygad->myim : 0,
				GA_Text,(LONG)text,
				IA_Left,0,
				IA_Top,0,
				IA_Width,width,
				IA_Height,height,
				MYIA_ReqNode,(LONG)reqnode,
				TAG_DONE)))
	{
		if ((mygad->gad = (struct Gadget *)NewObject(0,BUTTONGCLASS,
				GA_ID,id,
				GA_Left,left,
				GA_Top,top,
				GA_Width,width,
				GA_Height,height,
				GA_RelVerify,TRUE,
				GA_Image,(LONG)mygad->im,
				TAG_DONE)))
		{
			rc=TRUE;
			
			SetAttrs(mygad->im,MYIA_Gadget,(LONG)mygad->gad,
									 TAG_DONE);
		}
	}
	
 	return rc;
}

void FreeMyGadget(struct Window *win,struct MyGadget *mygad)
{
	if (mygad->gad)
	{
		if (win) RemoveGadget(win,mygad->gad);
		DisposeObject((Object *)mygad->gad);
		mygad->gad = 0;
	}
	
	if (mygad->im)
	{
		DisposeObject(mygad->im);
		mygad->im = 0;
	}
}


