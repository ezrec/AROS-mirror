#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "buttonclass.h"

static struct TagItem flagtags[] =
{
	{PUIA_Button_Toggle,PUIF_Button_Toggle},
	{TAG_DONE}
};

static void mRender(struct IClass *cl,struct Gadget *gad,struct gpRender *msg)
{
	struct RastPort *rp;
	struct buttondata *data;
	struct PUI_AreaData *adata;
	WORD x,y,a1,a2;
	BYTE selected;
	
	data = INST_DATA(cl,gad);
	adata = INST_DATA(areaclass,gad);
	
	selected = (gad->Flags & GFLG_SELECTED) ? TRUE : FALSE;

	if ((rp = msg->gpr_RPort))
	{	
		DoSuperMethodA(cl,(Object *)gad,(Msg)msg);

		SetAPen(rp,adata->ri->pens[selected ? FILLTEXTPEN : TEXTPEN]);
		SetDrMd(rp,JAM1);
		SetSoftStyle(rp,data->textstyle,AskSoftStyle(rp));

		a1 = gad->LeftEdge + adata->offsetl + (data->image ? ((struct Gadget *)data->image)->Width + image_text_spacing : 0);
		a2 = gad->LeftEdge + gad->Width - adata->offsetr;
		
		switch(data->textalign)
		{
			case PUIV_Align_Left:
				x = a1;
				break;
			
			case PUIV_Align_Right:
				x = a2 - data->textpixellen;
				break;
			
			default:
				x = (a1 + a2 - data->textpixellen) / 2;
				break;
		}

		a1 = gad->TopEdge + adata->offseto;
		a2 = gad->TopEdge + gad->Height - adata->offsetu;

		y = (a1 + a2 - adata->ri->fontheight) / 2 + adata->ri->fontbaseline ;

		if (data->text)
		{
			Move(rp,x,y);
			if (!data->t2 && data->textlen)
			{
				Text(rp,data->text,data->textlen);
			} else {
				if (data->textlen1)
				{
					Text(rp,data->text,data->textlen1);
				}
				if (data->textlen2)
				{
					Move(rp,x + data->textpixellen2,y);
					Text(rp,data->t2,data->textlen2);
				}
	
				Move(rp,x + data->textpixellen2,y+2);
				Draw(rp,x + data->textpixellen2 + data->textpixellen3,y+2);
	
			}

		} /* if (data->text) */
		
		if (data->image)
		{
			y = (a1 + a2 - ((struct Gadget *)data->image)->Height) / 2;

			((struct Gadget *)data->image)->Flags = gad->Flags;

			DoMethodA(data->image,(Msg)msg);
		}
	}
}

ULONG ButtonInstanceSize(void)
{
	return sizeof(struct buttondata);
}

ULONG ButtonDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	struct buttondata *data;
	struct PUI_AreaData *adata,*adatai;
	char *sp;
	WORD	x,y,w,h;
	ULONG rc;
	
	rc = 0;
	
	if (msg->MethodID != OM_NEW)
	{
		data = INST_DATA(cl,obj);
		adata = INST_DATA(areaclass,obj);
	}

	switch(msg->MethodID)
	{
		case OM_NEW:
			if ((obj = (Object *)DoSuperMethodA(cl,obj,msg)))
			{
				rc = (ULONG)obj;
				
				data = INST_DATA(cl,obj);

				data->text = (char *)GetTagData(PUIA_Button_Text,0,ATTRLIST);
				data->image = (Object *)GetTagData(PUIA_Button_Image,0,ATTRLIST);
				data->textstyle = GetTagData(PUIA_Button_TextStyle,FS_NORMAL,ATTRLIST);
				data->textalign = GetTagData(PUIA_Button_TextAlign,PUIV_Align_Center,ATTRLIST);
				data->flags = PackBoolTags(0,ATTRLIST,flagtags);


				data->t2 = 0;

				if (data->text)
				{
					data->textlen = strlen(data->text);
				}

			};
			break;
		
		case OM_DISPOSE:
			if (data->image) DisposeObject(data->image);
			DoSuperMethodA(cl,obj,msg);
			break;
			
		case PUIM_SETUP:
			DoSuperMethodA(cl,obj,msg);
			if (data->image)
			{
				DoMethodA((Object *)data->image,msg);
			}
			break;
			
		case PUIM_MINMAX:			
			x = y = 1;

			if (data->text)
			{
				MyTextExtent(adata->ri->font,data->text,data->textlen,data->textstyle,&data->textpixellen,0);

				if ((sp = strchr(data->text,'_')))
				{
					data->t2 = sp+1;
					data->textlen1 = (UBYTE)(sp - data->text);
					data->textlen2 = data->textlen - data->textlen1 - 1;

					MyTextExtent(adata->ri->font,data->text,data->textlen1,data->textstyle,&data->textpixellen2,0);
					MyTextExtent(adata->ri->font,data->t2,1,data->textstyle,&data->textpixellen3,0);

					data->textpixellen -= data->textpixellen3;
				}

				x = data->textpixellen;// + adata->ri->font->tf_XSize*2;
				y = adata->ri->fontheight;

				MINMAXMSG->minmax->minwidth += x;
				MINMAXMSG->minmax->defwidth += x;
				MINMAXMSG->minmax->maxwidth += PUI_MAXMAX;
				
				MINMAXMSG->minmax->minheight += y;
				MINMAXMSG->minmax->defheight += y;
				MINMAXMSG->minmax->maxheight += y;
			}

			if (data->image)
			{
				x = data->text ? image_text_spacing : 0;
				
				adata = INST_DATA(areaclass,data->image);
				DoMethod(data->image,PUIM_MINMAX,
											0,
											&adata->minmax);

				MINMAXMSG->minmax->minwidth += adata->minmax.minwidth + x;
				MINMAXMSG->minmax->defwidth += adata->minmax.defwidth + x;
				MINMAXMSG->minmax->maxwidth += adata->minmax.maxwidth + x;
				
				if (data->text)
				{
					if (adata->minmax.minheight > y)
					{
						MINMAXMSG->minmax->minheight += (adata->minmax.minheight - y);
					}
					if (adata->minmax.defheight > y)
					{
						MINMAXMSG->minmax->defheight += (adata->minmax.defheight - y); 
					}
					if (adata->minmax.maxheight > y)
					{
						MINMAXMSG->minmax->maxheight += (adata->minmax.maxheight - y);
					}
				} else {
					MINMAXMSG->minmax->minheight += adata->minmax.minheight;
					MINMAXMSG->minmax->defheight += adata->minmax.defheight;
					MINMAXMSG->minmax->maxheight += adata->minmax.maxheight;
				}
			} /* if (data->image) */

			DoSuperMethodA(cl,obj,msg);
			CorrectMinMax(MINMAXMSG->minmax);
			break;
		
		case PUIM_LAYOUT:
			DoSuperMethodA(cl,obj,msg);
			if (data->image)
			{
				adatai = INST_DATA(areaclass,data->image);

				w = adatai->minmax.minwidth;
				h = GAD->Height - adata->offseto - adata->offsetu;
				if (adatai->minmax.maxheight < h) h = adatai->minmax.maxheight;
				
				x = GAD->LeftEdge + adata->offsetl;
				y = (GAD->TopEdge + adata->offseto + GAD->TopEdge + GAD->Height - adata->offsetu - h) / 2;

    	    	    	    	{
				    struct LayoutMsg lmsg;
				    
				    lmsg.MethodID   = PUIM_LAYOUT;
				    lmsg.gi         = NULL;
				    lmsg.box.Left   = x;
				    lmsg.box.Top    = y;
				    lmsg.box.Width  = w;
				    lmsg.box.Height = h; 
				    
				    DoMethodA(data->image, (Msg)&lmsg);
				}
			}
			break;
		
		case GM_RENDER:
			if (PUI_MayRender(obj)) mRender(cl,(struct Gadget *)obj,(struct gpRender *)msg);
			break;
 
#ifdef _AROS
    	    	case GM_GOACTIVE:
#endif
		case GM_HANDLEINPUT:
			if (data->flags & PUIF_Button_Toggle)
			{
				GAD->Flags ^= GFLG_SELECTED;
				RenderObject_Update(obj,INPUTMSG->gpi_GInfo);
				rc = GMR_VERIFY | GMR_NOREUSE;
				*INPUTMSG->gpi_Termination = GAD->GadgetID;
				break;
			}

			switch(INPUTMSG->gpi_IEvent->ie_Class)
			{
				case IECLASS_RAWMOUSE:
					switch(INPUTMSG->gpi_IEvent->ie_Code)
					{
						case IECODE_LBUTTON | IECODE_UP_PREFIX:
							if (GAD->Flags & GFLG_SELECTED)
							{
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
					}
					break;
				
				case GM_GOINACTIVE:
					if (!(data->flags & PUIF_Button_Toggle))
					{
						if (GAD->Flags & GFLG_SELECTED)
						{
							GAD->Flags &= (~GFLG_SELECTED);
							RenderObject_Update(obj,GOINACTIVEMSG->gpgi_GInfo);
						}
					}
					break;
					
				default:
					rc = GMR_MEACTIVE;
					break;
			}
			break;
		
		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch(msg->MethodID) */
	
	return rc;
}

