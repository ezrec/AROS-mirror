/******************************************************************************

    MODUL
	keyhashes.h

    DESCRIPTION
	definition module for all keymodules

******************************************************************************/

#ifndef KEYHASHES_H
#define KEYHASHES_H

/**************************************
		Includes
**************************************/

#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef   EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_NODES_H */


/**************************************
	    Globale Variable
**************************************/


/**************************************
	Defines und Strukturen
**************************************/

#ifndef KEY_INTERNAL

#define TCODE UBYTE
#define TQUAL UWORD
#define KEYTABLE void
#define KEYSPEC  void
#define HASH	 void

#else

#ifndef HASHSIZE
# define HASHSIZE    0x80    /* power of 2   */
#endif

#ifndef HASHMASK
# define HASHMASK  (HASHSIZE-1)
#endif

/* all relevant bits of a possible qual-combi except for REPEAT and NUMLOCK */
#ifndef  QUALMASK
# define QUALMASK  0x77ff
#endif

#define TCODE UBYTE
#define TQUAL UWORD

struct keyspec {
    TQUAL ks_qualifiers;
    TQUAL ks_qualmask;
    TCODE ks_code;
}; /* struct keyspec */

struct KeyHash {
    struct KeyHash *next;   /* next hash    */
#if 0
    UWORD	   mask;    /* qual. mask   */
    UWORD	   qual;    /* qual. comp   */
    UBYTE	   code;    /* keycode	    */
#else
    struct keyspec key;
#endif
    char	 * comm;    /* command	    */
    char	 * help;    /* online help  */
}; /* struct KeyHash */


struct _2Strings {
    char  * from;
    char  * to;
};

struct _3Strings {
    char  * from;
    char  * to;
    char  * help;
};


struct KeyTable {
    struct Node      node;
    struct KeyHash * hash[HASHSIZE];
}; /* struct KeyTable */

#define HASH	     struct KeyHash
#define KEYTABLE     struct KeyTable
#define KEYSPEC      struct keyspec

#define TWOSTRINGS   struct _2Strings
#define THREESTRINGS struct _3Strings



#define KS_CODE(ks)    ((ks)->ks_code)
#define KS_QUAL(ks)    ((ks)->ks_qualifiers)
#define KS_MASK(ks)    ((ks)->ks_qualmask)

#define KS_FILL(ks,q,m,c)  { KS_QUAL(ks) = q; KS_MASK(ks) = m; KS_CODE(ks) = c; }
#define KS_COPY(kd,ks)     KS_FILL(kd, KS_QUAL(ks), KS_MASK(ks), KS_CODE(ks))

#define KT_SLOT(kt,ks) ((kt)->hash[KS_CODE(ks)&HASHMASK])

#define KS_MATCH_EXACT(k0,k1)  ((KS_CODE(k0) == KS_CODE(k1)) ? ((KS_MASK(k0) == KS_MASK(k1)) ? (KS_QUAL(k0) == KS_QUAL(k1)) : FALSE) : FALSE)
#define KS_MATCH_APPROX(k0,k1) ((KS_CODE(k0) == KS_CODE(k1)) ? (KS_QUAL(k0) == (KS_MASK(k1) & KS_QUAL(k1))) : FALSE)


/* not used yet */
#define KS_IS_RAWCODE(ks) (is_rawc[KS_CODE(ks)] != 0 && (KS_QUAL(ks)&QUAL_UP) == 0)



/**************************************
	       Prototypes
**************************************/

extern ULONG ext_qualifiers;
extern int   qualifier (char *);

extern void do_map	 (void);
extern void do_unmap	 (void);
extern void do_keyload	 (void);
extern void do_keysave	 (void);
extern void do_qualifier (void);
extern void do_new_keytable (void);
extern void do_del_keytable (void);
extern void do_use_keytable (void);

extern void   keyload	     (KEYTABLE * kt, char * name);
extern void   keysave	     (KEYTABLE * kt, char * name);
extern int    loadkeys	     (KEYTABLE * kt, FILE * fi, int * lineno);
extern int    savekeys	     (KEYTABLE * kt, FILE * fo);
extern HASH * findhash	     (KEYTABLE * kt, KEYSPEC * ks, UBYTE *);
extern APTR   keyspec2macro  (KEYTABLE * kt, const UBYTE * str);
extern int    resethash      (KEYTABLE * kt);
extern int    unmapkey	     (KEYTABLE * kt, const UBYTE * key);
extern int    mapkey	     (KEYTABLE * kt, const UBYTE * key, const UBYTE * value, const UBYTE *help);
extern int    remhash	     (KEYTABLE * kt, KEYSPEC * ks);
extern int    addhash	     (KEYTABLE * kt, KEYSPEC * ks, const UBYTE * comm, const UBYTE * help);
extern void   dealloc_hash   (KEYTABLE * kt);
extern KEYTABLE * get_keytable	  (char * name);
extern KEYTABLE * new_keytable	  (char * name, int defaults);
extern void	  delete_keytable (KEYTABLE * kt, int force);
extern void	  exit_keytables (void);

extern BOOL   get_codequal   (const UBYTE * str, KEYSPEC *ks);
extern UBYTE *cqtoa	     (KEYSPEC *ks);
extern TQUAL  iqual2qual     (unsigned long qual, int blen, char * buf, int code);
extern ULONG  qual2iqual     (TQUAL qual);
extern BOOL   a2iqual	     (const UBYTE * str, ULONG *piqual);
extern UBYTE *iqual2a	     (ULONG iqual);
extern UBYTE  CtoA	     (TCODE c);
extern TQUAL  CIgnoreQ	     (TCODE c);
extern BOOL   IsRawC	     (KEYSPEC * ks);
extern void   keyboard_init  (void);
extern void   init_kb	     (void);
extern void   exit_kb	     (void);
extern struct Library * ConsoleDevice;
extern int    DeadKeyConvert (struct IntuiMessage * msg, UBYTE * buf, int bufsize, struct KeyMap * keymap);

#endif

extern void keyctl (struct IntuiMessage *, int, ULONG);  /* PATCH_NULL [14 Feb 1993] : changed to void */
extern char*keyspectomacro(char*);

#endif /* KEYHASHES_H */

/******************************************************************************
*****  ENDE keyhashes.h
******************************************************************************/

