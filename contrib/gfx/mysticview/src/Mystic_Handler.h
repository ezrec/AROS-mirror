/*********************************************************************
----------------------------------------------------------------------

	MysticView
	definitions for picture scanning,
	list management and loading

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_HANDLER_H
#define	MYSTIC_HANDLER_H 1

#include "Mystic_Global.h"


//-----------------------------------------------------------

enum SORTMODES
{
 	SORTMODE_NONE, 
	SORTMODE_RANDOM, 
	SORTMODE_ALPHA_FILE, 
	SORTMODE_ALPHA_FULL, 
	SORTMODE_FILESIZE,
	SORTMODE_DATE,
	SORTMODE_REVERSE
};


#define	PICLIST_SEMAPHORE_NAME	"MysticView PicList Rendezvous"


#define	PICID_UNDEFINED		0

#define PHLIST_RELATIVE		0
#define PHLIST_START		1
#define PHLIST_END			2


//-----------------------------------------------------------

struct ScanList
{
	struct SignalSemaphore locksemaphore;
	int numentries;				// number of entries in the list
	int	numscanned;				// number of entries being scanned
	struct List *list;			// the list
};

struct ScanListNode
{
	struct	Node node;
	char	*filepattern;		// the list of filenames/patterns
	char	*rejectpattern;		// reject pattern
	BOOL	ispattern;			// is this a pattern or a single entry?
	BOOL	recursive;			// scan recursively?
	BOOL	includedirs;		// include directories as pictures?
	BOOL	simplescanning;		// dumb scan mode
};

//-----------------------------------------------------------

struct FileList
{
	struct SignalSemaphore locksemaphore;
	int	numentries;
	struct FileListNode *currentnode;		// seekpoint in filelist
	struct List *list;
	int sortmode;				// the list must be kept integral accoding to this!
	BOOL reverse;
};


struct FileListNode
{
	struct Node node;
	unsigned long picID;
	char *fullname;
	unsigned long filesize;
	struct DateStamp datestamp;
	char *formatID;
	int scanindex;				//	"natural" order index for SORTMODE_NONE	
	BOOL isdirectory;
};

//-----------------------------------------------------------

struct ToLoadList
{
	struct SignalSemaphore locksemaphore;
	int	numentries;
	struct List *list;
};

struct ToLoadNode
{
	struct Node node;
	float pri;
	BOOL primary;

		unsigned long picID;
		char *fullname;
		BOOL isdirectory;

	//	struct FileListNode *fnode;
};


//-----------------------------------------------------------

struct LoadedList
{
	struct SignalSemaphore locksemaphore;
	int	numentries;
	struct List *list;
	ULONG numbytes;
};

struct LoadedNode
{
	struct Node node;
	float pri;
	unsigned long picID;
	char *fullname;			// only for internal use!!!
	int lockcount;
	PICTURE *picture;
	//	PICTURE *thumbnail;
	ULONG numbytes;
};


//-----------------------------------------------------------

#define	PICH_Dummy				TAG_USER
#define	PICH_Recursive			(PICH_Dummy+1)
#define	PICH_SortMode			(PICH_Dummy+2)
#define PICH_Notify				(PICH_Dummy+3)
#define PICH_Reject				(PICH_Dummy+4)
#define PICH_SortReverse		(PICH_Dummy+5)
#define PICH_BufferPercent		(PICH_Dummy+6)
#define PICH_AutoCrop			(PICH_Dummy+7)
#define PICH_IncludeDirs		(PICH_Dummy+8)
#define PICH_SimpleScanning		(PICH_Dummy+9)


#define PHNOTIFY_NOTHING			(0x00000000)
#define PHNOTIFY_SCANNING			(0x00000001)
#define PHNOTIFY_SCANNED_NUMBER		(0x00000002)
#define PHNOTIFY_LOADING			(0x00000004)
#define PHNOTIFY_ID_LOADING			(0x00000008)
#define PHNOTIFY_ID_AVAILABLE		(0x00000010)
#define PHNOTIFY_ID_ERROR_LOADING	(0x00000020)
#define PHNOTIFY_PIC_AVAILABLE		(0x00000040)

//-----------------------------------------------------------

struct PicHandler
{
	APTR pichandletask;					// pichandler task
	struct MsgPort *msgport;			// caller's messageport
	struct MsgPort *replyport;			// pichandler's replyport

	BOOL autocrop;
	BOOL recursive;						// global default
	BOOL includedirs;					// global default
	BOOL simplescanning;				// global default
	char *rejectpattern;				// global default
	int sortmode;						// global default
	BOOL reverse;
	int bufferpercent;


	struct ToLoadList *toloadlist;		// list of pictures to be loaded


	//------------------------------------------
	//
	//		Notify

	struct SignalSemaphore notifysemaphore;
	ULONG notifymask;					// what messages to broadcast
	struct List notifylist;				// list of messages pending


	//------------------------------------------
	//
	//		Scanner

	APTR scantask;						// scan task
	struct ScanList *scanlist;			// scan list


	//------------------------------------------
	//
	//		Loader

	APTR loadtask;						// load task
	struct LoadedList *loadedlist;		// loaded list

	//------------------------------------------




	//
	//	lists:
	//

	struct FileList *filelist;

};


//-----------------------------------------------------------

struct PHMessage
{
	struct Message msg;

	struct Node notifylistnode;

	ULONG msgtype;

	//	data:
	
	ULONG numEntries;
	ULONG id;
	char *fullname;
	BOOL scanning;
	BOOL loading;
	LONG error;
};

//-----------------------------------------------------------

typedef struct PicInfo
{
	char	*fullname;
	int		listindex;
	BOOL	isdirectory;
	char	*formatname;

} PICINFO;


//-----------------------------------------------------------

extern char **PicHandler_CreateFileList(struct PicHandler *ph);

extern BOOL PicHandler_Rename(struct PicHandler *pichandler, int id, char *fullname);
extern BOOL PicHandler_Remove(struct PicHandler *pichandler, int id);

extern void PicHandler_FreePicInfo(PICINFO *picinfo);
extern PICINFO *PicHandler_GetPicInfo(struct PicHandler *pichandler, int id);

extern char *PicHandler_GetName(struct PicHandler *pichandler, int id);
extern LONG PicHandler_GetListIndex(struct PicHandler *pichandler, int id);

extern void PicHandler_LockFileList(struct PicHandler *ph);
extern void PicHandler_UnLockFileList(struct PicHandler *ph);

extern struct PicHandler * STDARGS PicHandler_Create(ULONG dummy, ...);
extern void PicHandler_Delete(struct PicHandler *pichandler);
extern ULONG PicHandler_GetSigMask(struct PicHandler *pichandler);

extern BOOL STDARGS PicHandler_SetAttributes(struct PicHandler *pichandler, ...);
extern BOOL STDARGS PicHandler_AddPatternList(struct PicHandler *pichandler, char **patternlist, ...);
extern struct PHMessage *PicHandler_GetNotification(struct PicHandler *handler);
extern void PicHandler_Acknowledge(struct PicHandler *handler, struct PHMessage *msg);

extern ULONG PicHandler_Seek(struct PicHandler *pichandler, int mode, int n);
extern ULONG PicHandler_GetID(struct PicHandler *pichandler, int mode, int n);
extern ULONG PicHandler_GetCurrentID(struct PicHandler *pichandler);

extern ULONG PicHandler_GetNumber(struct PicHandler *pichandler);
extern BOOL PicHandler_RequestPic(struct PicHandler *pichandler, ULONG id);

extern PICTURE *PicHandler_ObtainPicture(struct PicHandler *pichandler, ULONG id);
extern void PicHandler_ReleasePicture(struct PicHandler *pichandler, ULONG id);

extern BOOL STDARGS PostPHMessage(struct PicHandler *handler, ULONG msgtype, ...);

extern struct FileListNode *CreateFileListNode(char *fullname, char *formatID, long filesize, int scanindex, struct DateStamp *datestamp, BOOL isdir);
extern void InsertFileNode(struct FileList *filelist, struct FileListNode *newnode);

extern BOOL STDARGS PicHandler_AppendScanList(char **patternlist, ...);


#endif
