
#ifndef	OUTLINEVIEW_MCC_CLASS_H
#define	OUTLINEVIEW_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"


#define	MUIA_OutlineView_Document         (MUIA_OutlineView_TagBase + 1)
#define	MUIA_OutlineView_Page             (MUIA_OutlineView_TagBase + 2)
#define	MUIA_OutlineView_IsEmpty          (MUIA_OutlineView_TagBase + 3)

#define MUIM_OutlineView_NewEntrySelected (MUIM_OutlineView_Dummy + 1)

struct MUIP_OutlineView_NewEntrySelected {ULONG MethodID; APTR entry;};

#define	OutlineViewObject   NewObject( getOutlineViewClass() , NULL

DEFCLASS(OutlineView);

#endif
