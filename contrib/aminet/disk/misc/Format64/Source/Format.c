

/**************************************************************************/
/*				  Format64				  */
/*			       Version 1.04				  */
/*									  */
/* This is a replacement for the AmigaDOS "Format" command.  It sports    */
/* (for the Workbench user) more friendly user-interface, although it is  */
/* virtually identical to the standard Format command where the CLI	  */
/* interface is concerned.						  */
/*									  */
/* NewFormat v1.00 is Copyright 1992 by Dave Schreiber, All Rights Reserved.*/
/* Format64  v1.00 is Copyright 1998 by Jarkko Vatjus-Anttila*/
/* This program may not be sold for more than a small copying and shipping*/
/* and handling fee, except by written permission of Dave Schreiber.	  */
/*									  */
/* Version list:							  */
/*    1.00 - First release (August 31, 1992)                              */
/*    1.00 - Update to 64 bit command set. Support for disks >4GB */
/*    1.01 - Small bug fixes. Made the program to use ANSI C functions. */
/**************************************************************************/


/*System header files*/
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
#include "stdlib.h"


/* Global Variables */

BOOL FFS = FALSE;
BOOL QuickFmt = FALSE;
BOOL intl = FALSE;
BOOL Verify = TRUE;
BOOL Icon = TRUE;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *GadToolsBase = NULL;
struct Library *IconBase = NULL;
BPTR StdErr = NULL;

LONG args[7] = { NULL,NULL,0,0,0,0,0 };
ULONG ffs_format = ID_DOS_DISK;

extern Rect box;
struct WBStartup *WBenchMsg = NULL;

char *Version = "$VER: Format64 V1.04 (19.5.98)";
char temp[32];

//int main(int a, char **arg);

int main()
	{
   UWORD disk;
	ULONG i;
	char volumeName[256], statusString[80], driveName[64];
	char newName[256];
	BPTR driveLock;
	struct Process *process;
	APTR oldWdw;
	DriveLayout junk;
   prepResult stat;


   /*Open the various shared libraries that are needed*/
   IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37L);
   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37L);
   IconBase = (struct Library *)OpenLibrary("icon.library",37L);
   GadToolsBase = (struct Library *)OpenLibrary("gadtools.library",37L);

   if(IntuitionBase==NULL || GfxBase==NULL || IconBase==NULL || GadToolsBase==NULL)
      cleanup(100);

//	WBenchMsg = (struct WBStartup *)argv;
   if(WBenchMsg != NULL)
		{
      if(WBenchMsg->sm_NumArgs==1)
			{
			printError("Please select a drive to format and try again!",NULL,NULL);
			cleanup(100);
			}

		SetupScreen();

      for(disk=1;disk<WBenchMsg->sm_NumArgs && stat!=eQuit;disk++)
			{
			/*Get the volume/device name of the disk to format*/
			getVolumeName(volumeName,WBenchMsg->sm_ArgList,disk);

			/*If there is no lock to the volume, that means it is */
			/*unformatted, so just use the name given to us by Workbench*/
			if (WBenchMsg->sm_ArgList[disk].wa_Lock==NULL)
				strcpy(volumeName,WBenchMsg->sm_ArgList[disk].wa_Name);

			/*Open the options window*/
			if ((stat=OpenPrepWindow(volumeName))==0)
				{
				/* Get the users input */
				stat=getPrepInput();

				/*And close the window*/
				ClosePrepWindow();

				/*If the user selected 'OK'*/
				if (stat == eOK)
					{
					/*Get the new name of the disk*/
					strcpy(newName,((struct StringInfo *)
						(PrepGadgets[GD_NameGadget]->SpecialInfo))->Buffer);

					/*Ask the user for verification*/
					if (askAreYouSure(volumeName,
						(WBenchMsg->sm_ArgList[disk].wa_Lock!=NULL)))
						{

						/*If everythings OK, open the status window*/
						if ((stat=OpenStatusWindow(statusString))==0)
							{
							/*And format the disk*/
							formatVolume(&(WBenchMsg->sm_ArgList[disk].wa_Lock),
								volumeName,newName,FFS,QuickFmt,Verify,Icon,
								statusString);

							/*Were done, so close the status window*/
							CloseStatusWindow();
							}
						}
					}
				}
			}
		/*Free the visual info, etc.*/
		CloseDownScreen();
		}

   else     /*Weve been run from the CLI*/

		{
      /*Open a 'stderr' I/O channel to the shell*/
      /*(the normal stderr was not opened since we used _main() ) */
      StdErr=Open("CONSOLE:",MODE_OLDFILE);

      /*Make sure requestors dont open*/
      process=(struct Process *)FindTask(0L);
      oldWdw=process->pr_WindowPtr;
      process->pr_WindowPtr=(APTR)(-1);

      /*Get the command-line arguments*/
      parseArgs(driveName,volumeName,&FFS,&intl,&Icon,&QuickFmt,&Verify);

		for (i=0; i<strlen(volumeName); i++)
			if (volumeName[i] == ':' || volumeName[i] == '/')
				{ Write(Output(), "Volumename contains illegal characters!\n", 40);
				  cleanup(0); }

		if (FFS == TRUE)
      	{
			if (intl == TRUE) ffs_format = ID_INTER_FFS_DISK;
			else ffs_format = ID_FFS_DISK;
			}
		else
      	{
			if (intl == TRUE) ffs_format = ID_INTER_DOS_DISK;
			else ffs_format = ID_DOS_DISK;
			}

//	printf("%ld %ld $%lx\n", FFS, intl, ffs_format);

      /*Get a lock on the selected drive*/
      /*(note:  NULL is a valid return value;  it means that the disk we*/
      /*want to format is itself unformatted)*/
      driveLock=Lock(driveName,ACCESS_READ);
		strcpy(temp,driveName);

      /*Get the volume/drive name*/
      if (volumeToDevName(driveLock,temp,&junk))
      	{
			/*Wait for the user to tell us to go ahead*/
/*			Write(Output(),"Insert the disk to be formatted in drive ",41);
			Write(Output(),temp,strlen(temp));
			Write(Output()," and press RETURN ",18);
			Read(Input(),temp2,1);
*/
			/*Format the disk*/
			formatVolume(&driveLock,temp,volumeName,ffs_format,QuickFmt,Verify,Icon,statusString);
			}
      else Write(Output(),"Cannot find the specified drive!\n",33);

      Close(StdErr);

      /*Restore the old contents of this pointer*/
      process->pr_WindowPtr=(APTR)(-1);

      Write(Output(),"\n",1);
		}
	cleanup(0);
	return(0);
	}

/*Exit from the program, closing any open libraries*/
void cleanup(ULONG err)
{
  if(IntuitionBase != NULL) CloseLibrary((struct Library *)IntuitionBase);
  if(GfxBase != NULL) CloseLibrary((struct Library *)GfxBase);
  if(GadToolsBase != NULL) CloseLibrary(GadToolsBase);
  if(IconBase != NULL) CloseLibrary(IconBase);
  exit(err);
}


/*Update the status window (from Workbench), or CLI output (from the CLI)*/
/*If running from the CLI, this also checks to see if the user has pressed*/
/*control-C*/
BOOL updateStatWindow(char *string,UWORD percent)
	{
   UWORD width;
   ULONG class1;
   UWORD code;
   struct TagItem tags[3];
   static char msg[80];
	char CR = 0x0d;

   struct IntuiMessage *mesg;

   /*If this is NULL, were running from the CLI*/
   if(WBenchMsg==NULL)
		{
      /*Write the string to the CLI, followed by a carriage return (but*/
      /*not a line feed)*/
      Write(Output(),string,strlen(string));
      Write(Output(),&CR,1);

      /*Check to see if the user pressed Control-C*/
      if( (SetSignal(0,0) & SIGBREAKF_CTRL_C) == SIGBREAKF_CTRL_C)
			{
			/*If he did, print "***Break" and return TRUE, to signal that*/
			/*the user aborted the formatting process*/
			Write(Output(),"\n***Break\n",10);
			return(TRUE);
			}
      /*Otherwise, continue*/
      return(FALSE);
		}

   /*This code is used to update the status window that is displayed when*/
   /*the user runs NewFormat from the Workbench*/

   /*This puts the message into the text-display gadget*/
   /*This copy is so that the caller can change the contents of 'string'*/
   /*upon return (which cant be done without the copy, since GadTools*/
   /*wont copy the contents of string  to an internal buffer*/
   strcpy(msg,string);
   tags[0].ti_Tag=GTTX_Text;
   tags[0].ti_Data=(ULONG)msg;

   tags[1].ti_Tag=TAG_DONE;
   tags[1].ti_Data=NULL;

   GT_SetGadgetAttrsA(StatusGadgets[GD_StatusGadget], StatusWnd,NULL, tags);

   /*Fill the status box with the current percentage of completion*/
   SetAPen(StatusWnd->RPort,3);
   width=box.left+((box.width-3)*percent)/1000;
   RectFill(StatusWnd->RPort,box.left+2,box.top+1,width,box.top+box.height-2);

   /*Check user input*/
   mesg=GT_GetIMsg(StatusWnd->UserPort);

   /*Loop while there are messages*/
   while(mesg!=NULL)
		{
      class1 = mesg->Class;
      code = mesg->Code;
      GT_ReplyIMsg(mesg);

      switch(class1)
			{
			/*Return TRUE (user abort) if the user pressed 's', 'S'*/
			case IDCMP_VANILLAKEY:
				if (code == 's' || code == 'S') return(TRUE);
				break;

			/*Or if the user pressed the 'Stop' gadget*/
			case IDCMP_GADGETUP:
				return(TRUE);
			}
      /*Get the next message*/
      mesg=GT_GetIMsg(StatusWnd->UserPort);
		}
   return(FALSE);
	}

/*End of Format.c*/

