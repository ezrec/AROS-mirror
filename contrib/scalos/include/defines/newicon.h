/* Automatically generated header! Do not edit! */

#ifndef _INLINE_NEWICON_LIB_SFD_H
#define _INLINE_NEWICON_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef NEWICON_LIB_SFD_BASE_NAME
#define NEWICON_LIB_SFD_BASE_NAME NewIconBase
#endif /* !NEWICON_LIB_SFD_BASE_NAME */

#define GetNewDiskObject(___name) __GetNewDiskObject_WB(NEWICON_LIB_SFD_BASE_NAME, ___name)
#define __GetNewDiskObject_WB(___base, ___name) \
	AROS_LC1(struct NewDiskObject *, GetNewDiskObject, \
	AROS_LCA(UBYTE *, (___name), A0), \
	struct Library *, (___base), 5, Newicon_lib_sfd)

#define PutNewDiskObject(___name, ___newdiskobj) __PutNewDiskObject_WB(NEWICON_LIB_SFD_BASE_NAME, ___name, ___newdiskobj)
#define __PutNewDiskObject_WB(___base, ___name, ___newdiskobj) \
	AROS_LC2(BOOL, PutNewDiskObject, \
	AROS_LCA(UBYTE *, (___name), A0), \
	AROS_LCA(struct NewDiskObject *, (___newdiskobj), A1), \
	struct Library *, (___base), 6, Newicon_lib_sfd)

#define FreeNewDiskObject(___newdiskobj) __FreeNewDiskObject_WB(NEWICON_LIB_SFD_BASE_NAME, ___newdiskobj)
#define __FreeNewDiskObject_WB(___base, ___newdiskobj) \
	AROS_LC1NR(VOID, FreeNewDiskObject, \
	AROS_LCA(struct NewDiskObject *, (___newdiskobj), A0), \
	struct Library *, (___base), 7, Newicon_lib_sfd)

#define newiconPrivate1(___diskobj) __newiconPrivate1_WB(NEWICON_LIB_SFD_BASE_NAME, ___diskobj)
#define __newiconPrivate1_WB(___base, ___diskobj) \
	AROS_LC1(BOOL, newiconPrivate1, \
	AROS_LCA(struct NewDiskObject *, (___diskobj), A0), \
	struct Library *, (___base), 8, Newicon_lib_sfd)

#define newiconPrivate2(___diskobj) __newiconPrivate2_WB(NEWICON_LIB_SFD_BASE_NAME, ___diskobj)
#define __newiconPrivate2_WB(___base, ___diskobj) \
	AROS_LC1(UBYTE **, newiconPrivate2, \
	AROS_LCA(struct NewDiskObject *, (___diskobj), A0), \
	struct Library *, (___base), 9, Newicon_lib_sfd)

#define newiconPrivate3(___diskobj) __newiconPrivate3_WB(NEWICON_LIB_SFD_BASE_NAME, ___diskobj)
#define __newiconPrivate3_WB(___base, ___diskobj) \
	AROS_LC1NR(VOID, newiconPrivate3, \
	AROS_LCA(struct NewDiskObject *, (___diskobj), A0), \
	struct Library *, (___base), 10, Newicon_lib_sfd)

#define RemapChunkyImage(___chunkyimage, ___screen) __RemapChunkyImage_WB(NEWICON_LIB_SFD_BASE_NAME, ___chunkyimage, ___screen)
#define __RemapChunkyImage_WB(___base, ___chunkyimage, ___screen) \
	AROS_LC2(struct Image *, RemapChunkyImage, \
	AROS_LCA(struct ChunkyImage *, (___chunkyimage), A0), \
	AROS_LCA(struct Screen *, (___screen), A1), \
	struct Library *, (___base), 11, Newicon_lib_sfd)

#define FreeRemappedImage(___image, ___screen) __FreeRemappedImage_WB(NEWICON_LIB_SFD_BASE_NAME, ___image, ___screen)
#define __FreeRemappedImage_WB(___base, ___image, ___screen) \
	AROS_LC2NR(VOID, FreeRemappedImage, \
	AROS_LCA(struct Image *, (___image), A0), \
	AROS_LCA(struct Screen *, (___screen), A1), \
	struct Library *, (___base), 12, Newicon_lib_sfd)

#define GetDefNewDiskObject(___def_type) __GetDefNewDiskObject_WB(NEWICON_LIB_SFD_BASE_NAME, ___def_type)
#define __GetDefNewDiskObject_WB(___base, ___def_type) \
	AROS_LC1(struct NewDiskObject *, GetDefNewDiskObject, \
	AROS_LCA(LONG, (___def_type), D0), \
	struct Library *, (___base), 13, Newicon_lib_sfd)

#endif /* !_INLINE_NEWICON_LIB_SFD_H */
