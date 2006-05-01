/*
    Copyright © 2005, The AROS Development Team.
    All rights reserved.

    $Id$
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

#define        AIRCOS_MAXSERVERRESPONSE   4096
#define        SERVOUT_MAXLINES           150
#define        CHANOUT_MAXLINES           150

/* End Network Includes */

#define DEBUG 0
#include <aros/debug.h>

#define isIRCChannel(x) (*x == '#' || *x == '&' || *x == '+')

#define USERTYPE_IMAGE          "IMAGES:Gadgets/System/sys_user"
#define SPEEKTYPE_IMAGE         "ProgDir:Icons/img_speek"

#define FONT_BACK_IMAGE         "ProgDir:Icons/font_back"
#define FONT_BOLD_IMAGE         "ProgDir:Icons/font_bold"
#define FONT_COLOR_IMAGE        "ProgDir:Icons/font_color"
#define FONT_DEF_IMAGE          "ProgDir:Icons/font_def"
#define FONT_ITAL_IMAGE         "ProgDir:Icons/font_ital"
#define FONT_LARGE_IMAGE        "ProgDir:Icons/font_large"
#define FONT_SMALL_IMAGE        "ProgDir:Icons/font_small"
#define FONT_UNDER_IMAGE        "ProgDir:Icons/font_under"
#define POP_PIC_IMAGE           "ProgDir:Icons/pop_pic"
#define POP_SMILEY_IMAGE        "ProgDir:Icons/pop_smiley"
#define POP_URL_IMAGE           "ProgDir:Icons/pop_url"

#define PAGE_BUT_BACK        	"2:66666666,77777777,88888888"

#define ARG_TEMPLATE	"LOCAL/S"
#define ARG_LOCAL	0
#define TOTAL_ARGS	1

/* DATA : SERVER */

#define MAX_SERVER_ARGS 16
#define MAX_SEND_ARGS 5

struct IRC_Connection_Private
{
        struct Node  connection_node;
		char	*connection_server;
		UWORD	connection_port;
		int		connection_socket;
		char	*connection_user;
		char	*connection_pass;

		char	*connection_nick;
      /* TCP BUFFERS */
		char	*connection_buff_send;
		char	*connection_buff_recieve;

      char  *connection_rawdata;		
      ULONG  connection_rawdata_length;
      /* IRC PROCESSING */
      char  *connection_unprocessed;

      struct IRC_Server_Priv *connected_server;
		char *connection_serv_ARGS[MAX_SERVER_ARGS+1]; /* Pointers to MAX_SERVER_ARGS server arguments recieved from server */

};

struct serv_Outline
{
   struct Node                     so_node;
	char                            *so_name;
	UWORD                           flags;
   struct MUI_NList_TreeNode       *so_displaynode;
};

struct IRC_Server_Priv
{
        struct Node     serv_node;
        char            *serv_name;
        struct List     serv_chans;
		  IPTR			   serv_pageid;
        IPTR            serv_chan_no;   /* no of channels 'connected' */
        Object          *serv_page;
        Object          *serv_page_reg_grp;
        Object          *serv_page_reg_spcr;
        Object          *serv_pagemd_grp;
        Object          *serv_statuspage;
        Object          *serv_status_output;
        Object          *serv_message;
        Object          *serv_send;
        ULONG           serv_displayedlines;

		struct IRC_Connection_Private	*serv_connection;
};

/* DATA : CHANNEL */

struct IRC_Channel_Priv
{
        struct Node             chan_node;
        struct IRC_Server_Priv  *chan_serv;
		  IPTR					     chan_pageid;
        char                    *chan_name;

		  ULONG                    chan_currentpen;

        struct List             chan_usergroup;     // List of Users in group
        struct List             chan_outitems;
        
        Object                  *chan_page;
        char                    *chan_topic;
        Object                  *chan_topic_obj;        
        Object                  *chan_output_grp;
        Object                  *chan_output_grp_txt;        
        Object                  *chan_output;
        ULONG                   chan_displayedlines;
        Object                  *chan_users_grp;
        Object                  *chan_users;
        Object                  *chan_message;
        Object                  *chan_send;
        
        char                     *chan_send_ARGS[MAX_SEND_ARGS+1];
        
        struct Hook             chan_hook;
};

/* DATA : USERS / GROUPS */

#define CHANNEL_GROUPT_SUBGROUP   0x01
#define CHANNEL_GROUPT_USER       0x10

struct IRC_Channel_Group_SubGroup
{
        struct Node                     group_node;
        IPTR                            group_type;         /* Will be set to CHANNEL_GROUPT_SUBGROUP for subgroup */
        char                            *group_name;        /* Plain name */
        char                            *group_displayname; /* Name formatted for display */
        struct List                     group_usergroup;    /* List of Users in group */
        struct MUI_NListtree_TreeNode   *group_userobj;     /* Zune Object representing this group */
};

struct IRC_Channel_Group_User
{
        struct Node                     group_node;
        IPTR                            group_type;         /* Will be set to CHANNEL_GROUPT_SUBGROUP for subgroup */
        char                            *user_name;         /* Plain name */
        char                            *user_displayname;  /* Name formatted for display */
        struct MUI_NListtree_TreeNode   *user_obj;          /* Zune Object representing this user */
};

struct AiRcOS_internal
{
   struct Library *Ai_SocketBase;
   struct Library *Ai_MUIMasterBase;
   APTR            Ai_pool;            /* AiRcOS Memory Pool */

   int				Ai_errno;
   int				Ai_herrno;
   ULONG          Ai_sigbit_ProcessStack; /* Sigbit used to indicate stack activity */

   fd_set			Ai_readfs;

   char           *Ai_tmp;
   int            Ai_reconnect;

/* Internal Command Lists */
   struct List    funcs_FunctionList;
   struct List    funcs_ActionList;

/* Zune GUI elements */

   struct MUI_CustomClass *editor_mcc;
   LONG            editor_cmap[20];
   
   Object          *aircos_app;                    /* Main Application Object */
   Object          *aircos_clientwin;
   Object          *aircos_clientwin_menu;
   Object          *aircos_window_content;         /* Main Group holding   All our "bits" */
   Object          *aircos_window_page;
   Object          *aircos_window_pagemd_grp;      /* Main Group holding All our "bits" */
   Object          *aircos_window_pagemd_reg_grp;
   Object          *aircos_window_pagemd_reg_spcr;
  
   Object          *aircos_quickconnectwin;
   Object          *quickcon_servadd;
   Object          *quickcon_servport;
   Object          *quickcon_servuser;
   Object          *quickcon_servpass;
   Object          *quickcon_nick;
   Object          *quickcon_channel;
   Object          *butt_connectServer;

   struct List     aircos_serverlist;
   struct List     aircos_looseconnectionlist;     /* Connections "queued" here till joined to a server record. */

   struct IRC_Connection_Private	*temp_process_thisConnection;

   IPTR            aircos_serv_no;                 /* no of servers 'connected' */
   struct Hook     aircos_connect_hook;
   struct Hook     aircos_menuhook;

   BOOL            aircos_got_userimg;
   BOOL            aircos_got_speekimg;
   BOOL            aircos_got_fontbaimg;
   BOOL            aircos_got_fontboimg;
   BOOL            aircos_got_fontcoimg;
   BOOL            aircos_got_fontdeimg;
   BOOL            aircos_got_fontitimg;
   BOOL            aircos_got_fontlaimg;
   BOOL            aircos_got_fontsmimg;
   BOOL            aircos_got_fontunimg;
   BOOL            aircos_got_poppicimg;
   BOOL            aircos_got_popsmiimg;
   BOOL            aircos_got_popurlimg;
};

#define SocketBase AiRcOS_Base->Ai_SocketBase
#define MUIMasterBase AiRcOS_Base->Ai_MUIMasterBase

#define AiRcOS_MENUID (0x50505000)

/* Definitions for internal prefs parsed networks/servers */
struct aircos_networknode
{
  struct Node ann_Node;
  char        *ann_Network;
  ULONG       ann_ServerCount;
};

struct aircos_servernode
{
  struct Node               asn_Node;
  struct aircos_networknode *asn_Network;
  char                      *asn_Server;
  ULONG                     asn_Port;
  char                      *asn_User;
  char                      *asn_Nick;
  char                      *asn_Pass;
  char                      *asn_Description;
};

/* Function/Action records for internal command processing */
struct functionrecord
{
   char     *command;                                                   /* Commands textual name */
// ULONG    command_id;                                                 /* Commands numerical ID */
/* Function to perform for recieved server command */
   int      (*command_doFunction)(struct IRC_Connection_Private *);
   int      servArgCount;                                                   /* No. of args used by command */
/* Function to perform to send client command */
   int      (*command_clientFunction)(struct IRC_Channel_Priv *);
   int      clientArgCount;                                                   /* No. of args used by command */
};

