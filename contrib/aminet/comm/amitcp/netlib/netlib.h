/* $Id$
 *
 * netlib.h -- common Network Support Library definitions
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * This file is part of the AmiTCP/IP Network Support Library.
 *
 * Copyright © 1994 AmiTCP/IP Group, <AmiTCP-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *
 * Created      : Sun Mar 27 20:39:49 1994 ppessi
 * Last modified: Mon Mar 28 14:14:56 1994 ppessi
 */

/*
 * This is supposed to be compiler-independent error setting interface
 */
#ifdef __SASC
extern int _OSERR;
extern int errno;
extern int __io2errno(int);
#define __set_errno(x) do { errno = __io2errno(_OSERR = (x)); } while (0)
#define SET_OSERR(code) do { _OSERR = (code); } while (0)
#else
void __set_errno(UBYTE code);
#define SET_OSERR(code) do { } while (0)
#endif

#define set_errno __set_errno
#define OSERR _OSERR
