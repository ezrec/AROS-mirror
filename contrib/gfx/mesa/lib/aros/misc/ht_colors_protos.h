#if !defined(HT_COLORS_PROTOS_H)
#define HT_COLORS_PROTOS_H

#if !defined(HT_COLORS_H)
#include "ht_colors.h"
#endif

extern void AllocCMap(struct Screen *);
extern void FreeCMap(struct Screen *);
extern ULONG GetPen(struct ColorEntry *Cols, UWORD Number, long R, long G, long B);

extern struct ColorEntry pal[];

#endif /* HT_COLORS_H */
