/*
** $Id$
*/

int dobuffersize( struct Data *, int );
void freeaction( struct Data *, struct actionnode * );
char *doaction( struct Data *, ULONG, char *, int, char *, int );
char *undoaction( struct Data *, struct actionnode * );
