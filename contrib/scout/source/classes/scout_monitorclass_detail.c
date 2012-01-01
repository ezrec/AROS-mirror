/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#include "system_headers.h"

#define boolstr(val) (val ? txtTrue : txtFalse)

struct MonitorClassDetailWinData {
    TEXT mdwd_Title[WINDOW_TITLE_LENGTH];
    APTR mdwd_Texts[54];
    Object *mdwd_Monitor;
};

STATIC CONST struct LongFlag pointerTypeFlags[] = {
    { 0x00000001, "PointerType_3Plus1" },
    { 0x00000002, "PointerType_2Plus1" },
    { 0x00000004, "PointerType_ARGB"   },
    { 0x00000008, "< ??? >" },
    { 0x00000010, "< ??? >" },
    { 0x00000020, "< ??? >" },
    { 0x00000040, "< ??? >" },
    { 0x00000080, "< ??? >" },
    { 0x00000100, "< ??? >" },
    { 0x00000200, "< ??? >" },
    { 0x00000400, "< ??? >" },
    { 0x00000800, "< ??? >" },
    { 0x00001000, "< ??? >" },
    { 0x00002000, "< ??? >" },
    { 0x00004000, "< ??? >" },
    { 0x00008000, "< ??? >" },
    { 0x00010000, "< ??? >" },
    { 0x00020000, "< ??? >" },
    { 0x00040000, "< ??? >" },
    { 0x00080000, "< ??? >" },
    { 0x00100000, "< ??? >" },
    { 0x00200000, "< ??? >" },
    { 0x00400000, "< ??? >" },
    { 0x00800000, "< ??? >" },
    { 0x01000000, "< ??? >" },
    { 0x02000000, "< ??? >" },
    { 0x04000000, "< ??? >" },
    { 0x08000000, "< ??? >" },
    { 0x10000000, "< ??? >" },
    { 0x20000000, "< ??? >" },
    { 0x40000000, "< ??? >" },
    { 0x80000000, "< ??? >" },
    { 0         , NULL      },
};

#ifdef HAVE_MONITORCLASS

static void GetMonitor(struct MonitorClassDetailWinData *mdwd, ULONG attrId, UBYTE n)
{
    Object *obj;
    STRPTR name;

    GetAttr(attrId, mdwd->mdwd_Monitor, (IPTR *)&obj);
#ifdef __AROS__
    if (obj)
	GetAttr(MA_MonitorName, obj, (IPTR *)&name);
    else
	name = NULL;
    MySetContentsHealed(mdwd->mdwd_Texts[n], "%s", nonetest(name));
#else
    /* On production versions of MorphOS these attributes return monitor ID
       instead of object pointer. This is going to be fixed in MorphOS v2.6 */
    MySetContents(mdwd->mdwd_Texts[n], "$%08lx", obj);
#endif
}

static void SetDefaultPixFmt(struct MonitorClassDetailWinData *mdwd, UBYTE depth, UBYTE n)
{
    IPTR pf = -1;
    STRPTR pfname;

    DoMethod(mdwd->mdwd_Monitor, MM_GetDefaultPixelFormat, depth, &pf);

    switch (pf) {
    case -1:
	pfname = txtNotAvailable;
	break;
    case PIXFMT_LUT8:
	pfname = txtLUT8;
	break;
    case PIXFMT_RGB15:
	pfname = txtRGB15;
	break;
    case PIXFMT_BGR15:
	pfname = txtBGR15;
	break;
    case PIXFMT_RGB15PC:
	pfname = txtRGB15PC;
	break;
    case PIXFMT_BGR15PC:
	pfname = txtBGR15PC;
	break;
    case PIXFMT_RGB16:
	pfname = txtRGB16;
	break;
    case PIXFMT_BGR16:
	pfname = txtBGR16;
	break;
    case PIXFMT_RGB16PC:
	pfname = txtRGB16PC;
	break;
    case PIXFMT_BGR16PC:
	pfname = txtBGR16PC;
	break;
    case PIXFMT_RGB24:
	pfname = txtRGB24;
	break;
    case PIXFMT_BGR24:
	pfname = txtBGR24;
	break;
    case PIXFMT_ARGB32:
	pfname = txtARGB32;
	break;
    case PIXFMT_BGRA32:
	pfname = txtBGRA32;
	break;
    case PIXFMT_RGBA32:
	pfname = txtRGBA32;
	break;
    default:
	pfname = txtInvalid;
	break;
    }

    MySetContents(mdwd->mdwd_Texts[n], pfname);
}

static void QueryPointerBounds(struct MonitorClassDetailWinData *mdwd, ULONG type, UBYTE n)
{
    ULONG width = 0;
    ULONG height = 0;

    if (DoMethod(mdwd->mdwd_Monitor, MM_GetPointerBounds, type, &width, &height))
     	MySetContents(mdwd->mdwd_Texts[n], "%lD x %lD", width, height);
    else
	MySetContents(mdwd->mdwd_Texts[n], "%s", txtNotAvailable);
}

#endif

STATIC void SetDetails( struct IClass *cl,
                        Object *obj )
{
#ifdef HAVE_MONITORCLASS
    struct MonitorClassDetailWinData *mdwd = INST_DATA(cl, obj);
    Object *Mon = mdwd->mdwd_Monitor;
    STRPTR MonitorName;
    STRPTR str;
    IPTR id;
    ULONG *pixelformats;
    UBYTE n, pf;

    GetAttr(MA_MonitorName , Mon, (IPTR *)&MonitorName);
    MySetContents(mdwd->mdwd_Texts[ 0], "%s", MonitorName);
    GetAttr(MA_Manufacturer, Mon, (IPTR *)&str);
    MySetContents(mdwd->mdwd_Texts[ 1], "%s", nonetest(str));
    GetAttr(MA_ManufacturerID, Mon, &id);
    MySetContents(mdwd->mdwd_Texts[ 2], "$%08lx", id);
    GetAttr(MA_ProductID, Mon, &id);
    MySetContents(mdwd->mdwd_Texts[ 3], "$%08lx", id);
    GetAttr(MA_MemorySize, Mon, &id);
    MySetContents(mdwd->mdwd_Texts[ 4], "%12lD", id);
    GetMonitor(mdwd, MA_TopLeftMonitor     ,  5);
    GetMonitor(mdwd, MA_TopMiddleMonitor   ,  6);
    GetMonitor(mdwd, MA_TopRightMonitor    ,  7);
    GetMonitor(mdwd, MA_MiddleLeftMonitor  ,  8);
    GetMonitor(mdwd, MA_MiddleRightMonitor ,  9);
    GetMonitor(mdwd, MA_BottomLeftMonitor  , 10);
    GetMonitor(mdwd, MA_BottomMiddleMonitor, 11);
    GetMonitor(mdwd, MA_BottomRightMonitor , 12);
    GetAttr(MA_GammaControl, Mon, &id);
    MySetContents(mdwd->mdwd_Texts[13], "%s", boolstr(id));
    GetAttr(MA_PointerType, Mon, &id);
    set(mdwd->mdwd_Texts[14], MUIA_FlagsButton_Flags, id);
    GetAttr(MA_DriverName, Mon, (IPTR *)&str);
    MySetContentsHealed(mdwd->mdwd_Texts[15], "%s", nonetest(str));
    GetAttr(MA_MemoryClock, Mon, (IPTR *)&id);
    MySetContents(mdwd->mdwd_Texts[52], "%12lD", id);
#ifdef MA_Windowed
    GetAttr(MA_Windowed, Mon, (IPTR *)&id);
    MySetContents(mdwd->mdwd_Texts[53], "%s", boolstr(id));
#endif
    SetDefaultPixFmt(mdwd,  8, 16);
    SetDefaultPixFmt(mdwd, 15, 17);
    SetDefaultPixFmt(mdwd, 16, 18);
    SetDefaultPixFmt(mdwd, 24, 19);
    SetDefaultPixFmt(mdwd, 32, 20);
    QueryPointerBounds(mdwd, PointerType_3Plus1, 21);
    QueryPointerBounds(mdwd, PointerType_2Plus1, 22);
    QueryPointerBounds(mdwd, PointerType_ARGB  , 23);
    GetAttr(MA_PixelFormats, Mon, (IPTR *)&pixelformats);
    n = 24;
    for (pf = PIXFMT_LUT8; pf <= PIXFMT_RGBA32; pf++) {
	ULONG s3d = -1;
	STRPTR t3d;

	MySetContents(mdwd->mdwd_Texts[n++], "%s", boolstr(pixelformats[pf]));
	DoMethod(Mon, MM_Query3DSupport, pf, &s3d);
	switch (s3d) {
	case MSQUERY3D_UNKNOWN:
	    t3d = txt3dUnknown;
	    break;
	case MSQUERY3D_NODRIVER:
	    t3d = txt3dNoDriver;
	    break;
	case MSQUERY3D_SWDRIVER:
	    t3d = txt3dSwDriver;
	    break;
	case MSQUERY3D_HWDRIVER:
	    t3d = txt3dHwDriver;
	    break;
	default:
	    t3d = txtInvalid;
	    break;
	}
	MySetContents(mdwd->mdwd_Texts[n++], "%s", t3d);
    }

    set(obj, MUIA_Window_Title, MyGetChildWindowTitle(txtMonitorClassDetailTitle, MonitorName, mdwd->mdwd_Title, sizeof(mdwd->mdwd_Title)));
#endif
}

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    APTR group, texts[54], exitButton;

    if ((obj = (Object *)DoSuperNew(cl, obj,
	MUIA_HelpNode, "MonitorClass",
	MUIA_Window_ID, MakeID('M','C','L','D'),
        WindowContents, VGroup,

            Child, (IPTR)(group = (Object *)ScrollgroupObject,
                MUIA_CycleChain, TRUE,
                MUIA_Scrollgroup_FreeHoriz, FALSE,
                MUIA_Scrollgroup_Contents, (IPTR)VGroupV,
                    MUIA_Background, MUII_GroupBack,
                    Child, (IPTR)VGroup,
                        GroupFrame,
                        Child, (IPTR)ColGroup(2),
			    Child, (IPTR)MyLabel2(txtMonitorClassMonitorName),
                            Child, (IPTR)(texts[ 0] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassManufacturer),
                            Child, (IPTR)(texts[ 1] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassManufacturerID),
			    Child, (IPTR)(texts[ 2] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassProductID),
                            Child, (IPTR)(texts[ 3] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassMemorySize),
			    Child, (IPTR)(texts[ 4] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassTopLeft),
			    Child, (IPTR)(texts[ 5] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassTopMiddle),
			    Child, (IPTR)(texts[ 6] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassTopRight),
			    Child, (IPTR)(texts[ 7] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassMiddleLeft),
			    Child, (IPTR)(texts[ 8] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassMiddleRight),
			    Child, (IPTR)(texts[ 9] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassBottomLeft),
			    Child, (IPTR)(texts[10] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassBottomMiddle),
			    Child, (IPTR)(texts[11] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassBottomRight),
			    Child, (IPTR)(texts[12] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassGammaControl),
			    Child, (IPTR)(texts[13] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassPointerType),
			    Child, (IPTR)(texts[14] = (Object *)FlagsButtonObject,
				    MUIA_FlagsButton_Type, MUIV_FlagsButton_Type_Long,
				    MUIA_FlagsButton_Title, (IPTR)txtMonitorClassPointerType,
				    MUIA_FlagsButton_BitArray, (IPTR)pointerTypeFlags,
				    MUIA_FlagsButton_WindowTitle, (IPTR)txtMonitorClassPointerTypeTitle,
				End),
			    Child, (IPTR)MyLabel2(txtMonitorClassDriverName),
			    Child, (IPTR)(texts[15] = MyTextObject6()),
			    Child, (IPTR)MyLabel2(txtMonitorClassMemoryClock),
			    Child, (IPTR)(texts[52] = MyTextObject6()),
#ifdef MA_Windowed
			    Child, (IPTR)MyLabel2(txtMonitorClassWindowed),
			    Child, (IPTR)(texts[53] = MyTextObject6()),
#endif
			End,
		        Child, (IPTR)VGroup,
			    GroupFrameT(txtMonitorClassDefaultPixelFormat),
			    Child, (IPTR)ColGroup(2),
				Child, (IPTR)MyLabel2(txtScreenModeDepth),
				Child, (IPTR)MyLabel2(txtPixelFormat),
				Child, (IPTR)MyLabel2("8"),
				Child, (IPTR)(texts[16] = MyTextObject6()),
				Child, (IPTR)MyLabel2("15"),
				Child, (IPTR)(texts[17] = MyTextObject6()),
				Child, (IPTR)MyLabel2("16"),
				Child, (IPTR)(texts[18] = MyTextObject6()),
				Child, (IPTR)MyLabel2("24"),
				Child, (IPTR)(texts[19] = MyTextObject6()),
				Child, (IPTR)MyLabel2("32"),
				Child, (IPTR)(texts[20] = MyTextObject6()),
                            End,
			End,
		        Child, (IPTR)VGroup,
			    GroupFrameT(txtMonitorClassPointerBounds),
			    Child, (IPTR)ColGroup(2),
				Child, (IPTR)MyLabel2(txtPointerType),
				Child, (IPTR)MyLabel2(txtPointerSize),
				Child, (IPTR)MyLabel2("PointerType_3Plus1"),
				Child, (IPTR)(texts[21] = MyTextObject6()),
				Child, (IPTR)MyLabel2("PointerType_2Plus1"),
				Child, (IPTR)(texts[22] = MyTextObject6()),
				Child, (IPTR)MyLabel2("PointerType_ARGB"),
				Child, (IPTR)(texts[23] = MyTextObject6()),			   
                            End,
			End,
		        Child, (IPTR)VGroup,
			    GroupFrameT(txtMonitorClassPixelFormatsGroup),
			    Child, (IPTR)ColGroup(3),
				Child, (IPTR)MyLabel2(txtPixelFormat),
				Child, (IPTR)MyLabel2(txtMonitorClassPixelFormats),
				Child, (IPTR)MyLabel2(txtMonitorClass3DSupport),
				Child, (IPTR)MyLabel2(txtLUT8),
				Child, (IPTR)(texts[24] = MyTextObject6()),
				Child, (IPTR)(texts[25] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtRGB15),
				Child, (IPTR)(texts[26] = MyTextObject6()),
				Child, (IPTR)(texts[27] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtBGR15),
				Child, (IPTR)(texts[28] = MyTextObject6()),
				Child, (IPTR)(texts[29] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtRGB15PC),
				Child, (IPTR)(texts[30] = MyTextObject6()),
				Child, (IPTR)(texts[31] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtBGR15PC),
				Child, (IPTR)(texts[32] = MyTextObject6()),
				Child, (IPTR)(texts[33] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtRGB16),
				Child, (IPTR)(texts[34] = MyTextObject6()),
				Child, (IPTR)(texts[35] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtBGR16),
				Child, (IPTR)(texts[36] = MyTextObject6()),
				Child, (IPTR)(texts[37] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtRGB16PC),
				Child, (IPTR)(texts[38] = MyTextObject6()),
				Child, (IPTR)(texts[39] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtBGR16PC),
				Child, (IPTR)(texts[40] = MyTextObject6()),
				Child, (IPTR)(texts[41] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtRGB24),
				Child, (IPTR)(texts[42] = MyTextObject6()),
				Child, (IPTR)(texts[43] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtBGR24),
				Child, (IPTR)(texts[44] = MyTextObject6()),
				Child, (IPTR)(texts[45] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtARGB32),
				Child, (IPTR)(texts[46] = MyTextObject6()),
				Child, (IPTR)(texts[47] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtBGRA32),
				Child, (IPTR)(texts[48] = MyTextObject6()),
				Child, (IPTR)(texts[49] = MyTextObject6()),
				Child, (IPTR)MyLabel2(txtRGBA32),
				Child, (IPTR)(texts[50] = MyTextObject6()),
				Child, (IPTR)(texts[51] = MyTextObject6()),
                            End,
			End,
                    End,
                End,
            End),
            Child, (IPTR)MyVSpace(4),
            Child, (IPTR)(exitButton = MakeButton(txtExit)),
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct MonitorClassDetailWinData *mdwd = INST_DATA(cl, obj);
        APTR parent;

        CopyMemQuick(texts, mdwd->mdwd_Texts, sizeof(mdwd->mdwd_Texts));

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        set(obj, MUIA_Window_DefaultObject, group);

        DoMethod(parent,         MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,            MUIM_Notify, MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod(exitButton,     MUIM_Notify, MUIA_Pressed,             FALSE, obj,                     3, MUIM_Set, MUIA_Window_CloseRequest, TRUE);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    set(obj, MUIA_Window_Open, FALSE);

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct MonitorClassDetailWinData *mdwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_Window_ParentWindow:
                DoMethod(obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 5, MUIM_Application_PushMethod, tag->ti_Data, 2, MUIM_Window_RemChildWindow, obj);
                break;

	    case MUIA_MonitorClassDetailWin_MonitorObject:
		mdwd->mdwd_Monitor = (Object *)tag->ti_Data;
                SetDetails(cl, obj);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

DISPATCHER(MonitorClassDetailWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:     return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE: return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:     return (mSet(cl, obj, (APTR)msg));
    }

    return DoSuperMethodA(cl, obj, msg);
}

APTR MakeMonitorClassDetailWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct MonitorClassDetailWinData), ENTRY(MonitorClassDetailWinDispatcher));
}

