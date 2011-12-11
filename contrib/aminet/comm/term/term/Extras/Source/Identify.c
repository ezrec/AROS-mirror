/*
**	Identify.c
**
**	Heuristic file type identification routines (big words!).
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The file types we know. */

enum	{	TYPE_DIR,TYPE_FILE,
			TYPE_ICON,
			TYPE_TEXT,
			TYPE_C,TYPE_H,
			TYPE_ASM,TYPE_I,
			TYPE_MOD,
			TYPE_REXX,
			TYPE_BASIC,
			TYPE_AMIGAGUIDE,
			TYPE_TEX,TYPE_METAFONT,TYPE_GF,TYPE_TEXFONT,TYPE_TEXDVI,TYPE_FLIB,
			TYPE_OLDMANX,TYPE_NEWMANX,TYPE_OLDMANXLIB,TYPE_NEWMANXLIB,
			TYPE_OBJECT,TYPE_LIB,
			TYPE_EXECUTABLE,
			TYPE_LIBRARY,TYPE_DEVICE,TYPE_FILESYS,TYPE_HANDLER,
			TYPE_GIF,TYPE_DEGAS,TYPE_MACPAINT,TYPE_SUPERPAINT,TYPE_PICT,TYPE_SUNRASTER,TYPE_POSTSCRIPT,TYPE_PCX,TYPE_TIFF,TYPE_BMP,TYPE_JFIF,TYPE_ILBM,
			TYPE_ANIM,TYPE_8SVX,TYPE_SMUS,TYPE_FTXT,TYPE_PREFS,TYPE_TERM,TYPE_AMIGAVISION,TYPE_IFF,
			TYPE_IMPLODER,TYPE_POWERPACKER,TYPE_LHPAK,TYPE_LHASFX,
			TYPE_ARC,TYPE_ARJ,TYPE_COMPRESSED,TYPE_CPIO,TYPE_UUENCODED,TYPE_FREEZE,TYPE_MACCOMPRESS,TYPE_COMPACT,TYPE_DIAMOND,TYPE_LHARC,TYPE_LHA,TYPE_ZOO,TYPE_ZIP,TYPE_STUFFIT,TYPE_PACKIT,TYPE_DMS,TYPE_WARP,TYPE_ZOOM,
			TYPE_SPARCOBJECT,TYPE_SPARCEXECUTABLE,
			TYPE_MSDOSEXECUTABLE,
			TYPE_ATARIEXECUTABLE,
			TYPE_MACEXECUTABLE,
			TYPE_CDAF,TYPE_XPK,
			TYPE_PNG,TYPE_LZX,

			TYPE_COUNT
		};

	/* Locale string IDs for the file types */

UWORD TypeID[TYPE_COUNT] =
{
	MSG_IDENTIFY_FILETYPE_0,
	MSG_IDENTIFY_FILETYPE_1,
	MSG_IDENTIFY_FILETYPE_2,
	MSG_IDENTIFY_FILETYPE_3,
	MSG_IDENTIFY_FILETYPE_4,
	MSG_IDENTIFY_FILETYPE_5,
	MSG_IDENTIFY_FILETYPE_6,
	MSG_IDENTIFY_FILETYPE_7,
	MSG_IDENTIFY_FILETYPE_8,
	MSG_IDENTIFY_FILETYPE_9,
	MSG_IDENTIFY_FILETYPE_10,
	MSG_IDENTIFY_FILETYPE_11,
	MSG_IDENTIFY_FILETYPE_12,
	MSG_IDENTIFY_FILETYPE_13,
	MSG_IDENTIFY_FILETYPE_14,
	MSG_IDENTIFY_FILETYPE_15,
	MSG_IDENTIFY_FILETYPE_16,
	MSG_IDENTIFY_FILETYPE_17,
	MSG_IDENTIFY_FILETYPE_18,
	MSG_IDENTIFY_FILETYPE_19,
	MSG_IDENTIFY_FILETYPE_20,
	MSG_IDENTIFY_FILETYPE_21,
	MSG_IDENTIFY_FILETYPE_22,
	MSG_IDENTIFY_FILETYPE_23,
	MSG_IDENTIFY_FILETYPE_24,
	MSG_IDENTIFY_FILETYPE_25,
	MSG_IDENTIFY_FILETYPE_26,
	MSG_IDENTIFY_FILETYPE_27,
	MSG_IDENTIFY_FILETYPE_28,
	MSG_IDENTIFY_FILETYPE_29,
	MSG_IDENTIFY_FILETYPE_30,
	MSG_IDENTIFY_FILETYPE_31,
	MSG_IDENTIFY_FILETYPE_32,
	MSG_IDENTIFY_FILETYPE_33,
	MSG_IDENTIFY_FILETYPE_34,
	MSG_IDENTIFY_FILETYPE_35,
	MSG_IDENTIFY_FILETYPE_36,
	MSG_IDENTIFY_FILETYPE_37,
	MSG_IDENTIFY_FILETYPE_38,
	MSG_IDENTIFY_FILETYPE_39,
	MSG_IDENTIFY_FILETYPE_40,
	MSG_IDENTIFY_FILETYPE_41,
	MSG_IDENTIFY_FILETYPE_42,
	MSG_IDENTIFY_FILETYPE_43,
	MSG_IDENTIFY_FILETYPE_44,
	MSG_IDENTIFY_FILETYPE_45,
	MSG_IDENTIFY_FILETYPE_46,
	MSG_IDENTIFY_FILETYPE_47,
	MSG_IDENTIFY_FILETYPE_48,
	MSG_IDENTIFY_FILETYPE_49,
	MSG_IDENTIFY_FILETYPE_50,
	MSG_IDENTIFY_FILETYPE_51,
	MSG_IDENTIFY_FILETYPE_52,
	MSG_IDENTIFY_FILETYPE_53,
	MSG_IDENTIFY_FILETYPE_54,
	MSG_IDENTIFY_FILETYPE_55,
	MSG_IDENTIFY_FILETYPE_56,
	MSG_IDENTIFY_FILETYPE_57,
	MSG_IDENTIFY_FILETYPE_58,
	MSG_IDENTIFY_FILETYPE_59,
	MSG_IDENTIFY_FILETYPE_60,
	MSG_IDENTIFY_FILETYPE_61,
	MSG_IDENTIFY_FILETYPE_62,
	MSG_IDENTIFY_FILETYPE_63,
	MSG_IDENTIFY_FILETYPE_64,
	MSG_IDENTIFY_FILETYPE_65,
	MSG_IDENTIFY_FILETYPE_66,
	MSG_IDENTIFY_FILETYPE_67,
	MSG_IDENTIFY_FILETYPE_68,
	MSG_IDENTIFY_FILETYPE_69,
	MSG_IDENTIFY_FILETYPE_70,
	MSG_IDENTIFY_FILETYPE_71,
	MSG_IDENTIFY_FILETYPE_72,
	MSG_IDENTIFY_FILETYPE_73,
	MSG_IDENTIFY_CDAF,
	MSG_IDENTIFY_XPK,
	MSG_IDENTIFY_FILETYPE_74,
	MSG_IDENTIFY_FILETYPE_75
};

	/* File type/class mappings. */

STATIC WORD FileTypes[TYPE_COUNT] =
{
	FILETYPE_DIR,
	FILETYPE_FILE,
	FILETYPE_NONE,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_TEXT,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_PROGRAM,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_PICTURE,
	FILETYPE_SOUND,
	FILETYPE_SOUND,
	FILETYPE_TEXT,
	FILETYPE_PREFS,
	FILETYPE_PREFS,
	FILETYPE_FILE,
	FILETYPE_FILE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_TEXT,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_PROGRAM,
	FILETYPE_PROGRAM,
	FILETYPE_PROGRAM,
	FILETYPE_PROGRAM,
	FILETYPE_PROGRAM,
	FILETYPE_ARCHIVE,
	FILETYPE_ARCHIVE,
	FILETYPE_PICTURE,
	FILETYPE_ARCHIVE
};

	/* File class icon names. */

STATIC STRPTR ClassIconNames[FILETYPE_COUNT] =
{
	NULL,
	"ENV:sys/def_drawer",
	"ENV:sys/def_project",
	"ENV:sys/def_text",
	"ENV:sys/def_sound",
	"ENV:sys/def_picture",
	"ENV:sys/def_pref",
	"ENV:sys/def_archive",
	"ENV:sys/def_tool",
};

	/* A structure containing both a file name suffix and the
	 * appropriate file type.
	 */

struct Suffix
{
	UBYTE	*Name;
	UBYTE	 Type;
};

	/* A table of valid ASCII characters (7 bits). */

STATIC BYTE ID_ValidTab[256] =
{
	0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,
	0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

	/* A table of clearly invalid ASCII characters (8 bits). */

STATIC BYTE ID_InvalidTab[256] =
{
	1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,
	1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

	/* Some file name suffixes for text files and the appropriate
	 * file types.
	 */

STATIC struct Suffix TextSuffix[] =
{
	{ ".C",		TYPE_C, },
	{ ".CPP",		TYPE_C, },
	{ ".CXX",		TYPE_C, },
	{ ".C++",		TYPE_C, },
	{ ".CC",		TYPE_C, },
	{ ".H",		TYPE_H, },
	{ ".ASM",		TYPE_ASM, },
	{ ".A",		TYPE_ASM, },
	{ ".S",		TYPE_ASM, },
	{ ".I",		TYPE_I, },
	{ ".BAS",		TYPE_BASIC, },
	{ ".GFA",		TYPE_BASIC, },
	{ ".REXX",	TYPE_REXX, },
	{ ".CED",		TYPE_REXX, },
	{ ".TTX",		TYPE_REXX, },
	{ ".VLT",		TYPE_REXX, },
	{ ".CPR",		TYPE_REXX, },
	{ ".TxEd",	TYPE_REXX, },
	{ ".ADPro",	TYPE_REXX, },
	{ ".TEX",		TYPE_TEX, },
	{ ".STY",		TYPE_TEX, },
	{ ".MF",		TYPE_METAFONT, },
	{ ".MOD",		TYPE_MOD, },
	{ ".DEF",		TYPE_MOD, },
	{ ".PS",		TYPE_POSTSCRIPT, },
	{ ".GUIDE",	TYPE_AMIGAGUIDE, },
	{ ".UUE",		TYPE_UUENCODED },
};

	/* Some more file name suffixes for executable files and the
	 * appropriate file types.
	 */

STATIC struct Suffix ExecutableSuffix[] =
{
	{ ".device",	TYPE_DEVICE, },
	{ ".library",	TYPE_LIBRARY, },
	{ "FileSystem",	TYPE_FILESYS, },
	{ "Handler",	TYPE_HANDLER },
};

	/* Miscellaneous magic cookies. */

STATIC STRPTR MagicCookies[] =
{
	"P1",
	"P2",
	"P3",
	"P4",
	"P5",
	"P6",
	"begin",
	"xbtoa"
};

	/* LongCompare(UBYTE *Buffer,ULONG Value):
	 *
	 *	Compare space in memory with a longword value.
	 */

STATIC BOOL
LongCompare(UBYTE *Buffer,ULONG Value)
{
	UBYTE *OtherBuffer = (UBYTE *)&Value,i;

	for(i = 0 ; i < 4 ; i++)
	{
		if(OtherBuffer[i] != Buffer[i])
			return(FALSE);
	}

	return(TRUE);
}

	/* GetFileType(STRPTR Name,BOOL AttachComment):
	 *
	 *	Simple routine to identify a file type by looking at
	 *	its first 400 bytes. If successful a comment is
	 *	attached to the file describing the file type.
	 */

LONG
GetFileType(STRPTR Name,BOOL AttachComment)
{
	UBYTE IFFType[5];
	CONST_STRPTR TypeName;
	ULONG *Buffer;
	LONG i,Len;
	LONG Type;

	Type		= TYPE_FILE;
	TypeName	= NULL;
	Len			= strlen(Name);

		/* Allocate a buffer for the first 410 bytes of the
		 * file.
		 */

	if(Buffer = (ULONG *)AllocVecPooled(410,MEMF_ANY | MEMF_CLEAR))
	{
#ifdef __AROS__
		BPTR File;
		LONG Size;
#else
		BPTR File,Size;
#endif

			/* Open the file. */

		if(File = Open(Name,MODE_OLDFILE))
		{
				/* Read the first 410 bytes. */

			if((Size = Read(File,Buffer,410)) >= sizeof(ULONG))
			{
				UBYTE *CharBuffer;
				LONG Count;

				CharBuffer = (UBYTE *)Buffer;
				Count = 0;

					/* See if it's an ASCII file. */

				for(i = 0 ; i < Size ; i++)
				{
					if(ID_ValidTab[CharBuffer[i]])
						Count++;
					else
					{
						if(ID_InvalidTab[CharBuffer[i]])
						{
							Count = 0;

							break;
						}
					}
				}

					/* If more than 75% of the
					 * characters in the first
					 * 400 bytes are legal
					 * ASCII characters this
					 * file is supposed to be
					 * a text file.
					 */

				if(Count > 3 * (Size / 4))
					Type = TYPE_TEXT;

					/* Examine the first longword. */

				if(Type == TYPE_FILE)
				{
					switch(Buffer[0])
					{
						case 0x4C5A5800:

							Type = TYPE_LZX;
							break;

						case 0x58504B46:

							Type = TYPE_XPK;
							break;

						case 0x000003E7:

							Type = TYPE_OBJECT;
							break;

						case 0x000003F3:

							if(Buffer[10] == AROS_MAKE_ID('L','S','F','X'))
								Type = TYPE_LHPAK;
							else
							{
								if(Buffer[11] == AROS_MAKE_ID('S','F','X','!'))
									Type = TYPE_LHASFX;
								else
									Type = TYPE_EXECUTABLE;
							}

							break;

						case 0x000003FA:

							Type = TYPE_LIB;
							break;

						case 0xF7593647:

							Type = TYPE_TEXFONT;
							break;

						case 0xF7020183:

							Type = TYPE_TEXDVI;
							break;

						case 0xF7832020:

							Type = TYPE_GF;
							break;

						case 0x504B0304:

							Type = TYPE_ZIP;
							break;

						case 0x01030107:

							Type = TYPE_SPARCOBJECT;
							break;

						case 0x8103010B:

							Type = TYPE_SPARCEXECUTABLE;
							break;

						case 0x59A66A95:

							Type = TYPE_SUNRASTER;
							break;

						case 0x1F9D902A:

							Type = TYPE_COMPRESSED;
							break;

						case 0x30373037:

							Type = TYPE_CPIO;
							break;

						case AROS_MAKE_ID('F','L','I','B'):

							Type = TYPE_FLIB;
							break;

						case AROS_MAKE_ID('\211','P','N','G'):

							Type = TYPE_PNG;
							break;

						case AROS_MAKE_ID('F','O','R','M'):

							switch(Buffer[2])
							{
								case AROS_MAKE_ID('I','L','B','M'):

									Type = TYPE_ILBM;
									break;

								case AROS_MAKE_ID('A','N','I','M'):

									Type = TYPE_ANIM;
									break;

								case AROS_MAKE_ID('8','S','V','X'):

									Type = TYPE_8SVX;
									break;

								case AROS_MAKE_ID('S','M','U','S'):

									Type = TYPE_SMUS;
									break;

								case AROS_MAKE_ID('F','T','X','T'):

									Type = TYPE_FTXT;
									break;

								case AROS_MAKE_ID('P','R','E','F'):

									Type = TYPE_PREFS;
									break;

								case AROS_MAKE_ID('T','E','R','M'):

									Type = TYPE_TERM;
									break;

								case AROS_MAKE_ID('A','V','C','F'):

									Type = TYPE_AMIGAVISION;
									break;

								case AROS_MAKE_ID('C','D','A','F'):

									Type = TYPE_CDAF;
									break;

								default:

									Type = TYPE_IFF;

									CopyMem(&Buffer[2],IFFType,4);

									IFFType[4] = 0;

									break;
							}

							break;

						case AROS_MAKE_ID('I','M','P','!'):

							Type = TYPE_IMPLODER;
							break;

						case AROS_MAKE_ID('P','P','2','0'):

							Type = TYPE_POWERPACKER;
							break;

						case AROS_MAKE_ID('D','M','S','!'):

							Type = TYPE_DMS;
							break;

						case AROS_MAKE_ID('W','a','r','p'):

							Type = TYPE_WARP;
							break;

						case AROS_MAKE_ID('Z','O','M','5'):
						case AROS_MAKE_ID('Z','O','O','M'):

							Type = TYPE_ZOOM;
							break;

						case AROS_MAKE_ID('Z','O','O',' '):

							Type = TYPE_ZOO;
							break;

						case AROS_MAKE_ID('G','I','F','8'):

							Type = TYPE_GIF;
							break;
					}
				}

					/* Now for oddly placed magic cookies... */

				if(Type == TYPE_FILE)
				{
						/* Dumb check for PCX (awful header -- just
						 * a single byte indicates that the file
						 * is supposed to be a PCX file! Those
						 * PC guys just don't have any
						 * decent software culture!).
						 */

					if(CharBuffer[0] == 0x0A)
						Type = TYPE_PCX;

						/* Yet another awful file format... */

					if(CharBuffer[0] == 'B' && CharBuffer[1] == 'M')
						Type = TYPE_BMP;

						/* Check for JFIF... */

					if(LongCompare(&CharBuffer[6],AROS_MAKE_ID('J','F','I','F')))
						Type = TYPE_JFIF;

						/* Check for `freeze' output file. */

					if(CharBuffer[0] == (UBYTE)'\037' && CharBuffer[1] >= (UBYTE)'\236')
						Type = TYPE_FREEZE;
				}

					/* Check for Macintosh data (note:
					 * this code assumes that the Mac
					 * binary header is still present,
					 * so if you are dealing with stripped
					 * binary files, don't expect the
					 * following code to work!).
					 */

				if(Type == TYPE_FILE)
				{
					UBYTE *ByteBuffer = &((UBYTE *)Buffer)[65];

						/* Check for MacPaint... */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('P','N','T','G')))
						Type = TYPE_MACPAINT;

						/* Check for SuperPaint... */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('S','P','T','G')))
						Type = TYPE_SUPERPAINT;

						/* Check for PICT... */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('P','I','C','T')))
						Type = TYPE_PICT;

						/* Check for executable... */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('A','P','P','L')) || LongCompare(ByteBuffer,AROS_MAKE_ID('P','R','E','S')) || LongCompare(ByteBuffer,AROS_MAKE_ID('F','D','O','C')) || LongCompare(ByteBuffer,AROS_MAKE_ID('c','d','e','v')) || LongCompare(ByteBuffer,AROS_MAKE_ID('I','N','I','T')))
						Type = TYPE_MACEXECUTABLE;

						/* Check for StuffIt archive... */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('S','I','T','!')) || LongCompare(ByteBuffer,AROS_MAKE_ID('S','I','T','2')))
						Type = TYPE_STUFFIT;

						/* Check for PackIt archive... */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('P','I','T',' ')))
						Type = TYPE_PACKIT;

						/* Check for self-extracting Compact archive. */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('A','P','P','L')) && LongCompare(&ByteBuffer[4],AROS_MAKE_ID('E','X','T','R')))
						Type = TYPE_COMPACT;

						/* Check for MacCompress archive. */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('Z','I','V','M')))
						Type = TYPE_MACCOMPRESS;

						/* Check for LhArc archive with MacBinary header. */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('L','A','R','C')))
						Type = TYPE_LHARC;

						/* Check for Compact archive. */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('P','A','C','T')))
						Type = TYPE_COMPACT;

						/* Check for Diamond archive. */

					if(LongCompare(ByteBuffer,AROS_MAKE_ID('P','a','c','k')))
						Type = TYPE_DIAMOND;
				}

					/* Still no match? Have another try... */

				if(Type == TYPE_FILE)
				{
					if((Buffer[0] & 0x0000FFFF) == 0x00002D6C && (Buffer[1] & 0xFF00FF00) == 0x68002D00)
					{
						if(CharBuffer[5] == '5')
							Type = TYPE_LHA;
						else
							Type = TYPE_LHARC;
					}
					else
					{
						switch(Buffer[0] & 0xFFFF0000)
						{
							case 0x4D4D0000:
							case 0x49490000:

								Type = TYPE_TIFF;
								break;

							case 0x1A080000:

								Type = TYPE_ARC;
								break;

							case 0x60EA0000:

								Type = TYPE_ARJ;
								break;

							case 0x434A0000:

								Type = TYPE_NEWMANX;
								break;

							case 0x414A0000:

								Type = TYPE_OLDMANX;
								break;

							case 0x636A0000:

								Type = TYPE_NEWMANXLIB;
								break;

							case 0x616A0000:

								Type = TYPE_OLDMANXLIB;
								break;

							case 0xF5000000:

								Type = TYPE_BASIC;
								break;

							case 0xE3100000:

								Type = TYPE_ICON;
								break;

							case 0x4D5A0000:

								Type = TYPE_MSDOSEXECUTABLE;
								break;

							case 0x601A0000:

								Type = TYPE_ATARIEXECUTABLE;
								break;

							case 0x80000000:

								Type = TYPE_DEGAS;
								break;
						}
					}
				}

					/* Take a look at the file name
					 * suffixes.
					 */

				switch(Type)
				{
					case TYPE_TEXT:

						for(i = 0 ; i < sizeof(TextSuffix) / sizeof(struct Suffix) ; i++)
						{
							Size = strlen(TextSuffix[i].Name);

							if(Len >= Size)
							{
								if(!Stricmp(&Name[Len - Size],TextSuffix[i].Name))
								{
									Type = TextSuffix[i].Type;
									break;
								}
							}
						}

							/* Is it still a text file? */

						if(Type == TYPE_TEXT)
						{
								/* Check for magic cookies. */

							for(i = 0 ; i < sizeof(MagicCookies) / sizeof(STRPTR) ; i++)
							{
								if(!memcmp(CharBuffer,MagicCookies[i],strlen(MagicCookies[i])))
								{
									TypeName = LocaleString(MSG_IDENTIFY_COOKIE_0 + i);

									break;
								}
							}
						}

						break;

					case TYPE_EXECUTABLE:

						for(i = 0 ; i < sizeof(ExecutableSuffix) / sizeof(struct Suffix) ; i++)
						{
							Size = strlen(ExecutableSuffix[i].Name);

							if(Len >= Size)
							{
								if(!Stricmp(&Name[Len - Size],ExecutableSuffix[i].Name))
								{
									Type = ExecutableSuffix[i].Type;
									break;
								}
							}
						}

						break;

					case TYPE_OBJECT:

						if(Len >= 4)
						{
							if(!Stricmp(&Name[Len - 4],".LIB"))
								Type = TYPE_LIB;
						}

						break;
				}
			}

			Close(File);
		}

		FreeVecPooled(Buffer);
	}

	if(AttachComment)
	{
		if(Type > TYPE_FILE || TypeName)
		{
			if(Type == TYPE_IFF)
			{
				UBYTE NameBuffer[MAX_FILENAME_LENGTH];

				LimitedSPrintf(sizeof(NameBuffer),NameBuffer,LocaleString(MSG_IDENTIFY_IFF_FILE),IFFType);

				SetComment(Name,NameBuffer);
			}
			else
			{
				if(TypeName)
					SetComment(Name,TypeName);
				else
					SetComment(Name,LocaleString(TypeID[Type - TYPE_ICON]));
			}
		}

			/* Is it an executable file? */

		if(Type != TYPE_EXECUTABLE && Type != TYPE_LHPAK && Type != TYPE_LHASFX)
			AddProtection(Name,FIBF_EXECUTE);
	}

	return(Type);
}

	/* Identify(STRPTR Name):
	 *
	 *	Try to identify the type of a file,
	 *	attach an icon if necessary.
	 */

VOID
Identify(STRPTR Name,BOOL AttachComment)
{
	LONG Type = GetFileType(Name,AttachComment);

	if(Config->MiscConfig->CreateIcons)
		AddIcon(Name,FileTypes[Type],FALSE);
}

	/* AddIcon(STRPTR Name,LONG Type,BOOL Override):
	 *
	 *	Add an icon to a file.
	 */

VOID
AddIcon(STRPTR Name,LONG Index,BOOL Override)
{
	if(ClassIconNames[Index] && IconBase)
	{
		struct DiskObject *Icon;

		if(!Override)
		{
			if(Icon = GetDiskObject(Name))
			{
				FreeDiskObject(Icon);

				return;
			}
		}

		if(!(Icon = GetDiskObject(ClassIconNames[Index])))
		{
			if(Index == FILETYPE_PROGRAM)
				Icon = GetDefDiskObject(WBTOOL);
			else
			{
				AddProtection(Name,FIBF_EXECUTE);

				Icon = GetDiskObjectNew(Name);
			}
		}

		if(Icon)
		{
			Icon->do_CurrentX = Icon->do_CurrentY = NO_ICON_POSITION;

			PutDiskObject(Name,Icon);

			FreeDiskObject(Icon);
		}
	}
}
