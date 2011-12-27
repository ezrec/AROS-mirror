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
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/BetterString_mcc.h>

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
    {"ADMIN"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"AWAY"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"CLOSE"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"CONNECT"  , (APTR)aircos_IRCFuncs_DoNop,         3  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"DESCRIBE" , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"DIE"      , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"DNS"      , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"ERROR"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"HASH"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"HELP"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"INFO"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"INVITE"   , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"ISON"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"JOIN"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"KICK"     , (APTR)aircos_IRCFuncs_DoNop,         3  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"KILL"     , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"LEAVE"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"LINKS"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"LIST"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"LUSERS"   , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"ME"       , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"MSG"      , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"MOTD"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"MODE"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"NAMES"    , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"NICK"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"NOTE"     , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"NOTICE"   , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"OPER"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"PART"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"PASS"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"PING"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"PONG"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"PRIVMSG"  , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"QUIT"     , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"REHASH"   , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"RESTART"  , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"SERVER"   , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"SQUIT"    , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"STATS"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"SUMMON"   , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"TIME"     , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"TOPIC"    , (APTR)aircos_IRCFuncs_DoNop,         2  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"TRACE"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"USER"     , (APTR)aircos_IRCFuncs_DoNop,         4  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"USERHOST" , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"USERS"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"VERSION"  , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"WALLOPS"  , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"WHO"      , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"WHOIS"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"WHOWAS"   , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"IGNORE"   , (APTR)aircos_IRCFuncs_DoNop,         1  , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {"QUOTE"    , (APTR)aircos_IRCFuncs_DoNop,         15 , (APTR)aircos_IRCFuncs_DoNop,         15 },
    {NULL,NULL,0,NULL,0}
};

struct functionrecord IRC_actionList_array[]=
{
    {NULL,NULL,0,NULL,0}
};
