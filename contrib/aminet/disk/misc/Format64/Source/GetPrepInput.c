#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <intuition/iobsolete.h>
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

/*Get input from the original window*/
prepResult getPrepInput(void)
	{
   struct IntuiMessage *mesg;
   ULONG class;
   ULONG code;
   struct Gadget *gadget;
   struct TagItem tags[2];

   /*Setup tags that will be used to toggle the states of checkbox gadgets*/
   tags[0].ti_Tag=GTCB_Checked;
   tags[1].ti_Tag=TAG_DONE;
   tags[1].ti_Data=NULL;

   /*Loop until the user presses 'OK' or 'Cancel'*/
   for(;;)
	   {
      /*Wait for input*/
      Wait(1<<PrepWnd->UserPort->mp_SigBit);

      /*Get the input*/
      mesg=GT_GetIMsg(PrepWnd->UserPort);

      /*Loop while there are messages to be processed*/
      while(mesg != NULL)
			{
			/*Get the message type, etc.*/
			class=mesg->Class;
			code=mesg->Code;
			gadget=(struct Gadget *)mesg->IAddress;

			/*Reply to the message*/
			GT_ReplyIMsg(mesg);

			/*Act on the message*/
			switch(class)
				{
				/*User clicked on close gadget.  Treat it as a click on 'Cancel'*/
				case CLOSEWINDOW:
					return(eQuit);

				/*User pressed a gadget*/
				case GADGETUP:
					switch(gadget->GadgetID)
						{
						/*Checkbox gadgets*/
						/*(each toggles the appropriate status flag)*/
						case GD_FFSGadget:
							FFS=!FFS;
							break;
						case GD_IconGadget:
							Icon=!Icon;
							break;
						case GD_QuickFmtGadget:
							QuickFmt=!QuickFmt;
							break;
						case GD_VerifyGadget:
							Verify=!Verify;
							break;

						/*OK*/
						case GD_OKGadget:
							return(eOK);

						/*Cancel*/
						case GD_CancelGadget:
							return(eCancel);
						}
					break;

					/*Keypress (gadget equivalents)*/
				case VANILLAKEY:
					switch(code)
						{
						/*Disk name*/
						case 'n':
						case 'N':
							ActivateGadget(PrepGadgets[GD_NameGadget], PrepWnd,NULL);
							break;

						/*FFS*/
						case 'f':
						case 'F':
							tags[0].ti_Data=(FFS=!FFS);
							/*Toggle the checkmark state of the gadget*/
							GT_SetGadgetAttrsA(PrepGadgets[GD_FFSGadget], PrepWnd,NULL, tags);
							break;

						/*Verify*/
						case 'v':
						case 'V':
							tags[0].ti_Data=(Verify=!Verify);
							GT_SetGadgetAttrsA(PrepGadgets[GD_VerifyGadget], PrepWnd,NULL, tags);
							break;

						/*Quick Format*/
						case 'q':
						case 'Q':
							tags[0].ti_Data=(QuickFmt=!QuickFmt);
							GT_SetGadgetAttrsA(PrepGadgets[GD_QuickFmtGadget], PrepWnd,NULL, tags);
							break;

						/*Create icons*/
						case 'r':
						case 'R':
							tags[0].ti_Data=(Icon=!Icon);
							GT_SetGadgetAttrsA(PrepGadgets[GD_IconGadget], PrepWnd,NULL, tags);
							break;

						/*Cancel*/
						case 'c':
						case 'C':
							return(eCancel);

						/*OK*/
						case 'o':
						case 'O':
							return(eOK);
						}
					break;
				}
			/*Get the next message*/
			mesg=GT_GetIMsg(PrepWnd->UserPort);
			}
		}
	return(FALSE);
	}

