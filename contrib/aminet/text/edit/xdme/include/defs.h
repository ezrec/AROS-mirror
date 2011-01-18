
/*
 * DEFS.H
 *
 *	(C)Copyright 1987 by Matthew Dillon, All Rights Reserved
 *
 *	ARexx support added 03/19/88  -  Kim DeVaughn
 *
 */

#ifndef DEFS_H
#define DEFS_H

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#undef PATCH_RXVARS

#include "all.h"
#include "null.h"

/* added for keys */
#ifndef NOT_DEF
#   include "keyhashes.h"
#else
#   define currenthash()
#   define init_kb()
#   define exit_kb()
#endif

/* added for menus */
#ifndef NOT_DEF
#   include "menubase.h"
#else
#   define currentmenu()
#   define do_itemadd do_menuadd
#   define do_delitem do_menudel
#endif

/* #ifndef VARSBASES_C */
#   ifndef VBASE
#	define VBASE void*
#   endif
#   ifndef VARS
#	define VARS    void
#   endif
/* #endif */

#ifndef MACRO
#   define MACRO void
#endif

/* Added so ActualBlock is const everywhere except block.c */
#ifndef BLOCK_C
#   define NOT_BLOCK_C_CONST const
#else
#   define NOT_BLOCK_C_CONST
#endif

// cursor.c needs this
struct pos
{
    UBYTE * ptr;
    ULONG   len;
    Line    lin;
    Column  col;
};

/* Make GCC happy */
struct RexxMsg;

/* do this after all types have been defined */
#define DEFUSERCMD(str,nargs,flags,ret,func,param,ext)  extern ret func param;
#define DEFCMD(ret,func,param)                          extern ret func param;
#include "prog-protos.h"
#undef DEFUSERCMD
#undef DEFCMD
#define DEFUSERCMD(str,nargs,flags,ret,func,param,ext)  ret func param ext
#define DEFCMD(ret,func,param)                          ret func param

/* asm */
BOOL WildCmp (const char *, const char *);

/* Macros are faster than Functions */
#define alloclptr(lines)    AllocMem ((lines)*sizeof(LINE), 0)
#define allocb(bytes)       AllocMem ((bytes), 0)
#define allocl(lwords)      AllocMem ((lwords)<<2, 0)
#define bmovl(s,d,n)        memmove ((d), (s), (n) << 2)
#define movmem(s,d,n)       memmove ((d), (s), (n))
#define setmem(d,n,c)       memset ((d), (c), (n))

#define GETLINE(ed,nr)      ((ed)->list[(nr)])
#define SETLINE(ed,nr,ptr)  (ed)->list[(nr)] = (ptr)
#define LENGTH(line)        (strlen((char *)(line)))
#define CONTENTS(line)      ((char *)(line))
#define GETTEXT(ed,nr)      (CONTENTS(GETLINE((ed),(nr))))


#undef assert
#define assert(exp)         if (!(exp)) printf ("%s %d\n", __FILE__, __LINE__), exiterr ("assert failed")

#define  PATHSIZE	256	/* HD added for Requesters */

#define SCROLL_LEFT	0
#define SCROLL_RIGHT	1

#endif /* DEFS_H */
