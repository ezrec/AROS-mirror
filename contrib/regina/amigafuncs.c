/*
 *  Amiga REXX functions for regina
 *  Copyright © 2002, Staf Verhaegen
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "rexx.h"
#include <stdio.h>
#include <ctype.h>

typedef struct _amiga_tsd_t {
  proclevel amilevel;
} amiga_tsd_t;

static proclevel setamilevel( tsd_t *TSD )
{
  amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
  proclevel oldlevel = TSD->currlevel;
  
  if (atsd->amilevel==NULL)
  {
    atsd->amilevel = newlevel( TSD, NULL );
  }

  TSD->currlevel = atsd->amilevel;
  
  return oldlevel;
}

static FILE *getfile( tsd_t *TSD, const streng *name )
{
  proclevel oldlevel = setamilevel( TSD );
  const streng *s;
  char *txt;
  FILE *file=NULL;

  if ( isvariable( TSD, name ) )
  {
    s = getvalue( TSD, name, 0 );
    txt = str_of( TSD, s );
    sscanf( txt, "%p", &file );
    FreeTSD( txt );
  }

  TSD->currlevel = oldlevel;
  
  return file;
}

static void addfile( tsd_t *TSD, const streng *name, FILE *file )
{
  proclevel oldlevel = setamilevel( TSD );
  char txt[20];
  streng *s;

  sprintf( txt, "%p", (void *)file );
  s = Str_cre_TSD( TSD, txt );
  setvalue( TSD, name, s );
  
  TSD->currlevel = oldlevel;
}

static void rmfile( tsd_t *TSD, const streng *name )
{
  amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
  proclevel oldlevel = setamilevel( TSD );
  
  TSD->currlevel = atsd->amilevel;
  
  drop_var( TSD, name );
  
  TSD->currlevel = oldlevel;
}

/* Init amiga specific thread data */
int init_amigaf ( tsd_t *TSD )
{
  amiga_tsd_t *atsd = MallocTSD( sizeof(amiga_tsd_t) );
  
  if (atsd==NULL) return 0;

  /* Allocate later because systeminfo is not initialized at the moment */
  atsd->amilevel = NULL;
  TSD->ami_tsd = (void *)atsd;

  return 1;
}

/* Open a file */
streng *arexx_open ( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2, parm3;
  char *filename;
  FILE *file;
  int mode;
  static const char* modestrings[] = {
    "w",
    "r+",
    "a"
  };

  checkparam( parm1, 2, 3, "OPEN" );
  parm2 = parm1->next;
  parm3 = parm2->next;
  
  file = getfile( TSD, parm1->value );
  if ( file!=NULL )
  {
    return int_to_streng( TSD, 0 );
  }

  filename = str_of( TSD, parm2->value );
  
  if ( parm3==NULL
       || parm3->value==NULL
       || parm3->value->len==0
       || toupper(parm3->value->value[0])=='W' )
    mode=0;
  else if ( toupper(parm3->value->value[0])=='R' )
    mode=1;
  else if ( toupper(parm3->value->value[0])=='A' )
    mode=2;
  else
    exiterror( ERR_INCORRECT_CALL, 0 );

  file = fopen( filename, modestrings[mode] );
  FreeTSD( filename );

  if ( file==NULL )
  {
    return int_to_streng( TSD, 0 );
  }
  
  addfile( TSD, parm1->value, file );
  return int_to_streng( TSD, 1);
}

streng *arexx_close( tsd_t *TSD, cparamboxptr parm1 )
{
  FILE *file;
  
  checkparam( parm1, 1, 1, "CLOSE" );
  
  file = getfile( TSD, parm1->value );
  if ( file==NULL )
    return int_to_streng( TSD, 0 );

  fclose( file );
  rmfile( TSD, parm1->value );
  
  return int_to_streng( TSD, 1 );
}

streng *arexx_writech( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  char *txt;
  FILE *file;
  int count;
  
  checkparam( parm1, 2, 2, "WRITECH" );
  parm2 = parm1->next;

  file = getfile( TSD, parm1->value );
  if ( file==NULL )
    exiterror( ERR_INCORRECT_CALL, 0);
  
  txt = str_of( TSD, parm2->value );
  count = fprintf( file, "%s", txt );
  FreeTSD( txt );
  
  return int_to_streng( TSD, count );
}

streng *arexx_writeln( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  char *txt;
  FILE *file;
  int count;
  
  checkparam( parm1, 2, 2, "WRITELN" );
  parm2 = parm1->next;

  file = getfile( TSD, parm1->value );
  if ( file==NULL )
    exiterror( ERR_INCORRECT_CALL, 0 );

  txt = str_of( TSD, parm2->value );
  count = fprintf(file, "%s\n", txt);
  FreeTSD( txt );

  return int_to_streng( TSD, count );
}

streng *arexx_seek( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2, parm3;
  FILE *file;
  int pos, error, wench;
  long offset;
  
  checkparam( parm1, 2, 3, "SEEK" );
  parm2 = parm1->next;
  parm3 = parm2->next;
  
  file = getfile( TSD, parm1->value );
  if ( file==NULL )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  offset = streng_to_int( TSD, parm2->value, &error );
  if (error)
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  if ( parm3==NULL || parm3->value==NULL ||
       parm3->value->len == 0 ||
       toupper(parm3->value->value[0])=='C' )
    wench = SEEK_CUR;
  else if ( toupper( parm3->value->value[0] )=='B' )
    wench = SEEK_SET;
  else if ( toupper( parm3->value->value[0] )=='E' )
    wench = SEEK_END;
  else
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  pos = fseek( file, offset, wench );
  return int_to_streng( TSD, pos );
}

streng *arexx_readch( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  FILE *file;
  
  checkparam( parm1, 1, 2, "READCH");
  parm2 = parm1->next;
  
  file = getfile( TSD, parm1->value );
  if ( file==NULL )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  if ( parm2==NULL )
  {
    char buffer[2] = { 0, 0 };
    
    buffer[0] = getc( file );
    
    return Str_cre_TSD( TSD, buffer );
  }
  else
  {
    int count, error;
    char *buffer;
    streng *ret;
    
    count = streng_to_int( TSD, parm2->value, &error );
    
    if ( error )
      exiterror( ERR_INVALID_INTEGER, 0 );
    
    buffer = malloc( count + 1 );

    count = fread( buffer, 1, count, file );
    buffer[count + 1] = 0;
    
    ret = Str_cre_TSD( TSD, buffer );
    free(buffer);
    return ret;
  }
}

streng *arexx_readln( tsd_t *TSD, cparamboxptr parm )
{
  FILE *file;
  char buffer[1001];
  
  checkparam( parm, 1, 1, "READLN");
  
  file = getfile( TSD, parm->value );
  if ( file==NULL )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  fgets( buffer, 1001, file );
  if ( buffer[strlen(buffer)-1]=='\n' )
    buffer[strlen(buffer)-1]=0;
  
  return Str_cre_TSD( TSD, buffer );
}

streng *arexx_eof( tsd_t *TSD, cparamboxptr parm )
{
  FILE *file;
  
  checkparam( parm, 1, 1, "EOF" );
  
  file = getfile( TSD, parm->value );
  if ( file==NULL )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  return int_to_streng( TSD, feof( file )!=0 );
}
