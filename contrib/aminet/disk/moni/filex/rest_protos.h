/* Prototypes for functions defined in
rest.c
 */

BOOL IsHexString(UBYTE * );

BOOL MyGetString(char * , char * , ULONG );

BOOL GetWert(ULONG * , char * , ULONG , ULONG );

void DisplayLocaleText(CONST_STRPTR );

ULONG MyRequest(CONST_STRPTR , IPTR );

ULONG MyRequestNoLocale(CONST_STRPTR , IPTR );

#if !defined(__AROS__)
ULONG __stdargs MyFullRequestNoLocale(CONST_STRPTR , CONST_STRPTR , ...);

ULONG __stdargs MyFullRequest(CONST_STRPTR , CONST_STRPTR , ...);
#else
ULONG MyMainFullRequest( CONST_STRPTR, CONST_STRPTR, APTR );
#define MyFullRequestNoLocale(Text, ButtonText...) \
({ \
    IPTR tmpargs[] = { AROS_PP_VARIADIC_CAST2IPTR(ButtonText) }; \
    APTR args = 0; \
    if (sizeof(tmpargs) > 1) \
        args = &tmpargs[1]; \
    MyMainFullRequest(Text, (CONST_STRPTR)tmpargs[0], args); \
})
#define MyFullRequest(Text, ButtonText...) \
({ \
    IPTR tmpargs[] = { AROS_PP_VARIADIC_CAST2IPTR(ButtonText) }; \
    APTR args = 0; \
    if (sizeof(tmpargs) > 1) \
        args = &tmpargs[1]; \
    MyMainFullRequest(GetStr(Text), GetStr((CONST_STRPTR)tmpargs[0]), args); \
})
#endif

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

