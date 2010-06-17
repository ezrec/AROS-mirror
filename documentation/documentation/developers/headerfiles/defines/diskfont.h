#ifndef DEFINES_DISKFONT_H
#define DEFINES_DISKFONT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/diskfont/diskfont.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for diskfont
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __OpenDiskFont_WB(__DiskfontBase, __arg1) \
        AROS_LC1(struct TextFont *, OpenDiskFont, \
                  AROS_LCA(struct TextAttr *,(__arg1),A0), \
        struct Library *, (__DiskfontBase), 5, Diskfont)

#define OpenDiskFont(arg1) \
    __OpenDiskFont_WB(DiskfontBase, (arg1))

#define __AvailFonts_WB(__DiskfontBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, AvailFonts, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__DiskfontBase), 6, Diskfont)

#define AvailFonts(arg1, arg2, arg3) \
    __AvailFonts_WB(DiskfontBase, (arg1), (arg2), (arg3))

#define __NewFontContents_WB(__DiskfontBase, __arg1, __arg2) \
        AROS_LC2(struct FontContentsHeader *, NewFontContents, \
                  AROS_LCA(BPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
        struct Library *, (__DiskfontBase), 7, Diskfont)

#define NewFontContents(arg1, arg2) \
    __NewFontContents_WB(DiskfontBase, (arg1), (arg2))

#define __DisposeFontContents_WB(__DiskfontBase, __arg1) \
        AROS_LC1NR(void, DisposeFontContents, \
                  AROS_LCA(struct FontContentsHeader *,(__arg1),A1), \
        struct Library *, (__DiskfontBase), 8, Diskfont)

#define DisposeFontContents(arg1) \
    __DisposeFontContents_WB(DiskfontBase, (arg1))

#define __NewScaledDiskFont_WB(__DiskfontBase, __arg1, __arg2) \
        AROS_LC2(struct DiskFont *, NewScaledDiskFont, \
                  AROS_LCA(struct TextFont *,(__arg1),A0), \
                  AROS_LCA(struct TextAttr *,(__arg2),A1), \
        struct Library *, (__DiskfontBase), 9, Diskfont)

#define NewScaledDiskFont(arg1, arg2) \
    __NewScaledDiskFont_WB(DiskfontBase, (arg1), (arg2))

__END_DECLS

#endif /* DEFINES_DISKFONT_H*/
