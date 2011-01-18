/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	That module represents ab interface to all
	variables, which have a special meaning for
	XDME's behaviour; strictl speaking the current
	file is the interface part, while the implementation
	of the access functions is done via a automatically
	generated file (called _SPC_.c), which is created
	from a file in a meta language (SPC.pre)

    NOTES

******************************************************************************/

/**************************************
		Includes
**************************************/

#include "defs.h"
//#include "null.h"


/**************************************
	    Globale Variable
**************************************/

Prototype char	SPC_set (const char *name, const char *value);
Prototype char *SPC_get (const char *name);


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/

struct genflags genflags;

/**************************************
	   Interne Prototypes
**************************************/

// extern int VSTK_Internal;
extern UBYTE Fstr[];
extern UBYTE Rstr[];
UBYTE *ltostr (long l);



#include "_SPC_.c"


Prototype UBYTE *ltostr (long l);
UBYTE *ltostr (long l) {
    sprintf (tmp_buffer, "%ld", l);
    return tmp_buffer;
}

Prototype char *strrep (char *old, const char *new);
char * strrep (char *old, const char *new) {
    char *dup;
    if (dup = strdup(new)) {
	free (old);
	return dup;
    } else {
	nomemory();
	return old;
    } /* if */
} /* strrep */


#if 0
unsigned long Bin_access(const void *array, long E_size, long A_size, const char *find) {
    unsigned long upper, lower;
    unsigned long mid;
    const char **cptr;

    upper = A_size -1;
    lower = 0;

    /* ---- test lowest */
    cptr = (const char **)array;
    if (!strcmp (*cptr, find))
	return 0;
    /* ---- test highest */
    cptr = (const char **)(upper * E_size + (unsigned long)array);
    if (!strcmp (*cptr, find))
	return upper;

    /* ---- normal test loop */
    while (upper - lower > 1) {
	int val;
	mid  = (upper + lower) / 2;
	cptr = (void *)(mid * E_size + (unsigned long) array);
	val = strcmp (*cptr, find);
	if (val > 0) {
	    lower = mid;
	    continue;
	} else if (val) {
	    upper = mid;
	    continue;
	} /* if */
	return mid;
    } /* while */

    return (unsigned long)~0;
} /* Bin_access */
#else
unsigned long Bin_access(const char **array, long E_size, long A_size, const char *find) {
    long low, high, mid;
    int diff;

    high = A_size -1;
    low  = 0;

    do {
	mid  = (low + high)/2;
	diff = stricmp (find, array[mid]);
	if (diff > 0)
	    high = mid-1;
	else if (diff)
	    low  = mid+1;
	else
	    return (unsigned long)mid;
    } while (low <= high);

    return (unsigned long)~0;
} /* Bin_access */
#endif


long spc_lock(const char *name) {
    if (!name)
	return 0;
    return ~(long)Bin_access (spc_names, sizeof (char *), spc_num_vars, name);
}

#ifndef TEST

char SPC_set (const char *name, const char *value) {
    long lock;
    if ((lock = spc_lock(name))) {
	if (!spc_set(lock, value))
	    SETF_ABORTCOMMAND(Ep, 1 );
	return 1;
    } /* if */
    return 0;
} /* spc_set */


char *SPC_get (const char *name) {
    long lock;
    if ((lock = spc_lock(name))) {
	char * str;
	if (!(str = spc_get(lock))) {
	    SETF_ABORTCOMMAND(Ep, 1 );
	    return NULL;
	} /* if */
	if (str = strdup(str))
	    return str;
	nomemory();
    } /* if */
    return NULL;
} /* spc_get */




/*
**  Redefine these names to adjust the used buffers to Your settings
**  the commands are 'atomic' so there should be no interference to
**  other 'atomic' commands using them.
**
**  for XDME changed buffers[0]/buffers[1] to tmp_buffer/tmp_buf2
*/
#define firstbuffer  tmp_buffer




/*
* !  $[SPC.]currentword
* !	 that read-only special-variable
* !	 represents "the word under the cursor"
* !	 that means it finds the beginning of the current word on
* !	 on its own (but we do not move the cursor) if cursor is
* !	 not within a word, it returns the next word of the line
* !	 if there is no next word, an empty string is returned
* !  NOTE this variable is not accessible, if PATCH_VARS is not active
* !
**  current_word		   (uses firstbuffer)
**
**  Returns the word under the cursor,
**  is used by fastscan and should be used by ref and tag instead of
**  their own functions
*/

#define isC(ch)     (isalnum((ch)) || (ch) == '_')

Prototype char * current_word (void);
char * current_word (void)
{
    register char * str;
    register int i,j;

    str = (char *)firstbuffer;

    i = Ep->column;
    while ((i>0) && isC(Current[i]))
    {	       /*  goto beginning of word */
	i--;
    } /* while */
    while ((Current[i]) && !isC(Current[i]))
    {  /* skip spaces and other garbage */
	i++;
    } /* while */

    j = 0;
    while (isC(Current[i]) && (j <= LINE_LENGTH))
    {
	str[j] = Current[i];				/* copy "word" into buffer */
	j++;
	i++;
    } /* while */
    str[j] = 0;

    return (str);
} /* current_word */


#if 1
/*
 *	This function is to be called for all setting any special var
 *	via a command:	   SPC varname value
 *  The only thing is - the variable must be treated here
 */

Prototype void do_setspecialvar(void);
void do_setspecialvar(void)
{
    if (!SPC_set(av[1],av[2]))
	abort2();
} /* do_setspecialvar */

#endif
#endif






/***************************************************
	 test driver for Lock function
***************************************************/

#ifdef TEST

/*
struct __XDMEArgs XDMEArgs;
UWORD Mx, My;
char MShowTitle, noadj;
struct MsgPort *Sharedport;
*/

UBYTE tmp_buffer[256];

int main(int ac, char ** av) {
    int i;
    for (i = 0; i < spc_num_vars; ++i) {
	printf ("accessing %d/%s -> %d\n", i, spc_names[i], spc_lock(spc_names[i]));
    } /* for */

    printf ("accessing %d/%s -> %d\n", i, "hallo", spc_lock("hallo"));
    printf ("accessing %d/%s -> %d\n", i, "_oops", spc_lock("_oops"));
    printf ("accessing %d/%s -> %d\n", i, "Aetsch", spc_lock("Aetsch"));
    printf ("accessing %d/%s -> %d\n", i, "}{", spc_lock("}{"));
    printf ("accessing %d/%s -> %d\n", i, "00", spc_lock("00"));
    printf ("accessing %d/%s -> %d\n", i, "", spc_lock(""));

    return 0;
}

#endif

/******************************************************************************
*****  ENDE spc.c
******************************************************************************/

