#include <exec/types.h>
#include <exec/memory.h>
#include <proto/exec.h>

struct Library *Warp3DBase;

#include <inline/macros.h>
#define W3D_Settings() LP0(24,ULONG,W3D_Settings,,Warp3DBase)

void main(int argc, char **argv)
{
    Warp3DBase = OpenLibrary("Warp3D.library", 4L);
    if (Warp3DBase)
    {
    W3D_Settings();
    CloseLibrary(Warp3DBase);
    }
    exit(0);
}
