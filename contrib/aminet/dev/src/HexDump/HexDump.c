/*
**    HexDump
**
**        © 1996 by Timo C. Nentwig
**        All Rights Reserved !
**
**        Tcn@techbase.in-berlin.de
**
*/

#include <exec/types.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define INVALID_CHAR    '.'

struct    AnchorPath   *ap;
BPTR                    File;

	// Freeing resources if CTRL-C

#ifdef __SASC

	VOID __regargs
	_CXBRK (VOID)
	{

		if (File)              // close file
			Close (File);

		if (ap)                // free anchorpath struct
			FreeVec (ap);

		if (DOSBase)           // close dos.library
			CloseLibrary ((struct Library *) DOSBase);

		exit (0);

	}

#endif

	// Main

VOID
main (UWORD argc, STRPTR *argv)
{

		// No args or '?'

	if (argc == 1 || *argv [1] == '?')
	{

		printf ("USAGE: %s <file1> <file2> <...>\n", argv[0]);

	}
	else
	{

		#define    BUFFSIZE    240

			// Open dos.library

		if (DOSBase = (struct DosLibrary *) OpenLibrary ("dos.library", LIBRARY_MINIMUM))
		{

			LONG    err;

			if (ap = AllocVec (sizeof (struct AnchorPath) + BUFFSIZE, MEMF_CLEAR))
			{

				UWORD    cnt;
				LONG     number    =  1;

					// Evaluate every file

				for (cnt = 1; cnt < argc; cnt++)
				{

					ap -> ap_Strlen = BUFFSIZE;

						// Evaluate pattern

					for (err = MatchFirst (argv [cnt], ap); err == 0; err = MatchNext (ap), number++)
					{


							// Open file

						if (File = Open (ap -> ap_Buf, MODE_OLDFILE))
						{

							UWORD    k;
							UBYTE    buff [16];
							UWORD    offset = 0;

								// Read a block of bytes

							while ((k = Read (File, buff, 16)) > 0)
							{

								UWORD    j;

									// Print offset

								printf ("\033[1m%08lx:\033[22m  ", offset);

									// Hex dumping ...

								for (j = 0; j < k; j++)
								{

									printf ("%02x", (UBYTE) buff [j]);

									if ( ! ((j + 1) % 4))
										printf(" ");

								}

								for (j = 0; j < (17 - k); j++)
								{

									printf ("  ");

									if ( ! ((j+1) % 4))
										printf (" ");

								}

								for (j = 0; j < k; j++)
									printf ("%c", ((buff [j] >= ' ') && (buff [j] <= 'z')) ? buff [j] : INVALID_CHAR);

									// 16 bytes worked up

								if (k == 16)
									printf ("\n");
								else if (k > 0)
									printf("\n");

								offset += 4;

							}

								// Close file

							Close (File);

						}
						else
						{

							printf ("Couldn't access file \"%s\"\n", ap -> ap_Buf);

						}

							// Close dos.library

						CloseLibrary ((struct Library *) DOSBase);

					}

				}

					// Finished, free resources

				MatchEnd (ap);
				FreeVec  (ap);

			}

		}

	}

}
