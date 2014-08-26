/*
 * Wrap poppler document class functions into c functions.
 */

//#define USE_SPLASH

#define AROS_ALMOST_COMPATIBLE

#include "Object.h"

#if defined(__MORPHOS__)
#include <proto/charsets.h>
#endif
#define _NO_PPCINLINE
#include <proto/keymap.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <clib/debug_protos.h>
#include <exec/semaphores.h>
#include <exec/lists.h>
#include <constructor.h>



#define USE_FLOAT 1
#include <poppler-config.h>
#include <config.h>

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>

#include "PDFDoc.h"
#include "Outline.h"
#include "Link.h"
#include "GlobalParams.h"
#include "PDFDocEncoding.h"
#include "goo/GooList.h"

//#define USE_SPLASH
#ifdef USE_SPLASH
#include "splash/SplashBitmap.h"
#include "splash/Splash.h"
#include "SplashOutputDev.h"
#include "TextOutputDev.h"
#else
#include "CairoOutputDev.h"
#endif
#include "PSOutputDev.h"

#include "poppler.h"
#include "poppler_io.h"
#include "poppler_device.h"

extern struct Library *LocaleBase;
#define LOCALE_BASE_NAME LocaleBase

#warning TODO: make the semaphore per-document

struct SignalSemaphore semaphore;

extern struct Library *CairoBase;

/* all of this is just to not having to expose GBool in ppoppler.h... */

struct abortcallbackcontext
{
	int	(*userfunction)(void *);
	void *userdata;
	struct devicecontext *ctx;
};

/* this is bit hacky but we can't/shouldn't use boopsi in poppler.c */

#define D(x)

extern "C" {
	void vpdfErrorFunction(int pos, char *message);
};

static void	pdfErrorFunction(void *data, ErrorCategory category, Goffset pos, char *msg)
{
	vpdfErrorFunction(pos, msg);
}

static CONSTRUCTOR_P(init_poppler, 0)
{
	InitSemaphore(&semaphore);
	setErrorCallback(pdfErrorFunction, NULL);
	return 0;
}

static DESTRUCTOR_P(cleanup_poppler, 0)
{
	D(kprintf("destroy poppler:%p\n", &semaphore));
}

#if 1
#define ENTER_SECTION \
{ ObtainSemaphore(&semaphore);}
#else // time delay needed to enter critical section
#define ENTER_SECTION \
{ clock_t t0 = clock(); ObtainSemaphore(&semaphore); t0 = clock() - t0; kprintf("section enter:%f:%s\n", (float)t0/CLOCKS_PER_SEC, __FUNCTION__);}
#endif

//#define ENTER_SECTION
#define LEAVE_SECTION ReleaseSemaphore(&semaphore);
//#define LEAVE_SECTION

//struct Library *CairoBase;

/// macros

#undef MIN
#undef MAX

#define MIN(a,b)          \
  ({typeof(a) _a = (a);   \
    typeof(b) _b = (b);   \
    _a < _b ? _a : _b; })

#define MAX(a,b)          \
  ({typeof(a) _a = (a);   \
    typeof(b) _b = (b);   \
    _a > _b ? _a : _b; })

////

/// utility functions

static char *convertUTF16ToANSI(unsigned char *string, int length)
{
	char *buff = (char*)calloc(1, length + 1);
	if(buff != NULL)
		ConvertTagList((unsigned char*)string + 2, length * 2 - 2, buff, length + 1, MIBENUM_UTF_16BE, MIBENUM_SYSTEM, NULL);
	return buff;
}

static char *convertUTF32ToANSI(Unicode *string, int length)
{
	char *buff = (char*)calloc(1, length + 1);
	if(buff != NULL)
		ConvertTagList((unsigned char*)string, length * 4, buff, length + 1, MIBENUM_UTF_32BE, MIBENUM_SYSTEM, NULL);

	return buff;
}


static WCHAR *convertToUCS4(const char *string)
{
	size_t length = strlen(string);
	size_t i;

	WCHAR *buff = (WCHAR*)calloc(sizeof(WCHAR), length + 1);
	if(buff != NULL)
	{
		for(i = 0; i < length; i++)
			buff[i] = ToUCS4(string[i], NULL);
	}

	return buff;
}

static char *convertToANSI(GooString *s)
{
	if(s->hasUnicodeMarker())
	{
		//printf("has unicode marker\n");
		return convertUTF16ToANSI((unsigned char*)s->getCString(), s->getLength() - 2);
	}
	else
	{
		char *buffer = (char*)calloc(1, s->getLength() + 1);
		int i;

		//printf("no unicode marker\n");
		for(i = 0; i < s->getLength(); ++i)
			buffer[i] = s->getChar(i);

		return buffer;
	}
}

////

struct foo
{
	int bar;
};

struct foo foo;

void *pdfNew(const char *fname)
{
	//base1 = CairoBase;
	//CairoBase = OpenLibrary("cairo.library", 0);

//StackFrame[-1].LR-> Address 0x26870dd4 -> vpdf Hunk 1 Offset 0x00001284
//StackFrame[-2].LR-> Address 0x2686ff34 -> vpdf Hunk 1 Offset 0x000003e4

#ifndef USE_SPLASH
	kprintf("Cairo Base:%p. sema:%p, %p, %p\n", CairoBase, &semaphore, foo, &foo);
#endif

	struct devicecontext *ctx = (struct devicecontext*)calloc(1, sizeof(*ctx));
	if(ctx == NULL)
		return NULL;

	ENTER_SECTION


	try
	{
		int i;
		Object obj;
		obj.initNull();

		if(globalParams == NULL)
		{
			globalParams = new GlobalParams();
			globalParams->setScreenType(screenClustered);
			globalParams->setScreenSize(3);
			//globalParams->setPSBinary(true);
		}

		CachedFile *cachedFile = new CachedFile(new StdCacheLoader(), new GooString(fname));
		D(kprintf("pdf file opened...\n"));
		ctx->stream = new CachedFileStream(cachedFile, 0, gTrue, cachedFile->getLength(), &obj);
		D(kprintf("pdf stream created..\n"));
		ctx->doc = new PDFDoc(ctx->stream, NULL, NULL);
		D(kprintf("document opened...\n"));

		if(ctx->doc == NULL || !ctx->doc->isOk() || ctx->doc->getNumPages() == 0)
		{
			D(kprintf("failed to open a document:%p...\n", ctx->doc));
			pdfDelete(ctx);
			LEAVE_SECTION
			return NULL;
		}

#ifdef USE_SPLASH
		SplashColor paperColor;

		paperColor[0] = 255;
		paperColor[1] = 255;
		paperColor[2] = 255;

		ctx->dev = new SplashOutputDev(splashModeRGB8, 4, gFalse, paperColor);
		ctx->dev->startDoc(ctx->doc);
#else
		ctx->dev = new CairoOutputDev();
		ctx->dev->setPrinting(false);
		ctx->surface = NULL;
		ctx->dev->startDoc(ctx->doc);
#endif
		D(kprintf("media_box:%f,%f\n", ctx->doc->getPageMediaWidth(1), ctx->doc->getPageMediaHeight(1)));
		D(kprintf("media_box:%f, %f\n", pdfGetPageMediaWidth(ctx, 1), pdfGetPageMediaHeight(ctx, 1)));

		NEWLIST(&ctx->search.searchresultlist);

		/* cache some common values to remove need for locking */
		ctx->pagesnum = ctx->doc->getNumPages();
		ctx->documentwidth = ctx->documentheight = 0.0f;

        for(i = 0; i < ctx->pagesnum; i++)
		{
			Page *pdfpage = ctx->doc->getCatalog()->getPage(i + 1);
			int rotate = pdfpage->getRotate();
			float width, height;
			if(rotate == 90 || rotate == 270)
			{
				width = pdfpage->getMediaHeight();
				height = pdfpage->getMediaWidth();
			}
			else
			{
				width = pdfpage->getMediaWidth();
				height = pdfpage->getMediaHeight();
			}

			ctx->documentwidth = MAX(ctx->documentwidth, width);
			ctx->documentheight	= MAX(ctx->documentheight, height);
		}
	}
	catch(...)
	{
		D(kprintf("exception while opening pdf document\n"));
		pdfDelete(ctx);
		LEAVE_SECTION
		return NULL;
	}

	LEAVE_SECTION

	return ctx;
}

void pdfDelete(void *_ctx)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;

	ENTER_SECTION

	D(kprintf("delete output device:%p. base:%p\n", ctx->dev, CairoBase));
	if(ctx->dev != NULL)
		delete ctx->dev;

	if (ctx->selection_dev != NULL)
		delete ctx->selection_dev;

#ifndef USE_SPLASH
	D(kprintf("delete surface:%p. base:%p\n", ctx->surface, CairoBase));

	if(ctx->surface != NULL)
	{
		cairo_create(ctx->surface);
		cairo_surface_destroy(ctx->surface);
	}
#endif

	D(kprintf("delete document\n"));
	if(ctx->doc != NULL)
		delete ctx->doc;

	// Stream is disposed with document
	//if (ctx->stream != NULL)
	//	  delete ctx->stream;

	free(ctx);
	//CloseLibrary(CairoBase);
	LEAVE_SECTION
}

float pdfGetPageMediaWidth(void *_ctx, int page)
{
	ENTER_SECTION
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	//float width = ctx->doc->getPageMediaWidth(page);
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	int rotate = pdfpage->getRotate();
	float width;
	if(rotate == 90 || rotate == 270)
		width = pdfpage->getMediaHeight();
	else
		width = pdfpage->getMediaWidth();
	LEAVE_SECTION
	return width;
}

float pdfGetPageMediaHeight(void *_ctx, int page)
{
	ENTER_SECTION
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	//float height = ctx->doc->getPageMediaHeight(page);
	int rotate = pdfpage->getRotate();
	float height;
	if(rotate == 90 || rotate == 270)
		height = pdfpage->getMediaWidth();
	else
		height = pdfpage->getMediaHeight();
	LEAVE_SECTION
	return height;
}

int pdfGetDocumentDimensions(void *_ctx, float *width, float *height)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;

	*width = ctx->documentwidth;
	*height = ctx->documentheight;
	return 1;
}

int	pdfGetPagesNum(void *_ctx)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	return ctx->pagesnum;
}

static GBool abortcheckcbk_wrapper(void *data)
{
	struct abortcallbackcontext *abortctx = (struct abortcallbackcontext*)data;
	if (abortctx->userfunction != NULL)
		return (GBool)abortctx->userfunction(abortctx->userdata);

	//D(printf("abort check...\n"));
	return 0;
}


static void applyrotation(int *width, int *height, int rotation)
{
	if(rotation == 90 || rotation == 270)
	{
		int t = *height;
		*height = *width;
		*width = t;
	}
}

int pdfDisplayPageSlice(void *_ctx, int page, double scale, int rotate,
						int	useMediaBox, int crop, int printing, int sliceX, int sliceY, int sliceW, int sliceH, int (*abortcheckcbk)(void *), void *abortcheckcbkdata)
{
	ENTER_SECTION

	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	int cropwidth = (int)pdfGetPageMediaWidth(ctx, page);
	int cropheight = (int)pdfGetPageMediaHeight(ctx, page);
	int width = (int)ceil(cropwidth * scale);
	int height = (int)ceil(cropheight * scale);
	int rc = FALSE;

	applyrotation(&width, &height, rotate);
	applyrotation(&cropwidth, &cropheight, rotate);

#ifdef USE_SPLASH
#else

	if(ctx->surface != NULL)
	{
		if(cairo_image_surface_get_width(ctx->surface) != width || cairo_image_surface_get_height(ctx->surface) != height)
		{
			//cairo_destroy(ctx->cairo);
			cairo_surface_destroy(ctx->surface);
			ctx->surface = NULL;
		}
	}

	if(ctx->surface == NULL)
	{
		ctx->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	}

	if(ctx->surface == NULL)
	{
		LEAVE_SECTION
		return FALSE;
	}

	ctx->cairo = cairo_create(ctx->surface);

	{
		//cairo_font_options_t *options = cairo_font_options_create ();
		//cairo_get_font_options (ctx->cairo, options);
		//cairo_font_options_set_antialias (options, CAIRO_ANTIALIAS_SUBPIXEL);
		//cairo_font_options_set_subpixel_order (options, CAIRO_SUBPIXEL_ORDER_BGR);
		//cairo_set_font_options (ctx->cairo, options);
		//cairo_font_options_destroy (options);
	}

	cairo_save(ctx->cairo);
	cairo_set_source_rgba(ctx->cairo, 1., 1., 1., 1);
	cairo_paint(ctx->cairo);
	cairo_restore(ctx->cairo);
	ctx->dev->setCairo(ctx->cairo);
#endif

	//Delay(50);
	//cairo_save(ctx->cairo);
	//printf("render:%d, %d, crop:%d, %d\n", width, height, cropwidth, cropheight);
	try
	{
		struct abortcallbackcontext abortctx;
		abortctx.userfunction = abortcheckcbk;
		abortctx.userdata = abortcheckcbkdata;
		abortctx.ctx = ctx;

		pdfpage->displaySlice(ctx->dev, 72 * scale, 72 * scale, rotate, gTrue, gFalse, sliceX, sliceY, sliceW, sliceH,
			printing, abortcheckcbk_wrapper, &abortctx);
		rc = TRUE;
	}
	catch(...)
	{
		D(kprintf("Exception during page rendering\n"));
	}

	//cairo_restore(ctx->cairo);
	//Delay(20);

#ifndef USE_SPLASH
	ctx->dev->setCairo(NULL);
	cairo_destroy(ctx->cairo);
	ctx->cairo = NULL;
#endif
	//doc->displayPage(dev, page, hDPI, vDPI, rotate, useMediaBox, crop, printing);
	
	LEAVE_SECTION
	return rc;
}


unsigned char *pdfGetBitmapRowData(void *_ctx, int row)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
#ifdef USE_SPLASH
	return ctx->dev->getBitmap()->getDataPtr() + ctx->dev->getBitmap()->getRowSize() * row;
#else
	//printf("getrow:%d\n", row);
	unsigned char *data = cairo_image_surface_get_data(ctx->surface);
	data = data + cairo_image_surface_get_stride(ctx->surface) * row;
	//printf("done:%d, %p\n", row, data);
	return data;
#endif
}

int pdfGetBitmapWidth(void *_ctx)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
#ifdef USE_SPLASH
	return ctx->dev->getBitmap()->getWidth();
#else
	return cairo_image_surface_get_width(ctx->surface);
#endif
}

int pdfGetBitmapHeight(void *_ctx)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
#ifdef USE_SPLASH
	return ctx->dev->getBitmap()->getHeight();
#else
	return cairo_image_surface_get_height(ctx->surface);
#endif
}

void pdfGetBitmap(void *_ctx, struct pdfBitmap *bm)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	bm->width = pdfGetBitmapWidth(_ctx);
	bm->height = pdfGetBitmapHeight(_ctx);
	bm->stride = cairo_image_surface_get_stride(ctx->surface);
	bm->data = cairo_image_surface_get_data(ctx->surface);
}

void pdfDisposeRegionForSelection(void *_ctx, struct pdfSelectionRegion *region)
{
	free(region);
}

void pdfDisposeTextForSelection(void *_ctx, struct pdfSelectionText *text)
{
	free(text);
}

struct pdfSelectionRegion *pdfBuildRegionForSelection(void *_ctx, int page, double x1, double y1, double x2, double y2, struct pdfSelectionRegion *previous)
{
	ENTER_SECTION

	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	double cropwidth = pdfGetPageMediaWidth(ctx, page);
	double cropheight = pdfGetPageMediaHeight(ctx, page);

	TextOutputDev *text_dev;
	
	if (ctx->selection_dev == NULL)
		text_dev = ctx->selection_dev = new TextOutputDev(NULL, gTrue, 0, gFalse, gFalse);
	else
		text_dev = ctx->selection_dev;
	
	if (page != ctx->selection_pagenum)
		pdfpage->display(text_dev, 72, 72, 0, gTrue, gTrue, gFalse);
	
	ctx->selection_pagenum = page;
	
	PDFRectangle rect(x1 * cropwidth, y1 * cropheight, x2 * cropwidth, y2 * cropheight);
	GooList *list = text_dev->getSelectionRegion(&rect, selectionStyleGlyph, 1.0f);
	
	/* use gfx.lib region functions to merge all pdf rectangles. this sucks a bit but hopefuly using scaled coords will be fine */
	
	struct Region *region = NewRegion();
	int i;
	const float scale = (float)(1<<12);
	
	D(kprintf("source region:%f, %f, %f, %f\n", x1,y1,x2,y2));
	
	for (i = 0; i < list->getLength(); i++)
	{
		Rectangle rect;
		
		PDFRectangle *selection_rect = (PDFRectangle *) list->get(i);
		rect.MinX      = (int)(selection_rect->x1 * scale / cropwidth);
		rect.MinY      = (int)(selection_rect->y1 * scale / cropheight);
		rect.MaxX      = rect.MinX + (int)((selection_rect->x2 - selection_rect->x1) * scale / cropwidth);
		rect.MaxY      = rect.MinY + (int)((selection_rect->y2 - selection_rect->y1) * scale / cropheight);
		D(kprintf("reg:%d, %d, %d, %d, %f, %f, %f, %f\n", rect.MinX, rect.MinY, rect.MaxX, rect.MaxY, selection_rect->x1/cropwidth, selection_rect->y1/cropheight, selection_rect->x2/cropwidth, selection_rect->y2/cropheight));
				
		OrRectRegion(region, &rect);
		delete selection_rect;
	}
	
	/* convert gfx.lib region to pdfregion and return to the caller */
	
	delete list;
	
	int numrects = 0;
	RegionRectangle *rrect;
	
	D(kprintf("count regions...\n"));
	for(numrects = 0, rrect = region->RegionRectangle; rrect != NULL; rrect = rrect->Next, numrects++) {}; 
	
	struct pdfSelectionRegion *selection = (struct pdfSelectionRegion*)malloc(sizeof(struct pdfSelectionRegion) + numrects * sizeof(struct pdfSelectionRectangle));
	if (selection != NULL)
	{
		rrect = region->RegionRectangle;
		int rectind = 0;
		D(kprintf("build selection rectangles...\n"));
		while(rrect != NULL)
		{
			int px, py;
			int pw, ph;
			
			int x, y;
			Rectangle rect = rrect->bounds;
			struct pdfSelectionRectangle *pdfrect = &selection->rectangles[rectind++];
			
			rect.MinX += region->bounds.MinX;
			rect.MinY += region->bounds.MinY;
			rect.MaxX += region->bounds.MinX;
			rect.MaxY += region->bounds.MinY;
				
			pdfrect->x1 = rect.MinX / scale;
			pdfrect->y1 = rect.MinY / scale;
			pdfrect->x2 = rect.MaxX / scale;
			pdfrect->y2 = rect.MaxY / scale;
			rrect = rrect->Next;
		}
		
		selection->numrects = numrects;
	}
		
	DisposeRegion(region);
	
	LEAVE_SECTION
	
	return selection;
}

struct pdfSelectionText *pdfBuildTextForSelection(void *_ctx, int page, double x1, double y1, double x2, double y2)
{
	ENTER_SECTION

	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	double cropwidth = pdfGetPageMediaWidth(ctx, page);
	double cropheight = pdfGetPageMediaHeight(ctx, page);

	TextOutputDev *text_dev = new TextOutputDev(NULL, gTrue, 0, gFalse, gFalse);
	pdfpage->display(text_dev, 72, 72, 0, gTrue, gTrue, gFalse);
	PDFRectangle rect(x1 * cropwidth, y1 * cropheight, x2 * cropwidth, y2 * cropheight);
	GooString *text = text_dev->getSelectionText(&rect, selectionStyleGlyph);
		
	delete text_dev;
		
	struct pdfSelectionText *selection = (struct pdfSelectionText*)malloc(sizeof(struct pdfSelectionText) + text->getLength() + 1);
	if (selection != NULL)
	{
		memcpy(selection->utf8, text->getCString(), text->getLength());
		selection->utf8[text->getLength()] = 0;
	}
		
	delete text;
	
	LEAVE_SECTION
	
	return selection;
}

static int actionGetPage(void *_doc, void *_action)
{
	PDFDoc *doc = (PDFDoc*)_doc;
	LinkAction *link_action = (LinkAction *)_action;

	if(link_action == NULL)
		return 0;

	ENTER_SECTION

//	  printf("linkk:%p\n", link_action);
//	  printf("linkkind:%d\n", link_action->getKind());

	switch(link_action->getKind())
	{
	case actionGoTo:
	{
		LinkGoTo *link_goto = dynamic_cast<LinkGoTo*>(link_action);
		if(link_goto->getNamedDest() != NULL || link_goto->getDest() != NULL)
		{
			//printf("lookup dest for link_goto:%p\n", link_goto);
			LinkDest *d = link_goto->getDest() != NULL ? link_goto->getDest() : doc->findDest(link_goto->getNamedDest());
			if(d != NULL)
			{
				int ret;
				if(d->isPageRef() == FALSE)
				{
					ret = d->getPageNum();
				}
				else
				{
					Ref ref = d->getPageRef();
					//printf("page from ref:%p,%p\n", ref.num, ref.gen);

					int page = doc->findPage(ref.num, ref.gen);
					//printf("found pge:%d\n", page);
					ret = page;
				}
				LEAVE_SECTION
				return ret;
			}

			#if 0
			// this is a case where only getNamedDest() is not NULL but no idea how to handle that yet - kiero

			//char *buff = convertToANSI(link_goto->getNamedDest());
			//printf("%s\n", buff);
			//printf("goto link. isok:%d\n", link_goto->getDest()->isOk());

			if(link_goto->getDest()->isPageRef() == FALSE)
			{
				LEAVE_SECTION
				return link_goto->getDest()->getPageNum();
			}
			else
			{
				Ref ref = link_goto->getDest()->getPageRef();
				//printf("page from ref:%p,%p\n", ref.num, ref.gen);

				int page = doc->findPage(ref.num, ref.gen);
				//printf("found pge:%d\n", page);
				LEAVE_SECTION
				return page;
			}
			#endif
		}
		else
		{
		}
	}
	//action->type = POPPLER_ACTION_GOTO_DEST;
	//build_goto_dest (document, action, dynamic_cast <LinkGoTo *> (link));
	break;
	case actionGoToR:
		//action->type = POPPLER_ACTION_GOTO_REMOTE;
		//build_goto_remote (action, dynamic_cast <LinkGoToR *> (link));
		break;
	case actionLaunch:
		//action->type = POPPLER_ACTION_LAUNCH;
		//build_launch (action, dynamic_cast <LinkLaunch *> (link));
		break;
	case actionURI:
	{
		LinkURI *link_uri = dynamic_cast<LinkURI*>(link_action);
		if(link_uri->getURI() != NULL)
		{
			//char *buff = convertToANSI(link_uri->getURI());
			//printf("%s\n", buff);
		}
	}
	//action->type = POPPLER_ACTION_URI;
	//build_uri (action, dynamic_cast <LinkURI *> (link));
	break;
	case actionNamed:
		//action->type = POPPLER_ACTION_NAMED;
		//build_named (action, dynamic_cast <LinkNamed *> (link));
		break;
	case actionMovie:
		//action->type = POPPLER_ACTION_MOVIE;
		//build_movie (action, link);
		break;
	case actionUnknown:
	default:
		//action->type = POPPLER_ACTION_UNKNOWN;
		break;
	}

	LEAVE_SECTION
	return 0;
}

struct OutlineItemNode
{
	struct MinNode n;
	OutlineItem *item;
	struct MinList *children;
	char *title;
	int page;
};

static struct MinList *buildoutlineitemlist(PDFDoc *doc, GooList *items)
{
	struct MinList *l = (struct MinList*)calloc(1, sizeof(struct MinList));
	if(l != NULL)
	{
		NEWLIST(l);

		if(items != NULL)
		{

			int i;

			for(i = 0; i < items->getLength(); i++)
			{
				struct OutlineItemNode *n = (struct OutlineItemNode*)calloc(1, sizeof(*n));

				if(n != NULL)
				{
					n->item = (OutlineItem*)items->get(i);
					n->item->open();
					n->title = convertUTF32ToANSI(n->item->getTitle(), n->item->getTitleLength());
					n->page = actionGetPage(doc, n->item->getAction());


					//printf("outline item:%s, page:%d\n", n->title, n->page);

					if(n->item->hasKids())
					{
						//printf("has children, process...\n");
						n->children = buildoutlineitemlist(doc, n->item->getKids());
					}

					ADDTAIL(l, n);
				}
			}
		}
	}

	return l;
}

struct MinList *pdfGetOutlines(void *_ctx)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;

	ENTER_SECTION
	Outline	*outline = ctx->doc->getOutline();
	GooList *items = outline->getItems();
	struct MinList *l = buildoutlineitemlist(ctx->doc, items);
	LEAVE_SECTION
	return l;
}

int outlineHasChildren(void *_outline)
{
	struct OutlineItemNode *outline = (struct OutlineItemNode*)_outline;
	return outline->children != NULL ? TRUE : FALSE;
}

struct MinList *outlineGetChildren(void *_outline)
{
	struct OutlineItemNode *outline = (struct OutlineItemNode*)_outline;
	return outline->children;
}


char *outlineGetTitle(void *_outline)
{
	struct OutlineItemNode *outline = (struct OutlineItemNode*)_outline;
	return outline->title;
}

int outlineGetPage(void *_outline)
{
	struct OutlineItemNode *outline = (struct OutlineItemNode*)_outline;
	return outline->page;
}



void
poppler_page_get_size(Page *page,
                      double      *width,
                      double      *height)
{
	double page_width, page_height;
	int rotate;

	ENTER_SECTION

	rotate = page->getRotate();
	if(rotate == 90 || rotate == 270)
	{
		page_height = page->getMediaWidth();
		page_width = page->getMediaHeight();
	}
	else
	{
		page_width = page->getMediaWidth();
		page_height = page->getMediaHeight();
	}

	if(width != NULL)
		*width = page_width;
	if(height != NULL)
		*height = page_height;

	LEAVE_SECTION
}

void pdfConvertUserToDevice(void *_ctx, double* x, double* y)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;

	ENTER_SECTION
	OutputDev *dev = ctx->dev;
	int dx, dy;
	dev->cvtUserToDev(*x, *y, &dx, &dy);
	LEAVE_SECTION
	*x = dx;
	*y = dy;
}

void pdfConvertDeviceToUser(void *_ctx, int page, double x, double y, int *ix, int *iy)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;

	ENTER_SECTION
	OutputDev *dev = ctx->dev;
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	double dx, dy;
	double ctm[6];
	pdfpage->getDefaultCTM(ctm, 72, 72, 0, gFalse, gFalse);
	dev->setDefaultCTM(ctm);
	dev->cvtDevToUser(x, y, &dx, &dy);
	LEAVE_SECTION
	*ix = dx;
	*iy = dy;
}

void pdfLock(void *_ctx)
{
	ENTER_SECTION;
}
void pdfRelease(void *_ctx)
{
	LEAVE_SECTION;
}


void *pdfFindLink(void *_ctx, int pagenum, int x, int y)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	Links *links;
	Object obj;
	Page *page;

	double width, height;

	ENTER_SECTION

	Catalog *catalog = ctx->doc->getCatalog();
	//printf("catalog:%p\n", catalog);
	if(catalog == NULL)
	{
		LEAVE_SECTION
		return NULL;
	}


	page = catalog->getPage(pagenum);

	if(page == NULL)
	{
		LEAVE_SECTION
		return NULL;
	}


	links = new Links(page->getAnnots());
	obj.free();

	if(links == NULL)
	{
		LEAVE_SECTION
		return NULL;
	}

	poppler_page_get_size(page, &width, &height);

	if(links != NULL)
	{
		//printf("links:%p\n", links);
		int num = links->getNumLinks();
		int i;

		//printf("num annots:%d\n", num);

		for(i = 0; i < num; i++)
		{
			LinkAction *link_action;
			AnnotLink *link;

			link = links->getLink(i);
			link_action = link->getAction();

			if (link_action == NULL)
				continue;

			switch(link_action->getKind())
			{
			case actionGoTo:
			{
				LinkGoTo *link_goto = dynamic_cast<LinkGoTo*>(link_action);
				//if (link_goto->getDest() == NULL)
				//	  continue;
				//if (link_goto->getNamedDest() != NULL) {
				//char *buff = convertToANSI(link_goto->getNamedDest());
				//printf("%s\n", buff);
				//}
			}
			//action->type = POPPLER_ACTION_GOTO_DEST;
			//build_goto_dest (document, action, dynamic_cast <LinkGoTo *> (link));
			break;
			case actionGoToR:
				//action->type = POPPLER_ACTION_GOTO_REMOTE;
				//build_goto_remote (action, dynamic_cast <LinkGoToR *> (link));
				break;
			case actionLaunch:
				//action->type = POPPLER_ACTION_LAUNCH;
				//build_launch (action, dynamic_cast <LinkLaunch *> (link));
				break;
			case actionURI:
			{
				LinkURI *link_uri = dynamic_cast<LinkURI*>(link_action);
				if(link_uri->getURI() != NULL)
				{
					//char *buff = convertToANSI(link_uri->getURI());
					//printf("%s\n", buff);
				}
			}
			//action->type = POPPLER_ACTION_URI;
			//build_uri (action, dynamic_cast <LinkURI *> (link));
			break;
			case actionNamed:
				//action->type = POPPLER_ACTION_NAMED;
				//build_named (action, dynamic_cast <LinkNamed *> (link));
				break;
			case actionMovie:
				//action->type = POPPLER_ACTION_MOVIE;
				//build_movie (action, link);
				break;
			case actionUnknown:
			default:
				//action->type = POPPLER_ACTION_UNKNOWN;
				break;
			}

			double x1, y1, x2, y2;
			link->getRect(&x1, &y1, &x2, &y2);

			pdfConvertUserToDevice(_ctx, &x1, &y1);
			pdfConvertUserToDevice(_ctx, &x2, &y2);

			if(y1 > y2)
			{
				double t = y1;
				y1 = y2;
				y2 = t;
			}

			//printf("area:%f,%f,%f,%f\n", (float)x1, (float)y1, (float)x2, (float)y2);

			if(x1 <= x && x2 >= x && y1 <= y && y2 >= y)
			{
				//printf("found link:%p\n", link);
				LEAVE_SECTION
				return link;
			}

			/*
			if (annot->getName() != NULL)
			{
				printf("has name!\n");
				char *buff = convertToANSI(annot->getName());
			}
			if (annot->getContents() != NULL)
			{
				printf("has content\n");
				char *buff = convertToANSI(annot->getContents());
			}
			*/
			//printf("annot%s\n", buff);
			//free(buff);
			//buff = convertToANSI(annot->get());
			//printf("annot%s\n", buff);
			//free(buff);

		}
	}

	delete links;

	LEAVE_SECTION
	return NULL;
}

void pdfListLinks(void *_ctx, int pagenum)
{
	struct devicecontext *ctx = (struct devicecontext*)_ctx;

	Links *links;
	Object obj;

	double width, height;
	ENTER_SECTION
	Catalog *catalog = ctx->doc->getCatalog();
	//printf("catalog:%p\n", catalog);
	if(catalog == NULL)
	{
		LEAVE_SECTION
		return;
	}

	Page *page = catalog->getPage(pagenum);
	//printf("page:%p\n", page);

	if(page == NULL)
	{
		LEAVE_SECTION
		return;
	}

	links = new Links(page->getAnnots());
	obj.free();

	if(links == NULL)
	{
		LEAVE_SECTION;
		return;
	}

	poppler_page_get_size(page, &width, &height);

	if(links != NULL)
	{
		//printf("links:%p\n", links);
		int num = links->getNumLinks();
		int i;

		//printf("num annots:%d\n", num);

		for(i = 0; i < num; i++)
		{
			AnnotLink *link;

			link = links->getLink(i);


			double x1, y1, x2, y2;
			link->getRect(&x1, &y1, &x2, &y2);

			D(kprintf("area:%f,%f,%f,%f\n", (float)x1, (float)y1, (float)x2, (float)y2));
		}
	}

	//printf("media:%f,%f\n", page->getMediaWidth(), page->getMediaHeight());
	LEAVE_SECTION
	delete links;
}

char *linkGetDescription(void *_link)
{
	AnnotLink *link = (AnnotLink*)_link;
	LinkAction *link_action = link->getAction();

	ENTER_SECTION

	switch(link_action->getKind())
	{
	case actionGoTo:
	{
		LinkGoTo *link_goto = dynamic_cast<LinkGoTo*>(link_action);
		if(link_goto->getNamedDest() != NULL)
		{
			//char *buff = convertToANSI(link_goto->getNamedDest());
			//printf("%s. page:%d\n", buff, link_goto->getDest()->getPageNum());
		}
		else if(link_goto->getDest() != NULL)
		{
			//printf("reference:%p\n", link_goto->getDest());
		}
	}
	//action->type = POPPLER_ACTION_GOTO_DEST;
	//build_goto_dest (document, action, dynamic_cast <LinkGoTo *> (link));
	break;
	case actionGoToR:
		//action->type = POPPLER_ACTION_GOTO_REMOTE;
		//build_goto_remote (action, dynamic_cast <LinkGoToR *> (link));
		break;
	case actionLaunch:
		//action->type = POPPLER_ACTION_LAUNCH;
		//build_launch (action, dynamic_cast <LinkLaunch *> (link));
		break;
	case actionURI:
	{
		LinkURI *link_uri = dynamic_cast<LinkURI*>(link_action);
		if(link_uri->getURI() != NULL)
		{
			//char *buff = convertToANSI(link_uri->getURI());
			//printf("%s\n", buff);
		}
	}
	//action->type = POPPLER_ACTION_URI;
	//build_uri (action, dynamic_cast <LinkURI *> (link));
	break;
	case actionNamed:
		//action->type = POPPLER_ACTION_NAMED;
		//build_named (action, dynamic_cast <LinkNamed *> (link));
		break;
	case actionMovie:
		//action->type = POPPLER_ACTION_MOVIE;
		//build_movie (action, link);
		break;
	case actionUnknown:
	default:
		//action->type = POPPLER_ACTION_UNKNOWN;
		break;
	}

	LEAVE_SECTION
	return NULL;
}

int pdfGetActionPageFromLink(void *_ctx, void *_link)
{
	ENTER_SECTION
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	AnnotLink *link = (AnnotLink*)_link;
	int page = actionGetPage(ctx->doc, link->getAction());
	LEAVE_SECTION
	return page;
}

static struct searchresult *nextsearchresult(struct devicecontext *ctx, int direction)
{
	if(direction > 0)
	{
		return (struct searchresult*)GetSucc(ctx->search.currentsearchresult);
	}
	else
	{
		return (struct searchresult*)GetPred(ctx->search.currentsearchresult);
	}
}

/* result list is always built in same order. it is traversal order that differs */

int pdfSearch(void *_ctx, int *page, char *phrase, int direction, double *x1, double *y1, double *x2, double *y2)
{
	ENTER_SECTION
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	int found = FALSE;

	if((ctx->search.phrase == NULL || 0 == strcmp(ctx->search.phrase, phrase)) && ctx->search.page == *page && (ctx->search.currentsearchresult != NULL && nextsearchresult(ctx, direction) == NULL))
	{
		/* if same search criteria, same page but no more results */

		free(ctx->search.phrase);

		while(!IsListEmpty((struct List*)&ctx->search.searchresultlist))
		{
			struct searchresult *srn = (struct searchresult*)GetHead(&ctx->search.searchresultlist);
			REMOVE(srn);
			free(srn);
		}

		ctx->search.phrase = strdup(phrase);
		ctx->search.currentsearchresult = NULL;
		LEAVE_SECTION
		return PDFSEARCH_NEXTPAGE; 
	
	}
	else if(ctx->search.phrase == NULL || 0 != strcmp(ctx->search.phrase, phrase) || ctx->search.page != *page)
	{
		/* if new phrase or new page fill the list with new results */

		free(ctx->search.phrase);

		while(!IsListEmpty((struct List*)&ctx->search.searchresultlist))
		{
			struct searchresult *srn = (struct searchresult*)GetHead(&ctx->search.searchresultlist);
			REMOVE(srn);
			free(srn);
		}

		ctx->search.phrase = strdup(phrase);
		ctx->search.page = *page;
		ctx->search.currentsearchresult = NULL;

		/* new search */

		WCHAR *phraseUCS4 = convertToUCS4(phrase);
		TextOutputDev *text_dev = new TextOutputDev(NULL, gTrue, 0, gFalse, gFalse);
		
		{
			double xMin, yMin, xMax, yMax;
			double height;
			Page *pdfpage = ctx->doc->getCatalog()->getPage(*page);

			pdfpage->display(text_dev, 72, 72, 0,
							 gTrue, gTrue, gFalse);

			height = pdfpage->getMediaHeight();

			xMin = 0;
			yMin = 0;

			while(text_dev->findText((Unicode*)phraseUCS4, strlen(phrase),
			                         gFalse, gTrue, // startAtTop, stopAtBottom
			                         gTrue, gFalse, // startAtLast, stopAtLast
			                         gFalse, gFalse, // caseSensitive, backwards
									 gFalse,        // wholeWord
			                         &xMin, &yMin, &xMax, &yMax))
			{
				double ctm[6];
				struct searchresult *searchresult = (struct searchresult*)calloc(1, sizeof(*searchresult));
				searchresult->x1 = xMin;
				searchresult->y1 = height - yMax;
				searchresult->x2 = xMax;
				searchresult->y2 = height - yMin;

				text_dev->cvtDevToUser(searchresult->x1, searchresult->y1, &searchresult->x1, &searchresult->y1);
				text_dev->cvtDevToUser(searchresult->x2, searchresult->y2, &searchresult->x2, &searchresult->y2);
				searchresult->x1 /= pdfpage->getMediaWidth();
				searchresult->y1 /= pdfpage->getMediaHeight();
				searchresult->x2 /= pdfpage->getMediaWidth();
				searchresult->y2 /= pdfpage->getMediaHeight();

				ADDTAIL(&ctx->search.searchresultlist, searchresult);
				found = TRUE;
			}
		}
		
		delete text_dev;
		free(phraseUCS4);

		if (found == FALSE)
		{
			LEAVE_SECTION
			return PDFSEARCH_NEXTPAGE;
		}
		
		if (found)
		{
			if(direction > 0)
				ctx->search.currentsearchresult = (struct searchresult*)GetHead(&ctx->search.searchresultlist);
			else
				ctx->search.currentsearchresult = (struct searchresult*)GetTail(&ctx->search.searchresultlist);
				
			ctx->search.page = *page;
		}

	}
	else if(ctx->search.phrase != NULL && 0 == strcmp(ctx->search.phrase, phrase) && ctx->search.page == *page && ctx->search.currentsearchresult != NULL && nextsearchresult(ctx, direction) != NULL)
	{

		/* same phrase, same page and not empty pool of results */

		ctx->search.currentsearchresult = nextsearchresult(ctx, direction);
		found = TRUE;
	}

	if (ctx->search.currentsearchresult != NULL)
	{
		*x1 = ctx->search.currentsearchresult->x1;
		*y1 = ctx->search.currentsearchresult->y1;
		*x2 = ctx->search.currentsearchresult->x2;
		*y2 = ctx->search.currentsearchresult->y2;
	}

	LEAVE_SECTION
	return found ? PDFSEARCH_FOUND : PDFSEARCH_NOTFOUND;
}

static char *goo_string_to_utf8(GooString *s)
{
	char *result;

	if(s->hasUnicodeMarker())
	{
		result = convertToANSI(s);
	}
	else
	{
		int len;
		Unicode *ucs4_temp;
		int i;

		len = s->getLength();
		ucs4_temp = (Unicode*)calloc(sizeof(*ucs4_temp), len + 1);
		for(i = 0; i < len; ++i)
		{
			ucs4_temp[i] = pdfDocEncoding[(unsigned char)s->getChar(i)];
		}
		ucs4_temp[i] = 0;

		result = convertUTF32ToANSI(ucs4_temp, s->getLength());

		free(ucs4_temp);
	}

	return result;
}

static char *info_dict_get_string(Dict *info_dict, const char *key)
{
	GooString *goo_value;
	char *result;
	Object obj;

	if(!info_dict->lookup((char *)key, &obj)->isString())
	{
		obj.free();
		return NULL;
	}

	goo_value = obj.getString();
	result = goo_string_to_utf8(goo_value);

	obj.free();
	return result;
}

static struct pdfAttribute *buildStringAttribute(char *value)
{
	struct pdfAttribute *attr = (struct pdfAttribute*)malloc(sizeof(*attr));
	if(attr != NULL)
	{
		attr->type = PDFATTRTYPE_STRING;
		attr->value.s = value;
	}

	return attr;
}

struct pdfAttribute *pdfGetAttr(void *_ctx, int property)
{
	ENTER_SECTION
	struct devicecontext *ctx = (struct devicecontext*)_ctx;
	Object obj;
	struct pdfAttribute *ret = NULL;

	ctx->doc->getDocInfo(&obj);
	if(!obj.isDict())
	{
		LEAVE_SECTION;
		return NULL;
	}

	switch(property)
	{
	case PDFATTR_TITLE:
		ret = buildStringAttribute(info_dict_get_string(obj.getDict(), "Title"));
		break;
	};

	LEAVE_SECTION;
	return ret;

}

void pdfFreeAttr(void *_ctx, struct pdfAttribute *attr)
{
	if(attr != NULL)
	{
		if(attr->value.s != NULL && attr->type == PDFATTRTYPE_STRING)
			free(attr->value.s);

		free(attr);
	}
}

/* annotations */

struct MinList *pdfGetAnnotations(void *_ctx, int page)
{
	ENTER_SECTION
	struct devicecontext *ctx = (struct devicecontext*)_ctx; 
	Page *pdfpage = ctx->doc->getCatalog()->getPage(page);
	Annots *annots = pdfpage->getAnnots();

	if (annots == NULL || annots->getNumAnnots() == 0)
	{
		LEAVE_SECTION
		return NULL;
	}

	struct MinList *l = (struct MinList*)calloc(1, sizeof(*l));
	if (l == NULL)
	{	
		LEAVE_SECTION
		return NULL;
	}	
	
	NEWLIST(l);
	
	for(int i=0; i<annots->getNumAnnots(); i++)
	{
		Annot *pdfAnnot = annots->getAnnot(i);
		struct pdfAnnotation *annot = NULL;
		
		switch (pdfAnnot->getType ())
		{
			case Annot::typeText:
			case Annot::typeLink:
			case Annot::typeFreeText:
			case Annot::typeLine:
			case Annot::typeSquare:
			case Annot::typeCircle:
			case Annot::typePolygon:
			case Annot::typePolyLine:
			case Annot::typeHighlight:
			case Annot::typeUnderline:
			case Annot::typeSquiggly:
			case Annot::typeStrikeOut:
			case Annot::typeStamp:
			case Annot::typeCaret:
			case Annot::typeInk:
			case Annot::typePopup:
			case Annot::typeFileAttachment:
			case Annot::typeSound:
			case Annot::typeMovie:
			case Annot::typeWidget:
			case Annot::typeScreen:
			case Annot::typePrinterMark:
			case Annot::typeTrapNet:
			case Annot::typeWatermark:
			case Annot::type3D:
				if (pdfAnnot->getContents() != NULL)
				{
					annot = (struct pdfAnnotation*)calloc(1, sizeof(*annot));
					if (annot != NULL)
					{
						annot->type = 0; // TODO
						//annot->author = convertToANSI(pdfAnnot->getAuthor());
						annot->contents = convertToANSI(pdfAnnot->getContents());
					}
				}
				break;
			default:
			{
			}
		}
		
		if (annot != NULL)
		{
			int rotation = 0;
			double width = pdfGetPageMediaWidth(_ctx, page);
			double height = pdfGetPageMediaHeight(_ctx, page);
			PDFRectangle rect;
			
			if (!(pdfAnnot->getFlags () & Annot::flagNoRotate))
				rotation = pdfpage->getRotate();

			rect = *(pdfAnnot->getRect());

			switch (rotation)
			{
				case 90:
					annot->x1 = rect.y1;
					annot->y1 = height - rect.x2;
					annot->x2 = annot->x1 + (rect.y2 - rect.y1);
					annot->y2 = annot->y1 + (rect.x2 - rect.x1);
					break;
				case 180:
					annot->x1 = width - rect.x2;
					annot->y1 = height - rect.y2;
					annot->x2 = annot->x1 + (rect.x2 - rect.x1);
					annot->y2 = annot->y1 + (rect.y2 - rect.y1);
					break;
				case 270:
					annot->x1 = width - rect.y2;
					annot->y1 = rect.x1;
					annot->x2 = annot->x1 + (rect.y2 - rect.y1);
					annot->y2 = annot->y1 + (rect.x2 - rect.x1);
					break;
				default:
					annot->x1 = rect.x1;
					annot->y1 = rect.y1;
					annot->x2 = rect.x2;
					annot->y2 = rect.y2;
			}
			annot->y1 = height - annot->y1;
			annot->y2 = height - annot->y2;		

			annot->x1 /= pdfpage->getMediaWidth();
			annot->x2 /= pdfpage->getMediaWidth();
			annot->y1 /= pdfpage->getMediaHeight();
			annot->y2 /= pdfpage->getMediaHeight();

			ADDTAIL(l, annot);
		}
	}
		

	LEAVE_SECTION
	return l;
}

