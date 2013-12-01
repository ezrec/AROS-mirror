/* Automatically generated header! Do not edit! */

#ifndef _INLINE_ICONOBJECT_LIB_SFD_H
#define _INLINE_ICONOBJECT_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef ICONOBJECT_LIB_SFD_BASE_NAME
#define ICONOBJECT_LIB_SFD_BASE_NAME IconobjectBase
#endif /* !ICONOBJECT_LIB_SFD_BASE_NAME */

#define NewIconObject(___name, ___tagList) __NewIconObject_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___name, ___tagList)
#define __NewIconObject_WB(___base, ___name, ___tagList) \
	AROS_LC2(Object *, NewIconObject, \
	AROS_LCA(CONST_STRPTR, (___name), A0), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 5, Iconobject_lib_sfd)

#ifndef NO_INLINE_STDARG
#define NewIconObjectTags(___name, ___firstTag, ...) __NewIconObjectTags_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___name, ___firstTag, ## __VA_ARGS__)
#define __NewIconObjectTags_WB(___base, ___name, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __NewIconObject_WB((___base), (___name), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define DisposeIconObject(___iconObject) __DisposeIconObject_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___iconObject)
#define __DisposeIconObject_WB(___base, ___iconObject) \
	AROS_LC1NR(VOID, DisposeIconObject, \
	AROS_LCA(Object *, (___iconObject), A0), \
	struct Library *, (___base), 6, Iconobject_lib_sfd)

#define GetDefIconObject(___iconType, ___tagList) __GetDefIconObject_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___iconType, ___tagList)
#define __GetDefIconObject_WB(___base, ___iconType, ___tagList) \
	AROS_LC2(Object *, GetDefIconObject, \
	AROS_LCA(ULONG, (___iconType), D0), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A0), \
	struct Library *, (___base), 7, Iconobject_lib_sfd)

#ifndef NO_INLINE_STDARG
#define GetDefIconObjectTags(___iconType, ___firstTag, ...) __GetDefIconObjectTags_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___iconType, ___firstTag, ## __VA_ARGS__)
#define __GetDefIconObjectTags_WB(___base, ___iconType, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __GetDefIconObject_WB((___base), (___iconType), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define PutIconObject(___iconObject, ___path, ___tagList) __PutIconObject_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___iconObject, ___path, ___tagList)
#define __PutIconObject_WB(___base, ___iconObject, ___path, ___tagList) \
	AROS_LC3(LONG, PutIconObject, \
	AROS_LCA(Object *, (___iconObject), A0), \
	AROS_LCA(CONST_STRPTR, (___path), A1), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 8, Iconobject_lib_sfd)

#ifndef NO_INLINE_STDARG
#define PutIconObjectTags(___iconObject, ___path, ___firstTag, ...) __PutIconObjectTags_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___iconObject, ___path, ___firstTag, ## __VA_ARGS__)
#define __PutIconObjectTags_WB(___base, ___iconObject, ___path, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __PutIconObject_WB((___base), (___iconObject), (___path), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define IsIconName(___fileName) __IsIconName_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___fileName)
#define __IsIconName_WB(___base, ___fileName) \
	AROS_LC1(ULONG, IsIconName, \
	AROS_LCA(CONST_STRPTR, (___fileName), A0), \
	struct Library *, (___base), 9, Iconobject_lib_sfd)

#define Convert2IconObject(___diskObject) __Convert2IconObject_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___diskObject)
#define __Convert2IconObject_WB(___base, ___diskObject) \
	AROS_LC1(Object *, Convert2IconObject, \
	AROS_LCA(struct DiskObject *, (___diskObject), A0), \
	struct Library *, (___base), 10, Iconobject_lib_sfd)

#define Convert2IconObjectA(___diskObject, ___tagList) __Convert2IconObjectA_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___diskObject, ___tagList)
#define __Convert2IconObjectA_WB(___base, ___diskObject, ___tagList) \
	AROS_LC2(Object *, Convert2IconObjectA, \
	AROS_LCA(struct DiskObject *, (___diskObject), A0), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 11, Iconobject_lib_sfd)

#ifndef NO_INLINE_STDARG
#define Convert2IconObjectTags(___diskObject, ___firstTag, ...) __Convert2IconObjectTags_WB(ICONOBJECT_LIB_SFD_BASE_NAME, ___diskObject, ___firstTag, ## __VA_ARGS__)
#define __Convert2IconObjectTags_WB(___base, ___diskObject, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __Convert2IconObjectA_WB((___base), (___diskObject), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#endif /* !_INLINE_ICONOBJECT_LIB_SFD_H */
