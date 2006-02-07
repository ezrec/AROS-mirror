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

#include    <dos/dos.h>
#include    <intuition/gadgetclass.h>
#include    <intuition/icclass.h>
#include    <clib/alib_protos.h>

#include <libraries/mui.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NList_mcc.h>

#define DEBUG 0
#include <aros/debug.h>

STRPTR	network_list[2];
STRPTR	server_list[2];
Object	*serverconnectWin;
Object	*servermodWin;

Object	*aircos_showServerConnect()
{
D(bug("[AiRcOS] showServerConnect()\n"));
    network_list[0] = "<List Empty>";		//MSG(MSG_LIST_EMPTY);
	network_list[1] = NULL;

    server_list[0] = "<List Empty>";		//MSG(MSG_LIST_EMPTY);
    server_list[1] = NULL;

	Object	*select_dropbox_network = NULL;
	Object	*select_dropbox_server = NULL;

	Object	*butt_addServer = NULL;
	Object	*butt_editServer = NULL;
	Object	*butt_delServer = NULL;
	Object	*butt_serverConnect = NULL;

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

	serverconnectWin = WindowObject,
            MUIA_Window_Title, (IPTR) "Connect to Server..",
            MUIA_Window_Activate, TRUE,
            MUIA_Window_Width,600,
            MUIA_Window_Height,400,
     
            WindowContents, (IPTR) VGroup,
                Child, (IPTR) HGroup,
					   Child, (IPTR) VGroup,
						  Child, (IPTR) LLabel("IRC Network"),
						  Child, (IPTR) select_dropbox_network,
					   End,
					   Child, (IPTR) HVSpace,
                End,
                Child, (IPTR) HGroup,
					   Child, (IPTR) VGroup,
						  Child, (IPTR) LLabel("IRC Server"),
						  Child, (IPTR) select_dropbox_server,
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

	Object	*input_server_address = NULL;
	Object	*input_server_port =  NULL;
	Object	*input_server_description =  NULL;
	Object	*input_server_network =  NULL;
	Object	*input_server_pass =  NULL;

	servermodWin = WindowObject,
        
            MUIA_Window_Title, (IPTR) "Edit Server..",
            MUIA_Window_Activate, TRUE,
            MUIA_Window_Width,600,
            MUIA_Window_Height,400,
     
            WindowContents, (IPTR) VGroup,
                Child, (IPTR) HGroup,
					Child, (IPTR) LLabel("IRC Server Address"),
					Child, (IPTR) (input_server_address = StringObject,
										StringFrame,
										MUIA_String_Format, MUIV_String_Format_Right,
									End ),
                End,
                Child, (IPTR) HGroup,
					Child, (IPTR) LLabel("Port"),
					Child, (IPTR) (input_server_port = StringObject,
										StringFrame,
										MUIA_String_Format, MUIV_String_Format_Right,
										MUIA_String_Accept, "0123456789",
										MUIA_String_Integer, 0,
									End),
                End,
                Child, (IPTR) HGroup,
					Child, (IPTR) LLabel("Server Description"),
					Child, (IPTR) (input_server_description = StringObject,
										StringFrame,
										MUIA_String_Format, MUIV_String_Format_Right,
									End),
                End,
                Child, (IPTR) HGroup,
					Child, (IPTR) LLabel("IRC Network"),
					Child, (IPTR) (input_server_network = StringObject,
										StringFrame,
										MUIA_String_Format, MUIV_String_Format_Right,
									End),
                End,
				Child, (IPTR) LLabel("Enter server password here"),
				Child, (IPTR) LLabel("if applicable."),
                Child, (IPTR) HGroup,
					Child, (IPTR) LLabel("Password"),
					Child, (IPTR) (input_server_pass = StringObject,
										StringFrame,
										MUIA_String_Format, MUIV_String_Format_Right,
									End),
                End,
            End,
        End;

	if ((serverconnectWin)&&(servermodWin))
	{
D(bug("[AiRcOS](showServerConnect) Created GUI objects\n"));

      DoMethod
        (
            butt_addServer, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            (IPTR) servermodWin, 3, MUIM_Set, MUIA_Window_Open,TRUE
        );

      DoMethod
        (
            butt_editServer, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            (IPTR) servermodWin, 3, MUIM_Set, MUIA_Window_Open,TRUE
        );

//      DoMethod
//        (
//            butt_delServer, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
//            (IPTR) servermodWin, 3, MUIM_Set, MUIA_ShowMe, FALSE
//        );

D(bug("[AiRcOS](showServerConnect) Configured NOTIFICATIONS\n"));

      set(serverconnectWin, MUIA_Window_Open, TRUE);
      D(bug("[AiRcOS](showServerConnect) Window opened ..\n"));
     }
     else
     {
      if (!(servermodWin))
      {
D(bug("[AiRcOS](showServerConnect) Failed to create server edit window..\n"));
      }
      else MUI_DisposeObject(servermodWin);
      
      if (!(serverconnectWin))
      {
D(bug("[AiRcOS](showServerConnect) Failed to create server selection window..\n"));      
      }
      else MUI_DisposeObject(serverconnectWin);
     }

	return serverconnectWin;
}

