;/*
   F_Create.rexx EXE F_Dump
   QUIT
   _________________________________________________________________________

   F_Dump 1.00 (2003/02/10)

   Written by Olivier LAVIALE (gofromiel@gofromiel.com)

   This tool is for debuggin purpose only !! It will travel trough pools and
   puddles to dump memory allocations in several files.
*/

#include <exec/execbase.h>
#include "Feelin:Sources/Feelin/Private.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#undef SysBase
#undef DOSBase
#undef UtilityBase

extern struct ExecBase             *SysBase;
struct in_FeelinBase               *FeelinBase;
#define UtilityBase                 FeelinBase -> Public.Utility

void main(void)
{
    struct FeelinPool      *pool;
    struct FeelinPuddle    *puddle;
    struct FeelinMemChunk  *chunk;
    static char             str[256];
    BPTR                    file;
    ULONG                   args[] = { NULL, 0};
    APTR                    rdargs;

    if (FeelinBase = (APTR) FindName(&SysBase -> LibList,"feelin.library"))
    {
        Execute("Delete >NIL: T:F_DUMP_#?",NULL,NULL);

        rdargs = ReadArgs("N=NAME",(LONG *)(&args),NULL);

        Printf("[2m[41m                                              [0m\n");
        Printf("[2m[41m !! This tool is for debuggin purpose only !! [0m\n");
        Printf("[2m[41m                                              [0m\n\n");

        if (!args[0])
        {
            Printf("(Use 'N' or 'NAME' to save specific pools)\n\n");
        }

        for (pool = (APTR)(FeelinBase -> PoolsList.Head) ; pool ; pool = pool -> Next)
        {
            if (!args[0] || (args[0] && (Stricmp((STRPTR)(args[0]),pool -> Semaphore.ss_Link.ln_Name) == 0)))
            {
                for (puddle = (APTR)(pool -> PuddleList.Head) ; puddle ; puddle = puddle -> Next)
                {
                    for (chunk = puddle -> First ; chunk ; chunk = chunk -> Next)
                    {
                        Printf("F_Dump: Chunk 0x%lx - Pool 0x%lx %s\n",chunk,pool,pool -> Semaphore.ss_Link.ln_Name);

                        if (pool -> Semaphore.ss_Link.ln_Name)
                        {
                            F_StrFmt(str,"T:F_DUMP_%s_%08.lx",pool -> Semaphore.ss_Link.ln_Name,chunk);
                        }
                        else
                        {
                            F_StrFmt(str,"T:F_DUMP_private_%08.lx",chunk);
                        }

                        if (file = Open((STRPTR)(str),MODE_NEWFILE))
                        {
                            Write(file,(APTR)((ULONG)(chunk) + sizeof (struct FeelinMemChunk)),chunk -> Size - sizeof (struct FeelinMemChunk));
                            Close(file);
                            SetComment(str,pool -> Semaphore.ss_Link.ln_Name);
                        }
                        else
                        {
                            Printf("Unable to open file '%s'\n",str);
                        }

                        if (SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                        {
                            Printf("*** Break\n"); return;
                        }
                    }
                }
            }
        }

        if (rdargs) FreeArgs(rdargs);
    }
    else
    {
        Printf("Feelin is not running.\n");
    }
}
