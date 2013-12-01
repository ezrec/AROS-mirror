/*
 *  screen.c - Functions for grabbing and creating public screens
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010717 DM Created - only a function for finding public screens ATM
 *
 */

#include <exec/types.h>
#include <proto/intuition.h>

#include <exec/lists.h>
#include <exec/nodes.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>

#include <string.h>
#ifdef __VBCC__
#include <extra.h>
#endif




/* Finds the named public screen, or the default if NULL is supplied.
 * Specifying FRONT will search for the frontmost public screen in the
 * public screen list. Drops back to try to get Workbench if the named
 * screen could not be found, and if that fails, the default pubscreen.
 * Returns pointer to screen or NULL if one couldn't be found.
 */
struct Screen *FindPubScreen(STRPTR name)
{
	struct Screen           *pub = NULL;    /* The public screen we are going to lock */
	struct List             *psl;           /* The public screen list */
	struct PubScreenNode    *psn;           /* For traversing the public screen list */
	struct Screen           *front;         /* The frontmost screen */
	BOOL                    found;          /* Flag to show when to stop traversing lists */
	char                    lockname[MAXPUBSCREENNAME+1];   /* buffer name of screen to try to lock */


	if(name)
	{
		/* If the name is valid, we are either looking for a named screen
		 * or the frontmost public screen.
		 */
		if(stricmp(name, "FRONT")==0)
		{
			/* Find the frontmost public screen and copy name int buffer */
			psl = LockPubScreenList();
			front = IntuitionBase->FirstScreen;
			found = 0;
			while(front && !found)
			{
				psn = (struct PubScreenNode *)psl->lh_Head;
				while(psn->psn_Node.ln_Succ)
				{
					if(front==psn->psn_Screen && psn->psn_Flags&PSNF_PRIVATE==0)
					{
						found = 1;
						strncpy(lockname, psn->psn_Node.ln_Name, MAXPUBSCREENNAME);
					}
					psn = (struct PubScreenNode *)psn->psn_Node.ln_Succ;
				}
				front = front->NextScreen;
			}
			UnlockPubScreenList();
		}
		else
		{
			/* Not looking for front screen, so just copy name into buffer */
			strncpy(lockname, name, MAXPUBSCREENNAME);
		}

		/* Try to lock named screen (either found at front or named) and
		 * if that fails, try to grab the default public screen.
		 */
		if(NULL == (pub = LockPubScreen(lockname)) )
		{
			pub = LockPubScreen(NULL);
		}
	} /* if(name) */
	else
	{
		/* No name specified, go for the default public screen */
		pub = LockPubScreen(NULL);
	}

	if(!pub)
	{
		/* If we reach here, we will have failed to lock either the named/front
		 * public screen and/or the default public screen. Try the one which
		 * should exist.
		 */
		pub = LockPubScreen("Workbench");
	}

	return(pub);
}


