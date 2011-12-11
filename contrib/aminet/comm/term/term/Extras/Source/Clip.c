/*
**	Clip.c
**
**	Clipboard support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC struct IFFHandle	*ClipHandle;
STATIC STRPTR ClipBuffer,ClipIndex;
STATIC LONG ClipSize,ClipLength;

	/* CloseClip():
	 *
	 *	Close clipboard handle, stop reading.
	 */

VOID
CloseClip()
{
	CloseIFFClip(ClipHandle);
	ClipHandle = NULL;

	FreeVecPooled(ClipBuffer);
	ClipBuffer = NULL;
}

	/* GetClip(STRPTR Buffer,LONG Len):
	 *
	 *	Read text data from clipboard and put it into the supplied buffer.
	 */

LONG
GetClip(STRPTR Buffer,LONG Len)
{
	LONG BytesPut = 0;
	LONG c;

		/* Is the read buffer already exhausted? */

	if(!ClipLength)
	{
			/* Is there still any data to read? */

		if(ClipSize)
		{
			LONG Size = MIN(ClipSize,1024);

				/* Try to read the data and return failure if necessary. */

			if(ReadChunkBytes(ClipHandle,ClipBuffer,Size) != Size)
				return(-1);
			else
			{
				ClipSize	-= Size;
				ClipLength	 = Size;
				ClipIndex	 = ClipBuffer;
			}
		}
		else
		{
				/* We just parsed a single chunk, now go on and
				 * look for another one.
				 */

			if(!ParseIFF(ClipHandle,IFFPARSE_SCAN))
			{
				struct ContextNode *ContextNode = CurrentChunk(ClipHandle);

				if(ContextNode->cn_Type == ID_FTXT)
				{
					LONG Size;

					ClipSize	= ContextNode->cn_Size;
					Size		= MIN(ClipSize,1024);

					if(ReadChunkBytes(ClipHandle,ClipBuffer,Size) != Size)
						return(-1);
					else
					{
						ClipSize	-= Size;
						ClipLength	 = Size;
						ClipIndex	 = ClipBuffer;
					}
				}
				else
					return(-1);
			}
			else
				return(-1);
		}
	}

	while(ClipLength && BytesPut < Len)
	{
		ClipLength--;

		switch(c = *ClipIndex++)
		{
			case '\r':

				break;

			default:

				if(IsPrintable[c])
				{
					*Buffer++ = c;
					BytesPut++;
				}

				break;
		}
	}

	return(BytesPut);
}

	/* OpenClip():
	 *
	 *	Open the clipboard for sequential reading.
	 */

LONG
OpenClip(LONG Unit)
{
	LONG Error;

	CloseClip();

	if(ClipBuffer = (STRPTR)AllocVecPooled(1024,MEMF_ANY))
	{
		if(ClipHandle = OpenIFFClip(Unit,MODE_OLDFILE))
		{
			if(!(Error = StopChunk(ClipHandle,ID_FTXT,ID_CHRS)))
			{
				if(!ParseIFF(ClipHandle,IFFPARSE_SCAN))
				{
					struct ContextNode *ContextNode = CurrentChunk(ClipHandle);

					if(ContextNode->cn_Type == ID_FTXT)
					{
						ClipSize	= ContextNode->cn_Size;
						ClipLength	= 0;

						return(0);
					}
					else
						Error = ERROR_OBJECT_WRONG_TYPE;
				}
				else
					Error = ERROR_OBJECT_NOT_FOUND;
			}
		}
		else
			Error = IoErr();
	}
	else
		Error = ERROR_NO_FREE_STORE;

	CloseClip();

	return(Error);
}

	/* GetClipContents(LONG Unit,APTR *Buffer,LONG *Size):
	 *
	 *	Merge text contents of the clipboard into a single string.
	 */

BOOL
GetClipContents(LONG Unit,APTR *Buffer,LONG *Size)
{
	struct IFFHandle *Handle;
	LONG Bytes;
	APTR Store;

	Store = NULL;
	Bytes = 0;

	if(Handle = OpenIFFClip(Unit,MODE_OLDFILE))
	{
		if(!StopChunk(Handle,ID_FTXT,ID_CHRS))
		{
			struct ContextNode *Node;

			while(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				Node = CurrentChunk(Handle);

				if(Node->cn_Type == ID_FTXT)
					Bytes += Node->cn_Size;
			}
		}

		CloseIFFClip(Handle);
	}

	if(Bytes > 0)
	{
		if(Handle = OpenIFFClip(Unit,MODE_OLDFILE))
		{
			if(!StopChunk(Handle,ID_FTXT,ID_CHRS))
			{
				if(Store = AllocVecPooled(Bytes,MEMF_ANY))
				{
					struct ContextNode *Node;
					LONG BytesRead;
					STRPTR Index;

					Index		= Store;
					BytesRead	= 0;

					while(!ParseIFF(Handle,IFFPARSE_SCAN) && BytesRead < Bytes)
					{
						Node = CurrentChunk(Handle);

						if(Node->cn_Type == ID_FTXT)
						{
							LONG Count = Node->cn_Size;

							if(BytesRead + Count > Bytes)
								Count = Bytes - BytesRead;

							if(Count > 0)
							{
								if((Count = ReadChunkBytes(Handle,Index,Count)) > 0)
								{
									Index		+= Count;
									BytesRead	+= Count;
								}
							}
						}
					}

					Bytes = BytesRead;
				}
			}

			CloseIFFClip(Handle);
		}
	}

	if(Store && !Bytes)
	{
		FreeVecPooled(Store);

		Store = NULL;
	}

	*Buffer	= Store;
	*Size	= Bytes;

	return((BOOL)(Store != NULL));
}

	/* AddClip(CONST_STRPTR Buffer,LONG Size):
	 *
	 *	Merge previous clipboard contents with new text,
	 *	then store the new string in the clipboard.
	 */

BOOL
AddClip(CONST_STRPTR Buffer,LONG Size)
{
	LONG Bytes;
	APTR Store;

	if(GetClipContents(Config->ClipConfig->ClipboardUnit,&Store,&Bytes))
	{
		struct IFFHandle *Handle;
		LONG Error;

		if(Handle = OpenIFFClip(Config->ClipConfig->ClipboardUnit,MODE_NEWFILE))
		{
			if(!(Error = PushChunk(Handle,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN)))
			{
				if(!(Error = PushChunk(Handle,0,ID_CHRS,IFFSIZE_UNKNOWN)))
				{
					if(WriteChunkBytes(Handle,Store,Bytes) == Bytes)
					{
						if(WriteChunkBytes(Handle,(APTR)Buffer,Size) != Size)
							Error = IoErr();
					}
					else
						Error = IoErr();

					if(!Error)
						Error = PopChunk(Handle);
				}

				if(!Error)
					Error = PopChunk(Handle);
			}

			CloseIFFClip(Handle);
		}
		else
			Error = IoErr();

		FreeVecPooled(Store);

		if(Error)
		{
			SetIoErr(Error);

			return(FALSE);
		}
		else
			return(TRUE);
	}
	else
		return(SaveClip(Buffer,Size));
}

	/* SaveClip(STRPTR Buffer,LONG Size):
	 *
	 *	Send a given text buffer to the clipboard.
	 */

BOOL
SaveClip(CONST_STRPTR Buffer,LONG Size)
{
	LONG Error;

	if(Size > 0)
	{
		struct IFFHandle *Handle;

		if(Handle = OpenIFFClip(Config->ClipConfig->ClipboardUnit,MODE_NEWFILE))
		{
			if(!(Error = PushChunk(Handle,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN)))
			{
				if(!(Error = PushChunk(Handle,0,ID_CHRS,IFFSIZE_UNKNOWN)))
				{
					if(WriteChunkBytes(Handle,(APTR)Buffer,Size) == Size)
						Error = PopChunk(Handle);
					else
						Error = IoErr();
				}
			}

			if(!Error)
				Error = PopChunk(Handle);

			CloseIFFClip(Handle);
		}
		else
			Error = IoErr();
	}
	else
		Error = 0;

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
		return(TRUE);
}

	/* LoadClip(STRPTR Buffer,LONG Size):
	 *
	 *	Put the contents of the clipboard into a given
	 *	buffer. Note that only the first FTXT chunk will
	 *	be read. Since this code will only be called by
	 *	the clipboard server process which serves the
	 *	string gadget editing hook, this will hopefully
	 *	not be fatal. If you want more data to be read,
	 *	including multiple FTXT chunks, use the OpenClip(),
	 *	GetClip(), CloseClip() combo above.
	 */

LONG
LoadClip(STRPTR Buffer,LONG Size)
{
	struct IFFHandle *Handle;
	LONG Bytes;

	Bytes = 0;

	if(Handle = OpenIFFClip(Config->ClipConfig->ClipboardUnit,MODE_OLDFILE))
	{
		if(!StopChunk(Handle,ID_FTXT,ID_CHRS))
		{
			if(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				struct ContextNode *ContextNode = CurrentChunk(Handle);

				if(ContextNode->cn_Type == ID_FTXT)
				{
					if(Size > ContextNode->cn_Size)
						Size = ContextNode->cn_Size;

					if(ReadChunkRecords(Handle,Buffer,Size,1))
						Bytes = Size;
				}
			}
		}

		CloseIFFClip(Handle);
	}

	return(Bytes);
}

BOOL
WriteTranslatedToClip(struct IFFHandle *Handle,STRPTR Buffer,LONG Length)
{
	UBYTE LocalBuffer[256];
	LONG DestLen,i;
	LONG c;

	for(i = DestLen = 0 ; i < Length ; i++)
	{
		if(!(c = ISOConversion[Buffer[i]]))
			c = ' ';

		LocalBuffer[DestLen++] = c;

		if(DestLen == sizeof(LocalBuffer))
		{
			if(WriteChunkBytes(Handle,LocalBuffer,DestLen) != DestLen)
				return(FALSE);

			DestLen = 0;
		}
	}

	if(DestLen > 0)
	{
		if(WriteChunkBytes(Handle,LocalBuffer,DestLen) != DestLen)
			return(FALSE);
	}

	return(TRUE);
}
