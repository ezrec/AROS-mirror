/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Support functions for freedom.library
    Lang: English
*/

#include <string.h>

#include "freedom_intern.h"

void ClearGlyphRequest(struct MyGlyphEngine *glyphEngine)
{
    memset(&glyphEngine->gle_GlyphRequest,0,sizeof(struct MyGlyphReq));

    glyphEngine->gle_GlyphRequest.gr_DeviceDPI_X = 72;
    glyphEngine->gle_GlyphRequest.gr_DeviceDPI_Y = 72;
    glyphEngine->gle_GlyphRequest.gr_PointHeight = 10;
    glyphEngine->gle_GlyphRequest.gr_GlyphWidth = 0;
    glyphEngine->gle_GlyphRequest.gr_SetFactor = 0x10000; /* 1.0 */
    glyphEngine->gle_GlyphRequest.gr_RotateSin = 0;
    glyphEngine->gle_GlyphRequest.gr_RotateCos = 0x10000;
    glyphEngine->gle_GlyphRequest.gr_ShearSin = 0;
    glyphEngine->gle_GlyphRequest.gr_ShearCos = 0x10000;

}


