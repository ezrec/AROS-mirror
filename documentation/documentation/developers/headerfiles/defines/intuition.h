#ifndef DEFINES_INTUITION_H
#define DEFINES_INTUITION_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/intuition/intuition.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for intuition
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __AddGadget_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(UWORD, AddGadget, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Gadget *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
        struct IntuitionBase *, (__IntuitionBase), 7, Intuition)

#define AddGadget(arg1, arg2, arg3) \
    __AddGadget_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __ClearDMRequest_WB(__IntuitionBase, __arg1) \
        AROS_LC1(BOOL, ClearDMRequest, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 8, Intuition)

#define ClearDMRequest(arg1) \
    __ClearDMRequest_WB(IntuitionBase, (arg1))

#define __ClearMenuStrip_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ClearMenuStrip, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 9, Intuition)

#define ClearMenuStrip(arg1) \
    __ClearMenuStrip_WB(IntuitionBase, (arg1))

#define __ClearPointer_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ClearPointer, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 10, Intuition)

#define ClearPointer(arg1) \
    __ClearPointer_WB(IntuitionBase, (arg1))

#define __CloseScreen_WB(__IntuitionBase, __arg1) \
        AROS_LC1(BOOL, CloseScreen, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 11, Intuition)

#define CloseScreen(arg1) \
    __CloseScreen_WB(IntuitionBase, (arg1))

#define __CloseWindow_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, CloseWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 12, Intuition)

#define CloseWindow(arg1) \
    __CloseWindow_WB(IntuitionBase, (arg1))

#define __CloseWorkBench_WB(__IntuitionBase) \
        AROS_LC0(LONG, CloseWorkBench, \
        struct IntuitionBase *, (__IntuitionBase), 13, Intuition)

#define CloseWorkBench() \
    __CloseWorkBench_WB(IntuitionBase)

#define __CurrentTime_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, CurrentTime, \
                  AROS_LCA(ULONG *,(__arg1),A0), \
                  AROS_LCA(ULONG *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 14, Intuition)

#define CurrentTime(arg1, arg2) \
    __CurrentTime_WB(IntuitionBase, (arg1), (arg2))

#define __DisplayAlert_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, DisplayAlert, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(UBYTE *,(__arg2),A0), \
                  AROS_LCA(UWORD,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 15, Intuition)

#define DisplayAlert(arg1, arg2, arg3) \
    __DisplayAlert_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __DisplayBeep_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, DisplayBeep, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 16, Intuition)

#define DisplayBeep(arg1) \
    __DisplayBeep_WB(IntuitionBase, (arg1))

#define __DoubleClick_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(BOOL, DoubleClick, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(ULONG,(__arg3),D2), \
                  AROS_LCA(ULONG,(__arg4),D3), \
        struct IntuitionBase *, (__IntuitionBase), 17, Intuition)

#define DoubleClick(arg1, arg2, arg3, arg4) \
    __DoubleClick_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __DrawBorder_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, DrawBorder, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct Border *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
        struct IntuitionBase *, (__IntuitionBase), 18, Intuition)

#define DrawBorder(arg1, arg2, arg3, arg4) \
    __DrawBorder_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __DrawImage_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, DrawImage, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct Image *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
        struct IntuitionBase *, (__IntuitionBase), 19, Intuition)

#define DrawImage(arg1, arg2, arg3, arg4) \
    __DrawImage_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __EndRequest_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, EndRequest, \
                  AROS_LCA(struct Requester *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 20, Intuition)

#define EndRequest(arg1, arg2) \
    __EndRequest_WB(IntuitionBase, (arg1), (arg2))

#define __GetDefPrefs_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(struct Preferences *, GetDefPrefs, \
                  AROS_LCA(struct Preferences *,(__arg1),A0), \
                  AROS_LCA(WORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 21, Intuition)

#define GetDefPrefs(arg1, arg2) \
    __GetDefPrefs_WB(IntuitionBase, (arg1), (arg2))

#define __GetPrefs_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(struct Preferences *, GetPrefs, \
                  AROS_LCA(struct Preferences *,(__arg1),A0), \
                  AROS_LCA(WORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 22, Intuition)

#define GetPrefs(arg1, arg2) \
    __GetPrefs_WB(IntuitionBase, (arg1), (arg2))

#define __InitRequester_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, InitRequester, \
                  AROS_LCA(struct Requester *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 23, Intuition)

#define InitRequester(arg1) \
    __InitRequester_WB(IntuitionBase, (arg1))

#define __ItemAddress_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(struct MenuItem *, ItemAddress, \
                  AROS_LCA(struct Menu *,(__arg1),A0), \
                  AROS_LCA(UWORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 24, Intuition)

#define ItemAddress(arg1, arg2) \
    __ItemAddress_WB(IntuitionBase, (arg1), (arg2))

#define __ModifyIDCMP_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(BOOL, ModifyIDCMP, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 25, Intuition)

#define ModifyIDCMP(arg1, arg2) \
    __ModifyIDCMP_WB(IntuitionBase, (arg1), (arg2))

#define __ModifyProp_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, ModifyProp, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(ULONG,(__arg4),D0), \
                  AROS_LCA(ULONG,(__arg5),D1), \
                  AROS_LCA(ULONG,(__arg6),D2), \
                  AROS_LCA(ULONG,(__arg7),D3), \
                  AROS_LCA(ULONG,(__arg8),D4), \
        struct IntuitionBase *, (__IntuitionBase), 26, Intuition)

#define ModifyProp(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __ModifyProp_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __MoveScreen_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, MoveScreen, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 27, Intuition)

#define MoveScreen(arg1, arg2, arg3) \
    __MoveScreen_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __MoveWindow_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, MoveWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 28, Intuition)

#define MoveWindow(arg1, arg2, arg3) \
    __MoveWindow_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __OffGadget_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, OffGadget, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 29, Intuition)

#define OffGadget(arg1, arg2, arg3) \
    __OffGadget_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __OffMenu_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, OffMenu, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(UWORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 30, Intuition)

#define OffMenu(arg1, arg2) \
    __OffMenu_WB(IntuitionBase, (arg1), (arg2))

#define __OnGadget_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, OnGadget, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 31, Intuition)

#define OnGadget(arg1, arg2, arg3) \
    __OnGadget_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __OnMenu_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, OnMenu, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(UWORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 32, Intuition)

#define OnMenu(arg1, arg2) \
    __OnMenu_WB(IntuitionBase, (arg1), (arg2))

#define __OpenScreen_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct Screen *, OpenScreen, \
                  AROS_LCA(struct NewScreen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 33, Intuition)

#define OpenScreen(arg1) \
    __OpenScreen_WB(IntuitionBase, (arg1))

#define __OpenWindow_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct Window *, OpenWindow, \
                  AROS_LCA(struct NewWindow *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 34, Intuition)

#define OpenWindow(arg1) \
    __OpenWindow_WB(IntuitionBase, (arg1))

#define __OpenWorkBench_WB(__IntuitionBase) \
        AROS_LC0(IPTR, OpenWorkBench, \
        struct IntuitionBase *, (__IntuitionBase), 35, Intuition)

#define OpenWorkBench() \
    __OpenWorkBench_WB(IntuitionBase)

#define __PrintIText_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, PrintIText, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct IntuiText *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
        struct IntuitionBase *, (__IntuitionBase), 36, Intuition)

#define PrintIText(arg1, arg2, arg3, arg4) \
    __PrintIText_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __RefreshGadgets_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, RefreshGadgets, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 37, Intuition)

#define RefreshGadgets(arg1, arg2, arg3) \
    __RefreshGadgets_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __RemoveGadget_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(UWORD, RemoveGadget, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Gadget *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 38, Intuition)

#define RemoveGadget(arg1, arg2) \
    __RemoveGadget_WB(IntuitionBase, (arg1), (arg2))

#define __ReportMouse_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, ReportMouse, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(struct Window *,(__arg2),A0), \
        struct IntuitionBase *, (__IntuitionBase), 39, Intuition)

#define ReportMouse(arg1, arg2) \
    __ReportMouse_WB(IntuitionBase, (arg1), (arg2))

#define __Request_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(BOOL, Request, \
                  AROS_LCA(struct Requester *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 40, Intuition)

#define Request(arg1, arg2) \
    __Request_WB(IntuitionBase, (arg1), (arg2))

#define __ScreenToBack_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ScreenToBack, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 41, Intuition)

#define ScreenToBack(arg1) \
    __ScreenToBack_WB(IntuitionBase, (arg1))

#define __ScreenToFront_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ScreenToFront, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 42, Intuition)

#define ScreenToFront(arg1) \
    __ScreenToFront_WB(IntuitionBase, (arg1))

#define __SetDMRequest_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(BOOL, SetDMRequest, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Requester *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 43, Intuition)

#define SetDMRequest(arg1, arg2) \
    __SetDMRequest_WB(IntuitionBase, (arg1), (arg2))

#define __SetMenuStrip_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(BOOL, SetMenuStrip, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Menu *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 44, Intuition)

#define SetMenuStrip(arg1, arg2) \
    __SetMenuStrip_WB(IntuitionBase, (arg1), (arg2))

#define __SetPointer_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, SetPointer, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(UWORD *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
                  AROS_LCA(LONG,(__arg5),D2), \
                  AROS_LCA(LONG,(__arg6),D3), \
        struct IntuitionBase *, (__IntuitionBase), 45, Intuition)

#define SetPointer(arg1, arg2, arg3, arg4, arg5, arg6) \
    __SetPointer_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __SetWindowTitles_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, SetWindowTitles, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(CONST_STRPTR,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 46, Intuition)

#define SetWindowTitles(arg1, arg2, arg3) \
    __SetWindowTitles_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __ShowTitle_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, ShowTitle, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(BOOL,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 47, Intuition)

#define ShowTitle(arg1, arg2) \
    __ShowTitle_WB(IntuitionBase, (arg1), (arg2))

#define __SizeWindow_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, SizeWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 48, Intuition)

#define SizeWindow(arg1, arg2, arg3) \
    __SizeWindow_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __ViewAddress_WB(__IntuitionBase) \
        AROS_LC0(struct View *, ViewAddress, \
        struct IntuitionBase *, (__IntuitionBase), 49, Intuition)

#define ViewAddress() \
    __ViewAddress_WB(IntuitionBase)

#define __ViewPortAddress_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct ViewPort *, ViewPortAddress, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 50, Intuition)

#define ViewPortAddress(arg1) \
    __ViewPortAddress_WB(IntuitionBase, (arg1))

#define __WindowToBack_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, WindowToBack, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 51, Intuition)

#define WindowToBack(arg1) \
    __WindowToBack_WB(IntuitionBase, (arg1))

#define __WindowToFront_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, WindowToFront, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 52, Intuition)

#define WindowToFront(arg1) \
    __WindowToFront_WB(IntuitionBase, (arg1))

#define __WindowLimits_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(BOOL, WindowLimits, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(WORD,(__arg2),D0), \
                  AROS_LCA(WORD,(__arg3),D1), \
                  AROS_LCA(UWORD,(__arg4),D2), \
                  AROS_LCA(UWORD,(__arg5),D3), \
        struct IntuitionBase *, (__IntuitionBase), 53, Intuition)

#define WindowLimits(arg1, arg2, arg3, arg4, arg5) \
    __WindowLimits_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __SetPrefs_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct Preferences *, SetPrefs, \
                  AROS_LCA(struct Preferences *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(BOOL,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 54, Intuition)

#define SetPrefs(arg1, arg2, arg3) \
    __SetPrefs_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __IntuiTextLength_WB(__IntuitionBase, __arg1) \
        AROS_LC1(LONG, IntuiTextLength, \
                  AROS_LCA(struct IntuiText *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 55, Intuition)

#define IntuiTextLength(arg1) \
    __IntuiTextLength_WB(IntuitionBase, (arg1))

#define __WBenchToBack_WB(__IntuitionBase) \
        AROS_LC0(BOOL, WBenchToBack, \
        struct IntuitionBase *, (__IntuitionBase), 56, Intuition)

#define WBenchToBack() \
    __WBenchToBack_WB(IntuitionBase)

#define __WBenchToFront_WB(__IntuitionBase) \
        AROS_LC0(BOOL, WBenchToFront, \
        struct IntuitionBase *, (__IntuitionBase), 57, Intuition)

#define WBenchToFront() \
    __WBenchToFront_WB(IntuitionBase)

#define __AutoRequest_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8(BOOL, AutoRequest, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct IntuiText *,(__arg2),A1), \
                  AROS_LCA(struct IntuiText *,(__arg3),A2), \
                  AROS_LCA(struct IntuiText *,(__arg4),A3), \
                  AROS_LCA(ULONG,(__arg5),D0), \
                  AROS_LCA(ULONG,(__arg6),D1), \
                  AROS_LCA(ULONG,(__arg7),D2), \
                  AROS_LCA(ULONG,(__arg8),D3), \
        struct IntuitionBase *, (__IntuitionBase), 58, Intuition)

#define AutoRequest(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __AutoRequest_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __BeginRefresh_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, BeginRefresh, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 59, Intuition)

#define BeginRefresh(arg1) \
    __BeginRefresh_WB(IntuitionBase, (arg1))

#define __BuildSysRequest_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7(struct Window *, BuildSysRequest, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct IntuiText *,(__arg2),A1), \
                  AROS_LCA(struct IntuiText *,(__arg3),A2), \
                  AROS_LCA(struct IntuiText *,(__arg4),A3), \
                  AROS_LCA(ULONG,(__arg5),D0), \
                  AROS_LCA(WORD,(__arg6),D2), \
                  AROS_LCA(WORD,(__arg7),D3), \
        struct IntuitionBase *, (__IntuitionBase), 60, Intuition)

#define BuildSysRequest(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __BuildSysRequest_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __EndRefresh_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, EndRefresh, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(BOOL,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 61, Intuition)

#define EndRefresh(arg1, arg2) \
    __EndRefresh_WB(IntuitionBase, (arg1), (arg2))

#define __FreeSysRequest_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, FreeSysRequest, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 62, Intuition)

#define FreeSysRequest(arg1) \
    __FreeSysRequest_WB(IntuitionBase, (arg1))

#define __MakeScreen_WB(__IntuitionBase, __arg1) \
        AROS_LC1(LONG, MakeScreen, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 63, Intuition)

#define MakeScreen(arg1) \
    __MakeScreen_WB(IntuitionBase, (arg1))

#define __RemakeDisplay_WB(__IntuitionBase) \
        AROS_LC0(LONG, RemakeDisplay, \
        struct IntuitionBase *, (__IntuitionBase), 64, Intuition)

#define RemakeDisplay() \
    __RemakeDisplay_WB(IntuitionBase)

#define __RethinkDisplay_WB(__IntuitionBase) \
        AROS_LC0(LONG, RethinkDisplay, \
        struct IntuitionBase *, (__IntuitionBase), 65, Intuition)

#define RethinkDisplay() \
    __RethinkDisplay_WB(IntuitionBase)

#define __AllocRemember_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(APTR, AllocRemember, \
                  AROS_LCA(struct Remember **,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(ULONG,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 66, Intuition)

#define AllocRemember(arg1, arg2, arg3) \
    __AllocRemember_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __AlohaWorkbench_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, AlohaWorkbench, \
                  AROS_LCA(struct MsgPort *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 67, Intuition)

#define AlohaWorkbench(arg1) \
    __AlohaWorkbench_WB(IntuitionBase, (arg1))

#define __FreeRemember_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, FreeRemember, \
                  AROS_LCA(struct Remember **,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 68, Intuition)

#define FreeRemember(arg1, arg2) \
    __FreeRemember_WB(IntuitionBase, (arg1), (arg2))

#define __LockIBase_WB(__IntuitionBase, __arg1) \
        AROS_LC1(ULONG, LockIBase, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct IntuitionBase *, (__IntuitionBase), 69, Intuition)

#define LockIBase(arg1) \
    __LockIBase_WB(IntuitionBase, (arg1))

#define __UnlockIBase_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, UnlockIBase, \
                  AROS_LCA(ULONG,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 70, Intuition)

#define UnlockIBase(arg1) \
    __UnlockIBase_WB(IntuitionBase, (arg1))

#define __GetScreenData_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, GetScreenData, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(ULONG,(__arg3),D1), \
                  AROS_LCA(struct Screen *,(__arg4),A1), \
        struct IntuitionBase *, (__IntuitionBase), 71, Intuition)

#define GetScreenData(arg1, arg2, arg3, arg4) \
    __GetScreenData_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __RefreshGList_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, RefreshGList, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(LONG,(__arg4),D0), \
        struct IntuitionBase *, (__IntuitionBase), 72, Intuition)

#define RefreshGList(arg1, arg2, arg3, arg4) \
    __RefreshGList_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __AddGList_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(UWORD, AddGList, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Gadget *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
                  AROS_LCA(struct Requester *,(__arg5),A2), \
        struct IntuitionBase *, (__IntuitionBase), 73, Intuition)

#define AddGList(arg1, arg2, arg3, arg4, arg5) \
    __AddGList_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __RemoveGList_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(UWORD, RemoveGList, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Gadget *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct IntuitionBase *, (__IntuitionBase), 74, Intuition)

#define RemoveGList(arg1, arg2, arg3) \
    __RemoveGList_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __ActivateWindow_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ActivateWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 75, Intuition)

#define ActivateWindow(arg1) \
    __ActivateWindow_WB(IntuitionBase, (arg1))

#define __RefreshWindowFrame_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, RefreshWindowFrame, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 76, Intuition)

#define RefreshWindowFrame(arg1) \
    __RefreshWindowFrame_WB(IntuitionBase, (arg1))

#define __ActivateGadget_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, ActivateGadget, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 77, Intuition)

#define ActivateGadget(arg1, arg2, arg3) \
    __ActivateGadget_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __NewModifyProp_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, NewModifyProp, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(ULONG,(__arg4),D0), \
                  AROS_LCA(ULONG,(__arg5),D1), \
                  AROS_LCA(ULONG,(__arg6),D2), \
                  AROS_LCA(ULONG,(__arg7),D3), \
                  AROS_LCA(ULONG,(__arg8),D4), \
                  AROS_LCA(LONG,(__arg9),D5), \
        struct IntuitionBase *, (__IntuitionBase), 78, Intuition)

#define NewModifyProp(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __NewModifyProp_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __QueryOverscan_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, QueryOverscan, \
                  AROS_LCA(ULONG,(__arg1),A0), \
                  AROS_LCA(struct Rectangle *,(__arg2),A1), \
                  AROS_LCA(WORD,(__arg3),D0), \
        struct IntuitionBase *, (__IntuitionBase), 79, Intuition)

#define QueryOverscan(arg1, arg2, arg3) \
    __QueryOverscan_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __MoveWindowInFrontOf_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, MoveWindowInFrontOf, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 80, Intuition)

#define MoveWindowInFrontOf(arg1, arg2) \
    __MoveWindowInFrontOf_WB(IntuitionBase, (arg1), (arg2))

#define __ChangeWindowBox_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, ChangeWindowBox, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(LONG,(__arg4),D2), \
                  AROS_LCA(LONG,(__arg5),D3), \
        struct IntuitionBase *, (__IntuitionBase), 81, Intuition)

#define ChangeWindowBox(arg1, arg2, arg3, arg4, arg5) \
    __ChangeWindowBox_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __SetEditHook_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct Hook *, SetEditHook, \
                  AROS_LCA(struct Hook *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 82, Intuition)

#define SetEditHook(arg1) \
    __SetEditHook_WB(IntuitionBase, (arg1))

#define __SetMouseQueue_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(LONG, SetMouseQueue, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(UWORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 83, Intuition)

#define SetMouseQueue(arg1, arg2) \
    __SetMouseQueue_WB(IntuitionBase, (arg1), (arg2))

#define __ZipWindow_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ZipWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 84, Intuition)

#define ZipWindow(arg1) \
    __ZipWindow_WB(IntuitionBase, (arg1))

#define __LockPubScreen_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct Screen *, LockPubScreen, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 85, Intuition)

#define LockPubScreen(arg1) \
    __LockPubScreen_WB(IntuitionBase, (arg1))

#define __UnlockPubScreen_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, UnlockPubScreen, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
                  AROS_LCA(struct Screen *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 86, Intuition)

#define UnlockPubScreen(arg1, arg2) \
    __UnlockPubScreen_WB(IntuitionBase, (arg1), (arg2))

#define __LockPubScreenList_WB(__IntuitionBase) \
        AROS_LC0(struct List *, LockPubScreenList, \
        struct IntuitionBase *, (__IntuitionBase), 87, Intuition)

#define LockPubScreenList() \
    __LockPubScreenList_WB(IntuitionBase)

#define __UnlockPubScreenList_WB(__IntuitionBase) \
        AROS_LC0NR(void, UnlockPubScreenList, \
        struct IntuitionBase *, (__IntuitionBase), 88, Intuition)

#define UnlockPubScreenList() \
    __UnlockPubScreenList_WB(IntuitionBase)

#define __NextPubScreen_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(UBYTE *, NextPubScreen, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(UBYTE *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 89, Intuition)

#define NextPubScreen(arg1, arg2) \
    __NextPubScreen_WB(IntuitionBase, (arg1), (arg2))

#define __SetDefaultPubScreen_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, SetDefaultPubScreen, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 90, Intuition)

#define SetDefaultPubScreen(arg1) \
    __SetDefaultPubScreen_WB(IntuitionBase, (arg1))

#define __SetPubScreenModes_WB(__IntuitionBase, __arg1) \
        AROS_LC1(UWORD, SetPubScreenModes, \
                  AROS_LCA(UWORD,(__arg1),D0), \
        struct IntuitionBase *, (__IntuitionBase), 91, Intuition)

#define SetPubScreenModes(arg1) \
    __SetPubScreenModes_WB(IntuitionBase, (arg1))

#define __PubScreenStatus_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(UWORD, PubScreenStatus, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(UWORD,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 92, Intuition)

#define PubScreenStatus(arg1, arg2) \
    __PubScreenStatus_WB(IntuitionBase, (arg1), (arg2))

#define __ObtainGIRPort_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct RastPort *, ObtainGIRPort, \
                  AROS_LCA(struct GadgetInfo *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 93, Intuition)

#define ObtainGIRPort(arg1) \
    __ObtainGIRPort_WB(IntuitionBase, (arg1))

#define __ReleaseGIRPort_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ReleaseGIRPort, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 94, Intuition)

#define ReleaseGIRPort(arg1) \
    __ReleaseGIRPort_WB(IntuitionBase, (arg1))

#define __GadgetMouse_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, GadgetMouse, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct GadgetInfo *,(__arg2),A1), \
                  AROS_LCA(WORD *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 95, Intuition)

#define GadgetMouse(arg1, arg2, arg3) \
    __GadgetMouse_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __SetIPrefs_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(ULONG, SetIPrefs, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(ULONG,(__arg3),D1), \
        struct IntuitionBase *, (__IntuitionBase), 96, Intuition)

#define SetIPrefs(arg1, arg2, arg3) \
    __SetIPrefs_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __GetDefaultPubScreen_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct Screen *, GetDefaultPubScreen, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 97, Intuition)

#define GetDefaultPubScreen(arg1) \
    __GetDefaultPubScreen_WB(IntuitionBase, (arg1))

#define __EasyRequestArgs_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, EasyRequestArgs, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct EasyStruct *,(__arg2),A1), \
                  AROS_LCA(ULONG *,(__arg3),A2), \
                  AROS_LCA(APTR,(__arg4),A3), \
        struct IntuitionBase *, (__IntuitionBase), 98, Intuition)

#define EasyRequestArgs(arg1, arg2, arg3, arg4) \
    __EasyRequestArgs_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define EasyRequest(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    EasyRequestArgs((arg1), (arg2), (arg3), (APTR)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __BuildEasyRequestArgs_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(struct Window *, BuildEasyRequestArgs, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct EasyStruct *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
                  AROS_LCA(APTR,(__arg4),A3), \
        struct IntuitionBase *, (__IntuitionBase), 99, Intuition)

#define BuildEasyRequestArgs(arg1, arg2, arg3, arg4) \
    __BuildEasyRequestArgs_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define BuildEasyRequest(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    BuildEasyRequestArgs((arg1), (arg2), (arg3), (APTR)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SysReqHandler_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, SysReqHandler, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(ULONG *,(__arg2),A1), \
                  AROS_LCA(BOOL,(__arg3),D0), \
        struct IntuitionBase *, (__IntuitionBase), 100, Intuition)

#define SysReqHandler(arg1, arg2, arg3) \
    __SysReqHandler_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __OpenWindowTagList_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(struct Window *, OpenWindowTagList, \
                  AROS_LCA(struct NewWindow *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 101, Intuition)

#define OpenWindowTagList(arg1, arg2) \
    __OpenWindowTagList_WB(IntuitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define OpenWindowTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OpenWindowTagList((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __OpenScreenTagList_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(struct Screen *, OpenScreenTagList, \
                  AROS_LCA(struct NewScreen *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 102, Intuition)

#define OpenScreenTagList(arg1, arg2) \
    __OpenScreenTagList_WB(IntuitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define OpenScreenTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OpenScreenTagList((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __DrawImageState_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, DrawImageState, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct Image *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
                  AROS_LCA(ULONG,(__arg5),D2), \
                  AROS_LCA(struct DrawInfo *,(__arg6),A2), \
        struct IntuitionBase *, (__IntuitionBase), 103, Intuition)

#define DrawImageState(arg1, arg2, arg3, arg4, arg5, arg6) \
    __DrawImageState_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __PointInImage_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(BOOL, PointInImage, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(struct Image *,(__arg2),A0), \
        struct IntuitionBase *, (__IntuitionBase), 104, Intuition)

#define PointInImage(arg1, arg2) \
    __PointInImage_WB(IntuitionBase, (arg1), (arg2))

#define __EraseImage_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, EraseImage, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct Image *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
        struct IntuitionBase *, (__IntuitionBase), 105, Intuition)

#define EraseImage(arg1, arg2, arg3, arg4) \
    __EraseImage_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __NewObjectA_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(APTR, NewObjectA, \
                  AROS_LCA(struct IClass *,(__arg1),A0), \
                  AROS_LCA(UBYTE *,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 106, Intuition)

#define NewObjectA(arg1, arg2, arg3) \
    __NewObjectA_WB(IntuitionBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define NewObject(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    NewObjectA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __DisposeObject_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, DisposeObject, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 107, Intuition)

#define DisposeObject(arg1) \
    __DisposeObject_WB(IntuitionBase, (arg1))

#define __SetAttrsA_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(IPTR, SetAttrsA, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 108, Intuition)

#define SetAttrsA(arg1, arg2) \
    __SetAttrsA_WB(IntuitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define SetAttrs(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetAttrsA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetAttr_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(ULONG, GetAttr, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(Object *,(__arg2),A0), \
                  AROS_LCA(IPTR *,(__arg3),A1), \
        struct IntuitionBase *, (__IntuitionBase), 109, Intuition)

#define GetAttr(arg1, arg2, arg3) \
    __GetAttr_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __SetGadgetAttrsA_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(IPTR, SetGadgetAttrsA, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(struct TagItem *,(__arg4),A3), \
        struct IntuitionBase *, (__IntuitionBase), 110, Intuition)

#define SetGadgetAttrsA(arg1, arg2, arg3, arg4) \
    __SetGadgetAttrsA_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define SetGadgetAttrs(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetGadgetAttrsA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __NextObject_WB(__IntuitionBase, __arg1) \
        AROS_LC1(APTR, NextObject, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 111, Intuition)

#define NextObject(arg1) \
    __NextObject_WB(IntuitionBase, (arg1))

#define __FindClass_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct IClass *, FindClass, \
                  AROS_LCA(ClassID,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 112, Intuition)

#define FindClass(arg1) \
    __FindClass_WB(IntuitionBase, (arg1))

#define __MakeClass_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(struct IClass *, MakeClass, \
                  AROS_LCA(ClassID,(__arg1),A0), \
                  AROS_LCA(ClassID,(__arg2),A1), \
                  AROS_LCA(struct IClass *,(__arg3),A2), \
                  AROS_LCA(ULONG,(__arg4),D0), \
                  AROS_LCA(ULONG,(__arg5),D1), \
        struct IntuitionBase *, (__IntuitionBase), 113, Intuition)

#define MakeClass(arg1, arg2, arg3, arg4, arg5) \
    __MakeClass_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __AddClass_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, AddClass, \
                  AROS_LCA(struct IClass *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 114, Intuition)

#define AddClass(arg1) \
    __AddClass_WB(IntuitionBase, (arg1))

#define __GetScreenDrawInfo_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct DrawInfo *, GetScreenDrawInfo, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 115, Intuition)

#define GetScreenDrawInfo(arg1) \
    __GetScreenDrawInfo_WB(IntuitionBase, (arg1))

#define __FreeScreenDrawInfo_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, FreeScreenDrawInfo, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(struct DrawInfo *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 116, Intuition)

#define FreeScreenDrawInfo(arg1, arg2) \
    __FreeScreenDrawInfo_WB(IntuitionBase, (arg1), (arg2))

#define __ResetMenuStrip_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(BOOL, ResetMenuStrip, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Menu *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 117, Intuition)

#define ResetMenuStrip(arg1, arg2) \
    __ResetMenuStrip_WB(IntuitionBase, (arg1), (arg2))

#define __RemoveClass_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, RemoveClass, \
                  AROS_LCA(struct IClass *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 118, Intuition)

#define RemoveClass(arg1) \
    __RemoveClass_WB(IntuitionBase, (arg1))

#define __FreeClass_WB(__IntuitionBase, __arg1) \
        AROS_LC1(BOOL, FreeClass, \
                  AROS_LCA(struct IClass *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 119, Intuition)

#define FreeClass(arg1) \
    __FreeClass_WB(IntuitionBase, (arg1))

#define __AllocScreenBuffer_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct ScreenBuffer *, AllocScreenBuffer, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(struct BitMap *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
        struct IntuitionBase *, (__IntuitionBase), 128, Intuition)

#define AllocScreenBuffer(arg1, arg2, arg3) \
    __AllocScreenBuffer_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __FreeScreenBuffer_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, FreeScreenBuffer, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(struct ScreenBuffer *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 129, Intuition)

#define FreeScreenBuffer(arg1, arg2) \
    __FreeScreenBuffer_WB(IntuitionBase, (arg1), (arg2))

#define __ChangeScreenBuffer_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2(ULONG, ChangeScreenBuffer, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(struct ScreenBuffer *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 130, Intuition)

#define ChangeScreenBuffer(arg1, arg2) \
    __ChangeScreenBuffer_WB(IntuitionBase, (arg1), (arg2))

#define __ScreenDepth_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, ScreenDepth, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(APTR,(__arg3),A1), \
        struct IntuitionBase *, (__IntuitionBase), 131, Intuition)

#define ScreenDepth(arg1, arg2, arg3) \
    __ScreenDepth_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __ScreenPosition_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, ScreenPosition, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(LONG,(__arg4),D2), \
                  AROS_LCA(LONG,(__arg5),D3), \
                  AROS_LCA(LONG,(__arg6),D4), \
        struct IntuitionBase *, (__IntuitionBase), 132, Intuition)

#define ScreenPosition(arg1, arg2, arg3, arg4, arg5, arg6) \
    __ScreenPosition_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __ScrollWindowRaster_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, ScrollWindowRaster, \
                  AROS_LCA(struct Window *,(__arg1),A1), \
                  AROS_LCA(WORD,(__arg2),D0), \
                  AROS_LCA(WORD,(__arg3),D1), \
                  AROS_LCA(WORD,(__arg4),D2), \
                  AROS_LCA(WORD,(__arg5),D3), \
                  AROS_LCA(WORD,(__arg6),D4), \
                  AROS_LCA(WORD,(__arg7),D5), \
        struct IntuitionBase *, (__IntuitionBase), 133, Intuition)

#define ScrollWindowRaster(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __ScrollWindowRaster_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __LendMenus_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, LendMenus, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 134, Intuition)

#define LendMenus(arg1, arg2) \
    __LendMenus_WB(IntuitionBase, (arg1), (arg2))

#define __DoGadgetMethodA_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(IPTR, DoGadgetMethodA, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(Msg,(__arg4),A3), \
        struct IntuitionBase *, (__IntuitionBase), 135, Intuition)

#define DoGadgetMethodA(arg1, arg2, arg3, arg4) \
    __DoGadgetMethodA_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define DoGadgetMethod(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    DoGadgetMethodA((arg1), (arg2), (arg3), (Msg)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SetWindowPointerA_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, SetWindowPointerA, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 136, Intuition)

#define SetWindowPointerA(arg1, arg2) \
    __SetWindowPointerA_WB(IntuitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define SetWindowPointer(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetWindowPointerA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __TimedDisplayAlert_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(BOOL, TimedDisplayAlert, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(UBYTE *,(__arg2),A0), \
                  AROS_LCA(UWORD,(__arg3),D1), \
                  AROS_LCA(ULONG,(__arg4),A1), \
        struct IntuitionBase *, (__IntuitionBase), 137, Intuition)

#define TimedDisplayAlert(arg1, arg2, arg3, arg4) \
    __TimedDisplayAlert_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __HelpControl_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, HelpControl, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct IntuitionBase *, (__IntuitionBase), 138, Intuition)

#define HelpControl(arg1, arg2) \
    __HelpControl_WB(IntuitionBase, (arg1), (arg2))

#define __IsWindowVisible_WB(__IntuitionBase, __arg1) \
        AROS_LC1(LONG, IsWindowVisible, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 139, Intuition)

#define IsWindowVisible(arg1) \
    __IsWindowVisible_WB(IntuitionBase, (arg1))

#define __ShowWindow_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, ShowWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 140, Intuition)

#define ShowWindow(arg1) \
    __ShowWindow_WB(IntuitionBase, (arg1))

#define __HideWindow_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, HideWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 141, Intuition)

#define HideWindow(arg1) \
    __HideWindow_WB(IntuitionBase, (arg1))

#define __ChangeWindowShape_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct Region *, ChangeWindowShape, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Region *,(__arg2),A1), \
                  AROS_LCA(struct Hook *,(__arg3),A2), \
        struct IntuitionBase *, (__IntuitionBase), 143, Intuition)

#define ChangeWindowShape(arg1, arg2, arg3) \
    __ChangeWindowShape_WB(IntuitionBase, (arg1), (arg2), (arg3))

#define __SetDefaultScreenFont_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, SetDefaultScreenFont, \
                  AROS_LCA(struct TextFont *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 144, Intuition)

#define SetDefaultScreenFont(arg1) \
    __SetDefaultScreenFont_WB(IntuitionBase, (arg1))

#define __DoNotify_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(IPTR, DoNotify, \
                  AROS_LCA(Class *,(__arg1),A0), \
                  AROS_LCA(Object *,(__arg2),A1), \
                  AROS_LCA(struct ICData *,(__arg3),A2), \
                  AROS_LCA(struct opUpdate *,(__arg4),A3), \
        struct IntuitionBase *, (__IntuitionBase), 145, Intuition)

#define DoNotify(arg1, arg2, arg3, arg4) \
    __DoNotify_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#define __FreeICData_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, FreeICData, \
                  AROS_LCA(struct ICData *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 146, Intuition)

#define FreeICData(arg1) \
    __FreeICData_WB(IntuitionBase, (arg1))

#define __AllocIntuiMessage_WB(__IntuitionBase, __arg1) \
        AROS_LC1(struct IntuiMessage *, AllocIntuiMessage, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 148, Intuition)

#define AllocIntuiMessage(arg1) \
    __AllocIntuiMessage_WB(IntuitionBase, (arg1))

#define __FreeIntuiMessage_WB(__IntuitionBase, __arg1) \
        AROS_LC1NR(void, FreeIntuiMessage, \
                  AROS_LCA(struct IntuiMessage *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 149, Intuition)

#define FreeIntuiMessage(arg1) \
    __FreeIntuiMessage_WB(IntuitionBase, (arg1))

#define __SendIntuiMessage_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, SendIntuiMessage, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct IntuiMessage *,(__arg2),A1), \
        struct IntuitionBase *, (__IntuitionBase), 151, Intuition)

#define SendIntuiMessage(arg1, arg2) \
    __SendIntuiMessage_WB(IntuitionBase, (arg1), (arg2))

#define __ChangeDecoration_WB(__IntuitionBase, __arg1, __arg2) \
        AROS_LC2NR(void, ChangeDecoration, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(struct NewDecorator *,(__arg2),A0), \
        struct IntuitionBase *, (__IntuitionBase), 153, Intuition)

#define ChangeDecoration(arg1, arg2) \
    __ChangeDecoration_WB(IntuitionBase, (arg1), (arg2))

#define __WindowAction_WB(__IntuitionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, WindowAction, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(struct TagItem *,(__arg3),A1), \
        struct IntuitionBase *, (__IntuitionBase), 157, Intuition)

#define WindowAction(arg1, arg2, arg3) \
    __WindowAction_WB(IntuitionBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define WindowActionTags(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    WindowAction((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __ScrollWindowRasterNoFill_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, ScrollWindowRasterNoFill, \
                  AROS_LCA(struct Window *,(__arg1),A1), \
                  AROS_LCA(WORD,(__arg2),D0), \
                  AROS_LCA(WORD,(__arg3),D1), \
                  AROS_LCA(WORD,(__arg4),D2), \
                  AROS_LCA(WORD,(__arg5),D3), \
                  AROS_LCA(WORD,(__arg6),D4), \
                  AROS_LCA(WORD,(__arg7),D5), \
        struct IntuitionBase *, (__IntuitionBase), 159, Intuition)

#define ScrollWindowRasterNoFill(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __ScrollWindowRasterNoFill_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __SetPointerBounds_WB(__IntuitionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(ULONG, SetPointerBounds, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(struct Rectangle *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
                  AROS_LCA(struct TagItem *,(__arg4),A2), \
        struct IntuitionBase *, (__IntuitionBase), 160, Intuition)

#define SetPointerBounds(arg1, arg2, arg3, arg4) \
    __SetPointerBounds_WB(IntuitionBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define SetPointerBoundsTags(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetPointerBounds((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __StartScreenNotifyTagList_WB(__IntuitionBase, __arg1) \
        AROS_LC1(IPTR, StartScreenNotifyTagList, \
                  AROS_LCA(struct TagItem *,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 161, Intuition)

#define StartScreenNotifyTagList(arg1) \
    __StartScreenNotifyTagList_WB(IntuitionBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(INTUITION_NO_INLINE_STDARG)
#define StartScreenNotifyTags(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    StartScreenNotifyTagList((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __EndScreenNotify_WB(__IntuitionBase, __arg1) \
        AROS_LC1(BOOL, EndScreenNotify, \
                  AROS_LCA(IPTR,(__arg1),A0), \
        struct IntuitionBase *, (__IntuitionBase), 162, Intuition)

#define EndScreenNotify(arg1) \
    __EndScreenNotify_WB(IntuitionBase, (arg1))

__END_DECLS

#endif /* DEFINES_INTUITION_H*/
