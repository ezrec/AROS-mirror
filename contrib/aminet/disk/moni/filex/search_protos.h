/* Prototypes for functions defined in
search.c
 */

extern char searchstring[256];

extern char replacestring[256];

extern long searchmode;

void FreeSearchHistory(void);

void InitSearchHistory(void);

int TestHexString(char * );

BOOL SearchNext(int , BOOL , BOOL , struct DisplayData * );

void SetSearchString(char * );

void SetReplaceString(char * );

char * GetAktuSearchString(void);

char * GetAktuReplaceString(void);

void SendMsg(struct Window * , UBYTE );

extern struct Hook SearchHook;

BOOL OpenSearchWindow(BOOL );

