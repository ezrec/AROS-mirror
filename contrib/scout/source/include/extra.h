#ifndef SCOUT_EXTRA_H
#define SCOUT_EXTRA_H

#if defined(__GNUC__)

#ifndef _STDLIB_H
    #include <stdlib.h>
#endif

extern size_t stccpy(char *, const char *, size_t);
extern int stcd_l(const char *in, long *value);
extern int stcd_i(const char *in, int *value);

#endif /* __GNUC__ */


#if !defined(__amigaos4__)
#include <exec/types.h>
#include <SDI/SDI_stdarg.h>

STRPTR VASPrintf( CONST_STRPTR fmt, VA_LIST args );
STRPTR VARARGS68K STDARGS ASPrintf( CONST_STRPTR fmt, ... );
#endif /* !__amigaos4__ */

#ifndef CMD_NOTLOADED
#define CMD_NOTLOADED -998
#endif

#if defined(__MORPHOS__)
#ifndef IECLASS_EXTRAWKEY
#define IECLASS_EXTRAWKEY 0x18
#endif
#endif /* __MORPHOS__ */

/* MUI 3.9/4.0 tags */
#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses        0x8042e9a7 /* V20 isg STRPTR *          */
#endif

#ifndef TD_READ64
#define TD_READ64   (CMD_NONSTD + 15)
#define TD_WRITE64  (CMD_NONSTD + 16)
#define TD_SEEK64   (CMD_NONSTD + 17)
#define TD_FORMAT64 (CMD_NONSTD + 18)
#endif

#endif /* SCOUT_EXTRA_H */
