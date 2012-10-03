/*
    Example of starting from Wanderer with icon handling.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>

#include <workbench/startup.h>

#include <stdlib.h>
#include <stdio.h>


static STRPTR conwinname   = "CON:30/50/500/400/Tooltype parsing/AUTO/CLOSE/WAIT";
static BPTR conwin;
static BPTR olddir = (BPTR)-1;

static void read_icon(struct WBArg *wbarg);
static void clean_exit(CONST_STRPTR s);


int main(int argc, char **argv)
{
    if (argc)
    {
        clean_exit("Application must be started from Wanderer.");
        /* See dos_readargs.c for start from Shell. */
    }
    else
    {
        struct WBArg *wbarg;
        struct WBStartup *wbmsg = (struct WBStartup *)argv;
        LONG idx;
        
        /*
            An application started from Wanderer doesn't have a console window
            for output. We have to open our own con: window or all output will
            go to Nirwana.
        */
        conwin = fopen(conwinname, "w");
        if (!conwin)
        {
            clean_exit("Can't open console window");
        }
        
        /* Loop trough all icons. The first one is the application itself. */
        for
        (
            idx = 0, wbarg = wbmsg->sm_ArgList;
            idx < wbmsg->sm_NumArgs;
            idx++, wbarg++
        )
        {
            if ( (wbarg->wa_Lock) && (*wbarg->wa_Name) )
            {
                fprintf(conwin, "\n-------------------\nName %s\n", wbarg->wa_Name);
                
                /* We must enter the directory of the icon */
                olddir = CurrentDir(wbarg->wa_Lock);
                
                read_icon(wbarg);
                
                /*
                    Switch back to old directory. It's important that the
                    directory which was active at program start is set when the
                    application is quit.
                */
                if (olddir != (BPTR)-1)
                {
                    CurrentDir(olddir);
                    olddir = (BPTR)-1;
                }
            }
        }
    }

    clean_exit(NULL);

    return 0;
}


static void read_icon(struct WBArg *wbarg)
{
    struct DiskObject *dobj;
    STRPTR *toolarray;
    STRPTR result;

    /* Let's read some tooltypes from the icon */
    dobj = GetDiskObject(wbarg->wa_Name);
    if (dobj)
    {
        toolarray = dobj->do_ToolTypes;

        result = FindToolType(toolarray, "PUBSCREEN");
        if (result)
        {
            fprintf(conwin, "PUBSCREEN: %s\n", result);
        }
        else
        {
            fputs("Tooltype 'PUBSCREEN' doesn't exist\n", conwin);
        }
        
        result = FindToolType(toolarray, "COLOR");
        if (result)
        {
            if (MatchToolValue(result, "RED"))
            {
                fputs("Mode 'RED' set\n", conwin);
            }
            if (MatchToolValue(result, "GREEN"))
            {
                fputs("Mode 'GREEN' set\n", conwin);
            }
            if (MatchToolValue(result, "BLUE"))
            {
                fputs("Mode 'BLUE' set\n", conwin);
            }
        }
        else
        {
            fputs("Tooltype 'COLOR' doesn't exist\n", conwin);
        }           
            
        FreeDiskObject(dobj);
    }
    else
    {
        fputs("Can't open DiskObjekt\n", conwin);
    }
}


static void clean_exit(CONST_STRPTR s)
{
    if (s)
    {
        if (conwin)
        {
            fprintf(conwin, "%s\n", s);
        }
        else
        {
            printf("%s\n", s);
        }
    }
    
    /* Give back allocated resourses */
    if (conwin) fclose(conwin);
    
    exit(0);
}
