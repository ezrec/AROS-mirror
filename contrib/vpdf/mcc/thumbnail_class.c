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

#include "system/chunky.h"
#include "system/memory.h"
#include "system/functions.h"

#include "util.h"
#include "poppler.h"

typedef	struct ThbImage_s
{
	unsigned char *data;
	short width;
	short height;
} ThbImage;

struct Data
{
	ThbImage *image;
	struct MUI_EventHandlerNode eh;
	int mx, my;
	int moveregion;
	int markmx, markmy;
	int markAspect;

	int page;
	int ready;
	int prevwidth, prevheight;
	int mediawidth, mediaheight;
	int invalid;

	struct MinList markerlist;
	unsigned int lastmarkerid;

	struct SignalSemaphore sema;
};


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

	/* dispose parent class */

	return(DOSUPER);
}

////

///	askminmax

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

	msg->MinMaxInfo->MinHeight += data->layoutheight;
	msg->MinMaxInfo->DefHeight += data->layoutheight;
	msg->MinMaxInfo->MaxHeight = MUI_MAXMAX;

	return(0);
}

////

///	new

DEFNEW
{
	obj = DoSuperNew(cl, obj, MUIA_FillArea, FALSE,
								TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;

		memset(data, 0, sizeof(struct Data));

		InitSemaphore(&data->sema);
		NEWLIST(&data->markerlist);

		/* get properties */

		data->refreshrect.MinX = -1;
		data->layoutwidth = 128;
		data->layoutheight = 128;

		FORTAG(INITTAGS)
		{
				case MUIA_PageView_Image:
				{
					ChkImage *img = (ChkImage*)tag->ti_Data;

					if (img != NULL)
					{
						/* convert image into optimized form */

						ThbImage *timg = calloc(1, sizeof(ThbImage));

						timg->width = img->width;
						timg->height = img->height;
						timg->data = malloc(timg->width * timg->height * 3);

						if (timg->data != NULL)
							memcpy(timg->data, img->data.p, timg->width * timg->height * 3);
						else
						{
							free(timg);
							timg = NULL;
						}

						data->image = timg;
					}
					break;
				}
				case MUIA_PageView_HasRegion:
					data->hasregion = tag->ti_Data;
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

    /* path string event handler */

	if (1 || data->hasregion)
	{
		data->eh.ehn_Object = obj;
		data->eh.ehn_Class  = cl;
		data->eh.ehn_Events = IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE;
		data->eh.ehn_Priority = 100;
		data->eh.ehn_Flags = 0;//MUI_EHF_GUIMODE;

		DoMethod(_win(obj), MUIM_Window_AddEventHandler, &data->eh);
	}

	return TRUE;
}

////

///	cleanup

METHOD riCleanup(struct IClass *cl,Object *obj,Msg msg)
{
	GETDATA;

	if (data->eh.ehn_Object != NULL)
		DoMethod(_win(obj), MUIM_Window_RemEventHandler, &data->eh);

	return(DOSUPER);
}

////

///	draw

void kprintf(char *fmt,...);

METHOD riDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	unsigned int rc = DOSUPER;

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
			prvheight = _mheight(obj);


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

			if (data->refreshrect.MinX != -1)
			{
				width = data->refreshrect.MaxX - data->refreshrect.MinX;
				height = data->refreshrect.MaxY - data->refreshrect.MinY;
				WritePixelArray(image->data, data->refreshrect.MinX, data->refreshrect.MinY, image->width * 4, _rp(obj), x0 + data->refreshrect.MinX, y0 + data->refreshrect.MinY, width, height, RECTFMT_ARGB);
			}
			else
				WritePixelArray(image->data, 0, 0, image->width * 4, _rp(obj), x0, y0, width, height, RECTFMT_ARGB);

			/* draw remaining area with background */

			if ((image->width < _mwidth(obj) || image->height < _mheight(obj)))
			{
				int lb = x0 - _mleft(obj);
				int rb = _mwidth(obj) - image->width - lb;
				int tb = y0 - _mtop(obj);
				int bb = _mheight(obj) - image->height - tb;

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
			}

			/* draw active page marker (TODO: make it flickerfree) */

			if (data->page == activepage)
			{
				SetRPAttrs(_rp(obj), RPTAG_PenMode, FALSE, RPTAG_FgColor, 0xffff0000, TAG_DONE);
				Move(_rp(obj), x0, y0);
				Draw(_rp(obj), x0 + width - 1, y0);
				Draw(_rp(obj), x0 + width - 1, y0 + height - 1);
				Draw(_rp(obj), x0, y0 + height - 1);
				Draw(_rp(obj), x0, y0);
			}

			/* draw selection markers (TODO: make it flickerfree) */

			if (!ISLISTEMPTY(&data->markerlist))
			{
				struct markernode *mn;
				ITERATELIST(mn, &data->markerlist)
				{
					SetRPAttrs(_rp(obj), RPTAG_PenMode, FALSE, RPTAG_FgColor, 0xffff0000, TAG_DONE);
					Move(_rp(obj), x0 + mn->ux1, y0 + mn->uy1);
					Draw(_rp(obj), x0 + mn->ux2, y0 + mn->uy1);
					Draw(_rp(obj), x0 + mn->ux2, y0 + mn->uy2);
					Draw(_rp(obj), x0 + mn->ux1, y0 + mn->uy2);
					Draw(_rp(obj), x0 + mn->ux1, y0 + mn->uy1);
					ProcessPixelArray(_rp(obj), x0 + mn->ux1 - 1, y0 + mn->uy2 - 1, mn->ux2 - mn->ux1, mn->uy1 - mn->uy2, POP_DARKEN, 10, NULL);

				}
			}

		}
		else
		{
			SetAPen(_rp(obj), 1);
			RectFill(_rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));
			//DoMethod(obj, MUIM_DrawBackground, _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj), 0, 0, 0);
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

///	set

DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{

		case MUIA_Thumbnail_Image:
		
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
					ChkImage *img = (ChkImage*)tag->ti_Data;

					data->image = calloc(1, sizeof(ThbImage));

					data->region.x1 = 0;
					data->region.y1 = 0;
					data->region.x2 = img->width - 1;
					data->region.y2 = img->height - 1;
				}

				if (data->image != NULL)
				{
					ChkImage *img = (ChkImage*)tag->ti_Data;

					data->image->width = img->width;
					data->image->height = img->height;
					data->image->data = mmalloc(img->width * img->height * 4);
					if (data->image->data != NULL)
						memcpy(data->image->data, img->data.p, img->width * img->height * 4);
				}

				updatemarkers(obj, data, TRUE);
			}
				
			ReleaseSemaphore(&data->sema);

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

static void rectSetCorner(struct Data *data, struct PageViewRegion *r, int corner, int x, int y, int aspect)
{
	if (corner >= 1 && corner <= 9)
	{
		switch (corner)
		{
			case 1:
				r->x1 = x;
				r->y1 = y;
				break;
			case 2:
				r->x2 = x;
				r->y1 = y;
				break;
			case 3:
				r->x2 = x;
				r->y2 = y;
				break;
			case 4:
				r->x1 = x;
				r->y2 = y;
				break;
			case 5:
				r->y1 = y;
				break;
			case 6:
				r->x2 = x;
				break;
			case 7:
				r->y2 = y;
				break;
			case 8:
				r->x1 = x;
				break;
			case 9: // special case for whole area movement
			{
				int dx = x - data->markmx;
				int dy = y - data->markmy;

				if (dx < 0) // clip to left
					if (r->x1 < -dx)
						dx = -r->x1;

				if (dy < 0) // clip to top
					if (r->y1 < -dy)
						dy = -r->y1;

				if (dx > 0) // clip to right
					if (r->x2 + dx >= data->image->width)
						dx = data->image->width - r->x2 - 1;

				if (dy > 0) // clip to bottom
					if (r->y2 + dy >= data->image->height)
						dy = data->image->height - r->y2 - 1;

				r->x1 += dx;
				r->x2 += dx;
				r->y1 += dy;
				r->y2 += dy;

				data->markmx = x;
				data->markmy = y;
			}
			break;
		}
	}
	if (aspect)
	{
		int width, height;

		width = r->x2 - r->x1 + 1;
		height = r->y2 - r->y1 + 1;

		switch (corner)
		{
			case 1:
				r->x1 = r->x2 - width;
				r->y1 = r->y2 - height;
				break;
			case 2:
				r->x2 = r->x1 + width;
				r->y1 = r->y2 - height;
				break;
			case 3:
				r->x2 = r->x1 + width;
				r->y2 = r->y1 + height;
				break;
			case 4:
				r->x1 = r->x2 - width;
				r->y2 = r->y1 + height;
				break;
		}

		//width = data->image->width * (r->y2 - r->y1 + 1) / data->image->height;
		//height = data->image->height * (r->x2 - r->x1 + 1) / data->image->width;

		switch (corner)
		{
			case 5:
			case 7:
				r->x2 = r->x1 + (height / data->markAspect);
				break;

			case 6:
			case 8:
				r->y2 = r->y1 + (width * data->markAspect);
				break;
		}
	}

	if (r->x2 < r->x1)
		r->x2 = r->x1;
	if (r->y2 < r->y1)
		r->y2 = r->y1;
}

static int rectFindClosestHandle(struct Data *data, struct PageViewRegion *r, int x, int y, int aspect)
{
	int	d[9];
	int i, m = INT_MAX, ind = 0;

	d[0] = iabs(r->x1 - x) + iabs(r->y1 - y);
	d[1] = iabs(r->x2 - x) + iabs(r->y1 - y);
	d[2] = iabs(r->x2 - x) + iabs(r->y2 - y);
	d[3] = iabs(r->x1 - x) + iabs(r->y2 - y);
	d[4] = iabs(((r->x1 + r->x2) / 2) - x) + iabs(r->y1 - y);
	d[5] = iabs(r->x2 - x) + iabs((r->y1 + r->y2) / 2 - y);
	d[6] = iabs(((r->x1 + r->x2) / 2) - x) + iabs(r->y2 - y);
	d[7] = iabs(r->x1 - x) + iabs((r->y1 + r->y2) / 2 - y);
	d[8] = iabs(((r->x1 + r->x2) / 2) - x) + iabs((r->y1 + r->y2) / 2 - y);

	/* closest */

	for(i = 0; i < 9; i++)
	{
		if (d[i] < m)
		{
			m = d[ i ];
			ind = i;
		}
	}

	ind++;

	data->markmx = x;
	data->markmy = y;
	return ind;
}

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

	if (_isinobject(obj, msg->imsg->MouseX, msg->imsg->MouseY)/* || data->eh.ehn_Events & IDCMP_MOUSEMOVE*/)
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
							if (data->hasregion)
							{
								data->mx = mx;
								data->my = my;
								data->moveregion = rectFindClosestHandle(data, &data->region, mx, my, 0);
								rectSetCorner(data, &data->region, data->moveregion, mx, my, 0);

								set(obj, MUIA_PageView_Region, (ULONG)&data->region);
							}
						}
					}
					else if (msg->imsg->Code == SELECTUP)
					{
						if (data->hasregion)
						{
							data->moveregion = 0;
						}
						else if (data->ready)
						{
							double x = mx;
							double y = my;
							void *link = pdfFindLink(data->doc, data->page, x, y);
							if (link != NULL)
							{
								int page = pdfGetActionPageFromLink(data->doc, link);
								printf("find page:%d\n", page);
								//char *url = linkGetActionURL(link);
								if (page > 0)
									set(_parent(obj), MUIA_PageView_RedirectPage, page);
							}
						}
					}
					break;
				}

				case IDCMP_MOUSEMOVE:
				{
					int mx = max(0, msg->imsg->MouseX - _left(obj) - data->offsetx);
					int my = max(0, msg->imsg->MouseY - _top(obj) - data->offsety);
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

///	hide

DEFMMETHOD(Hide)
{
	GETDATA;

	/* TODO: move page to the cache. */

	ObtainSemaphore(&data->sema);

	if (data->image != NULL)
	{
		mfree(data->image->data);
		data->image->data = NULL;
	}

	ReleaseSemaphore(&data->sema);

	return DOSUPER;
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
	DECMMETHOD(Thumbnail_Update)

ENDMTABLE

DECSUBCLASS_NC(MUIC_Area, ThumbnailClass)

