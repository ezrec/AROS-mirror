#ifndef MYSTIC_DEBUG_H
#define	MYSTIC_DEBUG_H 1

/*********************************************************************
----------------------------------------------------------------------

	MysticView 
	debugging

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

/*--------------------------------------------------------------------

	debugging

--------------------------------------------------------------------*/

#ifdef DEBUG
#define DB(x)	x
#else
#define DB(x)	;
#endif

#ifdef __MORPHOS__
void dprintf(char *, ... );
#undef kprintf
#define kprintf dprintf
#else
VOID kprintf(STRPTR,...);
#endif


#endif
