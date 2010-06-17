#ifndef _SYS__IOVEC_H_
#define _SYS__IOVEC_H_
/*
    Copyright © 1995-2003, The AROS Development Team. All rights reserved.
    $Id: _iovec.h 30792 2009-03-07 22:40:04Z neil $
*/

#include <sys/_types.h>

struct iovec
{
    void        *iov_base;
    size_t       iov_len;
};

#endif /* _SYS__IOVEC_H_ */
