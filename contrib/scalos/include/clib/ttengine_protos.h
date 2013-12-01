#ifndef CLIB_TTENGINE_PROTOS_H
#define CLIB_TTENGINE_PROTOS_H


/*
**  $VER: ttengine_protos.h 7.0 (13.01.2005)
**
**  C prototypes. For use with 32 bit integers only.
**
**  (c) Grzegorz Kraszewski 2003 - 2005
**  All Rights Reserved
*/

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef  LIBRARIES_TTENGINE_H
#include <libraries/ttengine.h>
#endif
#ifndef  GRAPHICS_TEXT_H
#include <graphics/text.h>
#endif
#ifndef  GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

APTR TT_OpenFontA(struct TagItem * taglist);
APTR TT_OpenFont(Tag tag1, ...);
BOOL TT_SetFont(struct RastPort * rp, APTR font);
void TT_CloseFont(APTR font);
void TT_Text(struct RastPort * rp, APTR string, ULONG count);
ULONG TT_SetAttrsA(struct RastPort * rp, struct TagItem * taglist);
ULONG TT_SetAttrs(struct RastPort * rp, Tag tag1, ...);
ULONG TT_GetAttrsA(struct RastPort * rp, struct TagItem * taglist);
ULONG TT_GetAttrs(struct RastPort * rp, Tag tag1, ...);
ULONG TT_TextLength(struct RastPort * rp, APTR string, ULONG count);
void TT_TextExtent(struct RastPort * rp, APTR string, WORD count, struct TextExtent * te);
ULONG TT_TextFit(struct RastPort * rp, APTR string, UWORD count, struct TextExtent * te,
  struct TextExtent * tec, WORD dir, UWORD cwidth, UWORD cheight);
struct TT_Pixmap * TT_GetPixmapA(APTR font, APTR string, ULONG count,
  struct TagItem * taglist);
struct TT_Pixmap * TT_GetPixmap(APTR font, APTR string, ULONG count, Tag tag1, ...);
void TT_FreePixmap(struct TT_Pixmap * pixmap);
void TT_DoneRastPort(struct RastPort * rp);
APTR TT_AllocRequest(void);
struct TagItem* TT_RequestA(APTR request, struct TagItem * taglist);
struct TagItem* TT_Request(APTR request, Tag tag1, ...);
void TT_FreeRequest(APTR request);
STRPTR * TT_ObtainFamilyListA(struct TagItem *taglist);
STRPTR * TT_ObtainFamilyList(Tag tag1, ...);
void TT_FreeFamilyList(STRPTR * list);

#endif  /*  CLIB_TTENGINE_PROTOS_H  */
