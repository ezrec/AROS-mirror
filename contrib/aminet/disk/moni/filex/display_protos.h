/* Prototypes for functions defined in
display.c
 */
 
struct DisplayInhalt;
struct DisplayData;
struct FileData;

extern UBYTE CursorAPen;

extern UBYTE CursorBPen;

extern UBYTE CursorLinePen;

extern UBYTE NormalAPen;

extern UBYTE NormalBPen;

extern UBYTE BlockAPen;

extern UBYTE BlockBPen;

void CalcStatusZeilenBreite(struct DisplayData * );

void InvertStatusBalkenRahmen(struct DisplayData * , WORD );

struct DisplayData * GetroffenerView(WORD , WORD , struct DisplayInhalt * );

BOOL SetzeCursorMaus(WORD , WORD , struct DisplayData * , struct DisplayInhalt * );

BOOL StatusBalkenHit(WORD , WORD , struct DisplayInhalt * );

void MarkOn(struct DisplayData * );

void MarkOff(struct DisplayData * );

void MySetWriteMask(struct RastPort * , ULONG );

void CursorOn(struct DisplayData * );

void CursorOff(struct DisplayData * );

void BlockKuerzen(long , long , struct DisplayData * );

void BlockErweitern(long , long , struct DisplayData * );

void ChangeMark(long , long , long , long , struct DisplayData * );

void ByteToString(UBYTE , UBYTE * );

void MyText(struct RastPort * , WORD , WORD , char * , LONG );

void DisplayZeilen(long , long , long , struct DisplayData * );

void DisplayPart(long , long , struct DisplayData * );

void Display(long , struct DisplayData * );

void MoveDisplay(long , struct DisplayData * );

void RedrawDisplay(struct DisplayData * );

void RedrawPart(long, long, struct DisplayData *);

void AllDisplayChar(struct FileData * , long );

void AllDisplay(struct FileData * , long );

void MakeDisplay(struct DisplayInhalt * );

long GetFitHeight(struct DisplayInhalt * );

long GetFitWidth(struct DisplayInhalt * );

void AdjustWindowSize(struct DisplayInhalt * );

void SetStatusZeile(CONST_STRPTR , struct DisplayData * );

void UpdateStatusZeile(struct DisplayData * );

void SetScrollRand(short );

void WechselCursorBereich(struct DisplayData * );

BOOL MakeDisplayBigger(struct DisplayData * );

void ChangeAktuView(struct DisplayData * );

void ActivateNextDisplay(void);

void ActivatePreviousDisplay(void);

BOOL MakeDisplayMax(struct DisplayData * );

BOOL MakeDisplaySmaller(struct DisplayData * );

BOOL NewDisplay(BOOL );

BOOL CloseView(struct DisplayData * , BOOL );

BOOL CloseAktuView(void);

BOOL CloseDIWindow(struct DisplayInhalt * , BOOL );

BOOL CloseAktuWindow(void);

