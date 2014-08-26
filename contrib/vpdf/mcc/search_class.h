
#ifndef	SEARCH_MCC_CLASS_H
#define	SEARCH_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"

#define	MUIA_Search_DocumentView  	 ( MUIA_Search_TagBase + 1 )
#define	MUIA_Search_Info		  	 ( MUIA_Search_TagBase + 2 )

#define	MUIM_Search_Search           ( MUIM_Search_Dummy + 1 )

struct MUIP_Search_Search{ULONG MethodID; STRPTR phrase; LONG direction;};

#if defined(__AROS__)
#define SearchObject   BOOPSIOBJMACRO_START( getSearchClass() )
#else
#define	SearchObject   NewObject( getSearchClass() , NULL
#endif

#define MUIV_Search_Search_Next 0
#define MUIV_Search_Search_Previous 1

DEFCLASS(Search);

#endif
