

#include "find.h"
UBYTE *messages[] =
      {
       "0: OK",
       "1: No memory for new size",
       "2: Insert < 0",
       "3: Substitute < 0",
       "4: Delete < 0",
       "5: Filter <0 or >127",
       "6: Size < 20",
       "7: Please Wait",
       "8: File not found",
       "9: Not a text file",
       "10: Loading file...",
       "11: Program launched twice !",
       "12: Printing List",
       "13: Saving List",
       "14: Couldn't open file",
       "15: Couldn't get Printer",
       "16: File saved",
       "17: File printed",
       "18: loading configuration...",
       "19: saving configuration...",
       "20: Not a configuration file",
       "21: Couldn't open read file",
       "22: Couldn't open write file",
       "23: Building ClipList",
       "24: ClipList ready",
      };
    

void putmsg(ULONG msg)
{
 if(Project0Wnd)GT_SetGadgetAttrs(Project0Gadgets[GDX_errors],Project0Wnd,NULL,GTTX_Text,(ULONG)messages[msg],TAG_DONE);
 info.misc.lasterror = msg;
}
