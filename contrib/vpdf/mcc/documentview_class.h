
#ifndef	DOCUMENTVIEW_MCC_CLASS_H
#define	DOCUMENTVIEW_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"


#define	MUIA_DocumentView_FileName         ( MUIA_DocumentView_TagBase + 1 )
#define	MUIA_DocumentView_Page  		   ( MUIA_DocumentView_TagBase + 2 )
#define	MUIA_DocumentView_Renderer  	   ( MUIA_DocumentView_TagBase + 3 )
#define	MUIA_DocumentView_PDFDocument  	   ( MUIA_DocumentView_TagBase + 4 )
#define	MUIA_DocumentView_Layout           ( MUIA_DocumentView_TagBase + 5 )
#define	MUIA_DocumentView_RenderPriority   ( MUIA_DocumentView_TagBase + 6 )
#define MUIA_DocumentView_Outline          ( MUIA_DocumentView_TagBase + 7 )
#define MUIA_DocumentView_DragAction       ( MUIA_DocumentView_TagBase + 8 )

#define	MUIM_DocumentView_Render		   ( MUIM_DocumentView_Dummy + 1 )
#define	MUIM_DocumentView_EnqueueRender	   ( MUIM_DocumentView_Dummy + 2 )
#define	MUIM_DocumentView_SetupPageViewImage ( MUIM_DocumentView_Dummy + 3 )
#define	MUIM_DocumentView_FindViewForPage  ( MUIM_DocumentView_Dummy + 5 )
#define	MUIM_DocumentView_Layout           ( MUIM_DocumentView_Dummy + 4 )
#define	MUIM_DocumentView_RotateRight      ( MUIM_DocumentView_Dummy + 6 )
#define	MUIM_DocumentView_RotateLeft       ( MUIM_DocumentView_Dummy + 7 )
#define MUIM_DocumentView_UpdateAnnotations (MUIM_DocumentView_Dummy + 8 )
#define MUIM_DocumentView_ClearSelection   ( MUIM_DocumentView_Dummy + 9 )
#define MUIM_DocumentView_SelectionCopy    ( MUIM_DocumentView_Dummy + 10)

#if defined(__AROS__)
#define DocumentViewObject   BOOPSIOBJMACRO_START( getDocumentViewClass() )
#else
#define	DocumentViewObject   NewObject( getDocumentViewClass() , NULL
#endif

struct MUIP_DocumentView_Render{ULONG MethodID;};
struct MUIP_DocumentView_EnqueueRender{ULONG MethodID; LONG page;};
struct MUIP_DocumentView_SetupPageViewImage{ULONG MethodID; APTR pageview; APTR image;};
struct MUIP_DocumentView_Layout{ULONG MethodID; LONG layout;};
struct MUIP_DocumentView_FindViewForPage{ULONG MethodID; LONG page;};
struct MUIP_DocumentView_RotateRight{ULONG MethodID;};
struct MUIP_DocumentView_RotateLeft{ULONG MethodID;};
struct MUIP_DocumentView_ClearSelection{ULONG MethodID;};
struct MUIP_DocumentView_UpdateAnnotations{ULONG MethodID; int page;};
struct MUIP_DocumentView_SelectionCopy{ULONG MethodID;};

#define MUIV_DocumentView_Layout_Single 0
#define MUIV_DocumentView_Layout_ContinuousSingle 1
#define MUIV_DocumentView_Layout_Facing 2
#define MUIV_DocumentView_Layout_ContinuousFacing 3

#define MUIV_DocumentView_DragAction_Scroll 0
#define MUIV_DocumentView_DragAction_Mark 1


DEFCLASS(DocumentView);

#endif
