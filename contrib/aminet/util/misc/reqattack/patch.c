#include <intuition/intuition.h>
#include <cybergraphx/cybergraphics.h>
#include <utility/tagitem.h>
#include <libraries/reqtools.h>
#include <exec/memory.h>
#ifndef NO_POPUPMENU
#include <clib/pm_protos.h>
#include <libraries/pm.h>
#endif
//#include <dopus/requesters.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/graphics_lib.h>
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <proto/utility.h>
#ifndef NO_POPUPMENU
#include <proto/pm.h>
#endif
#include <proto/wb.h>
#include <proto/icon.h>
#endif

#include <clib/alib_protos.h>

#include <string.h>
#include <stdio.h>

#include "global.h"
#include "config.h"
#include "myimage.h"
#include "reqlist.h"
#include "mygadget.h"
#include "myrequester.h"
#include "patch.h"
#include "asmpatch.h"
#include "misc.h"
#include "asmmisc.h"
#include "imageloader.h"
#include "anim.h"
#include "cx.h"
#include "sound.h"
#include "application.h"

#include "mygadget_protos.h"
#include "myimage_protos.h"
#include "reqlist_protos.h"

#include <aros/debug.h>

APTR oldeasyrequestargs;
APTR oldautorequest;
APTR oldbuildeasyrequestargs;
APTR oldbuildsysrequest;
APTR oldfreesysrequest;
APTR oldsysreqhandler;
APTR oldreqtools;
//APTR olddopus;

struct PatchEntry patchtable[NUM_PATCHES] =
{
	{TRUE,LVO_SysReqHandler,		 &oldsysreqhandler,		  Asm_SysReqHandler			},
	{TRUE,LVO_FreeSysRequest,		 &oldfreesysrequest,		  Asm_FreeSysRequest			},
	{TRUE,LVO_BuildSysRequest,		 &oldbuildsysrequest,	  Asm_BuildSysRequest		},
	{TRUE,LVO_BuildEasyRequestArgs,&oldbuildeasyrequestargs,Asm_BuildEasyRequestArgs	},
	{TRUE,LVO_EasyRequestArgs,		 &oldeasyrequestargs,	  Asm_EasyRequestArgs		},
	{TRUE,LVO_AutoRequest,			 &oldautorequest,			  Asm_AutoRequest				}
};

struct PatchEntry reqtoolstable[2] =
{
	{TRUE,LVO_rtEZRequestA,&oldreqtools,Asm_ReqTools}
};

/*struct PatchEntry dopustable[1] =
{
	{TRUE,LVO_AsyncRequest,&olddopus,Asm_DOpus}
};*/

void InstallPatches(void)
{
	WORD i;

	if (cyreq)
	{
		patchtable[4].dopatch = FALSE;
		patchtable[5].dopatch = FALSE;
	}

	Forbid();

	for(i = 0;i < NUM_PATCHES;i++)
	{
		if (patchtable[i].dopatch)
		{
			*(patchtable[i].oldfunc) = SetFunction((struct Library *)IntuitionBase,
																patchtable[i].lvo,
																patchtable[i].newfunc);
			patchtable[i].patchinstalled = TRUE;
			CacheClearU();
		}
	}

	if (ReqToolsBase)
	{
			*(reqtoolstable[0].oldfunc) = SetFunction(ReqToolsBase,
																reqtoolstable[0].lvo,
																reqtoolstable[0].newfunc);
	}

	/*if (DOpusBase)
	{
			*(dopustable[0].oldfunc) = SetFunction(DOpusBase,
																dopustable[0].lvo,
																dopustable[0].newfunc);
		printf("dop patch ok\n");
	}*/

	CacheClearU();

	Permit();
}

BOOL RemovePatches(void)
{
	WORD i,i2;
	APTR oldfunc[NUM_PATCHES],reqfunc[1];//,dopusfunc[1];
	BOOL rc = FALSE;

#ifndef __AROS__
	if (CleanupApplication() == FALSE)
	{
		return FALSE;
	}
#endif

	Forbid();

	if ((patchentrycounter == 0) && ReqListEmpty())
	{

		rc = TRUE;

		if (ReqToolsBase)
		{
			reqfunc[0] = SetFunction(ReqToolsBase,
											 reqtoolstable[0].lvo,
											 *(reqtoolstable[0].oldfunc));
			if (reqfunc[0] != reqtoolstable[0].newfunc) rc=FALSE;
		}

		/*if (DOpusBase)
		{
		if (rc)
		{
			dopusfunc[0] = SetFunction(DOpusBase,
											 dopustable[0].lvo,
											 *(dopustable[0].oldfunc));
			if (dopusfunc[0] != dopustable[0].newfunc) rc=FALSE;
		}
		}*/

		if (rc) //added because there might be a possibility that
				  //we won't be able to remove ReqTools patch!
		{
		for (i = 0;i < NUM_PATCHES;i++)
		{
			if (patchtable[i].patchinstalled)
			{
				oldfunc[i] = SetFunction((struct Library *)IntuitionBase,
												 patchtable[i].lvo,
												 *(patchtable[i].oldfunc));

				if (oldfunc[i] != patchtable[i].newfunc)
				{
					for (i2 = 0;i2 <= i;i2++)
					{
						if (patchtable[i2].patchinstalled)
						{
							SetFunction((struct Library *)IntuitionBase,
											patchtable[i2].lvo,
											oldfunc[i2]);
						}
					}
					rc = FALSE;
					break;

				} /* if (oldfunc[i] != patchtable[i].newfunc) */

			} /* if (patchtable[i].patchinstalled) */

		} /* for (i = 0;i < NUM_PATCHES;i++) */
		} // if (rc)
		CacheClearU();

		Permit();

	} /* if ((patchentrycounter == 0) && ReqListEmpty()) */

	if (rc)
	{
		for (i = 0;i < NUM_PATCHES;i++)
		{
			patchtable[i].patchinstalled = FALSE;
		}
	}

	return rc;
}

/*#if 0*/

/* New_EasyRequestArgs now in asmpatch.asm -
   copied directly from CyReq!! */
/*
LONG New_EasyRequestArgs(struct Window *win,
								 struct EasyStruct *es,
								 ULONG *idcmpptr,
								 APTR arglist)
{
	struct Window *req;
	LONG rc;

	D(bug("\nEASYREQUESTARGS:\n\n"));
	D(bug("Title: %s\n",es->es_Title));
	D(bug("Text : %s\n",es->es_TextFormat));
	D(bug("Gad  : %s\n",es->es_GadgetFormat));

	req = BuildEasyRequestArgs(win,
										es,
										idcmpptr ? *idcmpptr : 0,
										arglist);

	if (!req)
	{
		rc = 0;
	} else if (req == (struct Window *)1) {
		rc = 1;
	} else {
		while ((rc = SysReqHandler(req,idcmpptr,TRUE)) == -2)
		{
		}
		FreeSysRequest(req);
	}

	return rc;
}


#endif

#if 0*/

/* New_AutoRequest now in asmpatch.asm -
   copied directly from CyReq!! */
/*
BOOL New_AutoRequest(struct Window *win,
							struct IntuiText *bodytext,
							struct IntuiText *postext,
							struct IntuiText *negtext,
							ULONG posflags,
							ULONG negflags,
							WORD width,
							WORD height)
{
	struct Window *req;
	ULONG idcmp;
	BOOL rc;

	req = BuildSysRequest(win,
								 bodytext,
								 postext,
								 negtext,
								 posflags|negflags,
								 width,
								 height);

	if (!req)
	{
		rc = FALSE;
	} else if (req == (struct Window *)1) {
		rc = TRUE;
	} else {
		while ((rc = SysReqHandler(req,&idcmp,TRUE)) == -2)
		{
		}

		if (rc == -1)
		{
			rc = (idcmp & posflags) ? TRUE : FALSE;
		}
		FreeSysRequest(req);
	}

	return rc;
}

#endif*/

struct Window *New_BuildEasyRequestArgs(struct Window *win,
													struct EasyStruct *es,
													ULONG idcmp,
													APTR args)
{
	struct ReqNode *reqnode = 0;
	struct IntuiText *it;
	struct MyGadget *mygad;
	char c,*sp;
	APTR nextarg;
	LONG l,y;

	struct Window *rc = 0;

	D(bug("BuildEasyRequestArgs for: %lx and %lx (%s / %s)\n",es->es_Title,es->es_GadgetFormat,es->es_Title ? es->es_Title : "no title",es->es_GadgetFormat ? es->es_GadgetFormat : "no gadgetform"));

	if (!cxinactive) if ((reqnode = NewReqNode()))
	{
		if (win)
		{
			reqnode->scr = win->WScreen;
		} else {
			reqnode->scr = LockPubScreen(0);
			if (!reqnode->scr) goto fail;
		}

		reqnode->vp  = &reqnode->scr->ViewPort;
		reqnode->cm = reqnode->vp->ColorMap;
		reqnode->dri = GetScreenDrawInfo(reqnode->scr);
		reqnode->font = reqnode->dri->dri_Font;

		/* BODY */

		l = 0;
		RawDoFmt(es->es_TextFormat,
					args,
				   (APTR)GetLength_PutChFunc,
				   &l);

		D(bug("*** TextFormat length %ld\n",l));

		if (!(reqnode->bodytext = MiscAllocVec(l + 1))) goto fail;

#ifdef NO_ASM
    	    {
	    	STRPTR strptr = reqnode->bodytext;
		
		nextarg = RawDoFmt(es->es_TextFormat,
					 			 args,
					 			 (APTR)Copy_PutChFunc,
					 			 &strptr);
	    }
#else
		nextarg = RawDoFmt(es->es_TextFormat,
					 			 args,
					 			 (APTR)Copy_PutChFunc,
					 			 reqnode->bodytext);
#endif

		/* count lines and replace '\n' with '\0' */

		l = 1;
		sp = reqnode->bodytext;
		while((c = *sp++))
		{
			if (c == '\n')
			{
				l++;
			}
		}
		sp[-1] = '\n';

		/* create body intuitexts */

		if (!(reqnode->body = MiscAllocVec(l * sizeof(struct IntuiText)))) goto fail;

		reqnode->flags |= REQF_FREEBODY;

		it = reqnode->body;
		sp = reqnode->bodytext;

		y = reqnode->font->tf_YSize -
			 reqnode->font->tf_Baseline;

		if (y < 1) y = 1;
		y += reqnode->font->tf_YSize - 1;

		while(l--)
		{
			if (l > 0)
			{
				it->NextText = it + 1;
				it->NextText->TopEdge = it->TopEdge + y;
			}
			it->IText = sp;
			it->FrontPen = reqnode->dri->dri_Pens[TEXTPEN];
			it->DrawMode = JAM1;

			while(*sp++ != '\n');

			it++;
		}

		/* BUTTONS */

		l = 0;
		RawDoFmt(es->es_GadgetFormat,
					nextarg,
				   (APTR)GetLength_PutChFunc,
				   &l);

		if (!(reqnode->buttontext = MiscAllocVec(l + 1))) goto fail;

#ifdef NO_ASM
    	    {
	    	STRPTR strptr = reqnode->buttontext;
		
		RawDoFmt(es->es_GadgetFormat,
					nextarg,
					(APTR)Copy_PutChFunc,
					&strptr);
	    }
		
#else
		RawDoFmt(es->es_GadgetFormat,
					nextarg,
					(APTR)Copy_PutChFunc,
					reqnode->buttontext);
#endif
		/* count gadgets and replace '|' with '\0' */

		l = 1;
		sp = reqnode->buttontext;
		while((c = *sp++))
		{
			if (c == '|')
			{
				l++;
				sp[-1] = '\0';
			}
		}

		/* create mygadget structures */

		if (!(reqnode->buttongadgets = MiscAllocVec(l * sizeof(struct MyGadget)))) goto fail;

		reqnode->num_buttongadgets = l;

		mygad = reqnode->buttongadgets;
		sp = reqnode->buttontext;
		while(l--)
		{
			mygad->text = sp;
			while (*sp++);
			mygad++;
		}

		reqnode->title = es->es_Title ? (char *)es->es_Title :
												  win ? (char *)win->Title : 0;
		D(bug("BERA: Calling MakeMyRequester\n"));
		rc = MakeMyRequester(reqnode,win,idcmp);

	} /* if ((reqnode = NewReqNode())) */

fail:
	if (reqnode)
	{
		if (!win && reqnode->scr) UnlockPubScreen(0,reqnode->scr);
	}

	if (rc)
	{
		AddReqNode(reqnode);
	} else {
		if (reqnode) FreeReqNode(reqnode);
	}

	return rc;
}



struct Window *New_BuildSysRequest(struct Window *win,
											  struct IntuiText *bodytext,
											  struct IntuiText *postext,
											  struct IntuiText *negtext,
											  ULONG idcmpflags,
											  WORD width,
											  WORD height)
{
	struct ReqNode *reqnode = 0;
	struct Window *rc = 0;
	char *sp;
	LONG l;

	if (!negtext || !bodytext || cxinactive) goto fail;
	if (postext) if (postext->NextText) goto fail;
	if (negtext->NextText) goto fail;

	if ((reqnode = NewReqNode()))
	{

		if (win)
		{
			reqnode->scr = win->WScreen;
		} else {
			reqnode->scr = LockPubScreen(0);
			if (!reqnode->scr) goto fail;
		}

		reqnode->vp  = &reqnode->scr->ViewPort;
		reqnode->cm = reqnode->vp->ColorMap;
		reqnode->dri = GetScreenDrawInfo(reqnode->scr);
		reqnode->font = reqnode->dri->dri_Font;

		reqnode->body = bodytext;

		l = 0;
		while(bodytext)
		{
			if (bodytext->IText) l += 1 + strlen(bodytext->IText);
			bodytext = bodytext->NextText;
		};

		if (!(reqnode->bodytext = MiscAllocVec(l + 1))) goto fail;

		bodytext = reqnode->body;
		sp = reqnode->bodytext;

		while(bodytext)
		{
			if (bodytext->IText)
			{
				l = strlen(bodytext->IText);
				memcpy(sp,bodytext->IText,l);
				sp += l;
				*sp++ = '\n';
			}
			bodytext = bodytext->NextText;
		};

		reqnode->num_buttongadgets = postext ? 2 : 1;

		if (!(reqnode->buttongadgets = MiscAllocVec(reqnode->num_buttongadgets * sizeof(struct MyGadget)))) goto fail;

		if (postext)
		{
			reqnode->buttongadgets[0].text = postext->IText;
			reqnode->buttongadgets[1].text = negtext->IText;
		} else {
			reqnode->buttongadgets[0].text = negtext->IText;
		}

		reqnode->title = win ? (char *)win->Title : 0;

		rc = MakeMyRequester(reqnode,win,idcmpflags);

	} /* if ((reqnode = NewReqNode())) */


fail:
	if (reqnode)
	{
		if (!win && reqnode->scr) UnlockPubScreen(0,reqnode->scr);
	}

	if (rc)
	{
		AddReqNode(reqnode);
	} else {
		if (reqnode) FreeReqNode(reqnode);
	}

	return rc;
}


/*ULONG New_DOpus5Request(REG(d1,struct TagItem *,tags),
					REG(a1,struct Window *,window),REG(a2,ULONG *,callback),
					REG(a3,APTR,data))
{
	struct ReqNode *reqnode = 0;
	struct Window *rc = 0;
	struct TagItem *tag,*tag2,*tstate,*tstate2;

	struct IntuiText *it;
	char c,*sp,*bodytext;
	LONG l,y;
	ULONG res=0;

printf("doppatch called\n");

	if (!(FindTagItem(AR_Message,tags))) goto fail;
	if (!(FindTagItem(AR_Button,tags))) goto fail;

	if ((reqnode = NewReqNode()))
	{

		if (window)
		{
			reqnode->scr = window->WScreen;
		} else {
			reqnode->scr = LockPubScreen(0);
			if (!reqnode->scr) goto fail;
		}

		reqnode->vp  = &reqnode->scr->ViewPort;
		reqnode->cm = reqnode->vp->ColorMap;
		reqnode->dri = GetScreenDrawInfo(reqnode->scr);
		reqnode->font = reqnode->dri->dri_Font;

		tag = FindTagItem(AR_Message,tags);
		bodytext = (char *)tag->ti_Data;

		//body

		l = strlen(bodytext);
		if (!(reqnode->bodytext = MiscAllocVec(l + 1))) goto fail;
		CopyMem(bodytext,reqnode->bodytext,l+1);

		// count lines and replace '\n' with '\0' /

		l = 1;
		sp = reqnode->bodytext;
		while((c = *sp++))
		{
			if (c == '\n')
			{
				l++;
			}
		}
		sp[-1] = '\n';

		// create body intuitexts

		if (!(reqnode->body = MiscAllocVec(l * sizeof(struct IntuiText)))) goto fail;

		reqnode->flags |= REQF_FREEBODY;

		it = reqnode->body;
		sp = reqnode->bodytext;

		y = reqnode->font->tf_YSize -
			 reqnode->font->tf_Baseline;

		if (y < 1) y = 1;
		y += reqnode->font->tf_YSize - 1;

		while(l--)
		{
			if (l > 0)
			{
				it->NextText = it + 1;
				it->NextText->TopEdge = it->TopEdge + y;
			}
			it->IText = sp;
			it->FrontPen = reqnode->dri->dri_Pens[TEXTPEN];
			it->DrawMode = JAM1;

			while(*sp++ != '\n');

			it++;
		}

		// create buttons /

		l = 0; //# of buttons
		tstate = tags;
		while (tag = NextTagItem(&tstate))
		{
			if (tag->ti_Tag == AR_Button) l++;
		}
		//we already know that there is some defined buttons!

		// create mygadget structures /

		if (!(reqnode->buttongadgets = MiscAllocVec(l * sizeof(struct MyGadget)))) goto fail;
		if (!(reqnode->reqnodeext->buttoncodes = MiscAllocVec((l * 4)+4))) goto fail;

		reqnode->num_buttongadgets = l;

		tstate = tags;
		l = 0;
		while (tag = NextTagItem(&tstate))
		{
			if (tag->ti_Tag == AR_Button)
			{

				reqnode->buttongadgets[l].text = (char *)tag->ti_Data;
				reqnode->reqnodeext->buttoncodes[l] = l+1;
				if (l == (reqnode->num_buttongadgets-1)) reqnode->reqnodeext->buttoncodes[l]=0;

				//hunt for button codes...
				tstate2 = tstate;
				tag2 = NextTagItem(&tstate2);
				if (tag2->ti_Tag == AR_ButtonCode)
				{
					reqnode->reqnodeext->buttoncodes[l] = tag2->ti_Data;
				}

				l ++;
			}
		}

		tag = FindTagItem(AR_Title,tags);
		if (tag)
		{
			reqnode->title = (char *)tag->ti_Data;
		}

		if (reqnode->title)
		{
			if (strlen(reqnode->title)<1)
			{
				reqnode->title = defaultreqtitle;
			}
		} else {
			reqnode->title = defaultreqtitle;
		}

		rc = MakeMyRequester(reqnode,window,0);

	} //NewReqNode()

fail:
	if (reqnode)
	{
		if (!window && reqnode->scr) UnlockPubScreen(0,reqnode->scr);
	}

	if (rc)
	{
		AddReqNode(reqnode);
		while ((res = SysReqHandler(rc,0,TRUE)) == -2)
		{
		}
		FreeSysRequest(rc);

	} else {
		//if (reqnode->reqnodeext) Misc //let's just hope I won't forget it ;)
		if (reqnode) FreeReqNode(reqnode);
	}

	return res;
}*/


LONG New_FreeSysRequest(struct Window *win)
{
	struct ReqNode *node;
	struct MyGadget *mygad;
	WORD i,rc = 0;

	if (win)
	{
		if ((node = FindReqNode(win)))
		{
			RemoveReqNode(node);

			if (node->dri) FreeScreenDrawInfo(node->scr,node->dri);

			if (node->bodytext) MiscFreeVec(node->bodytext);
			if (node->body)
			{
				if (node->flags & REQF_FREEBODY) MiscFreeVec(node->body);
			}
			if (node->buttontext) MiscFreeVec(node->buttontext);

			if (node->buttongadgets)
			{
				mygad = node->buttongadgets;
				for (i = 0;i < node->num_buttongadgets;i++)
				{
					FreeMyGadget(node->win,mygad);
					FreeMyImage(&mygad->myim);
					mygad++;
				}
				MiscFreeVec(node->buttongadgets);
			}

			/*if (node->reqnodeext)
			{
				if (node->reqnodeext->extragadgets)
				{
					mygad = node->reqnodeext->extragadgets;
					for (i = 0;i < node->reqnodeext->numextragadgets;i++)
					{
						FreeMyGadget(node->win,mygad);
						FreeMyImage(&mygad->myim);
						mygad++;
					}
					MiscFreeVec(node->reqnodeext->extragadgets)
				}


				MiscFreeVec(node->reqnodeext)
			}*/

			if (node->draggad)
			{
				if (node->win) RemoveGadget(node->win,node->draggad);
				DisposeObject(node->draggad);
			}

			// free the App WB stuff
			if (node->appicon)
			{
				RemoveAppIcon(node->appicon);
			}

			if (node->appicondiskobject)
			{
				FreeDiskObject(node->appicondiskobject);
			}

			// remove and free the menu open gadget (VP)
			if (node->menugadget)
			{
				RemoveGList(node->win,node->menugadget,1);
				DisposeObject(node->menugadget);
			}

			if (node->menuimage)
			{
				DisposeObject(node->menuimage);
			}

			// remove and free the iconify gadget (VP)
			if (node->iconifygadget)
			{
				RemoveGList(node->win,node->iconifygadget,1);
				DisposeObject(node->iconifygadget);
			}

			if (node->iconifyimage)
			{
				DisposeObject(node->iconifyimage);
			}

#ifndef NO_POPUPMENU
			// free the popupmenu
			if (PopupMenuBase)
			{
				if (node->popupmenu)
				{
					PM_FreePopupMenu(node->popupmenu);
				}
			}
#endif

			FreeMyImage(&node->backfillimage);
			FreeMyImage(&node->textbackfillimage);
			FreeMyImage(&node->buttonbackfillimage);
			FreeMyImage(&node->reqimage);

			if (node->flags & REQF_LOGOANIMATED)
			{
				SendAnimMsg(ANIMM_STOP,0,&node->reqimage,0,0);
			}

				FreeMyRequesterPens(node);

			if (node->win)
			{

				if (node->flags & REQF_POPBACKSCREEN)
				{
					ScreenToBack(node->win->WScreen);
				}
				CloseWindow(node->win);
			}

#ifndef NO_SOUND
			if (node->soundhandle)
			{
				SendSoundMsg(node->bcfg.soundport,
							    SOUNDCMD_STOP,
								 0,
								 node->soundhandle,node);
			}
#endif
			if (node->fontok)
			{
				CloseFont(node->font);
			}

			if (node->buttonfontok)
			{
				CloseFont(node->buttonfont);
			}

			FreeReqNode(node);

			/* rc = 1 means it was a ReqAttack requester */

			rc = 1;
		} else {
			Old_FreeSysRequest(win);
		}
	} /* if (win) */

	return rc;
}

LONG New_SysReqHandler(struct Window *win,
							  ULONG *idcmpptr,
							  BOOL waitinput)
{
	struct ReqNode *reqnode;
	LONG rc;

	if ((reqnode = FindReqNode(win)))
	{
		if (reqnode->cfg.reqoff > -1)
		{
			rc = reqnode->cfg.reqoff;
		} else {
			rc = HandleMyRequester(reqnode,idcmpptr,waitinput);
		}
	} else {
		rc = Old_SysReqHandler(win,idcmpptr,waitinput);
	}

	return rc;
}
// DOpus Patch ©2001 Jaca/Dreamolers-CleverAmigantsPolishSociety //
/*ULONG New_DOpus(REG(a0,struct Window *,window),REG(a1,struct DOpusData *,dpdta))
{
	ULONG rc=-3,gadmemlen=0;
	UBYTE gad=0;
	struct EasyStruct *st;
	char *gadgets=0;

	if (dpdta->string) return -3; //rename or other requester - not supported :(

	while (dpdta->buttons[gad])
	{
		gadmemlen += (strlen((char *)dpdta->buttons[gad]) + 2);
		gad++;
	}

	if (gadmemlen)
	{
		gadgets = AllocVec(gadmemlen,MEMF_PUBLIC+MEMF_CLEAR);
		if (!(gadgets)) return -3;

		gad=0;

		while (dpdta->buttons[gad])
		{
			strcat(gadgets,(char *)dpdta->buttons[gad]);
			if (dpdta->buttons[gad+1]) strcat(gadgets,"|");
			gad++;
		}
	} else { return -3;} //reqs without gadgets NOT supported

	st = AllocVec((sizeof(struct EasyStruct)),MEMF_PUBLIC+MEMF_CLEAR);

	if (st)
	{
		st->es_StructSize = sizeof(struct EasyStruct);
		st->es_Title = dpdta->title;
		st->es_TextFormat = dpdta->text;
		st->es_GadgetFormat = gadgets;
		rc=EasyRequestArgs(window,st,NULL,NULL);
		FreeVec(st)
	}

	FreeVec(gadgets);

	return rc;
}*/

// ReqTools Patch ©2001 Jaca/Dreamolers-CleverAmigantsPolishSociety //
ULONG New_ReqTools(REG(a0,struct TagItem *,taglist),
						REG(a1,char *,bodyfmt),REG(a2,char *,gadfmt),
						REG(a3,ULONG *,reqinfo),REG(a4,ULONG *,argarray))
{
	ULONG rc=-3,idcmp;
	struct EasyStruct *st;
	struct Window *win=0;
	struct Screen *sc=0;
	struct Process *proc;
	char *gadformat=0,*gf,*gf2;
	BOOL closew=FALSE;

	D(bug("ReqTools patch!\n"));

	if (gadfmt == 0) return -3; //kein gadgets not allowed ;)
	if (!(gadformat = AllocVec(strlen(gadfmt)+8,MEMF_PUBLIC+MEMF_CLEAR))) return -3;
	if ((GetTagData(RT_ReqHandler,0,taglist)) != 0) return -3; //asynch reqs not supported :(

	rc = GetTagData(RT_PubScrName,0,taglist);
	if (rc)
	{
		sc = LockPubScreen((char*)rc);
		if (sc)
		{
			win = OpenWindowTags(0,WA_PubScreen,(ULONG)sc,WA_Left,0,WA_Top,0,WA_Width,1,WA_Height,1,WA_Flags,WFLG_BACKDROP|WFLG_BORDERLESS,TAG_DONE);
			closew=TRUE;
			UnlockPubScreen(NULL,sc);
		}
	}

	if (!(win))
	{
		sc = (struct Screen *)GetTagData(RT_Screen,0,taglist);
		if (sc)
		{
			win = OpenWindowTags(0,WA_PubScreen,(ULONG)sc,WA_Left,0,WA_Top,0,WA_Width,1,WA_Height,1,WA_Flags,WFLG_BACKDROP|WFLG_BORDERLESS,TAG_DONE);
			closew=TRUE;
		}
	}

	if(!(win)) win = (struct Window *)GetTagData(RT_Window,0,taglist);

	if (!(win)) //if no window -> try pr_WindowPtr
	{
		proc = (struct Process *)FindTask(0);
		if (proc->pr_Task.tc_Node.ln_Type == NT_PROCESS)
		win = (proc->pr_WindowPtr);
		if (win == (struct Window *)-1) win = NULL;
	}

	rc = -3;

	//prepare the gadformat - new in 1.60
	gf = gadfmt;gf2 = gadformat;
	while(gf[0])
	{
		gf2[0] = gf[0];
		if (gf[0]==0x5F) gf2--;
		gf2++;gf++;
	}
	gf2[0]=0;	//end the string...

	idcmp = GetTagData(RT_IDCMPFlags,0,taglist);

	st = AllocVec((sizeof(struct EasyStruct)),MEMF_PUBLIC+MEMF_CLEAR);

	if (st)
	{
		st->es_StructSize = sizeof(struct EasyStruct);
		st->es_Title = (UBYTE *)GetTagData(RTEZ_ReqTitle,0,taglist);
		st->es_TextFormat = (UBYTE *)bodyfmt;
		st->es_GadgetFormat = (UBYTE *)gadformat;
		if (idcmp)
		{
			rc=EasyRequestArgs(win,st,&idcmp,argarray);
		} else {
			rc=EasyRequestArgs(win,st,NULL,argarray);
		}
		FreeVec(st);
	}
	if (gadformat) FreeVec(gadformat);
	if (closew) if (win) CloseWindow(win);

	return rc;
}



