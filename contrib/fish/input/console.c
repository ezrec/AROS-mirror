/*******************************
*  CONSOLE  08/04/90
*  Written by Timm Martin 
*  This code is public domain.
********************************/

#include <aros/oldprograms.h>
#include <devices/console.h>
#include <exec/devices.h>
#include <exec/types.h>
#include "input.h"

struct Device *ConsoleDevice = NULL;
struct IOStdReq ConsoleReq;
long ConsoleError = TRUE;

/****************
*  CONSOLE OPEN
*****************/

/* 
This function attempts to open the console device.  TRUE or FALSE is returned
whether it was successful.
*/

BOOL console_open( void )
{
  if (!(ConsoleError = OpenDevice( "console.device", -1L,
                                 (struct IORequest *)&ConsoleReq, 0 )))
    ConsoleDevice = ConsoleReq.io_Device;

  return (ConsoleError == FALSE);
}

/*****************
*  CONSOLE CLOSE
******************/

/* 
This procedure closes the console device if it was opened and resets the
corresponding pointers.
*/

void console_close( void )
{
  if (!ConsoleDevice)
  {
    CloseDevice( (struct IORequest *)&ConsoleReq );
    ConsoleDevice = NULL;
  }
}
