/* device.h: */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/filehandler.h>
#include <devices/timer.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <utility/date.h>

#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/utility_protos.h>

#include "generic.h"

#define DOS_FALSE   0
#define DOS_TRUE    -1

typedef struct Interrupt	INTERRUPT;
typedef struct Task		TASK;
typedef struct FileLock 	LOCK;	    /*	See LOCKLINK	*/
typedef struct FileInfoBlock	FIB;
typedef struct DosPacket	PACKET;
typedef struct Process		PROC;
typedef struct DeviceNode	DEVNODE;
typedef struct DosInfo		DOSINFO;
typedef struct FileHandle	FH;
typedef struct Message		MSG;
typedef struct MinList		LIST;
typedef struct MinNode		NODE;
typedef struct DateStamp	STAMP;
typedef struct InfoData 	INFODATA;
typedef struct DosLibrary	DOSLIB;
typedef struct ExecBase		EXECLIB;
typedef struct Library		LIB;

#define FILE_DIR    1
#define FILE_FILE   -1

#warning "This should go into volumes.h!"
void Register_File_Handle(struct ACDRBase *, CDROM_OBJ *p_obj);
void Unregister_File_Handle(struct ACDRBase *, CDROM_OBJ *p_obj);
struct DeviceList *Find_Dev_List (CDROM_OBJ *p_obj);
int Reinstall_File_Handles (void);
void Register_Volume_Node(struct ACDRBase *, struct DeviceList *p_volume);
void Unregister_Volume_Node(struct ACDRBase *, struct DeviceList *p_volume);
struct DeviceList *Find_Volume_Node(struct ACDRBase *, char *p_name);

extern t_bool g_disk_inserted;
