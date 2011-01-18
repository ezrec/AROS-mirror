/******************************************************************************

    MODUL
	CnvConfig.c

    DESCRIPTION
	Converts an old config-file into the actual version or prints an
	old version-file to stdout.

    NOTES

    BUGS

    TODO

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	30. Mar 1993	ada created

******************************************************************************/

/**************************************
		Includes
**************************************/
#include <exec/types.h>
#include <exec/nodes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pragmas/dos_pragmas.h>

#ifndef DOS_DOS_H
typedef void * BPTR;
#endif
/* typedef struct Node NODE; */

#include "global.h"


/**************************************
	    Globale Variable
**************************************/
extern struct DOSBase * DOSBase;


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/
void convert_config0001 (void);
void convert_config0002 (void);
void convert_config0003 (void);
void print_config (void);


struct Config new;
struct Config0001 c1;
struct Config0002 c2;
struct Config0003 c3;


void convert_config0001 (void)
{
    new.wwcol = c1.wwcol;
    new.winx = c1.winx;
    new.winy = c1.winy;
    new.winwidth = c1.winwidth;
    new.winheight = c1.winheight;
    new.iwinx = c1.iwinx;
    new.iwiny = c1.iwiny;
    new.aslleft = c1.aslleft;
    new.asltop = c1.asltop;
    new.aslwidth = c1.aslwidth;
    new.aslheight = c1.aslheight;
    new.tabstop = c1.tabstop;
    new.margin = c1.margin;
    new.fgpen = c1.fgpen;
    new.bgpen = c1.bgpen;
    new.hgpen = c1.hgpen;
    new.bbpen = c1.bbpen;
    new.tfpen = c3.fgpen;
    new.tbpen = c3.tpen;
    new._insertmode = c1.insertmode;
    new._ignorecase = c1.ignorecase;
    new._wordwrap = c1.wordwrap;
    new._autosplit = c1.autosplit;
    new._autoindent = c1.autoindent;
    new._autounblock = c1.autounblock;
} /* convert_config0001 */


void convert_config0002 (void)
{
    new.wwcol = c2.wwcol;
    new.winx = c2.winx;
    new.winy = c2.winy;
    new.winwidth = c2.winwidth;
    new.winheight = c2.winheight;
    new.iwinx = c2.iwinx;
    new.iwiny = c2.iwiny;
    new.aslleft = c2.aslleft;
    new.asltop = c2.asltop;
    new.aslwidth = c2.aslwidth;
    new.aslheight = c2.aslheight;
    new.tabstop = c2.tabstop;
    new.margin = c2.margin;
    new.fgpen = c2.fgpen;
    new.bgpen = c2.bgpen;
    new.hgpen = c2.hgpen;
    new.bbpen = c2.bbpen;
    new.tfpen = c3.fgpen;
    new.tbpen = c3.tpen;
    new._insertmode = c2.insertmode;
    new._ignorecase = c2.ignorecase;
    new._wordwrap = c2.wordwrap;
    new._autosplit = c2.autosplit;
    new._autoindent = c2.autoindent;
    new._autounblock = c2.autounblock;
} /* convert_config0002 */


void convert_config0003 (void)
{
    new.wwcol = c3.wwcol;
    new.winx = c3.winx;
    new.winy = c3.winy;
    new.winwidth = c3.winwidth;
    new.winheight = c3.winheight;
    new.iwinx = c3.iwinx;
    new.iwiny = c3.iwiny;
    new.aslleft = c3.aslleft;
    new.asltop = c3.asltop;
    new.aslwidth = c3.aslwidth;
    new.aslheight = c3.aslheight;
    new.tabstop = c3.tabstop;
    new.margin = c3.margin;
    new.fgpen = c3.fgpen;
    new.bgpen = c3.bgpen;
    new.hgpen = c3.hgpen;
    new.bbpen = c3.bbpen;
    new.tfpen = c3.fgpen;
    new.tbpen = c3.tpen;
    new._insertmode = c3.insertmode;
    new._ignorecase = c3.ignorecase;
    new._wordwrap = c3.wordwrap;
    new._autosplit = c3.autosplit;
    new._autoindent = c3.autoindent;
    new._autounblock = c3.autounblock;
} /* convert_config0003 */


void print_config (void)
{
    printf ("wwcol %d\n", new.wwcol);
    printf ("setgeometry %d %d %d %d\n",
	    new.winx, new.winy, new.winwidth, new.winheight);
    printf ("# iconpos %d %d\n", new.iwinx, new.iwiny);
    printf ("# aslgeometry %d %d %d %d\n",
		new.aslleft, new.asltop, new.aslwidth, new.aslheight);
    printf ("tabstop %d\n", new.tabstop);
    printf ("margin %d\n", new.margin);
    printf ("fgpen %d\n", new.fgpen);
    printf ("bgpen %d\n", new.bgpen);
    printf ("hgpen %d\n", new.hgpen);
    printf ("bbpen %d\n", new.bbpen);
    printf ("tfpen %d\n", new.tfpen);
    printf ("tbpen %d\n", new.tbpen);

    printf ("insertmode %s\n", new._insertmode ? "on" : "off");
    printf ("ignorecase %s\n", new._ignorecase ? "on" : "off");
    printf ("wordwrap %s\n", new._wordwrap ? "on" : "off");
    printf ("autosplit %s\n", new._autosplit ? "on" : "off");
    printf ("autoindent %s\n", new._autoindent ? "on" : "off");
    printf ("autounblock %s\n", new._autounblock ? "on" : "off");
    printf ("doback %s\n", new._doback ? "on" : "off");
    printf ("nicepaging %s\n", new._nicepaging ? "on" : "off");
    printf ("iconactive %s\n", new._iconactive ? "on" : "off");
    printf ("blockendsfloat %s\n", new._blockendsfloat ? "on" : "off");
} /* print_config */


void main (int argc, char ** argv)
{
    FILE * fh;
    char tmp_buffer[256];
    int out = 0;
    char * outfile = XDME_CONFIG;
    int current = FALSE;

    if (argc == 2 && argv[1][0] == '?')
    {
usage:
	puts ("CnvConfig converts old config-files\n"
	      "Usage : CnvConfig [-p] [-o <file>]\n\n"
	      "Without arguments it asks whether to write the new config\n"
	      "back or prints it on stdout.\n"
	      "You can specify either -p or -o. -p prints the old config\n"
	      "to stdout. You can redirect this into a file and let XDME\n"
	      "source this file. Or -o <file> writes the new config to\n"
	      "<file>.");
	exit (0);
    }
    else if (argc > 1 && argv[1][0] == '-')
    {
	if (argv[1][1] == 'p')
	    out = 2;
	else if (argv[1][1] == 'o')
	{
	    if (argv[1][2])
		outfile = &argv[1][2];
	    else
	    {
		if (argc != 3)
		    goto usage;

		outfile = argv[2];
	    }

	    out = 1;
	}
	else
	    goto usage;
    }

    if (fh = fopen (XDME_CONFIG, "r"))
    {
	fread (tmp_buffer, sizeof(CONFIG_VERSION)-1, 1, fh);

	if (!strncmp (CONFIG0001, tmp_buffer, sizeof(CONFIG0001)-1) )
	{
	    printf ("# Found config \"" CONFIG0001 "\" ...\n");

	    fgetc (fh);
	    memset (&c1, 0, CONFIG0001_SIZE);
	    fread (&c1, 1, CONFIG0001_SIZE, fh);

	    convert_config0001 ();
	}
	else if (!strncmp (CONFIG0002, tmp_buffer, sizeof(CONFIG0002)-1) )
	{
	    printf ("# Found config \"" CONFIG0002 "\" ...\n");

	    fgetc (fh);
	    memset (&c2, 0, CONFIG0002_SIZE);
	    fread (&c2, 1, CONFIG0002_SIZE, fh);

	    convert_config0002 ();
	}
	else if (!strncmp (CONFIG0003, tmp_buffer, sizeof(CONFIG0003)-1) )
	{
	    printf ("# Found config \"" CONFIG0003 "\" ...\n");

	    memset (&c3, 0, CONFIG0003_SIZE);
	    fread (&c3, 1, CONFIG0003_SIZE, fh);

	    convert_config0003 ();
	}
	else if (!strncmp (CONFIG_VERSION, tmp_buffer, sizeof(CONFIG0004)-1) )
	{
	    current = TRUE;

	    printf ("# Found current config ...\n");

	    fread (&new, 1, CONFIG_SIZE, fh);
	}
	else
	{
	    tmp_buffer[sizeof(CONFIG_VERSION)] = 0;

	    printf ("Unknown config-file \"%s\"\n", tmp_buffer);
	    exit (5);
	}

	fclose (fh);

	if (!out)
	{
	    printf ("Do you want\n%s"
		"\t(2) print config to stdout\n"
		"\t(3) exit without doing anything\n"
		":",
		current ? "" : "\t(1) convert \"" XDME_CONFIG "\" to \"" CONFIG_VERSION "\" \n"
		);

	    gets (tmp_buffer);
	    out = atoi (tmp_buffer);
	}

	switch (out)
	{
	case 1:
	    if (current)
	    {
		printf ("\"" XDME_CONFIG "\" is uptodate.\n");
		break;
	    }

	    printf ("Copying \"" XDME_CONFIG "\" to \"" XDME_CONFIG ".bak\"\n");
	    DeleteFile (XDME_CONFIG ".bak");
	    Rename (XDME_CONFIG, XDME_CONFIG ".bak");

	    if (fh = fopen (outfile, "w"))
	    {
		printf ("Writing new config-file.\n");
		fwrite (CONFIG_VERSION, sizeof(CONFIG_VERSION)-1, 1, fh);
		fwrite (&new, CONFIG_SIZE, 1, fh);
		fclose (fh);
	    }
	    else
	    {
		printf ("Cannot open \"%s\" for writing.\n"
			"copying \"" XDME_CONFIG ".bak\" back\n" , outfile);
		Rename (XDME_CONFIG ".bak", XDME_CONFIG);
		exit (10);
	    }

	    break;

	case 2:
	    print_config ();
	}
    }
    else
    {
	printf ("Cannot open \"" XDME_CONFIG "\" for reading.\n");
	exit (10);
    }

    exit (0);
}


/******************************************************************************
*****  ENDE CnvConfig.c
******************************************************************************/
