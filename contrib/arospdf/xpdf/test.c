#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <libraries/mui.h>
#include <utility/hooks.h>
static struct Hook myhook;

AROS_UFH3(ULONG, myfunction,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(APTR, msg, A1))
{
    AROS_USERFUNC_INIT

    return 0;
    AROS_USERFUNC_EXIT
}

int main(void)
{
    myhook.h_Entry = (HOOKFUNC)myfunction;
return 1;
}

