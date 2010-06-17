#ifndef DEFINES_AROS_H
#define DEFINES_AROS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/aros/aros.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for aros
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __ArosInquireA_WB(__ArosBase, __arg1) \
        AROS_LC1(ULONG, ArosInquireA, \
                  AROS_LCA(struct TagItem *,(__arg1),A0), \
        struct Library *, (__ArosBase), 5, Aros)

#define ArosInquireA(arg1) \
    __ArosInquireA_WB(ArosBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(AROS_NO_INLINE_STDARG)
#define ArosInquire(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    ArosInquireA((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

__END_DECLS

#endif /* DEFINES_AROS_H*/
