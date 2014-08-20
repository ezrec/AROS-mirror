#ifndef Annotation_MCC_H
#define	Annotation_MCC_H

#include "classes.h"

//#define	MUIA_Lay_HorizSpacing		 ( MUIA_Lay_TagBase + 1 )
//#/define	MUIA_Lay_VertSpacing	     ( MUIA_Lay_TagBase + 2 )
//#define	MUIA_Lay_Center     	     ( MUIA_Lay_TagBase + 3 )
#define	MUIA_Annotation_LayoutPosX      ( MUIA_Annotation_TagBase + 2 )
#define	MUIA_Annotation_LayoutPosY      ( MUIA_Annotation_TagBase + 3 )
#define	MUIA_Annotation_Contents        ( MUIA_Annotation_TagBase + 4 )
#define	MUIA_Annotation_PosX            ( MUIA_Annotation_TagBase + 5 ) 
#define	MUIA_Annotation_PosY            ( MUIA_Annotation_TagBase + 6 )
#define MUIA_Annotation_RefObject       ( MUIA_Annotation_TagBase + 7 )

#define MUIM_Annotation_Toggle          ( MUIM_Annotation_Dummy + 1 )

struct MUIP_Annotation_Toggle{ULONG MethodID;};

#define	AnnotationObject NewObject(getAnnotationClass(), NULL

DEFCLASS(Annotation);

#endif
