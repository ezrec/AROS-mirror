/* Prototypes for functions defined in
mainwindow.c
 */

extern struct Screen * Scr;

extern struct BitMap TempBM;

extern struct RastPort TempRP;

extern WORD TempBreite;

extern WORD TempHoehe;

extern WORD ScreenFlags;

extern char * SollPublicScreenName;

extern APTR VisualInfo;

extern struct ScreenModeData ScreenModeData;

extern struct ColorSpec ScreenColors[9];

extern UWORD __chip Stopwatch[36];

void FreeTempBitMap(void);

BOOL AllocTempBitMap(void);

void SetWaitPointer(struct Window * );

void ClrWaitPointer(struct Window * );

void BlockMainWindow(void);

void ReleaseMainWindow(void);

void SetScreenModeData(BOOL , ULONG , ULONG , ULONG , UWORD , UWORD , BOOL );

LONG OpenDisplay(void);

void CloseFileXWindow(struct DisplayInhalt * );

BOOL CloseDisplay(void);

void SetSollPublicScreenName(char * );

void SetScreenTypePublic(char * );

void ReopenDisplay(long );

void MySetFont(struct TextAttr * , struct DisplayInhalt * );

void MySetFontNameSize(char * , UWORD , struct DisplayInhalt * );

void DoFontrequester(struct DisplayInhalt * );

void SetScreenOwnPublic(void);

BOOL OpenPubScreenListReq(void);

void MyActivateWindow(struct DisplayInhalt * );

void ActivateNextWindow(void);

void ActivatePreviousWindow(void);

BOOL OpenNewWindow(BOOL );

char * GetOwnScreenName(void);

