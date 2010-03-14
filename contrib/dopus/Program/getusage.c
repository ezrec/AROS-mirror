#include <proto/exec.h>
#include <exec/execbase.h>


static ULONG oldidle,olddisp;

int getusage(void)
{
    LONG idlediff, dispdiff;
    
    idlediff = SysBase->IdleCount - oldidle;
    dispdiff = SysBase->DispCount - olddisp;
    
    oldidle = SysBase->IdleCount;
    olddisp = SysBase->DispCount;

    return ((idlediff+dispdiff)?(dispdiff * 100 / (idlediff + dispdiff)):100);
    
}
