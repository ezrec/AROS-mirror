// :ts=4 (Tabsize)

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
// we need to export our struct instance data because maintask needs its size

struct BitmapInst
{
//  struct SignalSemaphore   Lock;              // lock for this structure
        struct RastPort          FriendRastport;    // rastport for bitmap, besser ptr draus machen

        struct BitMap           *SrcBitmap;         // const, source to blit from
        
        APTR                     RawData;           // const, alternative source to blit from
        ULONG                    RawType;           // const, type of RawData, see defines in Scalos.h

        APTR                     Palette;           // const, guigfx
        ULONG                    NumColors;         // const, guigfx
        ULONG                    PaletteFormat;     // const, guigfx

        struct BitMap           *FriendBitmap;      // temporary bitmap that can be blitted in the most efficient way
        
        struct Screen           *Screen;            // screen that belongs to window rastport

        APTR                     Drawhandle;        // draw handle, guigfx

        APTR                     Psm;               // pen share map, guigfx
                                                                                                // a Pensharemap belongs to a screen or a private Rastport plus Colormap

        APTR                     Picture;           // picture handle, guigfx

        ULONG                    DestWidth;         // destination width of the friendbitmap (scaling)
        ULONG                    DestHeight;        // destination height of the friendbitmap (scaling)
        
        ULONG                    setDestWidth;      // destination width of the friendbitmap (scaling) as set by OM_SET
        ULONG                    setDestHeight;     // destination height of the friendbitmap (scaling) as set by OM_SET

        ULONG                    oldDestWidth;      // PreThinkWindow`s last DestWidth
        ULONG                    oldDestHeight;     // PreThinkWindow`s last DestHeight
        
        PLANEPTR                 BltMask;           // plane ptr to mask bitmap with incorrect modulos
        
        struct BitMap           *MaskBitmap;
        struct BitMap            MaskBitmapNotOk;   // corrected MaskBitmap for modulos and chipram
        BOOL                     copyorig;          // make a copy of the given data ?
};

