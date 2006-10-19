/* main.c: */

#include <proto/utility.h>
#include <proto/exec.h>

#include <exec/ports.h>

#include <stdlib.h>
#include <stdio.h>

#include "cdromemu.h"

static char* version = "$VER: " VERSION "\n";

void main (int argc, char *argv[])
{
  int remove = 0;
  int insert = 0;
  struct MsgPort *port;
  struct IORequest ior;

  if (argc != 2 ||
      (!(remove = !Stricmp ("remove", argv[1])) &&
       !(insert = !Stricmp ("insert", argv[1])))) {
    fprintf (stderr, "usage: cdromemu (insert|remove)\n");
    exit (1);
  }

  ior.io_Command = insert ? CMD_XXX_INSERT : CMD_XXX_REMOVE;
  ior.io_Message.mn_Length = sizeof (ior);

  Forbid ();
  port = FindPort ((STRPTR) "CDROMEMU");
  if (!port) {
    Permit ();
    fprintf (stderr, "cdromemu.device is not running\n");
    exit (1);
  } else {
    PutMsg (port, (struct Message*) &ior);
    Permit ();
  }

  exit (0);
}
