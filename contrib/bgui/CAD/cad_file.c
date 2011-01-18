/*
 * @(#) $Header$
 *
 * Change AutoDoc program
 * cad_file.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:10:51  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:23:13  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:55:37  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.5  1999/08/21 19:26:19  mlemos
 * Integrated Anton Rolls changes to fix the command line argument template
 * handling and XML format support.
 *
 * Revision 1.1.2.4  1999/05/31 02:52:47  mlemos
 * Integrated Anton Rolls changes to generate documentation files in
 * AmigaGuide and HTML formats.
 *
 * Revision 1.1.2.3  1998/11/28 14:43:45  mlemos
 * Changed LoadCadDesc() so that it was no longer inserting tildes (~) and
 * stripping whitespace from the files as they were loaded.
 *
 * Revision 1.1.2.2  1998/10/01 04:41:35  mlemos
 * Made the file handling functions work when the program main window is not
 * opened and return a success value.
 *
 * Revision 1.1.2.1  1998/09/19 01:50:16  mlemos
 * Ian sources.
 *
 *
 *
 */

#include "cad.h"

/*
 * Import data.
 */
extern Object        *LV_List, *WD_Main, *ST_Base;
extern struct Window    *MainWindow;
extern UBYTE          FileName[ 256 ], BaseName[ 64 ], GenName[ 256 ];
extern ULONG          NumDesc;

/*
 * Save the CAD file.
 */
BOOL SaveCadFile(BOOL window_opened)
{
   BPTR        file;
   CADHEADER      ch;
   CADDESC                 cd;
   CADNODE                *cn;

   SetIoErr(0);
   if(window_opened)
   {
      /*
       * Lock the window.
       */
      WindowBusy( WD_Main );
   }

   /*
    * Open the file.
    */
   if ( file = Open( FileName, MODE_NEWFILE )) {
      /*
       * Prepare header.
       */
      ch.ch_Ident = ID_CADF;
      ch.ch_Version  = CADF_VERSION;
      ch.ch_NumDesc  = NumDesc;
      strcpy( ( char * )&ch.ch_BaseName[ 0 ], BaseName );

      /*
       * Write the header.
       */
      if ( Write( file, ( UBYTE * )&ch, sizeof( CADHEADER )) == sizeof( CADHEADER )) {
         /*
          * Now write the descriptions one by one.
          */
         cn = ( CADNODE * )FirstEntry( LV_List );

         do {
            /*
             * Prepare CADDESC.
             */
            strcpy( &cd.cd_Name[ 0 ], &cn->cn_Name[ 0 ] );
            cd.cd_Length = cn->cn_DataLength;

            /*
             * Write the CADDESC.
             */
            if ( Write( file, ( UBYTE * )&cd, sizeof( CADDESC )) == sizeof( CADDESC )) {
               /*
                * And the corresponding data.
                */
               if ( cn->cn_DataLength ) {
                  if ( Write( file, cn->cn_Data, cn->cn_DataLength ) != cn->cn_DataLength ) {
                     if(window_opened)
                        DosRequest( "OK", ISEQ_C "Error writing\n\n`%s'", FileName );
                     break;
                  }
               }
            } else {
               if(window_opened)
                  DosRequest( "OK", ISEQ_C "Error writing\n\n`%s'", FileName );
               break;
            }
         } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));
      } else {
         if(window_opened)
            DosRequest( "OK", ISEQ_C "Error writing\n\n`%s'", FileName );
      }

      /*
       * Close the file.
       */
      Close( file );
      /*
       * Delete the file on error.
       */
      {
         LONG error;

         if ((error=IoErr()))
         {
            DeleteFile( FileName );
            SetIoErr(error);
         }
      }

   } else {
      if(window_opened)
         DosRequest( "OK", ISEQ_C "Unable to open\n\n`%s'", FileName );
   }

   if(window_opened)
   {
      /*
       * Unlock the window.
       */
      WindowReady( WD_Main );
   }
   return((BOOL)(IoErr()==0));
}

/*
 * Load a single CADDESC.
 */
STATIC CADNODE *LoadCadDesc( BPTR file )
{
   CADDESC                 cd;
   CADNODE                *cn;
   UBYTE                  *buffer;

   /*
    * Read the CADDESC.
    */
   if ( Read( file, ( UBYTE * )&cd, sizeof( CADDESC )) == sizeof( CADDESC )) {
      /*
       * Allocate node.
       */
      if ( cn = ( CADNODE * )AllocVec( sizeof( CADNODE ), MEMF_CLEAR )) {
         /*
          * Setup node name.
          */
         strcpy(cn->cn_Name, cd.cd_Name);

         /*
          * Allocate data.
          */
         if (cd.cd_Length)
         {
            if (buffer = (UBYTE *)AllocVec(cd.cd_Length + 1, MEMF_CLEAR))
            {
               if (Read(file, buffer, cd.cd_Length) == cd.cd_Length)
               {
                  cn->cn_DataLength = strlen(buffer);
                  cn->cn_Data       = AllocVec(cn->cn_DataLength, 0);
                  strncpy(cn->cn_Data, buffer, cn->cn_DataLength);
                  FreeVec(buffer);
                  return cn;
               }
               FreeVec(cn->cn_Data);
            } else
               SetIoErr(ERROR_NO_FREE_STORE);
         } else
            return(cn);
         FreeVec(cn);
      } else
         SetIoErr(ERROR_NO_FREE_STORE);
   }
   return( NULL );
}

/*
 * Load a CADLIST file.
 */
BOOL LoadCadFile(BOOL window_opened)
{
   BPTR        file;
   CADHEADER      ch;
   CADNODE                *cn;
   BOOL        err = FALSE;
   ULONG       i;

   if(window_opened)
   {
      /*
       * Lock the window.
       */
      WindowBusy( WD_Main );
   }

   BaseName[ 0 ] = '\0';
   NumDesc       = 0;
   ClearList( NULL, LV_List );

   /*
    * Open the file.
    */
   if ( file = Open( FileName, MODE_OLDFILE )) {
      /*
       * Load header.
       */
      if ( Read( file, ( UBYTE * )&ch, sizeof( CADHEADER )) == sizeof( CADHEADER )) {
         /*
          * Check identifier and version.
          */
         if ( ch.ch_Ident == ID_CADF ) {
            if ( ch.ch_Version <= CADF_VERSION ) {
               /*
                * Copy the base name.
                */
               strcpy( BaseName, &ch.ch_BaseName[ 0 ] );

               /*
                * Read all CADDESCs.
                */
               for ( i = 0; i < ch.ch_NumDesc; i++ ) {
                  /*
                   * Read CADDESC.
                   */
                  if ( cn = LoadCadDesc( file )) {
                     /*
                      * Add it to the list.
                      */
                     AddEntry( NULL, LV_List, cn, LVAP_TAIL );
                     NumDesc++;
                  } else {
                     err = TRUE;
                     if(window_opened)
                        DosRequest( "OK", ISEQ_C "Error reading\n\n`%s'", FileName );
                     break;
                  }
               }
            } else {
               err = TRUE;
               MyRequest( "OK", ISEQ_C "Unknown file version." );
            }
         } else {
            err = TRUE;
            MyRequest( "OK", ISEQ_C "Unknown file type." );
         }
      } else {
         err = TRUE;
         if(window_opened)
            DosRequest( "OK", ISEQ_C "Error reading\n\n`%s'", FileName );
      }
      Close( file );
   } else {
      err = TRUE;
      if(window_opened)
         DosRequest( "OK", ISEQ_C "Unable to open\n\n`%s'", FileName );
   }

   if ( err ) {
      /*
       * Errors clear all data.
       */
      BaseName[ 0 ] = '\0';
      NumDesc       = 0;
      ClearList( NULL, LV_List );
   }

   /*
    * Setup GUI.
    */
   SetGadgetAttrs(( struct Gadget * )ST_Base, MainWindow, NULL, STRINGA_TextVal, BaseName, TAG_END );
   SetGadgetAttrs(( struct Gadget * )LV_List, MainWindow, NULL, LISTV_Select, LISTV_Select_First, TAG_END );

   if(window_opened)
   {
	    RefreshList( MainWindow, LV_List );

      /*
       * Unlock the window.
       */
      WindowReady( WD_Main );
   }
   return((BOOL)!err);
}

/*
 * Generate autodoc.
 */
BOOL Generate(BOOL window_opened, ULONG outputFormat)
{
  BOOL result;

   if(window_opened)
   {
      /*
       * Lock the window.
       */
      WindowBusy( WD_Main );
   }

  switch( outputFormat )
  {
    case MATCH_DOC:
      result = GenerateDoc(window_opened);
      break;

    case MATCH_GUIDE:
      result = GenerateGuide(window_opened);
      break;

    case MATCH_HTML:
      result = GenerateHTML(window_opened);
      break;

    case MATCH_XML:
      result = GenerateXML(window_opened);
      break;

    default:
      result = FALSE; /* unsupported output format */
  }

   if(window_opened)
   {
      /*
       * Unlock the window.
       */
      WindowReady( WD_Main );
   }

   return( result );
}

BOOL GenerateDoc(BOOL window_opened)
{
   CADNODE  *cn;
   BPTR     file;

   /*
    * Open the output file.
    */
   if ( file = Open( GenName, MODE_NEWFILE )) {
      /* set secondary error code to 0 - Anton
         (Who cares if the file doesn't exist yet? need to check this, though)
       */
      SetIoErr(0);

      /*
       * Write table of contents.
       */
      MyFPrintf( file, "TABLE OF CONTENTS\n\n" );

      cn = ( CADNODE * )FirstEntry( LV_List );
      do {
         MyFPrintf( file, "%s/%s\n", BaseName, &cn->cn_Name[ 0 ] );
      } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));

      FPutC( file, '\n' );

      /*
       * Write entries...
       */
      cn = ( CADNODE * )FirstEntry( LV_List );
      do {
         /*
          * Header.
          */
         MyFPrintf( file, "%lc%s/%s\n\n", 12, BaseName, &cn->cn_Name[ 0 ] );

         /*
          * Data.
          */
         FWrite( file, cn->cn_Data, cn->cn_DataLength, 1 );

         /*
          * Add an empty line.
          */
         FPutC( file, '\n' );

         /*
          * Break on an error.
          */
         if ( IoErr()) {
            if(window_opened)
               DosRequest( "OK", ISEQ_C "Error during generation of\n\n`%s'", GenName );
            break;
         }
      } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));

      /*
       * Close the file.
       */
      Close( file );
      {
         LONG error;
         /*
          * Delete the file upon error.
          */
         if ((error=IoErr()))
         {
            DeleteFile( GenName );
            SetIoErr(error);
         }
      }
   }

   return((BOOL)(IoErr()==0));
}


/*                               
 * Generate HTML autodoc. - Anton.
 */
BOOL GenerateHTML(BOOL window_opened)
{
   CADNODE *cn;
   BPTR   file;

   /*
    * Open the output file.
    */
   if ( file = Open( GenName, MODE_NEWFILE )) {
      /* set secondary error code to 0
         (Who cares if the file doesn't exist yet? need to check this, though) - Anton
       */
      SetIoErr(0); 

      /* Write html header stuff - Anton */
      MyFPrintf( file, "<html>\n  <head><title>");
      MyFPrintf(file, BaseName); /* put BaseName into html document title */
      MyFPrintf( file, "</title>\n  </head>\n<body>\n");
                                 
      /*
       * Write table of contents.
       */
      MyFPrintf( file, "TABLE OF CONTENTS<br>\n<br>\n" );

      cn = ( CADNODE * )FirstEntry( LV_List );                      
      do {
        /* name the table of contents entry and link to the actual doc entry */
         MyFPrintf( file, "<a name=toc%s><a href=#%s>", cn->cn_Name, cn->cn_Name);
         MyFPrintf( file, "%s/%s</a></a><br>\n", BaseName, cn->cn_Name );
      } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));

      /*
       * Write entries...
       */
      cn = ( CADNODE * )FirstEntry( LV_List );
      do {
         /* html stuff before the actual node data. */
         MyFPrintf( file, "<a name=%s>", cn->cn_Name); /* name the link target here - Anton */
         MyFPrintf( file, "\n<hr>\n" ); /* horizontal rule and newline - Anton */
           /* link back to the table of contents - Anton */
         MyFPrintf( file, "\n<br>\n<a href=#toc%s>top</a> ", cn->cn_Name );
         MyFPrintf( file, "%s/%s<br>\n", BaseName, cn->cn_Name );
         MyFPrintf( file, "<pre>\n" ); /* preformatted text tag - Anton.*/

         /* Data. */
         FWrite( file, cn->cn_Data, cn->cn_DataLength, 1 );

         /* to do: search through the data, find "SEE ALSO" and match anything after this
            with any node names. If they match, make a link to it.
          */

         /* end preformatted text tag, end <a name> tag, newline and horizontal rule- Anton */
         MyFPrintf( file, "</pre>\n</a><br>\n" );

         /* Break on an error. */
         if ( IoErr()) {
            if(window_opened)
               DosRequest( "Go away!", ISEQ_C "Error during generation of\n\n`%s'", GenName );
            break;
         }
      } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));                        

      MyFPrintf( file, "\n<hr>\n" ); /* final horizontal rule and newline - Anton */
      MyFPrintf( file, "</body>\n</html>"); /* finish off the html stuff. - Anton */

      /* Close the file. */
      Close( file );
      {
         LONG error;
         /*
          * Delete the file upon error.
          */
         if ((error=IoErr()))
         {
            DeleteFile( GenName );
            SetIoErr(error);
         }
      }
   }

   return((BOOL)(IoErr()==0));
}

/*
 * Generate XML autodoc. - Anton.
 *
 * This functin is incomplete.
 */
BOOL GenerateXML(BOOL window_opened)
{
   BPTR   file;

   /*
    * Open the output file.
    */
   if ( file = Open( GenName, MODE_NEWFILE )) {
      /* set secondary error code to 0
         (Who cares if the file doesn't exist yet? need to check this, though)
       */
      SetIoErr(0);

      /* Write xml header stuff */
      /* QUOTE is defined in cad.h */
      /* encoding: ascii is a subset of UTF-8 and does not need to be specified. */
      /* Manuel recommends ISO-8859-1 encoding so it recognizes 8-bit characters */
      MyFPrintf( file, "<?xml version='1.0' encoding=\"ISO-8859-1\"?>\n\n");

      /* stuff goes here */                                
      MyFPrintf( file, "<!DOCTYPE test [\n");
      MyFPrintf( file, "<!ELEMENT test (#PCDATA) >\n");
      MyFPrintf( file, "<!ENTITY % xx '&#37;'>\n");  /* not used - example of entity */
      MyFPrintf( file, "<test>This format has yet to be implemented</test>\n");
      MyFPrintf( file, "]>\n");

      MyFPrintf( file, "\n</xml>"); /* finish off the xml stuff. */

      /* Close the file. */
      Close( file );
      {
         LONG error;
         /*
          * Delete the file upon error.
          */
         if ((error=IoErr()))
         {
            DeleteFile( GenName );
            SetIoErr(error);
         }
      }
   }

   return((BOOL)(IoErr()==0));
}

/*
 * Generate amigaguide. (just the same as text for now.) - Anton
 *
 * Currently cad node names with slashes are a problem.
 * (interpreted by amigaguide as directory.)
 * I've remapped slashes now, but double clicking on a link with a slash still won't work.
 * (not a big problem).
 *
 * Ok, this will be confusing, because there are CADNODEs and there are
 * amigaguide nodes in the resulting file. So read carefully and don't
 * be fooled.
 *
 */
BOOL GenerateGuide(BOOL window_opened)
{
   CADNODE *cn;
   BPTR   file;                                           
   UBYTE tempNodeName[32]; /* This will be copied from the CADNODE and slashes replaced */

   /*
    * Open the output file.
    */
   if ( file = Open( GenName, MODE_NEWFILE )) {
      /* set secondary error code to 0 - Anton
         (Who cares if the file doesn't exist yet? need to check this, though)
       */
      SetIoErr(0);

      /* guide header stuff */
      /* @database      to identify this file as amigaguide
         @tab 2         to make tabs look reasonable - a global attribute
         @node main     to begin the main node (which contains the Table of Contents)
       */
      MyFPrintf( file, "@database\n@tab 2\n@node main "QUOTE"%s"QUOTE"\n", FilePart(GenName));

      /* if( We-Want-A-SearchGuide-Button ) { */
      /* add a nice search button */
      MyFPrintf( file, "@{"QUOTE" Search "QUOTE" SYSTEM "QUOTE"RUN >nil: SearchGuide %s"QUOTE"}\n", FilePart(GenName));

      /* } */

      /*
       * Write table of contents.
       */
      MyFPrintf( file, "TABLE OF CONTENTS\n\n" );

      cn = ( CADNODE * )FirstEntry( LV_List );
      do
      {
         MyFPrintf( file, "@{"QUOTE"%s/%s"QUOTE" LINK ", BaseName, cn->cn_Name); /* button text */

         /* if the node name contains a slash then we are in trouble */
         MapSlash( cn->cn_Name, tempNodeName ); /* This should get us out of trouble */
         MyFPrintf( file, QUOTE"%s"QUOTE"}", tempNodeName); /* node name */
                                                   
         MyFPrintf( file, "\n"); /* newline */
      } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));

      MyFPrintf( file, "@endnode\n" ); /* end of the main node */

      /*
       * Write entries...
       */
      cn = ( CADNODE * )FirstEntry( LV_List );
      do {
         /*
          * Header.                        
          */

         MyFPrintf( file, "\n@node "QUOTE ); /* begin quote */

         /* if the node name contains a slash then we are in trouble */
         MapSlash( cn->cn_Name, tempNodeName ); /* This should get us out of trouble */
         MyFPrintf( file, "%s", tempNodeName ); /* node name */

         MyFPrintf( file, QUOTE" "QUOTE ); /* end quote, begin quote */
         MyFPrintf( file, "%s/%s", BaseName, cn->cn_Name ); /* node title */
         MyFPrintf( file, QUOTE"\n\n" ); /* end quote, newline */
         
         /*
          * Data.
          */
         FWrite( file, cn->cn_Data, cn->cn_DataLength, 1 );


         MyFPrintf( file, "@endnode\n" ); /* end node, new line */
         /* (there must be a linefeed after the last node) */

         /*
          * Break on an error.
          */
         if ( IoErr()) {                                        
            if(window_opened)
               DosRequest( "Go away!", ISEQ_C "Error during generation of\n\n`%s'", GenName );
            break;
         }
      } while ( cn = ( CADNODE * )NextEntry( LV_List, cn ));

      /*
       * Close the file.
       */
      Close( file );
      {
         LONG error;
         /*
          * Delete the file upon error.
          */
         if ((error=IoErr()))
         {
            DeleteFile( GenName );
            SetIoErr(error);
         }
      }
   }

   return((BOOL)(IoErr()==0));
}

/*
  support function for GenerateGuide()
  Copy source string to destination string, replacing slashes '/' with 'S'
  and ':' with 'C'
*/
UBYTE *MapSlash(UBYTE *source, UBYTE *destination)
{
  int i=0;
  do
  {
    switch(source[i])
    {
      case '/':
        destination[i] = 'S';
        break;

      case ':':
        destination[i] = 'C';
        break;

      default:
        destination[i] = source[i];
    }
  }while(source[i++] != '\0');

  return (destination);
}

