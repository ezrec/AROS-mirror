/*

$VER: F_Alloc 2.00 (2005/08/05)

   Written by Olivier LAVIALE (gofromiel@gofromiel.com)

   This tool is for debuggin purpose only !! It travels  trough  pools  and
   puddles to print every memory allocated by F_New() or F_NewP().

*/

#include <exec/execbase.h>
#if !defined(__AROS__)
#include "Feelin:Sources/Feelin/Private.h"
#else
#include "Sources/feelin/Private.h"
#endif

#undef SysBase
#undef DOSBase

#include <proto/exec.h>
#include <proto/dos.h>

extern struct ExecBase             *SysBase;
struct in_FeelinBase               *FeelinInternalBase;

int main()
{
    if (!FindName(&SysBase -> LibList,"feelin.library"))
    {
        Printf("Feelin is not running.\n");
    }
    else if (FeelinInternalBase = (struct in_FeelinBase *) OpenLibrary("feelin.library",FV_FEELIN_VERSION))
    {
        FPool *pool;
        FPuddle *puddle;
        FMemChunk *chunk;
        ULONG s1,s2,s3,n1=0,n2=0,n3=0,f=0,
              args[] = { FALSE, 0};
        APTR  rdargs;
        
        uint32 spreading=0;
        uint32 i;
      
/*** read arguments ********************************************************/

        if (rdargs = ReadArgs("T=TREE/S",(LONG *)(&args),NULL))
        {
            FreeArgs(rdargs);
        }

        Printf("[2m[41m                                              [0m\n");
        Printf("[2m[41m !! This tool is for debuggin purpose only !! [0m\n");
        Printf("[2m[41m                                              [0m\n\n");

        if (args[0] == FALSE)
        {
            Printf("(Use 'T' or 'TREE' switch for more informations)\n\n");
        }

        s1 = 0;

        for (pool = FeelinInternalBase -> pools ; pool ; pool = pool -> next)
        {
            if (args[0]) Printf("[33mPool[0m 0x%08lx %s (%s) ",pool,(pool -> Semaphore.ss_Link.ln_Name) ? pool -> Semaphore.ss_Link.ln_Name : "Private",(pool -> Semaphore.ss_Owner) ? "locked" : "unlocked");

            s2 = 0;

            if ((puddle = pool -> puddles) != NULL)
            {
                ULONG pf = 0;
     
                if (args[0]) Printf("\n");

                for ( ; puddle ; puddle = puddle -> next)
                {
                    if (args[0]) Printf("   [33mPuddle[0m 0x%08.lx - [1mSize[0m %ld\n",puddle,puddle -> size - sizeof (struct FeelinPuddle));

                    s3 = 0; pf += puddle -> free;

                    for (chunk = puddle -> chunks ; chunk ; chunk = chunk -> next)
                    {
                        if (args[0]) Printf("      [33mChunk[0m 0x%08.lx (0x%08.lx) - Next 0x%08.lx - Size %ld%s\n",chunk,(ULONG)(chunk) + sizeof (struct FeelinMemChunk),chunk -> next,chunk -> size,(chunk == puddle->middle) ? " (middle)" : "");

                        n3++; s3 += chunk -> size;

                        if (SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                        {
                           Printf("*** Break\n"); return;
                        }
                    }

                    if (args[0]) Printf("   [1mUsed[0m %ld - [1mFree[0m %ld\n",s3,puddle -> free);

                    n2++; s2 += s3; f += puddle -> free;
                }
                if (args[0]) Printf("[1mUsed[0m %ld - [1mFree[0m %ld\n\n",s2,pf);
            }
            else
            {
               if (args[0]) Printf(" is empty\n");
            }

            n1++; s1 += s2;
        }

        if (args[0]) Printf("\n");
        
        for (i = 0 ; i < FV_MEMORY_HASH_SIZE + 1; i ++)
        {
            if (FeelinInternalBase->hash_puddles[i])
            {
                spreading++;
            }
        }
 
        Printf("[1mUsed[0m %ld bytes (%ld%%), [1mFree[0m %ld bytes (%ld%%), [1mTotal[0m %ld bytes.\n",s1,s1 * 100 / (s1+f),f,f * 100 / (s1+f),s1+f);
        Printf("in %ld [1mPools[0m, %ld [1mPuddles[0m and %ld [1mChunks[0m. Puddles spreading %ld%%.\n",n1,n2,n3,spreading * 100 / n2);

#define FeelinBase FeelinInternalBase

        F_FEELIN_CLOSE;
    }
    return 0;
}
