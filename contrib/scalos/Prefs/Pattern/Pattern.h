// Pattern.c
// $Date$
// $Revision$

//----------------------------------------------------------------------------

#undef	d1
#undef	d2

#define	d1(x)	;
#define	d2(x)	x;

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

struct ScalosPattern_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

//----------------------------------------------------------------------------

