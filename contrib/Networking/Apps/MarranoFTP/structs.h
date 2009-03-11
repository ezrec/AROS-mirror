#ifndef STRUCTS_H
#define STRUCTS_H

#if defined(__AROS__)
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif

// SockConnect() API
enum {
    CSOCKERR_NO_ERROR = 0,                      // No error, connect() connected early
    CSOCKERR_NATIVE,                            // Socket error is native one, inspect sockerr
    CSOCKERR_BINDFAILED,                        // Bind() failed
    CSOCKERR_SOCKCREATFAILED,                   // Socket() failed
    CSOCKERR_SETNONBLOCKFAILED,                 // Cannot set socket to non blocking
    CSOCKERR_WRONGPARAMS,                       // Wrong parameters passed to SockConnect()
};

enum {
    CSOCKSTATUS_EARLYCONNECT = 0,               // connect() connected early
    CSOCKSTATUS_OK = 0,                         // 
    CSOCKSTATUS_PENDING,                        // connect() is pending
    CSOCKSTATUS_ERROR                           // SockConnect() failed, examine HostInfo csockerr
};

typedef struct CSOCK {
    int                 ip_i;                   // [IN ] Ip to connect to
    int                 port;                   // [IN ] Port to connect to
    int                 socket;                 // [OUT] Socket variable that will receive created
                                                //       socket
    int                 csockerr;               // [OUT] Custom Sock Error
    int                 sockerr;                // [OUT] Native Socket Error
} CSOCK;

int SockConnect(CSOCK *csock);

//

enum { 	
    FTP_CMD_CONNECT, FTP_CMD_DISCONNECT, FTP_CMD_QUIT, 
    CMD_DOWNLOAD, CMD_UPLOAD, CMD_REMOTE_DELETE, CMD_RENAME, 
    CMD_LOCAL_DIR_SCAN, CMD_REMOTE_DIR_SCAN,
    CMD_LOCAL_MKDIR, CMD_REMOTE_MKDIR,
    CMD_LOCAL_BASEPATH, CMD_REMOTE_CHDIR,
    STATUS_QUEUED, STATUS_COMPLETED, STATUS_ERROR 
};

enum { 
    SELECT_ERROR = -2, 
    ANSWER_PENDING, 
    NO_ERRORS, 
    COMMAND_PENDING, 
    NON_BLOCKING_COMMAND, 
    NOT_CONNECTED, 
    FLOW_CONTROL_ERROR 
};

enum { CTYPE_ACTV, CTYPE_PASV, MAX_HOSTS = 128 };
enum eFailedTS 		{ OPT_DOWNLOAD_DELETE_PARTIAL, OPT_DOWNLOAD_KEEP_PARTIAL }; // Failed Transfer Setting
enum nSettings 		{ NUM_OF_SETTINGS = 6 };
enum eQueueSettings { OPT_QUEUE_STOP_ON_ERROR, OPT_QUEUE_PROCESS_ALL };
enum eSettings 		{ SETTING_SOCKET_TIMEOUT, SETTING_TRANSFER_TIMEOUT, SETTING_CONNECT_TIMEOUT, SETTING_QUEUE_TYPE, SETTING_KEEP_PARTIAL, SETTING_LOCAL_REFRESH_AFTER_RETR };
enum eRefreshAfter  { OPT_NO_REFRESH_AFTER_RETR, OPT_REFRESH_AFTER_RETR };
enum eConfVer		{ CONFIG_VERSION1 = 0x100 };

typedef struct FTPMessage
{
    struct Message msg;
    int command;
} FTPMessage;

typedef struct TextMessage
{
    struct Message      msg;
    char                *data;
} TextMessage;

typedef struct TimerVal
{
    double              start_time;
    double              current_time;
    double              end_time;
} TimerVal;

typedef struct ViewColumn
{
    char                *name;
    char                *size;
    char                *flags;
    char                *date;
    BOOL                is_dir;
} ViewColumn;

typedef struct QueueColumn
{
    int                 command;
    char                *name;
    int                 status;
} QueueColumn;

typedef struct ClientInfo
{
    char                hostname[512];
    char                filename[512];
    char                ip_s[32];
    int                 ip_i;
    int                 port;
    SOCKET              cmd_socket;             // Command Socket, ftp commands pass here
    SOCKET              listen_socket;          // Listen socket, the socket to accept transfers of any kind
    SOCKET              transfer_socket;        // The transfer socket returned by accept()
    int                 bytes_transfered;       // Transfer size
    int                 last_transfer_in_bytes;
    BPTR                filehandle;
    BOOL                b_eof;
    int                 file_size, file_readed, file_sent;
    int                 queue_step, queue_to_step;
    
    BOOL	        b_file_transfer,        // We are transfering
                        b_file_download,        // We are downloading, if false uploading
                        b_connected,            // We are connected, cmd socket
                        b_connecting,           // We have issued a connect()
                        b_waitingfordataport,   // We are listening for an incoming connection, pre-dt-transf.
                        b_transfer_error,       // Last transfer didn't work out
                        b_passive,              // Passive connection
                        b_transfering,          // We are transfering either a file or a 'LIST'.
                        b_transfered,           // This flag is set when g_b_transfering goes to false, to
                                                // let the transfer status counter to be updated when
                                                // the transfer socket closes, it's set to false when
                                                // the final update is done.
                        b_can_send_command,     // If this flag is set we can safely send commands to the ftp server
                        b_directory_processed,  // If the data of the remote directory has been processed already
                        b_user_accepted,        // If the user at login has been accepted
                        b_pass_accepted,        // If the pass at login has been accepted
                        b_request_list,         // If the client is requesting list, otherwise it's a data transfer
                        b_request_download,     // Set if the ftp has to download a file
                        b_request_upload,       // Set if the ftp has to upload a file
                        b_request_conn,         // Set if the ftp has to connect to server
                        b_request_disconn,      // Set if the ftp has to disconnect from server
                        b_request_quit,         // Set if the ftp has to disconnect and quit (g_b_request_disconn) will be set as well
                        b_last_data,            // Set if the ftp has received EOF (conn. close) from server
                        b_processing_queue,     // Set if the ftp is processing download queue
                        b_queue_step;           // Set if the queue is a step queue
} ClientInfo;

#define ENTRYNAME_CONFIG_LEN 128
#define HOSTNAME_CONFIG_LEN 128
#define PORT_COFING_LEN 8
#define USERNAME_CONFIG_LEN 32
#define PASSWORD_CONFIG_LEN 32

typedef struct Host
{
    char                entryname[ENTRYNAME_CONFIG_LEN];
    char                hostname[HOSTNAME_CONFIG_LEN];
    char                port[PORT_COFING_LEN];
    char                username[USERNAME_CONFIG_LEN];
    char                password[PASSWORD_CONFIG_LEN];
    int                 conn_type;
} Host;

typedef struct HostInfo
{
    char                hostname[HOSTNAME_CONFIG_LEN];
    char                ip_s[HOSTNAME_CONFIG_LEN];
    int                 ip_i;
    int                 port;
    int                 conn_type;
    CSOCK               pasvsettings;           // Settings for active connection
    char                username[USERNAME_CONFIG_LEN];
    char                password[PASSWORD_CONFIG_LEN];
} HostInfo;

typedef struct buffer
{
    char                *ptr;
    int                 curpos;
    int                 size;
    BOOL                b_opened;
} buffer;

typedef struct LocalView
{
    APTR                ListView;
    buffer              ListBuffer;             // Buffer for displaying entries in listview
    buffer              SelectedEntriesBuffer;  // Buffer for selected entries in listview
    char                CurrentPath[512];
} LocalView;

typedef struct RemoteView
{
    APTR                ListView;
    buffer              ListBuffer;             // Buffer for displaying entries in listview
    int                 num_lines;
    int                 lines_start[16384];
    buffer              SelectedEntriesBuffer;  // Buffer for selected entries in listview
    char                CurrentPath[512];       // Current remote path
    char                QueueBasePath[512];     // Path that was active when the first queing command was
                                                // issued
} RemoteView;

typedef struct QueueItem
{
    int                 command;
    char                *data;
    char                *c_arg1;
    int                 i_arg1;
    BOOL                b_local_cmd;            // Local commands must be eated by the queue before any other remote 
                                                // command, otherwise the queue processing will be struck waiting for
                                                // ftp events
} QueueItem;

typedef struct StackItem
{
    char                *data;
} StackItem;

/*
    A connection has its associated ClientInfo, HostInfo, LocalView and RemoteView structures,
    and the MUI Window APTR as well.
*/
typedef struct Connection
{
    APTR                Window;
    ClientInfo          ci;
    HostInfo            hi;
    LocalView           lv;
    RemoteView          rv;
    APTR                QueueLV;
    
    buffer              transfer_buffer,
                        cmd_buffer,
                        queue_buffer,
                        temp_buffer;

#define QUEUE_SIZE 8192
    QueueItem           QueuedItems[QUEUE_SIZE+1];
    int                 queue_cur, queue_used, last_queue_update;
    
#define STACK_SIZE 8192
    StackItem           StackStack[STACK_SIZE+1];
    int                 stack_cur;
    
    // Objects
    APTR                LV_STATUS,
                        S_LEFT_VIEW_PATH, 
                        S_RIGHT_VIEW_PATH, 
                        BTN_DOWNLOAD,
                        BTN_UPLOAD, 
                        BTN_DELETE, 
                        BTN_CLEAR_QUEUE,
                        BTN_RUN_QUEUE, 
                        BTN_STEP_QUEUE,
                        BTN_STOP, 
                        BTN_RENAME, 
                        BTN_MAKEDIR, 
                        BTN_CONNDISC, 
                        BTN_REFRESH_L, 
                        BTN_REFRESH_R, 
                        BTN_QUIT, 
                        BTN_DUMP,
                        S_TRANSFER_INFO
                        ;
} Connection;

typedef struct AddressBook
{
    APTR                Window,
                        Window_HostsFull,
                        BTN_HostsFullOk,
                        HostsLV,
                        LV_HOSTS,
                        S_ENTRYNAME,
                        S_HOSTNAME,
                        S_PORT,
                        RDIO_CONN_TYPE,
                        S_USERNAME,
                        S_PASSWORD,
                        BTN_NEW,
                        BTN_CHANGE,
                        BTN_SAVE,
                        BTN_DELETE,
                        BTN_EXIT;
    BOOL                b_new_entry;
    Host                Hosts[MAX_HOSTS];
    Host                Hosts_Temp[MAX_HOSTS];  // For deletion
    int                 used_hosts;
    int                 selected_host;
} AddressBook;


typedef struct GlobalSettings
{
    APTR                Window,
                        S_WAITSELECT_SOCKET_TIMEOUT,
                        S_TRANSFER_SOCKET_TIMEOUT,
                        S_CONNECTION_SOCKET_TIMEOUT,
                        RDIO_QUEUE_TYPE,
                        RDIO_DELETE_PARTIAL,
                        BTN_SAVE,
                        BTN_CANCEL;

    int                 SettingsArray[NUM_OF_SETTINGS];
} GlobalSettings;

#ifdef __AROS__
#define MAKEID(x,y,z,w) MAKE_ID(x,y,z,w)
#endif

#define ID_MFTP MAKE_ID('M','F','T','P')
#define ID_HOST MAKE_ID('H','O','S','T')
#define ID_GLOB MAKE_ID('G','L','O','B')

#endif
