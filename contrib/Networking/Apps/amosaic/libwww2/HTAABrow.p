
/* MODULE							HTAABrow.c
**		BROWSER SIDE ACCESS AUTHORIZATION MODULE
**
**	Containts the code for keeping track on server hostnames,
**	port numbers, scheme names, usernames, passwords
**	(and servers' public keys).
**
** IMPORTANT:
**	Routines in this module use dynamic allocation, but free
**	automatically all the memory reserved by them.
**
**	Therefore the caller never has to (and never should)
**	free() any object returned by these functions.
**
**	Therefore also all the strings returned by this package
**	are only valid until the next call to the same function
**	is made. This approach is selected, because of the nature
**	of access authorization: no string returned by the package
**	needs to be valid longer than until the next call.
**
**	This also makes it easy to plug the AA package in:
**	you don't have to ponder whether to free() something
**	here or is it done somewhere else (because it is always
**	done somewhere else).
**
**	The strings that the package needs to store are copied
**	so the original strings given as parameters to AA
**	functions may be freed or modified with no side effects.
**
**	The AA package does not free() anything else than what
**	it has itself allocated.
**
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**
** HISTORY:
**
**
** BUGS:
**
**
*/

/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

typedef unsigned int size_t;
/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/***
*
*     ANSI copying functions
*
***/

extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern char *strcpy(char *, const char *);
extern char *strncpy(char *, const char *, size_t);


/***
*
*     ANSI concatenation functions
*
***/

extern char *strcat(char *, const char *);
extern char *strncat(char *, const char *, size_t);


/***
*
*     ANSI comparison functions
*
***/

extern int memcmp(const void *, const void *, size_t);
extern int strcmp(const char *, const char *);
extern int strcoll(const char *, const char *);
extern int strncmp(const char *, const char *, size_t);
extern size_t strxfrm(char *, const char *, size_t);


/***
*
*     ANSI search functions
*
***/

extern void *memchr(const void *, int, size_t);
extern char *strchr(const char *, int);
extern size_t strcspn(const char *, const char *);
extern char *strpbrk(const char *, const char *);
extern char *strrchr(const char *, int);
extern size_t strspn(const char *, const char *);
extern char *strstr(const char *, const char *);
extern char *strtok(char *, const char *);


/***
*
*     ANSI miscellaneous functions
*
***/

extern void *memset(void *, int, size_t);
extern char *strerror(int);
extern size_t strlen(const char *);

/***
*
*     SAS string and memory functions.
*
***/

extern int stcarg(const char *, const char *);
extern int stccpy(char *, const char *, int);
extern int stcgfe(char *, const char *);
extern int stcgfn(char *, const char *);
extern int stcis(const char *, const char *);
extern int stcisn(const char *, const char *);
extern int __stcd_i(const char *, int *);
extern int __stcd_l(const char *, long *);
extern int stch_i(const char *, int *);
extern int stch_l(const char *, long *);
extern int stci_d(char *, int);
extern int stci_h(char *, int);
extern int stci_o(char *, int);
extern int stcl_d(char *, long);
extern int __stcl_h(char *, long);
extern int __stcl_o(char *, long);
extern int stco_i(const char *, int *);
extern int stco_l(const char *, long *);
extern int stcpm(const char *, const char *, char **);
extern int stcpma(const char *, const char *);
extern int stcsma(const char *, const char *);
extern int astcsma(const char *, const char *);
extern int stcu_d(char *, unsigned);
extern int __stcul_d(char *, unsigned long);

extern char *stpblk(const char *);
extern char *stpbrk(const char *, const char *);
extern char *stpchr(const char *, int);
extern char *stpcpy(char *, const char *);
extern char *__stpcpy(char *, const char *);
extern char *stpdate(char *, int, const char *);
extern char *stpsym(const char *, char *, int);
extern char *stptime(char *, int, const char *);
extern char *stptok(const char *, char *, int, const char *);

extern int strbpl(char **, int, const char *);
extern int stricmp(const char *, const char *);
extern char *strdup(const char *);
extern void strins(char *, const char *);
extern int strmid(const char *, char *, int, int);
extern char *__strlwr(char *);
extern void strmfe(char *, const char *, const char *);
extern void strmfn(char *, const char *, const char *, const char *, 
                   const char *);
extern void strmfp(char *, const char *, const char *);
extern int strnicmp(const char *, const char *, size_t);
extern char *strnset(char *, int, int);

extern char *stpchrn(const char *, int);
extern char *strrev(char *);
extern char *strset(char *, int);
extern void strsfn(const char *, char *, char *, char *, char *);
extern char *__strupr(char *);
extern int stspfp(char *, int *);
extern void strsrt(char *[], int);

extern int stcgfp(char *, const char *);

extern void *memccpy(void *, const void *, int, unsigned);
extern void movmem(const void *, void *, unsigned);
extern void repmem(void *, const void *, size_t, size_t);
extern void setmem(void *, unsigned, int);
extern void __swmem(void *, void *, unsigned);
/* for BSD compatibility */
/**
*
* Builtin function definitions
*
**/

extern size_t  __builtin_strlen(const char *);
extern int     __builtin_strcmp(const char *, const char *);
extern char   *__builtin_strcpy(char *, const char *);

extern void   *__builtin_memset(void *, int, size_t);
extern int     __builtin_memcmp(const void *, const void *, size_t);
extern void   *__builtin_memcpy(void *, const void *, size_t);

extern int __builtin_max(int, int);
extern int __builtin_min(int, int);
extern int __builtin_abs(int);

/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*
**	$VER: types.h 40.1 (10.8.93)
**	Includes Release 40.13
**
**	Data typing.  Must be included before any other Amiga include.
**
**	(C) Copyright 1985-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/


  /*  WARNING: APTR was redefined for the V36 Includes!  APTR is a   */
 /*  32-Bit Absolute Memory Pointer.  C pointer math will not	    */
/*  operate on APTR --	use "ULONG *" instead.			   */
typedef void	       *APTR;	    /* 32-bit untyped pointer */
typedef long		LONG;	    /* signed 32-bit quantity */
typedef unsigned long	ULONG;	    /* unsigned 32-bit quantity */
typedef unsigned long	LONGBITS;   /* 32 bits manipulated individually */
typedef short		WORD;	    /* signed 16-bit quantity */
typedef unsigned short	UWORD;	    /* unsigned 16-bit quantity */
typedef unsigned short	WORDBITS;   /* 16 bits manipulated individually */
typedef signed char	BYTE;	    /* signed 8-bit quantity */
typedef unsigned char	UBYTE;	    /* unsigned 8-bit quantity */
typedef unsigned char	BYTEBITS;   /* 8 bits manipulated individually */
typedef unsigned short	RPTR;	    /* signed relative pointer */

typedef unsigned char  *STRPTR;     /* string pointer (NULL terminated) */
/* For compatibility only: (don't use in new code) */
typedef short		SHORT;	    /* signed 16-bit quantity (use WORD) */
typedef unsigned short	USHORT;     /* unsigned 16-bit quantity (use UWORD) */
typedef short		COUNT;
typedef unsigned short	UCOUNT;
typedef ULONG		CPTR;


/* Types with specific semantics */
typedef float		FLOAT;
typedef double		DOUBLE;
typedef short		BOOL;
typedef unsigned char	TEXT;

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

 /* #define LIBRARY_VERSION is now obsolete.  Please use LIBRARY_MINIMUM */
/* or code the specific minimum library version you require.		*/
  typedef BOOL BOOLEAN;
/*

Debug message control.

 */
/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */


/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

typedef char *va_list;
typedef unsigned long fpos_t;

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/**
*
* Definitions associated with __iobuf._flag
*
**/

struct __iobuf {
    struct __iobuf *_next;
    unsigned char *_ptr;	/* current buffer pointer */
    int _rcnt;		        /* current byte count for reading */
    int _wcnt;		        /* current byte count for writing */
    unsigned char *_base;	/* base address of I/O buffer */
    int _size;			/* size of buffer */
    int _flag;	        	/* control flags */
    int _file;		        /* file descriptor */
    unsigned char _cbuff;	/* single char buffer */
};

typedef struct __iobuf FILE;

extern struct __iobuf __iob[];

/***
*
*     Prototypes for ANSI standard functions.
*
***/


extern int remove(const char *);
extern int rename(const char *, const char *);
extern FILE *tmpfile(void);
extern char *tmpnam(char *s);

extern int fclose(FILE *);
extern int fflush(FILE *);
extern FILE *fopen(const char *, const char *);
extern FILE *freopen(const char *, const char *, FILE *);
extern void setbuf(FILE *, char *);
extern int setvbuf(FILE *, char *, int, size_t);

extern int fprintf(FILE *, const char *, ...);
extern int fscanf(FILE *, const char *, ...);
extern int printf(const char *, ...);
extern int __builtin_printf(const char *, ...);
extern int scanf(const char *, ...);
extern int sprintf(char *, const char *, ...);
extern int sscanf(const char *, const char *, ...);
extern int vfprintf(FILE *, const char *, va_list);
extern int vprintf(const char *, va_list);
extern int vsprintf(char *, const char *, va_list);

extern int fgetc(FILE *);
extern char *fgets(char *, int, FILE *);
extern int fputc(int, FILE *);
extern int fputs(const char *, FILE *);
extern int getc(FILE *);
extern int getchar(void);
extern char *gets(char *);
extern int putc(int, FILE *);

extern int putchar(int);
extern int puts(const char *);
extern int ungetc(int, FILE *);

extern size_t fread(void *, size_t, size_t, FILE *);
extern size_t fwrite(const void *, size_t, size_t, FILE *);
extern int fgetpos(FILE *, fpos_t *);
extern int fseek(FILE *, long int, int);
extern int fsetpos(FILE *, const fpos_t *);
extern long int ftell(FILE *);
extern void rewind(FILE *);
extern void clearerr(FILE *);
extern int feof(FILE *);
extern int ferror(FILE *);
extern void perror(const char *);

/* defines for mode of access() */
/***
*
*     Prototypes for Non-ANSI functions.
*
***/

extern int __io2errno(int);
extern int fcloseall(void);
extern FILE *fdopen(int, const char *);
extern int fhopen(long, int);
extern int fgetchar(void);
extern int fileno(FILE *);
extern int flushall(void);
extern void fmode(FILE *, int);
extern int _writes(const char *, ...);
extern int _tinyprintf(char *, ...);
extern int fputchar(int);
extern void setnbf(FILE *);
extern int __fillbuff(FILE *);
extern int __flushbuff(int, FILE *);
extern int __access(const char *, int);
extern int access(const char *, int);
extern int chdir(const char *);
extern int chmod(const char *, int);
extern char *getcwd(char *, int);
extern int unlink(const char *);
extern int poserr(const char *);

/***
*
*     The following routines allow for raw console I/O.
*
***/

int rawcon(int);
int getch(void);

extern unsigned long __fmask;
extern int __fmode;

        extern int WWW_TraceFlag;
/*

Standard C library for malloc() etc

 */
/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */


/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

typedef char wchar_t;
/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

typedef struct {
            int quot;
            int rem;
        } div_t;

typedef struct {
            long int quot;
            long int rem;
        } ldiv_t;

extern char __mb_cur_max;

extern double atof(const char *);
extern int atoi(const char *);
extern long int atol(const char *);

extern double strtod(const char *, char **);
extern long int strtol(const char *, char **, int);
extern unsigned long int strtoul(const char *, char **, int);


extern int rand(void);
extern void srand(unsigned int);


/***
*
*     ANSI memory management functions
*
***/

extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);

extern void *halloc(unsigned long);              /*  Extension  */
extern void *__halloc(unsigned long);            /*  Extension  */
/***
*
*     ANSI environment functions
*
***/

extern void abort(void);
extern int atexit(void (*)(void));
extern void exit(int);
extern char *__getenv(const char *);
extern char *getenv(const char *);
extern int system(const char *);


/***
*
*     ANSI searching and sorting functions
*
***/

extern void *bsearch(const void *, const void *, size_t, size_t,
                     int (*)(const void *, const void *));
extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));


/***
*
*     ANSI integer arithmetic functions
*
***/
extern int abs(int);
extern div_t div(int, int);
extern long int labs(long int);
extern ldiv_t ldiv(long int, long int);


/***
*
*     ANSI multibyte character functions
*
***/

extern int mblen(const char *, size_t);
extern int mbtowc(wchar_t *, const char *, size_t);
extern int wctomb(char *, wchar_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);


/***
*
*     SAS Level 2 memory allocation functions
*
***/

extern void *getmem(unsigned int);
extern void *getml(long);
extern void rlsmem(void *, unsigned int);
extern void rlsml(void *, long);
extern int bldmem(int);
extern long sizmem(void);
extern long chkml(void);
extern void rstmem(void);


/***
*
*     SAS Level 1 memory allocation functions
*
***/

extern void *sbrk(unsigned int);
extern void *lsbrk(long);
extern int rbrk(void);
extern void __stdargs _MemCleanup(void);

extern unsigned long _MemType;
extern void *_MemHeap;
extern unsigned long _MSTEP;


/**
*
* SAS Sort functions
*
*/

extern void dqsort(double *, size_t);
extern void fqsort(float *, size_t);
extern void lqsort(long *, size_t);
extern void sqsort(short *, size_t);
extern void tqsort(char **, size_t);


/***
*
*     SAS startup, exit and environment functions.
*
***/

extern void __exit(int);
extern void __stdargs __main(char *);
extern void __stdargs __tinymain(char *);
extern void __stdargs _XCEXIT(long);
extern char *argopt(int, char**, char *, int *, char *);
extern int iabs(int);
extern int onexit(void (*)(void));
extern int putenv(const char *);

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

#pragma msg 148 ignore push   /* Ignore message if tag is undefined*/
extern struct WBStartup *_WBenchMsg;  /* WorkBench startup, if the */
#pragma msg 148 pop                   /* WorkBench.   Same as argv.*/
/* The following two externs give you the information in the   */
/* WBStartup structure parsed out to look like an (argc, argv) */
/* pair.  Don't define them in your code;  just include this   */
/* file and use them.  If the program was not run from         */
/* WorkBench, _WBArgc will be zero.                            */

extern int _WBArgc;    /* Count of the number of WorkBench arguments */
extern char **_WBArgv; /* The actual arguments                       */

/*

Macros for declarations

 */
/*

Booleans

 */
/* Note: GOOD and BAD are already defined (differently) on RS6000 aix */
/* #define GOOD(status) ((status)38;1)   VMS style status: test bit 0         */
/* #define BAD(status)  (!GOOD(status))  Bit 0 set if OK, otherwise clear   */

/*      Inline Function WHITE: Is character c white space? */
/*      For speed, include all control characters */

/*

Sucess (>=0) and failure (<0) codes

 */

/*                                                                 String handling for libwww
                                         STRINGS
                                             
   Case-independent string comparison and allocations with copies etc
   
 */
/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */


extern int WWW_TraceFlag;       /* Global flag for all W3 trace */

extern  char * HTLibraryVersion;   /* String for help screen etc */

/*

Case-insensitive string comparison

   The usual routines (comp instead of cmp) had some problem.
   
 */
extern int strcasecomp  ( char *a,  char *b);
extern int strncasecomp ( char *a,  char *b, int n);

/*

Malloced string manipulation

 */
extern char * HTSACopy (char **dest,  char *src);
extern char * HTSACat  (char **dest,  char *src);

/*

Next word or quoted string

 */
extern char * HTNextField (char** pstr);


/*

   end
   
    */


/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */


/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/*

Out Of Memory checking for malloc() return:

 */
/*

Upper- and Lowercase macros

   The problem here is that toupper(x) is not defined officially unless isupper(x) is.
   These macros are CERTAINLY needed on #if defined(pyr) || define(mips) or BDSI
   platforms. For safefy, we make them mandatory.
   
 */
/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved. */


/**
*
* This header file defines various ASCII character manipulation macros,
* as follows:
*
*       isalpha(c)    non-zero if c is alpha
*       isupper(c)    non-zero if c is upper case
*       islower(c)    non-zero if c is lower case
*       isdigit(c)    non-zero if c is a digit (0 to 9)
*       isxdigit(c)   non-zero if c is a hexadecimal digit (0 to 9, A to F,
*                   a to f)
*       isspace(c)    non-zero if c is white space
*       ispunct(c)    non-zero if c is punctuation
*       isalnum(c)    non-zero if c is alpha or digit
*       isprint(c)    non-zero if c is printable (including blank)
*       isgraph(c)    non-zero if c is graphic (excluding blank)
*       iscntrl(c)    non-zero if c is control character
*       isascii(c)    non-zero if c is ASCII
*       iscsym(c)     non-zero if valid character for C symbols
*       iscsymf(c)    non-zero if valid first character for C symbols
*
**/

extern int isalnum(int);
extern int isalpha(int);
extern int iscntrl(int);
extern int isdigit(int);
extern int isgraph(int);
extern int islower(int);
extern int isprint(int);
extern int ispunct(int);
extern int isspace(int);
extern int isupper(int);
extern int isxdigit(int);

extern int tolower(int);
extern int toupper(int);

extern char __ctype[];   /* character type table */

/****
*
*   Extensions to the ANSI standard.
*
*
****/

extern int isascii(int);
extern int iscsym(int);
extern int iscsymf(int);

extern int toascii(int);


  /* Pyramid and Mips can't uppercase non-alpha */
/*

   end of utilities  */

/*                                                                 String handling for libwww
                                         STRINGS
                                             
   Case-independent string comparison and allocations with copies etc
   
 */
/*

   end
   
    */

/*                                                   HTParse:  URL parsing in the WWW Library
                                         HTPARSE
                                             
   This module of the WWW library contains code to parse URLs and various related things.
   Implemented by HTParse.c .
   
 */
/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */


/*

   The following are flag bits which may be ORed together to form a number to give the
   'wanted' argument to HTParse.
   
 */
/*

HTParse:  Parse a URL relative to another URL

   This returns those parts of a name which are given (and requested) substituting bits
   from the related name where necessary.
   
  ON ENTRY
  
  aName                   A filename given
                         
  relatedName             A name relative to which aName is to be parsed
                         
  wanted                  A mask for the bits which are wanted.
                         
  ON EXIT,
  
  returns                 A pointer to a malloc'd string which MUST BE FREED
                         
 */

extern char * HTParse  (char * aName, char * relatedName, int wanted);


/*

HTStrip: Strip white space off a string

  ON EXIT
  
   Return value points to first non-white character, or to 0 if none.
   
   All trailing white space is OVERWRITTEN with zero.
   
 */
extern char * HTStrip(char * s);
/*

HTSimplify: Simplify a UTL

   A URL is allowed to contain the seqeunce xxx/../ which may be replaced by "" , and the
   seqeunce "/./" which may be replaced by "/". Simplification helps us recognize
   duplicate filenames. It doesn't deal with soft links, though. The new (shorter)
   filename overwrites the old.
   
 */
/*
**      Thus,   /etc/junk/../fred       becomes /etc/fred
**              /etc/junk/./fred        becomes /etc/junk/fred
*/
extern void HTSimplify(char * filename);
/*

HTRelative:  Make Relative (Partial) URL

   This function creates and returns a string which gives an expression of one address as
   related to another. Where there is no relation, an absolute address is retured.
   
  ON ENTRY,
  
   Both names must be absolute, fully qualified names of nodes (no anchor bits)
   
  ON EXIT,
  
   The return result points to a newly allocated name which, if parsed by HTParse relative
   to relatedName, will yield aName. The caller is responsible for freeing the resulting
   name later.
   
 */
extern char * HTRelative(char * aName, char *relatedName);
/*

HTEscape:  Encode unacceptable characters in string

   This funtion takes a string containing any sequence of ASCII characters, and returns a
   malloced string containing the same infromation but with all "unacceptable" characters
   represented in the form %xy where X and Y are two hex digits.
   
 */
extern char * HTEscape (char * str);


/*

HTUnEscape: Decode %xx escaped characters

   This function takes a pointer to a string in which character smay have been encoded in
   %xy form, where xy is the acsii hex code for character 16x+y. The string is converted
   in place, as it will never grow.
   
 */
extern char * HTUnEscape ( char * str);


/*

   end of HTParse
   
    */

/*  */

/*              List object
**
**      The list object is a generic container for storing collections
**      of things in order.
*/
/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */


typedef struct _HTList HTList;

struct _HTList {
  void * object;
  HTList * next;
  HTList * last;
};

extern HTList * HTList_new (void);
extern void     HTList_delete (HTList *me);

/*      Add object to START of list
*/
extern void     HTList_addObject (HTList *me, void *newObject);
extern void     HTList_addObjectAtEnd (HTList *me, void *newObject);


extern BOOLEAN     HTList_removeObject (HTList *me, void *oldObject);
extern void *   HTList_removeLastObject (HTList *me);
extern void *   HTList_removeFirstObject (HTList *me);
extern int      HTList_count (HTList *me);
extern int      HTList_indexOf (HTList *me, void *object);
extern void *   HTList_objectAt (HTList *me, int position);

/* Fast macro to traverse the list. Call it first with copy of list header :
   it returns the first object and increments the passed list pointer.
   Call it with the same variable until it returns NULL. */
/*

    */

/*  */

/*      Displaying messages and getting input for WWW Library
**      =====================================================
**
**         May 92 Created By C.T. Barker
**         Feb 93 Portablized etc TBL
*/

/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */

/*		  System dependencies in the W3 library
				   SYSTEM DEPENDENCIES

   System-system differences for TCP include files and macros. This
   file includes for each system the files necessary for network and
   file I/O.

  AUTHORS

  TBL		     Tim Berners-Lee, W3 project, CERN, <timbl@info.cern.ch>
  EvA			  Eelco van Asperen <evas@cs.few.eur.nl>
  MA			  Marc Andreessen NCSA
  AT			  Aleksandar Totic <atotic@ncsa.uiuc.edu>
  SCW			  Susan C. Weber <sweber@kyle.eitech.com>
  MJW			  Michael J Witbrock <witbrock@cmu.edu>
  MWM			  Mike Meyer  <mwm@contessa.phone.net>

  HISTORY:
  22 Feb 91		  Written (TBL) as part of the WWW library.
  16 Jan 92		  PC code from EvA
  22 Apr 93		  Merged diffs bits from xmosaic release
  29 Apr 93		  Windows/NT code from SCW

  Much of the cross-system portability stuff has been intentionally
  REMOVED from this version of the library by Marc A in order to
  discourage attempts to make "easy" ports of Mosaic for X to non-Unix
  platforms.  The library needs to be rewritten from the ground up; in
  the meantime, Unix is *all* we support or intend to support with
  this set of source code.

*/

/*

Default values

   These values may be reset and altered by system-specific sections
   later on.  there are also a bunch of defaults at the end .

 */
/* Default values of those: */
/* Unless stated otherwise, */
typedef struct sockaddr_in SockA;  /* See netinet/in.h */


/* Solaris. */
/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* Use builtin strdup when appropriate. */
/*

VAX/VMS

   Under VMS, there are many versions of TCP-IP. Define one if you do
   not use Digital's UCX product:

  UCX			  DEC's "Ultrix connection" (default)
  WIN_TCP		  From Wollongong, now GEC software.
  MULTINET		  From SRI, now from TGV Inv.
  DECNET		  Cern's TCP socket emulation over DECnet

   The last three do not interfere with the
   unix i/o library, and so they need special calls to read, write and
   close sockets. In these cases the socket number is a VMS channel
   number, so we make the @@@ HORRIBLE @@@ assumption that a channel
   number will be greater than 10 but a unix file descriptor less than
   10.	It works.

 */
/*

SCO ODT unix version

 */

/*

MIPS unix

 */
/* Mips hack (bsd4.3/sysV mixture...) */

/*

Regular BSD unix versions

   These are a default unix where not already defined specifically.

 */
     typedef int bool_t;
     typedef long dev_t;
     typedef int enum_t;		/* For device numbers */
     typedef unsigned long ino_t;	/* For file serial numbers */
     typedef unsigned short mode_t;	/* For file types and modes */
     typedef short nlink_t;		/* For link counts */
     typedef long off_t;		/* For file offsets and sizes */
     typedef long pid_t;		/* For process and session IDs */
     typedef long gid_t;		/* For group IDs */
     typedef long uid_t;		/* For user IDs */
     typedef long time_t;		/* For times in seconds */
      typedef int ssize_t;		/* Signed version of size_t */
     typedef unsigned short __site_t;	/* see stat.h */
     typedef unsigned short __cnode_t;	/* see stat.h */
   typedef long __daddr_t;		/* For disk block addresses */
   typedef char *__caddr_t;		/* For character addresses */
   typedef long __swblk_t;

     typedef __caddr_t		caddr_t;   /* also in ptrace.h */
   typedef unsigned char  ubit8;
   typedef unsigned short ubit16;
   typedef unsigned long  ubit32;
   typedef char 	  sbit8;
   typedef short	  sbit16;
   typedef long 	  sbit32;

   typedef ubit8	  u_char;
   typedef ubit16	  u_short;
   typedef ubit32	  u_long;
   typedef ubit32	  u_int;

   typedef __swblk_t	  swblk_t;
   typedef __daddr_t	  daddr_t;
   typedef __site_t	  site_t;
   typedef __cnode_t	  cnode_t;

   typedef long 	  paddr_t;
   typedef short	  cnt_t;
   typedef unsigned int   space_t;
   typedef unsigned int   prot_t;
   typedef unsigned long  cdno_t;
   typedef unsigned short use_t;

   typedef struct _physadr { int r[1]; } *physadr;
   typedef struct _quad { long val[2]; } quad;

   typedef char spu_t;

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/* Types, macros, etc. for select() */

/*
 * MAXFUPLIM is the absolute limit of open files per process.  No process,
 * even super-user processes may increase u.u_maxof beyond MAXFUPLIM.
 * MAXFUPLIM means maximum files upper limit.
 * Important Note:  This definition should actually go into h/param.h, but
 * since it is needed by the select() macros which follow, it had to go here.
 * I did not put it in both files since h/param.h includes this file and that
 * would be error prone anyway.
 */
/*
 * These macros are used for select().  select() uses bit masks of file
 * descriptors in longs.  These macros manipulate such bit fields (the
 * file sysrem macros uses chars).  FD_SETSIZE may be defined by the user,
 * but must be >= u.u_highestfd + 1.  Since we know the absolute limit on
 * number of per process open files is 2048, we need to define FD_SETSIZE
 * to be large enough to accomodate this many file descriptors.  Unless the
 * user has this many files opened, he should redefine FD_SETSIZE to a
 * smaller number.
 */
     typedef long fd_mask;

     typedef struct fd_set {
       fd_mask fds_bits[(((128)+(((sizeof(fd_mask) * 8))-1))/((sizeof(fd_mask) * 8)))];
     } fd_set;

/* #include <streams/streams.h> 		not ultrix */
/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/*
 * Amiga standard errno.h
 */
/* non-blocking and interrupt i/o */
/* ipc/network software */

	/* argument errors */
	/* operational errors */
	/* */
/* should be rearranged */
/* quotas & mush */
/* Network File System */
/* streams */
/* SystemV IPC */
/* SystemV Record Locking */
/* Non-standard UNIX, ie Amigados errors */
extern char *__sys_errlist[];
extern int __sys_nerr, errno;

/*
** 4.x Unix time struct compatibility.
*/

/*
**	$VER: timer.h 36.16 (25.1.91)
**	Includes Release 40.13
**
**	Timer device name and useful definitions.
**
**	(C) Copyright 1985-1993 Commodore-Amiga Inc.
**		All Rights Reserved
*/

/*
**	$VER: io.h 39.0 (15.10.91)
**	Includes Release 40.13
**
**	Message structures used for device communication
**
**	(C) Copyright 1985-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/

/*
**	$VER: ports.h 39.0 (15.10.91)
**	Includes Release 40.13
**
**	Message ports and Messages.
**
**	(C) Copyright 1985-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/

/*
**	$VER: nodes.h 39.0 (15.10.91)
**	Includes Release 40.13
**
**	Nodes & Node type identifiers.
**
**	(C) Copyright 1985-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/

/*
 *  List Node Structure.  Each member in a list starts with a Node
 */

struct Node {
    struct  Node *ln_Succ;	/* Pointer to next (successor) */
    struct  Node *ln_Pred;	/* Pointer to previous (predecessor) */
    UBYTE   ln_Type;
    BYTE    ln_Pri;		/* Priority, for sorting */
    char    *ln_Name;		/* ID string, null terminated */
};	/* Note: word aligned */

/* minimal node -- no type checking possible */
struct MinNode {
    struct MinNode *mln_Succ;
    struct MinNode *mln_Pred;
};


/*
** Note: Newly initialized IORequests, and software interrupt structures
** used with Cause(), should have type NT_UNKNOWN.  The OS will assign a type
** when they are first used.
*/
/*----- Node Types for LN_TYPE -----*/
/*
**	$VER: lists.h 39.0 (15.10.91)
**	Includes Release 40.13
**
**	Definitions and macros for use with Exec lists
**
**	(C) Copyright 1985-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/

/*
 *  Full featured list header.
 */
struct List {
   struct  Node *lh_Head;
   struct  Node *lh_Tail;
   struct  Node *lh_TailPred;
   UBYTE   lh_Type;
   UBYTE   l_pad;
};	/* word aligned */

/*
 * Minimal List Header - no type checking
 */
struct MinList {
   struct  MinNode *mlh_Head;
   struct  MinNode *mlh_Tail;
   struct  MinNode *mlh_TailPred;
};	/* longword aligned */


/*
 *	Check for the presence of any nodes on the given list.	These
 *	macros are even safe to use on lists that are modified by other
 *	tasks.	However; if something is simultaneously changing the
 *	list, the result of the test is unpredictable.
 *
 *	Unless you first arbitrated for ownership of the list, you can't
 *	_depend_ on the contents of the list.  Nodes might have been added
 *	or removed during or after the macro executes.
 *
 *		if( IsListEmpty(list) )		printf("List is empty\n");
 */
/*
**	$VER: tasks.h 39.3 (18.9.92)
**	Includes Release 40.13
**
**	Task Control Block, Singals, and Task flags.
**
**	(C) Copyright 1985-1993 Commodore-Amiga, Inc.
**	    All Rights Reserved
*/

/* Please use Exec functions to modify task structure fields, where available.
 */
struct Task {
    struct  Node tc_Node;
    UBYTE   tc_Flags;
    UBYTE   tc_State;
    BYTE    tc_IDNestCnt;	    /* intr disabled nesting*/
    BYTE    tc_TDNestCnt;	    /* task disabled nesting*/
    ULONG   tc_SigAlloc;	    /* sigs allocated */
    ULONG   tc_SigWait;	    /* sigs we are waiting for */
    ULONG   tc_SigRecvd;	    /* sigs we have received */
    ULONG   tc_SigExcept;	    /* sigs we will take excepts for */
    UWORD   tc_TrapAlloc;	    /* traps allocated */
    UWORD   tc_TrapAble;	    /* traps enabled */
    APTR    tc_ExceptData;	    /* points to except data */
    APTR    tc_ExceptCode;	    /* points to except code */
    APTR    tc_TrapData;	    /* points to trap data */
    APTR    tc_TrapCode;	    /* points to trap code */
    APTR    tc_SPReg;		    /* stack pointer	    */
    APTR    tc_SPLower;	    /* stack lower bound    */
    APTR    tc_SPUpper;	    /* stack upper bound + 2*/
    void    (*tc_Switch)();	    /* task losing CPU	  */
    void    (*tc_Launch)();	    /* task getting CPU  */
    struct  List tc_MemEntry;	    /* Allocated memory. Freed by RemTask() */
    APTR    tc_UserData;	    /* For use by the task; no restrictions! */
};

/*
 * Stack swap structure as passed to StackSwap()
 */
struct	StackSwapStruct {
	APTR	stk_Lower;	/* Lowest byte of stack */
	ULONG	stk_Upper;	/* Upper end of stack (size + Lowest) */
	APTR	stk_Pointer;	/* Stack pointer at switch point */
};

/*----- Flag Bits ------------------------------------------*/
/*----- Task States ----------------------------------------*/
/*----- Predefined Signals -------------------------------------*/
/****** MsgPort *****************************************************/

struct MsgPort {
    struct  Node mp_Node;
    UBYTE   mp_Flags;
    UBYTE   mp_SigBit;		/* signal bit number	*/
    void   *mp_SigTask;		/* object to be signalled */
    struct  List mp_MsgList;	/* message linked list	*/
};

/* mp_Flags: Port arrival actions (PutMsg) */
/****** Message *****************************************************/

struct Message {
    struct  Node mn_Node;
    struct  MsgPort *mn_ReplyPort;  /* message reply port */
    UWORD   mn_Length;		    /* total message length, in bytes */
				    /* (include the size of the Message */
				    /* structure in the length) */
};

struct IORequest {
    struct  Message io_Message;
    struct  Device  *io_Device;     /* device node pointer  */
    struct  Unit    *io_Unit;	    /* unit (driver private)*/
    UWORD   io_Command;	    /* device command */
    UBYTE   io_Flags;
    BYTE    io_Error;		    /* error or warning num */
};

struct IOStdReq {
    struct  Message io_Message;
    struct  Device  *io_Device;     /* device node pointer  */
    struct  Unit    *io_Unit;	    /* unit (driver private)*/
    UWORD   io_Command;	    /* device command */
    UBYTE   io_Flags;
    BYTE    io_Error;		    /* error or warning num */
    ULONG   io_Actual;		    /* actual number of bytes transferred */
    ULONG   io_Length;		    /* requested number bytes transferred*/
    APTR    io_Data;		    /* points to data area */
    ULONG   io_Offset;		    /* offset for block structured devices */
};

/* library vector offsets for device reserved vectors */
/* io_Flags defined bits */
/* unit defintions */
struct timeval {
    ULONG tv_secs;
    ULONG tv_micro;
};

struct EClockVal {
    ULONG ev_hi;
    ULONG ev_lo;
};

struct timerequest {
    struct IORequest tr_node;
    struct timeval tr_time;
};

/* IO_COMMAND to use for adding a timer */
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

struct itimerval {
	struct timeval	it_value, it_interval;
};

/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

typedef unsigned long clock_t;

/**
*
* This structure contains the unpacked time as returned by "gmtime".
*
*/

struct tm {
    int tm_sec;      /* seconds after the minute */
    int tm_min;          /* minutes after the hour */
    int tm_hour;         /* hours since midnight */
    int tm_mday;         /* day of the month */
    int tm_mon;          /* months since January */
    int tm_year;         /* years since 1900 */
    int tm_wday;         /* days since Sunday */
    int tm_yday;         /* days since January 1 */
    int tm_isdst;        /* Daylight Savings Time flag */
};


/***
*
*     ANSI time functions.
*
***/

extern clock_t clock(void);
extern double difftime(time_t, time_t);
extern time_t mktime(struct tm *);
extern time_t time(time_t *);

extern char *asctime(const struct tm *);
extern char *ctime(const time_t *);
extern struct tm *gmtime(const time_t *);
extern struct tm *localtime(const time_t *);
extern size_t strftime(char *, size_t, const char *, const struct tm *);


/***
*
*     SAS time functions
*
***/

void getclk(unsigned char *);
int  chgclk(const unsigned char *);

void utunpk(long, char *);
long utpack(const char *);
int timer(unsigned int *);
int datecmp(const struct DateStamp *, const struct DateStamp *);

time_t __datecvt(const struct DateStamp *);
struct DateStamp *__timecvt(time_t);

/* for UNIX compatibility */
extern void      __tzset(void);
/***
*     SAS external variables
***/

extern int  __daylight;
extern long __timezone;
extern char *__tzname[2];
extern char __tzstn[4];
extern char __tzdtn[4];
extern char *_TZ;


/*
** stat structure used by fstat() and stat()
** will not be compatible with your compiler's stat() and fstat()
*/
struct stat
{
	dev_t	st_dev;
	ino_t	st_ino;
	mode_t	st_mode;
	nlink_t st_nlink;
	unsigned short st_reserved1; /* old st_uid, replaced spare positions */
	unsigned short st_reserved2; /* old st_gid, replaced spare positions */
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	int	st_spare1;
	time_t	st_mtime;
	int	st_spare2;
	time_t	st_ctime;
	int	st_spare3;
	long	st_blksize;
	long	st_blocks;
	unsigned int	st_pad:30;
	unsigned int	st_acl:1;   /* set if there are optional ACL entries */
	unsigned int	st_remote:1;	/* Set if file is remote */
	dev_t	st_netdev;	/* ID of device containing */
				/* network special file */
	ino_t	st_netino;	/* Inode number of network special file */
	__cnode_t	st_cnode;
	__cnode_t	st_rcnode;
	/* The site where the network device lives			*/
	__site_t	st_netsite;
	short	st_fstype;
	/* Real device number of device containing the inode for this file*/
	dev_t	st_realdev;
	/* Steal three spare for the device site number 		  */
	unsigned short	st_basemode;
	unsigned short	st_spareshort;
	uid_t	st_uid;
	gid_t	st_gid;
	long	st_spare4[3];
};

/* st_mode will have bits set as follows */
/* the least significant 9 bits will be the unix
** rwxrwxrwx bits (octal 777).
*/

/*
** Some signal definitions.  These are meant to build on the ones defined
** in the SAS/C headers.  Will probably need to be changed for other
** compilers.
*/

/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */


/**
*
* This header file contains definitions needed by the signal function.
*
**/

/**
*
* NSIG supposedly defines the number of signals recognized.  However,
* since not all signals are actually implemented under AmigaDOS, it actually
* is the highest legal signal number plus one.
*
*/

/**
*
* The following symbols are the defined signals.
*
*/

/***
*
* The following symbols are the special forms for the function pointer
* argument.  They specify certain standard actions that can be performed
* when the signal occurs.
*
***/

/***
*
* Function declarations
*
***/

extern void (*signal(int,void (*)(int)))(int);
extern int raise(int);

extern void (*__sigfunc[9])(int);

typedef int sig_atomic_t;

/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */


/***
*
* The following symbols are used for the "open" and "creat" functions.
* They are generally UNIX-compatible, except for O_APPEND under MSDOS,
* which has been moved in order to accomodate the file sharing flags
* defined in MSDOS Version 3.
*
* Also, O_TEMP and O_RAW are SAS extensions.
*
***/

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/***
*
* The following symbols are used for the "fcntl" function.
*
***/

/***
*
* External definitions
*
***/

extern int  __creat  (const char *, int);
extern long __lseek  (int, long, int);

extern int  __open   (const char *, int, ...);
extern int  __read   (int, void *, unsigned int);
extern int  __write  (int, const void *, unsigned int);
extern int  __close  (int);
extern int  open   (const char *, int, ...);
extern int  creat  (const char *, int);
extern int  read   (int, void *, unsigned int);
extern int  write  (int, const void *, unsigned int);
extern long lseek  (int, long, int);
extern long tell   (int);
extern int  close  (int);
extern int  unlink (const char *);
extern int  iomode (int, int);
extern int  isatty (int);

/* Copyright (c) 1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */

/* This header file contains common preprocessor symbol   */
/* definitions that were previously duplicated throughout */
/* the header files. Those definitions were moved here    */
/* and replaced with a #include of this header file.      */
/* This was done to purify the header files for GST       */
/* processing.                                            */

/* Copyright (c) 1992-1993 SAS Institute, Inc., Cary, NC USA */
/* All Rights Reserved */


/**
*
* The following symbols are specified in the ANSI C standard as limit
* values for various non-float characteristics.
*
**/

/*			Directory reading stuff - BSD or SYS V
*/
/* This stuff comes BEFORE the defaults below */

/*
** inet ioctl() operations.  Here rather than in more sensible places
** mostly for Unix compatibility.
*/
/*
** ttychars.h - emulate Berkeley ttychars structs.  Mostly psyched out
**		from telnet/sys_bsd.c
*/

struct ltchars {
	char	t_flushc;
	char	t_dsuspc;
	char	t_suspc;
	char	t_brkc;
	char	t_intrc;
	char	t_quitc;
};

/*  Comment out to prevent confusion for Manx 5.0
    Does anyone use this?

struct tchars {
	char	t_flushc;
	char	t_dsuspc;
	char	t_suspc;
	char	t_brkc;
	char	t_intrc;
	char	t_quitc;
	char	t_startc;
	char	t_stopc;
	char	t_syspc;
	char	t_eofc;
};

*********************************************/
/*
** 4.x Unix time struct compatibility.
*/

extern struct Library *SockBase;
/*
**	ss/socket.h
*/

/*
 * Copyright (c) 1982, 1985, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)socket.h	7.3 (Berkeley) 6/27/88
 */

/*
 * Definitions related to sockets: types, address families, options.
 */

/*
 * Types
 */
/*
 * Option flags per-socket.
 */
/*
 * Additional options, not kept in so_options.
 */
/*
 * Structure used for manipulating linger option.
 */
struct	linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
/*
 * Address families.
 */
/*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr {
	u_short	sa_family;		/* address family */
	char	sa_data[14];		/* up to 14 bytes of direct address */
};

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto {
	u_short	sp_family;		/* address family */
	u_short	sp_protocol;		/* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
/*
 * Maximum queue length specifiable by listen.
 */
/*
 * Message header for recvmsg and sendmsg calls.
 */
struct msghdr {
	caddr_t	msg_name;		/* optional address */
	int	msg_namelen;		/* size of address */
	struct	iovec *msg_iov;		/* scatter/gather array */
	int	msg_iovlen;		/* # elements in msg_iov */
	caddr_t	msg_accrights;		/* access rights sent/received */
	int	msg_accrightslen;
};

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)in.h        7.6 (Berkeley) 6/29/88
 */

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981.
 */

/*
 * Protocols
 */
/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.
 */
/*
 * Link numbers
 */
/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr {
	u_long s_addr;
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
/*
 * Socket address, internet style.
 */
struct sockaddr_in {
	short	sin_family;
	u_short sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 */
extern struct Library *SockBase;

/* prototypes */

int accept (int, struct sockaddr *, int *);
int bind (int, struct sockaddr *, int );
void cleanup_sockets ( void ) ;
int connect (int, struct sockaddr *, int);
int dn_expand ( u_char *, u_char *, u_char *, u_char *, int );
int dn_comp ( u_char *, u_char *, int, u_char **, u_char ** );
int __dn_skipname ( u_char *, u_char * );
void endhostent ( void );
void endnetent ( void );
void endprotoent ( void );
void endpwent( void );
void endservent ( void );
int getdomainname (char *, int);
gid_t getgid (void);
int getgroups (int, gid_t *);
int get_h_errno ( void );
struct hostent *gethostbyaddr ( char *, int, int );
struct hostent *gethostbyname ( char * );
struct hostent *gethostent ( void );
int gethostname (char *, int);
char *getlogin (void);
u_long getlong ( u_char * );
struct netent *getnetbyaddr ( long, int );
struct netent *getnetbyname ( char * );
struct netent *getnetent ( void );
int getpeername ( int, struct sockaddr *, int * );
struct protoent *getprotobyname ( char * );
struct protoent *getprotobynumber ( int );
struct protoent *getprotoent ( void );
struct passwd *getpwent( void ) ;
struct passwd *getpwnam( char * );
struct passwd *getpwuid( uid_t );
struct state *get_res ( void );
struct servent *getservent ( void );
struct servent *getservbyname ( char *, char * );
struct servent *getservbyport ( u_short, char * );
u_short _getshort ( u_char * );
int getsockname ( int, struct sockaddr *, int * );
int getsockopt ( int, int, int, char *, int * );
uid_t getuid (void);
mode_t getumask (void);
short get_tz(void);
u_long inet_addr ( char * );
int inet_lnaof ( struct in_addr );
struct in_addr inet_makeaddr ( int, int );
int inet_netof ( struct in_addr );
int inet_network ( char * );
char *inet_ntoa ( struct in_addr );
int listen (int , int);
void __putshort ( u_short, u_char * );
void __putlong ( u_long, u_char * );
int rcmd( char **, u_short, char *, char *, char *, int *);
int recv(int, char *, int, int );
int recvfrom( int, char *, int, int, struct sockaddr *, int *);
int recvmsg(int, struct msghdr *, int );
int res_init ( void );
int res_mkquery ( int, char *, int, int, char *, int, struct rrec *, char *, int );
int res_query ( char *, int, int, u_char *, int );
int res_querydomain ( char *, char *, int, int, u_char *, int );
int res_send ( char *, int, char *, int );
int res_search ( char *, int, int, u_char *, int );
int s_close ( int ) ;
void s_dev_list(u_long, int);
BYTE s_getsignal ( UWORD );
int s_inherit( void * );
int s_ioctl ( int, int, char * );
void *s_release( int );
void s_dev_list(u_long, int);
int select( int, fd_set *, fd_set *, fd_set *, struct timeval *);
int selectwait (int, fd_set *, fd_set *, fd_set *, struct timeval *, long *);
int send (int, char *, int, int );
int sendto (int, char *, int, int, struct sockaddr *, int );
int sendmsg (int, struct msghdr *, int );
int setgid ( gid_t );
int set_h_errno ( int );
void sethostent ( int );
void setnetent ( int );
void setprotoent ( int );
void setpwent( int );
void setservent ( int );
int  setuid ( uid_t );
int setsockopt ( int, int, int, char *, int );
ULONG setup_sockets ( UWORD, int * );
int shutdown (int, int);
int socket( int, int, int );
int s_syslog(int, char *);
int syslogA(int, char *, int *);
int syslog(int, char *, ...);
char *strerror( int );
mode_t umask ( mode_t );
int reconfig(void) ;
void _socket( caddr_t );
void _bind( caddr_t );
void _ioctl( caddr_t );
void _listen( caddr_t );
void _accept( caddr_t );
void _connect( caddr_t );
void _sendto( caddr_t );
void _send( caddr_t );
void _sendmsg( caddr_t );
void _recvfrom( caddr_t );
void _recv( caddr_t );
void _recvmsg( caddr_t );
void _shutdown( caddr_t );
void _setsockopt( caddr_t );
void _getsockopt( caddr_t );
void _getsockname( caddr_t );
void _getpeername( caddr_t );
void _select( caddr_t );
void _networkclose( caddr_t );
void _inherit( caddr_t );
void ConfigureInetA( struct TagItem * );
void ConfigureInet( ULONG, ... );
#pragma libcall SockBase setup_sockets 1e 8102
#pragma libcall SockBase cleanup_sockets 24 0
#pragma libcall SockBase socket 2a 21003
#pragma libcall SockBase s_close 30 001
#pragma libcall SockBase s_getsignal 36 101
#pragma libcall SockBase strerror 3c 101
#pragma libcall SockBase getuid 42 0
#pragma libcall SockBase getgid 48 0
#pragma libcall SockBase getgroups 4e 8002
#pragma libcall SockBase getlogin 54 0
#pragma libcall SockBase get_tz 5a 0
#pragma libcall SockBase getdomainname 60 1902
#pragma libcall SockBase getumask 66 0
#pragma libcall SockBase umask 6c 001
#pragma libcall SockBase gethostname 72 0802
#pragma libcall SockBase sethostent 78 101
#pragma libcall SockBase endhostent 7e 0
#pragma libcall SockBase gethostent 84 0
#pragma libcall SockBase gethostbyname 8a 801
#pragma libcall SockBase gethostbyaddr 90 10803
#pragma libcall SockBase inet_addr 96 901
#pragma libcall SockBase inet_makeaddr 9c 1002
#pragma libcall SockBase inet_lnaof a2 101
#pragma libcall SockBase inet_netof a8 101
#pragma libcall SockBase inet_network ae 901
#pragma libcall SockBase inet_ntoa b4 101
#pragma libcall SockBase accept ba 98003
#pragma libcall SockBase bind c0 19003
#pragma libcall SockBase connect c6 19003
#pragma libcall SockBase s_ioctl cc 81003
#pragma libcall SockBase listen d2 1002
#pragma libcall SockBase recv d8 218004
#pragma libcall SockBase recvfrom de A9218006
#pragma libcall SockBase recvmsg e4 18003
#pragma libcall SockBase select ea 1A98005
#pragma libcall SockBase selectwait f0 21A98006
#pragma libcall SockBase send f6 918004
#pragma libcall SockBase sendto fc 39218006
#pragma libcall SockBase sendmsg 102 18003
#pragma libcall SockBase shutdown 108 1002
#pragma libcall SockBase setsockopt 10e 3821005
#pragma libcall SockBase getsockopt 114 9821005
#pragma libcall SockBase setnetent 11a 101
#pragma libcall SockBase endnetent 120 0
#pragma libcall SockBase getnetent 126 0
#pragma libcall SockBase getnetbyaddr 12c 1002
#pragma libcall SockBase getnetbyname 132 801
#pragma libcall SockBase setprotoent 138 101
#pragma libcall SockBase endprotoent 13e 0
#pragma libcall SockBase getprotoent 144 0
#pragma libcall SockBase getprotobyname 14a 801
#pragma libcall SockBase getprotobynumber 150 001
#pragma libcall SockBase setservent 156 101
#pragma libcall SockBase endservent 15c 0
#pragma libcall SockBase getservent 162 0
#pragma libcall SockBase getservbyname 168 9802
#pragma libcall SockBase getservbyport 16e 8002
#pragma libcall SockBase getpwuid 174 101
#pragma libcall SockBase getpwnam 17a 801
#pragma libcall SockBase getpwent 180 0
#pragma libcall SockBase setpwent 186 101
#pragma libcall SockBase endpwent 18c 0
#pragma libcall SockBase rcmd 192 2A981006
#pragma libcall SockBase getpeername 198 98003
#pragma libcall SockBase getsockname 19e 98003
#pragma libcall SockBase s_syslog 1a4 8002
#pragma libcall SockBase reconfig 1aa 0
#pragma libcall SockBase s_release 1b0 101
#pragma libcall SockBase s_inherit 1b6 101
#pragma libcall SockBase s_dev_list 1bc 1002
#pragma libcall SockBase syslogA 1c2 98003
#pragma libcall SockBase vsyslog 1c2 98003
#pragma tagcall SockBase syslog 1c2 98003
#pragma libcall SockBase setgid 1c8 001
#pragma libcall SockBase setuid 1ce 001
#pragma libcall SockBase dn_expand 1d4 0BA9805
#pragma libcall SockBase dn_comp 1da BA09805
#pragma libcall SockBase __dn_skipname 1e0 9802
#pragma libcall SockBase _getshort 1e6 801
#pragma libcall SockBase getlong 1ec 801
#pragma libcall SockBase __putshort 1f2 8002
#pragma libcall SockBase __putlong 1f8 8002
#pragma libcall SockBase res_send 1fe 190804
#pragma libcall SockBase res_init 204 0
#pragma libcall SockBase res_query 20a 2910805
#pragma libcall SockBase res_search 210 2910805
#pragma libcall SockBase res_querydomain 216 2A109806
#pragma libcall SockBase res_mkquery 21c 4BA39218009
#pragma libcall SockBase get_h_errno 222 0
#pragma libcall SockBase set_h_errno 228 001
#pragma libcall SockBase get_res 22e 0
#pragma libcall SockBase _socket 234 101
#pragma libcall SockBase _bind 23a 101
#pragma libcall SockBase _ioctl 240 101
#pragma libcall SockBase _listen 246 101
#pragma libcall SockBase _accept 24c 101
#pragma libcall SockBase _connect 252 101
#pragma libcall SockBase _sendto  258 101
#pragma libcall SockBase _send 25e 101
#pragma libcall SockBase _sendmsg 264 101
#pragma libcall SockBase _recvfrom 26a 101
#pragma libcall SockBase _recv 270 101
#pragma libcall SockBase _recvmsg 276 101
#pragma libcall SockBase _shutdown 27c 101
#pragma libcall SockBase _setsockopt 282 101
#pragma libcall SockBase _getsockopt 288 101
#pragma libcall SockBase _getsockname 28e 101
#pragma libcall SockBase _getpeername 294 101
#pragma libcall SockBase _select 29a 101
#pragma libcall SockBase _networkclose 2a0 101
#pragma libcall SockBase _inherit 2a6 101
#pragma libcall SockBase ConfigureInetA 2ac 801
#pragma tagcall SockBase ConfigureInet 2ac 801
#pragma libcall SockBase s_dup 2b2 001
#pragma libcall SockBase s_dup2 2b8 1002
/*
 * Copyright (c) 1980, 1983, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)netdb.h	5.10 (Berkeley) 6/27/88
 */

/*
 * Structures returned by network
 * data base library.  All addresses
 * are supplied in host order, and
 * returned in network order (suitable
 * for use in system calls).
 */
struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	**h_addr_list;	/* list of addresses from name server */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent {
	char		*n_name;	/* official name of net */
	char		**n_aliases;	/* alias list */
	int		n_addrtype;	/* net address type */
	unsigned long	n_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};

struct rpcent {
	char	*r_name;	/* name of server for this rpc program */
	char	**r_aliases;	/* alias list */
	long	r_number;	/* rpc program number */
};

struct rpcent *getrpcbyname(), *getrpcbynumber(), *getrpcent();

/*
struct hostent	*gethostbyname(), *gethostbyaddr(), *gethostent();
struct netent	*getnetbyname(), *getnetbyaddr(), *getnetent();
struct servent	*getservbyname(), *getservbyport(), *getservent();
struct protoent	*getprotobyname(), *getprotobynumber(), *getprotoent();
*/


/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

/*

Defaults

  INCLUDE FILES FOR TCP

 */
/*

  MACROS FOR MANIPULATING MASKS FOR SELECT()

 */
/*      Display a message and get the input
**
**      On entry,
**              Msg is the message.
**
**      On exit,
**              Return value is malloc'd string which must be freed.
*/
extern char * HTPrompt ( char * Msg,  char * deflt);
extern char * HTPromptPassword ( char * Msg);


/*      Display a message, don't wait for input
**
**      On entry,
**              The input is a list of parameters for printf.
*/
extern void HTAlert ( char * Msg);


/*      Display a progress message for information (and diagnostics) only
**
**      On entry,
**              The input is a list of parameters for printf.
*/
extern void HTProgress ( char * Msg);
extern int HTCheckActiveIcon (int twirl);
extern void HTClearActiveIcon (void);


/*      Display a message, then wait for 'yes' or 'no'.
**
**      On entry,
**              Takes a list of parameters for printf.
**
**      On exit,
**              If the user enters 'YES', returns TRUE, returns FALSE
**              otherwise.
*/
extern BOOLEAN HTConfirm ( char * Msg);
/*

    */

/*                                            Utilities for the Authorization parts of libwww
             COMMON PARTS OF AUTHORIZATION MODULE TO BOTH SERVER AND BROWSER
                                             
   This module is the interface to the common parts of Access Authorization (AA) package
   for both server and browser. Important to know about memory allocation:
   
   Routines in this module use dynamic allocation, but free automatically all the memory
   reserved by them.
   
   Therefore the caller never has to (and never should) free() any object returned by
   these functions.
   
   Therefore also all the strings returned by this package are only valid until the next
   call to the same function is made. This approach is selected, because of the nature of
   access authorization: no string returned by the package needs to be valid longer than
   until the next call.
   
   This also makes it easy to plug the AA package in: you don't have to ponder whether to
   free() something here or is it done somewhere else (because it is always done somewhere
   else).
   
   The strings that the package needs to store are copied so the original strings given as
   parameters to AA functions may be freed or modified with no side effects.
   
   Also note: The AA package does not free() anything else than what it has itself
   allocated.
   
 */

/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */

/*  */

/*              List object
**
**      The list object is a generic container for storing collections
**      of things in order.
*/
/*

    */

/*		  System dependencies in the W3 library
				   SYSTEM DEPENDENCIES

   System-system differences for TCP include files and macros. This
   file includes for each system the files necessary for network and
   file I/O.

  AUTHORS

  TBL		     Tim Berners-Lee, W3 project, CERN, <timbl@info.cern.ch>
  EvA			  Eelco van Asperen <evas@cs.few.eur.nl>
  MA			  Marc Andreessen NCSA
  AT			  Aleksandar Totic <atotic@ncsa.uiuc.edu>
  SCW			  Susan C. Weber <sweber@kyle.eitech.com>
  MJW			  Michael J Witbrock <witbrock@cmu.edu>
  MWM			  Mike Meyer  <mwm@contessa.phone.net>

  HISTORY:
  22 Feb 91		  Written (TBL) as part of the WWW library.
  16 Jan 92		  PC code from EvA
  22 Apr 93		  Merged diffs bits from xmosaic release
  29 Apr 93		  Windows/NT code from SCW

  Much of the cross-system portability stuff has been intentionally
  REMOVED from this version of the library by Marc A in order to
  discourage attempts to make "easy" ports of Mosaic for X to non-Unix
  platforms.  The library needs to be rewritten from the ground up; in
  the meantime, Unix is *all* we support or intend to support with
  this set of source code.

*/

/*

Default filenames

 */
/*
** Numeric constants
*/
                                        /* (encrypted, so really only 3*8)*/
                                        /* protection setup file          */
                                        /* patname to allow               */

/*
** Helpful macros
*/
/*

Datatype definitions

  HTAASCHEME
  
   The enumeration HTAAScheme represents the possible authentication schemes used by the
   WWW Access Authorization.
   
 */

typedef enum {
    HTAA_UNKNOWN,
    HTAA_NONE,
    HTAA_BASIC,
    HTAA_PUBKEY,
    HTAA_KERBEROS_V4,
    HTAA_KERBEROS_V5,
    HTAA_MAX_SCHEMES /* THIS MUST ALWAYS BE LAST! Number of schemes */
} HTAAScheme;

/*

  ENUMERATION TO REPRESENT HTTP METHODS
  
 */

typedef enum {
    METHOD_UNKNOWN,
    METHOD_GET,
    METHOD_PUT
} HTAAMethod;

/*

Authentication Schemes

 */

/* PUBLIC                                               HTAAScheme_enum()
**              TRANSLATE SCHEME NAME TO A SCHEME ENUMERATION
** ON ENTRY:
**      name            is a string representing the scheme name.
**
** ON EXIT:
**      returns         the enumerated constant for that scheme.
*/
 HTAAScheme HTAAScheme_enum ( char* name);


/* PUBLIC                                               HTAAScheme_name()
**                      GET THE NAME OF A GIVEN SCHEME
** ON ENTRY:
**      scheme          is one of the scheme enum values:
**                      HTAA_NONE, HTAA_BASIC, HTAA_PUBKEY, ...
**
** ON EXIT:
**      returns         the name of the scheme, i.e.
**                      "none", "basic", "pubkey", ...
*/
 char *HTAAScheme_name (HTAAScheme scheme);

/*

Methods

 */

/* PUBLIC                                                   HTAAMethod_enum()
**              TRANSLATE METHOD NAME INTO AN ENUMERATED VALUE
** ON ENTRY:
**      name            is the method name to translate.
**
** ON EXIT:
**      returns         HTAAMethod enumerated value corresponding
**                      to the given name.
*/
 HTAAMethod HTAAMethod_enum ( char * name);


/* PUBLIC                                               HTAAMethod_name()
**                      GET THE NAME OF A GIVEN METHOD
** ON ENTRY:
**      method          is one of the method enum values:
**                      METHOD_GET, METHOD_PUT, ...
**
** ON EXIT:
**      returns         the name of the scheme, i.e.
**                      "GET", "PUT", ...
*/
 char *HTAAMethod_name (HTAAMethod method);


/* PUBLIC                                               HTAAMethod_inList()
**              IS A METHOD IN A LIST OF METHOD NAMES
** ON ENTRY:
**      method          is the method to look for.
**      list            is a list of method names.
**
** ON EXIT:
**      returns         YES, if method was found.
**                      NO, if not found.
*/
 BOOLEAN HTAAMethod_inList 
(HTAAMethod method, HTList * list);
/*

Match Template Against Filename

 */

/* PUBLIC                                               HTAA_templateMatch()
**              STRING COMPARISON FUNCTION FOR FILE NAMES
**                 WITH ONE WILDCARD * IN THE TEMPLATE
** NOTE:
**      This is essentially the same code as in HTRules.c, but it
**      cannot be used because it is embedded in between other code.
**      (In fact, HTRules.c should use this routine, but then this
**       routine would have to be more sophisticated... why is life
**       sometimes so hard...)
**
** ON ENTRY:
**      template        is a template string to match the file name
**                      agaist, may contain a single wildcard
**                      character * which matches zero or more
**                      arbitrary characters.
**      filename        is the filename (or pathname) to be matched
**                      agaist the template.
**
** ON EXIT:
**      returns         YES, if filename matches the template.
**                      NO, otherwise.
*/
 BOOLEAN HTAA_templateMatch 
( char * template,  char * filename);


/* PUBLIC                                       HTAA_makeProtectionTemplate()
**              CREATE A PROTECTION TEMPLATE FOR THE FILES
**              IN THE SAME DIRECTORY AS THE GIVEN FILE
**              (Used by server if there is no fancier way for
**              it to tell the client, and by browser if server
**              didn't send WWW-ProtectionTemplate: field)
** ON ENTRY:
**      docname is the document pathname (from URL).
**
** ON EXIT:
**      returns a template matching docname, and other files
**              files in that directory.
**
**              E.g.  /foo/bar/x.html  =>  /foo/bar/ *
**                                                  ^
**                              Space only to prevent it from
**                              being a comment marker here,
**                              there really isn't any space.
*/
 char *HTAA_makeProtectionTemplate ( char * docname);
/*

MIME Argument List Parser

 */


/* PUBLIC                                               HTAA_parseArgList()
**              PARSE AN ARGUMENT LIST GIVEN IN A HEADER FIELD
** ON ENTRY:
**      str     is a comma-separated list:
**
**                      item, item, item
**              where
**                      item ::= value
**                             | name=value
**                             | name="value"
**
**              Leading and trailing whitespace is ignored
**              everywhere except inside quotes, so the following
**              examples are equal:
**
**                      name=value,foo=bar
**                       name="value",foo="bar"
**                        name = value ,  foo = bar
**                         name = "value" ,  foo = "bar"
**
** ON EXIT:
**      returns a list of name-value pairs (actually HTAssocList*).
**              For items with no name, just value, the name is
**              the number of order number of that item. E.g.
**              "1" for the first, etc.
*/
 HTList *HTAA_parseArgList (char * str);

/*

Header Line Reader

 */

/* PUBLIC                                               HTAA_setupReader()
**              SET UP HEADER LINE READER, i.e. give
**              the already-read-but-not-yet-processed
**              buffer of text to be read before more
**              is read from the socket.
** ON ENTRY:
**      start_of_headers is a pointer to a buffer containing
**                      the beginning of the header lines
**                      (rest will be read from a socket).
**      length          is the number of valid characters in
**                      'start_of_headers' buffer.
**      soc             is the socket to use when start_of_headers
**                      buffer is used up.
** ON EXIT:
**      returns         nothing.
**                      Subsequent calls to HTAA_getUnfoldedLine()
**                      will use this buffer first and then
**                      proceed to read from socket.
*/
 void HTAA_setupReader 
(char * start_of_headers, int length, int soc);


/* PUBLIC                                               HTAA_getUnfoldedLine()
**              READ AN UNFOLDED HEADER LINE FROM SOCKET
** ON ENTRY:
**      HTAA_setupReader must absolutely be called before
**      this function to set up internal buffer.
**
** ON EXIT:
**      returns a newly-allocated character string representing
**              the read line.  The line is unfolded, i.e.
**              lines that begin with whitespace are appended
**              to current line.  E.g.
**
**                      Field-Name: Blaa-Blaa
**                       This-Is-A-Continuation-Line
**                       Here-Is_Another
**
**              is seen by the caller as:
**
**      Field-Name: Blaa-Blaa This-Is-A-Continuation-Line Here-Is_Another
**
*/
 char *HTAA_getUnfoldedLine (void);

/*

   End of file HTAAUtil.h. */

/*                       ASSOCIATION LIST FOR STORING NAME-VALUE PAIRS
                                             
   Lookups from assosiation list are not case-sensitive.
   
 */

/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */

/*  */

/*              List object
**
**      The list object is a generic container for storing collections
**      of things in order.
*/
/*

    */



typedef HTList HTAssocList;

typedef struct {
    char * name;
    char * value;
} HTAssoc;


 HTAssocList *HTAssocList_new (void);
 void HTAssocList_delete (HTAssocList * alist);

 void HTAssocList_add 
(HTAssocList * alist,  char * name,  char * value);

 char *HTAssocList_lookup 
(HTAssocList * alist,  char * name);

/*

   End of file HTAssoc.h.  */

/*                          BROWSER SIDE ACCESS AUTHORIZATION MODULE
                                             
   This module is the browser side interface to Access Authorization (AA) package.  It
   contains code only for browser.
   
   Important to know about memory allocation:
   
   Routines in this module use dynamic allocation, but free automatically all the memory
   reserved by them.
   
   Therefore the caller never has to (and never should) free() any object returned by
   these functions.
   
   Therefore also all the strings returned by this package are only valid until the next
   call to the same function is made. This approach is selected, because of the nature of
   access authorization: no string returned by the package needs to be valid longer than
   until the next call.
   
   This also makes it easy to plug the AA package in: you don't have to ponder whether to
   free()something here or is it done somewhere else (because it is always done somewhere
   else).
   
   The strings that the package needs to store are copied so the original strings given as
   parameters to AA functions may be freed or modified with no side effects.
   
   Also note:The AA package does not free() anything else than what it has itself
   allocated.
   
 */

/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */

/*                                            Utilities for the Authorization parts of libwww
             COMMON PARTS OF AUTHORIZATION MODULE TO BOTH SERVER AND BROWSER
                                             
   This module is the interface to the common parts of Access Authorization (AA) package
   for both server and browser. Important to know about memory allocation:
   
   Routines in this module use dynamic allocation, but free automatically all the memory
   reserved by them.
   
   Therefore the caller never has to (and never should) free() any object returned by
   these functions.
   
   Therefore also all the strings returned by this package are only valid until the next
   call to the same function is made. This approach is selected, because of the nature of
   access authorization: no string returned by the package needs to be valid longer than
   until the next call.
   
   This also makes it easy to plug the AA package in: you don't have to ponder whether to
   free() something here or is it done somewhere else (because it is always done somewhere
   else).
   
   The strings that the package needs to store are copied so the original strings given as
   parameters to AA functions may be freed or modified with no side effects.
   
   Also note: The AA package does not free() anything else than what it has itself
   allocated.
   
 */

/*

   End of file HTAAUtil.h. */



/*

Routines for Browser Side Recording of AA Info

   Most of the browser-side AA is done by the following two functions (which are called
   from file HTTP.c so the browsers using libwww only need to be linked with the new
   library and not be changed at all):
   
      HTAA_composeAuth() composes the Authorization: line contents, if the AA package
      thinks that the given document is protected. Otherwise this function returns NULL.
      This function also calls the functions HTPrompt(),HTPromptPassword() and HTConfirm()
      to get the username, password and some confirmation from the user.
      
      HTAA_shouldRetryWithAuth() determines whether to retry the request with AA or with a
      new AA (in case username or password was misspelled).

      HTAA_TryWithAuth() sets up everything for an automatic first try with authentication.

      HTAA_ClearAuth() clears the currently allocated authentication record.
      
 */

/* PUBLIC                                               HTAA_composeAuth()
**
**      COMPOSE THE ENTIRE AUTHORIZATION HEADER LINE IF WE
**      ALREADY KNOW, THAT THE HOST MIGHT REQUIRE AUTHORIZATION
**
** ON ENTRY:
**      hostname        is the hostname of the server.
**      portnumber      is the portnumber in which the server runs.
**      docname         is the pathname of the document (as in URL)
**
** ON EXIT:
**      returns NULL, if no authorization seems to be needed, or
**              if it is the entire Authorization: line, e.g.
**
**                 "Authorization: basic username:password"
**
**              As usual, this string is automatically freed.
*/
 char *HTAA_composeAuth 
( char * hostname,  int portnumber,  char * docname);


/* BROWSER PUBLIC                               HTAA_shouldRetryWithAuth()
**
**              DETERMINES IF WE SHOULD RETRY THE SERVER
**              WITH AUTHORIZATION
**              (OR IF ALREADY RETRIED, WITH A DIFFERENT
**              USERNAME AND/OR PASSWORD (IF MISSPELLED))
** ON ENTRY:
**      start_of_headers is the first block already read from socket,
**                      but status line skipped; i.e. points to the
**                      start of the header section.
**      length          is the remaining length of the first block.
**      soc             is the socket to read the rest of server reply.
**
**                      This function should only be called when
**                      server has replied with a 401 (Unauthorized)
**                      status code.
** ON EXIT:
**      returns         YES, if connection should be retried.
**                           The node containing all the necessary
**                           information is
**                              * either constructed if it does not exist
**                              * or password is reset to NULL to indicate
**                                that username and password should be
**                                reprompted when composing Authorization:
**                                field (in function HTAA_composeAuth()).
**                      NO, otherwise.
*/
 BOOLEAN HTAA_shouldRetryWithAuth 
(char * start_of_headers, int length, int soc);


/*

   End of file HTAABrow.h.  */

/*                              ENCODING TO PRINTABLE CHARACTERS
                                             
   File module provides functions HTUU_encode() and HTUU_decode() which convert a buffer
   of bytes to/from RFC 1113 printable encoding format. This technique is similar to the
   familiar Unix uuencode format in that it maps 6 binary bits to one ASCII character (or
   more aptly, 3 binary bytes to 4 ASCII characters).  However, RFC 1113 does not use the
   same mapping to printable characters as uuencode.
   
 */

/*                                                    Utitlity macros for the W3 code library
                                  MACROS FOR GENERAL USE
                                             
   Generates: HTUtils.h
   
   See also: the system dependent file "tcp.h"
   
 */

/*

   end of utilities  */


 int HTUU_encode 
(unsigned char *bufin, unsigned int nbytes, char *bufcoded);

 int HTUU_decode 
(char *bufcoded, unsigned char *bufplain, int outbufsize);

/*

   End of file.  */




/*
** Local datatype definitions
**
** HTAAServer contains all the information about one server.
*/
typedef struct {

    char *	hostname;	/* Host's name			*/
    int		portnumber;	/* Port number			*/
    HTList *	setups;		/* List of protection setups	*/
                                /* on this server; i.e. valid	*/
                                /* authentication schemes and	*/
                                /* templates when to use them.	*/
                                /* This is actually a list of	*/
                                /* HTAASetup objects.		*/
    HTList *	realms;		/* Information about passwords	*/
} HTAAServer;


/*
** HTAASetup contains information about one server's one
** protected tree of documents.
*/
typedef struct {
    HTAAServer *server;		/* Which server serves this tree	     */
    char *	template;	/* Template for this tree		     */
    HTList *	valid_schemes;	/* Valid authentic.schemes   		     */
    HTAssocList**scheme_specifics;/* Scheme specific params		     */
    BOOLEAN	retry;		/* Failed last time -- reprompt (or whatever)*/
} HTAASetup;


/*
** Information about usernames and passwords in
** Basic and Pubkey authentication schemes;
*/
typedef struct {
    char *	realmname;	/* Password domain name		*/
    char *	username;	/* Username in that domain	*/
    char *	password;	/* Corresponding password	*/
} HTAARealm;



/*
** Module-wide global variables
*/

static HTList *server_table	= 0L;	/* Browser's info about servers	     */
static char *secret_key	= 0L;	/* Browser's latest secret key       */
static HTAASetup *current_setup= 0L;	/* The server setup we are currently */
                                        /* talking to			     */
static char *current_hostname	= 0L;	/* The server's name and portnumber  */
static int current_portnumber	= 80;	/* where we are currently trying to  */
                                        /* connect.			     */
static char *current_docname	= 0L;	/* The document's name we are        */
                                        /* trying to access.		     */

/**************************** HTAAServer ***********************************/


/* PRIVATE						HTAAServer_new()
**		ALLOCATE A NEW NODE TO HOLD SERVER INFO
**		AND ADD IT TO THE LIST OF SERVERS
** ON ENTRY:
**	hostname	is the name of the host that the server
**			is running in.
**	portnumber	is the portnumber which the server listens.
**
** ON EXIT:
**	returns		the newly-allocated node with all the strings
**			duplicated.
**			Strings will be automatically freed by
**			the function HTAAServer_delete(), which also
**			frees the node itself.
*/
static HTAAServer *HTAAServer_new 
( char* hostname, int portnumber)
{
    HTAAServer *server;

    if (!(server = (HTAAServer *)malloc(sizeof(HTAAServer))))
	{ fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "HTAAServer_new"); exit(1);};

    server->hostname	= 0L;
    server->portnumber	= (portnumber > 0 ? portnumber : 80);
    server->setups	= HTList_new();
    server->realms	= HTList_new();

    if (hostname) HTSACopy (&(server->hostname), hostname);

    if (!server_table) server_table = HTList_new();
    
    HTList_addObject(server_table, (void*)server);

    return server;
}


/* PRIVATE						HTAAServer_delete()
**
**	DELETE THE ENTRY FOR THE SERVER FROM THE HOST TABLE,
**	AND FREE THE MEMORY USED BY IT.
**
** ON ENTRY:
**	killme		points to the HTAAServer to be freed.
**
** ON EXIT:
**	returns		nothing.
*/
/* PRIVATE						HTAAServer_lookup()
**		LOOK UP SERVER BY HOSTNAME AND PORTNUMBER
** ON ENTRY:
**	hostname	obvious.
**	portnumber	if non-positive defaults to 80.
**
**	Looks up the server in the module-global server_table.
**
** ON EXIT:
**	returns		pointer to a HTAAServer structure
**			representing the looked-up server.
**			NULL, if not found.
*/
static HTAAServer *HTAAServer_lookup 
( char * hostname, int portnumber)
{
    if (hostname) {
	HTList *cur = server_table;
	HTAAServer *server;

	if (portnumber <= 0) portnumber = 80;

	while (0L != (server = (HTAAServer*)(cur && (cur = cur ->next) ? cur ->object : 0L))) {
	    if (server->portnumber == portnumber  &&
		0== __builtin_strcmp(server->hostname,hostname))
		return server;
	}
    }
    return 0L;	/* NULL parameter, or not found */
}




/*************************** HTAASetup *******************************/    


/* PRIVATE						HTAASetup_lookup()
**	FIGURE OUT WHICH AUTHENTICATION SETUP THE SERVER
**	IS USING FOR A GIVEN FILE ON A GIVEN HOST AND PORT
**
** ON ENTRY:
**	hostname	is the name of the server host machine.
**	portnumber	is the port that the server is running in.
**	docname		is the (URL-)pathname of the document we
**			are trying to access.
**
** 	This function goes through the information known about
**	all the setups of the server, and finds out if the given
**	filename resides in one of the protected directories.
**
** ON EXIT:
**	returns		NULL if no match.
**			Otherwise, a HTAASetup structure representing
**			the protected server setup on the corresponding
**			document tree.
**			
*/
static HTAASetup *HTAASetup_lookup 
( char * hostname, int portnumber,  char * docname)
{
    HTAAServer *server;
    HTAASetup *setup;

    if (portnumber <= 0) portnumber = 80;

    if (hostname && docname && *hostname && *docname &&
	0L != (server = HTAAServer_lookup(hostname, portnumber))) {

	HTList *cur = server->setups;

	if ((WWW_TraceFlag)) fprintf((&__iob[2]), "%s (%s:%d:%s)\n",
			   "HTAASetup_lookup: resolving setup for",
			   hostname, portnumber, docname);

	while (0L != (setup = (HTAASetup*)(cur && (cur = cur ->next) ? cur ->object : 0L))) {
	    if (HTAA_templateMatch(setup->template, docname)) {
		if ((WWW_TraceFlag)) fprintf((&__iob[2]), "%s `%s' %s `%s'\n",
				   "HTAASetup_lookup:", docname,
				   "matched template", setup->template);
		return setup;
	    }
	    else if ((WWW_TraceFlag)) fprintf((&__iob[2]), "%s `%s' %s `%s'\n",
				    "HTAASetup_lookup:", docname,
				    "did NOT match template", setup->template);
	} /* while setups remain */
    } /* if valid parameters and server found */

    if ((WWW_TraceFlag)) fprintf((&__iob[2]), "%s `%s' %s\n",
		       "HTAASetup_lookup: No template matched",
		       (docname ? docname : "(null)"),
		       "(so probably not protected)");

    return 0L;	/* NULL in parameters, or not found */
}




/* PRIVATE						HTAASetup_new()
**			CREATE A NEW SETUP NODE
** ON ENTRY:
**	server		is a pointer to a HTAAServer structure
**			to which this setup belongs.
**	template	documents matching this template
**			are protected according to this setup.
**	valid_schemes	a list containing all valid authentication
**			schemes for this setup.
**			If NULL, all schemes are disallowed.
**	scheme_specifics is an array of assoc lists, which
**			contain scheme specific parameters given
**			by server in Authenticate: fields.
**			If NULL, all scheme specifics are
**			set to NULL.
** ON EXIT:
**	returns		a new HTAASetup node, and also adds it as
**			part of the HTAAServer given as parameter.
*/
static HTAASetup *HTAASetup_new 
(HTAAServer * server, char * template, HTList * valid_schemes, HTAssocList ** scheme_specifics)
{
    HTAASetup *setup;

    if (!server || !template || !*template) return 0L;

    if (!(setup = (HTAASetup*)malloc(sizeof(HTAASetup))))
	{ fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "HTAASetup_new"); exit(1);};

    setup->retry = (BOOLEAN)0;
    setup->server = server;
    setup->template = 0L;
    if (template) HTSACopy (&(setup->template), template);
    setup->valid_schemes = valid_schemes;
    setup->scheme_specifics = scheme_specifics;

    HTList_addObject(server->setups, (void*)setup);

    return setup;
}



/* PRIVATE						HTAASetup_delete()
**			FREE A HTAASetup STRUCTURE
** ON ENTRY:
**	killme		is a pointer to the structure to free().
**
** ON EXIT:
**	returns		nothing.
*/
/* PRIVATE					HTAASetup_updateSpecifics()
*		COPY SCHEME SPECIFIC PARAMETERS
**		TO HTAASetup STRUCTURE
** ON ENTRY:
**	setup		destination setup structure.
**	specifics	string array containing scheme
**			specific parameters for each scheme.
**			If NULL, all the scheme specific
**			parameters are set to NULL.
**
** ON EXIT:
**	returns		nothing.
*/
static void HTAASetup_updateSpecifics 
(HTAASetup * setup, HTAssocList ** specifics)
{
    int scheme;

    if (setup) {
	if (setup->scheme_specifics) {
	    for (scheme=0; scheme < HTAA_MAX_SCHEMES; scheme++) {
		if (setup->scheme_specifics[scheme])
		    HTAssocList_delete(setup->scheme_specifics[scheme]);
	    }
	    free(setup->scheme_specifics);
	}
	setup->scheme_specifics = specifics;
    }
}




/*************************** HTAARealm **********************************/

/* PRIVATE 						HTAARealm_lookup()
**		LOOKUP HTAARealm STRUCTURE BY REALM NAME
** ON ENTRY:
**	realm_table	a list of realm objects.
**	realmname	is the name of realm to look for.
**
** ON EXIT:
**	returns		the realm.  NULL, if not found.
*/
static HTAARealm *HTAARealm_lookup 
(HTList * realm_table,  char * realmname)
{
    if (realm_table && realmname) {
	HTList *cur = realm_table;
	HTAARealm *realm;
	
	while (0L != (realm = (HTAARealm*)(cur && (cur = cur ->next) ? cur ->object : 0L))) {
	    if (0== __builtin_strcmp(realm->realmname,realmname))
		return realm;
	}
    }
    return 0L;	/* No table, NULL param, or not found */
}



/* PRIVATE						HTAARealm_new()
**		CREATE A NODE CONTAINING USERNAME AND
**		PASSWORD USED FOR THE GIVEN REALM.
**		IF REALM ALREADY EXISTS, CHANGE
**		USERNAME/PASSWORD.
** ON ENTRY:
**	realm_table	a list of realms to where to add
**			the new one, too.
**	realmname	is the name of the password domain.
**	username	and
**	password	are what you can expect them to be.
**
** ON EXIT:
**	returns		the created realm.
*/
static HTAARealm *HTAARealm_new 
(HTList * realm_table,  char * realmname,  char * username,  char * password)
{
    HTAARealm *realm;

    realm = HTAARealm_lookup(realm_table, realmname);

    if (!realm) {
	if (!(realm = (HTAARealm*)malloc(sizeof(HTAARealm))))
	    { fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "HTAARealm_new"); exit(1);};
	realm->realmname = 0L;
	realm->username = 0L;
	realm->password = 0L;
	HTSACopy (&(realm->realmname), realmname);
	if (realm_table) HTList_addObject(realm_table, (void*)realm);
    }
    if (username) HTSACopy (&(realm->username), username);
    if (password) HTSACopy (&(realm->password), password);

    return realm;
}


/* BROWSER PRIVATE					HTAA_selectScheme()
**		SELECT THE AUTHENTICATION SCHEME TO USE
** ON ENTRY:
**	setup	is the server setup structure which can
**		be used to make the decision about the
**		used scheme.
**
**	When new authentication methods are added to library
**	this function makes the decision about which one to
**	use at a given time.  This can be done by inspecting
**	environment variables etc.
**
**	Currently only searches for the first valid scheme,
**	and if nothing found suggests Basic scheme;
**
** ON EXIT:
**	returns	the authentication scheme to use.
*/
static HTAAScheme HTAA_selectScheme (HTAASetup * setup)
{
    HTAAScheme scheme;

    if (setup && setup->valid_schemes) {
	for (scheme=HTAA_BASIC; scheme < HTAA_MAX_SCHEMES; scheme++)
	    if (-1 < HTList_indexOf(setup->valid_schemes, (void*)scheme))
		return scheme;
    }
    return HTAA_BASIC;
}


/***************** Basic and Pubkey Authentication ************************/

/* PRIVATE						compose_auth_string()
**
**		COMPOSE Basic OR Pubkey AUTHENTICATION STRING;
**		PROMPTS FOR USERNAME AND PASSWORD IF NEEDED
**
** ON ENTRY:
**	scheme		is either HTAA_BASIC or HTAA_PUBKEY.
**	realmname	is the password domain name.
**
** ON EXIT:
**	returns		a newly composed authorization string,
**			(with, of course, a newly generated secret
**			key and fresh timestamp, if Pubkey-scheme
**			is being used).
** NOTE:
**	Like throughout the entire AA package, no string or structure
**	returned by AA package needs to (or should) be freed.
**
*/
static char *compose_auth_string 
(HTAAScheme scheme, HTAASetup * setup)
{
    static char *result = 0L;	/* Uuencoded presentation, the result */
    char *cleartext = 0L;	/* Cleartext presentation */
    char *ciphertext = 0L;	/* Encrypted presentation */
    int len;
    char *username;
    char *password;
    char *realmname;
    HTAARealm *realm;
    char *inet_addr = "0.0.0.0";	/* Change... @@@@ */
    char *timestamp = "42";		/* ... these @@@@ */
    

    if (result) {free(result); result = 0L;};	/* From previous call */

    if ((scheme != HTAA_BASIC && scheme != HTAA_PUBKEY) || !setup ||
	!setup->scheme_specifics || !setup->scheme_specifics[scheme] ||
	!setup->server  ||  !setup->server->realms)
	return "";

    realmname = HTAssocList_lookup(setup->scheme_specifics[scheme], "realm");
    if (!realmname) return "";

    realm = HTAARealm_lookup(setup->server->realms, realmname);
    if (!realm || setup->retry) {
	char msg[2048];

	if (!realm) {
	    if ((WWW_TraceFlag)) fprintf((&__iob[2]), "%s `%s' %s\n",
			       "compose_auth_string: realm:", realmname,
			       "not found -- creating");
	    realm = HTAARealm_new(setup->server->realms, realmname, 0L,0L);
	    sprintf(msg,
		    "Document is protected.\nEnter username for %s at %s: ",
		    realm->realmname,
		    setup->server->hostname ? setup->server->hostname : "??");
	    realm->username =
		HTPrompt(msg, realm->username);
            /* Added by marca. */
            if (!realm->username)
              return "";
	}
	else {
	    sprintf(msg,"Enter username for %s at %s: ", realm->realmname,
		    setup->server->hostname ? setup->server->hostname : "??");
	    username = HTPrompt(msg, realm->username);
	    if (realm->username) {free(realm->username); realm->username = 0L;};
	    realm->username = username;
            /* Added by marca. */
            if (!realm->username)
              return "";
	}
	password = HTPromptPassword("Enter password to authenticate yourself: ");
	if (realm->password) {free(realm->password); realm->password = 0L;};
	realm->password = password;
        /* Added by marca. */
        if (!realm->password)
          return "";
    }
    
    len = __builtin_strlen(realm->username ? realm->username : "") +
	  __builtin_strlen(realm->password ? realm->password : "") + 3;

    if (scheme == HTAA_PUBKEY) {
	/* Room for secret key, timestamp and inet address */
	len += __builtin_strlen(secret_key ? secret_key : "") + 30;
    }
    else
	if (secret_key) {free(secret_key); secret_key = 0L;};

    if (!(cleartext  = (char*)calloc(len, 1)))
	{ fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "compose_auth_string"); exit(1);};

    if (realm->username) __builtin_strcpy(cleartext,realm->username);
    else *cleartext = (char)0;

    strcat(cleartext, ":");

    if (realm->password) strcat(cleartext, realm->password);

    if (scheme == HTAA_PUBKEY) {
	strcat(cleartext, ":");
	strcat(cleartext, inet_addr);
	strcat(cleartext, ":");
	strcat(cleartext, timestamp);
	strcat(cleartext, ":");
	if (secret_key) strcat(cleartext, secret_key);

	if (!((ciphertext = (char*)malloc(2*len)) &&
	      (result     = (char*)malloc(3*len))))
	    { fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "compose_auth_string"); exit(1);};
	free(cleartext);
	free(ciphertext);
    }
    else { /* scheme == HTAA_BASIC */
      /* Added "+ 1" marca. */
        /* Ari fix. */
	if (!(result = (char*)malloc(4 * ((len+2)/3) + 1)))
	    { fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "compose_auth_string"); exit(1);};
        /* Added cast to unsigned char * on advice of
           erik@sockdev.uni-c.dk (Erik Bertelsen). */
	HTUU_encode((unsigned char *)cleartext, __builtin_strlen(cleartext), result);
	free(cleartext);
    }
    if((WWW_TraceFlag)) fprintf((&__iob[2]),"sending auth line: %s\n",result);
    return result;
}




/* BROWSER PUBLIC					HTAA_composeAuth()
**
**	SELECT THE AUTHENTICATION SCHEME AND
**	COMPOSE THE ENTIRE AUTHORIZATION HEADER LINE
**	IF WE ALREADY KNOW THAT THE HOST REQUIRES AUTHENTICATION
**
** ON ENTRY:
**	hostname	is the hostname of the server.
**	portnumber	is the portnumber in which the server runs.
**	docname		is the pathname of the document (as in URL)
**
** ON EXIT:
**	returns	NULL, if no authorization seems to be needed, or
**		if it is the entire Authorization: line, e.g.
**
**		   "Authorization: Basic username:password"
**
**		As usual, this string is automatically freed.
*/
 char *HTAA_composeAuth 
( char * hostname,  int portnumber,  char * docname)
{
    static char *result = 0L;
    char *auth_string;
    BOOLEAN retry;
    HTAAScheme scheme;

    if (result) {free(result); result = 0L;};			/* From previous call */

    if ((WWW_TraceFlag))
	fprintf((&__iob[2]), 
		"Composing Authorization for %s:%d/%s\n",
		hostname, portnumber, docname);

    if (current_portnumber != portnumber ||
	!current_hostname || !current_docname ||
	!hostname         || !docname         ||
	0 != __builtin_strcmp(current_hostname,hostname) ||
	0 != __builtin_strcmp(current_docname,docname)) {

	retry = (BOOLEAN)0;

	current_portnumber = portnumber;
	
	if (hostname) HTSACopy (&(current_hostname), hostname);
	else if (current_hostname) {free(current_hostname); current_hostname = 0L;};

	if (docname) HTSACopy (&(current_docname), docname);
	else if (current_docname) {free(current_docname); current_docname = 0L;};
    }
    else retry = (BOOLEAN)1;
    
    if (!current_setup || !retry)
	current_setup = HTAASetup_lookup(hostname, portnumber, docname);

    if (!current_setup)
	return 0L;


    switch (scheme = HTAA_selectScheme(current_setup)) {
      case HTAA_BASIC:
      case HTAA_PUBKEY:
	auth_string = compose_auth_string(scheme, current_setup);
	break;
      case HTAA_KERBEROS_V4:
	/* OTHER AUTHENTICATION ROUTINES ARE CALLED HERE */
      default:
	{
	    char msg[2048];
	    sprintf(msg, "%s %s `%s'",
		    "This client doesn't know how to compose authentication",
		    "information for scheme", HTAAScheme_name(scheme));
	    HTAlert(msg);
	    auth_string = 0L;
	}
    } /* switch scheme */

    current_setup->retry = (BOOLEAN)0;

    /* Added by marca. */
    if (!auth_string)
      return 0L;

    if (!(result = (char*)malloc(sizeof(char) * (__builtin_strlen(auth_string)+40))))
	{ fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "HTAA_composeAuth"); exit(1);};
    __builtin_strcpy(result,"Authorization: ");
    strcat(result, HTAAScheme_name(scheme));
    strcat(result, " ");
    strcat(result, auth_string);
    return result;
}



	    
/* BROWSER PUBLIC				HTAA_shouldRetryWithAuth()
**
**		DETERMINES IF WE SHOULD RETRY THE SERVER
**		WITH AUTHORIZATION
**		(OR IF ALREADY RETRIED, WITH A DIFFERENT
**		USERNAME AND/OR PASSWORD (IF MISSPELLED))
** ON ENTRY:
**	start_of_headers is the first block already read from socket,
**			but status line skipped; i.e. points to the
**			start of the header section.
**	length		is the remaining length of the first block.
**	soc		is the socket to read the rest of server reply.
**
**			This function should only be called when
**			server has replied with a 401 (Unauthorized)
**			status code.
** ON EXIT:
**	returns		YES, if connection should be retried.
**			     The node containing all the necessary
**			     information is
**				* either constructed if it does not exist
**				* or password is reset to NULL to indicate
**				  that username and password should be
**				  reprompted when composing Authorization:
**				  field (in function HTAA_composeAuth()).
**			NO, otherwise.
*/
 BOOLEAN HTAA_shouldRetryWithAuth 
(char * start_of_headers, int length, int soc)
{
    HTAAScheme scheme;
    char *line;
    int num_schemes = 0;
    HTList *valid_schemes = HTList_new();
    HTAssocList **scheme_specifics = 0L;
    char *template = 0L;


    /* Read server reply header lines */

    if ((WWW_TraceFlag))
	fprintf((&__iob[2]), "Server reply header lines:\n");

    HTAA_setupReader(start_of_headers, length, soc);
    while (0L != (line = HTAA_getUnfoldedLine())  &&  *line != (char)0) {

	if ((WWW_TraceFlag)) fprintf((&__iob[2]), "%s\n", line);

	if (strchr(line, ':')) {	/* Valid header line */

	    char *p = line;
	    char *fieldname = HTNextField(&p);
	    char *arg1 = HTNextField(&p);
	    char *args = p;
	    
	    if (0==strcasecomp(fieldname, "WWW-Authenticate:")) {
		if (HTAA_UNKNOWN != (scheme = HTAAScheme_enum(arg1))) {
		    HTList_addObject(valid_schemes, (void*)scheme);
		    if (!scheme_specifics) {
			int i;
			scheme_specifics = (HTAssocList**)
			    malloc(HTAA_MAX_SCHEMES * sizeof(HTAssocList*));
			if (!scheme_specifics)
			    { fprintf((&__iob[2]), "%s %s: out of memory.\nProgram aborted.\n", "HTAABrow.c", "HTAA_shouldRetryWithAuth"); exit(1);};
			for (i=0; i < HTAA_MAX_SCHEMES; i++)
			    scheme_specifics[i] = 0L;
		    }
		    scheme_specifics[scheme] = HTAA_parseArgList(args);
		    num_schemes++;
		}
		else if ((WWW_TraceFlag)) {
		    fprintf((&__iob[2]), "Unknown scheme `%s' %s\n",
			    (arg1 ? arg1 : "(null)"),
			    "in WWW-Authenticate: field");
		}
	    }

	    else if (0==strcasecomp(fieldname, "WWW-Protection-Template:")) {
		if ((WWW_TraceFlag))
		    fprintf((&__iob[2]), "Protection template set to `%s'\n", arg1);
		HTSACopy (&(template), arg1);
	    }

	} /* if a valid header line */
	else if ((WWW_TraceFlag)) {
	    fprintf((&__iob[2]), "Invalid header line `%s' ignored\n", line);
	} /* else invalid header line */
    } /* while header lines remain */


    /* So should we retry with authorization */

    if (num_schemes == 0) {		/* No authentication valid */
	current_setup = 0L;
	return (BOOLEAN)0;
    }

    if (current_setup && current_setup->server) {
	/* So we have already tried with authorization.	*/
	/* Either we don't have access or username or	*/
	/* password was misspelled.			*/
	    
	/* Update scheme-specific parameters	*/
	/* (in case they have expired by chance).	*/
	HTAASetup_updateSpecifics(current_setup, scheme_specifics);

	if ((BOOLEAN)0 == HTConfirm("Authorization failed.  Retry?")) {
	    current_setup = 0L;
	    return (BOOLEAN)0;
	} /* HTConfirm(...) == NO */
	else { /* re-ask username+password (if misspelled) */
	    current_setup->retry = (BOOLEAN)1;
	    return (BOOLEAN)1;
	} /* HTConfirm(...) == YES */
    } /* if current_setup != NULL */

    else { /* current_setup == NULL, i.e. we have a	 */
	   /* first connection to a protected server or  */
	   /* the server serves a wider set of documents */
	   /* than we expected so far.                   */

	HTAAServer *server = HTAAServer_lookup(current_hostname,
					       current_portnumber);
	if (!server) {
	    server = HTAAServer_new(current_hostname,
				    current_portnumber);
	}
	if (!template)
	    template = HTAA_makeProtectionTemplate(current_docname);
	current_setup = HTAASetup_new(server, 
				      template,
				      valid_schemes,
				      scheme_specifics);

        HTAlert("Access without authorization denied -- retrying");
	return (BOOLEAN)1;
    } /* else current_setup == NULL */

    /* Never reached */
}



