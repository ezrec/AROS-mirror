#include "__arosc_privdata.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dosextens.h>
#include <sys/arosc.h>
#include <aros/symbolsets.h>
#include <stdlib.h>

#include "arosc_init.h"

#include "__ctype.h"
#include "etask.h"

#include <aros/debug.h>

extern struct aroscbase *aroscbase;

/* Provide local aroscbase for the static version of arosc */
#ifdef AROSC_STATIC
static struct aroscbase __aroscbase;
struct aroscbase *aroscbase = &__aroscbase;
#endif

struct arosc_userdata *__get_arosc_userdata(void)
{
#ifdef AROSC_SHARED
    struct aroscbase *aroscbase = __get_aroscbase();
#endif
#ifdef AROSC_STATIC
    static __init = 0;
    
    if(!__init)
    {
        __init = 1;
        
        __ctype_init();
    }
#endif
    
    return &(aroscbase->acb_acud);
}
