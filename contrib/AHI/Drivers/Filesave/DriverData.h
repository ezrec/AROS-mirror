#ifndef AHI_Drivers_Filesave_DriverData_h
#define AHI_Drivers_Filesave_DriverData_h

#include <exec/libraries.h>
#include <dos/dos.h>
#include <graphics/gfxbase.h>

#include <proto/asl.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/graphics.h>

#include "DriverBase.h"

struct FilesaveBase
{
    struct DriverBase driverbase;
    struct Library*   dosbase;
    struct Library*   gfxbase;
    struct Library*   aslbase;
    struct Library*   dtsbase;
};

#define DRIVERBASE_SIZEOF (sizeof (struct FilesaveBase))


#define DOSBase           ((struct DosLibrary*) FilesaveBase->dosbase)
#define GfxBase           ((struct GfxBase*)    FilesaveBase->gfxbase)
#define AslBase           (FilesaveBase->aslbase)
#define DataTypesBase     (FilesaveBase->dtsbase)

struct FilesaveData
{
	struct DriverData	 fs_DriverData;
	UBYTE			 fs_Flags;
	UBYTE			 fs_Pad1;
	BYTE			 fs_MasterSignal;
	BYTE			 fs_SlaveSignal;
	struct Process		*fs_MasterTask;
	struct Process		*fs_SlaveTask;
	struct FileRequester	*fs_FileReq;
	struct DriverBase	*fs_AHIsubBase;
	ULONG			 fs_Format;
	APTR			 fs_MixBuffer;
	APTR			 fs_SaveBuffer;
	APTR			 fs_SaveBuffer2;
	ULONG			 fs_SaveBufferSize;

	BYTE			 fs_RecMasterSignal;
	BYTE			 fs_RecSlaveSignal;
	struct Process		*fs_RecSlaveTask;
	struct FileRequester	*fs_RecFileReq;
	WORD			*fs_RecBuffer;
};

#define AHIDB_FileSaveFormat	(AHIDB_UserBase+0)	/* Private tag */

#define FORMAT_8SVX		0
#define FORMAT_AIFF		1
#define FORMAT_AIFC		2
#define FORMAT_S16		3
#define FORMAT_WAVE		4

#define SAVEBUFFERSIZE 100000   // in samples (min)
#define RECBUFFERSIZE  10000    // in samples



#endif /* AHI_Drivers_Filesave_DriverData_h */
