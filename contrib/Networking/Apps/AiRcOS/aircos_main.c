/*
    Copyright © 2002, The AROS Development Team.
    All rights reserved.

    $Id: aircos_main.c,v 1.1 2005/01/04 03:29:51 nicja Exp $
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

#define AiRcOS_MENUID_CONNECT          (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_CLOSE            (AiRcOS_MENUID + 2)
#define AiRcOS_MENUID_QUIT             (AiRcOS_MENUID + 3)
#define AiRcOS_MENUID_PREFS            (AiRcOS_MENUID + 4)
#define AiRcOS_MENUID_SERVERMANAGE     (AiRcOS_MENUID + 1)
#define AiRcOS_MENUID_HELP             (AiRcOS_MENUID + 5)
#define AiRcOS_MENUID_ABOUT            (AiRcOS_MENUID + 6)

extern int aircos_IRC_donumeric(struct IRC_Connection_Private	*currentConnection, int num);
extern int aircos_IRC_nop(struct IRC_Connection_Private	*currentConnection);
extern struct IRC_Server_Priv  *aircos_add_server(char *addserv);
extern struct irccommand commandList_array[];

struct AiRcOS_internal *AiRcOS_Base;

BOOL aircos_OpenBSDSOCKET()
{
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

int aircos_sendline(struct IRC_Connection_Private *forConnection)
{
    int count;

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
D(bug("[AiRcOS] FindNamedChannel('%s')\n",findThisChan));

   ForeachNode(&onThisConnection->connected_server->serv_chans, foundChannel)
   {
D(bug("[AiRcOS](FindNamedChannel) Checking against record for '%s'\n",foundChannel->chan_name));
      if (strcasecmp(foundChannel->chan_name,findThisChan)==0) return foundChannel;
   }
   return NULL;
}

void aircos_showChanOutput(struct IRC_Channel_Priv *thisChanPriv, struct serv_Outline *sa)
{
   if ( thisChanPriv->chan_displayedlines >= CHANOUT_MAXLINES)
   {
D(bug("[AiRcOS](showChanOutput) Freeing text at head of output ... \n"));
   }

   //if (aircos_PrefsStruct->prefs_LogDir)
   //{
      // Output Current line to log file for this channel... 
   // 
   //}

   //And finally output the line ...
   DoMethod( thisChanPriv->chan_output, MUIM_NList_InsertSingle, sa->so_name, MUIV_NList_Insert_Bottom );
   DoMethod( thisChanPriv->chan_output, MUIM_NList_Jump, MUIV_List_Jump_Bottom);
   thisChanPriv->chan_displayedlines++;
}



int aircos_serverconnect(struct IRC_Connection_Private *makeThisConnection)
{
D(bug("[AiRcOS] serverconnect(%s)\n",makeThisConnection->connection_server));

    struct sockaddr_in sa;
    struct hostent *hp=NULL;
    int t=0;

    if (!(SocketBase))
    {
        if (!(aircos_OpenBSDSOCKET()))
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
                aircos_sendline(makeThisConnection);
D(bug("[AiRcOS](serverconnect) Sent USER NAME ..\n"));
                if (makeThisConnection->connection_pass != "")
                {
                    sprintf(makeThisConnection->connection_buff_send, "PASS :%s\n", makeThisConnection->connection_pass);
                    aircos_sendline(makeThisConnection);
D(bug("[AiRcOS](serverconnect) Sent PASSWORD ..\n"));
                }

                sprintf(makeThisConnection->connection_buff_send, "NICK :%s\n", makeThisConnection->connection_nick);
                aircos_sendline(makeThisConnection);
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

int aircos_processServerData(struct IRC_Connection_Private	*process_thisConnection, char *serverin_buffer)
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
        return aircos_sendline(process_thisConnection);
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

    if (!(dtest_connection = AllocVec(sizeof(struct IRC_Connection_Private),MEMF_CLEAR)))
    {
D(bug("[AiRcOS](serverconnect_func) Failed to allocate connection record!!\n"));
        return;
    }

   set(AiRcOS_Base->butt_connectServer,MUIA_Disabled,TRUE);

   char  *join_channel=NULL;
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
      return;
   }

   strcpy(dtest_connection->connection_nick, str_connect_nick);

   get( AiRcOS_Base->quickcon_channel, MUIA_String_Contents, &join_channel);

   dtest_connection->connection_port = (UWORD)temp_servport;
D(bug("[AiRcOS](serverconnect_func) Attempting to connect to %s:%d, as %s\n", dtest_connection->connection_server, temp_servport, dtest_connection->connection_user));

D(bug("[AiRcOS](serverconnect_func) Port : %d, Nick %s\n", dtest_connection->connection_port,dtest_connection->connection_nick));

    int test_socket = aircos_serverconnect(dtest_connection);
    if (test_socket >= 0)
    {
        struct IRC_Server_Priv  *connected_server = NULL;
      
        if (!(connected_server = aircos_add_server(dtest_connection->connection_server)))
        {
D(bug("[AiRcOS](serverconnect_func) Failed to create connection page!!.\n"));
            return;
        }

        connected_server->serv_connection = dtest_connection;
        dtest_connection->connected_server = connected_server;

        set(AiRcOS_Base->aircos_quickconnectwin,MUIA_Window_Open,FALSE);
      
D(bug("[AiRcOS](serverconnect_func) Socket connection successfull\n"));
        if (join_channel != "")
        {
D(bug("[AiRcOS](serverconnect_func) Attempting to join channel %s ..\n", join_channel));

            sprintf(dtest_connection->connection_buff_send, "JOIN %s\n", join_channel);
            aircos_sendline(dtest_connection);
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
    }

    if (dtest_connection)
    {
        AddTail((struct List *)&AiRcOS_Base->aircos_looseconnectionlist,(struct Node *) &dtest_connection->connection_node);
    }
// AiRcOS_Base->temp_process_thisConnection = dtest_connection;   set(AiRcOS_Base->butt_connectServer,MUIA_Disabled,FALSE);

    AROS_USERFUNC_EXIT
};


void aircos_closeConnection(struct IRC_Connection_Private	*close_thisConnection)
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

void  aircos_MainProcessLoop()
{
    ULONG sigs = 0;

    D(bug("[AiRcOS] aircos_MainProcessLoop()\n"));
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
                                            if (!aircos_processServerData(process_thisConnection, tmpbuff))
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
                                        if (tmpunprocbuff)
                                        {
                                            process_thisConnection->connection_rawdata_length =
                                            (IPTR)&process_thisConnection->connection_buff_recieve[count] - (IPTR)tmpunprocbuff;
                                            process_thisConnection->connection_rawdata =
                                            AllocVec(process_thisConnection->connection_rawdata_length, MEMF_CLEAR);
                                            CopyMem(tmpunprocbuff, process_thisConnection->connection_rawdata,
                                            process_thisConnection->connection_rawdata_length);
                                        }
                                    }
                                    else
                                    {
                                        if (process_thisConnection->connection_rawdata)
                                            FreeVec(process_thisConnection->connection_rawdata);
                                        process_thisConnection->connection_rawdata = NULL;
                                        process_thisConnection->connection_rawdata_length = 0;
                                    }
                                }
                                FreeVec(tmpbuff);
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
            set(AiRcOS_Base->aircos_quickconnectwin,MUIA_Window_Open,TRUE);
            break;

        case AiRcOS_MENUID_QUIT:
            set(AiRcOS_Base->aircos_clientwin, MUIA_Window_Open, FALSE);
            break;

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
    IPTR                    argarray[TOTAL_ARGS] = {0};
    struct RDArgs           *args = NULL;
    
    BPTR                    lock = NULL;

//    struct IRC_Server_Priv  *irc_server_priv    = NULL;
//    struct IRC_Channel_Priv  *irc_channel_priv  = NULL;

    int                     error = RETURN_ERROR;
/**/
    if (!(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 37)))
    {
        error = ERROR_INVALID_RESIDENT_LIBRARY;
        goto LibError;
    }

    Locale_Initialize();

    if (!(AiRcOS_Base = AllocMem(sizeof(struct AiRcOS_internal), MEMF_PUBLIC|MEMF_CLEAR)))
    {
      return error;
    }

    aircos_OpenBSDSOCKET();

    args = ReadArgs( ARG_TEMPLATE, argarray, NULL);

    MUIMasterBase = (struct Library*)OpenLibrary("muimaster.library",0);
    if(!(AiRcOS_Base->Ai_pool = CreatePool(MEMF_CLEAR,(256*1024),(16*1024))))
    {
D(bug("[AiRcOS](main) Failed to allocate pool!!\n"));
    }

    NewList((struct List *)&AiRcOS_Base->aircos_serverlist);
    NewList((struct List *)&AiRcOS_Base->aircos_looseconnectionlist);

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
    
/**/

    AiRcOS_Base->aircos_app = ApplicationObject,
        MUIA_Application_Title,       __(MSG_TITLE),
        MUIA_Application_Version,     (IPTR) "$VER: AiRcOS 0.3 (11.12.05) ©AROS Dev Team",
        MUIA_Application_Copyright,   (IPTR) "Copyright © 2005, The AROS Development Team. All rights reserved.",
        MUIA_Application_Author,      (IPTR) "Nic Andrews",
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

   AiRcOS_Base->aircos_menuhook.h_MinNode.mln_Succ = NULL;
   AiRcOS_Base->aircos_menuhook.h_MinNode.mln_Pred = NULL;
   AiRcOS_Base->aircos_menuhook.h_Entry = HookEntry;
   AiRcOS_Base->aircos_menuhook.h_SubEntry = (void *)menuprocess_func;

    DoMethod (
        AiRcOS_Base->aircos_clientwin, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
        ( IPTR ) AiRcOS_Base->aircos_app, 3, MUIM_CallHook, ( IPTR )&AiRcOS_Base->aircos_menuhook, MUIV_EveryTime
    );

   set(AiRcOS_Base->butt_connectServer, MUIA_CycleChain, 1);

   set( AiRcOS_Base->quickcon_servadd, MUIA_String_Contents, _(MSG_DEFAULT_SERVER));
   set( AiRcOS_Base->quickcon_servport, MUIA_String_Integer, 6665);
   set( AiRcOS_Base->quickcon_servuser, MUIA_String_Contents, _(MSG_DEFAULT_USERNAME));
   set( AiRcOS_Base->quickcon_servpass, MUIA_String_Contents,"");
   set( AiRcOS_Base->quickcon_nick, MUIA_String_Contents, _(MSG_DEFAULT_NICKNAME));
   set( AiRcOS_Base->quickcon_channel, MUIA_String_Contents, _(MSG_DEFAULT_CHAN));


D(bug("[AiRcOS] prepare connect hook\n"));

   AiRcOS_Base->aircos_connect_hook.h_MinNode.mln_Succ = NULL;
   AiRcOS_Base->aircos_connect_hook.h_MinNode.mln_Pred = NULL;
   AiRcOS_Base->aircos_connect_hook.h_Entry = HookEntry;
   AiRcOS_Base->aircos_connect_hook.h_SubEntry = (void *)serverconnect_func;
//   aircos_connect_hook.h_Data = ;

   
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

    set(AiRcOS_Base->aircos_clientwin,MUIA_Window_Open,TRUE);
    set(AiRcOS_Base->aircos_quickconnectwin,MUIA_Window_Open,TRUE);

      aircos_MainProcessLoop();
    }
    else
    {
        printf("Failed to intialize Zune GUI\n");
    }

/** CLOSE ALL OPEN SERVER CONNECTIONS! **/

   struct IRC_Server_Priv *current_Server=NULL;
   ForeachNode(&AiRcOS_Base->aircos_serverlist, current_Server)
   {
      if ((SocketBase)&&(current_Server->serv_connection))
      {
         aircos_closeConnection(current_Server->serv_connection);
      }
   
   }

/** CLOSE ALL LOOSE CONNECTIONS! **/
   struct IRC_Connection_Private	*current_Connection=NULL;
   ForeachNode(&AiRcOS_Base->aircos_looseconnectionlist, current_Connection)
   {
         aircos_closeConnection(current_Connection);
   }

    if (args) FreeArgs(args);

    Locale_Deinitialize();

LibError:
    if (SocketBase) CloseLibrary((struct Library *)SocketBase);

    if (MUIMasterBase) CloseLibrary((struct Library *)MUIMasterBase);

    if (DOSBase) CloseLibrary((struct Library *)DOSBase);

    return(error);
}

