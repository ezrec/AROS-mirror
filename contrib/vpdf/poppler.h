#ifndef POPPLER_H

#include <exec/types.h>
#include <exec/lists.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void *pdfNew(const char *fname);
void pdfConvertUserToDevice(void *_ctx, double *x, double *y);
void pdfConvertDeviceToUser(void *_ctx, int page, double x, double y, int *ux, int *uy);

void pdfDelete(void *_ctx);

float pdfGetPageMediaWidth(void *_ctx, int page);
float pdfGetPageMediaHeight(void *_ctx, int page);
int pdfGetDocumentDimensions(void *_ctx, float *width, float *height);
int	pdfGetPagesNum(void *_ctx);

int pdfDisplayPageSlice(void *_ctx, int page, double scale, int rotate, int useMediaBox, int crop, int printing,
		int sliceX, int sliceY, int sliceW, int sliceH, int (*abortcheckcbk)(void *), void *abortcheckcbkdata);

/* output bitmap */

struct pdfBitmap
{
	int width;
	int height;
	int stride;	/* in bytes */
	unsigned char *data;
};

unsigned char *pdfGetBitmapRowData(void *_ctx, int row);
int pdfGetBitmapWidth(void *_ctx);
int pdfGetBitmapHeight(void *_ctx);
void pdfGetBitmap(void *_ctx, struct pdfBitmap *bm);

void *pdfGetPage(void *_ctx, int page);

/* outline handling functions */
struct MinList *pdfGetOutlines(void *_ctx);
char *outlineGetTitle(void *_outline);
struct MinList *outlineGetChildren(void *_outline);
int outlineHasChildren(void *_outline);
int outlineGetPage(void *_outline);

/* links */
void pdfListLinks(void *_ctx, int page);
void *pdfFindLink(void *_ctx, int page, int x, int y);
char *linkGetDescription(void *_link);
int pdfGetActionPageFromLink(void *_doc, void *_link);

/* searching */

int pdfSearch(void *_ctx, int *page, char *phrase, int direction, double *x1, double *y1, double *x2, double *y2);

/* locking (too lowlevel?) */

void pdfLock(void *_ctx);
void pdfRelease(void *_ctx);


/* document properties (sucky api?)*/

enum {
	PDFATTR_TITLE = 1,
};

enum {
	PDFATTRTYPE_STRING = 1,
	PDFATTRTYPE_INTEGER,
};

struct pdfAttribute
{
	int type;
	union value{
		char *s;
		int i;
	}value;
};

struct pdfAttribute *pdfGetAttr(void *_ctx, int property);
void pdfFreeAttr(void *_ctx, struct pdfAttribute *attr);

/* printing (PS) and Turboprint */

// based on radiobuttons from printer class
#define VPDF_PRINT_POSTSCRIPT 0
#define VPDF_PRINT_TURBOPRINT 1

void *pdfPrintInit(void *_ctx, const char *path, int first, int last, int format);
int pdfPrintPage(void *_pctx, int page, int center);
void pdfPrintEnd(void *_pctx);

/* annotations */

struct pdfAnnotation
{
	struct MinNode n;

	int type;
	double x1, y1, x2, y2; // area on image
	char *contents;        // in local charset
	char *author;          // in local charset	
	void *obj;             // attached mui object
	
};

struct MinList *pdfGetAnnotations(void *_ctx, int page);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
