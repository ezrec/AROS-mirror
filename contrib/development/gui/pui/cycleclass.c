#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "cycleclass.h"

ULONG CycleInstanceSize(void)
{
	return sizeof(struct cycledata);
}

static void drawuparrow(struct cycledata *data,struct PUI_AreaData *adata,BOOL force)
{
	WORD x;
	BOOL black;
	
	black = data->top > 0 ? TRUE : FALSE;
	if (force || (black != data->uparrowblack))
	{
		data->uparrowblack = black;
		
		SetAPen(data->rp,adata->ri->pens[black ? SHADOWPEN : BACKGROUNDPEN]);

		x = data->borderleft +
			 (data->itemwidth - ARROWWIDTH) / 2;
	
		DrawArrow(data->rp,data->menuleft + x,data->menutop + data->menuy1 - ARROWHEIGHT - 1,ARROW_UP);
	}
}

static void drawdownarrow(struct cycledata *data,struct PUI_AreaData *adata,BOOL force)
{
	WORD x;
	BOOL black;

	black = data->top + data->visible < data->numitems ? TRUE : FALSE;
	if (force || (black != data->downarrowblack))
	{
		data->downarrowblack = black;
		
		SetAPen(data->rp,adata->ri->pens[black ? SHADOWPEN : BACKGROUNDPEN]);
	
		x = data->borderleft +
			 (data->itemwidth - ARROWWIDTH) / 2;
	
		DrawArrow(data->rp,data->menuleft + x,data->menutop + data->menuy2 + 2,ARROW_DOWN);
	}
}


static void drawitem(Object *obj,struct cycledata *data,WORD num,BOOL nowhitefill)
{
	struct PUI_AreaData	*adata;
	struct cycleitem	*item;
	WORD					off,x,y;

	adata = INST_DATA(areaclass,obj);

	off = num - data->top;

	if ((off >= 0) && (off < data->visible))
	{
		item = &data->itemmemory[num];
		
		x = data->menuleft + data->menux1;
		y = data->menutop  + data->menuy1 + off * data->itemheight;
		
		if ((num == data->selected) || !nowhitefill)
		{
			SetAPen(data->rp,adata->ri->pens[num == data->selected ? SHADOWPEN : SHINEPEN]);
			RectFill(data->rp,x,y,
									x + data->itemwidth - 1,
									y + data->itemheight - 1);
		}
		
		x = (x + x + data->itemwidth - item->pixellen) / 2;
		y += adata->ri->fontbaseline + 1;
		
		SetAPen(data->rp,adata->ri->pens[num == data->selected ? SHINEPEN : TEXTPEN]);
		Move(data->rp,x,y);
		Text(data->rp,item->string,item->charlen);

	} /* if ((off >= 0) && (off < data->visible)) */
}

static void drawmenu(Object *obj,struct cycledata *data)
{
	struct PUI_AreaData *adata;
	struct IBox		 box;
	WORD				 i;

	adata = INST_DATA(areaclass,obj);

	box.Left = data->menuleft;
	box.Top  = data->menutop;

	box.Width  = data->menuwidth;
	box.Height = data->menuheight;

	DrawFrame(obj,data->rp,&box,PUIV_Frame_Cycle,FALSE);
	
	SetAPen (data->rp,adata->ri->pens[SHINEPEN]);
	RectFill(data->rp,box.Left + frameinfo[PUIV_Frame_Cycle].borderleft,
							box.Top  + frameinfo[PUIV_Frame_Cycle].bordertop,
							box.Left + box.Width  - 1 - frameinfo[PUIV_Frame_Cycle].borderright,
							box.Top  + box.Height - 1 - frameinfo[PUIV_Frame_Cycle].borderbottom);
							
	for(i = data->top;i < data->top + data->visible;i++)
	{
		drawitem(obj,data,i,TRUE);
	}

	drawuparrow(data,adata,TRUE);
	drawdownarrow(data,adata,TRUE);
}

static void unselectactiveitem(Object *obj,struct cycledata *data)
{
	WORD old;

	old = data->selected;
	data->selected = -1;
	drawitem(obj,data,old,FALSE);
}

ULONG CycleDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct cycledata	*data;
	struct PUI_AreaData	*adata;
	struct cycleitem	*item;
	struct RastPort	*rp;
	LONG					i,len,x,y,x2,y2,a1,a2;
	BOOL					selected;
	
	ULONG rc;
	
	rc = 0;

	if (msg->MethodID != OM_NEW)
	{
		data  = INST_DATA(cl,obj);
		adata = INST_DATA(areaclass,obj);
	}
	
	switch(msg->MethodID)
	{
		case OM_NEW:
			if ((obj = (Object *)DoSuperMethodA(cl,obj,msg)))
			{
				data = INST_DATA(cl,obj);
				
				data->entries = (char **)GetTagData(PUIA_Cycle_Entries,0,ATTRLIST);
				data->active  = GetTagData(PUIA_Cycle_Active,0,ATTRLIST);
				data->winmap = 0;
				NewList((struct List *)&data->itemlist);

				// count items
				for(i = 0;data->entries[i];i++);
				data->numitems = i;

				if ((data->itemmemory = AllocVec(i * sizeof(struct cycleitem),MEMF_PUBLIC | MEMF_CLEAR)))
				{
					item = data->itemmemory;

					for(i = 0;i < data->numitems;i++,item++)
					{
						item->string = data->entries[i];
						item->charlen = strlen(item->string);
					}

					rc = (ULONG)obj;
				} else {
				
					CoerceMethod(cl,obj,OM_DISPOSE);
					
				}
			};
			break;
		
		case OM_DISPOSE:
			if (data->itemmemory) FreeVec(data->itemmemory);
			DoSuperMethodA(cl,obj,msg);
			break;
		
		case PUIM_SETUP:
			DoSuperMethodA(cl,obj,msg);
			
			item = data->itemmemory;
			len = 0;

			for (i = 0;i < data->numitems;i++,item++)
			{
				MyTextExtent(adata->ri->font,item->string,item->charlen,0,&item->pixellen,0);
				if (item->pixellen > len) len = item->pixellen;	
			}

			data->itemwidth  = len + 2;
			x = GAD->Width - CYCLEIMAGEWIDTH - frameinfo[adata->frametype].spaceright;
			if (data->itemwidth < x) data->itemwidth = x;

			data->itemheight = adata->ri->fontheight + 2;

			data->borderleft   = frameinfo[PUIV_Frame_Cycle].borderleft +
										frameinfo[PUIV_Frame_Cycle].spaceleft;
			data->borderright  = frameinfo[PUIV_Frame_Cycle].borderright +
									   frameinfo[PUIV_Frame_Cycle].spaceright;
			data->bordertop    = frameinfo[PUIV_Frame_Cycle].bordertop +
										frameinfo[PUIV_Frame_Cycle].spacetop;
			data->borderbottom = frameinfo[PUIV_Frame_Cycle].borderbottom +
										frameinfo[PUIV_Frame_Cycle].spacebottom;
										
			data->menuwidth = data->itemwidth +
									data->borderleft +
									data->borderright;

			data->menuheight = data->numitems * data->itemheight +
									 data->bordertop +
									 data->borderbottom;

			data->menux1 = data->borderleft;

			data->maypopup = TRUE;

			if (data->menuheight > adata->ri->scrheight)
			{
				data->visible = (adata->ri->scrheight -
					  				  data->bordertop -
					  				  data->borderbottom -
					  				  (ARROWHEIGHT + 2) * 2) / data->itemheight;

				if (data->visible < 2)
				{
					data->maypopup = FALSE;
				} else {
					data->menuheight = data->bordertop +
											 data->visible * data->itemheight +
											 data->borderbottom +
											 (ARROWHEIGHT + 2) * 2;

					data->menuy1 = data->bordertop + ARROWHEIGHT + 2;
				}
			} else {
				data->visible = data->numitems;
				data->menuy1 = data->bordertop;
			}
			data->menux2 = data->menux1 + data->itemwidth - 1;
			data->menuy2 = data->menuy1 + data->visible * data->itemheight - 1;
			break;
			
		case PUIM_MINMAX:
			DoSuperMethodA(cl,obj,msg);

			x = data->itemwidth + 
				 frameinfo[adata->frametype].spaceright +
				 CYCLEIMAGEWIDTH -
				 adata->offsetr;	// +- = 0
			y = adata->ri->fontheight;

			x2 = PUI_MAXMAX;
			
			MINMAXMSG->minmax->minwidth  += x;
			MINMAXMSG->minmax->defwidth  += x;
			MINMAXMSG->minmax->maxwidth  += x2;

			MINMAXMSG->minmax->minheight += y;
			MINMAXMSG->minmax->defheight += y;
			MINMAXMSG->minmax->maxheight += y;
			break;
		
		case GM_RENDER:
			if ((rp = RENDERMSG->gpr_RPort) && PUI_MayRender(obj))
			{
				DoSuperMethodA(cl,obj,msg);

				selected = GAD->Flags & GFLG_SELECTED ? TRUE: FALSE;
				SetABPenDrMd(rp,adata->ri->pens[selected ? FILLTEXTPEN : TEXTPEN],0,JAM1);
				
				item = &data->itemmemory[data->active];

				a1 = GAD->LeftEdge + adata->offsetl;
				a2 = GAD->LeftEdge + GAD->Width -
					  CYCLEIMAGEWIDTH -
					  frameinfo[adata->frametype].spaceright;
				x  = (a1 + a2 - item->pixellen) / 2;
				
				a1 = GAD->TopEdge + adata->offseto;
				a2 = GAD->TopEdge + GAD->Height - adata->offsetu;
				y  = (a1 + a2 - adata->ri->fontheight) / 2 + adata->ri->fontbaseline;
				
				Move(rp,x,y);
				Text(rp,item->string,item->charlen);

				x = GAD->LeftEdge + GAD->Width - CYCLEIMAGEWIDTH;

				y = GAD->TopEdge +
					 frameinfo[adata->frametype].bordertop +
					 1;

				y2 = GAD->TopEdge +
					  GAD->Height -
					  frameinfo[adata->frametype].borderbottom -
					  2;

				// separator bar
				SetAPen(rp,adata->ri->pens[SHINEPEN]);
				RectFill(rp,x+1,y,x+1,y2);
				SetAPen(rp,adata->ri->pens[SHADOWPEN]);
				RectFill(rp,x,y,x,y2);

				y = (a1 + a2 - 4) / 2;
				x += 6;

				DrawArrow(rp,x,y,ARROW_DOWN);

			} /* if ((rp = RENDERMSG->gpr_RPort) && PUI_MayRender(obj)) */
			break;
		
		case GM_GOACTIVE:
			rc = GMR_MEACTIVE;
			data->popup = FALSE;
			data->sentgadgetup = FALSE;
			data->turbocountdown = TURBOCOUNTDOWN;

			if (data->maypopup)
			{
				x = INPUTMSG->gpi_GInfo->gi_Window->MouseX;
	
				if (x < GAD->LeftEdge + GAD->Width - CYCLEIMAGEWIDTH)
				{
					x = INPUTMSG->gpi_GInfo->gi_Window->LeftEdge +
						 GAD->LeftEdge;

					y = INPUTMSG->gpi_GInfo->gi_Window->TopEdge +
						 GAD->TopEdge + GAD->Height;
					
					if (x < 0) x = 0;
					if ((x + data->menuwidth) > adata->ri->scrwidth)
					{
						x = adata->ri->scrwidth - data->menuwidth;
					}

					if (y < 0) y = 0;
					if ((y + data->menuheight) > adata->ri->scrheight)
					{
						y = adata->ri->scrheight - data->menuheight;
					}

					x2 = x + data->menuwidth - 1;
					y2 = y + data->menuheight - 1;

					data->winmap = 0;
					
					if (!popup_usewindow)
					{
						if ((data->winmap =
							 (struct BitMap *)AllocBitMap(data->menuwidth,
																	data->menuheight,
																	GetBitMapAttr(adata->ri->bm,BMA_DEPTH),
																	BMF_MINPLANES,
																	adata->ri->bm)))
						{
							data->menuleft = x;
							data->menutop = y;
							// clone screen rastport
							data->rp = &data->clonerp;
							data->clonerp = adata->ri->scr->RastPort;

							BltBitMap(adata->ri->bm,x,y,
										 (struct BitMap *)data->winmap,0,0,
										 data->menuwidth,
										 data->menuheight,
										 192,
										 255,
										 0);

							LockLayers(adata->ri->li);

							data->winmaptype = WINMAP_BITMAP;
						}
					}
					
					if (!data->winmap)
					{
						if ((data->winmap = 
							 (struct Window *)OpenWindowTags(0,WA_PubScreen,adata->ri->scr,
																		  WA_Left,x,
																 		  WA_Top,y,
																		  WA_Width,data->menuwidth,
																		  WA_Height,data->menuheight,
																		  WA_Flags,WFLG_BORDERLESS,
																		  WA_SimpleRefresh,TRUE,
																		  WA_BackFill,LAYERS_NOBACKFILL,
																 		  TAG_DONE)))
						{
							data->menuleft = 0;
							data->menutop = 0;
							data->rp = ((struct Window *)data->winmap)->RPort;

							data->winmaptype = WINMAP_WINDOW;
						}
					}
					
					if (data->winmap)
					{
						data->layerx1 = x;data->layery1 = y;
						data->layerx2 = x2;data->layery2 = y2;
						
						data->top = 0;
						data->selected = -1;
						SetDrMd(data->rp,JAM1);
						SetFont(data->rp,adata->ri->font);
						SetWriteMask(data->rp,-1);

						data->popup = TRUE;
						drawmenu(obj,data);
					}

				} /* if (x < GAD->LeftEdge + GAD->Width - CYCLEIMAGEWIDTH) */
				
			} /* if (data->maypopup) */
	    	#ifndef _AROS
			break;
		#endif
		
		case GM_HANDLEINPUT:
			if (data->popup)
			{
				if ((INPUTMSG->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE) &&
					 (INPUTMSG->gpi_IEvent->ie_Code == (IECODE_LBUTTON | IECODE_UP_PREFIX)))
				{
					rc = GMR_NOREUSE;
					if (data->selected != -1)
					{
						data->active = data->selected;
						data->sentgadgetup = TRUE;

						rc |= GMR_VERIFY;
						*INPUTMSG->gpi_Termination = data->active;
					}
					break;	// go out of switch(msg->MethodID)

				} else {
					rc = GMR_MEACTIVE;
				}

				x = INPUTMSG->gpi_GInfo->gi_Screen->MouseX -
					 data->layerx1;
				y = INPUTMSG->gpi_GInfo->gi_Screen->MouseY -
					 data->layery1;
				
				if ((x < data->menux1) || (x > data->menux2))
				{
					unselectactiveitem(obj,data);
				} else {
					i = (y - data->menuy1) / data->itemheight;

					if ((i >= data->visible) || (y < data->menuy1))
					{
						unselectactiveitem(obj,data);
						
						if (data->turbocountdown && (INPUTMSG->gpi_IEvent->ie_Class == IECLASS_TIMER))
						{
							data->turbocountdown--;
						}
						
						if (!data->turbocountdown || (INPUTMSG->gpi_IEvent->ie_Class == IECLASS_TIMER))
						{
							if (i >= data->visible)
							{
								// scroll down 
								if (data->top + data->visible < data->numitems)
								{
									ClipBlit(data->rp,data->menux1 + data->menuleft,data->menuy1 + data->menutop + data->itemheight,
												data->rp,data->menux1 + data->menuleft,data->menuy1 + data->menutop,data->itemwidth,data->itemheight * (data->visible - 1),192);
									data->top++;
									drawitem(obj,data,data->top + data->visible - 1,FALSE);
									drawuparrow(data,adata,FALSE);
									drawdownarrow(data,adata,FALSE);
								}
							} else {
								// scroll up
								if (data->top > 0)
								{
									ClipBlit(data->rp,data->menux1 + data->menuleft,data->menuy1 + data->menutop,
												data->rp,data->menux1 + data->menuleft,data->menuy1 + data->menutop + data->itemheight,data->itemwidth,data->itemheight * (data->visible - 1),192);
									data->top--;
									drawitem(obj,data,data->top,FALSE);
									drawuparrow(data,adata,FALSE);
									drawdownarrow(data,adata,FALSE);
								}
							}
						} /* if (!data->turbocountdown || (INPUTMSG->gpi_IEvent->ie_Class == IECLASS_TIMER)) */

					} else {

						data->turbocountdown = TURBOCOUNTDOWN;
						x = data->selected;
						data->selected = data->top + i;
						if (data->selected != x)
						{
							drawitem(obj,data,x,FALSE);
							drawitem(obj,data,data->selected,FALSE);
						}
					}
				}
				
			} else {	/* if (data->popup) */

				/* like a GadTools cycle gadget */

				switch(INPUTMSG->gpi_IEvent->ie_Class)
				{
					case IECLASS_RAWMOUSE:
						switch(INPUTMSG->gpi_IEvent->ie_Code)
						{
							case IECODE_LBUTTON | IECODE_UP_PREFIX:
								if (GAD->Flags & GFLG_SELECTED)
								{
									data->active++;
									if (data->active >= data->numitems) data->active = 0;

									GAD->Flags &= (~GFLG_SELECTED);
									RenderObject_Update(obj,INPUTMSG->gpi_GInfo);
									rc |= GMR_VERIFY;
									*INPUTMSG->gpi_Termination = GAD->GadgetID;
								}
								rc |= GMR_NOREUSE;
								break;
								
							default:
								if (INPUTMSG->gpi_Mouse.X >= 0 &&
									 INPUTMSG->gpi_Mouse.Y >= 0 &&
									 INPUTMSG->gpi_Mouse.X <= GAD->Width &&
									 INPUTMSG->gpi_Mouse.Y <= GAD->Height)
								{
									if (!(GAD->Flags & GFLG_SELECTED))
									{
										GAD->Flags |= GFLG_SELECTED;
										RenderObject_Update(obj,INPUTMSG->gpi_GInfo);
									}
								} else {
									if (GAD->Flags & GFLG_SELECTED)
									{
										GAD->Flags &= (~GFLG_SELECTED);
										RenderObject_Update(obj,INPUTMSG->gpi_GInfo);
									}
								}
								rc = GMR_MEACTIVE;
	
								break;

						} /* switch(INPUTMSG->gpi_IEvent->ie_Code) */

				} /* switch(INPUTMSG->gpi_IEvent->ie_Class) */

			} /* if (data->popup) {...} else { */
			break;
		
		case GM_GOINACTIVE:
			if (data->winmap)
			{
				if (data->winmaptype == WINMAP_WINDOW)
				{
					CloseWindow((struct Window *)data->winmap);
				} else {
					BltBitMap((struct BitMap *)data->winmap,0,0,
								 adata->ri->bm,data->menuleft,data->menutop,
								 data->menuwidth,data->menuheight,
								 192,
								 255,
								 0);

					WaitBlit();
					FreeBitMap((struct BitMap *)data->winmap);					
					UnlockLayers(adata->ri->li);

				if (data->sentgadgetup)
					{					
						RenderObject_Update(obj,GOINACTIVEMSG->gpgi_GInfo);
					}
				}
				data->winmap = 0;
			}
			break;
			
		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch(msg->MethodID) */
	
	return rc;
}


