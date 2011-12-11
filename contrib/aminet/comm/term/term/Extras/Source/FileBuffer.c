/*
**	FileBuffer.c
**
**	Double-buffered file I/O routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Argument types. */

enum	{	ARG_NAME,ARG_MODE };
enum	{	ARG_OFFSET,ARG_ORIGIN };

	/* Seek offsets. */

enum	{	SEEKFILE_SET,SEEKFILE_CURR,SEEKFILE_END };

	/* Command codes. */

enum	{	BUF_CLOSE,BUF_SEEK,BUF_FILL,BUF_FLUSH };

	/* OpenFileWithMode(STRPTR Name,STRPTR ModeString):
	 *
	 *	Open a plain AmigaDOS file and turn the stdio mode string
	 *	into the appropriate Open() mode.
	 */

STATIC BPTR
OpenFileWithMode(CONST_STRPTR Name,CONST_STRPTR ModeString)
{
	LONG Mode;

	switch(ModeString[0])
	{
		case 'r':

			if(ModeString[1] == '+')
				Mode = MODE_READWRITE;
			else
				Mode = MODE_OLDFILE;

			break;

		case 'w':

			if(ModeString[1] == '+')
			{
				BPTR SomeLock;

				if(SomeLock = Lock(Name,EXCLUSIVE_LOCK))
				{
					UnLock(SomeLock);

					if(!DeleteFile(Name))
						return(BNULL);
				}

				Mode = MODE_READWRITE;
			}
			else
				Mode = MODE_NEWFILE;

			break;

		case 'a':

			return(OpenToAppend(Name,NULL));

		default:

			return(BNULL);
	}

	return(Open(Name,Mode));
}

	/* ObtainInfo(struct Buffer *File):
	 *
	 *	Obtain information on the disk a buffered file
	 *	resides on.
	 */

STATIC VOID
ObtainInfo(struct Buffer *File)
{
		/* Lock on the path available? */

	if(File->InfoPort)
	{
			/* This is more or less a trick; we try to obtain
			 * information on a filing system without having a
			 * shared lock on it. We cannot DupLockFromFH()
			 * the file handle since it was allocated for
			 * exclusive access. The ACTION_DISK_INFO packet
			 * solves this problem as it requires only the
			 * filing system task (MsgPort) to be given.
			 */

		if(!DoPkt(File->InfoPort,ACTION_DISK_INFO,(SIPTR)MKBADDR(&File->InfoData),0,0,0,0))
		{
			File->InfoData.id_NumBlocks		= 0;
			File->InfoData.id_BytesPerBlock	= 0;
		}
	}
}

	/* FileBufferServer():
	 *
	 *	Background process to handle the buffering
	 *	of a filehandle, automatically gets invoked
	 *	when a file is opened.
	 */

STATIC VOID SAVE_DS
FileBufferServer(VOID)
{
	BOOL Terminated,Done,WasFull;
	struct Buffer *Buffer;
	struct MsgPort *Port;
	struct Process *Me;
	UBYTE *String;
	LONG Length;
	APTR Data;

	Terminated = FALSE;

		/* Wait for startup message (-> Buffer). */

	Me = (struct Process *)FindTask(NULL);

	Port = &Me->pr_MsgPort;

	WaitPort(Port);

	Buffer = (struct Buffer *)GetMsg(Port);

		/* Open the file and obtain a filehandle. */

	String = (STRPTR)Buffer->ActionData[ARG_MODE];

		/* Put the message into the list. */

	ObtainSemaphore(&DoubleBufferSemaphore);

	AddTail(&DoubleBufferList,(struct Node *)Buffer);

	ReleaseSemaphore(&DoubleBufferSemaphore);

		/* Remember the opening date. */

	DateStamp(&Buffer->OpenDate);

		/* Check for the open type. */

	Buffer->WriteAccess = (BOOL)(String[0] != 'r');

	Buffer->FileHandle = OpenFileWithMode((STRPTR)Buffer->ActionData[ARG_NAME],String);

		/* Clear signal bit. */

	ClrSignal(SIG_COMMAND);

		/* Did the file open? */

	if(Buffer->FileHandle)
	{
		Buffer->Data		= Buffer->DataBuffer[0];
		Buffer->DataCount	= 1;
		Buffer->Fresh		= TRUE;

			/* If not in write mode fill the buffers. */

		if(!Buffer->WriteAccess)
		{
				/* Fill the first one synchronously. */

			Buffer->ReadBufFull		= Read(Buffer->FileHandle,Buffer->Data,Buffer->BufLength);
			Buffer->Read			= TRUE;
			Buffer->RealPosition	= Buffer->ReadBufFull;

				/* Restart caller. */

			Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);

				/* Fill the second buffe asynchronously. */

			Buffer->DataLength[1]	 = Buffer->Cached = Read(Buffer->FileHandle,Buffer->DataBuffer[1],Buffer->BufLength);
			Buffer->RealPosition	+= Buffer->Cached;
		}
		else
		{
			Buffer->InfoPort = ((struct FileHandle *)BADDR(Buffer->FileHandle))->fh_Type;
			ObtainInfo(Buffer);

			Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);
		}
	}
	else
		Terminated = TRUE;

		/* Go into loop waiting for commands. */

	while(!Terminated)
	{
		Wait(SIG_COMMAND);

		Done = FALSE;

		Buffer->Result = 0;

			/* Take care of each action. */

		switch(Buffer->Action)
		{
				/* Close the file, flush any dirty
				 * buffers and exit.
				 */

			case BUF_CLOSE:

				Buffer->Result = TRUE;

				if(Buffer->BufPosition && Buffer->Written)
				{
					if(Write(Buffer->FileHandle,Buffer->Data,Buffer->BufPosition) != Buffer->BufPosition)
						Buffer->Result = FALSE;
				}

				if(!Close(Buffer->FileHandle))
					Buffer->Result = FALSE;

				Terminated = TRUE;

				break;

				/* Seek to a specific file position. */

			case BUF_SEEK:

				Buffer->Result = 0;

					/* Do nothing if buffer is still
					 * untouched and we are required
					 * to seek back to the beginning
					 * of the file.
					 */

				if(Buffer->Fresh && !Buffer->ActionData[ARG_OFFSET] && Buffer->ActionData[ARG_ORIGIN] == SEEKFILE_SET)
				{
					Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);

					Done = TRUE;
				}
				else
				{
					Buffer->WriteBufFull	= Buffer->BufLength;
					Buffer->Read			= FALSE;

					if(Buffer->BufPosition && Buffer->Written)
					{
						if(Write(Buffer->FileHandle,Buffer->Data,Buffer->BufPosition) != Buffer->BufPosition)
							Buffer->Result = -1;
						else
							ObtainInfo(Buffer);
					}

					if(!Buffer->Result)
					{
						Buffer->Result = Buffer->RealPosition - (Buffer->ReadBufFull + Buffer->Cached);

						switch(Buffer->ActionData[ARG_ORIGIN])
						{
							case SEEKFILE_SET:

								if(Seek(Buffer->FileHandle,Buffer->ActionData[ARG_OFFSET],OFFSET_BEGINNING) != -1)
									Buffer->RealPosition = Buffer->ActionData[ARG_OFFSET];
								else
									Buffer->Result = -1;

								break;

							case SEEKFILE_CURR:

								if(!Buffer->WriteAccess && Buffer->ActionData[ARG_OFFSET] >= 0 && Buffer->ReadBufFull - Buffer->ActionData[ARG_OFFSET] >= 0)
								{
									Buffer->ReadBufFull	-= Buffer->ActionData[ARG_OFFSET];
									Buffer->Data		+= Buffer->ActionData[ARG_OFFSET];

									Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);

									Done = TRUE;

									break;
								}

								if(Seek(Buffer->FileHandle,-(Buffer->ReadBufFull + Buffer->Cached) + Buffer->ActionData[ARG_OFFSET],OFFSET_CURRENT) != -1)
									Buffer->RealPosition += -(Buffer->ReadBufFull + Buffer->Cached) + Buffer->ActionData[ARG_OFFSET];
								else
									Buffer->Result = -1;

								break;

							case SEEKFILE_END:

								if(Seek(Buffer->FileHandle,Buffer->ActionData[ARG_OFFSET],OFFSET_END) != -1)
									Buffer->RealPosition = Seek(Buffer->FileHandle,0,OFFSET_CURRENT);
								else
									Buffer->Result = -1;

								break;

							default:

								Buffer->Result = -1;
						}

						Buffer->ReadBufFull = 0;

						if(Buffer->Result != -1)
						{
							Buffer->Data		= Buffer->DataBuffer[0];
							Buffer->DataCount	= 1;

							if(!Buffer->WriteAccess)
							{
								Buffer->ReadBufFull		 = Read(Buffer->FileHandle,Buffer->Data,Buffer->BufLength);
								Buffer->WriteBufFull	 = 0;
								Buffer->Read			 = TRUE;
								Buffer->RealPosition	+= Buffer->ReadBufFull;

								if(Buffer->ReadBufFull)
								{
									Buffer->Cached = Buffer->DataLength[1] = Read(Buffer->FileHandle,Buffer->DataBuffer[1],Buffer->BufLength);

									Buffer->RealPosition += Buffer->Cached;
								}
							}
						}
						else
							Buffer->LastActionFailed = TRUE;
					}
					else
						Buffer->ReadBufFull = 0;

					Buffer->BufPosition	= 0;
					Buffer->Written	= FALSE;
				}

				break;

				/* Fill the buffer with fresh data. */

			case BUF_FILL:

				Buffer->Data			= Buffer->DataBuffer[Buffer->DataCount];
				Buffer->ReadBufFull		= Buffer->DataLength[Buffer->DataCount];
				Buffer->WriteBufFull	= 0;
				Buffer->BufPosition		= 0;
				Buffer->Read			= TRUE;
				Buffer->Written			= FALSE;
				Buffer->Fresh			= FALSE;

				if(Buffer->ReadBufFull)
					WasFull = TRUE;
				else
					WasFull = FALSE;

					/* The buffer contents have been
					 * swapped, now wake the caller
					 * up and fill the next buffer
					 * asynchronously.
					 */

				Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);

				Done = TRUE;

				if(WasFull)
				{
					Buffer->DataCount = (Buffer->DataCount + 1) % BUFFER_NUMBER;

					Buffer->Cached = Buffer->DataLength[Buffer->DataCount] = Read(Buffer->FileHandle,Buffer->DataBuffer[Buffer->DataCount],Buffer->BufLength);

					Buffer->RealPosition += Buffer->Cached;

					if(!Buffer->DataLength[Buffer->DataCount])
					{
						if(IoErr())
							Buffer->LastActionFailed = TRUE;
					}
				}

				break;

				/* Flush the contents of the buffer to disk. */

			case BUF_FLUSH:

				if(Buffer->BufPosition && Buffer->Written)
				{
					Data					= Buffer->Data;
					Length					= Buffer->BufPosition;

					Buffer->Data			= Buffer->DataBuffer[Buffer->DataCount];
					Buffer->DataCount		= (Buffer->DataCount + 1) % BUFFER_NUMBER;

					Buffer->ReadBufFull		= 0;
					Buffer->WriteBufFull	= Buffer->BufLength;
					Buffer->BufPosition		= 0;
					Buffer->Read			= FALSE;
					Buffer->Written			= FALSE;

					Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);

					Done = TRUE;

					if(Write(Buffer->FileHandle,Data,Length) != Length)
						Buffer->LastActionFailed = TRUE;
					else
					{
						ObtainInfo(Buffer);

						Buffer->RealPosition += Length;
					}
				}
				else
				{
					Buffer->ReadBufFull		= 0;
					Buffer->WriteBufFull	= Buffer->BufLength;
					Buffer->BufPosition		= 0;
					Buffer->Read			= FALSE;
					Buffer->Written			= FALSE;
				}

				Buffer->Fresh = FALSE;

				break;
		}

			/* Ring back if necessary. */

		if(!Done && !Terminated)
			Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);
	}

		/* Remove the message from the list. */

	ObtainSemaphore(&DoubleBufferSemaphore);

	Remove((struct Node *)Buffer);

	ReleaseSemaphore(&DoubleBufferSemaphore);

		/* Lock & quit. */

	Forbid();

	Signal((struct Task *)Buffer->Caller,SIG_HANDSHAKE);
}

	/* BufferFill(struct Buffer *Buffer):
	 *
	 *	Fills a given buffer with fresh data.
	 */

STATIC BOOL
BufferFill(struct Buffer *Buffer)
{
	if(Buffer->LastActionFailed)
		return(FALSE);
	else
	{
		if(!Buffer->ReadBufFull)
		{
			Buffer->Action = BUF_FILL;

			ShakeHands((struct Task *)Buffer->Child,SIG_COMMAND);
		}

		return(TRUE);
	}
}

	/* IsValidBuffer(struct Buffer *Buffer):
	 *
	 *	Scans the double buffered file list for
	 *	a valid entry.
	 */

STATIC BOOL
IsValidBuffer(struct Buffer *Buffer)
{
	struct Node *Node;
	BOOL GotIt;

	ObtainSemaphore(&DoubleBufferSemaphore);

	GotIt = FALSE;

	for(Node = DoubleBufferList.lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
	{
		if(Buffer == (struct Buffer *)Node)
		{
			GotIt = TRUE;

			break;
		}
	}

	ReleaseSemaphore(&DoubleBufferSemaphore);

	return(GotIt);
}

	/* OpenFileSimple(STRPTR Name,STRPTR AccessMode):
	 *
	 *	Open simple (unbuffered) file.
	 */

STATIC struct Buffer *
OpenFileSimple(CONST_STRPTR Name,CONST_STRPTR AccessMode)
{
	struct Buffer *Buffer;

		/* Allocate buffer handle (dummy). */

	if(Buffer = (struct Buffer *)AllocVecPooled(sizeof(struct Buffer),MEMF_ANY | MEMF_CLEAR))
	{
			/* Provide basic information. */

		DateStamp(&Buffer->OpenDate);

		Buffer->WriteAccess = (BOOL)(AccessMode[0] != 'r');

		Buffer->SimpleIO = TRUE;

		Buffer->FileHandle = OpenFileWithMode(Name,AccessMode);

			/* Did we succeed in opening the file? */

		if(Buffer->FileHandle)
		{
				/* To keep track of the free space still
				 * available on the filing system.
				 */

			if(Buffer->WriteAccess)
			{
				Buffer->InfoPort = ((struct FileHandle *)BADDR(Buffer->FileHandle))->fh_Type;
				ObtainInfo(Buffer);
			}

				/* Link the file into the list. */

			ObtainSemaphore(&DoubleBufferSemaphore);

			AddTail(&DoubleBufferList,(struct Node *)Buffer);

			ReleaseSemaphore(&DoubleBufferSemaphore);

			return(Buffer);
		}
		else
			FreeVecPooled(Buffer);
	}

	return(NULL);
}

	/* OpenFileBuffered(STRPTR Name,STRPTR AccessMode):
	 *
	 *	Open double-buffered file.
	 */

STATIC struct Buffer *
OpenFileBuffered(CONST_STRPTR Name,CONST_STRPTR AccessMode)
{
	struct Buffer *Buffer;
	LONG Size;

	Size = (Config->MiscConfig->IOBufferSize + 15) & ~15;

	if(Size < 2048)
		Size = 2048;

		/* Allocate the buffer data. */

	do
	{
		Buffer = (struct Buffer *)AllocVecPooled(sizeof(struct Buffer) + ((Size + 15) & ~15) * BUFFER_NUMBER + 15,MEMF_ANY | MEMF_CLEAR);

		Size /= 2;
	}
	while(!Buffer && Size > 2048);

	if(Buffer)
	{
		struct Process *Process;
		LONG i;

			/* Set up the first buffer. */

		Buffer->DataBuffer[0] = (UBYTE *)(((IPTR)(Buffer + 1) + 15) & ~15);

			/* Set up the individual buffers. */

		for(i = 1 ; i < BUFFER_NUMBER ; i++)
			Buffer->DataBuffer[i] = Buffer->DataBuffer[i - 1] + Size;

		Buffer->BufLength		= Size;
		Buffer->WriteBufFull	= Buffer->BufLength;

			/* Create the asynchronous file server. */

		if(!(Process = CreateNewProcTags(
			NP_Entry,		FileBufferServer,
			NP_Name,		"term File Process",
			NP_WindowPtr,	-1,
		TAG_DONE)))
		{
			FreeVecPooled(Buffer);

			return(NULL);
		}

			/* Set up the message header. */

		Buffer->Message.mn_Length		= sizeof(struct Buffer);

		Buffer->ActionData[ARG_NAME]	= (IPTR)Name;
		Buffer->ActionData[ARG_MODE]	= (IPTR)AccessMode;

		Buffer->Child	= Process;
		Buffer->Caller	= (struct Process *)FindTask(NULL);

		Forbid();

			/* Send it to the waiting server process. */

		PutMsg(&Process->pr_MsgPort,&Buffer->Message);

			/* Wait for ringback. */

		WaitForHandshake();

		Permit();

			/* Do we have a valid filehandle? */

		if(!Buffer->FileHandle)
		{
			FreeVecPooled(Buffer);

			return(NULL);
		}
		else
			return(Buffer);
	}

	return(NULL);
}

	/* BPrintf():
	 *
	 *	Prints text into a buffered file.
	 */

LONG
BPrintf(struct Buffer *Buffer,CONST_STRPTR Format,...)
{
	UBYTE	String[256];
	va_list	VarArgs;

	va_start(VarArgs,Format);
	LimitedVSPrintf(sizeof(String),String,Format,VarArgs);
	va_end(VarArgs);

	return(BufferWrite(Buffer,String,strlen(String)));
}

	/* BufferFlush(struct Buffer *Buffer):
	 *
	 *	Flush the contents of a given buffer to disk.
	 */

BOOL
BufferFlush(struct Buffer *Buffer)
{
	if(Buffer->LastActionFailed)
		return(FALSE);
	else
	{
		if(Buffer->BufPosition && Buffer->Written)
		{
			Buffer->Action = BUF_FLUSH;

			ShakeHands((struct Task *)Buffer->Child,SIG_COMMAND);
		}

		return(TRUE);
	}
}

	/* BufferClose(struct Buffer *Buffer):
	 *
	 *	Close a buffered filehandle.
	 */

BOOL
BufferClose(struct Buffer *Buffer)
{
	if(IsValidBuffer(Buffer))
	{
		BOOL Success;

			/* Unbuffered file handle? */

		if(Buffer->SimpleIO)
		{
				/* Close the file. */

			if(Close(Buffer->FileHandle))
				Success = TRUE;
			else
				Success = FALSE;

				/* Unlink the handle. */

			ObtainSemaphore(&DoubleBufferSemaphore);

			Remove((struct Node *)Buffer);

			ReleaseSemaphore(&DoubleBufferSemaphore);
		}
		else
		{
			Buffer->Action = BUF_CLOSE;

			ShakeHands((struct Task *)Buffer->Child,SIG_COMMAND);

			Success = Buffer->Result;
		}

		FreeVecPooled(Buffer);

		return(Success);
	}
	else
		return(FALSE);
}

	/* BufferOpen(STRPTR Name,STRPTR AccessMode):
	 *
	 *	Open a file for buffered I/O.
	 */

struct Buffer *
BufferOpen(CONST_STRPTR Name,CONST_STRPTR AccessMode)
{
		/* Simple file handling? */

	if(Config->MiscConfig->SimpleIO)
		return(OpenFileSimple(Name,AccessMode));
	else
	{
		struct Buffer *Buffer;

			/* Try to open a buffered file, if unsuccessful
			 * fall back to simple file I/O.
			 */

		if(!(Buffer = OpenFileBuffered(Name,AccessMode)))
			Buffer = OpenFileSimple(Name,AccessMode);

		return(Buffer);
	}
}

	/* BufferSeek(struct Buffer *Buffer,LONG Offset,LONG Origin):
	 *
	 *	Move the read/write pointer to a specific position
	 *	in a file (not really buffered).
	 */

BOOL
BufferSeek(struct Buffer *Buffer,LONG Offset,LONG Origin)
{
	if(Buffer->SimpleIO)
	{
		if(Seek(Buffer->FileHandle,Offset,Origin) == -1)
			return(FALSE);
		else
			return(TRUE);
	}
	else
	{
		Buffer->Action					= BUF_SEEK;
		Buffer->ActionData[ARG_OFFSET]	= Offset;
		Buffer->ActionData[ARG_ORIGIN]	= Origin;

		ShakeHands((struct Task *)Buffer->Child,SIG_COMMAND);

		if(Buffer->Result == -1)
			return(FALSE);
		else
			return(TRUE);
	}
}

	/* BufferRead():
	 *
	 *	Read data from a file (buffered).
	 */

LONG
BufferRead(struct Buffer *Buffer,STRPTR Destination,LONG Size)
{
	if(Buffer->SimpleIO)
	{
		LONG Bytes;

		Buffer->Used = TRUE;

		if((Bytes = Read(Buffer->FileHandle,Destination,Size)) < 0)
			Bytes = 0;

		return(Bytes);
	}
	else
	{
		LONG BytesRead,ToCopy,BufPosition,ReadBufFull;
		UBYTE *Data;

		BytesRead = 0;

			/* If there is still data to be written in
			 * the buffer, write it.
			 */

		if(Buffer->Written)
		{
			if(!BufferFlush(Buffer))
				return(0);
		}

			/* Set up for read access. */

		BufPosition		= Buffer->BufPosition;
		ReadBufFull		= Buffer->ReadBufFull;
		Data			= &Buffer->Data[BufPosition];

			/* Remember access. */

		Buffer->Used	= TRUE;

			/* Continue until all data has been processed. */

		while(Size)
		{
				/* Determine number of bytes to transfer. */

			if(ToCopy = (Size > ReadBufFull) ? ReadBufFull : Size)
			{
				CopyMem(Data,Destination,ToCopy);

				Size		-= ToCopy;
				BufPosition	+= ToCopy;
				ReadBufFull	-= ToCopy;
				Destination	+= ToCopy;
				Data		+= ToCopy;
				BytesRead	+= ToCopy;
			}
			else
			{
					/* Refill buffer with data. */

				Buffer->BufPosition	= BufPosition;
				Buffer->ReadBufFull	= ReadBufFull;

				if(!BufferFill(Buffer))
					return(BytesRead);

				if(!Buffer->ReadBufFull)
				{
					Buffer->BufPosition = BufPosition;

					return(BytesRead);
				}

					/* Pick up new data. */

				BufPosition		= Buffer->BufPosition;
				ReadBufFull		= Buffer->ReadBufFull;
				Data			= Buffer->Data;
			}
		}

			/* Install new data. */

		Buffer->BufPosition	= BufPosition;
		Buffer->ReadBufFull	= ReadBufFull;

		return(BytesRead);
	}
}

	/* BufferWrite():
	 *
	 *	Write data to a file (buffered).
	 */

LONG
BufferWrite(struct Buffer *Buffer,CONST_STRPTR Source,LONG Size)
{
	if(Buffer->SimpleIO)
	{
		LONG Bytes;

		Buffer->Used = TRUE;

		Buffer->WriteAccess = TRUE;

		if((Bytes = Write(Buffer->FileHandle,Source,Size)) < 0)
			Bytes = 0;
		else
		{
			Buffer->BufPosition += Bytes;

			if(Buffer->BufPosition >= Config->MiscConfig->IOBufferSize)
			{
				Buffer->BufPosition = 0;

				ObtainInfo(Buffer);
			}
		}

		return(Bytes);
	}
	else
	{
		LONG BytesWritten,ToCopy,BufPosition,WriteBufFull;
		UBYTE *Data;

		BytesWritten = 0;

			/* If there is still read data in the buffer,
			 * reset the control information.
			 */

		if(Buffer->Read)
		{
			Buffer->WriteBufFull	= Buffer->BufLength;
			Buffer->BufPosition		= 0;
			Buffer->Read			= FALSE;
		}

			/* Set up for write access. */

		Buffer->Written = TRUE;

		BufPosition		= Buffer->BufPosition;
		WriteBufFull	= Buffer->WriteBufFull;
		Data			= &Buffer->Data[BufPosition];

			/* Remember access. */

		Buffer->Used	= TRUE;

			/* Continue until all data has been processed. */

		while(Size)
		{
				/* Determine number of bytes to transfer. */

			if(ToCopy = (Size > WriteBufFull ? WriteBufFull : Size))
			{
				CopyMem(Source,Data,ToCopy);

				Size			-= ToCopy;
				BufPosition		+= ToCopy;
				WriteBufFull	-= ToCopy;
				Source			+= ToCopy;
				Data			+= ToCopy;
				BytesWritten	+= ToCopy;
			}
			else
			{
					/* Flush the contents of the
					 * write buffer.
					 */

				Buffer->BufPosition		= BufPosition;
				Buffer->WriteBufFull	= WriteBufFull;

				if(!BufferFlush(Buffer))
					return(BytesWritten);

					/* Pick up new data. */

				BufPosition		= Buffer->BufPosition;
				WriteBufFull	= Buffer->WriteBufFull;
				Data			= Buffer->Data;

					/* Important - or BufferFlush() won't
					 * write the final buffer contents when
					 * the buffered file handle is freed up.
					 */

				Buffer->Written = TRUE;
			}
		}

			/* Install new data. */

		Buffer->BufPosition		= BufPosition;
		Buffer->WriteBufFull	= WriteBufFull;

		return(BytesWritten);
	}
}
