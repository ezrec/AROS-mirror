/* Prototypes for functions defined in
cursor.c
 */

struct DisplayData;

void CursorMarkOn(struct DisplayData * );

void CursorMarkOff(struct DisplayData * );

void ExpandBlock(long , long , struct DisplayData * );

void ShrinkBlock(long , long , struct DisplayData * );

void SetCursor(long , struct DisplayData * );

void MoveCursorX(WORD , struct DisplayData * );

void MoveCursorY(WORD , struct DisplayData * );

void SetCursorX(WORD , struct DisplayData * );

void SetCursorY(WORD , struct DisplayData * );

void SetCursorStart(struct DisplayData * );

void SetCursorEnd(struct DisplayData * );

void SetCursorStartOfLine(struct DisplayData * );

void SetCursorEndOfLine(struct DisplayData * );

void SetCursorAltLeft(struct DisplayData * );

void SetCursorAltRight(struct DisplayData * );

