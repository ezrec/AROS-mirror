#include "HTFormat.h"
#include "HTFile.h"
#include "HTUtils.h"
#include "tcp.h"
#include "HTML.h"
#include "HTMLDTD.h"
#include "HText.h"
#include "HTAlert.h"
#include "HTList.h"
#include "HTInit.h"
#include "HTFWriter.h"
#include "HTPlain.h"
#include "SGML.h"
#include "HTMLGen.h"

/* #define TRACE 1 */

struct _HTStream 
{
  CONST HTStreamClass*	isa;
  /* ... */
};

extern char *mo_tmpnam (void);
extern void application_user_feedback (char *);
extern char *uncompress_program, *gunzip_program;

extern void HTFileCopyToText (FILE *fp, HText *text);

/* Given a filename of a local compressed file, compress it in place.

   We assume that the file does not already have a .Z or .z extension
   at this point -- this is a little weird but it's convenient. */
void HTCompressedFileToFile (char *fnam, int compressed)
{
  char *znam;
  char *cmd;
  
  if (TRACE)
    fprintf 
      (stderr, "[HTCompressedFileToFile] Entered; fnam '%s', compressed %d\n",
       fnam, compressed);

  /* Punt if we can't handle it. */
  if (compressed != COMPRESSED_BIGZ && compressed != COMPRESSED_GNUZIP)
    return;

  HTProgress ("Preparing to uncompress data.");
  
  znam = (char *)malloc (sizeof (char) * (strlen (fnam) + 8));

  /* Either compressed or gzipped. */
  if (compressed == COMPRESSED_BIGZ)
    sprintf (znam, "%s.Z", fnam);
  else
    sprintf (znam, "%s.gz", fnam);

  cmd = (char *)malloc (sizeof (char) * (strlen (fnam) + strlen (znam) + 32));
#if defined(_AMIGA) || defined(__AROS__)
  sprintf (cmd, "rename >NIL: <NIL:  %s %s", fnam, znam);
#else
  sprintf (cmd, "/bin/mv %s %s", fnam, znam);  
#endif
  if (system(cmd) != 0)
    {
      application_user_feedback
        ("Unable to uncompressed compressed data;\nresults may be in error.");
      free (znam);
      free (cmd);
      return;
    }

  if (TRACE)
    fprintf (stderr, "[HTCompressedFileToFile] Moved '%s' to '%s'\n",
             fnam, znam);

  free (cmd);
  
  if (compressed == COMPRESSED_BIGZ)
    {
      cmd = (char *)malloc (strlen (uncompress_program) + strlen (znam) + 8);
      sprintf (cmd, "%s %s", uncompress_program, znam);
    }
  else
    {
      cmd = (char *)malloc (strlen (gunzip_program) + strlen (znam) + 8);
      sprintf (cmd, "%s %s", gunzip_program, znam);
    }

  HTProgress ("Uncompressing data.");
  
  if (system (cmd) != 0)
    {
      application_user_feedback 
        ("Unable to uncompress compressed data;\nresults may be in error.");
      free (cmd);
      return;
    }

  HTProgress ("Data uncompressed.");

  if (TRACE)
    fprintf 
      (stderr, "[HTCompressedFileToFile] Uncompressed '%s' with command '%s'\n",
       znam, cmd);
  
  free (cmd);
  free (znam);

  return;
}


void HTCompressedHText (HText *text, int compressed, int plain)
{
  char *fnam;
  char *cmd;
  FILE *fp;
  int rv, size_of_data;
  
  if (TRACE)
    fprintf 
      (stderr, "[HTCompressedHText] Entered; compressed %d\n",
       compressed);

  /* Punt if we can't handle it. */
  if (compressed != COMPRESSED_BIGZ && compressed != COMPRESSED_GNUZIP)
    return;

  /* Hmmmmmmmmm, I'm not sure why we subtract 1 here, but it is
     indeed working... */
  size_of_data = HText_getTextLength (text) - 1;

  if (size_of_data == 0)
    {
      fprintf (stderr, "[HTCompressedHText] size_of_data 0; punting\n");
      return;
    }
  
  fnam = mo_tmpnam ();
  fp = fopen (fnam, "w");
  if (!fp)
    {
      if (TRACE)
        fprintf (stderr, "COULD NOT OPEN TMP FILE '%s'\n", fnam);
      application_user_feedback
        ("Unable to uncompressed compressed data;\nresults may be in error.");
      free (fnam);
      return;
    }

  if (TRACE)
    fprintf (stderr, "[HTCmopressedHText] Going to write %d bytes.\n",
             size_of_data);
  rv = fwrite (HText_getText (text), sizeof (char), size_of_data, fp);
  if (rv != size_of_data)
    {
      if (TRACE)
        fprintf (stderr, "ONLY WROTE %d bytes\n", rv);
      application_user_feedback
        ("Unable to write compressed data to local disk;\nresults may be in error.");
    }
  fclose (fp);

  if (TRACE)
    fprintf (stderr, "HTCompressedHText: Calling CompressedFileToFile\n");
  HTCompressedFileToFile (fnam, compressed);

  HText_clearOutForNewContents (text);

  HText_beginAppend (text);
  
  if (plain)
    {
      if (TRACE)
        fprintf (stderr, "[HTCompressedHText] Throwing in PLAINTEXT token...\n");
      HText_appendText(text, "<PLAINTEXT>\n");
    }

  fp = fopen (fnam, "r");
  if (!fp)
    {
      if (TRACE)
        fprintf (stderr, "COULD NOT OPEN TMP FILE FOR READING '%s'\n", fnam);
      /* We already get error dialog up above. */
      free (fnam);
      return;
    }

  HTFileCopyToText (fp, text);

  if (TRACE)
    fprintf (stderr, "[HTCompressedHText] I think we're done...\n");

  cmd = (char *)malloc (sizeof (char) * (strlen (fnam) + 32));
#if defined(_AMIGA) || defined(__AROS__)
  sprintf (cmd, "delete <NIL: >NIL: %s", fnam);
#else
  sprintf (cmd, "/bin/rm -f %s", fnam);
#endif
  system (cmd);
  free (cmd);
  
  return;
}
