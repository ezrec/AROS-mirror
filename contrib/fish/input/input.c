/********************************************
*  INPUT.C  08/04/90
*  © Copyright 1990 Timm Martin
*  This source code is freely distributable
*  and may be used without compensation in
*  any commercial or non-commercial product
*  as long as this notice is included and
*  remains intact.  This code may be used
*  in an executable program without
*  acknowledgement of the author.
*********************************************/

#include <aros/oldprograms.h>
#include <proto/console.h>

#include <devices/inputevent.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include "input.h"

extern struct Device *ConsoleDevice;

/*************
*  INPUT KEY
**************/

/*
Given a pointer to the IntuiMessage structure received for the current window
by Intuition, this function will return the key that was pressed, or zero if
no valid key was pressed.
*/

#define QUAL ievent.ie_Qualifier
#define SIZE 20L

USHORT input_key( struct IntuiMessage *imessage )
{
  UBYTE  buffer[SIZE];  /* buffer into which raw key conversion is read */
  short  codes = 0;     /* number of ANSI codes returned */
  static struct InputEvent ievent = { NULL, IECLASS_RAWKEY, 0, 0, 0 };
  USHORT key = 0;       /* key value returned by this function (see below) */
  USHORT qual = 0;      /* key qualifiers (shift, alt, ctl, etc.) */

  /* copy rawkey codes and qualifiers into InputEvent structure for conversion */
  ievent.ie_Code = imessage->Code;
  QUAL = imessage->Qualifier;

/*

LG = left Amiga key   RG = right Amiga key
LA = left ALT key     RA = right ALT key
LS = left SHIFT key   RS = right SHIFT key
CT = ConTRoL key
CO = command key

Format of the imessage->Qualifier variable (necessary bits only):

                                RG  LG  RA  LA  CT      RS  LS  <-- keys
15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00  <-- bits

 0   0   0   0   0   0   0   0   1   1   1   1   1   0   0   0  <-- mask
               |               |               |                <-- bytes
       0               0               F               8        <-- hex
 <------- 8 bit positions -------<                              <-- shift left
RG  LG  RA  LA  CT                                              <-- new pos
                              and
 0   0   0   0   0   0   0   0   0   0   0   0   0   0   1   1  <-- mask
               |               |               |                <-- bytes
       0               0               0               3        <-- hex
                     <--------- 9 bit positions ---------<      <-- shift left
                    RS  LS                                      <-- new pos

Format of the "key" variable returned by sfInputKey() function:

RG  LG  RA  LA  CT  RS  LS  CO  (-------- key value ---------)
15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00  <-- bits

where key value equals:
  if CO (command) bit set (== 1):
    command key value as defined in input.h
  if CO (command) bit cleared (== 0):
    ASCII value of key pressed

*/

  /* determine key qualifier as shown above */
  qual = ((QUAL & 0xf8) << 8) | ((QUAL & 0x03) << 9);

  /* if control key, ignore this fact so that RawKeyConvert() will process
   * (remember that the console device processes some control key sequences
   * as other keys, for example, ^M equals RETURN)
   */
  if (QUAL & IEQUALIFIER_CONTROL)
    QUAL &= ~IEQUALIFIER_CONTROL;

  /* used for converting dead keys */
  ievent.ie_position.ie_addr = *((APTR *)imessage->IAddress);
  /* convert rawkey */
  codes = RawKeyConvert( &ievent, (char *)buffer, SIZE, NULL );

  /* if any codes returned (i.e., down transition and not a dead key) */
  if (codes)
  {
    switch (buffer[0])
    {
      /* single code keys */
      case 0x1b: key = COMMAND_ESCAPE; break;
      case 0x08: key = COMMAND_BACKSPACE; break;
      case 0x7f: key = COMMAND_DELETE; break;
      case 0x09: key = COMMAND_TAB; break;
      case 0x0d: key = COMMAND_RETURN; break;
      case 0x9b:
        if (codes > 1)
        {
          /* "escape" code keys */
          switch (buffer[1])
          {
            case 0x5a: key = COMMAND_TAB; break;  /* shift-TAB */
            case 0x3f: key = COMMAND_HELP; break;
            case 0x54:
            case 0x41: key = COMMAND_UP_ARROW; break;
            case 0x53:
            case 0x42: key = COMMAND_DOWN_ARROW; break;
            case 0x43: key = COMMAND_RIGHT_ARROW; break;
            case 0x44: key = COMMAND_LEFT_ARROW; break;
            case 0x20:
              switch (buffer[2])
              {
                /* shifted-arrow keys */
                case 0x40: key = COMMAND_RIGHT_ARROW; break;
                case 0x41: key = COMMAND_LEFT_ARROW; break;
              }
              break;
            default:
              /* function keys F1-F10 */
              if (buffer[2] == 0x7e)
                key = COMMAND_F1 + (int)buffer[1]-0x30;
              else if (buffer[3] == 0x7e)
                key = COMMAND_F1 + (int)buffer[2]-0x30;
              break;
          }
        }
        else
          key = COMMAND_ESCAPE;
        break;
    }
    /* if key!=0, a command key was pressed */
    if (key)
      key += QUAL_COMMAND;
    /* else no command key, must've been ASCII character */
    else
      key = buffer[0];
  }
  /* attach qualifiers to key */
  if (key)
    key += qual;

  return (key);
}
