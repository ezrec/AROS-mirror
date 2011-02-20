#ifndef _INLINE_BGUI_H
#define _INLINE_BGUI_H

#ifndef __INLINE_STUB_H
#include <inline/stubs.h>
#endif

#ifndef BASE_EXT_DECL
#define BASE_EXT_DECL
#define BASE_EXT_DECL0 extern struct Library *BGUIBase;
#endif
#ifndef BASE_PAR_DECL
#define BASE_PAR_DECL
#define BASE_PAR_DECL0 void
#endif
#ifndef BASE_NAME
#define BASE_NAME BGUIBase
#endif

BASE_EXT_DECL0

__inline Class *
BGUI_GetClassPtr(BASE_PAR_DECL ULONG num)
{
   BASE_EXT_DECL
   register Class * res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register ULONG d0 __asm("d0") = num;
   __asm volatile ("jsr a6@(-0x1E:W)"
   : "=r" (res)
   : "r" (a6), "r" (d0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline Object *
BGUI_NewObjectA(BASE_PAR_DECL ULONG num, struct TagItem * tags)
{
   BASE_EXT_DECL
   register Object * res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register ULONG d0 __asm("d0") = num;
   register struct TagItem * a0 __asm("a0") = tags;
   __asm volatile ("jsr a6@(-0x24:W)"
   : "=r" (res)
   : "r" (a6), "r" (d0), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

Object * BGUI_NewObject(ULONG num, Tag tags, ...)
{
  return BGUI_NewObjectA(num, (struct TagItem *) &tags);
}

__inline ULONG
BGUI_RequestA(BASE_PAR_DECL struct Window * win, struct bguiRequest * estruct, ULONG * args)
{
   BASE_EXT_DECL
   register ULONG res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct Window * a0 __asm("a0") = win;
   register struct bguiRequest * a1 __asm("a1") = estruct;
   register ULONG * a2 __asm("a2") = args;
   __asm volatile ("jsr a6@(-0x2A:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline BOOL
BGUI_Help(BASE_PAR_DECL struct Window * win, UBYTE * name, UBYTE * node, ULONG line)
{
   BASE_EXT_DECL
   register BOOL res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct Window * a0 __asm("a0") = win;
   register UBYTE * a1 __asm("a1") = name;
   register UBYTE * a2 __asm("a2") = node;
   register ULONG d0 __asm("d0") = line;
   __asm volatile ("jsr a6@(-0x30:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2), "r" (d0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline APTR
BGUI_LockWindow(BASE_PAR_DECL struct Window * win)
{
   BASE_EXT_DECL
   register APTR res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct Window * a0 __asm("a0") = win;
   __asm volatile ("jsr a6@(-0x36:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline VOID
BGUI_UnlockWindow(BASE_PAR_DECL APTR lock)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register APTR a0 __asm("a0") = lock;
   __asm volatile ("jsr a6@(-0x3C:W)"
   : /* No Output */
   : "r" (a6), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline ULONG
BGUI_DoGadgetMethodA(BASE_PAR_DECL Object * object, struct Window * win, struct Requester * req, Msg msg)
{
   BASE_EXT_DECL
   register ULONG res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register Object * a0 __asm("a0") = object;
   register struct Window * a1 __asm("a1") = win;
   register struct Requester * a2 __asm("a2") = req;
   register Msg a3 __asm("a3") = msg;
   __asm volatile ("jsr a6@(-0x42:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2), "r" (a3)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

ULONG BGUI_DoGadgetMethod(Object * object, struct Window * win, struct Requester * req, ULONG msg, ...)
{
  return BGUI_DoGadgetMethodA(object, win, req, (Msg) &msg);
}

__inline struct BitMap *
BGUI_AllocBitMap(BASE_PAR_DECL ULONG width, ULONG height, ULONG depth, ULONG flags, struct BitMap * friend)
{
   BASE_EXT_DECL
   register struct BitMap * res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register ULONG d0 __asm("d0") = width;
   register ULONG d1 __asm("d1") = height;
   register ULONG d2 __asm("d2") = depth;
   register ULONG d3 __asm("d3") = flags;
   register struct BitMap * a0 __asm("a0") = friend;
   __asm volatile ("jsr a6@(-0x54:W)"
   : "=r" (res)
   : "r" (a6), "r" (d0), "r" (d1), "r" (d2), "r" (d3), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline VOID
BGUI_FreeBitMap(BASE_PAR_DECL struct BitMap * bitmap)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct BitMap * a0 __asm("a0") = bitmap;
   __asm volatile ("jsr a6@(-0x5A:W)"
   : /* No Output */
   : "r" (a6), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline struct RastPort *
BGUI_CreateRPortBitMap(BASE_PAR_DECL struct RastPort * rport, ULONG width, ULONG height, ULONG depth)
{
   BASE_EXT_DECL
   register struct RastPort * res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct RastPort * a0 __asm("a0") = rport;
   register ULONG d0 __asm("d0") = width;
   register ULONG d1 __asm("d1") = height;
   register ULONG d2 __asm("d2") = depth;
   __asm volatile ("jsr a6@(-0x60:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (d0), "r" (d1), "r" (d2)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline VOID
BGUI_FreeRPortBitMap(BASE_PAR_DECL struct RastPort * rport)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct RastPort * a0 __asm("a0") = rport;
   __asm volatile ("jsr a6@(-0x66:W)"
   : /* No Output */
   : "r" (a6), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline VOID
BGUI_InfoTextSize(BASE_PAR_DECL struct RastPort * rp, UBYTE * text, UWORD * width, UWORD * height)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct RastPort * a0 __asm("a0") = rp;
   register UBYTE * a1 __asm("a1") = text;
   register UWORD * a2 __asm("a2") = width;
   register UWORD * a3 __asm("a3") = height;
   __asm volatile ("jsr a6@(-0x6C:W)"
   : /* No Output */
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2), "r" (a3)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline VOID
BGUI_InfoText(BASE_PAR_DECL struct RastPort * rp, UBYTE * text, struct IBox * bounds, struct DrawInfo * drawinfo)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct RastPort * a0 __asm("a0") = rp;
   register UBYTE * a1 __asm("a1") = text;
   register struct IBox * a2 __asm("a2") = bounds;
   register struct DrawInfo * a3 __asm("a3") = drawinfo;
   __asm volatile ("jsr a6@(-0x72:W)"
   : /* No Output */
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2), "r" (a3)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline STRPTR
BGUI_GetLocaleStr(BASE_PAR_DECL struct bguiLocale * bl, ULONG id)
{
   BASE_EXT_DECL
   register STRPTR res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct bguiLocale * a0 __asm("a0") = bl;
   register ULONG d0 __asm("d0") = id;
   __asm volatile ("jsr a6@(-0x78:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (d0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline STRPTR
BGUI_GetCatalogStr(BASE_PAR_DECL struct bguiLocale * bl, ULONG id, STRPTR def)
{
   BASE_EXT_DECL
   register STRPTR res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct bguiLocale * a0 __asm("a0") = bl;
   register ULONG d0 __asm("d0") = id;
   register STRPTR a1 __asm("a1") = def;
   __asm volatile ("jsr a6@(-0x7E:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (d0), "r" (a1)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline VOID
BGUI_FillRectPattern(BASE_PAR_DECL struct RastPort * rport, struct bguiPattern * bpat, LONG x1, LONG y1, LONG x2, LONG y2)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct RastPort * a1 __asm("a1") = rport;
   register struct bguiPattern * a0 __asm("a0") = bpat;
   register LONG d0 __asm("d0") = x1;
   register LONG d1 __asm("d1") = y1;
   register LONG d2 __asm("d2") = x2;
   register LONG d3 __asm("d3") = y2;
   __asm volatile ("jsr a6@(-0x84:W)"
   : /* No Output */
   : "r" (a6), "r" (a1), "r" (a0), "r" (d0), "r" (d1), "r" (d2), "r" (d3)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline VOID
BGUI_PostRender(BASE_PAR_DECL Class * cl, Object * obj, struct gpRender * gpr)
{
   BASE_EXT_DECL
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register Class * a0 __asm("a0") = cl;
   register Object * a2 __asm("a2") = obj;
   register struct gpRender * a1 __asm("a1") = gpr;
   __asm volatile ("jsr a6@(-0x8A:W)"
   : /* No Output */
   : "r" (a6), "r" (a0), "r" (a2), "r" (a1)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
}

__inline Class *
BGUI_MakeClassA(BASE_PAR_DECL struct TagItem * tags)
{
   BASE_EXT_DECL
   register Class * res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register struct TagItem * a0 __asm("a0") = tags;
   __asm volatile ("jsr a6@(-0x90:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

Class * BGUI_MakeClass(ULONG tags, ...)
{
  return BGUI_MakeClassA((struct TagItem *) &tags);
}

__inline BOOL
BGUI_FreeClass(BASE_PAR_DECL Class * cl)
{
   BASE_EXT_DECL
   register BOOL res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register Class * a0 __asm("a0") = cl;
   __asm volatile ("jsr a6@(-0x96:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline ULONG
BGUI_PackStructureTags(BASE_PAR_DECL APTR pack, ULONG * tab, struct TagItem * tags)
{
   BASE_EXT_DECL
   register ULONG res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register APTR a0 __asm("a0") = pack;
   register ULONG * a1 __asm("a1") = tab;
   register struct TagItem * a2 __asm("a2") = tags;
   __asm volatile ("jsr a6@(-0x9C:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

__inline ULONG
BGUI_UnpackStructureTags(BASE_PAR_DECL APTR pack, ULONG * tab, struct TagItem * tags)
{
   BASE_EXT_DECL
   register ULONG res __asm("d0");
   register struct Library *a6 __asm("a6") = BASE_NAME;
   register APTR a0 __asm("a0") = pack;
   register ULONG * a1 __asm("a1") = tab;
   register struct TagItem * a2 __asm("a2") = tags;
   __asm volatile ("jsr a6@(-0xA2:W)"
   : "=r" (res)
   : "r" (a6), "r" (a0), "r" (a1), "r" (a2)
   : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
   return res;
}

#undef BASE_EXT_DECL
#undef BASE_EXT_DECL0
#undef BASE_PAR_DECL
#undef BASE_PAR_DECL0
#undef BASE_NAME

#endif /*  _INLINE_BGUI_H  */
