#ifndef _ARCH_AMIGAOS_KERNING_H
#define _ARCH_AMIGAOS_KERNING_H

#include "ttfglyphengine.h"
#include <diskfont/glyph.h>
#include <exec/lists.h>

void            FreeWidthList(struct MinList *);
struct MinList *GetWidthList(TTF_GlyphEngine *);
int             get_kerning_dir(TTF_GlyphEngine *);

#endif /* _ARCH_AMIGAOS_KERNING_H */
