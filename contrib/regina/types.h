/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * $Id$
 */


typedef struct varbox *variableptr ;
typedef const struct varbox *cvariableptr ;
typedef struct varbox {
   variableptr next, prev, realbox, *index ;
   struct strengtype *name, *value ;
   int guard ;
   num_descr *num ;
   int flag ;
   long hwired, valid ; /* FGC: at least valid may be too small for many
                                recursions with short instead of long.
                                27.09.98 (09/27/98). */
   variableptr stem ;
} variable ;

/* typedef int bool ; */

typedef struct {
   unsigned int lnum:1 ;
   unsigned int rnum:1 ;
   unsigned int lsvar:1 ;
   unsigned int rsvar:1 ;
   unsigned int lcvar:1 ;
   unsigned int rcvar:1 ;
} compflags ;

typedef struct pparambox *paramboxptr ;
typedef const struct pparambox *cparamboxptr ;
typedef struct tnode *nodeptr ;
typedef const struct tnode *cnodeptr ;

typedef struct  {
   long sec ;
   long usec ;
} rexx_time;

struct _tsd_t; /* If this won't work change "struct _tsd_t *" to "void *"
                * below. This will require more changes. Let your compiler
                * choose the places.
                */
typedef struct tnode {
   unsigned int type ;
   int charnr, lineno ;
   int called;
   struct strengtype *name ;
   rexx_time *now ;
   struct tnode *p[4] ;
   union {
      streng *(*func)(struct _tsd_t *,cparamboxptr) ;
      streng *strng ;
      struct tnode *node ;
      num_descr *number ;
      compflags flags ;
      variable *varbx ;
   } u ;
   struct tnode *next ;
   unsigned long nodeindex ; /* for an effectiv relocation, never change! */
} treenode ;

typedef struct pparambox {
   paramboxptr next ;
   int dealloc ;
   struct strengtype *value ;
} parambox ;

typedef struct lineboxx *lineboxptr ;
typedef const struct lineboxx *clineboxptr ;
typedef struct lineboxx {
   lineboxptr next, prev ;
   struct strengtype *line ;
   int lineno ;
} linebox ;

typedef struct labelboxx *labelboxptr ;
typedef const struct labelboxx *clabelboxptr ;
typedef struct labelboxx {
   labelboxptr next ;
   unsigned long hash ;
   nodeptr entry ;
} labelbox ;

typedef struct trap_type  /* index is type of signal */
{
   unsigned int on_off:1 ;     /* true if trap is active/on */
/* unsigned int trapped:1 ; */ /* true if condition has been raised */
   unsigned int def_act:1 ;    /* true if default action is to ignore */
   unsigned int delayed:1 ;    /* true if trap is in delay mode */
   unsigned int ignored:1 ;    /* true if ignored when in delayed mode */
   unsigned int invoked:1 ;    /* true if invoked by SIGNAL */
   streng *name ;          /* label to transfer control to */
} trap ;

typedef struct sig_type
{
   int type ;
   streng *info ;
   streng *descr ;
   int invoke ;     /* is true if invoked with SIGNAL */
   int rc, subrc, lineno ;
} sigtype ;

typedef void (*signal_handler)(int);

typedef struct option
{
   char *name ;
   int offset ;
   char *contains ;
} option_type ;



typedef struct proclevelbox *proclevel ;
typedef const struct proclevelbox *cproclevel ;
typedef struct proclevelbox {
   int numfuzz, currnumsize, numform ;
   int mathtype ;
   rexx_time time ;
   proclevel prev, next ;
   variableptr *vars ;
   paramboxptr args ;
   struct strengtype *environment, *prev_env ;
   char tracestat, varflag ;
   sigtype *sig ;
   trap *traps ;
   jmp_buf *buf ;  /* for use by longjmp */
   union {
#ifdef OLD_OPTIONS
      struct {
         unsigned int flushstack : 1 ;
         unsigned int lineouttrunc : 1 ;
         unsigned int close_bif : 1 ;
         unsigned int open_bif : 1 ;
         unsigned int buftype_bif : 1 ;
         unsigned int desbuf_bif : 1 ;
         unsigned int dropbuf_bif : 1 ;
         unsigned int makebuf_bif : 1 ;
         unsigned int cacheext : 1 ;
         unsigned int find_bif : 1 ;
         unsigned int prune_trace : 1 ;
         unsigned int ext_commands_as_funcs : 1 ;
         unsigned int stdout_for_stderr : 1 ;
         unsigned int trace_html : 1 ;
         unsigned int default_lines_bif_is_actual : 1 ;
         unsigned int ansi : 1 ;
         unsigned int unusedbit0 : 1 ;
         unsigned int unusedbit1 : 1 ;
         unsigned int unusedbit2 : 1 ;
         unsigned int unusedbit3 : 1 ;
         unsigned int unusedbit4 : 1 ;
         unsigned int unusedbit5 : 1 ;
         unsigned int unusedbit6 : 1 ;
         unsigned int unusedbit7 : 1 ;
         unsigned char unusedchar3 ;
      } options ;
#endif
      unsigned char flags[4] ;
   } u ;
} proclevbox ;

typedef struct _ttree { /* bucket list of treenodes which allows ultra fast
                         * loading of instore macros.
                         */
   struct _ttree *next;
   unsigned long  max; /* maximum number of elements in the bucket */
   unsigned long  num; /* current number of elements in the bucket */
   unsigned long  sum; /* sum of indices until element 0 */
   treenode *     elems;
} ttree; /* treenode type */

typedef struct { /* offsrcline: offset based source lines */
   unsigned long length;
   unsigned long offset;
   /* That's all. You need the source string of the incore macro to create
    * a real sourceline using this information.
    */
} offsrcline;

typedef struct _otree { /* bucket list of offscrlines which allows ultra fast
                         * loading of instore macros.
                         */
   struct _otree *next;
   unsigned long  max; /* maximum number of elements in the bucket */
   unsigned long  num; /* current number of elements in the bucket */
   unsigned long  sum; /* sum of indices until element 0 */
   offsrcline *   elems;
} otree; /* offsrcline type */

typedef struct { /* internal_parser_type is a structure containing data from a
                  * parsing operation.
                  */
   lineboxptr     first_source_line; /* Either this two values  */
   lineboxptr     last_source_line ; /* exist or srclines below */
   int            tline;
   int            tstart;
   labelboxptr    first_label;
   labelboxptr    last_label;
   unsigned long  numlabels;
   labelboxptr    sort_labels ;
   int            result;
   nodeptr        root;
   ttree *        nodes;
   otree *        srclines;          /* Either this two values exist */
   const char *   incore_source;     /* or the two values above      */
   streng *       kill; /* Probably the true source of incore_source in case
                         * of an "INTERPRET" instruction or other not user
                         * generated but interpreted strings; else NULL.
                         */
   struct _tsd_t *TSD; /* needed during the parsing step */
} internal_parser_type;

typedef struct { /* extstring: external (instore) string */
   unsigned long length;
   /* and directly following the string's content */
} extstring;

typedef struct { /* external_parser_type: the instore macro in user space */
   /* Never change from here until source including since it allows the
    * reconstruction of the source if the machine type or Regina version
    * doesn't match.
    */
   char     Magic[32]; /* "Regina's Internal Format\r\n" filled with 0 */
#define MAGIC "Regina's Internal Format\r\n"
   char     ReginaVersion[64]; /* PARSE_VERSION_STRING */

   /* The following structure allows the detection of different
    * architectures. We don't want to try to decode something from
    * a 64 bit big endian encoded parsing tree on an i586 for
    * example.
    */
   union {
      char ignore[4 * 256/8]; /* Allow 256 bit machines */
      struct {
         unsigned long one; /* value one */
         unsigned long two; /* value two */
         void *   ptr3; /* value (void*)3, size may be different to unsigned */
         void *   ptr4; /* value (void*)4 */
      } s;
   } arch_detector;

   unsigned long OverallSize; /* in byte of this structure and all dependencies */

   unsigned long version; /* INSTORE_VERSION */

   /* We describe the sourcelines first */
   unsigned long NumberOfSourceLines;
   unsigned long source;
   /* Offset to table of source lines. Imagine a value of 1000 and
    * 5 source lines (previous value). This structure has an address
    * of 500 in the memory (what you get from malloc or something else).
    * Then: The table has 5 entries at position 1500 in memory.
    * Each table entry is an offsrcline structure. The source string is the
    * instore[0] string of RexxStart or another source string.
    */

   unsigned long NumberOfTreeElements;
   unsigned long TreeStart; /* Within 0 .. (NumberOfTreeElements-1) */
   unsigned long tree;
   /* Offset to table of nodes. Imagine a value of 2000 and
    * 6 elements (NumberIfTreeElements). This structure has an address
    * of 500 in the memory (what you get from malloc or something else).
    * Then: The table has 6 treenodes at position 2500 in memory.
    * Each table entry (treenode) must be relocated. Every nodeptr within a
    * element is just an index within this table. If node->p[2] of one picked
    * table entry is ((nodeptr) 4) then the address is calculated as follows:
    * 500 + tree + (4*sizeof(treenode))
    * This value should be assigned to node->p[2] to use the value in its
    * normal manner.
    * A NULL value is represented by (nodeptr) (unsigned) -1.
    * Every string within a treenode is relocated by adding 500. Imagine a
    * value of 3000 for node->name. Then an extstring structure is located
    * at 3500 in memory which represents the string's content.
    */
} external_parser_type;

typedef struct systeminfobox *sysinfo ;
typedef const struct systeminfobox *csysinfo ;
typedef struct systeminfobox {
   struct strengtype *called_as;
   struct strengtype *input_file ; /* must be 0-terminated without counting of the '\0' */
   streng *environment ;
   FILE *input_fp;
   int tracing ;
   int interactive ;
   jmp_buf *panic ;
   streng *result ;
   proclevbox *currlevel0 ;
   struct systeminfobox *previous ;
   nodeptr *callstack ;
   int cstackcnt, cstackmax ;
   int hooks ;
   int invoked ;
   int trace_override;
   internal_parser_type tree;
} sysinfobox ;

struct entrypt {
   char *name ;
   streng *(*addr)() ;
} ;


struct library {
   streng *name ;
   void *handle ;
   short number ;
   struct library_func *first ;
   struct entrypt *funcs ;
   struct library *next, *prev ;
} ;

#ifndef DONT_TYPEDEF_PFN
typedef unsigned long (*PFN)() ;
#endif

struct library_func {
   streng *name ;
   PFN addr  ;
   unsigned long hash ;
   struct library *lib ;
   struct library_func *next, *prev ;
   struct library_func *forw, *backw ;
} ;
