#ifndef DEFINES_COOLIMAGES_H
#define DEFINES_COOLIMAGES_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/coolimages/coolimages.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for coolimages
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __COOL_ObtainImageA_WB(__CoolImagesBase, __arg1, __arg2) \
        AROS_LC2(const struct CoolImage *, COOL_ObtainImageA, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(struct TagItem *,(__arg2),A0), \
        struct Library *, (__CoolImagesBase), 5, CoolImages)

#define COOL_ObtainImageA(arg1, arg2) \
    __COOL_ObtainImageA_WB(CoolImagesBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(COOLIMAGES_NO_INLINE_STDARG)
#define COOL_ObtainImage(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    COOL_ObtainImageA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

__END_DECLS

#endif /* DEFINES_COOLIMAGES_H*/
