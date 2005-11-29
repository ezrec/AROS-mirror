/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id: dos_dosdoio.h 12448 2001-10-24 01:56:02Z chodorowski $

    Desc: Internal proto and define for DosDoIO
    Lang: English
*/
#ifndef DOS_DOSDOIO_H
#define DOS_DOSDOIO_H

#include <exec/io.h>


BYTE DosDoIO(struct IORequest * iORequest,
	     struct ExecBase * SysBase);

#define DosDoIO(iorequest) DosDoIO(iorequest, SysBase)


#endif /* DOS_DOSDOIO_H */
