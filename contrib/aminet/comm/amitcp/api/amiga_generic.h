/*Header for amiga_generic.c */
/* Structures that are used in other files */

#ifndef API_AMIGA_GENERIC_H
#define API_AMIGA_GENERIC_H

struct newselbuf {
  int s_state;
#define	SB_CLEAR	0		/* wait condition cleared */
#define	SB_WILLWAIT	1		/* will wait if not cleared */
#define	SB_WAITING	2		/* waiting - wake up */
  int s_count;
  struct newselitem {
    struct	newselitem  *	si_next;	/* next selbuf in item chain */
    struct	newselitem **	si_prev;	/* back pointer */
    struct	newselbuf   *	si_selbuf;	/* 'thread' waiting */
  } s_item[0]; /* selitems are allocated at select when right
		  number of descriptors are known */
};


#endif /*API_AMIGA_GENERIC_H*/
