// loaddt.h
// $Date$
// $Revision$

//--------------------------------------------------------------------------------

extern struct Library *DataTypesBase;

//--------------------------------------------------------------------------------

struct ScalosBitMapAndColor *DoLoadDT(STRPTR source, struct RastPort *rast,
	ULONG ScaledWidth, ULONG ScaledHeight,
	struct Screen *screen);
void ShowError(const char *str, ...);

//--------------------------------------------------------------------------------

extern int KPrintF(const char *fmt, ...);

//--------------------------------------------------------------------------------

