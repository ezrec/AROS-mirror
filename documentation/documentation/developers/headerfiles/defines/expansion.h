#ifndef DEFINES_EXPANSION_H
#define DEFINES_EXPANSION_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/expansion/expansion.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for expansion
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __AddConfigDev_WB(__ExpansionBase, __arg1) \
        AROS_LC1NR(void, AddConfigDev, \
                  AROS_LCA(struct ConfigDev *,(__arg1),A0), \
        struct Library *, (__ExpansionBase), 5, Expansion)

#define AddConfigDev(arg1) \
    __AddConfigDev_WB(ExpansionBase, (arg1))

#define __AddBootNode_WB(__ExpansionBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(BOOL, AddBootNode, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(struct DeviceNode *,(__arg3),A0), \
                  AROS_LCA(struct ConfigDev *,(__arg4),A1), \
        struct Library *, (__ExpansionBase), 6, Expansion)

#define AddBootNode(arg1, arg2, arg3, arg4) \
    __AddBootNode_WB(ExpansionBase, (arg1), (arg2), (arg3), (arg4))

#define __AllocBoardMem_WB(__ExpansionBase, __arg1) \
        AROS_LC1NR(void, AllocBoardMem, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Library *, (__ExpansionBase), 7, Expansion)

#define AllocBoardMem(arg1) \
    __AllocBoardMem_WB(ExpansionBase, (arg1))

#define __AllocConfigDev_WB(__ExpansionBase) \
        AROS_LC0(struct ConfigDev *, AllocConfigDev, \
        struct Library *, (__ExpansionBase), 8, Expansion)

#define AllocConfigDev() \
    __AllocConfigDev_WB(ExpansionBase)

#define __AllocExpansionMem_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2(APTR, AllocExpansionMem, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
        struct Library *, (__ExpansionBase), 9, Expansion)

#define AllocExpansionMem(arg1, arg2) \
    __AllocExpansionMem_WB(ExpansionBase, (arg1), (arg2))

#define __ConfigBoard_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2NR(void, ConfigBoard, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(struct ConfigDev *,(__arg2),A1), \
        struct Library *, (__ExpansionBase), 10, Expansion)

#define ConfigBoard(arg1, arg2) \
    __ConfigBoard_WB(ExpansionBase, (arg1), (arg2))

#define __ConfigChain_WB(__ExpansionBase, __arg1) \
        AROS_LC1NR(void, ConfigChain, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__ExpansionBase), 11, Expansion)

#define ConfigChain(arg1) \
    __ConfigChain_WB(ExpansionBase, (arg1))

#define __FindConfigDev_WB(__ExpansionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct ConfigDev *, FindConfigDev, \
                  AROS_LCA(struct ConfigDev *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__ExpansionBase), 12, Expansion)

#define FindConfigDev(arg1, arg2, arg3) \
    __FindConfigDev_WB(ExpansionBase, (arg1), (arg2), (arg3))

#define __FreeBoardMem_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2NR(void, FreeBoardMem, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
        struct Library *, (__ExpansionBase), 13, Expansion)

#define FreeBoardMem(arg1, arg2) \
    __FreeBoardMem_WB(ExpansionBase, (arg1), (arg2))

#define __FreeConfigDev_WB(__ExpansionBase, __arg1) \
        AROS_LC1NR(void, FreeConfigDev, \
                  AROS_LCA(struct ConfigDev *,(__arg1),A0), \
        struct Library *, (__ExpansionBase), 14, Expansion)

#define FreeConfigDev(arg1) \
    __FreeConfigDev_WB(ExpansionBase, (arg1))

#define __FreeExpansionMem_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2NR(void, FreeExpansionMem, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
        struct Library *, (__ExpansionBase), 15, Expansion)

#define FreeExpansionMem(arg1, arg2) \
    __FreeExpansionMem_WB(ExpansionBase, (arg1), (arg2))

#define __ReadExpansionByte_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2(UBYTE, ReadExpansionByte, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct Library *, (__ExpansionBase), 16, Expansion)

#define ReadExpansionByte(arg1, arg2) \
    __ReadExpansionByte_WB(ExpansionBase, (arg1), (arg2))

#define __ReadExpansionRom_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2NR(void, ReadExpansionRom, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(struct ConfigDev *,(__arg2),A1), \
        struct Library *, (__ExpansionBase), 17, Expansion)

#define ReadExpansionRom(arg1, arg2) \
    __ReadExpansionRom_WB(ExpansionBase, (arg1), (arg2))

#define __RemConfigDev_WB(__ExpansionBase, __arg1) \
        AROS_LC1NR(void, RemConfigDev, \
                  AROS_LCA(struct ConfigDev *,(__arg1),A0), \
        struct Library *, (__ExpansionBase), 18, Expansion)

#define RemConfigDev(arg1) \
    __RemConfigDev_WB(ExpansionBase, (arg1))

#define __WriteExpansionByte_WB(__ExpansionBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, WriteExpansionByte, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(ULONG,(__arg3),D1), \
        struct Library *, (__ExpansionBase), 19, Expansion)

#define WriteExpansionByte(arg1, arg2, arg3) \
    __WriteExpansionByte_WB(ExpansionBase, (arg1), (arg2), (arg3))

#define __ObtainConfigBinding_WB(__ExpansionBase) \
        AROS_LC0NR(void, ObtainConfigBinding, \
        struct Library *, (__ExpansionBase), 20, Expansion)

#define ObtainConfigBinding() \
    __ObtainConfigBinding_WB(ExpansionBase)

#define __ReleaseConfigBinding_WB(__ExpansionBase) \
        AROS_LC0NR(void, ReleaseConfigBinding, \
        struct Library *, (__ExpansionBase), 21, Expansion)

#define ReleaseConfigBinding() \
    __ReleaseConfigBinding_WB(ExpansionBase)

#define __SetCurrentBinding_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2NR(void, SetCurrentBinding, \
                  AROS_LCA(struct CurrentBinding *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct Library *, (__ExpansionBase), 22, Expansion)

#define SetCurrentBinding(arg1, arg2) \
    __SetCurrentBinding_WB(ExpansionBase, (arg1), (arg2))

#define __GetCurrentBinding_WB(__ExpansionBase, __arg1, __arg2) \
        AROS_LC2(ULONG, GetCurrentBinding, \
                  AROS_LCA(struct CurrentBinding *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct Library *, (__ExpansionBase), 23, Expansion)

#define GetCurrentBinding(arg1, arg2) \
    __GetCurrentBinding_WB(ExpansionBase, (arg1), (arg2))

#define __MakeDosNode_WB(__ExpansionBase, __arg1) \
        AROS_LC1(struct DeviceNode *, MakeDosNode, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__ExpansionBase), 24, Expansion)

#define MakeDosNode(arg1) \
    __MakeDosNode_WB(ExpansionBase, (arg1))

#define __AddDosNode_WB(__ExpansionBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, AddDosNode, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(struct DeviceNode *,(__arg3),A0), \
        struct Library *, (__ExpansionBase), 25, Expansion)

#define AddDosNode(arg1, arg2, arg3) \
    __AddDosNode_WB(ExpansionBase, (arg1), (arg2), (arg3))

__END_DECLS

#endif /* DEFINES_EXPANSION_H*/
