// ScaLibrary.h
// $Date$
// $Revision$


#ifndef SCALIBRARY_H
#define SCALIBRARY_H

extern UWORD ScaLibPluginOpenCount;
extern UBYTE fInitializingPlugins;

extern const char libIdString[];
extern const char scalosLibName[];

LIBFUNC_P6_PROTO(struct AppObject *, sca_NewAddAppIcon,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, Object *, iconObj,
	A1, struct MsgPort *, msgPort,
	A2, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(ULONG, sca_ScalosControl,
	A0, CONST_STRPTR, name,
	A1, struct TagItem *, taglist,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P6_PROTO(struct AppObject *, sca_NewAddAppMenuItem,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, MenuText,
	A1, struct MsgPort *, msgPort,
	A2, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(BOOL, sca_WBStart,
	A0, struct WBArg *, ArgArray,
	A1, struct TagItem *, TagList,
	D0, ULONG, NumArgs,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P5_PROTO(struct ScalosClass *, sca_MakeScalosClass,
	A0, CONST_STRPTR, ClassName,
	A1, CONST_STRPTR, SuperClassName,
	D0, UWORD, InstSize,
	A2, APTR, DispFunc,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(BOOL, sca_FreeScalosClass,
	A0, struct ScalosClass *, sccl,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(Object *, sca_NewScalosObject,
	A0, CONST_STRPTR, ClassName,
	A1, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(void, sca_DisposeScalosObject,
	A0, Object *, o,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(BOOL, sca_RemoveAppObject,
	A0, struct AppObject *, appObj,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(void, sca_ScreenTitleMsg,
	A0, CONST_STRPTR, Format,
	A1, APTR, Args,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P6_PROTO(struct AppObject *, sca_NewAddAppWindow,
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, struct Window *, win,
	A1, struct MsgPort *, msgPort,
	A2, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(struct MinNode *, sca_AllocNode,
	A0, struct ScalosNodeList *, nodeList,
	D0, ULONG, ExtraSize,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(void, sca_FreeNode,
	A0, struct ScalosNodeList *, nodeList,
	A1, struct MinNode *, oldNode,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(void, sca_FreeAllNodes,
	A0, struct ScalosNodeList *, nodeList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(struct MinNode *, sca_AllocStdNode,
	A0, struct ScalosNodeList *, nodeList,
	D0, ULONG, NodeType,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(struct ScalosMessage *, sca_AllocMessage,
	D0, ULONG, MessageType,
	D1, UWORD, AdditionalSize,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(void, sca_FreeMessage,
	A1, struct ScalosMessage *, msg,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(void, sca_UpdateIcon,
	D0, UBYTE, WindowType,
	A0, struct ScaUpdateIcon_IW *, uiiw,
	D1, ULONG, ui_SIZE,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(void, sca_FreeWBArgs,
	A0, struct WBArg *, wbArg,
	D0, ULONG, NumArgs,
	D1, ULONG, Flags,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(ULONG, sca_MakeWBArgs,
	A0, struct WBArg *, wbArg,
	A1, struct ScaIconNode *, in,
	D0, ULONG, MaxArg,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P2_PROTO(struct ScaWindowList *, sca_LockWindowList,
	D0, LONG, accessmode,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P1_PROTO (void, sca_UnLockWindowList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(void, sca_MoveNode,
	A0, struct ScalosNodeList *, src,
	A1, struct ScalosNodeList *, dest,
	D0, struct MinNode *, node,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(void, sca_SwapNodes,
	A0, struct MinNode *, srcNode,
	A1, struct MinNode *, destNode,
	A2, struct ScalosNodeList *, nodeList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(void, sca_SortNodes,
	A0, struct ScalosNodeList *, nodeList,
	A1, struct Hook *, compareHook,
	D0, ULONG, SortType,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P4_PROTO(void, sca_RemapBitmap,
	A0, struct BitMap *, bmSrc,
	A1, struct BitMap *, bmDest,
	A2, const BYTE *, penArray,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(Object *, sca_GetDefIconObject,
	A0, BPTR, dirLock,
	A1, CONST_STRPTR, name,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P3_PROTO(struct ScaWindowStruct *, sca_OpenDrawerByName,
	A0, CONST_STRPTR, path,
	A1, struct TagItem *, TagList,
	A6, struct ScalosBase *, ScalosBase);
LIBFUNC_P1_PROTO(ULONG, sca_CountWBArgs,
	A0, struct ScaIconNode *, in);
LIBFUNC_P4_PROTO(Object *, sca_GetDefIconObjectA,
	A0, BPTR, dirLock,
	A1, CONST_STRPTR, name,
	A2, struct TagItem *, tagList,
	A6, struct ScalosBase *, ScalosBase);

#endif // SCALIBRARY_H

