extern struct GTLayoutBase * GTLayoutBase;

#define REG(X)

#include <exec/types.h>
#include <intuition/intuition.h>

#include "gtlayout_defines_aros.h"
//#include "../gtlayout_internal.h"

struct LayerMsg;


#define CATCOMP_NUMBERS

#define ASM
#define SAVE_DS
#define LIBENT

#define __regargs
#define __asm
#define __saveds
#define __aligned
#define __far
#define __chip
#define __stdargs

#define register

#define putreg(X,Y)


#define a0
#define a1
#define a2
#define a3
#define a4
#define a5
#define a6
#define a7 

#define d0
#define d1
#define d2
#define d3
#define d4
#define d5
#define d6
#define d7
 
 
#if 0
 VOID GTLayout_LT_LevelWidth( struct LayoutHandle *handle, STRPTR levelFormat, APTR dispFunc, LONG min, LONG max, LONG *maxWidth, LONG *maxLen, LONG fullCheck );
 VOID GTLayout_LT_DeleteHandle( struct LayoutHandle *handle );
 struct LayoutHandle *GTLayout_LT_CreateHandle( struct Screen *screen, struct TextAttr *font );
 struct LayoutHandle *GTLayout_LT_CreateHandleTagList( struct Screen *screen, struct TagItem *tagList );
#endif
 struct LayoutHandle *GTLayout_LT_CreateHandleTags( struct Screen *screen, ... );
#if 0
 BOOL GTLayout_LT_Rebuild( struct LayoutHandle *handle, struct IBox *bounds, LONG extraWidth, LONG extraHeight, LONG clear );
 VOID GTLayout_LT_HandleInput( struct LayoutHandle *handle, ULONG msgQualifier, ULONG *msgClass, UWORD *msgCode, struct Gadget **msgGadget );
 VOID GTLayout_LT_BeginRefresh( struct LayoutHandle *handle );
 VOID GTLayout_LT_EndRefresh( struct LayoutHandle *handle, LONG complete );
 LONG GTLayout_LT_GetAttributesA( struct LayoutHandle *handle, LONG id, struct TagItem *tagList );
#endif
 LONG GTLayout_LT_GetAttributes( struct LayoutHandle *handle, LONG id, ... );
#if 0
 VOID GTLayout_LT_SetAttributesA( struct LayoutHandle *handle, LONG id, struct TagItem *tagList );
#endif
 VOID GTLayout_LT_SetAttributes( struct LayoutHandle *handle, LONG id, ... );
#if 0
 VOID GTLayout_LT_AddA( struct LayoutHandle *handle, LONG type, STRPTR label, LONG id, struct TagItem *tagList );
#endif
 VOID GTLayout_LT_Add( struct LayoutHandle *handle, LONG type, STRPTR label, LONG id, ... );
#if 0
 VOID GTLayout_LT_NewA( struct LayoutHandle *handle, struct TagItem *tagList );
#endif
 VOID LT_New( struct LayoutHandle *handle, ... );
#if 0
 VOID GTLayout_LT_EndGroup( struct LayoutHandle *handle );
 struct Window *GTLayout_LT_LayoutA( struct LayoutHandle *handle, STRPTR title, struct IBox *bounds, LONG extraWidth, LONG extraHeight, ULONG idcmp, LONG align, struct TagItem *tagParams );
#endif
 struct Window *LT_Layout( struct LayoutHandle *handle, STRPTR title, struct IBox *bounds, LONG extraWidth, LONG extraHeight, ULONG idcmp, LONG align, ... );
#if 0
 struct Menu *GTLayout_LT_LayoutMenusA( struct LayoutHandle *handle, struct NewMenu *menuTemplate, struct TagItem *tagParams );
#endif
// struct Menu *LT_LayoutMenus( struct LayoutHandle *handle, struct NewMenu *menuTemplate, ... );
#if 0
 LONG GTLayout_LT_LabelWidth( struct LayoutHandle *handle, STRPTR label );
 LONG GTLayout_LT_LabelChars( struct LayoutHandle *handle, STRPTR label );
 VOID GTLayout_LT_LockWindow( struct Window *window );
 VOID GTLayout_LT_UnlockWindow( struct Window *window );
 VOID GTLayout_LT_DeleteWindowLock( struct Window *window );
 VOID GTLayout_LT_ShowWindow( struct LayoutHandle *handle, LONG activate );
 VOID GTLayout_LT_Activate( struct LayoutHandle *handle, LONG id );
 BOOL GTLayout_LT_PressButton( struct LayoutHandle *handle, LONG id );
 LONG GTLayout_LT_GetCode( ULONG msgQualifier, ULONG msgClass, ULONG msgCode, struct Gadget *msgGadget );
/*--- Added in v1.78 --------------------------------------------------*/
 struct IntuiMessage *GTLayout_LT_GetIMsg( struct LayoutHandle *handle );
 VOID GTLayout_LT_ReplyIMsg( struct IntuiMessage *msg );
/*--- Added in v3.0 ---------------------------------------------------*/
 struct Window *GTLayout_LT_BuildA( struct LayoutHandle *handle, struct TagItem *tagParams );
#endif
 struct Window *LT_Build( struct LayoutHandle *handle, ... );
#if 0
 BOOL GTLayout_LT_RebuildTagList( struct LayoutHandle *handle, LONG clear, struct TagItem *tags );
#endif
// BOOL LT_RebuildTags( struct LayoutHandle *handle, LONG clear, ... );
#if 0
/*--- Added in v9.0 ---------------------------------------------------*/
 VOID GTLayout_LT_UpdateStrings( struct LayoutHandle *handle );
/*--- Added in v11.0 ---------------------------------------------------*/
 VOID GTLayout_LT_DisposeMenu( struct Menu *menu );
 struct Menu *GTLayout_LT_NewMenuTemplate( struct Screen *screen, struct TextAttr *textAttr, struct Image *amigaGlyph, struct Image *checkGlyph, LONG *error, struct NewMenu *menuTemplate );
 struct Menu *GTLayout_LT_NewMenuTagList( struct TagItem *tagList );
#endif
 struct Menu *LT_NewMenuTags( Tag firstTag, ... );
#if 0
 VOID GTLayout_LT_MenuControlTagList( struct Window *window, struct Menu *intuitionMenu, struct TagItem *tags );
#endif
 VOID LT_MenuControlTags( struct Window *window, struct Menu *intuitionMenu, ... );
#if 0
 struct MenuItem *GTLayout_LT_GetMenuItem( struct Menu *menu, ULONG id );
 struct MenuItem *GTLayout_LT_FindMenuCommand( struct Menu *menu, ULONG msgCode, ULONG msgQualifier, struct Gadget *msgGadget );
/*--- Added in v14.0 ---------------------------------------------------*/
 VOID GTLayout_LT_NewLevelWidth( struct LayoutHandle *handle, STRPTR levelFormat, APTR dispFunc, LONG min, LONG max, LONG *maxWidth, LONG *maxLen, LONG fullCheck );
/*--- Added in v31.0 ---------------------------------------------------*/
 VOID GTLayout_LT_Refresh( struct LayoutHandle *handle );
/*--- Added in v34.0 ---------------------------------------------------*/
 VOID GTLayout_LT_CatchUpRefresh( struct LayoutHandle *handle );
/*--- Added in v39.0 ---------------------------------------------------*/
 APTR GTLayout_LT_GetWindowUserData( struct Window *window, APTR defaultValue );
#endif
