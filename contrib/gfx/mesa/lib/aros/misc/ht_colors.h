#include <exec/types.h>
#include <intuition/screens.h>

#define RGBA(amesa,r,g,b,a) GetPen(pal,htc_NumCols,r,g,b)
#define RGB(amesa,r,g,b) GetPen(pal,htc_NumCols,r,g,b)

struct ColorEntry
	{
	UBYTE r,g,b,pad;
	UBYTE ar,ag,ab,pad2;
	LONG PenNo;
	};

void AllocCMap(struct Screen *);
void FreeCMap(struct Screen *);
#ifdef __GNUC__
ULONG GetPen(register struct ColorEntry *Cols,register UWORD Number,register long R,register long G,register long B);
#else
__asm ULONG GetPen(register __a0 struct ColorEntry *Cols,register __d0 UWORD Number,register __d1 long R,register __d2 long G,register __d3 long B);
#endif

extern struct ColorEntry pal[];

#define htc_NumCols 189
