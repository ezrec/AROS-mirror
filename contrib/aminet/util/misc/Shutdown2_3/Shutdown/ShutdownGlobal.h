enum	{	MODE_SHUTDOWN, MODE_LIST, MODE_NAME, MODE_RESET, MODE_FASTRESET };
enum	{	SHUTDOWN_NORMAL, SHUTDOWN_FAST };

enum	{	ERROR_NOKEYPORT=1,ERROR_NOKEYREQUEST,ERROR_NOKEYDEVICE,ERROR_NOKEYRESET,
		ERROR_WEDGESETUP,ERROR_NORESET,ERROR_NOTABLE };

#include "shutdown.h"

#define SIG_NOTIFY		(1 << NotifySignal)
#define SIG_WINDOW		(1 << Window -> UserPort -> mp_SigBit)
#define SIG_KILL		SIGBREAKF_CTRL_C
#define SIG_HANDSHAKE		SIGBREAKF_CTRL_C
#define SIG_NOTICE		SIGBREAKF_CTRL_D
#define SIG_SHUTDOWN		SIGBREAKF_CTRL_E
#define SIG_TIME		(1 << TimePort -> mp_SigBit)

#define SDB_RESET		0
#define SDF_RESET		(1 << 0)

#define SD_CHECK		0
#define SD_EXIT			1

	/* A jump vector (six bytes, just like in library jump tables). */

struct Vector
{
	UWORD	Command;	/* The MC680x0 `JMP ea' opcode. */
	APTR	Address;	/* The destination address. */
};

struct FileInfo
{
	struct MinNode		 Node;
	BPTR			 Handle;
	struct DeviceNode	*DeviceNode;
	ULONG			 Accesses;
	BYTE			 Ignore;
	BYTE			 Pad[3];
	struct Process		*LastAccess;
	UBYTE			 Name[1];
};

struct ShutdownBase
{
	struct Library		 LibNode;

	struct SignalSemaphore	 BlockLock;
	struct SignalSemaphore	 DevBlockLock;
	struct SignalSemaphore	 AccessLock;
	struct SignalSemaphore	 ShutdownLock;

	BYTE			 Shutdown;
	BYTE			 DevShutdown;
	BYTE			 Running;
	BYTE			 Closing;

	struct MinList		 AccessList;
	struct MinList		 ShutdownList;

	VOID			(*Main)();

	struct Process		*Father,
				*Child,
				*CxProcess;

	LONG			 OpenCount;

	LONG			 ErrorCode;
};

struct ShutdownInfo
{
	struct MinNode	 sdi_Node;
	STRPTR		 sdi_Name;
	struct Hook	*sdi_Hook;
};

	/* Patch.asm */

extern VOID			BlockBeginIO();
extern VOID			NewDevExpunge();

	/* LocaleSupport.c */

VOID __regargs			LocaleOpen(STRPTR CatalogName,STRPTR Builtin);
VOID				LocaleClose(VOID);
STRPTR __regargs		GetString(LONG ID);

	/* DevWatch.c */

BYTE				WedgeSetup(VOID);
VOID				DeleteWedges(VOID);
VOID				NewWedge(STRPTR DeviceName);
ULONG __asm			DevExpunge(register __a6 struct Device *Device);
VOID __asm			BlockRoutine(register __a1 struct IOStdReq *Request);

	/* LibCode.c */

VOID __saveds			Main(VOID);
BYTE				OpenAll(BYTE Mode);

LONG __saveds __asm		RexxDispatch(register __a0 struct RexxMsg *Message);
LONG __saveds __asm		Shutdown(register __d0 ULONG Mode);
APTR __saveds __asm		AddShutdownInfoTagList(register __a0 struct Hook *Hook,register __a1 STRPTR Name,register __a2 struct TagItem *TagList);
LONG __saveds __asm		RemShutdownInfo(register __a0 struct ShutdownInfo *Info);

extern struct ExecBase		*SysBase;
extern struct DosLibrary	*DOSBase;
extern struct IntuitionBase	*IntuitionBase;
extern struct GfxBase		*GfxBase;
extern struct RxsLib		*RexxSysBase;
extern struct Library		*UtilityBase,
				*GadToolsBase;
extern struct ShutdownBase	*GlobalBase;

extern struct Process		*HandlerProcess,
				*MainProcess;

extern struct SignalSemaphore	 DeviceWatchSemaphore;

extern BYTE			 NotifySignal;

extern BYTE			 DeviceInUse,
				 DeviceProbablyInUse;
