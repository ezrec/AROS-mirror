#include "ttfglyphengine.h"

#include <diskfont/glyph.h>

void CloseEngine(struct GlyphEngine *engine)
{
    FreeGE((TTF_GlyphEngine *)engine);
    
    return;
}
