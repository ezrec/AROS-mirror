#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <workbench/icon.h>
#include <devices/trackdisk.h>
#include <dos/rdargs.h>

/*Prototypes for system functions*/
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/graphics.h>

/*Other headers*/
#include "Format.h"
#include "GUI.h"
#include "string.h"
#include "stdio.h"

extern BOOL FFS;
extern BOOL QuickFmt;
extern BOOL Verify;
extern BOOL Icon;
extern struct Library *GadToolsBase;
extern struct Library *IconBase;
extern BPTR StdErr;
extern LONG args[];
extern struct WBStartup *WBenchMsg;

/*Get the command-line arguments given by the user, by using ReadArgs()*/
void parseArgs(char *drive,char *newName,BOOL *ffs,BOOL *intl,BOOL *icons,BOOL *quick, BOOL *verify)
{
  APTR r;
  
  /*Get the arguments*/
  r=ReadArgs("DRIVE/K/A,NAME/K/A,FFS/S,INTL=INTERNATIONAL/S,NOICONS/S,QUICK/S,NOVERIFY/S",args, NULL);
  
  /*If the user didnt specify a drive name, print an error*/
  if(args[0] == NULL)
    {
      printError("You need to specify a drive to format",NULL,NULL);
      if(r != NULL) FreeArgs(r);
      cleanup(200);
    }
  else strcpy(drive,(char *)args[0]);
  
  /*Likewise for a name for the newly formatted volume*/
  if(args[1]==NULL)
    {
      printError("You need to specify a name for the volume",NULL,NULL);
      cleanup(200);
    }
  else strcpy(newName,(char *)args[1]);
  
  /*Get the four togglable settings*/
  *ffs=(args[2]!=0);
  *intl=(args[3]!=0);
  *icons=(args[4]==0);
  *quick=(args[5]!=0);
  *verify=(args[6]==0);

  /*Were done, so free the ReadArgs result*/
  FreeArgs(r);
  
  /*And return*/
  return;
}
