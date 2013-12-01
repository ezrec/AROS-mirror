// GetInputFromWindow.c
// $Date$
// $Revision$


#include <string.h>

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <dos/dos.h>
#include <dos/filehandler.h>
#include <workbench/startup.h>
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
#include <proto/utility.h>

#include <defs.h>

#include "Format.h"
#define	FormatDisk_NUMBERS
#include STR(SCALOSLOCALE)

#include "GUI.h"

//-----------------------------------------------------------------------------

extern BOOL FFS;
extern BOOL intl;
extern BOOL DirCache;

//-----------------------------------------------------------------------------
static void GetMsgidUnderScore(void);
//static ULONG FindUnderscoredToLower(STRPTR text);

static UWORD Name_key, FFS_key, INTL_key, Verify_key, WithTrash_key;
static UWORD Dircache_key, Trash_key, Format_key, QuickFormat_key, Cancel_key;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static void GetMsgidUnderScore(void)
{
	Name_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_DEVNAME));

	FFS_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_FFS));

	INTL_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_INTL));

	Verify_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_VERIFY));

	WithTrash_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_TRASHCAN));

	Dircache_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_DIRCACHE));

	Trash_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_TRASHCAN_NAME));

	Format_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_GUI_FORMAT));

	QuickFormat_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_QUICK_FORMAT));

	Cancel_key = FindUnderscoredToLower( (STRPTR) GetLocString(MSGID_GADGET_GUI_CANCEL));
}

//-----------------------------------------------------------------------------

ULONG FindUnderscoredToLower(STRPTR text)
{
	ULONG c;
	while(( c = *text++))
	{
		if( c == '_' ) return ToLower(*text);
	}
	return 0;
}
//-----------------------------------------------------------------------------

// Get input from the original window
prepResult getPrepInput(void)
{
	struct IntuiMessage *mesg;
	ULONG class;
	ULONG code;
	struct Gadget *gadget;
	struct TagItem tags[2];

	GetMsgidUnderScore();

	// Setup tags that will be used to toggle the states of checkbox gadgets
	tags[0].ti_Tag = GTCB_Checked;
	tags[1].ti_Tag = TAG_DONE;
	tags[1].ti_Data = 0UL;

	// Loop until the user presses 'OK' or 'Cancel'
	for(;;)
		{
		// Wait for input
		Wait(1<<PrepWnd->UserPort->mp_SigBit);

		// Get the input
		mesg = GT_GetIMsg(PrepWnd->UserPort);

		// Loop while there are messages to be processed
		while (mesg != NULL)
			{
			// Get the message type, etc.
			class = mesg->Class;
			code = mesg->Code;
			gadget = (struct Gadget *)mesg->IAddress;

			// Reply to the message
			GT_ReplyIMsg(mesg);

			// Act on the message
			switch(class)
				{
			// User clicked on close gadget.  Treat it as a click on 'Cancel'
			case IDCMP_CLOSEWINDOW:
				return(eQuit);

			// User pressed a gadget
			case IDCMP_GADGETUP:
				switch(gadget->GadgetID)
					{
				// Checkbox gadgets
				// (each toggles the appropriate status flag)

				// FastfileSystem
				case GD_FFSGadget:
					FFS = !FFS;
					break;

				// Withe Trashcan
				case GD_IconGadget:
					Icon = !Icon;
					break;

				// Verify
				case GD_VerifyGadget:
					Verify = !Verify;
					break;

				// International
				case GD_IntlGadget:
					intl = !intl;
					break;

				// DirCache
				case GD_DirCacheGadget:
					DirCache = !DirCache;
					break;

				// OK
				case GD_OKGadget:
					return(eOK);

				// Quick format
				case GD_QuickFmtGadget:
					return(eQuick);

				// Cancel
				case GD_CancelGadget:
					return(eCancel);
					}
				break;

			// Keypress (gadget equivalents)
			case IDCMP_VANILLAKEY:
				if (code == Name_key)
					ActivateGadget(PrepGadgets[GD_NameGadget], PrepWnd,NULL);
				else if( code == FFS_key )
					{
					// Toggle the checkmark state of the gadget
					tags[0].ti_Data = (FFS = !FFS);
					GT_SetGadgetAttrsA(PrepGadgets[GD_FFSGadget], PrepWnd,NULL, tags);
					}
				else if( code == INTL_key )
					{
					if (!DirCache)
						{
						tags[0].ti_Data = (intl = !intl);
						GT_SetGadgetAttrsA(PrepGadgets[GD_IntlGadget], PrepWnd,NULL, tags);
						}
					}
				else if( code == Verify_key )
					{
					tags[0].ti_Data = (Verify = !Verify);
					GT_SetGadgetAttrsA(PrepGadgets[GD_VerifyGadget], PrepWnd,NULL, tags);
					}
				else if( code == WithTrash_key )
					{
					tags[0].ti_Data = (Icon = !Icon);
					GT_SetGadgetAttrsA(PrepGadgets[GD_IconGadget], PrepWnd,NULL, tags);
					}
				else if( code == Dircache_key )
					{
					tags[0].ti_Data = (DirCache = !DirCache);
					GT_SetGadgetAttrsA(PrepGadgets[GD_DirCacheGadget], PrepWnd,NULL, tags);
					}
				else if( code == Trash_key )
					{
					if (Icon)
						{
						ActivateGadget(PrepGadgets[GD_TrashCanNameGadget], PrepWnd,NULL);
						}
					}
				else if( code == Format_key )
					return(eOK);
				else if( code == QuickFormat_key )
					return(eQuick);
				else if( code == Cancel_key )
					return(eCancel);

				break;
				}

			// Get the next message
			mesg = GT_GetIMsg(PrepWnd->UserPort);
			}

			// Update state of some gadgets.
			GT_SetGadgetAttrs(PrepGadgets[GD_IntlGadget], PrepWnd, NULL, GA_Disabled, DirCache, TAG_DONE);

			GT_SetGadgetAttrs(PrepGadgets[GD_TrashCanNameGadget], PrepWnd, NULL, GA_Disabled, !Icon, TAG_DONE);

			d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld: SELECTED FROM GUI: FFS = %ld INTL = %ld DIRCACHE = %ld ICON = %ld VERIFY = %ld\n", 
				__LINE__, FFS, intl, DirCache, Icon, Verify));
		}

	return(FALSE);
}

