/*********************************************************************
----------------------------------------------------------------------

	guigfx_pensharemap
	
----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <utility/tagitem.h>

#include <exec/lists.h>

#include <proto/render.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include <guigfx/guigfx.h>

#include "guigfx_global.h"


/*********************************************************************
----------------------------------------------------------------------

	psm = CreatePenShareMap ( tags )

	create and set-up a pensharemap.
	
	GGFX_HSType				Auflösung des Histogramms	default: 12BIT_TURBO

----------------------------------------------------------------------
*********************************************************************/

PSM SAVE_DS ASM *CreatePenShareMapA(REG(a0) TAGLIST taglist)
{
	PSM *psm;
	
	if ((psm = (PSM *) AllocRenderVec(MemHandler, sizeof(struct PenShareMap))))
	{
		ULONG default_hstype = DEFAULT_HSTYPE;

		psm->histogram = NULL;

		psm->hstype =
			GetTagData(GGFX_HSType, default_hstype, taglist);

		DB(kprintf("(!) Histogramm der Pensharemap: %ld\n", psm->hstype));

		InitSemaphore(&psm->semaphore);
		NewList(&psm->colorlist);
		psm->numcolorhandles = 0;
		psm->modified = FALSE;

		return psm;
	}

	return NULL;
}


/*********************************************************************
----------------------------------------------------------------------

	DeletePenShareMap(psm)

----------------------------------------------------------------------
*********************************************************************/

void SAVE_DS ASM DeletePenShareMap(REG(a0) PSM *psm)
{
	if (psm)
	{
		struct Node *node; 

		ObtainSemaphore(&psm->semaphore);
	
		if (psm->histogram)
		{
			DeleteHistogram(psm->histogram);
		}
	
		if (!IsListEmpty(&psm->colorlist))
		{
			struct Node *nextnode;
			node = (struct Node *) psm->colorlist.lh_Head;
			while ((nextnode = node->ln_Succ))
			{
				RemColorHandle((COLORHANDLE *)node);
				node = nextnode;
			}
		}
	
		ReleaseSemaphore(&psm->semaphore);
	
		FreeRenderVec(psm);
	}
	
}

