/*
**	SaveWindow.c
**
**	Support routines for saving IFF-ILBM files
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Current compression mode. */

#define DUMP		0
#define RUN			1

	/* ByteRun compression data. */

#define MINRUN		3
#define MAXRUN		128
#define MAXDAT		128

	/* Local packer data. */

STATIC BYTE Buffer[MAXDAT + 1];
STATIC LONG PackedBytes;

	/* PutDump(PLANEPTR Destination,LONG Count):
	 *
	 *	Output a byte dump.
	 */

STATIC PLANEPTR
PutDump(PLANEPTR Destination,LONG Count)
{
	PLANEPTR Source = Buffer;

	*Destination++ = Count - 1;

	PackedBytes += Count + 1;

	while(Count--)
		*Destination++ = *Source++;

	return(Destination);
}

	/* PutRun(PLANEPTR Destination,LONG Count,WORD Char):
	 *
	 *	Output a byte run.
	 */

STATIC PLANEPTR
PutRun(PLANEPTR Destination,LONG Count,WORD Char)
{
	*Destination++ = -(Count - 1);
	*Destination++ = Char;

	PackedBytes += 2;

	return(Destination);
}

	/* PackRow(PLANEPTR *SourcePtr,PLANEPTR Destination,LONG RowSize):
	 *
	 *	Pack a row of bitmap data using ByteRun compression,
	 *	based on the original "EA IFF 85" pack.c example code.
	 */

STATIC LONG
PackRow(PLANEPTR *SourcePtr,PLANEPTR Destination,LONG RowSize)
{
	PLANEPTR Source;
	LONG Buffered,RunStart;
	LONG Mode,LastChar,Char;

	Source		= *SourcePtr;
	Buffered	= 1,
	RunStart	= 0;
	Mode		= DUMP;

	PackedBytes = 0;

	Buffer[0] = LastChar = Char = *Source++;

	RowSize--;

	while(RowSize--)
	{
		Buffer[Buffered++] = Char = *Source++;

		if(Mode)
		{
			if((Char != LastChar) || (Buffered - RunStart > MAXRUN))
			{
				Destination	= PutRun(Destination,Buffered - 1 - RunStart,LastChar);
				Buffer[0]	= Char;
				Buffered	= 1;
				RunStart	= 0;
				Mode		= DUMP;
			}
		}
		else
		{
			if(Buffered > MAXDAT)
			{
				Destination	= PutDump(Destination,Buffered - 1);
				Buffer[0]	= Char;
				Buffered	= 1;
				RunStart	= 0;
			}
			else
			{
				if(Char == LastChar)
				{
					if(Buffered - RunStart >= MINRUN)
					{
						if(RunStart)
							Destination = PutDump(Destination,RunStart);

						Mode = RUN;
					}
					else
					{
						if(!RunStart)
							Mode = RUN;
					}
				}
				else
					RunStart = Buffered - 1;
			}
		}

		LastChar = Char;
	}

	if(Mode)
		PutRun(Destination,Buffered - RunStart,LastChar);
	else
		PutDump(Destination,Buffered);

	*SourcePtr = Source;

	return(PackedBytes);
}

	/* PutANNO(struct IFFHandle *Handle):
	 *
	 *	Output `ANNO' chunk.
	 */

STATIC BOOL
PutANNO(struct IFFHandle *Handle)
{
	LONG Len;
	LONG Error;

	Len = strlen(&VersTag[1]);

		/* Push the `ANNO' chunk on the stack. */

	if(!(Error = PushChunk(Handle,0,ID_ANNO,Len)))
	{
			/* Write the creator string. */

		if(WriteChunkBytes(Handle,&VersTag[1],Len) == Len)
		{
				/* Pop the `ANNO' chunk. */

			if(!(Error = PopChunk(Handle)))
				return(TRUE);
		}
		else
			Error = IoErr();
	}

	SetIoErr(Error);

	return(FALSE);
}

	/* PutBMHD():
	 *
	 *	Output `BMHD' chunk.
	 */

STATIC BOOL
PutBMHD(struct IFFHandle *Handle,struct Window *Window,UBYTE Compression,LONG Left,LONG Top,LONG Width,LONG Height)
{
	struct DisplayInfo DisplayInfo;
	LONG Error;

		/* Get the display aspect ratio. */

	if(GetDisplayInfoData(NULL,(APTR)&DisplayInfo,sizeof(struct DisplayInfo),DTAG_DISP,GetVPModeID(&Window->WScreen->ViewPort)))
	{
		struct BitMapHeader Header;

			/* Fill in the bitmap header. */

		Header.bmh_Width		= Width;
		Header.bmh_Height		= Height;
		Header.bmh_PageWidth	= Window->WScreen->Width;
		Header.bmh_PageHeight	= Window->WScreen->Height;
		Header.bmh_Left			= Left;
		Header.bmh_Top			= Top;
		Header.bmh_Depth		= GetBitMapDepth(Window->WScreen->RastPort.BitMap);
		Header.bmh_Masking		= mskNone;
		Header.bmh_Compression	= Compression;
		Header.bmh_Pad			= 0;
		Header.bmh_Transparent	= 0;
		Header.bmh_XAspect		= DisplayInfo.Resolution.x;
		Header.bmh_YAspect		= DisplayInfo.Resolution.y;

			/* Push the `BMHD' chunk on the stack. */

		if(!(Error = PushChunk(Handle,0,ID_BMHD,sizeof(Header))))
		{
				/* Write the bitmap header. */

			if(WriteChunkBytes(Handle,&Header,sizeof(Header)) == sizeof(Header))
			{
					/* Pop the `BMHD' chunk. */

				if(!(Error = PopChunk(Handle)))
					return(TRUE);
			}
			else
				Error = IoErr();
		}
	}
	else
		Error = ERROR_NO_FREE_STORE;

	if(Error)
		SetIoErr(Error);

	return(FALSE);
}

	/* PutCMAP(struct IFFHandle *Handle,struct ViewPort *VPort):
	 *
	 *	Output `CMAP' chunk, only 4-bit colour registers
	 *	are supported so far.
	 */

STATIC BOOL
PutCMAP(struct IFFHandle *Handle,struct ViewPort *VPort)
{
	LONG Error;

		/* Push the `CMAP' chunk on the stack. */

	if(!(Error = PushChunk(Handle,0,ID_CMAP,3 * VPort->ColorMap->Count)))
	{
		struct ColorRegister Colour;
		LONG i;

			/* Read and convert all the
			 * ColorMap entries (4 bit colour
			 * components only).
			 */


		for(i = 0 ; i < VPort->ColorMap->Count ; i++)
		{
			if(Kick30)
			{
				ULONG RGB[3];

					/* Read colour value. */

				GetRGB32(VPort->ColorMap,i,1,RGB);

					/* Store the colour components. */

				Colour.red		= RGB[0] >> 24;
				Colour.green	= RGB[1] >> 24;
				Colour.blue		= RGB[2] >> 24;
			}
			else
			{
				ULONG Value;

					/* Read colour value. */

				Value = GetRGB4(VPort->ColorMap,i);

					/* Split the value into components and store them. */

				Colour.red		= ((Value >> 8) & 0xF) * 0x11;
				Colour.green	= ((Value >> 4) & 0xF) * 0x11;
				Colour.blue		= ((Value     ) & 0xF) * 0x11;
			}

				/* Write the colours. */

			if(WriteChunkBytes(Handle,&Colour,3) != 3)
				return(FALSE);
		}

			/* Pop the `CMAP' chunk. */

		if(!(Error = PopChunk(Handle)))
			return(TRUE);
	}

	SetIoErr(Error);

	return(FALSE);
}

	/* PutCAMG(struct IFFHandle *Handle,struct ViewPort *VPort):
	 *
	 *	Output `CAMG' chunk.
	 */

STATIC BOOL
PutCAMG(struct IFFHandle *Handle,struct ViewPort *VPort)
{
	LONG Error;

	if(!(Error = PushChunk(Handle,0,ID_CAMG,sizeof(ULONG))))
	{
		ULONG ViewModes = GetVPModeID(VPort);

		if(WriteChunkBytes(Handle,&ViewModes,sizeof(ULONG)) == sizeof(ULONG))
		{
			if(!(Error = PopChunk(Handle)))
				return(TRUE);
		}
		else
			Error = IoErr();
	}

	if(Error)
		SetIoErr(Error);

	return(FALSE);
}

	/* PutBODY(struct IFFHandle *Handle)
	 *
	 *	Output `BODY' chunk.
	 */

STATIC BOOL
PutBODY(struct IFFHandle *Handle,struct BitMap *BitMap,UBYTE Compression)
{
	PLANEPTR *Planes;
	BOOL Success;
	LONG Error;

	Success = FALSE;

		/* Allocate temporary bitplane pointers. */

	if(Planes = (PLANEPTR *)AllocVecPooled(BitMap->Depth * sizeof(PLANEPTR *),MEMF_ANY | MEMF_CLEAR))
	{
		LONG i;

			/* Copy the bitplane pointers. */

		for(i = 0 ; i < BitMap->Depth ; i++)
			Planes[i] = BitMap->Planes[i];

			/* Are we to compress the data? */

		if(Compression == cmpByteRun1)
		{
			PLANEPTR PackBuffer;

				/* Allocate line compression buffer. */

			if(PackBuffer = (PLANEPTR)AllocVecPooled(BitMap->BytesPerRow * 2,MEMF_ANY))
			{
					/* Push the `BODY' chunk on the stack. */

				if(!(Error = PushChunk(Handle,0,ID_BODY,IFFSIZE_UNKNOWN)))
				{
					LONG PackedBytes,j;

						/* So far, we are quite successful,
						 * any write access to fail will
						 * cause `Success' to drop to FALSE.
						 */

					Success = TRUE;

						/* Compress all the rows. */

					for(i = 0 ; Success && i < BitMap->Rows ; i++)
					{
							/* Compress all the planes. */

						for(j = 0 ; Success && j < BitMap->Depth ; j++)
						{
								/* Do the compression. */

							PackedBytes = PackRow(&Planes[j],PackBuffer,BitMap->BytesPerRow);

								/* Write the compressed data. */

							if(WriteChunkBytes(Handle,PackBuffer,PackedBytes) != PackedBytes)
							{
								Error = IoErr();

								Success = FALSE;
							}
						}
					}

						/* Pop the `BODY' chunk. */

					if(Success)
					{
						if(Error = PopChunk(Handle))
							Success = FALSE;
					}
				}

					/* Free the line compression buffer. */

				FreeVecPooled(PackBuffer);
			}
			else
				Error = ERROR_NO_FREE_STORE;
		}
		else
		{
				/* Push the `BODY' chunk on the stack. */

			if(!(Error = PushChunk(Handle,0,ID_BODY,IFFSIZE_UNKNOWN)))
			{
				LONG j;

					/* So far, we are quite successful,
					 * any write access to fail will
					 * cause `Success' to drop to FALSE.
					 */

				Success = TRUE;

					/* Compress all the rows. */

				for(i = 0 ; Success && i < BitMap->Rows ; i++)
				{
						/* Compress all the planes. */

					for(j = 0 ; Success && j < BitMap->Depth ; j++)
					{
							/* Write the row. */

						if(WriteChunkBytes(Handle,Planes[j],BitMap->BytesPerRow) != BitMap->BytesPerRow)
						{
							Error = IoErr();

							Success = FALSE;
						}
						else
							Planes[j] += BitMap->BytesPerRow;
					}
				}

					/* Pop the `BODY' chunk. */

				if(Success)
				{
					if(Error = PopChunk(Handle))
						Success = FALSE;
				}
			}
		}

			/* Free the temporary bitplane pointers. */

		FreeVecPooled(Planes);
	}
	else
		Error = ERROR_NO_FREE_STORE;

	if(Error)
		SetIoErr(Error);

		/* Return the result. */

	return(Success);
}

	/* SaveWindow(STRPTR Name,struct Window *Window):
	 *
	 *	Save the contents of a window to a file.
	 */

BOOL
SaveWindow(STRPTR Name,struct Window *Window)
{
	BOOL NewFile;
	LONG Error;

	NewFile	= FALSE,
	Error	= 0;

		/* Datatypes available? */

	if(DataTypesBase)
	{
		struct BitMap *BitMap;
		struct Screen *Screen;
		LONG Left,Top,Width,Height;
		Object *Picture;

		Screen = Window->WScreen;
		Picture = NULL;

			/* Keep these handy */

		Left	= Window->LeftEdge	+ Window->BorderLeft;
		Top		= Window->TopEdge	+ Window->BorderTop;
		Width	= Window->Width		- (Window->BorderLeft + Window->BorderRight);
		Height	= Window->Height	- (Window->BorderTop + Window->BorderBottom);

		if(!StatusWindow)
			Height -= StatusDisplayHeight;

			/* This is where the window contents will go */

		if(BitMap = AllocBitMap(Width,Height,GetBitMapAttr(Window->RPort->BitMap,BMA_DEPTH),0,Window->RPort->BitMap))
		{
			struct RastPort RPort;
			LONG NumColours;

				/* Provide temporary storage */

			InitRastPort(&RPort);

			RPort.BitMap = BitMap;

				/* Copy the window contents */

			ClipBlit(Window->RPort,Window->BorderLeft,Window->BorderTop,&RPort,0,0,Width,Height,MINTERM_COPY);

				/* Wait until all data is transferred */

			WaitBlit();

				/* Get the number of colours available on this screen */

			NumColours = Screen->ViewPort.ColorMap->Count;

				/* Create a picture object */

			if(Picture = NewDTObject("term image",
				DTA_SourceType,	DTST_RAM,
				DTA_GroupID,	GID_PICTURE,
				PDTA_NumColors,	NumColours,
				PDTA_BitMap,	BitMap,
				PDTA_ModeID,	GetVPModeID(&Screen->ViewPort),
			TAG_DONE))
			{
				struct ColorRegister *ColourMap;
				struct BitMapHeader *Header;
				ULONG *Colours;

					/* Get the internal data arrays */

				if(GetDTAttrs(Picture,
					PDTA_BitMapHeader,		&Header,
					PDTA_ColorRegisters,	&ColourMap,
					PDTA_CRegs,				&Colours,
				TAG_DONE) == 3)
				{
					LONG i;

						/* Fill in the bitmap header */

					Header->bmh_Left		= Left;
					Header->bmh_Top			= Top;
					Header->bmh_Width		= Width;
					Header->bmh_Height		= Height;
					Header->bmh_Depth		= GetBitMapAttr(BitMap,BMA_DEPTH);
					Header->bmh_PageWidth	= Screen->Width;
					Header->bmh_PageHeight	= Screen->Height;

						/* Get the 32 colours */

					GetRGB32(Screen->ViewPort.ColorMap,0,NumColours,Colours);

						/* Get the 24 bit colours */

					for(i = 0 ; i < NumColours ; i++)
					{
						ColourMap[i].red	= (UBYTE)(Colours[i * 3 + 0] >> 24);
						ColourMap[i].green	= (UBYTE)(Colours[i * 3 + 1] >> 24);
						ColourMap[i].blue	= (UBYTE)(Colours[i * 3 + 2] >> 24);
					}
				}
				else
				{
					Error = IoErr();

					DisposeDTObject(Picture);

					Picture = NULL;
				}
			}
			else
				Error = IoErr();
		}
		else
			Error = ERROR_NO_FREE_STORE;

			/* Successful so far? */

		if(Picture)
		{
			BPTR FileHandle;

				/* Open the output file */

			if(FileHandle = Open(Name,MODE_NEWFILE))
			{
					/* Save the image */

				if(!DoMethod(Picture,DTM_WRITE,NULL,FileHandle,DTWM_IFF,NULL))
				{
					Error = IoErr();

					NewFile = TRUE;
				}

				Close(FileHandle);
			}
			else
				Error = IoErr();

			DisposeDTObject(Picture);
		}
		else
		{
			if(BitMap)
				FreeBitMap(BitMap);
		}
	}
	else
	{
		struct RastPort *RPort;
		LONG Depth;

			/* Get the depth. */

		Depth = GetBitMapDepth(Window->WScreen->RastPort.BitMap);

		if(Depth > 8)
		{
			SetIoErr(ERROR_OBJECT_WRONG_TYPE);

			return(FALSE);
		}

			/* Allocate a dummy rastport, we will need only
			 * to copy the contents of the window into the
			 * bitmap.
			 */

		if(RPort = (struct RastPort *)AllocVecPooled(sizeof(struct RastPort),MEMF_ANY))
		{
			struct BitMap *BitMap;

				/* Initialize the rastport with defaults. */

			InitRastPort(RPort);

				/* Allocate a bitmap. */

			if(BitMap = (struct BitMap *)AllocVecPooled(sizeof(struct BitMap),MEMF_ANY))
			{
				LONG i,Left,Top,Width,Height;

				Width	= Window->Width		- (Window->BorderLeft + Window->BorderRight);
				Height	= Window->Height	- (Window->BorderTop + Window->BorderBottom);
				Left	= Window->LeftEdge	+ Window->BorderLeft;
				Top		= Window->TopEdge	+ Window->BorderTop;

				if(!StatusWindow)
					Height -= StatusDisplayHeight;

					/* Put it into the rastport. */

				RPort->BitMap = BitMap;

					/* Initialize it with the window dimensions. */

				InitBitMap(BitMap,Depth,Width,Height);

					/* Allocate all the bitplanes necessary. */

				for(i = 0 ; !Error && i < BitMap->Depth ; i++)
				{
					if(!(BitMap->Planes[i] = AllocRaster(Width,Height)))
						Error = ERROR_NO_FREE_STORE;
				}

					/* Did we get all the planes we wanted? */

				if(!Error)
				{
					struct IFFHandle *Handle;

						/* Copy the window contents to the
						 * local bitmap.
						 */

					ClipBlit(Window->RPort,Window->BorderLeft,Window->BorderTop,RPort,0,0,Width,Height,MINTERM_COPY);

						/* Wait for the data to arrive. */

					WaitBlit();

						/* Allocate an iff handle. */

					if(Handle = OpenIFFStream(Name,MODE_NEWFILE))
					{
						NewFile = TRUE;

							/* Push parent chunk on the
							 * stack.
							 */

						if(!(Error = PushChunk(Handle,ID_ILBM,ID_FORM,IFFSIZE_UNKNOWN)))
						{
								/* Output all the chunk data. */

							if(PutANNO(Handle))
							{
								UBYTE Compression;

									/* Don't compress the bitmap if
									 * isn't really worth it.
									 */

								if(BitMap->BytesPerRow > 4)
									Compression = cmpByteRun1;
								else
									Compression = cmpNone;

								if(PutBMHD(Handle,Window,Compression,Left,Top,Width,Height))
								{
									if(PutCMAP(Handle,&Window->WScreen->ViewPort))
									{
										if(PutCAMG(Handle,&Window->WScreen->ViewPort))
										{
											if(PutBODY(Handle,BitMap,Compression))
											{
													/* Pop the parent chunk
													 * from the stack.
													 */

												Error = PopChunk(Handle);
											}
											else
												Error = IoErr();
										}
										else
											Error = IoErr();
									}
									else
										Error = IoErr();
								}
								else
									Error = IoErr();
							}
							else
								Error = IoErr();
						}

							/* Free the iff handle. */

						CloseIFFStream(Handle);
					}
					else
						Error = IoErr();
				}
				else
					Error = ERROR_NO_FREE_STORE;

					/* Free all bitplanes. */

				for(i = 0 ; i < BitMap->Depth ; i++)
				{
					if(BitMap->Planes[i])
						FreeRaster(BitMap->Planes[i],Width,Height);
				}

					/* Free the bitmap. */

				FreeVecPooled(BitMap);
			}
			else
				Error = ERROR_NO_FREE_STORE;

				/* Free the rastport. */

			FreeVecPooled(RPort);
		}
		else
			Error = ERROR_NO_FREE_STORE;
	}

		/* Did we get an error? */

	if(Error)
	{
		if(NewFile)
			DeleteFile(Name);

		SetIoErr(Error);

		return(FALSE);
	}
	else
	{
			/* If successful, clear the `executable' bit. */

		AddProtection(Name,FIBF_EXECUTE);

		if(Config->MiscConfig->CreateIcons)
			AddIcon(CaptureName,FILETYPE_PICTURE,TRUE);

		return(TRUE);
	}
}
