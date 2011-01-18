#include <aros/oldprograms.h>

#include <exec/types.h>
#include <intuition/intuition.h>

extern void DisplayT(char **);

extern struct IntuitionBase *IntuitionBase;
extern struct Window *mm_w;

char *Help_display[]= {
    "",
    "          MemoMaster V2.0",
    "",
    "      Select a memo for action by clicking on it here.   ",
    "   The Edit and Delete routines process all selected memos.   ",
    "       Any number of memos may be selected at any time.",
    "",
    '\0' };

char *Help_close[]= {
    "",
    "          MemoMaster V2.0",
    "",
    "   Click on close gadget to exit MemoMaster, ",
    "     saving list of memos back to disk if",
    "              any have changed.  ",
    "",
    '\0' };

char *Help_add[]= {
    "",
    "                    MemoMaster V2.0",
    "",
    "   Displays a requester allowing you to add a memo item   ",
    "                      to the list.",
    "",
    '\0' };

char *Help_del[]= {
    "",
    "                    MemoMaster V2.0",
    "",
    "    Removes selected memos from the list in memory.   ",
    "    The memos you want to delete must be selected",
    "    in the display window first. The program will  ",
    "    ask for confirmation that each selected memo",
    "                    be deleted.",
    "",
    '\0' };

char *Help_edi[]= {
    "",
    "          MemoMaster V2.0",
    "",
    "   Displays a requester allowing you to   ",
    "   edit an existing memo. The memos you",
    "   want to edit must be selected in the",
    "   display window first.",
    "",
    '\0' };

char *Help_chk[]= {
    "",
    "                 MemoMaster V2.0",
    "",
    "   Checks the list of memos in memory for any to be   ",
    "                  displayed today.",
    "",
    '\0' };

char *Help_shr[]= {
    "",
    "                   MemoMaster V2.0",
    "",
    "   Close main window but keep process in memory. A small",
    "   window will open with a gadget marked EXPAND. Clicking",
    "   inside the small window (not the border) will reopen",
    "   the main window in the same state as when suspended.",
    "   While the main window is closed, the process will be",
    "   in a WAIT state therefore taking minimal processor",
    "        resource from your other applications.",
    "",
    '\0' };

char *Help_beg[]= {
    "",
    "         MemoMaster V2.0",
    "",
    "   The first memos in the list   ",
    "     in memory are displayed.",
    "",
    '\0' };

char *Help_bkb[]= {
    "",
    "           MemoMaster V2.0",
    "",
    "   Move the display back one block of 6   ",
    "                  memos.",
    "",
    '\0' };

char *Help_bki[]= {
    "",
    "             MemoMaster V2.0",
    "",
    "   Move memos 1 back toward start of list.   ",
    "",
    '\0' };

char *Help_fwi[]= {
    "",
    "           MemoMaster V2.0",
    "",
    "   Move memos 1 on toward end of list.   ",
    "",
    '\0' };

char *Help_fwb[]= {
    "",
    "          MemoMaster V2.0",
    "",
    "   Moves memos in display window one block   ",
    "              of 6 forwards.",
    "",
    '\0' };

char *Help_eol[]= {
    "",
    "              MemoMaster V2.0",
    "",
    "   Click here to move display to end of list.   ",
    "",
    '\0' };


void Mode_Help()
  {
  int Class,g;
  struct IntuiMessage *msg;
  APTR address;
  BOOL finished;
  ULONG SignalMask;

  SignalMask = 1L << mm_w->UserPort->mp_SigBit;
  finished=FALSE;
  while(!finished)
    {
    Wait( SignalMask );
    msg = (struct IntuiMessage *) GetMsg( mm_w->UserPort );
    if(msg)
      {
      Class = msg->Class;
      address = msg->IAddress;
      ReplyMsg( (struct Library *)msg );
      switch( Class )
	{
	case CLOSEWINDOW:
	  DisplayT(Help_close);
	  break;
	case GADGETUP:
	  g=((struct Gadget *)address)->GadgetID;
	  switch (g)
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	      DisplayT(Help_display);
	      break;
	    case 6:
	      DisplayT(Help_add);
	      break;
	    case 7:
	      DisplayT(Help_del);
	      break;
	    case 8:
	      DisplayT(Help_edi);
	      break;
	    case 9:
	      DisplayT(Help_chk);
	      break;
	    case 10:
	      DisplayT(Help_shr);
	      break;
	    case 11:
	      finished=TRUE;
	      break;
	    case 12:
	      DisplayT(Help_beg);
	      break;
	    case 13:
	      DisplayT(Help_bkb);
	      break;
	    case 14:
	      DisplayT(Help_bki);
	      break;
	    case 15:
	      DisplayT(Help_fwi);
	      break;
	    case 16:
	      DisplayT(Help_fwb);
	      break;
	    case 17:
	      DisplayT(Help_eol);
	      break;
	    }
	  break;
	}
      }
    }
  }

/*=====================================================================*/
