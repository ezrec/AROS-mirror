#ifndef CLIB_PROMETHEUS_PROTOS_H
#define CLIB_PROMETHEUS_PROTOS_H

/*
    *** Automatically generated from 'prometheus.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <utility/utility.h>
#include <exec/interrupts.h>
#include <libraries/prometheus.h>
AROS_LP2(PCIBoard *, Prm_FindBoardTagList,
         AROS_LPA(PCIBoard *, previous, A0),
         AROS_LPA(struct TagItem *, tag_list, A1),
         LIBBASETYPEPTR, PrometheusBase, 5, Prometheus
);
AROS_LP2(ULONG, Prm_GetBoardAttrsTagList,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(struct TagItem *, tag_list, A1),
         LIBBASETYPEPTR, PrometheusBase, 6, Prometheus
);
AROS_LP2(ULONG, Prm_ReadConfigLong,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(UBYTE, offset, D0),
         LIBBASETYPEPTR, PrometheusBase, 7, Prometheus
);
AROS_LP2(UWORD, Prm_ReadConfigWord,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(UBYTE, offset, D0),
         LIBBASETYPEPTR, PrometheusBase, 8, Prometheus
);
AROS_LP2(UBYTE, Prm_ReadConfigByte,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(UBYTE, offset, D0),
         LIBBASETYPEPTR, PrometheusBase, 9, Prometheus
);
AROS_LP3(void, Prm_WriteConfigLong,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(ULONG, data, D0),
         AROS_LPA(UBYTE, offset, D1),
         LIBBASETYPEPTR, PrometheusBase, 10, Prometheus
);
AROS_LP3(void, Prm_WriteConfigWord,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(UWORD, data, D0),
         AROS_LPA(UBYTE, offset, D1),
         LIBBASETYPEPTR, PrometheusBase, 11, Prometheus
);
AROS_LP3(void, Prm_WriteConfigByte,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(UBYTE, data, D0),
         AROS_LPA(UBYTE, offset, D1),
         LIBBASETYPEPTR, PrometheusBase, 12, Prometheus
);
AROS_LP2(ULONG, Prm_SetBoardAttrsTagList,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(struct TagItem *, tag_list, A1),
         LIBBASETYPEPTR, PrometheusBase, 13, Prometheus
);
AROS_LP2(BOOL, Prm_AddIntServer,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(struct Interrupt *, interrupt, A1),
         LIBBASETYPEPTR, PrometheusBase, 14, Prometheus
);
AROS_LP2(void, Prm_RemIntServer,
         AROS_LPA(PCIBoard *, board, A0),
         AROS_LPA(struct Interrupt *, interrupt, A1),
         LIBBASETYPEPTR, PrometheusBase, 15, Prometheus
);
AROS_LP1(APTR, Prm_AllocDMABuffer,
         AROS_LPA(ULONG, size, D0),
         LIBBASETYPEPTR, PrometheusBase, 16, Prometheus
);
AROS_LP2(void, Prm_FreeDMABuffer,
         AROS_LPA(APTR, buffer, A0),
         AROS_LPA(ULONG, size, D0),
         LIBBASETYPEPTR, PrometheusBase, 17, Prometheus
);
AROS_LP1(APTR, Prm_GetPhysicalAddr,
         AROS_LPA(APTR, address, D0),
         LIBBASETYPEPTR, PrometheusBase, 18, Prometheus
);

#endif /* CLIB_PROMETHEUS_PROTOS_H */
