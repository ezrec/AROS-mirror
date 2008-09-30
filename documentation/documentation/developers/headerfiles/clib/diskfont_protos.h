#ifndef CLIB_DISKFONT_PROTOS_H
#define CLIB_DISKFONT_PROTOS_H

/*
    *** Automatically generated from 'diskfont.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <dos/dos.h>
#include <graphics/text.h>
AROS_LP1(struct TextFont *, OpenDiskFont,
         AROS_LPA(struct TextAttr *, textAttr, A0),
         LIBBASETYPEPTR, DiskfontBase, 5, Diskfont
);
AROS_LP3(LONG, AvailFonts,
         AROS_LPA(STRPTR, buffer, A0),
         AROS_LPA(LONG, bufBytes, D0),
         AROS_LPA(LONG, flags, D1),
         LIBBASETYPEPTR, DiskfontBase, 6, Diskfont
);
AROS_LP2(struct FontContentsHeader *, NewFontContents,
         AROS_LPA(BPTR, fontsLock, A0),
         AROS_LPA(STRPTR, fontName, A1),
         LIBBASETYPEPTR, DiskfontBase, 7, Diskfont
);
AROS_LP1(void, DisposeFontContents,
         AROS_LPA(struct FontContentsHeader *, fontContentsHeader, A1),
         LIBBASETYPEPTR, DiskfontBase, 8, Diskfont
);
AROS_LP2(struct DiskFont *, NewScaledDiskFont,
         AROS_LPA(struct TextFont *, sourceFont, A0),
         AROS_LPA(struct TextAttr *, destTextAttr, A1),
         LIBBASETYPEPTR, DiskfontBase, 9, Diskfont
);

#endif /* CLIB_DISKFONT_PROTOS_H */
