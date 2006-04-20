/*
    Copyright © 2002, The AROS Development Team.
    All rights reserved.
*/

//#include    <exec/types.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>

#include    <proto/alib.h>
#include    <proto/exec.h>
#include    <proto/dos.h>
#include    <proto/intuition.h>
#include    <proto/graphics.h>
#include    <proto/muimaster.h>

#include    <dos/dos.h>
#include    <intuition/gadgetclass.h>
#include    <intuition/icclass.h>
#include    <clib/alib_protos.h>

#include <libraries/mui.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NList_mcc.h>
#include <MUI/BetterString_mcc.h>

#include <utility/hooks.h>

/* Start Network Includes */
#include <proto/socket.h>
#include <bsdsocket/socketbasetags.h>
//#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "aircos_global.h"
#include "locale.h"

extern struct AiRcOS_internal *AiRcOS_Base;

extern int aircosApp_RegisterFunction(char *, void *, int , void *, int);
extern int aircosApp_RegisterAction(char *, void *, int , void *, int);

struct functionrecord DCC_commandList_array[];
struct functionrecord DCC_actionList_array[];

int aircos_DCCFuncs_RegisterFuncs(void)
{
  UBYTE pos;

  for (pos = 0; DCC_commandList_array[pos].command != NULL; pos++)
  {
     aircosApp_RegisterFunction(DCC_commandList_array[pos].command, DCC_commandList_array[pos].command_doFunction, DCC_commandList_array[pos].servArgCount, DCC_commandList_array[pos].command_clientFunction, DCC_commandList_array[pos].clientArgCount);
  }

  for (pos = 0; DCC_actionList_array[pos].command != NULL; pos++)
  {
     aircosApp_RegisterAction(DCC_actionList_array[pos].command, DCC_actionList_array[pos].command_doFunction, DCC_actionList_array[pos].servArgCount, DCC_actionList_array[pos].command_clientFunction, DCC_actionList_array[pos].clientArgCount);
  }

  return 1;
}

/* ************** IRC OPERATIONS !! ***************************** */
/** SERVER SIDE COMMANDS (RECIEVING) **/

int aircos_DCCFuncs_DoNop(struct IRC_Connection_Private	*currentConnection)
{
	struct serv_Outline *sa;

D(bug("[AiRcOS] ## DCC ## NOP('%s')\n",currentConnection->connection_serv_ARGS[0]));

   if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( currentConnection->connection_unprocessed ) + 1, MEMF_CLEAR ) ))
	{
      char *tmpline = (char *)&sa[1];
        strcpy( tmpline, currentConnection->connection_unprocessed );
        sa->so_name = (STRPTR)&sa[1];

//      DoMethod( currentConnection->connected_server->serv_status_output, MUIM_NList_InsertSingle, sa->so_name, MUIV_NList_Insert_Bottom );
//      DoMethod( currentConnection->connected_server->serv_status_output, MUIM_NList_Jump, MUIV_List_Jump_Bottom);
	}

   return 1;
}

/**********/

/**********/

struct functionrecord DCC_commandList_array[]=
{
    {NULL,NULL,0,NULL,0}
};

struct functionrecord DCC_actionList_array[]=
{
    {NULL,NULL,0,NULL,0}
};
