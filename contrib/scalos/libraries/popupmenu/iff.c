// iff.c
//
// $Date$
// $Revision$

/* Standard module for reading/writing simple IFF files */

#include "pmpriv.h"

#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <libraries/iffparse.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <prefs/prefhdr.h>
#include <prefs/popupmenu.h>

/* This function opens an IFF file and prepares it for reading or writing. */
/* If the file is opened for writing, it is initialized as a FORM of the   */
/* specified 'type'. If an error occurs, it is returned in 'error'.        */

#ifdef __amigaos4__
struct IFFParseIFace *IIFFParse;
#endif

struct IFFHandle *OpenIFFFile(CONST_STRPTR name, LONG type, ULONG mode, LONG *error, struct Library *IFFParseBase)
{
	struct IFFHandle *iffh;
	BPTR file;
	LONG dosmode = (mode == IFFF_WRITE)? MODE_NEWFILE: MODE_OLDFILE;

	*error = ERROR_OBJECT_NOT_FOUND;

	d1(KPrintF("%s/%s/%ld: name=<%s>\n", __FILE__, __FUNC__, __LINE__, name));
	if ((file = Open(name,dosmode)))
		{
		*error = IFFERR_NOMEM;

		d1(KPrintF("%s/%s/%ld: Open success  file=%08lx\n", __FILE__, __FUNC__, __LINE__, file));

		if ((iffh = AllocIFF()))
			{
			d1(KPrintF("%s/%s/%ld: AllocIFF success  iffh=%08lx\n", __FILE__, __FUNC__, __LINE__, iffh));
			iffh->iff_Stream = (ULONG)file;
			InitIFFasDOS(iffh);

			*error = OpenIFF(iffh,mode);
			d1(KPrintF("%s/%s/%ld: OpenIFF returned error=%ld\n", __FILE__, __FUNC__, __LINE__, *error));

			if (RETURN_OK == *error)
				{
				if (mode == IFFF_WRITE)
					{
					*error = PushChunk(iffh,type,ID_FORM,IFFSIZE_UNKNOWN);
					if (*error)
						PopChunk(iffh);
					}

				if (RETURN_OK == *error)
					return (iffh);
				}

			CloseIFF(iffh);
			FreeIFF(iffh);
			}

		Close(file);
		}

	return (NULL);
}

/* This function closes a previously opened IFF file. */

void CloseIFFFile(struct IFFHandle *iffh, struct Library *IFFParseBase)
{
   if (iffh)
   {
      if (iffh->iff_Flags & IFFF_WRITE)
      {
         PopChunk(iffh);
      }

      CloseIFF(iffh);
      Close((BPTR)(iffh->iff_Stream));
      FreeIFF(iffh);
   }
}

/* This function resets the reading/writing position of an IFF file */
/* so that the next I/O operation restarts from the beginning.      */

LONG ResetIFFFile(struct IFFHandle *iffh, struct Library *IFFParseBase)
{
   ULONG flags;
   BPTR file;
   LONG error = 0L;

   if (iffh)
   {
      file = (BPTR)(iffh->iff_Stream);
      flags = iffh->iff_Flags;
      CloseIFF(iffh);
      Seek(file,0,OFFSET_BEGINNING);
      error = OpenIFF(iffh,flags & IFFF_RWBITS);
   }

   return (error);
}

/* This function can be used to read a chunk of the specified ID, or just  */
/* the next chunk if 'id' is zero. You can read a series of chunks having  */
/* the same ID just by calling ReadChunk() multiple times, as it remembers */
/* its current position. By doing this with an 'id' of zero, you can even  */
/* read all the chunks in the IFF file. If you need to read chunks having  */
/* different IDs, you should call ResetIFFFile() before each new search,   */
/* to make sure the scan always starts from the beginning. If the wanted   */
/* chunk is found, no more than 'size' bytes are copied from it to the     */
/* buffer pointed to by 'data'; if either 'data' or 'size' is zero, no     */
/* copy occurs. You can always retrieve the chunk bytes from the returned  */
/* ContextNode structure; a NULL return value means that no more chunks of */
/* the specified ID were found (or the end of file was reached).           */

struct ContextNode *ReadChunk(struct IFFHandle *iffh, LONG id, APTR data, ULONG size, struct Library *IFFParseBase)
{
   LONG error;
   struct ContextNode *cn = NULL;

   if (iffh && ((iffh->iff_Flags & IFFF_RWBITS) == IFFF_READ))
   {
      while (TRUE)
      {
         error = ParseIFF(iffh,IFFPARSE_RAWSTEP);

         if (error == IFFERR_EOC)
         {
            continue;
         }
         else if (error)
         {
            break;
         }
         else
         {
            cn = CurrentChunk(iffh);

            if (!id || (cn->cn_ID == id))
            {
               if (data && size && (cn->cn_ID != ID_FORM) && (cn->cn_ID != ID_PROP))
               {
                  error = ReadChunkBytes(iffh,data,size);
               }

               if (error < 0L) cn = NULL;
               break;
            }

            cn = NULL;
         }
      }
   }

   return (cn);
}


void PM_LoadPrefsFile(CONST_STRPTR filename, ULONG flags, struct oldPopupMenuPrefs *prefs, const struct oldPopupMenuPrefs *defprefs)
{
	struct IFFHandle *iffh = NULL;
	struct ContextNode *cn;
	struct Library *IFFParseBase;   
	ULONG read;
	LONG error = 0;


	if ((IFFParseBase = OpenLibrary("iffparse.library", 0)))
		{
		d1(KPrintF("%s/%s/%ld: OpenLibrary(iffparse.library) success \n", __FILE__, __FUNC__, __LINE__));
#ifdef __amigaos4__
		IIFFParse = (struct IFFParseIFace *)GetInterface(IFFParseBase, "main", 1, NULL);
#endif
		d1(KPrintF("%s/%s/%ld: filename=<%s> \n", __FILE__, __FUNC__, __LINE__, filename));
		if ((iffh = OpenIFFFile(filename, ID_PREF, IFFF_READ, &error, IFFParseBase)))
			{
			d1(KPrintF("%s/%s/%ld: OpenIFFFile success\n", __FILE__, __FUNC__, __LINE__));
			if ((cn = ReadChunk(iffh,ID_PMNU,prefs,sizeof(struct oldPopupMenuPrefs),IFFParseBase)))
				{
				d1(KPrintF("%s/%s/%ld: ReadChunk success  cn_Size=%lu\n", __FILE__, __FUNC__, __LINE__, cn->cn_Size));
				read = cn->cn_Size;
				}
			else
				{
				d1(KPrintF("%s/%s/%ld: ReadChunk failed\n", __FILE__, __FUNC__, __LINE__));
				read = 0L;
				}

			if (read != sizeof(struct oldPopupMenuPrefs))
				{
				d1(KPrintF("%s/%s/%ld: size mismatch:  read=%lu  expected=%lu\n", \
					__FILE__, __FUNC__, __LINE__, read, sizeof(struct oldPopupMenuPrefs)));
				error = IFFERR_READ;
				}

			CloseIFFFile(iffh, IFFParseBase);
			}

#ifdef __amigaos4__
		DropInterface((struct Interface *)IIFFParse);
#endif
		CloseLibrary(IFFParseBase);
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: OpenLibrary(iffparse.library) success \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(KPrintF("%s/%s/%ld: error=%ld \n", __FILE__, __FUNC__, __LINE__, error));
	if (!iffh || (RETURN_OK != error))
		{
		*prefs = *defprefs;
		}
}

