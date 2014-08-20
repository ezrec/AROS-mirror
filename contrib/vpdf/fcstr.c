
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define FcChar32 unsigned int
#define FcChar8 char
#define FcChar16 unsigned short
#define FcBool int
#define FcTrue 1
#define FcFalse 0
#define FcMemAlloc(group, size)
#define FcMemFree(group, size)
#define intptr_t unsigned int

#define FC_CHARSET_MAP_SIZE (256/32)
#define FC_CHARSET_DONE	((FcChar32) -1)
#define FC_UTF8_MAX_LEN	6

typedef enum { FcEndianBig, FcEndianLittle } FcEndian;

typedef struct _FcCaseFold {
    unsigned int   upper;
    unsigned short method : 2;
	unsigned short count : 14;
    short offset;	    /* lower - upper for RANGE, table id for FULL */
} FcCaseFold;

typedef struct _FcStrBuf {
    char *buf;
    int  allocated;
    int  failed;
    int  len;
    int  size;
    char buf_static[16 * sizeof (void *)];
} FcStrBuf;

struct _FcCharSet {
    int		    ref;	/* reference count */
    int		    num;	/* size of leaves and numbers arrays */
    intptr_t	    leaves_offset;
    intptr_t	    numbers_offset;
}FcCharSet;


typedef struct _FcStrSet {
    int		    ref;	/* reference count */
    int		    num;
    int		    size;
    FcChar8	    **strs;
} FcStrSet;



#define FC_CASE_FOLD_RANGE	    0
#define FC_CASE_FOLD_EVEN_ODD	    1
#define FC_CASE_FOLD_FULL	    2

static int FcUtf8ToUcs4 (const FcChar8 *src_orig, FcChar32	    *dst, int	    len);
static int FcUcs4ToUtf8 (FcChar32	ucs4, FcChar8	dest[FC_UTF8_MAX_LEN]);
static FcBool FcStrBufChar (FcStrBuf *buf, FcChar8 c);

static FcChar8 *FcStrCopy (const FcChar8 *s)
{
    int     len;
    FcChar8 *r;

    if (!s)
	return 0;
    len = strlen ((char *) s) + 1;
    r = (FcChar8 *) malloc (len);
    if (!r)
	return 0;
    FcMemAlloc (FC_MEM_STRING, len);
    memcpy (r, s, len);
    return r;
}

static FcChar8 *FcStrPlus (const FcChar8 *s1, const FcChar8 *s2)
{
    int	    l = strlen ((char *)s1) + strlen ((char *) s2) + 1;
    FcChar8 *s = malloc (l);

    if (!s)
	return 0;
    FcMemAlloc (FC_MEM_STRING, l);
    strcpy ((char *) s, (char *) s1);
    strcat ((char *) s, (char *) s2);
    return s;
}

static void FcStrFree (FcChar8 *s)
{
    FcMemFree (FC_MEM_STRING, strlen ((char *) s) + 1);
    free (s);
}

#include "fccase.h"

#define FcCaseFoldUpperCount(cf) \
    ((cf)->method == FC_CASE_FOLD_FULL ? 1 : (cf)->count)


#define FC_STR_CANON_BUF_LEN	1024

typedef struct _FcCaseWalker {
    const FcChar8   *read;
    const FcChar8   *src;
    FcChar8	    utf8[FC_MAX_CASE_FOLD_CHARS + 1];
} FcCaseWalker;

static void FcStrCaseWalkerInit (const FcChar8 *src, FcCaseWalker *w)
{
    w->src = src;
    w->read = 0;
}

static FcChar8 FcStrCaseWalkerLong (FcCaseWalker *w, FcChar8 r)
{
    FcChar32	ucs4;
    int		slen;
    int		len = strlen((char*)w->src);

    slen = FcUtf8ToUcs4 (w->src - 1, &ucs4, len + 1);
    if (slen <= 0)
	return r;
    if (FC_MIN_FOLD_CHAR <= ucs4 && ucs4 <= FC_MAX_FOLD_CHAR)
    {
	int min = 0;
	int max = FC_NUM_CASE_FOLD;

	while (min <= max)
	{
	    int		mid = (min + max) >> 1;
	    FcChar32    low = fcCaseFold[mid].upper;
	    FcChar32    high = low + FcCaseFoldUpperCount (&fcCaseFold[mid]);
	    
	    if (high <= ucs4)
		min = mid + 1;
	    else if (ucs4 < low)
		max = mid - 1;
	    else
	    {
		const FcCaseFold    *fold = &fcCaseFold[mid];
		int		    dlen;
		
		switch (fold->method) {
		case  FC_CASE_FOLD_EVEN_ODD:
		    if ((ucs4 & 1) != (fold->upper & 1))
			return r;
		    /* fall through ... */
		default:
		    dlen = FcUcs4ToUtf8 (ucs4 + fold->offset, w->utf8);
		    break;
		case FC_CASE_FOLD_FULL:
		    dlen = fold->count;
		    memcpy (w->utf8, fcCaseFoldChars + fold->offset, dlen);
		    break;
		}
		
		/* consume rest of src utf-8 bytes */
		w->src += slen - 1;
		
		/* read from temp buffer */
		w->utf8[dlen] = '\0';
		w->read = w->utf8;
		return *w->read++;
	    }
	}
    }
    return r;
}

static FcChar8 FcStrCaseWalkerNext (FcCaseWalker *w)
{
    FcChar8	r;

    if (w->read)
    {
	if ((r = *w->read++))
	    return r;
	w->read = 0;
    }
    r = *w->src++;
    
    if ((r & 0xc0) == 0xc0)
	return FcStrCaseWalkerLong (w, r);
    if ('A' <= r && r <= 'Z')
        r = r - 'A' + 'a';
    return r;
}

static FcChar8 FcStrCaseWalkerNextIgnoreBlanks (FcCaseWalker *w)
{
    FcChar8	r;

    if (w->read)
    {
	if ((r = *w->read++))
	    return r;
	w->read = 0;
    }
    do
    {
	r = *w->src++;
    } while (r == ' ');
    
    if ((r & 0xc0) == 0xc0)
	return FcStrCaseWalkerLong (w, r);
    if ('A' <= r && r <= 'Z')
        r = r - 'A' + 'a';
    return r;
}

static FcChar8 *FcStrDowncase (const FcChar8 *s)
{
    FcCaseWalker    w;
    int		    len = 0;
    FcChar8	    *dst, *d;

    FcStrCaseWalkerInit (s, &w);
    while (FcStrCaseWalkerNext (&w))
	len++;
    d = dst = malloc (len + 1);
    if (!d)
	return 0;
    FcMemAlloc (FC_MEM_STRING, len + 1);
    FcStrCaseWalkerInit (s, &w);
    while ((*d++ = FcStrCaseWalkerNext (&w)));
    return dst;
}

static int FcStrCmpIgnoreCase (const FcChar8 *s1, const FcChar8 *s2)
{
    FcCaseWalker    w1, w2;
    FcChar8	    c1, c2;

    if (s1 == s2) return 0;
    
    FcStrCaseWalkerInit (s1, &w1);
    FcStrCaseWalkerInit (s2, &w2);
    
    for (;;) 
    {
	c1 = FcStrCaseWalkerNext (&w1);
	c2 = FcStrCaseWalkerNext (&w2);
	if (!c1 || (c1 != c2))
	    break;
    }
    return (int) c1 - (int) c2;
}

static int FcStrCmpIgnoreBlanksAndCase (const FcChar8 *s1, const FcChar8 *s2)
{
    FcCaseWalker    w1, w2;
    FcChar8	    c1, c2;

    if (s1 == s2) return 0;
    
    FcStrCaseWalkerInit (s1, &w1);
    FcStrCaseWalkerInit (s2, &w2);
    
    for (;;) 
    {
	c1 = FcStrCaseWalkerNextIgnoreBlanks (&w1);
	c2 = FcStrCaseWalkerNextIgnoreBlanks (&w2);
	if (!c1 || (c1 != c2))
	    break;
    }
    return (int) c1 - (int) c2;
}

static int FcStrCmp (const FcChar8 *s1, const FcChar8 *s2)
{
    FcChar8 c1, c2;
    
    if (s1 == s2)
	return 0;
    for (;;) 
    {
	c1 = *s1++;
	c2 = *s2++;
	if (!c1 || c1 != c2)
	    break;
    }
    return (int) c1 - (int) c2;
}

/*
 * Return a hash value for a string
 */

static FcChar32 FcStrHashIgnoreCase (const FcChar8 *s)
{
    FcChar32	    h = 0;
    FcCaseWalker    w;
    FcChar8	    c;

    FcStrCaseWalkerInit (s, &w);
    while ((c = FcStrCaseWalkerNext (&w)))
	h = ((h << 3) ^ (h >> 3)) ^ c;
    return h;
}

/*
 * Is the head of s1 equal to s2?
 */

static FcBool
FcStrIsAtIgnoreBlanksAndCase (const FcChar8 *s1, const FcChar8 *s2)
{
    FcCaseWalker    w1, w2;
    FcChar8	    c1, c2;

    FcStrCaseWalkerInit (s1, &w1);
    FcStrCaseWalkerInit (s2, &w2);
    
    for (;;) 
    {
	c1 = FcStrCaseWalkerNextIgnoreBlanks (&w1);
	c2 = FcStrCaseWalkerNextIgnoreBlanks (&w2);
	if (!c1 || (c1 != c2))
	    break;
    }
    return c1 == c2 || !c2;
}

/*
 * Does s1 contain an instance of s2 (ignoring blanks and case)?
 */

static const FcChar8 *FcStrContainsIgnoreBlanksAndCase (const FcChar8 *s1, const FcChar8 *s2)
{
    while (*s1)
    {
	if (FcStrIsAtIgnoreBlanksAndCase (s1, s2))
	    return s1;
	s1++;
    }
    return 0;
}

static FcBool FcCharIsPunct (const FcChar8 c)
{
    if (c < '0')
	return FcTrue;
    if (c <= '9')
	return FcFalse;
    if (c < 'A')
	return FcTrue;
    if (c <= 'Z')
	return FcFalse;
    if (c < 'a')
	return FcTrue;
    if (c <= 'z')
	return FcFalse;
    if (c <= '~')
	return FcTrue;
    return FcFalse;
}

/*
 * Is the head of s1 equal to s2?
 */

static FcBool FcStrIsAtIgnoreCase (const FcChar8 *s1, const FcChar8 *s2)
{
    FcCaseWalker    w1, w2;
    FcChar8	    c1, c2;

    FcStrCaseWalkerInit (s1, &w1);
    FcStrCaseWalkerInit (s2, &w2);
    
    for (;;) 
    {
	c1 = FcStrCaseWalkerNext (&w1);
	c2 = FcStrCaseWalkerNext (&w2);
	if (!c1 || (c1 != c2))
	    break;
    }
    return c1 == c2 || !c2;
}

/*
 * Does s1 contain an instance of s2 (ignoring blanks and case)?
 */

static const FcChar8 *FcStrContainsIgnoreCase (const FcChar8 *s1, const FcChar8 *s2)
{
    while (*s1)
    {
	if (FcStrIsAtIgnoreCase (s1, s2))
	    return s1;
	s1++;
    }
    return 0;
}

/*
 * Does s1 contain an instance of s2 on a word boundary (ignoring case)?
 */

static const FcChar8 *FcStrContainsWord (const FcChar8 *s1, const FcChar8 *s2)
{
    FcBool  wordStart = FcTrue;
    int	    s1len = strlen ((char *) s1);
    int	    s2len = strlen ((char *) s2);
	
    while (s1len >= s2len)
    {
	if (wordStart && 
	    FcStrIsAtIgnoreCase (s1, s2) &&
	    (s1len == s2len || FcCharIsPunct (s1[s2len])))
	{
	    return s1;
	}
	wordStart = FcFalse;
	if (FcCharIsPunct (*s1))
	    wordStart = FcTrue;
	s1++;
	s1len--;
    }
    return 0;
}

static const FcChar8 *FcStrStrIgnoreCase (const FcChar8 *s1, const FcChar8 *s2)
{
    FcCaseWalker    w1, w2;
    FcChar8	    c1, c2;
    const FcChar8   *cur;

    if (!s1 || !s2)
	return 0;

    if (s1 == s2)
	return s1;
    
    FcStrCaseWalkerInit (s1, &w1);
    FcStrCaseWalkerInit (s2, &w2);
    
    c2 = FcStrCaseWalkerNext (&w2);
    
    for (;;)
    {
	cur = w1.src;
	c1 = FcStrCaseWalkerNext (&w1);
	if (!c1)
	    break;
	if (c1 == c2)
	{
	    FcCaseWalker    w1t = w1;
	    FcCaseWalker    w2t = w2;
	    FcChar8	    c1t, c2t;

	    for (;;)
	    {
		c1t = FcStrCaseWalkerNext (&w1t);
		c2t = FcStrCaseWalkerNext (&w2t);

		if (!c2t)
		    return cur;
		if (c2t != c1t)
		    break;
	    }
	}
    }
    return 0;
}

static const FcChar8 *FcStrStr (const FcChar8 *s1, const FcChar8 *s2)
{
    FcChar8 c1, c2;
    const FcChar8 * p = s1;
    const FcChar8 * b = s2;

    if (!s1 || !s2)
	return 0;

    if (s1 == s2)
	return s1;

again:
    c2 = *s2++;

    if (!c2)
	return 0;

    for (;;) 
    {
	p = s1;
	c1 = *s1++;
	if (!c1 || c1 == c2)
	    break;
    }

    if (c1 != c2)
	return 0;

    for (;;)
    {
	c1 = *s1;
	c2 = *s2;
	if (c1 && c2 && c1 != c2)
	{
	    s1 = p + 1;
	    s2 = b;
	    goto again;
	}
	if (!c2)
	    return p;
	if (!c1)
	    return 0;
	++ s1;
	++ s2;
    }
    /* never reached. */
}

static int FcUtf8ToUcs4 (const FcChar8 *src_orig,
	      FcChar32	    *dst,
	      int	    len)
{
    const FcChar8   *src = src_orig;
    FcChar8	    s;
    int		    extra;
    FcChar32	    result;

    if (len == 0)
	return 0;
    
    s = *src++;
    len--;
    
    if (!(s & 0x80))
    {
	result = s;
	extra = 0;
    } 
    else if (!(s & 0x40))
    {
	return -1;
    }
    else if (!(s & 0x20))
    {
	result = s & 0x1f;
	extra = 1;
    }
    else if (!(s & 0x10))
    {
	result = s & 0xf;
	extra = 2;
    }
    else if (!(s & 0x08))
    {
	result = s & 0x07;
	extra = 3;
    }
    else if (!(s & 0x04))
    {
	result = s & 0x03;
	extra = 4;
    }
    else if ( ! (s & 0x02))
    {
	result = s & 0x01;
	extra = 5;
    }
    else
    {
	return -1;
    }
    if (extra > len)
	return -1;
    
    while (extra--)
    {
	result <<= 6;
	s = *src++;
	
	if ((s & 0xc0) != 0x80)
	    return -1;
	
	result |= s & 0x3f;
    }
    *dst = result;
    return src - src_orig;
}

static FcBool FcUtf8Len (const FcChar8    *string,
	   int		    len,
	   int		    *nchar,
	   int		    *wchar)
{
    int		n;
    int		clen;
    FcChar32	c;
    FcChar32	max;
    
    n = 0;
    max = 0;
    while (len)
    {
	clen = FcUtf8ToUcs4 (string, &c, len);
	if (clen <= 0)	/* malformed UTF8 string */
	    return FcFalse;
	if (c > max)
	    max = c;
	string += clen;
	len -= clen;
	n++;
    }
    *nchar = n;
    if (max >= 0x10000)
	*wchar = 4;
    else if (max > 0x100)
	*wchar = 2;
    else
	*wchar = 1;
    return FcTrue;
}

static int FcUcs4ToUtf8 (FcChar32	ucs4,
	      FcChar8	dest[FC_UTF8_MAX_LEN])
{
    int	bits;
    FcChar8 *d = dest;
    
    if      (ucs4 <       0x80) {  *d++=  ucs4;                         bits= -6; }
    else if (ucs4 <      0x800) {  *d++= ((ucs4 >>  6) & 0x1F) | 0xC0;  bits=  0; }
    else if (ucs4 <    0x10000) {  *d++= ((ucs4 >> 12) & 0x0F) | 0xE0;  bits=  6; }
    else if (ucs4 <   0x200000) {  *d++= ((ucs4 >> 18) & 0x07) | 0xF0;  bits= 12; }
    else if (ucs4 <  0x4000000) {  *d++= ((ucs4 >> 24) & 0x03) | 0xF8;  bits= 18; }
    else if (ucs4 < 0x80000000) {  *d++= ((ucs4 >> 30) & 0x01) | 0xFC;  bits= 24; }
    else return 0;

    for ( ; bits >= 0; bits-= 6) {
	*d++= ((ucs4 >> bits) & 0x3F) | 0x80;
    }
    return d - dest;
}

#define GetUtf16(src,endian) \
    ((FcChar16) ((src)[endian == FcEndianBig ? 0 : 1] << 8) | \
     (FcChar16) ((src)[endian == FcEndianBig ? 1 : 0]))

static int FcUtf16ToUcs4 (const FcChar8	*src_orig,
	       FcEndian		endian,
	       FcChar32		*dst,
	       int		len)	/* in bytes */
{
    const FcChar8   *src = src_orig;
    FcChar16	    a, b;
    FcChar32	    result;

    if (len < 2)
	return 0;
    
    a = GetUtf16 (src, endian); src += 2; len -= 2;
    
    /* 
     * Check for surrogate 
     */
    if ((a & 0xfc00) == 0xd800)
    {
	if (len < 2)
	    return 0;
	b = GetUtf16 (src, endian); src += 2; len -= 2;
	/*
	 * Check for invalid surrogate sequence
	 */
	if ((b & 0xfc00) != 0xdc00)
	    return 0;
	result = ((((FcChar32) a & 0x3ff) << 10) |
		  ((FcChar32) b & 0x3ff)) + 0x10000;
    }
    else
	result = a;
    *dst = result;
    return src - src_orig;
}

static FcBool FcUtf16Len (const FcChar8   *string,
	    FcEndian	    endian,
	    int		    len,	/* in bytes */
	    int		    *nchar,
	    int		    *wchar)
{
    int		n;
    int		clen;
    FcChar32	c;
    FcChar32	max;
    
    n = 0;
    max = 0;
    while (len)
    {
	clen = FcUtf16ToUcs4 (string, endian, &c, len);
	if (clen <= 0)	/* malformed UTF8 string */
	    return FcFalse;
	if (c > max)
	    max = c;
	string += clen;
	len -= clen;
	n++;
    }
    *nchar = n;
    if (max >= 0x10000)
	*wchar = 4;
    else if (max > 0x100)
	*wchar = 2;
    else
	*wchar = 1;
    return FcTrue;
}

static void FcStrBufInit (FcStrBuf *buf, FcChar8 *init, int size)
{
    if (init)
    {
	buf->buf = init;
	buf->size = size;
    } else
    {
	buf->buf = buf->buf_static;
	buf->size = sizeof (buf->buf_static);
    }
    buf->allocated = FcFalse;
    buf->failed = FcFalse;
    buf->len = 0;
}

static void FcStrBufDestroy (FcStrBuf *buf)
{
    if (buf->allocated)
    {
	FcMemFree (FC_MEM_STRBUF, buf->size);
	free (buf->buf);
	FcStrBufInit (buf, 0, 0);
    }
}

static FcChar8 *FcStrBufDone (FcStrBuf *buf)
{
    FcChar8 *ret;

    if (buf->failed)
	ret = NULL;
    else
	ret = malloc (buf->len + 1);
    if (ret)
    {
	FcMemAlloc (FC_MEM_STRING, buf->len + 1);
	memcpy (ret, buf->buf, buf->len);
	ret[buf->len] = '\0';
    }
    FcStrBufDestroy (buf);
    return ret;
}

static FcChar8 *FcStrBufDoneStatic (FcStrBuf *buf)
{
    FcStrBufChar (buf, '\0');

    if (buf->failed)
	return NULL;

    return buf->buf;
}

static FcBool FcStrBufChar (FcStrBuf *buf, FcChar8 c)
{
    if (buf->len == buf->size)
    {
	FcChar8	    *new;
	int	    size;

	if (buf->failed)
	    return FcFalse;

	if (buf->allocated)
	{
	    size = buf->size * 2;
	    new = realloc (buf->buf, size);
	}
	else
	{
	    size = buf->size + 64;
	    new = malloc (size);
	    if (new)
	    {
		buf->allocated = FcTrue;
		memcpy (new, buf->buf, buf->len);
	    }
	}
	if (!new)
	{
	    buf->failed = FcTrue;
	    return FcFalse;
	}
	if (buf->size)
	    FcMemFree (FC_MEM_STRBUF, buf->size);
	FcMemAlloc (FC_MEM_STRBUF, size);
	buf->size = size;
	buf->buf = new;
    }
    buf->buf[buf->len++] = c;
    return FcTrue;
}

static FcBool FcStrBufString (FcStrBuf *buf, const FcChar8 *s)
{
    FcChar8 c;
    while ((c = *s++))
	if (!FcStrBufChar (buf, c))
	    return FcFalse;
    return FcTrue;
}

static FcBool FcStrBufData (FcStrBuf *buf, const FcChar8 *s, int len)
{
    while (len-- > 0)
	if (!FcStrBufChar (buf, *s++))
	    return FcFalse;
    return FcTrue;
}



static FcStrSet *FcStrSetCreate (void)
{
    FcStrSet	*set = malloc (sizeof (FcStrSet));
    if (!set)
	return 0;
    FcMemAlloc (FC_MEM_STRSET, sizeof (FcStrSet));
    set->ref = 1;
    set->num = 0;
    set->size = 0;
    set->strs = 0;
    return set;
}

static FcBool FcStrSetMember (FcStrSet *set, const FcChar8 *s);


static FcBool _FcStrSetAppend (FcStrSet *set, FcChar8 *s)
{
    if (FcStrSetMember (set, s))
    {
	FcStrFree (s);
	return FcTrue;
    }
    if (set->num == set->size)
    {
	FcChar8	**strs = malloc ((set->size + 2) * sizeof (FcChar8 *));

	if (!strs)
	    return FcFalse;
	FcMemAlloc (FC_MEM_STRSET, (set->size + 2) * sizeof (FcChar8 *));
	if (set->num)
	    memcpy (strs, set->strs, set->num * sizeof (FcChar8 *));
	if (set->strs)
	{
	    FcMemFree (FC_MEM_STRSET, (set->size + 1) * sizeof (FcChar8 *));
	    free (set->strs);
	}
	set->size = set->size + 1;
	set->strs = strs;
    }
    set->strs[set->num++] = s;
    set->strs[set->num] = 0;
    return FcTrue;
}



static FcBool FcStrSetMember (FcStrSet *set, const FcChar8 *s)
{
    int	i;

    for (i = 0; i < set->num; i++)
	if (!FcStrCmp (set->strs[i], s))
	    return FcTrue;
    return FcFalse;
}

static FcBool FcStrSetEqual (FcStrSet *sa, FcStrSet *sb)
{
    int	i;
    if (sa->num != sb->num)
	return FcFalse;
    for (i = 0; i < sa->num; i++)
	if (!FcStrSetMember (sb, sa->strs[i]))
	    return FcFalse;
    return FcTrue;
}

static FcBool FcStrSetAdd (FcStrSet *set, const FcChar8 *s)
{
    FcChar8 *new = FcStrCopy (s);
    if (!new)
	return FcFalse;
    if (!_FcStrSetAppend (set, new))
    {
	FcStrFree (new);
	return FcFalse;
    }
    return FcTrue;
}

#if 0
FcBool
FcStrSetAddFilename (FcStrSet *set, const FcChar8 *s)
{
    FcChar8 *new = FcStrCopyFilename (s);
    if (!new)
	return FcFalse;
    if (!_FcStrSetAppend (set, new))
    {
	FcStrFree (new);
	return FcFalse;
    }
    return FcTrue;
}
#endif

static FcBool FcStrSetDel (FcStrSet *set, const FcChar8 *s)
{
    int	i;

    for (i = 0; i < set->num; i++)
	if (!FcStrCmp (set->strs[i], s))
	{
	    FcStrFree (set->strs[i]);
	    /*
	     * copy remaining string pointers and trailing
	     * NULL
	     */
	    memmove (&set->strs[i], &set->strs[i+1], 
		     (set->num - i) * sizeof (FcChar8 *));
	    set->num--;
	    return FcTrue;
	}
    return FcFalse;
}

static void FcStrSetDestroy (FcStrSet *set)
{
    if (--set->ref == 0)
    {
	int	i;
    
	for (i = 0; i < set->num; i++)
	    FcStrFree (set->strs[i]);
	if (set->strs)
	{
	    FcMemFree (FC_MEM_STRSET, (set->size + 1) * sizeof (FcChar8 *));
	    free (set->strs);
	}
	FcMemFree (FC_MEM_STRSET, sizeof (FcStrSet));
	free (set);
    }
}


