#ifndef Lay_MCC_H
#define	Lay_MCC_H

#include "classes.h"

#define	MUIA_Lay_HorizSpacing		 ( MUIA_Lay_TagBase + 1 )
#define	MUIA_Lay_VertSpacing	     ( MUIA_Lay_TagBase + 2 )
#define	MUIA_Lay_Center     	     ( MUIA_Lay_TagBase + 3 )
#define	MUIA_Lay_KeepPosition        ( MUIA_Lay_TagBase + 4 )

#if defined(__AROS__)
#define LayObject BOOPSIOBJMACRO_START( getLayClass() )
#else
#define	LayObject NewObject(getLayClass(), NULL
#endif

DEFCLASS(Lay);

#endif
