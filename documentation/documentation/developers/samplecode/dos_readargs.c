/*
    Example for ReadArgs() command line parsing
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <stdlib.h>
#include <stdio.h>

/*
    Comma separated list of command line arguments.
    See Documentation/User/Shell commands/Introduction for
    possible flags. This flags influence how the result
    will be stored.
    Example call for this application:
      dos_readargs abcde 45 bool
*/
#define ARG_TEMPLATE "STRING/A,NUMBER/N,BOOL/S"

/*
    Give the arguments index names. The value of
    ARG_COUNT is the number of arguments. Of course,
    this must be synchronized with the template string.
*/
enum
{
    ARG_STRING,
    ARG_NUMBER,
    ARG_BOOL,
    ARG_COUNT
};

struct RDArgs *rda;
BOOL started_from_wanderer;

static void clean_exit(CONST_STRPTR s);


int main(int argc, char **argv)
{
    if (argc)
    {
        /*
            Storage place for the result. It's important to initialize
            the array with default values.
        */
        IPTR args[ARG_COUNT] = {0};
        
        rda = ReadArgs(ARG_TEMPLATE, args, NULL);
        if (!rda)
        {
            PrintFault(IoErr(), argv[0]);
            clean_exit("ReadArgs() failed.");
        }
        
        if (args[ARG_STRING])
        {
            /* Array element is a pointer to a string */
            printf("Argument 'STRING' %s\n", (STRPTR)args[ARG_STRING]);
        }
        else
        {
            puts("Argument 'STRING' wasn't given.");
        }
        
        if (args[ARG_NUMBER])
        {
            /* Array element is a pointer to a number */
            printf("Argument 'NUMBER' %d\n", *(LONG *)args[ARG_NUMBER]);
        }
        else
        {
            puts("Argument 'NUMBER' wasn't given.");
        }
        
        /* Array element contains the boolean result */
        printf("Argument 'BOOL' %s.\n", args[ARG_BOOL] ? "TRUE" : "FALSE");
        
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
    if (rda) FreeArgs(rda);
    
    exit(0);
}
