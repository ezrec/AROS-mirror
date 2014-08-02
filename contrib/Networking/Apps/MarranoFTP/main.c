/*
    Copyright © 2009, The AROS Development Team.
    All rights reserved.

    $Id$
*/

/*  

    LuKeJerry Edit - 14 June 2010:
    - Removed usage of MARRANO: assign, now prefs file is saved in the
      same folder where MarranoFTP is launched (PROGDIR:) with filename "marranoftp.prefs" 
    
    - Changed button label "Change" to "Add/Update" and removed button "SAVE" from Address book window
    - Changed code to save configuration file on exit from Address book window 
    - Reduced main window size to not use 100% of screen
    - Changed button labels Rename/Delete/Makedir to reflect the remote actions: 
      "Remote Delete" - "Remote Rename" - "Remote Makedir" 
*/     


/* MarranoFTP - written by Stefano Crosara */
/* This code is released under APL         */

/* Generic Amiga includes */
// #define MUIMASTER_YES_INLINE_STDARG 1
#include <exec/types.h>
#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/datetime.h>
#include <dos/dosextens.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <libraries/gadtools.h>
#include <stdio.h>
#include <netdb.h>

#include <utility/hooks.h>
#include <errno.h>

#ifdef __AROS__
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/socket.h>
#include <proto/iffparse.h>
#include <proto/timer.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#endif

#ifdef __MORPHOS__
#include <ppcinline/exec.h>
#include <ppcinline/dos.h>
#include <ppcinline/intuition.h>
#include <ppcinline/socket.h>
#include <ppcinline/muimaster.h>
#include <ppcinline/timer.h>
#include <ppcinline/iffparse.h>
#include <net/socketbasetags.h>
#include <clib/alib_protos.h>
#endif

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

// Caf includes
#include <caf/caf_types.h>
#include <caf/caf_debugging.h>
#include <caf/networking_helpers.h>
#include <caf/thread_helpers.h>
#include <caf/errors.h>

// STDLIB
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Program includes
#include "structs.h"

/*			TODO LIST

 - Quando si fa new nella Address Book e poi Add/Update per inserire i dati, 
   la selezione non si sposta sulla nuova entry dell'address book. 
 - gestire il List quando i dati ricevuti sono + grandi del buffer di ricezione
 - gestione interattiva del logon, in caso di fallimento
 - Clear del listbox status? prende memoria?
 - Levare gli asd
 - Implementare TFTP per QEMU - http://en.wikipedia.org/wiki/TFTP
 - Implementare il PASV
 - Implementare l'upload, con resume
 - Implementare il download, con resume
 - Implementare una porta arexx
 - Implementare l'FXP
 - Aggiungere il resume sul download
 - Implementare il rename
 - Listbox selezionabili così posso togliere doppi button per le due differenti listview
 - Mettere il sort da colonna nelle NListe
 - Criptare le password
                   
 -  Ottimizzare il download, Write() viene chiamata ad ogni evento socket, mettere in un buffer
        i dati e scriverli dopo
-  <AlieM> devi aggiungere a mFTP anche un controllo sullo spazio disponibile, così (se già lo sai) evita 
   di scaricare file a metà

        DONE
 
 -  > Rimosso il bottone "SAVE" <
    (Quando si fa new nella Address Book e poi save nei textbox rimane il nuovo entry creato
    ma nella selezione rimane selezionato un'altro entry, ripremendo save si salva le nuove
    informazioni sul vecchio entry)
 - Se l'utente preme il bottone di chiusura mftp non salva la config 
 - Implementare l'upload
 - Implementare "Global Settings"
 - gestire tutti i comandi di errore in risposta (generico)
 - continuare con il tipo di gestione ftp a stack
 - implementare una coda di comandi da lanciare, per prendere ad es. una directory intera
 - aggiungere la addressbook
 - Attivare le frecce sui LB? - NLIST auto.
 - salvare i file nel listview a sinistra
 - fare il parsing dei dati dell'ftp per il nome del file
 - Mettere i listbox multicolumn
 - same, all zune apps are exchange commodities. always give a regular version tag, 
   a description and a basename
        
 ************* INVERTIRE L'ORDINE DI PROCESSING DELLA QUEUE ************* 
        
        Per la ricorsione
                
           loop:
           aggiungere tutte le directory allo stack e alla coda (mkdir)
           aggiungere tutti i file alla coda
           processare la prima directory che si trova nello stack stack[stack_cur]
           goto loop

[00:23] <olivier2222> MUIA_Application_Version, (IPTR) "$VER: MyApp 0.1 (dd.mm.yyyy) © AROS Dev Team"
[00:24] <olivier2222> MUIA_Application_Description,  __(MSG_DESCRIPTION),
[00:24] <olivier2222> with __() the macro for localization, in this ex
[00:24] <olivier2222> because you want the exchange application description field to be localized
[00:25] <olivier2222> MUIA_Application_Base, (IPTR) "MYAPP"
[00:25] <olivier2222> will give a base name for the zune prefs saving
[00:25] <olivier2222> MYAPP.1.prefs
[00:25] <olivier2222> etc
[00:26] <olivier2222> the rexx port could be named after the basename, the temp files, etc

*/

// #define DEBUG
// #define DEBUGLV2
// #define DEBUGLV3
// #define NO_SOCKETLIB
char g_config_filename[512];

enum 
{
    /* Menu Ids */
    MNID_OPENCONFIG = 1,
    MNID_SAVECONFIG,
    MNID_EXPORTCONFIG,
    MNID_IMPORTCONFIG,
    MNID_ABOUT,
    MNID_QUIT,
    MNID_GLOBALSETTINGS,
    MNID_OPENADDRESSBOOK,

    /* Main Window */
    ID_CLICKED_CONNDISC, 
    ID_CLICKED_REFRESH_L, ID_CLICKED_REFRESH_R, 
    ID_CLICKED_STOP, ID_CLICKED_UPLOAD, 
    ID_CLICKED_DELETE, ID_CLICKED_RENAME, ID_CLICKED_MAKEDIR, 
    ID_CLICKED_QUEUE_PROCESS, ID_CLICKED_QUEUE_STEP, ID_CLICKED_DOWNLOAD,
    ID_CLICKED_CLEAR_QUEUE,
    ID_MODIFIED_LPATHBOX, ID_DBLCLICKED_LEFT_LISTVIEW,
    ID_MODIFIED_RPATHBOX, ID_DBLCLICKED_RIGHT_LISTVIEW,
    ID_CLICKED_QUIT, ID_CLICKED_DUMP,

    /* Address Book Window */
    ID_SITE_WINDOW_FULL_HOSTS_WINDOW_OK,
    ID_SITE_WINDOW_CLICKED_NEW,
    ID_SITE_WINDOW_CLICKED_CHANGE,
    ID_SITE_WINDOW_CLICKED_DELETE,
    ID_SITE_WINDOW_CLICKED_SAVE,
    ID_SITE_WINDOW_CLICKED_EXIT,
    ID_DBLCLICKED_SITE_WINDOW_HOSTS,
    ID_CHANGESEL_SITE_WINDOW_HOSTS,

    /* Global Settings Window */
    ID_GLOBAL_SETTINGS_WINDOW_SAVE,
    ID_GLOBAL_SETTINGS_WINDOW_CANCEL
};

/* Statics */
static const char *g_AddressBook_ConnectionTypes[] =
{
    "Active",
    "Passive",
    NULL
};

static const char *g_GlobalSettings_QueueTypes[] = 
{
    "Stop at error",
    "Process All",
    NULL
};

static const char *g_GlobalSettings_DeletePartial[] = 
{
    "Delete",
    "Keep",
    NULL
};

/* Globals */
#ifdef __MORPHOS__
struct Library *MUIMasterBase;
struct IntuitionBase *IntuitionBase;
struct IFFParseBase *IFFParseBase;
struct DOSBase *DOSBase;
struct SysBase *SysBase;
#endif

struct Library 		*SocketBase;
struct Device 	   	*TimerBase;
struct timerequest 	*g_TimerIO;
struct MsgPort     	*g_TimerMP;

// Const. Strings
char *STR_CANT_SEND = "STATUS: cannot send command, waiting for ftp response to previous command";
char *STR_CANT_SEND_NOT_CONNECTED = "STATUS: cannot send command, not connected";

Object	*app;

#define Title(t)        { NM_TITLE, t, NULL, 0, 0, NULL }
#define Item(t,s,i)     { NM_ITEM, t, s, 0, 0, (APTR)i }
#define ItemBar         { NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL }
#define SubItem(t,s,i)  { NM_SUB, t, s, 0, 0, (APTR)i }
#define SubBar          { NM_SUB, NM_BARLABEL, NULL, 0, 0, NULL }
#define EndMenu         { NM_END, NULL, NULL, 0, 0, NULL }
#define ItCk(t,s,i,f)   { NM_ITEM, t, s, f, 0, (APTR)i }

static struct NewMenu Menus[] = {
    Title( "File" /* File */ ),
        Item( "Open config file",	NULL, MNID_OPENCONFIG),
        Item( "Save config file",	NULL, MNID_SAVECONFIG),
        Item( "Export config file",	NULL, MNID_EXPORTCONFIG),
        Item( "Import config file",	NULL, MNID_IMPORTCONFIG),
        ItemBar,
        Item( "About", NULL, MNID_ABOUT),
        ItemBar,
        Item( "Quit", NULL, MNID_QUIT),
    Title( "Settings" /* Settings */ ),
        Item( "Global settings",   NULL, MNID_GLOBALSETTINGS   ),
        Item( "Open address book",   NULL, MNID_OPENADDRESSBOOK   ),
    EndMenu
};

#define STACK_BOTTOM (7)
#define STACK_CURRENT (0)
int recv_command_stack[STACK_BOTTOM+1];
char sent_command_stack[512][STACK_BOTTOM+1];

TimerVal g_listen_timer, g_last_packet_transfered_timer, g_trying_connect_timer, g_transfer_start_timer, g_gui_update_timer;
char g_temp[1024];

/* Ftp variables */
Connection g_Connection;
AddressBook g_AddressBook;
GlobalSettings g_GlobalSettings;

/* Prototypes */
static BOOL Init();
static BOOL OpenLibs();
static BOOL CloseLibs();
static void CleanUp();
BOOL RefreshLocalView(Connection *cn);

/* FTP Commands */
BOOL FtpConnect(Connection *cn);
int FtpDisconnect(Connection *cn);
BOOL OpenListenSocket(Connection *cn);
BOOL PasvConnection(Connection *cn);
BOOL CloseListenSocket(Connection *cn);
BOOL CloseDataSocket(Connection *cn);
int SetTransferMode(Connection *cn, char mode);
int SetTransferPort(Connection *cn);
int SendLISTCMD(Connection *cn);
int SendPWDCMD(Connection *cn);
int SendCWDCMD(Connection *cn, char *path);
int SendRETRCMD(Connection *cn, char *filename);
int SendSTORCMD(Connection *cn, char *filename);
int SendMKDCMD(Connection *cn, char *pathname);
int SendPASVCMD(Connection *cn);

/* FTP Utility functions */
int SendFtpCommand(Connection *cn, char *CMD, BOOL b_block);
int GetFtpCommand(struct buffer *buffer, Connection *cn);
BOOL CheckConnect(Connection *cn);
BOOL CheckSocket(SOCKET socket);
void InvalidateSocket(SOCKET *socket);
BOOL GetConnectionInfo(ClientInfo *ci);
BOOL CheckSockForErrors(Connection *cn, SOCKET *socket);
void CleanSockAndFlags(Connection *cn);
void RecvStackPush(int command);
void SentStackPush(char *command);
int GetCommandDigit(int command, int digit);
void ProcessCommand(Connection *cn, int command, int *mui_res);
BOOL ProcessError(Connection *cn, int command);
void ProcessDirectoryData(Connection *cn);
BOOL IsLastSentCmd(char *str);
void ClearTransferFlags(Connection *cn);
BOOL AbortFileTransfer(Connection *cn);
int SockConnect(CSOCK *csock);
void DisplaySockErrs(Connection *cn, CSOCK *csock);
void AcknowledgeConnection(Connection *cn);
int GatherIpPortNumber(Connection *cn);

void AfterRETR(Connection *cn);
void AfterSTOR(Connection *cn);
void AfterMKD(Connection *cn);
void AfterCWD(Connection *cn);
void AfterDELE(Connection *cn);

void GetPathFromRawData(char *text);
int GetFtpCommandValue(char *buffer);
void SetConnected(Connection *cn, BOOL b_con);
void FormatFtpListing(Connection *cn, buffer *buffer, BOOL b_last_data);
char *IpToStr(int ip, char sepchars);
int InitiateListTransfer(Connection *cn);
int InitiateFileTransfer(Connection *cn, char *filename, BOOL b_download);
int InitiateFileDeletion(Connection *cn, char *filename);

BOOL LocalMakedir(Connection *cn, char *pathname);
void RemoteMakedir(Connection *cn, char *pathname);
void LocalBasePath(Connection *cn, char *pathname);
void RemoteChdir(Connection *cn, char *pathname);
BOOL LocalDirScan(Connection *cn, char *pathname, char *basepath, int base_start);
void RemoteDirScan(Connection *cn, char *pathname);

/* Transfering functions */
void HandleDownload(Connection *cn);
void HandleUpload(Connection *cn);

/* Timer funtions */
void StartTimer(TimerVal *tv);
void UpdateTimer(TimerVal *tv);
void EndTimer(TimerVal *tv);
int TimeInSecs(TimerVal *tv);
double TimeInFloat(TimerVal *tv);

/* Gui stuff */
void MUI_SetConnectButton(Connection *cn);
void MUI_SetDisconnectButton(Connection *cn);
void MUI_AddLocalViewText(Connection *cn, char *text);
void MUI_AddRemoteViewText(Connection *cn, char *text);
void MUI_AddStatusWindow(Connection *cn, char *text);
void MUI_AddListboxCMDText(Connection *cn, char *text);
void MUI_AddListboxASWText(Connection *cn);
void MUI_ClearListbox(Object *LBox);
void MUI_GetSet(APTR object, int attrib, int state);
void MUI_UpdateQueueListbox(Connection *cn, BOOL b_full_update);
int MUI_GetListboxSelItemCount(APTR Listbox);
void OnQueueProcess(Connection *cn);
void OnQueueStep(Connection *cn);
void OnClearQueue(Connection *cn);
void OnDownloadBtnClick(Connection *cn);
void OnUploadBtnClick(Connection *cn);
void OnDeleteBtnClick(Connection *cn);
void OnLeftListviewDblClick(Connection *cn);
void OnRightListviewDblClick(Connection *cn);
void LeftListviewDblClickSingle(Connection *cn);
void OnSiteWindowNew();
void OnSiteWindowChange();
void OnSiteWindowSave(Connection *cn);
void OnSiteWindowDelete();
void OnSiteWindowExit(Connection *cn);
void OnSiteWindowCancel();
void OnSiteWindowHostsLVDBLClick(Connection *cn);
void OnSiteWindowHostsSelectChange(Connection *cn);
void UpdateSiteWindow(int active);
void UpdateSiteWindowListBox(int active);
void UpdateSiteWindowContents(int active);
void EvalSiteWindowStatus();
void OnGlobalSettingsSave();
void EvalGlobalSettingsWindow();

// Queue functions
void QueueAdvance(Connection *cn);
BOOL QueuePush(Connection *cn, int command, char *data, char *c_arg1, int i_arg1, BOOL b_local_cmd);
void *QueuePushAddStr(Connection *cn, buffer *buf, char *string, int command, char *c_arg1, int i_arg1, BOOL b_local_cmd);
BOOL QueuePop(Connection *cn);
void QueueClear(Connection *cn);
void QueueProcess(Connection *cn);
void QueuePostProcess(Connection *cn);

// General utility functions
void DateStampToStr(char *dest, struct DateStamp *ds);
void DecodeBitFlags(char *dest, LONG flags);
void BufferFlush(buffer *buf);
void BufferOpen(buffer *buf, BOOL b_bufclear);
char *BufferAddStr(buffer *buf, char *string);
void *BufferAddStruct(buffer *buf, void *struc, int size);
void BufferClose(buffer *buf);
void *AlignPtr(void *ptr, int align_size, int *align_amount);
void RemoveLFCF(char *str);
BOOL SaveConfig();
void LoadConfig(Connection *cn);
void LoadConfig2(Connection *cn);
int MemReadString(char *dest, char **ptr, int len, int maxlen);
int MemReadDword(char **ptr);

#ifdef __AROS__
AROS_UFH3(void, KeyPressFunc,
AROS_UFHA(struct Hook *, hook, A0),
AROS_UFHA( APTR, obj, A2),
AROS_UFHA(struct TagItem *, tag_list, A1))
{
    AROS_USERFUNC_INIT

    DebugOutput("HOOK HOOK!\n");

    AROS_USERFUNC_EXIT
}

/*
ULONG HookEntry(struct Hook* h, void* o, void* msg) 
{
    return (((ULONG(*)(struct Hook*, void*, void*)) *h->h_SubEntry)( h, o, msg ));
}
*/

AROS_UFH3(void, LeftViewDispFunc,
AROS_UFHA(struct Hook *, hook, A0),
AROS_UFHA(char **, strings, A2),
AROS_UFHA(ViewColumn *, data, A1))
{
    AROS_USERFUNC_INIT

    if (data) {
        strings[0] = data->name;
        strings[1] = data->size;
        strings[2] = data->flags;
        strings[3] = data->date;
    } else {
        strings[0] = "Name";
        strings[1] = "Size";
        strings[2] = "Flags";
        strings[3] = "Date";
    }

    AROS_USERFUNC_EXIT
}

AROS_UFH3(void, RightViewDispFunc,
AROS_UFHA(struct Hook *, hook, A0),
AROS_UFHA(char **, strings, A2),
AROS_UFHA(ViewColumn *, data, A1))
{
    AROS_USERFUNC_INIT

    if (data) {
        strings[0] = data->name;
        strings[1] = data->size;
        strings[2] = data->flags;
        strings[3] = data->date;
    } else {
        strings[0] = "Name";
        strings[1] = "Size";
        strings[2] = "Flags";
        strings[3] = "Date";
    }

    AROS_USERFUNC_EXIT
}

AROS_UFH3(void, QueueViewDispFunc,
AROS_UFHA(struct Hook *, hook, A0),
AROS_UFHA(char **, strings, A2),
AROS_UFHA(QueueColumn *, data, A1))
{
    AROS_USERFUNC_INIT

    if (data) {
        switch(data->command) {
            case CMD_DOWNLOAD:
                    strings[0] = "DOWNLOAD";
                    break;

            case CMD_UPLOAD:
                    strings[0] = "UPLOAD";
                    break;

            case CMD_REMOTE_DELETE:
                    strings[0] = "REMOTE DELETE";
                    break;

            case CMD_RENAME:
                    strings[0] = "RENAME";
                    break;

            case CMD_LOCAL_DIR_SCAN:
                    strings[0] = "LOCAL DIR SCAN";
                    break;

            case CMD_REMOTE_DIR_SCAN:
                    strings[0] = "REMOTE DIR SCAN";
                    break;

            case CMD_LOCAL_MKDIR:
                    strings[0] = "LOCAL MKDIR";
                    break;

            case CMD_REMOTE_MKDIR:
                    strings[0] = "REMOTE MKDIR";
                    break;

            case CMD_LOCAL_BASEPATH:
                    strings[0] = "LOCAL BASEPATH";
                    break;

            case CMD_REMOTE_CHDIR:
                    strings[0] = "REMOTE CHDIR";
                    break;

            default:
                    strings[0] = "ERROR";
                    break;
        }

        strings[1] = data->name;

        switch (data->status) {
            case STATUS_COMPLETED:
                    strings[2] = "COMPLETED";
                    break;

            case STATUS_ERROR:
                    strings[2] = "ERROR";
                    break;

            case STATUS_QUEUED:
                    strings[2] = "QUEUED";
                    break;
        }
    } else {
        strings[0] = "Command";
        strings[1] = "Name";
        strings[2] = "Status";
    }

    AROS_USERFUNC_EXIT
}

#endif

#ifdef __MORPHOS__
ULONG KeyPressFunc(struct Hook *MyHook, Object *MyObject, APTR MyMsg)
{
    /* MyHook->h_Data can be used to pass userdata */
    /* MyMsg is whatever the specific hook makes it to be */
    DebugOutput("HOOK HOOK!\n");
    
    return 0;
}

ULONG LeftViewDispFunc(struct Hook *MyHook, char **strings, ViewColumn *data)
{
    if (data) {
        strings[0] = data->name;
        strings[1] = data->size;
        strings[2] = data->flags;
        strings[3] = data->date;
    } else {
        strings[0] = "Name";
        strings[1] = "Size";
        strings[2] = "Flags";
        strings[3] = "Date";
    }

    return 0;
}

ULONG RightViewDispFunc(struct Hook *MyHook, char **strings, ViewColumn *data)
{
    if (data) {
        strings[0] = data->name;
        strings[1] = data->size;
        strings[2] = data->flags;
        strings[3] = data->date;
    } else {
        strings[0] = "Name";
        strings[1] = "Size";
        strings[2] = "Flags";
        strings[3] = "Date";
    }

    return 0;
}

ULONG QueueViewDispFunc(struct Hook *MyHook, char **strings, QueueColumn *data)
{
    if (data) {
        switch(data->command) {
            case CMD_DOWNLOAD:
                    strings[0] = "DOWNLOAD";
                    break;

            case CMD_UPLOAD:
                    strings[0] = "UPLOAD";
                    break;

            case CMD_REMOTE_DELETE:
                    strings[0] = "REMOTE DELETE";
                    break;

            case CMD_RENAME:
                    strings[0] = "RENAME";
                    break;

            case CMD_LOCAL_DIR_SCAN:
                    strings[0] = "LOCAL DIR SCAN";
                    break;

            case CMD_REMOTE_DIR_SCAN:
                    strings[0] = "REMOTE DIR SCAN";
                    break;

            case CMD_LOCAL_MKDIR:
                    strings[0] = "LOCAL MKDIR";
                    break;

            case CMD_REMOTE_MKDIR:
                    strings[0] = "REMOTE MKDIR";
                    break;

            case CMD_LOCAL_BASEPATH:
                    strings[0] = "LOCAL BASEPATH";
                    break;

            case CMD_REMOTE_CHDIR:
                    strings[0] = "REMOTE CHDIR";
                    break;

            default:
                    strings[0] = "ERROR";
                    break;
        }

        strings[1] = data->name;

        switch (data->status) {
            case STATUS_COMPLETED:
                    strings[2] = "COMPLETED";
                    break;

            case STATUS_ERROR:
                    strings[2] = "COMPLETED";
                    break;

            case STATUS_QUEUED:
                    strings[2] = "QUEUED";
                    break;
        }
    } else {
        strings[0] = "Command";
        strings[1] = "Name";
        strings[2] = "Status";
    }

    return 0;
}
#endif

struct Hook KeyPressHook = 
{
    {NULL, NULL}, 
    (HOOKFUNC) HookEntry,
    (HOOKFUNC) KeyPressFunc, 
    NULL
};

struct Hook RightViewDispHook =
{
    {NULL, NULL}, 
    (HOOKFUNC) HookEntry,
    (HOOKFUNC) RightViewDispFunc, 
    NULL
};

struct Hook LeftViewDispHook =
{
    {NULL, NULL}, 
    (HOOKFUNC) HookEntry,
    (HOOKFUNC) LeftViewDispFunc,
    NULL
};

struct Hook QueueDisplayHook =
{
    {NULL, NULL}, 
    (HOOKFUNC) HookEntry,
    (HOOKFUNC) QueueViewDispFunc, 
    NULL
};

static BOOL OpenLibs()
{
    // Caf stuff...
#ifdef DEBUG
//	SetDebugFile("ram:marranoftp.log");
//	CafDebugInit(CAF_DEBUG_FILE);
//	SetDebugFile("CON:");
//	CafDebugInit(CAF_DEBUG_FILE | CAF_DEBUG_STDOUT);
    CafDebugInit(CAF_DEBUG_STDOUT);
#endif

#ifndef NO_SOCKETLIB
    SocketBase = (struct Library *) OpenLibrary("bsdsocket.library", 0);
    if (!SocketBase) {
        printf("Cannot open bsdsocket.library\n");
        return FALSE;
    } else { 
#ifdef DEBUGLV3
        DebugOutput("OpenLibs(): Opened bsdsocket.library\n");
#endif
    }
#endif

    g_TimerMP = CreateMsgPort();
    if (g_TimerMP) {
        g_TimerIO = (struct timerequest *) CreateIORequest(g_TimerMP, sizeof(struct timerequest));
        if (g_TimerIO) {
            if (OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *) g_TimerIO, 0) == 0) {
                TimerBase = g_TimerIO->tr_node.io_Device;
#ifdef DEBUGLV3
                DebugOutput("OpenLibs(): Opened timer.device\n");
#endif
            } else {
                printf("Cannot open timer.device\n");
                return FALSE;
            }
        } else {
            printf("CreateIORequest() failed\n");
            return FALSE;
        }
    } else {
        printf("CreateMsgPort() failed\n");
        return FALSE;
    }

    return TRUE;
}

static BOOL CloseLibs()
{
    if (TimerBase) {
        CloseDevice(&g_TimerIO->tr_node);
        TimerBase = 0;
    }

    if (g_TimerIO) {
        DeleteIORequest((APTR) g_TimerIO);
        g_TimerIO = 0;
    }

    if (g_TimerMP) {
        DeleteMsgPort(g_TimerMP);
        g_TimerMP = 0;
    }

    if (SocketBase) {
        CloseLibrary(SocketBase);
        SocketBase = 0;
    }

    return TRUE;
}

static BOOL Init()
{
    Connection *cn = &g_Connection;
    ClientInfo *ci = &cn->ci;
    LocalView *lv  = &cn->lv;
    RemoteView *rv = &cn->rv;
    BOOL b_flag = TRUE;

    // Initialize everything
    caf_memset(&g_listen_timer, 0, sizeof(TimerVal));
    caf_memset(&g_last_packet_transfered_timer, 0, sizeof(TimerVal));
    caf_memset(&g_trying_connect_timer, 0, sizeof(TimerVal));
    caf_memset(&g_transfer_start_timer, 0, sizeof(TimerVal));
    caf_memset(&g_gui_update_timer, 0, sizeof(TimerVal));
    caf_memset(&sent_command_stack, 0, sizeof(sent_command_stack));
    caf_memset(&recv_command_stack, 0, sizeof(recv_command_stack));
    caf_memset(cn, 0, sizeof(Connection));
    caf_memset(&cn->cmd_buffer, 0, sizeof(buffer));
    caf_memset(&cn->transfer_buffer, 0, sizeof(buffer));
    caf_memset(&cn->queue_buffer, 0, sizeof(buffer));
    caf_memset(&cn->temp_buffer, 0, sizeof(buffer));
    caf_memset(&g_AddressBook, 0, sizeof(AddressBook));
    ci->cmd_socket = ci->listen_socket = ci->transfer_socket = cn->hi.pasvsettings.socket = INVALID_SOCKET;

#define COMMAND_BUFFER_SIZE (4096)
#define TRANSFER_BUFFER_SIZE (1048572)
#define LIST_BUFFER_SIZE (1048572)
#define QUEUE_BUFFER_SIZE (512*1024)
#define TEMP_BUFFER_SIZE (256*1024)

    cn->cmd_buffer.size 	 	= COMMAND_BUFFER_SIZE;
    cn->transfer_buffer.size 	= TRANSFER_BUFFER_SIZE;
    cn->queue_buffer.size		= QUEUE_BUFFER_SIZE;
    cn->temp_buffer.size		= TEMP_BUFFER_SIZE;
    cn->cmd_buffer.ptr 		 	= malloc(COMMAND_BUFFER_SIZE);
    cn->transfer_buffer.ptr    	= malloc(TRANSFER_BUFFER_SIZE);
    cn->queue_buffer.ptr		= malloc(QUEUE_BUFFER_SIZE);
    cn->temp_buffer.ptr 		= malloc(TEMP_BUFFER_SIZE);

    caf_memset(lv, 0, sizeof(LocalView));
    lv->ListBuffer.size = LIST_BUFFER_SIZE;
    lv->ListBuffer.ptr	 = malloc(LIST_BUFFER_SIZE);

    caf_memset(rv, 0, sizeof(RemoteView));
    rv->ListBuffer.size = LIST_BUFFER_SIZE;
    rv->ListBuffer.ptr  = malloc(LIST_BUFFER_SIZE); 

    if (cn->cmd_buffer.ptr == 0) {
        printf("Command buffer allocation failed\n");
        b_flag = FALSE;
    }

    if (cn->transfer_buffer.ptr == 0) {
        printf("Transfer buffer allocation failed\n");
        b_flag = FALSE;
    }

    if (cn->queue_buffer.ptr == 0) {
        printf("Queue buffer allocation failed\n");
        b_flag = FALSE;
    }

    if (cn->temp_buffer.ptr == 0) {
        printf("Temp buffer allocation failed\n");
        b_flag = FALSE;
    }

    if (lv->ListBuffer.ptr == 0) {
        printf("Local listview buffer allocation failed\n");
        b_flag = FALSE;
    }

    if (rv->ListBuffer.ptr == 0) {
        printf("Remote listview buffer allocation failed\n");
        b_flag = FALSE;
    }

    if (OpenLibs() == FALSE || b_flag == FALSE) {
        CleanUp();
        return FALSE;
    }

    g_AddressBook.used_hosts = 0;
    g_GlobalSettings.SettingsArray[SETTING_SOCKET_TIMEOUT] = 2;
    g_GlobalSettings.SettingsArray[SETTING_TRANSFER_TIMEOUT] = 5;
    g_GlobalSettings.SettingsArray[SETTING_CONNECT_TIMEOUT] = 10;
    g_GlobalSettings.SettingsArray[SETTING_QUEUE_TYPE] = OPT_QUEUE_STOP_ON_ERROR;
    g_GlobalSettings.SettingsArray[SETTING_KEEP_PARTIAL] = OPT_DOWNLOAD_DELETE_PARTIAL;
    g_GlobalSettings.SettingsArray[SETTING_LOCAL_REFRESH_AFTER_RETR] = OPT_NO_REFRESH_AFTER_RETR;
    QueueClear(cn);

    caf_strncpy(g_config_filename, "PROGDIR:marranoftp.prefs", 512); 
    return TRUE;
}

void CleanUp()
{
    Connection *cn = &g_Connection;
    ClientInfo *ci = &cn->ci;
    LocalView *lv  = &cn->lv;
    RemoteView *rv = &cn->rv;

    if (ci) {
        if (ci->filehandle) {
            Close(ci->filehandle);
            ci->filehandle = 0;
        }
    }

    if (cn->cmd_buffer.ptr)
        free(cn->cmd_buffer.ptr);

    if (cn->transfer_buffer.ptr)
        free(cn->transfer_buffer.ptr);

    if (cn->queue_buffer.ptr)
        free(cn->queue_buffer.ptr);

    if (cn->temp_buffer.ptr)
        free(cn->temp_buffer.ptr);

    if (lv->ListBuffer.ptr)
        free(lv->ListBuffer.ptr);

    if (rv->ListBuffer.ptr)
        free(rv->ListBuffer.ptr);

#ifdef DEBUGLV3
    CafDebugCleanup();
#endif

    CloseLibs();
}

static const UBYTE MSG_logtab_TransferQueue[] = "Transfer Queue...";
static const UBYTE MSG_logtab_Log[] = "Log...";

static UBYTE *logtabs[] =
{
    (UBYTE *)MSG_logtab_TransferQueue,
    (UBYTE *)MSG_logtab_Log,
    NULL
};

int main(int argc,char *argv[])
{
    Connection *cn = &g_Connection;
    ClientInfo *ci = &cn->ci;
    APTR MUIMenu;
    IPTR iresult;

    if (Init() == FALSE)
        return RETURN_FAIL;

    strcpy(cn->lv.CurrentPath, "ram:");

    /* MUI Gui Definition */    
    app = ApplicationObject,
        MUIA_Application_Title, (IPTR) "MarranoFTP",
        MUIA_Application_Version, (IPTR) "$VER: MarranoFTP 0.71 (22.06.2010) © Stefano Crosara aka Suppah at marranosoft@gmail.com",
        MUIA_Application_Copyright, (IPTR) "© Stefano Crosara aka Suppah",
        MUIA_Application_Author, (IPTR) "Stefano Crosara",
        MUIA_Application_Description,  (IPTR) "A very 'marrano' FTP client",
        MUIA_Application_Base, (IPTR) "MarranoFTP",
        MUIA_Application_SingleTask, FALSE,
        MUIA_Application_Menustrip, (IPTR)(MUIMenu = MUI_MakeObject(MUIO_MenustripNM,Menus,0)),

        // Main FTP Browse Window(s)
        SubWindow, (IPTR)(cn->Window = WindowObject,
            MUIA_Window_Title, (IPTR) "MarranoFTP 0.71 (22.06.2010) © Stefano Crosara aka Suppah at marranosoft@gmail.com",
            MUIA_Window_Width, MUIV_Window_Width_Visible(100),
            MUIA_Window_Height, MUIV_Window_Height_Visible(90),  /*  changed main window height from 100% to 90% and from MinMax to Visible*/
            MUIA_Window_ID, MAKE_ID('M','F','T','P'),
            WindowContents, VGroup,

                Child, VGroup,
                    Child, HGroup,
                        MUIA_VertWeight, 60,
                        Child, VGroup,
                            MUIA_Group_VertSpacing, 1,
                            Child, (IPTR)(cn->lv.ListView = NListviewObject,
                                MUIA_NListview_NList, NListObject,
                                    MUIA_CycleChain, 1, 
                                    MUIA_NList_Format, "MAXW=50,MAXW=20,MAXW=20,MAXW=10",
                                    MUIA_NList_TypeSelect, MUIV_NList_TypeSelect_Line,
                                    MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Shifted, 
                                    MUIA_Font, MUIV_Font_Fixed,
                                    MUIA_NList_DisplayHook, &LeftViewDispHook,
                                    MUIA_NList_Title, TRUE,
                                    MUIA_NList_TitleSeparator, TRUE,
                                    MUIA_NList_AutoVisible, TRUE,
                                    MUIA_NList_EntryValueDependent, TRUE,
                                    MUIA_NList_MinColSortable, 0,
                                End, // NListObject
                            End), // NListviewObject

                            Child, (IPTR)(cn->S_LEFT_VIEW_PATH = (APTR)StringObject, StringFrame, MUIA_CycleChain, 1, MUIA_String_MaxLen, 512, End),
                        End, // VGroup

                        Child, VGroup,
                            MUIA_Group_VertSpacing, 1,
                            Child, (IPTR)(cn->rv.ListView = NListviewObject, 
                                MUIA_NListview_NList, NListObject,
                                    MUIA_CycleChain, 1, 
                                    MUIA_NList_Format, "MAXW=50,MAXW=20,MAXW=20,MAXW=10",
                                    MUIA_NList_TypeSelect, MUIV_NList_TypeSelect_Line,
                                    MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Shifted, 
                                    MUIA_Font, MUIV_Font_Fixed,
                                    MUIA_NList_DisplayHook, &RightViewDispHook,
                                    MUIA_NList_Title, TRUE,
                                    MUIA_NList_TitleSeparator, TRUE,
                                    MUIA_NList_AutoVisible, TRUE,
                                    MUIA_NList_EntryValueDependent, TRUE,
                                    MUIA_NList_MinColSortable, 0,
                                End, // NListObject
                            End), // NListviewObject

                            Child, (IPTR)(cn->S_RIGHT_VIEW_PATH = (APTR) StringObject, StringFrame, MUIA_CycleChain, 1, MUIA_String_MaxLen, 512, End),
                        End, // VGroup
                    End, // HGroup

                    Child, BalanceObject, End,

                    Child, RegisterObject,
                        MUIA_VertWeight,    40,
                        MUIA_Background,    MUII_RegisterBack,
                        MUIA_CycleChain,    TRUE,
                        MUIA_Register_Titles, logtabs,
                        Child, HGroup,
                            Child, (IPTR)(cn->QueueLV = NListviewObject, 
                                MUIA_NListview_NList, NListObject,
                                    MUIA_CycleChain, 1, 
                                    MUIA_NList_Format, "MAXW=50,MAXW=20,MAXW=20",
                                    MUIA_NList_TypeSelect, MUIV_NList_TypeSelect_Line,
                                    MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_Shifted, 
                                    MUIA_Font, MUIV_Font_Fixed,
                                    MUIA_NList_DisplayHook, &QueueDisplayHook,
                                    MUIA_NList_Title, TRUE,
                                    MUIA_NList_TitleSeparator, TRUE,
                                    MUIA_NList_AutoVisible, TRUE,
                                    MUIA_NList_EntryValueDependent, TRUE,
                                    MUIA_NList_MinColSortable, 0,
                                End, // NListObject
                            End), // NListviewObject
                        End, // VGroup

                        Child, HGroup, 
                            Child, (IPTR)(cn->LV_STATUS = NListviewObject, 
                                MUIA_Listview_Input, FALSE, 
                                MUIA_Listview_List, NListObject,
                                    MUIA_CycleChain, 1, 
                                    ReadListFrame,
                                    MUIA_List_ConstructHook, MUIV_NList_ConstructHook_String,
                                    MUIA_List_DestructHook, MUIV_NList_DestructHook_String,
                                    MUIA_List_AutoVisible, TRUE,
                                End, // NListObject
                            End), // NListviewObject
                        End, // HGroup
                    End,
                End,

                Child, (IPTR)(cn->S_TRANSFER_INFO =  StringObject, StringFrame, 
                    MUIA_InputMode, MUIV_InputMode_None,
                    MUIA_String_Accept, "",
                    MUIA_String_MaxLen, 512, 
                End),

                Child, HGroup, 
                    Child, (IPTR)(cn->BTN_DOWNLOAD = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cDownload", End,
                    End),

                    Child, (IPTR)(cn->BTN_UPLOAD = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cUpload", End,
                    End),

                    Child, (IPTR)(cn->BTN_DELETE = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cRemote Delete", End,
                    End),

                    Child, (IPTR)(cn->BTN_STOP = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cStop", End,
                    End),

                    Child, (IPTR)(cn->BTN_RUN_QUEUE = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cRun Queue", End,
                    End),

                    Child, (IPTR)(cn->BTN_STEP_QUEUE = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cStep Queue", End,
                    End),

                    Child, (IPTR)(cn->BTN_CLEAR_QUEUE = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cClear Queue", End,
                    End),

                    Child, (IPTR)(cn->BTN_RENAME = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cRemote Rename", End,
                    End),

                    Child, (IPTR)(cn->BTN_MAKEDIR = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cRemote Makedir", End,
                    End),

                    //
                    //Child, (IPTR)(cn->BTN_DUMP = HGroup,
                        //ButtonFrame,
                        //MUIA_InputMode, MUIV_InputMode_RelVerify,
                        //MUIA_Background, MUII_ButtonBack,
                        //Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Dump", End,
                    //End),
                End,

                Child, HGroup,
                    Child, (IPTR)(cn->BTN_CONNDISC = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cDisconnect", End,
                    End),

                    Child, (IPTR)(cn->BTN_QUIT = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cQuit", End,
                    End),

                    Child, (IPTR)(cn->BTN_REFRESH_L = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cRefresh Left", End,
                    End),

                    Child, (IPTR)(cn->BTN_REFRESH_R = HGroup,
                        ButtonFrame,
                        MUIA_InputMode , MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child,  TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33cRefresh Right", End,
                    End),
                End,
            End,
        End),

        // Address Book Window
        SubWindow, g_AddressBook.Window = WindowObject,
            MUIA_Window_Title, "Address book",
//            MUIA_Window_Width, MUIV_Window_Width_MinMax(20),
//            MUIA_Window_Height, MUIV_Window_Height_MinMax(20),
            MUIA_Window_ID, MAKE_ID('S','I','T','E'),
            WindowContents, VGroup, 
                Child, HGroup, 
                    Child, (IPTR)(g_AddressBook.LV_HOSTS = NListviewObject, 
                        MUIA_Listview_List, NListObject,
                            MUIA_CycleChain, 1, 
                            ReadListFrame,
                            MUIA_List_AutoVisible, TRUE,
                        End,
                    End),
                End,

                Child, HGroup, 
                    Child, VGroup,
                        Child, HGroup,
                            MUIA_Weight, 70,
                            Child, TextObject, MUIA_Text_Contents, "Entry name", End,
                        End,

                        Child, HGroup,
                            MUIA_Weight, 70,
                            Child, (IPTR)(g_AddressBook.S_ENTRYNAME = (APTR) StringObject, MUIA_CycleChain, 1, StringFrame, MUIA_String_MaxLen, 128, End),
                        End,

                        Child, HGroup,
                            Child, HGroup,
                                MUIA_Weight, 70,
                                Child, TextObject, MUIA_Text_Contents, "Address", End,
                            End,
                            Child, HGroup,
                                MUIA_Weight, 20,
                                Child, TextObject, MUIA_Text_Contents, "Port", End,
                            End,
                        End,

                        Child, HGroup, 
                            Child, HGroup,
                                MUIA_Weight, 70,
                                Child, (IPTR)(g_AddressBook.S_HOSTNAME = (APTR) StringObject, MUIA_CycleChain, 1, StringFrame, MUIA_String_MaxLen, 128, End),
                            End,
                            Child, HGroup,
                                MUIA_Weight, 20,
                                Child, (IPTR)(g_AddressBook.S_PORT = (APTR) StringObject, MUIA_CycleChain, 1, StringFrame, MUIA_String_MaxLen, 8, End),
                            End,
                        End, 

                        Child, TextObject, MUIA_Text_Contents, "Username", End,
                            Child, (IPTR)(g_AddressBook.S_USERNAME = (APTR) StringObject, MUIA_CycleChain, 1,StringFrame, MUIA_String_MaxLen, 32, End),
                            Child, TextObject, MUIA_Text_Contents, "Password", End,
                            Child, (IPTR)(g_AddressBook.S_PASSWORD = (APTR) StringObject, MUIA_CycleChain, 1,StringFrame, MUIA_String_MaxLen, 32, End),
                        End,
                    End,

                    Child, HGroup,
                        Child, VGroup,
                            Child, TextObject, MUIA_Text_Contents, "Transfer type", End,
                            Child, (IPTR)(g_AddressBook.RDIO_CONN_TYPE = RadioObject,
                                MUIA_Group_Horiz, TRUE,
                                MUIA_Radio_Entries, g_AddressBook_ConnectionTypes,
                            End),
                        End,
                    End,

                    Child, HGroup, 
                        Child, (IPTR)(g_AddressBook.BTN_NEW = HGroup,
                            ButtonFrame,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Background, MUII_ButtonBack,
                            Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c New", End,
                        End),

                        Child, (IPTR)(g_AddressBook.BTN_CHANGE = HGroup,
                            ButtonFrame,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Background, MUII_ButtonBack,
                            Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Add/Update", End,  
                        End),

                        Child, (IPTR)(g_AddressBook.BTN_DELETE = HGroup,
                            ButtonFrame,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Background, MUII_ButtonBack,
                            Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Delete", End,
                        End),

                        Child, (IPTR)(g_AddressBook.BTN_EXIT = HGroup,
                            ButtonFrame,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Background, MUII_ButtonBack,
                            Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Exit", End,
                        End),

			/* Removed SAVE button as address-book entries are already saved somewhere else on add/update action or exit */ 
                        /*Child, (IPTR)(g_AddressBook.BTN_SAVE = HGroup,    
                            ButtonFrame,
                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                            MUIA_Background, MUII_ButtonBack,
                            Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Save", End,
                        End),*/
                    End,
                End,
            End,

            // Error Window
        SubWindow, g_AddressBook.Window_HostsFull = WindowObject,
            MUIA_Window_Title, "Error!",
            WindowContents, VGroup, 
                Child, TextObject, MUIA_Text_Contents, "Reached maximum number of hosts", End,
                Child, (IPTR)(g_AddressBook.BTN_HostsFullOk = HGroup,
                    ButtonFrame,
                    MUIA_InputMode, MUIV_InputMode_RelVerify,
                    MUIA_Background, MUII_ButtonBack,
                    Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Exit", End,
                End),
            End,
        End,

        // Global Settings Window 
        SubWindow, g_GlobalSettings.Window = WindowObject,
            MUIA_Window_Title, "Global Settings",
            WindowContents, VGroup, 
                Child, TextObject, MUIA_Text_Contents, "\33cFile Transfer Timeout", End,
                Child, g_GlobalSettings.S_TRANSFER_SOCKET_TIMEOUT = StringObject, StringFrame, MUIA_String_MaxLen, 3, End,

                Child, TextObject, MUIA_Text_Contents, "\33cConnection Timeout", End,
                Child, g_GlobalSettings.S_CONNECTION_SOCKET_TIMEOUT = StringObject, StringFrame, MUIA_String_MaxLen, 3, End,

                Child, TextObject, MUIA_Text_Contents, "\33cQueue Processing", End,
                Child, g_GlobalSettings.RDIO_QUEUE_TYPE = RadioObject,
                        MUIA_Group_Horiz, TRUE,
                        MUIA_Radio_Entries, g_GlobalSettings_QueueTypes,
                End,

                Child, TextObject, MUIA_Text_Contents, "\33cPartial files", End,
                Child, g_GlobalSettings.RDIO_DELETE_PARTIAL = RadioObject,
                        MUIA_Group_Horiz, TRUE,
                        MUIA_Radio_Entries, g_GlobalSettings_DeletePartial,
                End,

                Child, HGroup,
                    Child, (IPTR)(g_GlobalSettings.BTN_SAVE = HGroup,
                        ButtonFrame,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Save", End,
                    End),

                    Child, (IPTR)(g_GlobalSettings.BTN_CANCEL = HGroup,
                        ButtonFrame,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_Background, MUII_ButtonBack,
                        Child, TextObject, MUIA_CycleChain, 1, MUIA_Text_Contents, "\33c Cancel", End,
                    End),
                End,
            End,
        End,
    End; // Application Object
    /* END MUI Gui Definition */

    if (!app) {
        DebugOutput("Cannot create application object.\n");
        printf("Cannot create application object.\n");
        CleanUp();
        return 1;
    }

    // Main Window
    DoMethod(cn->Window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    DoMethod(cn->BTN_CONNDISC, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_CONNDISC);
    DoMethod(cn->BTN_STOP, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_STOP);
    DoMethod(cn->BTN_RUN_QUEUE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_QUEUE_PROCESS);
    DoMethod(cn->BTN_STEP_QUEUE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_QUEUE_STEP);
    DoMethod(cn->BTN_CLEAR_QUEUE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_CLEAR_QUEUE);
    DoMethod(cn->BTN_DOWNLOAD, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_DOWNLOAD);
    DoMethod(cn->BTN_UPLOAD, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_UPLOAD);
    DoMethod(cn->BTN_DELETE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_DELETE);
    DoMethod(cn->BTN_RENAME, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_RENAME);
    DoMethod(cn->BTN_MAKEDIR, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_MAKEDIR);
    DoMethod(cn->BTN_QUIT, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_QUIT);
    DoMethod(cn->BTN_REFRESH_L, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_REFRESH_L);
    DoMethod(cn->BTN_REFRESH_R, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_CLICKED_REFRESH_R);
    
    DoMethod(cn->S_LEFT_VIEW_PATH, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_MODIFIED_LPATHBOX);
    DoMethod(cn->S_RIGHT_VIEW_PATH, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_MODIFIED_RPATHBOX); 
    
    DoMethod(cn->lv.ListView, MUIM_Notify, MUIA_NList_DoubleClick , MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_DBLCLICKED_LEFT_LISTVIEW);
    DoMethod(cn->rv.ListView, MUIM_Notify, MUIA_NList_DoubleClick , MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_DBLCLICKED_RIGHT_LISTVIEW);
    
    // Address book window
    DoMethod(g_AddressBook.Window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_CLICKED_EXIT);
    DoMethod(g_AddressBook.BTN_NEW, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_CLICKED_NEW);
    DoMethod(g_AddressBook.BTN_CHANGE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_CLICKED_CHANGE);
    DoMethod(g_AddressBook.BTN_DELETE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_CLICKED_DELETE);
    DoMethod(g_AddressBook.BTN_SAVE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_CLICKED_SAVE);
    DoMethod(g_AddressBook.BTN_EXIT, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_CLICKED_EXIT);
    DoMethod(g_AddressBook.BTN_HostsFullOk, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_SITE_WINDOW_FULL_HOSTS_WINDOW_OK);
    DoMethod(g_AddressBook.LV_HOSTS, MUIM_Notify, MUIA_NList_DoubleClick, MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_DBLCLICKED_SITE_WINDOW_HOSTS);
    DoMethod(g_AddressBook.LV_HOSTS, MUIM_Notify, MUIA_NList_SelectChange, MUIV_EveryTime, app, 2, MUIM_Application_ReturnID, ID_CHANGESEL_SITE_WINDOW_HOSTS);
    
    // Global settings window
    DoMethod(g_GlobalSettings.Window, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, ID_GLOBAL_SETTINGS_WINDOW_CANCEL);
    DoMethod(g_GlobalSettings.BTN_SAVE, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_GLOBAL_SETTINGS_WINDOW_SAVE);
    DoMethod(g_GlobalSettings.BTN_CANCEL, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_GLOBAL_SETTINGS_WINDOW_CANCEL);
    
/*	Blah
    
    DoMethod(BTN_DUMP, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, ID_DUMP);
//  Can't get those bitches to work under aros
    DoMethod(S_LEFT_VIEW_PATH, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, app, 2, MUIM_CallHook, &KeyPressHook);
    DoMethod(S_RIGHT_VIEW_PATH, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, app, 2, MUIM_CallHook, &KeyPressHook);
*/
    
    DoMethod(cn->LV_STATUS, MUIM_NList_Clear);
    set(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, (IPTR) cn->lv.CurrentPath);
    set(cn->Window, MUIA_Window_Open, TRUE);
    get(cn->Window, MUIA_Window_Open, &iresult);
    if ((BOOL) iresult == TRUE)
    {
#ifdef DEBUGLV3
	int local_errno;
#endif
        int command, sel_sock, mui_res, ret, optlen;
        struct fd_set readfd, writefd, exceptfd;
        SOCKET highest_sock = INVALID_SOCKET;
        ULONG sigs = 0, sigmask = 0;
        struct sockaddr sockaddr;
        struct timeval timeout;
        char temp[512];
        IPTR itmp;		
        
        // Carica la configurazione
        LoadConfig(cn);
        
        MUI_SetConnectButton(cn);
        RefreshLocalView(cn);
        while ((mui_res = DoMethod(app, MUIM_Application_NewInput, &sigs)) != MUIV_Application_ReturnID_Quit) {
                if (sigs) {
                        sigmask = sigs | SIGBREAKF_CTRL_C;
                        if (ci->cmd_socket == INVALID_SOCKET && cn->ci.b_connecting == FALSE && cn->ci.b_connected == FALSE) {
                                #ifdef DEBUGLV3
                                DebugOutput("MainLoop(): Wait()\n");
                                #endif
                                sigs = Wait(sigmask);
                        } else {
                                // WaitSelect.....
                                timeout.tv_secs = g_GlobalSettings.SettingsArray[SETTING_SOCKET_TIMEOUT]; timeout.tv_micro = 0;
                                highest_sock = (ci->listen_socket > ci->cmd_socket) ? ci->listen_socket : ci->cmd_socket;
                                highest_sock = (ci->transfer_socket > highest_sock) ? ci->transfer_socket : highest_sock;
                                
                                // Zero them fd sets					
                                FD_ZERO(&readfd); FD_ZERO(&writefd); FD_ZERO(&exceptfd);
                                
                                if (cn->ci.b_connected == TRUE) {
                                        // We are connected, set the command socket in the readfd
                                        // we wait for ftp commands and answers as well
                                        FD_SET(ci->cmd_socket, &readfd);
                                        
                                        if (cn->ci.b_waitingfordataport == TRUE) {
                                                if (cn->ci.b_passive == FALSE) {
                                                        // Active data connection, we are listening
                                                        FD_SET(ci->listen_socket, &readfd);
                                                } else {
                                                        // Passive data connection, we are connecting
                                                        
                                                        // if we are waiting for data port in active mode,
                                                        // a connect() has been issued, wait for writefd in the
                                                        // socket
                                                        FD_SET(ci->transfer_socket, &writefd);
                                                        #ifdef DEBUG
                                                        DebugOutput("MainLoop(): Setting ci->transfer_socket in writefd (PASV)\n");
                                                        #endif
                                                }
                                        }
                                        
                                        if (cn->ci.b_transfering == TRUE) {
                                                if (cn->ci.b_file_transfer) {
                                                        /* FILE TRANSFER */
                                                        if (ci->b_file_download)
                                                                // We are downloading, let's set the transfer socket in readfd
                                                                FD_SET(ci->transfer_socket, &readfd);
                                                        else
                                                                // We are uploading, let's set the transfer socket in writefd
                                                                FD_SET(ci->transfer_socket, &writefd);
                                                } else {
                                                        /* LIST */
                                                        
                                                        // We are downloading, let's set the transfer socket in readfd
                                                        FD_SET(ci->transfer_socket, &readfd);
                                                }
                                        }
                                } else if (cn->ci.b_connecting == TRUE) {
                                        // If we are going to connect, let's test the write and except fd
                                        FD_SET(ci->cmd_socket, &writefd);
                                        FD_SET(ci->cmd_socket, &exceptfd);
                                } else {
                                        DebugOutput("ERROR! code should not reach this stage\n");
                                }
                                
                                #ifdef DEBUGLV3
                                DebugOutput("MainLoop(): WaitSelect()\n");
                                #endif
                                sel_sock = WaitSelect(highest_sock+1, &readfd, &writefd, &exceptfd, &timeout, &sigmask);
                                sigs = sigmask;
                                
                                if (sel_sock > 0) {
                                        if (cn->ci.b_transfering == TRUE) {
                                                // We are transfering stuph, or are we transfering stuph?
                                                if (CheckSocket(ci->transfer_socket) == FALSE) {
                                                        CloseDataSocket(cn);
                                                        MUI_AddStatusWindow(cn, "STATUS: Error on transfer socket");
                                                } else {
                                                        if (FD_ISSET(ci->transfer_socket, &readfd)) {
                                                                // Downloading...
                                                                #ifdef DEBUG
                                                                printf("Calling HandleDownload()\n");
                                                                #endif
                                                                HandleDownload(cn);
                                                        } else if (FD_ISSET(ci->transfer_socket, &writefd) && ci->b_file_transfer && ci->b_file_download == FALSE) {
                                                                // Uploading...
                                                                #ifdef DEBUG
                                                                printf("Calling HandleUpload()\n");
                                                                #endif
                                                                HandleUpload(cn);
                                                        }
                                                }
                                                
                                                if (TimeInSecs(&g_last_packet_transfered_timer) > g_GlobalSettings.SettingsArray[SETTING_TRANSFER_TIMEOUT]) {
                                                        // Houston Houston, we have a problem! last packet sent is
                                                        // very old.. maybe a connection problem ? let's close the
                                                        // transfer socket
                                                        if (ci->b_file_download) {
                            AbortFileTransfer(cn);
                                                                // SendFtpCommand(cn, "ABOR", FALSE);
                                                                MUI_AddStatusWindow(cn, "STATUS: Closing transfer socket, data timeout, no more data was received in a timely fashion");
                                                        } else
                                                                MUI_AddStatusWindow(cn, "STATUS: Closing transfer socket, data timeout, no more data was sent in a timely fashion");
                                                        
                                                        CloseDataSocket(cn);
                                                }
                                        }
                                        
                                        if (cn->ci.b_connecting == TRUE) {
                                                // We are trying to connect
                                                if (FD_ISSET(ci->cmd_socket, &writefd)) {
                                                        AcknowledgeConnection(cn);
                                                } else if (FD_ISSET(ci->cmd_socket, &exceptfd)) {
                                                        // Command socket event happened, during connect(), error!
                                                        CleanSockAndFlags(cn);
                                                        MUI_AddStatusWindow(cn, "STATUS: exceptfd was set, socket error");
                                                } else if(TimeInSecs(&g_trying_connect_timer) >= 6) {
                                                        MUI_AddStatusWindow(cn, "STATUS: Connect timed out");
                                                        CleanSockAndFlags(cn);
                                                }
                                        }
                                        
                                        // This code has to be before the latter code, transfered byte
                                        // should be updated before the data is sent to listviews
                                        if (cn->ci.b_transfering == TRUE || cn->ci.b_transfered == TRUE) {
                                                double KbOverSec;
                                                UpdateTimer(&g_gui_update_timer);
                                                if (g_gui_update_timer.current_time-g_gui_update_timer.start_time >= 1.0f || cn->ci.b_transfered == TRUE) {
                                                        UpdateTimer(&g_transfer_start_timer);
                                                        KbOverSec = (double)cn->ci.bytes_transfered/(g_transfer_start_timer.current_time-g_transfer_start_timer.start_time);
                                                        KbOverSec /= 1024;
                                                        
                                                        if (cn->ci.b_transfering) {
                                                                // We are transfering stuph..
                                                                snprintf(temp, 511, "Transfering ... %d bytes %d KB/Sec", cn->ci.bytes_transfered, (int)KbOverSec);
                                                        } else if (cn->ci.b_transfered) {
                                                                // We have transfered stuph..
                                                                snprintf(temp, 511, "Transfered ... %d bytes %d KB/Sec", cn->ci.bytes_transfered, (int)KbOverSec);
                                                        }
                                                        
                                                        set(cn->S_TRANSFER_INFO, MUIA_String_Contents, temp);
                                                        StartTimer(&g_gui_update_timer);
                                                }
                                        }
                                        
                                        if (cn->ci.b_connected == TRUE) {
                                                // We are connected, we can expect stuph from cmd socket
                                                if (FD_ISSET(ci->cmd_socket, &readfd)) {
                                                        // Command socket event happened, after connect(), must be ftp stuph
                                                        command = GetFtpCommand(&cn->cmd_buffer, cn);
                                                        MUI_AddListboxASWText(cn);
                                                        
                                                        RecvStackPush(command);
                                                        ProcessCommand(cn, command, &mui_res);
                                                }
                                        }
                                        
/************************************** WAITING FOR DATA PORT ************************************/
                                        if (cn->ci.b_waitingfordataport == TRUE) {
                                                // We are waiting for a data port, let's check if it's
                                                // active or passive connection
                                                
                                                #ifdef DEBUG
                                                DebugOutput("MainLoop(): b_waitingfordataport\n");
                                                #endif
                                                
                                                if (cn->ci.b_passive) {
                                                        #ifdef DEBUG
                                                        snprintf(temp, 511, "MainLoop(): ci.b_passive = TRUE, ci->transfer_socket (PASV) = %d\n", ci->transfer_socket);
                                                        DebugOutput(temp);
                                                        #endif
                                                        
                                                        if (FD_ISSET(ci->transfer_socket, &writefd)) {
                                                                // PASSIVE DATA PORT
                                                                
                                                                // Transfer socket connected in passive mode
                                                                // Previously issued connect() worked
                                                                #ifdef DEBUG
                                                                DebugOutput("MainLoop(): FD_ISSET passive data port\n");
                                                                #endif
                                                                
                                                                snprintf(temp, 511, "STATUS: Connected to server's data port %d", cn->hi.pasvsettings.port);
                                                                StartTimer(&g_transfer_start_timer);
                                                                StartTimer(&g_last_packet_transfered_timer);
                                                                MUI_AddStatusWindow(cn, temp);
                                                                cn->ci.b_transfering = TRUE;
                                                                cn->ci.b_transfered = FALSE;
                                                                cn->ci.b_waitingfordataport = FALSE;
                                                                
                                                                // if (cn->ci.b_file_transfer == FALSE) {
                                                                // ASD, this send has to be fixed ?
                                                                // should i wait for write flag in socket?
                                                                send(ci->transfer_socket, "\n\n", 2, 0);
                            // }
                                                        }
                                                } else {
/********************************************* ACCEPT *****************************************/
                                                        if (FD_ISSET(ci->listen_socket, &readfd)) {
                                                                // ACTIVE DATA PORT
                                                                
                                                                // Data socket event happened, we are listening, so some1 connected
                                                                command = sizeof(sockaddr);
                                                                
                                                                ci->transfer_socket = accept(ci->listen_socket, &sockaddr, &command);
                                                                if (ci->transfer_socket == INVALID_SOCKET) {
                                                                        snprintf(temp, 511, "STATUS: Unable to accept an incoming connection on port %d", ci->port);
                                                                        MUI_AddStatusWindow(cn, temp);
                                                                } else {
                                                                        snprintf(temp, 511, "STATUS: Accepted an incoming connection on port %d", ci->port);
                                                                        StartTimer(&g_transfer_start_timer);
                                                                        StartTimer(&g_last_packet_transfered_timer);
                                                                        MUI_AddStatusWindow(cn, temp);
                                                                        cn->ci.b_transfering = TRUE;
                                                                        cn->ci.b_transfered = FALSE;
                                                                }
                                                                
                                                                cn->ci.b_waitingfordataport = FALSE;
                                                                CloseListenSocket(cn);
                                                        }
                                                }
                                        }
                                } else if (sel_sock < 0) {
                                        #ifdef DEBUGLV3
                                        local_errno = errno;
                                        DebugOutput("\nMainLoop(): sel_sock < 0\n");
                                        #endif
                                        
                                        optlen = sizeof(ret);
                            getsockopt (ci->cmd_socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
                            #ifdef DEBUGLV3
                                        printf("\nMainLoop(): Error code(ret) = %d string = '%s'\n", ret, GetErrnoDesc(ret));
                                        printf("\nMainLoop(): Error code(errno) = %d string = '%s'\n", local_errno, GetErrnoDesc(local_errno));
                                        printf("\nMainLoop(): Error code(sel_sock) = %d string = '%s'\n", sel_sock, GetErrnoDesc(sel_sock));
                                        #endif
                                } else if (sel_sock == 0) {
                                        #ifdef DEBUGLV3
                                        printf("select() timeout expired, WaitSelect() sigmask = %ld sigmask = %08x\n", sigs, sigs);
                                        #endif
                                }
                        }
                        
                        if (sigs & SIGBREAKF_CTRL_C) {
                                #ifdef DEBUG
                                DebugOutput("MainLoop(): SIGBREAKF_CTRL_C\n");
                                #endif
                                break;
                        }
                }
                
                if (cn->ci.b_waitingfordataport && TimeInSecs(&g_listen_timer) > g_GlobalSettings.SettingsArray[SETTING_CONNECT_TIMEOUT]) {
                        CloseListenSocket(cn);
                        MUI_AddStatusWindow(cn, "STATUS: No connection happened in a timely fashion, closed listen socket");
                }
                
                // Socket related gui stuph
                switch (mui_res) {
                        case ID_CLICKED_CONNDISC:
                                if (cn->ci.b_connected == TRUE) {
                                        FtpDisconnect(cn);
                                } else if (cn->ci.b_connecting == FALSE) {
                                        // Connecting to host
                                        FtpConnect(cn);
                                } else {
                                                MUI_AddStatusWindow(cn, "STATUS: Already trying to connect");
                                }
                        break;
                                        
                        case ID_CLICKED_STOP:
                                        // Stop the command queue
                                        ci->b_processing_queue = FALSE;
                                        if (cn->ci.b_transfering) {
                    AbortFileTransfer(cn);
                    /* This code has been superseded by AbortFileTransfer() which
                       implements it as it as
                    
                    if (ci->b_passive == FALSE)
                                                SendFtpCommand(cn, "ABOR", FALSE);
                                        else {
                                                InvalidateSocket(&cn->ci.transfer_socket);
                                                ci->b_transfer_error = TRUE;
                                                ci->b_transfering = FALSE;
                    }
                    */
                                        } else if (cn->ci.b_connecting) {
                                                InvalidateSocket(&cn->ci.cmd_socket);
                                                cn->ci.b_connecting = FALSE;
                                                MUI_AddStatusWindow(cn, "STATUS: Connection aborted");
                                        } else
                                                MUI_AddStatusWindow(cn, "STATUS: Nothing to abort");
                                        
                                        break;
                                        
                        case ID_MODIFIED_LPATHBOX:
                        case ID_CLICKED_REFRESH_L:
                                        if (RefreshLocalView(cn) == TRUE)
                                                MUI_AddStatusWindow(cn, "STATUS: Refreshed local view");
                                        break;
                                        
                        case ID_DBLCLICKED_LEFT_LISTVIEW:
                                        OnLeftListviewDblClick(cn);
                                        break;
                                        
                        case ID_DBLCLICKED_RIGHT_LISTVIEW:
                                        OnRightListviewDblClick(cn);
                                        break;
                                        
                        case ID_MODIFIED_RPATHBOX:
                                        // Modify the path
                                        if (cn->ci.b_can_send_command) {
                                                get(cn->S_RIGHT_VIEW_PATH, MUIA_String_Contents, &itmp);
                                                SendCWDCMD(cn, (char *) itmp);
                                        }
                                        break;
                                        
                        case ID_CLICKED_REFRESH_R:
                                        InitiateListTransfer(cn);
                                        break;
                                        
                        case ID_CLICKED_QUEUE_PROCESS:
                                        OnQueueProcess(cn);
                                        break;
                                        
                        case ID_CLICKED_QUEUE_STEP:
                                        OnQueueStep(cn);
                                        break;
                                        
                        case ID_CLICKED_CLEAR_QUEUE:
                                        OnClearQueue(cn);
                                        break;
                        
                        case ID_CLICKED_DOWNLOAD:
                                        OnDownloadBtnClick(cn);
                                        break;
                        
                        case ID_CLICKED_UPLOAD:
                                        OnUploadBtnClick(cn);
                                        break;
                        
                        case ID_CLICKED_DELETE:
                                        OnDeleteBtnClick(cn);
                                        break;
                                        
                        case MNID_QUIT:
                        case ID_CLICKED_QUIT:
                                // Disconnecting...
                                if (cn->ci.b_connected == FALSE) {
                                        mui_res = MUIV_Application_ReturnID_Quit;
                                } else {
                                        if (cn->ci.b_transfering) {
                                                // If we are transfering a file, let's abort it
                                                AbortFileTransfer(cn);
                                                // SendFtpCommand(cn, "ABOR", FALSE);
                                                cn->ci.b_request_disconn = TRUE;
                                                cn->ci.b_request_quit = TRUE;
                                        } else {
                                                if (cn->ci.b_can_send_command) {
                                                        // Otherwise, just quit
                                                        SendFtpCommand(cn, "QUIT", FALSE);
                                                        cn->ci.b_request_quit = TRUE;
                                                }
                                        }
                                }
                                break;
                        
                        case ID_SITE_WINDOW_CLICKED_NEW:
                                OnSiteWindowNew();
                                break;
                        
                        case ID_SITE_WINDOW_CLICKED_CHANGE:
                                OnSiteWindowChange();
                                break;

                        case ID_SITE_WINDOW_CLICKED_SAVE:
                                OnSiteWindowSave(cn);
                                break;

                        case ID_SITE_WINDOW_CLICKED_DELETE:
                                OnSiteWindowDelete();
                                break;
                        
                        case ID_SITE_WINDOW_CLICKED_EXIT:
                                OnSiteWindowExit(cn);
                                break;
                        
                        case ID_DBLCLICKED_SITE_WINDOW_HOSTS:
                                OnSiteWindowHostsLVDBLClick(cn);
                                break;
                                
                        case ID_CHANGESEL_SITE_WINDOW_HOSTS:
                                OnSiteWindowHostsSelectChange(cn);
                                break;
                        
                        case ID_SITE_WINDOW_FULL_HOSTS_WINDOW_OK:
                                set(g_AddressBook.Window_HostsFull, MUIA_Window_Open, FALSE);
                                break;
                        
                        case MNID_OPENADDRESSBOOK:
                                set(g_AddressBook.Window, MUIA_Window_Open, TRUE);
                                get(cn->Window, MUIA_Window_Open, &iresult);
                                if ((BOOL) iresult == FALSE) {
                                        MUI_AddStatusWindow(cn, "ERROR: Cannot open address book window");
                                } else {
                                        g_AddressBook.b_new_entry = FALSE;
                                        g_AddressBook.selected_host = 0;
                                        UpdateSiteWindow(0);
                                }
                                break;
                                
                        case MNID_GLOBALSETTINGS:
                                EvalGlobalSettingsWindow();
                                set(g_GlobalSettings.Window, MUIA_Window_Open, TRUE);
                                break;
                                
                        case ID_GLOBAL_SETTINGS_WINDOW_SAVE:
                                OnGlobalSettingsSave();
                                break;
                                
                        case ID_GLOBAL_SETTINGS_WINDOW_CANCEL:
                                set(g_GlobalSettings.Window, MUIA_Window_Open, FALSE);
                                break;
                                
                                /*
                                #ifdef DEBUG
                                printf("\n\n\n\n");
                                ptr = g_left_list_buffer.ptr;
                                clmn = 0;
                                for (i = 0; ((ptr[i] == 0) + (ptr[i+1] == 0) + (ptr[i+2] == 0) + (ptr[i+3] == 0)) != 4; i++) {
                                        if (ptr[i] != 0)
                                                printf("%c", ptr[i]);
                                        else {
                                                if (clmn == 3) {
                                                        clmn = 0;
                                                        printf("\n");
                                                } else {
                                                        clmn++;
                                                        printf(" ");
                                                }
                                        }
                                }
                                printf("\n\n\n\n");
                                #endif
                                break;
                                */
                }
                
                if (mui_res == MUIV_Application_ReturnID_Quit) {
                        #ifdef DEBUGLV2
                        DebugOutput("MainLoop(): goodbye at mui\n");
                        #endif

                        SaveConfig();  

                        break;
                }
        }
    } else {
#ifdef DEBUGLV2
        DebugOutput("MainLoop(): Couldn't open the window\n");
#endif
        printf("Failed to create the Application object\n");
    }

#ifdef DEBUGLV2
    DebugOutput("MainLoop(): goodbye at end!\n");
#endif
    MUI_DisposeObject(app);
    CleanUp();

    return RETURN_OK;
}

void HandleDownload(Connection *cn)
{
        int res = 0, local_errno;
        ClientInfo *ci = &cn->ci;
        char temp[512];
        
        #ifdef DEBUG
        FILE *fp;
        #endif
        
        /******************************************* DOWNLOAD ****************************************/
        // Transfer socket event happened while we are transfering, 
        // we have received data
        if (ci->b_file_transfer == FALSE) {
                #ifdef DEBUG
                DebugOutput("MainLoop(): DIRECTORY TRANSFER\n");
                #endif
                
                if (ci->bytes_transfered+65535 > TRANSFER_BUFFER_SIZE) {
                        #ifdef DEBUG
                        DebugOutput("MainLoop(): ci->bytes_transfered+65535 > TRANSFER_BUFFER_SIZE, sending ABORT\n");
                        #endif
                        AbortFileTransfer(cn);
                        // SendFtpCommand(cn, "ABOR", FALSE);
                }
        
                res = recv(ci->transfer_socket, &cn->transfer_buffer.ptr[ci->bytes_transfered], 65535, 0);
        } else {
                #ifdef DEBUG
                DebugOutput("MainLoop(): FILE TRANSFER\n");
                #endif
                
                // If we are transfering a file, we do not need a big buffer, stuff will be
                // written to disk
                res = recv(ci->transfer_socket, cn->transfer_buffer.ptr, cn->transfer_buffer.size, 0);
        }
        
        if (res > 0) {
                #ifdef DEBUG
                snprintf(temp, 511, "MainLoop(): recv() ha ricevuto '%d' bytes\n", res);
                DebugOutput(temp);
                #endif
                cn->ci.b_last_data = FALSE;
                StartTimer(&g_last_packet_transfered_timer);
                if (ci->b_file_transfer && ci->filehandle) {
                        if (Write(ci->filehandle, cn->transfer_buffer.ptr, res) != res) {
                                local_errno = IoErr();
                                ci->b_file_transfer = FALSE;
                                ci->filehandle = 0;
                                AbortFileTransfer(cn);
                                // SendFtpCommand(cn, "ABOR", FALSE);
                                sprintf(temp, "STATUS: Transfer failed, '%d' error during file write", local_errno);
                                MUI_AddStatusWindow(cn, temp);
                                cn->ci.b_last_data = TRUE;
                        }
                }
                
                #ifdef DEBUG
                fp = fopen("ram:dump.txt", "wb");
                if (fp) {
                        fwrite(cn->transfer_buffer.ptr, 1, res, fp);
                        fclose(fp);
                }
                #endif
                
                ci->bytes_transfered += res;
        } else {
                cn->ci.b_last_data = TRUE;
                if (res == SOCKET_ERROR) {
                        #ifdef DEBUG
                        DebugOutput("MainLoop(): res == SOCKET_ERROR, closing transfer socket\n");
                        #endif
                } else if (res == 0) {
                        #ifdef DEBUG
                        DebugOutput("MainLoop(): res is 0\n");
                        #endif
                        StartTimer(&g_last_packet_transfered_timer);
                } else
                        MUI_AddStatusWindow(cn, "STATUS: I SHOULD NOT BE HERE!");
        }
        
        if (cn->ci.b_last_data) {
                CloseDataSocket(cn);
                MUI_AddStatusWindow(cn, "STATUS: Remote closed transfer socket");
                cn->ci.b_transfered = TRUE;
                ClearTransferFlags(cn);
                #ifdef DEBUG
                snprintf(temp, 511, "MainLoop(): recv_command_stack[STACK_CURRENT] = %d\n", recv_command_stack[STACK_CURRENT]);
                DebugOutput(temp);
                #endif
                if (IsLastSentCmd("LIST")) {
                        // Last sent was directory listing
                        if (GetCommandDigit(recv_command_stack[STACK_CURRENT], 1) == 2) {
                                ProcessDirectoryData(cn);
                                #ifdef DEBUG
                                DebugOutput("MainLoop(): called ProcessDirectoryData() from g_b_last_data if()\n");
                                #endif
                        }
                } else if (IsLastSentCmd("RETR")) {
                        // Last sent was RETR
                        if (GetCommandDigit(recv_command_stack[STACK_CURRENT], 1) == 2)
                                AfterRETR(cn);
                }
        }
}

void HandleUpload(Connection *cn)
{
        int res = 0, local_errno;
        ClientInfo *ci = &cn->ci;
        char temp[512];
        
        /******************************************* UPLOAD ****************************************/
        // Transfer socket event happened, we can send data
        
        /* UPLOAD */
        local_errno = 0;
        if (ci->file_readed == 0) {
                ci->file_readed = Read(ci->filehandle, cn->transfer_buffer.ptr, 65535);
                if (ci->file_readed == 0)
                        ci->b_eof = TRUE;

                local_errno = IoErr();
                ci->file_sent = 0;
        }
        
        if (ci->file_readed == -1) {
                ci->b_file_transfer = FALSE;
                Close(ci->filehandle);
                ci->filehandle = 0;
        
        AbortFileTransfer(cn);
                // SendFtpCommand(cn, "ABOR", FALSE);
                sprintf(temp, "ERROR: Transfer failed, '%d' error during file read", local_errno);
                MUI_AddStatusWindow(cn, temp);
                res = -1;
        } else {
                if (ci->file_readed) {
                        res = send(ci->transfer_socket, &cn->transfer_buffer.ptr[ci->file_sent], ci->file_readed, 0);
                        if (res > 0) {
                                ci->file_readed -= res;
                                ci->file_sent += res;
                                ci->bytes_transfered += res;
                                StartTimer(&g_last_packet_transfered_timer);
                        } else {
                                /* SOCKET ERROR */
                        }
                }
        }
        
        if (ci->b_eof == TRUE || res == 0) {
                // EOF reached
                Close(ci->filehandle);
                ci->filehandle = 0;

                ci->b_file_transfer = FALSE;
                CloseDataSocket(cn);
                cn->ci.b_transfered = TRUE;
        }
        
        if (res < 0) {
                Close(ci->filehandle);
                ci->filehandle = 0;

                ci->b_file_transfer = FALSE;
                CloseDataSocket(cn);
        }
}

/*
BOOL FtpConnect(Connection *cn)
{
        int ret, status, socket = 1, optlen;
        struct sockaddr_in sockaddr;
        ClientInfo *ci = &cn->ci;
        HostInfo *hi = &cn->hi;
        BOOL b_retflag = FALSE;
        struct hostent *ent;
        char temp[512];
        
        if (cn->ci.b_connected == TRUE) {
                #ifdef DEBUG
                DebugOutput("Already connected!\n");
                #endif
                return FALSE;
        }
        
        if (hi->hostname[0] == 0 || hi->port == 0) {
                MUI_AddStatusWindow(cn, "ERROR: Please double click a site from the address book");
                return FALSE;
        }
        
        #ifdef DEBUG
        DebugOutput("Creating socket\n");
        #endif
        
        socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (socket != INVALID_SOCKET)
        {
                #ifdef DEBUG
                printf("socket = %d\n", socket);
                DebugOutput("Socket created\n");
                #endif
                #ifdef DEBUG
                DebugOutput(temp);
                DebugOutput("\n");
                #endif
                
                // server address
                sockaddr.sin_family = AF_INET;
                sockaddr.sin_addr.s_addr = INADDR_ANY;
                sockaddr.sin_port = 0;
                
                status = bind(socket, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in));
                if (status != INVALID_SOCKET)
                {
                        #ifdef DEBUG
                        DebugOutput("Socket binded\n");
                        #endif
                        
                        if (SetBlocking(socket, FALSE) == FALSE) {
                                // If SetBlocking fails, we can't continue because later
                                // code is made for non blocking sockets
                                MUI_AddStatusWindow(cn->LV_STATUS, "ERROR: SetBlocking() failed, cannot change socket status");
                                CleanUp();
                                ci->cmd_socket = INVALID_SOCKET;
                                return FALSE;
                        }
                        #ifdef DEBUG
                        DebugOutput("Socket set to non blocking\n");
                        #endif
                        
                        ent = gethostbyname(hi->hostname);
                        if (ent)
                                CopyMem(ent->h_addr,(char *)&sockaddr.sin_addr.s_addr, ent->h_length);
                        else
                                sockaddr.sin_addr.s_addr = inet_addr(hi->hostname);
                        
                        sockaddr.sin_family = AF_INET;
                        sockaddr.sin_port = htons(hi->port);
                        
                        #ifdef DEBUG
                        DebugOutput("Trying to connect..\n");
                        #endif
                        
                        optlen = sockaddr.sin_addr.s_addr;
                        #ifdef __AROS__
                        BSwapDWORD(&optlen, &optlen);
                        #endif
                        
                        snprintf(temp, 511, "STATUS: Connecting to %s (%s)", hi->hostname, IpToStr(optlen, '.'));
                        MUI_AddStatusWindow(cn, temp);
                        ret = connect(socket, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in));
                        if (ret == 0) {
                                // Connect already connected us to the ftp server
                                #ifdef DEBUG
                                DebugOutput("Already accepted a connection\n");
                                #endif
                                GetConnectionInfo(ci);
                        } else if (ret == SOCKET_ERROR) {
                    #ifdef DEBUG
                                int local_errno = errno;
                                #endif
                                
                                optlen = sizeof(ret);
                    getsockopt (socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
                    
                    #ifdef DEBUG
                                DebugOutput(GetErrnoDesc(ret));
                                printf("\nError code = %d\n", ret);
                                DebugOutput(GetErrnoDesc(local_errno));
                                printf("\nErrno = %d\n", local_errno);
                                #endif
                                
                                // Connection is probably pending
                                #ifdef DEBUG
                                DebugOutput("Connection wasn't estabilished early\n");
                                #endif
                                b_retflag = TRUE;
                                StartTimer(&g_trying_connect_timer);
                        }
                        ci->cmd_socket = socket;
                } else			
                        MUI_AddStatusWindow(cn, "ERROR: Socket bind() failed");
        }
         else
                MUI_AddStatusWindow(cn, "ERROR: Socket creation error");
        
        cn->ci.b_connecting = b_retflag;
        #ifdef DEBUG
        if (cn->ci.b_connecting)
                DebugOutput("Connecting!!!\n");
        else
                DebugOutput("Not connecting :(!!! \n");
        #endif
        
        return b_retflag;
}
*/

void DisplaySockErrs(Connection *cn, CSOCK *csock)
{
        char temp[512];
        
        switch (csock->csockerr) {
                case CSOCKERR_SETNONBLOCKFAILED:
                        MUI_AddStatusWindow(cn->LV_STATUS, "ERROR: SetBlocking() failed, cannot change socket status");
                        break;
                                
                case CSOCKERR_SOCKCREATFAILED:
                        MUI_AddStatusWindow(cn, "ERROR: Socket creation error");
                        break;
                                
                case CSOCKERR_BINDFAILED:
                        MUI_AddStatusWindow(cn, "ERROR: Socket bind() failed");
                        break;
                                
                case CSOCKERR_WRONGPARAMS:
                        MUI_AddStatusWindow(cn, "ERROR: Please double click a site from the address book");
                        break;
                                
                case CSOCKERR_NATIVE:
                        sprintf(temp, "ERROR: socket error '%s'", strerror(csock->sockerr));
                        MUI_AddStatusWindow(cn, temp);
                        break;
        }
}

BOOL FtpConnect(Connection *cn)
{
        ClientInfo *ci = &cn->ci;
        BOOL b_retflag = FALSE;
        HostInfo *hi = &cn->hi;
        struct hostent *ent;
        char temp[512];
        CSOCK csock;
        int ret;
        
        if (cn->ci.b_connected == TRUE) {
                #ifdef DEBUG
                DebugOutput("FtpConnect(): Already connected!\n");
                #endif
                return FALSE;
        }
        
        // Resolve site hostname
        ent = gethostbyname(hi->hostname);
        if (ent) {
                CopyMem(ent->h_addr, &cn->hi.ip_i, sizeof(int));
        } else {
                cn->hi.ip_i = inet_addr(hi->hostname);
        }
                
        // 
        ret = cn->hi.ip_i;
        
        #ifdef CPU_LITTLE_ENDIAN
        BSwapDWORD(&ret, &ret);
        #endif
        
        if (ret != 0) {
                snprintf(temp, 511, "STATUS: Connecting to %s (%s)", hi->hostname, IpToStr(ret, '.'));
                MUI_AddStatusWindow(cn, temp);
        }
        
        csock.ip_i = cn->hi.ip_i;
        csock.port = cn->hi.port;
        ret = SockConnect(&csock);
        ci->cmd_socket = csock.socket;
        if (ret != CSOCKSTATUS_ERROR) {
                // If we already connected, we need to get ConnectionInfo here
                if (ret == CSOCKSTATUS_EARLYCONNECT) {
                        AcknowledgeConnection(cn);
                } else		
                        cn->ci.b_connecting = TRUE;
                
                ci->cmd_socket = csock.socket;
                b_retflag = TRUE;
        } else {
                DisplaySockErrs(cn, &csock);
        }
        
        return b_retflag;
}

int SockConnect(CSOCK *csock)
{
    int ret, status, socket = 1;
    struct sockaddr_in sockaddr;

    csock->socket = INVALID_SOCKET;
    csock->csockerr = 0;
    csock->sockerr  = 0;

    if (csock->ip_i == 0 || csock->port == 0) {
            csock->csockerr = CSOCKERR_WRONGPARAMS;
            return CSOCKSTATUS_ERROR;
    }

#ifdef DEBUG
    DebugOutput("SockConnect()\n");
#endif

    socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket != INVALID_SOCKET)
    {
#ifdef DEBUG
        printf("SockConnect(): socket = %d\n", socket);
        DebugOutput("SockConnect(): Socket created\n");
#endif
        csock->socket = socket;

        /* server address  */
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        sockaddr.sin_port = 0;

        status = bind(socket, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in));
        if (status != INVALID_SOCKET)
        {
#ifdef DEBUG
            DebugOutput("SockConnect(): Socket binded\n");
#endif

            if (SetBlocking(socket, FALSE) == FALSE) {
                // If SetBlocking fails, we can't continue because later
                // code is made for non blocking sockets
                csock->csockerr = CSOCKERR_SETNONBLOCKFAILED;
                return CSOCKSTATUS_ERROR;
            }

#ifdef DEBUG
            DebugOutput("SockConnect(): Socket set to non blocking\n");
            printf("SockConnect() connecting to %s\n", IpToStr(csock->ip_i, '.'));
#endif

            sockaddr.sin_addr.s_addr = csock->ip_i;
            sockaddr.sin_family = AF_INET;
            sockaddr.sin_port = htons(csock->port);

#ifdef DEBUG
            DebugOutput("SockConnect(): Trying to connect..\n");
#endif

            ret = connect(socket, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in));
            if (ret == -1) {
                int local_errno = errno;

#ifdef DEBUG
                printf("SockConnect(): connect() = %d\n", ret);
                printf("SockConnect(): Errno = %d\n", local_errno);
#endif

#ifdef __AROS__
                // ASD: Shouldn't local_errno be EINPROGRESS ?...
                if (local_errno == 0)
                    return CSOCKSTATUS_PENDING;
#elif __MORPHOS__
                if (local_errno == EINPROGRESS)
                    return CSOCKSTATUS_PENDING;
#endif

                csock->csockerr = CSOCKERR_NATIVE;
                csock->sockerr  = ret;
                csock->socket = socket;
                return CSOCKSTATUS_ERROR;
            } else if (ret == 0) {
                // Connect already connected us to the ftp server
#ifdef DEBUG
                DebugOutput("SockConnect(): Already accepted a connection\n");
#endif

                csock->socket = socket;
                return CSOCKSTATUS_EARLYCONNECT;
            } else {
                csock->csockerr = CSOCKERR_NATIVE;
                csock->sockerr  = errno;
                csock->socket = socket;
                return CSOCKSTATUS_ERROR;
            }
        } else
            csock->csockerr = CSOCKERR_BINDFAILED;
    } else
        csock->csockerr = CSOCKERR_SOCKCREATFAILED;
    
    return CSOCKSTATUS_ERROR;
}

BOOL CheckSocket(SOCKET socket)
{
    int ret, optlen;
#ifdef DEBUGLV2
    char temp[512];
#endif

    // Let's check SO_ERROR
    optlen = sizeof(ret);
    if (getsockopt (socket, SOL_SOCKET, SO_ERROR, &ret, &optlen) != SOCKET_ERROR) {
        if (ret == 0) {
#ifdef DEBUGLV2
            DebugOutput("CheckSocket(): getsockopt() no error\n");
#endif
            // No error
            return TRUE;
        }

#ifdef DEBUGLV2
        snprintf(temp, 511, "\nCheckSocket(): Error code(ret) = %d Error code(hex(ret)) %08x string = '%s'\n", ret, ret, GetErrnoDesc(ret));
        DebugOutput(temp);
#endif
        return TRUE;
    } 
#ifdef DEBUG
    else
        DebugOutput("CheckSocket(): getsockopt() failed\n");
#endif

    // Errors
    return FALSE;
}

BOOL CheckConnect(Connection *cn)
{
    ClientInfo *ci = &cn->ci;
    HostInfo   *hi = &cn->hi;
    BOOL b_retflag = FALSE;
    int ret, optlen;
    char temp[512];

    // Socket has returned Writeable, but we still do not known if connect worked
    // we need to check SO_ERROR as well
    optlen = sizeof(ret);
    getsockopt (ci->cmd_socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
    if (ret == 0) {
        snprintf(temp, 511, "STATUS: Connected to %s", hi->hostname);
        MUI_AddStatusWindow(cn, temp);
        b_retflag = TRUE;
    } else {
        snprintf(temp, 511, "STATUS: Connection failed, error: '%s'", GetErrnoDesc(ret));
        MUI_AddStatusWindow(cn, temp);
#ifdef DEBUG
        DebugOutput("CheckConnect(): Connection failed, ");
        DebugOutput(GetErrnoDesc(ret));
        DebugOutput("\n");
#endif
    }

    cn->ci.b_connecting = FALSE;
    SetConnected(cn, b_retflag);
    return b_retflag;
}

void FormatFtpListing(Connection *cn, buffer *buffer, BOOL b_last_data)
{
    ClientInfo *ci = &cn->ci;
    int i, start_of_line = 0;
    char *dest = buffer->ptr;
#ifdef DEBUG
    char temp[512];
#endif

    // First line always start at 0, of course
    cn->rv.lines_start[0] = 0;
    cn->rv.num_lines = 0;

    for (i = 0; dest[i] != 0 && i < ci->bytes_transfered; i++) {
        if (dest[i] == '\r') {
            dest[i] = 0;
            continue;
        }

        if (dest[i] == '\n') {
            // Terminate this line
            dest[i] = 0;
            
            // Add the start of the line in the lines database
            cn->rv.num_lines++;
            cn->rv.lines_start[cn->rv.num_lines-1] = start_of_line;
            start_of_line = 1+i;
        }
    }

#ifdef DEBUG
    snprintf(temp, 511, "FormatFtpListing(): parsed %d bytes", i);
    DebugOutput(temp);
#endif
}

void GetPathFromRawData(char *text)
{
    char *dest = g_temp;
    BOOL b_copy = FALSE;
    int i;

    for (i = 0; text[i] != 0; i++) {
        if (b_copy && text[i] != '"')
            *dest++ = text[i];
        
        if (text[i] == '"') {
            if (b_copy == FALSE)
                b_copy = TRUE;
            else
                b_copy = FALSE;
        }
    }

    *dest++ = 0;
}

int FtpDisconnect(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    // Disconnecting...
    if (ci->b_transfering) {
        // If we are transfering a file, let's abort it
        AbortFileTransfer(cn);
        // SendFtpCommand(cn, "ABOR", FALSE);
        ci->b_request_disconn = TRUE;
    } else {
        // Otherwise, just quit
        SendFtpCommand(cn, "QUIT", FALSE);
    }

#ifdef DEBUG
    DebugOutput("FtpDisconnect(): disconnected\n");
#endif

    return 0;
}

void InvalidateSocket(int *socket)
{
#ifdef DEBUG
    char temp[512];
    sprintf(temp, "InvalidateSocket(): Closing socket %d\n", *socket);
    DebugOutput(temp);
#endif

    if (*socket != INVALID_SOCKET) {
        shutdown(*socket, 0);
        CloseSocket(*socket);
    }

    *socket = INVALID_SOCKET;
}

int SetTransferMode(Connection *cn, char mode)
{
    char cmd[512], t2[3];
    int answer;

    t2[0] = mode;	t2[1] = 0;
    caf_strncpy(cmd, "TYPE ", 511);
    caf_strncat(cmd, t2, 511);
    answer = SendFtpCommand(cn, cmd, FALSE);
    return answer;
}

int SetTransferPort(Connection *cn)
{
    ClientInfo *ci = &cn->ci;
    int answer, port;
    char cmd[512];

    port = ci->port;
    snprintf(cmd, 511, "PORT %s,%02d,%02d", ci->ip_s, (port >> 8)&0xFF, port & 0xFF);
    answer = SendFtpCommand(cn, cmd, FALSE);
    ci->b_passive = FALSE;
    return answer;
}

int SendLISTCMD(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    ci->b_file_transfer = FALSE;
    cn->ci.b_directory_processed = FALSE;
    cn->ci.b_last_data = FALSE;
#ifdef DEBUG
    DebugOutput("SendLISTCMD(): b_directory_processed = FALSE\n");
#endif
    MUI_ClearListbox(cn->rv.ListView);
    return SendFtpCommand(cn, "LIST", FALSE);
}

int SendRETRCMD(Connection *cn, char *filename)
{
    ClientInfo *ci = &cn->ci;
    char temp[512];
    IPTR itemp;
    int chr;

    // Open the file on disk
    get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itemp);

    chr = caf_getlastchar((char *) itemp);
    if (chr == '/' || chr == ':')
        snprintf(temp, 511, "%s%s", (char *) itemp, filename);
    else
        snprintf(temp, 511, "%s/%s",(char *) itemp, filename);

    ci->filehandle = Open(temp, MODE_NEWFILE);
    if (ci->filehandle == 0) {
        ci->b_file_transfer = FALSE;
        cn->ci.b_last_data = FALSE;
        MUI_AddStatusWindow(cn, "ERROR: Cannot create the local file");
        return 0;
    }

    ci->b_eof = FALSE;
    ci->b_file_transfer = TRUE;
    cn->ci.b_last_data = FALSE;
    snprintf(temp, 511, "RETR %s", filename);
#ifdef DEBUGLV2
    DebugOutput(temp);
    DebugOutput("SendRETRCMD\n");
#endif
    return SendFtpCommand(cn, temp, FALSE);
}

int SendSTORCMD(Connection *cn, char *filename)
{
    char temp[512], temp2[512], temp3[512];
    ClientInfo *ci = &cn->ci;
    int chr, ioerr;
    IPTR itemp;

    // Open the file on disk
    if (cn->ci.b_processing_queue == TRUE)
        itemp = (IPTR) &cn->lv.CurrentPath;
    else
        get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itemp);

    chr = caf_getlastchar((char *) itemp);
    if (chr == '/' || chr == ':')
        snprintf(temp, 511, "%s%s", (char *) itemp, filename);
    else
        snprintf(temp, 511, "%s/%s",(char *) itemp, filename);

    ci->file_readed = ci->file_size = ci->file_sent = 0;
    ci->filehandle = Open(temp, MODE_OLDFILE);
    if (ci->filehandle == 0) {
        ioerr = IoErr();
        Fault(ioerr, "", temp3, 511);
        ci->b_file_transfer = FALSE;
        sprintf(temp2, "ERROR: Cannot open local file '%s', Open() failed, error '%s'", temp, temp3);
        MUI_AddStatusWindow(cn, temp2);
        CloseListenSocket(cn);
        CloseDataSocket(cn);
        return 0;
    }

    ci->b_eof = FALSE;
    ci->b_file_transfer = TRUE;
    ci->b_file_download = FALSE;
    cn->ci.b_last_data = FALSE;
    snprintf(temp, 511, "STOR %s", filename);
#ifdef DEBUGLV2
    DebugOutput(temp);
    DebugOutput("SendSTORCMD\n");
#endif
    return SendFtpCommand(cn, temp, FALSE);
}

int SendPWDCMD(Connection *cn)
{
    return SendFtpCommand(cn, "PWD", FALSE);
}

int SendCWDCMD(Connection *cn, char *path)
{
    char temp[511];

    snprintf(temp, 511, "CWD %s", path);
    return SendFtpCommand(cn, temp, FALSE);
}

int GetFtpCommandValue(char *buffer)
{
    char command[32];
    int slen, numba;

    slen = caf_strlen(buffer);
    command[0] = 0;

    // The buffer string must be at lest four bytes
    if (slen < 4)
    {
        // Cannot get the command, string too short
        return 0;
    }

    // Copy the command to the receiving string
    command[0] = buffer[0];
    command[1] = buffer[1];
    command[2] = buffer[2];
    command[3] = buffer[3];
    command[4] = 0;

    numba = caf_atol(command);
    if (command[3] == '-')
        numba = -numba;

    return numba;
}

int GetFtpCommand(struct buffer *buffer, Connection *cn)
{
    BOOL need_break_after_newline = FALSE, done = FALSE;
    int readed = 0, linestart = 0, command = 0, ret = -1;
    ClientInfo *ci = &cn->ci;

    // First line always start at 0, of course
    cn->rv.lines_start[0] = 0;
    cn->rv.num_lines = 1;

    caf_memset(buffer->ptr, 0, buffer->size);

//	while ((ret = ReadSocketNB(&buffer->ptr[readed], ci->cmd_socket, 1, g_GlobalSettings.socket_timeout, 0)) == TRUE) {
    while (CanReadSocket(ci->cmd_socket, 0, 0) == SOCKET_SELECT_OK) {
        ret = recv(ci->cmd_socket, &buffer->ptr[readed], 1, 0);
        if (need_break_after_newline == FALSE) {
            command = GetFtpCommandValue(&buffer->ptr[linestart]);
            if (command > 0) {
                // We found a valid command, go out
                need_break_after_newline = TRUE;
            }
        }

        if (buffer->ptr[readed] == '\n') {
            // Terminate this line
            buffer->ptr[readed] = 0;

            // We found a new line
            if (need_break_after_newline == FALSE) {
                // If the server is still sending data, we add each line in the array
                linestart = 1+readed;
                cn->rv.num_lines++;
                cn->rv.lines_start[cn->rv.num_lines-1] = linestart;
            } else {
                // Server finished sending data
                done = TRUE;
                break;
            }
        }

        // Skip carriage return
        if (buffer->ptr[readed] == '\r')
            continue;

        readed++;
        if (readed == buffer->size-3)
            break;
    }

    if (ret == 0) {
        // Command socket has been closed, let's go away
        CleanSockAndFlags(cn);
        MUI_AddStatusWindow(cn, "STATUS: closed control connection");
    }

#ifdef DEBUG
    if (!readed) {
        DebugOutput("GetFtpCommand(): Readed = 0\n");
    }

    if (!done) {
        DebugOutput("GetFtpCommand(): Valid command not found\n");
        DebugOutput(buffer->ptr);
    } 
#else
    (void)done; // Unused
#endif
    
    return command;
}

int SendFtpCommand(Connection *cn, char *CMD, BOOL b_block)
{
    ClientInfo *ci = &cn->ci;
    int to_write, written;
    char temp[512];

    if (strcmp(CMD, "ABOR")) {
        if (cn->ci.b_can_send_command == FALSE) {
            if (cn->ci.b_connected == TRUE) {
                MUI_AddStatusWindow(cn, STR_CANT_SEND);
                return ANSWER_PENDING;
            } else {
                MUI_AddStatusWindow(cn, STR_CANT_SEND_NOT_CONNECTED);
                return NOT_CONNECTED;
            }
        }
    }

    // Send Command
    caf_strncpy(temp, CMD, 511);
    MUI_AddListboxCMDText(cn, temp);
    caf_strncat(temp, "\r\n", 511);

    to_write = caf_strlen(temp); 
    written = 0;
    while ((CanWriteSocket(ci->cmd_socket, 0, 0) == SOCKET_SELECT_OK) && (to_write > written)) {
        written += send(ci->cmd_socket, &temp[written], to_write-written, 0);
    }

    SentStackPush(CMD);

    // Each command needs to wait for server response
    // ci->b_can_send_command = FALSE;
    return 0;
}

BOOL ProcessError(Connection *cn, int command)
{
    switch (command) {
        case SELECT_ERROR:
                MUI_AddStatusWindow(cn, "STATUS: cannot send command, select() failed");
                return TRUE;

        case ANSWER_PENDING:
                MUI_AddStatusWindow(cn, STR_CANT_SEND);
                return TRUE;

                // General FTP Error
    }

    return FALSE;
}

void ProcessCommand(Connection *cn, int command, int *mui_res)
{
    ClientInfo *ci = &cn->ci;
    HostInfo *hi = &cn->hi;
    char temp[512], digit;
    BOOL b_goon;
    int ret;

    digit = GetCommandDigit(command, 1);
    switch (digit) {
        case 1:
/*********************************************	DIGIT 1 *****************************************/
                // Positive Preliminary reply, expect another reply before sending commands
                cn->ci.b_can_send_command = FALSE;
                if (IsLastSentCmd("LIST")) {
                } else
                    if (IsLastSentCmd("RETR")) {
                    }
                break;
                
        case 2:
/*********************************************	DIGIT 2 *****************************************/
                // Positive Completion reply, we can send commands
                cn->ci.b_can_send_command = TRUE;
                if (IsLastSentCmd("CONN")) {
                    caf_strncpy(temp, "user ", 511);	caf_strncat(temp, hi->username, 511);
                    SendFtpCommand(cn, temp, FALSE);
                } else if (IsLastSentCmd("pass")) {
                    cn->ci.b_pass_accepted = TRUE;
                    SendPWDCMD(cn);
                } else if (IsLastSentCmd("PWD")) {
                    GetPathFromRawData(cn->cmd_buffer.ptr);
                    set(cn->S_RIGHT_VIEW_PATH, MUIA_String_Contents, (IPTR) g_temp);
                    InitiateListTransfer(cn);
                } else if (IsLastSentCmd("LIST") && cn->ci.b_last_data == TRUE) {
#ifdef DEBUG
                    DebugOutput("ProcessCommand(): ProcessDirectoryData() called from 226 case\n");
#endif
                    ProcessDirectoryData(cn);
                } else if (IsLastSentCmd("ABOR") && cn->ci.b_request_disconn == TRUE) {
                    SendFtpCommand(cn, "QUIT", FALSE);
                } else if (IsLastSentCmd("STOR")) {
                    AfterSTOR(cn);
                } else if (IsLastSentCmd("RETR")) {
                    AfterRETR(cn);
                } else if (IsLastSentCmd("DELE")) {
                    AfterDELE(cn);
                } else if (IsLastSentCmd("CWD") || IsLastSentCmd("CDUP")) {
                    AfterCWD(cn);
                } else if (IsLastSentCmd("MKD")) {
                    AfterMKD(cn);
                } else if (IsLastSentCmd("ABOR") && cn->ci.b_request_disconn == TRUE) {
                    SendFtpCommand(cn, "QUIT", FALSE);
                } else if (IsLastSentCmd("QUIT")) {
                    CleanSockAndFlags(cn);
                    if (cn->ci.b_request_quit)
                        *mui_res = MUIV_Application_ReturnID_Quit;
                } else if (IsLastSentCmd("PORT") || IsLastSentCmd("PASV")) {
                    b_goon = TRUE;
                    if (hi->conn_type == CTYPE_PASV) {
#ifdef DEBUG
                        printf("conn_type = CTYPE_PASV\n");
#endif

                        // Gets detail for passive connection
                        if (GatherIpPortNumber(cn) != 0) {
                            // Tries to connect to specified ip - port
                            // ASD
                            ret = PasvConnection(cn);
                            if (ret == CSOCKSTATUS_EARLYCONNECT) {
                                // connect() connected at first call
                                ci->transfer_socket = cn->hi.pasvsettings.socket;
                                ci->b_waitingfordataport = TRUE;
                                StartTimer(&g_listen_timer);
                                b_goon = TRUE;
#ifdef DEBUG
                                printf("PasvConnection(): CSOCKSTATUS_EARLYCONNECT\n");
#endif
                            } else if (ret == CSOCKSTATUS_PENDING) {
                                // connect() is in progress, pending...
                                // we have to wait the socket in the select() loop
                                ci->transfer_socket = cn->hi.pasvsettings.socket;
                                ci->b_waitingfordataport = TRUE;
                                StartTimer(&g_listen_timer);
                                b_goon = TRUE;
#ifdef DEBUG
                                printf("PasvConnection(): CSOCKSTATUS_PENDING\n");
#endif
                            } else if (ret == CSOCKSTATUS_ERROR) {
                                // connect() failed
                                DisplaySockErrs(cn, &cn->hi.pasvsettings);
                                b_goon = FALSE;
                            }
                        } else
                            b_goon = FALSE;
                    }

                    if (b_goon) {
                        if (cn->ci.b_request_list)
                            SendLISTCMD(cn);
                        else if (cn->ci.b_request_download) {
                            if (ci->b_file_download)
                                SendRETRCMD(cn, ci->filename);
                            else {
                                SendSTORCMD(cn, ci->filename);
                            }
                        }
                    }
                } else if (IsLastSentCmd("TYPE")) {
                    if (hi->conn_type == CTYPE_ACTV) {
                        // Active transfer mode
                        SetTransferPort(cn);
                    } else {
                        // Passive transfer mode
                        SendPASVCMD(cn);
                    }
                }
                break;

        case 3:
                // Positive Intermediate reply
                cn->ci.b_can_send_command = TRUE;
/*********************************************	DIGIT 3 *****************************************/
                if (IsLastSentCmd("user")) {
                    cn->ci.b_user_accepted = TRUE;
                    caf_strncpy(temp, "pass ", 511);	caf_strncat(temp, hi->password, 511);
                    SendFtpCommand(cn, temp, FALSE);
                }
                break;

        case 4:
                // Transient Negative Completion reply;
                cn->ci.b_can_send_command = TRUE;
/********************************************* DIGIT 4 *****************************************/
                if (IsLastSentCmd("RETR") || IsLastSentCmd("LIST") || IsLastSentCmd("STOR")) {
                    if (ci->port != 0)
                        snprintf(temp, 511, "STATUS: Server error encountered, Transfer failed, closed port %d", ci->port); 
                    else
                        caf_strncpy(temp, "STATUS: Server error encountered, Transfer failed", 512); 
                    
                    MUI_AddStatusWindow(cn, temp);
                    CloseListenSocket(cn);
                    CloseDataSocket(cn);
                } else
                    if (IsLastSentCmd("DELE")) {
                    }
                break;

        case 5:
                // Permanent Negative Completion reply
                cn->ci.b_can_send_command = TRUE;
/********************************************* DIGIT 5 *****************************************/
                if (IsLastSentCmd("RETR") || IsLastSentCmd("LIST") || IsLastSentCmd("STOR")) {
                    snprintf(temp, 511, "STATUS: Server error encountered, file unavailable, closed port %d", ci->port); 
                    MUI_AddStatusWindow(cn, temp);
                    CloseListenSocket(cn);
                    CloseDataSocket(cn);
                } else if (IsLastSentCmd("DELE")) {
                    if (ci->b_processing_queue) {
                    }
                }
                break;
    }
    
#ifdef DEBUGLV2
    sprintf(temp, "ProcessCommand(): CMD(%d) = %s digit = %d\n", command, (ci->b_can_send_command) ? "TRUE" : "FALSE", digit);
    DebugOutput(temp);
#endif
}

BOOL OpenListenSocket(Connection *cn)
{
    int status, socket = INVALID_SOCKET, ret;
    struct sockaddr_in sockaddr;
    ClientInfo *ci = &cn->ci;
    BOOL b_ret = FALSE;
    char temp[512];
    u_long opt;

    if (cn->ci.b_waitingfordataport == TRUE) {
        MUI_AddStatusWindow(cn, "WARNING: Already listening for connection...");
        return FALSE;
    }

    cn->ci.b_transfering = FALSE;
    ci->bytes_transfered = 0;
    InvalidateSocket(&ci->listen_socket);
    InvalidateSocket(&ci->transfer_socket);
#ifdef DEBUG
    DebugOutput("OpenListenSocket(): Creating listen socket\n");
#endif
    socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket != INVALID_SOCKET)
    {	
#ifdef DEBUG
        DebugOutput("OpenListenSocket(): Socket created\n");
#endif
        
        /* server address  */
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        sockaddr.sin_port = 0;
        
        status = bind(socket, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in));
        if (status != INVALID_SOCKET)
        {
#ifdef DEBUG
            DebugOutput("OpenListenSocket(): Socket binded\n");
#endif
            
            // Get the port of the socket 
            status = sizeof(struct sockaddr);
            getsockname(socket, (struct sockaddr *) &sockaddr, &status);
            ci->port = htons(sockaddr.sin_port);
            
            if (SetBlocking(socket, FALSE) == FALSE) {
                // If SetBlocking fails, we can't continue because later
                // code is made for non blocking sockets
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): SetBlocking() failed\n");
#endif
                CleanUp();
                return FALSE;
            }
            
            opt = 1;
            if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): Cannot set addr to reuse\n");
#endif
            } else {
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): Addr set to reuse\n");
#endif
            }
            
#ifndef __AROS__
            opt = 1;
            if (setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): Cannot set port to reuse\n");
#endif
            } else {
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): Port set to reuse\n");
#endif
            }
#endif
            
            // Setting the socket to listen state
            ret = listen(socket, 1);
            if (ret == 0) {
                cn->ci.b_waitingfordataport = TRUE;
                snprintf(temp, 511, "STATUS: Listening on port %d", ci->port);
                MUI_AddStatusWindow(cn, temp);
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): Socket listening()....\n");
#endif
                b_ret = TRUE;
                StartTimer(&g_listen_timer);
            } else {
                cn->ci.b_waitingfordataport = FALSE;
                CloseSocket(socket);
                socket = INVALID_SOCKET;
#ifdef DEBUG
                DebugOutput("OpenListenSocket(): Listen() failed on data socket\n");
#endif
                snprintf(temp, 511, "STATUS: failed listening on port %d", ci->port);
                ci->port = 0;
                MUI_AddStatusWindow(cn, temp);
            }
        }
    }
    
    ci->listen_socket = socket;
    return b_ret;
}

BOOL PasvConnection(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    cn->ci.b_transfering = FALSE;
    ci->bytes_transfered = 0;
    InvalidateSocket(&ci->listen_socket);
    InvalidateSocket(&ci->transfer_socket);
#ifdef DEBUG
    printf("PasvConnection(): Connecting to %s port %d\n", IpToStr(cn->hi.pasvsettings.ip_i, '.'), cn->hi.pasvsettings.port);
#endif
    return SockConnect(&cn->hi.pasvsettings);
}

BOOL CloseListenSocket(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

#ifdef DEBUG
    DebugOutput("CloseListenSocket()\n");
#endif

    InvalidateSocket(&ci->listen_socket);
    cn->ci.b_waitingfordataport = FALSE;
    return TRUE;
}

BOOL CloseDataSocket(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    // Is there an opened file? close it..
    if (ci->filehandle) {
        Close(ci->filehandle);
        ci->filehandle = 0;
    }

    // ASDASDASDASD
    if (ci->b_passive == FALSE) {
        if (ci->transfer_socket == INVALID_SOCKET) {
#ifdef DEBUG
            DebugOutput("CloseDataSocket(): socket already closed\n");
#endif
            return FALSE;
        }

        InvalidateSocket(&ci->transfer_socket);
    }

    cn->ci.b_transfering = FALSE;
    ci->port = ci->file_readed = ci->file_size = ci->file_sent = 0;

    //	
#ifdef DEBUG
    DebugOutput("CloseDataSocket(): closed socket\n");
#endif

    return TRUE;	
}

char *IpToStr(int ip, char sepchar)
{
    static char temp[512];

    snprintf(temp, 511, "%d%c%d%c%d%c%d", ((ip>>24)&0xFF), sepchar, ((ip>>16)&0xFF), sepchar, ((ip>>8)&0xFF), sepchar, ip&0xFF);
    return temp;
}

BOOL RefreshLocalView(Connection *cn)
{
    char path[512], temp[512], lastchar;
    struct FileInfoBlock *fib = 0;
    LocalView *lv = &cn->lv;
    ViewColumn vc, *vcptr;
    BOOL b_ret = TRUE;
    IPTR itemp;
    BPTR lock;

    get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itemp);
    caf_strncpy(path, (char *) itemp, 512);

    fib = AllocDosObject(DOS_FIB, NULL);
    if (!fib) {
        MUI_AddStatusWindow(cn, "LOCAL: AllocDosObject() failed, cannot retrieve directory listing");
        return FALSE;
    }

    lock = Lock(path, ACCESS_READ);
    if (lock) {
        if (Examine(lock, fib) != FALSE) {
            if (fib->fib_DirEntryType == ST_USERDIR || fib->fib_DirEntryType == ST_ROOT) {
                // Sets the current directory in local structures
                lastchar = caf_getlastchar((char *) itemp);
                if (lastchar != '/' && lastchar != ':') {
                    caf_strncat(path, "/", 512);
                    set(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, path);
                    strcpy(cn->lv.CurrentPath, path);
                }

                // Make the list quiet and clear it
                set(lv->ListView, MUIA_NList_Quiet, TRUE);
                MUI_ClearListbox(lv->ListView);

                // Open the listview buffer
                BufferOpen(&lv->ListBuffer, TRUE);

                while ((ExNext(lock, fib) != DOSFALSE)) {
                    vc.name = BufferAddStr(&lv->ListBuffer, fib->fib_FileName);
                    if (fib->fib_DirEntryType == ST_USERDIR) {
                        vc.size = BufferAddStr(&lv->ListBuffer, "<DIR>");
                    } else {
                        snprintf(temp, 511, "%ld", (long)fib->fib_Size);
                        vc.size = BufferAddStr(&lv->ListBuffer, temp);
                    }
    
                    DecodeBitFlags(temp, fib->fib_Protection);
                    vc.flags = BufferAddStr(&lv->ListBuffer, temp);
                    DateStampToStr(temp, &fib->fib_Date);
                    vc.date = BufferAddStr(&lv->ListBuffer, temp);
                    if (fib->fib_DirEntryType == ST_USERDIR)
                        vc.is_dir = TRUE;
                    else
                        vc.is_dir = FALSE;

                    vcptr = BufferAddStruct(&lv->ListBuffer, &vc, sizeof(ViewColumn));

                    if (vc.name == 0 || vc.size == 0 || vc.flags == 0 || vc.date == 0 || vcptr == 0) {
                        MUI_AddStatusWindow(cn, "ERROR: Local listview buffer full, cannot continue directory scan");
                        break;
                    }

                    if (fib->fib_DirEntryType == ST_USERDIR)
                        DoMethod(lv->ListView, MUIM_NList_InsertSingle, (IPTR *) vcptr, MUIV_NList_Insert_Top);
                    else
                        DoMethod(lv->ListView, MUIM_NList_InsertSingle, (IPTR *) vcptr, MUIV_NList_Insert_Bottom);
                }

                // Add CDUP
                vc.name = "..";
                vc.size = BufferAddStr(&lv->ListBuffer, "<DIR>");
                vc.flags = "RWED";
                vc.date = "N/A";
                vc.is_dir = TRUE;
                vcptr = BufferAddStruct(&lv->ListBuffer, &vc, sizeof(ViewColumn));
                if (vcptr)
                    DoMethod(lv->ListView, MUIM_NList_InsertSingle, (IPTR *) vcptr, MUIV_NList_Insert_Top);
            } else {
                snprintf(temp, 512, "LOCAL: Examine() failed, the specified path is not a directory, fib->fib_DirEntryType = %d", (int)fib->fib_DirEntryType);
                MUI_AddStatusWindow(cn, temp);
                set(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, cn->lv.CurrentPath);
            }
        } else {
            MUI_AddStatusWindow(cn, "LOCAL: Examine() failed, cannot get directory listing");
            b_ret = FALSE;
        }

        UnLock(lock);
    } else {
        MUI_AddStatusWindow(cn, "LOCAL: cannot lock (GENERIC_READ) specified directory");
    }

    if (fib)
        FreeDosObject(DOS_FIB, fib);

    BufferClose(&lv->ListBuffer);
    set(lv->ListView, MUIA_NList_Quiet, FALSE);
    set(lv->ListView, MUIV_NList_Select_On, TRUE);
    return b_ret;
}

BOOL GetConnectionInfo(ClientInfo *ci)
{
    struct sockaddr_in sockaddr;
    int status;

    // Get the port of the socket 
    status = sizeof(struct sockaddr);
    getsockname(ci->cmd_socket, (struct sockaddr *) &sockaddr, &status);
    ci->port = htons(sockaddr.sin_port);

    // Get the IP of the cmd socket because the stack won't return the ip information
    // of an unconnected socket (MSDN)
    status = sizeof(struct sockaddr);
    getsockname(ci->cmd_socket, (struct sockaddr *) &sockaddr, &status);
    ci->ip_i = htonl(sockaddr.sin_addr.s_addr);
    caf_strncpy(ci->ip_s, IpToStr(ci->ip_i, ','), 31);

    return TRUE;
}

BOOL CheckSockForErrors(Connection *cn, SOCKET *socket)
{
    ClientInfo *ci = &cn->ci;
    BOOL b_ret = FALSE;
    int ret, optlen;

    getsockopt (*socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
    if (ret != 0) {
        // Something wrong.. whops
        printf("\nError code(getsockopt()) = %d string = '%s'\n", ret, GetErrnoDesc(ret));

        if (*socket == ci->cmd_socket) {
            // Cmd socket is closed? hell!
            CleanSockAndFlags(cn);
        } else if (*socket == ci->listen_socket) {
            // Error on listen socket, better close it
            CloseListenSocket(cn);
        } else if (*socket == ci->transfer_socket) {
            // Error on data socket
            // CloseDataSocket(cn);
        }
    } else {
        // No errors
        b_ret = TRUE;
    }

    return b_ret;
}

void CleanSockAndFlags(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    InvalidateSocket(&ci->cmd_socket);
    InvalidateSocket(&ci->listen_socket);
    InvalidateSocket(&ci->transfer_socket);
    ci->port = ci->ip_i = 0; ci->ip_s[0] = ci->bytes_transfered = 0;
    SetConnected(cn, FALSE);

    cn->ci.b_file_transfer =
    cn->ci.b_file_download =
    cn->ci.b_connected =
    cn->ci.b_connecting =
    cn->ci.b_waitingfordataport =
    cn->ci.b_transfer_error =
    cn->ci.b_transfering =
    cn->ci.b_transfered =
    cn->ci.b_can_send_command =
    cn->ci.b_directory_processed =
    cn->ci.b_user_accepted =
    cn->ci.b_pass_accepted =
    cn->ci.b_request_list =
    cn->ci.b_request_download =
    cn->ci.b_request_upload =
    cn->ci.b_request_conn =
    cn->ci.b_last_data =
    cn->ci.b_passive =
    cn->ci.b_processing_queue = FALSE;

//	cn->ci.b_request_disconn =
//	cn->ci.b_request_quit =

    MUI_AddStatusWindow(cn, "STAUTS: Disconnected");
}

/* Timer Stuff */
void StartTimer(TimerVal *tv)
{
    struct timeval tval;
    double ctime;

    GetSysTime(&tval);
    ctime  = (double) tval.tv_secs;
    ctime += (double) tval.tv_micro / 1000000.0;
    tv->start_time = ctime;
}

void UpdateTimer(TimerVal *tv)
{
    struct timeval tval;
    double ctime;

    GetSysTime(&tval);
    ctime  = (double) tval.tv_secs;
    ctime += (double) tval.tv_micro / 1000000.0;
    tv->current_time = ctime;
}

int TimeInSecs(TimerVal *tv)
{
    struct timeval tval;
    double ctime;

    GetSysTime(&tval);
    ctime  = (double) tval.tv_secs;
    ctime += (double) tval.tv_micro / 1000000.0;
    return (int)ctime - tv->start_time;
}

double TimeInFloat(TimerVal *tv)
{
    struct timeval tval;
    double ctime;

    GetSysTime(&tval);
    ctime  = (double) tval.tv_secs;
    ctime += (double) tval.tv_micro / 1000000.0;
    tv->end_time = ctime - tv->start_time;
    return tv->end_time;
}

void RecvStackPush(int command)
{
#ifdef DEBUGLV2
    char temp[512];
#endif
    int i;

    for (i = 0; i < STACK_BOTTOM; i++)
        recv_command_stack[i] = recv_command_stack[i+1];

    recv_command_stack[STACK_CURRENT] = command;
#ifdef DEBUGLV2
    snprintf(temp, 511, "RecvStackPush(): added cmd %d to stack\n", command);
    DebugOutput(temp);
#endif
}

void SentStackPush(char *CMD)
{
    char temp[512];
    int i;

    sscanf(CMD, "%s", temp);
#ifdef DEBUGLV2
    DebugOutput("SentStackPush(): Putting '");
    DebugOutput(temp);
    DebugOutput("' into the sent stack\n");
#endif
    for (i = 0; i < STACK_BOTTOM; i++)
        strncpy(sent_command_stack[i], sent_command_stack[i+1], 511);

    strncpy(sent_command_stack[STACK_CURRENT], temp, 511);
}

int GetCommandDigit(int command, int digit)
{
    int numbas[3];

    if ((unsigned) digit > 3) {
#ifdef DEBUG
        DebugOutput("GetCommandDigit(): Wrong digit requested at GetCommandDigit() function\n");
#endif
        return 0;
    }

    numbas[0] = command % 10;
    command /= 10;
    numbas[1] = command % 10;
    command /= 10;
    numbas[2] = command % 10;
    return numbas[3-digit];
}

void ProcessDirectoryData(Connection *cn)
{
    char dirflags[32], id[32], user[32], group[32], fsize[32], month[32], date[32], time_or_year[32];
    ClientInfo *ci = &cn->ci;
    ViewColumn vc, *vcptr;
    char temp[512];
    int i, last;

    /* Cleans ftp data and fills the appropiate listview */
#ifdef DEBUG
    snprintf(temp, 511, "ProcessDirectoryData(): ci->b_file_transfer = %d ci->b_transfered == %d b_directory_processed = %d\n", ci->b_file_transfer, ci->b_transfered, ci->b_directory_processed);
    DebugOutput(temp);
#endif

    if (cn->ci.b_directory_processed == TRUE) {
#ifdef DEBUG
        DebugOutput("ProcessDirectoryData(): Directory listing already processed, exiting\n");
#endif
        return;
    } else {
#ifdef DEBUG
        sprintf(temp, "ProcessDirectoryData(): Processing dirlisting, ci->bytes_transfered = %d\n", ci->bytes_transfered);
        DebugOutput(temp);
#endif
    }

    BufferOpen(&cn->rv.ListBuffer, TRUE);
    if (ci->b_file_transfer == FALSE && cn->ci.b_transfered == TRUE) {
        // ASDASDASDASD, buffer temporaneo per il list?
        FormatFtpListing(cn, &cn->transfer_buffer, TRUE);
        set(cn->rv.ListView, MUIA_NList_Quiet, TRUE);
        MUI_ClearListbox(cn->rv.ListView);

        // Add the directories
        for (i = 0; i < cn->rv.num_lines; i++) {
            // Split the UNIX directory structure in segments
            dirflags[0] = id[0] = user[0] = group[0] = fsize[0] = month[0] = date[0] = time_or_year[0] = 0;
            sscanf(&cn->transfer_buffer.ptr[cn->rv.lines_start[i]], "%s %s %s %s %s %s %s %s", dirflags, id, user, group, fsize, month, date, time_or_year);

            // Find where the nine ninenth (filename) begins
            last = caf_istrstrnum(9, &cn->transfer_buffer.ptr[cn->rv.lines_start[i]]);
            if (last) {
                // We found the ninenth string, format the LIST data
                if (dirflags[0] == 'd') {
                    vc.size = BufferAddStr(&cn->rv.ListBuffer, "<DIR>");
                    vc.is_dir = TRUE;
                } else {
                    vc.size = BufferAddStr(&cn->rv.ListBuffer, fsize);
                    vc.is_dir = FALSE;
                }

                vc.name = BufferAddStr(&cn->rv.ListBuffer, &cn->transfer_buffer.ptr[cn->rv.lines_start[i]+last]);
                vc.flags = BufferAddStr(&cn->rv.ListBuffer, dirflags);
                sprintf(temp, "%s %s %s", month, date, time_or_year);
                vc.date = BufferAddStr(&cn->rv.ListBuffer, temp);
            } else {
                // We did not find the ninenth string, fallback to old formatting style
                vc.name = vc.flags = vc.date = vc.size = "Error";
            }
            
            vcptr = BufferAddStruct(&cn->rv.ListBuffer, &vc, sizeof(ViewColumn));
            if (vc.name == 0 || vc.size == 0 || vc.flags == 0 || vc.date == 0 || vcptr == 0) {
                MUI_AddStatusWindow(cn, "ERROR: Remote listview buffer full, cannot continue directory scan");
                break;
            }

            if (vc.is_dir == TRUE)
                DoMethod(cn->rv.ListView, MUIM_NList_InsertSingle, (IPTR) vcptr, MUIV_NList_Insert_Top);
            else
                DoMethod(cn->rv.ListView, MUIM_NList_InsertSingle, (IPTR) vcptr, MUIV_NList_Insert_Bottom);
        }

        // Add CDUP
        vc.name = "..";
        vc.size = BufferAddStr(&cn->rv.ListBuffer, "<DIR>");
        vc.flags = "RWED";
        vc.date = "N/A";
        vc.is_dir = TRUE;
        vcptr = BufferAddStruct(&cn->rv.ListBuffer, &vc, sizeof(ViewColumn));
        if (vcptr)
            DoMethod(cn->rv.ListView, MUIM_NList_InsertSingle, (IPTR *) vcptr, MUIV_NList_Insert_Top);

        set(cn->rv.ListView, MUIA_NList_Quiet, FALSE);

        cn->ci.b_directory_processed = TRUE;
#ifdef DEBUG
        DebugOutput("ProcessDirectoryData(): b_directory_processed = TRUE\n");
#endif
    }

    BufferClose(&cn->rv.ListBuffer);
}

BOOL IsLastSentCmd(char *str)
{
#ifdef DEBUGLV2
    DebugOutput("IsLastSentCmd(");
    DebugOutput(str);
    DebugOutput("), sent_command_stack[STACK_CURRENT] = '"); 
    DebugOutput(sent_command_stack[STACK_CURRENT]);
    DebugOutput("'\n");
#endif
    return (strncmp(sent_command_stack[STACK_CURRENT], str, 511) == 0) ? TRUE: FALSE;
}

void ClearTransferFlags(Connection *cn)
{
    cn->ci.b_request_list = FALSE;
    cn->ci.b_request_download = FALSE;
    cn->ci.b_request_upload = FALSE;
    cn->ci.b_transfering = FALSE;
}

BOOL AbortFileTransfer(Connection *cn)
{
    if (cn->ci.b_transfering) {
        if (cn->ci.b_passive == FALSE)
           SendFtpCommand(cn, "ABOR", FALSE);
        else {
            InvalidateSocket(&cn->ci.transfer_socket);
            cn->ci.b_transfer_error = FALSE;
            cn->ci.b_transfering = FALSE;
            MUI_AddStatusWindow(cn, "STATUS: Transfer aborted by user");
            SentStackPush("ABOR");
            CloseDataSocket(cn);
        }
    }

#ifdef DEBUG
    printf("WARNING: AbortFileTransfer() was called but no file transfer is in progress\n");
#endif

    return TRUE;
}

/* General utility functions */
void QueueAdvance(Connection *cn)
{
    if (cn->ci.b_queue_step == TRUE) {
        if (cn->ci.queue_step > cn->ci.queue_to_step)
            return;

        cn->ci.queue_step++;
        MUI_UpdateQueueListbox(cn, FALSE);
    }

    while (cn->queue_cur < cn->queue_used) {
        MUI_UpdateQueueListbox(cn, FALSE);
        QueueProcess(cn);

        if (QueuePop(cn) == FALSE) {
            QueuePostProcess(cn);
            return;
        }

        if (cn->ci.b_queue_step == TRUE)
            return;

        // Eat every local command and stop to non local commands
        if (cn->QueuedItems[cn->queue_cur-1].b_local_cmd == FALSE)
            break;
    }
}

BOOL QueuePush(Connection *cn, int command, char *data, char *c_arg1, int i_arg1, BOOL b_local_cmd)
{
    if (cn->queue_cur+1 >= QUEUE_SIZE) {
        MUI_AddStatusWindow(cn, "ERROR: Queue items stack is full, aborting operation");
        return FALSE;
    }

    cn->QueuedItems[cn->queue_used].command = command;
    cn->QueuedItems[cn->queue_used].data 	= data;
    cn->QueuedItems[cn->queue_used].c_arg1 	= c_arg1;
    cn->QueuedItems[cn->queue_used].i_arg1 	= i_arg1;
    cn->QueuedItems[cn->queue_used].b_local_cmd = b_local_cmd;
    cn->queue_used++;

    return TRUE;
}

void *QueuePushAddStr(Connection *cn, buffer *buf, char *string, int command, char *c_arg1, int i_arg1, BOOL b_local_cmd)
{
    void *ptr;

    // Adds the current dir to the queue, change dir
    ptr = BufferAddStr(buf, string);
    if (ptr == 0) {
        MUI_AddStatusWindow(cn, "ERROR: Queue buffer is full, cannot continue with directory scanning");
        return 0;
    }

    if (QueuePush(cn, command, ptr, c_arg1, i_arg1, b_local_cmd) == FALSE) {
        MUI_AddStatusWindow(cn, "ERROR: QueuePush() failed, queue items buffer is full, cannot continue with directory scanning");
        return 0;
    }

    return ptr;
}

BOOL QueuePop(Connection *cn)
{
    if (cn->queue_cur+1 >= cn->queue_used)
        return FALSE;

    cn->queue_cur++;
    return TRUE;
}

void QueueClear(Connection *cn)
{
    cn->ci.b_processing_queue = FALSE;
    cn->queue_cur = 0;
    cn->queue_used = 0;
    cn->last_queue_update = 0;
    cn->ci.queue_step = 0;
    cn->ci.queue_to_step = 1;
}

void QueueProcess(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    if (ci->b_connected == FALSE) {
        MUI_AddStatusWindow(cn, "ERROR: Not connected");
        return;
    }

    switch (cn->QueuedItems[cn->queue_cur].command) {
        case 	CMD_DOWNLOAD:
                        InitiateFileTransfer(cn, cn->QueuedItems[cn->queue_cur].data, TRUE);
                        break;
                        
        case 	CMD_UPLOAD:
                        InitiateFileTransfer(cn, cn->QueuedItems[cn->queue_cur].data, FALSE);
                        break;
                        
        case	CMD_REMOTE_DELETE:
                        InitiateFileDeletion(cn, cn->QueuedItems[cn->queue_cur].data);
                        break;
                        
        case 	CMD_LOCAL_MKDIR:
                        LocalMakedir(cn, cn->QueuedItems[cn->queue_cur].data);
                        break;
        
        case 	CMD_REMOTE_MKDIR:
                        RemoteMakedir(cn, cn->QueuedItems[cn->queue_cur].data);
                        break;
                        
        case 	CMD_LOCAL_BASEPATH:
                        LocalBasePath(cn, cn->QueuedItems[cn->queue_cur].data);
                        break;
        
        case 	CMD_LOCAL_DIR_SCAN:
                        LocalDirScan(cn, cn->QueuedItems[cn->queue_cur].data, cn->QueuedItems[cn->queue_cur].data, caf_strlen(cn->QueuedItems[cn->queue_cur].data));
                        break;
                        
        case 	CMD_REMOTE_DIR_SCAN:
                        // ASD
                        RemoteDirScan(cn, cn->QueuedItems[cn->queue_cur].data);
                        break;
                        
        case	CMD_REMOTE_CHDIR:
                        RemoteChdir(cn, cn->QueuedItems[cn->queue_cur].data);
                        break;
    }
}

void QueuePostProcess(Connection *cn)
{
    // La coda e' stata tutta processata
    RefreshLocalView(cn);
    QueueClear(cn);
    MUI_UpdateQueueListbox(cn, TRUE);
    RefreshLocalView(cn);
    InitiateListTransfer(cn);
}

void DateStampToStr(char *dest, struct DateStamp *ds)
{
    struct DateTime dt;

    caf_strncpy(dest, "", 5);
    dt.dat_Stamp = *ds;
    dt.dat_StrDate = dest;
    dt.dat_StrDay = 0;
    dt.dat_StrTime = 0;
    DateToStr(&dt);
}

void DecodeBitFlags(char *dest, LONG flags)
{
    char temp[32];
    int i = 0;

    temp[i++] = (flags & FIBF_READ) == 0 ? 'R': ' ';
    temp[i++] = (flags & FIBF_WRITE) == 0 ? 'W': ' ';
    temp[i++] = (flags & FIBF_EXECUTE) == 0 ? 'E': ' ';
    temp[i++] = (flags & FIBF_DELETE) == 0 ? 'D': ' ';

    /* Not supported in aros ?
    temp[i++] = (1-(flags & FIBF_ARCHIVE))*'A';
    temp[i++] = (1-(flags & FIBF_PURE))*'P';
    temp[i++] = (1-(flags & FIBF_SCRIPT))*'S';
    */

    temp[i++] = 0;
    caf_strncpy(dest, temp, 512);
}

void BufferFlush(buffer *buf)
{
    buf->curpos = 0;
}

void BufferOpen(buffer *buf, BOOL b_clear)
{
    if (b_clear == TRUE)
        BufferFlush(buf);

    buf->b_opened = TRUE;
}

char *BufferAddStr(buffer *buf, char *string)
{
    char *ret_ptr, copied;
    int slen, pad = 0;

    if (buf->b_opened == FALSE) {
        printf("BufferAddStr() buffer is closed, cannot write\n");
        return buf->ptr;	
    }

    slen = caf_strlen(string);
    ret_ptr = &buf->ptr[buf->curpos];
    ret_ptr = AlignPtr(ret_ptr, 4, &pad);

    if (buf->curpos+slen+pad+16 >= buf->size) {
        printf("ERROR: Buffer full in BufferAddStr()\n");
        return 0;
    }
    copied = caf_strcpy(ret_ptr, string);
    (void)copied; // FIXME: Check for overrun!
    buf->curpos += slen+1+pad;

    return ret_ptr;
}

void *BufferAddStruct(buffer *buf, void *struc, int size)
{
    char *ret_ptr;
    int pad;

    ret_ptr = &buf->ptr[buf->curpos];
    ret_ptr = AlignPtr(ret_ptr, 4, &pad);
    if (buf->curpos+size+pad+16 >= buf->size) {
        printf("ERROR: Buffer full in BufferAddStruct()\n");
        return 0;
    }

    memcpy(ret_ptr, struc, size);
    buf->curpos += size+pad;

    return ret_ptr;
}

void BufferClose(buffer *buf)
{
    buf->b_opened = FALSE;
}

void *AlignPtr(void *ptr, int align_size, int *align_amount)
{
    char *ret_ptr = ptr;
    int pad = 0;

    pad = ((long)ret_ptr) & (align_size-1);
    if (pad) {
        ret_ptr += align_size-pad;
        pad = align_size-pad;
    }

    if (align_amount)
        *align_amount = pad;

    return ret_ptr;
}

int InitiateListTransfer(Connection *cn)
{
    HostInfo *hi = &cn->hi;

    if (cn->ci.b_can_send_command) {
        if (hi->conn_type == CTYPE_ACTV) {
            /* LIST, ACTIVE */
            if (OpenListenSocket(cn)) {
                SetTransferMode(cn, 'A');
                cn->ci.b_request_list = TRUE;
                return NO_ERRORS;
            } else
                MUI_AddStatusWindow(cn, "STATUS: OpenListenSocket() failed, cannot listen on a socket, cannot get directory listing");
        } else {
            /* LIST, PASSIVE */
            // ASD
            SetTransferMode(cn, 'A');
            cn->ci.b_request_list = TRUE;
        }
    } else {
        if (cn->ci.b_connected == TRUE) {
            MUI_AddStatusWindow(cn, STR_CANT_SEND);
            return ANSWER_PENDING;
        } else {
            MUI_AddStatusWindow(cn, STR_CANT_SEND_NOT_CONNECTED);
            return NOT_CONNECTED;
        }
    }

    return FLOW_CONTROL_ERROR;
}

int InitiateFileTransfer(Connection *cn, char *filename, BOOL b_download)
{
    ClientInfo *ci = &cn->ci;
    HostInfo *hi = &cn->hi;

    if (cn->ci.b_can_send_command) {
        if (hi->conn_type == CTYPE_ACTV) {
            /* DOWNLOAD, ACTIVE */
            if (OpenListenSocket(cn)) {
                SetTransferMode(cn, 'I');
                cn->ci.b_request_download = TRUE;
                cn->ci.b_file_download = b_download;
                strncpy(ci->filename, filename, 255);
                return NO_ERRORS;
            } else {
                MUI_AddStatusWindow(cn, "STATUS: OpenListenSocket() failed, cannot listen on a socket, cannot request a file transfer");
                ci->b_processing_queue = FALSE;
            }
        } else {
            /* DOWNLOAD, PASSIVE */
            SetTransferMode(cn, 'I');
            cn->ci.b_request_download = TRUE;
            cn->ci.b_file_download = b_download;
            strncpy(ci->filename, filename, 255);
            return NO_ERRORS;
        }
    } else {
        if (cn->ci.b_connected == TRUE) {
            MUI_AddStatusWindow(cn, STR_CANT_SEND);
            return ANSWER_PENDING;
        } else {
            MUI_AddStatusWindow(cn, STR_CANT_SEND_NOT_CONNECTED);
            return NOT_CONNECTED;
        }
    }

    return FLOW_CONTROL_ERROR;
}

int InitiateFileDeletion(Connection *cn, char *filename)
{
    char temp[512];

    sprintf(temp, "DELE %s", filename);
    SendFtpCommand(cn, temp, FALSE);
    return ANSWER_PENDING;
}

void AcknowledgeConnection(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    // Command socket event happened, during connect() process it
    if (CheckConnect(cn) == TRUE) {
        // We are connected
        GetConnectionInfo(ci);
    } else
        CleanSockAndFlags(cn);
}

int GatherIpPortNumber(Connection *cn)
{
    int i, port_start = 0, comma_counter = 0, port_comma = 0, temp_counter = 0;
    char *text = cn->cmd_buffer.ptr, temp[512];
    HostInfo *hi = &cn->hi;
    BOOL b_copy = FALSE;

    for (i = 0; text[i] != 0 && i < 511; i++) {
        // Exit at closed paranthesis
        if (text[i] == ')')
            break;

        if (b_copy) {
            // Substitutes comma with point, for inet_addr
            if (text[i] != ',')
                temp[temp_counter++] = text[i];
            else {
                comma_counter++;
                
                if (port_start && port_comma == 0)
                    port_comma = temp_counter;

                if (comma_counter == 4) {
                    // Port data starts here+1
                    port_start = temp_counter;
                }

                temp[temp_counter++] = '.';
            }
        }

        if (text[i] == '(')
            b_copy = TRUE;
    }
    temp[temp_counter++] = 0;

    if (port_start) {
        temp[port_start] = 0;
        temp[port_comma] = 0;
        hi->pasvsettings.ip_i = inet_addr(temp);
        hi->pasvsettings.port = atol(&temp[port_start+1]) << 8;
        hi->pasvsettings.port |= atol(&temp[port_comma+1]);
    } else {
        MUI_AddStatusWindow(cn, "ERROR: Cannot get passive connection information");
    }

    return port_start;
}

int SendMKDCMD(Connection *cn, char *pathname)
{
    char temp[512];

    sprintf(temp, "MKD %s", pathname);
    SendFtpCommand(cn, temp, FALSE);
    return ANSWER_PENDING;
}

int SendPASVCMD(Connection *cn)
{
    SendFtpCommand(cn, "PASV", FALSE);
    cn->ci.b_passive = TRUE;
    return ANSWER_PENDING;
}

void AfterRETR(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    if (ci->b_transfering == TRUE)
        return;

    // DOWNLOADED
    if (g_GlobalSettings.SettingsArray[SETTING_LOCAL_REFRESH_AFTER_RETR] == OPT_REFRESH_AFTER_RETR)
        RefreshLocalView(cn);

    if (ci->b_processing_queue) {
        QueueAdvance(cn);
    } else
        if (g_GlobalSettings.SettingsArray[SETTING_LOCAL_REFRESH_AFTER_RETR] == OPT_NO_REFRESH_AFTER_RETR)
        {
            // Refresh Local Directory
            RefreshLocalView(cn);
        }
}

void AfterSTOR(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    // UPLOADED
    if (ci->b_processing_queue) {
        QueueAdvance(cn);
    } else {
        // Refresh Remote Directory
        InitiateListTransfer(cn);
    }
}

void AfterDELE(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    // DELETED
    if (ci->b_processing_queue) {
        QueueAdvance(cn);
    } else {
        // Refresh the remote view
        InitiateListTransfer(cn);
    }
}

void AfterCWD(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    if (ci->b_processing_queue) {
        QueueAdvance(cn);
    } else {
        // Refresh the remote view
        SendPWDCMD(cn);
    }
}

void AfterMKD(Connection *cn)
{
    ClientInfo *ci = &cn->ci;

    if (ci->b_processing_queue) {
        QueueAdvance(cn);
    } else {
        // Refresh the remote view 
        InitiateListTransfer(cn);
    }
}

BOOL LocalMakedir(Connection *cn, char *pathname)
{
    BPTR lock;

    lock = CreateDir(pathname);
    if (lock != 0) {
        UnLock(lock);
        return TRUE;
    }

    return FALSE;
}

void RemoteMakedir(Connection *cn, char *pathname)
{
    SendMKDCMD(cn, pathname);
}

BOOL LocalDirScan(Connection *cn, char *pathname, char *basepath, int base_start)
{
    struct FileInfoBlock *fib = 0;
    char temp[512], curdir[512];
    BPTR lock = (BPTR)-1;
    char *ptr;

    fib = AllocDosObject(DOS_FIB, NULL);
    if (!fib) {
        MUI_AddStatusWindow(cn, "LOCAL: AllocDosObject() failed, cannot proceed with the directory scan");
        return FALSE;
    }

    BufferOpen(&cn->temp_buffer, FALSE);
    BufferOpen(&cn->queue_buffer, FALSE);
    caf_strncpy(curdir, pathname, 512);
    while (lock != 0) {
        lock = Lock(curdir, ACCESS_READ);
        if (lock) {
            ptr = QueuePushAddStr(cn, &cn->queue_buffer, curdir, CMD_LOCAL_BASEPATH, 0, 0, TRUE);
            if (ptr == 0) {
                UnLock(lock);
                return FALSE;
            }

            // Changes remote directory
            sprintf(temp, "%s/%s", cn->rv.QueueBasePath, &curdir[base_start+1]);
            ptr = QueuePushAddStr(cn, &cn->queue_buffer, temp, CMD_REMOTE_CHDIR, 0, 0, FALSE);
            if (ptr == 0) {
                UnLock(lock);
                return FALSE;
            }

            if (Examine(lock, fib) != FALSE) {
                while ((ExNext(lock, fib) != DOSFALSE)) {
                    if (fib->fib_DirEntryType == ST_USERDIR) {
                        // Adds the directory to the walk stack
                        sprintf(temp, "%s/%s", curdir, fib->fib_FileName);
                        ptr = BufferAddStr(&cn->temp_buffer, temp);
                        if (ptr == 0) {
                            MUI_AddStatusWindow(cn, "ERROR: The temporary buffer is full, cannot continue with directory scanning");
                            UnLock(lock);
                            return FALSE;
                        }

                        cn->stack_cur--;
                        if (cn->stack_cur == 0) {
                            MUI_AddStatusWindow(cn, "ERROR: Maximum number of stack entries reached, file queuing stopped");
                            return FALSE;
                        }
                        cn->StackStack[cn->stack_cur].data = ptr;

                        // Adds the directory to the queue, createdir
                        sprintf(temp, "%s/%s/%s", cn->rv.QueueBasePath, &curdir[base_start+1], fib->fib_FileName);
                        ptr = QueuePushAddStr(cn, &cn->queue_buffer, temp, CMD_REMOTE_MKDIR, 0, 0, FALSE);
                        if (ptr == 0) {
                            UnLock(lock);
                            return FALSE;
                        }
                    } else {
                        // Adds the file to the queue
                        ptr = QueuePushAddStr(cn, &cn->queue_buffer, fib->fib_FileName, CMD_UPLOAD, 0, 0, FALSE);
                        if (ptr == 0) {
                            UnLock(lock);
                            return FALSE;
                        }
                    }
                }
            }
            
            UnLock(lock);
            if (cn->stack_cur == STACK_SIZE) {
#ifdef DEBUG
                DebugOutput("LocalDirScan(): STACK IS ZERO, EXITING");
#endif
                break;
            }

            caf_strncpy(curdir, cn->StackStack[cn->stack_cur].data, 512);
            cn->stack_cur++;
        } else {
            sprintf(temp, "Locking of '%s' failed\n", curdir);
            MUI_AddStatusWindow(cn, temp);
        }
    }

    //BufferClose(&cn->temp_buffer);
    //BufferClose(&cn->queue_buffer);
    return TRUE;
}

void RemoteDirScan(Connection *cn, char *pathname)
{
    
}

/*
void LoadConfig()
{
    char temp[32];
    int i = 0;
    FILE *fp;

    fp = fopen("ram:config", "r");
    if (fp) {
        if (!feof(fp)) {
            do {
                fgets(g_AddressBook.Hosts[i].hostname	, 128, 	fp);
                fgets(g_AddressBook.Hosts[i].port		, 8, 	fp);
                fgets(g_AddressBook.Hosts[i].username	, 32, 	fp);
                fgets(g_AddressBook.Hosts[i].password	, 32, 	fp);
                fgets(temp								, 1, 	fp);
                g_AddressBook.Hosts[i].conn_type = temp[0]-'0';
                RemoveLFCF(g_AddressBook.Hosts[i].hostname);
                RemoveLFCF(g_AddressBook.Hosts[i].port);
                RemoveLFCF(g_AddressBook.Hosts[i].username);
                RemoveLFCF(g_AddressBook.Hosts[i].password);
                printf("Hostname = %s\n", g_AddressBook.Hosts[i].hostname);

                i++;
            } while (!feof(fp));
        }

        g_AddressBook.used_hosts = i;
        fclose(fp);
    }
}
*/

void LoadConfig(Connection *cn)
{
    int i = 0, itemp, nrecords;
    struct StoredProperty *sp;
    struct CollectionItem *ci;
    struct IFFHandle  *iff;
    struct Process *myproc;
    char *ptr, temp[640];
    BOOL b_done = FALSE;
    APTR oldptr;

    if((iff = AllocIFF())) {
        // Disables system requests - [Thanks Itix]
        myproc = (struct Process *) FindTask(NULL);
        oldptr = myproc->pr_WindowPtr;
        myproc->pr_WindowPtr = (APTR) -1;

        // Tries to open the file
        iff->iff_Stream = (IPTR) Open(g_config_filename, MODE_OLDFILE);
        if (!iff->iff_Stream) {
            caf_strncpy(g_config_filename, "PROGDIR:marranoftp.prefs", 512);  
            iff->iff_Stream = (IPTR) Open(g_config_filename, MODE_OLDFILE);
        }

        // Re-enables system assign request
        myproc->pr_WindowPtr = oldptr;

        if(iff->iff_Stream) {
            InitIFFasDOS(iff);
            if(!OpenIFF(iff, IFFF_READ)) {
                if(!(PropChunk(iff, ID_MFTP, ID_GLOB) || 
                     CollectionChunk(iff, ID_MFTP, ID_HOST) ||
                     StopOnExit(iff, ID_MFTP, ID_FORM))) {
                    itemp = ParseIFF(iff, IFFPARSE_SCAN);
                    if(itemp == IFFERR_EOC) {
                        sp = FindProp(iff, ID_MFTP, ID_GLOB);
                        if (sp) {
#ifdef DEBUGLV2
                            printf("LoadConfig(): ID_GLOB found\n");
#endif

                            // We have global config chunk
                            ptr = sp->sp_Data;

                            // Read connection timeout
                            itemp = MemReadDword(&ptr);
                            g_GlobalSettings.SettingsArray[SETTING_TRANSFER_TIMEOUT] = itemp;
                            if (itemp == CONFIG_VERSION1) {
                                // Found config version 2
                                CloseIFF(iff);
                                Close((BPTR) iff->iff_Stream);
                                FreeIFF(iff);
                                LoadConfig2(cn);
                                return;
                            }

                            // Read number of records in file
                            nrecords = MemReadDword(&ptr);
                            (void)nrecords; // FIXME: Check # of records!

#ifdef DEBUGLV2
                            printf("LoadConfig(): connect_timeout = %d\n", itemp);
#endif

                            // Read transfer timeout
                            itemp = MemReadDword(&ptr);
                            g_GlobalSettings.SettingsArray[SETTING_CONNECT_TIMEOUT] = itemp;
#ifdef DEBUGLV2
                            printf("LoadConfig(): transfer_timeout = %d\n", itemp);
#endif

                            // Read queue type
                            itemp = MemReadDword(&ptr);
                            g_GlobalSettings.SettingsArray[SETTING_QUEUE_TYPE] = itemp;
#ifdef DEBUGLV2
                            printf("LoadConfig(): queue_type = %d\n", itemp);
#endif
                        } 
#ifdef DEBUGLV2
                        else
                            printf("LoadConfig(): ID_GLOB not found :(\n");
#endif

                        ci = FindCollection(iff, ID_MFTP, ID_HOST);
                        while (ci) {
                            ptr = ci->ci_Data;

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].entryname, &ptr, itemp+1, ENTRYNAME_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig(): strlen = %08x hostname = '%s'\n", itemp, g_AddressBook.Hosts[i].entryname);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].hostname, &ptr, itemp+1, HOSTNAME_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig(): strlen = %08x hostname = '%s'\n", itemp, g_AddressBook.Hosts[i].hostname);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].port, &ptr, itemp+1, PORT_COFING_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig(): strlen = %08x port = '%s'\n", itemp, g_AddressBook.Hosts[i].port);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].username, &ptr, itemp+1, USERNAME_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig(): strlen = %08x username = '%s'\n", itemp, g_AddressBook.Hosts[i].username);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].password, &ptr, itemp+1, PASSWORD_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig(): strlen = %08x password = '%s'\n", itemp, g_AddressBook.Hosts[i].password);
#endif

                            g_AddressBook.Hosts[i].conn_type = MemReadDword(&ptr);
#ifdef DEBUGLV2
                            printf("LoadConfig(): Connection type = %d\n", g_AddressBook.Hosts[i].conn_type);
#endif

                            i++;
                            ci = ci->ci_Next;
                        }

                        g_AddressBook.used_hosts = i;
                        b_done = TRUE;
                    } else {
#ifdef DEBUGLV2
                        printf("LoadConfig(): ParseIFF() failed, rc = %d\n", itemp);
#endif
                    }
                } else {
#ifdef DEBUGLV2
                    printf("LoadConfig(): CollectionChunk() of StopOnExit() failed\n");
#endif
                }
                    
                CloseIFF(iff);
            } else {
#ifdef DEBUGLV2
                printf("LoadConfig(): OpenIFF() failed\n");
#endif
            }

            Close((BPTR) iff->iff_Stream);
        } else {
            MUI_AddStatusWindow(cn, "ERROR: Cannot load configuration file marranoftp.prefs");  /* Removed references to PROGDIR: and renamed prefs file */ 
            caf_strncpy(g_config_filename, "PROGDIR:marranoftp.prefs", 512);
        }

        FreeIFF(iff);
    } else {
#ifdef DEBUGLV2
        printf("LoadConfig(): AllocIFF() failed\n");
#endif
    }

    if (b_done == TRUE) {
        sprintf(temp, "STATUS: Old configuration format loaded from %s", g_config_filename);
        MUI_AddStatusWindow(cn, temp);
    }
}

void LoadConfig2(Connection *cn)
{
    int i = 0, itemp, config_version, nrecs;
    struct StoredProperty *sp;
    struct CollectionItem *ci;
    struct IFFHandle  *iff;
    struct Process *myproc;
    char *ptr, temp[640];
    BOOL b_done = FALSE;
    APTR oldptr;

    if((iff = AllocIFF())) {
        // Disables system request - [Thanks Itix]
        myproc = (struct Process *) FindTask(NULL);
        oldptr = myproc->pr_WindowPtr;
        myproc->pr_WindowPtr = (APTR)-1;

        // Tries to open the file
        iff->iff_Stream = (IPTR) Open(g_config_filename, MODE_OLDFILE);
        if (!iff->iff_Stream) {
            caf_strncpy(g_config_filename, "PROGDIR:marranoftp.prefs", 512); 
            iff->iff_Stream = (IPTR) Open(g_config_filename, MODE_OLDFILE);
        }

        // Re-enables system assign request
        myproc->pr_WindowPtr = oldptr;

        //
        if(iff->iff_Stream) {
            InitIFFasDOS(iff);
            if(!OpenIFF(iff, IFFF_READ)) {
                if(!(PropChunk(iff, ID_MFTP, ID_GLOB) || 
                     CollectionChunk(iff, ID_MFTP, ID_HOST) ||
                     StopOnExit(iff, ID_MFTP, ID_FORM))) {
                    itemp = ParseIFF(iff, IFFPARSE_SCAN);
                    if(itemp == IFFERR_EOC) {
                        sp = FindProp(iff, ID_MFTP, ID_GLOB);
                        if (sp) {
#ifdef DEBUGLV2
                            printf("LoadConfig2(): ID_GLOB found\n");
#endif

                            // We have global config chunk
                            ptr = sp->sp_Data;

                            // Read configuration version
                            config_version = MemReadDword(&ptr);
                            (void)config_version; // FIXME: Check version!

                            // Read number of records
                            nrecs =  MemReadDword(&ptr);

                            // Read the settings
                            for (i = 0; i < nrecs; i++) {
                                itemp = MemReadDword(&ptr);
                                g_GlobalSettings.SettingsArray[i] = itemp;
                            }
                        } 
#ifdef DEBUGLV2
                        else
                            printf("LoadConfig2(): ID_GLOB not found :(\n");
#endif

                        ci = FindCollection(iff, ID_MFTP, ID_HOST);
                        i = 0;
                        while (ci) {
                            ptr = ci->ci_Data;

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].entryname, &ptr, itemp+1, ENTRYNAME_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig2(): strlen = %08x hostname = '%s'\n", itemp, g_AddressBook.Hosts[i].entryname);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].hostname, &ptr, itemp+1, HOSTNAME_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig2(): strlen = %08x hostname = '%s'\n", itemp, g_AddressBook.Hosts[i].hostname);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].port, &ptr, itemp+1, PORT_COFING_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig2(): strlen = %08x port = '%s'\n", itemp, g_AddressBook.Hosts[i].port);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].username, &ptr, itemp+1, USERNAME_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig2(): strlen = %08x username = '%s'\n", itemp, g_AddressBook.Hosts[i].username);
#endif

                            itemp = MemReadDword(&ptr);
                            MemReadString(g_AddressBook.Hosts[i].password, &ptr, itemp+1, PASSWORD_CONFIG_LEN);
#ifdef DEBUGLV2
                            printf("LoadConfig2(): strlen = %08x password = '%s'\n", itemp, g_AddressBook.Hosts[i].password);
#endif

                            g_AddressBook.Hosts[i].conn_type = MemReadDword(&ptr);
#ifdef DEBUGLV2
                            printf("LoadConfig2(): Connection type = %d\n", g_AddressBook.Hosts[i].conn_type);
#endif

                            i++;
                            ci = ci->ci_Next;
                        }

                        g_AddressBook.used_hosts = i;
                        b_done = TRUE;
                    } else {
#ifdef DEBUGLV2
                        printf("LoadConfig2(): ParseIFF() failed, rc = %d\n", itemp);
#endif
                    }
                } else {
#ifdef DEBUGLV2
                    printf("LoadConfig2(): CollectionChunk() of StopOnExit() failed\n");
#endif
                }

                CloseIFF(iff);
            } else {
#ifdef DEBUGLV2
                printf("LoadConfig2(): OpenIFF() failed\n");
#endif
            }

            Close((BPTR) iff->iff_Stream);
        } else {
            MUI_AddStatusWindow(cn, "ERROR: Cannot load configuration file marranoftp.prefs");
            caf_strncpy(g_config_filename, "PROGDIR:marranoftp.prefs", 512); 
        }

        FreeIFF(iff);
    } else {
#ifdef DEBUGLV2
        printf("LoadConfig2(): AllocIFF() failed\n");
#endif
    }

    if (b_done == TRUE) {
        sprintf(temp, "STATUS: Configuration loaded from %s", g_config_filename);
        MUI_AddStatusWindow(cn, temp);
    }
}

BOOL SaveConfig()
{
    struct IFFHandle  *iff;
    int i, itemp, to_write;
    struct Process *myproc;
    BOOL ret = FALSE;
    APTR oldptr;

    if((iff = AllocIFF())) {
        // Disables system assign request - [Thanks Itix]
        myproc = (struct Process *) FindTask(NULL);
        oldptr = myproc->pr_WindowPtr;
        myproc->pr_WindowPtr = (APTR)-1;

        iff->iff_Stream = (IPTR) Open(g_config_filename, MODE_NEWFILE);
        if (!iff->iff_Stream) {
            caf_strncpy(g_config_filename, "PROGDIR:marranoftp.prefs", 512); 
            iff->iff_Stream = (IPTR) Open(g_config_filename, MODE_NEWFILE);
        }

        // Re-enables system assign request
        myproc->pr_WindowPtr = oldptr;

        if(iff->iff_Stream) {
            InitIFFasDOS(iff);
            if(!OpenIFF(iff, IFFF_WRITE)) {
                if(!PushChunk(iff, ID_MFTP, ID_FORM, IFFSIZE_UNKNOWN)) {
                    if(!PushChunk(iff, ID_MFTP, ID_GLOB, IFFSIZE_UNKNOWN)) {
                        // Writes configuration version
                        itemp = CONFIG_VERSION1;
#ifdef CPU_BIG_ENDIAN
                        BSwapDWORD(&to_write, &itemp);
#else
                        to_write = itemp;
#endif
                        WriteChunkBytes(iff, &to_write, sizeof(int));

                        // Writes number of records
                        itemp = NUM_OF_SETTINGS;
#ifdef CPU_BIG_ENDIAN
                        BSwapDWORD(&to_write, &itemp);
#else
                        to_write = itemp;
#endif
                        WriteChunkBytes(iff, &to_write, sizeof(int));

                        // Writes the settings
                        for (i = 0; i < NUM_OF_SETTINGS; i++) {
                            itemp = g_GlobalSettings.SettingsArray[i];
#ifdef CPU_BIG_ENDIAN
                            BSwapDWORD(&to_write, &itemp);
#else
                            to_write = itemp;
#endif
                            WriteChunkBytes(iff, &to_write, sizeof(int));
                        }

                        PopChunk(iff);
                    }

                    if (g_AddressBook.used_hosts) {
                        ret = TRUE;
                        for (i = g_AddressBook.used_hosts-1; i >= 0; i--) {
                            if(!PushChunk(iff, ID_MFTP, ID_HOST, IFFSIZE_UNKNOWN)) {
                                itemp = caf_strlen(g_AddressBook.Hosts[i].entryname);
#ifdef CPU_BIG_ENDIAN
                                BSwapDWORD(&to_write, &itemp);
#else
                                to_write = itemp;
#endif
                                WriteChunkBytes(iff, &to_write, sizeof(int));
                                WriteChunkBytes(iff, g_AddressBook.Hosts[i].entryname, itemp);

                                itemp = caf_strlen(g_AddressBook.Hosts[i].hostname);
#ifdef CPU_BIG_ENDIAN
                                BSwapDWORD(&to_write, &itemp);
#else
                                to_write = itemp;
#endif
                                WriteChunkBytes(iff, &to_write, sizeof(int));
                                WriteChunkBytes(iff, g_AddressBook.Hosts[i].hostname, itemp);

                                itemp = caf_strlen(g_AddressBook.Hosts[i].port);
#ifdef CPU_BIG_ENDIAN
                                BSwapDWORD(&to_write, &itemp);
#else
                                to_write = itemp;
#endif
                                WriteChunkBytes(iff, &to_write, sizeof(int));
                                WriteChunkBytes(iff, g_AddressBook.Hosts[i].port, itemp);

                                itemp = caf_strlen(g_AddressBook.Hosts[i].username);
#ifdef CPU_BIG_ENDIAN
                                BSwapDWORD(&to_write, &itemp);
#else
                                to_write = itemp;
#endif
                                WriteChunkBytes(iff, &to_write, sizeof(int));
                                WriteChunkBytes(iff, g_AddressBook.Hosts[i].username, itemp);

                                itemp = caf_strlen(g_AddressBook.Hosts[i].password);
#ifdef CPU_BIG_ENDIAN
                                BSwapDWORD(&to_write, &itemp);
#else
                                to_write = itemp;
#endif
                                WriteChunkBytes(iff, &to_write, sizeof(int));
                                WriteChunkBytes(iff, g_AddressBook.Hosts[i].password, itemp);

                                itemp = g_AddressBook.Hosts[i].conn_type;
#ifdef CPU_BIG_ENDIAN
                                BSwapDWORD(&itemp, &itemp);
#endif
                                WriteChunkBytes(iff, &itemp, sizeof(int));
                            } else {
                                printf("ERROR: PushChunk() failed!, cannot save configuration file");						
                                ret = FALSE;
                            }

                            PopChunk(iff);
                        }
                    }

                    PopChunk(iff);
                } else
                    printf("failed to store ID_FORM chunk\n");

                CloseIFF(iff);
            } else
                printf("ERROR: OpenIFF() failed!, cannot save configuration file");

            Close((BPTR) iff->iff_Stream);
        } else
            printf("ERROR: cannot save configuration file marranoftp.prefs");

        FreeIFF(iff);
    } else {
        printf("ERROR: AllocIFF() failed!, cannot save configuration file");
    }

    return ret;
}

void RemoveLFCF(char *str)
{
    int i;

    for (i = 0; str[i] != 0; i++)
        if (str[i] == '\n' || str[i] == '\r') {
            str[i] = 0;
            break;
        }
}


int MemReadDword(char **ptr)
{
    char *p = *ptr;
    int tmp;

    tmp = (p[0] | p[1]<<8 | p[2]<<16 | p[3]<<24);
    *ptr += 4;
    return tmp;
}

int MemReadString(char *dest, char **ptr, int len, int maxlen)
{
    int rc;

    if (len > maxlen) {
            #ifdef DEBUG
            DebugOutput("MemReadString(): len > maxlen!\n");
            #endif
            return 0;
    }

    rc = caf_strncpy(dest, *ptr, len);
    *ptr += rc;
    return rc;
}

/* Mui or gui Stuff */
void MUI_SetConnectButton(Connection *cn)
{
    set(cn->BTN_CONNDISC, MUIA_Text_Contents, "\33c  Connect  ");
}

void MUI_SetDisconnectButton(Connection *cn)
{
    set(cn->BTN_CONNDISC, MUIA_Text_Contents, "\33c  Disconnect  ");
}

void MUI_AddStatusWindow(Connection *cn, char *text)
{
    DoMethod(cn->LV_STATUS, MUIM_NList_InsertSingle, (APTR *) text, MUIV_NList_Insert_Bottom);
    DoMethod(cn->LV_STATUS, MUIM_NList_Jump, MUIV_List_Jump_Bottom);
}

void MUI_AddListboxCMDText(Connection *cn, char *text)
{
    char temp[512];

    caf_strncpy(temp, "COMMAND: ", 511);
    caf_strncat(temp, text, 511);

    DoMethod(cn->LV_STATUS, MUIM_NList_InsertSingle, (APTR *) temp, MUIV_NList_Insert_Bottom);
    DoMethod(cn->LV_STATUS, MUIM_NList_Jump, MUIV_List_Jump_Bottom);
}

void MUI_AddListboxASWText(Connection *cn)
{
    char temp[512];
    int i;

    for (i = 0; i < cn->rv.num_lines; i++) {
        caf_strncpy(temp, "ANSWER: ", 511);
        caf_strncat(temp, &cn->cmd_buffer.ptr[cn->rv.lines_start[i]], 511);
        DoMethod(cn->LV_STATUS, MUIM_NList_InsertSingle, (APTR *) temp, MUIV_NList_Insert_Bottom);
        DoMethod(cn->LV_STATUS, MUIM_NList_Jump, MUIV_List_Jump_Bottom);
    }
}

void MUI_ClearListbox(Object *LBox)
{
    DoMethod(LBox, MUIM_NList_Clear);
}

void MUI_GetSet(APTR object, int attrib, int state)
{
    IPTR itmp;

    get(object, attrib, &itmp);
    if (itmp != state)
        set(object, attrib, state);
}

void MUI_UpdateQueueListbox(Connection *cn, BOOL b_full_update)
{
    QueueColumn *qcptr, qc;
    int i, to_rem;

    // Quiet and clear the LV
    set(cn->QueueLV, MUIA_NList_Quiet, TRUE);

    if (b_full_update == TRUE) {
        // Full update, the whole LB is updated
        MUI_ClearListbox(cn->QueueLV);

        for (i = cn->queue_cur; i < cn->queue_used; i++) {
            qc.command 	= cn->QueuedItems[i].command;
            qc.name		= cn->QueuedItems[i].data;
            qc.status 	= STATUS_QUEUED;
            qcptr = BufferAddStruct(&cn->queue_buffer, &qc, sizeof(QueueColumn));
            if (qcptr)
                DoMethod(cn->QueueLV, MUIM_NList_InsertSingle, (IPTR *) qcptr, MUIV_NList_Insert_Bottom);
            else {
                MUI_AddStatusWindow(cn, "ERROR: Cannot update the queue window, the queue buffer is full");
                break;
            }
        }
    } else {
        to_rem = cn->queue_cur - cn->last_queue_update;
        for (i = 0; i < to_rem; i++)
            DoMethod(cn->QueueLV, MUIM_NList_Remove, 0);
        cn->last_queue_update = cn->queue_cur;
    }

    set(cn->QueueLV, MUIA_NList_Quiet, FALSE);
}

int MUI_GetListboxSelItemCount(APTR Listbox)
{
    int sel_items = 0;
    LONG id;

    // Is only one item selected ?
    id = MUIV_List_NextSelected_Start;
    for (;;) {
        DoMethod(Listbox, MUIM_List_NextSelected, &id);
        if (id == MUIV_List_NextSelected_End) 
            break;

        sel_items++;
    }

    return sel_items;
}

void SetConnected(Connection *cn, BOOL b_con)
{
    if (b_con == TRUE) {
        MUI_SetDisconnectButton(cn);
        cn->ci.b_connected = TRUE;
        SentStackPush("CONN");
    } else {
        MUI_SetConnectButton(cn);
        cn->ci.b_connected = FALSE;
    }
}

void OnQueueProcess(Connection *cn)
{
    cn->ci.b_processing_queue = TRUE;
    cn->ci.b_queue_step = FALSE;
    QueueAdvance(cn);
}

void OnQueueStep(Connection *cn)
{
    cn->ci.b_processing_queue = TRUE;
    cn->ci.b_queue_step = TRUE;
    cn->ci.queue_to_step++;
    QueueAdvance(cn);
}

void OnClearQueue(Connection *cn)
{
    QueueClear(cn);
    MUI_UpdateQueueListbox(cn, TRUE);
}

void OnDownloadBtnClick(Connection *cn)
{
    int sel_items = 0, command;
    RemoteView *rv = &cn->rv;
    ClientInfo *ci = &cn->ci;
    ViewColumn *vcptr;
    IPTR itemp;
    void *ptr;
    LONG id;

    if (ci->b_connected == FALSE) {
        MUI_AddStatusWindow(cn, "ERROR: Not connected");
    }

    // Updates the remote path variable
    get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itemp);
    caf_strncpy(rv->CurrentPath, (char *) itemp, 512);

    // Updates the remote queue path variable
    get(cn->S_RIGHT_VIEW_PATH, MUIA_String_Contents, &itemp);
    caf_strncpy(cn->rv.QueueBasePath, (char *) itemp, 512);
    if (caf_getlastchar(cn->rv.QueueBasePath) == '/')
        cn->rv.QueueBasePath[caf_strlen(cn->rv.QueueBasePath)-1] = 0;

    // Is only one item selected ?
    sel_items = MUI_GetListboxSelItemCount(rv->ListView);
    if (sel_items == 1) {
        // Only one item selected, no need to queue stuph
        OnRightListviewDblClick(cn);
        return;
    }

    // Multiple items, download will queue
    id = MUIV_List_NextSelected_Start;
    QueueClear(cn);
    BufferOpen(&cn->queue_buffer, TRUE);
    for (;;) {
        DoMethod(rv->ListView, MUIM_List_NextSelected, &id);
        if (id == MUIV_List_NextSelected_End) 
            break;

        DoMethod(rv->ListView, MUIM_List_GetEntry, id, &vcptr);

        // Skip ".." (previous dir)
        if (vcptr->name[0] == '.' && vcptr->name[1] == '.' && vcptr->name[2] == 0)
            continue;

        // Add remote directory scanning
        if (vcptr->is_dir)
            command = CMD_REMOTE_DIR_SCAN;
        else
            command = CMD_DOWNLOAD;

        ptr = QueuePushAddStr(cn, &cn->queue_buffer, vcptr->name, command, 0, 0, FALSE);
        if (ptr == 0)
            break;

        // Add local makedir command
        if (vcptr->is_dir) {
            ptr = QueuePushAddStr(cn, &cn->queue_buffer, vcptr->name, CMD_LOCAL_MKDIR, 0, 0, TRUE);
            if (ptr == 0)
                break;
        }
    }

    MUI_UpdateQueueListbox(cn, TRUE);
    BufferClose(&cn->queue_buffer);
}

void OnUploadBtnClick(Connection *cn)
{
    BOOL b_firstfile = TRUE, b_done_dirs = FALSE;
    char temp[512], local_path[512];
    LocalView *lv = &cn->lv;
    int sel_items = 0;
    ViewColumn *vcptr;
    int offset;
    IPTR itemp;
    void *ptr;
    LONG id;

    // Updates the local path variable
    get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itemp);
    caf_strncpy(lv->CurrentPath, (char *) itemp, 512);

    // Updates the remote queue path variable
    get(cn->S_RIGHT_VIEW_PATH, MUIA_String_Contents, &itemp);
    caf_strncpy(cn->rv.QueueBasePath, (char *) itemp, 512);
    if (caf_getlastchar(cn->rv.QueueBasePath) == '/')
        cn->rv.QueueBasePath[caf_strlen(cn->rv.QueueBasePath)-1] = 0;

    // Saves currentpath for later reuse
    caf_strncpy(local_path, lv->CurrentPath, 512);

    sel_items = MUI_GetListboxSelItemCount(lv->ListView);
    if (sel_items == 1) {
        // Only one item is selected
        get(lv->ListView, MUIA_List_Active, &itemp);
        if (itemp != MUIV_NList_Active_Off) {
            DoMethod(lv->ListView, MUIM_NList_GetEntry, itemp, &vcptr);
            if (vcptr->is_dir == FALSE) {
                // Only one item selected, no need to queue stuph
                LeftListviewDblClickSingle(cn);
                return;
            }
        } else {
            // Only one item selected, no need to queue stuph
            LeftListviewDblClickSingle(cn);
            return;
        }
    }

    // Multiple items selected
    BufferFlush(&cn->temp_buffer);

    // Initialize the stack
    cn->stack_cur = STACK_SIZE;

    id = MUIV_List_NextSelected_Start;
    QueueClear(cn);
    BufferOpen(&cn->queue_buffer, TRUE);

    // Uploads will queue
    for (;;) {
        DoMethod(lv->ListView, MUIM_List_NextSelected, &id);
        if (id == MUIV_List_NextSelected_End) 
                break;
        
        DoMethod(lv->ListView, MUIM_List_GetEntry, id, &vcptr);
        
        if (vcptr->is_dir) {
            b_done_dirs = TRUE;

            // Skip ".." (previous dir)
            if (vcptr->name[0] == '.' && vcptr->name[1] == '.' && vcptr->name[2] == 0)
                continue;

            // Add remote makedir
            sprintf(temp, "%s/%s", cn->rv.QueueBasePath, vcptr->name);
            ptr = QueuePushAddStr(cn, &cn->queue_buffer, temp, CMD_REMOTE_MKDIR, 0, 0, FALSE);
            if (ptr == 0)
                break;

            // Add local directory scanning
            get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itemp);
            sprintf(temp, "%s%s", (char *) itemp, vcptr->name);
            if (caf_getlastchar(lv->CurrentPath) == '/')
                offset = 0;
            else
                offset = 1;

            if (LocalDirScan(cn, temp, lv->CurrentPath, caf_strlen(lv->CurrentPath)-offset) == FALSE)
                break;
        } else {
            if (b_firstfile && b_done_dirs) {
                // If we are doing the first file we need to reset the path both local and
                // remote to the initial paths that were active at time the user pressed
                // the button, this is because we know and assume that directories are first
                // in the listbox
                b_firstfile = FALSE;

                ptr = QueuePushAddStr(cn, &cn->queue_buffer, local_path, CMD_LOCAL_BASEPATH, 0, 0, TRUE);
                if (ptr == 0)
                    break;

                if (cn->rv.QueueBasePath[0] == 0)
                    caf_strncpy(cn->rv.QueueBasePath, "/", 512);

                ptr = QueuePushAddStr(cn, &cn->queue_buffer, cn->rv.QueueBasePath, CMD_REMOTE_CHDIR, 0, 0, FALSE);
                if (ptr == 0)
                    break;
            }

            ptr = QueuePushAddStr(cn, &cn->queue_buffer, vcptr->name, CMD_UPLOAD, 0, 0, FALSE);
            if (ptr == 0)
                break;
        }
    }

    MUI_UpdateQueueListbox(cn, TRUE);
    BufferClose(&cn->queue_buffer);
}

void OnDeleteBtnClick(Connection *cn)
{
    RemoteView *rv = &cn->rv;
    int sel_items, command;
    ViewColumn *vcptr;
    void *ptr;
    LONG id;

    // Is only one item selected ?
    sel_items = MUI_GetListboxSelItemCount(rv->ListView);
    if (sel_items == 1) {
            // Only one item selected, no need to queue stuph, delete the file already
            id = MUIV_List_NextSelected_Start;
            DoMethod(rv->ListView, MUIM_List_NextSelected, &id);
            if (id != MUIV_List_NextSelected_End) {
                    DoMethod(rv->ListView, MUIM_List_GetEntry, id, &vcptr);
                    InitiateFileDeletion(cn, vcptr->name);
                    return;
            }
    }

    // Multiple select, deletes will queue
    id = MUIV_List_NextSelected_Start;
    QueueClear(cn);
    BufferOpen(&cn->queue_buffer, TRUE);
    for (;;) {
        DoMethod(rv->ListView, MUIM_List_NextSelected, &id);
        if (id == MUIV_List_NextSelected_End) 
            break;

        // 
        DoMethod(rv->ListView, MUIM_List_GetEntry, id, &vcptr);

        // Skip ".." (previous dir)
        if (vcptr->name[0] == '.' && vcptr->name[1] == '.' && vcptr->name[2] == 0)
            continue;

        ptr = BufferAddStr(&cn->queue_buffer, vcptr->name);
        if (!ptr) {
            MUI_AddStatusWindow(cn, "ERROR: Queue buffer is full cannot continue");
            break;
        }

        command = CMD_REMOTE_DELETE;
        if (QueuePush(cn, command, ptr, 0, 0, FALSE) == FALSE) {
            MUI_AddStatusWindow(cn, "ERROR: QueuePush() failed, queue items buffer is full, cannot continue with directory scanning");
            break;
        }
    }

    MUI_UpdateQueueListbox(cn, TRUE);
    BufferClose(&cn->queue_buffer);
}

void OnLeftListviewDblClick(Connection *cn)
{
    LocalView *lv = &cn->lv;
    BPTR lock, lockp;
    ViewColumn *vc;
    char temp[512];
    IPTR itmp;
    int chr;

    get(lv->ListView, MUIA_List_Active, &itmp);
    if (itmp != MUIV_NList_Active_Off) {
        DoMethod(lv->ListView, MUIM_NList_GetEntry, itmp, &vc);
        if (vc->is_dir == TRUE) {
            if (!strcmp(vc->name, "..")) {
                lock = Lock(cn->lv.CurrentPath, ACCESS_READ);
                if (lock) {
                    lockp = ParentDir(lock);
                    if (lockp) {
                        NameFromLock(lockp, temp, 511);
                        caf_strncpy(cn->lv.CurrentPath, temp, 511);
                        set(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, (IPTR) cn->lv.CurrentPath);
                        UnLock(lockp);
                        RefreshLocalView(cn);
                    } else
                        MUI_AddStatusWindow(cn, "ParentDir() failed");

                    UnLock(lock);
                } else
                    MUI_AddStatusWindow(cn, "Lock() failed");
            } else {
                get(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, &itmp);
                chr = caf_getlastchar((char *) itmp);
                if (chr == ':' || chr == '/')
                    sprintf(temp, "%s%s", (char *) itmp, vc->name);
                else
                    sprintf(temp, "%s/%s", (char *) itmp, vc->name);

                set(cn->S_LEFT_VIEW_PATH, MUIA_String_Contents, (IPTR) &temp);
                RefreshLocalView(cn);
            }
        } else {
            InitiateFileTransfer(cn, vc->name, FALSE);
        }
    }
}

void OnRightListviewDblClick(Connection *cn)
{
    RemoteView *rv = &cn->rv; 
    ViewColumn *vc;
    char temp[512];
    IPTR itmp;
    int chr;

    get(rv->ListView, MUIA_List_Active, &itmp);
    if (itmp != MUIV_NList_Active_Off) {
        DoMethod(rv->ListView, MUIM_NList_GetEntry, itmp, &vc);
        if (vc->is_dir == TRUE) {
            if (!strcmp(vc->name, "..")) {
                SendFtpCommand(cn, "CDUP", FALSE);
            } else {
                get(cn->S_RIGHT_VIEW_PATH, MUIA_String_Contents, &itmp);
                chr = caf_getlastchar((char *) itmp);
                if (chr == ':' || chr == '/')
                    sprintf(temp, "CWD %s%s", (char *) itmp, vc->name);
                else
                    sprintf(temp, "CWD %s/%s", (char *) itmp, vc->name);

                SendFtpCommand(cn, temp, FALSE);
            }
        } else {
            InitiateFileTransfer(cn, vc->name, TRUE);
        }
    }
}

void LeftListviewDblClickSingle(Connection *cn)
{
    LocalView *lv = &cn->lv;
    ViewColumn *vc;
    IPTR itmp;

    get(lv->ListView, MUIA_List_Active, &itmp);
    if (itmp != MUIV_NList_Active_Off) {
        DoMethod(lv->ListView, MUIM_NList_GetEntry, itmp, &vc);
        InitiateFileTransfer(cn, vc->name, FALSE);
    }
}

void LocalBasePath(Connection *cn, char *path)
{
    caf_strncpy(cn->lv.CurrentPath, path, 512);
}

void RemoteChdir(Connection *cn, char *path)
{
    SendCWDCMD(cn, path);
}

/* Site Window */
void OnSiteWindowNew()
{
    if (g_AddressBook.used_hosts+1 >= MAX_HOSTS) {
        set(g_AddressBook.Window_HostsFull, MUIA_Window_Open, TRUE);
        return;
    }
    g_AddressBook.b_new_entry = TRUE;

    set(g_AddressBook.S_ENTRYNAME, 		MUIA_String_Contents, (IPTR) "");
    set(g_AddressBook.S_HOSTNAME, 		MUIA_String_Contents, (IPTR) "");
    set(g_AddressBook.S_PORT, 			MUIA_String_Contents, (IPTR) "21");
    set(g_AddressBook.S_USERNAME, 		MUIA_String_Contents, (IPTR) "");
    set(g_AddressBook.S_PASSWORD, 		MUIA_String_Contents, (IPTR) "");
    set(g_AddressBook.RDIO_CONN_TYPE, 	MUIA_Radio_Active, 0);
    set(g_AddressBook.Window, MUIA_Window_ActiveObject, g_AddressBook.S_ENTRYNAME);
    EvalSiteWindowStatus();
}

void OnSiteWindowChange()
{
    Host *hst;
    IPTR itmp;

    if (g_AddressBook.b_new_entry) {
        hst = &g_AddressBook.Hosts[g_AddressBook.used_hosts];
        g_AddressBook.used_hosts++;
    } else {
        hst = &g_AddressBook.Hosts[g_AddressBook.selected_host];
    }

    caf_memset(hst, 0, sizeof(Host));

    get(g_AddressBook.S_ENTRYNAME, MUIA_String_Contents, &itmp);
    caf_strncpy(hst->entryname, (char *) itmp, 128);

    get(g_AddressBook.S_HOSTNAME, MUIA_String_Contents, &itmp);
    caf_strncpy(hst->hostname, (char *) itmp, 128);

    get(g_AddressBook.S_PORT, MUIA_String_Contents, &itmp);
    caf_strncpy(hst->port, (char *) itmp, 8);

    get(g_AddressBook.S_USERNAME, MUIA_String_Contents, &itmp);
    caf_strncpy(hst->username, (char *) itmp, 32);

    get(g_AddressBook.S_PASSWORD, MUIA_String_Contents, &itmp);
    caf_strncpy(hst->password, (char *) itmp, 32);

    get(g_AddressBook.RDIO_CONN_TYPE, 	MUIA_Radio_Active, &itmp);
    if (itmp == 0)
        hst->conn_type = CTYPE_ACTV;
    else
        hst->conn_type = CTYPE_PASV;

    g_AddressBook.b_new_entry = FALSE;
    UpdateSiteWindowListBox(g_AddressBook.selected_host);
}

void OnSiteWindowDelete()
{
    IPTR itmp;
    int i, used = 0;

    if (g_AddressBook.b_new_entry) {
        g_AddressBook.b_new_entry = FALSE;
        UpdateSiteWindowContents(0);
        EvalSiteWindowStatus();
        return;
    }

    get(g_AddressBook.LV_HOSTS, MUIA_List_Active, &itmp);
    if (itmp != MUIV_NList_Active_Off) {
        for (i = 0; i < g_AddressBook.used_hosts; i++) {
            if (i == itmp)
                continue;

            g_AddressBook.Hosts_Temp[used] = g_AddressBook.Hosts[i];
            used++;
        }

        caf_memset(&g_AddressBook.Hosts, 0, sizeof(Host)*MAX_HOSTS);
        g_AddressBook.used_hosts = used;
        for (i = 0; i < g_AddressBook.used_hosts; i++)
            g_AddressBook.Hosts[i] = g_AddressBook.Hosts_Temp[i];

        UpdateSiteWindow(0);
    }
}

void OnSiteWindowSave(Connection *cn)
{
    if (SaveConfig() == TRUE) {
        MUI_AddStatusWindow(cn, "STATUS: Configuration saved");
    } else {
        MUI_AddStatusWindow(cn, "STATUS: Error while saving configuration");
    }
}

void OnSiteWindowExit(Connection *cn)
{
    set(g_AddressBook.Window, MUIA_Window_Open, FALSE);
 
    /* Saving address book on window exit */
    OnSiteWindowSave(cn);


}

void OnSiteWindowHostsLVDBLClick(Connection *cn)
{
    ClientInfo *ci = &cn->ci;
    HostInfo *hi = &cn->hi;
    Host *hst;
    IPTR itmp;

    get(g_AddressBook.LV_HOSTS, MUIA_List_Active, &itmp);
    if (itmp != MUIV_NList_Active_Off) {
            hst = &g_AddressBook.Hosts[itmp];
        caf_strncpy(hi->hostname, 	hst->hostname, HOSTNAME_CONFIG_LEN);
        caf_strncpy(hi->ip_s,		hst->hostname, HOSTNAME_CONFIG_LEN);
        hi->port = caf_atol(hst->port);
        caf_strncpy(hi->username, 	hst->username, USERNAME_CONFIG_LEN); 
        caf_strncpy(hi->password, 	hst->password, USERNAME_CONFIG_LEN); 
        hi->conn_type = hst->conn_type;

        set(g_AddressBook.Window, MUIA_Window_Open, FALSE);
        if (ci->b_connected) {
            MUI_AddStatusWindow(cn, "ERROR: Disconnect first!");
        } else {
            FtpConnect(cn);
        }
    }
}

void OnSiteWindowHostsSelectChange(Connection *cn)
{
    IPTR itmp;

    get(g_AddressBook.LV_HOSTS, MUIA_NList_Active, &itmp);
    g_AddressBook.selected_host = itmp;
    UpdateSiteWindowContents(itmp);
}

void UpdateSiteWindowListBox(int active)
{
    int i;

    // Make the list quiet and clear it
    set(g_AddressBook.LV_HOSTS, MUIA_NList_Quiet, TRUE);
    MUI_ClearListbox(g_AddressBook.LV_HOSTS);
    for (i = 0; i < g_AddressBook.used_hosts; i++)
        DoMethod(g_AddressBook.LV_HOSTS, MUIM_NList_InsertSingle, (IPTR *) g_AddressBook.Hosts[i].entryname, MUIV_NList_Insert_Bottom);

    set(g_AddressBook.LV_HOSTS, MUIA_NList_Active, active);
    set(g_AddressBook.LV_HOSTS, MUIA_NList_Quiet, FALSE);

    EvalSiteWindowStatus();
}

void UpdateSiteWindowContents(int active)
{
    Host *hst;

    hst = &g_AddressBook.Hosts[active];	
    set(g_AddressBook.S_ENTRYNAME, MUIA_String_Contents, (IPTR) hst->entryname);
    set(g_AddressBook.S_HOSTNAME, MUIA_String_Contents, (IPTR) hst->hostname);
    set(g_AddressBook.S_PORT, MUIA_String_Contents, (IPTR) hst->port);
    set(g_AddressBook.S_USERNAME, MUIA_String_Contents, (IPTR) hst->username);
    set(g_AddressBook.S_PASSWORD, MUIA_String_Contents, (IPTR) hst->password);
    set(g_AddressBook.RDIO_CONN_TYPE, MUIA_Radio_Active, hst->conn_type);
}

void UpdateSiteWindow(int active)
{
    UpdateSiteWindowListBox(active);
    UpdateSiteWindowContents(active);
    EvalSiteWindowStatus();
}

void EvalSiteWindowStatus()
{
    BOOL bState;

    if (g_AddressBook.used_hosts || g_AddressBook.b_new_entry)
        bState = FALSE;
    else
        bState = TRUE;

    MUI_GetSet(g_AddressBook.BTN_SAVE, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.BTN_DELETE, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.S_ENTRYNAME, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.S_HOSTNAME, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.S_PORT, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.S_USERNAME, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.S_PASSWORD, MUIA_Disabled, bState);
    MUI_GetSet(g_AddressBook.RDIO_CONN_TYPE, MUIA_Disabled, bState);
}

void OnGlobalSettingsSave()
{
    IPTR itmp;

    set(g_GlobalSettings.Window, MUIA_Window_Open, FALSE);

    get(g_GlobalSettings.S_TRANSFER_SOCKET_TIMEOUT, MUIA_String_Contents, &itmp);
    g_GlobalSettings.SettingsArray[SETTING_TRANSFER_TIMEOUT] = caf_atol((char *) itmp);

    get(g_GlobalSettings.S_CONNECTION_SOCKET_TIMEOUT, MUIA_String_Contents, &itmp);
    g_GlobalSettings.SettingsArray[SETTING_CONNECT_TIMEOUT] = caf_atol((char *) itmp);

    get(g_GlobalSettings.RDIO_QUEUE_TYPE, MUIA_Radio_Active, &itmp);
    g_GlobalSettings.SettingsArray[SETTING_QUEUE_TYPE] = itmp;

    get(g_GlobalSettings.RDIO_DELETE_PARTIAL, MUIA_Radio_Active, &itmp);
    g_GlobalSettings.SettingsArray[SETTING_KEEP_PARTIAL] = itmp;
}

void EvalGlobalSettingsWindow()
{
    char temp[512];

    sprintf(temp, "%d", g_GlobalSettings.SettingsArray[SETTING_TRANSFER_TIMEOUT]);
    set(g_GlobalSettings.S_TRANSFER_SOCKET_TIMEOUT, MUIA_String_Contents, temp);

    sprintf(temp, "%d", g_GlobalSettings.SettingsArray[SETTING_CONNECT_TIMEOUT]);
    set(g_GlobalSettings.S_CONNECTION_SOCKET_TIMEOUT, MUIA_String_Contents, temp);

    set(g_GlobalSettings.RDIO_QUEUE_TYPE, MUIA_Radio_Active, g_GlobalSettings.SettingsArray[SETTING_QUEUE_TYPE]);	
    set(g_GlobalSettings.RDIO_DELETE_PARTIAL, MUIA_Radio_Active, g_GlobalSettings.SettingsArray[SETTING_KEEP_PARTIAL]);
}
