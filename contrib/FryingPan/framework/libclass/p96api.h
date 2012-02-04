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

#ifndef _LIBCLASS_PICASSO96API_H
#define _LIBCLASS_PICASSO96API_H

#include "exec.h"
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>

struct BitMap;
struct TagItem;
struct List;
struct Screen;
struct RenderInfo;
struct TrueColorInfo;
struct Window;
struct MsgPort;
struct IntuiMessage;


   BEGINDECL(Picasso96, "Picasso96API.library")
      FUNC6(BitMap*,    AllocBitMap,            5, uint32, sizex, d0, uint32, sizey, d1, uint32, depth, d2, uint32, flags, d3, BitMap*, friendbmap, a0, uint32, format, d4);   // RGBFTYPE?
	   PROC1(            FreeBitMap,             6, BitMap*, bmap, a0);
      FUNC2(uint32,     GetBitMapAttr,          7, BitMap*, bmap, a0, uint32, attr, d0);
      FUNC3(int32,      LockBitMap,             8, BitMap*, bmap, a0, uint8*, buffer, a1, uint32, size, d0);
      PROC2(            UnlockBitMap,           9, BitMap*, bmap, a0, int32, lock, d0);
      FUNC1(uint32,     BestModeIDTagList,     10, const TagItem*, tags, a0);
      PROC0(            os4varargs1,           10);
      FUNC1(uint32,     RequestModeIDTagList,  11, const TagItem*, tags, a0);
      PROC0(            os4varargs2,           11);
      FUNC1(List*,      AllocModeListTagList,  12, const TagItem*, tags, a0);
      PROC0(            os4varargs3,           12);
      PROC1(            FreeModeList,          13, List*, list, a0);
      FUNC2(uint32,     GetModeIDAttr,         14, uint32, mode, d0, uint32, attribute, d1);
      FUNC1(Screen*,    OpenScreenTagList,     15, const TagItem*, tags, a0);
      PROC0(            os4varargs4,           15);
      FUNC1(bool,       CloseScreen,           16, Screen*, screen, a0);
      PROC8(            WritePixelArray,       17, RenderInfo*, ri, a0, uint32, srcx, d0, uint32, srcy, d1, RastPort*, rp, a1, uint32, dstx, d2, uint32, dsty, d3, uint32, sizex, d4, uint32, sizey, d5);
      PROC8(            ReadPixelArray,        18, RenderInfo*, ri, a0, uint32, dstx, d0, uint32, dsty, d1, RastPort*, rp, a1, uint32, srcx, d2, uint32, srcy, d3, uint32, sizex, d4, uint32, sizey, d5);
      FUNC4(uint32,     WritePixel,            19, RastPort*, rp, a0, uint32, x, d0, uint32, y, d1, uint32, color, d2);
      FUNC3(uint32,     ReadPixel,             20, RastPort*, rp, a0, uint32, x, d0, uint32, y, d1);
      PROC6(            RectFill,              21, RastPort*, rp, a0, uint32, minx, d0, uint32, miny, d1, uint32, maxx, d2, uint32, maxy, d3, uint32, color, d4);
      PROC8(            WriteTrueColorData,    22, TrueColorInfo*, tci, a0, uint32, srcx, d0, uint32, srcy, d1, RastPort*, rastport, a1, uint32, dstx, d2, uint32, dsty, d3, uint32, sizex, d4, uint32, sizey, d5);
      PROC8(            ReadTrueColorData,     23, TrueColorInfo*, tci, a0, uint32, dstx, d0, uint32, dsty, d1, RastPort*, rastport, a1, uint32, srcx, d2, uint32, srcy, d3, uint32, sizex, d4, uint32, sizey, d5);
      FUNC1(Window*,    PIP_OpenTagList,       24, const TagItem*, tags, a0);
      PROC0(            os4varargs5,           24);
      FUNC1(bool,       PIP_Close,             25, Window*, win, a0);
      FUNC2(int32,      PIP_SetTagList,        26, Window*, win, a0, const TagItem*, tags, a1);
      PROC0(            os4varargs6,           26);
      FUNC2(int32,      PIP_GetTagList,        27, Window*, win, a0, const TagItem*, tags, a1);
      PROC0(            os4varargs7,           27);
      FUNC1(IntuiMessage*, PIP_GetIMsg,        28, MsgPort*, port, a0);
      PROC1(            PIP_ReplyIMsg,         29, IntuiMessage*, imsg, a0);
      FUNC1(int32,      GetRTGDataTagList,     30, const TagItem*, tags, a0);
      PROC0(            os4varargs8,           30);
      FUNC2(int32,      GetBoardDataTagList,   31, uint32, board, d0, const TagItem*, tags, a0);
      PROC0(            os4varargs9,           31);
      FUNC2(uint32,     EncodeColor,           32, uint32, RGBFormat, d0, uint32, color, d1);
   ENDDECL

#endif
