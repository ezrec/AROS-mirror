
/******************************************************************************

    MODULE
	tplter.c

    DESCRIPTION
	ein kleines kommando, welches
	ein Templatefile einliest,
	das bestimmte Platzhalter enthaelt
	und dann das verwendbare Ergebnis
	in den aktuellen Text einbindet.

    NOTES
	* for XDME some features of flexprintf were commented out
	* that module is highly experimental

    BUGS
       ?

    TODO
	* linenumber in read_file
	* check for success in flexprintf
	* flexSCANf needs to be created - but that thing needs a
	  little bit more work, since we can't parse line by line
	* another version of that module might support loops, conditionals
	  and subtemplates - lets see

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	17-08-93 b_noll created
	18-08-93 b_noll xdme-interface
	15-06-94 b_noll added another flexprintf-version
	15-08-94 b_noll added to the 1.83.** tree (no DynCom features)

    PRIVATE
	we need access to the following functions:
	    do_command();
	    do_insertwrite();
	    getvar();
	    COM_Add(), COM_Lock(), COM_Remove()

	    fopen, fclose, xefgets may be replaced by Dos-Based Open, Close, [own-creation]
	    malloc && free may be replaced by Exec-Based AllocVec, FreeVec

	structures:
	    globalflags;
	    Ep;
	    av

******************************************************************************/

/**************************************
	    Includes
**************************************/
#include "defs.h"
/* #include "COM.h" */
/* #include "libs/AUTO.h" */

/**************************************
	    Global Variables
**************************************/

Prototype int do_readtplt (void);

/**************************************
	    Internal Defines & Structures
**************************************/

/* we do not need that structure in XDME */
#ifndef   STREAM
#define   STREAM APTR
#endif /* STREAM */

/**************************************
	    Internal Variables
**************************************/

static const char * mod_name = "Templater";

/**************************************
	    Internal Prototypes
**************************************/

BOOL	     flexprintf (STREAM*, BOOL (*)(STREAM*, STRPTR), STRPTR, STREAM*, STRPTR (*)(STREAM*, STRPTR));
char*	     mygetv	(APTR  , char* );
BOOL	     myfputs	(APTR , char* );

/**************************************
	    Macros
**************************************/

#define ERROR_NO_NAME_OF_QUERY	 /* error ("%s:\n%s!\n", mod_name, "No name given"); */
#define ERROR_NO_VALUE_OF_QUERY  /* error ("%s:\n%s %s!\n", mod_name, "No value for variable", memo); */
#define ERROR_UNTERMINATED_QUERY /* error ("%s:\n%s!\n", mod_name, "Unterminated variable query"); */
#define ERROR_NO_MEMORY
#define ERROR_NO_FILE		 error ("%s:\n%s %s!\n", mod_name, "File not found\n", name);

#define INC(x) ++(x)
#define DEC(x) --(x)


/* MACROS 18-08-93 by b_noll */
#define is_char(x)     (isalnum(x) || ((x) ==  '_') || ((x) == '.'))
#define is_command(x)  (is_varstart(*(x)) && (*(x) == (x)[1]))
#define is_comment(x)  (*(x) == '#')


/* ---- definitions for "is_varstart" */
#ifdef SLOW_VARSTART
# define is_varstart(x) strchr(query_starts, x)
#else
# define is_varstart(x) (((x) == '$') || ((x) == '%'))
		    /*	((x) == '%') */                  /* <<< use that line to enable only   '%'-variables */
		    /*	((x) == '$') */                  /* <<< use that line to enable only   '$'-variables */
		    /* (((x) == '$') || ((x) == '%')) */ /* <<< use that line to enable '$' or '%'-variables */
#endif


/* ---- definitions for query-recognition */
#define QUERY_START    "%$" /* <<< this line only has affect if SLOW_VARSTART is defined */

/* ---- definitions for bracket-recognition */
#ifdef AMIGA
# define BRACKET_OPEN  "(`[{<\253" /* the last one is '<<' */
# define BRACKET_CLOSE ")']}>\273" /* the last one is '>>' */
#else
# define BRACKET_OPEN  "(`[{<"
# define BRACKET_CLOSE ")']}>"
#endif /* AMIGA */



#ifdef XDME

/* ---- XDME uses a subset of the possibles methods; */
/*	we currently do only allow $(...) and $`...' */
/*	to be recognized and replaced */
# undef SLOW_VARSTART
# undef is_varstart
# undef QUERY_START
# undef BRACKET_OPEN
# undef BRACKET_CLOSE


# define is_varstart(x) ((x) == '$')
# define QUERY_START	"$"
# define BRACKET_OPEN	"(`"
# define BRACKET_CLOSE	")'"

#endif /* XDME */

/**************************************
	    Implementation
**************************************/





/* ---- there are currently 2 Versions of flexprintf here;
**	the first - older - version was used over 'bout 1/2 a year
**	and did not show any error; the second one is not yet really
**	tested w/ "XDME", it was used with "Nyktos Ophthalmoi" for
**	'bout 2 months.
**	please do't ask me, why I created 2nd version, there was
**	a reason, but I can't remember ... */

#ifdef OLD


/*****************************************************************************

    NAME
	flexprintf

    PARAMETER
	STREAM*outstream;
	;BOOL (*writechar)  (STREAM*, char);
	BOOL (*writestring)(STREAM*, STRPTR);
	STRPTR format;
	STREAM*instream;
	STRPTR (*getstring)(STREAM*, STRPTR);

    RESULT
	success

    RETURN
	BOOL;

    DESCRIPTION
	similar to C_sprintf but slightly different.

	wir parsen ein template, das
	$(...) oder %(...) oder ${...} oder %{...} oder
	$«...» oder %«...» oder $<...> oder %<...> oder
	$[...] oder %[...] oder $`...' oder %`...'
	als signalworte hat;
	was zwischen den Klammern steht, wird an
	getstring uebergeben; dessen Ergebnis
	(wir erwarten IMMER strings) wird dann
	eingesetzt.

	man kann die Funktion auf 2 sehr unterschiediche
	Weisen verwenden:
	    wie sprintf - in diesem Falle ignoriert die
	getfunktion das 2. Argument sondern gibt jedesmal den
	"naechsten" wert zurueck
	    wie hier gedacht - in diesem falle liest die getfunktion
	aus einem baum o.ae. den Wert, der der namen von arg2 traegt.

    NOTES
	STREAM == APTR
	format must not be const!

	not like in normal XDME-scripts there MUST be a form
	of quotes around any variable!
	for XDME "%" and "<...>" "«...»" "[...]" "{...}" were
	disabled.

    BUGS
	we do ignore the result of our writestring
	and we do always return TRUE
	(to fix - change "writestring..."
		  into	 "if (!writestring...) return (FALSE);")

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	21-07-93    b_noll  created
	18-08-93    b_noll modified for XDME

******************************************************************************/

BOOL flexprintf (STREAM* outstream, BOOL (*writestring)(STREAM*, STRPTR), STRPTR format, STREAM* instream, STRPTR (*getstring)(STREAM*, STRPTR))
{
    char	* cptr = format;
    unsigned char curr;
    unsigned char attn = 0;

    for (; (curr = *format) != 0; ++format) {
	if (is_varstart(curr)) {
	    if (attn) *cptr = attn;
	    *format = 0;
	    writestring (outstream, cptr);
	    cptr = format;
	    attn = curr;
	    continue;
	} /* if */

	if (attn) {
	    char scan = 0;

	    switch (curr) {
	 /* some inconsistancies to XDME were commented out */
	 /* case '[': scan = ']'; break; */
	 /* case '«': scan = '»'; break; */
	 /* case '<': scan = '>'; break; */
	 /* case '{': scan = '}'; break; */
	    case '(': scan = ')'; break;
	    case '`': scan = '\''; break;
	    } /* switch */

	    if (scan) {
		char * memo;
		char * rslt;

		++format;
		memo = format;
		while ((curr = *format) && !(curr == scan)) {
		    ++format;
		} /* while */

		*format = 0;
		if (*memo) {
		    rslt = getstring (instream, memo);
		    if (rslt) {
			writestring (outstream, rslt);

			*format = scan;
			attn	= 0;
			cptr	= format+1;
			continue;

		    } else {
			ERROR_NO_VALUE
		    } /* if */
		} else {
		    ERROR_NO_NAME
		} /* if */
		*format = scan;
	    } /* if */

	    *cptr = attn;
	    attn  = 0;
	} /* if */
    } /* for */
    writestring (outstream, cptr);
    return (TRUE);

} /* flexprintf */

#else /* !OLD */


/*****************************************************************************

    NAME
	flexprintf

    PARAMETER
	STREAM*outstream;
	BOOL (*writestring)(STREAM*, STRPTR);
		; we expect writestring to return <>0 on Success and
		; 0 in case of any Failure.
	STRPTR tmplt;
		; Format is destroyed during the scan, we cannot
		; use const-strings in System with mem-protection
	STREAM*instream;
	STRPTR (*getstring)(STREAM*, STRPTR);

    RESULT
	success

    RETURN
	BOOL;

    DESCRIPTION
	similar to C/sprintf but slightly different in usage and syntax:
	    while sending tmplt through "writestring(outstream,...)",
	we do replace occurancies of [$%][[(`{|<]char-seq[])'}>] with
	the string, that is returned by "getstring(instream, char-seq)";
	the meaning of [$%] can be escaped by duplicating the character.


    NOTES
	* tmplt MUST NOT be const! (see BUGS)
	  for _flexprintf in any case and for flexprintf
	  as long as FLEXPRINT_BUFFER is not defined to 1

	* not like in normal Shell- or XDME-scripts there MUST be a form
	  of quotes around any variable!

	* formatting of replaced variables must be supported by the
	  get-function, if such features are needed.

	* between the brackets of a variable-query there must be at least
	  one char; empty variablenames are not allowed (also if Your getstring
	  knows, what is wanted), and lead to a break.

	* getstring returning NULL means an error has occured; we will break
	  for this reason. Empty strings should be given via "".
	  Furthermore we do not free the strings gotten from getstring(),
	  since we don't know, if they are static or not, so if these strings
	  are dynamically created, You should store them in a static variable.
	  my preferred way looks like the following:
	    char *getvar(void *x, char *n) {
		static char *ptr = NULL;
		if (ptr) free(ptr);
		ptr = ...;
		return ptr;
	    }


	* not terminating a query will cause a break, if there is no other
	  query behind, else the result is not defined.

	* we use different macros to inticate the common errors, which are
	  recognized: ERROR_NO_NAME_OF_QUERY, ERROR_NO_VALUE_OF_QUERY and
	  ERROR_UNTERMINATED_QUERY, ERROR_NO_MEMORY; they are defined to do nothing, so You
	  might want to redefine them to Your needs.

	Please note, that "%$" and "(`[{<«"/")']}>»" may be changed via some
	defines described below:

	* the macro "is_varstart" is used to recognize %/$ - You might want to
	  redefine it according to Your needs (see above for different examples)
	  if defined "SLOW_VARSTART" You can use the define "QUERY_START" to
	  define a list of characters, which may be used as start of a query-
	  indicator.

	* the defines "BRACKET_OPEN" and "BRACKET_CLOSE" can be used to define
	  the different open-brackets and their corresponding close-brackets
	  (see above for examples)

	* to gain more flexibility You may directly call _flexprintf()
	  which has the additional parameters query_starts ("$%"),
	  open_brackets ("([{...") and close_brackets (")]}...")

    BUGS
	* tmplt is temporarily destroyed during the scan. this is done,
	  since we have no information about a necessary buffersize to
	  keep a string in, but we wanted to use string-operations instead
	  of character operations for writestring.
	  (the described behaiviour can be changed (at least for flexprintf,
	  not for _flexprintf) by defining FLEXPRINT_BUFFER to 1. (that toggle
	  makes flexprintf create a copy of the template and send the copy
	  to _flexprintf))

    EXAMPLES

    SEE ALSO

    INTERNALS
	* Funktionsweise:
	  wir parsen ein template, das normalerweise
	  $(...) oder %(...) oder ${...} oder %{...} oder
	  $«...» oder %«...» oder $<...> oder %<...> oder
	  $[...] oder %[...] oder $`...' oder %`...'
	  als signalworte hat; sobald wir auf ein %/$ stossen, wird der
	  bisher geparste und nicht ausgegebene Teil des templates nach
	  writestring geschickt. Falls nach %/$ dann eine der Klammern kommt,
	  wird, was zwischen den Klammern steht, an getstring uebergeben,
	  dessen Ergebnis (wir erwarten IMMER strings) wird dann an
	  writestring geschickt. Um die Bedeutung von %/$ zu loeschen
	  muss das entsprechende Symbol dupliziert werden, nur eines der
	  beiden wird dann ausgegeben. Falls keine bracket folgt, ist eine
	  Verdopplung eigentlich unnoetig, sollte aber dennoch erfolgen.

	* Anwendung:
	  man kann die Funktion auf 2 sehr unterschiediche
	  Weisen verwenden:
	    wie sprintf - in diesem Falle ignoriert die
	  get-funktion das 2. Argument und gibt jedesmal den
	  "naechsten" wert zurueck
	    wie hier gedacht - in diesem falle liest die getfunktion
	  aus einem baum o.ae. den Wert, der der namen von arg2 traegt.


    HISTORY
	21-07-93    b_noll created
	17-08-93    b_noll rewritten without "writechar()"
	18-08-93    b_noll modified for XDME
	01-05-94    b_null major rewrites ...

******************************************************************************/


BOOL _flexprintf (STREAM* outstream, BOOL (*writestring)(STREAM*, STRPTR), STRPTR tmplt, STREAM* instream, STRPTR (*getstring)(STREAM*, STRPTR), const STRPTR query_starts, const STRPTR open_brackets, const STRPTR close_brackets) {
    unsigned char *memo = tmplt;
    unsigned char  curr;
    unsigned char  attn = 0;

    for (; (curr = *tmplt) != 0; ++tmplt) {

	/* ---- seems, that we are starting a query, output all chars up to here, mark the pos w/ memo */
#ifdef SLOW_VARSTART
	if (strchr(query_starts, curr))
#else
	if (is_varstart(curr))
#endif
	{

	    /* ---- previous char was the same? - then allow output of one of'em */
	    /*	    that hack was done to allow something like escaping; now we  */
	    /*	    forbid a query by preceding it with two the same varstarts	 */
	    if (curr == attn) {
		memo = tmplt;
		attn = 0;
		continue;
	    } /* if */

	    /* ---- output the tmplt up to this point and     */
	    /*	    mark the position with an additional ptr  */
	    /*	    keep also a copy of varstart to recognize */
	    /*	    duplicates in order to ignore them	      */

	    /* ---- we do temporarily modify tmplt here!!!! */
	    *tmplt = 0;
	    if (!writestring (outstream, memo)) {
		*tmplt = curr;
		return FALSE;
	    } /* if */
	    *tmplt = curr;

	    memo   = tmplt;
	    attn   = curr;
	    continue;
	} /* if varstart */

	if (attn) {
	    unsigned char scan = 0;

	    attn    = 0;

	    /* ---- check, if the current character is an open-bracket	*/
	    /*	    and get the close-bracket, that is correponding to	*/
	    /*	    this open-bracket					*/
	    {
		int i;
		for (i = 0; open_brackets[i]; ++i) {
		    if (curr == open_brackets[i]) {
			scan = close_brackets[i];
			break;
		    } /* if */
		} /* for */
	    }

	    /* ---- we found a bracket after var_start */
	    if (scan) {
		char * rslt;

		/* ---- skip the open-bracket */
		++tmplt;

		/* ---- seach the close-bracket */
		while ((curr = *tmplt) && !(curr == scan))
		    ++tmplt;

		/* ---- make sure we have found a terminating close-bracket */
		if (!curr) {
		    ERROR_UNTERMINATED_QUERY
		    return FALSE;
		} /* if */

		/* ---- make sure we have got at least one char */
		/*	between the backets			*/
		if (tmplt == memo + 2) {
		    ERROR_NO_NAME_OF_QUERY
		    return FALSE;
		} /* if */

		/* ---- query for the variable described between */
		/*	the brackets				 */

		/* ---- here we do temporarily modify tmplt!!! */
		*tmplt = 0;
		rslt = getstring (instream, memo + 2);
		*tmplt = scan;

		/* ---- make sure we got a valid string from getstring() */
		if (!rslt) {
		    ERROR_NO_VALUE_OF_QUERY
		    return FALSE;
		} /* if !rslt */

		/* ---- output the resultstring */
		if (!writestring (outstream, rslt))
		    return FALSE;

		memo = tmplt+1;
	    } /* if bracket */
	} /* if attn */
    } /* for */

    if (!writestring (outstream, memo))
	return FALSE;

    return TRUE;
} /* _flexprintf */



BOOL flexprintf (STREAM* outstream, BOOL (*writestring)(STREAM*, STRPTR), STRPTR tmplt, STREAM* instream, STRPTR (*getstring)(STREAM*, STRPTR)) {
#if defined(FLEXPRIN_BUFFER) && FLEXPRINT_BUFFER
    char *dup;
    BOOL  rslt;

    if (NULL == (dup = strdup(tmplt))) {
	ERROR_NO_MEMORY
	return FALSE;
    } /* if */
    rslt = _flexprintf(outstream, writestring, dup, instream, getstring, QUERY_START, BRACKET_OPEN, BRACKET_CLOSE);
    free (dup);
    return rslt;
#else
    return _flexprintf(outstream, writestring, tmplt, instream, getstring, QUERY_START, BRACKET_OPEN, BRACKET_CLOSE);
#endif
} /* flexprintf */


#endif /* OLD */









/*****************************************************************************

    NAME
	read_file

    PARAMETER
	char* tpltname;

    RESULT
	success;

    RETURN
	BOOL;

    DESCRIPTION
	lese ein file ein,
	bearbeite es nach den Vorgaben
	und fuege es in den aktuellen kontext ein

    NOTES
	beim einlesen des Files werden alle zeilen, die mit
	$$ (oder %%) beginnen separat interpretiert, und nicht
	wieder ausgegeben.

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	21-07-93 b_noll created  for TEMPLATER
	18-08-93 b_noll modified for XDME

******************************************************************************/

BOOL read_file (char* name)
{
    char * buffer;
    char * b2;
    FILE * fi;
    long   len;
    BOOL   success = FALSE;

    if (name && *name) {
	if ((buffer = malloc(MAXLINELEN))) {
	    if ((fi = fopen (name, "r"))) {
		success = TRUE;
		while (success && (len = xefgets (fi, buffer, MAXLINELEN)) >= 0) {
		    b2 = buffer;
/* printf (">%s",buffer); */
		    if (is_command(b2)) {
			if (is_comment(buffer+2)) continue;
			success = do_command (buffer+2);
		    } else {
			b2 = buffer + len;
			*b2 = '\n';
			++b2;
			*b2 = 0;

			success = flexprintf (NULL, (APTR)myfputs, buffer, NULL, (APTR)mygetv);

		    } /* if */
		} /* while */
		fclose (fi);
	    } else {
		ERROR_NO_FILE
	    } /* if */

	    free (buffer);
	} else {
	    nomemory();
	} /* if */
    } /* if */
    return (success);
} /* read_file */

/***************************************************
		XDME ADAPTION 18-08-93
***************************************************/


/* get a variable - XDME ignores arg1 */
/* currently (20-10-93) we can NOT use a const char* name */

#if 1


char* mygetv (APTR db, char* name)
{
    static char tmp_buf[256];
    int  len;
    char*template = "%s";

/* PrintF ("mygetVAR: <%s>\n", name); */

    if (!name) return (NULL);

    strcpy (tmp_buf, name);
    name = tmp_buf;

    len = strlen (name);

    { /* cut trailing whitespace */
	char * dummy;
	dummy = name + len - 1;

	while (dummy > name && isspace(*dummy)) {
	    *dummy = 0;
	    --dummy;
	} /* while */
    }

    while (isspace(*name)) {++name;}

    {
	char* dummy = name;

	/* find postpended SPRINTF-tplt */
	while (*dummy && (*dummy != ':')) ++dummy;
	if (*dummy && dummy[1]) { /* has a postpended sPrintF - template */
	    template = dummy + 1;

	    /* Remove Whitespace between name and Template */
	    --dummy;
	    while (isspace(*dummy) && dummy > name) --dummy;
	    ++dummy;
	    *dummy = 0;
	} /* if */
    }

    {
	if ((name = getvar(name))) {
	    sprintf (tmp_buf, template, name, name, name, name);
	    return (tmp_buf);
	} /* if */
    }



    return (NULL);
} /* mygetv */

#else

char* mygetv (APTR* db, char* name)
{
    char * x;
#if 0
    if (x = getvar (name)) {
	strcpy (tmp_buffer, x);
	free   (x);
	return (tmp_buffer);
    } else
#endif
    {
	char* dummy = name;
	char* tpl   = "%s";
	char  sec;

	/* find postpended SPRINTF-tplt */

	while (*dummy && (*dummy != ':')) ++dummy;
	if (*dummy && dummy[1]) { /* has a postpended sPrintF - template */
	    tpl = dummy + 1;
	} /* if */

#if 0
	/* Remove Whitespace between name and Template */
	--dummy;
	while (isspace(*dummy) && dummy > name) --dummy;
	++dummy;
#endif
	sec    = *dummy;
	*dummy = 0;

	if ((x = getvar (name))) {
	    sprintf(tmp_buffer, tpl, x);
	    free   (x);
	    return (tmp_buffer);
	} /* if */
	*dummy = sec;
    } /* if */
    return (NULL);
} /* mygetv */

#endif



/* output a string - XDME ignores arg1 */

BOOL myfputs (APTR out, char* str)
{
    if (!*str) return (TRUE);
    av[0] = "insert";
    av[1] = str;
    do_insertwrite();
    return (BOOL)(GETF_ABORTCOMMAND(Ep) ? FALSE : TRUE);
} /* myfputs */




/***************************************************
		XDME COMMAND INTERFACE 18-08-93
***************************************************/

/*DEFLONG #long READTEMPLATE

This command inserts @{B}file@{UB}.
passing all lines starting with "$$" to EVAL
(lines starting with "$$#" are ignored) and
replacing all "$(...)" and "$`...'" by their
values if matching variables do exist.

That function might be very useful for
handling very formalistic structs, which
do need only little work by user.

Since the parse is line-oriented, usage of
linefeeds in "$(...)" and "$`...' is forbidden

*/

/*DEFHELP READTEMPLATE filename - read in a file and replace all occurencies of $(varname) with the contents of that varname */

DEFUSERCMD("readtemplate", 1, CF_ICO, int, do_readtplt, (void),)
{
    read_file (av[1]);
    return (1);
} /* do_readtplt */




#ifdef STATIC_COM

    COMMAND("readtemplate",        1, CF_ICO, (FPTR)do_readtplt )

#endif


/******************************************************************************
*****  END tplter.c
******************************************************************************/

