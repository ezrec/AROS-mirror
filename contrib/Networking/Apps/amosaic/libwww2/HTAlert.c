/*	Displaying messages and getting input for LineMode Browser
**	==========================================================
**
**	REPLACE THIS MODULE with a GUI version in a GUI environment!
**
** History:
**	   Jun 92 Created May 1992 By C.T. Barker
**	   Feb 93 Simplified, portablised (ha!) TBL
**
*/


#include "HTAlert.h"

#include "tcp.h"		/* for TOUPPER */
#include <ctype.h> 		/* for toupper - should be in tcp.h */

extern void mo_gui_notify_progress (CONST char *);
extern int mo_gui_check_icon (int);
extern void mo_gui_clear_icon (void);

PUBLIC void HTAlert ARGS1(CONST char *, Msg)
{
  mo_gui_notify_progress (Msg);
  return;
}

PUBLIC void HTProgress ARGS1(CONST char *, Msg)
{
  mo_gui_notify_progress (Msg);
  return;
}

PUBLIC int HTCheckActiveIcon ARGS1(int, twirl)
{
  int ret;

  ret = mo_gui_check_icon (twirl);
  return(ret);
}

PUBLIC void HTClearActiveIcon NOARGS
{
  mo_gui_clear_icon ();
  return;
}

PUBLIC BOOL HTConfirm ARGS1(CONST char *, Msg)
{
  extern int prompt_for_yes_or_no (CONST char *);

  if (prompt_for_yes_or_no (Msg))
    return(YES);
  else
    return(NO);
}

PUBLIC char * HTPrompt ARGS2(CONST char *, Msg, CONST char *, deflt)
{
  extern char *prompt_for_string (CONST char *);
  char *Tmp = prompt_for_string (Msg);
  char *rep = 0;

  StrAllocCopy (rep, (Tmp && *Tmp) ? Tmp : deflt);
  return rep;
}

PUBLIC char * HTPromptPassword ARGS1(CONST char *, Msg)
{
  extern char *prompt_for_password (CONST char *);
  char *Tmp = prompt_for_password (Msg);

  return Tmp;
}
