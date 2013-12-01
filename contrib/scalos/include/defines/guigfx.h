/* Automatically generated header! Do not edit! */

#ifndef _INLINE_GUIGFX_LIB_SFD_H
#define _INLINE_GUIGFX_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef GUIGFX_LIB_SFD_BASE_NAME
#define GUIGFX_LIB_SFD_BASE_NAME GuiGFXBase
#endif /* !GUIGFX_LIB_SFD_BASE_NAME */

#define MakePictureA(___array, ___width, ___height, ___tags) __MakePictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___array, ___width, ___height, ___tags)
#define __MakePictureA_WB(___base, ___array, ___width, ___height, ___tags) \
	AROS_LC4(APTR, MakePictureA, \
	AROS_LCA(APTR, (___array), A0), \
	AROS_LCA(UWORD, (___width), D0), \
	AROS_LCA(UWORD, (___height), D1), \
	AROS_LCA(struct TagItem *, (___tags), A1), \
	struct Library *, (___base), 5, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define MakePicture(___array, ___width, ___height, ___firstTag, ...) __MakePicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___array, ___width, ___height, ___firstTag, ## __VA_ARGS__)
#define __MakePicture_WB(___base, ___array, ___width, ___height, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __MakePictureA_WB((___base), (___array), (___width), (___height), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define LoadPictureA(___filename, ___tags) __LoadPictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___filename, ___tags)
#define __LoadPictureA_WB(___base, ___filename, ___tags) \
	AROS_LC2(APTR, LoadPictureA, \
	AROS_LCA(STRPTR, (___filename), A0), \
	AROS_LCA(struct TagItem *, (___tags), A1), \
	struct Library *, (___base), 6, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define LoadPicture(___filename, ___firstTag, ...) __LoadPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___filename, ___firstTag, ## __VA_ARGS__)
#define __LoadPicture_WB(___base, ___filename, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __LoadPictureA_WB((___base), (___filename), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ReadPictureA(___rp, ___colormap, ___x, ___y, ___width, ___height, ___tags) __ReadPictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___rp, ___colormap, ___x, ___y, ___width, ___height, ___tags)
#define __ReadPictureA_WB(___base, ___rp, ___colormap, ___x, ___y, ___width, ___height, ___tags) \
	AROS_LC7(APTR, ReadPictureA, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(struct ColorMap *, (___colormap), A1), \
	AROS_LCA(UWORD, (___x), D0), \
	AROS_LCA(UWORD, (___y), D1), \
	AROS_LCA(UWORD, (___width), D2), \
	AROS_LCA(UWORD, (___height), D3), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 7, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ReadPicture(___rp, ___colormap, ___x, ___y, ___width, ___height, ___firstTag, ...) __ReadPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___rp, ___colormap, ___x, ___y, ___width, ___height, ___firstTag, ## __VA_ARGS__)
#define __ReadPicture_WB(___base, ___rp, ___colormap, ___x, ___y, ___width, ___height, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ReadPictureA_WB((___base), (___rp), (___colormap), (___x), (___y), (___width), (___height), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ClonePictureA(___pic, ___tags) __ClonePictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___tags)
#define __ClonePictureA_WB(___base, ___pic, ___tags) \
	AROS_LC2(APTR, ClonePictureA, \
	AROS_LCA(APTR, (___pic), A0), \
	AROS_LCA(struct TagItem *, (___tags), A1), \
	struct Library *, (___base), 8, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ClonePicture(___pic, ___firstTag, ...) __ClonePicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___firstTag, ## __VA_ARGS__)
#define __ClonePicture_WB(___base, ___pic, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ClonePictureA_WB((___base), (___pic), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define DeletePicture(___pic) __DeletePicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic)
#define __DeletePicture_WB(___base, ___pic) \
	AROS_LC1NR(VOID, DeletePicture, \
	AROS_LCA(APTR, (___pic), A0), \
	struct Library *, (___base), 9, Guigfx_lib_sfd)

#define AddPictureA(___psm, ___pic, ___tags) __AddPictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___pic, ___tags)
#define __AddPictureA_WB(___base, ___psm, ___pic, ___tags) \
	AROS_LC3(APTR, AddPictureA, \
	AROS_LCA(APTR, (___psm), A0), \
	AROS_LCA(APTR, (___pic), A1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 11, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define AddPicture(___psm, ___pic, ___firstTag, ...) __AddPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___pic, ___firstTag, ## __VA_ARGS__)
#define __AddPicture_WB(___base, ___psm, ___pic, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __AddPictureA_WB((___base), (___psm), (___pic), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AddPaletteA(___psm, ___palette, ___tags) __AddPaletteA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___palette, ___tags)
#define __AddPaletteA_WB(___base, ___psm, ___palette, ___tags) \
	AROS_LC3(APTR, AddPaletteA, \
	AROS_LCA(APTR, (___psm), A0), \
	AROS_LCA(APTR, (___palette), A1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 12, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define AddPalette(___psm, ___palette, ___firstTag, ...) __AddPalette_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___palette, ___firstTag, ## __VA_ARGS__)
#define __AddPalette_WB(___base, ___psm, ___palette, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __AddPaletteA_WB((___base), (___psm), (___palette), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define AddPixelArrayA(___psm, ___array, ___width, ___height, ___tags) __AddPixelArrayA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___array, ___width, ___height, ___tags)
#define __AddPixelArrayA_WB(___base, ___psm, ___array, ___width, ___height, ___tags) \
	AROS_LC5(APTR, AddPixelArrayA, \
	AROS_LCA(APTR, (___psm), A0), \
	AROS_LCA(APTR, (___array), A1), \
	AROS_LCA(UWORD, (___width), D0), \
	AROS_LCA(UWORD, (___height), D1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 13, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define AddPixelArray(___psm, ___array, ___width, ___height, ___firstTag, ...) __AddPixelArray_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___array, ___width, ___height, ___firstTag, ## __VA_ARGS__)
#define __AddPixelArray_WB(___base, ___psm, ___array, ___width, ___height, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __AddPixelArrayA_WB((___base), (___psm), (___array), (___width), (___height), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define RemColorHandle(___colorhandle) __RemColorHandle_WB(GUIGFX_LIB_SFD_BASE_NAME, ___colorhandle)
#define __RemColorHandle_WB(___base, ___colorhandle) \
	AROS_LC1NR(VOID, RemColorHandle, \
	AROS_LCA(APTR, (___colorhandle), A0), \
	struct Library *, (___base), 14, Guigfx_lib_sfd)

#define CreatePenShareMapA(___tags) __CreatePenShareMapA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___tags)
#define __CreatePenShareMapA_WB(___base, ___tags) \
	AROS_LC1(APTR, CreatePenShareMapA, \
	AROS_LCA(struct TagItem *, (___tags), A0), \
	struct Library *, (___base), 15, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define CreatePenShareMap(___tags, ...) __CreatePenShareMap_WB(GUIGFX_LIB_SFD_BASE_NAME, ___tags, ## __VA_ARGS__)
#define __CreatePenShareMap_WB(___base, ___tags, ...) \
	({IPTR _tags[] = { (IPTR) ___tags, ## __VA_ARGS__ }; __CreatePenShareMapA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define DeletePenShareMap(___psm) __DeletePenShareMap_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm)
#define __DeletePenShareMap_WB(___base, ___psm) \
	AROS_LC1NR(VOID, DeletePenShareMap, \
	AROS_LCA(APTR, (___psm), A0), \
	struct Library *, (___base), 16, Guigfx_lib_sfd)

#define ObtainDrawHandleA(___psm, ___rp, ___cm, ___tags) __ObtainDrawHandleA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___rp, ___cm, ___tags)
#define __ObtainDrawHandleA_WB(___base, ___psm, ___rp, ___cm, ___tags) \
	AROS_LC4(APTR, ObtainDrawHandleA, \
	AROS_LCA(APTR, (___psm), A0), \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(struct ColorMap *, (___cm), A2), \
	AROS_LCA(struct TagItem *, (___tags), A3), \
	struct Library *, (___base), 17, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ObtainDrawHandle(___psm, ___rp, ___cm, ___firstTag, ...) __ObtainDrawHandle_WB(GUIGFX_LIB_SFD_BASE_NAME, ___psm, ___rp, ___cm, ___firstTag, ## __VA_ARGS__)
#define __ObtainDrawHandle_WB(___base, ___psm, ___rp, ___cm, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ObtainDrawHandleA_WB((___base), (___psm), (___rp), (___cm), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ReleaseDrawHandle(___drawhandle) __ReleaseDrawHandle_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle)
#define __ReleaseDrawHandle_WB(___base, ___drawhandle) \
	AROS_LC1NR(VOID, ReleaseDrawHandle, \
	AROS_LCA(APTR, (___drawhandle), A0), \
	struct Library *, (___base), 18, Guigfx_lib_sfd)

#define DrawPictureA(___drawhandle, ___pic, ___x, ___y, ___tags) __DrawPictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___pic, ___x, ___y, ___tags)
#define __DrawPictureA_WB(___base, ___drawhandle, ___pic, ___x, ___y, ___tags) \
	AROS_LC5(BOOL, DrawPictureA, \
	AROS_LCA(APTR, (___drawhandle), A0), \
	AROS_LCA(APTR, (___pic), A1), \
	AROS_LCA(UWORD, (___x), D0), \
	AROS_LCA(UWORD, (___y), D1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 19, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define DrawPicture(___drawhandle, ___pic, ___x, ___y, ___firstTag, ...) __DrawPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___pic, ___x, ___y, ___firstTag, ## __VA_ARGS__)
#define __DrawPicture_WB(___base, ___drawhandle, ___pic, ___x, ___y, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __DrawPictureA_WB((___base), (___drawhandle), (___pic), (___x), (___y), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define MapPaletteA(___drawhandle, ___palette, ___pentab, ___tags) __MapPaletteA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___palette, ___pentab, ___tags)
#define __MapPaletteA_WB(___base, ___drawhandle, ___palette, ___pentab, ___tags) \
	AROS_LC4(BOOL, MapPaletteA, \
	AROS_LCA(APTR, (___drawhandle), A0), \
	AROS_LCA(APTR, (___palette), A1), \
	AROS_LCA(UBYTE *, (___pentab), A2), \
	AROS_LCA(struct TagItem *, (___tags), A3), \
	struct Library *, (___base), 20, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define MapPalette(___drawhandle, ___palette, ___pentab, ___firstTag, ...) __MapPalette_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___palette, ___pentab, ___firstTag, ## __VA_ARGS__)
#define __MapPalette_WB(___base, ___drawhandle, ___palette, ___pentab, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __MapPaletteA_WB((___base), (___drawhandle), (___palette), (___pentab), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define MapPenA(___drawhandle, ___rgb, ___tags) __MapPenA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___rgb, ___tags)
#define __MapPenA_WB(___base, ___drawhandle, ___rgb, ___tags) \
	AROS_LC3(LONG, MapPenA, \
	AROS_LCA(APTR, (___drawhandle), A0), \
	AROS_LCA(ULONG, (___rgb), A1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 21, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define MapPen(___drawhandle, ___rgb, ___firstTag, ...) __MapPen_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___rgb, ___firstTag, ## __VA_ARGS__)
#define __MapPen_WB(___base, ___drawhandle, ___rgb, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __MapPenA_WB((___base), (___drawhandle), (___rgb), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define CreatePictureBitMapA(___drawhandle, ___pic, ___tags) __CreatePictureBitMapA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___pic, ___tags)
#define __CreatePictureBitMapA_WB(___base, ___drawhandle, ___pic, ___tags) \
	AROS_LC3(struct BitMap *, CreatePictureBitMapA, \
	AROS_LCA(APTR, (___drawhandle), A0), \
	AROS_LCA(APTR, (___pic), A1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 22, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define CreatePictureBitMap(___drawhandle, ___pic, ___firstTag, ...) __CreatePictureBitMap_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___pic, ___firstTag, ## __VA_ARGS__)
#define __CreatePictureBitMap_WB(___base, ___drawhandle, ___pic, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __CreatePictureBitMapA_WB((___base), (___drawhandle), (___pic), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define DoPictureMethodA(___pic, ___method, ___arguments) __DoPictureMethodA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___method, ___arguments)
#define __DoPictureMethodA_WB(___base, ___pic, ___method, ___arguments) \
	AROS_LC3(ULONG, DoPictureMethodA, \
	AROS_LCA(APTR, (___pic), A0), \
	AROS_LCA(ULONG, (___method), D0), \
	AROS_LCA(ULONG *, (___arguments), A1), \
	struct Library *, (___base), 23, Guigfx_lib_sfd)

#ifndef NO_INLINE_VARARGS
#define DoPictureMethod(___pic, ___method, ___firstTag, ...) __DoPictureMethod_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___method, ___firstTag, ## __VA_ARGS__)
#define __DoPictureMethod_WB(___base, ___pic, ___method, ___firstTag, ...) \
	({IPTR _message[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __DoPictureMethodA_WB((___base), (___pic), (___method), (ULONG *) _message); })
#endif /* !NO_INLINE_VARARGS */

#define GetPictureAttrsA(___pic, ___tags) __GetPictureAttrsA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___tags)
#define __GetPictureAttrsA_WB(___base, ___pic, ___tags) \
	AROS_LC2(ULONG, GetPictureAttrsA, \
	AROS_LCA(APTR, (___pic), A0), \
	AROS_LCA(struct TagItem *, (___tags), A1), \
	struct Library *, (___base), 24, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define GetPictureAttrs(___pic, ___firstTag, ...) __GetPictureAttrs_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___firstTag, ## __VA_ARGS__)
#define __GetPictureAttrs_WB(___base, ___pic, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __GetPictureAttrsA_WB((___base), (___pic), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define LockPictureA(___pic, ___mode, ___args) __LockPictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___mode, ___args)
#define __LockPictureA_WB(___base, ___pic, ___mode, ___args) \
	AROS_LC3(ULONG, LockPictureA, \
	AROS_LCA(APTR, (___pic), A0), \
	AROS_LCA(ULONG, (___mode), D0), \
	AROS_LCA(ULONG *, (___args), A1), \
	struct Library *, (___base), 25, Guigfx_lib_sfd)

#ifndef NO_INLINE_VARARGS
#define LockPicture(___pic, ___mode, ___firstTag, ...) __LockPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___mode, ___firstTag, ## __VA_ARGS__)
#define __LockPicture_WB(___base, ___pic, ___mode, ___firstTag, ...) \
	({IPTR _message[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __LockPictureA_WB((___base), (___pic), (___mode), (ULONG *) _message); })
#endif /* !NO_INLINE_VARARGS */

#define UnLockPicture(___pic, ___mode) __UnLockPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___mode)
#define __UnLockPicture_WB(___base, ___pic, ___mode) \
	AROS_LC2NR(VOID, UnLockPicture, \
	AROS_LCA(APTR, (___pic), A0), \
	AROS_LCA(ULONG, (___mode), D0), \
	struct Library *, (___base), 26, Guigfx_lib_sfd)

#define IsPictureA(___filename, ___tags) __IsPictureA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___filename, ___tags)
#define __IsPictureA_WB(___base, ___filename, ___tags) \
	AROS_LC2(BOOL, IsPictureA, \
	AROS_LCA(STRPTR, (___filename), A0), \
	AROS_LCA(struct TagItem *, (___tags), A1), \
	struct Library *, (___base), 27, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define IsPicture(___filename, ___firstTag, ...) __IsPicture_WB(GUIGFX_LIB_SFD_BASE_NAME, ___filename, ___firstTag, ## __VA_ARGS__)
#define __IsPicture_WB(___base, ___filename, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __IsPictureA_WB((___base), (___filename), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define CreateDirectDrawHandleA(___drawhandle, ___sw, ___sh, ___dw, ___dh, ___tags) __CreateDirectDrawHandleA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___sw, ___sh, ___dw, ___dh, ___tags)
#define __CreateDirectDrawHandleA_WB(___base, ___drawhandle, ___sw, ___sh, ___dw, ___dh, ___tags) \
	AROS_LC6(APTR, CreateDirectDrawHandleA, \
	AROS_LCA(APTR, (___drawhandle), A0), \
	AROS_LCA(UWORD, (___sw), D0), \
	AROS_LCA(UWORD, (___sh), D1), \
	AROS_LCA(UWORD, (___dw), D2), \
	AROS_LCA(UWORD, (___dh), D3), \
	AROS_LCA(struct TagItem *, (___tags), A1), \
	struct Library *, (___base), 28, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define CreateDirectDrawHandle(___drawhandle, ___sw, ___sh, ___dw, ___dh, ___firstTag, ...) __CreateDirectDrawHandle_WB(GUIGFX_LIB_SFD_BASE_NAME, ___drawhandle, ___sw, ___sh, ___dw, ___dh, ___firstTag, ## __VA_ARGS__)
#define __CreateDirectDrawHandle_WB(___base, ___drawhandle, ___sw, ___sh, ___dw, ___dh, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __CreateDirectDrawHandleA_WB((___base), (___drawhandle), (___sw), (___sh), (___dw), (___dh), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define DeleteDirectDrawHandle(___ddh) __DeleteDirectDrawHandle_WB(GUIGFX_LIB_SFD_BASE_NAME, ___ddh)
#define __DeleteDirectDrawHandle_WB(___base, ___ddh) \
	AROS_LC1NR(VOID, DeleteDirectDrawHandle, \
	AROS_LCA(APTR, (___ddh), A0), \
	struct Library *, (___base), 29, Guigfx_lib_sfd)

#define DirectDrawTrueColorA(___ddh, ___array, ___x, ___y, ___tags) __DirectDrawTrueColorA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___ddh, ___array, ___x, ___y, ___tags)
#define __DirectDrawTrueColorA_WB(___base, ___ddh, ___array, ___x, ___y, ___tags) \
	AROS_LC5(BOOL, DirectDrawTrueColorA, \
	AROS_LCA(APTR, (___ddh), A0), \
	AROS_LCA(ULONG *, (___array), A1), \
	AROS_LCA(UWORD, (___x), D0), \
	AROS_LCA(UWORD, (___y), D1), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 30, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define DirectDrawTrueColor(___ddh, ___array, ___x, ___y, ___firstTag, ...) __DirectDrawTrueColor_WB(GUIGFX_LIB_SFD_BASE_NAME, ___ddh, ___array, ___x, ___y, ___firstTag, ## __VA_ARGS__)
#define __DirectDrawTrueColor_WB(___base, ___ddh, ___array, ___x, ___y, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __DirectDrawTrueColorA_WB((___base), (___ddh), (___array), (___x), (___y), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define CreatePictureMaskA(___pic, ___mask, ___maskwidth, ___tags) __CreatePictureMaskA_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___mask, ___maskwidth, ___tags)
#define __CreatePictureMaskA_WB(___base, ___pic, ___mask, ___maskwidth, ___tags) \
	AROS_LC4(BOOL, CreatePictureMaskA, \
	AROS_LCA(APTR, (___pic), A0), \
	AROS_LCA(UBYTE *, (___mask), A1), \
	AROS_LCA(UWORD, (___maskwidth), D0), \
	AROS_LCA(struct TagItem *, (___tags), A2), \
	struct Library *, (___base), 31, Guigfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define CreatePictureMask(___pic, ___mask, ___maskwidth, ___firstTag, ...) __CreatePictureMask_WB(GUIGFX_LIB_SFD_BASE_NAME, ___pic, ___mask, ___maskwidth, ___firstTag, ## __VA_ARGS__)
#define __CreatePictureMask_WB(___base, ___pic, ___mask, ___maskwidth, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __CreatePictureMaskA_WB((___base), (___pic), (___mask), (___maskwidth), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#endif /* !_INLINE_GUIGFX_LIB_SFD_H */
