
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/gadtools.h>
#include <workbench/icon.h>
#include <devices/trackdisk.h>
#include <dos/rdargs.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/graphics.h>

#include <aros/debug.h>

#include "Format.h"
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
extern struct WBStartup *WBenchMsg;
extern char temp[];

checkfor64bitcommandset(struct IOStdReq *io);


/*This functions handles the low-level format, the high-level format, the*/
/*creation of icons, etc.*/
void formatVolume(BPTR *volumeLock,char *volumeName,char *newName,ULONG ffs,
		  BOOL quick,BOOL verify,BOOL icon,char *statString)
	{
   struct IOExtTD *io1;
   BOOL fmtResult=TRUE;
   int result=0;
   char deviceName[64], temp2[4];
   DriveLayout layout;
   struct MsgPort *devPort;
   BOOL writeProtCont=TRUE;
   /*If the volume lock is NULL, assume that the volumeName string holds*/
   /*a valid device pointer*/
   if(*volumeLock==NULL) strcpy(deviceName,volumeName);

   /*Get the drive name (if possible)*/
   if(!volumeToDevName(*volumeLock,deviceName,&layout))
		{
      printError("Can't find the drive!",NULL,NULL);
      return;
		}

   /*This port will be used to communicate with the filesystem*/
   devPort=DeviceProc(deviceName);
   if(devPort==NULL)
		{
      printError("Can't find the drive",NULL,NULL);
      return;
		}

   /*Inhibit the drive*/
//   DoPkt(devPort,ACTION_INHIBIT,DOSTRUE,NULL,NULL,NULL,NULL);

   /*If we got a lock to the volume that we're going to format it, destroy*/
   /*it, since the volume that it points to is about to be erased anyway*/
   if(*volumeLock != NULL)
		{
      UnLock(*volumeLock);
      *volumeLock=NULL;
		}
	Inhibit(deviceName, DOSTRUE);

   /*Open the disk device*/
   if((io1=OpenDrive(layout.devName,layout.unit,layout.flags))!=NULL)
		{
      /*Determine the write protect status*/
      io1->iotd_Req.io_Data=NULL;
      io1->iotd_Req.io_Length=0;
      io1->iotd_Req.io_Command=TD_PROTSTATUS;
      DoIO((struct IORequest *)io1);

      /*Loop while the disk stays protected and user keeps pressing Retry*/
      while(io1->iotd_Req.io_Actual!=0 && (writeProtCont=alertIsWriteProtected(volumeName)))
			DoIO((struct IORequest *)io1);

      /*If the disk is not write-protected*/
		if(writeProtCont)
			{
			/*Do a full format if the user didnt select the Quick option*/
			if (checkfor64bitcommandset((struct IOStdReq *)io1) == TRUE)
				printf("***Device supports 64bit commands! No size limit!!\n");
			else printf("***Device does NOT support 64bit commands. 4GB limit activated!!\n");

			if (WBenchMsg == NULL)
				{
				Write(Output(),"Insert the disk to be formatted in drive ",41);
				Write(Output(),temp,strlen(temp));
				Write(Output()," and press RETURN ",18);
				Read(Input(),temp2,1);
				}

	      if( (SetSignal(0,0) & SIGBREAKF_CTRL_C) != SIGBREAKF_CTRL_C)
				{
				if (!quick) fmtResult=doFullFormat(&layout,statString,volumeName,io1);
				if (fmtResult == TRUE)
					{
					/*Write an extra carriage return, if run from the CLI*/
					if(WBenchMsg==NULL) Write(Output(),"\n",1);
					/*Tell the user that were doing the high-level format*/
				   fmtResult =! updateStatWindow("Initializing disk...",1000);
					/*If the user hasnt clicked on Stop, do the high-level format*/
					if (fmtResult) result=Format(deviceName,newName, ffs);
					}
				}
			else printf("***Break\n");
			}
		else printError("Cannot format volume", volumeName, "because the disk is write protected");

		/*Close the disk device*/
		CloseDrive(io1);
	   }
   else
	   {
      printError("Couldn't access the device",NULL,NULL);
      return;
	   }

   /*Uninhibit the drive*/
//   DoPkt(devPort,ACTION_INHIBIT,DOSFALSE,NULL,NULL,NULL,NULL);
	Inhibit(deviceName, DOSFALSE);

   /*Wait for the drive to come on line*/
   Delay(50);

   /*If the disk was never unprotected, return*/
   if(!writeProtCont) return;

   /*If the format was successful and the user wants icons created*/
   if(icon && result==DOSTRUE)
		{
      struct DiskObject *trashIcon;
      BPTR trashLock;

      /*Update the user*/
      fmtResult=!updateStatWindow("Creating Trashcan  ",1000);

      /*If she didnt press Stop*/
      if(fmtResult)
			{
			/*Create the trashcan name (<volume>:Trashcan)*/
			strcpy(deviceName,newName);
			strcat(deviceName,":");
			strcat(deviceName,"Trashcan");

			/*Create the trashcan directory*/
			trashLock=CreateDir(deviceName);

			/*If it was successfully created*/
			if(trashLock!=NULL)
				{
				UnLock(trashLock);
				/*Get the trashcan icon*/
				trashIcon=GetDefDiskObject(WBGARBAGE);

				/*If there wasnt an error*/
				if (trashIcon != NULL)
					{
					/*Write the icon to disk*/
					if(!PutDiskObject(deviceName,trashIcon))
						printError("There was an error while creating the trashcan", NULL,NULL);

					/*and free it*/
					FreeDiskObject(trashIcon);
					}
				else printError("There was an error while creating the trashcan", NULL,NULL);
				}
			else printError("There was an error while creating the trashcan directory", NULL,NULL);
			}
		}
	return;
	}

