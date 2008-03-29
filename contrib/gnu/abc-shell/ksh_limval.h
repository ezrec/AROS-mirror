/* Wrapper around the values.h/limits.h includes/ifdefs */
/* $Id: ksh_limval.h,v 1.4 2005/12/10 18:43:05 hnl_dk Exp $ */

/* limits.h is included in sh.h */

#ifndef DMAXEXP
# define DMAXEXP        128     /* should be big enough */
#endif

#ifndef BITSPERBYTE
# define BITSPERBYTE   CHAR_BIT
#endif

#ifndef BITS
# define BITS(t)        (CHAR_BIT * sizeof(t))
#endif
