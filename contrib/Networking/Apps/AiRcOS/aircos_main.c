/*
    Copyright © 2002, The AROS Development Team.
    All rights reserved.

    $Id$
*/

#include    <exec/types.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>

#include    <proto/alib.h>
#include    <proto/exec.h>
#include    <proto/dos.h>
#include    <proto/intuition.h>
#include    <proto/muimaster.h>
#include    <proto/graphics.h>
#include    <proto/utility.h>

#include    <dos/dos.h>
#include    <intuition/gadgetclass.h>
#include    <intuition/icclass.h>
#include    <clib/alib_protos.h>

#include <libraries/mui.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/NListview_mcc.h>
#include <MUI/NList_mcc.h>
//#include <MUI/BetterString_mcc.h>

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

/* Texteditor + custom attributes */
#include <MUI/TextEditor_mcc.h>
#define MUIA_CustTextEditor_ChannelPrivate (TextEditor_Dummy + 0xf01)
#define MUIA_CustTextEditor_ServerPrivate  (TextEditor_Dummy + 0xf02)

/* Menu IDs */
#define AiRcOS_MENUID_CONNECT          (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_CLOSE            (AiRcOS_MENUID + 2)
#define AiRcOS_MENUID_QUIT             (AiRcOS_MENUID + 3)
#define AiRcOS_MENUID_PREFS            (AiRcOS_MENUID + 4)
#define AiRcOS_MENUID_SERVERMANAGE     (AiRcOS_MENUID + 5)
#define AiRcOS_MENUID_HELP             (AiRcOS_MENUID + 6)
#define AiRcOS_MENUID_ABOUT            (AiRcOS_MENUID + 7)

/*
#define AiRcOS_MENUID_CONNECT          (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_CLOSE            (AiRcOS_MENUID + 2)
#define AiRcOS_MENUID_QUIT             (AiRcOS_MENUID + 3)
#define AiRcOS_MENUID_PREFS            (AiRcOS_MENUID + 4)
#define AiRcOS_MENUID_SERVERMANAGE     (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_HELP             (AiRcOS_MENUID + 5)
#define AiRcOS_MENUID_ABOUT            (AiRcOS_MENUID + 6)

#define AiRcOS_MENUID_CONNECT          (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_CLOSE            (AiRcOS_MENUID + 2)
#define AiRcOS_MENUID_QUIT             (AiRcOS_MENUID + 3)
#define AiRcOS_MENUID_PREFS            (AiRcOS_MENUID + 4)
#define AiRcOS_MENUID_SERVERMANAGE     (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_HELP             (AiRcOS_MENUID + 5)
#define AiRcOS_MENUID_ABOUT            (AiRcOS_MENUID + 6)

#define AiRcOS_MENUID_CONNECT          (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_CLOSE            (AiRcOS_MENUID + 2)
#define AiRcOS_MENUID_QUIT             (AiRcOS_MENUID + 3)
#define AiRcOS_MENUID_PREFS            (AiRcOS_MENUID + 4)
#define AiRcOS_MENUID_SERVERMANAGE     (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_HELP             (AiRcOS_MENUID + 5)
#define AiRcOS_MENUID_ABOUT            (AiRcOS_MENUID + 6)
*/

extern int aircos_IRC_donumeric(struct IRC_Connection_Private	*currentConnection, int num);
extern int aircos_IRC_nop(struct IRC_Connection_Private	*currentConnection);
extern struct IRC_Server_Priv  *aircos_add_server(char *addserv);
extern Object	*aircos_showServerConnect();
extern char *FormatToSend(char *unformatted_string);
extern char *FormatToDisplay(char *formatted_string);
extern BOOL aircosApp_loadPrefs();

extern char                          *aircos_Prefs_DefUser;
extern char                          *aircos_Prefs_DefNick;
extern struct functionrecord         commandList_array[];
extern struct aircos_servernode      *aircos_Prefs_ServerActive;
Object	                            *serverconnectWin;

#define INTERNALFUNCID(functionID) (functionID + 1)

struct intern_functionrecord
{
   struct Node           ifr_Node;
   struct functionrecord ifr_command;
};

extern int aircos_IRCFuncs_RegisterFuncs(void);
//extern int aircos_DCCFuncs_RegisterFuncs(void);

struct AiRcOS_internal *AiRcOS_Base;

BOOL aircosApp__OpenBSDSOCKET()
{
D(bug("[AiRcOS] aircosApp__OpenBSDSOCKET()\n"));
    if (AiRcOS_Base->Ai_SocketBase!=NULL) return TRUE;

    if ((AiRcOS_Base->Ai_SocketBase = OpenLibrary("bsdsocket.library",3)))
    {
        if ((AiRcOS_Base->Ai_sigbit_ProcessStack=AllocSignal(-1))!=-1)
        {
            if (SocketBaseTags(SBTM_SETVAL(SBTC_ERRNOLONGPTR), &AiRcOS_Base->Ai_errno,
                    SBTM_SETVAL(SBTC_HERRNOLONGPTR), &AiRcOS_Base->Ai_herrno,
                    SBTM_SETVAL(SBTC_SIGIOMASK), 1<<AiRcOS_Base->Ai_sigbit_ProcessStack,
                    TAG_END)<=0)
            {
                return TRUE;
            }
            else
            {
D(bug("[AiRcOS](openbsdsocket) Couldnt Set socketbase!\n"));
            }
            FreeSignal(AiRcOS_Base->Ai_sigbit_ProcessStack);
        }
        else
        {
D(bug("[AiRcOS](openbsdsocket) Couldnt allocate sigbit_ProcessStack!\n"));
        }
        CloseLibrary(SocketBase);
        SocketBase=0;
    }

    return FALSE;
}

/**************************************/
/* Custom TextEditor class dispatcher */
/**************************************/

struct CustomTEInstData
{
  struct  MUI_EventHandlerNode   ehnode;           /* Enable us to trap input events                      */
  struct IRC_Channel_Priv        *cte_ChanPriv;
  struct IRC_Server_Priv         *cte_ServPriv;
  char                           *partialNameToMatch;
  char                           *currentNameMatch;
  ULONG									partMatchY;
  ULONG									partMatchMinX;
  ULONG									partMatchMaxX;
};


BOOPSI_DISPATCHER(IPTR, TextEditor_Dispatcher, CLASS, self, message)
{

D(bug("[AiRcOS] TextEditor_Dispatcher(%x)\n", message->MethodID));
  switch(message->MethodID)
  {
  case MUIM_Setup:
  {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_Setup\n"));
    struct CustomTEInstData *data = INST_DATA(CLASS, self);
    if ((data->cte_ChanPriv)||(data->cte_ServPriv))
    {
      /* Only capture events in our input gadgets */
      data->ehnode.ehn_Priority = 0;
      data->ehnode.ehn_Flags    = 0;
      data->ehnode.ehn_Object   = self;
      data->ehnode.ehn_Class    = CLASS;
      data->ehnode.ehn_Events   = IDCMP_RAWKEY;

      DoMethod(_win(self), MUIM_Window_AddEventHandler, &data->ehnode);
    }
    break;
  }
  case OM_NEW:
  {
D(bug("[AiRcOS] TextEditor_Dispatcher: OM_NEW\n"));
    if((self = (Object *)DoSuperMethodA(CLASS, self, (Msg)message)))
    {
      struct CustomTEInstData *data = INST_DATA(CLASS, self);

      data->cte_ChanPriv = (struct IRC_Channel_Priv *)GetTagData (MUIA_CustTextEditor_ChannelPrivate, (ULONG) NULL, ((struct opSet *)message)->ops_AttrList);;
      data->cte_ServPriv = (struct IRC_Channel_Priv *)GetTagData (MUIA_CustTextEditor_ServerPrivate, (ULONG) NULL, ((struct opSet *)message)->ops_AttrList);;
      return self;
    }
    return FALSE;
    break;
  }
  case OM_DISPOSE:
  {
D(bug("[AiRcOS] TextEditor_Dispatcher: OM_DISPOSE\n"));
    break;
  }
  case MUIM_Cleanup:
  {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_Cleanup\n"));
    struct CustomTEInstData *data = INST_DATA(CLASS, self);
    if ((data->cte_ChanPriv)||(data->cte_ServPriv))
    {
      DoMethod(_win(self), MUIM_Window_RemEventHandler, &data->ehnode);
    }
    break;
  }


		case MUIM_Show:
		{
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_Show\n"));
				struct ColorMap *cm = muiRenderInfo(self)->mri_Screen->ViewPort.ColorMap;

/* IRC Colors 

   BLACK
   DARK BLUE
   DARK GREEN
   GREEN
   RED
   LIGHT RED
   DARK RED
   PURPLE
   BROWN
   ORANGE
   YELLOW
   LIGHT GREEN
   AQUA
   LIGHT BLUE
   BLUE
   VIOLET
   GREY
   LIGHT GRAY
   WHITE */

			AiRcOS_Base->editor_cmap[0] = ObtainBestPenA(cm, 0x00<<24, 0x00<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[1] = ObtainBestPenA(cm, 0xff<<24, 0xff<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[2] = ObtainBestPenA(cm, 0xff<<24, 0x00<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[3] = ObtainBestPenA(cm, 0x00<<24, 0x93<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[4] = ObtainBestPenA(cm, 0x00<<24, 0xff<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[5] = ObtainBestPenA(cm, 0xff<<24, 0xff<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[6] = ObtainBestPenA(cm, 0x00<<24, 0x00<<24, 0x7f<<24, NULL);
			AiRcOS_Base->editor_cmap[7] = ObtainBestPenA(cm, 0xff<<24, 0x00<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[8] = ObtainBestPenA(cm, 0x00<<24, 0x00<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[9] = ObtainBestPenA(cm, 0xff<<24, 0xff<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[10] = ObtainBestPenA(cm, 0xff<<24, 0x00<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[11] = ObtainBestPenA(cm, 0x00<<24, 0x93<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[12] = ObtainBestPenA(cm, 0x00<<24, 0xff<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[13] = ObtainBestPenA(cm, 0xff<<24, 0xff<<24, 0x00<<24, NULL);
			AiRcOS_Base->editor_cmap[14] = ObtainBestPenA(cm, 0x00<<24, 0x00<<24, 0x7f<<24, NULL);
			AiRcOS_Base->editor_cmap[15] = ObtainBestPenA(cm, 0xff<<24, 0x00<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[16] = ObtainBestPenA(cm, 0x00<<24, 0x00<<24, 0x7f<<24, NULL);
			AiRcOS_Base->editor_cmap[17] = ObtainBestPenA(cm, 0xff<<24, 0x00<<24, 0xff<<24, NULL);
			AiRcOS_Base->editor_cmap[18] = ObtainBestPenA(cm, 0xff<<24, 0xff<<24, 0xff<<24, NULL);
			break;
		}

		case MUIM_Hide:
		{
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_Hide\n"));
				struct ColorMap *cm = muiRenderInfo(self)->mri_Screen->ViewPort.ColorMap;
				int c;

			for(c = 0; c < 8; c++)
			{
				if(AiRcOS_Base->editor_cmap[c] >= 0)
				{
					ReleasePen(cm, AiRcOS_Base->editor_cmap[c]);
				}
			}
			break;
		}

		case MUIM_DragQuery:
		{
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_DragQuery\n"));
			return(TRUE);
		}

		case MUIM_DragDrop:
		{
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_DragDrop\n"));
         struct MUIP_DragDrop *drop_msg = (struct MUIP_DragDrop *)message;
         ULONG active;

			//if(GetAttr(MUIA_List_Active, drop_msg->obj, &active))
			//{
				//DoMethod(obj, MUIM_TextEditor_InsertText, StdEntries[active]);
			//}
			break;
		}

      case MUIM_HandleEvent:
      {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent\n"));
         struct CustomTEInstData *data = INST_DATA(CLASS, self);
         struct MUIP_HandleEvent *hevent_msg = (struct MUIP_HandleEvent *)message;
         struct IntuiMessage *imsg = hevent_msg->imsg;
         if (imsg->Class == IDCMP_RAWKEY)
         {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: RAWKEY Code %d\n", imsg->Code));
           if (imsg->Code != 66)
           {
             if (data->partialNameToMatch)
             {
               FreeVec(data->partialNameToMatch);
               data->partialNameToMatch = NULL;
             }
           }

           if (imsg->Code == 66)
           {
             char *foundName = NULL;
             struct Rectangle crsr_Ncomplete;
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: TAB Detected!\n"));
             if (!(data->partialNameToMatch))
             {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Start new name match\n"));
               if (data->cte_ChanPriv)
               {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Gadget has UserList - Try to perform name completion ..\n"));
                 get(self, MUIA_TextEditor_CursorX, &crsr_Ncomplete.MaxX);
                 get(self, MUIA_TextEditor_CursorY, &crsr_Ncomplete.MinY);

                 if (crsr_Ncomplete.MaxX!=0)
                 {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Valid cursor X offset\n"));
                   char *inputGadBuffer = NULL;
                   crsr_Ncomplete.MinX = 0;
                   crsr_Ncomplete.MaxY = crsr_Ncomplete.MinY;

                   inputGadBuffer = DoMethod( self, MUIM_TextEditor_ExportText );                                      

                   int curline = 0, cur_pos=0;
                   while (curline <= crsr_Ncomplete.MinY)
                   {
                     if (curline == crsr_Ncomplete.MinY)
                       break;

                     if (inputGadBuffer[cur_pos]=='\n')
                     {
                       curline++;
                     }
                     cur_pos++;
                   }

D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: cursor line (Y) @ starts at pos %d\n", cur_pos));

					    int start_char = 0;                   
                   for (start_char = 0; start_char < crsr_Ncomplete.MaxX; start_char++)
                   {
                     if (inputGadBuffer[((cur_pos + crsr_Ncomplete.MaxX) - start_char)]==' ')
                     {
                       if (start_char != 0)
                       start_char = start_char -1;
                       break;
                     }
                   }
                   start_char = crsr_Ncomplete.MaxX - start_char;

D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: partname starts @ %d, ends at %d\n", start_char, crsr_Ncomplete.MaxX));

                   if ((crsr_Ncomplete.MaxX - start_char) > 0)
                   {
                     if (data->partialNameToMatch = AllocVec(crsr_Ncomplete.MaxX - start_char, MEMF_CLEAR|MEMF_PUBLIC))
                     {
                       CopyMem(inputGadBuffer + cur_pos + start_char, data->partialNameToMatch, crsr_Ncomplete.MaxX - start_char);
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Attempt to match partialname '%s'\n", data->partialNameToMatch));
                       struct IRC_Channel_Group_SubGroup *current_Group=NULL;
                       struct IRC_Channel_Group_User *current_User=NULL;
                       ForeachNode(&data->cte_ChanPriv->chan_usergroup, current_Group)
                       {
                         ForeachNode(&current_Group->group_usergroup, current_User)
                         {
                           if (strncasecmp(current_User->user_name, data->partialNameToMatch, strlen(data->partialNameToMatch))==0)
                           {
                             foundName = current_User->user_name;
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Possible Match found ...  '%s'\n", foundName));
                             goto possiblefound;
                           }
                         }
                       }
possiblefound:         if (foundName)
                       {
                         data->partMatchY = crsr_Ncomplete.MinY;
                         data->partMatchMinX = start_char;
                         data->partMatchMaxX = crsr_Ncomplete.MaxX;
                       }
                     }
                   }
                   FreeVec(inputGadBuffer);
                 }                
               }
             }
             else
             {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Already Active match for '%s'!\n", data->partialNameToMatch));
               BOOL FoundCurrentMatch = FALSE;
               struct IRC_Channel_Group_SubGroup *current_Group=NULL;
               struct IRC_Channel_Group_User *current_User=NULL;

               ForeachNode(&data->cte_ChanPriv->chan_usergroup, current_Group)
               {
                 ForeachNode(&current_Group->group_usergroup, current_User)
                 {
                   if (FoundCurrentMatch)
                   {
                     if (strncasecmp(current_User->user_name, data->partialNameToMatch, strlen(data->partialNameToMatch))==0)
                     {
                       foundName = current_User->user_name;
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Next Possible Match found ...  '%s'\n", foundName));
                       goto possnextmatched;
                     }
                   }
                   else
                   {
                     if (strcasecmp(current_User->user_name, data->currentNameMatch)==0)
                     {
                       FoundCurrentMatch = TRUE;
                     }
                   }
                 }
               }
possnextmatched:
             }

             if (foundName)
             {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Inseting matched name in input gadget ..\n"));
               crsr_Ncomplete.MinX = data->partMatchMinX;
               crsr_Ncomplete.MinY = data->partMatchY;
               crsr_Ncomplete.MaxX = data->partMatchMaxX;
               crsr_Ncomplete.MaxY = data->partMatchY;
               data->currentNameMatch = foundName;

               set(self, MUIA_TextEditor_Quiet, TRUE);
               DoMethod(self, MUIM_TextEditor_MarkText,
                                    crsr_Ncomplete.MinX, crsr_Ncomplete.MinY,
                                    crsr_Ncomplete.MaxX, crsr_Ncomplete.MaxY);

               DoMethod(self, MUIM_TextEditor_Replace, data->currentNameMatch, 0);

               crsr_Ncomplete.MaxX = crsr_Ncomplete.MinX + strlen(data->currentNameMatch);

               DoMethod(self, MUIM_TextEditor_MarkText,
                                      crsr_Ncomplete.MaxX, crsr_Ncomplete.MaxY,
                                      crsr_Ncomplete.MaxX, crsr_Ncomplete.MaxY);
               set(self, MUIA_TextEditor_CursorX, crsr_Ncomplete.MaxX);
               set(self, MUIA_TextEditor_CursorY, crsr_Ncomplete.MaxY);

               set(self, MUIA_TextEditor_Quiet, FALSE);                        
             }
             else
             {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: No Match Found\n"));             
             }

             return(MUI_EventHandlerRC_Eat);
           } else if (imsg->Code == 68) {
             if(imsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
             {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Shift + Enter Detected!\n"));
               // Treat as a standard <ENTER> - and let the superclass deal with it
               imsg->Qualifier &= ~(IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
               break;
             }
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Enter Detected!\n"));
             // Cause a send for this object instead
             if (data->cte_ChanPriv)
             {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Causing Channel Send ..\n"));
               DoMethod(data->cte_ChanPriv->chan_send, MUIM_Set, MUIA_Pressed, TRUE);
               DoMethod(data->cte_ChanPriv->chan_send, MUIM_Set, MUIA_Pressed, FALSE);
             } else if (data->cte_ServPriv) {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Causing Server Send ..\n"));
               DoMethod(data->cte_ServPriv->serv_send, MUIM_Set, MUIA_Pressed, TRUE);
               DoMethod(data->cte_ServPriv->serv_send, MUIM_Set, MUIA_Pressed, FALSE);
             }             
             return(MUI_EventHandlerRC_Eat);
           } else if ((imsg->Code == 0x4c)||(imsg->Code == 0x3e)) {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Curs Up Detected!\n"));
             return(MUI_EventHandlerRC_Eat);
           } else if ((imsg->Code == 0x4d)||(imsg->Code == 0x1e)) {
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleEvent: Curs Down Detected!\n"));
             return(MUI_EventHandlerRC_Eat);
           }
         }
			break;
		}

		case MUIM_TextEditor_HandleError:
		{
				char *errortxt = NULL;
D(bug("[AiRcOS] TextEditor_Dispatcher: MUIM_TextEditor_HandleError\n"));
			switch((( struct MUIP_TextEditor_HandleError *)message)->errorcode)
			{
				case Error_ClipboardIsEmpty:
					errortxt = "\33cThe clipboard is empty.";
					break;
				case Error_ClipboardIsNotFTXT:
					errortxt = "\33cThe clipboard does not contain text.";
					break;
				case Error_MacroBufferIsFull:
					break;
				case Error_MemoryAllocationFailed:
					break;
				case Error_NoAreaMarked:
					errortxt = "\33cNo area marked.";
					break;
				case Error_NoMacroDefined:
					break;
				case Error_NothingToRedo:
					errortxt = "\33cNothing to redo.";
					break;
				case Error_NothingToUndo:
					errortxt = "\33cNothing to undo.";
					break;
				case Error_NotEnoughUndoMem:
					errortxt = "There is not enough memory\nto keep the undo buffer.\n\nThe undobuffer is lost.";
					break;
				case Error_StringNotFound:
					break;
				case Error_NoBookmarkInstalled:
					errortxt = "There is no bookmark installed!";
					break;
				case Error_BookmarkHasBeenLost:
					errortxt = "Your bookmark has unfortunately been lost.";
					break;
			}
			if(errortxt)
			{
D(bug("[AiRcOS] TextEditor:Error %s\n", errortxt));
//				MUI_Request(app, window, 0L, NULL, "Continue", errortxt);
			}
			break;
		}
	}
  return(DoSuperMethodA(CLASS, self, message));

}
BOOPSI_DISPATCHER_END

int aircosApp_sendline(struct IRC_Connection_Private *forConnection)
{
    int count;

    forConnection->connection_buff_send = FormatToSend(forConnection->connection_buff_send);

    if ((count = send(forConnection->connection_socket, forConnection->connection_buff_send,
        strlen(forConnection->connection_buff_send),0)) < 0)
    {
        return -1;
    }
    return count;
}

struct IRC_Channel_Priv  *FindNamedChannel( struct IRC_Connection_Private *onThisConnection, char *findThisChan)
{
   struct IRC_Channel_Priv  *foundChannel = NULL;
D(bug("[AiRcOS] FindNamedChannel('%s' on '%s')\n", findThisChan, onThisConnection->connection_server));

   ForeachNode(&onThisConnection->connected_server->serv_chans, foundChannel)
   {
D(bug("[AiRcOS](FindNamedChannel) Checking against record for '%s'\n",foundChannel->chan_name));
      if (strcasecmp(foundChannel->chan_name,findThisChan)==0) return foundChannel;
   }
   return NULL;
}

void aircosApp_showChanOutput(struct IRC_Channel_Priv *thisChanPriv, struct serv_Outline *sa)
{
  if ( thisChanPriv->chan_displayedlines >= CHANOUT_MAXLINES)
  {
D(bug("[AiRcOS](showChanOutput) Freeing text at head of output ... \n"));
  }

// if (aircos_PrefsStruct->prefs_LogDir)
// {
//     Output Current line to log file for this channel... 
// }

  //And finally output the line ...
  struct Rectangle crsr_output;

  set(thisChanPriv->chan_output, MUIA_TextEditor_Quiet, TRUE);  
  if (thisChanPriv->chan_currentpen != 0)
  {
    get(thisChanPriv->chan_output, MUIA_TextEditor_CursorX, &crsr_output.MinX);
    get(thisChanPriv->chan_output, MUIA_TextEditor_CursorY, &crsr_output.MinY);
  }
  
  sa->so_name = FormatToDisplay(sa->so_name);
  DoMethod( thisChanPriv->chan_output, MUIM_TextEditor_InsertText, sa->so_name, MUIV_TextEditor_InsertText_Bottom );

  if (thisChanPriv->chan_currentpen != 0)
  {
    get(thisChanPriv->chan_output, MUIA_TextEditor_CursorX, &crsr_output.MaxX);
    get(thisChanPriv->chan_output, MUIA_TextEditor_CursorY, &crsr_output.MaxY);
    DoMethod(thisChanPriv->chan_output, MUIM_TextEditor_MarkText,
                                      crsr_output.MinX, crsr_output.MinY,
                                      crsr_output.MaxX, crsr_output.MaxY);

    DoMethod(thisChanPriv->chan_output, MUIM_Set, MUIA_TextEditor_Pen, thisChanPriv->chan_currentpen);

    DoMethod(thisChanPriv->chan_output, MUIM_TextEditor_MarkText,
                                      crsr_output.MaxX, crsr_output.MaxY,
                                      crsr_output.MaxX, crsr_output.MaxY);
    set(thisChanPriv->chan_output, MUIA_TextEditor_CursorX, crsr_output.MaxX);
    set(thisChanPriv->chan_output, MUIA_TextEditor_CursorY, crsr_output.MaxY);
  }
  set(thisChanPriv->chan_output, MUIA_TextEditor_Quiet, FALSE);
  thisChanPriv->chan_displayedlines++;
}

void aircosApp_setChanPen(struct IRC_Channel_Priv *thisChanPriv, ULONG pen)
{
  thisChanPriv->chan_currentpen = pen;
}

/****/

int aircosApp_RegisterFunction(char *functionName, void *functionRcvCall, int functionRcvCallArgCnt, void *functionSendCall, int functionSendCallArgCnt)
{
D(bug("[AiRcOS] aircosApp_RegisterFunction(%s)\n", functionName));

  struct intern_functionrecord *newRecord = AllocVec(sizeof(struct intern_functionrecord),MEMF_CLEAR);
  newRecord->ifr_command.command = functionName;
  newRecord->ifr_command.command_doFunction = functionRcvCall;
  newRecord->ifr_command.servArgCount = functionRcvCallArgCnt;
  newRecord->ifr_command.command_clientFunction = functionSendCall;
  newRecord->ifr_command.clientArgCount = functionSendCallArgCnt;
  AddTail(&AiRcOS_Base->funcs_FunctionList, &newRecord->ifr_Node);

  return 1;
}

APTR aircosApp_FindFunction(char * functionName)
{
D(bug("[AiRcOS] aircosApp_FindFunction(%s)\n", functionName));
  struct intern_functionrecord *foundFunction = NULL;

  ForeachNode(&AiRcOS_Base->funcs_FunctionList, foundFunction)
  {
D(bug("[AiRcOS](aircosApp_FindFunction) Checking against record for '%s'\n",foundFunction->ifr_command.command));
    if (strcasecmp(foundFunction->ifr_command.command, functionName)==0) return &foundFunction->ifr_command;
  }
  return NULL;
}

int aircosApp_RemoveFunction(char * functionName)
{
D(bug("[AiRcOS] aircosApp_RemoveFunction(%s)\n", functionName));

  return 1;
}

int aircosApp_RegisterAction(char *actionName, void *actionRcvCall, int actionRcvCallArgCnt, void *actionSendCall, int actionSendCallArgCnt)
{
D(bug("[AiRcOS] aircosApp_RegisterAction(%s)\n", actionName));

  struct intern_functionrecord *newRecord = AllocVec(sizeof(struct intern_functionrecord),MEMF_CLEAR);
  newRecord->ifr_command.command = actionName;
  newRecord->ifr_command.command_doFunction = actionRcvCall;
  newRecord->ifr_command.servArgCount = actionRcvCallArgCnt;
  newRecord->ifr_command.command_clientFunction = actionSendCall;
  newRecord->ifr_command.clientArgCount = actionSendCallArgCnt;
  AddTail(&AiRcOS_Base->funcs_FunctionList, &newRecord->ifr_Node);

  return 1;
}

APTR aircosApp_FindAction(char * actionName)
{
D(bug("[AiRcOS] aircosApp_FindAction(%s)\n", actionName));
  struct intern_functionrecord *foundAction = NULL;

  ForeachNode(&AiRcOS_Base->funcs_ActionList, foundAction)
  {
D(bug("[AiRcOS](aircosApp_FindAction) Checking against record for '%s'\n",foundAction->ifr_command.command));
    if (strcasecmp(foundAction->ifr_command.command, actionName)==0) return &foundAction->ifr_command;
  }
  return NULL;
}

int aircosApp_RemoveAction(char * actionName)
{
D(bug("[AiRcOS] aircosApp_RemoveAction(%s)\n", actionName));

  return 1;
}

int aircosApp_serverconnect(struct IRC_Connection_Private *makeThisConnection)
{
D(bug("[AiRcOS] serverconnect(%s)\n",makeThisConnection->connection_server));

    struct sockaddr_in sa;
    struct hostent *hp=NULL;
    int t=0;

    if (!(SocketBase))
    {
        if (!(aircosApp__OpenBSDSOCKET()))
        {
D(bug("[AiRcOS](serverconnect) No BSDSOCKET library!\n"));
        return -1;
        }
    }

    if (!(makeThisConnection->connection_buff_send))
    {
        if(!(makeThisConnection->connection_buff_send = AllocVec(AIRCOS_MAXSERVERRESPONSE,MEMF_CLEAR)))
        {
D(bug("[AiRcOS](serverconnect) Failed to allocate transmit buffer!\n"));
        return -1;
        }
    }
    if (!(makeThisConnection->connection_buff_recieve))
    {
        if(!(makeThisConnection->connection_buff_recieve = AllocVec(AIRCOS_MAXSERVERRESPONSE,MEMF_CLEAR)))
        {
D(bug("[AiRcOS](serverconnect) Failed to allocate recieve buffer!\n"));
        FreeVec(makeThisConnection->connection_buff_send);
        makeThisConnection->connection_buff_send = NULL;
        return -1;
        }
    }

    if ((hp = gethostbyname(makeThisConnection->connection_server)) == NULL) return -1;

D(bug("[AiRcOS](serverconnect) found hostname record for '%s'\n",hp->h_name));

#warning "TODO: hostent record seems malformed?"
//    for (t = 0, makeThisConnection->connection_socket = -1; makeThisConnection->connection_socket < 0 && hp->h_addr_list[t] != NULL; t++)
// {
        memset(&sa, 0L, sizeof(struct sockaddr_in));
        CopyMem(hp->h_addr,(char *)&sa.sin_addr,hp->h_length);
        sa.sin_family = hp->h_addrtype;
        sa.sin_port = htons((unsigned short) makeThisConnection->connection_port);

D(bug("[AiRcOS](serverconnect) Attempting to connect to hostentry record %d\n",t));

        makeThisConnection->connection_socket = socket(hp->h_addrtype, SOCK_STREAM, 0);
D(bug("[AiRcOS](serverconnect) Return from socket()=%d\n",makeThisConnection->connection_socket));

        if (makeThisConnection->connection_socket >= 0)
        {
D(bug("[AiRcOS](serverconnect) socket created ..\n"));
            if (connect(makeThisConnection->connection_socket, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0)
            {
                CloseSocket(makeThisConnection->connection_socket);
                makeThisConnection->connection_socket = -1;
D(bug("[AiRcOS](serverconnect) socket connect failed\n"));
            }
            else
            {
D(bug("[AiRcOS](serverconnect) socket lvl connection established\n[AiRcOS](serverconnect) Attempting login ..\n"));

                sprintf(makeThisConnection->connection_buff_send, "USER %s * * :The POWER of AROS!\n",
                    makeThisConnection->connection_user);
                aircosApp_sendline(makeThisConnection);
D(bug("[AiRcOS](serverconnect) Sent USER NAME ..\n"));
                if (makeThisConnection->connection_pass != "")
                {
                    sprintf(makeThisConnection->connection_buff_send, "PASS :%s\n", makeThisConnection->connection_pass);
                    aircosApp_sendline(makeThisConnection);
D(bug("[AiRcOS](serverconnect) Sent PASSWORD ..\n"));
                }

                sprintf(makeThisConnection->connection_buff_send, "NICK :%s\n", makeThisConnection->connection_nick);
                aircosApp_sendline(makeThisConnection);
D(bug("[AiRcOS](serverconnect) Sent NICK ..\n"));
            }
        }

        if (makeThisConnection->connection_socket == -1)
        {
D(bug("[AiRcOS](serverconnect) create socket failed\n"));
            switch (errno)
            {
            case EINVAL:
            case EPROTONOSUPPORT:
D(bug("[AiRcOS](serverconnect)   socket() : SOCK_STREAM unsupported on this domain.\n"));
                break;

            case ENFILE:
D(bug("[AiRcOS](serverconnect)   socket() : no free sockets to allocate.\n"));
                break;

            case EMFILE:
D(bug("[AiRcOS](serverconnect)   socket() : process table overflow while requesting socket.\n"));
                break;

            case EACCES:
D(bug("[AiRcOS](serverconnect)   socket() : Permission denied creating socket.\n"));
                break;

            case ENOMEM:
D(bug("[AiRcOS](serverconnect)   socket() : Not enough free mem to allocate socket.\n"));
                break;

            default:
D(bug("[AiRcOS](serverconnect)   socket() : Unknown error allocating socket\n"));
                break;
            }

            FreeVec(makeThisConnection->connection_buff_send);
            makeThisConnection->connection_buff_send = NULL;
            FreeVec(makeThisConnection->connection_buff_recieve);
            makeThisConnection->connection_buff_recieve = NULL;
        }
// }

    return makeThisConnection->connection_socket;
}

int aircosApp_processServerData(struct IRC_Connection_Private	*process_thisConnection, char *serverin_buffer)
{
    int pos, found = 0;
D(bug("[AiRcOS] processserverdata('%s')\n", serverin_buffer));
D(bug("[AiRcOS](processserverdata) Connected Nick '%s'\n", process_thisConnection->connection_nick));

    if((AiRcOS_Base->Ai_tmp=strstr(serverin_buffer, "PING :")))
    {
D(bug("[AiRcOS](processserverdata) Quick Reply to PING ..\n"));
        strncpy(process_thisConnection->connection_buff_send,AiRcOS_Base->Ai_tmp, 512);
        process_thisConnection->connection_buff_send[510] = '\0';
        process_thisConnection->connection_buff_send[1] = 'O'; /* Change pIng to pOng */
        strcat(process_thisConnection->connection_buff_send, "\n");
        return aircosApp_sendline(process_thisConnection);
    }

    if (process_thisConnection->connection_unprocessed) FreeVec(process_thisConnection->connection_unprocessed);
    process_thisConnection->connection_unprocessed = AllocVec(strlen(serverin_buffer) +1,MEMF_CLEAR);
    strcpy( process_thisConnection->connection_unprocessed, serverin_buffer );
    
    pos = 0;
    process_thisConnection->connection_serv_ARGS[pos] = serverin_buffer;
    if (*(process_thisConnection->connection_serv_ARGS[pos]) == ':')
        process_thisConnection->connection_serv_ARGS[pos]++;

    /* Seperate the server string into args .. */
    while (process_thisConnection->connection_serv_ARGS[pos] != NULL && pos < (MAX_SERVER_ARGS-1))
        {
            if (*(process_thisConnection->connection_serv_ARGS[pos]) == ':') break;
            else
            {
            if ((AiRcOS_Base->Ai_tmp = strchr(process_thisConnection->connection_serv_ARGS[pos], ' ')))
            {
                    process_thisConnection->connection_serv_ARGS[++pos] = &AiRcOS_Base->Ai_tmp[1];
                    *AiRcOS_Base->Ai_tmp = '\0';
            }
                else process_thisConnection->connection_serv_ARGS[++pos] = NULL;
            }
        }

#if defined(DEBUG)
D(bug("[AiRcOS](processserverdata) ARGS :"));
    int debug_args_count;
        for (debug_args_count = 0; debug_args_count <= pos ; debug_args_count++)
        {
D(bug(" %d:'%s'",debug_args_count,process_thisConnection->connection_serv_ARGS[debug_args_count]));
        }
D(bug("\n"));
#endif

    if (process_thisConnection->connection_serv_ARGS[pos] != NULL
        && *(process_thisConnection->connection_serv_ARGS[pos]) == ':')
        process_thisConnection->connection_serv_ARGS[pos]++;
    
    process_thisConnection->connection_serv_ARGS[++pos] = NULL;
    
    if ((AiRcOS_Base->Ai_tmp = strchr(process_thisConnection->connection_serv_ARGS[0], '!')))
    {
            *AiRcOS_Base->Ai_tmp = '\0';
    }
    
    if ((pos = atoi(process_thisConnection->connection_serv_ARGS[1])))
    {
D(bug("[AiRcOS](processserverdata) Calling IRC DoNumeric funtion for %d\n", pos));
        pos = aircos_IRC_donumeric(process_thisConnection, pos);
    }
    else
    {
D(bug("[AiRcOS](processserverdata) Text Command recieved '%s'\n",process_thisConnection->connection_serv_ARGS[1]));
            for (pos = 0; commandList_array[pos].command!=NULL && !found; pos++) found =
               (strcasecmp(commandList_array[pos].command, process_thisConnection->connection_serv_ARGS[1]) == 0);
    
            if (found)
            {
D(bug("[AiRcOS](processserverdata) Calling IRC funtion %d\n", pos-1));
                pos = (*commandList_array[pos-1].command_doFunction)(process_thisConnection);
            }
            else
            {
D(bug("[AiRcOS](processserverdata) Calling IRC NOP funtion\n"));
                pos = aircos_IRC_nop(process_thisConnection);
            }
    }
D(bug("[AiRcOS](processserverdata) Returned from IRC funtion\n"));
    
    if (strncmp(process_thisConnection->connection_serv_ARGS[1], "Closing", 7) == 0)
    {
    return (AiRcOS_Base->Ai_reconnect = 0);
    }

    return 1;
}



AROS_UFH3(void, serverconnect_func,
   AROS_UFHA(struct Hook *, unused_hook, A0),
   AROS_UFHA(APTR, obj, A2),
   AROS_UFHA(struct IRC_Channel_Priv  *, hook_channel_arg, A1 ))
{
  AROS_USERFUNC_INIT

D(bug("[AiRcOS] serverconnect_func()\n"));

  struct IRC_Connection_Private	*dtest_connection = NULL;
  char  *join_channel = NULL;

    if (!(dtest_connection = AllocVec(sizeof(struct IRC_Connection_Private),MEMF_CLEAR)))
    {
D(bug("[AiRcOS](serverconnect_func) Failed to allocate connection record!!\n"));
        return;
    }

   if (!(aircos_Prefs_ServerActive))
   {
    /* CONNECT USING QUICK CONNECT */
D(bug("[AiRcOS](serverconnect_func) Using Quick Connect\n"));
     set(AiRcOS_Base->butt_connectServer, MUIA_Disabled, TRUE);

     IPTR  temp_servport=0;

     char *str_connect_nick = NULL;

     get( AiRcOS_Base->quickcon_servadd, MUIA_String_Contents, &dtest_connection->connection_server);
     get( AiRcOS_Base->quickcon_servport, MUIA_String_Integer, &temp_servport);
     get( AiRcOS_Base->quickcon_servuser, MUIA_String_Contents, &dtest_connection->connection_user);
     get( AiRcOS_Base->quickcon_servpass, MUIA_String_Contents, &dtest_connection->connection_pass);

     get( AiRcOS_Base->quickcon_nick, MUIA_String_Contents, &str_connect_nick);
     if (!(dtest_connection->connection_nick = (char *)AllocVec(strlen(str_connect_nick)+1,MEMF_CLEAR)))
     {
D(bug("[AiRcOS](serverconnect_func) Failed to allocate nick buffer!!.\n"));
       set(AiRcOS_Base->butt_connectServer, MUIA_Disabled, FALSE);
       return;
     }

     strcpy(dtest_connection->connection_nick, str_connect_nick);

     get( AiRcOS_Base->quickcon_channel, MUIA_String_Contents, &join_channel);

     dtest_connection->connection_port = (UWORD)temp_servport;
D(bug("[AiRcOS](serverconnect_func) Attempting to connect to %s:%d, as %s\n", dtest_connection->connection_server, temp_servport, dtest_connection->connection_user));

D(bug("[AiRcOS](serverconnect_func) Port : %d, Nick %s\n", dtest_connection->connection_port,dtest_connection->connection_nick));

    } else {
    /* CONNECT USING AN EXISTING PREFS RECORD */
D(bug("[AiRcOS](serverconnect_func) Using Prefs Connection.\n"));
      dtest_connection->connection_server = aircos_Prefs_ServerActive->asn_Server;
      dtest_connection->connection_port = aircos_Prefs_ServerActive->asn_Port;

      dtest_connection->connection_user = aircos_Prefs_DefUser;
      if (!(dtest_connection->connection_nick = (char *)AllocVec(strlen(aircos_Prefs_DefNick)+1,MEMF_CLEAR)))
      {
D(bug("[AiRcOS](serverconnect_func) Failed to allocate nick buffer!!.\n"));
        return;
      }
      strcpy(dtest_connection->connection_nick, aircos_Prefs_DefNick);   
      join_channel = "#AROS";
    }

    int test_socket = aircosApp_serverconnect(dtest_connection);
    if (test_socket >= 0)
    {
        struct IRC_Server_Priv  *connected_server = NULL;
      
        if (!(connected_server = aircos_add_server(dtest_connection->connection_server)))
        {
D(bug("[AiRcOS](serverconnect_func) Failed to create connection page!!.\n"));
            set(AiRcOS_Base->butt_connectServer, MUIA_Disabled, FALSE);
            return;
        }

        connected_server->serv_connection = dtest_connection;
        dtest_connection->connected_server = connected_server;

        set(AiRcOS_Base->butt_connectServer, MUIA_Disabled, FALSE);
        set(AiRcOS_Base->aircos_quickconnectwin, MUIA_Window_Open, FALSE);
      
D(bug("[AiRcOS](serverconnect_func) Socket connection successfull\n"));
        if (join_channel != "")
        {
D(bug("[AiRcOS](serverconnect_func) Attempting to join channel %s ..\n", join_channel));

            sprintf(dtest_connection->connection_buff_send, "JOIN %s\n", join_channel);
            aircosApp_sendline(dtest_connection);
        }
    }
    else
    {
D(bug("[AiRcOS](serverconnect_func) Socket connection failed\n"));
        if (dtest_connection->connection_buff_send)
        {
            FreeVec(dtest_connection->connection_buff_send);
            dtest_connection->connection_buff_send = NULL;
        }
    
        if (dtest_connection->connection_buff_recieve)
        {
            FreeVec(dtest_connection->connection_buff_recieve);
            dtest_connection->connection_buff_recieve = NULL;
        }
        FreeVec(dtest_connection);
        dtest_connection = NULL;
        set(AiRcOS_Base->butt_connectServer, MUIA_Disabled, FALSE);
    }

    if (dtest_connection)
    {
        AddTail((struct List *)&AiRcOS_Base->aircos_looseconnectionlist,(struct Node *) &dtest_connection->connection_node);
    }
// AiRcOS_Base->temp_process_thisConnection = dtest_connection;   set(AiRcOS_Base->butt_connectServer,MUIA_Disabled,FALSE);

    AROS_USERFUNC_EXIT
};


void aircosApp_closeConnection(struct IRC_Connection_Private	*close_thisConnection)
{
    CloseSocket(close_thisConnection->connection_socket);

    if (close_thisConnection->connection_buff_send)
    {
        FreeVec(close_thisConnection->connection_buff_send);
        close_thisConnection->connection_buff_send = NULL;
    }
    
    if (close_thisConnection->connection_buff_recieve)
    {
        FreeVec(close_thisConnection->connection_buff_recieve);
        close_thisConnection->connection_buff_recieve = NULL;
    }
    
    FreeVec(close_thisConnection);
    close_thisConnection = NULL;
}

void  aircosApp_MainProcessLoop()
{
    ULONG sigs = 0;

D(bug("[AiRcOS] aircosApp_MainProcessLoop()\n"));
    while (DoMethod(AiRcOS_Base->aircos_app, MUIM_Application_NewInput, (IPTR) &sigs) != MUIV_Application_ReturnID_Quit)
    {
        if (sigs)
        {
            ULONG mask = sigs | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D | (1<<AiRcOS_Base->Ai_sigbit_ProcessStack);
            int sel_sock, connection_Count = 0, highest_socket = 0;
            struct IRC_Connection_Private	*process_thisConnection = NULL;

            FD_ZERO(&AiRcOS_Base->Ai_readfs);
            ForeachNode(&AiRcOS_Base->aircos_looseconnectionlist, process_thisConnection)
            {
               FD_SET(process_thisConnection->connection_socket, &AiRcOS_Base->Ai_readfs);
               if (process_thisConnection->connection_socket > highest_socket)
                   highest_socket = process_thisConnection->connection_socket;
               connection_Count++;
            }

            if (connection_Count > 0)
            {
                //D(bug("[AiRcOS](MPL) Use WaitSelect ..\n"));
                sel_sock = WaitSelect(highest_socket+1, &AiRcOS_Base->Ai_readfs, NULL, NULL, NULL, &mask);
                if (sel_sock > 0)
                {
                    ForeachNode(&AiRcOS_Base->aircos_looseconnectionlist, process_thisConnection)
                    {
                        if (FD_ISSET(process_thisConnection->connection_socket, &AiRcOS_Base->Ai_readfs))
                        {
                            //D(bug("[AiRcOS](MPL) Recieved signal from stack ..\n"));
                            char *tmpbuff = NULL,*tmpunprocbuff=NULL;
                            UWORD tmpbuff_currpos=0;
                            int count, i;
                            BOOL processed = FALSE;

                            if(process_thisConnection->connection_rawdata)
                            {
                             CopyMem(process_thisConnection->connection_rawdata,
                                process_thisConnection->connection_buff_recieve,
                                process_thisConnection->connection_rawdata_length);
                            }
                            if ((count =
                            recv(process_thisConnection->connection_socket,(process_thisConnection->connection_buff_recieve) +
                            process_thisConnection->connection_rawdata_length , AIRCOS_MAXSERVERRESPONSE -
                            process_thisConnection->connection_rawdata_length,0)) > 0)
                            {
                                if(process_thisConnection->connection_rawdata)
                                {
                                    FreeVec(process_thisConnection->connection_rawdata);
                                    process_thisConnection->connection_rawdata = NULL;
                                    process_thisConnection->connection_rawdata_length = 0;
                                }

                                if ((tmpbuff = AllocVec(count,MEMF_CLEAR)))
                                {
D(bug("[AiRcOS](MPL) Processing server output (%d bytes)\n", count));
                                    for (i = 0; i < count; i++)
                                    {
                                        if (process_thisConnection->connection_buff_recieve[i] == '\n')
                                        {
D(bug("\n[AiRcOS](MPL) Process server line (%d bytes)\n"));
                                            tmpbuff[tmpbuff_currpos] = '\0';
                                            processed = TRUE;
                                            if (!aircosApp_processServerData(process_thisConnection, tmpbuff))
                                            {
D(bug("[AiRcOS](MPL) Response End/Error?\n"));
                                                // return 0;
                                            }
D(bug("[AiRcOS](MPL)         '%s'\n",tmpbuff));
                                            tmpbuff_currpos=0;
                                            tmpunprocbuff=NULL;
                                        }
                                        else if (process_thisConnection->connection_buff_recieve[i] != '\r')
                                        {
D(bug("."));
                                            tmpbuff[tmpbuff_currpos++] = process_thisConnection->connection_buff_recieve[i];
                                        }
                                        else 
                                        {
D(bug("[AiRcOS](MPL) Response = r after %d bytes\n", i));
                                            // return 1;
                                            if ((count - i) > 1)
                                            {
                                                if (tmpunprocbuff==NULL)
                                                {
                                                    tmpunprocbuff=&process_thisConnection->connection_buff_recieve[i]+1;
                                                    processed = FALSE;
                                                }
                                            }
                                        }
                                    }
D(bug("[AiRcOS](MPL) Server output done\n"));
                                    if (!(processed))
                                    {
D(bug("[AiRcOS](MPL) unprocessed data remains ..\n"));
                                        if (tmpunprocbuff)
                                        {
D(bug("[AiRcOS](MPL) storing unproccessed buffer info ..\n"));
                                            process_thisConnection->connection_rawdata_length = (IPTR)&process_thisConnection->connection_buff_recieve[count] - (IPTR)tmpunprocbuff;
                                            process_thisConnection->connection_rawdata = AllocVec(process_thisConnection->connection_rawdata_length, MEMF_CLEAR);
                                            CopyMem(tmpunprocbuff, process_thisConnection->connection_rawdata, process_thisConnection->connection_rawdata_length);
                                        }
                                    }
                                    else
                                    {
D(bug("[AiRcOS](MPL) data processed completely ..\n"));
                                        if (process_thisConnection->connection_rawdata)
                                        {
D(bug("[AiRcOS](MPL) freeing unproccessed buffer\n"));
                                          FreeVec(process_thisConnection->connection_rawdata);
                                        }
                                        process_thisConnection->connection_rawdata = NULL;
                                        process_thisConnection->connection_rawdata_length = 0;
                                    }
												FreeVec(tmpbuff);
D(bug("[AiRcOS](MPL) output processing complete\n"));
                                }
                            }
                        }
                    }
                }
                else if (sel_sock < 0)
                {
                    D(bug("[AiRcOS](MPL) Signal ERROR from stack ..\n"));
                }
            }
            else
            {
                //D(bug("[AiRcOS](MPL) Use standard Wait ..\n"));
                mask = Wait(mask);
            }

            if (mask & SIGBREAKF_CTRL_C) break;
            if (mask & SIGBREAKF_CTRL_D) break;
        }
    }         
    D(bug("[AiRcOS](MPL) Application exiting ..\n"));
    MUI_DisposeObject(AiRcOS_Base->aircos_app);
}

AROS_UFH3 ( void, menuprocess_func,
    AROS_UFHA ( struct Hook*, h, A0 ),
    AROS_UFHA ( APTR, obj, A2 ),
    AROS_UFHA ( APTR, param, A1 ))
{
    AROS_USERFUNC_INIT

D(bug("[AiRcOS] menuprocess_func()\n"));

    switch ( *( int* )param )
    {
        case AiRcOS_MENUID_CONNECT:
        {
            set(AiRcOS_Base->aircos_quickconnectwin, MUIA_Window_Open, TRUE);
            break;
        }
        case AiRcOS_MENUID_QUIT:
        {
            set(AiRcOS_Base->aircos_clientwin, MUIA_Window_Open, FALSE);
            break;
        }
        case AiRcOS_MENUID_SERVERMANAGE:
        {
            set(serverconnectWin, MUIA_Window_Open, TRUE);
            break;
        }
        default:
            break;
    }
    AROS_USERFUNC_EXIT
}

//
/* ************** MAIN/NORMAL PROGRAM ENTRY POINT *************** */
//

int main(int argc, char **argv)
{
    IPTR                    argarray[TOTAL_ARGS] = { 0 };
    struct RDArgs           *args = NULL;
    
    BPTR                    lock = NULL;

//    struct IRC_Server_Priv  *irc_server_priv    = NULL;
//    struct IRC_Channel_Priv  *irc_channel_priv  = NULL;

    int                     error = RETURN_ERROR;
	 struct MUI_CustomClass *custom_mcc = NULL;

/**/
    if (!(AiRcOS_Base = AllocMem(sizeof(struct AiRcOS_internal), MEMF_PUBLIC|MEMF_CLEAR)))
    {
      return error;
    }
D(bug("[AiRcOS](main) Allocated App Internal Base.\n"));

    if (!(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 37)))
    {
        error = ERROR_INVALID_RESIDENT_LIBRARY;
        goto LibError;
    }

    Locale_Initialize();
D(bug("[AiRcOS](main) Localisation Initialised.\n"));

    MUIMasterBase = (struct Library*)OpenLibrary("muimaster.library",0);

    if ((custom_mcc = MUI_CreateCustomClass(NULL, "TextEditor.mcc", NULL, sizeof(struct CustomTEInstData), TextEditor_Dispatcher)))
    {
        AiRcOS_Base->editor_mcc = custom_mcc;
D(bug("[AiRcOS](main) Created TextEditor Custom Class @ %x\n", AiRcOS_Base->editor_mcc));

        aircosApp__OpenBSDSOCKET();
    
        args = ReadArgs( ARG_TEMPLATE, argarray, NULL);
    
        if(!(AiRcOS_Base->Ai_pool = CreatePool(MEMF_CLEAR,(256*1024),(16*1024))))
        {
D(bug("[AiRcOS](main) Failed to allocate pool!!\n"));
        }
    
        NewList((struct List *)&AiRcOS_Base->aircos_serverlist);
        NewList((struct List *)&AiRcOS_Base->aircos_looseconnectionlist);

        NewList((struct List *)&AiRcOS_Base->funcs_FunctionList);
        NewList((struct List *)&AiRcOS_Base->funcs_ActionList);

        aircos_IRCFuncs_RegisterFuncs();
//        aircos_DCCFuncs_RegisterFuncs(void)

        AiRcOS_Base->aircos_serv_no = 0;
        struct NewMenu AiRcOs_Menus[] =
        {
        {NM_TITLE, _(MENU_PROJECT)},
            {NM_ITEM, _(MENU_CONNECT), _(MENU_CONNECT_SC), 2, 0L, (APTR)AiRcOS_MENUID_CONNECT},
            {NM_ITEM, _(MENU_CLOSE), _(MENU_CLOSE_SC),     2, 0L, (APTR)AiRcOS_MENUID_CLOSE},
            {NM_ITEM, _(MENU_QUIT), _(MENU_QUIT_SC),       2, 0L, (APTR)AiRcOS_MENUID_QUIT},
        {NM_TITLE, _(MENU_SETTINGS)},
            {NM_ITEM, _(MENU_APP_OPTIONS), _(MENU_APP_OPTIONS_SC ),      2, 0L, (APTR)AiRcOS_MENUID_PREFS},
            {NM_ITEM, _(MENU_SERVER_MANAGER), _(MENU_SERVER_MANAGER_SC), 2, 0L, (APTR)AiRcOS_MENUID_SERVERMANAGE},
        {NM_TITLE, _(MENU_HELP)},
            {NM_ITEM, _(MENU_APP_HELP), _(MENU_APP_HELP_SC),   2, 0L, (APTR)AiRcOS_MENUID_HELP},
            {NM_ITEM, _(MENU_ABOUT), _(MENU_ABOUT_SC),         2, 0L, (APTR)AiRcOS_MENUID_ABOUT},
        {NM_END}
        };
    /*
    static struct NewMenu AiRcOs_Group_Opp_CMenu[] =
    {
        {NM_TITLE, "Project" },
            {NM_ITEM, "Connect ..",             "Ctrl C",       2, 0L, (APTR)AiRcOS_MENUID_CONNECT},
            {NM_ITEM, "Close connection",       "Ctrl X",       2, 0L, (APTR)AiRcOS_MENUID_CLOSE},
            {NM_ITEM, "Quit",                  "Ctrl Q",        2, 0L, (APTR)AiRcOS_MENUID_QUIT},
        {NM_TITLE, "Settings" },
            {NM_ITEM, "Application Options",    "Ctrl P",       2, 0L, (APTR)AiRcOS_MENUID_PREFS},
            {NM_ITEM, "Server Manager",    "Ctrl S",        2, 0L, (APTR)AiRcOS_MENUID_SERVERMANAGE},
        {NM_TITLE, "Help" },
            {NM_ITEM, "Application Help",      "Ctrl H",        2, 0L, (APTR)AiRcOS_MENUID_HELP},
            {NM_ITEM, "About AiRcOS",         "Ctrl A",         2, 0L, (APTR)AiRcOS_MENUID_ABOUT},
        {NM_END}
    };
    
    static struct NewMenu AiRcOs_Group_Voice_CMenu[] =
    {
        {NM_TITLE, "Project" },
            {NM_ITEM, "Connect ..",             "Ctrl C",       2, 0L, (APTR)AiRcOS_MENUID_CONNECT},
            {NM_ITEM, "Close connection",       "Ctrl X",       2, 0L, (APTR)AiRcOS_MENUID_CLOSE},
            {NM_ITEM, "Quit",                  "Ctrl Q",        2, 0L, (APTR)AiRcOS_MENUID_QUIT},
        {NM_TITLE, "Settings" },
            {NM_ITEM, "Application Options",    "Ctrl P",       2, 0L, (APTR)AiRcOS_MENUID_PREFS},
            {NM_ITEM, "Server Manager",    "Ctrl S",        2, 0L, (APTR)AiRcOS_MENUID_SERVERMANAGE},
        {NM_TITLE, "Help" },
            {NM_ITEM, "Application Help",      "Ctrl H",        2, 0L, (APTR)AiRcOS_MENUID_HELP},
            {NM_ITEM, "About AiRcOS",         "Ctrl A",         2, 0L, (APTR)AiRcOS_MENUID_ABOUT},
        {NM_END}
    };
    
    static struct NewMenu AiRcOs_Group_Normal_CMenu[] =
    {
        {NM_TITLE, "Project" },
            {NM_ITEM, "Connect ..",             "Ctrl C",       2, 0L, (APTR)AiRcOS_MENUID_CONNECT},
            {NM_ITEM, "Close connection",       "Ctrl X",       2, 0L, (APTR)AiRcOS_MENUID_CLOSE},
            {NM_ITEM, "Quit",                  "Ctrl Q",        2, 0L, (APTR)AiRcOS_MENUID_QUIT},
        {NM_TITLE, "Settings" },
            {NM_ITEM, "Application Options",    "Ctrl P",       2, 0L, (APTR)AiRcOS_MENUID_PREFS},
            {NM_ITEM, "Server Manager",    "Ctrl S",        2, 0L, (APTR)AiRcOS_MENUID_SERVERMANAGE},
        {NM_TITLE, "Help" },
            {NM_ITEM, "Application Help",      "Ctrl H",        2, 0L, (APTR)AiRcOS_MENUID_HELP},
            {NM_ITEM, "About AiRcOS",         "Ctrl A",         2, 0L, (APTR)AiRcOS_MENUID_ABOUT},
        {NM_END}
    };
    */
    /* Get the speech icon image */
        
        lock = NULL;
        if ((lock = Lock(SPEEKTYPE_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_speekimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_speekimg = FALSE;
        
    /* Get the user icon image */
        
        lock = NULL;
        if ((lock = Lock(USERTYPE_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_userimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_userimg = FALSE;
    
    /* Get the font etc icons */
        
        lock = NULL;
        if ((lock = Lock(FONT_BACK_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontbaimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontbaimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_BOLD_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontboimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontboimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_COLOR_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontcoimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontcoimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_DEF_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontdeimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontdeimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_ITAL_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontitimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontitimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_LARGE_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontlaimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontlaimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_SMALL_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontsmimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontsmimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(FONT_UNDER_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_fontunimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_fontunimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(POP_PIC_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_poppicimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_poppicimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(POP_SMILEY_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_popsmiimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_popsmiimg = FALSE;
        
        lock = NULL;
        if ((lock = Lock(POP_URL_IMAGE, ACCESS_READ)) != NULL)
        {
            AiRcOS_Base->aircos_got_popurlimg = TRUE;
    
            UnLock(lock);
        }
        else AiRcOS_Base->aircos_got_popurlimg = FALSE;
D(bug("[AiRcOS](main) Base gadget Images locked\n"));
    /**/
    
        AiRcOS_Base->aircos_app = ApplicationObject,
            MUIA_Application_Title,       __(MSG_TITLE),
            MUIA_Application_Version,     (IPTR) "$VER: AiRcOS 0.6 (30.04.06) ©AROS Dev Team",
            MUIA_Application_Copyright,   (IPTR) "Copyright © 2005-2006, The AROS Development Team. All rights reserved.",
            MUIA_Application_Author,      (IPTR) "Nick Andrews",
            MUIA_Application_Description, __(MSG_DESCRIPTION),
            MUIA_Application_Base,        (IPTR) "AIRCOS",
    
            SubWindow, (IPTR) (AiRcOS_Base->aircos_clientwin = WindowObject,
                MUIA_Window_Menustrip,(IPTR) ( AiRcOS_Base->aircos_clientwin_menu = MUI_MakeObject(MUIO_MenustripNM,
                    (IPTR)AiRcOs_Menus, (IPTR)NULL)),
    
                MUIA_Window_Title, __(MSG_WIN_TITLE),
                MUIA_Window_Activate, TRUE,
                MUIA_Window_Width,600,
                MUIA_Window_Height,400,
         
                WindowContents, (IPTR) HGroup,
                    Child, (IPTR) (AiRcOS_Base->aircos_window_content = VGroup,
                        Child, (IPTR) (AiRcOS_Base->aircos_window_page = VGroup,
                            InputListFrame,
                            MUIA_Background, MUII_HSHADOWSHADOW,
                            Child, RectangleObject,
                            End,
                        End),
                    End),
                End,
            End),
    
            SubWindow, (IPTR) (AiRcOS_Base->aircos_quickconnectwin = WindowObject,
                MUIA_Window_Title, _(MSG_SERVER_CONNECT),
                MUIA_Window_Activate, TRUE,
         
                WindowContents, (IPTR) VGroup,
                    Child, (IPTR) VGroup, 
                        GroupFrame,
                        Child, (IPTR) ColGroup(2),
                            Child, (IPTR) LLabel(_(MSG_SERVER_ADDRESS)),
                                Child, (IPTR) (AiRcOS_Base->quickcon_servadd = StringObject,
                                    StringFrame,
                                    MUIA_CycleChain, 1,
                                        MUIA_String_Format, MUIV_String_Format_Right,
                                End ),
                                Child, (IPTR) LLabel(_(MSG_SERVER_PORT)),
                                Child, (IPTR) (AiRcOS_Base->quickcon_servport = StringObject,
                                    StringFrame,
                                    MUIA_CycleChain, 1,
                                        MUIA_String_Format, MUIV_String_Format_Right,
                                        MUIA_String_Accept, "0123456789",
                                        MUIA_String_Integer, 0,
                                End ),
                                Child, (IPTR) LLabel(_(MSG_USERNAME)),
                                Child, (IPTR) (AiRcOS_Base->quickcon_servuser = StringObject,
                                    StringFrame,
                                    MUIA_CycleChain, 1,
                                        MUIA_String_Format, MUIV_String_Format_Left,
                                    End ),
                                Child, (IPTR) LLabel(_(MSG_PASSWORD)),
                                Child, (IPTR) (AiRcOS_Base->quickcon_servpass = StringObject,
                                    StringFrame,
                                    MUIA_CycleChain, 1,
                                        MUIA_String_Format, MUIV_String_Format_Left,
                                        MUIA_String_Secret, TRUE,
                                    End ),
                        End,
                    End,
                    Child, (IPTR) VGroup,
                        GroupFrame,
                        Child, (IPTR) ColGroup(2),
                                Child, (IPTR) LLabel(_(MSG_NICKNAME)),
                                Child, (IPTR) (AiRcOS_Base->quickcon_nick = StringObject,
                                    StringFrame,
                                    MUIA_CycleChain, 1,
                                        MUIA_String_Format, MUIV_String_Format_Left,
                                    End ),
                                Child, (IPTR) LLabel(_(MSG_JOIN)),
                                Child, (IPTR) (AiRcOS_Base->quickcon_channel = StringObject,
                                    StringFrame,
                                    MUIA_CycleChain, 1,
                                        MUIA_String_Format, MUIV_String_Format_Left,
                                    End ),
                        End,
                    End,
                    Child, (IPTR) (AiRcOS_Base->butt_connectServer = SimpleButton(_(MSG_CONNECT))),
                End,
            End),
        End;

       if (AiRcOS_Base->aircos_app)
       {

D(bug("[AiRcOS](main) Zune Application Objects Created\n"));
    
         AiRcOS_Base->aircos_menuhook.h_MinNode.mln_Succ = NULL;
         AiRcOS_Base->aircos_menuhook.h_MinNode.mln_Pred = NULL;
         AiRcOS_Base->aircos_menuhook.h_Entry = HookEntry;
         AiRcOS_Base->aircos_menuhook.h_SubEntry = (void *)menuprocess_func;
    
         DoMethod (
            AiRcOS_Base->aircos_clientwin, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
            ( IPTR ) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, ( IPTR )&AiRcOS_Base->aircos_menuhook, MUIV_EveryTime
         );
    
         set( (IPTR)AiRcOS_Base->butt_connectServer, MUIA_CycleChain, 1);
    
         set( (IPTR)AiRcOS_Base->quickcon_servadd, MUIA_String_Contents, _(MSG_DEFAULT_SERVER));
         set( (IPTR)AiRcOS_Base->quickcon_servport, MUIA_String_Integer, 6665);
         set( (IPTR)AiRcOS_Base->quickcon_servpass, MUIA_String_Contents,"");
         set( (IPTR)AiRcOS_Base->quickcon_channel, MUIA_String_Contents, _(MSG_DEFAULT_CHAN));
    
    
D(bug("[AiRcOS] prepare connect hook\n"));
    
         AiRcOS_Base->aircos_connect_hook.h_MinNode.mln_Succ = NULL;
         AiRcOS_Base->aircos_connect_hook.h_MinNode.mln_Pred = NULL;
         AiRcOS_Base->aircos_connect_hook.h_Entry = HookEntry;
         AiRcOS_Base->aircos_connect_hook.h_SubEntry = (void *)serverconnect_func;
    
         DoMethod
         (
            AiRcOS_Base->aircos_clientwin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (IPTR) AiRcOS_Base->aircos_app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
         );
    
         DoMethod
         (
            AiRcOS_Base->butt_connectServer, MUIM_Notify, MUIA_Pressed, FALSE,
                (IPTR) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, &AiRcOS_Base->aircos_connect_hook, NULL
         );
    
      /* ********** WINDOW KEY NOTIFIES ********** */
    
         DoMethod
         (
                AiRcOS_Base->aircos_clientwin, MUIM_Notify, MUIA_Window_InputEvent, "f2",
                AiRcOS_Base->aircos_quickconnectwin, 3, MUIM_Set, MUIA_Window_Open, TRUE
         );
    
          set((IPTR)AiRcOS_Base->aircos_clientwin, MUIA_Window_Open, TRUE);

          serverconnectWin = aircos_showServerConnect(); 
          aircosApp_loadPrefs();

          set((IPTR)AiRcOS_Base->quickcon_servuser, MUIA_String_Contents, aircos_Prefs_DefUser);
          set((IPTR)AiRcOS_Base->quickcon_nick, MUIA_String_Contents, aircos_Prefs_DefNick);
          set((IPTR)AiRcOS_Base->aircos_quickconnectwin, MUIA_Window_Open, TRUE);

          aircosApp_MainProcessLoop();
        }
        else
        {
            printf("Failed to intialize Zune GUI\n");
        }
    
    /** CLOSE ALL OPEN SERVER CONNECTIONS! **/
D(bug("[AiRcOS](main) Closing Active connections\n"));
       struct IRC_Server_Priv *current_Server=NULL;
       ForeachNode(&AiRcOS_Base->aircos_serverlist, current_Server)
       {
          if ((SocketBase)&&(current_Server->serv_connection))
          {
             Remove(&current_Server->serv_connection->connection_node);
             Remove(&current_Server->serv_node);
             aircosApp_closeConnection(current_Server->serv_connection);
          }
       
       }
    
    /** CLOSE ALL LOOSE CONNECTIONS! **/
D(bug("[AiRcOS](main) Closing Loose connections\n"));
       struct IRC_Connection_Private	*current_Connection=NULL;
       ForeachNode(&AiRcOS_Base->aircos_looseconnectionlist, current_Connection)
       {
             Remove(&current_Connection->connection_node);
             aircosApp_closeConnection(current_Connection);
       }
D(bug("[AiRcOS](main) Application finished ..\n"));
   }
   else
   {
D(bug("[AiRcOS](main) Failed to create TextEditor Custom Class!!\n"));
   }

    if (args) FreeArgs(args);

    Locale_Deinitialize();

LibError:
    if (SocketBase) CloseLibrary((struct Library *)SocketBase);

    if (MUIMasterBase) CloseLibrary((struct Library *)MUIMasterBase);

    if (DOSBase) CloseLibrary((struct Library *)DOSBase);

    return(error);
}

