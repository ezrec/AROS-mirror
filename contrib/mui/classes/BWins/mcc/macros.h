#ifndef _MACROS_H
#define _MACROS_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif


/****************************************************************************/

#ifdef __AROS__

#define ASM
#define STDARGS
#define REG(x)

#else

#ifndef SAVEDS
#define SAVEDS  __saveds
#endif

#ifndef ASM
#define ASM     __asm
#endif

#ifndef REGARGS
#define REGARGS __regargs
#endif

#ifndef STDARGS
#define STDARGS __stdargs
#endif

#ifndef INLINE
#define INLINE __inline
#endif

#ifndef REG
#define REG(x)  register __ ## x
#endif

typedef ULONG IPTR;
#define STACKED

#endif /* __AROS__ */

#ifndef NODE
#define NODE(a) ((struct Node *)(a))
#endif

#ifndef MINNODE
#define MINNODE(a) ((struct MinNode *)(a))
#endif

#ifndef LIST
#define LIST(a) ((struct List *)(a))
#endif

#ifndef MINLIST
#define MINLIST(a) ((struct MinList *)(a))
#endif

#ifndef MESSAGE
#define MESSAGE(m) ((struct Message *)(m))
#endif

#ifndef NEWLIST
#define NEWLIST(l) (LIST(l)->lh_Head = NODE(&LIST(l)->lh_Tail), \
                    LIST(l)->lh_Tail = NULL, \
                    LIST(l)->lh_TailPred = NODE(&LIST(l)->lh_Head))
#endif

#ifndef INITPORT
#define INITPORT(p,s) ((p)->mp_Flags = PA_SIGNAL, \
                       (p)->mp_SigBit = (UBYTE)(s), \
                       (p)->mp_SigTask = FindTask(NULL), \
                       NEWLIST(&((p)->mp_MsgList)))
#endif

#ifndef INITMESSAGE
#define INITMESSAGE(m,p,l) (MESSAGE(m)->mn_Node.ln_Type = NT_MESSAGE, \
                            MESSAGE(m)->mn_ReplyPort = (p), \
                            MESSAGE(m)->mn_Length = (l))
#endif

#ifndef debug
#define debug kprintf
#endif

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG)(a)<<24 | (ULONG)(b)<<16 | (ULONG)(c)<<8 | (ULONG)(d))
#endif

#ifndef BOOLSAME
#define BOOLSAME(a,b) (((a) ? TRUE : FALSE)==((b) ? TRUE : FALSE))
#endif

#ifndef MIN
#define MIN(a,b) (((int)(a)<(int)(b)) ? (int)(a) : (int)(b))
#endif

#ifndef MAX
#define MAX(a,b) (((int)(a)>(int)(b)) ? (int)(a) : (int)(b))
#endif

/****************************************************************************/

#endif /* _MACROS_H */
