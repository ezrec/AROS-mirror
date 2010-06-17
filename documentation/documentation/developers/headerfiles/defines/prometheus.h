#ifndef DEFINES_PROMETHEUS_H
#define DEFINES_PROMETHEUS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/prometheus/prometheus.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for prometheus
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __Prm_FindBoardTagList_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(PCIBoard *, Prm_FindBoardTagList, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__PrometheusBase), 5, Prometheus)

#define Prm_FindBoardTagList(arg1, arg2) \
    __Prm_FindBoardTagList_WB(PrometheusBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PROMETHEUS_NO_INLINE_STDARG)
#define Prm_FindBoardTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    Prm_FindBoardTagList((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __Prm_GetBoardAttrsTagList_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(ULONG, Prm_GetBoardAttrsTagList, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__PrometheusBase), 6, Prometheus)

#define Prm_GetBoardAttrsTagList(arg1, arg2) \
    __Prm_GetBoardAttrsTagList_WB(PrometheusBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PROMETHEUS_NO_INLINE_STDARG)
#define Prm_GetBoardAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    Prm_GetBoardAttrsTagList((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __Prm_ReadConfigLong_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(ULONG, Prm_ReadConfigLong, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(UBYTE,(__arg2),D0), \
        struct Library *, (__PrometheusBase), 7, Prometheus)

#define Prm_ReadConfigLong(arg1, arg2) \
    __Prm_ReadConfigLong_WB(PrometheusBase, (arg1), (arg2))

#define __Prm_ReadConfigWord_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(UWORD, Prm_ReadConfigWord, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(UBYTE,(__arg2),D0), \
        struct Library *, (__PrometheusBase), 8, Prometheus)

#define Prm_ReadConfigWord(arg1, arg2) \
    __Prm_ReadConfigWord_WB(PrometheusBase, (arg1), (arg2))

#define __Prm_ReadConfigByte_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(UBYTE, Prm_ReadConfigByte, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(UBYTE,(__arg2),D0), \
        struct Library *, (__PrometheusBase), 9, Prometheus)

#define Prm_ReadConfigByte(arg1, arg2) \
    __Prm_ReadConfigByte_WB(PrometheusBase, (arg1), (arg2))

#define __Prm_WriteConfigLong_WB(__PrometheusBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, Prm_WriteConfigLong, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(UBYTE,(__arg3),D1), \
        struct Library *, (__PrometheusBase), 10, Prometheus)

#define Prm_WriteConfigLong(arg1, arg2, arg3) \
    __Prm_WriteConfigLong_WB(PrometheusBase, (arg1), (arg2), (arg3))

#define __Prm_WriteConfigWord_WB(__PrometheusBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, Prm_WriteConfigWord, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(UWORD,(__arg2),D0), \
                  AROS_LCA(UBYTE,(__arg3),D1), \
        struct Library *, (__PrometheusBase), 11, Prometheus)

#define Prm_WriteConfigWord(arg1, arg2, arg3) \
    __Prm_WriteConfigWord_WB(PrometheusBase, (arg1), (arg2), (arg3))

#define __Prm_WriteConfigByte_WB(__PrometheusBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, Prm_WriteConfigByte, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(UBYTE,(__arg2),D0), \
                  AROS_LCA(UBYTE,(__arg3),D1), \
        struct Library *, (__PrometheusBase), 12, Prometheus)

#define Prm_WriteConfigByte(arg1, arg2, arg3) \
    __Prm_WriteConfigByte_WB(PrometheusBase, (arg1), (arg2), (arg3))

#define __Prm_SetBoardAttrsTagList_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(ULONG, Prm_SetBoardAttrsTagList, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__PrometheusBase), 13, Prometheus)

#define Prm_SetBoardAttrsTagList(arg1, arg2) \
    __Prm_SetBoardAttrsTagList_WB(PrometheusBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(PROMETHEUS_NO_INLINE_STDARG)
#define Prm_SetBoardAttrsTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    Prm_SetBoardAttrsTagList((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __Prm_AddIntServer_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2(BOOL, Prm_AddIntServer, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(struct Interrupt *,(__arg2),A1), \
        struct Library *, (__PrometheusBase), 14, Prometheus)

#define Prm_AddIntServer(arg1, arg2) \
    __Prm_AddIntServer_WB(PrometheusBase, (arg1), (arg2))

#define __Prm_RemIntServer_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2NR(void, Prm_RemIntServer, \
                  AROS_LCA(PCIBoard *,(__arg1),A0), \
                  AROS_LCA(struct Interrupt *,(__arg2),A1), \
        struct Library *, (__PrometheusBase), 15, Prometheus)

#define Prm_RemIntServer(arg1, arg2) \
    __Prm_RemIntServer_WB(PrometheusBase, (arg1), (arg2))

#define __Prm_AllocDMABuffer_WB(__PrometheusBase, __arg1) \
        AROS_LC1(APTR, Prm_AllocDMABuffer, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Library *, (__PrometheusBase), 16, Prometheus)

#define Prm_AllocDMABuffer(arg1) \
    __Prm_AllocDMABuffer_WB(PrometheusBase, (arg1))

#define __Prm_FreeDMABuffer_WB(__PrometheusBase, __arg1, __arg2) \
        AROS_LC2NR(void, Prm_FreeDMABuffer, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct Library *, (__PrometheusBase), 17, Prometheus)

#define Prm_FreeDMABuffer(arg1, arg2) \
    __Prm_FreeDMABuffer_WB(PrometheusBase, (arg1), (arg2))

#define __Prm_GetPhysicalAddr_WB(__PrometheusBase, __arg1) \
        AROS_LC1(APTR, Prm_GetPhysicalAddr, \
                  AROS_LCA(APTR,(__arg1),D0), \
        struct Library *, (__PrometheusBase), 18, Prometheus)

#define Prm_GetPhysicalAddr(arg1) \
    __Prm_GetPhysicalAddr_WB(PrometheusBase, (arg1))

__END_DECLS

#endif /* DEFINES_PROMETHEUS_H*/
