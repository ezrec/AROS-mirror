#define NUM_FASTMSG 500

#define MAX_PLAYERS 4
#define COMNAME "DOOMCOM"

struct PlayerPort
{
    struct MsgPort msgport;
    BOOL inuse;
};

struct ComPort
{
    struct SignalSemaphore sem;
    struct PlayerPort playerport[MAX_PLAYERS];
    APTR pool;
    WORD users;
};

struct ComMsg
{
    struct Message msg;
    APTR pool;
    BYTE flags;
    BYTE playerid;
    WORD datalen;
    doomdata_t dd;
};

#define CMF_ALLOCED 1
#define CMF_DEAD    2

boolean AROSNetLocal_InitNetwork(void);
void AROSNetLocal_NetCmd(void);
void AROSNetLocal_NetSend(void);
void AROSNetLocal_NetGet(void);
void AROSNetLocal_CleanupNetwork(void);
