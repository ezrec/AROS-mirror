

#include <stdarg.h>

#if 1
#   define BOOL int
#endif

struct _DSTR {
    int   len;
    int   use;
    char *str;
}; /* struct DSTR */

#define DSTR	  struct _DSTR





char*DynValue  (DSTR *pstr);
void DynInit   (DSTR *pstr);
int  DynLength (DSTR *pstr);
void DynClear  (DSTR *pstr);
void vDynAppend(DSTR *pstr, int num, va_list adds);
void DynSet    (DSTR *pstr, int num, ...);
void DynAppend (DSTR *pstr, int num, ...);
BOOL DynApp1   (DSTR *dest, char* str);

int  DynStrip	 (DSTR *pstr);
int  DynDecomment(DSTR *pstr);


extern const DSTR _EmptyDyn;
#define      EmptyDyn	{ 0,0,NULL }

#define DynInit(pstr)   *(pstr) = _EmptyDyn
#define DynValue(pstr)  ((pstr)->str)
#define DynLength(pstr) ((pstr)->use)

