/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef AMIGA_ERRLISTS_H
#define AMIGA_ERRLISTS_H

extern const char * const __sys_errlist[];
extern const int __sys_nerr;

extern const char * const h_errlist[];
extern const int h_nerr;

extern const char * const sana2io_errlist[];
extern const short sana2io_nerr;

extern const char * const io_errlist[];
extern const short io_nerr;

extern const char * const sana2wire_errlist[];
extern const short sana2wire_nerr;

#endif /* AMIGA_ERRLISTS_H */
