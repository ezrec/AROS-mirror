
#ifndef	DOCUMENTLAYOUT_MCC_CLASS_H
#define	DOCUMENTLAYOUT_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"

/* interface methods and attributes */

#define	MUIA_DocumentLayout_PDFDocument     (MUIA_DocumentLayout_TagBase + 1)
#define	MUIA_DocumentLayout_PDFOutputDevice (MUIA_DocumentLayout_TagBase + 2)
#define	MUIA_DocumentLayout_Page            (MUIA_DocumentLayout_TagBase + 3)
#define	MUIA_DocumentLayout_Scaling		    (MUIA_DocumentLayout_TagBase + 4)
#define	MUIA_DocumentLayout_Zoom   		    (MUIA_DocumentLayout_TagBase + 5) /* in percent, fixedpoint 16:16 */
#define	MUIA_DocumentLayout_Columns		    (MUIA_DocumentLayout_TagBase + 6)
#define	MUIA_DocumentLayout_Rotation	    (MUIA_DocumentLayout_TagBase + 7)

#define	MUIM_DocumentLayout_FindViewForPage (MUIM_DocumentLayout_Dummy + 1)
#define	MUIM_DocumentLayout_IsPageVisible   (MUIM_DocumentLayout_Dummy + 2)
#define MUIM_DocumentLayout_Refresh         (MUIM_DocumentLayout_Dummy + 3)

struct MUIP_DocumentLayout_FindViewForPage {ULONG MethodID; int page;};
struct MUIP_DocumentLayout_IsPageVisible {ULONG MethodID; int page;};

/* classspecific methods and attributes */

#define	MUIM_ContinuousLayout_Relayout (MUIM_DocumentLayout_Dummy + 10)
#define	MUIM_ContinuousLayout_SetupPage (MUIM_DocumentLayout_Dummy + 12)
#define	MUIM_ContinuousLayout_ClippedPageOffset (MUIM_DocumentLayout_Dummy + 13)
#define	MUIM_SinglePageLayout_Relayout (MUIM_DocumentLayout_Dummy + 11)

struct MUIP_ContinuousLayout_Relayout {ULONG MethodID;};
struct MUIP_ContinuousLayout_SetupPage{ULONG MethodID;};
struct MUIP_ContinuousLayout_ClippedPageOffset{ULONG MethodID; int page;};
struct MUIP_SinglePageLayout_Relayout {ULONG MethodID;};

#define MUIV_DocumentLayout_Scaling_FitPage 0
#define MUIV_DocumentLayout_Scaling_FitWidth 1
#define MUIV_DocumentLayout_Scaling_None 2
#define MUIV_DocumentLayout_Scaling_Zoom 3
#define MUIV_DocumentLayout_Scaling_FixWidth(x) (4 | ((x) << 16))

/* NOTE: keep them matching constants in pageview class */
#define MUIV_DocumentLayout_Rotation_None 0 
#define MUIV_DocumentLayout_Rotation_90 1
#define MUIV_DocumentLayout_Rotation_180 2
#define MUIV_DocumentLayout_Rotation_270 3

/* */

#define ContinuousLayoutObject   NewObject(getContinuousLayoutClass() , NULL
#define ContinuousDynamicLayoutObject   NewObject(getContinuousDynamicLayoutClass() , NULL
#define SinglePageLayoutObject   NewObject(getSinglePageLayoutClass() , NULL

DEFCLASS(ContinuousLayout);
DEFCLASS(ContinuousDynamicLayout);
DEFCLASS(SinglePageLayout);

#endif
