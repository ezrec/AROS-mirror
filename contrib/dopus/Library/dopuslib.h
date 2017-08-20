#ifndef _DOPUSLIB_H
#define _DOPUSLIB_H

/****************************************************************

   This file was created automatically by `FlexCat 2.4'
   from "../catalogs/dopus4_lib.cd".

   Do NOT edit by hand!

****************************************************************/

/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#ifdef DEBUG
  #include <debug.h>
  #define bug kprintf
  #define D(x) x
#else
  #define D(x)
#endif

#if defined(__PPC__) || defined(__AROS__)
  #undef  __saveds
  #define __saveds
  #define __chip
  #define __aligned __attribute__((__aligned__(4)))
  #define lsprintf sprintf
  #define __asm(A)
  #define __stdargs
  #define __regargs
#endif

//#include <fctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
/*
#include <dos/exall.h>
*/
#include <intuition/intuitionbase.h>
#include <intuition/iobsolete.h>
#include <intuition/sghooks.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <libraries/commodities.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
/*
#include <devices/audio.h>
#include <devices/input.h>
#include <devices/keyboard.h>
#include <devices/trackdisk.h>
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <devices/conunit.h>
#include <hardware/intbits.h>
#include <datatypes/datatypesclass.h>
*/
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/console.h>
#include <proto/locale.h>
#include <clib/alib_protos.h>

#include <dopus/dopusbase.h>
#include <dopus/config.h>
#include <dopus/configflags.h>
#include <dopus/requesters.h>
#include <dopus/stringdata.h>

extern struct ExecBase *SysBase;
extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct DosLibrary *DOSBase;
extern struct Library *LayersBase;

/* dosstuff.c */

int __saveds DoAssign(register char *name __asm("a0"),
    register char *dir __asm("a1"));

char *__saveds DoBaseName(register char *path __asm("a0"));

int __saveds DoCompareLock(register BPTR l1 __asm("a0"),
    register BPTR l2 __asm("a1"));

int __saveds DoPathName(register BPTR lock __asm("a0"),
    register char *buf __asm("a1"),
    register int len __asm("d0"));

int __saveds DoSendPacket(register struct MsgPort *port __asm("a0"),
    register int action __asm("d0"),
    register IPTR *args __asm("a1"),
    register int nargs __asm("d1"));

int __saveds DoTackOn(register char *path __asm("a0"),
    register char *file __asm("a1"),
    register int len __asm("d0"));

void __saveds DoStampToStr(register struct DOpusDateTime *dt __asm("a0"));

int __saveds DoStrToStamp(register struct DOpusDateTime *dt __asm("a0"));

APTR __saveds DoAllocRemember(register struct DOpusRemember **key __asm("a0"),
    register ULONG size __asm("d0"),
    register ULONG type __asm("d1"));

void __saveds DoFreeRemember(register struct DOpusRemember **key __asm("a0"));

void __saveds DoFreeRemEntry(register struct DOpusRemember **key __asm("a0"),
    register char *pointer __asm("a1"));

int __saveds DoCompareDate(register struct DateStamp *date __asm("a0"),
    register struct DateStamp *date2 __asm("a1"));

void __saveds DoDoArrow(register struct RastPort * __asm("a0"),
    register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"),
    register int __asm("d3"),
    register int __asm("d4"),
    register int __asm("d5"),
    register int __asm("d6"));

int __saveds DoStrCombine(register char *buf __asm("a0"),
    register const char *one __asm("a1"),
    register const char *two __asm("a2"),
    register int lim __asm("d0"));

int __saveds DoStrConcat(register char *buf __asm("a0"),
    register const char *cat __asm("a1"),
    register int lim __asm("d0"));

void __saveds DoDecode_RLE(register char *source __asm("a0"),
    register char *dest __asm("a1"),
    register int size __asm("d0"));

/* dosstuff2.c */

int __saveds DoSearchPathList(register char * __asm("a0"),
    register char * __asm("a1"),
    register int __asm("d0"));

int __saveds DoCheckExist(register char * __asm("a0"),
    register int * __asm("a1"));

int __saveds DoRawkeyToStr(register UWORD __asm("d0"),
    register UWORD __asm("d1"),
    register char * __asm("a0"),
    register char * __asm("a1"),
    register int __asm("d2"));

int __saveds DoDoRMBGadget(register struct RMBGadget * __asm("a0"),
    register struct Window * __asm("a1"));

int __saveds DoAddGadgets(register struct Window * __asm("a0"),
    register struct Gadget * __asm("a1"),
    register char ** __asm("a2"),
    register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"),
    register int __asm("d3"));

int __saveds DoCheckNumGad(register struct Gadget * __asm("a0"),
    register struct Window * __asm("a1"),
    register int __asm("d0"),
    register int __asm("d1"));

int __saveds DoCheckHexGad(register struct Gadget * __asm("a0"),
    register struct Window * __asm("a1"),
    register int __asm("d0"),
    register int __asm("d1"));

int __saveds DoAtoh(register unsigned char * __asm("a0"),
    register int __asm("d0"));

int __saveds DoDoSimpleRequest(register struct Window * __asm("a0"),
    register struct DOpusSimpleRequest * __asm("a1"));

void __saveds DoDoCycleGadget(register struct Gadget * __asm("a0"),
    register struct Window * __asm("a1"),
    register char ** __asm("a2"),
    register int __asm("d0"));

void __saveds DoDrawRadioButton(register struct RastPort * __asm("a0"),
    register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"),
    register int __asm("d3"),
    register int __asm("d4"),
    register int __asm("d5"));

struct Image * __saveds DoGetButtonImage(register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"),
    register int __asm("d3"),
    register int __asm("d4"),
    register int __asm("d5"),
    register struct DOpusRemember ** __asm("a0"));

struct Image *__saveds DoGetCheckImage(register UBYTE fg __asm("d0"),
    register UBYTE bg __asm("d1"),
    register int pen __asm("d2"),
    register struct DOpusRemember **key __asm("a0"));

void __saveds DoUScoreText(register struct RastPort * __asm("a0"),
    register char * __asm("a1"),
    register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"));

void __saveds DoDo3DFrame(register struct RastPort * __asm("a0"),
    register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"),
    register int __asm("d3"),
    register char * __asm("a1"),
    register int __asm("d4"),
    register int __asm("d5"));

void __saveds DoDoGlassImage(register struct RastPort *rp __asm("a0"),
    register struct Gadget *gadget __asm("a1"),
    register int shine __asm("d0"),
    register int shadow __asm("d1"),
    register int type __asm("d2"));

/* dosstuff3.c */

int __saveds DoReadConfig(register char * __asm("a0"),
    register struct ConfigStuff * __asm("a1"));

int __saveds DoSaveConfig(register char * __asm("a0"),
    register struct ConfigStuff * __asm("a1"));

int __saveds DoDefaultConfig(register struct ConfigStuff * __asm("a0"));

int __saveds DoGetDevices(register struct ConfigStuff * __asm("a0"));

void __saveds DoAssignGadget(register struct ConfigStuff * __asm("a0"),
    register int __asm("d0"),
    register int __asm("d1"),
    register const char * __asm("a1"),
    register const char * __asm("a2"));

void __saveds DoAssignMenu(register struct ConfigStuff * __asm("a0"),
    register int __asm("d0"),
    register const char * __asm("a1"),
    register const char * __asm("a2"));

void __saveds DoFreeConfig(register struct ConfigStuff * __asm("a0"));

int __saveds DoCheckConfig(register struct ConfigStuff * __asm("a0"));

int __saveds DoFindSystemFile(register char * __asm("a0"),
    register char * __asm("a1"),
    register int __asm("d0"),
    register int __asm("d1"));

/* filereq.c */

int __saveds DoFileRequest(register struct DOpusFileReq * __asm("a0"));

void __saveds DoSetBusyPointer(register struct Window * __asm("a0"));

/* listview.c */

int __saveds DoAddListView(register struct DOpusListView * __asm("a0"),
    register int __asm("d0"));

void __saveds DoFixSliderBody(register struct Window *win __asm("a0"),
  register struct Gadget *gad __asm("a1"),
  register int count __asm("d0"),
  register int lines __asm("d1"),
  register int show __asm("d2"));

void __saveds DoFixSliderPot(register struct Window *win __asm("a0"),
  register struct Gadget *gad __asm("a1"),
  register int off __asm("d0"),
  register int count __asm("d1"),
  register int lines __asm("d2"),
  register int show __asm("d3"));

void __saveds DoShowSlider(register struct Window * __asm("a0"),
    register struct Gadget * __asm("a1"));

int __saveds DoGetSliderPos(register struct Gadget *gad __asm("a0"),
  register int count __asm("d0"),
  register int lines __asm("d1"));

struct DOpusListView *__saveds DoListViewIDCMP(
    register struct DOpusListView * __asm("a0"),
    register struct IntuiMessage * __asm("a1"));

int __saveds DoRefreshListView(register struct DOpusListView * __asm("a0"),
    register int __asm("d0"));

int __saveds DoRemoveListView(register struct DOpusListView * __asm("a0"),
    register int __asm("d0"));

/* wildcard.c */


int __saveds DoMatchPattern(register unsigned char *pat __asm("a0"),
    register unsigned char *str __asm("a1"),
    register int cas __asm("d0"));

void __saveds DoParsePattern(register unsigned char *pat __asm("a0"),
    register unsigned char *patbuf __asm("a1"),
    register int cas __asm("d0"));

/* requesters.c */

struct Window *__saveds R_OpenRequester(
    register struct RequesterBase * __asm("a0"));

void __saveds R_CloseRequester(
    register struct RequesterBase * __asm("a0"));

APTR __saveds R_AddRequesterObject(
    register struct RequesterBase * __asm("a0"),
    register struct TagItem * __asm("a1"));

void __saveds R_RefreshRequesterObject(
    register struct RequesterBase * __asm("a0"),
    register struct RequesterObject * __asm("a1"));

void __saveds R_ObjectText(register struct RequesterBase *reqbase __asm("a0"),
    register short left __asm("d0"),
    register short top __asm("d1"),
    register short width __asm("d2"),
    register short height __asm("d3"),
    register char *text __asm("a1"),
    register unsigned short textpos __asm("d4"));

/* language.c */

int __saveds DoReadStringFile(
    register struct StringData * __asm("a0"),
    register char * __asm("a1"));

void __saveds DoFreeStringFile(
    register struct StringData * __asm("a0"));

/* borders.c */

void __saveds DoAddGadgetBorders(register struct DOpusRemember ** __asm("a0"),
    register struct Gadget * __asm("a1"),
    register int __asm("d0"),
    register int __asm("d1"),
    register int __asm("d2"));

void __saveds DoCreateGadgetBorders(register struct DOpusRemember ** __asm("a0"),
    register int __asm("d0"),
    register int __asm("d1"),
    register struct Border ** __asm("a1"),
    register struct Border ** __asm("a2"),
    register int __asm("d2"),
    register int __asm("d3"),
    register int __asm("d4"));

void __saveds DoSelectGadget(register struct Window * __asm("a0"),
    register struct Gadget * __asm("a1"));

/* menus.c */

int __saveds DoFSSetMenuStrip(register struct Window * __asm("a0"),
    register struct Menu * __asm("a1"));

/* functions.a */

void GetWBScreen(register struct Screen *screen __asm("a0"));
void Seed(register int seed __asm("d0"));
UWORD Random(register UWORD limit __asm("d0"));
void BtoCStr(register BSTR bstr __asm("a0"), register char *cstr __asm("a1"), register int len __asm("d0"));
void SwapMem(register char *src __asm("a0"), register char *dst __asm("a1"), register int size __asm("d0"));
__stdargs void LSprintf(char *buf, char *fmt, ...);
unsigned char LToUpper(register unsigned char c __asm("d0"));
unsigned char LToLower(register unsigned char c __asm("d0"));
void StrToUpper(register unsigned char *src __asm("a0"), register unsigned char *dst __asm("a1"));
void StrToLower(register unsigned char *src __asm("a0"), register unsigned char *dst __asm("a1"));
void LStrnCat(register char *s1 __asm("a0"), register char *s2 __asm("a1"), register int len __asm("d0"));
void LStrCat(register char *s1 __asm("a0"), register char *s2 __asm("a1"));
void LStrCpy(register char *dst __asm("a0"), register const char *src __asm("a1"));
void LStrnCpy(register char *dst __asm("a0"), register const char *src __asm("a1"), register int len __asm("d0"));
int LStrnCmpI(register char *s1 __asm("a0"), register char *s2 __asm("a1"), register int len __asm("d0"));
int LStrCmpI(register char *s1 __asm("a0"), register char *s2 __asm("a1"));
int LStrnCmp(register char *s1 __asm("a0"), register char *s2 __asm("a1"), register int len __asm("d0"));
int LStrCmp(register char *s1 __asm("a0"), register char *s2 __asm("a1"));

void Do3DBox(register struct RastPort *rp __asm("a0"),register int x __asm("d0"),register int y __asm("d1"),register int w __asm("d2"),register int h __asm("d3"),register int tp __asm("d4"),register int bp __asm("d5"));
void Do3DStringBox(register struct RastPort *rp __asm("a0"),register int x __asm("d0"),register int y __asm("d1"),register int w __asm("d2"),register int h __asm("d3"),register int tp __asm("d4"),register int bp __asm("d5"));
void Do3DCycleBox(register struct RastPort *rp __asm("a0"),register int x __asm("d0"),register int y __asm("d1"),register int w __asm("d2"),register int h __asm("d3"),register int tp __asm("d4"),register int bp __asm("d5"));
void DrawCheckMark(register struct RastPort *rp __asm("a0"),register int x __asm("d0"),register int y __asm("d1"),register int clear __asm("d2"));

void LParsePattern(register unsigned char *pat __asm("a0"),register unsigned char *patbuf __asm("a1"));
void LParsePatternI(register unsigned char *pat __asm("a0"),register unsigned char *patbuf __asm("a1"));
int LMatchPattern(register unsigned char *pat __asm("a0"), register unsigned char *str __asm("a1"));
int LMatchPatternI(register unsigned char *pat __asm("a0"), register unsigned char *str __asm("a1"));

void GhostGadget(register struct Gadget *gad __asm("a0"), register struct RastPort *rp __asm("a1"), register int xoff __asm("d0"), register int yoff __asm("d1"));
void HiliteGad(register struct Gadget *gad __asm("a0"), register struct RastPort *rp __asm("a1"));
void ActivateStrGad(register struct Gadget *gad __asm("a0"), register struct Window *win __asm("a1"));
void RefreshStrGad(register struct Gadget *gad __asm("a0"), register struct Window *win __asm("a1"));
void DisableGadget(register struct Gadget *gad __asm("a0"), register struct RastPort *rp __asm("a1"), register int xoff __asm("d0"), register int yoff __asm("d1"));
void EnableGadget(register struct Gadget *gad __asm("a0"), register struct RastPort *rp __asm("a1"), register int xoff __asm("d0"), register int yoff __asm("d1"));

struct IORequest *DoCreateExtIO(register struct MsgPort *port __asm("a0"), register int size __asm("d0"));
void DoDeleteExtIO(register struct IORequest *extio __asm("a0"));
struct MsgPort *DoCreatePort(register char *name __asm("a0"), register int pri __asm("d0"));
void DoDeletePort(register struct MsgPort *port __asm("a0"));

#define STRING_VERSION 1

enum {
    STR_CONFIG_TOO_OLD=0,
    STR_CONTINUE=1,
    STR_REQUEST=2,
    STR_TRY_AGAIN=3,
    STR_CANCEL=4,
    STR_CANNOT_FIND_FILE_REQ=5,
    STR_FILEREQ_FILE=6,
    STR_FILEREQ_DRAWER=7,
    STR_FILEREQ_ACCEPT=8,
    STR_FILEREQ_PARENT=9,
    STR_FILEREQ_DRIVES=10,
    STR_FILEREQ_DIR=11,
    STR_FILEREQ_NAME=12,
    STR_DEV_DEV=13,
    STR_DEV_ASN=14,
    STR_DEV_VOL=15,
    STR_DEV_DFR=16,
    STR_DEV_NBD=17,
    STR_SELECT_DIR=18,
    STR_SELECT_FILE=19,

    STR_STRING_COUNT
};

extern char **string_table;

void initstrings(void);

#endif
