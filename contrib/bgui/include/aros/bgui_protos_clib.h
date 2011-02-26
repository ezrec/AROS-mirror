#ifndef CLIB_BGUI_PROTOS_H
#define CLIB_BGUI_PROTOS_H

#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#endif

#include <libraries/bgui.h>

/* Prototypes */
AROS_LP1(Class *, BGUI_GetClassPtr,
	AROS_LPA(ULONG ,num, D0),
	struct Library *, BGUIBase, 5, BGUI
);
AROS_LP2(Object *, BGUI_NewObjectA,
	AROS_LPA(ULONG , num, D0),
	AROS_LPA(struct TagItem *, tags, A0),
	struct Library *, BGUIBase, 6, BGUI
);
AROS_LP3(ULONG, BGUI_RequestA,
	AROS_LPA(struct Window *, win, A0),
	AROS_LPA(struct bguiRequest *, estruct, A1),
	AROS_LPA(IPTR * , args, A2),
	struct Library *, BGUIBase, 7, BGUI
);
AROS_LP4(BOOL, BGUI_Help,
	AROS_LPA(struct Window * , win, A0),
	AROS_LPA(UBYTE *, name, A1),
	AROS_LPA(UBYTE *, node, A2),
	AROS_LPA(ULONG, line, A3),	
	struct Library *, BGUIBase, 8, BGUI
);
AROS_LP1(APTR, BGUI_LockWindow,
	AROS_LPA(struct Window *, win, A0),
	struct Library *, BGUIBase, 9, BGUI
);
AROS_LP1(VOID, BGUI_UnlockWindow,
	AROS_LPA(APTR, lock, A0),
	struct Library *, BGUIBase, 10, BGUI
);
AROS_LP4(ULONG, BGUI_DoGadgetMethodA,
	AROS_LPA(Object *, object, A0),
	AROS_LPA(struct Window *, win, A1),
	AROS_LPA(struct Requester *, req, A2),
	AROS_LPA(Msg, msg, A3),
	struct Library *, BGUIBase, 11, BGUI
);	
AROS_LP1(APTR, BGUI_AllocPoolMem,
	AROS_LPA(ULONG, size, D0),
	struct Library *, BGUIBase, 12, BGUI
);	
AROS_LP1(void, BGUI_FreePoolMem,
	AROS_LPA(APTR, mem, A0),
	struct Library *, BGUIBase, 13, BGUI
);	
AROS_LP5(struct BitMap *, BGUI_AllocBitMap,
	AROS_LPA(ULONG, width, D0),
	AROS_LPA(ULONG, height, D1),
	AROS_LPA(ULONG, depth, D2),
	AROS_LPA(ULONG, flags, D3),
	AROS_LPA(struct BitMap *, friend, A0),
	struct Library *, BGUIBase, 14, BGUI
);
AROS_LP1(VOID, BGUI_FreeBitMap,
	AROS_LPA(struct BitMap *,bitmap, A0),
	struct Library *, BGUIBase, 15, BGUI
);
AROS_LP4(struct RastPort *, BGUI_CreateRPortBitMap,
	AROS_LPA(struct RastPort *, rport, A0),
	AROS_LPA(ULONG, width, D0),
	AROS_LPA(ULONG, height, D1),
	AROS_LPA(ULONG, depth, D2),
	struct Library *, BGUIBase, 16, BGUI
);
AROS_LP1(VOID, BGUI_FreeRPortBitMap,
	AROS_LHA(struct RastPort *, rport, A0),
	struct Library *, BGUIBase, 17, BGUI
);
AROS_LP4(VOID, BGUI_InfoTextSize,
	AROS_LHA(struct RastPort *, rp, A0),
	AROS_LHA(UBYTE *, text, A1),
	AROS_LHA(UWORD *, width, A2),
	AROS_LHA(UWORD *, height, A3),
	struct Library *, BGUIBase, 18, BGUI
);
AROS_LP4(void, BGUI_InfoText,
	AROS_LPA(struct RastPort *, rp, A0),
	AROS_LPA(UBYTE *, text, A1),
	AROS_LPA(struct IBox *, bounds, A2),
	AROS_LPA(struct DrawInfo *, drawinfo, A3),
	struct Library *, BGUIBase, 19, BGUI
);
AROS_LP2(STRPTR, BGUI_GetLocaleStr,
	AROS_LPA(struct bguiLocale *, bl, A0),
	AROS_LPA(struct bguiLocale *, id, D0),
	struct Library *, BGUIBase, 20, BGUI
);
AROS_LP3(STRPTR, BGUI_GetCatalogStr,
	AROS_LPA(struct bguiLocale *, bl, A0),
	AROS_LPA(ULONG, id, D0),
	AROS_LPA(STRPTR, def, A1),
	struct Library *, BGUIBase, 21, BGUI
);
AROS_LP6(VOID, BGUI_FillRectPattern,
	AROS_LPA(struct RastPort *, rport, A1),
	AROS_LPA(struct bguiPattern *, bpat, A0),
	AROS_LPA(LONG, x1, D0),
	AROS_LPA(LONG, y1, D1),
	AROS_LPA(LONG, x2, D2),
	AROS_LPA(LONG, y2, D3),
	struct Library *, BGUIBase, 22, BGUI
);	
AROS_LP3(VOID, BGUI_PostRender,
	AROS_LHA(Class *, cl, A0),
	AROS_LHA(Object *, obj, A2),
	AROS_LHA(struct gpRender *, gpr, A1),
	struct Library *, BGUIBase, 23, BGUI
);	
AROS_LP1(Class *, BGUI_MakeClassA,
	AROS_LHA(struct TagItem *, tags, A0),
	struct Library *, BGUIBase, 24, BGUI
);
AROS_LP1(BOOL, BGUI_FreeClass,
	AROS_LPA(Class *, cl, A0),
	struct Library *, BGUIBase, 25, BGUI
);	
AROS_LP3(ULONG, BGUI_PackStructureTags,
	AROS_LPA(APTR, pack, A0),
	AROS_LPA(ULONG *, tab, A1),
	AROS_LPA(struct TagItem *, tags, A2),
	struct Library *, BGUIBase, 26, BGUI
);
AROS_LP3(ULONG, BGUI_UnpackStructureTags,
	AROS_LPA(APTR, pack, A0),
	AROS_LPA(ULONG *, tab, A1),
	AROS_LPA(struct TagItem *, tags, A2),
	struct Library *, BGUIBase, 27, BGUI
);
/* private */
AROS_LP1(struct TagItem *, BGUI_GetDefaultTags,
	AROS_LPA(ULONG, id, D0),
	struct Library *, BGUIBase, 28, BGUI
);	
AROS_LP0(VOID, GetDefaultPrefs,
	struct Library *, BGUIBase, 29, BGUI
);	
AROS_LP1(void, BGUI_LoadPrefs,
	AROS_LPA(UBYTE *, name, A0),
	struct Library *, BGUIBase, 30, BGUI
);	

AROS_LP3(void, BGUI_AllocPoolMemDebug,
	AROS_LPA(ULONG, size, D0),
	AROS_LPA(STRPTR, file, A0),
	AROS_LPA(ULONG, line, D1),
	struct Library *, BGUIBase, 31, BGUI
);	

AROS_LP3(void, FreePoolMemDebug,
	AROS_LPA(APTR, mem, A0),
	AROS_LPA(STRPTR, file, A1),
	AROS_LPA(ULONG, line, D0),
	struct Library *, BGUIBase, 32, BGUI
);
/* varargs */
Object *BGUI_NewObject( ULONG, Tag, ... );
#define BGUI_Request(win, breq, ...) \
	({ IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
	   BGUI_RequestA(win, breq, __args); })
ULONG BGUI_DoGadgetMethod( Object *, struct Window *, struct Requester *, IPTR, ... );
Class *BGUI_MakeClass( Tag, ... );


#endif
