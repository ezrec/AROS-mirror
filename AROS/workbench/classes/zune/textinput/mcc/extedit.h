/*
** $Id$
*/

APTR ee_add( char *filename, APTR obj, int async );
void ee_free( struct exteditinfonode *ee );
void ee_setobj( struct exteditinfonode *ee, APTR obj );
void ee_restart( struct exteditinfonode *ee );
