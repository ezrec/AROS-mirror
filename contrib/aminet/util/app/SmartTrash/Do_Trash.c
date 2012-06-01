
//************************
//
// Name : Do_Trash.c
//
// Ver 2.3
//
//************************

//************************ FORWARD DECLARACTIONS

#ifndef DO_TRASH_H
#include "Do_Trash.h"
#endif

//************************ EXISTS STUFF


//** Decide what to rename the file as
void FindSlot(char *buffer, char *fname, BPTR trash) {
  char tbuf[2][FNAMESIZE];
  BPTR oldp=CurrentDir(trash);

  while ( (ExistsFile(fname)) || (ExistsInfo(fname)) ) {
    if (fname==tbuf[0]) {
      BumpRevision(tbuf[1],fname);
      fname=tbuf[1];
    } else {
      BumpRevision(tbuf[0],fname);
      fname=tbuf[0];
    }
  }

  AddPart( buffer, fname, BUFSIZE );
  CurrentDir(oldp);
} // FindSlot


//** Does this file exist?
BOOL ExistsFile(char *file) {
  BPTR lk;

  if (lk=Lock(file,SHARED_LOCK)) {
    UnLock(lk);
    return TRUE;
  }
  return FALSE;
} // ExistsFile


//** Is there an icon with this name?
BOOL ExistsInfo(char *file) {
  struct DiskObject *dobj;

  if (dobj=GetDiskObject(file)) {
    FreeDiskObject( dobj );
    return TRUE;
  }
  return FALSE;
} // ExistsInfo


//************************ MOVE STUFF


//** Decide which drawer (if any) to use, returning a LOCK on it
BPTR FigureTrash(char *dest) {
  BPTR lk;
  char *path;

  for ( path=TrashPath ; path=tok(dest,path) ; ) {
    if (lk=AttemptCD(dest)) return lk;
  }

  for ( path=TrashPath ; path=tok(dest,path) ; ) {
    if (lk=AttemptND(dest)) return lk;
  }

  return BNULL;
} // FigureTrash


//** Is this path accessible?
BPTR AttemptCD(char *dir) {
  struct FileInfoBlock fib;
  BPTR lk;

  if (lk=Lock(dir,SHARED_LOCK)) {
    Examine(lk,&fib);
    if (fib.fib_DirEntryType>=0) {
      DEBUGMSG("AttemptCD '%s' OK\n",(APTR)dir);
      return lk;
    }
    UnLock(lk);
  }
  DEBUGMSG("AttemptCD '%s' FAIL\n",(APTR)dir);
  return BNULL;
} // AttemptCD


//** Create a directory and icon
BPTR AttemptND(char *dir) {
  struct DiskObject *dobj;
  BPTR lk;

  if (lk=CreateDir(dir)) {
    if (dobj=GetDefDiskObject(WBGARBAGE)) {
      PutDiskObject( dir, dobj );
      FreeDiskObject( dobj );
    }
    DEBUGMSG("AttemptND '%s' OK\n",(APTR)dir);
    return lk;
  } // if CreateDir
  DEBUGMSG("AttemptND '%s' FAIL\n",(APTR)dir);
  return BNULL;
} // AttemptND


//************************

//** Tokenises sour, stuffing the tokens into dest
char *tok(char *dest, char *sour) {
  register char *d=dest;
  register char *s=sour;

  while ((*s==';') || (*s==' ')) s++;	// skip semicolons and spaces
  if ((*s=='\n') || (*s==0)) return 0L;
  while ((*s!=';') && (*s!='\n') && (*s!=0)) *d++=*s++;
  *d=0;
  return s;
} // tok

//************************ END OF FILE
