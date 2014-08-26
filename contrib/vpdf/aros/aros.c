#include <utility/tagitem.h>

IPTR DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}
