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
#include <math.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include <defs.h>

#include "Video.h"

//---------------------------------------------------------------------------------------

BOOL InitExtra(void)
{
	return TRUE;
}

void ExitExtra(void)
{
}

BOOL InitInstance(void)
{
	return TRUE;
}

void ExitInstance(void)
{
}

//---------------------------------------------------------------------------------------

int open(const char *filename, int mode, int protection)
{
	BPTR fh;

	d1(KPrintF("%s/%s/%ld: name=<%s>  mode=%d  protection=%d\n", __FILE__, __FUNC__, __LINE__, filename, mode, protection));

	fh = Open((STRPTR) filename, MODE_OLDFILE);

	return fh ? ((int) fh) : -1;
}

//-----------------------------------------------------------------------------

int close(int fh)
{
	d1(KPrintF("%s/%s/%ld: fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));

	return Close((BPTR) fh) ? 0 : -1;
}

//-----------------------------------------------------------------------------

int read(int fh, void *buffer, unsigned int length)
{
	LONG Result;

	d1(KPrintF("%s/%s/%ld: fh=%08lx  length=%lu\n", __FILE__, __FUNC__, __LINE__, fh, length));

	Result = Read((BPTR) fh, buffer, length);
	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return (int) Result;
}

//-----------------------------------------------------------------------------

int write(int fh, void *buffer, unsigned int length)
{
	// Writing is not supported
	d1(KPrintF("%s/%s/%ld: fh=%08lx  length=%lu\n", __FILE__, __FUNC__, __LINE__, fh, length));

	return -1;
}

//-----------------------------------------------------------------------------

int lseek(int fd, int pos, int whence)
{
	int mode;
	LONG Result;

	d1(KPrintF("%s/%s/%ld: fd=%08lx  pos=%ld  whence=%ld\n", __FILE__, __FUNC__, __LINE__, fd, pos, whence));

	switch (whence)
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

	Result = Seek((BPTR) fd, pos, mode);
	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
	if (Result >= 0)
		{
		// if no error occurred, we return the current position
		Result = Seek((BPTR) fd, 0, OFFSET_CURRENT);
		d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		}

	return (int) Result;
}

//-----------------------------------------------------------------------------

int fclose(FILE *fh)
{
	d1(KPrintF("%s/%s/%ld: fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));

	errno = 0;

	return Close((BPTR) fh) ? 0 : -1;
}

//-----------------------------------------------------------------------------

FILE *fopen(const char *name, const char *mode)
{
	LONG accessMode;
	FILE *fh;

	d1(KPrintF("%s/%s/%ld: name=<%s>  mode=<%s>\n", __FILE__, __FUNC__, __LINE__, name, mode));

	errno = 0;

	if ('r' == *mode)
		accessMode = MODE_OLDFILE;
	else
		accessMode = MODE_NEWFILE;

	fh = (FILE *) Open((STRPTR) name, accessMode);

	d1(kprintf("%s/%s/%ld: name=<%s>  mode=<%s>  fh=%08lx\n", __FILE__, __FUNC__, __LINE__, name, mode, fh));
	if (NULL == fh)
		{
		LONG error = IoErr();

		d1(kprintf("%s/%s/%ld: error=%ld\n", __FILE__, __FUNC__, __LINE__, error));

		if (ERROR_OBJECT_NOT_FOUND == error)
			errno = ENOENT;
		else
			errno = EIO;
		}

	return fh;
}

//-----------------------------------------------------------------------------

int fflush(FILE *fh)
{
	d1(KPrintF("%s/%s/%ld: fh=%08lx\n", __FILE__, __FUNC__, __LINE__, fh));

	return Flush((BPTR) fh) ? 0 : -1;
}

//-----------------------------------------------------------------------------

size_t fread(void *buf, size_t length, size_t nobj, FILE *fh)
{
	d1(KPrintF("%s/%s/%ld:  fh=%08lx  buf=%08lx  nobj=%lu  length=%lu\n", \
		__FILE__, __FUNC__, __LINE__, fh, buf, nobj, length));

	errno = 0;

	return (size_t) FRead((BPTR) fh, buf, length, nobj);
}

//-----------------------------------------------------------------------------

size_t fwrite(const void *buf, size_t length, size_t nobj, FILE *fh)
{
	d1(KPrintF("%s/%s/%ld:  fh=%08lx  buf=%08lx  nobj=%lu  length=%lu\n", \
		__FILE__, __FUNC__, __LINE__, fh, buf, nobj, length));
	// Writing is not supported

	return 0;
}

//-----------------------------------------------------------------------------

int fputc(int c, FILE *stream)
{
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

int fcntl(int fd, int cmd, ...)
{
	d1(KPrintF("%s/%s/%ld: fd=%08lx  cmd=%ld\n", __FILE__, __FUNC__, __LINE__, fd, cmd));

	return -1;
}

//-----------------------------------------------------------------------------

#if (__GNUC__ < 4) || (__GNUC_MINOR__ < 4)
double trunc(double x)
{
	if (isinf(x))
		return x;

	if (x < 0.0)
		return - floor (-x);
	else
		return floor (x);
}

float truncf(float x)
{
	return (float) trunc (x);
}

float cbrtf(float x)
{
	if( isinf(x) )
		return x;
	else
		{
		if (x >= 0)
			return powf (x, 1.0 / 3.0);
		else
			return -powf (-x, 1.0 / 3.0);
		}
}
#endif // __VERSION__

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
	d1(KPrintF("%s/%s/%ld: template=<%s>\n", __FILE__, __FUNC__, __LINE__, template));
	return -1;
}

//-----------------------------------------------------------------------------

#if (__GNUC__ < 4) || (__GNUC_MINOR__ < 4)
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
#endif // (__GNUC__ < 4) || (__GNUC_MINOR__ < 4)

//-----------------------------------------------------------------------------

struct stat;

int fstat(int fd, struct stat *stat_buf)
{
	return -1;
}

int please_use_av_log(void)
{
	return 0;
}

int please_use_av_log_instead_of_printf(void)
{
	return 0;
}

int usleep(int us)
{
	return 0;
}

//-----------------------------------------------------------------------------

/*
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 */

typedef union
{
	double value;
	struct
	{
		unsigned int msw;
		unsigned int lsw;
	} parts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0,ix1,d)					\
do {												\
  ieee_double_shape_type ew_u;						\
  ew_u.value = (d);									\
  (ix0) = ew_u.parts.msw;							\
  (ix1) = ew_u.parts.lsw;							\
} while (0)

#define INSERT_WORDS(d,ix0,ix1)						\
do {												\
  ieee_double_shape_type iw_u;						\
  iw_u.parts.msw = (ix0);							\
  iw_u.parts.lsw = (ix1);							\
  (d) = iw_u.value;									\
} while (0)

double round(double x)
{
  /* Most significant word, least significant word. */
  LONG msw, exponent_less_1023;
  ULONG lsw;

  EXTRACT_WORDS(msw, lsw, x);

  /* Extract exponent field. */
  exponent_less_1023 = ((msw & 0x7ff00000) >> 20) - 1023;

  if (exponent_less_1023 < 20)
    {
      if (exponent_less_1023 < 0)
        {
          msw &= 0x80000000;
          if (exponent_less_1023 == -1)
            /* Result is +1.0 or -1.0. */
            msw |= (1023 << 20);
          lsw = 0;
        }
      else
        {
          ULONG exponent_mask = 0x000fffff >> exponent_less_1023;
          if ((msw & exponent_mask) == 0 && lsw == 0)
            /* x in an integral value. */
            return x;

          msw += 0x00080000 >> exponent_less_1023;
          msw &= ~exponent_mask;
          lsw = 0;
        }
    }
  else if (exponent_less_1023 > 51)
    {
      if (exponent_less_1023 == 1024)
        /* x is NaN or infinite. */
        return x + x;
      else
        return x;
    }
  else
    {
      ULONG exponent_mask = 0xffffffff >> (exponent_less_1023 - 20);
      ULONG tmp;

      if ((lsw & exponent_mask) == 0)
        /* x is an integral value. */
        return x;

      tmp = lsw + (1 << (51 - exponent_less_1023));
      if (tmp < lsw)
        msw += 1;
      lsw = tmp;

      lsw &= ~exponent_mask;
    }
  INSERT_WORDS(x, msw, lsw);

  return x;
}

//-----------------------------------------------------------------------------
#if 1
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

	bytes = (bytes + (blocksize - 1)) & ~(blocksize - 1);

	return malloc(bytes);
}
#endif
//-----------------------------------------------------------------------------

 FILE **__sF;

char _ProgramName[] = "xx";

//-----------------------------------------------------------------------------

clock_t clock(void)
{
	return 0;
}

//-----------------------------------------------------------------------------


