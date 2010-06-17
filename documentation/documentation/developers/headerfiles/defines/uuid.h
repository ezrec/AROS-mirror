#ifndef DEFINES_UUID_H
#define DEFINES_UUID_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/uuid/uuid.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for uuid
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __UUID_Compare_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2(int, UUID_Compare, \
                  AROS_LCA(const uuid_t *,(__arg1),A0), \
                  AROS_LCA(const uuid_t *,(__arg2),A1), \
        struct Library *, (__UUIDBase), 5, UUID)

#define UUID_Compare(arg1, arg2) \
    __UUID_Compare_WB(UUIDBase, (arg1), (arg2))

#define __UUID_GetNameSpace_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2NR(void, UUID_GetNameSpace, \
                  AROS_LCA(uuid_namespace_t,(__arg1),D0), \
                  AROS_LCA(uuid_t *,(__arg2),A0), \
        struct Library *, (__UUIDBase), 6, UUID)

#define UUID_GetNameSpace(arg1, arg2) \
    __UUID_GetNameSpace_WB(UUIDBase, (arg1), (arg2))

#define __UUID_Parse_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2(int, UUID_Parse, \
                  AROS_LCA(const char *,(__arg1),A0), \
                  AROS_LCA(uuid_t *,(__arg2),A1), \
        struct Library *, (__UUIDBase), 7, UUID)

#define UUID_Parse(arg1, arg2) \
    __UUID_Parse_WB(UUIDBase, (arg1), (arg2))

#define __UUID_Unparse_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2NR(void, UUID_Unparse, \
                  AROS_LCA(const uuid_t *,(__arg1),A0), \
                  AROS_LCA(char *,(__arg2),A1), \
        struct Library *, (__UUIDBase), 8, UUID)

#define UUID_Unparse(arg1, arg2) \
    __UUID_Unparse_WB(UUIDBase, (arg1), (arg2))

#define __UUID_Pack_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2NR(void, UUID_Pack, \
                  AROS_LCA(const uuid_t *,(__arg1),A0), \
                  AROS_LCA(uint8_t *,(__arg2),A1), \
        struct Library *, (__UUIDBase), 9, UUID)

#define UUID_Pack(arg1, arg2) \
    __UUID_Pack_WB(UUIDBase, (arg1), (arg2))

#define __UUID_Unpack_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2NR(void, UUID_Unpack, \
                  AROS_LCA(const uint8_t *,(__arg1),A0), \
                  AROS_LCA(uuid_t *,(__arg2),A1), \
        struct Library *, (__UUIDBase), 10, UUID)

#define UUID_Unpack(arg1, arg2) \
    __UUID_Unpack_WB(UUIDBase, (arg1), (arg2))

#define __UUID_Clear_WB(__UUIDBase, __arg1) \
        AROS_LC1NR(void, UUID_Clear, \
                  AROS_LCA(uuid_t *,(__arg1),A0), \
        struct Library *, (__UUIDBase), 11, UUID)

#define UUID_Clear(arg1) \
    __UUID_Clear_WB(UUIDBase, (arg1))

#define __UUID_Copy_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2NR(void, UUID_Copy, \
                  AROS_LCA(const uuid_t *,(__arg1),A0), \
                  AROS_LCA(uuid_t *,(__arg2),A1), \
        struct Library *, (__UUIDBase), 12, UUID)

#define UUID_Copy(arg1, arg2) \
    __UUID_Copy_WB(UUIDBase, (arg1), (arg2))

#define __UUID_Generate_WB(__UUIDBase, __arg1, __arg2) \
        AROS_LC2NR(void, UUID_Generate, \
                  AROS_LCA(uuid_type_t,(__arg1),D0), \
                  AROS_LCA(uuid_t *,(__arg2),A0), \
        struct Library *, (__UUIDBase), 13, UUID)

#define UUID_Generate(arg1, arg2) \
    __UUID_Generate_WB(UUIDBase, (arg1), (arg2))

__END_DECLS

#endif /* DEFINES_UUID_H*/
