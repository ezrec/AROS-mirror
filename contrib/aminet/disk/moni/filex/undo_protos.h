/* Prototypes for functions defined in
undo.c
 */

struct FileData;
struct Speicher;
struct Undostruct;

extern WORD UndoLevel;

extern WORD maxundomemsize;

BOOL AllocUndostructs(struct FileData * );

void FreeUndostructs(struct FileData * );

BOOL Undo(struct FileData * );

BOOL Redo(struct FileData * );

void FreeUndo(struct Undostruct * , struct FileData * );

void AddUndoChar(long , UBYTE , UBYTE , struct FileData * );

void AddUndoCharHex(long , UBYTE , UBYTE , BOOL , struct FileData * );

void AddUndoInsert(long , struct Speicher , struct FileData * );

void AddUndoCut(long , struct Speicher , struct FileData * );

void AddUndoPaste(long , struct Speicher , struct Speicher , struct FileData * );

void PruefeUndomemsizeRedo(struct FileData * );

void PruefeUndomemsize(struct FileData * );

void FreeSpeicher(struct Speicher * , struct FileData * );

void SetMaxUndoMemSize(long );

void SetUndoLevel(long );

