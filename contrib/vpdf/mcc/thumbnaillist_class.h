
#ifndef	THUMBNAILLIST_MCC_CLASS_H
#define	THUMBNAILLIST_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"


#define	MUIA_ThumbnailList_Renderer  	    ( MUIA_ThumbnailList_TagBase + 3 )
#define	MUIA_ThumbnailList_PDFDocument      ( MUIA_ThumbnailList_TagBase + 4 )

#if defined(__AROS__)
#define ThumbnailListObject   BOOPSIOBJMACRO_START( getThumbnailListClass() )
#else
#define	ThumbnailListObject   NewObject( getThumbnailListClass() , NULL
#endif

DEFCLASS(ThumbnailList);

#endif
