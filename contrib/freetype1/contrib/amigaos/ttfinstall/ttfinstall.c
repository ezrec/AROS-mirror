/*
 * Based on the code from the ttf.library by Richard Griffith.
 */
/* ttfinstall - install truetype font(s) to a specified directory */

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <diskfont/diskfonttag.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <freetype/freetype.h>
#include <freetype/ftxerr18.h> /* error strings */

#include "ttfinscore.h"

//static char VERTAG[] = "\0$VER: ttfinstall 0.8.5 " __AMIGADATE__;

struct AnchorPath MyAp;

/* readargs stuff */
#define TEMPLATE          "FROM/A,TO/A"
#define OPT_FROM          0
#define OPT_TO            1

#define OPT_COUNT         2

LONG opts[OPT_COUNT];		/* C guarantees this will be all 0's! */


/* freetype related items */

static  TT_Error     error;

void printmsg(void)
{
    printf("%s\n",message_buffer);
}

LONG do_pattern(char *pattern,int all, int verbose)
{
    LONG retval = RETURN_ERROR;
    char dirname[256];

    /* Initialize the AnchorPath structure	*/
    MyAp.ap_BreakBits = SIGBREAKF_CTRL_C; /* Break on these bits	*/

    for (retval = MatchFirst(pattern,&MyAp);
	 retval == 0;
	 retval = MatchNext(&MyAp))
    {
	/* if (MyAp.ap_Flags & APF_DirChanged)
		{
			printf("Changed directories...\n");
		}
*/
	if (NameFromLock(MyAp.ap_Current->an_Lock,dirname,
			 sizeof(dirname)) == FALSE)
	{
	    PrintFault(IoErr(),"Error on NameFromLock");
	    break;
	}
	if (AddPart(dirname,&(MyAp.ap_Info.fib_FileName[0]),
		    sizeof(dirname)) == FALSE)
	{
	    PrintFault(IoErr(),"Error on AddPart");
	    break;
	}

	/* This code section deals with ALL and directory ascent/descent */
	if (MyAp.ap_Info.fib_DirEntryType > 0)
	{
	    if (MyAp.ap_Flags & APF_DIDDIR)
	    {
		/*printf("Ascending from directory %s\n",dirname);*/
	    }
	    else if (all)
	    {
		if(verbose)	printf("\n\nEntering directory %s\n\n",dirname);

		/* make it enter the directory */
		MyAp.ap_Flags |= APF_DODIR;
	    }
	    else
	    {
		/*printf("The next dir is  ... %s\n",dirname); */
	    }

	    /* clear the completed directory flag */
	    MyAp.ap_Flags &= ~APF_DIDDIR;
	}
	else
	{
	    /* Here is code for handling each particular file */

	    printf("Installing %s ",dirname);
	    if(ttfinst(dirname,NULL)) printmsg();
	    else printf("to %s\n",install_font);
	}
    }

    /* This absolutely, positively must be called, all of the time. */
    MatchEnd(&MyAp);

    /* Check for error, if so, print error message */
    if (retval != ERROR_NO_MORE_ENTRIES)
    {
	printf("FROM %s expansion failed -  Error code %ld\n"
	       ,pattern,retval);
		
	PrintFault(retval,NULL);
    }
    return retval;
}

int main (int argc, char **argv)
{
    struct RDArgs *argsptr;

    char *from, *to;
    int all=0;
    int verbose=0;

    argsptr = ReadArgs(TEMPLATE, opts, NULL);

    if (argsptr == NULL)
    {
        PrintFault(IoErr(), NULL);	/* prints the appropriate err message */
	return RETURN_ERROR;
    }
    else
    {
	from = (char *) (opts[OPT_FROM]);
	to   = (char *) (opts[OPT_TO]);

	printf("ttfinstall from=%s to=%s\n\n",from,to);

	if(sanctify_to_dir(to))
	{
	    printmsg();
	    return RETURN_ERROR;
	}

	if(initialize_freetype())
	{
	    printmsg();
	    return RETURN_ERROR;
	}
	
	do_pattern(from,all,verbose);

	/* cleanup */
	FreeArgs(argsptr);
	terminate_freetype();
    }

    return RETURN_OK;		/* program succeeded */
}
