#ifndef DEFINES_BULLET_H
#define DEFINES_BULLET_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/bullet/bullet.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for bullet
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __OpenEngine_WB(__BulletBase) \
        AROS_LC0(struct GlyphEngine *, OpenEngine, \
        struct Library *, (__BulletBase), 5, Bullet)

#define OpenEngine() \
    __OpenEngine_WB(BulletBase)

#define __CloseEngine_WB(__BulletBase, __arg1) \
        AROS_LC1NR(void, CloseEngine, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
        struct Library *, (__BulletBase), 6, Bullet)

#define CloseEngine(arg1) \
    __CloseEngine_WB(BulletBase, (arg1))

#define __SetInfoA_WB(__BulletBase, __arg1, __arg2) \
        AROS_LC2(ULONG, SetInfoA, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__BulletBase), 7, Bullet)

#define SetInfoA(arg1, arg2) \
    __SetInfoA_WB(BulletBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(BULLET_NO_INLINE_STDARG)
#define SetInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __ObtainInfoA_WB(__BulletBase, __arg1, __arg2) \
        AROS_LC2(ULONG, ObtainInfoA, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__BulletBase), 8, Bullet)

#define ObtainInfoA(arg1, arg2) \
    __ObtainInfoA_WB(BulletBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(BULLET_NO_INLINE_STDARG)
#define ObtainInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    ObtainInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __ReleaseInfoA_WB(__BulletBase, __arg1, __arg2) \
        AROS_LC2(ULONG, ReleaseInfoA, \
                  AROS_LCA(struct GlyphEngine *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__BulletBase), 9, Bullet)

#define ReleaseInfoA(arg1, arg2) \
    __ReleaseInfoA_WB(BulletBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(BULLET_NO_INLINE_STDARG)
#define ReleaseInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    ReleaseInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

__END_DECLS

#endif /* DEFINES_BULLET_H*/
