
#ifndef	SEARCH_MCC_CLASS_H
#define	SEARCH_MCC_CLASS_H

#include "system/chunky.h"
#include "classes.h"

#define	MUIA_Search_DocumentView  	 ( MUIA_Search_TagBase + 1 )
#define	MUIM_Search_Search           ( MUIM_Search_Dummy + 1 )

struct MUIP_Search_Search{ULONG MethodID; STRPTR phrase; LONG direction;};


#define	SearchObject   NewObject( getSearchClass() , NULL

#define MUIV_Search_Search_Next 0
#define MUIV_Search_Search_Previous 1

DEFCLASS(Search);

#endif
