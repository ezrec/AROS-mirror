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
extern LONG args[7];
//extern Rect box;
extern struct WBStartup *WBenchMsg;


/*Used in askAreYouSure()*/
struct EasyStruct areYouSure = {
	sizeof(struct EasyStruct), 0, "Format Request...",
   "Are you sure you want to format volume\n%s\n(the contents of the disk will be destroyed)?",
   "Yes|No"
};
struct EasyStruct writeProtected = {
   sizeof(struct EasyStruct), 0, "Format Request...",
   "Volume\n%s\nis write protected", "Retry|Cancel"
};
struct EasyStruct errorReq = {
   sizeof(struct EasyStruct), 0, "Format Request...", NULL, "Ok" };

char *oneLine="%s";
char *twoLine="%s\n%s";
char *threeLine="%s\n%s\n%s";



/*Ask the user if she really wants to format the disk*/
BOOL askAreYouSure(char *volumeName,BOOL truncColon)
	{
   char name[36];
   APTR args[2];
   UBYTE result;

   /*Get the device name*/
   strcpy(name,volumeName);

   /*Truncate the trailing colon if so specified*/
   if(truncColon) name[strlen(name)-1] = NULL;

   /*Setup the device name as the argument to the requestor*/
   args[0]=name;
   args[1]=NULL;

   /*Put up the requestor*/
   result=EasyRequestArgs(NULL,&areYouSure,NULL,args);

   /*Return the result*/
   return(result==1);
	}


/*Alert the user to the fact that the disk is write protected, and give*/
/*the user a chance to unprotect the disk*/
BOOL alertIsWriteProtected(char *devName)
{
   APTR args[2];
   BYTE result;

   /*Setup the device name as the argument to the requestor*/
   args[0]=devName;
   args[1]=NULL;

   /*Put up the requestor*/
   result=EasyRequestArgs(NULL,&writeProtected,NULL,args);

   /*Return the result*/
   return(result==1);
}


/*Print one, two, or three lines of error messages in an EasyRequestor*/
/*or to 'StdErr'*/
void printError(char *first,char *second,char *third)
{
   APTR args[4];
   args[0]=args[3]=NULL;

   /*If were running from the CLI*/
   if(WBenchMsg==NULL)
	   {
      /*And a StdErr handle was opened successfully*/
      if(StdErr!=NULL)
   		{
			char LF=0x0A;

			/*Print the first line*/
			if(first!=NULL)
				{
				Write(StdErr,first,strlen(first));
				Write(StdErr," ",1);
				}

			/*Print the second line*/
			if(second!=NULL)
				{
				Write(StdErr,second,strlen(second));
				Write(StdErr," ",1);
				}

			/*Print the third line*/
			if(third!=NULL)
				{
				Write(StdErr,third,strlen(third));
				Write(StdErr," ",1);
				}

			/*Print the terminating carriage return*/
			Write(StdErr,&LF,1);
			}
		}
   else  /*Otherwise, were running from Workbench, so put up the requestor*/
		{
      /*Three lines*/
      if(third!=NULL)
			{
			args[2]=third;
			args[1]=second;
			args[0]=first;
			errorReq.es_TextFormat=threeLine;
			}
		else if(second!=NULL)   /*Two lines*/
			{
			args[1]=second;
			args[0]=first;
			errorReq.es_TextFormat=twoLine;
			}
		else if(first!=NULL)    /*One line*/
			{
			args[0]=first;
			errorReq.es_TextFormat=oneLine;
			}
      /*Put up the requestor*/
      EasyRequestArgs(NULL,&errorReq,NULL,args);
		}
	return;
	}

/*Get the name of a volume, given a lock to that volume*/
void getVolumeName(char *name,struct WBArg *argList,UWORD disk)
	{
   /*Get the name*/
   if(NameFromLock(argList[disk].wa_Lock,name,256)==DOSFALSE)
      /*Or return <unknown> if the name couldnt be determined*/
		strcpy(name,"<unknown>");

   return;
	}

