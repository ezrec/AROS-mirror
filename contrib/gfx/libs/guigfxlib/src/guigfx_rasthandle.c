/*********************************************************************
----------------------------------------------------------------------

	guigfx_rasthandle

----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <libraries/cybergraphics.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>


#include <guigfx/guigfx.h>

#include "guigfx_global.h"
#include "guigfx_bitmap.h"


#ifdef __AROS__
#define GetBitMapInfo(bm,displayid,args...) \
({ \
     IPTR __args[] = { args }; \
     GetBitMapInfoA((bm), (displayid), (TAGLIST)__args); \
})
#endif

/*--------------------------------------------------------------------

		DeleteRastHandle(rh)

		löscht ein RastHandle
	
--------------------------------------------------------------------*/

void DeleteRastHandle(RASTHANDLE *rh)
{
	if(rh->temprp)
	{
		if(rh->temprp->BitMap)
		{
			FreeBitMap(rh->temprp->BitMap);
		}

		FreeRenderVec(rh->temprp);	
	}
	FreeRenderVec(rh);
}


/*--------------------------------------------------------------------

		rasthandle = CreateRastHandle(rp)

		ermittelt Rastport-Daten und erzeugt
		ein RastHandle.
	
--------------------------------------------------------------------*/

RASTHANDLE *CreateRastHandle(struct RastPort *rp, ULONG modeID)
{
	RASTHANDLE *rh;

	int iscyber;
	int depth;
	int flags;
	int width;

	BOOL success = FALSE;

	if ((rh = AllocRenderVecClear(MemHandler,sizeof(RASTHANDLE))))
	{
		rh->rp = rp;
		rh->drawmode = DRAWMODE_WRITEPIXELARRAY;
		rh->truecolor = FALSE;
		rh->colormode = COLORMODE_CLUT;

		GetBitMapInfo(rp->BitMap, modeID,
			BMAPATTR_CyberGFX, (IPTR)&iscyber,
			BMAPATTR_Depth, (IPTR)&depth,
			BMAPATTR_Flags, (IPTR)&flags,
			BMAPATTR_Width,(IPTR) &width,
			TAG_DONE);


		if (iscyber)
		{
			DB(kprintf("cyber bitmap detected\n"));

			rh->drawmode = DRAWMODE_CYBERGFX;
			rh->truecolor = (depth > 8);
		}


		if (flags & BMF_STANDARD)
		{
			DB(kprintf("BMF_STANDARD bitmap detected\n"));

			if (env_usewpa8)
			{
				rh->drawmode = DRAWMODE_WRITEPIXELARRAY;
			}
			else
			{
				rh->drawmode = DRAWMODE_BITMAP;
			}
		}


		if ((rh->temprp = AllocRenderVec(MemHandler, sizeof(struct RastPort))))
		{
			TurboCopyMem(rp, rh->temprp, sizeof(struct RastPort));
			rh->temprp->Layer = NULL;
			if ((rh->temprp->BitMap = AllocBitMap(width, 1, depth, 0, rp->BitMap)))
			{
				success = TRUE;
			}
		}




/*
		if(CyberGfxBase)
		{
			if (GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
			{
				DB(kprintf("Cybergraphics bitmap detected\n"));
				rh->drawmode = DRAWMODE_CYBERGFX;
				rh->truecolor = (GetCyberMapAttr(rp->BitMap, CYBRMATTR_DEPTH) > 8);
			}
		}


		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if (GetBitMapAttr(rp->BitMap, BMA_FLAGS) & BMF_STANDARD)
		{
			DB(kprintf("BMF_STANDARD bitmap detected\n"));

			if (env_usewpa8)
			{
				rh->drawmode = DRAWMODE_WRITEPIXELARRAY;
			}
			else
			{
				rh->drawmode = DRAWMODE_BITMAP;
			}
		}


//		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if (rh->temprp = AllocRenderVec(MemHandler, sizeof(struct RastPort)))
		{
			ULONG width, depth;

			TurboCopyMem(rp, rh->temprp, sizeof(struct RastPort));
			rh->temprp->Layer = NULL;
			
			width = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
			depth = GetBitMapAttr(rp->BitMap, BMA_DEPTH);
			
			if (rh->temprp->BitMap = AllocBitMap(width,1,depth,0,rp->BitMap))
			{
				success = TRUE;
			}
		}
*/


	}

	if (success)
	{
		if (modeID != INVALID_ID)
		{
			/* colormode (ggf. HAM) ermitteln */
			
			DisplayInfoHandle dih;
			
			if((dih = FindDisplayInfo(modeID)))
			{
				struct DisplayInfo di;	
			
				if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DISP, modeID))
				{
					if (di.PropertyFlags & DIPF_IS_HAM)
					{
						rh->colormode = (depth == 8) ?
							COLORMODE_HAM8 : COLORMODE_HAM6;
					}
				}
			}

		}
	}
	else
	{
		if (rh)
		{
			DeleteRastHandle(rh);
			rh = NULL;
		}
	}

	return rh;
}

