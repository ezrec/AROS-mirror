/*
    Copyright © 2002, The AROS Development Team.
    All rights reserved.

    $Id$
*/

//#include    <exec/types.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

#include    <proto/alib.h>
#include    <proto/exec.h>
#include    <proto/dos.h>
#include    <proto/intuition.h>
#include    <proto/muimaster.h>
#include    <proto/utility.h>

#include    <dos/dos.h>
#include    <intuition/gadgetclass.h>
#include    <intuition/icclass.h>
#include    <clib/alib_protos.h>

#include <libraries/mui.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NList_mcc.h>

#include "aircos_global.h"
#include "locale.h"

extern struct AiRcOS_internal *AiRcOS_Base;
struct List                   aircos_Prefs_ServerNetworks;
struct List                   aircos_Prefs_Servers;
struct aircos_servernode      *aircos_Prefs_ServerActive;
struct Hook                   aircos_serversave_hook;
struct Hook                   aircos_networklistupdate_hook;
struct Hook                   aircos_serverlistupdate_hook;
struct Hook                   aircos_chooseserver_hook;

BOOL                          aircos_Prefs_ServersLoaded = FALSE;

Object	                     *input_server_address = NULL;
Object	                     *input_server_port =  NULL;
Object	                     *input_server_description =  NULL;
Object	                     *input_server_network =  NULL;
Object	                     *input_server_pass =  NULL;

Object                        *select_dropboxgrp_network = NULL;
Object                        *select_dropbox_network = NULL;
Object                        *select_dropboxgrp_server = NULL;
Object                        *select_dropbox_server = NULL;

STRPTR	                     network_list_empty[2] =
{
  {"<List Empty>"},
  NULL
};
STRPTR	                     server_list_empty[2] =
{
  {"<List Empty>"},
  NULL
};
Object	                     *servermodWin;
STRPTR	                     *network_list;
STRPTR	                     *server_list;

//network_list_empty[0] = "<List Empty>";		//MSG(MSG_LIST_EMPTY);
//network_list_empty[1] = NULL;

//server_list_empty[0] = "<List Empty>";		//MSG(MSG_LIST_EMPTY);
//server_list_empty[1] = NULL;

#define AIRCOS_DEF_SERVERSFILE "servers.dat"

aircosApp_LoadServers()
{
  aircos_Prefs_ServersLoaded = TRUE;
}

struct aircos_networknode *aircosApp_FindServerNetworkNode(char * findNetwork)
{
  struct aircos_networknode *current_Node = NULL;
  ForeachNode(&aircos_Prefs_ServerNetworks, current_Node)
  {
D(bug("[AiRcOS](FindServerNetworkNode) Checking against record for '%s'\n", current_Node->ann_Network));
      if (strcasecmp(current_Node->ann_Network, findNetwork)==0) return current_Node;
  }
  return NULL;
}

struct aircos_servernode  *aircosApp_FindServerNode(char * findServer)
{
struct aircos_servernode  *current_Node = NULL;
  ForeachNode(&aircos_Prefs_Servers, current_Node)
  {
D(bug("[AiRcOS](FindServerNode) Checking against record for '%s'\n", current_Node->asn_Server));
      if (strcasecmp(current_Node->asn_Server, findServer)==0) return current_Node;
  }
  return NULL;
}

AROS_UFH3(void, chooseserver_func,
   AROS_UFHA(struct Hook *, unused_hook, A0),
   AROS_UFHA(APTR, obj, A2),
   AROS_UFHA(struct IRC_Channel_Priv  *, hook_channel_arg, A1 ))
{
   AROS_USERFUNC_INIT
D(bug("[AiRcOS] chooseserver_func()\n"));
  ULONG                      currentPrefsServerID;
  get(select_dropbox_server, MUIA_Cycle_Active, &currentPrefsServerID);

  struct aircos_servernode *currentPrefsServer = NULL;
  if (!(currentPrefsServer = aircosApp_FindServerNode(server_list[currentPrefsServerID])))
  {
D(bug("[AiRcOS] chooseserver_func: Couldnt find Server Node!\n"));
    return;
  }

  aircos_Prefs_ServerActive = currentPrefsServer;

  AROS_USERFUNC_EXIT
};

AROS_UFH3(void, updatenetworklist_func,
   AROS_UFHA(struct Hook *, unused_hook, A0),
   AROS_UFHA(APTR, obj, A2),
   AROS_UFHA(struct IRC_Channel_Priv  *, hook_channel_arg, A1 ))
{
   AROS_USERFUNC_INIT
D(bug("[AiRcOS] updatenetworklist_func()\n"));
  struct aircos_networknode *currentPrefsNetwork = NULL;
  ULONG                      prefsNetworkCount = 0;
  ULONG                      setprefsNetworkActive = 0;
  Object                     *new_dropbox_network = NULL;
  
  ForeachNode(&aircos_Prefs_ServerNetworks, currentPrefsNetwork)
  {
      prefsNetworkCount++;
      if (aircos_Prefs_ServerActive)
        if (strcasecmp(currentPrefsNetwork->ann_Network, aircos_Prefs_ServerActive->asn_Network->ann_Network)==0)
        {
          setprefsNetworkActive = prefsNetworkCount -1;
        }
  }

D(bug("[AiRcOS] updatenetworklist_func: %d network nodes\n", prefsNetworkCount));
  
  if (prefsNetworkCount > 0)
  {
    if (network_list != network_list_empty) {
      FreeVec(network_list);
    }
    
    if ((network_list = AllocVec(sizeof(IPTR) * prefsNetworkCount+1, MEMF_CLEAR|MEMF_PUBLIC)))
    {
      int loop_count = 0;

      ForeachNode(&aircos_Prefs_ServerNetworks, currentPrefsNetwork)
      {
        network_list[loop_count] = currentPrefsNetwork->ann_Network;
        loop_count++;
      }
    }
    else
    {
D(bug("[AiRcOS] updatenetworklist_func: ERROR - couldnt allocate memory for network name pointer table\n"));
      network_list = network_list_empty;
    }
  } else if (network_list != network_list_empty) {
    FreeVec(network_list);
    network_list = network_list_empty; 
  }

  if (!(new_dropbox_network = MUI_MakeObject(MUIO_Cycle, NULL, network_list)))
  {
D(bug("[AiRcOS] updatenetworklist_func: Failed to create Network dropdown\n"));
    return NULL;
  }

  if (DoMethod(select_dropboxgrp_network, MUIM_Group_InitChange))
  {
    DoMethod(select_dropboxgrp_network, OM_REMMEMBER, select_dropbox_network);
    DoMethod(select_dropboxgrp_network, OM_ADDMEMBER, new_dropbox_network);
    DoMethod(select_dropboxgrp_network, MUIM_Group_ExitChange);
    select_dropbox_network = new_dropbox_network;
  }

  DoMethod(select_dropbox_network, MUIM_NoNotifySet, MUIA_Cycle_Active, setprefsNetworkActive);
  DoMethod
  (
      select_dropbox_network, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
      (IPTR) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, &aircos_serverlistupdate_hook, NULL
  );
        
  AROS_USERFUNC_EXIT
};

AROS_UFH3(void, updateserverlist_func,
   AROS_UFHA(struct Hook *, unused_hook, A0),
   AROS_UFHA(APTR, obj, A2),
   AROS_UFHA(struct IRC_Channel_Priv  *, hook_channel_arg, A1 ))
{
   AROS_USERFUNC_INIT
D(bug("[AiRcOS] updateserverlist_func()\n"));
  struct aircos_servernode   *currentPrefsServer = NULL;
  ULONG                      prefsNetworkServerCount = 0;
  ULONG                      setprefsServerActive = 0;
  Object                     *new_dropbox_server = NULL;

  ULONG                      currentPrefsNetworkID;
  get(select_dropbox_network, MUIA_Cycle_Active, &currentPrefsNetworkID);

  struct aircos_networknode *currentPrefsNetwork = NULL;
  if (!(currentPrefsNetwork = aircosApp_FindServerNetworkNode(network_list[currentPrefsNetworkID])))
  {
D(bug("[AiRcOS] updateserverlist_func: Couldnt find Network Node!\n"));
    return;
  }

  ForeachNode(&aircos_Prefs_Servers, currentPrefsServer)
  {
      if (strcasecmp(currentPrefsServer->asn_Network->ann_Network, currentPrefsNetwork->ann_Network)==0)
      {
        prefsNetworkServerCount++;
        if (aircos_Prefs_ServerActive)
          if (strcasecmp(currentPrefsServer->asn_Server, aircos_Prefs_ServerActive->asn_Server)==0)
          {
            setprefsServerActive = prefsNetworkServerCount -1;
          }
      }
  }

D(bug("[AiRcOS] updateserverlist_func: %d server nodes for network '%s'\n", prefsNetworkServerCount, currentPrefsNetwork->ann_Network));
  
  if (prefsNetworkServerCount > 0)
  {
    if (server_list != server_list_empty) {
      FreeVec(server_list);
    }
    
    if ((server_list = AllocVec(sizeof(IPTR) * prefsNetworkServerCount+1, MEMF_CLEAR|MEMF_PUBLIC)))
    {
      int loop_count = 0;

      ForeachNode(&aircos_Prefs_Servers, currentPrefsServer)
      {
        if (strcasecmp(currentPrefsServer->asn_Network->ann_Network, currentPrefsNetwork->ann_Network)==0)
        {
          server_list[loop_count] = currentPrefsServer->asn_Server;
          loop_count++;
        }
      }
    }
    else
    {
D(bug("[AiRcOS] updateserverlist_func: ERROR - couldnt allocate memory for server name pointer table\n"));
      server_list = server_list_empty;
    }
  } else if (server_list != server_list_empty) {
    FreeVec(server_list);
    server_list = server_list_empty; 
  }

  if (!(new_dropbox_server = MUI_MakeObject(MUIO_Cycle, NULL, server_list)))
  {
D(bug("[AiRcOS] updateserverlist_func: Failed to create Server dropdown\n"));
    return NULL;
  }

  if (DoMethod(select_dropboxgrp_server, MUIM_Group_InitChange))
  {
    DoMethod(select_dropboxgrp_server, OM_REMMEMBER, select_dropbox_server);
    DoMethod(select_dropboxgrp_server, OM_ADDMEMBER, new_dropbox_server);
    DoMethod(select_dropboxgrp_server, MUIM_Group_ExitChange);
    select_dropbox_server = new_dropbox_server;
  }

  DoMethod(select_dropbox_server, MUIM_NoNotifySet, MUIA_Cycle_Active, setprefsServerActive);

  AROS_USERFUNC_EXIT
};

AROS_UFH3(void, serversave_func,
   AROS_UFHA(struct Hook *, unused_hook, A0),
   AROS_UFHA(APTR, obj, A2),
   AROS_UFHA(struct IRC_Channel_Priv  *, hook_channel_arg, A1 ))
{
   AROS_USERFUNC_INIT
D(bug("[AiRcOS] serversave_func()\n"));

  struct aircos_networknode *newsaveNetwork = NULL;
  char                      *newsaveNetwork_name;
  BOOL                      newsaveNetwork_update = FALSE;

  struct aircos_servernode  *newsaveServer = NULL;
  char                      *newsaveServer_name;
  BOOL                      newsaveServer_update = FALSE;

   get( input_server_network, MUIA_String_Contents, &newsaveNetwork_name);
   get( input_server_address, MUIA_String_Contents, &newsaveServer_name);

  if (!(newsaveNetwork = aircosApp_FindServerNetworkNode(newsaveNetwork_name)))
  {
D(bug("[AiRcOS](serversave_func) created new network node for '%s'\n", newsaveNetwork_name));
     newsaveNetwork = AllocVec(sizeof(struct aircos_networknode), MEMF_CLEAR|MEMF_PUBLIC);
     newsaveNetwork->ann_Network = AllocVec(strlen(newsaveNetwork_name)+1, MEMF_CLEAR|MEMF_PUBLIC);
     CopyMem(newsaveNetwork_name, newsaveNetwork->ann_Network, strlen(newsaveNetwork_name)+1);

     AddTail((struct List *)&aircos_Prefs_ServerNetworks, (struct Node *)&newsaveNetwork->ann_Node);
     newsaveNetwork_update = TRUE;
  }
  newsaveNetwork->ann_ServerCount += 1;
D(bug("[AiRcOS](serversave_func) %s Network node server count = %d\n", newsaveNetwork->ann_Network, newsaveNetwork->ann_ServerCount));

  if (!(newsaveServer = aircosApp_FindServerNode(newsaveServer_name)))
  {
D(bug("[AiRcOS](serversave_func) created new server node for '%s'\n", newsaveServer_name));
     newsaveServer = AllocVec(sizeof(struct aircos_servernode), MEMF_CLEAR|MEMF_PUBLIC);
     newsaveServer->asn_Server = AllocVec(strlen(newsaveServer_name)+1, MEMF_CLEAR|MEMF_PUBLIC);
     CopyMem(newsaveServer_name, newsaveServer->asn_Server, strlen(newsaveServer_name)+1);
     newsaveServer->asn_Network = newsaveNetwork;
     get( input_server_port, MUIA_String_Integer, &newsaveServer->asn_Port);

#warning "TODO: we need to store the password also here .."

     AddTail((struct List *)&aircos_Prefs_Servers, (struct Node *)&newsaveServer->asn_Node);
     newsaveServer_update = TRUE;
  }
  else
  {
D(bug("[AiRcOS](serversave_func) node already exists for server '%s'\n", newsaveServer_name));
  }

  aircos_Prefs_ServerActive = newsaveServer;

  set( servermodWin, MUIA_Window_Open, FALSE);

  if (newsaveNetwork_update) CallHookPkt(&aircos_networklistupdate_hook, obj, hook_channel_arg);
  if (newsaveServer_update) CallHookPkt(&aircos_serverlistupdate_hook, obj, hook_channel_arg);

  AROS_USERFUNC_EXIT
};


Object	*aircos_showServerConnect()
{
D(bug("[AiRcOS] showServerConnect()\n"));
  if (aircos_Prefs_ServersLoaded)
  {
D(bug("[AiRcOS](showServerConnect) Server windows already configured!\n"));
    return NULL;
  }

   Object   *tmp_connectWin = NULL;

  NewList((struct List *)&aircos_Prefs_ServerNetworks);
  NewList((struct List *)&aircos_Prefs_Servers);

	Object	*butt_addServer = NULL;
	Object	*butt_editServer = NULL;
	Object	*butt_delServer = NULL;
	Object	*butt_serverConnect = NULL;
	Object   *butt_serverSave = NULL;

   network_list = network_list_empty;
   server_list = server_list_empty;

	if (!(select_dropbox_network = MUI_MakeObject(MUIO_Cycle, NULL, network_list)))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create Network dropdown\n"));
      return NULL;
	}
	
	if (!(select_dropbox_server = MUI_MakeObject(MUIO_Cycle, NULL, server_list)))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create Server dropdown\n"));
      return NULL;
	}

   if (!(butt_addServer = SimpleButton("Add")))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create 'ADD' button\n"));
      return NULL;
	}
	
   if (!(butt_editServer = SimpleButton("Edit")))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create 'EDIT' button\n"));
      return NULL;
	}
	
   if (!(butt_delServer = SimpleButton("Del")))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create 'DEL' button\n"));
      return NULL;
	}

   if (!(butt_serverConnect = SimpleButton("Connect!")))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create 'CONNECT' button\n"));
      return NULL;
	}

   if (!(butt_serverSave = SimpleButton("Save!")))
	{
D(bug("[AiRcOS](showServerConnect) Failed to create 'SAVE' button\n"));
      return NULL;
	}

   select_dropboxgrp_network = VGroup,
                                 Child, (IPTR) select_dropbox_network,
                               End;

   select_dropboxgrp_server = VGroup,
                                Child, (IPTR) select_dropbox_server,
                              End;

	tmp_connectWin = WindowObject,
            MUIA_Window_Title, (IPTR) "Connect to Server..",
            MUIA_Window_Activate, TRUE,
            MUIA_Window_Width,200,
            MUIA_Window_Height,200,
     
            WindowContents, (IPTR) VGroup,
                Child, (IPTR) HGroup,
                  GroupFrame,
					   Child, (IPTR) VGroup,
						  Child, (IPTR) LLabel("IRC Network"),
						  Child, (IPTR) HGroup,
						    Child, (IPTR) HSpace(0),
						    Child, (IPTR) select_dropboxgrp_network,
						    Child, (IPTR) HSpace(0),
						  End,
					   End,
					   Child, (IPTR) HVSpace,
                End,
                Child, (IPTR) HGroup,
                  GroupFrame,
					   Child, (IPTR) VGroup,
						  Child, (IPTR) LLabel("IRC Server"),
						  Child, (IPTR) HGroup,
						    Child, (IPTR) HVSpace,
						    Child, (IPTR) select_dropboxgrp_server,
						    Child, (IPTR) HVSpace,
						  End,
					   End,
  					   Child, (IPTR) HVSpace,
                End,
                Child, (IPTR) HGroup,
					   Child, (IPTR) HVSpace,
					   Child, (IPTR) butt_addServer,
					   Child, (IPTR) butt_editServer,
					   Child, (IPTR) butt_delServer,
                End,
                Child, (IPTR) butt_serverConnect,
            End,
        End;

	servermodWin = WindowObject,
        
            MUIA_Window_Title, (IPTR) "Edit Server..",
            MUIA_Window_Activate, TRUE,
            MUIA_Window_Width,350,
            MUIA_Window_Height,400,
     
            WindowContents, (IPTR) VGroup,
               Child, (IPTR) VGroup,
                 GroupFrame,
                 Child, (IPTR) HGroup,
					    Child, (IPTR) LLabel("IRC Server Address"),
					    Child, (IPTR) (input_server_address = StringObject,
										StringFrame,
                              MUIA_CycleChain, TRUE,
										MUIA_String_Format, MUIV_String_Format_Right,
                   End ),
                 End,
                 Child, (IPTR) HGroup,
					    Child, (IPTR) LLabel("Port"),
					    Child, (IPTR) (input_server_port = StringObject,
										StringFrame,
                              MUIA_CycleChain, TRUE,
										MUIA_String_Format, MUIV_String_Format_Right,
										MUIA_String_Accept, "0123456789",
										MUIA_String_Integer, 0,
					    End),
                 End,
                 Child, (IPTR) HGroup,
					    Child, (IPTR) LLabel("Server Description"),
					    Child, (IPTR) (input_server_description = StringObject,
										StringFrame,
                              MUIA_CycleChain, TRUE,
										MUIA_String_Format, MUIV_String_Format_Right,
					    End),
                 End,
                 Child, (IPTR) HGroup,
					    Child, (IPTR) LLabel("IRC Network"),
					    Child, (IPTR) (input_server_network = StringObject,
										StringFrame,
                              MUIA_CycleChain, TRUE,
										MUIA_String_Format, MUIV_String_Format_Right,
					    End),
                 End,
               End,
               Child, (IPTR) VGroup,
                 GroupFrame,
				     Child, (IPTR) LLabel("Enter server password here if applicable."),
                 Child, (IPTR) HGroup,
					    Child, (IPTR) LLabel("Password"),
					    Child, (IPTR) (input_server_pass = StringObject,
										StringFrame,
                              MUIA_CycleChain, TRUE,
										MUIA_String_Format, MUIV_String_Format_Right,
					    End),
                 End,
               End,
               Child, (IPTR) butt_serverSave,
            End,
        End;

	if ((tmp_connectWin)&&(servermodWin))
	{
D(bug("[AiRcOS](showServerConnect) Created GUI objects\n"));
      DoMethod
        (
            AiRcOS_Base->aircos_app, OM_ADDMEMBER,
            (IPTR) tmp_connectWin
        );

      DoMethod
        (
            AiRcOS_Base->aircos_app, OM_ADDMEMBER,
            (IPTR) servermodWin
        );

        DoMethod
        (
            tmp_connectWin, MUIM_Notify, MUIA_Window_Open, TRUE,
            (IPTR) AiRcOS_Base->aircos_quickconnectwin, 3, MUIM_Set, MUIA_Window_Open, FALSE
        );

        DoMethod
        (
            AiRcOS_Base->aircos_quickconnectwin, MUIM_Notify, MUIA_Window_Open, TRUE,
            (IPTR) tmp_connectWin, 3, MUIM_Set, MUIA_Window_Open, FALSE
        );

        DoMethod
        (
            tmp_connectWin, MUIM_Notify, MUIA_Window_Open, FALSE,
            (IPTR) servermodWin, 3, MUIM_Set, MUIA_Window_Open, FALSE
        );

      DoMethod
        (
            butt_addServer, MUIM_Notify, MUIA_Selected, FALSE,
            (IPTR) servermodWin, 3, MUIM_Set, MUIA_Window_Open, TRUE
        );

      DoMethod
        (
            butt_editServer, MUIM_Notify, MUIA_Selected, FALSE,
            (IPTR) servermodWin, 3, MUIM_Set, MUIA_Window_Open, TRUE
        );

//      DoMethod
//        (
//            butt_delServer, MUIM_Notify, MUIA_Selected, FALSE,
//            (IPTR) servermodWin, 3, MUIM_Set, MUIA_ShowMe, FALSE
//        );

        DoMethod
        (
            tmp_connectWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (IPTR) tmp_connectWin, 3, MUIM_Set, MUIA_Window_Open, FALSE
        );
        
        DoMethod
        (
            servermodWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (IPTR) servermodWin, 3, MUIM_Set, MUIA_Window_Open, FALSE
        );

        DoMethod
        (
            butt_serverConnect, MUIM_Notify, MUIA_Pressed, FALSE,
                (IPTR) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, &AiRcOS_Base->aircos_connect_hook, NULL
        );

D(bug("[AiRcOS](showServerConnect) prepare list update hooks\n"));
    
       aircos_serversave_hook.h_MinNode.mln_Succ = NULL;
       aircos_serversave_hook.h_MinNode.mln_Pred = NULL;
       aircos_serversave_hook.h_Entry = HookEntry;
       aircos_serversave_hook.h_SubEntry = (void *)serversave_func;

       aircos_networklistupdate_hook.h_MinNode.mln_Succ = NULL;
       aircos_networklistupdate_hook.h_MinNode.mln_Pred = NULL;
       aircos_networklistupdate_hook.h_Entry = HookEntry;
       aircos_networklistupdate_hook.h_SubEntry = (void *)updatenetworklist_func;
       
       aircos_serverlistupdate_hook.h_MinNode.mln_Succ = NULL;
       aircos_serverlistupdate_hook.h_MinNode.mln_Pred = NULL;
       aircos_serverlistupdate_hook.h_Entry = HookEntry;
       aircos_serverlistupdate_hook.h_SubEntry = (void *)updateserverlist_func;

       aircos_chooseserver_hook.h_MinNode.mln_Succ = NULL;
       aircos_chooseserver_hook.h_MinNode.mln_Pred = NULL;
       aircos_chooseserver_hook.h_Entry = HookEntry;
       aircos_chooseserver_hook.h_SubEntry = (void *)chooseserver_func;


        DoMethod
        (
            butt_serverSave, MUIM_Notify, MUIA_Pressed, FALSE,
                (IPTR) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, &aircos_serversave_hook, NULL
        );


        DoMethod
        (
            select_dropbox_network, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                (IPTR) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, &aircos_serverlistupdate_hook, NULL
        );

        DoMethod
        (
            select_dropbox_server, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                (IPTR) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, &aircos_chooseserver_hook, NULL
        );

D(bug("[AiRcOS](showServerConnect) Configured NOTIFICATIONS\n"));

       set(tmp_connectWin, MUIA_Window_Open, TRUE);
       D(bug("[AiRcOS](showServerConnect) Window opened ..\n"));
     }
     else
     {
      if (!(servermodWin))
      {
D(bug("[AiRcOS](showServerConnect) Failed to create server edit window..\n"));
      }
      else MUI_DisposeObject(servermodWin);
      
      if (!(tmp_connectWin))
      {
D(bug("[AiRcOS](showServerConnect) Failed to create server selection window..\n"));      
      }
      else MUI_DisposeObject(tmp_connectWin);
     }

	return tmp_connectWin;
}

