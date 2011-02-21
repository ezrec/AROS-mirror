/*
 * @(#) $Header$
 *
 * CXDemo.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2000/08/10 11:50:54  chodorowski
 * Cleaned up and prettyfied the GUIs a bit.
 *
 * Revision 42.0  2000/05/09 22:19:29  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:19  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:40  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:09  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0.
dcc CXDemo.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"

/*
**      Key ID.
**/
#define CX_F1_PRESSED           1L

/*
**      Gadget ID's.
**/
#define ID_HIDE                 1L
#define ID_QUIT                 2L

/*
**      Information text.
**/
UBYTE *InfoTxt = ISEQ_C ISEQ_B ISEQ_HIGHLIGHT
		 "\33sCxDemo\n\n" ISEQ_TEXT ISEQ_N
		 "This is a small \"do-nothing\" example of how\n"
		 "to use the BGUI commodity class.\n"
		 "In this example F1 is the Hotkey used to\n"
		 "signal the broker to open the window.";

VOID StartDemo( void )
{
	Object  *CM_Broker, *WN_Window, *GA_Hide, *GA_Quit;
	ULONG           sigrec, type, id, rc;
	IPTR            signal = 0L, winsig = 0L;
	BOOL            running = TRUE;

	/*
	**      Setup a commodity object.
	**/
	CM_Broker = CommodityObject,
		COMM_Name,              "CxDemo",
		COMM_Title,             "Simple BGUI broker.",
		COMM_Description,       "Does not do anything useful.",
		COMM_ShowHide,          TRUE,
	EndObject;

	/*
	**      Object OK?
	**/
	if ( CM_Broker )
	{
		/*
		**      Create a small window.
		**/
		WN_Window = WindowObject,
			WINDOW_Title,           "CxDemo",
			WINDOW_RMBTrap,         TRUE,
			WINDOW_SizeGadget,      FALSE,  /* No use in this window. */
			WINDOW_AutoAspect,      TRUE,
			WINDOW_AutoKeyLabel,            TRUE,
			WINDOW_MasterGroup,
				VGroupObject, HOffset(6), VOffset(6), Spacing(6), GROUP_BackFill, SHINE_RASTER,
					StartMember,
						InfoObject, ButtonFrame,
							FRM_Flags,                              FRF_RECESSED,
							INFO_TextFormat,                InfoTxt,
							INFO_FixTextWidth,      TRUE,
							INFO_MinLines,                  6,
						EndObject,
					EndMember,
					StartMember,
						HGroupObject,
							StartMember, GA_Hide = PrefButton( "_Hide", ID_HIDE ), EndMember,
							VarSpace(DEFAULT_WEIGHT),
							StartMember, GA_Quit = PrefButton( "_Quit", ID_QUIT ), EndMember,
						EndObject, FixMinHeight,
					EndMember,
				EndObject,
			EndObject;

		/*
		**      Window OK?
		**/
		if ( WN_Window )
		{
			/*
			**      Add F1 as hotkey.
			**/
			if ( AddHotkey( CM_Broker, "f1", CX_F1_PRESSED, 0L ))
			{
				/*
				**              Obtain broker signal mask.
				**/
				GetAttr( COMM_SigMask, CM_Broker, &signal );
				/*
				**      Activate the broker.
				**/
				EnableBroker( CM_Broker );
				/*
				**      Open up the window.
				**/
				if ( WindowOpen( WN_Window ))
				{
					/*
					**      Obtain window sigmask.
					**/
					GetAttr( WINDOW_SigMask, WN_Window, &winsig );
					/*
					**      Wait for messages.
					**/
					do {
						sigrec = Wait( signal | winsig | SIGBREAKF_CTRL_C );
						/*
						**      Broker signal?
						**/
						if ( sigrec & signal )
						{
							/*
							**      Obtain the messages from the broker.
							**/
							while ( MsgInfo( CM_Broker, &type, &id, NULL ) != CMMI_NOMORE )
							{
			/*
			**      Evaluate message.
			**/
			switch ( type ) {

			   case    CXM_IEVENT:
			      switch ( id ) {

				 case    CX_F1_PRESSED:
				    goto openUp;
			      }
			      break;

			   case    CXM_COMMAND:
			      switch ( id ) {

				 case    CXCMD_KILL:
				    Tell( "bye bye\n" );
				    running = FALSE;
				    break;

				 case    CXCMD_DISABLE:
				    Tell( "broker disabled\n" );
				    DisableBroker( CM_Broker );
				    break;

				 case    CXCMD_ENABLE:
				    Tell( "broker enabled\n" );
				    EnableBroker( CM_Broker );
				    break;

				 case    CXCMD_UNIQUE:
				 case    CXCMD_APPEAR:
				    openUp:
				    if ( WindowOpen( WN_Window ))
				       GetAttr( WINDOW_SigMask, WN_Window, &winsig );
				    break;

				 case    CXCMD_DISAPPEAR:
				    WindowClose( WN_Window );
				    winsig = 0L;
				    break;
			      }
			      break;
			}
		     }
		  }

		  /*
		  **      Window signal?
		  **/
		  if ( sigrec & winsig ) {
		     while ( WN_Window && (( rc = HandleEvent( WN_Window )) != WMHI_NOMORE )) {
			switch ( rc ) {

			   case ID_HIDE:
			   case WMHI_CLOSEWINDOW:
			      /*
			      **      Hide the window.
			      **/
			      WindowClose( WN_Window );
			      winsig = 0L;
			      break;

			   case ID_QUIT:
			      /*
			      **      The end.
			      **/
			      Tell( "bye bye\n" );
			      running = FALSE;
			      break;
			}
		     }
		  }

		  /*
		  **      CTRL+C?
		  **/
		  if ( sigrec & SIGBREAKF_CTRL_C ) {
		     Tell( "bye bye\n" );
		     running = FALSE;
		  }
	       } while ( running );
	    } else
	       Tell( "unable to open the window\n" );
	 } else
	    Tell( "unable to add the hotkey\n" );
	 DisposeObject( WN_Window );
      } else
	 Tell( "unable to create a window object\n" );
      DisposeObject( CM_Broker );
   } else
      Tell( "unable to create a commodity object\n" );
}
