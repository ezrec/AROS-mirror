#include <exec/semaphores.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <intuition/intuition.h>
#include <graphics/videocontrol.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/graphics_lib.h>
#include <pragma/keymap_lib.h>
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/cybergraphics.h>
#include <proto/layers.h>
#include <proto/dos.h>
#endif

#include "global.h"
#include "config.h"
#include "misc.h"

#include <string.h>
#include <stdio.h>

static APTR mempool;
static struct SignalSemaphore memsem;

/*********************************************************/

static void GadSelect(struct Gadget *gad,struct Window *win,BOOL on)
{
	LONG oldpos;

	oldpos = RemoveGadget(win,gad);

	if (on)
	{
		gad->Flags |= GFLG_SELECTED;
	} else {
		gad->Flags &= (~GFLG_SELECTED);
	}

	AddGadget(win,gad,oldpos);
	RefreshGList(gad,win,0,1);
}

static WORD mystrlen(char *s)
{
	WORD rc = 0;

	while ((*s != '\0') &&
			 (*s != '\n'))
	{
		s++;
		rc++;
	}

	return rc;
}

/*********************************************************/

void InitMisc(void)
{
	InitSemaphore(&memsem);

	if (!(mempool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR,2000,2000)))
	{
		Cleanup("Misc: Out of memory!");
	}
}

void CleanupMisc(void)
{
	if (mempool) DeletePool(mempool);
}

APTR MiscAllocVec(ULONG size)
{
	ULONG *rc;

	ObtainSemaphore(&memsem);
	if ((rc = AllocPooled(mempool,size + sizeof(ULONG))))
	{
		*rc++ = size + sizeof(ULONG);
	}
	ReleaseSemaphore(&memsem);

	return rc;
}

void MiscFreeVec(APTR mem)
{
	ULONG *realmem,size;

	if (mem)
	{
		ObtainSemaphore(&memsem);

		realmem = (ULONG *)mem;
		size = *--realmem;

		FreePooled(mempool,realmem,size);

		ReleaseSemaphore(&memsem);
	}
}

void CalcITextSize(struct ReqNode *reqnode,struct IntuiText *it,WORD *width,WORD *height)
{
	WORD w,h;

	ObtainSemaphore(&temprpsem);

	SetFont(&temprp,reqnode->font);

	*width = 0;
	*height = 0;

	while(it)
	{
		w = TextLength(&temprp,it->IText,mystrlen(it->IText));
		h = reqnode->font->tf_YSize;

		if (it->LeftEdge > 0) w += it->LeftEdge;

		if (it->TopEdge > 0) h += it->TopEdge;

		if (w > *width) *width = w;
		if (h > *height) *height = h;

		it = it->NextText;

	} /* while(it) */

	ReleaseSemaphore(&temprpsem);
}

void MyPrintIText(struct ReqNode *reqnode,struct RastPort *rp,struct IntuiText *it,WORD dx,WORD dy,WORD itextwidth)
{
	WORD x,len,pixellen;

	SetDrMd(rp,JAM1);

	while(it)
	{
		len = mystrlen(it->IText);
		pixellen = TextLength(rp,it->IText,len);

		switch(reqnode->cfg.reqtextalign)
		{
			case ALIGN_RIGHT:
				x = dx + itextwidth - pixellen;
				break;

			case ALIGN_CENTER:
				x = dx + (itextwidth - pixellen) / 2;
				break;

			default:
				x = dx + it->LeftEdge;
				break;
		}

		Move(rp,x,dy + it->TopEdge + reqnode->font->tf_Baseline);

		Text(rp,it->IText,len);

		it = it->NextText;

	} /* while(it) */

}

BOOL KeyGadget(struct Window *win,struct Gadget *gad,WORD icode,BOOL rawkey)
{
	struct IntuiMessage *msg;
	//ULONG oldidcmp;
	BYTE buff[2];
	char c;
	BOOL rc = TRUE,exitme = FALSE,exitok = FALSE;

	c = icode;

	if (rawkey)
	{
		buff[0] = icode;
		icode = 1;
	} else {
		icode = MapANSI(&c,1,buff,1,0);
	}

	if (gad && (icode == 1))
	{
		if (buff[0] == 67) buff[1] = 68;
			else if (buff[0] == 68) buff[1] = 67;
				else buff[1] = buff[0];

		//ldidcmp = win->IDCMPFlags;
		//ModifyIDCMP(win,IDCMP_RAWKEY|IDCMP_GADGETUP|IDCMP_GADGETDOWN);

		GadSelect(gad,win,TRUE);

		while(!(exitme && exitok))
		{
			WaitPort(win->UserPort);
			while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
			{
				if (msg->Class == IDCMP_GADGETUP) exitme = TRUE;
				//if (msg->Class == IDCMP_GADGETDOWN) exitme = TRUE;
					exitok = exitme;
				if (msg->Class == IDCMP_RAWKEY)
				{
					if (!exitme)
					{
						/* LSHIFT, RSHIFT, ESC? */
						if (msg->Code == 0x60 || msg->Code == 0x61 || (buff[0] != 69 && msg->Code == 69))
						{
							rc = FALSE;exitme = TRUE;
							GadSelect(gad,win,FALSE);
						}
					}

					/* initial key released? */
					if (msg->Code == (buff[0] | IECODE_UP_PREFIX) || msg->Code == (buff[1] | IECODE_UP_PREFIX))
					{
						exitme=TRUE;
						exitok=TRUE;
					}
				}
				if (msg->Class == IDCMP_MOUSEBUTTONS)
				{
					switch (msg->Code)
					{
						case IECODE_UP_PREFIX|IECODE_MBUTTON:
							exitme = TRUE;exitok = TRUE;
						break;

						default:
							rc = FALSE;exitme = TRUE;
							GadSelect(gad,win,FALSE);
						break;
					}
				}
				ReplyMsg((struct Message *)msg);

			} /* while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort))) */

		} /* while(!(exitme && exitok)) */

		GadSelect(gad,win,FALSE);
		//ModifyIDCMP(win,oldidcmp);

	} /* if (gad && (MapANSI(&c,1,buff,1,0) == 1)) */

	return rc;
}

void VisibleScreenSize(struct Screen *scr,WORD *width,WORD *height)
{
	struct TagItem cmtags[]={{VTAG_VIEWPORTEXTRA_GET,0},
									 {TAG_DONE}};
	struct Rectangle myrect;
	struct ViewPortExtra *vpe;
	ULONG displayid;
	WORD x,y;

	if(!VideoControl(scr->ViewPort.ColorMap,cmtags))
	{
		vpe = (struct ViewPortExtra *)cmtags[0].ti_Data;
	} else {
		vpe = scr->ViewPort.ColorMap->cm_vpe;
		if(!vpe)
		{
			vpe=(struct ViewPortExtra *)GfxLookUp(&scr->ViewPort);
		}
	}

	if(vpe && ((displayid = GetVPModeID(&scr->ViewPort)) != INVALID_ID))
	{
		QueryOverscan(displayid,&myrect,OSCAN_TEXT);

		x = vpe->DisplayClip.MaxX - vpe->DisplayClip.MinX + 1;
		y = vpe->DisplayClip.MaxY - vpe->DisplayClip.MinY + 1;

		if(x < (myrect.MaxX - myrect.MinX + 1))
		{
			x = myrect.MaxX - myrect.MinX + 1;
		}

		if(y < (myrect.MaxY - myrect. MinY + 1))
		{
			y = myrect.MaxY - myrect.MinY + 1;
		}

	} else {

		x = scr->Width;
		y = scr->Height;

	}

	if (width) *width = x;
	if (height) *height = y;

}

void DrawFrame(struct ReqNode *reqnode,struct RastPort *rp,WORD x1,WORD y1,WORD x2,WORD y2,WORD pen1,WORD pen2,WORD pen3,WORD pen4,WORD bgpen,BYTE gadq)
{
	UBYTE count,add;

	SetDrMd(rp,JAM1);

	for(count = 0;count < 2;count++)
	{

		add = 0;
		if (gadq)
		{
			if (count == 0)
			{
				if (reqnode->cfg.roundbuttons)
				{
					add = 1;
				}
			}
		}	/*J*/

		if (PENOK(pen1))
		{
			if (reqnode->rtg)
			{
				FillPixelArray(rp,x1+add,y1,x2-x1-add,1,PEN(pen1));
				FillPixelArray(rp,x1,y1+1,1,y2-add-y1,PEN(pen1));
			} else {
				SetAPen(rp,PEN(pen1));

				/* top edge */
				RectFill(rp,x1 + add,y1,x2 - 1, y1);

				/* left edge */
				RectFill(rp,x1,y1 + 1,x1, y2 - add);
			}
		}

		if (PENOK(pen2))
		{
			if (reqnode->rtg)
			{
				FillPixelArray(rp,x2,y1+add,1,y2-add-y1,PEN(pen2));
				FillPixelArray(rp,x1+1,y2,x2-x1-add,1,PEN(pen2));
			} else {
				SetAPen(rp,PEN(pen2));

				/* right edge */
				RectFill(rp,x2,y1 + add,x2,y2 - add);

				/* bottom edge */
				RectFill(rp,x1 + 1,y2,x2 - 1,y2);
			}
		}


		if (PENOK(pen1))
		{
			x1++;
			y1++;
		}

		if (PENOK(pen2))
		{
			x2--;
			y2--;
		}

		pen1 = pen3;
		pen2 = pen4;

	} /* for(count = 0;count < 2;count++) */

			// j added = button background pattern :))
			if (bgpen >= 0)
			{
			if (MYIMAGEOK(&reqnode->buttonbackfillimage))
			{
				reqnode->actbackfillimage = &reqnode->buttonbackfillimage;
				EraseRect(rp,x1,y1,x2,y2);
				reqnode->actbackfillimage = &reqnode->backfillimage;

			} else {
				if (reqnode->rtg)
				{
					FillPixelArray(rp,x1,y1,x2-x1+1,y2-y1+1,PEN(bgpen));
				} else {
					SetAPen(rp,PEN(bgpen));
					RectFill(rp,x1,y1,x2,y2);
				}
			}
			} //if MYIMAGEOK
	if (gadq)
	{
		if (reqnode->cfg.roundbuttons)
		{
			if (PENOK(pen1))
			{
				if (reqnode->rtg)
				{
					FillPixelArray(rp,x1,y1,1,1,PEN(pen1));
				} else {
					SetAPen(rp,PEN(pen1));
					RectFill(rp,x1,y1,x1,y1);
				}
			}
			if (PENOK(pen2))
			{
				if (reqnode->rtg)
				{
					FillPixelArray(rp,x2,y2,1,1,PEN(pen2));
				} else {
					SetAPen(rp,PEN(pen2));
					RectFill(rp,x2,y2,x2,y2);
				}
			}
		}
	}
}

/*********************************************************/

char *GetTaskName(struct ReqNode *reqnode)
{
	struct Task *t;

	t = FindTask(0);

	return t->tc_Node.ln_Name ? t->tc_Node.ln_Name : "";
}

char *GetProcName(struct ReqNode *reqnode)
{
	struct Process *pr;
	struct CommandLineInterface *cli;
	char *rc = "";

	pr = (struct Process *)FindTask(0);

	if (pr->pr_Task.tc_Node.ln_Type == NT_PROCESS)
	{
		if ((cli = BADDR(pr->pr_CLI)))
		{
			if (cli->cli_CommandName)
			{
				rc = BADDR(cli->cli_CommandName);
				rc++;
			}
		}
	}

	return rc;
}

char *GetProgName(struct ReqNode *reqnode)
{
	char *rc;

	rc = GetTaskName(reqnode);

	if (rc[0] == '\0') rc = GetProcName(reqnode);

	return rc;
}

char *GetProgName2(struct ReqNode *reqnode)
{
	char *rc;

	rc = GetProcName(reqnode);

	if (rc[0] == '\0') rc = GetTaskName(reqnode);

	return rc;
}

char *GetScreenTitle(struct ReqNode *reqnode)
{
	return reqnode->scr->Title ? (char *)reqnode->scr->Title : "";
}

char *GetReqTitle(struct ReqNode *reqnode)
{
	return reqnode->title;
}

char *GetReqText(struct ReqNode *reqnode)
{
	return reqnode->bodytext;
}

LONG GetScreenDepth(struct ReqNode *reqnode)
{
	return GetBitMapAttr(reqnode->scr->RastPort.BitMap,BMA_DEPTH);
}

LONG GetScreenWidth(struct ReqNode *reqnode)
{
	return reqnode->scr->Width;
}

LONG GetScreenHeight(struct ReqNode *reqnode)
{
	return reqnode->scr->Height;
}

LONG GetVisibleWidth(struct ReqNode *reqnode)
{
	WORD width;

	VisibleScreenSize(reqnode->scr,&width,0);

	return width;
}

LONG GetVisibleHeight(struct ReqNode *reqnode)
{
	WORD height;

	VisibleScreenSize(reqnode->scr,0,&height);

	return height;
}

LONG GetResolutionX(struct ReqNode *reqnode)
{
	return reqnode->dri->dri_Resolution.X;
}

LONG GetResolutionY(struct ReqNode *reqnode)
{
	return reqnode->dri->dri_Resolution.Y;
}

LONG GetAspect(struct ReqNode *reqnode)
{
	WORD x,y,rc;

	x = reqnode->dri->dri_Resolution.X;
	y = reqnode->dri->dri_Resolution.Y;

	if (x > y)
	{
		rc = -((x * 32 / y + 16) / 32);
	} else {
		rc = (y * 32 / x + 16) / 32;
	}

	if (rc == -1) rc = 1;

	return rc;
}


LONG GetNumButtons(struct ReqNode *reqnode)
{
	return reqnode->num_buttongadgets;
}

/*********************************************************/

