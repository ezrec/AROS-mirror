/****************************************************************************
*
*	TestDMS - A portable utility for viewing and testing DMS archives
*
*	Author: Bjorn Stenberg (bjst@sth.frontec.se)
*
*
*	This software and this source code is in the Public Domain. You may do
*	with it exactly what you want. I wrote it to satisfy my own needs and 
*	if other people can make use of it - fine.
*
*	Thanks to:
*	David Connors (dconnors@gucis.cit.gu.edu.au)
*		for providing a source with basic information on DMS archives.
*	Geoff C. Wing (gwing@mullauna.cs.mu.oz.au)
*		for the little-endian compatibility functions.
*
*	If this source looks strange, you've probably set tab-size to 8.
*	Real men use 4... ;-)
*
*	[ Nice try, Bjorn, but ANIS .. oops .. ANSI standard is 8 - Dave]
*
****************************************************************************
*
*	When porting this, make sure you pay attention to the LITTLEINDIAN
*	define! Most boxes won't need this, but Intels (and some others) will.
*
*	The compression time printout is cancelled as default, since SPARCs
*	(and maybe others) have problems with word (16-bit) aligned longs.
*	You can try how your machine reacts by uncommenting the WORD_ALIGN
*	define below. It works on the Intel 80x86s and Motorola 680x0s I've
*	tested, but not the SPARCs.
*
****************************************************************************/
/*
 * CPU byte order. if not LITTLEINDIAN, big-endian is assumed
 */

#define LITTLEINDIAN

/* 
 * Does your machine support 16-bit aligned 32-bit variables ?
 * If not, leave this define commented out
 */

/* #define WORD_ALIGN */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************************************************************************
*
*  Defines & Macros
*
*************************************************************************/

/* readability boosters: */
#define OK				0
#define ERROR			1
#define FALSE			0
#define TRUE			1

#define OR				||
#define AND 			&&
#define NOT 			!
#define EQUAL			0
#define PRIVATE 		static

/* errors */
#define	ERR_SHORT_READ		1
#define ERR_IDENT			2
#define ERR_CRC				3
#define ERR_SIZE			4
#define	ERR_OUT_OF_MEM		255

/* application limits */
#define TRACK_BUF_SIZE		60000
#define	MAX_FILES			200

#ifdef LITTLEINDIAN
#define swap_short(a) (((a & 0x00ff) << 8) \
                     + ((a & 0xff00) >> 8))
#define swap_long(a)  (((a & 0x000000ff) << 24) \
                     + ((a & 0x0000ff00) <<  8) \
                     + ((a & 0x00ff0000) >>  8) \
                     + ((a & 0xff000000) >> 24))
#else	/* not LITTLEINDIAN */
#define swap_short(a)  (a)
#define swap_long(a)   (a)
#endif	/* not LITTLEINDIAN */

/* archive header definitions */
#define	DMS_IDENT			"DMS!"		/* DMS archive identifier 	*/
#define	DMS_IDENT_SIZE 		4			/* length of identifier		*/
#define	DMS_HEADER_SIZE		50			/* length of archive header */
#define DMS_TOTAL_SIZE		56			/* ident + header + crc 	*/

/* track header definitions */
#define	TRK_TOTAL_SIZE		20			/* header + crc				*/

/*
 * Information extraction macros
 * These exist to remove the portability problems of struct alignments.
 */
#define DHEAD_FROM(tab)    (SHORT)swap_short(*((SHORT *) (tab+12)))
#define DHEAD_TO(tab)      (SHORT)swap_short(*((SHORT *) (tab+14)))
#define DHEAD_CSIZE(tab)   (ULONG)swap_long(*((ULONG *) (tab+16)))
#define DHEAD_SIZE(tab)    (ULONG)swap_long(*((ULONG *) (tab+20)))
#define DHEAD_CMODE(tab)   (SHORT)swap_short(*((SHORT *) (tab+48)))
#define DHEAD_MACHINE(tab) (SHORT)swap_short(*((SHORT *) (tab+32)))
#define DHEAD_CPU(tab) 	   (SHORT)swap_short(*((SHORT *) (tab+28)))
#define DHEAD_FPU(tab) 	   (SHORT)swap_short(*((SHORT *) (tab+30)))
#define DHEAD_INFO(tab)	   (LONG)swap_long(*((LONG*) (tab+4)))
#define DHEAD_DATE(tab)    (LONG)swap_long(*((LONG*) (tab+8)))
#define DHEAD_TIME(tab)    (LONG)swap_long(*((LONG*) (tab+38)))
#define DHEAD_CVER(tab)	   (SHORT)swap_short(*((SHORT*) (tab+42)))
#define DHEAD_NVER(tab)	   (SHORT)swap_short(*((SHORT*) (tab+44)))
#define DHEAD_TYPE(tab)	   (SHORT)swap_short(*((SHORT*) (tab+46)))

#define THEAD_IDENT(tab) (USHORT)swap_short(*((USHORT *) (tab+0 )))
#define THEAD_TNUM(tab)  (SHORT)swap_short(*((SHORT *)   (tab+2 )))
#define THEAD_SIZE(tab)  (ULONG)swap_long(*((ULONG *)    (tab+4 )))
#define THEAD_PLEN(tab)  (USHORT)swap_short(*((USHORT *) (tab+8 )))
#define THEAD_ULEN(tab)  (USHORT)swap_short(*((USHORT *) (tab+10)))
#define THEAD_USUM(tab)  (USHORT)swap_short(*((USHORT *) (tab+14)))
#define THEAD_DCRC(tab)  (USHORT)swap_short(*((USHORT *) (tab+16)))
#define THEAD_CMODE(tab)  (SHORT)swap_short(*((SHORT *) (tab+12)))

/*************************************************************************
*
*  Type Definitions
*
*************************************************************************/

typedef char           BYTE;
typedef unsigned char  UBYTE;
typedef short          SHORT;
typedef unsigned short USHORT;
typedef long           LONG;
typedef unsigned long  ULONG;

typedef struct {			/* offset - sizeof(File_Head) = 50				*/
	long u1;                /* 0 - ???			         					*/
	long info;				/* 4 - General info ( 0x01: NoZero (no bitmap)	*/
							/* 					  0x08: Banner )			*/
	long date;              /* 8 - Creation date of file 					*/
	short from;             /* 12 - index of first track 					*/
	short to;               /* 14 - index of last track  					*/
	ULONG  size_after;      /* 16 - total size of data after compression	*/
	ULONG  size_before;     /* 20 - total size of data before compression	*/
	long u3;                /* 24 -  Always 0								*/
	short cpu;              /* 28 -  Cpu type ( 1: 68000,					*/
							/*					2: 68010,					*/
							/*					3: 68020,					*/
							/*					4: 68030,					*/
							/*					5: 68040 )					*/
	short copross;          /* 30 -  Cpu coprocessor ( 1: 68881,			*/
							/*						   2: 68882 )			*/
	short machine;          /* 32 -  Machine used    ( 1: Amiga, 			*/
							/*						   2: PC )				*/
	short u4;               /* 34 -  ???                 					*/
	short cpu_speed;        /* 36 -  CPU Speed. Mostly left blank.			*/
	long time;              /* 38 -  Time to create archive in sec.			*/
	short c_version;        /* 42 -  Version of creator (0x64 = v1.00		*/
							/*							 0x65 = v1.01		*/
							/*							 0x66 = v1.02		*/
							/*							 0x67 = v1.03		*/
							/*							 0x6F = v1.11	)	*/
	short n_version;        /* 44 -  Version needed to extract				*/
	short disk_type;        /* 46 -  Disk type 	(1 = Amiga OFS v1.0)		*/
	short cmode;            /* 48 -  Compression mode (0 = NONE,			*/
							/*						   1 = SIMPLE,			*/
							/*						   2 = QUICK(?)			*/
							/*						   3 = MEDIUM,			*/
							/*						   4 = DEEP,			*/
							/*						   5 = HEAVY1,			*/
							/*						   6 = HEAVY2 )			*/
} t_Disk;

typedef struct {            /* offset -  sizeof(Track_Head) = 18	*/
	short delim;            /* 0 - delimiter, 0x5452, is 'TR'		*/
	short number;           /* 2 - track number, -1 if text			*/
	ULONG size;             /* 4 - size of data part     			*/
	USHORT plength;         /* 8 - length of non-encoded data		*/
	USHORT ulength;         /* 10 - length of encoded data			*/
	short mode;             /* 12 - encryption mode (  0: SIMPLE,	*/
							/*						 102: QUICK )	*/
	USHORT usum;            /* 14 - raw data checksum    			*/
	USHORT dcrc;            /* 16 - data CRC             			*/
} t_Track;

typedef struct {
	UBYTE	Silent_B;	/* produce no output whatsoever	*/
	UBYTE	Quick_B;	/* check archive header only 	*/
	UBYTE	Verbose_B;	/* verbose output 				*/
} t_Flags;

#define FLAGS		"sqv"		/* parameter flags in order of appearance	*/
                                /* in the t_Flags struct 					*/

/*************************************************************************
*
*  Functions
*
*************************************************************************/

void 	main		( int 		argc, 
					  char** 	argv );

int		ParseParams( int		argc,
					 char*		argv[],
					 t_Flags*	Options_PT );

void	ParseCommand( int       argc,       
					  char**    argv,       
					  int*      TotFiles_PI,
					  t_Flags*  Options_PT, 
					  char**    FileList_PPC );

void	PrintHelp	( void );

int		CheckFile	( char*		Filename_PC,
					  t_Flags*	Options_PT );

int		CheckHeader	( FILE*		File_PT,
					  int*		FirstTrack_PI, 
					  int*		TotTracks_PI,
					  UBYTE		Verbose_B );

int		CheckTracks	( FILE*		File_PT,
					  int		FirstTrack_I,
					  int		TotTracks_I,
					  UBYTE		Verbose_I);

USHORT	CreateCRC	( UBYTE* 	Mem, 
					  int 		Size );

void	PrintFileHeader	( UBYTE*	RawHeader_PT );

void	PrintTrackHeader( UBYTE*	RawHeader_PT );

/*******************************************************************
*
* Function   : main()
* Description: Main loop of the program
* Parameters : none
*
********************************************************************/
void main( int argc, char** argv )
{
	t_Flags		Options_T;
	int			File_I			= 0;
	int			Result_I		= 0;

	if ( argc < 2 )
		PrintHelp();

	Options_T.Verbose_B = FALSE;
	Options_T.Silent_B = FALSE;
	Options_T.Quick_B = FALSE;

	for ( File_I = ParseParams( argc, argv, &Options_T );
		  File_I < argc; File_I++ )
		Result_I = CheckFile( argv[ File_I ], &Options_T );

	exit( Result_I );
}



/*******************************************************************
*
* Function   : ParseParams()
* Description: Parse command-line parameters and set options
*
********************************************************************/
int		ParseParams( int		argc,
					 char*		argv[],
					 t_Flags*	Options_PT )
{
	char*			Flags_PC 	= FLAGS;
	char			FoundFlag_B	= FALSE;
	int				NumFlags_I	= strlen( Flags_PC );
	register int	Arg_I	 	= 0;
	register int 	Char_I 	 	= 0;
	register int	Flag_I	 	= 0;
	register UBYTE*	Options_PC	= (UBYTE*) Options_PT;

	for ( Arg_I = 1; (Arg_I < argc) AND (argv[ Arg_I ][0] == '-'); Arg_I++ ) {

		for ( Char_I=1; Char_I< strlen( argv[ Arg_I ] ); Char_I++ ) {

			FoundFlag_B = FALSE;

			for( Flag_I=0; Flag_I < NumFlags_I; Flag_I++)
				if ( ( argv[ Arg_I ][ Char_I] ) == Flags_PC[ Flag_I ] )
					Options_PC[ Flag_I ] = FoundFlag_B = TRUE;

			if ( NOT FoundFlag_B )	/* if no flag was found,		*/
				return Arg_I;		/* this parameter is a filename */
		}
	}

	return Arg_I;		/* return position of first filename */
}

/*******************************************************************
*
* Function   : PrintHelp()
* Description: Display program/usage info
* Parameters : none
*
********************************************************************/
void PrintHelp( void )
{
	printf("TestDMS  <%s>  by Bjorn Stenberg (bjst@sth.frontec.se)\n\n",
		   __DATE__);
	puts("usage: testdms <options> <files>");
	puts("\n\t-s = silent mode  (do not print success reports)");
	puts("\t-v = verbose mode (display verbose archive information)");
	puts("\t-q = quick mode   (check only the header)");
	puts("\nTestDMS returns 0 on successful operations. If multiple files are");
	puts("processed, the result of the last file is returned.");
	exit(0);
}

/*******************************************************************
*
* Function   : CheckFile()
* Description: Test integrity of a file
* Parameters : char*	Filename_PC;	// name of file to test
*
********************************************************************/
int		CheckFile( char*	Filename_PC,
				   t_Flags*	Options_PT )
{
	FILE*	File_PT			= NULL;
	int		Status_I		= OK;
	int		FirstTrack_I	= 0;
	int		TotTracks_I		= 0;
	char	TmpName_AC		[ 250 ];

	File_PT = fopen( Filename_PC, "rb" );

	if ( NULL == File_PT ) {
		strcpy( TmpName_AC, Filename_PC );
		strcat( TmpName_AC, ".dms" );

		File_PT = fopen( TmpName_AC, "rb" );

		if ( NULL == File_PT ) {
			if ( NOT Options_PT->Silent_B )	
				printf( "\tFailed to open '%s'.\n", Filename_PC );
			return ERROR;
		} else
			Filename_PC = TmpName_AC;
	}

	Status_I = CheckHeader( File_PT, 
						    &FirstTrack_I, 
						    &TotTracks_I,
						    Options_PT->Verbose_B );

	/*
	 * if header was fine, and we're not in quick mode,
	 * check out the tracks.
	 */

	if ( OK == Status_I )
		if ( NOT Options_PT->Quick_B )			
			Status_I = CheckTracks( File_PT, 	
								    FirstTrack_I, 
								    TotTracks_I,
								    Options_PT->Verbose_B );

	/*
	 * if we're in silent mode, don't print what's happened
	 */

	if ( Options_PT->Silent_B )				
		return Status_I;					

	switch ( Status_I & 0xFF ) {
	  case ERR_IDENT: {
		  printf("\t'%s' is not a DMS archive.\n",
				 Filename_PC );
		  return ERROR;
	  }

	  case ERR_SHORT_READ:	{
		  if ( Status_I & 0xFF00 )
			  printf("\tShort read in track #%d. '%s' is corrupt.\n",
					 (Status_I & 0xFF00) >> 8, Filename_PC );
		  else
			  printf("\tShort read in header. '%s' is corrupt. (%04X)\n",
					 Filename_PC, Status_I );
		  return ERROR;
	  }
		
	  case ERR_CRC: {
		  if ( Status_I & 0xFF00 )
			  printf("\tCRC Error in track #%d. '%s' is corrupt.\n",
					 (Status_I & 0xFF00) >> 8, Filename_PC );
		  else
			  printf("\tCRC Error in header. '%s' is corrupt.\n",
					 Filename_PC );
		  return ERROR;
	  }

	  case ERR_SIZE: {
		  printf("\tFile is too short! '%s' is corrupt.\n",
					 Filename_PC);
		  return ERROR;
	  }

	  case ERR_OUT_OF_MEM: {
		  printf("\tOut of memory! Operations aborted.\n");
		  return ERROR;
	  }

      case OK: {
		  printf("\t'%s' is ok.\n", Filename_PC );
		  return OK;
	  }
	}

	return OK;
}

/*******************************************************************
*
* Function   : CheckHeader()
* Description: Test tegrity of archive header
* Parameters : FILE*  	File_PT;		// file handler of the archive
*			   int*		FirstTrack_PI;	// ret: first track
*			   int*		TotTracks_PI;	// ret: no of tracks
* Returns    : int						// status
*
********************************************************************/
int		CheckHeader( FILE*	File_PT,
					 int*	FirstTrack_PI, 
					 int*	TotTracks_PI,
					 UBYTE	Verbose_B )
{
	int 		Blocks_I 	= 0;
	UBYTE		RawHeader_AT[ DMS_TOTAL_SIZE ];
	char*		Ident_PC	= NULL;
	UBYTE* 		Header_PT	= NULL;
	USHORT*		Crc_PI		= NULL;
	USHORT		DiskCrc_I	= 0;
	USHORT		CalcCrc_I	= 0;

	/* read archive header */
	Blocks_I = fread( RawHeader_AT, DMS_TOTAL_SIZE, 1, File_PT );
	if ( Blocks_I < 1 )
		return ERR_SHORT_READ;

	Ident_PC	= (char*) RawHeader_AT;
	Header_PT	= (UBYTE*) RawHeader_AT + DMS_IDENT_SIZE;
	Crc_PI		= (USHORT*) (RawHeader_AT + DMS_IDENT_SIZE + DMS_HEADER_SIZE);

	/* test identifier */
	if ( EQUAL != strncmp( Ident_PC, DMS_IDENT, DMS_IDENT_SIZE ) )
		return ERR_IDENT;

	/* test archive header & CRC */
	DiskCrc_I = swap_short(*Crc_PI);
	CalcCrc_I = CreateCRC( Header_PT, DMS_HEADER_SIZE );

	if ( DiskCrc_I != CalcCrc_I )
		return ERR_CRC;

	if ( Verbose_B )
		PrintFileHeader( RawHeader_AT );

	*FirstTrack_PI	= DHEAD_FROM( Header_PT );
	*TotTracks_PI	= DHEAD_TO( Header_PT ) - DHEAD_FROM( Header_PT );

	return OK;
}

/*******************************************************************
*
* Function   : CheckTracks()
* Description: Test tegrity of the tracks
* Parameters : FILE*	File_PT;		// file handler of the archive
*			   int		FirstTrack_I;	// no of first track
*			   int		TotTracks_I;	// no of tracks
* Returns    : int						// status
*
********************************************************************/
int		CheckTracks( FILE*	File_PT,
					 int	FirstTrack_I,
					 int	TotTracks_I,
					 UBYTE	Verbose_B )
{
	int		Status_I	= 0;
	int		DataSize_I	= 0;
	int		Blocks_I	= 0;
	int		Track_I		= 0;
	UBYTE*	TrackBuf_PT	= NULL;
	UBYTE	RawHeader_AT [ TRK_TOTAL_SIZE ];
	USHORT	CalcCrc_I	= 0;
	
	/* allocate buffer for track data */
	TrackBuf_PT = (UBYTE*) malloc( TRACK_BUF_SIZE );
	if ( NULL == TrackBuf_PT )
		return ERR_OUT_OF_MEM;

	if ( Verbose_B ) {
		puts(" Track  PLength  ULength  CMode   USUM  DCRC");
		puts(" -----  -------  -------  ------  ----  ----");
	}

	for ( Track_I = FirstTrack_I; Status_I == OK; Track_I++ ) {
		/* read track header */
		Blocks_I = fread( RawHeader_AT, TRK_TOTAL_SIZE, 1, File_PT );
		if ( Blocks_I < 1 ) {
			free( TrackBuf_PT );
			if ( Track_I > TotTracks_I )
				return OK;
			else
				return ( ERR_SHORT_READ | ( Track_I << 8 ) );
		}

		/* read track data */
		DataSize_I = (int) THEAD_SIZE( RawHeader_AT );
		Blocks_I = fread( TrackBuf_PT, DataSize_I, 1, File_PT);
		if ( Blocks_I < 1 ) {
			free( TrackBuf_PT );
			return ( ERR_SHORT_READ | ( Track_I << 8 ) );
		}

		/* test track data */
		CalcCrc_I = CreateCRC( TrackBuf_PT, DataSize_I );
		if ( CalcCrc_I != THEAD_DCRC( RawHeader_AT ) ) {
			free( TrackBuf_PT );
			return ( ERR_CRC | ( Track_I << 8 ) );
		}
		if ( Verbose_B )
			PrintTrackHeader( RawHeader_AT );
	}

	free( TrackBuf_PT );
	return OK;
}


/*******************************************************************
*
* Function   : CreateCRC()
* Description: Create CRC of a piece of memory
* Parameters : UBYTE*	Mem;
*			   int		Size;
* Returns    : int						// status
*
********************************************************************/
USHORT CreateCRC( UBYTE* Mem, int Size )
{
	static USHORT CRCTab[256]={
        0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
        0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
        0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
        0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
        0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
        0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
        0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
        0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
        0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
        0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
        0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
        0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
        0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
        0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
        0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
        0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
        0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
        0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
        0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
        0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
        0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
        0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
        0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
        0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
        0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
        0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
        0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
        0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
        0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
        0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
        0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
        0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
	};
	register USHORT CRC = 0;

	while(Size--)
		CRC = CRCTab[((CRC ^ *Mem++) & 255)] ^ ((CRC >> 8) & 255);

	return (CRC);
}


/*******************************************************************
*
* Function   : PrintFileHeader()
* Description: Display verbose information about file header
* Parameters : UBYTE*	RawHeader_PT;
*
********************************************************************/
void	PrintFileHeader( UBYTE*	RawHeader_PT )
{
	UBYTE*	Header_PT = RawHeader_PT + DMS_IDENT_SIZE;
	USHORT* Crc_PI	= (USHORT*) (RawHeader_PT + DMS_IDENT_SIZE + 
								 DMS_HEADER_SIZE);
	char	TmpStr_AC [ 50 ];
	LONG	Info_I = DHEAD_INFO( Header_PT );

#ifdef WORD_ALIGN
	LONG	Time_I = 0;
#endif

	printf("\n Lowtrack in Archive      : %02d\n", (int)DHEAD_FROM( Header_PT ) );
	printf(" Hightrack in Archive     : %02d\n", (int)DHEAD_TO( Header_PT ) );
	printf(" Packed size              : %d bytes\n", (int)DHEAD_CSIZE( Header_PT ));
	printf(" UnPacked size            : %d bytes\n", (int)DHEAD_SIZE( Header_PT ));
	printf(" Serial Number of Creator : Generic\n");

	switch( DHEAD_MACHINE( Header_PT ) ) {
	  case 1:  strcpy( TmpStr_AC, "Amiga" );break;
	  case 2:  strcpy( TmpStr_AC, "PC" );break;
	  default: strcpy( TmpStr_AC, "Unknown" );break;
	}
	printf(" Machine Used             : %s\n", TmpStr_AC );

	switch( DHEAD_CPU( Header_PT ) ) {
	  case 1:  strcpy( TmpStr_AC, "MC68000" );break;
	  case 2:  strcpy( TmpStr_AC, "MC68010" );break;
	  case 3:  strcpy( TmpStr_AC, "MC68020" );break;
	  case 4:  strcpy( TmpStr_AC, "MC68030" );break;
	  case 5:  strcpy( TmpStr_AC, "MC68040" );break;
	  default: strcpy( TmpStr_AC, "Unknown" );break;
	}
	printf(" Machine CPU              : %s\n", TmpStr_AC );

	switch( DHEAD_FPU( Header_PT ) ) {
	  case 1:  strcpy( TmpStr_AC, "MC68881" );break;
	  case 2:  strcpy( TmpStr_AC, "MC68882" );break;
	  default: strcpy( TmpStr_AC, "None" );break;
	}
	printf(" CPU Coprocessor          : %s\n", TmpStr_AC );

#ifdef WORD_ALIGN
	Time_I = DHEAD_TIME( Header_PT );
	printf(" Time to create archive   : %d mins. %d secs.\n",
		   Time_I / 60, Time_I % 60 );
#endif

	printf(" Version Number of Creator: v1.%02d\n",
		   DHEAD_CVER( Header_PT ) - 0x64);
	printf(" Version Needed to Extract: v1.%02d\n", 
		   DHEAD_NVER( Header_PT ) - 0x64);

	switch( DHEAD_TYPE( Header_PT ) ) {
	  case 1:  strcpy( TmpStr_AC, "Amiga 1.0 OFS" );break;
	  default: strcpy( TmpStr_AC, "Unknown" );break;
	}
	printf(" Disktype of Archive      : %s\n", TmpStr_AC );

	switch( DHEAD_CMODE( Header_PT ) ) {
	  case 0:  strcpy( TmpStr_AC, "None" );break;
	  case 1:  strcpy( TmpStr_AC, "Simple" );break;
	  case 2:  strcpy( TmpStr_AC, "Quick" );break;
	  case 3:  strcpy( TmpStr_AC, "Medium" );break;
	  case 4:  strcpy( TmpStr_AC, "Deep" );break;
	  case 5:  strcpy( TmpStr_AC, "Heavy1" );break;
	  case 6:  strcpy( TmpStr_AC, "Heavy2" );break;
	  default: strcpy( TmpStr_AC, "Unknown" );break;
	}
	printf(" Compression Mode Used    : %s\n", TmpStr_AC );
	printf(" General Info             : " );

	if ( Info_I & 1 )
		printf("NoZero");

	if ( Info_I & 8 )
		printf(", Banner");
	printf("\n");

	printf(" Info Header CRC          : %04X\n\n", 
		   swap_short( *Crc_PI ) );
}


/*******************************************************************
*
* Function   : PrintFileHeader()
* Description: Display verbose information about file header
* Parameters : UBYTE*	RawHeader_PT;
*
********************************************************************/
void	PrintTrackHeader( UBYTE*	RawHeader_PT )
{
	UBYTE*	Header_PT = RawHeader_PT;
	char	TmpStr_AC [ 50 ];

	switch( THEAD_CMODE( Header_PT ) & 0xff ) {
	  case 0:  		strcpy( TmpStr_AC, "NoComp" 	);break;
	  case 1:  		strcpy( TmpStr_AC, "Simple" 	);break;
	  case 2:  		strcpy( TmpStr_AC, "Quick " 	);break;
	  case 3: 	 	strcpy( TmpStr_AC, "Medium" 	);break;
	  case 4:  		strcpy( TmpStr_AC, "Deep  " 	);break;
	  case 5:  		strcpy( TmpStr_AC, "Heavy1" 	);break;
	  case 6:  		strcpy( TmpStr_AC, "Heavy2" 	);break;
	  default: sprintf( TmpStr_AC, "0x%04x", THEAD_CMODE( Header_PT ) );break;
	}

	if ( THEAD_TNUM( Header_PT ) == -1 )
		printf(" Banner  %5d    %5d   %s  %04X  %04X\n",
			   (int)THEAD_SIZE( Header_PT ),
			   THEAD_PLEN( Header_PT ),
			   TmpStr_AC,
			   THEAD_USUM( Header_PT ),
			   THEAD_DCRC( Header_PT ) );
			 
	else
		printf("  %2d     %5d    %5d   %s  %04X  %04X\n",
			   THEAD_TNUM( Header_PT ),
			   (int)THEAD_SIZE( Header_PT ),
			   THEAD_PLEN( Header_PT ),
			   TmpStr_AC,
			   THEAD_USUM( Header_PT ),
			   THEAD_DCRC( Header_PT ) );
			 
}

