/*
    Copyright © 1997-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc: DOpus library initialization code.
    Lang: English.
*/

#include <stddef.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <aros/symbolsets.h>
#include <proto/exec.h>

/* #include "initstruct.h" */
#include "dopuslib.h"
/*#include "initstruct.h"*/
#include LC_LIBDEFS_FILE

#define DEBUG 0
#include <aros/debug.h>


extern UBYTE pdb_cycletop[];
extern UBYTE pdb_cyclebot[];
extern UBYTE pdb_check[];
extern const struct DefaultString default_strings[];

struct StringData stringdata = {default_strings,STR_STRING_COUNT,NULL,NULL,
    0,STRING_VERSION,NULL,NULL};

#warning Remove DOpusBase as global variable
/* AROS: DOpusBase global var shadows the parameter with the same name */

void set_global_dopusbase(struct DOpusBase *base)
{
	DOpusBase = base;
}

static int GM_UNIQUENAME(Init)(LIBBASETYPEPTR LIBBASE)
{
    /* This function is single-threaded by exec by calling Forbid. */

    /* We have to use a function to set the global var DOpusBase
       since it shadows the LIBBASE parameter (LIBBASE is a #define
       for DOpusBase
    */
    
    set_global_dopusbase(LIBBASE);

    /* ----------------- */
    
    LIBBASE->DOSBase = DOSBase;
    LIBBASE->IntuitionBase = IntuitionBase;
    LIBBASE->GfxBase = GfxBase;
    LIBBASE->LayersBase = (struct LayersBase *)LayersBase;
    
//    init_dopus_library();
    
    LIBBASE->pdb_cycletop = (PLANEPTR)&pdb_cycletop;
    LIBBASE->pdb_cyclebot = (PLANEPTR)&pdb_cyclebot;
kprintf("DOPUS_INIT: ct: %p, cb: %p\n"
    , LIBBASE->pdb_cycletop
    , LIBBASE->pdb_cyclebot
);
    LIBBASE->pdb_check = (PLANEPTR)&pdb_check;
    
    LIBBASE->pdb_Flags = 0;

    if (DoReadStringFile(&stringdata,"dopus4_lib.catalog"))
    {
        string_table=stringdata.string_table;
        initstrings();
    }
    else
        return FALSE;

    return TRUE;
}

ADD2INITLIB(GM_UNIQUENAME(Init), 0)

AROS_LH1(void, GetWBScreen,
    AROS_LHA(struct Screen *, scrbuf, A0),
    struct Library *, DOpusBase, 51, DOpus)
{
    AROS_LIBFUNC_INIT

    GetWBScreen(scrbuf);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, Seed,
    AROS_LHA(int , seed, D0),
    struct Library *, DOpusBase, 55, DOpus)
{
    AROS_LIBFUNC_INIT

    return Seed(seed);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, Random,
	AROS_LHA(int , limit, D0),
	struct Library *, DOpusBase, 56, DOpus)
{
    AROS_LIBFUNC_INIT

    return Random(limit);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, StrToUpper,
	AROS_LHA(char *, from, A0),
	AROS_LHA(char *, to, A1),
	struct Library *, DOpusBase, 57, DOpus)
{
    AROS_LIBFUNC_INIT

    StrToUpper(from, to);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, StrToLower,
	AROS_LHA(char *, from, A0),
	AROS_LHA(char *, to, A1),
	struct Library *, DOpusBase, 58, DOpus)
{
    AROS_LIBFUNC_INIT

    StrToLower(from, to);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, DisableGadget,
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp,  A1),
	AROS_LHA(int, xoff, D0),
	AROS_LHA(int, yoff, D1),
	struct Library *, DOpusBase, 80, DOpus)
{
    AROS_LIBFUNC_INIT

    DisableGadget(gad, rp, xoff, yoff);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, EnableGadget,
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp,  A1),
	AROS_LHA(int, xoff, D0),
	AROS_LHA(int, yoff, D1),
	struct Library *, DOpusBase, 81, DOpus)
{
    AROS_LIBFUNC_INIT

    EnableGadget(gad, rp, xoff, yoff);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, GhostGadget,
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp,  A1),
	AROS_LHA(int, xoff, D0),
	AROS_LHA(int, yoff, D1),
	struct Library *, DOpusBase, 82, DOpus)
{
    AROS_LIBFUNC_INIT

    GhostGadget(gad, rp, xoff, yoff);

    AROS_LIBFUNC_EXIT
}

#if 1 // Removed from new lib version?
AROS_LH2(struct IORequest *, LCreateExtIO,
	AROS_LHA(struct MsgPort *, port, A0),
	AROS_LHA(int , size,  D0),
	struct Library *, DOpusBase, 11, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCreateExtIO(port, size);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(struct MsgPort *, LCreatePort,
	AROS_LHA(char *, name, A0),
	AROS_LHA(int , pri,  D0),
	struct Library *, DOpusBase, 12, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCreatePort(name, pri);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, LDeleteExtIO,
	AROS_LHA(struct IORequest *, ioext, A0),
	struct Library *, DOpusBase, 13, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDeleteExtIO(ioext);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, LDeletePort,
	AROS_LHA(struct MsgPort *, port, A0),
	struct Library *, DOpusBase, 14, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDeletePort(port);

    AROS_LIBFUNC_EXIT
}
#endif

AROS_LH1(char, LToUpper,
	AROS_LHA(char, ch, D0),
	struct Library *, DOpusBase, 15, DOpus)
{
    AROS_LIBFUNC_INIT

    return LToUpper(ch);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(char, LToLower,
	AROS_LHA(char, ch, D0),
	struct Library *, DOpusBase, 16, DOpus)
{
    AROS_LIBFUNC_INIT

    return LToLower(ch);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, LStrCat,
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	struct Library *, DOpusBase, 17, DOpus)
{
    AROS_LIBFUNC_INIT

    LStrCat(s1, s2);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, LStrnCat,
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	AROS_LHA(int, len, D0),
	struct Library *, DOpusBase, 18, DOpus)
{
    AROS_LIBFUNC_INIT

    LStrnCat(s1, s2, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, LStrCpy,
	AROS_LHA(char *, to, A0),
	AROS_LHA(char *, from, A1),
	struct Library *, DOpusBase, 19, DOpus)
{
    AROS_LIBFUNC_INIT

    LStrCpy(to, from);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, LStrnCpy,
	AROS_LHA(char *, to, A0),
	AROS_LHA(char *, from, A1),
	AROS_LHA(int, len, A1),
	struct Library *, DOpusBase, 20, DOpus)
{
    AROS_LIBFUNC_INIT

    LStrnCpy(to, from, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, LStrCmpI,
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	struct Library *, DOpusBase, 23, DOpus)
{
    AROS_LIBFUNC_INIT

    return LStrCmpI(s1, s2);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, LStrnCmpI,
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	AROS_LHA(int, len, A1),
	struct Library *, DOpusBase, 24, DOpus)
{
    AROS_LIBFUNC_INIT

    return LStrnCmpI(s1, s2, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, LStrnCmp,
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	AROS_LHA(int, len, A1),
	struct Library *, DOpusBase, 22, DOpus)
{
    AROS_LIBFUNC_INIT

    return LStrnCmp(s1, s2, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, LStrCmp,
	AROS_LHA(char *, s1, A0),
	AROS_LHA(char *, s2, A1),
	struct Library *, DOpusBase, 21, DOpus)
{
    AROS_LIBFUNC_INIT

    return LStrCmp(s1, s2);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, BtoCStr,
	AROS_LHA(BSTR, bstr, A0),
	AROS_LHA(char *, cstr, A1),
	AROS_LHA(int, len, D0),
	struct Library *, DOpusBase, 31, DOpus)
{
    AROS_LIBFUNC_INIT

    BtoCStr(bstr, cstr, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, ActivateStrGad,
	AROS_LHA(struct Gadget *, gadget, A0),
	AROS_LHA(struct Window *, window, A1),
	struct Library *, DOpusBase, 62, DOpus)
{
    AROS_LIBFUNC_INIT

    ActivateStrGad(gadget, window);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, RefreshStrGad,
	AROS_LHA(struct Gadget *, gadget, A0),
	AROS_LHA(struct Window *, window, A1),
	struct Library *, DOpusBase, 63, DOpus)
{
    AROS_LIBFUNC_INIT

    RefreshStrGad(gadget, window);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, ReadConfig,
	AROS_LHA(char *, name, A0),
	AROS_LHA(struct ConfigStuff *, cstuff, A1),
	struct Library *, DOpusBase, 69, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoReadConfig(name, cstuff);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, SaveConfig,
	AROS_LHA(char *, name, A0),
	AROS_LHA(struct ConfigStuff *, cstuff, A1),
	struct Library *, DOpusBase, 70, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoSaveConfig(name, cstuff);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, DefaultConfig,
	AROS_LHA(struct ConfigStuff *, cstuff, A0),
	struct Library *, DOpusBase, 71, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoDefaultConfig(cstuff);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, GetDevices,
	AROS_LHA(struct ConfigStuff *, cstuff, A0),
	struct Library *, DOpusBase, 72, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoGetDevices(cstuff);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, AssignGadget,
	AROS_LHA(struct ConfigStuff *, cstuff, A0),
	AROS_LHA(int, bk,	D0),
	AROS_LHA(int, gad,	D1),
	AROS_LHA(char *, name,	A1),
	AROS_LHA(char *, func,	A2),
	struct Library *, DOpusBase, 73, DOpus)
{
    AROS_LIBFUNC_INIT

    DoAssignGadget(cstuff, bk, gad, name, func);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, AssignMenu,
	AROS_LHA(struct ConfigStuff *, cstuff, A0),
	AROS_LHA(int, men,	D0),
	AROS_LHA(char *, name,	A1),
	AROS_LHA(char *, func,	A2),
	struct Library *, DOpusBase, 74, DOpus)
{
    AROS_LIBFUNC_INIT

    DoAssignMenu(cstuff, men, name, func);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, FreeConfig,
	AROS_LHA(struct ConfigStuff *, cstuff, A0),
	struct Library *, DOpusBase, 77, DOpus)
{
    AROS_LIBFUNC_INIT

    DoFreeConfig(cstuff);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, CheckConfig,
	AROS_LHA(struct ConfigStuff *, cstuff, A0),
	struct Library *, DOpusBase, 86, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCheckConfig(cstuff);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, FindSystemFile,
	AROS_LHA(char *, name,	A0),
	AROS_LHA(char *, buf,	A1),
	AROS_LHA(int, size, 	D0),
	AROS_LHA(int, type, 	A1),
	struct Library *, DOpusBase, 75, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoFindSystemFile(name, buf, size, type);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, Assign,
	AROS_LHA(char *, name, 	A0),
	AROS_LHA(char *, dir, 	A1),
	struct Library *, DOpusBase, 32, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoAssign(name, dir);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(char *, BaseName,
	AROS_LHA(char *, path, 	A0),
	struct Library *, DOpusBase, 33, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoBaseName(path);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int , PathName,
	AROS_LHA(BPTR , lock, 	A0),
	AROS_LHA(char *, buf, 	A1),
	AROS_LHA(int , len, 	D0),
	struct Library *, DOpusBase, 35, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoPathName(lock, buf, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int , TackOn,
	AROS_LHA(char *, path, 	A0),
	AROS_LHA(char *, file, 	A1),
	AROS_LHA(int , len, 	D0),
	struct Library *, DOpusBase, 37, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoTackOn(path, file, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int , CompareLock,
	AROS_LHA(BPTR, l1, 	A0),
	AROS_LHA(BPTR, l2, 	A1),
	struct Library *, DOpusBase, 34, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCompareLock(l1, l2);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int , SearchPathList,
	AROS_LHA(char *, name, 	A0),
	AROS_LHA(char *, buf, 	A1),
	AROS_LHA(int, len, 	D0),
	struct Library *, DOpusBase, 52, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoSearchPathList(name, buf, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int , CheckExist,
	AROS_LHA(char *, name, 	A0),
	AROS_LHA(int *, size, 	A1),
	struct Library *, DOpusBase, 53, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCheckExist(name, size);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, Do3DBox,
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, tp, D4),
	AROS_LHA(int, bp, D5),
	struct Library *, DOpusBase, 6, DOpus)
{
    AROS_LIBFUNC_INIT

    Do3DBox(rp, x, y, w, h, tp, bp);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, Do3DStringBox,
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, tp, D4),
	AROS_LHA(int, bp, D5),
	struct Library *, DOpusBase, 7, DOpus)
{
    AROS_LIBFUNC_INIT

    Do3DStringBox(rp, x, y, w, h, tp, bp);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, Do3DCycleBox,
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, tp, D4),
	AROS_LHA(int, bp, D5),
	struct Library *, DOpusBase, 8, DOpus)
{
    AROS_LIBFUNC_INIT

    Do3DCycleBox(rp, x, y, w, h, tp, bp);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, DrawCheckMark,
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, checked, D2),
	struct Library *, DOpusBase, 44, DOpus)
{
    AROS_LIBFUNC_INIT

    DrawCheckMark(rp, x, y, checked);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, HiliteGad,
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(struct RastPort *, rp, A1),
	struct Library *, DOpusBase, 67, DOpus)
{
    AROS_LIBFUNC_INIT

    HiliteGad(gad, rp);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, AddGadgetBorders,
	AROS_LHA(struct DOpusRemember ** , key, A0),
	AROS_LHA(struct Gadget * , gadget, 	A1),
	AROS_LHA(int , count, 	D0),
	AROS_LHA(int , shine, 	D1),
	AROS_LHA(int , shadow, 	D2),
	struct Library *, DOpusBase, 98, DOpus)
{
    AROS_LIBFUNC_INIT

    DoAddGadgetBorders(key, gadget, count, shine, shadow);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, CreateGadgetBorders,
	AROS_LHA(struct DOpusRemember ** , key, A0),
	AROS_LHA(int , w, 	D0),
	AROS_LHA(int , h, 	D1),
	AROS_LHA(struct Border ** , selborder, 		A1),
	AROS_LHA(struct Border ** , unselborder, 	A2),
	AROS_LHA(int , dogear, 	D2),
	AROS_LHA(int , shine, 	D3),
	AROS_LHA(int , shadow, 	D4),
	struct Library *, DOpusBase, 99, DOpus)
{
    AROS_LIBFUNC_INIT

    DoCreateGadgetBorders(key, w, h, selborder, unselborder, dogear, shine,
        shadow);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, SelectGadget,
	AROS_LHA(struct Window * , window, A0),
	AROS_LHA(struct Gadget * , gadget, A1),
	struct Library *, DOpusBase, 100, DOpus)
{
    AROS_LIBFUNC_INIT

    DoSelectGadget(window, gadget);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, StampToStr,
	AROS_LHA(struct DOpusDateTime *, dt, A0),
	struct Library *, DOpusBase, 38, DOpus)
{
    AROS_LIBFUNC_INIT

    DoStampToStr(dt);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(int, StrToStamp,
	AROS_LHA(struct DOpusDateTime *, dt, A0),
	struct Library *, DOpusBase, 39, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoStrToStamp(dt);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, CompareDate,
	AROS_LHA(struct DateStamp *, date,	A0),
	AROS_LHA(struct DateStamp *, date2,	A1),
	struct Library *, DOpusBase, 54, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCompareDate(date, date2);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(int, AddGadgets,
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, firstgad, A1),
	AROS_LHA(char **, text, A2),
	AROS_LHA(int, count, 	D0),
	AROS_LHA(int, fg, 	D1),
	AROS_LHA(int, bg, 	D2),
	AROS_LHA(int, add, 	D3),
	struct Library *, DOpusBase, 61, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoAddGadgets(win, firstgad, text, count, fg, bg, add);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, DoRMBGadget,
	AROS_LHA(struct RMBGadget *, gad,	A0),
	AROS_LHA(struct Window *, window,	A1),
	struct Library *, DOpusBase, 60, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoDoRMBGadget(gad, window);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, DoCycleGadget,
	AROS_LHA(struct Gadget *, gad,		A0),
	AROS_LHA(struct Window *, window,	A1),
	AROS_LHA(char **	, choices,	A2),
	AROS_LHA(int 		, select,	D0),
	struct Library *, DOpusBase, 78, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDoCycleGadget(gad, window, choices, select);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, UScoreText,
	AROS_LHA(struct RastPort *, rp,		A0),
	AROS_LHA(char *, buf,	A1),
	AROS_LHA(int, xp,	D0),
	AROS_LHA(int, yp,	D1),
	AROS_LHA(int, up,	D2),
	struct Library *, DOpusBase, 79, DOpus)
{
    AROS_LIBFUNC_INIT

    DoUScoreText(rp, buf, xp, yp, up);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, DoArrow,
	AROS_LHA(struct RastPort *, p, A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(int, fg, D4),
	AROS_LHA(int, bg, D5),
	AROS_LHA(int, d,  D6),
	struct Library *, DOpusBase, 9, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDoArrow(p, x, y, w, h, fg, bg, d); /* Do-do-do-do-do-do... */

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, DrawRadioButton,
	AROS_LHA(struct RastPort *, rp,  A0),
	AROS_LHA(int, x, 	D0),
	AROS_LHA(int, y, 	D1),
	AROS_LHA(int, w, 	D2),
	AROS_LHA(int, h, 	D3),
	AROS_LHA(int, hi, 	D4),
	AROS_LHA(int, lo, 	D5),
	struct Library *, DOpusBase, 83, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDrawRadioButton(rp, x, y, w, h, hi, lo);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(struct Image *, GetButtonImage,
	AROS_LHA(int, w, 	D0),
	AROS_LHA(int, h, 	D1),
	AROS_LHA(int, fg, 	D2),
	AROS_LHA(int, bg, 	D3),
	AROS_LHA(int, fpen, 	D4),
	AROS_LHA(int, bpen, 	D5),
	AROS_LHA(struct DOpusRemember **, key, A0),
	struct Library *, DOpusBase, 84, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoGetButtonImage(w, h, fg, bg, fpen, bpen, key);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(struct Image *, GetCheckImage,
	AROS_LHA(int, fg, D0),
	AROS_LHA(int, bg, D1),
	AROS_LHA(int, pen, D2),
	AROS_LHA(struct DOpusRemember **, key, A0),
	struct Library *, DOpusBase, 87, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoGetCheckImage(fg, bg, pen, key);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, Do3DFrame,
	AROS_LHA(struct RastPort *, rp,  A0),
	AROS_LHA(int, x, D0),
	AROS_LHA(int, y, D1),
	AROS_LHA(int, w, D2),
	AROS_LHA(int, h, D3),
	AROS_LHA(char *, title, A1),
	AROS_LHA(int, hi, D4),
	AROS_LHA(int, lo, D5),
	struct Library *, DOpusBase, 76, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDo3DFrame(rp, x, y, w, h, title, hi, lo);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, DoGlassImage,
	AROS_LHA(struct RastPort *, rp,  A0),
	AROS_LHA(struct Gadget *, gadget, A1),
	AROS_LHA(int, shine,	D0),
	AROS_LHA(int, shadow, 	D1),
	AROS_LHA(int, type, 	D2),
	struct Library *, DOpusBase, 93, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDoGlassImage(rp, gadget, shine, shadow, type);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, ReadStringFile,
	AROS_LHA(struct StringData *, stringdata, 	A0),
	AROS_LHA(char *, filename, 	A1),
	struct Library *, DOpusBase, 95, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoReadStringFile(stringdata, filename);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, FreeStringFile,
	AROS_LHA(struct StringData *, stringdata, 	A0),
	struct Library *, DOpusBase, 96, DOpus)
{
    AROS_LIBFUNC_INIT

    DoFreeStringFile(stringdata);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, AddListView,
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(int, count, D0),
	struct Library *, DOpusBase, 40, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoAddListView(view, count);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, FixSliderBody,
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, gad, A1),
	AROS_LHA(int, count, D0),
	AROS_LHA(int, lines, D1),
	AROS_LHA(int, show,  D2),
	struct Library *, DOpusBase, 45, DOpus)
{
    AROS_LIBFUNC_INIT

    DoFixSliderBody(win, gad, count, lines, show);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, FixSliderPot,
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, gad, A1),
	AROS_LHA(int, off, 	D0),
	AROS_LHA(int, count, 	D1),
	AROS_LHA(int, lines, 	D2),
	AROS_LHA(int, show,  	D3),
	struct Library *, DOpusBase, 46, DOpus)
{
    AROS_LIBFUNC_INIT

    DoFixSliderPot(win, gad, off, count, lines, show);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, ShowSlider,
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, gad, A1),
	struct Library *, DOpusBase, 85, DOpus)
{
    AROS_LIBFUNC_INIT

    DoShowSlider(win, gad);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, GetSliderPos,
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(int, count, D0),
	AROS_LHA(int, lines, D1),
	struct Library *, DOpusBase, 47, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoGetSliderPos(gad, count, lines);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(struct DOpusListView *, ListViewIDCMP,
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(struct IntuiMessage *, imsg,  A1),
	struct Library *, DOpusBase, 41, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoListViewIDCMP(view, imsg);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, RefreshListView,
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(int, count, D0),
	struct Library *, DOpusBase, 42, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoRefreshListView(view, count);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, RemoveListView,
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(int, count, D0),
	struct Library *, DOpusBase, 43, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoRemoveListView(view, count);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, FSSetMenuStrip,
	AROS_LHA(struct Window *, window, 	A0),
	AROS_LHA(struct Menu *, firstmenu, 	A1),
	struct Library *, DOpusBase, 101, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoFSSetMenuStrip(window, firstmenu);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(int, RawkeyToStr,
	AROS_LHA(UWORD, code,	D0),
	AROS_LHA(UWORD, qual,	D1),
	AROS_LHA(char *, buf,	A0),
	AROS_LHA(char *, kbuf,	A1),
	AROS_LHA(int, len, 	D2),
	struct Library *, DOpusBase, 59, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoRawkeyToStr(code, qual, buf, kbuf, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, CheckNumGad,
	AROS_LHA(struct Gadget *, gad,	A0),
	AROS_LHA(struct Window *, win,	A1),
	AROS_LHA(int, min, 	D0),
	AROS_LHA(int, max, 	D1),
	struct Library *, DOpusBase, 64, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCheckNumGad(gad, win, min, max);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, CheckHexGad,
	AROS_LHA(struct Gadget *, gad,	A0),
	AROS_LHA(struct Window *, win,	A1),
	AROS_LHA(int, min, 	D0),
	AROS_LHA(int, max, 	D1),
	struct Library *, DOpusBase, 65, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoCheckHexGad(gad, win, min, max);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, Decode_RLE,
	AROS_LHA(char *, source,	A0),
	AROS_LHA(char *, dest,		A1),
	AROS_LHA(int, size, 	D0),
	struct Library *, DOpusBase, 94, DOpus)
{
    AROS_LIBFUNC_INIT

    DoDecode_RLE(source, dest, size);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, SetBusyPointer,
	AROS_LHA(struct Window *, wind,	A0),
	struct Library *, DOpusBase, 50, DOpus)
{
    AROS_LIBFUNC_INIT

    DoSetBusyPointer(wind);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(struct Window *, OpenRequester,
	AROS_LHA(struct RequesterBase *, reqbase,	A0),
	struct Library *, DOpusBase, 88, DOpus)
{
    AROS_LIBFUNC_INIT

    return R_OpenRequester(reqbase);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, CloseRequester,
	AROS_LHA(struct RequesterBase *, reqbase,	A0),
	struct Library *, DOpusBase, 89, DOpus)
{
    AROS_LIBFUNC_INIT

    R_CloseRequester(reqbase);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(APTR, AddRequesterObject,
	AROS_LHA(struct RequesterBase *, reqbase,	A0),
	AROS_LHA(struct TagItem *, taglist,		A1),
	struct Library *, DOpusBase, 90, DOpus)
{
    AROS_LIBFUNC_INIT

    return R_AddRequesterObject(reqbase, taglist);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, RefreshRequesterObject,
	AROS_LHA(struct RequesterBase *, reqbase,	A0),
	AROS_LHA(struct RequesterObject *, object,	A1),
	struct Library *, DOpusBase, 91, DOpus)
{
    AROS_LIBFUNC_INIT

    R_RefreshRequesterObject(reqbase, object);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, ObjectText,
	AROS_LHA(struct RequesterBase *, reqbase,	A0),
	AROS_LHA(short, left,	D0),
	AROS_LHA(short, top,	D1),
	AROS_LHA(short, width,	D2),
	AROS_LHA(short, height,	D3),
	AROS_LHA(char *, text,		A1),
	AROS_LHA(short, textpos,	D4),
	struct Library *, DOpusBase, 92, DOpus)
{
    AROS_LIBFUNC_INIT

    R_ObjectText(reqbase, left, top, width, height, text, textpos);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, StrCombine,
	AROS_LHA(char *, buf, A0),
	AROS_LHA(char *, one, A1),
	AROS_LHA(char *, two, A2),
	AROS_LHA(int, lim, D0),
	struct Library *, DOpusBase, 25, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoStrCombine(buf, one, two, lim);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(int, StrConcat,
	AROS_LHA(char *, buf, A0),
	AROS_LHA(char *, cat, A1),
	AROS_LHA(int, lim, D0),
	struct Library *, DOpusBase, 26, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoStrConcat(buf, cat, lim);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, Atoh,
	AROS_LHA(char *, buf, A0),
	AROS_LHA(int, len, D0),
	struct Library *, DOpusBase, 66, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoAtoh(buf, len);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, LMatchPattern,
	AROS_LHA(char *, parsepat, A0),
	AROS_LHA(char *, str, A1),
	struct Library *, DOpusBase, 28, DOpus)
{
    AROS_LIBFUNC_INIT

    return LMatchPattern(parsepat, str);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, LMatchPatternI,
	AROS_LHA(char *, parsepat, A0),
	AROS_LHA(char *, str, A1),
	struct Library *, DOpusBase, 30, DOpus)
{
    AROS_LIBFUNC_INIT

    return LMatchPatternI(parsepat, str);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, LParsePattern,
	AROS_LHA(char *, pat, A0),
	AROS_LHA(char *, parsepat, A1),
	struct Library *, DOpusBase, 27, DOpus)
{
    AROS_LIBFUNC_INIT

    LParsePattern(pat, parsepat);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, LParsePatternI,
	AROS_LHA(char *, pat, A0),
	AROS_LHA(char *, parsepat, A1),
	struct Library *, DOpusBase, 29, DOpus)
{
    AROS_LIBFUNC_INIT

    LParsePatternI(pat, parsepat);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(int, DoSimpleRequest,
	AROS_LHA(struct Window *, window, A0),
	AROS_LHA(struct DOpusSimpleRequest *, simple, A1),
	struct Library *, DOpusBase, 68, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoDoSimpleRequest(window, simple);

    AROS_LIBFUNC_EXIT
}


AROS_LH1(int, FileRequest,
	AROS_LHA(struct DOpusFileReq *, freq, A0),
	struct Library *, DOpusBase, 5, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoFileRequest(freq);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(int, SendPacket,
	AROS_LHA(struct MsgPort *, port, A0),
	AROS_LHA(int , action, 		D0),
	AROS_LHA(IPTR * , args, 	A1),
	AROS_LHA(int , nargs, 		D1),
	struct Library *, DOpusBase, 36, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoSendPacket(port, action, args, nargs);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void *, LAllocRemember,
	AROS_LHA(struct DOpusRemember **, key, A0),
	AROS_LHA(ULONG, size, 	D0),
	AROS_LHA(ULONG, type, 	D1),
	struct Library *, DOpusBase, 48, DOpus)
{
    AROS_LIBFUNC_INIT

    return DoAllocRemember(key, size, type);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, LFreeRemember,
	AROS_LHA(struct DOpusRemember **, key, A0),
	struct Library *, DOpusBase, 49, DOpus)
{
    AROS_LIBFUNC_INIT

    DoFreeRemember(key);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, LFreeRemEntry,
	AROS_LHA(struct DOpusRemember **, key, A0),
	AROS_LHA(char *, pointer, A1),
	struct Library *, DOpusBase, 97, DOpus)
{
    AROS_LIBFUNC_INIT

    DoFreeRemEntry(key, pointer);

    AROS_LIBFUNC_EXIT
}


