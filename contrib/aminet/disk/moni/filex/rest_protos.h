/* Prototypes for functions defined in
rest.c
 */

BOOL IsHexString(UBYTE * );

BOOL MyGetString(char * , char * , ULONG );

BOOL GetWert(ULONG * , char * , ULONG , ULONG );

void DisplayLocaleText(char * );

ULONG MyRequest(char * , ULONG );

ULONG MyRequestNoLocale(char * , ULONG );

ULONG __stdargs MyFullRequestNoLocale(char * , char * , ...);

ULONG __stdargs MyFullRequest(char * , char * , ...);

char * GetStr(char * );

void JumpToByte(void);

void MarkLocation(ULONG , struct DisplayData * );

void JumpToLocation(LONG , struct DisplayData * );

BOOL MyRequestFile(char * , char * , char * , BOOL );

void ExecuteARexxCommand(char * );

void ExecuteARexxCommandNumber(LONG );

BOOL StringToMem(char * , UBYTE * , long );

BOOL SaveClip(STRPTR , LONG );

UBYTE * GetClip(ULONG * );

BOOL SetClipboardUnit(LONG );

LONG GetClipboardUnit(void);

