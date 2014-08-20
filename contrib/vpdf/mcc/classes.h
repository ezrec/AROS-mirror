#ifndef MCC_CLASSES_H
#define MCC_CLASSES_H

#include <utility/tagitem.h>
#include <private/vapor/vapor.h>

#define	MUIM_PageView_Dummy            	( OM_Dummy + 0x1000 )
#define MUIA_PageView_TagBase		   	( TAG_USER + 0x1000 )

#define	MUIM_VPDF_Dummy            		( OM_Dummy + 0x2000 )
#define MUIA_VPDF_TagBase		   		( TAG_USER + 0x2000 )

#define	MUIM_DocumentView_Dummy        	( OM_Dummy + 0x03000 )
#define MUIA_DocumentView_TagBase      	( TAG_USER + 0x03000 )

#define	MUIM_OutlineView_Dummy         	( OM_Dummy + 0x04000 )
#define MUIA_OutlineView_TagBase       	( TAG_USER + 0x04000 )

#define	MUIM_DocumentLayout_Dummy       ( OM_Dummy + 0x05000 )
#define MUIA_DocumentLayout_TagBase     ( TAG_USER + 0x05000 )

#define	MUIM_Toolbar_Dummy              ( OM_Dummy + 0x6000 )
#define MUIA_Toolbar_TagBase            ( TAG_USER + 0x6000 )

#define	MUIM_Renderer_Dummy             ( OM_Dummy + 0x7000 )
#define MUIA_Renderer_TagBase           ( TAG_USER + 0x7000 )

#define	MUIM_Search_Dummy               ( OM_Dummy + 0x8000 )
#define MUIA_Search_TagBase             ( TAG_USER + 0x8000 )

#define	MUIM_VPDFWindow_Dummy           ( OM_Dummy + 0x9000 )
#define MUIA_VPDFWindow_TagBase		    ( TAG_USER + 0x9000 )

#define	MUIM_Thumbnail_Dummy            ( OM_Dummy + 0xa000 )
#define MUIA_Thumbnail_TagBase	        ( TAG_USER + 0xa000 )

#define	MUIM_ThumbnailList_Dummy        ( OM_Dummy + 0xb000 )
#define MUIA_ThumbnailList_TagBase	    ( TAG_USER + 0xb000 )

#define	MUIM_Lay_Dummy                  ( OM_Dummy + 0xb000 )
#define MUIA_Lay_TagBase                ( TAG_USER + 0xb000 )

#define	MUIM_Logger_Dummy               ( OM_Dummy + 0xc000 )
#define MUIA_Logger_TagBase             ( TAG_USER + 0xc000 )

#define	MUIM_VPDFSettings_Dummy         ( OM_Dummy + 0xd000 )
#define MUIA_VPDFSettings_TagBase       ( TAG_USER + 0xd000 )

#define	MUIM_VPDFTitle_Dummy            ( OM_Dummy + 0xe000 )
#define MUIA_VPDFTitle_TagBase          ( TAG_USER + 0xe000 )

#define MUIM_VPDFPrinter_Dummy          ( OM_Dummy + 0xf000 )
#define MUIA_VPDFPrinter_TagBase        ( TAG_USER + 0xf000 )

#define MUIM_Annotation_Dummy           ( OM_Dummy + 0x1100 )
#define MUIA_Annotation_TagBase         ( TAG_USER + 0x1100 )

#define MUIM_VPDFNumberGenerator_Dummy           ( OM_Dummy + 0x2100 )
#define MUIA_VPDFNumberGenerator_TagBase         ( TAG_USER + 0x2100 )

#define DEFCLASS(s) ULONG create_##s##Class(void); \
	APTR get##s##Class(void); \
	APTR get##s##Classroot(void); \
	void delete_##s##Class(void)

#endif
