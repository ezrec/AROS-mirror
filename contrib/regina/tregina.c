/*
 * Copyright (C) 2002  Mark Hessling <M.Hessling@qut.edu.au>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Mark Hessling  M.Hessling@qut.edu.au  http://www.lightlink.com/hessling/
 *
 */

#include <stdlib.h>
#include <stdio.h>

#if defined(HAVE_CONFIG_H)
# include "config.h"
#else
# include "configur.h"
#endif

#include "rexxsaa.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifndef PATH_MAX
# define PATH_MAX 1024
#endif

void DestroyInstore( char *strptr )
{
#if defined(REXXFREEMEMORY)
   RexxFreeMemory( strptr );
#elif defined(WIN32) && (defined(USE_OREXX) || defined(USE_WINREXX) || defined(USE_QUERCUS))
   GlobalFree( strptr );
#elif defined(USE_OS2REXX)
   DosFreeMem( strptr );
#else
   free( strptr );
#endif
}

void usage( char *prog )
{
   fprintf( stderr, "Usage:\n" );
   fprintf( stderr, "   To create a file of tokenised code...\n" );
   fprintf( stderr, "      %s -t srcfile tokenfile\n", prog );
   fprintf( stderr, "   To execute a file of tokenised code...\n" );
   fprintf( stderr, "      %s tokenfile [Rexx program arguments]\n", prog );
}

int Tokenise( char *infile, char *outfile )
{
   RXSTRING instore[2];
   RXSTRING ArgList;
   FILE *infp;
   FILE *outfp;
   short rexxrc=0;
   int rc, size;
   RXSTRING retstr;
   CHAR retbuf[RXAUTOBUFLEN];

   infp = fopen( infile, "rb" );
   if ( infp == NULL )
   {
      perror( "Unable to open input file");
      exit(1);
   }
   fseek( infp, 0, SEEK_END );
   size = ftell( infp );
   rewind( infp );
   instore[0].strptr = malloc( size );
   fread( instore[0].strptr, size, 1, infp );
   instore[0].strlength = size;
   instore[1].strptr = NULL;
   instore[1].strlength = 0;
   fclose(infp);

   MAKERXSTRING( ArgList, "//T", 3 );
   strcpy( retbuf, "" );
   MAKERXSTRING( retstr, retbuf, sizeof( retbuf ) );
   rc = RexxStart(1,
                  &ArgList,
                  infile,
                  instore,
                  "SYSTEM",
                  RXCOMMAND,
                  NULL,
                  &rexxrc,
                  &retstr);
   if ( rc == 0
   &&   instore[1].strptr)
   {
      outfp = fopen( outfile, "wb" );
      if ( outfp == NULL )
      {
         perror( "Unable to create output file");
         DestroyInstore( instore[1].strptr );
         exit(1);
      }
      if ( fwrite( instore[1].strptr, 1, instore[1].strlength, outfp ) != instore[1].strlength )
      {
         perror( "Unable to write output file");
         DestroyInstore( instore[1].strptr );
         exit(1);
      }
      fclose( outfp );
      DestroyInstore( instore[1].strptr );
   }
   return rc;
}

int Execute( char *tokfile, char *srcfile, int ArgCount, PRXSTRING ArgList )
{
   RXSTRING instore[2];
   FILE *tokfp=NULL;
   FILE *srcfp=NULL;
   short rexxrc=0;
   int rc, size;
   RXSTRING retstr;
   CHAR retbuf[RXAUTOBUFLEN];

   if ( srcfile )
   {
      srcfp = fopen( srcfile, "rb" );
      if ( srcfp == NULL )
      {
         perror( "Error opening source file" );
         return(1);
      }
      fseek( srcfp, 0, SEEK_END );
      size = ftell( srcfp );
      rewind( srcfp );
      instore[0].strptr = malloc( size );
      if (instore[0].strptr == NULL)
      {
         fclose(srcfp);
         perror( "Error allocating memory for instore[0].strptr" );
         return(3);
      }
      fread( instore[0].strptr, size, 1, srcfp );
      instore[0].strlength = size;
      fclose(srcfp);
   }
   else
   {
      instore[0].strptr = NULL;
      instore[0].strlength = 0;
   }
   tokfp = fopen( tokfile, "rb" );
   if ( tokfp == NULL )
   {
      perror( "Error opening token file" );
      if (instore[0].strptr)
         free( instore[0].strptr );
      return(1);
   }
   fseek( tokfp, 0, SEEK_END );
   size = ftell( tokfp );
   rewind( tokfp );
   instore[1].strptr = malloc( size );
   if ( instore[1].strptr == NULL )
   {
      perror( "Error allocating memory for instore[1].strptr" );
      fclose( tokfp );
      if (instore[0].strptr)
         free( instore[0].strptr );
      return(2);
   }
   fread( instore[1].strptr, size, 1, tokfp );
   instore[1].strlength = size;
   fclose( tokfp );

   strcpy( retbuf, "" );
   MAKERXSTRING( retstr, retbuf, sizeof( retbuf ) );
   rc = RexxStart((ArgCount) ? 1 : 0,
                  ArgList,
                  (srcfile) ? srcfile : "<no_source>",
                  instore,
                  "SYSTEM",
                  RXCOMMAND,
                  NULL,
                  &rexxrc,
                  &retstr);
   if (instore[1].strptr)
      free( instore[1].strptr );
   if (instore[0].strptr)
      free( instore[0].strptr );
   return rc;
}

int main( int argc, char *argv[])
{
   int rc;
   int i=0;
   long ArgCount=0;
   RXSTRING ArgList;

   if ( argc == 1 )
   {
      usage( argv[0] );
      return(0);
   }
   if ( strcmp( argv[1], "-t" ) == 0 )
   {
      if ( argc == 4 )
         rc = Tokenise( argv[2], argv[3] );
      else
      {
         (void)fprintf( stderr, "Must supply both srcfile and tokenfile as parameters when tokenising.\n" );
         exit( 1 );
      }
   }
   else
   {
      /* 
       * Build an argument string, if any.
       */
      if ( argc > 2 )
      {
         int len=0;
   
         ArgCount = argc - 2;
         for ( i = 2; i < argc; i++ )
         {
            len += strlen( (char *)argv[i] );
         }
         if ( ( ArgList.strptr = (char *)malloc( len + 1 + ArgCount) ) == NULL )
         {
            (void)fprintf( stderr, "%s: out of memory\n", argv[0] );
            exit( 1 );
         }
         strcpy( ArgList.strptr, "" );
         for ( i = 2; i < argc; i++ )
         {
            strcat( ArgList.strptr, argv[i] );
            if ( i != argc )
               strcat( ArgList.strptr, " " );
         }
         ArgList.strlength = ArgCount + len - 1;
      }
      else
      {
         ArgList.strptr = NULL;
         ArgList.strlength = 0;
      }
      rc = Execute( argv[1], NULL, ArgCount, &ArgList );
   }
   return rc;
}
