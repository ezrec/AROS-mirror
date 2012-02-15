/*
** $Id$
*/

#ifdef MBX

#ifndef MININT
#define MININT		     0x80000000
#endif

#ifndef MAXINT
#define MAXINT		     0x7FFFFFFF
#endif

#define HANDLECHAR_NEW 1

#define CHARLEN(chr) UTF8CodeLen(chr)
#define PREVCHARLEN(chrptr) PrevUTF8CodeLen(chrptr)

#define SECRETCHAR 'x'
#define SECRETSTR  "x"

#define DEBLEVEL 0
#include <system/debug.h>
#include <audio_lib_calls.h>
#include "master.h" // from modules/mui_lib

#undef ihn // someone ?? defines ihn but textinput uses it as member name of struct Data
#define VERSION 42
#define REVISION 42
#define GS(x) "no locale!"
#undef DEBUG
#define DEBUG KPrintF
#define MUIMasterBase MUIBase
#define __inline 
#define cp_x rp_PenPos.X
#define cp_y rp_PenPos.Y
#define LinePtrn rp_LinePattern
#define DoSuperNew mmDoSuperNew
#define patextlen(fa,text,len) mbxpatextlen(data->font,text,len)
#define patextfit(fa,text,len,pix) textfit(data->font,text,len,pix)
#define ClassInitFunc TextinputInit
#define ClassExitFunc TextinputExit


#else // MBX

#define CHARLEN(chr) 1
#define PREVCHARLEN(chrptr) 1

#define SECRETCHAR '·'
#define SECRETSTR  "·"

#undef DEBUG
//#define DL(n,x) if (DEBLEVEL>=n) { x } 
//#define DEBUG KPrintF

#define ww_Flags Flags
#define im_MouseX MouseX
#define im_MouseY MouseY
#define im_Class Class
#define im_Code Code
#define im_Qualifier Qualifier
#define rect_MinX MinX
#define rect_MinY MinY
#define rect_MaxX MaxX
#define rect_MaxY MaxY

#define __USE_SYSBASE
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/keymap.h>
#include <proto/layers.h>
#include <proto/asl.h>
#include <proto/diskfont.h>
#include <proto/commodities.h>
#include <proto/timer.h>
#include <proto/iffparse.h>
#include <proto/locale.h>
#include <graphics/gfxbase.h>
#include <dos/dostags.h>
#include <libraries/gadtools.h>
#include <dos/notify.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <graphics/gfxmacros.h>
#include <cybergraphics/cybergraphics.h>

#define USE_BUILTIN_MATH
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "/vat/proto/vat.h"

#include "mui.h"
#include <mui/pophotkey_mcc.h>

#endif

#define INCSTRPTR(str) (str += CHARLEN(*str))

#include "lists.h"


typedef UBYTE BFLAG;

#define GETDATA struct Data *data = INST_DATA( cl, obj )
#define DOSUPER DoSuperMethodA(cl,obj,(Msg)msg)

#define DECMMETHOD(name) static ULONG handleMUIM_##name( struct IClass *cl,Object*obj,struct MUIP_##name *msg)
#define DEFMETHOD(mid) case mid:return(handle##mid(cl,obj,(APTR)msg));
#define DEFMMETHOD(mid) case MUIM_##mid:return(handleMUIM_##mid(cl,obj,(APTR)msg));
#define DECMMETHOD2(name) static ULONG handle2MUIM_##name( struct IClass *cl,Object*obj,struct MUIP_##name *msg)
#define DEFMETHOD2(mid) case mid:return(handle2##mid(cl,obj,(APTR)msg));
#define DEFMMETHOD2(mid) case MUIM_##mid:return(handle2MUIM_##mid(cl,obj,(APTR)msg));
#define DECCONST static ULONG handleOM_NEW( struct IClass *cl,Object*obj,struct opSet *msg )
#define DECSET static ULONG handleOM_SET( struct IClass *cl,Object*obj,struct opSet *msg )
#define DECGET static ULONG handleOM_GET( struct IClass *cl,Object*obj,struct opGet *msg )
#define DECDEST static ULONG handleOM_DISPOSE( struct IClass *cl,Object*obj,struct opSet *msg )
