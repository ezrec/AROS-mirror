/*
 * @(#) $Header$
 *
 * BGUI Tree List View class
 *
 * (C) Copyright 1999 Manuel Lemos.
 * (C) Copyright 1996-1999 Nick Christie.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:48  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:34  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:32  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.3  1999/05/31 01:14:43  mlemos
 * Fixed bug of trying to access the last clicked entry when was not set.
 *
 * Revision 1.1.2.2  1999/05/24 23:58:36  mlemos
 * Fixed bug of attempting to notifying a change in NULL tree node pointer.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:47  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
******************  TREEVIEW CLASS: LISTVIEW HANDLERS  ******************
************************************************************************/


/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
************************************************************************/


/************************************************************************
*************************  EXTERNAL REFERENCES  *************************
************************************************************************/

/*
 * Functions from TVUtil are listed in TVUtil.h
 */

/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

//ASM SAVEDS ULONG TV_LVRsrcHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvResource *lvr);
ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVRsrcHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvResource *, lvr));

//ASM SAVEDS ULONG TV_LVDispHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvRender *lvr);
ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVDispHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvRender *, lvr));

//ASM SAVEDS ULONG TV_LVCompHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvCompare *lvc);
ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVCompHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvCompare *, lvc));

//ASM SAVEDS ULONG TV_LVNotifyHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct opUpdate *opu);
ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVNotifyHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/

/*
 * Default DrawInfo pens
 */

LOCAL UWORD DefDriPens[ 12 ] = { 0, 1, 1, 2, 1, 3, 1, 0, 2, 1, 2, 1 };

/*
 * Pattern used for ghosting disabled entries
 */

LOCAL UWORD GhostPattern[] = { 0x2222, 0x8888 };

/************************************************************************
*************************  TV_LVRSRCHANDLER()  **************************
*************************************************************************
* Resource handler for embedded listview. We add TreeNodes to this
* listview and we don't want them copied or anything, so this handler
* just tells the listview to add the TreeNode ptr itself. When removing
* entries, there is similarly nothing special to do.
*
*************************************************************************/

//ASM SAVEDS ULONG TV_LVRsrcHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvResource *lvr)
ASM SAVEDS REGFUNC3(ULONG, TV_LVRsrcHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvResource *, lvr))
{
if (lvr->lvr_Command == LVRC_MAKE)
	return((ULONG) lvr->lvr_Entry);
else
	return(0);
}
REGFUNC_END

/************************************************************************
*************************  TV_LVDISPHANDLER()  **************************
*************************************************************************
* Display handler for embedded listview. The members of the list are
* TreeNodes, so the display must take into account their depth in the
* hierarchy (represented by indentation), and whether their children
* are expanded out (render a boxed plus sign or minus sign to the
* left). The last item to render is the treenode's label, this will
* need to be passed to the user's custom display hook (if any).
*
* STUB! Doesn't use custom display hook yet.
*
*************************************************************************/

//ASM SAVEDS ULONG TV_LVDispHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvRender *lvr)
ASM SAVEDS REGFUNC3(ULONG, TV_LVDispHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvRender *, lvr))
{
TVData				*tv;
struct RastPort		*rp;
struct Rectangle	*rect;
TNPTR				tn;
struct Image		*img;
WORD				imgwid,imghgt;
struct IBox			box;
struct TextExtent	te;
STRPTR				text;
ULONG				len;
ULONG				imgstate;
ULONG				depth;
UWORD				fpen,bpen;
UWORD				*dripens;
BOOL				disabled;

tv = (TVData *) hook->h_Data;
tn = (TNPTR) lvr->lvr_Entry;
rp = lvr->lvr_RPort;
text = tn->tn_Entry;
rect = &lvr->lvr_Bounds;
disabled = FALSE;
depth = TV_TreeNodeDepth(tn) - 1;

/* debug
KPrintF("Render 1: Node:%08lx   Name:%s  L:%ld  T:%ld  R:%ld  B:%ld\n",
		tn, tn->tn_Entry, rect->MinX, rect->MinY, rect->MaxX, rect->MaxY);
*/

/*
 * Determine pens to use
 */

dripens = lvr->lvr_DrawInfo ? lvr->lvr_DrawInfo->dri_Pens : DefDriPens;

switch(lvr->lvr_State)
	{
	case LVRS_SELECTED:
		fpen = TEXTPEN;
		bpen = FILLPEN;
		imgstate = IDS_SELECTED;
		break;

	case LVRS_NORMAL_DISABLED:
		fpen = TEXTPEN;
		bpen = BACKGROUNDPEN;
		imgstate = IDS_DISABLED;
		disabled = TRUE;
		break;

	case LVRS_SELECTED_DISABLED:
		fpen = TEXTPEN;
		bpen = FILLPEN;
		imgstate = IDS_DISABLED;
		disabled = TRUE;
		break;

	/* case LVRS_NORMAL: */
	default:
		fpen = TEXTPEN;
		bpen = BACKGROUNDPEN;
		imgstate = IDS_NORMAL;
		break;

	} /* endswitch lvr_State */

fpen = dripens[fpen];
bpen = dripens[bpen];

/*
 * Prefill the area with the background pen
 */

SetAPen(rp,bpen);
SetDrMd(rp,JAM1);
RectFill(rp,rect->MinX,rect->MinY,rect->MaxX,rect->MaxY);

/*
 * Leave appropriate indentation based on node's depth in tree,
 * unless TVA_LeftAlignImage has been set.
 */

box.Left = rect->MinX + 2;
if (!tv->tv_LeftAlignImage)
	box.Left += depth * tv->tv_Indentation;
box.Top = rect->MinY;
box.Width = rect->MaxX - box.Left + 1;
box.Height = rect->MaxY - rect->MinY + 1;

/*
 * Render the node image for this item, if there's space.
 * If TVA_NoLeafImage is set, don't draw an image for
 * leaf nodes.
 */

if (tv->tv_NoLeafImage && !HasChildren(tn))
	{
	img = NULL;
	imgwid = ((IMGPTR) tv->tv_ExpandedImage)->Width;
	imghgt = ((IMGPTR) tv->tv_ExpandedImage)->Height;
	}
else
	{
	img = (IMGPTR) (IsExpanded(tn) ?
		tv->tv_ExpandedImage : tv->tv_ContractedImage);
	imgwid = img->Width;
	imghgt = img->Height;
	}

if (imgwid <= box.Width)
	{
	if (img && (imghgt <= box.Height))
		{
		UWORD	y;

		y = box.Top + (box.Height - imghgt) / 2;
		DrawImageState(rp,img,box.Left,y,imgstate,lvr->lvr_DrawInfo);
		}

	if (	tv->tv_GoingActive && img &&
			(tv->tv_LastClickX >= box.Left) &&
			(tv->tv_LastClickX <= box.Left + imgwid - 1) &&
			(tv->tv_LastClickY >= box.Top) &&
			(tv->tv_LastClickY <= box.Top + box.Height - 1)
			)
		{
		tv->tv_ImageClicked = tn;

		/* debug
		KPrintF("Render 2: ImageClicked: Node:%08lx   Name:%s\n",
			tn, tn->tn_Entry);
		*/
		}

	/*
	 * If TVA_LeftAlignImage was set, put indentation in now,
	 * after the image
	 */

	if (tv->tv_LeftAlignImage)
		{
		box.Left += depth * tv->tv_Indentation;
		box.Width = rect->MaxX - box.Left + 1;
		}

	/*
	 * Determine if we need to draw connecting lines,
	 * and in what style, from tv_LineStyle. If the
	 * image is left-aligned, we don't draw lines.
	 */

	if ((tv->tv_LineStyle != TVLS_NONE) && !tv->tv_LeftAlignImage)
		{
		TNPTR	pn,pn2;
		UWORD	x,y;

		SetAPen(rp,dripens[TEXTPEN]);
		SetDrMd(rp,JAM1);

		switch(tv->tv_LineStyle)
			{
			case TVLS_DOTS:
				SetDrPt(rp,0xaaaa);
				break;

			case TVLS_DASH:
				SetDrPt(rp,0xcccc);
				break;

			case TVLS_SOLID:
			default:
				SetDrPt(rp,0xffff);
				break;
			}

		/*
		 * If entry has children, draw small vertical below image
		 */

		if (IsExpanded(tn) && HasChildren(tn))
			{
			x = box.Left + (imgwid / 2) - 1;
			y = box.Top + imghgt + (box.Height - imghgt) / 2;
			Move(rp,x,y);
			Draw(rp,x,box.Top + box.Height - 1);
			}

		/*
		 * If depth > 0, draw horizontal to left of image.
		 * If img is NULL then this was an image-less leaf
		 * node and we should start just to the left of the
		 * label, not the left of the (nonexistent) image.
		 */

		if (depth > 0)
			{
			y = box.Top + (box.Height / 2);
			x = box.Left - 1;
			Move(rp,x + (img ? 0 : imgwid),y);
			x -= tv->tv_Indentation - (imgwid / 2);
			Draw(rp,x,y);

			/*
			 * If entry has next sibling, draw full height vertical
			 * at lefthand point of horizontal line, otherwise just
			 * draw half-height vertical.
			 */

			Move(rp,x,box.Top);
			Draw(rp,x,NextSiblingOf(tn) ? box.Top + box.Height - 1 : y);

			/*
			 * For each further parent with a next sibling,
			 * draw a full height vertical line.
			 */

			pn = ParentOf(tn);

			while(pn2 = ParentOf(pn))
				{
				x -= tv->tv_Indentation;

				if (NextSiblingOf(pn))
					{
					Move(rp,x,box.Top);
					Draw(rp,x,box.Top + box.Height - 1);
					}

				pn = pn2;
				}
			}

		/*
		 * Reset rastport to solid line pattern
		 */

		SetDrPt(rp,0xffff);
		}

	box.Left += imgwid + rp->TxWidth;
	box.Width -= imgwid + rp->TxWidth;

	SetAPen(rp,fpen);
	SetDrMd(rp,JAM1);
	Move(rp,box.Left,box.Top + rp->Font->tf_Baseline);
	len = TextFit(rp,text,strlen(text),&te,NULL,1,box.Width,32767);
	Text(rp,text,len);
	}

return(0);
}
REGFUNC_END

/************************************************************************
*************************  TV_LVCOMPHANDLER()  **************************
*************************************************************************
* Compare handler for embedded listview. Sorts entries by parents
* before sorting by individual entry.
*
*************************************************************************/

//ASM SAVEDS ULONG TV_LVCompHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvCompare *lvc)
ASM SAVEDS REGFUNC3(ULONG, TV_LVCompHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvCompare *, lvc))
{
struct tvCompare	tvc;
TVData				*tv;
TNPTR				tna,tnb;
ULONG				da,db;

tv = (TVData *) hook->h_Data;
tna = (TNPTR) lvc->lvc_EntryA;
tnb = (TNPTR) lvc->lvc_EntryB;
da = TV_TreeNodeDepth(tna);
db = TV_TreeNodeDepth(tnb);

while(db < da)
	{ tna = ParentOf(tna); da--; }

while(da < db)
	{ tnb = ParentOf(tnb); db--; }

tvc.tvc_EntryA = tna->tn_Entry;
tvc.tvc_EntryB = tnb->tn_Entry;

return(CallHookPkt(tv->tv_CompareHook,tv->tv_TreeView,&tvc));
}
REGFUNC_END

/************************************************************************
************************  TV_LVNOTIFYHANDLER()  *************************
*************************************************************************
* Notification hook added to embedded listview: called when the currently
* selected item in the listview changes. If the user clicked on the
* tree image for an entry, and the released the LMB while the mouse
* pointer was over that entry, we expand or contract it. Same applies
* if the user has just double-clicked on an entry.
*
* This relies on the fact that our TV_GoActive method has determined
* the initial position of the mousepointer, and our TV_LVDispHandler
* hook has set tv_ImageClicked to point at the clicked entry, if the
* user clicked on the tree image, rather than the entry itself.
*
*************************************************************************/

//ASM SAVEDS ULONG TV_LVNotifyHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct opUpdate *opu)
ASM SAVEDS REGFUNC3(ULONG, TV_LVNotifyHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu))
{
struct TagItem	*tag;
TVData			*tv;
TNPTR			tn,pn;
ULONG			time[2];
ULONG			rc;

tv = (TVData *) hook->h_Data;
rc = 0;

/*
 * Find the LISTV_Entry tag in the notification taglist, and verify
 * that this notification is a final one, not an interim update.
 */

if (tag = FindTagItem(LISTV_Entry,opu->opu_AttrList))
	{
	tn = (TNPTR) tag->ti_Data;

	if (!(opu->opu_Flags & OPUF_INTERIM))
		{
		/*
		 * User activated gadget with LMB down on a tree image,
		 * and released the LMB with the pointer over that entry?
		 */

		if (tv->tv_ImageClicked && (tn == tv->tv_ImageClicked))
			{
			/*
			 * Yes: expand/contract this entry
			 */

			tv->tv_LastClickTime[0] = tv->tv_LastClickTime[1] = 0;
			rc = 1;

			/* debug
			KPrintF("LVNotifyHandler: Image click expand node: %s\n",
				tv->tv_ImageClicked->tn_Entry);
			*/
			}
		else
			{
			/*
			 * User double-clicked on entry? And it's not an
			 * image-less leaf node?
			 */

			CurrentTime(&time[0],&time[1]);

			if (tn
					&& (tn == tv->tv_LastClicked)
					&& (!tv->tv_NoLeafImage || HasChildren(tn))
					&& (DoubleClick(tv->tv_LastClickTime[0],tv->tv_LastClickTime[1],
						time[0],time[1])))
				{
				/*
				 * Yes: expand/contract this entry
				 */

				tv->tv_LastClickTime[0] = tv->tv_LastClickTime[1] = 0;
				rc = 1;

				/* debug
				KPrintF("LVNotifyHandler: Double click expand node: %s\n",
					tn->tn_Entry);
				*/
				}
			else
				{
				tv->tv_LastClickTime[0] = time[0];
				tv->tv_LastClickTime[1] = time[1];
				}
			}

		if (tv->tv_LastClicked)
			tv->tv_LastClicked->tn_Flags &= ~TNF_SELECTED;

		if (rc)		/* expand/contract entry */
			{
			rc = BGUI_DoGadgetMethod(tv->tv_TreeView,
				opu->opu_GInfo ? opu->opu_GInfo->gi_Window : NULL,
				NULL, TVM_EXPAND, NULL, tv->tv_LastClicked,
				TVW_ENTRY, TVF_TOGGLE|TVF_USER_ACTION|TVF_INTERNAL);
			}
		}

		if((tv->tv_LastClicked = tn))
		{
			tn->tn_Flags |= TNF_SELECTED;
			pn = ParentOf(tn);

			TV_NotifyAttrChange(tv->tv_TreeView,opu->opu_GInfo,opu->opu_Flags,
				TVA_Entry,		tn->tn_Entry,
				TVA_Parent,		pn ? pn->tn_Entry : NULL,
				TVA_Moved,		FALSE,
				TAG_DONE);
		}
	}

return(rc);
}
REGFUNC_END

