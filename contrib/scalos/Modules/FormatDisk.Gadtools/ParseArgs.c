// ParseArgs.c
// $Date$
// $Revision$
// $Id$


#include <string.h>
#include <stdio.h>

#include <exec/types.h>
#include <dos/dos.h>
// #include <dos/filehandler.h>
// #include <workbench/startup.h>
// #include <libraries/gadtools.h>
// #include <workbench/icon.h>
// #include <devices/trackdisk.h>
#include <dos/rdargs.h>

// Prototypes for system functions
#include <proto/exec.h>
#include <proto/dos.h>

// Other headers
#include "Format.h"
#include "GUI.h"
#include <defs.h>

extern SIPTR args[];

// Get the command-line arguments given by the user, by using ReadArgs()
void parseArgs(char *drive,char *newName,BOOL *ffs,BOOL *intl, BOOL *icons, char *TrashName, BOOL *quick, BOOL *verify)
{
	APTR r;

	// Get the arguments
	r=ReadArgs("DRIVE/K/A,NAME/K/A,FFS/S,INTL=INTERNATIONAL/S,NOICONS/S,QUICK/S,NOVERIFY/S/TRASCHCAN/N",args, NULL);

	// If the user didnt specify a drive name, print an error
	if(args[0] == 0L)
		{
		printError("You need to specify a drive to format",NULL,NULL);
		if(r != NULL)
			FreeArgs(r);
		cleanup(200);
		}
	else
		strcpy(drive,(char *)args[0]);

	// Likewise for a name for the newly formatted volume
	if(args[1]==0L)
		{
		printError("You need to specify a name for the volume",NULL,NULL);
		cleanup(200);
		}
	else
		strcpy(newName,(char *)args[1]);

	// Get the four togglable settings
	*ffs=(args[2]!=0);
	*intl=(args[3]!=0);
	*icons=(args[4]==0);

	if(args[5]==0L)
		strcpy(TrashName, "Trashcan");
	else
		strcpy(TrashName,(char *)args[5]);

	*quick=(args[6]!=0);
	*verify=(args[7]==0);

	// Were done, so free the ReadArgs result
	FreeArgs(r);

	// And return
	return;
}

