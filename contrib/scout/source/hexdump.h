#ifndef HEXDUMP_H
#define HEXDUMP_H 1

void khexdump( void *buffer,
               LONG size );

void fhexdump( BPTR fh,
               void *buffer,
               LONG size );

#endif /* HEXDUMP_H */

