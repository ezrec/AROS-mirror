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

#include <ctype.h>
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
#include <intuition/sghooks.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <libraries/commodities.h>
#include <libraries/iffparse.h>
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
/*#include <proto/all.h>
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/asl.h>
#include <proto/dopus.h>


#include "dopusbase.h"

extern struct ExecBase *SysBase;
extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct DosLibrary *DOSBase;
extern struct Library *LayersBase;

/* dosstuff.c */

/* AROS: We have to get protos some other way than this */
#if 0

__asm __saveds DoAssign(register __a0 char *name,
	register __a1 char *dir);

char *__asm __saveds DoBaseName(register __a0 char *path);

__asm __saveds DoCompareLock(register __a0 BPTR l1,
	register __a1 BPTR l2);

__asm __saveds DoPathName(register __a0 BPTR lock,
	register __a1 char *buf,
	register __d0 int len);

__asm __saveds DoSendPacket(register __a0 struct MsgPort *port,
	register __d0 int action,
	register __a1 ULONG *args,
	register __d1 int nargs);

__asm __saveds DoTackOn(register __a0 char *path,
	register __a1 char *file,
	register __d0 int len);

__asm __saveds DoStampToStr(register __a0 struct DOpusDateTime *dt);

__asm __saveds DoStrToStamp(register __a0 struct DOpusDateTime *dt);

ULONG __asm __saveds DoAllocRemember(register __a0 struct DOpusRemember **key,
	register __d0 ULONG size,
	register __d1 ULONG type);

void __asm __saveds DoFreeRemember(register __a0 struct DOpusRemember **key);

void __asm __saveds DoFreeRemEntry(register __a0 struct DOpusRemember **key,
	register __a1 char *pointer);

__asm __saveds DoCompareDate(register __a0 struct DateStamp *date,
	register __a1 struct DateStamp *date2);

void __asm __saveds DoDoArrow(register __a0 struct RastPort *,
	register __d0 int,
	register __d1 int,
	register __d2 int,
	register __d3 int,
	register __d4 int,
	register __d5 int,
	register __d6 int);

__asm __saveds DoStrCombine(register __a0 char *buf,
	register __a1 char *one,
	register __a2 char *two,
	register __d0 int lim);

__asm __saveds DoStrConcat(register __a0 char *buf,
	register __a1 char *cat,
	register __d0 int lim);

void __asm __saveds DoDecode_RLE(register __a0 char *source,
	register __a1 char *dest,
	register __d0 int size);

/* dosstuff2.c */

__asm __saveds DoSearchPathList(register __a0 char *,
	register __a1 char *,
	register __d0 int);

__asm __saveds DoCheckExist(register __a0 char *,
	register __a1 int *);

__asm __saveds DoRawkeyToStr(register __d0 USHORT,
	register __d1 USHORT,
	register __a0 char *,
	register __a1 char *,
	register __d2 int);

__asm __saveds DoDoRMBGadget(register __a0 struct RMBGadget *,
	register __a1 struct Window *);

__asm __saveds DoAddGadgets(register __a0 struct Window *,
	register __a1 struct Gadget *,
	register __a2 char **,
	register __d0 int,
	register __d1 int,
	register __d2 int,
	register __d3 int);

__asm __saveds DoCheckNumGad(register __a0 struct Gadget *,
	register __a1 struct Window *,
	register __d0 int,
	register __d1 int);

__asm __saveds DoCheckHexGad(register __a0 struct Gadget *,
	register __a1 struct Window *,
	register __d0 int,
	register __d1 int);

__asm __saveds DoAtoh(register __a0 unsigned char *,
	register __d0 int);

__asm __saveds DoDoSimpleRequest(register __a0 struct Window *,
	register __a1 struct DOpusSimpleRequest *);

void __asm __saveds DoDoCycleGadget(register __a0 struct Gadget *,
	register __a1 struct Window *,
	register __a2 char **,
	register __d0 int);

void __asm __saveds DoDrawRadioButton(register __a0 struct RastPort *,
	register __d0 int,
	register __d1 int,
	register __d2 int,
	register __d3 int,
	register __d4 int,
	register __d5 int);

struct Image * __asm __saveds DoGetButtonImage(register __d0 int,
	register __d1 int,
	register __d2 int,
	register __d3 int,
	register __d4 int,
	register __d5 int,
	register __a0 struct DOpusRemember **);

struct Image *__asm __saveds DoGetCheckImage(register __d0 int fg,
	register __d1 int bg,
	register __d2 int pen,
	register __a0 struct DOpusRemember **key);

void __asm __saveds DoUScoreText(register __a0 struct RastPort *,
	register __a1 char *,
	register __d0 int,
	register __d1 int,
	register __d2 int);

void __asm __saveds DoDo3DFrame(register __a0 struct RastPort *,
	register __d0 int,
	register __d1 int,
	register __d2 int,
	register __d3 int,
	register __a1 char *,
	register __d4 int,
	register __d5 int);

void __asm __saveds DoDoGlassImage(register __a0 struct RastPort *rp,
	register __a1 struct Gadget *gadget,
	register __d0 int shine,
	register __d1 int shadow,
	register __d2 int type);

/* dosstuff3.c */

__asm __saveds DoReadConfig(register __a0 char *,
	register __a1 struct ConfigStuff *);

__asm __saveds DoSaveConfig(register __a0 char *,
	register __a1 struct ConfigStuff *);

__asm __saveds DoDefaultConfig(register __a0 struct ConfigStuff *);

__asm __saveds DoGetDevices(register __a0 struct ConfigStuff *);

void __asm __saveds DoAssignGadget(register __a0 struct ConfigStuff *,
	register __d0 int,
	register __d1 int,
	register __a1 char *,
	register __a2 char *);

void __asm __saveds DoAssignMenu(register __a0 struct ConfigStuff *,
	register __d0 int,
	register __a1 char *,
	register __a2 char *);

void __asm __saveds DoFreeConfig(register __a0 struct ConfigStuff *);

__asm __saveds DoCheckConfig(register __a0 struct ConfigStuff *);

__asm __saveds DoFindSystemFile(register __a0 char *,
	register __a1 char *,
	register __d0 int,
	register __d1 int);

/* filereq.c */

__asm __saveds DoFileRequest(register __a0 struct DOpusFileReq *);

void __asm __saveds DoSetBusyPointer(register __a0 struct Window *);

/* listview.c */

__asm __saveds DoAddListView(register __a0 struct DOpusListView *,
	register __d0 int);

void __stdargs __saveds DoFixSliderBody(struct Window *,
	struct Gadget *,
	int,
	int,
	int);

void __stdargs __saveds DoFixSliderPot(struct Window *,
	struct Gadget *,
	int,
	int,
	int,
	int);

void __asm __saveds DoShowSlider(register __a0 struct Window *,
	register __a1 struct Gadget *);

__stdargs __saveds DoGetSliderPos(struct Gadget *,
	int,
	int);

struct DOpusListView *__asm __saveds DoListViewIDCMP(
	register __a0 struct DOpusListView *,
	register __a1 struct IntuiMessage *);

__asm __saveds DoRefreshListView(register __a0 struct DOpusListView *,
	register __d0 int);

__asm __saveds DoRemoveListView(register __a0 struct DOpusListView *,
	register __d0 int);

/* wildcard.c */

__stdargs __saveds DoMatchPattern(unsigned char *,
	unsigned char *,
	int);

void __stdargs __saveds DoParsePattern(unsigned char *,
	unsigned char *,
	int);

/* requesters.c */

struct Window *__asm __saveds R_OpenRequester(
	register __a0 struct RequesterBase *);

void __asm __saveds R_CloseRequester(
	register __a0 struct RequesterBase *);

APTR __asm __saveds R_AddRequesterObject(
	register __a0 struct RequesterBase *,
	register __a1 struct TagItem *);

void __asm __saveds R_RefreshRequesterObject(
	register __a0 struct RequesterBase *,
	register __a1 struct RequesterObject *);

void __stdargs __saveds R_ObjectText(
	struct RequesterBase *,
	short,
	short,
	short,
	short,
	char *,
	unsigned short);

/* language.c */

__asm __saveds DoReadStringFile(
	register __a0 struct StringData *,
	register __a1 char *);

void __asm __saveds DoFreeStringFile(
	register __a0 struct StringData *);

/* borders.c */

void __asm __saveds DoAddGadgetBorders(register __a0 struct DOpusRemember **,
	register __a1 struct Gadget *,
	register __d0 int,
	register __d1 int,
	register __d2 int);

void __asm __saveds DoCreateGadgetBorders(register __a0 struct DOpusRemember **,
	register __d0 int,
	register __d1 int,
	register __a1 struct Border **,
	register __a2 struct Border **,
	register __d2 int,
	register __d3 int,
	register __d4 int);

void __asm __saveds DoSelectGadget(register __a0 struct Window *,
	register __a1 struct Gadget *);

/* menus.c */

__asm __saveds DoFSSetMenuStrip(register __a0 struct Window *,
	register __a1 struct Menu *);

#endif
