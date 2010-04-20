#include <aros/debug.h>
#include <proto/exec.h>

void _abort(char *file, unsigned int line)
{
    kprintf("*** abort() in file %s at line %u\n", file, line);
    Wait(0);
}
