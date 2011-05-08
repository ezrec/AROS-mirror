/*
    Copyright © 2008-2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/peropener.h>
#include <proto/peropener2.h>
#include <proto/perid.h>

struct Library *PeropenerBase = NULL;

int main (int argc, char ** argv)
{
    struct Library *base1, *base2;
    BPTR seglist;
    
    FPuts(Output(), (STRPTR)"Testing peropener.library\n");
    
    base1=OpenLibrary((STRPTR)"peropener.library",0);
    base2=OpenLibrary((STRPTR)"peropener.library",0);

    FPrintf(Output(), (STRPTR)"base1=%lx, base2=%lx\n", base1, base2);

    if (base1 == NULL || base2 == NULL)
        return 20;
    
    PeropenerBase = base1;
    pob_settestvalue(1);
    
    PeropenerBase = base2;
    pob_settestvalue(2);
    
    PeropenerBase = base1;
    FPrintf(Output(), (STRPTR)"testvalue base1: %ld (== 1?)\n", pob_gettestvalue());
    
    PeropenerBase = base2;
    FPrintf(Output(), (STRPTR)"testvalue base2: %ld (== 2?)\n", pob_gettestvalue());

    PeropenerBase = NULL;
    
    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);

    FPuts(Output(), (STRPTR)"Testing peropener2.library\n");
    
    base1=OpenLibrary((STRPTR)"peropener2.library",0);
    base2=OpenLibrary((STRPTR)"peropener2.library",0);

    FPrintf(Output(), (STRPTR)"base1=%lx, base2=%lx\n", base1, base2);

    if (base1 == NULL || base2 == NULL)
        return 20;
    
    Peropener2Base = base1;
    pob2_settestvalue(1);
    
    Peropener2Base = base2;
    pob2_settestvalue(2);
    
    Peropener2Base = base1;
    FPrintf(Output(), (STRPTR)"testvalue base1: %ld (== 1?)\n", pob2_gettestvalue());
    
    Peropener2Base = base2;
    FPrintf(Output(), (STRPTR)"testvalue base2: %ld (== 2?)\n", pob2_gettestvalue());

    Peropener2Base = NULL;
    
    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);

    FPuts(Output(), (STRPTR)"\nTesting perid.library\n");

    base1=OpenLibrary((STRPTR)"perid.library",0);
    base2=OpenLibrary((STRPTR)"perid.library",0);
    
    FPrintf(Output(), (STRPTR)"base1=%lx, base2=%lx\n", base1, base2);

    seglist = LoadSeg((CONST_STRPTR)"peropenertest_child");
    if (seglist != (BPTR)NULL)
    {
        RunCommand(seglist, 10*1024, "\n", -1);
        UnLoadSeg(seglist);
    }
    else
    {
        FPrintf(Output(), (STRPTR)"Failed to load peropenertest_child\n");
    }
    
    if (base1 != NULL)
        CloseLibrary(base1);
    if (base2 != NULL)
        CloseLibrary(base2);

    Flush (Output ());
    
    return 0;
}
