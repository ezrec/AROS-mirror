/*
**	Translate.c
**
**	Character translation support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Translation chunk header types. */

enum	{	TYPE_NONE, TYPE_SEND, TYPE_RECEIVE };

	/* Master translation table entry types. */

enum	{	TRANSLATION_VERBATIM, TRANSLATION_IGNORE, TRANSLATION_OTHER };

	/* Symbolic names for character codes. */

STATIC struct { STRPTR Name; UBYTE Value; } CodeTab[] =
{
	{	"NUL",	0	},
	{	"SOH",	1	},
	{	"STX",	2	},
	{	"ETX",	3	},
	{	"EOT",	4	},
	{	"ENQ",	5	},
	{	"ACK",	6	},
	{	"BEL",	7	},
	{	"BS",	8	},
	{	"HT",	9	},
	{	"LF",	10	},
	{	"VT",	11	},
	{	"FF",	12	},
	{	"CR",	13	},
	{	"SO",	14	},
	{	"SI",	15	},
	{	"DLE",	16	},
	{	"DC1",	17	},
	{	"DC2",	18	},
	{	"DC3",	19	},
	{	"DC4",	20	},
	{	"NAK",	21	},
	{	"SYN",	22	},
	{	"ETB",	23	},
	{	"CAN",	24	},
	{	"EM",	25	},
	{	"SUB",	26	},
	{	"ESC",	27	},
	{	"FS",	28	},
	{	"GS",	29	},
	{	"RS",	30	},
	{	"US",	31	},
	{	"SP",	32	},
	{	"DEL",	127	},
	{	"SS2",	142	},
	{	"SS3",	143	},
	{	"DCS",	144	},
	{	"CSI",	155	},
	{	"ST",	156	},
	{	"OSC",	157	},
	{	"PM",	158	},
	{	"APC",	159	},
	{	"NBS",	160	},
	{	"SHY",	173	}
};

	/* TranslateSetup():
	 *
	 *	Set up for buffer translation.
	 */

VOID
TranslateSetup(struct TranslationHandle *Handle,STRPTR SourceBuffer,LONG SourceLen,STRPTR DestinationBuffer,LONG DestinationLen,struct TranslationEntry **Table)
{
	Handle->LocalBuffer			= NULL;
	Handle->LocalLen			= 0;

	Handle->SourceBuffer		= SourceBuffer;
	Handle->SourceLen			= SourceLen;

	Handle->DestinationBuffer	= DestinationBuffer;
	Handle->DestinationLen		= DestinationLen;

	Handle->Table				= Table;
}

	/* TranslateBuffer(struct TranslationHandle *Handle):
	 *
	 *	Translate buffer contents according to
	 *	translation table contents.
	 */

LONG
TranslateBuffer(struct TranslationHandle *Handle)
{
	struct TranslationEntry	*Entry;
	LONG BytesWritten;
	STRPTR Data;

	Data = Handle->DestinationBuffer;
	BytesWritten = 0;

		/* Are we to return any translated data? */

	while(Handle->LocalLen && BytesWritten < Handle->DestinationLen)
	{
			/* Decrement number of bytes in buffer. */

		Handle->LocalLen--;

			/* Return next character. */

		*Data++ = *Handle->LocalBuffer++;

			/* Add another byte. */

		BytesWritten++;
	}

		/* Loop until done. */

	while(Handle->SourceLen && BytesWritten < Handle->DestinationLen)
	{
			/* Another byte eaten. */

		Handle->SourceLen--;

			/* Get table entry. */

		if(Entry = Handle->Table[*Handle->SourceBuffer++])
		{
				/* Copy to local data area. */

			Handle->LocalBuffer	= Entry->String;
			Handle->LocalLen	= Entry->Len;

				/* Translate the data. */

			while(Handle->LocalLen && BytesWritten < Handle->DestinationLen)
			{
					/* Decrement number of bytes in buffer. */

				Handle->LocalLen--;

					/* Return next character. */

				*Data++ = *Handle->LocalBuffer++;

					/* Add another byte. */

				BytesWritten++;
			}
		}
	}

	return(BytesWritten);
}

	/* NameToCode(STRPTR Name):
	 *
	 *	Translate a symbolic character code name of numeral.
	 */

UBYTE
NameToCode(STRPTR Name)
{
	LONG i;

	for(i = 0 ; i < NUM_ELEMENTS(CodeTab) ; i++)
	{
		if(!Stricmp(CodeTab[i].Name,Name))
			return(CodeTab[i].Value);
	}

	return((UBYTE)Atol(Name));
}

	/* CodeToName(UBYTE Code):
	 *
	 *	Translate a character code into a symbolic
	 *	name or numeral.
	 */

STRPTR
CodeToName(UBYTE Code)
{
	STATIC UBYTE Name[6];

	LONG i;

	for(i = 0 ; i < NUM_ELEMENTS(CodeTab) ; i++)
	{
		if(CodeTab[i].Value == Code)
			return(CodeTab[i].Name);
	}

	LimitedSPrintf(sizeof(Name),Name,"%03ld",Code);

	return(Name);
}

	/* FreeTranslationTable(struct TranslationEntry **Table):
	 *
	 *	Free a character translation table.
	 */

VOID
FreeTranslationTable(struct TranslationEntry **Table)
{
	LONG i;

	for(i = 0 ; i < 256 ; i++)
	{
		if(Table[i])
			FreeTranslationEntry(Table[i]);
	}

	FreeVecPooled(Table);
}

	/* AllocTranslationTable():
	 *
	 *	Allocate a character translation table.
	 */

struct TranslationEntry **
AllocTranslationTable()
{
	return((struct TranslationEntry **)AllocVecPooled(sizeof(struct TranslationEntry *) * 256,MEMF_ANY|MEMF_CLEAR));
}

	/* FreeTranslationEntry(struct TranslationEntry *Entry):
	 *
	 *	Free a character translation table entry.
	 */

VOID
FreeTranslationEntry(struct TranslationEntry *Entry)
{
	FreeVecPooled(Entry);
}

	/* TranslateString(STRPTR From,STRPTR To):
	 *
	 *	Translate a string to contain control codes
	 *	into a string to contain the real codes.
	 */

LONG
TranslateString(CONST_STRPTR From,STRPTR To)
{
	BOOL GotControl,GotEscape;
	LONG Len,Count,i;

	GotControl	= FALSE;
	GotEscape	= FALSE;

	Len		= strlen(From);
	Count	= 0;

	for(i = 0 ; i < Len ; i++)
	{
		if(!GotControl && !GotEscape)
		{
			if(From[i] == '\\')
			{
				GotControl = TRUE;

				continue;
			}

			if(From[i] == '^')
			{
				GotEscape = TRUE;

				continue;
			}

			To[Count++] = From[i];
		}
		else
		{
			if(GotEscape)
			{
				if(ToUpper(From[i]) >= 'A' && ToUpper(From[i]) <= '_')
					To[Count++] = ToUpper(From[i]) - '@';
				else
					To[Count++] = From[i];

				GotEscape = FALSE;
			}
			else
			{
				if(GotControl)
				{
					switch(ToUpper(From[i]))
					{
							/* These macro commands are plainly
							 * ignored.
							 */

						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':

							break;

							/* These are macro commands and
							 * not supported.
							 */

						case 'A':
						case 'C':
						case 'D':
						case 'G':
						case 'H':
						case 'I':
						case 'P':
						case 'U':
						case 'X':

							To[Count++] = '\\';
							To[Count++] = From[i];
							break;

							/* Translate code. */

						case '*':

							i++;

							while(i < Len && From[i] == ' ')
								i++;

							if(i < Len)
							{
								UBYTE DummyBuffer[5],j = 0,Char;

								if(From[i] >= '0' && From[i] <= '9')
								{
									while(j < 3 && i < Len)
									{
										Char = From[i++];

										if(Char >= '0' && Char <= '9')
											DummyBuffer[j++] = Char;
										else
										{
											i--;

											break;
										}
									}
								}
								else
								{
									while(j < 4 && i < Len)
									{
										Char = ToLower(From[i++]);

										if((Char >= '0' && Char <= '9') || (Char >= 'a' && Char <= 'z'))
											DummyBuffer[j++] = Char;
										else
										{
											i--;

											break;
										}
									}
								}

								DummyBuffer[j] = 0;

								To[Count++] = NameToCode(DummyBuffer);
							}

							i--;

							break;

							/* This is a backspace. */

						case 'B':

							To[Count++] = '\b';
							break;

							/* This is a form feed. */

						case 'F':

							To[Count++] = '\f';
							break;

							/* This is a line feed. */

						case 'N':

							To[Count++] = '\n';
							break;

							/* This is a carriage return. */

						case 'R':

							To[Count++] = '\r';
							break;

							/* This is a tab. */

						case 'T':

							To[Count++] = '\t';
							break;

							/* Produce the escape character. */

						case 'E':

							To[Count++] = ESC;
							break;

							/* Stuff the character into the buffer. */

						default:

							To[Count++] = From[i];
							break;
					}

					GotControl = FALSE;
				}
			}
		}
	}

	return(Count);
}

	/* AllocTranslationEntry(STRPTR String):
	 *
	 *	Allocate a character translation table entry.
	 */

struct TranslationEntry *
AllocTranslationEntry(CONST_STRPTR String)
{
	UBYTE LocalBuffer[256];
	LONG Count;

	Count = TranslateString(String,LocalBuffer);

	if(Count > 0)
	{
		struct TranslationEntry *Entry;
		UBYTE Type;

		if(Count > 1)
		{
			Type = TRANSLATE_STRING;

			if(Entry = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry) + Count + 1,MEMF_ANY|MEMF_CLEAR))
			{
				Entry->String = (STRPTR)(Entry + 1);

				CopyMem(LocalBuffer,Entry->String,Count);
			}
		}
		else
		{
			Type = TRANSLATE_SINGLE;

			if(Entry = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry),MEMF_ANY|MEMF_CLEAR))
			{
				Entry->String	= &Entry->Extra;
				Entry->Extra	= LocalBuffer[0];
			}
		}

		if(Entry)
		{
			Entry->Type	= Type;
			Entry->Len	= Count;

			return(Entry);
		}
	}

	return(NULL);
}

	/* FillTranslationTable(struct TranslationEntry **Table):
	 *
	 *	Fill the translation table with defaults.
	 */

BOOL
FillTranslationTable(struct TranslationEntry **Table)
{
	BOOL Success;
	LONG i;

	Success = TRUE;

	for(i = 0 ; Success && i < 256 ; i++)
	{
		if(Table[i] = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry),MEMF_ANY|MEMF_CLEAR))
		{
			Table[i]->Type		= TRANSLATE_SINGLE;
			Table[i]->String	= &Table[i]->Extra;
			Table[i]->Extra		= i;
			Table[i]->Len		= 1;
		}
		else
			Success = FALSE;
	}

	if(!Success)
	{
		for(i = 0 ; i < 256 ; i++)
		{
			FreeVecPooled(Table[i]);
			Table[i] = NULL;
		}
	}

	return(Success);
}

	/* IsStandardTable(struct TranslationEntry **Table):
	 *
	 *	Checks a translation table to see if it contains
	 *	standard data.
	 */

BOOL
IsStandardTable(struct TranslationEntry **Table)
{
	LONG i;

	for(i = 0 ; i < 256 ; i++)
	{
		if(Table[i])
		{
			if(Table[i]->Type != TRANSLATE_SINGLE || Table[i]->Len != 1 || Table[i]->String[0] != i)
				return(FALSE);
		}
		else
			return(FALSE);
	}

	return(TRUE);
}

	/* TranslateBack(STRPTR From,LONG Len,STRPTR To):
	 *
	 *	Translate a precompiled translation string
	 *	back.
	 */

VOID
TranslateBack(STRPTR From,LONG Len,STRPTR To,LONG ToLen)
{
	STATIC BYTE TypeTable[256] =
	{
		 1 , 1 , 1 , 1 , 1 , 1 , 1 ,1 ,'b','t','n', 1 ,'f','r', 1 , 1,
		 1 , 1 , 1 , 1 , 1 , 1 , 1 ,1 , 1 , 1 , 1 ,'e', 1 , 1 , 1 , 1,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 ,'\\',2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0,
		 0 , 0 , 0 , 0 , 0 , 0 , 0 ,0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
		 0 , 0 , 0 , 0 , 0 , 0 , 0 ,0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0,
		 0 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 0 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2,
		 2 , 2 , 2 , 2 , 2 , 2 , 2 ,2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2
	};

	STRPTR Buffer;
	LONG i,j,BufLen;

	for(i = j = 0 ; i < Len ; i++)
	{
		switch(TypeTable[From[i]])
		{
			case 0:

				Buffer = CodeToName(From[i]);

				BufLen = strlen(Buffer);

				if(j + 2 + BufLen >= ToLen)
					break;

				To[j++] = '\\';
				To[j++] = '*';

				CopyMem(Buffer,&To[j],BufLen);

				j += BufLen;

				break;

			case 1:

				if(j + 2 >= ToLen)
					break;

				To[j++] = '^';
				To[j++] = From[i] + '@';
				break;

			case 2:

				if(j + 1 >= ToLen)
					break;

				To[j++] = From[i];
				break;

			default:

				if(j + 2 >= ToLen)
					break;

				To[j++] = '\\';
				To[j++] = TypeTable[From[i]];
				break;
		}
	}

	To[j] = 0;
}

	/* SaveTranslationTables():
	 *
	 *	Save a character translation table to a file.
	 */

BOOL
SaveTranslationTables(STRPTR Name,struct TranslationEntry **SendTable,struct TranslationEntry **ReceiveTable)
{
	LONG Error,i;
	BOOL DoIt;

	DoIt = FALSE;

	for(i = 0 ; !DoIt && i < 256 ; i++)
	{
		if(SendTable[i] || ReceiveTable[i])
			DoIt = TRUE;
	}

	Error = 0;

	if(DoIt)
	{
		struct IFFHandle *Handle;
		UBYTE MasterTable[256];

		for(i = 0 ; i < 256 ; i++)
		{
			if(SendTable[i])
			{
				if(SendTable[i]->Type == TRANSLATE_SINGLE && SendTable[i]->Len == 1 && SendTable[i]->String[0] == i)
					MasterTable[i] = TRANSLATION_VERBATIM;
				else
					MasterTable[i] = TRANSLATION_OTHER;
			}
			else
				MasterTable[i] = TRANSLATION_IGNORE;

			if(ReceiveTable[i])
			{
				if(ReceiveTable[i]->Type == TRANSLATE_SINGLE && ReceiveTable[i]->Len == 1 && ReceiveTable[i]->String[0] == i)
					MasterTable[i] |= TRANSLATION_VERBATIM << 4;
				else
					MasterTable[i] |= TRANSLATION_OTHER << 4;
			}
			else
				MasterTable[i] |= TRANSLATION_IGNORE << 4;
		}

		if(!(Handle = OpenIFFStream(Name,MODE_NEWFILE)))
			Error = IoErr();
		else
		{
			if(!(Error = PushChunk(Handle,ID_TERM,ID_CAT,IFFSIZE_UNKNOWN)))
			{
				if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
				{
					if(!(Error = PushChunk(Handle,0,ID_VERS,IFFSIZE_UNKNOWN)))
					{
						struct TermInfo TermInfo;

						TermInfo.Version	= CONFIG_FILE_VERSION;
						TermInfo.Revision	= CONFIG_FILE_REVISION;

						if(WriteChunkRecords(Handle,&TermInfo,sizeof(struct TermInfo),1) != 1)
							Error = IoErr();

						if(!Error)
							Error = PopChunk(Handle);
					}

					if(!Error)
					{
						if(!(Error = PushChunk(Handle,0,ID_TRNS,256)))
						{
							if(WriteChunkBytes(Handle,MasterTable,256) != 256)
								Error = IoErr();

							if(!Error)
								Error = PopChunk(Handle);
						}
					}

					if(!Error)
						Error = PopChunk(Handle);
				}

				if(!Error)
				{
					struct TranslationHeader Header;

					for(i = 0 ; !Error && i < 256 ; i++)
					{
						Header.Code	= i;
						Header.Pad	= 0;

						if(SendTable[i] && (MasterTable[i] & 0xF) == TRANSLATION_OTHER)
						{
							Header.Type	= SendTable[i]->Type;
							Header.Len	= SendTable[i]->Len;

							if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
							{
								if(!(Error = PushChunk(Handle,0,ID_SEND,sizeof(struct TranslationHeader))))
								{
									if(WriteChunkRecords(Handle,&Header,sizeof(struct TranslationHeader),1) != 1)
										Error = IoErr();

									if(!Error)
										Error = PopChunk(Handle);
								}

								if(!Error)
								{
									if(!(Error = PushChunk(Handle,0,ID_CHRS,SendTable[i]->Len)))
									{
										if(WriteChunkRecords(Handle,SendTable[i]->String,SendTable[i]->Len,1) != 1)
											Error = IoErr();

										if(!Error)
											Error = PopChunk(Handle);
									}
								}

								if(!Error)
									Error = PopChunk(Handle);
							}
						}

						if(ReceiveTable[i] && (MasterTable[i] >> 4) == TRANSLATION_OTHER)
						{
							Header.Type	= ReceiveTable[i]->Type;
							Header.Len	= ReceiveTable[i]->Len;

							if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
							{
								if(!(Error = PushChunk(Handle,0,ID_RECV,sizeof(struct TranslationHeader))))
								{
									if(WriteChunkRecords(Handle,&Header,sizeof(struct TranslationHeader),1) != 1)
										Error = IoErr();

									if(!Error)
										Error = PopChunk(Handle);
								}

								if(!Error)
								{
									if(!(Error = PushChunk(Handle,0,ID_CHRS,ReceiveTable[i]->Len)))
									{
										if(WriteChunkRecords(Handle,ReceiveTable[i]->String,ReceiveTable[i]->Len,1) != 1)
											Error = IoErr();

										if(!Error)
											Error = PopChunk(Handle);
									}
								}

								if(!Error)
									Error = PopChunk(Handle);
							}
						}
					}
				}

				if(!Error)
					Error = PopChunk(Handle);
			}

			CloseIFFStream(Handle);
		}
	}

	if(Error)
	{
		SetIoErr(Error);
		return(FALSE);
	}
	else
		return(TRUE);
}

	/* LoadTranslationTables():
	 *
	 *	Load a translation table from a file.
	 */

BOOL
LoadTranslationTables(STRPTR Name,struct TranslationEntry **SendTable,struct TranslationEntry **ReceiveTable)
{
	STATIC ULONG Stops[5 * 2] =
	{
		ID_TERM,	ID_VERS,
		ID_TERM,	ID_TRNS,
		ID_TERM,	ID_SEND,
		ID_TERM,	ID_RECV,
		ID_TERM,	ID_CHRS
	};

	struct IFFHandle *Handle;
	UBYTE MasterTable[256];
	BOOL GotMasterTable;
	BOOL Success;
	LONG Error;
	LONG i;

	GotMasterTable = FALSE;
	Success = FALSE;

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
		if(!(Error = StopChunks(Handle,(LONG *)Stops,5)))
		{
			struct TranslationHeader Header;
			struct ContextNode *Chunk;
			struct TermInfo TermInfo;
			LONG LastType;

			LastType = TYPE_NONE;
			Success = TRUE;

			while(Success && !ParseIFF(Handle,IFFPARSE_SCAN))
			{
				Chunk = CurrentChunk(Handle);

				switch(Chunk->cn_ID)
				{
					case ID_VERS:

						if(ReadChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) == sizeof(struct TermInfo))
						{
							if(TermInfo.Version > CONFIG_FILE_VERSION || (TermInfo.Version == CONFIG_FILE_VERSION && TermInfo.Revision > CONFIG_FILE_REVISION))
							{
								Error = ERR_OUTDATED;

								Success = FALSE;
							}
						}
						else
						{
							Error = IoErr();

							Success = FALSE;
						}

						break;

					case ID_TRNS:

						if(ReadChunkBytes(Handle,MasterTable,256) == 256)
							GotMasterTable = TRUE;
						else
						{
							Error = IoErr();

							Success = FALSE;
						}

						break;

					case ID_SEND:

						if(LastType != TYPE_NONE)
							Success = FALSE;
						else
						{
							if(!ReadChunkRecords(Handle,&Header,MIN(sizeof(struct TranslationHeader),Chunk->cn_Size),1))
							{
								Error = IoErr();

								Success = FALSE;
							}
							else
								LastType = TYPE_SEND;
						}

						break;

					case ID_RECV:

						if(LastType != TYPE_NONE)
						{
							Error = IFFERR_MANGLED;

							Success = FALSE;
						}
						else
						{
							if(!ReadChunkRecords(Handle,&Header,MIN(sizeof(struct TranslationHeader),Chunk->cn_Size),1))
							{
								Success = FALSE;

								Error = IoErr();
							}
							else
								LastType = TYPE_RECEIVE;
						}

						break;

					case ID_CHRS:

						if(LastType == TYPE_NONE)
						{
							Error = IFFERR_MANGLED;

							Success = FALSE;
						}
						else
						{
							UBYTE LocalBuffer[256];

							if(ReadChunkRecords(Handle,LocalBuffer,Header.Len,1))
							{
								struct TranslationEntry *Entry;

								if(Header.Type == TRANSLATE_SINGLE)
								{
									if(Entry = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry),MEMF_ANY|MEMF_CLEAR))
									{
										Entry->String	= &Entry->Extra;
										Entry->Extra	= LocalBuffer[0];
									}
								}
								else
								{
									if(Entry = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry) + Header.Len + 1,MEMF_ANY|MEMF_CLEAR))
									{
										Entry->String = (STRPTR)(Entry + 1);

										CopyMem(LocalBuffer,Entry->String,Header.Len);
									}
								}

								if(Entry)
								{
									Entry->Type	= Header.Type;
									Entry->Len	= Header.Len;

									if(LastType == TYPE_SEND)
										SendTable[Header.Code] = Entry;
									else
										ReceiveTable[Header.Code] = Entry;

									LastType = TYPE_NONE;
								}
								else
								{
									Error = ERROR_NO_FREE_STORE;

									Success = FALSE;
								}
							}
							else
							{
								Error = IoErr();

								Success = FALSE;
							}
						}

						break;
				}
			}
		}

		CloseIFFStream(Handle);
	}
	else
		Error = IoErr();

	if(Success && GotMasterTable)
	{
		struct TranslationEntry *Entry;

		for(i = 0 ; i < 256 ; i++)
		{
			if((MasterTable[i] & 0xF) == TRANSLATION_VERBATIM)
			{
				if(Entry = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry),MEMF_ANY|MEMF_CLEAR))
				{
					Entry->Type		= TRANSLATE_SINGLE;
					Entry->String	= &Entry->Extra;
					Entry->Extra	= i;
					Entry->Len		= 1;

					SendTable[i] = Entry;
				}
				else
				{
					Error = ERROR_NO_FREE_STORE;

					Success = FALSE;

					break;
				}
			}

			if((MasterTable[i] >> 4) == TRANSLATION_VERBATIM)
			{
				if(Entry = (struct TranslationEntry *)AllocVecPooled(sizeof(struct TranslationEntry),MEMF_ANY|MEMF_CLEAR))
				{
					Entry->Type		= TRANSLATE_SINGLE;
					Entry->String	= &Entry->Extra;
					Entry->Extra	= i;
					Entry->Len		= 1;

					ReceiveTable[i] = Entry;
				}
				else
				{
					Error = ERROR_NO_FREE_STORE;

					Success = FALSE;

					break;
				}
			}
		}
	}

	if(!Success)
	{
		for(i = 0 ; i < 256 ; i++)
		{
			if(SendTable[i])
			{
				FreeTranslationEntry(SendTable[i]);

				SendTable[i] = NULL;
			}

			if(ReceiveTable[i])
			{
				FreeTranslationEntry(ReceiveTable[i]);

				ReceiveTable[i] = NULL;
			}
		}
	}

	if(Error)
		SetIoErr(Error);

	return(Success);
}

	/* LoadDefaultTranslationTables(STRPTR FileName):
	 *
	 *	Load the default character translation file.
	 */

BOOL
LoadDefaultTranslationTables(STRPTR FileName)
{
	BOOL Success;

	Success = FALSE;

	if(SendTable = AllocTranslationTable())
	{
		if(ReceiveTable = AllocTranslationTable())
		{
			BOOL ReleaseIt;

			if(LoadTranslationTables(FileName,SendTable,ReceiveTable))
			{
				ReleaseIt = (BOOL)(IsStandardTable(SendTable) && IsStandardTable(ReceiveTable));

				Success = TRUE;
			}
			else
				ReleaseIt = TRUE;

			if(ReleaseIt)
			{
				FreeTranslationTable(SendTable);
				SendTable = NULL;

				FreeTranslationTable(ReceiveTable);
				ReceiveTable = NULL;
			}
		}
		else
		{
			FreeTranslationTable(SendTable);
			SendTable = NULL;
		}
	}

	return(Success);
}
