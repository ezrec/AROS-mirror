/*
    Example for ReadArgs() command line parsing
    with help text
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <stdlib.h>
#include <stdio.h>

/*
    For this example we're using only a simple template.
    See dos_readargs.c for detailled example for a template.
*/
#define ARG_TEMPLATE "STRING"

enum
{
    ARG_STRING,
    ARG_COUNT
};

struct RDArgs *rda;
BOOL started_from_wanderer;

static void clean_exit(CONST_STRPTR s);


int main(int argc, char **argv)
{
    if (argc)
    {
        IPTR args[ARG_COUNT] = {0};
        
        rda = (struct RDArgs *)AllocDosObject(DOS_RDARGS, NULL);
        if (!rda)
        {
            clean_exit("Can't allocate RDArgs");
        }

        rda->RDA_ExtHelp =
            "This text will be printed if the user calls this\n"
            "command with a question mark as argument and types\n"
            "again a question mark when the command requests\n"
            "the arguments.\n";
        
        if (!ReadArgs(ARG_TEMPLATE, args, rda))
        {
            PrintFault(IoErr(), argv[0]);
            clean_exit("ReadArgs() failed.");
        }
        
        if (args[ARG_STRING])
        {
            printf("Argument 'STRING' %s\n", (STRPTR)args[ARG_STRING]);
        }
    }
    else
    {
        started_from_wanderer = TRUE;
        clean_exit("Application must be started from Shell.");
    }

    clean_exit(NULL);

    return 0;
}



static void clean_exit(CONST_STRPTR s)
{
    if (s)
    {
        if (started_from_wanderer)
        {
            /* 
                We use an EasyRequest because applications started
                from Wanderer don't have an output console by default.
            */
            struct EasyStruct es = {
                sizeof(struct EasyStruct),
                0,
                "Error",
                s,
                "OK"
            };
            EasyRequest(NULL, &es, NULL);
        }
        else
        {
            puts(s);
        }
    }
    
    // Give back allocated resourses
    if (rda)
    {
        FreeArgs(rda);
        FreeDosObject(DOS_RDARGS, rda);
    }
    
    exit(0);
}
