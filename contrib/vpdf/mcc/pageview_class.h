
#ifndef	PAGEVIEW_MCC_CLASS_H
#define	PAGEVIEW_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"


#define	MUIA_PageView_Width			   (MUIA_PageView_TagBase + 1)
#define	MUIA_PageView_Height		   (MUIA_PageView_TagBase + 2)
#define	MUIA_PageView_Region           (MUIA_PageView_TagBase + 8)
#define	MUIA_PageView_HasRegion        (MUIA_PageView_TagBase + 9)
#define	MUIA_PageView_Page             (MUIA_PageView_TagBase +10)
#define	MUIA_PageView_PDFDocument      (MUIA_PageView_TagBase +11)
#define	MUIA_PageView_PDFOutputDevice  (MUIA_PageView_TagBase +12)
#define	MUIA_PageView_PDFReady         (MUIA_PageView_TagBase +13)
#define	MUIA_PageView_NeedRefresh      (MUIA_PageView_TagBase +14)
#define	MUIA_PageView_AspectRatio 	   (MUIA_PageView_TagBase +15)
#define	MUIA_PageView_LayoutWidth	   (MUIA_PageView_TagBase +16)
#define	MUIA_PageView_LayoutHeight	   (MUIA_PageView_TagBase +17)
#define	MUIA_PageView_RedirectPage     (MUIA_PageView_TagBase +18)
#define	MUIA_PageView_MediaWidth       (MUIA_PageView_TagBase +19)
#define	MUIA_PageView_MediaHeight      (MUIA_PageView_TagBase +20)
#define	MUIA_PageView_Information      (MUIA_PageView_TagBase +21)
#define	MUIA_PageView_RenderWidth	   (MUIA_PageView_TagBase +22)	/* dimmensions of area page image can be placed into */
#define	MUIA_PageView_RenderHeight	   (MUIA_PageView_TagBase +23)
#define	MUIA_PageView_IsPreview   	   (MUIA_PageView_TagBase +24)
#define	MUIA_PageView_Rotation   	   (MUIA_PageView_TagBase +25)
#define	MUIA_PageView_PDFBitmap		   (MUIA_PageView_TagBase +26)
#define MUIA_PageView_Quiet            (MUIA_PageView_TagBase +27)

#define	MUIM_PageView_Update		   (MUIM_PageView_Dummy + 1)
#define	MUIM_PageView_AddMarker		   (MUIM_PageView_Dummy + 2)
#define	MUIM_PageView_RemoveMarker	   (MUIM_PageView_Dummy + 3)
#define	MUIM_PageView_Flush       	   (MUIM_PageView_Dummy + 4)
#define	MUIM_PageView_AddAnnotation	   (MUIM_PageView_Dummy + 5)
#define	MUIM_PageView_RemoveAnnotation (MUIM_PageView_Dummy + 6)
#define MUIM_PageView_ClearSelection   (MUIM_PageView_Dummy + 7)
#define MUIM_PageView_GetSelection     (MUIM_PageView_Dummy + 8)

#define	PageViewObject   NewObject( getPageViewClass() , NULL

struct PageViewSelectionRegion
{
	float x1;
	float x2;
	float y1;
	float y2;
};

struct MUIP_PageView_Update {ULONG MethodID; int x; int y; int width; int height;};
struct MUIP_PageView_AddMarker {ULONG MethodID; int id; float *coords; unsigned int color;};
struct MUIP_PageView_RemoveMarker {ULONG MethodID; unsigned int id;};
struct MUIP_PageView_AddAnnotation {ULONG MethodID; Object *obj; float *coords;};
struct MUIP_PageView_RemoveAnnotation {ULONG MethodID; Object *obj;};
struct MUIP_PageView_Flush {ULONG MethodID; };
struct MUIP_PageView_ClearSelection {ULONG MethodID; };
struct MUIP_PageView_GetSelection {ULONG MethodID; struct PageViewSelectionRegion region;};


#define MUIV_PageView_AddMarker_New -1
#define MUIV_PageView_RemoveMarker_All -1

#define MUIV_PageView_Information_None 0
#define MUIV_PageView_Information_Number 1

#define MUIV_PageView_Rotation_None 0
#define MUIV_PageView_Rotation_90 1
#define MUIV_PageView_Rotation_180 2
#define MUIV_PageView_Rotation_270 3

struct PageViewRegion
{
	int x1;
	int x2;
	int y1;
	int y2;
};


DEFCLASS(PageView);

#endif
