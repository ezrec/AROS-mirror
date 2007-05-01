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

#include <aros/debug.h>

/*Prototypes for system functions*/
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/graphics.h>

/*Other headers*/
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
//extern Rect box;
extern struct WBStartup *WBenchMsg;

/*Convert a volume name to a device name, and get information*/
/*on the layout of the disk*/
BOOL volumeToDevName(BPTR volumeLock,char *dev,DriveLayout *layout)
	{
   BOOL stat;
   struct DosList *dosList;
   struct DeviceNode *devNode;
   char name[36];
   char *temp;
   int c;
   BPTR tempLock=NULL;
   struct DosEnvec *driveEnv;
   struct Process *process;
   APTR oldWdw;
   struct FileSysStartupMsg *startup;
   char *devName;


   /*Disable requestors during the execution of this function*/
   process=(struct Process *)FindTask(0L);
   oldWdw=process->pr_WindowPtr;
   process->pr_WindowPtr=(APTR)(-1);

   /*Get the DOS device list*/
   dosList=LockDosList(LDF_DEVICES|LDF_READ);

   /*Go through each entry*/
   while(dosList != NULL)
		{
		dosList = NextDosEntry(dosList,LDF_DEVICES|LDF_READ);
		if (dosList == NULL) break;
      devNode=(struct DeviceNode *)dosList;

      /*If the node in the list is a volume*/
      if (devNode->dn_Startup > 1000 &&
			(/*devNode->dn_Task ||*/ devNode->dn_Handler /*|| devNode->dn_SegList*/))
			{
			/*Get the name of the device*/
//			temp=(char *)BADDR(devNode->dn_Name);
			temp=devNode->dn_Ext.dn_AROS.dn_DevName;

//			for(c=0;c<temp[0];c++) name[c]=temp[c+1];
			for(c=0;temp[c];c++) name[c]=temp[c];

			name[c]=':';
			name[c+1]=0;

			/*Get the information on the device*/
			/*	   driveEnv=(struct DosEnvec *)
			BADDR(((struct FileSysStartupMsg *)
			BADDR(devNode->dn_Startup))->fssm_Environ);*/

	 /*If the volume lock is NULL, the 'dev' is assumed to be the*/
	 /*name of a device holding an unformatted disk, so compare the*/
	 /*name to the name of the current node*/
			if (volumeLock == NULL) stat = (stricmp(dev,name)==0);
			else
				{
				/*Otherwise, since 'dev' could hold a volume rather than*/
				/*device name, get a lock on it and compare it to the lock*/
				/*on the device that was given;  if they're the same, we've*/
				/*found the drive*/

				tempLock=Lock(name,ACCESS_READ);
				stat=(SameLock(tempLock,volumeLock)==LOCK_SAME);
				}

			/*If weve found the drive, get the information on it*/
			if(stat)
				{

				/*Get a pointer to the structure that holds the needed info*/
				startup=(struct FileSysStartupMsg *)BADDR(devNode->dn_Startup);
				driveEnv=(struct DosEnvec *)BADDR(startup->fssm_Environ);

				/*Get the information*/
				layout->unit=startup->fssm_Unit;

				devName=(char *)BADDR(startup->fssm_Device);

				/*Copy the device name to layout->devName*/
//				for(c=0;c<devName[0];c++) layout->devName[c]=devName[c+1];
				for(c=0;devName[c];c++) layout->devName[c]=devName[c];
				layout->devName[c/*+1*/]=0;

				layout->flags=startup->fssm_Flags;
				layout->memType=driveEnv->de_BufMemType;
				layout->lowCyl=driveEnv->de_LowCyl;
				layout->highCyl=driveEnv->de_HighCyl;
				layout->surfaces=driveEnv->de_Surfaces;
				layout->BPT=driveEnv->de_BlocksPerTrack;
				layout->blockSize=driveEnv->de_SizeBlock;

				/*Copy the device name back to 'dev'*/
				strcpy(dev,name);

				/*UnLock the drive lock, if it exists*/
				if(tempLock != NULL) UnLock(tempLock);

				/*Unlock the DOS list*/
				UnLockDosList(LDF_DEVICES|LDF_READ);

				/*Restore the requester pointer*/
				process->pr_WindowPtr=oldWdw;

				/*And return TRUE*/
				return(TRUE);
				}

			/*We didnt find the drive, so unlock the lock and try again*/
			if(tempLock != NULL) UnLock(tempLock);
			}
		}

   /*We didnt find the drive in the list, so unlock the list*/
   UnLockDosList(LDF_DEVICES|LDF_READ);

   /*Restore the requester pointer*/
   process->pr_WindowPtr=oldWdw;

   /*And return*/
   return(FALSE);
	}

/*Create a communications port linking this process to the drive*/
struct IOExtTD *OpenDrive(char *driveDevName,ULONG unit,ULONG flags)
	{
   struct MsgPort *diskPort;
   struct IOExtTD *diskRequest;

   /*Create the message port*/
   if ((diskPort = CreateMsgPort()) != NULL)
		{
      /*Create the IORequest*/
      diskRequest = (struct IOExtTD *)
      	CreateIORequest(diskPort,sizeof(struct IOExtTD));
      if(diskRequest != NULL)
			{
			/*Open the device, and return the IORequest if the device*/
			/*opened successfully*/
			if(!OpenDevice(driveDevName,unit,(struct IORequest *)diskRequest,flags))
				return(diskRequest);

			/*The device didnt open, so clean up*/
			DeleteIORequest(diskRequest);
			}

		DeleteMsgPort(diskPort);
		}

   /*Return NULL to indicate that an error occurred*/
   return(NULL);
	}

/*Close an open device and delete the accompanying port, etc.*/
void CloseDrive(struct IOExtTD *diskRequest)
	{
   CloseDevice((struct IORequest *)diskRequest);
   DeleteMsgPort(diskRequest->iotd_Req.io_Message.mn_ReplyPort);
   DeleteIORequest(diskRequest);
	}

/*Convert a CSTR to a BSTR*/
BSTR makeBSTR(char *in,char *out)
	{
   int c;

   out[0]=strlen(in);
   for(c=0;c<out[0];c++) out[c+1]=in[c];

   return(MKBADDR(out));
	}
