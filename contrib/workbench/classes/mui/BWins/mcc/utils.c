
#include "class.h"

/****************************************************************************/

IPTR STDARGS
DoSuperNew(struct IClass *cl,Object *obj,Tag tag1,...)
{
#ifdef __AROS__
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = DoSuperMethod(cl,obj,OM_NEW,AROS_SLOWSTACKTAGS_ARG(tag1), NULL);
    AROS_SLOWSTACKTAGS_POST
#else
    return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
#endif
}

/***********************************************************************/
