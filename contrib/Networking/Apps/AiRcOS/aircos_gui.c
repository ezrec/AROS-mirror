/*
    Copyright © 2002, The AROS Development Team.
    All rights reserved.

    $Id: aircos_gui.c,v 1.1 2005/01/04 03:29:51 nicja Exp $
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


extern struct IRC_Channel_Priv  *FindNamedChannel( struct IRC_Connection_Private *onThisConnection, char *findThisChan);
extern Object	*aircos_showServerConnect();
extern void aircos_showChanOutput(struct IRC_Channel_Priv *thisChanPriv, struct serv_Outline *sa);
extern int aircos_sendline(struct IRC_Connection_Private *forConnection);

/******* DATA : INTERN PROTOS *******/

struct IRC_Channel_Priv  *aircos_add_channel(char *addtoserv,char *addchann);
struct IRC_Channel_Group_User *aircos_Add_ChannelUser( struct IRC_Channel_Priv  * addToChannel, char * addToThisGroup, char * addThisUser);
struct IRC_Channel_Group_User *aircos_Find_User( struct IRC_Channel_Priv  * searchChannel, char * searchGroup, char * findThisUser);

/******* CODE START ********/


/******* SOCKET FUNCS ******/

char *FormatToSend(char *unformatted_string)
{
   /* Convert all Input notation formatting to IRC noatation */
   return unformatted_string;
}

char *FormatToDisplay(char *formatted_string)
{
   /* Convert all IRC notation formatting to our outputs format */
   return formatted_string;
}

/* ************** IRC OPERATIONS !! ***************************** */
/** SERVER SIDE COMMANDS (RECIEVING) **/

int aircos_IRC_nop(struct IRC_Connection_Private	*currentConnection)
{
	struct serv_Outline *sa;

D(bug("[AiRcOS] ## IRC ## NOP('%s')\n",currentConnection->connection_serv_ARGS[0]));

   if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( currentConnection->connection_unprocessed ) + 1, MEMF_CLEAR ) ))
	{
      char *tmpline = (char *)&sa[1];
		strcpy( tmpline, currentConnection->connection_unprocessed );
		sa->so_name = (STRPTR)&sa[1];

      DoMethod( currentConnection->connected_server->serv_status_output, MUIM_NList_InsertSingle, sa->so_name, MUIV_NList_Insert_Bottom );
      DoMethod( currentConnection->connected_server->serv_status_output, MUIM_NList_Jump, MUIV_List_Jump_Bottom);
	}

   return 1;
}

static int aircos_IRC_doerror(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoError()\n"));
    return 2;
}

static int aircos_IRC_doinvite(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoInvite()\n"));
    return 0;
}

static int aircos_IRC_dojoin(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoJoin('%s') [ nick='%s', server ='%s' ]\n", currentConnection->connection_serv_ARGS[0], currentConnection->connection_nick, currentConnection->connection_server));

    if (strcasecmp(currentConnection->connection_serv_ARGS[0], currentConnection->connection_nick) == 0)
	{
D(bug("[AiRcOS]#DoJoin# YOU have joined channel ..\n"));
      struct IRC_Channel_Priv *irc_newChannel = NULL;
      irc_newChannel = aircos_add_channel(currentConnection->connected_server->serv_name, currentConnection->connection_serv_ARGS[2]);
    }
	else
	{
D(bug("[AiRcOS]#DoJoin# User '%s' joined channel ..\n",currentConnection->connection_serv_ARGS[0]));

      struct IRC_Channel_Priv *thisChanPriv = NULL;
      if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
      {
D(bug("[AiRcOS](irc:DoJoin) Found internal channel record\n"));
         char *username = currentConnection->connection_serv_ARGS[0]; 

	      struct serv_Outline *sa;

         if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( currentConnection->connection_serv_ARGS[2] ) + 20, MEMF_CLEAR ) ))
	      {
D(bug("[AiRcOS](irc:DoJoin) Displaying Message\n"));
            char *tmpline = (char *)&sa[1];

		      sprintf( tmpline, "\0335*** %s has joined %s", username, currentConnection->connection_serv_ARGS[2] );
		      sa->so_name = (STRPTR)&sa[1];

            aircos_showChanOutput(thisChanPriv,sa);
	      }
	      
	      /* Add the user to the usergroup */
         if (username[0] == '@') aircos_Add_ChannelUser( thisChanPriv, "Admins", username+1);
         else if (username[0] == '+') aircos_Add_ChannelUser( thisChanPriv, "Voice", username+1);
         else aircos_Add_ChannelUser( thisChanPriv, "Normal", username);
      }
	}

    return 0;
}

static int aircos_IRC_dokick(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoKick()\n"));
//    printf("*** %s was kicked from %s by %s (%s)", currentConnection->connection_serv_ARGS[3], currentConnection->connection_serv_ARGS[2], currentConnection->connection_serv_ARGS[0], currentConnection->connection_serv_ARGS[4]);

    return 0;
}

static int aircos_IRC_dokill(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoKill()\n"));
//    printf("*** %s killed by %s: ", currentConnection->connection_serv_ARGS[3], currentConnection->connection_serv_ARGS[0]);
    if (strcasecmp(currentConnection->connection_serv_ARGS[3], currentConnection->connection_nick) == 0) AiRcOS_Base->Ai_reconnect = 0;
    return 4;
}

static int aircos_IRC_domode(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoMode()\n"));

   struct IRC_Channel_Priv *thisChanPriv = NULL;

   if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
   {
D(bug("[AiRcOS](irc;DoMode) Found record for channel '%s'\n", currentConnection->connection_serv_ARGS[2]));
	   /* Find users usergroup record and remove */
      struct IRC_Channel_Group_SubGroup *current_Group=NULL;
      ForeachNode(&thisChanPriv->chan_usergroup, current_Group)
      {
         struct IRC_Channel_Group_User *change_ChanUser = NULL;
         if (( change_ChanUser = aircos_Find_User( thisChanPriv, current_Group->group_name, currentConnection->connection_serv_ARGS[4])))
         {
D(bug("[AiRcOS](irc;DoMode) Users Record for '%s' found\n", currentConnection->connection_serv_ARGS[4]));
            char *new_modeset = currentConnection->connection_serv_ARGS[3];
            char *new_group = NULL;

D(bug("[AiRcOS](irc;DoMode) Checking if mode changed ..\n"));
            if (new_modeset[1] == 'o')
            {
               if (new_modeset[0] == '+') new_group = "Admins";
               else new_group = "Normal";
            }
            else if (new_modeset[1] == 'v')
            {
               if (new_modeset[0] == '+') new_group = "Voice";
               else new_group = "Normal";
            }
            else
            {
               new_group = "Normal";
            }
            
            if (strcasecmp(current_Group->group_name, new_group)!=0)
            {
D(bug("{AiRcOS](irc;DoMode) CHANGE : removing old user record\n"));
               Remove(&change_ChanUser->group_node);
               struct serv_Outline *sa = NULL;

               if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( currentConnection->connection_serv_ARGS[0] ) + strlen( currentConnection->connection_serv_ARGS[3] ) + strlen( currentConnection->connection_serv_ARGS[4] ) + 30, MEMF_CLEAR ) ))
               {
D(bug("[AiRcOS](irc;DoMode) Displaying Message\n"));
                  sa->so_name = (STRPTR)&sa[1];

   		         sprintf( sa->so_name, "\0335*** %s sets mode: %s %s", currentConnection->connection_serv_ARGS[0], currentConnection->connection_serv_ARGS[3], currentConnection->connection_serv_ARGS[4]);
	
                  aircos_showChanOutput(thisChanPriv,sa);
   	         }

               DoMethod( thisChanPriv->chan_users, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, change_ChanUser->user_obj );
D(bug("[AiRcOS](irc;DoMode) ReInserting User\n"));                        
               aircos_Add_ChannelUser( thisChanPriv, new_group, change_ChanUser->user_name);
            
D(bug("[AiRcOS](irc;DoMode) Freeing old User resources\n"));
               FreeVec(change_ChanUser->user_displayname);
               FreeVec(change_ChanUser->user_name);
               FreeVec(change_ChanUser);
            }
            else
            {
D(bug("[AiRcOS](irc;DoMode) Mode remains the same\n"));
            }
         }
      }
   }
   return 3;
}

static int aircos_IRC_donick(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoNick()\n"));

   char *nicknamenew = NULL;

    if (strcasecmp(currentConnection->connection_serv_ARGS[0], currentConnection->connection_nick) == 0)
    {
        if ((nicknamenew = (char *)AllocVec(strlen(currentConnection->connection_serv_ARGS[2])+1,MEMF_CLEAR)))
      {
         strcpy(nicknamenew, currentConnection->connection_serv_ARGS[2]);
         FreeVec(currentConnection->connection_nick);
         currentConnection->connection_nick = nicknamenew;
      }
    }

   struct IRC_Channel_Priv *thisChanPriv = NULL;

   char *username = currentConnection->connection_serv_ARGS[0];
   nicknamenew = currentConnection->connection_serv_ARGS[2];
      
   ForeachNode(&currentConnection->connected_server->serv_chans, thisChanPriv)
   {
D(bug("[AiRcOS](irc;DoNick) Searching For user record on '%s'\n",thisChanPriv->chan_name));

	   struct serv_Outline *sa = NULL;

	   /* Find users usergroup record and remove */
      struct IRC_Channel_Group_SubGroup *current_Group=NULL;
      ForeachNode(&thisChanPriv->chan_usergroup, current_Group)
      {
         struct IRC_Channel_Group_User *rename_ChanUser = NULL;
         if ((rename_ChanUser = aircos_Find_User( thisChanPriv, current_Group->group_name, username)))
         {
D(bug("[AiRcOS](irc;DoNick) Users Record found - removing\n"));
            Remove(&rename_ChanUser->group_node);
            
            if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( nicknamenew ) + 30, MEMF_CLEAR ) ))
            {
D(bug("[AiRcOS](irc;DoNick) Displaying Message\n"));
               char *tmpline = (char *)&sa[1];

		       sprintf( tmpline, "\0335*** %s has changed nick to %s", username, nicknamenew );
		       sa->so_name = (STRPTR)&sa[1];
		
               aircos_showChanOutput(thisChanPriv,sa);
	         }

            DoMethod( thisChanPriv->chan_users, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, rename_ChanUser->user_obj );
            
            FreeVec(rename_ChanUser->user_displayname);
            FreeVec(rename_ChanUser->user_name);
            FreeVec(rename_ChanUser);
            
            aircos_Add_ChannelUser( thisChanPriv, current_Group->group_name, nicknamenew);
         }
      }
   }

    return 0;
}

static int aircos_IRC_donotice(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoNotice()\n"));

    if (!isIRCChannel(currentConnection->connection_serv_ARGS[2]))
	{
D(bug("[AiRcOS](irc;DoNotice.?) Do Server notice?\n"));
	}
    else
	{
      struct IRC_Channel_Priv *thisChanPriv = NULL;
      if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
      {
D(bug("[AiRcOS](irc;DoNotice.chan) Found internal channel record\n"));
         char *username = currentConnection->connection_serv_ARGS[0]; 

	      struct serv_Outline *sa;

         if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( currentConnection->connection_serv_ARGS[3] ) + 4, MEMF_CLEAR ) ))
	      {
D(bug("[AiRcOS](irc;DoNotice.chan) Displaying Message\n"));
            char *tmpline = (char *)&sa[1];

	   	   sprintf( tmpline, "\0335-%s- %s", username, currentConnection->connection_serv_ARGS[3] );
	   	   sa->so_name = (STRPTR)&sa[1];

            aircos_showChanOutput(thisChanPriv,sa);
         }
      }	      
	}

    return 3;
}

static int aircos_IRC_dopart(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoPart()\n"));

   struct IRC_Channel_Priv *thisChanPriv = NULL;

   if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
   {
D(bug("[AiRcOS](irc;DoPart) Found internal channel record\n"));

      char *username = currentConnection->connection_serv_ARGS[0]; 

	   struct serv_Outline *sa;

      if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( currentConnection->connection_serv_ARGS[2] ) + 20, MEMF_CLEAR ) ))
      {
D(bug("[AiRcOS](irc;DoPart) Displaying Message\n"));
         char *tmpline = (char *)&sa[1];

         sprintf( tmpline, "\0335*** %s has left %s", username, currentConnection->connection_serv_ARGS[2] );
		   sa->so_name = (STRPTR)&sa[1];

         aircos_showChanOutput(thisChanPriv,sa);
	   }
	   /* Find users usergroup record and remove */
      struct IRC_Channel_Group_SubGroup *current_Group=NULL;
      ForeachNode(&thisChanPriv->chan_usergroup, current_Group)
      {
         struct IRC_Channel_Group_User *left_ChanUser = NULL;
         if ((left_ChanUser = aircos_Find_User( thisChanPriv, current_Group->group_name, username)))
         {
D(bug("[AiRcOS](irc;DoPart) Users Record found - removing\n"));
            Remove(&left_ChanUser->group_node);
            DoMethod( thisChanPriv->chan_users, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, left_ChanUser->user_obj );
            
            FreeVec(left_ChanUser->user_displayname);
            FreeVec(left_ChanUser->user_name);
            FreeVec(left_ChanUser);
         }
      }
   }

   return 0;
}

static int aircos_IRC_dopong(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoPong()\n"));

//    column = printf("*** Got PONG from %s:", currentConnection->connection_serv_ARGS[0]);
    return 3;
}

static int aircos_IRC_handleAction(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] handleAction('%s')\n", currentConnection->connection_unprocessed));
   if ((AiRcOS_Base->Ai_tmp = strchr(currentConnection->connection_unprocessed, ' ')))
   {
       char *action_command = NULL;
       char *action_param = NULL;

       ULONG action_command_len = AiRcOS_Base->Ai_tmp - currentConnection->connection_unprocessed;
D(bug("[AiRcOS](irc;handleAction) command length %d\n", action_command_len));
       
       if (!(action_command = AllocVec(action_command_len + 2, MEMF_CLEAR)))
         return 0;

       strncpy(action_command, currentConnection->connection_unprocessed, action_command_len);
       action_command[action_command_len +1] = '\0';
       action_param = currentConnection->connection_unprocessed + action_command_len +1;
       
/* Handle Actions .. */

      if (strcmp("ACTION",action_command)==0)
      {
D(bug("[AiRcOS](irc;handleAction) Found 'ACTION' command\n"));
         struct IRC_Channel_Priv  *thisChanPriv = NULL;
         if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
         {
D(bug("[AiRcOS](irc;handleAction) 'ACTION' : Found internal channel record\n"));
	         struct serv_Outline *sa = NULL;
            char *username = currentConnection->connection_serv_ARGS[0]; 

            if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( action_param ) + 10, MEMF_CLEAR ) ))
	         {
D(bug("[AiRcOS](irc;handleAction) 'ACTION' : Displaying Message\n"));
               sa->so_name = (STRPTR)&sa[1];

		         sprintf( sa->so_name, "\033P[-3]*** %s %s", username, action_param );

               aircos_showChanOutput(thisChanPriv,sa);
	        }
         }
         return 0;
      }
//    else if (strcmp("XXXXXXX",action_command)==0)
//    else if (strcmp("XXXXXXX",action_command)==0)
   }
   return 0;
}

static int aircos_IRC_doprivmsg(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoPrivMsg()\n"));

#warning "TODO : DoPrivMsg() If the user is on our ignore list, do nothing.."

    if (!isIRCChannel(currentConnection->connection_serv_ARGS[2]))
	{
D(bug("[AiRcOS](irc;DoPrivMsg) Doing PrivMessage from .. A\n"));
	}
   else
	{
/* Handle channel message */
D(bug("[AiRcOS](irc;DoPrivMsg) Doing PrivMessage from .. B\n"));
      struct IRC_Channel_Priv  *thisChanPriv = NULL;
      
      if (*currentConnection->connection_serv_ARGS[3] == '\001')
      {
// Handle Action message
D(bug("[AiRcOS](irc;DoPrivMsg.B) Handle ACTION message ...\n"));
         char * end_actionmsg = currentConnection->connection_serv_ARGS[3];
         int i;
         
         for ( i = 1; i < strlen(currentConnection->connection_serv_ARGS[3]); i++)
         {
            if (end_actionmsg[i] == '\001')
            {
               end_actionmsg[i] = '\0';
               end_actionmsg = &end_actionmsg[i];
               break;
            }         
         }
         
         if ((end_actionmsg != currentConnection->connection_serv_ARGS[3])&&(currentConnection->connection_unprocessed))
         {
            ULONG actionmsg_size = end_actionmsg - currentConnection->connection_serv_ARGS[3];
            
D(bug("[AiRcOS](irc;DoPrivMsg.B) Processing ACTION Message [len:%d]\n", actionmsg_size));
            strncpy(currentConnection->connection_unprocessed, currentConnection->connection_serv_ARGS[3] +1, actionmsg_size);
            aircos_IRC_handleAction(currentConnection);
         }
      }      
      else
      {
D(bug("[AiRcOS](irc;DoPrivMsg.B) Handle Standard message ...\n"));
      if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
      {
D(bug("[AiRcOS](irc;DoPrivMsg.B) Found internal channel record\n"));

         char *username = currentConnection->connection_serv_ARGS[0]; 

	      struct serv_Outline *sa;

         if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( currentConnection->connection_serv_ARGS[3] ) + 5, MEMF_CLEAR ) ))
	      {
D(bug("[AiRcOS](irc;DoPrivMsg) B: Displaying Message\n"));
		      sa->so_name = (STRPTR)&sa[1];

		      sprintf( sa->so_name, "<%s> %s", username, currentConnection->connection_serv_ARGS[3] );

            aircos_showChanOutput(thisChanPriv,sa);
	      }
      }
      }
	}

    return 3;
}

static int aircos_IRC_doquit(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoQuit()\n"));

   struct IRC_Channel_Priv *thisChanPriv = NULL;
   char *username = currentConnection->connection_serv_ARGS[0]; 
      
   ForeachNode(&currentConnection->connected_server->serv_chans, thisChanPriv)
   {
D(bug("[AiRcOS](irc;DoPart) Searching For user record on '%s'\n",thisChanPriv->chan_name));
	   struct serv_Outline *sa = NULL;

	   /* Find users usergroup record and remove */
      struct IRC_Channel_Group_SubGroup *current_Group=NULL;
      ForeachNode(&thisChanPriv->chan_usergroup, current_Group)
      {
         struct IRC_Channel_Group_User *left_ChanUser = NULL;
         if ((left_ChanUser = aircos_Find_User( thisChanPriv, current_Group->group_name, username)))
         {
D(bug("[AiRcOS](irc;DoQuit) Users Record found - removing\n"));
            Remove(&left_ChanUser->group_node);
            
            if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( currentConnection->connection_serv_ARGS[2] ) + 28, MEMF_CLEAR ) ))
            {
D(bug("[AiRcOS](irc;DoQuit) Displaying Message\n"));
               char *tmpline = (char *)&sa[1];

		       sprintf( tmpline, "\0335*** %s has quit IRC ('%s')", username, currentConnection->connection_serv_ARGS[2] );
		       sa->so_name = (STRPTR)&sa[1];
		
               aircos_showChanOutput(thisChanPriv,sa);
	         }

            DoMethod( thisChanPriv->chan_users, MUIM_NListtree_Remove, MUIV_NListtree_Remove_ListNode_Root, left_ChanUser->user_obj );
            
            FreeVec(left_ChanUser->user_displayname);
            FreeVec(left_ChanUser->user_name);
            FreeVec(left_ChanUser);
         }
      }
   }
   
   return 0;
}

static int aircos_IRC_dotopic(struct IRC_Connection_Private	*currentConnection)
{
D(bug("[AiRcOS] ## IRC ## DoTopic()\n"));

   struct IRC_Channel_Priv  *thisChanPriv = NULL;
      
   if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[2])))
   {

D(bug("[AiRcOS](irc;DoTopic) Changing topic display\n"));

      char *tmptopic = AllocVec(strlen(currentConnection->connection_serv_ARGS[3]) + strlen(thisChanPriv->chan_name)+4,MEMF_CLEAR);
      
      sprintf(tmptopic, "%s: %s", thisChanPriv->chan_name, currentConnection->connection_serv_ARGS[3]);
      set(thisChanPriv->chan_topic_obj, MUIA_Text_Contents, tmptopic);
      if (thisChanPriv->chan_topic) FreeVec(thisChanPriv->chan_topic);
      thisChanPriv->chan_topic = tmptopic;
      
      /* Output confirmation of the change on the channel */
      
      char *username = currentConnection->connection_serv_ARGS[0]; 
	   struct serv_Outline *sa;

      if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( username ) + strlen( currentConnection->connection_serv_ARGS[3] ) + 26, MEMF_CLEAR ) ))
	   {
D(bug("[AiRcOS](irc;DoTopic) Displaying Message in local channel\n"));
         char *tmpline = (char *)&sa[1];

		   sprintf( tmpline, "\0335*** %s changes topic to %s", username, currentConnection->connection_serv_ARGS[3] );
		   sa->so_name = (STRPTR)&sa[1];
		
         aircos_showChanOutput(thisChanPriv,sa);
	   }
      
   }
   return 0;
}

int aircos_IRC_donumeric(struct IRC_Connection_Private	*currentConnection, int num)
{
D(bug("[AiRcOS] ## IRC ## DoNumeric(%d)\n",num));
   // Process IRC commands given by numeric value ....

   switch (num)
	{
	case 001: /* RPL_WELCOME */
	case 002: /* RPL_YOURHOST */
	case 003: /* RPL_CREATED */
	case 004: /* RPL_MYINFO */
	case 005:
	{
// Show Server connection messages on server log window
// * raw buff should be longer than the string we need since it contains the original unparsed vers
//   so just use it ..
      if (currentConnection->connection_unprocessed) sprintf( currentConnection->connection_unprocessed, "%s *** %s", currentConnection->connection_serv_ARGS[0], currentConnection->connection_serv_ARGS[3] );

      return aircos_IRC_nop(currentConnection);
   }

	case 332: /* RPL_TOPIC */
   {
//Do chan connection topic
      struct IRC_Channel_Priv  *thisChanPriv = NULL;
      
      if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[3])))
      {

D(bug("[AiRcOS](numeric;initialtopic) Changing topic display\n"));

         char *tmptopic = AllocVec(strlen(currentConnection->connection_serv_ARGS[4]) + strlen(thisChanPriv->chan_name)+4,MEMF_CLEAR);
      
         sprintf(tmptopic, "%s: %s", thisChanPriv->chan_name, currentConnection->connection_serv_ARGS[4]);
         set(thisChanPriv->chan_topic_obj, MUIA_Text_Contents, tmptopic);
         if (thisChanPriv->chan_topic) FreeVec(thisChanPriv->chan_topic);
         thisChanPriv->chan_topic = tmptopic;
      
         /* Output confirmation of the initial channel topic */

	      struct serv_Outline *sa;

         if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( currentConnection->connection_serv_ARGS[4] ) + 22, MEMF_CLEAR ) ))
	      {
D(bug("[AiRcOS](numeric;initialtopic) Displaying Message in local channel\n"));
            char *tmpline = (char *)&sa[1];

		      sprintf( tmpline, "\0335*** Initial topic: %s", currentConnection->connection_serv_ARGS[4] );
		      sa->so_name = (STRPTR)&sa[1];

            aircos_showChanOutput(thisChanPriv,sa);
	      }
      
      }
      return 0;
   }

   case 375: /* RPL_MOTDSTART */
	case 372: /* RPL_MOTD */
   case 376: /* RPL_ENDOFMOTD */
   {
// Show MOTD messages on server log window
// * raw buff should be longer than the string we need since it contains the original unparsed vers
//   so just use it ..
      if (currentConnection->connection_unprocessed) sprintf( currentConnection->connection_unprocessed, "%s *** %s", currentConnection->connection_serv_ARGS[0], currentConnection->connection_serv_ARGS[3] );

      return aircos_IRC_nop(currentConnection);
   }

   case 351: /* RPL_WHOREPLY */
// ## UNHANDLED NUMERIC FUNC
      return 0;
	     
   case 352: /* RPL_WHOREPLY */
// ## UNHANDLED NUMERIC FUNC
//   inputs
//      arg = currentConnection->connection_serv_ARGS[3]
//      username = currentConnection->connection_serv_ARGS[4]
//      host = currentConnection->connection_serv_ARGS[5]
//      arg = currentConnection->connection_serv_ARGS[7]
//      arg = currentConnection->connection_serv_ARGS[8]
		return 9;

   case 353: /* RPL_NAMEREPLY */
   {
// Process the channel user list ...
      struct IRC_Channel_Priv  *thisChanPriv = NULL;
D(bug("[AiRcOS](numeric) Parsing userlist ..\n"));
      if ((thisChanPriv = FindNamedChannel(currentConnection, currentConnection->connection_serv_ARGS[4])))
      {
D(bug("[AiRcOS](numeric;userlist) Found channel record for '%s'\n", currentConnection->connection_serv_ARGS[4]));
         char *userlist = currentConnection->connection_serv_ARGS[5];
         int   userlist_len = strlen(userlist),
               i=0,
               namestart=0,
               usercount=0;

D(bug("[AiRcOS](numeric;userlist) Processing userlist [%d]'%s' ..\n", userlist_len, userlist));
         for (i = 0; i < (userlist_len + 1); i++)
         {
            if (((userlist[i] == ' ')||(i == userlist_len))&&( namestart < (userlist_len-1) ))
            {
               userlist[i] = '\0';
D(bug("[AiRcOS](numeric;userlist) Adding user  %d '%s' [start %d, end %d]\n",usercount, &userlist[namestart], namestart, i));

               if (userlist[namestart] == '@') aircos_Add_ChannelUser( thisChanPriv, "Admins", &userlist[namestart+1]);
               else if (userlist[namestart] == '+') aircos_Add_ChannelUser( thisChanPriv, "Voice", &userlist[namestart+1]);
               else aircos_Add_ChannelUser( thisChanPriv, "Normal", &userlist[namestart]);
               namestart = i+1;

               usercount++;
            }
         }
      }
      return 6;
    }

    case 311: /* RPL_WHOISUSER */
// ## UNHANDLED NUMERIC FUNC
//   inputs
//      nick = currentConnection->connection_serv_ARGS[3]
//      username = currentConnection->connection_serv_ARGS[4]
//      @domain = currentConnection->connection_serv_ARGS[5]
//
// format nick = username@domain
		return 6;

    case 324: /* RPL_CHANNELMODEIS */
// ## UNHANDLED NUMERIC FUNC
//   inputs
//      channel = currentConnection->connection_serv_ARGS[3]
//      mode = currentConnection->connection_serv_ARGS[4]
//
		break;

    case 329: /* ??? */
// ## UNHANDLED NUMERIC FUNC
//  Channel created info?
		return 0;

    case 333: /* ??? */
// ## UNHANDLED NUMERIC FUNC
//  Topic Set info?
		return 0;

    case 317: /* RPL_WHOISIDLE */
// ## UNHANDLED NUMERIC FUNC
		return 0;

    case 432: /* ERR_ERRONEUSNICKNAME */
    case 433: /* ERR_NICKNAMEINUSE */
// ## UNHANDLED NUMERIC FUNC
		return 0;

    default:
		break;
    }
    return 3;
}

/** CLIENT SIDE COMMANDS (SENDING) **/


static int aircos_CLIENT_nop(struct IRC_Channel_Priv  *sendOnThisChannel)
{
	struct serv_Outline *sa;

D(bug("[AiRcOS] ## CLIENT ## NOP('%s')\n",sendOnThisChannel->chan_send_ARGS[0]));

   if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( sendOnThisChannel->chan_send_ARGS[0] ) + 28, MEMF_CLEAR ) ))
	{
      char *tmpline = (char *)&sa[1];
		sprintf( tmpline, "*** # Unhandled command '%s'", sendOnThisChannel->chan_send_ARGS[0] );
		sa->so_name = (STRPTR)&sa[1];

      aircos_showChanOutput(sendOnThisChannel,sa);
	}

   return 1;
}

static int aircos_CLIENT_doSingleArg(struct IRC_Channel_Priv  *sendOnThisChannel)
{
D(bug("[AiRcOS] ## CLIENT ## DoSingleArg(Action:'%s', Arg:'",sendOnThisChannel->chan_send_ARGS[0]));
   int action_argcount = 1;
//   int i = 0;
   ULONG action_argsize = 0;

   while (sendOnThisChannel->chan_send_ARGS[action_argcount] != NULL)
   {
D(bug(" %s",sendOnThisChannel->chan_send_ARGS[action_argcount]));
       action_argcount++;
       *(sendOnThisChannel->chan_send_ARGS[action_argcount]-1) = ' ';
       action_argsize = strlen(sendOnThisChannel->chan_send_ARGS[action_argcount])+1;
   }
D(bug("')\n"));

   sprintf(sendOnThisChannel->chan_serv->serv_connection->connection_buff_send, "%s %s\n\r", sendOnThisChannel->chan_send_ARGS[0], sendOnThisChannel->chan_send_ARGS[1]);

   aircos_sendline(sendOnThisChannel->chan_serv->serv_connection);

   return 1;
}

static int aircos_CLIENT_doMe(struct IRC_Channel_Priv  *sendOnThisChannel)
{
D(bug("[AiRcOS] ## CLIENT ## DoMe('"));
   int action_argcount = 1;
//   int i = 0;
   ULONG action_argsize = 0;

while (sendOnThisChannel->chan_send_ARGS[action_argcount] != NULL)
   {
D(bug(" %s",sendOnThisChannel->chan_send_ARGS[action_argcount]));
      action_argcount++;
      *(sendOnThisChannel->chan_send_ARGS[action_argcount]-1) = ' ';
      action_argsize = strlen(sendOnThisChannel->chan_send_ARGS[action_argcount])+1;
   }
D(bug("')\n"));

   sprintf(sendOnThisChannel->chan_serv->serv_connection->connection_buff_send, "PRIVMSG %s :\1ACTION %s\1\n\r", sendOnThisChannel->chan_name, sendOnThisChannel->chan_send_ARGS[1]);

   aircos_sendline(sendOnThisChannel->chan_serv->serv_connection);

	struct serv_Outline *sa;

   if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( sendOnThisChannel->chan_serv->serv_connection->connection_nick ) + strlen( sendOnThisChannel->chan_send_ARGS[1] ) + 10, MEMF_CLEAR ) ))
	{
D(bug("[AiRcOS](client;dome) Displaying local Message\n"));
      char *tmpline = (char *)&sa[1];

		sprintf( tmpline, "\033P[-3]*** %s %s", sendOnThisChannel->chan_serv->serv_connection->connection_nick, sendOnThisChannel->chan_send_ARGS[1] );
	   sa->so_name = (STRPTR)&sa[1];

      aircos_showChanOutput(sendOnThisChannel,sa);
	}
   return 1;
}

struct irccommand commandList_array[]=
{
    {"ADMIN"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"AWAY"     , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"CLOSE"    , aircos_IRC_nop,         15 , aircos_CLIENT_doSingleArg, 15 },
    {"CONNECT"  , aircos_IRC_nop,         3  , aircos_CLIENT_nop,         15 },
    {"DESCRIBE" , aircos_IRC_nop,         2  , aircos_CLIENT_nop,         15 },
    {"DIE"      , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"DNS"      , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"ERROR"    , aircos_IRC_doerror,     15 , aircos_CLIENT_nop,         15 },
    {"HASH"     , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"HELP"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"INFO"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"INVITE"   , aircos_IRC_doinvite,    15 , aircos_CLIENT_nop,         15 },
    {"ISON"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"JOIN"     , aircos_IRC_dojoin,      15 , aircos_CLIENT_doSingleArg, 15 },
    {"KICK"     , aircos_IRC_dokick,      3  , aircos_CLIENT_doSingleArg, 15 },
    {"KILL"     , aircos_IRC_dokill,      2  , aircos_CLIENT_nop,         15 },
    {"LEAVE"    , aircos_IRC_nop,         15 , aircos_CLIENT_doSingleArg, 15 },
    {"LINKS"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"LIST"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"LUSERS"   , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"ME"       , aircos_IRC_nop,         1  , aircos_CLIENT_doMe,        15 },
    {"MSG"      , aircos_IRC_nop,         2  , aircos_CLIENT_nop,         15 },
    {"MOTD"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"MODE"     , aircos_IRC_domode,      15 , aircos_CLIENT_nop,         15 },
    {"NAMES"    , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"NICK"     , aircos_IRC_donick,      1  , aircos_CLIENT_doSingleArg, 15 },
    {"NOTE"     , aircos_IRC_nop,         2  , aircos_CLIENT_nop,         15 },
    {"NOTICE"   , aircos_IRC_donotice,    2  , aircos_CLIENT_nop,         15 },
    {"OPER"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"PART"     , aircos_IRC_dopart,      15 , aircos_CLIENT_nop,         15 },
    {"PASS"     , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"PING"     , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"PONG"     , aircos_IRC_dopong,      1  , aircos_CLIENT_nop,         15 },
    {"PRIVMSG"  , aircos_IRC_doprivmsg,   2  , aircos_CLIENT_nop,         15 },
    {"QUIT"     , aircos_IRC_doquit,      1  , aircos_CLIENT_doSingleArg, 15 },
    {"REHASH"   , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"RESTART"  , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"SERVER"   , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"SQUIT"    , aircos_IRC_nop,         2  , aircos_CLIENT_nop,         15 },
    {"STATS"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"SUMMON"   , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"TIME"     , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"TOPIC"    , aircos_IRC_dotopic,     2  , aircos_CLIENT_nop,         15 },
    {"TRACE"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"USER"     , aircos_IRC_nop,         4  , aircos_CLIENT_nop,         15 },
    {"USERHOST" , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"USERS"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"VERSION"  , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"WALLOPS"  , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"WHO"      , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"WHOIS"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"WHOWAS"   , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {"IGNORE"   , aircos_IRC_nop,         1  , aircos_CLIENT_nop,         15 },
    {"QUOTE"    , aircos_IRC_nop,         15 , aircos_CLIENT_nop,         15 },
    {NULL,NULL,0,NULL,0}
};

/* ************** USER/GROUP MANAGEMENT FUNCTIONS *************** */

struct IRC_Channel_Group_SubGroup *aircos_Find_Group
( struct IRC_Channel_Priv  * searchChannel, char * findThisGroup)
{
   struct IRC_Channel_Group_SubGroup *current_Group=NULL;
   ForeachNode(&searchChannel->chan_usergroup, current_Group)
   {
      if (strcasecmp(current_Group->group_name,findThisGroup)==0) return current_Group;
   }
   return NULL;
}


struct IRC_Channel_Group_SubGroup *aircos_Add_ChannelGroup
( struct IRC_Channel_Priv  * addToChannel, char * addThisGroup)
{
        struct IRC_Channel_Group_SubGroup *current_Group = NULL;

        if (!(addThisGroup)) addThisGroup = "<DEFAULT>";

        if (!(current_Group = aircos_Find_Group(addToChannel,addThisGroup)))
        {
           if (!(current_Group = AllocVec(sizeof(struct IRC_Channel_Group_SubGroup), MEMF_CLEAR)))
           {
D(bug("[AiRcOS](addgrp) Failed to allocate space for group record!!\n"));
           }
           else
           {
              current_Group->group_type = CHANNEL_GROUPT_SUBGROUP;
              if (!(current_Group->group_name = AllocVec(strlen(addThisGroup) + 1, MEMF_CLEAR)))
              {
D(bug("[AiRcOS](addgrp) Failed to allocate space for group name!!\n"));
                 FreeVec(current_Group);
                 return NULL;
              }
              if (!(current_Group->group_displayname = AllocVec(strlen(addThisGroup) + 3, MEMF_CLEAR)))
              {
D(bug("[AiRcOS](addgrp) Failed to allocate space for group label!!\n"));
                 FreeVec(current_Group->group_name);
                 FreeVec(current_Group);
                 return NULL;
              }
            
              strcpy( current_Group->group_name, addThisGroup );

              sprintf( current_Group->group_displayname, "\033b%s", current_Group->group_name );

              NewList((struct List *)&current_Group->group_usergroup);
			     if (strcmp("<DEFAULT>",current_Group->group_name)!=0)
			     {
                 current_Group->group_userobj = (struct MUI_NListtree_TreeNode *)DoMethod( addToChannel->chan_users, MUIM_NListtree_Insert, current_Group->group_displayname, (IPTR)current_Group, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Sorted, TNF_LIST | TNF_OPEN, TAG_DONE );
D(bug("[AiRcOS](addgrp) NList item for group '%s' @ %x\n",current_Group->group_name,current_Group->group_userobj));
			     }
			     else
              {
D(bug("[AiRcOS](addgrp) SKIPPING NList item for group '%s'\n",current_Group->group_name));
              }
              AddTail((struct List *)&addToChannel->chan_usergroup, (struct Node *)&current_Group->group_node);
           }
        }
        return current_Group;    
}

struct IRC_Channel_Group_User *aircos_Find_User
( struct IRC_Channel_Priv  * searchChannel, char * searchGroup, char * findThisUser)
{
   struct IRC_Channel_Group_User *current_User=NULL;
   struct IRC_Channel_Group_SubGroup *current_Group=NULL;

   if (!(searchGroup)) searchGroup = "<DEFAULT>";

   if ((current_Group = aircos_Find_Group(searchChannel,searchGroup)))
   {
      ForeachNode(&current_Group->group_usergroup, current_User)
      {
         if (strcasecmp(current_User->user_name,findThisUser)==0) return current_User;
      }
   }
   
   return NULL;
}

struct IRC_Channel_Group_User *aircos_Add_ChannelUser( struct IRC_Channel_Priv  * addToChannel, char * addToThisGroup, char * addThisUser)
{
   struct IRC_Channel_Group_SubGroup *current_Group = NULL;
   struct IRC_Channel_Group_User *current_User = NULL;
        
   if (!(addToThisGroup)) addToThisGroup = "<DEFAULT>";

D(bug("[AiRcOS](adduser) Looking for users channel record '%s'\n",addToThisGroup));
        
   if (!(current_Group = aircos_Find_Group(addToChannel,addToThisGroup)))
   {
      if(!(current_Group = aircos_Add_ChannelGroup(addToChannel,addToThisGroup)))
      {
D(bug("[AiRcOS](adduser) Failed to find or create a record for group '%s'\n",addToThisGroup));  
         return NULL;          
      }
   }
        
D(bug("[AiRcOS](adduser) Looking for exisiting user record for '%s' in group '%s'\n",addThisUser, current_Group->group_name));

   if (!(current_User = aircos_Find_User(addToChannel,addToThisGroup,addThisUser)))
   {
D(bug("[AiRcOS](adduser) no record for '%s' found\n",addThisUser));
      if ((current_User = AllocVec(sizeof(struct IRC_Channel_Group_User), MEMF_CLEAR)))
      { 
         current_User->group_type = CHANNEL_GROUPT_USER;
         if (!(current_User->user_name = AllocVec(strlen(addThisUser)+1,MEMF_CLEAR)))
         {
D(bug("[AiRcOS](adduser) Failed to allocate space to store user name!!\n"));
            FreeVec(current_User);
            return NULL;
         }
         else
         {
            if (!(current_User->user_displayname = AllocVec(strlen(addThisUser)+3,MEMF_CLEAR)))
            {
D(bug("[AiRcOS](adduser) Failed to allocate space to store user label!!\n"));
               FreeVec(current_User);
               return NULL;
            }
         
            strcpy( current_User->user_name, addThisUser );
            sprintf( current_User->user_displayname, "  %s" , current_User->user_name );

            if (!(current_Group->group_userobj))
            {
D(bug("[AiRcOS](addUser) Users group has no object..\n"));
               return NULL;
            }

     		   if (strcmp("<DEFAULT>", current_Group->group_name) == 0)
            {
               current_User->user_obj = (struct MUI_NListtree_TreeNode *)DoMethod( addToChannel->chan_users, MUIM_NListtree_Insert, current_User->user_displayname, current_User, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Sorted, 0x8000, TAG_DONE );
D(bug("[AiRcOS](adduser) Adding NList item for user '%s' to ROOT node\n",current_User->user_name));
            }
            else
            {
               current_User->user_obj = (struct MUI_NListtree_TreeNode *)DoMethod( addToChannel->chan_users, MUIM_NListtree_Insert, current_User->user_displayname, current_User, current_Group->group_userobj, MUIV_NListtree_Insert_PrevNode_Sorted, 0x8000, TAG_DONE );
D(bug("[AiRcOS](adduser) Adding NList item for user '%s' to group node\n",current_User->user_name));
            }

            AddTail((struct List *)&current_Group->group_usergroup, (struct Node *)&current_User->group_node);
         }
      }
      else
      {
D(bug("[AiRcOS](adduser) Failed to allocate user record!!\n"));
      }
   }
   else
   {
D(bug("[AiRcOS](adduser) User record already exists\n"));
   }
     
   return current_User;
}

/* ************** SERVER MANAGEMENT FUNCTIONS *************** */

struct IRC_Server_Priv  *aircos_add_server(char *addserv)
{
        struct IRC_Server_Priv  *new_ircServer=NULL;
        Object      *new_butt       = NULL;

        D(bug("[AiRcOS] aircos_add_server(%s)\n",addserv));
        
        if (!(new_ircServer = AllocVec(sizeof(struct IRC_Server_Priv)+strlen(addserv)+1, MEMF_CLEAR))) return NULL;
        char    *tmp_str = (char *)&new_ircServer[1];
        CopyMem(addserv,tmp_str,strlen(addserv));
        tmp_str[strlen(addserv)+1] = '\0';

        NewList((struct List *)&new_ircServer->serv_chans);

        new_ircServer->serv_name = tmp_str;

        D(bug("[AiRcOS](addserv) ## allocated private record for %s\n",new_ircServer->serv_name));

        // SERVER OUTPUT
        Object * servout_tmp = NListviewObject,
                                    MUIA_Listview_List, (IPTR) (new_ircServer->serv_status_output = NListObject,
                                         ReadListFrame,
                                         MUIA_NList_ListBackground, MUII_SHINE,
                                         MUIA_CycleChain, TRUE,
                                         MUIA_NList_MinLineHeight, 18,
                                    End),
                               End;

        if (!(servout_tmp)) goto newircs_err1;
        
        new_ircServer->serv_statuspage = VGroup,
                        Child, (IPTR) servout_tmp,
                    End;

        if (!(new_ircServer->serv_statuspage)) goto newircs_err2;

        new_ircServer->serv_pagemd_grp = VGroup,
                        MUIA_Group_PageMode, TRUE,
                        Child, (IPTR) new_ircServer->serv_statuspage,
                    End;

        if (!(new_ircServer->serv_pagemd_grp)) goto newircs_err3;

        Object          * tmp_servlog = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("Server Log"), MUIA_InputMode, MUIV_InputMode_RelVerify, End;
        
        new_ircServer->serv_page = VGroup,
            Child, (IPTR) (new_ircServer->serv_page_reg_grp = HGroup,
                Child, (IPTR) tmp_servlog,
                Child, (IPTR) (new_ircServer->serv_page_reg_spcr = RectangleObject, MUIA_Background, MUII_SHADOWFILL,
                End),
            End),
            Child, (IPTR) HGroup,
                Child, (IPTR) new_ircServer->serv_pagemd_grp,
            End,
        End;

        if (!(new_ircServer->serv_page)) goto newircs_err4;

        D(bug("[AiRcOS](addserv) server page created\n"));

        if (AiRcOS_Base->aircos_serv_no!=0)
        {
            D(bug("[AiRcOS](addserv) Adding Server Button to existing server group\n"));
            /* Add the new server to our existing group */
        
            new_butt = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel(new_ircServer->serv_name), MUIA_InputMode, MUIV_InputMode_RelVerify, End;

            if (DoMethod(AiRcOS_Base->aircos_window_pagemd_reg_grp, MUIM_Group_InitChange))
            {
                  DoMethod(AiRcOS_Base->aircos_window_pagemd_reg_grp, OM_REMMEMBER, AiRcOS_Base->aircos_window_pagemd_reg_spcr);
            
                  DoMethod(AiRcOS_Base->aircos_window_pagemd_reg_grp, OM_ADDMEMBER, new_butt);
                  DoMethod(AiRcOS_Base->aircos_window_pagemd_reg_grp, OM_ADDMEMBER, AiRcOS_Base->aircos_window_pagemd_reg_spcr);

                  DoMethod(AiRcOS_Base->aircos_window_pagemd_reg_grp, MUIM_Group_ExitChange);
            }
        
            D(bug("[AiRcOS](addserv) Adding page to existing server page group\n"));
            /* Add the new server to our existing group */
            if (DoMethod(AiRcOS_Base->aircos_window_pagemd_grp, MUIM_Group_InitChange))
            {
                  DoMethod(AiRcOS_Base->aircos_window_pagemd_grp, OM_ADDMEMBER, new_ircServer->serv_page);

                  DoMethod(AiRcOS_Base->aircos_window_pagemd_grp, MUIM_Group_ExitChange);
            }
          
            set(AiRcOS_Base->aircos_window_pagemd_grp,MUIA_Group_ActivePage,MUIV_Group_ActivePage_Last);
        }        
        else
        {
            D(bug("[AiRcOS](addserv) Creating new server page group\n"));
            /* Add The first Server and remove our spacer */
            Object      *old_root       = AiRcOS_Base->aircos_window_page;
          
            AiRcOS_Base->aircos_window_pagemd_grp = VGroup,
                        MUIA_Group_PageMode, TRUE,

                        Child, (IPTR) new_ircServer->serv_page,
                    End;

            AiRcOS_Base->aircos_window_page = VGroup,
                Child, (IPTR) (AiRcOS_Base->aircos_window_pagemd_reg_grp = HGroup,
                    Child, (IPTR)(new_butt = HGroup,
                        MUIA_Weight, 0,
                        MUIA_Background, PAGE_BUT_BACK,
                        Child, (IPTR) LLabel( new_ircServer->serv_name),
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                        End),
                    Child, (IPTR) (AiRcOS_Base->aircos_window_pagemd_reg_spcr = RectangleObject,
                            MUIA_Background, MUII_SHADOWFILL,
                    End),
                End),
                Child, (IPTR) AiRcOS_Base->aircos_window_pagemd_grp,
                End;
        
              if (DoMethod(AiRcOS_Base->aircos_window_content, MUIM_Group_InitChange))
              {
                    DoMethod(AiRcOS_Base->aircos_window_content, OM_ADDMEMBER, AiRcOS_Base->aircos_window_page);
                    DoMethod(AiRcOS_Base->aircos_window_content, OM_REMMEMBER, old_root);

                    DoMethod(AiRcOS_Base->aircos_window_content, MUIM_Group_ExitChange);
              }
        
        }
      
        new_ircServer->serv_pageid = AiRcOS_Base->aircos_serv_no++;
		DoMethod
        (
            new_butt, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			(IPTR) AiRcOS_Base->aircos_window_pagemd_grp, 3, MUIM_Set, MUIA_Group_ActivePage, new_ircServer->serv_pageid
        );
D(bug("[AiRcOS](addserv) Set NOTIFICATION for SERVER page '%s' [ID:%d]\n", new_ircServer->serv_name, new_ircServer->serv_pageid));
		DoMethod
        (
            tmp_servlog, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			(IPTR) new_ircServer->serv_pagemd_grp, 3, MUIM_Set, MUIA_Group_ActivePage, 0
        );
D(bug("[AiRcOS](addserv) Set NOTIFICATION for 'Server Log' button [ID:%d]\n", new_ircServer->serv_name, 0));
        AddTail((struct List *)&AiRcOS_Base->aircos_serverlist,(struct Node *) &new_ircServer->serv_node);

        return  new_ircServer;

newircs_err4:

      MUI_DisposeObject(new_ircServer->serv_pagemd_grp);
      goto newircs_err1;

newircs_err3:

      MUI_DisposeObject(new_ircServer->serv_statuspage);
      goto newircs_err1;

newircs_err2:

      MUI_DisposeObject(new_ircServer->serv_status_output);
      goto newircs_err1;

newircs_err1:

      FreeVec(new_ircServer);
	      return NULL;
}

/* returns our private structure for a named server, or null if not found */

struct IRC_Server_Priv  *aircos_find_server(char *find_server_name)
{
        struct IRC_Server_Priv  *cur_server=NULL;

D(bug("[AiRcOS] findServer('%s')\n", find_server_name));

        ForeachNode(&AiRcOS_Base->aircos_serverlist, cur_server)
        {
D(bug("[AiRcOS](findserver) checking '%s'\n", cur_server->serv_name));
            if (strcasecmp(cur_server->serv_name,find_server_name)==0) return cur_server;
        }
      
        return NULL;
}


/* destroy the zune data and private records for the named server */

BOOL  aircos_rem_server(char *remserv)
{
   struct IRC_Server_Priv  *rem_this_serv;
          
        if (!(rem_this_serv = aircos_find_server(remserv)))
        {
            return FALSE;
        }

        AiRcOS_Base->aircos_serv_no--;

        if (AiRcOS_Base->aircos_serv_no!=0)
        {
            /* remove the page from the server group */
            if (DoMethod(AiRcOS_Base->aircos_window_pagemd_grp, MUIM_Group_InitChange))
            {
                  DoMethod(AiRcOS_Base->aircos_window_pagemd_grp, OM_REMMEMBER, rem_this_serv);

                  DoMethod(AiRcOS_Base->aircos_window_pagemd_grp, MUIM_Group_ExitChange);
            }
        }        
        else
        {
            Object *old_root = AiRcOS_Base->aircos_window_pagemd_grp;
            AiRcOS_Base->aircos_window_pagemd_grp = VGroup,
                    InputListFrame,
                    MUIA_Background, MUII_SHADOWFILL,
                    Child, RectangleObject,
                    End,
                End;
            /* and remove the old object */
            if (DoMethod(AiRcOS_Base->aircos_window_content, MUIM_Group_InitChange))
            {
                  DoMethod(AiRcOS_Base->aircos_window_content, OM_ADDMEMBER, AiRcOS_Base->aircos_window_pagemd_grp);
                  DoMethod(AiRcOS_Base->aircos_window_content, OM_REMMEMBER, old_root);

                  DoMethod(AiRcOS_Base->aircos_window_content, MUIM_Group_ExitChange);
            }
        }
        FreeVec(rem_this_serv);
        return TRUE;
}


AROS_UFH3(void, parseoutput_func,
   AROS_UFHA(struct Hook *, unused_hook, A0),
   AROS_UFHA(APTR, obj, A2),
   AROS_UFHA(struct IRC_Channel_Priv  *, hook_channel_arg, A1 ))
{
   AROS_USERFUNC_INIT

   struct IRC_Channel_Priv  *sendOnThisChannel = unused_hook->h_Data;
   char                    *rawoutput = NULL;
   int pos, found = 0;

   get( obj, MUIA_String_Contents, &rawoutput );
D(bug("[AiRcOS](parseoutput_func) Parse output for channel '%s' ['%s']\n", sendOnThisChannel->chan_name, rawoutput));

   if (rawoutput == "") return;

   if (!(rawoutput[0] == '/'))
   {
      /* Handle plain message sending (quick send) */
D(bug("[AiRcOS](parseoutput_func) Sending plain message ..\n"));
   	sprintf(sendOnThisChannel->chan_serv->serv_connection->connection_buff_send, "PRIVMSG %s :%s\n\r", sendOnThisChannel->chan_name, rawoutput);

	  aircos_sendline(sendOnThisChannel->chan_serv->serv_connection);
	  
	  /* Display our sent message in our output window also.. */
	  struct serv_Outline *sa;

     if (( sa = (struct serv_Outline *)AllocVec( sizeof( struct serv_Outline) + strlen( sendOnThisChannel->chan_serv->serv_connection->connection_nick ) + strlen( rawoutput ) + 5, MEMF_CLEAR ) ))
	  {
D(bug("[AiRcOS](parseoutput_func) Displaying Message localy\n"));
         char *tmpline = (char *)&sa[1];

		   sprintf( tmpline, "<%s> %s", sendOnThisChannel->chan_serv->serv_connection->connection_nick, rawoutput );
		   sa->so_name = (STRPTR)&sa[1];

//		   sa->flags = (UWORD)msg->UserData;
		
         aircos_showChanOutput(sendOnThisChannel,sa);
	   }
   }
   else
   {
      /* Handle command sending */
D(bug("[AiRcOS](parseoutput_func) Handle command message ..\n"));   
      pos = 0;
      sendOnThisChannel->chan_send_ARGS[pos] = &rawoutput[1];

      /* Seperate the server string into seperate args .. */
      while (sendOnThisChannel->chan_send_ARGS[pos] != NULL && pos < MAX_SEND_ARGS)
	   {
         if ((AiRcOS_Base->Ai_tmp = strchr(sendOnThisChannel->chan_send_ARGS[pos], ' ')))
         {
       	   sendOnThisChannel->chan_send_ARGS[++pos] = &AiRcOS_Base->Ai_tmp[1];
            *AiRcOS_Base->Ai_tmp = '\0';
         }
			else sendOnThisChannel->chan_send_ARGS[++pos] = NULL;
	   }
D(bug("[AiRcOS](parseoutput_func) Parsed %d args\n",pos));

   sendOnThisChannel->chan_send_ARGS[++pos] = NULL;

#if defined(DEBUG)
D(bug("[AiRcOS](parseoutput_func) ARGS :"));
   int debug_args_count;
	for (debug_args_count = 0; debug_args_count <= pos ; debug_args_count++)
	{
D(bug(" %d:'%s'",debug_args_count,sendOnThisChannel->chan_send_ARGS[debug_args_count]));
	}
D(bug("\n"));
#endif

		for (pos = 0; commandList_array[pos].command!=NULL && !found; pos++) found = (strcasecmp(commandList_array[pos].command, sendOnThisChannel->chan_send_ARGS[0]) == 0);

		if (found)
		{
D(bug("[AiRcOS](parseoutput_func) Calling IRC funtion %d\n", pos-1));
		    pos = (*commandList_array[pos-1].command_clientFunction)(sendOnThisChannel);
		}
   }

   set( obj, MUIA_String_Contents, NULL);                               /* Clear the current buffer */
//	return( 0 );
	
	AROS_USERFUNC_EXIT
};

/* ************** CHANNEL MANAGEMENT FUNCTIONS *************** */

struct IRC_Channel_Priv  *aircos_add_channel(char *addtoserv,char *addchann)
{
        Object                  *img_user           = NULL,
                                *img_speek          = NULL,
                                *user_nodes         = NULL,
                                *gad_font_col       = NULL,
                                *gad_font_back      = NULL,
                                *gad_font_s_s       = NULL,
                                *gad_font_s_d       = NULL,
                                *gad_font_s_l       = NULL,
                                *gad_font_bold      = NULL,
                                *gad_font_ital      = NULL,
                                *gad_font_under     = NULL,
                                *gad_pop_url        = NULL,
                                *gad_Pop_smiley     = NULL,
                                *gad_close_irc_out   = NULL,
                                *gad_close_userlist  = NULL;

        struct IRC_Channel_Priv  *new_ircChannel=NULL;
        if (!(new_ircChannel = (struct IRC_Channel_Priv  *)AllocVec(sizeof(struct IRC_Channel_Priv)+strlen(addchann)+1, MEMF_CLEAR))) return NULL;

        char    *tmp_str = (char *)&new_ircChannel[1];
        CopyMem(addchann,tmp_str,strlen(addchann));
        tmp_str[strlen(addchann)+1] = '\0';

        if (!(new_ircChannel->chan_serv = aircos_find_server(addtoserv)))
        {
D(bug("[AiRcOS] Failed to locate server record for '%s'..\n",addtoserv));
                FreeVec(new_ircChannel);
                return NULL;
        }

        new_ircChannel->chan_name = tmp_str;
        NewList((struct List *)&new_ircChannel->chan_usergroup);    /* Prepare User List */

D(bug("[AiRcOS](addchannel) ## allocated private record for %s\n",new_ircChannel->chan_name));

        aircos_Add_ChannelGroup(new_ircChannel, NULL);    			/* Add the Base "Users" Group */

        /* Create the buttons */      
      
        new_ircChannel->chan_send = HGroup, ButtonFrame, MUIA_Weight, 0, MUIA_Background, MUII_ButtonBack, Child, (IPTR) LLabel("send"), MUIA_InputMode, MUIV_InputMode_RelVerify, End;
    
        gad_close_irc_out   = HGroup, MUIA_Weight, 0, Child, (IPTR) LLabel(" X "), MUIA_InputMode, MUIV_InputMode_RelVerify, End;
        gad_close_userlist  = HGroup, MUIA_Weight, 0, Child, (IPTR) LLabel(" X "), MUIA_InputMode, MUIV_InputMode_RelVerify, End;

        /* Create the image objects */

        if (AiRcOS_Base->aircos_got_speekimg)
        {
            img_speek = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"SPEEKTYPE_IMAGE,
                End;
        }
        else img_speek= RectangleObject,End;

        if (AiRcOS_Base->aircos_got_userimg)
        {
            img_user = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"USERTYPE_IMAGE,
                End;
        }
        else img_user = RectangleObject,End;

        /**/

        if (AiRcOS_Base->aircos_got_fontbaimg)
        {
            gad_font_back = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_BACK_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_font_back = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("F"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* background color */
        

        if (AiRcOS_Base->aircos_got_fontboimg)
        {
            gad_font_bold = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_BOLD_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_Toggle, 
                End;
        }
        else gad_font_bold = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("B"), MUIA_InputMode, MUIV_InputMode_Toggle, End; /* bold */
        

        if (AiRcOS_Base->aircos_got_fontcoimg)
        {
            gad_font_col = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_COLOR_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_font_col = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("F"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* Font color */
        

        if (AiRcOS_Base->aircos_got_fontdeimg)
        {
            gad_font_s_d = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_DEF_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify,
                End;
        }
        else gad_font_s_d = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("F"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* Font size def */
        

        if (AiRcOS_Base->aircos_got_fontitimg)
        {
            gad_font_ital = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_ITAL_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_Toggle, 
                End;
        }
        else gad_font_ital = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("I"), MUIA_InputMode, MUIV_InputMode_Toggle, End; /* italic */
        

        if (AiRcOS_Base->aircos_got_fontlaimg)
        {
            gad_font_s_l = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_LARGE_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_font_s_l = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("F"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* Font size larger */
        

        if (AiRcOS_Base->aircos_got_fontsmimg)
        {
            gad_font_s_s = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_SMALL_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_font_s_s = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("F"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* Font size smaller */
        

        if (AiRcOS_Base->aircos_got_fontunimg)
        {
            gad_font_under = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"FONT_UNDER_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_Toggle, 
                End;
        }
        else gad_font_under = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("U"), MUIA_InputMode, MUIV_InputMode_Toggle, End; /* underline */
        
/*  NOT YET USED */
/*        if (AiRcOS_Base->aircos_got_poppicimg)
        {
            gad_Pop_pic = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"POP_PIC_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_Pop_pic = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("P"), MUIA_InputMode, MUIV_InputMode_RelVerify, End;*/ /* picture! */

        if (AiRcOS_Base->aircos_got_popsmiimg)
        {
            gad_Pop_smiley = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"POP_SMILEY_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_Pop_smiley = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("S"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* smilley! */
        

        if (AiRcOS_Base->aircos_got_popurlimg)
        {
            gad_pop_url = ImageObject,
                    MUIA_Frame, MUIV_Frame_None,
                    MUIA_ShowSelState, FALSE,
                    MUIA_Image_Spec, (IPTR)"3:"POP_URL_IMAGE,
                    MUIA_InputMode, MUIV_InputMode_RelVerify, 
                End;
        }
        else gad_pop_url = HGroup, MUIA_Weight, 0, MUIA_Background, PAGE_BUT_BACK, Child, (IPTR) LLabel("A"), MUIA_InputMode, MUIV_InputMode_RelVerify, End; /* URL */
        
        /* Create the user list object */
    
        new_ircChannel->chan_users = MUI_NewObject( MUIC_NListtree,
                        ReadListFrame,
                        MUIA_NList_ListBackground, MUII_SHINE,
                        MUIA_CycleChain, TRUE,
                        MUIA_NList_MinLineHeight, 18,
                        MUIA_NListtree_ShowTree, FALSE,
                        MUIA_NListtree_NoRootTree, TRUE,
                        MUIA_NListtree_MultiSelect, MUIV_NListtree_MultiSelect_Shifted,
                        MUIA_NListtree_DoubleClick, MUIV_NListtree_DoubleClick_Tree,
                        MUIA_NListtree_EmptyNodes, FALSE,
                        MUIA_NListtree_TreeColumn, 0,
                        MUIA_NListtree_DragDropSort, TRUE,
                        MUIA_NListtree_Title, FALSE,
                        MUIA_NListtree_Format, ",",
        TAG_DONE);
    
         user_nodes = MUI_NewObject( MUIC_NListview,
                        MUIA_HorizWeight, 20,
                        MUIA_ShortHelp, (IPTR) "Users",
                        MUIA_NListview_NList, (IPTR) new_ircChannel->chan_users,
                TAG_DONE);

        /**/
        /* Create the channels page objects */
        /**/

		ULONG output_grp_txt_len = strlen(new_ircChannel->chan_serv->serv_name) + strlen(new_ircChannel->chan_name) + 10;

		new_ircChannel->chan_topic = AllocVec(strlen(new_ircChannel->chan_name) +1, MEMF_CLEAR);
		char *output_grp_label = AllocVec(output_grp_txt_len, MEMF_CLEAR);

		sprintf(output_grp_label,"From %s to %s", new_ircChannel->chan_serv->serv_name, new_ircChannel->chan_name);
		sprintf(new_ircChannel->chan_topic,"%s",new_ircChannel->chan_name);

        new_ircChannel->chan_page = VGroup,
                            Child, (IPTR) HGroup,
                                MUIA_Group_SameWidth, FALSE,
                                MUIA_Weight,0,
                                Child, (IPTR) LLabel("Topic:"),
                                Child, (IPTR) ( new_ircChannel->chan_topic_obj = TextObject,
                                    MUIA_Text_Contents, new_ircChannel->chan_topic,
                                End),
                                Child, HVSpace,
                            End,
                            Child, (IPTR) HGroup,
                                Child, (IPTR) ( new_ircChannel->chan_output_grp = VGroup,
                                    MUIA_HorizWeight, 80,
                                    Child, HGroup,
                                        MUIA_Group_SameWidth, FALSE,
                                        MUIA_Weight,0,
                                        MUIA_Background, MUII_SHINE,
                                        Child, (IPTR) img_speek,
                                        Child, (IPTR) ( new_ircChannel->chan_output_grp_txt = TextObject,
                                           MUIA_Text_Contents, output_grp_label,
                                        End),
                                        Child, HVSpace,
                                        Child, (IPTR) gad_close_irc_out,
                                    End,
        
                                    Child, ( new_ircChannel->chan_output = NListviewObject, /* IRC OUTPUT */
                                        MUIA_Listview_List, NListObject,
                                            MUIA_NList_Columns,3,
                                            MUIA_NList_ListBackground, MUII_SHINE,
                                            ReadListFrame,
                                        End,
                                    End),
        
                                    Child, (IPTR) BalanceObject,
                                    End,
        
                                    Child, (IPTR) HGroup,
                                        MUIA_Group_SameWidth, FALSE,
                                        MUIA_Weight,0,
        
                                        Child, (IPTR) gad_font_col, /* Font color */
                                        Child, (IPTR) gad_font_back, /* background color */
        
                                        Child, RectangleObject,
                                            MUIA_Weight,0,
                                            MUIA_Rectangle_VBar, TRUE,
                                        End,
        
                                        Child, (IPTR) gad_font_s_s, /* Font size smaller */
                                        Child, (IPTR) gad_font_s_d, /* Font size def */
                                        Child, (IPTR) gad_font_s_l, /* Font size larger */
        
                                        Child, RectangleObject,
                                            MUIA_Weight,0,
                                            MUIA_Rectangle_VBar, TRUE,
                                        End,
        
                                        Child, (IPTR) gad_font_bold, /* bold */
                                        Child, (IPTR) gad_font_ital, /* italic */
                                        Child, (IPTR) gad_font_under, /* underline */
        
                                        Child, RectangleObject,
                                            MUIA_Weight,0,
                                            MUIA_Rectangle_VBar, TRUE,
                                        End,
        
                                        Child, (IPTR) gad_pop_url, /* URL */
               
                                        Child, RectangleObject,
                                            MUIA_Weight,0,
                                            MUIA_Rectangle_VBar, TRUE,
                                        End,
        
                                        Child, (IPTR) gad_Pop_smiley, /* smilley! */
                                                                                   
                                        Child, RectangleObject,
                                            MUIA_Weight,100,
                                        End,
                                    End,
                                    Child, (IPTR) HGroup,
                                        MUIA_Group_SameWidth, FALSE,
                                        MUIA_Weight,0,
                                        Child, (IPTR) ( new_ircChannel->chan_message = BetterStringObject,
                                                ReadListFrame,
                                                MUIA_BetterString_StayActive, TRUE,
                                                MUIA_Background, MUII_SHINE,
                                                MUIA_HorizWeight,100,
                                        End),
                                        Child, (IPTR)  new_ircChannel->chan_send,
                                    End,
                                End),
                                Child, (IPTR) BalanceObject,
                                End,
                                Child, (IPTR) ( new_ircChannel->chan_users_grp = VGroup,
                                    MUIA_HorizWeight, 20,
                                    MUIA_Group_SameSize, FALSE,
                                    Child, HGroup,
                                        MUIA_Weight,0,
                                        MUIA_Background, MUII_FILL,
                                        Child, (IPTR) img_user,
                                        Child, (IPTR) LLabel("Users"),
                                        Child, HVSpace,
                                        Child, (IPTR) gad_close_userlist,
                                    End,
                                    Child, (IPTR) user_nodes,
                                End),
                            End,
                        End;

		new_ircChannel->chan_serv->serv_chan_no++;
        new_ircChannel->chan_pageid = new_ircChannel->chan_serv->serv_chan_no;
        AddTail((struct List *)&new_ircChannel->chan_serv->serv_chans,(struct Node *)&new_ircChannel->chan_node);

        Object      *new_butt       = NULL;

D(bug("[AiRcOS](addchan) Adding Channel Button to existing channel group\n"));
        /* Add the new server to our existing group */
    
        new_butt = HGroup,MUIA_Weight,0,MUIA_Background,PAGE_BUT_BACK, Child, (IPTR) LLabel(new_ircChannel->chan_name), MUIA_InputMode, MUIV_InputMode_RelVerify, End;

        if (DoMethod(new_ircChannel->chan_serv->serv_page_reg_grp, MUIM_Group_InitChange))
        {
        
              DoMethod(new_ircChannel->chan_serv->serv_page_reg_grp, OM_REMMEMBER, new_ircChannel->chan_serv->serv_page_reg_spcr);
        
              DoMethod(new_ircChannel->chan_serv->serv_page_reg_grp, OM_ADDMEMBER, new_butt);
              DoMethod(new_ircChannel->chan_serv->serv_page_reg_grp, OM_ADDMEMBER, new_ircChannel->chan_serv->serv_page_reg_spcr);

              DoMethod(new_ircChannel->chan_serv->serv_page_reg_grp, MUIM_Group_ExitChange);
        }
        
        if (DoMethod(new_ircChannel->chan_serv->serv_pagemd_grp, MUIM_Group_InitChange))
        {
                        DoMethod(new_ircChannel->chan_serv->serv_pagemd_grp, OM_ADDMEMBER, new_ircChannel->chan_page);

                        DoMethod(new_ircChannel->chan_serv->serv_pagemd_grp, MUIM_Group_ExitChange);
        }

        set(new_ircChannel->chan_serv->serv_pagemd_grp,MUIA_Group_ActivePage,MUIV_Group_ActivePage_Last);

D(bug("[AiRcOS](addchan) prepare send hook\n"));

        new_ircChannel->chan_hook.h_MinNode.mln_Succ = NULL;
        new_ircChannel->chan_hook.h_MinNode.mln_Pred = NULL;
        new_ircChannel->chan_hook.h_Entry = HookEntry;
        new_ircChannel->chan_hook.h_SubEntry = (void *)parseoutput_func;
        new_ircChannel->chan_hook.h_Data = new_ircChannel;

D(bug("[AiRcOS](addchan) Setting channel page notifications\n"));

	DoMethod
        (
            gad_close_irc_out, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            (IPTR) new_ircChannel->chan_output_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE
        );

	DoMethod
        (
            gad_close_userlist, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            (IPTR) new_ircChannel->chan_users_grp, 3, MUIM_Set, MUIA_ShowMe, FALSE
        );

	DoMethod
        (
            new_butt, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
			(IPTR) new_ircChannel->chan_serv->serv_pagemd_grp, 3, MUIM_Set, MUIA_Group_ActivePage, new_ircChannel->chan_pageid
        );

	DoMethod
	     (
	         new_ircChannel->chan_message, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
				(IPTR)new_ircChannel->chan_message, 3, MUIM_CallHook, &new_ircChannel->chan_hook, new_ircChannel
	     );

	DoMethod
	     (
	         new_ircChannel->chan_send, MUIM_Notify, MUIA_Pressed, FALSE,
				(IPTR)new_ircChannel->chan_message, 3, MUIM_CallHook, &new_ircChannel->chan_hook, new_ircChannel
	     );

   set( AiRcOS_Base->aircos_clientwin, MUIA_Window_DefaultObject, new_ircChannel->chan_message);
   set( AiRcOS_Base->aircos_clientwin, MUIA_Window_ActiveObject, new_ircChannel->chan_message);
   
D(bug("[AiRcOS](addchan) Set NOTIFICATION for channel page '%s' [ID:%d]\n", new_ircChannel->chan_name, new_ircChannel->chan_pageid));
        return new_ircChannel;
}

BOOL  aircos_rem_channel(char *remfromserv,char *remchann)
{
        return FALSE;
}

