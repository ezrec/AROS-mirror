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

#define SAVEDS
#define ASM
#define REG(x)
#define STDARGS

#else /* __AROS__ */

// integer which is large enough to hold a pointer
typedef ULONG IPTR;

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

#define debug kprintf

/****************************************************************************/

#endif /* _MACROS_H */
