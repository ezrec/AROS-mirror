/*

	This is a simple version of the preview class. No subtasks,
	and no context menu. It also keeps it's copy of passed image,
	so no need to keep if for yourself.


*/

#define AROS_ALMOST_COMPATIBLE

#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <exec/libraries.h>
#include <graphics/gfx.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <graphics/rpattr.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <constructor.h>
#include <intuition/pointerclass.h>

#define	 SYSTEM_PRIVATE
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#undef	 SYSTEM_PRIVATE

#include <private/vapor/vapor.h>
#include "pageview_class.h"
#include "documentview_class.h"
#include "documentlayout_class.h"
#include "annotation_class.h"

#include "system/chunky.h"
#include "system/memory.h"
#include "system/functions.h"

#include "util.h"
#include "poppler.h"

typedef	struct ThbImage_s
{
	unsigned char *data;
	int width;
	int height;
	int bpp;				// can be 1 or 4
} ThbImage;

struct markernode {
	struct MinNode n;
	float x1, y1, x2, y2;	// pdf document device coordinates
	int ux1, uy1, ux2, uy2;	// user space coordinates
	int valid, id;
	unsigned int color;
};

struct annotationnode {
	struct MinNode n;
	float x1, y1, x2, y2;
	int ux1, uy1, ux2, uy2;
	Object *obj;
};

struct lasso {
	int enabled;
	struct markernode marker;
	struct pdfSelectionRegion *oldregion;
};

struct Data
{
	ThbImage *image;
	void *parent;
	struct MUI_EventHandlerNode eh;
	struct PageViewRegion region;
	int mx, my;
	int moveregion;
	int offsetx, offsety;
	int markmx, markmy;
	int pointertype;
	int mousedown;
	int markAspect;
	struct Rect32 refreshrect;
	int layoutwidth;
	int layoutheight;

	void *doc;
	int page;
	int ready;
	int prevwidth, prevheight;
	int mediawidth, mediaheight;
	int invalid;
	int flushmethodid;
	int information;
	int ispreview;
	int rotation;

	int quiet;
	
	struct lasso lasso;
	struct MinList markerlist;
	struct MinList annotationlist;

	struct SignalSemaphore sema;
};

#define D(x) x

/*
 * MUI Standard methods
 */

///	dispose

DEFDISP
{
	GETDATA;

	ObtainSemaphore(&data->sema);
	if (data->image != NULL)
		mfree(data->image->data);
	free(data->image);
	ReleaseSemaphore(&data->sema);

	if (data->lasso.oldregion != NULL)
		pdfDisposeRegionForSelection(data->doc, data->lasso.oldregion);

	/* remove all markers and annotations (only links to annotation objects!) */

	while(!ISLISTEMPTY(&data->markerlist))
	{
		struct markernode *mn = (struct markernode*)GetHead(&data->markerlist);
        REMOVE(mn);
		free(mn);
	}

	while(!ISLISTEMPTY(&data->annotationlist))
	{
		struct annotationnode *an = (struct annotationnode*)GetHead(&data->annotationlist);
		REMOVE(an);
		free(an);
	}

	/* dispose parent class */

	return(DOSUPER);
}

////

///	askminmax

static int getinformationspace(Object *obj, struct Data *data)
{
	int space = 0;
	if (data->information & MUIV_PageView_Information_Number)
		space += _font(obj)->tf_YSize + 2;

	return space;
}

METHOD ThbAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	GETDATA;

	/*
	 let our superclass first fill in what it thinks about sizes.
	 this will e.g. add the size of frame and inner spacing.
	*/

	DOSUPER;

	/*
	 now add the values specific to our object. note that we
	 indeed need to *add* these values, not just set them!
	*/

	msg->MinMaxInfo->MinWidth  += data->layoutwidth;
	msg->MinMaxInfo->DefWidth  += data->layoutwidth;
	msg->MinMaxInfo->MaxWidth  = MUI_MAXMAX;

	msg->MinMaxInfo->MinHeight += data->layoutheight + getinformationspace(obj, data);
	msg->MinMaxInfo->DefHeight += data->layoutheight + getinformationspace(obj, data);
	msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;

	return(0);
}

////

///	new

DEFNEW
{
	obj = DoSuperNew(cl, obj, MUIA_FillArea, FALSE,
							//	  MUIA_CustomBackfill, TRUE,
							//	MUIA_Background, MUII_SHADOW,
								TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;

		memset(data, 0, sizeof(struct Data));

		InitSemaphore(&data->sema);
		NEWLIST(&data->markerlist);
		NEWLIST(&data->annotationlist);

		/* get properties */

		data->refreshrect.MinX = -1;
		data->layoutwidth = 128;
		data->layoutheight = 128;
		data->information = MUIV_PageView_Information_None;

		FORTAG(INITTAGS)
		{
				case MUIA_PageView_Rotation:
					data->rotation = tag->ti_Data;
					break;
		
				case MUIA_PageView_PDFDocument:
					data->doc = (void*)tag->ti_Data;
					break;
				case MUIA_PageView_Page:
					data->page = tag->ti_Data;
					break;
				case MUIA_PageView_MediaWidth:
					data->mediawidth = tag->ti_Data;
					break;
				case MUIA_PageView_MediaHeight:
					data->mediaheight = tag->ti_Data;
					break;
				case MUIA_PageView_Information:
					data->information = tag->ti_Data;
					break;
				case MUIA_PageView_IsPreview:
					data->ispreview= tag->ti_Data;
					break;
		}
		NEXTTAG

	}

	return (ULONG)obj;

}

////

///	setup

METHOD riSetup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
	GETDATA;

	if (!(DOSUPER))
		return(FALSE);

	return TRUE;
}

////

///	cleanup

METHOD riCleanup(struct IClass *cl,Object *obj,Msg msg)
{
	GETDATA;

	if (data->flushmethodid != 0)
	{
		DoMethod(_app(obj), MUIM_Application_UnpushMethod, NULL, data->flushmethodid, 0);
		data->flushmethodid = 0;
	}

	DoMethod(obj, MUIM_PageView_Flush);

	return(DOSUPER);
}

////

///	draw

void kprintf(char *fmt,...);

static void rendermarker(struct Data *data, struct RastPort *rp, struct markernode *mn, int x0, int y0)
{
	int x1 = mn->ux1, y1 = mn->uy1;
	int x2 = mn->ux2, y2 = mn->uy2;
						
	SetRPAttrs(rp, RPTAG_PenMode, FALSE, RPTAG_FgColor, 0xffff0000, TAG_DONE);
	RectFill(rp, x0 + x1 - 2, y0 + y2 - 2, x0 + x2 + 2, y0 + y2 - 1);
	RectFill(rp, x0 + x1 - 2, y0 + y1 + 1, x0 + x2 + 2, y0 + y1 + 2);
	RectFill(rp, x0 + x1 - 2, y0 + y2, x0 + x1 - 1, y0 + y1);
	RectFill(rp, x0 + x2 + 1, y0 + y2, x0 + x2 + 2, y0 + y1);

	ProcessPixelArray(rp, x0 + x1 - 1, y0 + y2 - 1, x2 - x1, y1 - y2, POP_DARKEN, 10, NULL);
}

METHOD riDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	unsigned int rc = DOSUPER;
	//return rc;
	/* If MADF_DRAWOBJECT is not set then we shoulnt draw anything */

	if (msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE))
    {
		GETDATA;
		int activepage;

		ObtainSemaphore(&data->sema);

		if (data->parent == NULL)
			data->parent = (void*)xget(obj, MUIA_Parent);

		activepage = xget(data->parent, MUIA_DocumentLayout_Page);

		if (data->image && data->image->data)
		{
			/* write image */

			int	width, height;
			int	x0, y0;
			int	prvwidth, prvheight;

			ThbImage *image = data->image;

			x0 = _mleft(obj);
			y0 = _mtop(obj);
			prvwidth = _mwidth(obj);
			prvheight = _mheight(obj) - getinformationspace(obj, data);

			/* calculate placement */

			if (image->height < prvheight)
			{
				y0 += (prvheight - image->height) / 2;
				height = image->height;
			}
			else
				height = prvheight;
			
			if (image->width < prvwidth)
			{
				x0 += (prvwidth - image->width) / 2;
				width = image->width;
			}
			else
				width = prvwidth;

			/* store offset for eventhandler */

			data->offsetx = x0 - _mleft(obj);
			data->offsety = y0 - _mtop(obj);

			/* install cliprect and draw */

			if (data->quiet == FALSE)
			{
				if (0 && data->refreshrect.MinX != -1)
				{
					width = data->refreshrect.MaxX - data->refreshrect.MinX;
					height = data->refreshrect.MaxY - data->refreshrect.MinY;
					WritePixelArray(image->data, data->refreshrect.MinX, data->refreshrect.MinY, image->width * image->bpp, _rp(obj), x0 + data->refreshrect.MinX, y0 + data->refreshrect.MinY, width, height, image->bpp == 4 ? RECTFMT_ARGB : RECTFMT_GREY8);
				}
				else
					WritePixelArray(image->data, 0, 0, image->width * image->bpp, _rp(obj), x0, y0, width, height, data->image->bpp == 4 ? RECTFMT_ARGB : RECTFMT_GREY8);

				/* draw remaining area with background */

				if ((image->width < _mwidth(obj) || image->height < _mheight(obj)))
				{
					int lb = x0 - _mleft(obj);
					int rb = _mwidth(obj) - image->width - lb;
					int tb = y0 - _mtop(obj);
					int bb = _mheight(obj) - image->height - tb;
					#if 1
					if (lb > 0)
						DoMethod(obj, MUIM_DrawBackground, _mleft(obj), _mtop(obj), lb, _mheight(obj), 0, 0, 0);
					if (rb > 0)
						DoMethod(obj, MUIM_DrawBackground, _mright(obj) - rb + 1, _mtop(obj), rb, _mheight(obj), 0, 0, 0);

					lb = max(0, lb);
					rb = max(0, rb);

					if (tb > 0)
						DoMethod(obj, MUIM_DrawBackground, _mleft(obj) + lb, _mtop(obj), _mwidth(obj) - lb - rb, tb, 0, 0, 0);
					if (bb > 0)
						DoMethod(obj, MUIM_DrawBackground, _mleft(obj) + lb, _mbottom(obj) - bb + 1, _mwidth(obj) - lb - rb, bb, 0, 0, 0);
					#endif
				}

				/* render page number if requested */

				if (data->information & MUIV_PageView_Information_Number)
				{
					char buff[32];
					int informationspace = getinformationspace(obj, data);
					int textwidth, textpos;
					unsigned int textdim;

					snprintf(buff, sizeof(buff), "%d", data->page);

					textdim = DoMethod(obj, MUIM_TextDim, buff, strlen(buff), 0, 0);
					textwidth = textdim & 0xffff;
					textpos = _mwidth(obj) /2 - textwidth / 2;
					DoMethod(obj, MUIM_Text, _mleft(obj) + textpos, _mbottom(obj) - informationspace, _mwidth(obj), informationspace, buff, strlen(buff), 0, 0);

				}

				/* draw active page marker (TODO: make it flickerfree) */

				if (data->page == activepage)
				{
					SetRPAttrs(_rp(obj), RPTAG_PenMode, FALSE, RPTAG_FgColor, 0xffff0000, TAG_DONE);
					RectFill(_rp(obj), x0, y0, x0 + width - 1, y0 + 1);
					RectFill(_rp(obj), x0, y0 + height - 2, x0 + width - 1, y0 + height - 1);
					RectFill(_rp(obj), x0, y0, x0 + 1, y0 + height - 1);
					RectFill(_rp(obj), x0 + width - 2, y0, x0 + width - 1, y0 + height - 1);
				}

				/* draw selection markers (TODO: make it flickerfree) */

				if (!ISLISTEMPTY(&data->markerlist))
				{
					struct markernode *mn;
								
					ITERATELIST(mn, &data->markerlist)
					{
						rendermarker(data, _rp(obj), mn, x0, y0);
					}
				}
				
				/* render lasso */
				
				if (data->lasso.enabled)
				{
					rendermarker(data, _rp(obj), &data->lasso.marker, x0, y0);	
				}
			}

		}
		else if (data->quiet == FALSE)
		{
			DoMethod(obj, MUIM_DrawBackground, _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj), 0, 0, 0);
		}

		if (data->image == NULL || data->image->data == NULL || data->invalid /*|| data->prevwidth != _mwidth(obj) || data->prevheight != _mheight(obj)*/)
		{
			/* setup needrefresh param so the main app can rerender page */
			//data->prevwidth = _mwidth(obj);
			//data->prevheight = _mheight(obj);
			data->invalid = FALSE;
			SetAttrs(_parent(obj), MUIA_Group_Forward, FALSE, MUIA_PageView_NeedRefresh, data->page, TAG_DONE);
		}

		ReleaseSemaphore(&data->sema);
	}

	return rc;
}

////

/// render selection region

static void renderselectionregion(Object *obj, struct Data *data, struct pdfSelectionRegion *selection)
{
	if (selection != NULL && data->image != NULL && data->image->data != NULL)
	{
		int i;
		for(i=0; i<selection->numrects; i++)
		{
			int px, py;
			int pw, ph;
			
			int x, y;
			struct pdfSelectionRectangle *rect = &selection->rectangles[i];

			if (1)
			for(y=rect->y1 * data->image->height; y<=rect->y2 * data->image->height; y++)
			{
				if (data->image->bpp == 4)
				{
					unsigned int *img = (unsigned int*)data->image->data;
					int width = data->image->width;
									
					for(x=rect->x1 * data->image->width; x<=rect->x2 * data->image->width; x++)
					{
						if (x >= 0 && y >= 0 && x < data->image->width && y < data->image->height)
							img[x + width * y] = 0x00ffffff ^ img[x + width * y];
					}
				}
				else if (data->image->bpp == 1)
				{
					unsigned char *img = (unsigned char*)data->image->data;
					int width = data->image->width;
									
					for(x=rect->x1 * data->image->width; x<=rect->x2 * data->image->width; x++)
					{
						if (x >= 0 && y >= 0 && x < data->image->width && y < data->image->height)
							img[x + width * y] = 0xff ^ img[x + width * y];
					}
				}
			}
		}
	}
}

////

///	updatemarkers (resize to device coords)

static void updatemarkers(Object *obj, struct Data *data, int all)
{
	struct markernode *mn;
	struct annotationnode *an;
	//pdfLock(data->doc);

	if (data->image != NULL && data->image->data != NULL)
	{
			
		ITERATELIST(mn, &data->markerlist)
		{
			//pdfConvertDeviceToUser(data->doc, data->page, mn->x1, mn->y1, &mn->ux1, &mn->uy1);
			//pdfConvertDeviceToUser(data->doc, data->page, mn->x2, mn->y2, &mn->ux2, &mn->uy2);
			
			float ox1 = mn->x1 - 0.5f, oy1 = mn->y1 - 0.5f;
			float ox2 = mn->x2 - 0.5f, oy2 = mn->y2 - 0.5f;
			float x1, x2, y1, y2;
			float t;
			
			if (data->rotation == 0)
			{
				x1 = ox1; y1 = oy1;
				x2 = ox2; y2 = oy2;
			}					
			else if (data->rotation == 1)
			{
				x1 = -oy1; y1 = ox1;
				x2 = -oy2; y2 = ox2;
			}
			else if (data->rotation == 2)
			{
				x1 = -ox1; y1 = -oy1;
				x2 = -ox2; y2 = -oy2;
			}	
			else /*(data->rotation == 3)*/
			{
				x1 = oy1; y1 = -ox1;
				x2 = oy2; y2 = -ox2;
			}											
			
			if (x1 > x2) /*can happen after rotation */
			{
				t = x1;
				x1 = x2;
				x2 = t;
			}
			if (y1 < y2)
			{
				t = y1;
				y1 = y2;
				y2 = t;
			}
			
			x1 += 0.5f; y1 += 0.5f;
			x2 += 0.5f; y2 += 0.5f;
			
			mn->ux1 = x1 * data->image->width;
			mn->uy1 = y1 * data->image->height;
			mn->ux2 = x2 * data->image->width;
			mn->uy2 = y2 * data->image->height;
			mn->valid = TRUE;
		}
		
		ITERATELIST(an, &data->annotationlist)
		{
			//pdfConvertDeviceToUser(data->doc, data->page, mn->x1, mn->y1, &mn->ux1, &mn->uy1);
			//pdfConvertDeviceToUser(data->doc, data->page, mn->x2, mn->y2, &mn->ux2, &mn->uy2);
			
			float ox1 = an->x1 - 0.5f, oy1 = an->y1 - 0.5f;
			float ox2 = an->x2 - 0.5f, oy2 = an->y2 - 0.5f;
			float x1, x2, y1, y2;
			float t;
			
			if (data->rotation == 0)
			{
				x1 = ox1; y1 = oy1;
				x2 = ox2; y2 = oy2;
			}					
			else if (data->rotation == 1)
			{
				x1 = -oy1; y1 = ox1;
				x2 = -oy2; y2 = ox2;
			}
			else if (data->rotation == 2)
			{
				x1 = -ox1; y1 = -oy1;
				x2 = -ox2; y2 = -oy2;
			}	
			else /*(data->rotation == 3)*/
			{
				x1 = oy1; y1 = -ox1;
				x2 = oy2; y2 = -ox2;
			}											
			
			if (x1 > x2) /*can happen after rotation */
			{
				t = x1;
				x1 = x2;
				x2 = t;
			}
			if (y1 < y2)
			{
				t = y1;
				y1 = y2;
				y2 = t;
			}
			
			x1 += 0.5f; y1 += 0.5f;
			x2 += 0.5f; y2 += 0.5f;
			
			an->ux1 = x1 * data->image->width;
			an->uy1 = y1 * data->image->height;
			an->ux2 = x2 * data->image->width;
			an->uy2 = y2 * data->image->height;
			
			
			SetAttrs(an->obj, MUIA_Annotation_PosX, an->ux1, MUIA_Annotation_PosY, an->uy1, TAG_DONE);
		}
	
		/* lasso. ignore page rotation as it can't change when marker is enabled */
	
		{
			struct markernode *mn = &data->lasso.marker;
			//pdfConvertDeviceToUser(data->doc, data->page, mn->x1, mn->y1, &mn->ux1, &mn->uy1);
			//pdfConvertDeviceToUser(data->doc, data->page, mn->x2, mn->y2, &mn->ux2, &mn->uy2);
			
			float ox1 = mn->x1 - 0.5f, oy1 = mn->y1 - 0.5f;
			float ox2 = mn->x2 - 0.5f, oy2 = mn->y2 - 0.5f;
			float x1, x2, y1, y2;
			float t;
			
			x1 = ox1; y1 = oy1;
			x2 = ox2; y2 = oy2;
						
			if (x1 > x2) /*can happen for lasso */
			{
				t = x1;
				x1 = x2;
				x2 = t;
			}
			if (y1 < y2)
			{
				t = y1;
				y1 = y2;
				y2 = t;
			}
			
			x1 += 0.5f; y1 += 0.5f;
			x2 += 0.5f; y2 += 0.5f;
			
			mn->ux1 = x1 * data->image->width;
			mn->uy1 = y1 * data->image->height;
			mn->ux2 = x2 * data->image->width;
			mn->uy2 = y2 * data->image->height;
			mn->valid = TRUE;
		}
		
	}
	//pdfRelease(data->doc);
}

////

///	set

DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{

		case MUIA_PageView_LayoutWidth:
			data->layoutwidth = tag->ti_Data;
			data->invalid = TRUE;
			break;

		case MUIA_PageView_LayoutHeight:
			data->layoutheight = tag->ti_Data;
			data->invalid = TRUE;
			break;

		case MUIA_PageView_Page:
			data->page = tag->ti_Data;
			break;

		case MUIA_PageView_PDFReady:
			data->ready = tag->ti_Data;
			break;

		case MUIA_PageView_Quiet:
			data->quiet = tag->ti_Data;
			break;
			
		case MUIA_PageView_PDFBitmap:
		
			/* delete old image if present and create new. can NOT refresh display! */

			ObtainSemaphore(&data->sema);

			if (tag->ti_Data == NULL)
			{
				if (data->image != NULL)
				{
					mfree(data->image->data);
					data->image->data = NULL;
				}
			}
			else
			{
				if (data->image == NULL)
				{
					struct pdfBitmap *bm = (struct pdfBitmap*)tag->ti_Data;

					data->image = calloc(1, sizeof(ThbImage));

					data->region.x1 = 0;
					data->region.y1 = 0;
					data->region.x2 = bm->width - 1;
					data->region.y2 = bm->height - 1;
				}

				if (data->image != NULL)
				{
					struct pdfBitmap *bm = (struct pdfBitmap*)tag->ti_Data;
					int isgrayscale = TRUE;
					int x, y;
					
					for(y=0; y<bm->height; y++)
					{
						unsigned char *src = bm->data + bm->stride * y;
						unsigned int rowdiff = 0;
						for(x=bm->width; x>0; x--)
						{
							int r = src[1];
							int g = src[2];
							int b = src[3];
							int different = (r | g | b)  - (r & g & b);	// hopefuly this works and is faster than two conditions
																		// when equal both terms should be equal so diff == 0	
																		// also, when different it will always be > 0
																		
							rowdiff += different;	
							src += 4;					
						}
						
						if (rowdiff != 0)
						{
							isgrayscale = FALSE;
							break;
						}						
					}
					
					D(kprintf("image is grayscale:%d\n", isgrayscale));
										
					data->image->width = bm->width;
					data->image->height = bm->height;
					data->image->bpp = isgrayscale ? 1 : 4;
					data->image->data = mmalloc(bm->width * bm->height * data->image->bpp);
					if (data->image->data != NULL)
					{
						int i, j;
						for(i=0; i<bm->height; i++)
						{
							unsigned char *src = bm->data + bm->stride * i;
							if (isgrayscale == FALSE)
								memcpy((unsigned int*)data->image->data + i * bm->width, src, bm->width * 4);
							else
							{
								unsigned char *dst = data->image->data + data->image->width * i;
								for(j=bm->width; j>0; j--)
								{
									*dst++ = src[1];
									src += 4;
								}
							}
						}
					}
				}

				updatemarkers(obj, data, TRUE);
				renderselectionregion(obj, data, data->lasso.oldregion);
			}
				
			ReleaseSemaphore(&data->sema);

			break;
			
		case MUIA_PageView_Rotation:
			
			data->rotation = tag->ti_Data;
			data->invalid = TRUE;
			break;

	}
	NEXTTAG;

	return(DOSUPER);
}

////

///	get

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		case MUIA_PageView_Width:
			*(msg->opg_Storage) = (ULONG)(data->image ? data->image->width : 0);
			return TRUE;

		case MUIA_PageView_Height:
			*(msg->opg_Storage) = (ULONG)(data->image ? data->image->height : 0);
			return TRUE;

		case MUIA_PageView_LayoutWidth:
			*(msg->opg_Storage) = (ULONG)(data->layoutwidth);
			return TRUE;

		case MUIA_PageView_LayoutHeight:
			*(msg->opg_Storage) = (ULONG)(data->layoutheight);
			return TRUE;

		case MUIA_PageView_Region:
			*(msg->opg_Storage) = (ULONG)&data->region;
			return TRUE;

		case MUIA_PageView_Page:
			*(msg->opg_Storage) = (ULONG)data->page;
			return TRUE;

		case MUIA_PageView_MediaWidth:
			*(msg->opg_Storage) = (ULONG)data->mediawidth;
			return TRUE;

		case MUIA_PageView_MediaHeight:
			*(msg->opg_Storage) = (ULONG)data->mediaheight;
			return TRUE;

		case MUIA_PageView_RedirectPage:
			return TRUE;

		case MUIA_PageView_NeedRefresh:
			return TRUE;

		case MUIA_PageView_RenderWidth:
			*(msg->opg_Storage) = xget(obj, MUIA_Width);
			return TRUE;

		case MUIA_PageView_RenderHeight:
			*(msg->opg_Storage) = xget(obj, MUIA_Height) - getinformationspace(obj, data);
			return TRUE;
			
		case MUIA_PageView_Rotation:
			*(msg->opg_Storage) = data->rotation;
			return TRUE;

	}

	return(DOSUPER);
}

////

///	update

DEFMMETHOD(PageView_Update)
{
	GETDATA;

	data->refreshrect.MinX = msg->x;
	data->refreshrect.MaxX = msg->x + msg->width;
	data->refreshrect.MinY = msg->y;
	data->refreshrect.MaxY = msg->y + msg->height;

	MUI_Redraw(obj, MADF_DRAWUPDATE);

	data->refreshrect.MinX = -1;
	return 0;
}

////

///	handleevent

METHOD riHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
	#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
	#define _isinobject(obj,x,y) (_between(vl,(x),vl+vw) && _between(vt,(y),vt+vh))
	#define _isinimage(obj,x,y) (_between(vl + data->offsetx, (x), vl + vw - data->offsetx) && _between(vt + data->offsety,(y), vt + vh - data->offsety))

	GETDATA;
	int vl = _mleft(obj);
	int vt = _mtop(obj);
	int vw = _mwidth(obj);
	int vh = _mheight(obj);

	if (msg->imsg != NULL && _isinobject(obj, msg->imsg->MouseX, msg->imsg->MouseY)/* || data->eh.ehn_Events & IDCMP_MOUSEMOVE*/)
	{
		if (msg->imsg != NULL)
		{
			switch (msg->imsg->Class)
			{
				case IDCMP_MOUSEBUTTONS:
				{
					int mx = max(0, msg->imsg->MouseX - _left(obj) - data->offsetx);
					int my = max(0, msg->imsg->MouseY - _top(obj) - data->offsety);

					if (msg->imsg->Code == SELECTDOWN)
					{
						if (_isinimage(obj, msg->imsg->MouseX, msg->imsg->MouseY))
						{
							data->mousedown = TRUE;
							if (data->ispreview == FALSE && data->lasso.enabled == FALSE && xget(objFindContainerByAttribute(obj, MUIA_DocumentView_DragAction), MUIA_DocumentView_DragAction) == MUIV_DocumentView_DragAction_Mark) /* first coord */
							{
								/* clear selection on all previous pages */
								
								DoMethod(objFindContainerByAttribute(obj, MUIA_DocumentView_DragAction), MUIM_DocumentView_ClearSelection);							
							
								data->lasso.marker.x1 = data->lasso.marker.x2 = (float)mx / data->image->width;
								data->lasso.marker.y1 = data->lasso.marker.y2 = (float)my / data->image->height;
								data->lasso.enabled = TRUE;
							}
						}
					}
					else if (msg->imsg->Code == SELECTUP)
					{
						if (data->lasso.enabled)
						{
							data->lasso.enabled = FALSE;
						}
						else if (data->ready && !data->ispreview)
						{
							double x = mx;
							double y = my;
							void *link = pdfFindLink(data->doc, data->page, x, y);
							if (link != NULL)
							{
								int page = pdfGetActionPageFromLink(data->doc, link);
								//printf("find page:%d\n", page);
								//char *url = linkGetActionURL(link);
								if (page > 0)
									set(_parent(obj), MUIA_PageView_RedirectPage, page);
							}
							
							/* lookup annotation and toggle */
							if (!ISLISTEMPTY(&data->annotationlist))
							{
								struct annotationnode *an;
								ITERATELIST(an, &data->annotationlist)
								{
									//printf("checking %d, %d vs %d,%d  %d,%d\n", mx, my, an->ux1, an->uy1, an->ux2, an->uy2);
									if (_between(an->ux1, mx, an->ux2) && _between(an->uy2, my, an->uy1))
									{
										DoMethod(an->obj, MUIM_Annotation_Toggle);
										break;
									}
								}
							}
							
						}
						else if (data->ispreview && data->mousedown) /* for page preview, like in outlines */
						{
							set(_parent(obj), MUIA_PageView_RedirectPage, data->page);
						}

						data->mousedown = TRUE;
					}
					break;
				}

				case IDCMP_MOUSEMOVE:
				{
					int mx = max(0, msg->imsg->MouseX - _left(obj) - data->offsetx);
					int my = max(0, msg->imsg->MouseY - _top(obj) - data->offsety);
					
					if (data->lasso.enabled)
					{
						struct pdfSelectionRegion *selection;
						
						data->lasso.marker.x2 = min(1.0, (float)mx / data->image->width);
						data->lasso.marker.y2 = min(1.0, (float)my / data->image->height);
						
						/* fetch selection from pdf document */
						
						if (data->lasso.oldregion != NULL)
						{
							renderselectionregion(obj, data, data->lasso.oldregion);
							pdfDisposeRegionForSelection(data->doc, data->lasso.oldregion);
						}
						
						selection = pdfBuildRegionForSelection(data->doc, data->page, data->lasso.marker.x1, data->lasso.marker.y1, data->lasso.marker.x2, data->lasso.marker.y2, NULL);
						if (selection != NULL)
						{
							renderselectionregion(obj, data, selection);							
							data->lasso.oldregion = selection;
						}
						
						updatemarkers(obj, data, TRUE);
						MUI_Redraw(obj, MADF_DRAWUPDATE);
						return MUI_EventHandlerRC_Eat;
					}
					#if 0
					/* find area over which we are and setup appropriate pointer */
					if (data->hasregion && data->moveregion == 0)
					{
						int handle = rectFindClosestHandle(data, &data->region, mx, my, 0);
						int cursormapping[9] = {
							POINTERTYPE_DIAGONALRESIZE2,
							POINTERTYPE_DIAGONALRESIZE1,
							POINTERTYPE_DIAGONALRESIZE2,
							POINTERTYPE_DIAGONALRESIZE1,
							POINTERTYPE_VERTICALRESIZE,
							POINTERTYPE_HORIZONTALRESIZE,
							POINTERTYPE_VERTICALRESIZE,
							POINTERTYPE_HORIZONTALRESIZE,
							POINTERTYPE_MOVE};

						if (_isinimage(obj, msg->imsg->MouseX, msg->imsg->MouseY))
						{
							if (data->pointertype != cursormapping[handle - 1])
								SetWindowPointer(_window(obj), WA_PointerType, data->pointertype = cursormapping[handle - 1], TAG_DONE);
						}
						else
							SetWindowPointer(_window(obj), WA_PointerType, data->pointertype = POINTERTYPE_NORMAL, TAG_DONE);
					}
					else if (data->hasregion)
					{
						rectSetCorner(data, &data->region, data->moveregion, mx, my, 0);
						set(obj, MUIA_PageView_Region, (ULONG)&data->region);
					}
					else if (!data->hasregion && data->ready)
					{
						double x = mx;
						double y = my;
						void *link = NULL;

						//link = pdfFindLink(data->doc, data->page, x, y);

						if (link != NULL)
						{
							//char *description = linkGetDescription(link);
						}
						else
						{
							//kprintf("mouse:%d,%d\n", (int)x, (int)y);
						}
					}
					#endif
					break;
				}
			}
		}
	}

	return(DOSUPER);
}

////

///	backfill

DEFMMETHOD(Backfill)
{
	//printf("%d, %d, %d, %d\n", msg->left, msg->top, msg->right, msg->bottom);
	return 0;//DOSUPER;
}

////

///	hide

DEFMMETHOD(Hide)
{
	GETDATA;

	/* hide is a bit tricky in virtgroups. it is called while scrolling so
	   we just push the method and if show is called right after it then flush
	   is canceled.*/

#if defined(__AROS__)
    kprintf("[Pageview::Hide] not implemented\n");
    // FIXME: implement MUIV_PushMethod_Delay in Zune/AROS
	data->flushmethodid = DoMethod(_app(obj), MUIM_Application_PushMethod, obj, 1, MUIM_PageView_Flush);
#else
	data->flushmethodid = DoMethod(_app(obj), MUIM_Application_PushMethod, obj, 1 | MUIV_PushMethod_Delay(150), MUIM_PageView_Flush);
#endif
	D(kprintf("cleanup:%d\n", data->page));
	return DOSUPER;
}

////

///	show

DEFMMETHOD(Show)
{
	GETDATA;

	if (data->flushmethodid != 0)
	{
		DoMethod(_app(obj), MUIM_Application_UnpushMethod, NULL, data->flushmethodid, 0);
		data->flushmethodid = 0;
	}

	if ((1) && data->eh.ehn_Object == NULL)
	{
		data->eh.ehn_Object = obj;
		data->eh.ehn_Class  = cl;
		data->eh.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE;
		data->eh.ehn_Priority = 100;
		data->eh.ehn_Flags = MUI_EHF_GUIMODE;

		DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->eh);
	}  

	return DOSUPER;
}

////

///	flush

DEFMMETHOD(PageView_Flush)
{
	GETDATA;

	ObtainSemaphore(&data->sema);

	if (data->image != NULL)
	{
		mfree(data->image->data);
		data->image->data = NULL;
	}

	if (data->eh.ehn_Object != NULL)
	{
		DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->eh);
		data->eh.ehn_Object = NULL;
	}
	
	data->flushmethodid = 0;
	ReleaseSemaphore(&data->sema);

	return 0;
}

////

///	addmarker

DEFMMETHOD(PageView_AddMarker)
{
	GETDATA;
	unsigned int id = 0;

	struct markernode *mn;

	if (msg->id != MUIV_PageView_AddMarker_New)
	{
		int found = FALSE;
		ITERATELIST(mn, &data->markerlist)
		{
			if (mn->id == msg->id)
			{
				found = TRUE;
				id = msg->id;
				break;
			}
		}

		if (found == FALSE)
			mn = NULL;
	}
	else
	{
		mn = calloc(1, sizeof(*mn));
	}

	if (mn != NULL)
	{
		mn->x1 = msg->coords[0];
		mn->y1 = msg->coords[1];
		mn->x2 = msg->coords[2];
		mn->y2 = msg->coords[3];
		mn->color = msg->color;
		mn->id = id;
		ADDTAIL(&data->markerlist, mn);
		updatemarkers(obj, data, FALSE);
	}

	return id;
}

////

///	removemarker

DEFMMETHOD(PageView_RemoveMarker)
{
	GETDATA;

	/* TODO: add single marker removal */

	while(!ISLISTEMPTY(&data->markerlist))
	{
		struct markernode *mn = (struct markernode*)GetHead(&data->markerlist);
		REMOVE(mn);
		free(mn);
	}            

	return 0;
}

////

///	addannotation

DEFMMETHOD(PageView_AddAnnotation)
{
	GETDATA;

	struct annotationnode *an;
	an = calloc(1, sizeof(*an));

	if (an != NULL)
	{
		an->x1 = msg->coords[0];
		an->y1 = msg->coords[1];
		an->x2 = msg->coords[2];
		an->y2 = msg->coords[3];
		an->obj = msg->obj;
		ADDTAIL(&data->annotationlist, an);
		updatemarkers(obj, data, FALSE);
	}

	return an != NULL;
}

////

/// removeannotation

DEFMMETHOD(PageView_RemoveAnnotation)
{
	GETDATA;
	struct annotationnode *an;
	
	ITERATELIST(an, &data->annotationlist)
	{
		if (an->obj == msg->obj)
		{	
			REMOVE(an);
			free(an);
			break;
		}
	}
	
	return TRUE;
}

////

/// clearselection

DEFMMETHOD(PageView_ClearSelection)
{
	GETDATA;
	
	if (data->lasso.oldregion != NULL)
	{
		renderselectionregion(obj, data, data->lasso.oldregion);
		pdfDisposeRegionForSelection(data->doc, data->lasso.oldregion);
		
		if (data->image != NULL && data->image->data != NULL) /* also have to refresh here! */
			MUI_Redraw(obj, MADF_DRAWUPDATE);
	}
	data->lasso.oldregion = NULL;
	data->lasso.marker.x2 = data->lasso.marker.x1;
	data->lasso.marker.y2 = data->lasso.marker.y1;
	
	
	return TRUE;
}

////

/// clearselection

DEFMMETHOD(PageView_GetSelection)
{
	GETDATA;
	
	if (data->lasso.oldregion == NULL)
		return FALSE;
	
	msg->region.x1 = data->lasso.marker.x1; msg->region.y1 = data->lasso.marker.y1;
	msg->region.x2 = data->lasso.marker.x2; msg->region.y2 = data->lasso.marker.y2;	
	
	return TRUE;
}

////

BEGINMTABLE
	case MUIM_Draw:				return(riDraw(cl,obj,(APTR)msg));
	case MUIM_Setup:			return(riSetup(cl,obj,(APTR)msg));
	case MUIM_Cleanup:			return(riCleanup(cl,obj,(APTR)msg));
	case MUIM_HandleEvent:		return(riHandleEvent(cl,obj,(APTR)msg));
	case MUIM_AskMinMax:		return(ThbAskMinMax(cl,obj,(APTR)msg));
	DECSET
	DECGET
	DECNEW
	DECDISP
	DECMMETHOD(Hide)
	DECMMETHOD(Show)
	DECMMETHOD(Backfill)
	DECMMETHOD(PageView_AddMarker)
	DECMMETHOD(PageView_AddAnnotation)
	DECMMETHOD(PageView_RemoveMarker)
	DECMMETHOD(PageView_RemoveAnnotation)
	DECMMETHOD(PageView_Update)
	DECMMETHOD(PageView_Flush)
	DECMMETHOD(PageView_ClearSelection)
	DECMMETHOD(PageView_GetSelection)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Area, PageViewClass)

