#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/

#ifdef __AROS__

#define ASM
#define SAVEDS
#define STDARGS
#define REG(x)

#else /* __AROS__ */

#ifndef SAVEDS
#define SAVEDS __saveds
#endif /* SAVEDS */

#ifndef ASM
#define ASM __asm
#endif /* ASM */

#ifndef REGARGS
#define REGARGS __regargs
#endif /* REGARGS */

#ifndef STDARGS
#define STDARGS __stdargs
#endif /* STDARGS */

#ifndef INLINE
#define INLINE __inline
#endif /* INLINE */

#ifndef REG
#define REG(x) register __ ## x
#endif /* REG */

#endif /* __AROS__ */

#ifndef NODE
#define NODE(a) ((struct Node *)(a))
#endif /* NODE */

#ifndef MINNODE
#define MINNODE(a) ((struct MinNode *)(a))
#endif /* MINNODE */

#ifndef LIST
#define LIST(a) ((struct List *)(a))
#endif /* LIST */

#ifndef MINLIST
#define MINLIST(a) ((struct MinList *)(a))
#endif /* MINLIST */

#ifndef MESSAGE
#define MESSAGE(m) ((struct Message *)(m))
#endif /* MESSAGE */

#ifndef NEWLIST
#define NEWLIST(l) (LIST(l)->lh_Head = NODE(&LIST(l)->lh_Tail), \
                    LIST(l)->lh_Tail = NULL, \
                    LIST(l)->lh_TailPred = NODE(&LIST(l)->lh_Head))
#endif /* NEWLIST */

#ifndef QUICKNEWLIST
#define QUICKNEWLIST(l) (LIST(l)->lh_Head = NODE(&LIST(l)->lh_Tail), \
                         LIST(l)->lh_TailPred = NODE(&LIST(l)->lh_Head))
#endif /* QUICKNEWLIST */

#ifndef PORT
#define PORT ((struct MsgPort *)(port))
#endif /* PORT */

#ifndef INITPORT
#define INITPORT(p,s) (PORT(p)->mp_Flags = PA_SIGNAL, \
                       PORT(p)->mp_SigBit = (UBYTE)(s), \
                       PORT(p)->mp_SigTask = FindTask(NULL), \
                       NEWLIST(&(PORT(p)->mp_MsgList)))
#endif /* INITPORT */

#ifndef QUICKINITPORT
#define QUICKINITPORT(p,s,t) (PORT(p)->mp_Flags = PA_SIGNAL, \
                              PORT(p)->mp_SigBit = (UBYTE)(s), \
                              PORT(p)->mp_SigTask = (t), \
                              QUICKNEWLIST(&(PORT(p)->mp_MsgList)))
#endif /* QUICKINITPORT */

#ifndef INITMESSAGE
#define INITMESSAGE(m,p,l) (MESSAGE(m)->mn_Node.ln_Type = NT_MESSAGE, \
                            MESSAGE(m)->mn_ReplyPort = PORT(p), \
                            MESSAGE(m)->mn_Length = ((UWORD)l))
#endif /* INITMESSAGE */

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif /* MAKE_ID */

#ifndef MIN
#define MIN(a,b) ((a<b) ? (a) : (b))
#endif /* MIN */

#ifndef MAX
#define MAX(a,b) ((a>b) ? (a) : (b))
#endif /* MAX */

#ifndef ABS
#define ABS(a) (((a)>0) ? (a) : -(a))
#endif /* ABS */

/****************************************************************************/

#endif /* _MACROS_H */
