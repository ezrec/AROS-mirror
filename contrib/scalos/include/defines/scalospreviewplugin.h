/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SCALOSPREVIEWPLUGIN_LIB_SFD_H
#define _INLINE_SCALOSPREVIEWPLUGIN_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOSPREVIEWPLUGIN_LIB_SFD_BASE_NAME
#define SCALOSPREVIEWPLUGIN_LIB_SFD_BASE_NAME ScalosPreviewPluginBase
#endif /* !SCALOSPREVIEWPLUGIN_LIB_SFD_BASE_NAME */

#define SCAPreviewGenerate(___wt, ___dirLock, ___iconName, ___tagList) __SCAPreviewGenerate_WB(SCALOSPREVIEWPLUGIN_LIB_SFD_BASE_NAME, ___wt, ___dirLock, ___iconName, ___tagList)
#define __SCAPreviewGenerate_WB(___base, ___wt, ___dirLock, ___iconName, ___tagList) \
	AROS_LC4(LONG, SCAPreviewGenerate, \
	AROS_LCA(struct ScaWindowTask *, (___wt), A0), \
	AROS_LCA(BPTR, (___dirLock), A1), \
	AROS_LCA(CONST_STRPTR, (___iconName), A2), \
	AROS_LCA(struct TagItem *, (___tagList), A3), \
	struct Library *, (___base), 5, Scalospreviewplugin_lib_sfd)

#ifndef NO_INLINE_STDARG
#define SCAPreviewGenerateTags(___wt, ___dirLock, ___iconName, ___firstTag, ...) __SCAPreviewGenerateTags_WB(SCALOSPREVIEWPLUGIN_LIB_SFD_BASE_NAME, ___wt, ___dirLock, ___iconName, ___firstTag, ## __VA_ARGS__)
#define __SCAPreviewGenerateTags_WB(___base, ___wt, ___dirLock, ___iconName, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __SCAPreviewGenerate_WB((___base), (___wt), (___dirLock), (___iconName), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#endif /* !_INLINE_SCALOSPREVIEWPLUGIN_LIB_SFD_H */
