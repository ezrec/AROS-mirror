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
#include <stdlib.h>
#include <assert.h>

#if defined(_AMIGA) || defined(__AROS__)
#include "envir.h"
#include <exec/lists.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <rexx/rxslib.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/rexxsyslib.h>

struct amiga_envir {
  struct envir envir;
  struct MsgPort *port;
};

#endif

typedef struct _amiga_tsd_t {
  proclevel amilevel;
#if defined(_AMIGA) || defined(__AROS__)
  struct amiga_envir portenvir;
  struct RxsLib *rexxsysbase;
#endif
} amiga_tsd_t;

#define RexxSysBase (((amiga_tsd_t *)TSD->ami_tsd)->rexxsysbase)

/* Init amiga specific thread data, this function is called during initialisation
 * of the thread specific data
 */
int init_amigaf ( tsd_t *TSD )
{
  amiga_tsd_t *atsd = MallocTSD( sizeof(amiga_tsd_t) );
  
  if (atsd==NULL) return 0;

  /* Allocate later because systeminfo is not initialized at the moment */
  atsd->amilevel = NULL;
#if defined(_AMIGA) || defined(__AROS__)
  atsd->portenvir.envir.name = NULL;
  atsd->portenvir.envir.type = ENVIR_AMIGA;
  atsd->rexxsysbase = (struct RxsLib *)OpenLibrary( "rexxsyslib.library", 44 );
  if ( atsd->rexxsysbase == NULL )
    return 0;
#endif
  TSD->ami_tsd = (void *)atsd;

  return 1;
}


/*
 * Support function for exec lists
 */
static streng *getlistnames( tsd_t *TSD, struct List *list, const streng *sep )
{
  int first = 1;
  struct Node *ln;
  streng *retval, *tmpstr;

  ForeachNode( list, ln )
  {
    if ( first )
    {
      retval = Str_cre_TSD( TSD, ln->ln_Name );
      first = 0;
    }
    else
    {
      tmpstr = Str_cat_TSD( TSD, retval, sep );
      if ( tmpstr != retval )
      {
	Free_string_TSD( TSD, retval );
	retval = tmpstr;
      }
      tmpstr = Str_catstr_TSD( TSD, retval, ln->ln_Name );
      if ( tmpstr != retval )
      {
	Free_string_TSD( TSD, retval );
	retval = tmpstr;
      }
    }
  }

  return retval;
}

/*
 * Support functions for the ARexx IO functions
 */
/* setamilevel will change the environment to the variables used for open files */
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


/* getfile will return the FILE pointer of given name */
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


/* getfilenames will return a list of all opened files */
static streng *getfilenames( tsd_t *TSD, const streng *sep )
{
  proclevel oldlevel = setamilevel( TSD );
  streng *retval, *tmpstr;
  int first = 1;
  variableptr var;
  char *test;

  get_next_variable( TSD, 1 );
  for( var = get_next_variable( TSD, 0);
       var != NULL;
       var = get_next_variable( TSD, 0) )
  {
    while ( var != NULL && var->realbox != NULL )
      var = var->realbox;

    if ( var != NULL && ( (var->flag & (VFLAG_STR | VFLAG_NUM)) || var->stem ) )
    {
      if ( first )
      {
	retval = Str_dup_TSD( TSD, var->name );
	first = 0;
      }
      else
      {
	tmpstr = Str_cat_TSD( TSD, retval, sep );
	if ( tmpstr != retval )
	{
	  Free_string_TSD( TSD, retval );
	  retval = tmpstr;
	}
	tmpstr = Str_cat_TSD( TSD, retval, var->name );
	if ( tmpstr != retval )
	{
	  Free_string_TSD( TSD, retval );
	  retval = tmpstr;
	}
      }
    }
  }

  TSD->currlevel = oldlevel;

  /* If no variable present return NULL string */
  if (first)
    retval = nullstringptr();
  
  return retval;
}

/* addfile: store the FILE pointer in a given name */
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


/* rmfile: remove a given of open files list */
static void rmfile( tsd_t *TSD, const streng *name )
{
  amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
  proclevel oldlevel = setamilevel( TSD );
  
  TSD->currlevel = atsd->amilevel;
  
  drop_var( TSD, name );
  
  TSD->currlevel = oldlevel;
}


/*
 * Implementation of the ARexx IO functions
 * See general documentation for more information
 * Functions implemented: OPEN, CLOSE, READCH, READLN, WRITECH, WRITELN, EOF, SEEK
 */
streng *arexx_open( tsd_t *TSD, cparamboxptr parm1 )
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
       || parm3->value->len==0 )
    mode=0;
  else switch( getoptionchar( TSD, parm3->value, "OPEN", 3, "WRA" ) )
  {
  case 'W':
    mode=0;
    break;
    
  case 'R':
    mode=1;
    break;
      
  case 'A':
    mode=2;
    break;
      
  default:
    mode=0;
    assert(0);
    break;
  }

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
    exiterror( ERR_INCORRECT_CALL, 27, "WRITECH", tmpstr_of( TSD, parm1->value ));
  
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
    exiterror( ERR_INCORRECT_CALL, 27, "WRITELN", tmpstr_of( TSD, parm1->value ) );

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
    exiterror( ERR_INCORRECT_CALL, 27, "SEEK", tmpstr_of( TSD, parm1->value ) );
  
  offset = streng_to_int( TSD, parm2->value, &error );
  if (error)
    exiterror( ERR_INCORRECT_CALL, 11, "SEEK", 2, tmpstr_of( TSD, parm2->value ) );
  
  if ( parm3==NULL
       || parm3->value==NULL
       || parm3->value->len == 0 )
    wench = SEEK_CUR;
  else switch( getoptionchar( TSD, parm3->value, "SEEK", 3, "CBE" ) )
  {
  case 'C':
    wench = SEEK_CUR;
    break;
      
  case 'B':
    wench = SEEK_SET;
    break;
      
  case 'E':
    wench = SEEK_END;
    break;
      
  default:
    wench = SEEK_CUR;
    assert(0);
    break;
  }
  
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
    exiterror( ERR_INCORRECT_CALL, 27, "READCH", tmpstr_of( TSD, parm1->value ) );
  
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
      exiterror( ERR_INCORRECT_CALL, 11, "READCH", 2, tmpstr_of( TSD, parm2->value ) );
    if ( count<=0 )
      exiterror( ERR_INCORRECT_CALL, 14, "READCH", 2, tmpstr_of( TSD, parm2->value ) );
    
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
    exiterror( ERR_INCORRECT_CALL, 27, "READLN", tmpstr_of( TSD, parm->value ) );
  
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
    exiterror( ERR_INCORRECT_CALL, 27, "EOF", tmpstr_of( TSD, parm->value ) );
  
  return int_to_streng( TSD, feof( file )!=0 );
}


/* 
 * Implementation of the additional conversion functions from ARexx
 * Functions: B2C, C2B
 */
streng *arexx_b2c( tsd_t *TSD, cparamboxptr parm )
{
  parambox parm2;
  streng *ret;
  
  checkparam( parm, 1, 1, "B2C" );

  parm2.next = NULL;
  parm2.value = std_b2x( TSD, parm );
  
  ret = std_x2c( TSD, &parm2 );
  Free_string_TSD( TSD, parm2.value );
  
  return ret;
}


streng *arexx_c2b( tsd_t *TSD, cparamboxptr parm )
{
  parambox parm2;
  streng *ret;
  
  checkparam( parm, 1, 1, "B2C" );

  parm2.next = NULL;
  parm2.value = std_c2x( TSD, parm );
  
  ret = std_x2b( TSD, &parm2 );
  Free_string_TSD( TSD, parm2.value );
  
  return ret;
}


/*
 * Implementation of the bitwise function from ARexx
 * Functions: BITCHG, BITCLR, BITSET, BITTST, BITCOMP
 */
streng *arexx_bitchg( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  streng *ret;
  int bit, error, byte;
  div_t dt;
  
  checkparam( parm1, 2, 2, "BITCHG" );
  parm2 = parm1->next;
  
  bit = streng_to_int( TSD, parm2->value, &error );
  if ( error )
    exiterror( ERR_INCORRECT_CALL, 11, "BITCHG", 2, tmpstr_of( TSD, parm2->value ) );
  if ( bit<0 )
    exiterror( ERR_INCORRECT_CALL, 13, "BITCHG", 2, tmpstr_of( TSD, parm2->value ) );
  
  dt = div( bit, 8 );

  byte = parm1->value->len-dt.quot-1;
  if ( byte<0 )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  ret = Str_dup_TSD( TSD, parm1->value );
  ret->value[byte]^=(char)(1<<dt.rem);
  return ret;
}


streng *arexx_bitclr( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  streng *ret;
  int bit, error, byte;
  div_t dt;
  
  checkparam( parm1, 2, 2, "BITCLR" );
  parm2 = parm1->next;
  
  bit = streng_to_int( TSD, parm2->value, &error );
  if ( error )
    exiterror( ERR_INCORRECT_CALL, 11, "BITCLR", 2, tmpstr_of( TSD, parm2->value ) );
  if ( bit<0 )
    exiterror( ERR_INCORRECT_CALL, 13, "BITCLR", 2, tmpstr_of( TSD, parm2->value ) );
  
  dt = div( bit, 8 );
  
  byte = parm1->value->len-dt.quot-1;
  if ( byte<0 )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  ret = Str_dup_TSD( TSD, parm1->value );
  ret->value[byte]&=~(char)(1<<dt.rem);
  return ret;
}


streng *arexx_bitset( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  streng *ret;
  int bit, error, byte;
  div_t dt;
  
  checkparam( parm1, 2, 2, "BITSET" );
  parm2 = parm1->next;
  
  bit = streng_to_int( TSD, parm2->value, &error );
  if ( error )
    exiterror( ERR_INCORRECT_CALL, 11, "BITSET", 2, tmpstr_of( TSD, parm2->value ) );
  if ( bit<0 )
    exiterror( ERR_INCORRECT_CALL, 13, "BITSET", 2, tmpstr_of( TSD, parm2->value ) );
  
  dt = div( bit, 8 );
  
  byte = parm1->value->len-dt.quot-1;
  if ( byte<0 )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  ret = Str_dup_TSD( TSD, parm1->value );
  ret->value[byte]|=(char)(1<<dt.rem);
  return ret;
}


streng *arexx_bittst( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  streng *ret;
  int bit, error, byte;
  div_t dt;
  
  checkparam( parm1, 2, 2, "BITTST" );
  parm2 = parm1->next;
  
  bit = streng_to_int( TSD, parm2->value, &error );
  if ( error )
    exiterror( ERR_INCORRECT_CALL, 11, "BITTST", 2, tmpstr_of( TSD, parm2->value ) );
  if ( bit<0 )
    exiterror( ERR_INCORRECT_CALL, 13, "BITTST", 2, tmpstr_of( TSD, parm2->value ) );
  
  dt = div( bit, 8 );
  
  byte = parm1->value->len-dt.quot-1;
  if ( byte<0 )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  ret = int_to_streng( TSD, (parm1->value->value[byte] & (char)(1<<dt.rem))!=0 );
  return ret;
}


/* Help function for arexx_bitcomp */
static int firstbit(char c)
{
  int i;
  assert(c!=0);
  
  for( i=0; i<8; i++)
  {
    if (c & 1)
      return i;
    else
      c = c >> 1;
  }
  
  return 8;
}

/* This ARexx function has very weird usage of the pad byte,
 * the shortest string is padded on the left with this byte
 */
streng *arexx_bitcomp( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2, parm3;
  const streng *s1, *s2;
  const char *cp1, *cp2;
  char pad;
  int i;
  
  checkparam( parm1, 2, 3, "BITCOMP" );
  parm2 = parm1->next;
  
  /* Make s2 always shorter or equal to s1 */
  if( parm1->value->len < parm2->value->len )
  {
    s1 = parm2->value;
    s2 = parm1->value;
  } else {
    s1 = parm1->value;
    s2 = parm2->value;
  }
  
  for( cp1=s1->value+s1->len-1, cp2=s2->value+s2->len-1, i=0;
       cp2 >= s2->value;
       cp1--, cp2--, i++ )
  {
    if ( *cp1 != *cp2 )
      return int_to_streng( TSD, i*8 + firstbit( *cp1 ^ *cp2 ) );
  }
  
  parm3 = parm2->next;
  if ( parm3==NULL || parm3->value==NULL || parm3->value->len==0 )
    pad = 0;
  else
    pad = parm3->value->value[0];
  
  for ( ;
	cp1 >= s1->value;
	cp1--, i++ )
  {
    if ( *cp1 != pad )
      return int_to_streng( TSD, i*8 + firstbit( *cp1 ^ pad ) );
  }
  
  return int_to_streng( TSD, -1 );
}


/*
 * Some more misc. ARexx functions
 * Functions: COMPRESS, HASH, RANDU, TRIM, UPPER
 */
streng *arexx_hash( tsd_t *TSD, cparamboxptr parm1 )
{
  unsigned char *uc;
  int i, sum=0;
  
  checkparam( parm1, 1, 1, "HASH" );

  uc = (unsigned char *)parm1->value->value;
  for( i=0; i<parm1->value->len; i++)
  {
    sum = (sum + uc[i]) & 255;
  }
  
  return int_to_streng( TSD, sum );
}


streng *arexx_compress( tsd_t *TSD, cparamboxptr parm1 )
{
  const char *match;
  int i, start;
  streng *ret;
  
  checkparam( parm1, 1, 2, "COMPRESS" );

  match = ( parm1->next!=NULL ) ? str_of( TSD, parm1->next->value ) : " ";

  ret = Str_dup_TSD( TSD, parm1->value );
  for( i=start=0; i<ret->len; i++ )
  {
    /* Copy char if not found */
    if ( strchr( match, ret->value[i] )==NULL )
    {
      ret->value[start] = ret->value[i];
      start++;
    }
  }
  ret->len = start;

  if ( parm1->next!=NULL )
    FreeTSD( (char *)match );
  
  return ret;
}


static const streng T_str = { 1, 1, "T" };
static const parambox T_parm = { NULL, 0, (streng *)&T_str };

streng *arexx_trim( tsd_t *TSD, cparamboxptr parm1 )
{
  parambox parm;
  
  checkparam( parm1, 1, 1, "TRIM" );

  parm = *parm1;
  parm.next = (paramboxptr)&T_parm;
  
  return std_strip( TSD, parm1 );
}


streng *arexx_upper( tsd_t *TSD, cparamboxptr parm1 )
{
  streng *ret;
  
  checkparam( parm1, 1, 1, "UPPER" );
  
  ret = Str_dup_TSD( TSD, parm1->value );

  return Str_upper( ret );
}


streng *arexx_randu( tsd_t *TSD, cparamboxptr parm1 )
{
  int error, seed;
  char text[30];
  streng *s, *retval;
  
  checkparam( parm1, 0, 1, "RANDU" );
  
  if ( parm1!=NULL && parm1->value!=NULL )
  {
    seed = streng_to_int( TSD, parm1->value, &error );
    if ( error )
      exiterror( ERR_INCORRECT_CALL, 11, "RANDU", 1, tmpstr_of( TSD, parm1->value ) );
    
    srand48( (long int)seed );
  }
  
  sprintf( text, "%.20f", drand48() );
  s = Str_cre_TSD( TSD, text );
  retval = str_format( TSD, s, -1, -1, -1, -1);
  FreeTSD( s );
  
  return retval;
}



/*
 * Two memory allocation/deallocation functions: getspace and freespace
 */
streng *arexx_getspace( tsd_t *TSD, cparamboxptr parm1 )
{
  int length, error;
  void *ptr;
  
  checkparam( parm1, 1, 1, "GETSPACE" );
  
  length = streng_to_int( TSD, parm1->value, &error);
  if ( error )
    exiterror( ERR_INCORRECT_CALL, 11, "READCH", 1, tmpstr_of( TSD, parm1->value ) );
  if ( length<=0 )
    exiterror( ERR_INCORRECT_CALL, 14, "READCH", 1, tmpstr_of( TSD, parm1->value ) );

  ptr = Malloc_TSD( TSD, length );
  memset( ptr, 0, length );
  if ( ptr == NULL )
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  
  return Str_ncre_TSD( TSD, (char *)&ptr, sizeof(void *) );
}


streng *arexx_freespace( tsd_t *TSD, cparamboxptr parm1 )
{
  /* For backwards compatibility there may be two arguments
     But the second argument is ignored in regina */
  checkparam( parm1, 0, 2, "FREESPACE" );
  
  if ( parm1 == NULL || parm1->value == NULL || parm1->value->len == 0 )
#if defined(_AMIGA) || defined(__AROS__)
    return int_to_streng( TSD, AvailMem( MEMF_ANY ) );
#else
    return int_to_streng( -1 );
#endif
  
  if ( parm1->value->len != sizeof(void *) )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  Free_TSD( TSD, *((void **)parm1->value->value) );
  
  return nullstringptr();
}




/*
 * ARexx memory <-> string conversion routines: IMPORT, EXPORT, STORAGE
 */
streng *arexx_import( tsd_t *TSD, cparamboxptr parm1 )
{
  void *memptr;
  cparamboxptr parm2;
  int len, error;
  
  checkparam( parm1, 1, 2, "IMPORT" );
  
  if ( parm1->value->len != sizeof(void *) )
    exiterror( ERR_INCORRECT_CALL, 0 );
  
  memptr = *((void **)parm1->value->value);

  parm2 = parm1->next;
  if ( parm2 == NULL || parm2->value == NULL || parm2->value->len == 0 )
    len = strlen((char *)memptr);
  else
  {
    len = streng_to_int( TSD, parm2->value, &error );
    if ( error )
      exiterror( ERR_INCORRECT_CALL, 11, "IMPORT", 2, tmpstr_of( TSD, parm2->value ) );
    if ( len<=0 )
      exiterror( ERR_INCORRECT_CALL, 14, "IMPORT", 2, tmpstr_of( TSD, parm2->value ) );
  }
  
  return Str_ncre_TSD( TSD, memptr, len );
}


streng *arexx_export( tsd_t *TSD, cparamboxptr parm1 )
{
  void *memptr;
  cparamboxptr parm2 = NULL, parm3 = NULL, parm4 = NULL;
  int len, error;
  char fill;
  streng *src;
  
  checkparam( parm1, 1, 4, "EXPORT" );

  if ( parm1->value == NULL || parm1->value->len == 0 )
    exiterror( ERR_INCORRECT_CALL, 21, "EXPORT", 1 );
  memptr = *((void **)parm1->value->value);
  
  parm2 = parm1->next;
  if ( parm2 != NULL )
    parm3 = parm2->next;
  if ( parm3 != NULL )
    parm4 = parm3->next;
  
  if ( parm2 == NULL || parm2->value == NULL || parm2->value->len == 0 )
    src = nullstringptr();
  else
    src = Str_dup_TSD( TSD, parm2->value );
  
  if ( parm3 == NULL || parm3->value == NULL || parm3->value->len == 0 )
    len = src->len;
  else
  {
    len = streng_to_int( TSD, parm3->value, &error );
    if ( error )
      exiterror( ERR_INCORRECT_CALL, 11, "EXPORT", 3, tmpstr_of( TSD, parm3->value ) );
    if ( len<0 )
      exiterror( ERR_INCORRECT_CALL, 13, "EXPORT", 3, tmpstr_of( TSD, parm3->value ) );
  }
  
  if ( parm4 == NULL || parm4->value == NULL || parm4->value->len == 0 )
    fill = 0;
  else
    fill = parm4->value->value[0];
  
  if (len > src->len)
  {
    memcpy( memptr, src->value, src->len );
    memset( memptr+src->len, fill, len - src->len );
  }
  else
    memcpy( memptr, src->value, len );

  Free_string_TSD( TSD, src );
  
  return int_to_streng( TSD, len );
}


streng *arexx_storage( tsd_t *TSD, cparamboxptr parm1 )
{
  void *memptr;
  cparamboxptr parm2 = NULL, parm3 = NULL, parm4 = NULL;
  int len, error;
  char fill;
  streng *src, *retval;
  
  checkparam( parm1, 0, 4, "STORAGE" );

  if ( parm1 == NULL )
    return arexx_getspace( TSD, NULL );

  if ( parm1->value == NULL || parm1->value->len == 0 )
    exiterror( ERR_INCORRECT_CALL, 21, "STORAGE", 1 );
  memptr = *((void **)parm1->value->value);
  
  parm2 = parm1->next;
  if ( parm2 != NULL )
    parm3 = parm2->next;
  if ( parm3 != NULL )
    parm4 = parm3->next;
  
  if ( parm2 == NULL || parm2->value == NULL || parm2->value->len == 0 )
    src = nullstringptr();
  else
    src = Str_dup_TSD( TSD, parm2->value );
  
  if ( parm3 == NULL || parm3->value == NULL || parm3->value->len == 0 )
    len = src->len;
  else
  {
    len = streng_to_int( TSD, parm3->value, &error );
    if ( error )
      exiterror( ERR_INCORRECT_CALL, 11, "EXPORT", 3, tmpstr_of( TSD, parm3->value ) );
    if ( len<0 )
      exiterror( ERR_INCORRECT_CALL, 13, "EXPORT", 3, tmpstr_of( TSD, parm3->value ) );
  }
  
  if ( parm4 == NULL || parm4->value == NULL || parm4->value->len == 0 )
    fill = 0;
  else
    fill = parm4->value->value[0];

  retval = Str_ncre_TSD( TSD, memptr, len );
  
  if (len > src->len)
  {
    memcpy( memptr, src->value, src->len );
    memset( memptr+src->len, fill, len - src->len );
  }
  else
    memcpy( memptr, src->value, len );

  Free_string_TSD( TSD, src );
  
  return retval;
}



/*
 * SHOW a function the names available in different resource lists
 */
streng *arexx_show( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2 = NULL, parm3 = NULL;
  streng *name = NULL, *sep, *retval;
  
  checkparam( parm1, 1, 3, "SHOW" );
  parm2 = parm1->next;
  if ( parm2 != NULL )
    parm3 = parm2->next;

  if ( parm2 != NULL && parm2->value != NULL && parm2->value->len != 0 )
    name = parm2->value;
  
  if ( parm3 == NULL || parm3->value == NULL || parm3->value->len == 0 )
    sep = Str_cre_TSD( TSD, " " );
  else
    sep = Str_dup_TSD( TSD, parm3->value );
  
#if defined(_AMIGA) || defined(__AROS__)
  switch( getoptionchar( TSD, parm1->value, "SHOW", 1, "CFLP" ) )
#else
  switch( getoptionchar( TSD, parm1->value, "SHOW", 1, "F" ) )
#endif
  {
  case 'F':
    if ( name == NULL )
      retval = getfilenames( TSD, sep );
    else
    {
      FILE *f = getfile( TSD, name );
      retval = int_to_streng( TSD, f != NULL );
    }
    break;
    
#if defined(_AMIGA) || defined(__AROS__)
  case 'C':
    LockRexxBase( 0 );
    if ( name == NULL )
      retval = getlistnames( TSD, &RexxSysBase->rl_ClipList, sep );
    else
    {
      char *s = str_of( TSD, name );
      struct Node *ln = FindName( &RexxSysBase->rl_ClipList, s );
      retval = int_to_streng( TSD, ln != NULL );
      Free_TSD( TSD, s );
    }
    UnlockRexxBase( 0 );
    break;
    
  case 'L':
    LockRexxBase( 0 );
    if ( name == NULL )
      retval = getlistnames( TSD, &RexxSysBase->rl_LibList, sep );
    else
    {
      char *s = str_of( TSD, name );
      struct Node *ln = FindName( &RexxSysBase->rl_LibList, s );
      retval = int_to_streng( TSD, ln != NULL );
      Free_TSD( TSD, s );
    }
    UnlockRexxBase( 0 );
    break;
    
  case 'P':
    Forbid();
    if ( name == NULL )
      retval = getlistnames( TSD, &SysBase->PortList, sep );
    else
    {
      char *s = str_of( TSD, name );
      struct Node *ln = FindName( &SysBase->PortList, s );
      retval = int_to_streng( TSD, ln != NULL );
      Free_TSD( TSD, s );
    }
    Enable();
    break;
#endif
  }
  Free_string_TSD( TSD, sep );
  
  return retval;
}


/* Now the function are given which are only valid in amiga or compatible
 * platforms (e.g. AROS, ... )
 */
#if defined(_AMIGA) || defined(__AROS__)

#include <proto/dos.h>
#include <rexx/storage.h>

#include "rxiface.h"

struct envir *amiga_find_envir( const tsd_t *TSD, const streng *name )
{
  amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
  char *s;
  struct MsgPort *port; 
  
  s = str_of( TSD, name );
  port = FindPort( s );
  FreeTSD( s );
  
  if (port == NULL)
    return NULL;

  if ( atsd->portenvir.envir.name != NULL )
    Free_stringTSD( atsd->portenvir.envir.name );
    
  atsd->portenvir.envir.name = Str_dupTSD( name );
  atsd->portenvir.port = port;

  return (struct envir *)&(atsd->portenvir);
}

streng *AmigaSubCom( const tsd_t *TSD, const streng *command, struct envir *envir, int *rc )
{
  struct RexxMsg *msg;
  struct MsgPort *port = ((struct amiga_envir *)envir)->port, *replyport;
  
  replyport = CreatePort( NULL, 0 );
  msg = CreateRexxMsg( port, NULL, NULL );
  msg->rm_Action = RXCOMM;
  msg->rm_Args[0] = (IPTR)CreateArgstring( (STRPTR)command->value, command->len );
  fflush(stdout);
  msg->rm_Stdin = Input();
  msg->rm_Stdout = Output();
  PutMsg(port, (struct Message *)msg);

  *rc = RXFLAG_OK;
  DeleteRexxMsg( msg );
  return Str_crestrTSD( "Message sent and replied" );
}


/*
 * Here follows now the support function for ARexx style function hosts and libraries:
 * addlib and remlib.
 * Also here the try_func_amiga is defined which is called when a function is called
 * in an ARexx script.
 */

/* When addlib is called with two arguments the first argument is considered as a function
 * host name. When it is called with three or four arguments a function library is assumed
 */
streng *arexx_addlib( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2 = NULL, parm3 = NULL, parm4 = NULL;
  struct MsgPort *replyport, *rexxport;
  struct RexxMsg *msg;
  int pri, offset, version, error, count;
  char *name;
  streng *retval;
  
  checkparam( parm1, 2, 4, "ADDLIB" );
  parm2 = parm1->next;
  pri = streng_to_int( TSD, parm2->value, &error );
  if (error || abs(pri) > 100 )
    exiterror( ERR_INCORRECT_CALL, 11, "ADDLIB", 2, tmpstr_of( TSD, parm2->value ) );
  
  parm3 = parm2->next;
  if ( parm3 != NULL && parm3->value != NULL && parm3->value->len == 0 )
    exiterror( ERR_INCORRECT_CALL, 21, "ADDLIB", 3 );
  if ( parm3 == NULL || parm3->value == NULL )
    offset = -30;
  else
  {
    offset = streng_to_int( TSD, parm2->value, &error );
    if ( error || offset >= 0 )
      exiterror( ERR_INCORRECT_CALL, 11, "ADDLIB", 3, tmpstr_of( TSD, parm3->value ) );
  }
  
  if ( parm3 != NULL )
    parm4 = parm3->next;
  if ( parm4 == NULL || parm4->value == NULL || parm4->value->len == 0 )
    version = 0;
  else
  {
    version = streng_to_int( TSD, parm4->value, &error );
    if ( error )
      exiterror( ERR_INCORRECT_CALL, 11, "ADDLIB", 4, tmpstr_of( TSD, parm2->value ) );
    if ( version < 0 )
      exiterror( ERR_INCORRECT_CALL, 13, "ADDLIB", 4, tmpstr_of( TSD, parm2->value ) );
  }

  name = str_of( TSD, parm1->value );
  replyport = CreatePort( NULL, 0 );
  if ( replyport == NULL )
  {
    Free_TSD( TSD, name );
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  }
  msg = CreateRexxMsg( replyport, NULL, NULL );
  if ( msg == NULL )
  {
    Free_TSD( TSD, name );
    DeletePort( replyport );
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  }
    
  if ( parm3 == NULL || parm3->value == NULL || parm3->value->len == 0 )
  {
    msg->rm_Action = RXADDFH;
    msg->rm_Args[0] = (IPTR)name;
    msg->rm_Args[1] = (IPTR)pri;
    count = 2;
    if ( !FillRexxMsg( msg, 2, 1<<1 ) )
    {
      Free_TSD( TSD, name );
      DeletePort( replyport );
      DeleteRexxMsg( msg );
      exiterror( ERR_STORAGE_EXHAUSTED, 0 );
    }
  }
  else
  {
    msg->rm_Action = RXADDLIB;
    msg->rm_Args[0] = (IPTR)name;
    msg->rm_Args[1] = (IPTR)pri;
    msg->rm_Args[2] = (IPTR)offset;
    msg->rm_Args[3] = (IPTR)version;
    count = 4;
    if ( !FillRexxMsg( msg, 4, 1<<1 | 1<<2 | 1<<3 ) )
    {
      Free_TSD( TSD, name );
      DeletePort( replyport );
      DeleteRexxMsg( msg );
      exiterror( ERR_STORAGE_EXHAUSTED, 0 );
    }
  }

  rexxport = FindPort( "REXX" );
  if (rexxport == NULL)
  {
    Free_TSD( TSD, name );
    DeletePort( replyport );
    DeleteRexxMsg( msg );
    exiterror( ERR_EXTERNAL_QUEUE, 0 );
  }
  PutMsg( rexxport, (struct Message *)msg );
    
  while( (struct RexxMsg *)WaitPort( replyport ) != msg ) /* Nothing */;
    
  Free_TSD( TSD, name );
  DeletePort( replyport );
  ClearRexxMsg( msg, count );
    
  retval = ( msg->rm_Result1 == 0 ) ? Str_cre_TSD( TSD, "1" ) : Str_cre_TSD( TSD, "0" );

  DeleteRexxMsg( msg );
  
  return retval;
}

streng *arexx_remlib( tsd_t *TSD, cparamboxptr parm1 )
{
  struct MsgPort *replyport, *rexxport;
  struct RexxMsg *msg;
  streng *retval;
  
  checkparam( parm1, 1, 1, "REMLIB" );
  
  replyport = CreatePort( NULL, 0 );
  if ( replyport == NULL )
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  msg = CreateRexxMsg( replyport, NULL, NULL );
  if ( msg == NULL )
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );

  msg->rm_Action = RXREMLIB;
  msg->rm_Args[0] = (IPTR)CreateArgstring( parm1->value->value, parm1->value->len );
  
  rexxport = FindPort( "REXX" );
  if (rexxport == NULL )
  {
    DeleteArgstring( (UBYTE *)msg->rm_Args[0] );
    DeleteRexxMsg( msg );
    exiterror( ERR_EXTERNAL_QUEUE, 0 );
  }
  PutMsg( rexxport, (struct Message *)msg );

  while( (struct RexxMsg *)WaitPort( replyport ) != msg ) /* Nothing */;
  DeletePort( replyport );
  
  retval = ( msg->rm_Result1 == 0 ) ? Str_cre_TSD( TSD, "1" ) : Str_cre_TSD( TSD, "0" );

  DeleteArgstring( (UBYTE *)msg->rm_Args[0] );
  DeleteRexxMsg( msg );
  
  return retval;
}

streng *try_func_amiga( tsd_t *TSD, const streng *name, cparamboxptr parms, char called )
{
  struct MsgPort *replyport, *rexxport;
  struct RexxMsg *msg;
  streng *retval;
  unsigned int parmcount;
  cparamboxptr parmit;
  
  replyport = CreatePort( NULL, 0 );
  if (replyport == NULL )
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  msg = CreateRexxMsg( replyport, NULL, NULL );
  if ( msg == NULL )
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  
  msg->rm_Action = RXFUNC | RXFF_RESULT | RXFF_FUNCLIST;
  msg->rm_Args[0] = (IPTR)CreateArgstring( (char *)name->value, name->len );
  for (parmit = parms, parmcount = 0; parmit != NULL; parmit = parmit->next, parmcount++)
  {
    if ( parmit->value != NULL && parmit->value->len > 0 )
      msg->rm_Args[1+parmcount] = (IPTR)CreateArgstring( parmit->value->value, parmit->value->len );
  }
  msg->rm_Action |= parmcount;
  msg->rm_Stdin = Input( );
  msg->rm_Stdout = Output( );
  
  rexxport = FindPort( "REXX" );
  if (rexxport == NULL )
  {
    ClearRexxMsg( msg, parmcount + 1 );
    DeleteRexxMsg( msg );
    DeletePort( replyport);
    exiterror( ERR_EXTERNAL_QUEUE, 0 );
  }
  PutMsg( rexxport, (struct Message *)msg );
  
  while( (struct RexxMsg *)WaitPort( replyport ) != msg ) /* Nothing */;
  DeletePort( replyport );
  
  if ( msg->rm_Result1 != 0 )
  {
    if ( msg->rm_Result2 != 0 )
      exiterror( ERR_EXTERNAL_QUEUE, 0 );
    else
      retval = NULL;
  }
  else
  {
    if ( msg->rm_Result2 == NULL )
      retval = nullstringptr();
    else
    {
      retval = Str_ncre_TSD( TSD, (const char *)msg->rm_Result2, LengthArgstring( (UBYTE *)msg->rm_Result2 ) );
      DeleteArgstring( (UBYTE *)msg->rm_Result2 );
    }
  }
  
  ClearRexxMsg( msg, parmcount + 1 );
  DeleteRexxMsg( msg );

  return retval;
};



/* The clip handling functions for AROS/amiga: setclip, getclip */

streng *arexx_setclip( tsd_t *TSD, cparamboxptr parm1 )
{
  cparamboxptr parm2;
  struct MsgPort *replyport, *rexxport;
  struct RexxMsg *msg;
  streng *retval;
  
  checkparam( parm1, 1, 2, "SETCLIP" );
  parm2 = parm1->next;

  replyport = CreatePort(NULL, 0);
  if ( replyport == NULL )
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  msg = CreateRexxMsg( replyport, NULL, NULL );
  if ( msg == NULL )
  {
    DeletePort( replyport );
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  }
  
  if ( parm2 == NULL || parm2->value == NULL || parm2->value->len == 0 )
  {
    msg->rm_Action = RXREMCON;
    msg->rm_Args[0] = (IPTR)str_of( TSD, parm1->value );
  }
  else
  {
    msg->rm_Action = RXADDCON;
    msg->rm_Args[0] = (IPTR)str_of( TSD, parm1->value );
    msg->rm_Args[1] = (IPTR)parm2->value->value;
    msg->rm_Args[2] = (IPTR)parm2->value->len;
  }
  
  rexxport = FindPort( "REXX" );
  if ( rexxport == NULL )
  {
    Free_TSD( TSD, (void *)msg->rm_Args[0] );
    DeleteRexxMsg( msg );
    DeletePort( replyport );
    exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  }
  PutMsg( rexxport, (struct Message *)msg );
  
  while( (struct RexxMsg *)WaitPort( replyport ) != msg ) /* Nothing */;

  Free_TSD( TSD, (void *)msg->rm_Args[0] );
  DeletePort( replyport );

  retval = ( msg->rm_Result1 == 0 ) ? Str_cre_TSD( TSD, "1" ) : Str_cre_TSD( TSD, "0" );
  DeleteRexxMsg( msg );
  
  return retval;
}

streng *arexx_getclip( tsd_t *TSD, cparamboxptr parm1 )
{
  struct RexxRsrc *rsrc;
  char *name;
  
  checkparam( parm1, 1, 1, "GETCLIP" );
  
  name = str_of( TSD, parm1->value );
  
  LockRexxBase(0);
  rsrc = (struct RexxRsrc *)FindName( &RexxSysBase->rl_ClipList, name );
  UnlockRexxBase(0);
  
  Free_TSD( TSD, name );
  
  if ( rsrc == NULL )
    return nullstringptr();
  else
    return Str_ncre_TSD( TSD, (const char *)rsrc->rr_Arg1, LengthArgstring( (UBYTE *)rsrc->rr_Arg1 ) );
}

#endif /* _AMIGA || __AROS__ */
