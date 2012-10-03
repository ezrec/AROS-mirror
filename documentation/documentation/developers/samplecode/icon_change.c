/*
    Example for changing icon tooltypes.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>

#include <workbench/startup.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static STRPTR conwinname   = "CON:30/50/500/400/Tooltype parsing/AUTO/CLOSE/WAIT";
static BPTR conwin;
static BPTR olddir = (BPTR)-1;
static struct DiskObject *dobj;
static STRPTR *oldtoolarray, *newtoolarray;

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
        struct WBStartup *wbmsg = (struct WBStartup *)argv;
        struct WBArg *wbarg = wbmsg->sm_ArgList;
        STRPTR *toolarray;
        LONG toolcnt = 0;
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
        
        if ( (wbarg->wa_Lock) && (*wbarg->wa_Name) )
        {
            fprintf(conwin, "Trying to open %s\n", wbarg->wa_Name);
            
            /* We must enter the directory of the icon */
            olddir = CurrentDir(wbarg->wa_Lock);

            dobj = GetDiskObject(wbarg->wa_Name);
            if (dobj)
            {
                /*
                    Remember the old toolarray, so that we can put it back later.
                */
                oldtoolarray = dobj->do_ToolTypes;
            
                /* Count entries */
                if (oldtoolarray)
                {
                    toolarray = oldtoolarray;
                    while (*toolarray)
                    {
                        toolcnt++;
                        toolarray++;
                    }
                }
                fprintf(conwin, "Old icon has %d tooltype entries\n", toolcnt);
                
                /* Create new toolarray */
                newtoolarray = AllocVec(sizeof(STRPTR) * (toolcnt + 3), MEMF_ANY);
                if (!newtoolarray)
                {
                    clean_exit("Can't allocate memory for new toolarray");
                }
                /*
                    Add two new entries and copy the pointers to the
                    old values. If w'd want to change the strings we'd
                    have to work with copies of the strings.
                */
                newtoolarray[0] = "START";
                for (idx = 0 ; idx < toolcnt; idx++)
                {
                    newtoolarray[idx+1] = oldtoolarray[idx];
                }
                newtoolarray[toolcnt + 1] = "END";
                newtoolarray[toolcnt + 2] = NULL;
                
                /* Change toolarray pointer and save icon. */
                dobj->do_ToolTypes = newtoolarray;
                if (!PutDiskObject(wbarg->wa_Name, dobj))
                {
                    clean_exit("Can't write Diskobject");
                }
            }
            else
            {
                clean_exit("Can't open DiskObjekt");
            }
        }
    }

    clean_exit(NULL);

    return 0;
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

    /*
        Free DiskObject. We have to set back the pointer to the toolarray or
        we'll get memory corruption.
    */
    if (dobj)
    {
        dobj->do_ToolTypes = oldtoolarray;
        FreeDiskObject(dobj);
    }
    
    FreeVec(newtoolarray);
    
    /*
        Switch back to old directory. It's important that the directory which
        was active at program start is set when the application is quit.
    */
    if (olddir != (BPTR)-1)
    {
        CurrentDir(olddir);
        olddir = (BPTR)-1;
    }
    
    exit(0);
}
