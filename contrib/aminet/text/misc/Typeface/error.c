/*****************/
/*		 */
/* Error handing */
/*		 */
/*****************/

#include "typeface.h"

void ErrorCode(ULONG code)
{
  ShowReq(GetString(msgFatalError),GetString(msgCancel),ErrorText(code));
  Quit();
}

char *ErrorText(ULONG code)
{
  switch (code)
  {
    case ASLALLOC:
      return "AllocAslRequest()";
      break;
    case CREATEPORT:
      return "CreateMsgPort()";
      break;
    case LOCKSCREEN:
      return "LockPubScreen()";
      break;
    case OPENSCREEN:
      return "OpenScreenTagList()";
      break;
    case ALLOCVEC:
      return "AllocVec()";
      break;
    case NEWWINDOW:
      return "BGUI_NewObject(WINDOW)";
      break;
    case NEWFILE:
      return "BGUI_NewObject(FILEREQ)";
      break;
    case OPENWINDOW:
      return "DoMethod(WM_OPEN)";
      break;
    case NEWPROP:
      return "NewObject(PROPGCLASS)";
      break;
    case NEWBUTTON:
      return "NewObject(BUTTONGCLASS)";
      break;
    default:
      return "<Unknown>";
  }
}
