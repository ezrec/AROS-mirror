#ifndef DEFINES_OOP_H
#define DEFINES_OOP_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/oop/oop.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for oop
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __OOP_NewObject_WB(__OOPBase, __arg1, __arg2, __arg3) \
        AROS_LC3(APTR, OOP_NewObject, \
                  AROS_LCA(struct OOP_IClass *,(__arg1),A0), \
                  AROS_LCA(UBYTE *,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct Library *, (__OOPBase), 5, OOP)

#define OOP_NewObject(arg1, arg2, arg3) \
    __OOP_NewObject_WB(OOPBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(OOP_NO_INLINE_STDARG)
#define OOP_NewObjectTags(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OOP_NewObject((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __OOP_ObtainAttrBase_WB(__OOPBase, __arg1) \
        AROS_LC1(OOP_AttrBase, OOP_ObtainAttrBase, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
        struct Library *, (__OOPBase), 6, OOP)

#define OOP_ObtainAttrBase(arg1) \
    __OOP_ObtainAttrBase_WB(OOPBase, (arg1))

#define __OOP_GetMethodID_WB(__OOPBase, __arg1, __arg2) \
        AROS_LC2(OOP_MethodID, OOP_GetMethodID, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct Library *, (__OOPBase), 7, OOP)

#define OOP_GetMethodID(arg1, arg2) \
    __OOP_GetMethodID_WB(OOPBase, (arg1), (arg2))

#define __OOP_AddClass_WB(__OOPBase, __arg1) \
        AROS_LC1NR(void, OOP_AddClass, \
                  AROS_LCA(OOP_Class *,(__arg1),A0), \
        struct Library *, (__OOPBase), 8, OOP)

#define OOP_AddClass(arg1) \
    __OOP_AddClass_WB(OOPBase, (arg1))

#define __OOP_ReleaseAttrBase_WB(__OOPBase, __arg1) \
        AROS_LC1NR(void, OOP_ReleaseAttrBase, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
        struct Library *, (__OOPBase), 9, OOP)

#define OOP_ReleaseAttrBase(arg1) \
    __OOP_ReleaseAttrBase_WB(OOPBase, (arg1))

#define __OOP_DisposeObject_WB(__OOPBase, __arg1) \
        AROS_LC1NR(void, OOP_DisposeObject, \
                  AROS_LCA(OOP_Object *,(__arg1),A0), \
        struct Library *, (__OOPBase), 10, OOP)

#define OOP_DisposeObject(arg1) \
    __OOP_DisposeObject_WB(OOPBase, (arg1))

#define __OOP_RemoveClass_WB(__OOPBase, __arg1) \
        AROS_LC1NR(void, OOP_RemoveClass, \
                  AROS_LCA(OOP_Class *,(__arg1),A0), \
        struct Library *, (__OOPBase), 11, OOP)

#define OOP_RemoveClass(arg1) \
    __OOP_RemoveClass_WB(OOPBase, (arg1))

#define __OOP_GetAttrBase_WB(__OOPBase, __arg1) \
        AROS_LC1(OOP_AttrBase, OOP_GetAttrBase, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
        struct Library *, (__OOPBase), 15, OOP)

#define OOP_GetAttrBase(arg1) \
    __OOP_GetAttrBase_WB(OOPBase, (arg1))

#define __OOP_GetAttr_WB(__OOPBase, __arg1, __arg2, __arg3) \
        AROS_LC3(IPTR, OOP_GetAttr, \
                  AROS_LCA(OOP_Object *,(__arg1),A0), \
                  AROS_LCA(OOP_AttrID,(__arg2),A1), \
                  AROS_LCA(IPTR *,(__arg3),A2), \
        struct Library *, (__OOPBase), 16, OOP)

#define OOP_GetAttr(arg1, arg2, arg3) \
    __OOP_GetAttr_WB(OOPBase, (arg1), (arg2), (arg3))

#define __OOP_SetAttrs_WB(__OOPBase, __arg1, __arg2) \
        AROS_LC2(IPTR, OOP_SetAttrs, \
                  AROS_LCA(OOP_Object *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__OOPBase), 17, OOP)

#define OOP_SetAttrs(arg1, arg2) \
    __OOP_SetAttrs_WB(OOPBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(OOP_NO_INLINE_STDARG)
#define OOP_SetAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OOP_SetAttrs((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __OOP_ObtainAttrBases_WB(__OOPBase, __arg1) \
        AROS_LC1(BOOL, OOP_ObtainAttrBases, \
                  AROS_LCA(struct OOP_ABDescr *,(__arg1),A0), \
        struct Library *, (__OOPBase), 18, OOP)

#define OOP_ObtainAttrBases(arg1) \
    __OOP_ObtainAttrBases_WB(OOPBase, (arg1))

#define __OOP_ReleaseAttrBases_WB(__OOPBase, __arg1) \
        AROS_LC1NR(void, OOP_ReleaseAttrBases, \
                  AROS_LCA(struct OOP_ABDescr *,(__arg1),A0), \
        struct Library *, (__OOPBase), 19, OOP)

#define OOP_ReleaseAttrBases(arg1) \
    __OOP_ReleaseAttrBases_WB(OOPBase, (arg1))

#define __OOP_ParseAttrs_WB(__OOPBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(LONG, OOP_ParseAttrs, \
                  AROS_LCA(struct TagItem *,(__arg1),A0), \
                  AROS_LCA(IPTR *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
                  AROS_LCA(OOP_AttrCheck *,(__arg4),A2), \
                  AROS_LCA(OOP_AttrBase,(__arg5),D1), \
        struct Library *, (__OOPBase), 20, OOP)

#define OOP_ParseAttrs(arg1, arg2, arg3, arg4, arg5) \
    __OOP_ParseAttrs_WB(OOPBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __OOP_GetMethod_WB(__OOPBase, __arg1, __arg2) \
        AROS_LC2(void *, OOP_GetMethod, \
                  AROS_LCA(OOP_Object *,(__arg1),A0), \
                  AROS_LCA(OOP_MethodID,(__arg2),D0), \
        struct Library *, (__OOPBase), 21, OOP)

#define OOP_GetMethod(arg1, arg2) \
    __OOP_GetMethod_WB(OOPBase, (arg1), (arg2))

#define __OOP_FindClass_WB(__OOPBase, __arg1) \
        AROS_LC1(OOP_Class *, OOP_FindClass, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
        struct Library *, (__OOPBase), 22, OOP)

#define OOP_FindClass(arg1) \
    __OOP_FindClass_WB(OOPBase, (arg1))

__END_DECLS

#endif /* DEFINES_OOP_H*/
