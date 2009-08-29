#if !defined(HT_COLORS_H)
#define HT_COLORS_H

#include <exec/types.h>
#include <intuition/screens.h>

#define htc_NumCols 189

#define RGBA(amesa,r,g,b,a) GetPen(pal,htc_NumCols,r,g,b)
#define RGB(amesa,r,g,b) GetPen(pal,htc_NumCols,r,g,b)

struct ColorEntry
{
	UBYTE r,g,b,pad;
	UBYTE ar,ag,ab,pad2;
	LONG PenNo;
};

#endif /* HT_COLORS_H */
