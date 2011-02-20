#ifndef DEFINES_BGUI_PROTOS_H
#define DEFINES_BGUI_PROTOS_H

#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#endif
#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif

#include <libraries/bgui.h>
#include <intuition/intuition.h>


/* Defines */
#define BGUI_GetClassPtr(num) 			\
	AROS_LC1(Class *, BGUI_GetClassPtr, 	\
	AROS_LCA(ULONG ,num, D0), 		\
	struct Library *, BGUIBase, 5, BGUI)
	
#define BGUI_NewObjectA(num, tags) 		\
	AROS_LC2(Object *, BGUI_NewObjectA, 	\
	AROS_LCA(ULONG , num, D0), 		\
	AROS_LCA(struct TagItem *, tags, A0), 	\
	struct Library *, BGUIBase, 6, BGUI)

#define BGUI_RequestA(win, estruct,args) 	\
	AROS_LC3(ULONG, BGUI_RequestA, 		\
	AROS_LCA(struct Window *, win, A0), 	\
	AROS_LCA(struct bguiRequest *, estruct, A1), \
	AROS_LCA(IPTR * , args, A2), 		\
	struct Library *, BGUIBase, 7, BGUI)

#define BGUI_Help(win,name,node,line) 		\
	AROS_LC4(BOOL, BGUI_Help, 		\
	AROS_LCA(struct Window * , win, A0), 	\
	AROS_LCA(UBYTE *, name, A1), 		\
	AROS_LCA(UBYTE *, node, A2), 		\
	AROS_LCA(ULONG, line, A3), 		\
	struct Library *, BGUIBase, 8, BGUI)

#define BGUI_LockWindow(win) 			\
	AROS_LC1(APTR, BGUI_LockWindow, 	\
	AROS_LCA(struct Window *, win, A0), 	\
	struct Library *, BGUIBase, 9, BGUI)

#define BGUI_UnlockWindow(lock) 		\
	AROS_LC1NR(VOID, BGUI_UnlockWindow, 	\
	AROS_LCA(APTR, lock, A0), 		\
	struct Library *, BGUIBase, 10, BGUI)

#define BGUI_DoGadgetMethodA(object,win,req,msg) \
	AROS_LC4(ULONG, BGUI_DoGadgetMethodA, 	\
	AROS_LCA(Object *, object, A0), 	\
	AROS_LCA(struct Window *, win, A1), 	\
	AROS_LCA(struct Requester *, req, A2), 	\
	AROS_LCA(Msg, msg, A3), 		\
	struct Library *, BGUIBase, 11, BGUI)
	
#define BGUI_AllocPoolMem(size) 		\
	AROS_LC1(APTR, BGUI_AllocPoolMem, 	\
	AROS_LCA(ULONG, size, D0), 		\
	struct Library *, BGUIBase, 12, BGUI)
	
#define BGUI_FreePoolMem(mem) 			\
	AROS_LC1NR(void, BGUI_FreePoolMem, 	\
	AROS_LCA(APTR, mem, A0), 		\
	struct Library *, BGUIBase, 13, BGUI)

#define BGUI_AllocBitMap(width,height,depth,flags,Friend) \
	AROS_LC5(struct BitMap *, BGUI_AllocBitMap, \
	AROS_LCA(ULONG, width, D0), 		\
	AROS_LCA(ULONG, height, D1), 		\
	AROS_LCA(ULONG, depth, D2), 		\
	AROS_LCA(ULONG, flags, D3), 		\
	AROS_LCA(struct BitMap *, Friend, A0), 	\
	struct Library *, BGUIBase, 14, BGUI)

#define BGUI_FreeBitMap(bitmap) 		\
	AROS_LC1NR(VOID, BGUI_FreeBitMap, 	\
	AROS_LCA(struct BitMap *,bitmap, A0), 	\
	struct Library *, BGUIBase, 15, BGUI)

#define BGUI_CreateRPortBitMap(rport,width,height,depth) \
	AROS_LC4(struct RastPort *, BGUI_CreateRPortBitMap, \
	AROS_LCA(struct RastPort *, rport, A0), \
	AROS_LCA(ULONG, width, D0), 		\
	AROS_LCA(ULONG, height, D1), 		\
	AROS_LCA(ULONG, depth, D2), 		\
	struct Library *, BGUIBase, 16, BGUI)

#define BGUI_FreeRPortBitMap(rport) 		\
	AROS_LC1NR(VOID, BGUI_FreeRPortBitMap, 	\
	AROS_LHA(struct RastPort *, rport, A0), \
	struct Library *, BGUIBase, 17, BGUI)

#define BGUI_InfoTextSize(rp,text,width,height) \
	AROS_LC4NR(VOID, BGUI_InfoTextSize, 	\
	AROS_LHA(struct RastPort *, rp, A0), 	\
	AROS_LHA(UBYTE *, text, A1), 		\
	AROS_LHA(UWORD *, width, A2), 		\
	AROS_LHA(UWORD *, height, A3), 		\
	struct Library *, BGUIBase, 18, BGUI)

#define BGUI_InfoText(rp,text,bounds,drawinfo) 	\
	AROS_LC4NR(void, BGUI_InfoText, 		\
	AROS_LCA(struct RastPort *, rp, A0), 	\
	AROS_LCA(UBYTE *, text, A1), 		\
	AROS_LCA(struct IBOX *, bounds, A2), 	\
	AROS_LCA(struct DrawInfo *, drawinfo, A3), \
	struct Library *, BGUIBase, 19, BGUI)

#define BGUI_GetLocaleStr(bl,id) 		\
	AROS_LC2(STRPTR, BGUI_GetLocaleStr, 	\
	AROS_LCA(struct bguiLocale *, bl, A0), 	\
	AROS_LCA(struct bguiLocale *, id, D0), 	\
	struct Library *, BGUIBase, 20, BGUI)

#define BGUI_GetCatalogStr(bl,id,def) 		\
	AROS_LC3(STRPTR, BGUI_GetCatalogStr, 	\
	AROS_LCA(struct bguiLocale *, bl, A0), 	\
	AROS_LCA(ULONG, id, D0), 		\
	AROS_LCA(STRPTR, def, A1), 		\
	struct Library *, BGUIBase, 21, BGUI)

#define BGUI_FillRectPattern(rport, bpat,x1,y1,x2,y2) \
	AROS_LC6NR(VOID, BGUI_FillRectPattern, 	\
	AROS_LCA(struct RastPort *, rport, A1), \
	AROS_LCA(struct bguiPattern *, bpat, A0), \
	AROS_LCA(LONG, x1, D0), 		\
	AROS_LCA(LONG, y1, D1), 		\
	AROS_LCA(LONG, x2, D2), 		\
	AROS_LCA(LONG, y2, D3), 		\
	struct Library *, BGUIBase, 22, BGUI)
	
#define BGUI_PostRender(cl,obj,gpr) 		\
	AROS_LC3NR(VOID, BGUI_PostRender, 	\
	AROS_LHA(Class *, cl, A0), 		\
	AROS_LHA(Object *, obj, A2), 		\
	AROS_LHA(struct gpRender *, gpr, A1), 	\
	struct Library *, BGUIBase, 23, BGUI)
	
#define BGUI_MakeClassA(tags) 			\
	AROS_LC1(Class *, BGUI_MakeClassA, 	\
	AROS_LHA(struct TagItem *, tags, A0), 	\
	strcut Library *, BGUIBase, 24, BGUI)

#define BGUI_FreeClass(cl) 			\
	AROS_LC1(BOOL, BGUI_FreeClass, 		\
	AROS_LCA(Class *, cl, A0), 		\
	struct Library *, BGUIBase, 25, BGUI)
	
#define BGUI_PackStructureTags(pack,tab,tags) 	\
	AROS_LC3(ULONG, BGUI_PackStructureTags, \
	AROS_LCA(APTR, pack, A0), 		\
	AROS_LCA(ULONG *, tab, A1), 		\
	AROS_LCA(struct TagItem *, tags, A2), 	\
	struct Library *, BGUIBase, 26, BGUI)

#define BGUI_UnpackStructureTags(pack,tab,tags) \
	AROS_LC3(ULONG, BGUI_UnpackStructureTags, \
	AROS_LCA(APTR, pack, A0), 		\
	AROS_LCA(ULONG *, tab, A1), 		\
	AROS_LCA(struct TagItem *, tags, A2), 	\
	struct Library *, BGUIBase, 27, BGUI)

/* private */
#define BGUI_GetDefaultTags(id) 		\
	AROS_LC1(struct TagItem *, BGUI_GetDefaultTags, 	\
	AROS_LCA(ULONG , id, D0), 		\
	struct Library *, BGUIBase, 28, BGUI)
	
#define BGUI_DefaultPrefs() 			\
	AROS_LC0NR(VOID, GetDefaultPrefs, 	\
	struct Library *, BGUIBase, 29, BGUI)
	
#define BGUI_LoadPrefs(name) 			\
	AROS_LC1NR(void, BGUI_LoadPrefs, 		\
	AROS_LCA(UBYTE *, name, A0), 		\
	struct Library *, BGUIBase, 30, BGUI)
	

#define BGUI_AllocPoolMemDebug(size,file,line) 	\
	AROS_LC3NR(void, BGUI_AllocPoolMemDebug, 	\
	AROS_LCA(ULONG, size, D0), 		\
	AROS_LCA(STRPTR, file, A0), 		\
	AROS_LCA(ULONG, line, D1), 		\
	struct Library *, BGUIBase, 31, BGUI)
	

#define BGUI_FreePoolMemDebug(mem,file,line) 	\
	AROS_LC3NR(void, FreePoolMemDebug,	\
	AROS_LCA(APTR, mem, A0), 		\
	AROS_LCA(STRPTR, file, A1), 		\
	AROS_LCA(ULONG, line, D0), 		\
	struct Library *, BGUIBase, 32, BGUI)


#endif
