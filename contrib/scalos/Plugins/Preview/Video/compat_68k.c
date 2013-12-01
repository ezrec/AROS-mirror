// Video.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <scalos/scalos.h>
#include <utility/hooks.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include <defs.h>

#undef	GLOBAL

#include "Video.h"

//---------------------------------------------------------------------------------------

static int infunc(const unsigned char **s);
static void unfunc(int c, const unsigned char **s);
static double mypow10(int n);
static int _scanfshared(int (*infunc)(void *inarg),void (*unfunc)(int c, void *inarg),void *inarg,const char *format,va_list args);

//---------------------------------------------------------------------------------------

int fputc(int c, FILE *stream)
{
	d1(KPrintF("%s/%s/%ld: stream=%08lx  c=%c\n", __FILE__, __FUNC__, __LINE__, stream, c));
	// Writing is not supported
	return -1;
}

//-----------------------------------------------------------------------------

int fseek(FILE *fh, long offset, int origin)
{
	LONG OldPos;
	LONG mode;

	d1(KPrintF("%s/%s/%ld:  fh=%08lx  offset=%ld  origin=%ld\n", \
		__FILE__, __FUNC__, __LINE__, fh, offset, origin));

	switch (origin)
		{
	case SEEK_SET:
		mode = OFFSET_BEGINNING;
		break;
	case SEEK_CUR:
		mode = OFFSET_CURRENT;
		break;
	case SEEK_END:
		mode = OFFSET_END;
		break;
	default:
		return -1;
		break;
		}

	errno = 0;

	OldPos = Seek((BPTR) fh, offset, mode);
	d1(kprintf("%s/%s/%ld: OldPos=%ld\n", __FILE__, __FUNC__, __LINE__, OldPos));

	return OldPos >= 0 ? 0 : -1;
}

//-----------------------------------------------------------------------------

int unlink(const char *FileName)
{
	// Deleting files is not supported
	d1(KPrintF("%s/%s/%ld: FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	errno = 0;

	return -1;
}

//-----------------------------------------------------------------------------

int fprintf(FILE *fh, const char *fmt, ...)
{
#if 1
	d1(KPrintF("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));
#else
	char buf[256];
	va_list	ap;

	d1(KPrintF("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));

	(void) fh;
	(void) fmt;

	va_start(ap, fmt);

	vsnprintf(buf, sizeof(buf), fmt, ap);
	d1(KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buf));

	va_end(ap);
#endif
	return 0;
}

//-----------------------------------------------------------------------------

int vfprintf(FILE *fh, const char *fmt, va_list ap)
{
#if 1
	d1(KPrintF("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));
#else
	char buf[256];

	d1(KPrintF("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));

	(void) fh;
	(void) fmt;
	(void) ap;

	vsnprintf(buf, sizeof(buf), fmt, ap);
	d1(KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buf));
#endif
	return 0;
}

//-----------------------------------------------------------------------------

/*
DESCRIPTION

    The mkstemp() function shall replace the contents of the string pointed to
    by template by a unique filename, and return a file descriptor
    for the file open for reading and writing.
    The function thus prevents any possible race condition between testing
    whether the file exists and opening it for use.
    The string in template should look like a filename with six trailing 'X' s;
    mkstemp() replaces each 'X' with a character from the portable filename
    character set. The characters are chosen such that the resulting name
    does not duplicate the name of an existing file
    at the time of a call to mkstemp().

RETURN VALUE

    Upon successful completion, mkstemp() shall return an open file descriptor.
    Otherwise, -1 shall be returned if no suitable file could be created.
*/

int mkstemp(char *template)
{
	return -1;
}

//-----------------------------------------------------------------------------

/*
    These functions shall round their argument to the nearest integer value,
    rounding according to the current rounding direction.

RETURN VALUE
    Upon successful completion, these functions shall return the rounded integer value.
    [MX] [Option Start] If x is NaN, a domain error shall occur, and an unspecified value is returned.
    If x is +Inf, a domain error shall occur and an unspecified value is returned.
    If x is -Inf, a domain error shall occur and an unspecified value is returned.
    If the correct value is positive and too large to represent as a long long, a domain error shall occur and an unspecified value is returned.
    If the correct value is negative and too large to represent as a long long, a domain error shall occur and an unspecified value is returned. [Option End]
*/

long long int llrint(double x)
{
#warning need some work here
	return (long long) x;
}

//-----------------------------------------------------------------------------

int puts(const char *s)
{
	d1(KPrintF("%s/%s/%ld: s=<%s>\n", __FILE__, __FUNC__, __LINE__, s));
	return 0;
}

//-----------------------------------------------------------------------------

int fputs(const char *s, FILE *fp)
{
	(void) fp;

	d1(KPrintF("%s/%s/%ld: s=<%s>\n", __FILE__, __FUNC__, __LINE__, s));
	return 0;
}

//-----------------------------------------------------------------------------

void perror(const char *s)
{
	d1(KPrintF("%s/%s/%ld: s=<%s>\n", __FILE__, __FUNC__, __LINE__, s));
}

//-----------------------------------------------------------------------------

/*
The memalign() function returns a block of memory of
size bytes aligned to blocksize. The blocksize must be given
as a power of two. It sets errno and returns a null pointer upon failure.

Pointers returned by memalign() may be passed to free().
Pointers passed to realloc() are checked and if not
aligned to the system, the realloc() fails and returns NULL.
*/
void *memalign(size_t blocksize, size_t bytes)
{
	d1(KPrintF("%s/%s/%ld: blocksize=%lu  bytes=%lu\n", __FILE__, __FUNC__, __LINE__, blocksize, bytes));
	(void) blocksize;

	return malloc(bytes);
}

//-----------------------------------------------------------------------------

int errno = 0;

//int stderr = 0;

//-----------------------------------------------------------------------------

int sprintf(char *s, const char *fmt, ...)
{
	d1(KPrintF("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));
	*s = 0;
	return 0;
}

//-----------------------------------------------------------------------------

int snprintf(char *s, size_t maxlen, const char *fmt, ...)
{
	d1(KPrintF("%s/%s/%ld: fmt=<%s>\n", __FILE__, __FUNC__, __LINE__, fmt));
	*s = 0;
	return 0;
}

//-----------------------------------------------------------------------------

int sscanf(const char *ss, const char *fmt, ...)
{
	int retval;
	va_list args;

	va_start(args, fmt);
	retval = vsscanf(ss, fmt, args);
	va_end(args);

	d1(KPrintF("%s/%s/%ld: ss=<%s>  fmt=<%s>  retval=%ld\n", __FILE__, __FUNC__, __LINE__, ss, fmt, retval));

	return retval;
}

static int infunc(const unsigned char **s)
{
	int c;

	c = (int)**s;

	if (c) (*s)++;
	else c = EOF;

	return c;
}

static void unfunc(int c, const unsigned char **s)
{
	(*s)--;
}

int vsscanf(const char *s,const char *format,va_list args)
{
	return _scanfshared((int (*)(void *))infunc, (void (*)(int, void *))unfunc, &s, format, args);
}

/* some macros to cut this short
 * NEXT(c);     read next character
 * PREV(c);     ungetc a character
 * VAL(a)       leads to 1 if a is true and valid
 */
#define NEXT(c) ((c)=infunc(inarg),size++,incount++)
#define PREV(c) do{if((c)!=EOF)unfunc((c),inarg);size--;incount--;}while(0)
#define VAL(a)  ((a)&&size<=width)

extern unsigned char *__decimalpoint;

static unsigned char undef[3][sizeof(double)]= /* Undefined numeric values, IEEE */
{ { 0x7f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00 }, /* +inf */
  { 0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00 }, /* -inf */
  { 0x7f,0xf1,0x00,0x00,0x00,0x00,0x00,0x00 }  /*  NaN */
};

static double mypow10(int n)
{
  int neg=0;
  double x=1.0;
  double y=10.0;
  if(n==0)
    return 1.0;
  if(n<0)
  { neg=1;
    n=-n; }
  while(n)
  { if(n&1)
      x*=y;
    y*=y;
    n>>=1; }
  if(neg)
    x=1/x;
  return x;
}

static int _scanfshared(int (*infunc)(void *inarg),void (*unfunc)(int c, void *inarg),void *inarg,const char *format,va_list args)
{
  size_t blocks=0,incount=0;
  int c=0;

  while(*format)
  {
    size_t size=0;

    if(*format=='%')
    {
      size_t width=ULONG_MAX;
      char type,subtype='i',ignore=0;
      const unsigned char *ptr=format+1;
      size_t i;

      if(isdigit(*ptr))
      { width=0;
	while(isdigit(*ptr))
	  width=width*10+(*ptr++-'0'); }

      while(*ptr=='h'||*ptr=='L'||*ptr=='*'||*ptr=='l')
      { if(*ptr=='*')
          ignore=1;
        else
        {
          if (*ptr=='l'&&subtype=='l') subtype='L';
          else subtype=*ptr;
        }
        ptr++;
      }

      type=*ptr++;

      if(type&&type!='%'&&type!='c'&&type!='n'&&type!='[')
      { do /* ignore leading whitespace characters */
	  NEXT(c);
	while(isspace(c));
	size=1; } /* The first non-whitespace character is already read */

      switch(type)
      { case 'c':
	{ unsigned char *bp;

	  if(width==ULONG_MAX) /* Default */
	    width=1;

	  if(!ignore)
	    bp=va_arg(args,char *);
	  else
	    bp=NULL; /* Just to get the compiler happy */

	  NEXT(c); /* 'c' did not skip whitespace */
	  while(VAL(c!=EOF))
	  { if(!ignore)
	      *bp++=c;
	    NEXT(c);
	  }
	  PREV(c);

	  if(!ignore&&size)
	    blocks++;
	  break;
	}
	case '[':
	{ unsigned char *bp;
	  unsigned char tab[32],a,b;
	  char circflag=0;

	  if(*ptr=='^')
	  { circflag=1;
	    ptr++; }
	  for(i=0;i<sizeof(tab);i++)
	    tab[i]=circflag?255:0;
	  for(;;)
	  { if(!*ptr)
	      break;
	    a=b=*ptr++;
	    if(*ptr=='-'&&ptr[1]&&ptr[1]!=']')
	    { ptr++;
	      b=*ptr++; }
	    for(i=a;i<=b;i++)
	      if(circflag)
		tab[i/8]&=~(1<<(i&7));
	      else
		tab[i/8]|=1<<(i&7);
	    if(*ptr==']')
	    { ptr++;
	      break; }
	  }

	  if(!ignore)
	    bp=va_arg(args,char *);
	  else
	    bp=NULL; /* Just to get the compiler happy */

	  NEXT(c);
	  while(VAL(c!=EOF&&tab[c/8]&(1<<(c&7))))
	  { if(!ignore)
	      *bp++=c;
	    NEXT(c);
	  }
	  PREV(c);

	  if(!ignore&&size)
	  { *bp++='\0';
	    blocks++; }
	  break;
	}
	case 's':
	{ unsigned char *bp;

	  if(!ignore)
	    bp=va_arg(args,char *);
	  else
	    bp=NULL; /* Just to get the compiler happy */

	  while(VAL(c!=EOF&&!isspace(c)))
	  { if(!ignore)
	      *bp++=c;
	    NEXT(c);
	  }
	  PREV(c);

	  if(!ignore&&size)
	  { *bp++='\0';
	    blocks++; }
	  break;
	}
	case 'e':
	case 'f':
	case 'g':
	{ double v;
	  int ex=0;
	  int min=0,mine=0; /* This is a workaround for gcc 2.3.3: should be char */

	  do /* This is there just to be able to break out */
	  {
	    if(VAL(c=='-'||c=='+'))
	    { min=c;
	      NEXT(c); }

	    if(VAL(tolower(c)=='i')) /* +- inf */
	    { int d;
	      NEXT(d);
	      if(VAL(tolower(d)=='n'))
	      { int e;
		NEXT(e);
		if(VAL(tolower(e)=='f'))
		{ v=*(double *)&undef[min=='-'];
		  break; } /* break out */
		PREV(e);
	      }
	      PREV(d);
	    }
	    else if(VAL(toupper(c)=='N')) /* NaN */
	    { int d;
	      NEXT(d);
	      if(VAL(tolower(d)=='a'))
	      { int e;
		NEXT(e);
		if(VAL(toupper(e)=='N'))
		{ v=*(double *)&undef[2];
		  break; }
		PREV(e);
	      }
	      PREV(d);
	    }

	    v=0.0;
	    while(VAL(isdigit(c)))
	    { v=v*10.0+(c-'0');
	      NEXT(c);
	    }

	    if(VAL(c==__decimalpoint[0]))
	    { double dp=0.1;
	      NEXT(c);
	      while(VAL(isdigit(c)))
	      { v=v+dp*(c-'0');
		dp=dp/10.0;
		NEXT(c); }
	      if(size==2+(min!=0)) /* No number read till now -> malformatted */
	      { PREV(c);
		c=__decimalpoint[0]; }
	    }

	    if(min&&size==2) /* No number read till now -> malformatted */
	    { PREV(c);
	      c=min; }
	    if(size==1)
	      break;

	    if(VAL(tolower(c)=='e'))
	    { int d;
	      NEXT(d);
	      if(VAL(d=='-'||d=='+'))
	      { mine=d;
		NEXT(d); }

	      if(VAL(isdigit(d)))
	      { do
		{ ex=ex*10+(d-'0');
		  NEXT(d);
		}while(VAL(isdigit(d)&&ex<100));
		c=d;
	      }else
	      { PREV(d);
		if(mine)
		  PREV(mine);
	      }
	    }
	    PREV(c);

	    if(mine=='-')
	      v=v/mypow10(ex);
	    else
	      v=v*mypow10(ex);

	    if(min=='-')
	      v=-v;

	  }while(0);

	  if(!ignore&&size)
	  { switch(subtype)
	    { case 'L':
#warning "FIXME: llf will be treated as Lf (double in current implementation) now!"
	      case 'l':
		*va_arg(args,double *)=v;
		break;
	      case 'i':
		*va_arg(args,float *)=v;
		break;
	    }
	    blocks++;
	  }
	  break;
	}
	case '%':
	  NEXT(c);
	  if(c!='%')
	    PREV(c); /* unget non-'%' character */
	  break;
	case 'n':
	  if(!ignore)
	    *va_arg(args,int *)=incount;
	  size=1; /* fake a valid argument */
	  break;
	default:
	{ unsigned long long v=0;
	  int base;
	  int min=0;

	  if(!type)
	    ptr--; /* unparse NUL character */

	  if(type=='p')
	  { /*subtype='l';*/ /* This is the same as %lx */
	    type='x'; }

	  if(VAL((c=='-'&&type!='u')||c=='+'))
	  { min=c;
	    NEXT(c); }

	  if(type=='i') /* which one to use ? */
	  { if(VAL(c=='0')) /* Could be octal or sedecimal */
	    { int d;
	      NEXT(d); /* Get a look at next character */
	      if(VAL(tolower(d)=='x'))
	      { int e;
		NEXT(e); /* And the next */
		if(VAL(isxdigit(c)))
		  type='x'; /* Is a valid x number with '0x?' */
		PREV(e);
	      }else
		type='o';
	      PREV(d);
	    }else if(VAL(!isdigit(c)&&isxdigit(c)))
	      type='x'; /* Is a valid x number without '0x' */
	  }

	  while(type=='x'&&VAL(c=='0')) /* sedecimal */
	  { int d;
	    NEXT(d);
	    if(VAL(tolower(d)=='x'))
	    { int e;
	      NEXT(e);
	      if(VAL(isxdigit(e)))
	      { c=e;
		break; } /* Used while just to do this ;-) */
	      PREV(e);
	    }
	    PREV(d);
	    break; /* Need no loop */
	  }

	  base=type=='x'||type=='X'?16:(type=='o'?8:10);
	  while(VAL(isxdigit(c)&&(base!=10||isdigit(c))&&(base!=8||c<='7')))
	  { v=v*base+(isdigit(c)?c-'0':0)+(isupper(c)?c-'A'+10:0)+(islower(c)?c-'a'+10:0);
	    NEXT(c);
	  }

	  if(min&&size==2) /* If there is no valid character after sign, unget last */
	  { PREV(c);
	    c=min; }

	  PREV(c);

	  if(ignore||!size)
	    break;

	  if(type=='u')
	    switch(subtype)
	    { case 'L':
		*va_arg(args,unsigned long long *)=v;
		break;
	      case 'l':
	      case 'i':
		*va_arg(args,unsigned int *)=v;
		break;
	      case 'h':
		*va_arg(args,unsigned short *)=v;
		break;
	    }
	  else
	  { signed long long v2;
	    if(min=='-')
	      v2=-v;
	    else
	      v2=v;
	    switch(subtype)
	    { case 'L':
		*va_arg(args,signed long long *)=v2;
		break;
	      case 'l':
	      case 'i':
		*va_arg(args,signed int *)=v2;
		break;
	      case 'h':
		*va_arg(args,signed short *)=v2;
		break;
	    }
	  }
	  blocks++;
	  break;
	}
      }
      format=ptr;
    }else
    { if(isspace(*format))
      { do
	  NEXT(c);
	while(isspace(c));
	PREV(c);
	size=1; }
      else
      { NEXT(c);
	if(c!=*format)
	  PREV(c); }
      format++;
    }
    if(!size)
      break;
  }

  if(c==EOF&&!blocks)
    return c;
  else
    return blocks;
}

//-----------------------------------------------------------------------------

void __assertion_failure(const char *file_name,int line_number,const char * expression)
{
	d1(kprintf("ASSERT %s/%ld: %s\n", file_name, line_number, expression));
}

//-----------------------------------------------------------------------------

#if 0
struct InitListStruct
	{
	void (* FuncPtr)(void);
	LONG Priority;
	};

extern void *__INIT_LIST__[];
extern void *__EXIT_LIST__[];
#endif

extern struct DosLibrary *__DOSBase;
extern struct Library *MathIeeeSingBasBase;
extern struct Library *MathIeeeDoubBasBase;
extern struct Library *MathIeeeDoubTransBase;
extern T_UTILITYBASE __UtilityBase;

static struct Library *MathIeeeSingBasBaseClone;
static struct Library *MathIeeeDoubBasBaseClone;
static struct Library *MathIeeeDoubTransBaseClone;

static struct SignalSemaphore VideoSema;

BOOL InitExtra(void)
{
	BOOL	Success = FALSE;

	do	{
		InitSemaphore(&VideoSema);

		__DOSBase = DOSBase;

		MathIeeeSingBasBase = MathIeeeSingBasBaseClone = OpenLibrary("mathieeesingbas.library", 39);
		d1(kprintf("%s/%s/%ld: MathIeeeSingBasBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeSingBasBase));
		if (NULL == MathIeeeSingBasBase)
			break;

		MathIeeeDoubBasBase = MathIeeeDoubBasBaseClone = OpenLibrary("mathieeedoubbas.library", 39);
		d1(kprintf("%s/%s/%ld: MathIeeeDoubBasBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeDoubBasBase));
		if (NULL == MathIeeeDoubBasBase)
			break;

		MathIeeeDoubTransBase = MathIeeeDoubTransBaseClone = OpenLibrary("mathieeedoubtrans.library", 39);
		d1(kprintf("%s/%s/%ld: MathIeeeDoubTransBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeDoubTransBase));
		if (NULL == MathIeeeDoubTransBase)
			break;

		__UtilityBase = UtilityBase;
		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: MathIeeeSingBasBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeSingBasBase));

	return Success;
}


void ExitExtra(void)
{
	if (MathIeeeSingBasBaseClone)
		{
		CloseLibrary(MathIeeeSingBasBaseClone);
		MathIeeeSingBasBaseClone = NULL;
		}
	if (MathIeeeDoubBasBaseClone)
		{
		CloseLibrary(MathIeeeDoubBasBaseClone);
		MathIeeeDoubBasBaseClone = NULL;
		}
	if (MathIeeeDoubTransBaseClone)
		{
		CloseLibrary(MathIeeeDoubTransBaseClone);
		MathIeeeDoubTransBaseClone = NULL;
		}
}


BOOL InitInstance(void)
{
	BOOL	Success = FALSE;

	do	{
		ObtainSemaphore(&VideoSema);

		// Create private math library bases

		MathIeeeSingBasBase = OpenLibrary("mathieeesingbas.library", 39);
		d1(kprintf("%s/%s/%ld: MathIeeeSingBasBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeSingBasBase));
		if (NULL == MathIeeeSingBasBase)
			break;

		MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library", 39);
		d1(kprintf("%s/%s/%ld: MathIeeeDoubBasBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeDoubBasBase));
		if (NULL == MathIeeeDoubBasBase)
			break;

		MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library", 39);
		d1(kprintf("%s/%s/%ld: MathIeeeDoubTransBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeDoubTransBase));
		if (NULL == MathIeeeDoubTransBase)
			break;

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: MathIeeeSingBasBase=%08lx\n", __FILE__, __FUNC__, __LINE__, MathIeeeSingBasBase));

	return Success;
}


void ExitInstance(void)
{
	// Fall back to global math library bases
	if (MathIeeeSingBasBase)
		{
		CloseLibrary(MathIeeeSingBasBase);
		MathIeeeSingBasBase = MathIeeeSingBasBaseClone;
		}
	if (MathIeeeDoubBasBase)
		{
		CloseLibrary(MathIeeeDoubBasBase);
		MathIeeeDoubBasBase = MathIeeeDoubBasBaseClone;
		}
	if (MathIeeeDoubTransBase)
		{
		CloseLibrary(MathIeeeDoubTransBase);
		MathIeeeDoubTransBase = MathIeeeDoubTransBaseClone;
		}

	ReleaseSemaphore(&VideoSema);
}

//-----------------------------------------------------------------------------

struct iob ** __iob;	// #define stderr (&__iob[2])	/* standard error file pointer */
struct WBStartup *__WBenchMsg = NULL;

//-----------------------------------------------------------------------------
