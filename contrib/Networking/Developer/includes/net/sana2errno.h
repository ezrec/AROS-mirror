#ifndef NET_SANA2ERRNO_H
#define NET_SANA2ERRNO_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

extern const short io_nerr;
extern const char * const io_errlist[]; 
extern const short sana2io_nerr;
extern const char * const sana2io_errlist[];
extern const short sana2wire_nerr;
extern const char * const sana2wire_errlist[];

void sana2perror(const char *banner, struct IOSana2Req *ios2);
void Sana2PrintFault(const char *banner, struct IOSana2Req *ios2);

#endif
