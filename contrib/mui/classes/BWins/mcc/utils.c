
#include "class.h"

/****************************************************************************/

ULONG STDARGS
DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...)
{
    return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}

/***********************************************************************/
