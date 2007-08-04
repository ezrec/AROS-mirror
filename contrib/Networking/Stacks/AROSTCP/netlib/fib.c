/* $Id: fib.c,v 1.1.1.2 2005/12/07 10:50:34 sonic_amiga Exp $
 *
 *      fib.c - common fib buffer for stat() and chown()
 *
 *      Copyright © 1994 AmiTCP/IP Group, 
 *                       Network Solutions Development Inc.
 *                       All rights reserved.
 */

/* DOS 3.0 and MuFS extensions to file info block */
#include "fibex.h"

struct FileInfoBlock __dostat_fib[1];

