/* Prototypes for functions defined in
cut.c
 */

void PrintBlock(void);

void Copy(struct DisplayData * );

void Cut(struct DisplayData * );

void CutBlockForce(ULONG , ULONG , struct DisplayData * );

void Paste(struct DisplayData * );

void InsertBlock(struct DisplayData * );

void InsertBlockForce(long , UBYTE * , ULONG , struct DisplayData * );

void AppendBlock(struct DisplayData * );

BOOL LoadBlock(char * );

BOOL LoadBlockFR(void);

BOOL SaveBlock(char * );

BOOL SaveBlockFR(void);

void Fill(ULONG , ULONG , UBYTE * , ULONG , struct DisplayData * );

void SetFillString(char * );

UBYTE * GetFillString(void);

void SetFillMode(ULONG );

ULONG GetFillMode(void);

void OpenFillWindow(void);

void FillDD(struct DisplayData * );

