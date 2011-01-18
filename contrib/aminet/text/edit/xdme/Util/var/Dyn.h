

#include <stdarg.h>
#include <exec/types.h>

#define REF_VERS 1

struct _DSTR {
    int   len;
    int   use;
    char *str;
}; /* struct DSTR */

#define DSTR	  struct _DSTR





extern char*DynValue  (DSTR *pstr);
extern void DynInit   (DSTR *pstr);
extern int  DynLength (DSTR *pstr);
extern void DynClear  (DSTR *pstr);
extern void vDynAppend(DSTR *pstr, int num, va_list adds);
extern void DynSet    (DSTR *pstr, int num, ...);
extern void DynAppend (DSTR *pstr, int num, ...);
extern BOOL DynApp1   (DSTR *dest, char* str);

extern BOOL  DynStrip	 (DSTR *pstr);
extern int  DynDecomment(DSTR *pstr);


extern const DSTR _EmptyDyn;
#define      EmptyDyn	{ 0,0,NULL }

/* #define DynInit(pstr)   *(pstr) = _EmptyDyn */
#define DynValue(pstr)  ((pstr)->str)
#define DynLength(pstr) ((pstr)->use)
