
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <clib/alib_protos.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <private/vapor/vapor.h>
#include "lay_class.h"

#include "util.h"

struct Data
{
	int	n;  /* number of children */
	int	w;  /* width of children  */
	int	h;  /* height of children */
	int	hs; /* horiz spacing      */
	int	vs; /* vert spacing       */

	int columns;

	int width, height;
	int center;
	int vertdist;
	int childcount;
	int keeppos;
};

MUI_HOOK(layoutfunc, APTR grp, struct MUI_LayoutMsg *lm)
{
	struct IClass *cl = (struct IClass*)(h->h_Data);
	struct Data *data = INST_DATA(cl, grp);
	float vgpos = (float)xget(grp, MUIA_Virtgroup_Top) / xget(grp, MUIA_Virtgroup_Height);

    switch (lm->lm_Type)
    {
        case MUILM_MINMAX:
        {
			register Object *child;
			Object *cstate;
			unsigned int n, done = FALSE;

			// look for first visible child dimensions (reference)

			for (n = 0, cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); n++)
            {
                ULONG sm;

                get(child,MUIA_ShowMe,&sm);
                if (!sm) continue;

                if (!done)
                {
                    data->w = _minwidth(child);
                    data->h = _minheight(child);

                    done = TRUE;
                }
            }

            data->n = n;

			lm->lm_MinMax.MinWidth  = data->w;
			lm->lm_MinMax.MinHeight = data->h;
            lm->lm_MinMax.DefWidth  = data->w;
            lm->lm_MinMax.DefHeight = data->h;
            lm->lm_MinMax.MaxWidth  = MUI_MAXMAX;
            lm->lm_MinMax.MaxHeight = MUI_MAXMAX;

            return 0;
        }

        case MUILM_LAYOUT:
        {
            register Object *child;
			Object *cstate;
			int	il, it, ir, ib, w, h, c, r, x, y, n, nd, wd, hd, hsd, vsd;
			int hborder;
			int count = 0;
			int height;

			get(grp, MUIA_InnerLeft, &il);
			get(grp, MUIA_InnerTop, &it);
			get(grp, MUIA_InnerRight, &ir);
			get(grp, MUIA_InnerBottom, &ib);

            il = it = ir = ib = 0;

			get(grp, MUIA_Width, &w);
			get(grp, MUIA_Height, &h);

			w = _mwidth(grp);
			h = _mheight(grp);

            nd  = data->n;
            wd  = data->w;
            hd  = data->h;
            hsd = data->hs;
            vsd = data->vs;

            for (c = 1; ; c++)
            {
                x = il+hsd*(c-1)+c*wd+ir;

				if (x == w) break;
                
                if (x>w)
                {
					if (c > 1)
						c--;

                    break;
                }
            }

            x = il;
            y = it;

			if (data->center)
				hborder = ( w - ( c - 1 ) * hsd - c * wd ) / 2;
			else
				hborder = 0;

            r = nd/c;
			if (nd%c>0)
				r++;

            height = it+r*hd+vsd*(r-1)+ib;

			/* keep focus on same entry */

			if (data->keeppos)
			{
				nfset(grp, MUIA_Virtgroup_Top, (int)(height * vgpos));
				data->keeppos = FALSE;
			}

			for (n = 0, cstate = (Object *)lm->lm_Children->mlh_Head; (child = NextObject(&cstate)); )
            {
				//ULONG sm;

				//get(child,MUIA_ShowMe,&sm);
				//if (!sm) continue;

				if (!MUI_Layout(child,x + hborder,y,wd,hd,0))
					return FALSE;

                n++;

				if (n == c)
                {
                    x =  il;
                    y += hd+vsd;
					n = 0;
                }
                else
                {
                    x += wd+hsd;
                }

				count++;
            }

            lm->lm_Layout.Width  = il+c*wd+hsd*(c-1)+ir;
			lm->lm_Layout.Height = height;

			data->columns = c;
			data->vertdist = hd + vsd;
			data->childcount = count;

            return TRUE;
        }
    }

    return MUILM_UNKNOWN;
}

DEFNEW
{
	obj = DoSuperNew(cl,obj,
			MUIA_Group_LayoutHook, (ULONG)&layoutfunc_hook,
			MUIA_CustomBackfill, TRUE,
			MUIA_Group_SameSize, TRUE,
			TAG_MORE, INITTAGS);

	layoutfunc_hook.h_Data = cl;

	if (obj != NULL)
    {
		GETDATA;

		data->hs = GetTagData(MUIA_Lay_HorizSpacing, 4, INITTAGS);
		data->vs = GetTagData(MUIA_Lay_VertSpacing, 4, INITTAGS);
		data->center = GetTagData(MUIA_Lay_Center, TRUE, INITTAGS);
		data->columns = 1;

    }

    return (ULONG)obj;
}

void kprintf(char *fmt, ...);
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

static void *buildregionfromicons(struct Data *data, Object *obj, int x1, int y1, int x2, int y2)
{
	struct Region *clipregion = NewRegion();

	if (clipregion != NULL)
	{
		int vpos = xget(obj, MUIA_Virtgroup_Top);
		int vtop = _top(obj) + vpos;
		struct Rectangle r;
		int columns = data->columns;
		int n = 0;
		int cwidth = 0, cheight = 0;
		int cleft = 0;
		int ctop = 0;
		int i;
		int count = data->childcount;

		void *first = (void*)DoMethod(obj, MUIM_Group_GetChild, MUIV_Group_GetChild_First);

		/* initial region */

		r.MinX = x1;
		r.MinY = y1;
		r.MaxX = x2;
		r.MaxY = y2;
		OrRectRegion(clipregion, &r);

		/* first child position */

		if (first != NULL)
		{
			ctop = xget(first, MUIA_TopEdge) + vpos;
			cleft = _left(first);
			cwidth = _width(first);
			cheight = _height(first);
		}
		else
		{
			return clipregion;
		}

		{
			int skip;

			/* skip entries which are avobe top */

			skip = (vtop - ctop) / (data->vertdist);
			ctop += skip * data->vertdist;
			count -= columns * skip;

			/* calculate number of remaining entries we need to process */

			count = min(count, columns * ( 1 + ceil((float)xget(obj, MUIA_Height) / data->vertdist)));
		}

		for(i = 0; i<count; i++)
		{
			if (n == columns)
			{
				ctop += data->vertdist;
				cleft = _left(first);
				n = 0;
			}

			r.MinX = cleft;
			r.MinY = ctop - vpos;
			r.MaxX = r.MinX + cwidth - 1;
			r.MaxY = ctop + cheight - 1 - vpos;

			/* remove the area */
			ClearRectRegion(clipregion, &r);

			cleft += cwidth + data->hs;
			n++;

		}

	}

	#if 0
	{
		int i = 0;
		struct RegionRectangle *rr = clipregion->RegionRectangle;
		while(rr)
		{
			i++;
			rr = rr->Next;
		}

		kprintf("Number of regions:%d for blit:%d,%d,%d,%d\n", i, x1, y1, x2, y2);
	}
	#endif

	return clipregion;
}

DEFMMETHOD(Backfill)
{
	GETDATA;

	{
		APTR cliphandle = NULL;
		struct Region *clipregion = NULL;

		if (msg->bottom < msg->top)
			return 0;
		if (msg->right < msg->left)
			return 0;


		if (muiRenderInfo(obj) != NULL && (data->width != _width(obj) || data->height != _height(obj) || (msg->right - msg->left + 1 == _mwidth(obj) && msg->bottom - msg->top + 1 == _mheight(obj)) ))
		{
			/*
			 * Iterate through thumbs and mask them from backfill.
			 */

			data->width = _width(obj);
			data->height = _height(obj);
			clipregion = buildregionfromicons(data, obj, msg->left, msg->top, msg->right, msg->bottom);

			if (clipregion != NULL)
			{
				cliphandle = MUI_AddClipRegion(muiRenderInfo(obj), clipregion);
			}
		}

		/* draw */

		DOSUPER;

		if (clipregion != NULL)
		{
			MUI_RemoveClipRegion(muiRenderInfo(obj), cliphandle); /* DisposeRegion() is done by MUI */
		}
	}

	return (0);
}

DEFSET
{
	GETDATA;

	FORTAG(INITTAGS)
	{
		case MUIA_Lay_KeepPosition:
			data->keeppos = tag->ti_Data;
			break;
	}
	NEXTTAG

	return DOSUPER;
}

BEGINMTABLE
	DECMMETHOD(Backfill)
	DECSET
	DECNEW
ENDMTABLE

DECSUBCLASS_NC(MUIC_Virtgroup, LayClass)
