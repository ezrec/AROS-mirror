#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include "pui.h"

void DrawFrame(Object *obj,struct RastPort *rp,struct IBox *box,WORD id,BOOL rec)
{
	struct PUI_AreaData *data;

	WORD type,pen1,pen2,pen3;
	BOOL recessed;

	WORD dummy;
	WORD x1,y1,x2,y2;

	data = INST_DATA(areaclass,obj);

	type 		= frameinfo[id].type;
	pen1 		= data->ri->pens[frameinfo[id].pen1];
	pen2 		= data->ri->pens[frameinfo[id].pen2];
	pen3 		= data->ri->pens[frameinfo[id].pen3];
	recessed = (frameinfo[id].recessed ? TRUE : FALSE) ^ (rec ? TRUE : FALSE);
		
	if (type)
	{
		x1 = box->Left;
		y1 = box->Top;
		x2 = x1 + box->Width - 1;
		y2 = y1 + box->Height - 1;
	
		if (recessed)
		{
			dummy = pen1;
			pen1 = pen2;
			pen2 = dummy;
		}
		
		switch (type)
		{
			case FRAMETYPE_BUTTON:
				SetAPen(rp,pen1);
				RectFill(rp,x1,y1,x1,y2);
				RectFill(rp,x1+1,y1,x2-1,y1);
				SetAPen(rp,pen2);
				RectFill(rp,x2,y1,x2,y2);
				RectFill(rp,x1+1,y2,x2-1,y2);
				break;
				
			case FRAMETYPE_STRING:
				SetAPen(rp,pen1);
				RectFill(rp,x1,y1,x1,y2);
				RectFill(rp,x1+1,y1,x2-1,y1);
				RectFill(rp,x2-1,y1+1,x2-1,y2-1);
				RectFill(rp,x1+2,y2-1,x2-2,y2-1);
				SetAPen(rp,pen2);
				RectFill(rp,x1+1,y1+1,x2-2,y1+1);
				RectFill(rp,x1+1,y1+2,x1+1,y2);
				RectFill(rp,x1+2,y2,x2,y2);
				RectFill(rp,x2,y1,x2,y2-1);
				break;
			
			case FRAMETYPE_DROP:
				SetAPen(rp,pen1);
				RectFill(rp,x1,y1,x1,y2);
				RectFill(rp,x1+1,y1,x2-1,y1);
				RectFill(rp,x2-2,y1+2,x2-2,y2-2);
				RectFill(rp,x1+3,y2-2,x2-3,y2-2);
				SetAPen(rp,pen2);
				RectFill(rp,x2,y1,x2,y2);
				RectFill(rp,x1+1,y2,x2-1,y2);
				RectFill(rp,x1+2,y1+2,x1+2,y2-2);
				RectFill(rp,x1+3,y1+2,x2-3,y1+2);
				SetAPen(rp,pen3);
				RectFill(rp,x1+1,y1+1,x2-1,y1+1);
				RectFill(rp,x1+1,y1+2,x1+1,y2-1);
				RectFill(rp,x1+2,y2-1,x2-1,y2-1);
				RectFill(rp,x2-1,y1+2,x2-1,y2-2);
				break;
			
			case FRAMETYPE_MONO:
				SetAPen(rp,pen1);
				RectFill(rp,x1,y1,x2,y1);
				RectFill(rp,x2,y1+1,x2,y2);
				RectFill(rp,x1,y2,x2-1,y2);
				RectFill(rp,x1,y1+1,x1,y2-1);
				break;
	
		} /* switch (type) */

	} /* if (type) */
};


void DrawLine(struct RastPort *rp,WORD x1,WORD y1,WORD x2,WORD y2)
{
	Move(rp,x1,y1);
	Draw(rp,x2,y2);
}

void DrawArrow(struct RastPort *rp,WORD x1,WORD y1,WORD type)
{
	switch(type)
	{
		case ARROW_DOWN:
			RectFill(rp,x1,y1,x1+6,y1);y1++;
			RectFill(rp,x1+1,y1,x1+5,y1);y1++;
			RectFill(rp,x1+2,y1,x1+4,y1);y1++;
			WritePixel(rp,x1+3,y1);
			break;

		case ARROW_UP:
			WritePixel(rp,x1+3,y1);y1++;
			RectFill(rp,x1+2,y1,x1+4,y1);y1++;
			RectFill(rp,x1+1,y1,x1+5,y1);y1++;
			RectFill(rp,x1,y1,x1+6,y1);
			break;
	}
}

void RectFillBox(struct RastPort *rp,struct IBox *box)
{
	RectFill(rp,box->Left,
					box->Top,
					box->Left + box->Width - 1,
					box->Top + box->Height - 1);
}

void MyTextExtent(struct TextFont *font,char *text,WORD len,WORD style,WORD *width,WORD *height)
{
	struct TextExtent te;
	
	ObtainSemaphore(&temprpsem);

	SetFont(&temprp,font);
	SetSoftStyle(&temprp,style,AskSoftStyle(&temprp));
	TextExtent(&temprp,text,len,&te);
	
	if (width)  *width = te.te_Width;
	if (height) *height = te.te_Height;

	ReleaseSemaphore(&temprpsem);
}

LONG SelectWidth(struct IBox *box,struct PUI_MinMax *minmax)
{
	LONG rc;
	
	rc = minmax->maxwidth;
	if (rc > box->Width) rc = box->Width;

	return rc;
}

LONG SelectHeight(struct IBox *box,struct PUI_MinMax *minmax)
{
	LONG rc;
	
	rc = minmax->maxheight;
	if (rc > box->Height) rc = box->Height;

	return rc;
}

LONG SelectLeft(Object *obj,struct IBox *box,struct PUI_MinMax *minmax)
{
	struct PUI_AreaData *adata;
	LONG width,rc;
	
	adata = INST_DATA(areaclass,obj);

	width = SelectWidth(box,minmax);

	if (adata->flags & PUIF_Area_AlignLeft)
	{
		rc = box->Left;
	} else if (adata->flags & PUIF_Area_AlignRight)
	{
		rc = box->Left + box->Width - width;
	} else {
		rc = box->Left + (box->Width - width) / 2;
	}
	
	return rc;
}

LONG SelectTop(Object *obj,struct IBox *box,struct PUI_MinMax *minmax)
{
	struct PUI_AreaData *adata;
	LONG height,rc;
	
	adata = INST_DATA(areaclass,obj);

	height = SelectHeight(box,minmax);

	if (adata->flags & PUIF_Area_AlignTop)
	{
		rc = box->Top;
	} else if (adata->flags & PUIF_Area_AlignBottom)
	{
		rc = box->Top + box->Height - height;
	} else {
		rc = box->Top + (box->Height - height) / 2;
	}
	
	return rc;
}

void CorrectMinMax(struct PUI_MinMax *minmax)
{
	if (minmax->maxwidth < minmax->minwidth)
	{
		minmax->maxwidth = minmax->minwidth;
	}
	
	if (minmax->maxheight < minmax->minheight)
	{
		minmax->maxheight = minmax->minheight;
	}
}

void ShrinkBox(struct IBox *box,WORD left,WORD top,WORD right,WORD bottom)
{
	box->Left += left;
	box->Top  += top;
	box->Width -= (left + right);
	box->Height -= (top + bottom);
}

LONG GreatestMinWidth(struct MinList *list)
{
	struct PUI_AreaData *adata;
	Object *obj,*objstate;
	LONG rc;
	
	rc = 0;

	objstate = (Object *)list->mlh_Head;
	while ((obj = NextObject(&objstate)))
	{
		adata = INST_DATA(areaclass,obj);
		if (adata->minmax.minwidth > rc)
		{
			rc = adata->minmax.minwidth;
		}
	}
	
	return rc;
};

LONG GreatestMinHeight(struct MinList *list)
{
	struct PUI_AreaData *adata;
	Object *obj,*objstate;
	LONG rc;
	
	rc = 0;

	objstate = (Object *)list->mlh_Head;
	while ((obj = NextObject(&objstate)))
	{
		adata = INST_DATA(areaclass,obj);
		if (adata->minmax.minheight > rc)
		{
			rc = adata->minmax.minheight;
		}
	}
	
	return rc;
};

void RenderObject_Update(Object *obj,struct GadgetInfo *gi)
{
	struct RastPort *rp;
	
	if ((rp = ObtainGIRPort(gi)))
	{
		DoMethod(obj,GM_RENDER,
						 gi,
						 rp,
						 GREDRAW_UPDATE);

		ReleaseGIRPort(rp);
	}				 
}

void RenderObject_Redraw(Object *obj,struct GadgetInfo *gi)
{
	struct RastPort *rp;
	
	if ((rp = ObtainGIRPort(gi)))
	{
		DoMethod(obj,GM_RENDER,
						 gi,
						 rp,
						 GREDRAW_REDRAW);

		ReleaseGIRPort(rp);
	}				 
}

Object *NextTabCycleObject(Object *actobj)
{
	struct MinList *list = 0;
	Object *rc = 0, *o, *objstate, *parent = 0;

	if (GetAttr(PUIA_Parent,actobj,(ULONG *)&parent))
	{
		if (parent)
		{
			GetAttr(PUIA_Group_ChildList,parent,(ULONG *)&list);
		}
			
		if (list)
		{
			objstate = (Object *)list->mlh_Head;
			while ((o = NextObject(&objstate)))
			{
				if (o == actobj) break;
			}
					
			if (o)
			{
				while ((o = NextObject(&objstate)))
				{
					if (((struct Gadget *)o)->Flags & GFLG_TABCYCLE)
					{
						rc = o;
						break;
					}
				}
			
				if (!rc)
				{
					objstate = (Object *)list->mlh_Head;
					while ((o = NextObject(&objstate)))
					{
						if (o == actobj) break;

						if (((struct Gadget *)o)->Flags & GFLG_TABCYCLE)
						{
							rc = o;
							break;
						}
					}

				} /* if (!rc) */
			
			} /* if (o) */
		
		} /* if (list) */
			
	} /* if (GetAttr(PUIA_Parent,actobj)) */

	return rc;
}

Object *PrevTabCycleObject(Object *actobj)
{
	struct MinList *list = 0;
	Object *rc = 0, *o, *objstate, *parent = 0, *prevo = 0;

	if (GetAttr(PUIA_Parent,actobj,(ULONG *)&parent))
	{
		if (parent)
		{
			GetAttr(PUIA_Group_ChildList,parent,(ULONG *)&list);
		}
			
		if (list)
		{
			objstate = (Object *)list->mlh_Head;
			while ((o = NextObject(&objstate)))
			{
				if (o == actobj)
				{
					if (prevo) rc = prevo;
					break;
				}

				if (((struct Gadget *)o)->Flags & GFLG_TABCYCLE)
				{
					prevo = o;
				}
			}
					
			if (o && !rc)
			{
				prevo = 0;
				while ((o = NextObject(&objstate)))
				{
					if (((struct Gadget *)o)->Flags & GFLG_TABCYCLE)
					{
						prevo = o;
					}
				}
			
				rc = prevo;

			} /* if (o && !rc) */
		
		} /* if (list) */
			
	} /* if (GetAttr(PUIA_Parent,actobj)) */

	return rc;
}

