/* Prototypes for functions defined in
rest.c
 */

BOOL IsHexString(UBYTE * );

BOOL MyGetString(char * , char * , ULONG );

BOOL GetWert(ULONG * , char * , ULONG , ULONG );

void DisplayLocaleText(CONST_STRPTR );

ULONG MyRequest(CONST_STRPTR , ULONG );

ULONG MyRequestNoLocale(CONST_STRPTR , ULONG );

ULONG __stdargs MyFullRequestNoLocale(CONST_STRPTR , CONST_STRPTR , ...);

ULONG __stdargs MyFullRequest(CONST_STRPTR , CONST_STRPTR , ...);

CONST_STRPTR GetStr(CONST_STRPTR );

void JumpToByte(void);

void MarkLocation(ULONG , struct DisplayData * );

void JumpToLocation(LONG , struct DisplayData * );

BOOL MyRequestFile(char * , CONST_STRPTR , CONST_STRPTR , BOOL );

void ExecuteARexxCommand(CONST_STRPTR );

void ExecuteARexxCommandNumber(LONG );

BOOL StringToMem(char * , UBYTE * , long );

BOOL SaveClip(STRPTR , LONG );

UBYTE * GetClip(ULONG * );

BOOL SetClipboardUnit(LONG );

LONG GetClipboardUnit(void);

