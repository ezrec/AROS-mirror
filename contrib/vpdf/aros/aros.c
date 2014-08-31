#include <utility/tagitem.h>
#include <intuition/classusr.h>
#include <clib/alib_protos.h>

#include <proto/intuition.h>

#include <stdlib.h>

IPTR DoSuperNew(struct IClass *cl, Object *obj, IPTR tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}


void *mmalloc(int size)
{
    return malloc(size);
}

//void *mcalloc(int size, int num);
void mfree(void *mem)
{
    free(mem);
}
