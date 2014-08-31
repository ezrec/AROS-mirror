
#define SYSTEM_PRIVATE 1

/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <clib/macros.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <libraries/asl.h>
#include <workbench/workbench.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <proto/alib.h>
#include <proto/utility.h>

#include <clib/debug_protos.h>
#if defined(__AROS__)
#include <clib/arossupport_protos.h>
#endif

#include <devices/rawkeycodes.h>

#if defined(__MORPHOS__)
#include <emul/emulregs.h>
#include <emul/emulinterface.h>
#endif
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler.h"
#include "pageview_class.h"
#include "documentview_class.h"
#include "renderer_class.h"
#include "documentlayout_class.h"

#include "system/chunky.h"
#include "system/gentexture.h"

struct RenderingQueueNode
{
	struct Node n;
	int id;
	int page;
	int rerender;
	Object *grpLayout;
	void *userdata;
};

struct Data
{
	int currentid;
	Object *proc;
	int pendingpage;
	int aborted;
	struct List renderingqueue;
	struct SignalSemaphore listsema;
	int quiting;
};

#define gFalse 0

#define D(x) x

//struct Library *CairoBase;
//struct Library *FontConfigBase;

DEFNEW
{

	obj = DoSuperNew(cl, obj,
				TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		NEWLIST(&data->renderingqueue);
		InitSemaphore(&data->listsema);
		data->currentid = 1;
		data->proc = MUI_NewObject(MUIC_Process,
					MUIA_Process_SourceClass , cl,
					MUIA_Process_SourceObject, obj,
					MUIA_Process_Name        , "[VPDF]: Rendering process",
					MUIA_Process_Priority    , -1,
					MUIA_Process_AutoLaunch  , FALSE,
					MUIA_Process_StackSize   , 1000000,
					TAG_DONE);

		DoMethod(data->proc, MUIM_Process_Launch);
	}

	return (ULONG)obj;
}

DEFDISP
{
	GETDATA;

	data->quiting = TRUE;
	D(kprintf("disposing...\n"));
	D(kprintf("Rendering thread exiting...\n"));

	ObtainSemaphore((struct SignalSemaphore*)data->proc);
	ObtainSemaphore(&data->listsema);

	while(!ISLISTEMPTY(&data->renderingqueue))
	{
		struct RenderingQueueNode *rqn = GetHead(&data->renderingqueue);
		REMOVE(rqn);
		free(rqn);
	}

	ReleaseSemaphore(&data->listsema);
	ReleaseSemaphore((struct SignalSemaphore*)data->proc);

	D(kprintf("disposing2...:%p, %p\n", data->proc, &data->listsema));

	D(kprintf("Queue empty. finalize...\n"));
	MUI_DisposeObject(data->proc);

	return DOSUPER;
}

static int documentviewCheckRenderAbort(void *_data)
{
	//struct Data *data = _data;

	//if (pendingpage != 0)
	//{
	//	  aborted = TRUE;
//		  return TRUE;
//	  }
//	  else
		return FALSE;

}


static void applyrotation(float *width, float *height, int rotation)
{
	if (rotation == 1 || rotation == 3)
	{
		float t = *height;
		*height = *width;
		*width = t;
	}
}


DEFMMETHOD(Process_Process)
{
	GETDATA;
	struct Process *myproc = (struct Process *)FindTask(NULL);
	APTR oldwindowptr = myproc->pr_WindowPtr;
	struct Library *base1;
	myproc->pr_WindowPtr = (APTR)-1;

	//base1 = CairoBase;
	//CairoBase = OpenLibrary("cairo.library", 0);

	while (!*msg->kill)
	{
		int page = 0;
		struct RenderingQueueNode *rqn = NULL;
		Object *pageview = NULL;

		ObtainSemaphore(&data->listsema);

		if (!ISLISTEMPTY(&data->renderingqueue))
		{
			struct RenderingQueueNode *rqnn = GetHead(&data->renderingqueue);
			page = rqnn->page;
			data->pendingpage = 0;
			data->aborted = FALSE;
			//kprintf("  render node for page:%d with priority:%d\n", page, rqnn->n.ln_Pri);
			/* make copy of a node. list is unlocked from now on so it might cease to exist */
			rqn = malloc(sizeof(*rqn));
			if (rqn != NULL)
				memcpy(rqn, rqnn, sizeof(*rqn));
		}

		/* start locking renderer semaphore so nothing is actualy removed (nodes can still be altered or added) */

		ObtainSemaphore((struct SignalSemaphore*)data->proc);

		ReleaseSemaphore(&data->listsema);

		if (page > 0 && DoMethod(rqn->grpLayout, MUIM_DocumentLayout_IsPageVisible, page) == FALSE)
			page = 0;

		if (page > 0)
		{
			void *pdfDocument = (void*)xget(rqn->grpLayout, MUIA_DocumentLayout_PDFDocument);

			float scale = 1.0f;
			float mediaheight, mediawidth;
			int bmwidth, bmheight;
			int rotation;
			int i;
			int rc;

			/* find pageview object which should receive new image */

			pageview = (Object*)DoMethod(rqn->grpLayout, MUIM_DocumentLayout_FindViewForPage, page);

			/* calculate scale based on page height dimmensions (scale page to fit) */
			mediaheight = pdfGetPageMediaHeight(pdfDocument, page);
			mediawidth = pdfGetPageMediaWidth(pdfDocument, page);
			
			rotation = xget(pageview, MUIA_PageView_Rotation);
			applyrotation(&mediawidth, &mediaheight, rotation);
			
			rotation *= 90;  // (yeah, we abuse the constants here...)
			
			scale = xget(pageview, MUIA_PageView_LayoutHeight) / mediaheight;
			scale = MIN(scale, xget(pageview, MUIA_PageView_LayoutWidth) / mediawidth);
			
			rc = pdfDisplayPageSlice(pdfDocument, page, scale, rotation, 0, gFalse, gFalse, -1, -1, -1, -1, documentviewCheckRenderAbort, rqn->userdata);
			//printf("dimmensions:%d,%d. media height:%f, %f. scale:%f. page:%d\n", pdfGetBitmapWidth(pdfDocument), pdfGetBitmapHeight(pdfDocument), pdfGetPageMediaWidth(pdfDocument, page), pdfGetPageMediaHeight(pdfDocument, page), scale, page);

			/* pass rendered bitmap to the page view */

			DoMethod(_app(pageview), MUIM_Application_KillPushMethod, pageview, 0, 0);
			if (rc == TRUE && data->aborted == FALSE)
			{
				struct pdfBitmap bm;
				pdfGetBitmap(pdfDocument, &bm);
				
				/* setting these attributes must guarantee that nothing will be redrawn! simplifies memory management */
				set(pageview, MUIA_PageView_PDFBitmap, NULL);
				set(pageview, MUIA_PageView_PDFBitmap, &bm);

				/* page is read to receive input */
				DoMethod(_app(pageview), MUIM_Application_PushMethod, pageview, 3, MUIM_NoNotifySet, MUIA_PageView_PDFReady, TRUE);
				DoMethod(_app(pageview), MUIM_Application_PushMethod, rqn->grpLayout, 1, MUIM_DocumentLayout_Refresh);
				//DoMethod(_app(pageview), MUIM_Application_PushMethod, pageview, 5, MUIM_PageView_Update, -1, -1, -1, -1);
			}
		}

		/* finished rendering. page object no longer needed. release renderer semaphore */

		ReleaseSemaphore((struct SignalSemaphore*)data->proc);

		/* remove (if really done) node from a list */

		ObtainSemaphore(&data->listsema);

		if (rqn != NULL)
		{
			struct RenderingQueueNode *rqnn;
			int valid = FALSE;

			/* check if the node is still on the list. it might have been removed */

			ITERATELIST(rqnn, &data->renderingqueue)
			{
				if (rqnn->id == rqn->id)
				{
					valid = TRUE;
					free(rqn);
					rqn = rqnn;
					break;
				}
			}

			if (valid)
			{
				/* remove the node or reinsert into queue if it was requested */

				REMOVE(rqn);

				if (rqn->rerender && data->quiting == FALSE)
				{
					/* rerender state causes page to be pushed back to the end of the line */
					rqn->rerender = FALSE;
					Enqueue(&data->renderingqueue, rqn);
				}
				else
				{
					free(rqn);
				}
			}
			else
			{
				/* dispose node copy */
				free(rqn);
			}
		}

		if (!ISLISTEMPTY(&data->renderingqueue))
		{
			ReleaseSemaphore(&data->listsema);
			continue;
		}

		ReleaseSemaphore(&data->listsema);

		Wait(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_C);
	}

	myproc->pr_WindowPtr = oldwindowptr;

	//printf("Dispose Cairo Library:%p. New Base:%p\n", CairoBase, base1);
	//CloseLibrary(CairoBase);
	//CairoBase =  base1;

	D(kprintf("Rendering thread exiting...\n"));

	return 0;
}

DEFMMETHOD(Renderer_Enqueue)
{
	GETDATA;
	struct RenderingQueueNode *rqn;

	/* add page to the list of pages that have to be rendered (if its not there already) */

	ObtainSemaphore(&data->listsema);

	//kprintf("enqueue2:%d\n", msg->pageIndex);

	ITERATELIST(rqn, &data->renderingqueue)
	{
		if (rqn->page == msg->pageIndex && rqn->grpLayout == msg->grpLayout)
		{
			rqn->rerender = TRUE;
			ReleaseSemaphore(&data->listsema);
			return 0;
		}
	}

	rqn = malloc(sizeof(*rqn));
	if (rqn != NULL)
	{
		rqn->id = data->currentid++;
		rqn->page = msg->pageIndex;
		rqn->rerender = FALSE;
		rqn->grpLayout = msg->grpLayout;
		rqn->userdata = NULL;
		rqn->n.ln_Pri = 32 + msg->priority;  // keep it like this. MUIV_ values might be negative
		Enqueue(&data->renderingqueue, rqn);
	}

	DoMethod(data->proc, MUIM_Process_Signal, SIGBREAKF_CTRL_D);

	ReleaseSemaphore(&data->listsema);

	return 0;
}

DEFMMETHOD(Renderer_Remove)
{
	GETDATA;
	struct RenderingQueueNode *rqn, *rqnn;
	int locked = FALSE;
	
	/*
	 * removing a page for rendering can only be done when the page is not being rendered.
	 * process semaphore is locked for long time so we lock it only when node is actualy
	 * being removed.
	 */

	D(kprintf("remove rendering nodes for view:%p\n", msg->grpLayout));

	ObtainSemaphore(&data->listsema);

	ITERATELISTSAFE(rqn, rqnn, &data->renderingqueue)
	{
		if (rqn->grpLayout == msg->grpLayout)
		{
			if (!locked)
			{
				D(kprintf("  locking rendering thread semaphore...\n"));
				ObtainSemaphore((struct SignalSemaphore*)data->proc);
				locked = TRUE;
			}

			D(kprintf("  killed node for page:%d\n", rqn->page));
			REMOVE(rqn);
			free(rqn);
		}
	}

	if (locked)
		ReleaseSemaphore((struct SignalSemaphore*)data->proc);

	ReleaseSemaphore(&data->listsema);

	return 0;
}

BEGINMTABLE
	DECNEW
	DECDISP
	DECMMETHOD(Process_Process)
	DECMMETHOD(Renderer_Enqueue)
	DECMMETHOD(Renderer_Remove)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Notify, RendererClass)

