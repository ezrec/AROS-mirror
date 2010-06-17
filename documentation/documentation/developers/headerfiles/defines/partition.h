#ifndef DEFINES_PARTITION_H
#define DEFINES_PARTITION_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/partition/partition.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for partition
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __OpenRootPartition_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(struct PartitionHandle *, OpenRootPartition, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A1), \
                  AROS_LCA(LONG,(__arg2),D1), \
        struct PartitionBase *, (__PartitionBase), 5, Partition)

#define OpenRootPartition(arg1, arg2) \
    __OpenRootPartition_WB(PartitionBase, (arg1), (arg2))

#define __CloseRootPartition_WB(__PartitionBase, __arg1) \
        AROS_LC1NR(void, CloseRootPartition, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 6, Partition)

#define CloseRootPartition(arg1) \
    __CloseRootPartition_WB(PartitionBase, (arg1))

#define __OpenPartitionTable_WB(__PartitionBase, __arg1) \
        AROS_LC1(LONG, OpenPartitionTable, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 7, Partition)

#define OpenPartitionTable(arg1) \
    __OpenPartitionTable_WB(PartitionBase, (arg1))

#define __ClosePartitionTable_WB(__PartitionBase, __arg1) \
        AROS_LC1NR(void, ClosePartitionTable, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 8, Partition)

#define ClosePartitionTable(arg1) \
    __ClosePartitionTable_WB(PartitionBase, (arg1))

#define __WritePartitionTable_WB(__PartitionBase, __arg1) \
        AROS_LC1(LONG, WritePartitionTable, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 9, Partition)

#define WritePartitionTable(arg1) \
    __WritePartitionTable_WB(PartitionBase, (arg1))

#define __CreatePartitionTable_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(LONG, CreatePartitionTable, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
                  AROS_LCA(ULONG,(__arg2),D1), \
        struct PartitionBase *, (__PartitionBase), 10, Partition)

#define CreatePartitionTable(arg1, arg2) \
    __CreatePartitionTable_WB(PartitionBase, (arg1), (arg2))

#define __AddPartition_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(struct PartitionHandle *, AddPartition, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A2), \
        struct PartitionBase *, (__PartitionBase), 11, Partition)

#define AddPartition(arg1, arg2) \
    __AddPartition_WB(PartitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PARTITION_NO_INLINE_STDARG)
#define AddPartitionTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    AddPartition((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __DeletePartition_WB(__PartitionBase, __arg1) \
        AROS_LC1NR(void, DeletePartition, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 12, Partition)

#define DeletePartition(arg1) \
    __DeletePartition_WB(PartitionBase, (arg1))

#define __GetPartitionTableAttrs_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(LONG, GetPartitionTableAttrs, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A2), \
        struct PartitionBase *, (__PartitionBase), 13, Partition)

#define GetPartitionTableAttrs(arg1, arg2) \
    __GetPartitionTableAttrs_WB(PartitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PARTITION_NO_INLINE_STDARG)
#define GetPartitionTableAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GetPartitionTableAttrs((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SetPartitionTableAttrs_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(LONG, SetPartitionTableAttrs, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A2), \
        struct PartitionBase *, (__PartitionBase), 14, Partition)

#define SetPartitionTableAttrs(arg1, arg2) \
    __SetPartitionTableAttrs_WB(PartitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PARTITION_NO_INLINE_STDARG)
#define SetPartitionTableAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetPartitionTableAttrs((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetPartitionAttrs_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(LONG, GetPartitionAttrs, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A2), \
        struct PartitionBase *, (__PartitionBase), 15, Partition)

#define GetPartitionAttrs(arg1, arg2) \
    __GetPartitionAttrs_WB(PartitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PARTITION_NO_INLINE_STDARG)
#define GetPartitionAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GetPartitionAttrs((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SetPartitionAttrs_WB(__PartitionBase, __arg1, __arg2) \
        AROS_LC2(LONG, SetPartitionAttrs, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A2), \
        struct PartitionBase *, (__PartitionBase), 16, Partition)

#define SetPartitionAttrs(arg1, arg2) \
    __SetPartitionAttrs_WB(PartitionBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PARTITION_NO_INLINE_STDARG)
#define SetPartitionAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetPartitionAttrs((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __QueryPartitionTableAttrs_WB(__PartitionBase, __arg1) \
        AROS_LC1(struct PartitionAttribute *, QueryPartitionTableAttrs, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 17, Partition)

#define QueryPartitionTableAttrs(arg1) \
    __QueryPartitionTableAttrs_WB(PartitionBase, (arg1))

#define __QueryPartitionAttrs_WB(__PartitionBase, __arg1) \
        AROS_LC1(struct PartitionAttribute *, QueryPartitionAttrs, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 18, Partition)

#define QueryPartitionAttrs(arg1) \
    __QueryPartitionAttrs_WB(PartitionBase, (arg1))

#define __DestroyPartitionTable_WB(__PartitionBase, __arg1) \
        AROS_LC1(LONG, DestroyPartitionTable, \
                  AROS_LCA(struct PartitionHandle *,(__arg1),A1), \
        struct PartitionBase *, (__PartitionBase), 19, Partition)

#define DestroyPartitionTable(arg1) \
    __DestroyPartitionTable_WB(PartitionBase, (arg1))

__END_DECLS

#endif /* DEFINES_PARTITION_H*/
