/* Prototypes for functions defined in
main.c
 */

struct WindowData;

extern struct Library * CxBase;

extern struct Library * KeymapBase;

extern struct IntuitionBase * IntuitionBase;

extern struct Library * GadToolsBase;

extern struct Library * DiskfontBase;

extern struct Library * AslBase;

extern struct RxsLib * RexxSysBase;

extern struct ReqToolsBase * ReqToolsBase;

extern struct GfxBase * GfxBase;

extern struct Library * IconBase;

#ifdef NEED_UTILITYBASE_UTILITYBASE
extern struct UtilityBase * UtilityBase;
#else
extern struct Library * UtilityBase;
#endif

extern struct DosLibrary * DOSBase;

extern struct Library * WorkbenchBase;

extern struct Library * IFFParseBase;

extern struct ExecBase * SysBase;

extern struct LocaleBase * LocaleBase;

extern char programname[256];

extern struct MsgPort * WndPort;

extern struct MsgPort * AppPort;

extern struct DisplayData * AktuDD;

extern struct DisplayInhalt * AktuDI;

extern struct List FileList;

extern struct List DisplayInhaltList;

extern int secretpos;

extern char * secret[];

long ElementZahl(struct List * );

BOOL QuitView(long , struct DisplayData * );

BOOL QuitRequester(long , APTR );

void DoFileXWindowMessage(void);

void DoAppWindowMessage(void);

void DoARexxMsg(void);

extern struct DiskObject * dob;

char * ParseArgsAndAllocThings(void);

void __saveds NewTaskStart(void);

int main(void);

void About(void);

void MyBeep(void);

BOOL LoadUserDisplaytyp(char * );

BOOL LoadUserDisplaytypFR(void);

void SetScrollerGadget(struct DisplayData * );

long GetFileLaenge(BPTR );

long GetFileLaengeName(char * );

void FreeNLNode(struct Node * );

void AddNLName(struct List * , char * );

struct FileData * AllocFD(WORD );

void FreeFD(struct FileData * );

void FreeDD(struct DisplayData * );

struct DisplayData * AllocDD(struct DisplayInhalt * , struct FileData * );

void FreeDI(struct DisplayInhalt * );

struct DisplayInhalt * AllocDI(void);

BOOL MyOpen(char * , struct DisplayData * );

BOOL FileLoaded(struct FileData * );

void PrintFile(struct FileData * );

BOOL SaveFile(struct FileData * );

BOOL OpenFile(struct DisplayData * );

void ClearDDFile(struct DisplayData * , BOOL );

BOOL SaveNewName(char * , struct FileData * );

BOOL Save(struct FileData * );

void UpdateAllStatusZeilen(struct FileData * );

BOOL SaveAsFile(struct FileData * );

void DisplayFromScroller(struct DisplayData * );

void Mark(struct DisplayData * );

void Palette(void);

void MyAddSignal(ULONG , void (* )());

void MyRemoveSignal(ULONG );

void MyAddWindow(struct WindowData * );

void MyRemoveWindow(struct WindowData * );

void CloseAllOpenWindows(void);

void __saveds DoWindow(void);

UBYTE * AllocRequester(void);

void FreeRequester(void);

void LocalizeAll(void);

UBYTE * OpenLibs(void);

void CloseAll(void);

void CloseLibs(void);

void Gedenken(void);

