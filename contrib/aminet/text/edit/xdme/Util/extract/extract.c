
/*#include <dos/dosasl.h>*/
#include <string.h>
#include <stdio.h>
#include <errno.h>
/*
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
*/
#ifdef __SAS
extern void __regargs __chkabort (void);
/* disable ^C checking */
void __regargs __chkabort (void) { /* empty */ }
#endif


/*__aligned struct AnchorPath ap;*/
char search[64] = "";
int searchlen;
int ccomment;
char line[256];

int stop;

void ProcessFile (const char * filename)
{
    int do_put;
    FILE * fh;
    char * ptr, * ptr2;

    if (!(fh = fopen (filename, "r")) )
    {
	printf ("Cannot open %s for reading: %s\n",
		filename, strerror (errno));
	return;
    }

    do_put = 0;
    stop = 0;

    while (fgets (line, sizeof (line), fh))
    {
#if 0 // Not necessary on Unix
	if (SetSignal (0,0) & SIGBREAKF_CTRL_C)
	{
	    SetSignal (0, SIGBREAKF_CTRL_C);
	    puts ("*** Break");
	    stop = 1;
	    break;
	}
#endif

	ptr = line;

	if (!strncmp (search, line, searchlen))
	{
	    do_put = 1;
	    ptr += searchlen+1;
	}

	if (do_put)
	{
	    if (!ccomment)
	    {
		fputs (line, stdout);
		do_put = 0;
	    }
	    else
	    {
		ptr2 = ptr;
		while (*ptr2)
		{
		    if (ptr2[0] == '*' && ptr2[1] == '/')
			break;

		    ptr2 ++;
		}

		if (*ptr2)
		{
		    do_put = 0;
		    *ptr2 = 0;
		}

		fputs (ptr, stdout);

		if (!do_put)
		    putchar ('\n');
	    }
	}
    }

    fclose (fh);
} /* ProcessFile */

#define NEXT	argv ++, argc --

int main (int argc, char ** argv)
{
    int error;

//    ap.ap_BreakBits  = SIGBREAKF_CTRL_C;

    NEXT;

    if (!argc)
	return 0;

    strcat (search, *argv);
    searchlen = strlen (search);

    ccomment = (search[0] == '/' && search[1] == '*');

    NEXT;

    while (argc)
    {
#if 0
	BPTR fl;

	for (error = MatchFirst (*argv, &ap); !error; error = MatchNext (&ap))
	{
	    fl = CurrentDir (ap.ap_Current->an_Lock);
	    ProcessFile ();
	    CurrentDir (fl);

	    if (stop)
		break;
	}

	MatchEnd (&ap);

	if (stop)
	    break;
#endif
	ProcessFile (*argv);

	NEXT;
    }

    return 0;
} /* main */
