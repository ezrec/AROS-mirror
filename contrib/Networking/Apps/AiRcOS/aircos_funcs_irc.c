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

struct functionrecord IRC_commandList_array[];
struct functionrecord IRC_actionList_array[];

int aircos_IRCFuncs_RegisterFuncs(void)
{
  UBYTE pos;

  for (pos = 0; IRC_commandList_array[pos].command != NULL; pos++)
  {
     aircosApp_RegisterFunction(IRC_commandList_array[pos].command, IRC_commandList_array[pos].command_doFunction, IRC_commandList_array[pos].servArgCount, IRC_commandList_array[pos].command_clientFunction, IRC_commandList_array[pos].clientArgCount);
  }

  for (pos = 0; IRC_actionList_array[pos].command != NULL; pos++)
  {
     aircosApp_RegisterAction(IRC_actionList_array[pos].command, IRC_actionList_array[pos].command_doFunction, IRC_actionList_array[pos].servArgCount, IRC_actionList_array[pos].command_clientFunction, IRC_actionList_array[pos].clientArgCount);
  }

  return 1;
}

/* ************** IRC OPERATIONS !! ***************************** */
/** SERVER SIDE COMMANDS (RECIEVING) **/

int aircos_IRCFuncs_DoNop(struct IRC_Connection_Private	*currentConnection)
{
	struct serv_Outline *sa;

D(bug("[AiRcOS] ## IRC ## NOP('%s')\n",currentConnection->connection_serv_ARGS[0]));

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

struct functionrecord IRC_commandList_array[]=
{
    {"ADMIN"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"AWAY"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"CLOSE"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"CONNECT"  , aircos_IRCFuncs_DoNop,         3  , aircos_IRCFuncs_DoNop,         15 },
    {"DESCRIBE" , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"DIE"      , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"DNS"      , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"ERROR"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"HASH"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"HELP"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"INFO"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"INVITE"   , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"ISON"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"JOIN"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"KICK"     , aircos_IRCFuncs_DoNop,         3  , aircos_IRCFuncs_DoNop,         15 },
    {"KILL"     , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"LEAVE"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"LINKS"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"LIST"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"LUSERS"   , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"ME"       , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"MSG"      , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"MOTD"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"MODE"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"NAMES"    , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"NICK"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"NOTE"     , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"NOTICE"   , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"OPER"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"PART"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"PASS"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"PING"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"PONG"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"PRIVMSG"  , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"QUIT"     , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"REHASH"   , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"RESTART"  , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"SERVER"   , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"SQUIT"    , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"STATS"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"SUMMON"   , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"TIME"     , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"TOPIC"    , aircos_IRCFuncs_DoNop,         2  , aircos_IRCFuncs_DoNop,         15 },
    {"TRACE"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"USER"     , aircos_IRCFuncs_DoNop,         4  , aircos_IRCFuncs_DoNop,         15 },
    {"USERHOST" , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"USERS"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"VERSION"  , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"WALLOPS"  , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"WHO"      , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"WHOIS"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"WHOWAS"   , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {"IGNORE"   , aircos_IRCFuncs_DoNop,         1  , aircos_IRCFuncs_DoNop,         15 },
    {"QUOTE"    , aircos_IRCFuncs_DoNop,         15 , aircos_IRCFuncs_DoNop,         15 },
    {NULL,NULL,0,NULL,0}
};

struct functionrecord IRC_actionList_array[]=
{
    {NULL,NULL,0,NULL,0}
};
