#ifndef POPPLER_DEVICE_H

#ifdef USE_SPLASH
struct devicecontext
{
	PDFDoc *doc;
	BaseStream *stream;
	SplashOutputDev *dev;
	struct searchcontext search;
	int pagesnum;
	float documentwidth, documentheight;
};
#else
struct devicecontext
{
	PDFDoc *doc;
	BaseStream *stream;
	CairoOutputDev *dev;
	cairo_surface_t *surface;
	TextOutputDev *selection_dev;   // cached output device used for selection
	int selection_pagenum;          // last page num used for selection
	cairo_t *cairo;
	struct searchcontext search;
	int pagesnum;
	float documentwidth, documentheight;
};
#endif

#endif