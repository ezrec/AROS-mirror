#ifndef _ARCH_AMIGAOS_GLYPH_H
#define _ARCH_AMIGAOS_GLYPH_H

#include "ttfglyphengine.h"

#include <diskfont/glyph.h>

int              char_to_glyph      (TTF_GlyphEngine *, int);
int              UnicodeToGlyphIndex(TTF_GlyphEngine *);
void             RenderGlyph        (TTF_GlyphEngine *);
void             XeroGlyph          (TTF_GlyphEngine *);
struct GlyphMap *GetGlyph           (TTF_GlyphEngine *);

#endif /* _ARCH_AMIGAOS_GLYPH_H */
