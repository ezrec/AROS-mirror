
#ifndef	TOOLBAR_MCC_CLASS_H
#define	TOOLBAR_MCC_CLASS_H

#include "classes.h"

#define	MUIA_Toolbar_DocumentView         (MUIA_Toolbar_TagBase + 1)

#define MUIM_Toolbar_Zoom                 (MUIM_Toolbar_Dummy + 1)
#define MUIM_Toolbar_Layout               (MUIM_Toolbar_Dummy + 2)
#define MUIM_Toolbar_ZoomPopup            (MUIM_Toolbar_Dummy + 3)
#define MUIM_Toolbar_ToggleOutline        (MUIM_Toolbar_Dummy + 4)
#define MUIM_Toolbar_DragAction           (MUIM_Toolbar_Dummy + 5)

struct MUIP_Toolbar_Zoom {ULONG MethodID; LONG change;};
struct MUIP_Toolbar_ZoomPopup {ULONG MethodID;};
struct MUIP_Toolbar_Layout {ULONG MethodID; LONG layout;};
struct MUIP_Toolbar_DragAction {ULONG MethodID; LONG dragaction;};
struct MUIP_Toolbar_ToggleOutline {ULONG MethodID; LONG visible; };

#define	ToolbarObject   NewObject( getToolbarClass() , NULL

DEFCLASS(Toolbar);

#endif
