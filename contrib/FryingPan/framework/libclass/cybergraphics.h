/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _LIBCLASS_CGX_H
#define _LIBCLASS_CGX_H

#include "exec.h"
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#include <Generic/Types.h>

struct TagItem;
struct List;
struct RastPort;
struct Hook;
struct ViewPort;

   BEGINDECL(CyberGfx, "cybergraphics.library")
      PROC0(               resvd0,                 5);
      PROC0(               resvd1,                 6);
      PROC0(               resvd2,                 7);
      PROC0(               resvd3,                 8);
      FUNC1(bool,          IsCyberModeID,          9, uint32,              displayid,     d0);
      FUNC1(uint32,        BestCModeIDTagList,    10, const TagItem*,      tags,          a0);
      PROC0(               resvd4,                10);   // os4 vararg
      FUNC2(uint32,        CModeRequestTagList,   11, void*,               modereqeust,   a0, const TagItem*,        requesttags,   a1);
      PROC0(               resvd5,                11);   // os4 vararg
      FUNC1(List*,         AllocCModeListTagList, 12, const TagItem*,      modelisttags,  a1);
      PROC0(               resvd6,                12);   // os4 vararg
      PROC1(               FreeCModeList,         13, List*,               modelist,      a0);
      PROC0(               resvd7,                14);
      FUNC10(int32,        ScalePixelArray,       15, void*, srcRect, a0, uint32, srcw, d0, uint32, srch, d1, uint32, srcmod, d2, RastPort*, rp, a1, uint32, destx, d3, uint32, desty, d4, uint32, destw, d5, uint32, desth, d6, uint32, srcfmt, d7);
      FUNC2(uint32,        GetCyberMapAttr,       16, BitMap*,             cyberbitmap,   a0, uint32,                cyberattrtag,  d0);
      FUNC2(uint32,        GetCyberIDAttr,        17, uint32,              cyberidattr,   d0, uint32,                cybermodeid,   d1);
      FUNC3(uint32,        ReadRGBPixel,          18, RastPort*,           rastport,      a1, uint32,    x, d0, uint32,    y, d1);
      FUNC4(int32,         WriteRGBPixel,         19, RastPort*,           rastport,      a1, uint32,    x, d0, uint32,    y, d1, uint32,    argb, d2);
      FUNC10(uint32,       ReadPixelArray,        20, void*, destrect, a0, uint32, destx, d0, uint32, desty, d1, uint32, destmod, d2, RastPort*, rastport, a1, uint32, srcx, d3, uint32, srcy, d4, uint32, sizex, d5, uint32, sizey, d6, uint32, destfmt, d7);
      FUNC10(uint32,       WritePixelArray,       21, void*, srcrect, a0, uint32, srcx, d0, uint32, srcy, d1, uint32, srcmod, d2, RastPort*, rastport, a1, uint32, dstx, d3, uint32, dsty, d4, uint32, sizex, d5, uint32, sizey, d6, uint32, srcfmt, d7);
      FUNC7(uint32,        MovePixelArray,        22, uint32, srcx, d0, uint32, srcy, d1, RastPort*, rastport, a1, uint32, dstx, d2, uint32, dsty, d3, uint32, sizex, d4, uint32, sizey, d5);
      PROC0(               resvd8,                23);
      FUNC5(uint32,        InvertPixelArray,      24, RastPort*, rastport, a1, uint32, destx, d0, uint32, desty, d1, uint32, sizex, d2, uint32, sizey, d3);
      FUNC6(uint32,        FillPixelArray,        25, RastPort*, rastport, a1, uint32, destx, d0, uint32, desty, d1, uint32, sizex, d2, uint32, sizey, d3, uint32, argb, d4);
      PROC3(               DoCDrawMethodTagList,  26, const Hook*, hook, a0, RastPort*, rastport, a1, const TagItem*, taglist, a2);
      PROC0(               resvd9,                26);   // os4 varargs
      PROC2(               CVideoCtrlTagList,     27, ViewPort*, viewport, a0, const TagItem*, tags, a1);
      PROC0(               resvd10,               27);   // os4 varargs
      FUNC2(void*,         LockBitMapTagList,     28, void*, bitmap, a0, const TagItem*, tags, a1);
      PROC0(               resvd11,               28);   // os4 varargs
      PROC1(               UnLockBitMap,          29, void*, handle, a0);
      PROC2(               UnLockBitMapTagList,   30, void*, handle, a0, const TagItem*, tags, a1);
      PROC0(               resvd12,               30);   // os4 varargs
      FUNC7(uint32,        ExtractColor,          31, RastPort*, raster, a0, BitMap*, bitmap, a1, uint32, colour, d0, uint32, srcx, d1, uint32, srcy, d2, uint32, width, d3, uint32, height, d4);
      PROC0(               resvd13,               32);   // os4 varargs
      FUNC11(uint32,       WriteLUTPixelArray,    33, void*, srcrect, a0, uint32, srcx, d0, uint32, srcy, d1, uint32, srcmod, d2, RastPort*, rastport, a1, void*, colortab, a2, uint32, destx, d3, uint32, desty, d4, uint32, sizex, d5, uint32, sizey, d6, uint32, ctformat, d7);
   ENDDECL

#endif
