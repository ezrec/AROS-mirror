/*
 * Wrap poppler document class functions into c functions.
 */

//#define USE_SPLASH

#include "Object.h"

#define AROS_ALMOST_COMPATIBLE
#define _NO_PPCINLINE
#include <proto/keymap.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <clib/debug_protos.h>
#include <exec/semaphores.h>
#include <exec/lists.h>
#include <constructor.h>

#define USE_FLOAT 1
#include <poppler-config.h>

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "PDFDoc.h"
#include "Outline.h"
#include "Link.h"
#include "GlobalParams.h"
#include "goo/GooList.h"

#define USE_SPLASH
#ifdef USE_SPLASH
#include "splash/SplashBitmap.h"
#include "splash/Splash.h"
#include "SplashOutputDev.h"
#include "TextOutputDev.h"
#else
#include "CairoOutputDev.h"
#endif

#include "poppler.h"
#include "poppler_io.h"


extern struct Library *LocaleBase;
#define LOCALE_BASE_NAME LocaleBase
#define IsUnicode(__p0, __p1) \
	(((ULONG (*)(APTR , ULONG , void *))*(void**)((long)(LOCALE_BASE_NAME) - 430))(__p0, __p1, (void*)(LOCALE_BASE_NAME)))

#warning TODO: make the semaphore per-document
static struct SignalSemaphore semaphore;

extern struct Library *CairoBase;

struct searchresult {
	struct MinNode n;
	double x1, y1, x2, y2;		// bounding rectangle in pdf points
};

struct searchcontext {
	MinList searchresultlist;
	struct searchresult *currentsearchresult; // current node on a page;
	int page;			
	char *phrase;						// on which page we are currently searching
};

#ifdef USE_SPLASH
struct devicecontext {
	PDFDoc *doc;
	BaseStream *stream;
	SplashOutputDev *dev;
	struct searchcontext search;
};
#else
struct devicecontext {
	PDFDoc *doc;
	BaseStream *stream;
	CairoOutputDev *dev;
	cairo_surface_t *surface;
	cairo_t *cairo;
	struct searchcontext search;
};
#endif

static CONSTRUCTOR_P(init_poppler, 0)
{
	InitSemaphore(&semaphore);
	return 0;
}

static DESTRUCTOR_P(cleanup_poppler, 0)
{
	kprintf("destroy poppler:%p\n", &semaphore);
}

#define ENTER_SECTION ObtainSemaphore(&semaphore);
//#define ENTER_SECTION
#define LEAVE_SECTION ReleaseSemaphore(&semaphore);
//#define LEAVE_SECTION


void *pdfNew(const char *fname)
{
	return NULL;
}

void pdfDelete(void *_ctx)
{
}

float pdfGetPageMediaWidth(void *_ctx, int page)
{
	return 0;
}

float pdfGetPageMediaHeight(void *_ctx, int page)
{
	return 0;
}

int	pdfGetPagesNum(void *_ctx)
{
	return 0;
}

static GBool renderabortchk(void *data)
{
	printf("abort check...\n");
	return 0;
}

void pdfDisplayPageSlice(void *_ctx, int page, double scale, int rotate,
			int	useMediaBox, int crop, int printing, int sliceX, int sliceY, int sliceW, int sliceH, int (*abortcheckcbk)(void *), void *abortcheckcbkdata)
{
}

unsigned char *pdfGetBitmapRowData(void *_ctx, int row)
{
	return NULL;
}

int pdfGetBitmapWidth(void *_ctx)
{
	return 0;
}

int pdfGetBitmapHeight(void *_ctx)
{
	return 0;
}


struct MinList *pdfGetOutlines(void *_ctx)
{
	return NULL;
}

int outlineHasChildren(void *_outline)
{
	return FALSE;
}

struct MinList *outlineGetChildren(void *_outline)
{
	return NULL;
}


char *outlineGetTitle(void *_outline)
{
	return NULL;
}

int outlineGetPage(void *_outline)
{
	return 0;
}



void
poppler_page_get_size (Page *page,
		       double      *width,
		       double      *height)
{
}

void pdfConvertUserToDevice(void *_ctx, double* x, double* y)
{
}

void pdfConvertDeviceToUser(void *_ctx, int page, double x, double y, int *ix, int *iy)
{
}

void pdfLock(void *_ctx)
{
}
void pdfRelease(void *_ctx)
{
}


void *pdfFindLink(void *_ctx, int pagenum, int x, int y)
{
	return NULL;
}

void pdfListLinks(void *_ctx, int pagenum)
{
}

char *linkGetDescription(void *_link)
{
	return NULL;
}

int pdfGetActionPageFromLink(void *_ctx, void *_link)
{
	return 0;
}

int pdfSearch(void *_ctx, int *page, char *phrase, double *x1, double *y1, double *x2, double *y2)
{
	return 0;
}
