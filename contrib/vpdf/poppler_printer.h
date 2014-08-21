#ifndef POPPLER_PRINTER_H


#include <exec/types.h>
#include <exec/lists.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* printing (PS) and Turboprint */

// based on radiobuttons from printer class
#define VPDF_PRINT_POSTSCRIPT_2 0
#define VPDF_PRINT_POSTSCRIPT_3 1
#define VPDF_PRINT_TURBOPRINT   2

struct printerjob
{
	int  first;
	int  last;
	int  step;
	char  mode;
	char reverse;
	char autorotatescale;
	char selection_mode;
	int  copies;
	char collate;
	int  total_pages;
	char page_scaling;
	int pages_per_sheet;
	char pages_order;
	int  pages[64];
	char output[1024];
};

void *pdfPrintInit(void *_ctx, const char *path, int first, int last, struct printerjob *pj);
int pdfPrintPage(void *_pctx, int *page, int center, int);
void pdfPrintEnd(void *_pctx);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif