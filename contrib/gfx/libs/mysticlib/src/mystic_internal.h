
#ifndef MYSTIC_INTERNAL_H
#define	MYSTIC_INTERNAL_H 1

#include "exec/exec.h"
#include "graphics/view.h"
#include "utility/tagitem.h"


/****************************************************************************

	debugging

****************************************************************************/

#ifndef NDEBUG
#define DB(x)	x
#else
#define DB(x)	;
#endif

#ifdef __MORPHOS__
void dprintf(char *, ... );
#undef kprintf
#define kprintf dprintf
#elif defined(__AROS__)
#include <aros/debug.h>
#else
VOID kprintf(STRPTR,...);
#endif


#define	GUIGFX_VERSION		19
#define	DATATYPES_VERSION	39
#define	CYBERGFX_VERSION	40


typedef struct TagItem * TAGLIST;

#endif
