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

#ifndef _LIBCLASS_INTUTION_H_
#define _LIBCLASS_INTUTION_H_
#define  CLIB_INTUITION_PROTOS_H

#if defined (_PROTO_INTUITION_H) || defined (PROTO_INTUITION_H)
#error "Please remove proto/intuition includes first!"
#endif

#define _PROTO_INTUITION_H
#define PROTO_INTUITION_H

#include <libclass/exec.h>
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS

extern class IntuitionIFace *Intuition;

#include <Generic/Types.h>
#include <intuition/classusr.h>
#include <intuition/classes.h>

   BEGINDECL(Intuition, "intuition.library")
      PROC0(OpenIntuition, 5);
      PROC1(Intuition, 6, struct InputEvent *, iEvent, a0);
      FUNC3(uint16, AddGadget, 7, struct Window *, window, a0, struct Gadget *, gadget, a1, uint16, position, d0);
      FUNC1(bool, ClearDMRequest, 8, struct Window *, window, a0);
      PROC1(ClearMenuStrip, 9, struct Window *, window, a0);
      PROC1(ClearPointer, 10, struct Window *, window, a0);
      FUNC1(bool, CloseScreen, 11, struct Screen *, screen, a0);
      PROC1(CloseWindow, 12, struct Window *, window, a0);
      FUNC0(int32, CloseWorkBench, 13);
      PROC2(CurrentTime, 14, uint32 *, seconds, a0, uint32 *, micros, a1);
      FUNC3(bool, DisplayAlert, 15, uint32, alertNumber, d0, const char *, string, a0, uint16, height, d1);
      PROC1(DisplayBeep, 16, struct Screen *, screen, a0);
      FUNC4(bool, DoubleClick, 17, uint32, sSeconds, d0, uint32, sMicros, d1, uint32, cSeconds, d2, uint32, cMicros, d3);
      PROC4(DrawBorder, 18, struct RastPort *, rp, a0, const struct Border *, border, a1, int16, leftOffset, d0, int16, topOffset, d1);
      PROC4(DrawImage, 19, struct RastPort *, rp, a0, struct Image *, image, a1, int16, leftOffset, d0, int16, topOffset, d1);
      PROC2(EndRequest, 20, struct Requester *, requester, a0, struct Window *, window, a1);
      FUNC2(struct Preferences *, GetDefPrefs, 21, struct Preferences *, preferences, a0, int16, size, d0);
      FUNC2(struct Preferences *, GetPrefs, 22, struct Preferences *, preferences, a0, int16, size, d0);
      PROC1(InitRequester, 23, struct Requester *, requester, a0);
      FUNC2(struct MenuItem *, ItemAddress, 24, const struct Menu *, menuStrip, a0, uint16, menuNumber, d0);
      FUNC2(bool, ModifyIDCMP, 25, struct Window *, window, a0, uint32, flags, d0);
      PROC0(os4_multiargs0, 0);
      //PROC8(ModifyProp, 26, struct Gadget *, gadget, a0, struct Window *, window, a1, struct Requester *, requester, a2, uint16, flags, d0, uint16, horizPot, d1, uint16, vertPot, d2, uint16, horizBody, d3, uint16, vertBody, d4);
      PROC3(MoveScreen, 27, struct Screen *, screen, a0, int16, dx, d0, int16, dy, d1);
      PROC3(MoveWindow, 28, struct Window *, window, a0, int16, dx, d0, int16, dy, d1);
      PROC3(OffGadget, 29, struct Gadget *, gadget, a0, struct Window *, window, a1, struct Requester *, requester, a2);
      PROC2(OffMenu, 30, struct Window *, window, a0, uint16, menuNumber, d0);
      PROC3(OnGadget, 31, struct Gadget *, gadget, a0, struct Window *, window, a1, struct Requester *, requester, a2);
      PROC2(OnMenu, 32, struct Window *, window, a0, uint16, menuNumber, d0);
      FUNC1(struct Screen *, OpenScreen, 33, const struct NewScreen *, newScreen, a0);
      FUNC1(struct Window *, OpenWindow, 34, const struct NewWindow *, newWindow, a0);
      FUNC0(IPTR, OpenWorkBench, 35);
      PROC4(PrintIText, 36, struct RastPort *, rp, a0, const struct IntuiText *, iText, a1, int16, left, d0, int16, top, d1);
      PROC3(RefreshGadgets, 37, struct Gadget *, gadgets, a0, struct Window *, window, a1, struct Requester *, requester, a2);
      FUNC2(uint16, RemoveGadget, 38, struct Window *, window, a0, struct Gadget *, gadget, a1);
      PROC2(ReportMouse, 39, bool, flag, d0, struct Window *, window, a0);
      PROC2(ReportMouse1, 0, struct Window *, window, d0, bool, flag, d1);
      FUNC2(bool, Request, 40, struct Requester *, requester, a0, struct Window *, window, a1);
      PROC1(ScreenToBack, 41, struct Screen *, screen, a0);
      PROC1(ScreenToFront, 42, struct Screen *, screen, a0);
      FUNC2(bool, SetDMRequest, 43, struct Window *, window, a0, struct Requester *, requester, a1);
      FUNC2(bool, SetMenuStrip, 44, struct Window *, window, a0, struct Menu *, menu, a1);
      PROC6(SetPointer, 45, struct Window *, window, a0, uint16 *, pointer, a1, int16, height, d0, int16, width, d1, int16, xOffset, d2, int16, yOffset, d3);
      PROC3(SetWindowTitles, 46, struct Window *, window, a0, const char *, windowTitle, a1, const char *, screenTitle, a2);
      PROC2(ShowTitle, 47, struct Screen *, screen, a0, bool, showIt, d0);
      PROC3(SizeWindow, 48, struct Window *, window, a0, int16, dx, d0, int16, dy, d1);
      FUNC0(struct View *, ViewAddress, 49);
      FUNC1(struct ViewPort *, ViewPortAddress, 50, const struct Window *, window, a0);
      PROC1(WindowToBack, 51, struct Window *, window, a0);
      PROC1(WindowToFront, 52, struct Window *, window, a0);
      FUNC5(bool, WindowLimits, 53, struct Window *, window, a0, int32, widthMin, d0, int32, heightMin, d1, uint32, widthMax, d2, uint32, heightMax, d3);
      FUNC3(struct Preferences *, SetPrefs, 54, const struct Preferences *, preferences, a0, int32, size, d0, bool, inform, d1);
      FUNC1(int32, IntuiTextLength, 55, const struct IntuiText *, iText, a0);
      FUNC0(bool, WBenchToBack, 56);
      FUNC0(bool, WBenchToFront, 57);
      PROC0(os4multiargs1, 0);
      //FUNC8(bool, AutoRequest, 58, struct Window *, window, a0, const struct IntuiText *, body, a1, const struct IntuiText *, posText, a2, const struct IntuiText *, negText, a3, uint32, pFlag, d0, uint32, nFlag, d1, uint16, width, d2, uint16, height, d3);
      PROC1(BeginRefresh, 59, struct Window *, window, a0);
      FUNC7(struct Window *, BuildSysRequest, 60, struct Window *, window, a0, const struct IntuiText *, body, a1, const struct IntuiText *, posText, a2, const struct IntuiText *, negText, a3, uint32, flags, d0, uint16, width, d1, uint16, height, d2);
      PROC2(EndRefresh, 61, struct Window *, window, a0, int32, complete, d0);
      PROC1(FreeSysRequest, 62, struct Window *, window, a0);
      FUNC1(int32, MakeScreen, 63, struct Screen *, screen, a0);
      FUNC0(int32, RemakeDisplay, 64);
      FUNC0(int32, RethinkDisplay, 65);
      FUNC3(void *, AllocRemember, 66, struct Remember **, rememberKey, a0, uint32, size, d0, uint32, flags, d1);
      PROC0(dummy1, 0);
      PROC2(FreeRemember, 68, struct Remember **, rememberKey, a0, bool, reallyForget, d0);
      FUNC1(uint32, LockIBase, 69, uint32, dontknow, d0);
      PROC1(UnlockIBase, 70, uint32, ibLock, a0);
      FUNC4(int32, GetScreenData, 71, void *, buffer, a0, uint16, size, d0, uint16, type, d1, const struct Screen *, screen, a1);
      PROC4(RefreshGList, 72, struct Gadget *, gadgets, a0, struct Window *, window, a1, struct Requester *, requester, a2, int16, numGad, d0);
      FUNC5(uint16, AddGList, 73, struct Window *, window, a0, struct Gadget *, gadget, a1, uint16, position, d0, int16, numGad, d1, struct Requester *, requester, a2);
      FUNC3(uint16, RemoveGList, 74, struct Window *, remPtr, a0, struct Gadget *, gadget, a1, int16, numGad, d0);
      PROC1(ActivateWindow, 75, struct Window *, window, a0);
      PROC1(RefreshWindowFrame, 76, struct Window *, window, a0);
      FUNC3(bool, ActivateGadget, 77, struct Gadget *, gadgets, a0, struct Window *, window, a1, struct Requester *, requester, a2);
      PROC0(os4multiarg2, 0);
//    PROC9(os4multiarg2, 78, struct Gadget *, gadget, d0, struct Window *, window, d1, struct Requester *, requester, d2, uint16, flags, d3, uint16, horizPot, d4, uint16, vertPot, d5, uint16, horizBody, d6, uint16, vertBody, d7, int16, numGad, a0);
      FUNC3(int32, QueryOverscan, 79, uint32, displayID, a0, struct Rectangle *, rect, a1, int16, oScanType, d0);
      PROC2(MoveWindowInFrontOf, 80, struct Window *, window, a0, struct Window *, behindWindow, a1);
      PROC5(ChangeWindowBox, 81, struct Window *, window, a0, int16, left, d0, int16, top, d1, int16, width, d2, int16, height, d3);
      FUNC1(struct Hook *, SetEditHook, 82, struct Hook *, hook, a0);
      FUNC2(int32, SetMouseQueue, 83, struct Window *, window, a0, uint16, queueLength, d0);
      PROC1(ZipWindow, 84, struct Window *, window, a0);
      FUNC1(struct Screen *, LockPubScreen, 85, const char *, name, a0);
      PROC2(UnlockPubScreen, 86, const char *, name, a0, struct Screen *, screen, a1);
      FUNC0(struct List *, LockPubScreenList, 87);
      PROC0(UnlockPubScreenList, 88);
      FUNC2(const char *, NextPubScreen, 89, const struct Screen *, screen, a0, const char *, namebuf, a1);
      PROC1(SetDefaultPubScreen, 90, const char *, name, a0);
      FUNC1(uint16, SetPubScreenModes, 91, uint16, modes, d0);
      FUNC2(uint16, PubScreenStatus, 92, struct Screen *, screen, a0, uint16, statusFlags, d0);
      FUNC1(struct RastPort *, ObtainGIRPort, 93, struct GadgetInfo *, gInfo, a0);
      PROC1(ReleaseGIRPort, 94, struct RastPort *, rp, a0);
      PROC3(GadgetMouse, 95, struct Gadget *, gadget, a0, struct GadgetInfo *, gInfo, a1, int16 *, mousePoint, a2);
      PROC0(dummy3, 0);
      PROC1(GetDefaultPubScreen, 97, const char *, nameBuffer, a0);
      FUNC4(int32, EasyRequestArgs, 98, struct Window *, window, a0, const struct EasyStruct *, easyStruct, a1, uint32 *, idcmpPtr, a2, const void *, args, a3);
      PROC0(dummy4, 0);
      FUNC4(struct Window *, BuildEasyRequestArgs, 99, struct Window *, window, a0, const struct EasyStruct *, easyStruct, a1, uint32, idcmp, d0, const void *, args, a3);
      PROC0(dummy5, 0);
      FUNC3(int32, SysReqHandler, 100, struct Window *, window, a0, uint32 *, idcmpPtr, a1, bool, waitInput, d0);
      FUNC2(struct Window *, OpenWindowTagList, 101, const struct NewWindow *, newWindow, a0, const struct TagItem *, tagList, a1);
      PROC0(dummy6, 0);
      FUNC2(struct Screen *, OpenScreenTagList, 102, const struct NewScreen *, newScreen, a0, const struct TagItem *, tagList, a1);
      PROC0(dummy7, 0);
      PROC6(DrawImageState, 103, struct RastPort *, rp, a0, struct Image *, image, a1, int16, leftOffset, d0, int16, topOffset, d1, uint32, state, d2, const struct DrawInfo *, drawInfo, a2);
      FUNC2(bool, PointInImage, 104, uint32, point, d0, struct Image *, image, a0);
      PROC4(EraseImage, 105, struct RastPort *, rp, a0, struct Image *, image, a1, int16, leftOffset, d0, int16, topOffset, d1);
      FUNC3(void *, NewObjectA, 106, struct IClass *, classPtr, a0, const char *, classID, a1, const struct TagItem *, tagList, a2);
      PROC0(dummy8, 0);
      PROC1(DisposeObject, 107, void *, object, a0);
      FUNC2(IPTR, SetAttrsA, 108, void *, object, a0, const struct TagItem *, tagList, a1);
      PROC0(dummy9, 0);
      FUNC3(uint32, GetAttr, 109, uint32, attrID, d0, void *, object, a0, IPTR *, storagePtr, a1);
      FUNC4(IPTR, SetGadgetAttrsA, 110, struct Gadget *, gadget, a0, struct Window *, window, a1, struct Requester *, requester, a2, const struct TagItem *, tagList, a3);
      PROC0(dummy10, 0);
      FUNC1(void *, NextObject, 111, void *, objectPtrPtr, a0);
      PROC0(dummy11, 0);
      FUNC5(struct IClass *, MakeClass, 113, const char *, classID, a0, const char *, superClassID, a1, const struct IClass *, superClassPtr, a2, uint16, instanceSize, d0, uint32, flags, d1);
      PROC1(AddClass, 114, struct IClass *, classPtr, a0);
      FUNC1(struct DrawInfo *, GetScreenDrawInfo, 115, struct Screen *, screen, a0);
      PROC2(FreeScreenDrawInfo, 116, struct Screen *, screen, a0, struct DrawInfo *, drawInfo, a1);
      FUNC2(bool, ResetMenuStrip, 117, struct Window *, window, a0, struct Menu *, menu, a1);
      PROC1(RemoveClass, 118, struct IClass *, classPtr, a0);
      FUNC1(bool, FreeClass, 119, struct IClass *, classPtr, a0);
      PROC0(dummy12, 0);
      PROC0(dummy13, 0);
      PROC0(reserved1, 0);
      PROC0(reserved2, 0);
      PROC0(reserved3, 0);
      PROC0(reserved4, 0);
      PROC0(reserved5, 0);
      PROC0(reserved6, 0);
      FUNC3(struct ScreenBuffer *, AllocScreenBuffer, 128, struct Screen *, sc, a0, struct BitMap *, bm, a1, uint32, flags, d0);
      PROC2(FreeScreenBuffer, 129, struct Screen *, sc, a0, struct ScreenBuffer *, sb, a1);
      FUNC2(uint32, ChangeScreenBuffer, 130, struct Screen *, sc, a0, struct ScreenBuffer *, sb, a1);
      PROC3(ScreenDepth, 131, struct Screen *, screen, a0, uint32, flags, d0, void *, reserved, a1);
      PROC6(ScreenPosition, 132, struct Screen *, screen, a0, uint32, flags, d0, int32, x1, d1, int32, y1, d2, int32, x2, d3, int32, y2, d4);
      PROC7(ScrollWindowRaster, 133, struct Window *, win, a1, int16, dx, d0, int16, dy, d1, int16, xMin, d2, int16, yMin, d3, int16, xMax, d4, int16, yMax, d5);
      PROC2(LendMenus, 134, struct Window *, fromwindow, a0, struct Window *, towindow, a1);
      FUNC4(IPTR, DoGadgetMethodA, 135, struct Gadget *, gad, a0, struct Window *, win, a1, struct Requester *, req, a2, IPTR, message, a3);
      PROC0(dummy14, 0);
      PROC2(SetWindowPointerA, 136, struct Window *, win, a0, const struct TagItem *, taglist, a1);
      PROC0(dummy15, 0);
      FUNC4(bool, TimedDisplayAlert, 137, uint32, alertNumber, d0, const char *, string, a0, uint16, height, d1, uint32, time, a1);
      PROC2(HelpControl, 138, struct Window *, win, a0, uint32, flags, d0);
      FUNC2(bool, ShowWindow, 0, struct Window *, window, d0, struct Window *, other, d1);
      FUNC1(bool, HideWindow, 0, struct Window *, window, d0);
      FUNC2(IPTR, GetAttrsA, 0, void *, object, d0, const struct TagItem *, tagList, d1);
      PROC0(dummy16, 0);
      FUNC1(void *, LockGUIPrefs, 0, uint32, reserved, d0);
      PROC1(UnlockGUIPrefs, 0, void *, lock, d0);
      FUNC3(uint32, SetGUIAttrsA, 0, void *, reserved, d0, struct DrawInfo *, drawinfo, d1, const struct TagItem *, taglist, d2);
      PROC0(dummy17, 0);
      FUNC3(uint32, GetGUIAttrsA, 0, void *, reserved, d0, struct DrawInfo *, drawinfo, d1, const struct TagItem *, taglist, d2);
      PROC0(dummy18, 0);
      FUNC4(uint32, GetHalfPens, 0, struct DrawInfo *, drawinfo, d0, uint32, basepen, d1, uint16 *, halfshineptr, d2, uint16 *, halfshadowptr, d3);
      FUNC5(uint32, GadgetBox, 0, struct Gadget *, gadget, d0, void *, domain, d1, uint32, domaintype, d2, uint32, flags, d3, void *, box, d4);
      PROC4(RefreshSetGadgetAttrsA, 0, struct Gadget *, gadget, d0, struct Window *, window, d1, struct Requester *, req, d2, const struct TagItem *, taglist, d3);
      PROC0(dummy19, 0);
      FUNC3(IPTR, IDoSuperMethodA, 0, Class *, cl, d0, Object *, object, d1, IPTR, msg, d2);
      PROC0(dummy20, 0);
      FUNC3(IPTR, ISetSuperAttrsA, 0, Class *, cl, d0, Object *, object, d1, const struct TagItem *, taglist, d2);
      PROC0(dummy21, 0);
      FUNC3(IPTR, ICoerceMethodA, 0, Class *, cl, d0, Object *, object, d1, IPTR, msg, d2);
      PROC0(dummy22, 0);
      FUNC2(IPTR, IDoMethodA, 0, Object *, object, d0, IPTR, msg, d1);
      PROC0(dummy23, 0);
      FUNC3(struct ClassLibrary *, OpenClass, 0, const char *, name, d0, uint32, version, d1, Class **, cl_ptr, d2);
      PROC1(CloseClass, 0, struct ClassLibrary *, cl, d0);
      PROC0(dummy24, 0);
      FUNC2(bool, LockScreen, 0, struct Screen *, screen, d0, uint32, micros, d1);
      PROC1(UnlockScreen, 0, struct Screen *, screen, d0);
      FUNC2(int32, GetWindowAttrsA, 0, struct Window *, win, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy25, 0);
      FUNC2(int32, SetWindowAttrsA, 0, struct Window *, win, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy26, 0);
      FUNC4(IPTR, GetWindowAttr, 0, struct Window *, win, d0, uint32, attr, d1, void *, data, d2, uint32, size, d3);
      FUNC4(IPTR, SetWindowAttr, 0, struct Window *, win, d0, uint32, attr, d1, void *, data, d2, uint32, size, d3);
      PROC2(StripIntuiMessages, 0, struct MsgPort *, port, d0, struct Window *, win, d1);
      PROC0(Reserved7, 0);
      PROC0(Reserved8, 0);
      FUNC2(int32, GetScreenAttrsA, 0, struct Screen *, scr, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy27, 0);
      FUNC2(int32, SetScreenAttrsA, 0, struct Screen *, scr, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy28, 0);
      FUNC4(int32, GetScreenAttr, 0, struct Screen *, scr, d0, uint32, attr, d1, void *, data, d2, uint32, size, d3);
      FUNC4(int32, SetScreenAttr, 0, struct Screen *, scr, d0, uint32, attr, d1, void *, data, d2, uint32, size, d3);
      FUNC0(struct Screen *, LockScreenList, 0);
      PROC0(UnlockScreenList, 0);
      FUNC2(struct RastPort *, LockScreenGI, 0, struct GadgetInfo *, gi, d0, uint32, micros, d1);
      PROC2(UnlockScreenGI, 0, struct GadgetInfo *, gi, d0, struct RastPort *, rp, d1);
      FUNC1(uint32, GetMarkedBlock, 0, struct SGWork *, sgw, d0);
      PROC2(SetMarkedBlock, 0, struct SGWork *, sgw, d0, uint32, block, d1);
      FUNC2(void *, ObtainBitMapSourceA, 0, const char *, name, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy29, 0);
      PROC1(ReleaseBitMapSource, 0, void *, bitmapsource, d0);
      FUNC3(void *, ObtainBitMapInstanceA, 0, void *, bitmapsource, d0, struct Screen *, screen, d1, const struct TagItem *, taglist, d2);
      PROC0(dummy30, 0);
      PROC1(ReleaseBitMapInstance, 0, void *, bitmapinstance, d0);
      PROC0(os4multiargs2, 0);
      //PROC8(EmbossDisableRect, 0, struct RastPort *, rp, d0, int32, minx, d1, int32, miny, d2, int32, maxx, d3, int32, maxy, d4, uint32, backtype, d5, uint32, contrast, d6, struct DrawInfo *, dri, d7);
      PROC6(EmbossDisableText, 0, struct RastPort *, rp, d0, const char *, text, d1, uint32, len, d2, uint32, backtype, d3, uint32, contrast, d4, struct DrawInfo *, dri, d5);
      PROC7(PrintEmbossedDisabledIText, 0, struct RastPort *, rp, d0, struct IntuiText *, itext, d1, int32, left, d2, int32, top, d3, uint32, backtype, d4, uint32, contrast, d5, struct DrawInfo *, dri, d6);
      FUNC3(uint32, IntuiTextExtent, 0, struct RastPort *, rp, d0, struct IntuiText *, itext, d1, struct TextExtent *, textent, d2);
      PROC0(os4multiarg31, 0);
//    FUNC9(uint32, os4multiarg31, 0, struct RastPort *, rp, d0, int32, minx, d1, int32, miny, d2, int32, maxx, d3, int32, maxy, d4, uint32, shadelevel, d5, uint32, backtype, d6, uint32, state, d7, struct DrawInfo *, dri, a0);
      PROC0(dummy32, 0);
      PROC0(dummy33, 0);
      FUNC2(uint16, FindMenuKey, 0, struct Menu *, menu, d0, int32, code, d1);
      FUNC2(uint32, BitMapInstanceControlA, 0, void *, bitmapinstance, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy34, 0);
      FUNC3(struct List *, ObtainIPluginList, 0, uint32, type, d0, uint32, attrmask, d1, uint32, applymask, d2);
      PROC1(ReleaseIPluginList, 0, struct List *, list, d0);
      FUNC5(struct GUIPlugin *, OpenGUIPlugin, 0, const char *, name, d0, uint32, version, d1, uint32, type, d2, uint32, attrmask, d3, uint32, applymask, d4);
      PROC1(CloseGUIPlugin, 0, struct GUIPlugin *, plugin, d0);
      PROC0(os4multiarg35, 0);
//    FUNC10(uint32, os4multiarg35, 0, struct RastPort *, rp, d0, int32, left, d1, int32, top, d2, int32, width, d3, int32, height, d4, uint32, which, d5, uint32, backtype, d6, uint32, state, d7, struct DrawInfo *, dri, a0, const struct TagItem *, taglist, a1);
      PROC0(dummy36, 0);
      FUNC3(uint32, DoRender, 0, Object *, o, d0, struct GadgetInfo *, gi, d1, uint32, flags, d2);
      FUNC2(uint32, SetRenderDomain, 0, struct RastPort *, rp, d0, struct Rectangle *, domain, d1);
      FUNC2(uint32, GetRenderDomain, 0, struct RastPort *, rp, d0, struct Rectangle *, domain, d1);
      PROC0(os4multiarg37, 0);
//    FUNC9(uint32, os4multiarg37, 0, struct RastPort *, rp, d0, int32, left, d1, int32, top, d2, int32, width, d3, int32, height, d4, struct IBox *, domain, d5, uint32, reserved, d6, struct GradientSpec *, gradientspec, d7, struct DrawInfo *, dri, a0);
      FUNC1(uint32, DirectionVector, 0, uint32, degrees, d0);
      PROC0(os4multiarg38, 0);
//    FUNC10(uint32, os4multiarg38, 0, struct RastPort *, rp, d0, int32, minx, d1, int32, miny, d2, int32, maxx, d3, int32, maxy, d4, uint32, shadelevel, d5, uint32, backtype, d6, uint32, state, d7, struct DrawInfo *, dri, a0, const struct TagItem *, taglist, a1);
      PROC0(dummy39, 0);
      PROC2(DoScrollHook, 0, struct ScrollHook *, scrollhook, d0, int32, scrollmode, d1);
      FUNC4(struct Hook *, ObtainIBackFill, 0, struct DrawInfo *, dri, d0, uint32, element, d1, uint32, state, d2, uint32, flags, d3);
      PROC1(ReleaseIBackFill, 0, struct Hook *, hook, d0);
      FUNC2(uint32, IntuitionControlA, 0, void *, object, d0, const struct TagItem *, taglist, d1);
      PROC0(dummy40, 0);
      FUNC1(void *, StartScreenNotifyTagList, 0, const struct TagItem *, taglist, d0);
      PROC0(dummy41, 0);
      FUNC1(bool, EndScreenNotify, 0, void *, request, d0);
   ENDDECL

#endif
