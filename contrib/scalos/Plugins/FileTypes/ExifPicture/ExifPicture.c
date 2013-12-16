// ExifPicture.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <datatypes/pictureclass.h>
#include <scalos/scalos.h>
#include <utility/hooks.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <libraries/locale.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/locale.h>
#include <proto/utility.h>

#include <defs.h>

#include "plugin.h"

#include "ExPicDefs.h"

#include "ExifPicture_base.h"
#include "ExifPicture.h"
#define	ExifPicturePlugin_NUMBERS
#define	ExifPicturePlugin_ARRAY
#define	ExifPicturePlugin_CODE
#include STR(SCALOSLOCALE)

//---------------------------------------------------------------

#define	NOT_A_LOCK		((BPTR) ~0)
#define	IS_VALID_LOCK(x)	((x) != NOT_A_LOCK)

//---------------------------------------------------------------

#define	MEMPOOL_MEMFLAGS	MEMF_ANY
#define	MEMPOOL_PUDDLESIZE	4096
#define	MEMPOOL_THRESHSIZE	2048

//---------------------------------------------------------------

// Describes format descriptor

#define NUM_FORMATS 12

#define FMT_BYTE       1 
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

#define MAX_SECTIONS 20
#define PSEUDO_IMAGE_MARKER 0x123; // Extra value.

//--- Global buffer ---
typedef struct {
    char Buffer[MAX_COMMENT*4];
}Ttype;


//--- Readmodes ---
typedef enum {
    READ_EXIF = 1,
    READ_IMAGE = 2,
    READ_ALL = 3
}ReadMode_t;

//---- Sections ---
typedef struct {
    uchar *  Data;
    int      Type;
    unsigned Size;
}Section_t;

//---- Image informations ----
typedef struct {
    char  FileName[PATH_MAX+1];
    time_t FileDateTime;
    unsigned FileSize;
    char  CameraMake[32];
    char  CameraModel[40];
    char  DateTime[20];
    int   Height, Width;
    int   Orientation;
    int   IsColor;
    int   Process;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    float ExposureBias;
    int   Whitebalance;
    int   MeteringMode;
    int   ExposureProgram;
    int   ISOequivalent;
    char  Comments[MAX_COMMENT];

    unsigned char * ThumbnailPointer;  // Pointer at the thumbnail
    unsigned ThumbnailSize;     // Size of thumbnail.

    char * DatePointer;
}ImageInfo_t;

//----------------------

typedef struct {
    unsigned short Tag;
    char * Desc;
}TagTable_t;


//---------- Exif section --------------------------------

static unsigned char * LastExifRefd;
static unsigned char * DirWithThumbnailPtrs;
static double FocalplaneXRes;
static double FocalplaneUnits;
static int ExifImageWidth;
static int MotorolaOrder = 0;

// for fixing the rotation.
static void * OrientationPtr;
static int    OrientationNumFormat; 
static int CheckFileSkip(void);
static BOOL ResultJPEG = TRUE;

//static char * Processtable_Baseline;

//--- Table of Jpeg encoding process names ------------------

static char LocBaseline[20]="";
static char LocExtendedSeq[50]="";
static char LocProgressive[30]="";
static char LocLossless[20]="";
static char LocDiffSeq[50]="";
static char LocDiffProgressive[50]="";
static char LocDiffLossless[50]="";
static char LocExtendedSeqAritCoding[100]="";
static char LocProgressiveAritCoding[100]="";
static char LocLosslessAritCoding[100]="";
static char LocDiffSeqAritCoding[100]="";
static char LocDiffProgressiveAritCoding[100]="";
static char LocDiffLosslessAritCoding[100]="";
static char LocUnknown[20]="";

static TagTable_t ProcessTable[] = {
    { M_SOF0,   LocBaseline },			// "Baseline"
    { M_SOF1,   LocExtendedSeq },		// "Extended sequential"
    { M_SOF2,   LocProgressive },		// "Progressive"
    { M_SOF3,   LocLossless },			// "Lossless"
    { M_SOF5,   LocDiffSeq },			// "Differential sequential"
    { M_SOF6,   LocDiffProgressive },		// "Differential progressive"
    { M_SOF7,   LocDiffLossless },		// "Differential lossless"
    { M_SOF9,   LocExtendedSeqAritCoding },	// "Extended sequential, arithmetic coding",
    { M_SOF10,  LocProgressiveAritCoding },	// "Progressive, arithmetic coding"
    { M_SOF11,  LocLosslessAritCoding },	// "Lossless, arithmetic coding"
    { M_SOF13,  LocDiffSeqAritCoding },		// "Differential sequential, arithmetic coding"
    { M_SOF14,  LocDiffProgressiveAritCoding },	// "Differential progressive, arithmetic coding,
    { M_SOF15,  LocDiffLosslessAritCoding },	// "Differential lossless, arithmetic coding",
    { 0,        LocUnknown}
};

// 1 - "The 0th row is at the visual top of the image,    and the 0th column is the visual left-hand side."
// 2 - "The 0th row is at the visual top of the image,    and the 0th column is the visual right-hand side."
// 3 - "The 0th row is at the visual bottom of the image, and the 0th column is the visual right-hand side."
// 4 - "The 0th row is at the visual bottom of the image, and the 0th column is the visual left-hand side."
// 5 - "The 0th row is the visual left-hand side of of the image,  and the 0th column is the visual top."
// 6 - "The 0th row is the visual right-hand side of of the image, and the 0th column is the visual top."
// 7 - "The 0th row is the visual right-hand side of of the image, and the 0th column is the visual bottom."
// 8 - "The 0th row is the visual left-hand side of of the image,  and the 0th column is the visual bottom."

// Note: The descriptions here are the same as the name of the command line
// option to pass to jpegtran to right the image

static char LocOrientUndef[50]="";
static char LocOrientNormal[50]="";
static char LocOrientFlipHor[80]="";
static char LocOrientRot180[80]="";
static char LocOrientFlipVert[80]="";
static char LocOrientTranspose[80]="";
static char LocOrientRot90[80]="";
static char LocOrientTransverse[80]="";
static char LocOrientRot270[80]="";

static const char * OrientTab[9] = {
    LocOrientUndef,		// "Undefined"
    LocOrientNormal,		// "Normal"		-  1
    LocOrientFlipHor,		// "flip horizontal"	- left right reversed mirror
    LocOrientRot180,		// "rotate 180"		- 3
    LocOrientFlipVert,		// "flip vertical"	- upside down mirror
    LocOrientTranspose,		// "transpose"		- Flipped about top-left <--> bottom-right axis.
    LocOrientRot90,		// "rotate 90"		- rotate 90 cw to right it.
    LocOrientTransverse,	// "transverse"		- flipped about top-right <--> bottom-left axis
    LocOrientRot270,		// "rotate 270"		- rotate 270 to right it.
};


//--- Prototypes from jpg files -------------------------------------------

Section_t * FindSection(int SectionType);
Section_t * CreateSection(int SectionType, unsigned char * Data, int size);

//--- Describes format descriptor ------------------------------------------

static int BytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};

static TagTable_t TagTable[] = {
  {   0x100,   "ImageWidth"},
  {   0x101,   "ImageLength"},
  {   0x102,   "BitsPerSample"},
  {   0x103,   "Compression"},
  {   0x106,   "PhotometricInterpretation"},
  {   0x10A,   "FillOrder"},
  {   0x10D,   "DocumentName"},
  {   0x10E,   "ImageDescription"},
  {   0x10F,   "Make"},
  {   0x110,   "Model"},
  {   0x111,   "StripOffsets"},
  {   0x112,   "Orientation"},
  {   0x115,   "SamplesPerPixel"},
  {   0x116,   "RowsPerStrip"},
  {   0x117,   "StripByteCounts"},
  {   0x11A,   "XResolution"},
  {   0x11B,   "YResolution"},
  {   0x11C,   "PlanarConfiguration"},
  {   0x128,   "ResolutionUnit"},
  {   0x12D,   "TransferFunction"},
  {   0x131,   "Software"},
  {   0x132,   "DateTime"},
  {   0x13B,   "Artist"},
  {   0x13E,   "WhitePoint"},
  {   0x13F,   "PrimaryChromaticities"},
  {   0x156,   "TransferRange"},
  {   0x200,   "JPEGProc"},
  {   0x201,   "ThumbnailOffset"},
  {   0x202,   "ThumbnailLength"},
  {   0x211,   "YCbCrCoefficients"},
  {   0x212,   "YCbCrSubSampling"},
  {   0x213,   "YCbCrPositioning"},
  {   0x214,   "ReferenceBlackWhite"},
  {   0x828D,  "CFARepeatPatternDim"},
  {   0x828E,  "CFAPattern"},
  {   0x828F,  "BatteryLevel"},
  {   0x8298,  "Copyright"},
  {   0x829A,  "ExposureTime"},
  {   0x829D,  "FNumber"},
  {   0x83BB,  "IPTC/NAA"},
  {   0x8769,  "ExifOffset"},
  {   0x8773,  "InterColorProfile"},
  {   0x8822,  "ExposureProgram"},
  {   0x8824,  "SpectralSensitivity"},
  {   0x8825,  "GPSInfo"},
  {   0x8827,  "ISOSpeedRatings"},
  {   0x8828,  "OECF"},
  {   0x9000,  "ExifVersion"},
  {   0x9003,  "DateTimeOriginal"},
  {   0x9004,  "DateTimeDigitized"},
  {   0x9101,  "ComponentsConfiguration"},
  {   0x9102,  "CompressedBitsPerPixel"},
  {   0x9201,  "ShutterSpeedValue"},
  {   0x9202,  "ApertureValue"},
  {   0x9203,  "BrightnessValue"},
  {   0x9204,  "ExposureBiasValue"},
  {   0x9205,  "MaxApertureValue"},
  {   0x9206,  "SubjectDistance"},
  {   0x9207,  "MeteringMode"},
  {   0x9208,  "LightSource"},
  {   0x9209,  "Flash"},
  {   0x920A,  "FocalLength"},
  {   0x927C,  "MakerNote"},
  {   0x9286,  "UserComment"},
  {   0x9290,  "SubSecTime"},
  {   0x9291,  "SubSecTimeOriginal"},
  {   0x9292,  "SubSecTimeDigitized"},
  {   0xA000,  "FlashPixVersion"},
  {   0xA001,  "ColorSpace"},
  {   0xA002,  "ExifImageWidth"},
  {   0xA003,  "ExifImageLength"},
  {   0xA005,  "InteroperabilityOffset"},
  {   0xA20B,  "FlashEnergy"},                // 0x920B in TIFF/EP
  {   0xA20C,  "SpatialFrequencyResponse"},   // 0x920C    -  -
  {   0xA20E,  "FocalPlaneXResolution"},      // 0x920E    -  -
  {   0xA20F,  "FocalPlaneYResolution"},      // 0x920F    -  -
  {   0xA210,  "FocalPlaneResolutionUnit"},   // 0x9210    -  -
  {   0xA214,  "SubjectLocation"},            // 0x9214    -  -
  {   0xA215,  "ExposureIndex"},              // 0x9215    -  -
  {   0xA217,  "SensingMethod"},              // 0x9217    -  -
  {   0xA300,  "FileSource"},
  {   0xA301,  "SceneType"},
  {      0, NULL}
} ;

//--- Storage for simplified info extracted from file -----------------------------------
//--- ImageInfo: Pointer to structure ImageInfo_t     -----------------------------------

ImageInfo_t ImageInfo;

//--- Pointer to structure Ttype (Global buffer) ----------------------------------------

Ttype BufType;

//---------------------------------------------------------------------------------------

//static int FilesMatched;
static const char *CurrentFile;
static char *ShowImageInfo(struct ExifPictureBase *ExifPictureBase);

//---  Some command line options flags - Currently Unused -------------------------------
//
//static char *strftime_args = NULL; // Format for new file name.
//static int DoModify     = FALSE;
//static int DoReadAction = FALSE;
static int ShowTags     = FALSE;    // Do not show raw by default.
//static int ShowConcise  = FALSE;
//static char *ApplyCommand = NULL;  // Apply this command to all images.
static char *FilterModel = NULL;

//--- Variables for Arguments ----------------------------------------------------------

static int ExifOnly    		= FALSE;
static int ShowFileDateTime 	= TRUE;
static int ShowFileSize 	= TRUE;
static int ShowJpegProcess	= TRUE;
static int ShowJpegComment	= TRUE;

//---------------------------------------------------------------------------------------

//static time_t ExifTimeAdjust = 0;   // Timezone adjust
//static time_t ExifTimeSet = 0;      // Set exif time to a value.

//static char *ThumbnailName = NULL;  // If not NULL, use this string to make up
                                    // the filename to store the thumbnail to.

//static char *ExifXferScrFile = NULL; // Extract Exif header from this file, and
                                     // put it into the jpegs processed.

//static int EditComment = FALSE;     // Invoke an editor for editing the comment
//static int SupressNonFatalErrors = FALSE; // Wether or not to pint warnings on recoverable errors
//static char *CommentSavefileName = NULL; // Save comment to this file.
//static char *CommentInsertfileName = NULL; // Insert comment from this file.
//static int AutoRotate = 0;

//---------------------------------------------------------------------------------------

static void *MemPool;
static struct SignalSemaphore MemPoolSemaphore;

//---------------------------------------------------------------------------------------

#ifndef __AROS__
struct DosLibrary *DOSBase;
T_UTILITYBASE UtilityBase;
T_LOCALEBASE LocaleBase;
#endif

#ifdef __amigaos4__
struct DOSIFace *IDOS;
struct UtilityIFace *IUtility;
struct LocaleIFace *ILocale;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__)
extern T_UTILITYBASE __UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

//-------------------------------- LOCALE MESSAGES ---------------------------------------

static CONST_STRPTR GetLocString(ULONG MsgId, struct ExifPictureBase *ExifPictureBase);

//---------------------------------------------------------------------------------------
static void process_EXIF (unsigned char *ExifSection, unsigned int length);
static void ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength, int NestingLevel);
//static int Exif2tm(struct tm *timeptr, char *ExifTime);
static Section_t Sections[MAX_SECTIONS];
static int SectionsRead;
static int HaveAll;
static BOOL ReadJpegSections(BPTR infile, ReadMode_t ReadMode);
static void process_COM(const uchar *Data, int length);
static void process_SOFn(const uchar *Data, int marker);
static void DiscardData(void);
//static void DiscardAllButExif(void);
//static int RemoveSectionType(int SectionType);
static void ResetJpgfile(void);
static double ConvertAnyFormat(void * ValuePtr, int Format);
static void PrintFormatNumber(void * ValuePtr, int Format, int ByteCount);
//static void Put16u(void * Short, unsigned short PutValue);
static int Get16u(void * Short);
static int Get32s(void * Long);
static unsigned Get32u(void * Long);
static int Get16m(const void * Short);
APTR MyAllocVecPooled(size_t Size);
void MyFreeVecPooled(APTR mem);

//-----------------------------------------------------------

static struct Locale *ExifPictureLocale;

static STRPTR ScaFormatString(CONST_STRPTR FormatString, STRPTR Buffer, size_t MaxLen, LONG NumArgs, ...);
static void ScaFormatDate(struct DateTime *dt, ULONG DayMaxLen, ULONG DateMaxLen, ULONG TimeMaxLen);

static M68KFUNC_P3_PROTO(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch);

static struct FileInfoBlock fib;
static BOOL FibValid = FALSE;
static BOOL BreakPos = FALSE;

//-------------------------------------------------------------

struct FormatDateHookData
	{
	STRPTR fdhd_Buffer;		// buffer to write characters to
	size_t fdhd_Length;		// length of buffer
	};

//-------------------------------------------------------------

BOOL initPlugin(struct PluginBase *base)
{
	struct ExifPictureBase *ExifPictureBase = (struct ExifPictureBase *)base;

	d1(kprintf("%s/%s/%ld:   ExifPictureBase=%08lx  procName=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, \
		ExifPictureBase, FindTask(NULL)->tc_Node.ln_Name));

	ExifPictureBase->pdb_Locale = NULL;
	ExifPictureBase->pdb_Catalog = NULL;

	DOSBase  = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		{
		d1(kprintf("%s/%s/%ld:  Fail\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}
#endif

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#ifdef __amigaos4__
	IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

#ifdef __amigaos4__
	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	// LocaleBase may be NULL
	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39 );
#ifdef __amigaos4__
	if (NULL != LocaleBase)
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		if (NULL == ILocale)
			{
			CloseLibrary((struct Library *)LocaleBase);
			LocaleBase = NULL;
			}
		}
#endif

	if (LocaleBase)
		{
		InitSemaphore(&MemPoolSemaphore);

		MemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		if (NULL == MemPool)
			return FALSE;

		if (NULL == ExifPictureBase->pdb_Locale)
			ExifPictureLocale = ExifPictureBase->pdb_Locale = OpenLocale(NULL);

		if (ExifPictureBase->pdb_Locale)
			{
			if (NULL == ExifPictureBase->pdb_Catalog)
				{
				ExifPictureBase->pdb_Catalog = OpenCatalog(ExifPictureBase->pdb_Locale, 
					(STRPTR) "Scalos/ExifPicturePlugin.catalog", NULL);
				}
			}
		}

	return TRUE;
}


VOID closePlugin(struct PluginBase *base)
{
	struct ExifPictureBase *ExifPictureBase = (struct ExifPictureBase *)base;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (LocaleBase)
		{
		if (ExifPictureBase->pdb_Catalog)
			{
			CloseCatalog(ExifPictureBase->pdb_Catalog);
			ExifPictureBase->pdb_Catalog = NULL;
			}
		if (ExifPictureBase->pdb_Locale)
			{
			CloseLocale(ExifPictureBase->pdb_Locale);
			ExifPictureLocale = ExifPictureBase->pdb_Locale = NULL;
			}

#ifdef __amigaos4__
		if (ILocale)
			{
			DropInterface((struct Interface *)ILocale);
			ILocale = NULL;
			}
#endif
		CloseLibrary((struct Library *)LocaleBase);
		LocaleBase = NULL;
		}
#ifdef __amigaos4__
	if (INewlib)
		{
		DropInterface(INewlib);
		INewlib = NULL;
		}
	if (NewlibBase)
		{
		CloseLibrary(NewlibBase);
		NewlibBase = NULL;
		}
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
#endif
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
	if (MemPool)
		{
		DeletePool(MemPool);
		MemPool = NULL;
		}
}

void InitProccessTableLocale(struct ExifPictureBase *ExifPictureBase)
{
	CONST_STRPTR TempLine;

	TempLine = GetLocString(MSGID_BASELINE, ExifPictureBase);					// "Baseline"
	stccpy(LocBaseline, TempLine, sizeof(LocBaseline));

	TempLine = GetLocString(MSGID_EXTENDED_SEQUENTIAL, ExifPictureBase);				// "Extended sequential"
	stccpy(LocExtendedSeq, TempLine, sizeof(LocExtendedSeq));

	TempLine = GetLocString(MSGID_PROGRESSIVE, ExifPictureBase);					// "Progressive"
	stccpy(LocProgressive, TempLine, sizeof(LocProgressive));

	TempLine = GetLocString(MSGID_LOSSLESS, ExifPictureBase);					// "Lossless"
	stccpy(LocLossless, TempLine, sizeof(LocLossless));

	TempLine = GetLocString(MSGID_DIFFERENTIAL_SEQUENTIAL, ExifPictureBase);			// "Differential sequential"
	stccpy(LocDiffSeq, TempLine, sizeof(LocDiffSeq));

	TempLine = GetLocString(MSGID_DIFFERENTIAL_PROGRESSIVE, ExifPictureBase);			// "Differential progressive"
	stccpy(LocDiffProgressive, TempLine, sizeof(LocDiffProgressive));

	TempLine = GetLocString(MSGID_DIFFERENTIAL_LOSSLESS, ExifPictureBase);				// "Differential lossless"
	stccpy(LocDiffLossless, TempLine, sizeof(LocDiffLossless));

	TempLine = GetLocString(MSGID_EXTENDED_SEQUENTIAL_ARITHMETIC_CODING, ExifPictureBase);		// "Extended sequential, arithmetic coding"
	stccpy(LocExtendedSeqAritCoding, TempLine, sizeof(LocExtendedSeqAritCoding));

	TempLine = GetLocString(MSGID_PROGRESSIVE_ARITHMETIC_CODING, ExifPictureBase);			// "Progressive, arithmetic coding"
	stccpy(LocProgressiveAritCoding, TempLine, sizeof(LocProgressiveAritCoding));

	TempLine = GetLocString(MSGID_LOSSLESS_ARITHMETIC_CODING, ExifPictureBase);			// "Lossless, arithmetic coding"
	stccpy(LocLosslessAritCoding, TempLine, sizeof(LocLosslessAritCoding));

	TempLine = GetLocString(MSGID_DIFFERENTIAL_SEQUENTIAL_ARITHMETIC_CODING, ExifPictureBase);	// "Differential sequential, arithmetic coding"
	stccpy(LocDiffSeqAritCoding, TempLine, sizeof(LocDiffSeqAritCoding));

	TempLine = GetLocString(MSGID_DIFFERENTIAL_PROGRESSIVE_ARITHMETIC_CODING, ExifPictureBase);	// "Differential progressive, arithmetic coding"
	stccpy(LocDiffProgressiveAritCoding, TempLine, sizeof(LocDiffProgressiveAritCoding));

	TempLine = GetLocString(MSGID_DIFFERENTIAL_LOSSLESS_ARITHMETIC_CODING, ExifPictureBase);	// "Differential lossless, arithmetic coding"
	stccpy(LocDiffLosslessAritCoding, TempLine, sizeof(LocDiffLosslessAritCoding));

	TempLine = GetLocString(MSGID_UNKNOWN, ExifPictureBase);					// "Unknown"
	stccpy(LocUnknown, TempLine, sizeof(LocUnknown));

	//----------------------------- OrienTab[] locale strings ----------------------------------------------------------------------------------------

	TempLine = GetLocString(MSGID_ORIENTTAB_UNDEFINED, ExifPictureBase);				// "Undefined"
	stccpy(LocOrientUndef, TempLine, sizeof(LocOrientUndef));

	TempLine = GetLocString(MSGID_ORIENTTAB_NORMAL, ExifPictureBase);				// "Normal"
	stccpy(LocOrientNormal, TempLine, sizeof(LocOrientNormal));

	TempLine = GetLocString(MSGID_ORIENTTAB_FLIP_HORIZONTAL, ExifPictureBase);			// "flip horizontal"
	stccpy(LocOrientFlipHor, TempLine, sizeof(LocOrientFlipHor));

	TempLine = GetLocString(MSGID_ORIENTTAB_ROTATE_180, ExifPictureBase);				// "rotate 180"
	stccpy(LocOrientRot180, TempLine, sizeof(LocOrientRot180));

	TempLine = GetLocString(MSGID_ORIENTTAB_FLIP_VERTICAL, ExifPictureBase);			// "flip vertical"
	stccpy(LocOrientFlipVert, TempLine, sizeof(LocOrientFlipVert));

	TempLine = GetLocString(MSGID_ORIENTTAB_TRANSPOSE, ExifPictureBase);				// "transpose"
	stccpy(LocOrientTranspose, TempLine, sizeof(LocOrientTranspose));

	TempLine = GetLocString(MSGID_ORIENTTAB_ROTATE_90, ExifPictureBase);				// "rotate 90"
	stccpy(LocOrientRot90, TempLine, sizeof(LocOrientRot90));

	TempLine = GetLocString(MSGID_ORIENTTAB_TRANSVERSE, ExifPictureBase);				// "transverse"
	stccpy(LocOrientTransverse, TempLine, sizeof(LocOrientTransverse));

	TempLine = GetLocString(MSGID_ORIENTTAB_ROTATE_270, ExifPictureBase);				// "rotate 270"
	stccpy(LocOrientRot270, TempLine, sizeof(LocOrientRot270));


}

// NewExifPicture()
LIBFUNC_P3(STRPTR, LIBToolTipInfoString,
	A0, struct ScaToolTipInfoHookData *, ttshd,
	A1, CONST_STRPTR, args,
	A6, struct PluginBase *, PluginBase, 5);
{
	struct ExifPictureBase *ExifPictureBase = (struct ExifPictureBase *)PluginBase;
	BPTR oldDir = NOT_A_LOCK;
	BPTR dirLock;
	BPTR infile = (BPTR)NULL;
	struct RDArgs *allocRdArg = NULL;
	struct RDArgs *rdArg = NULL;
	STRPTR ArgsCopy = NULL;
	char FaultBuffer[120];
	InitProccessTableLocale(ExifPictureBase);

	(void) args;

	do	{
		SIPTR ArgArray[5];
		BPTR fLock;

		ReadMode_t ReadMode = READ_EXIF;

		dirLock = ParentDir(ttshd->ttshd_FileLock);
		if ((BPTR)NULL == dirLock)
			break;
		memset(ArgArray, 0, sizeof(ArgArray));

		allocRdArg = AllocDosObject(DOS_RDARGS, NULL);

		d1(kprintf("%s/%s/%ld: allocRdArg=%08lx\n", __FILE__, __FUNC__, __LINE__, allocRdArg));
		if (NULL == allocRdArg)
			break;

		ArgsCopy = MyAllocVecPooled(2 + strlen(args));
		if (NULL == ArgsCopy)
			break;

		// important: ReadArgs() requires a trailing "\n" !!
		strcpy(ArgsCopy, args);
		strcat(ArgsCopy, "\n");

		allocRdArg->RDA_Source.CS_Buffer = ArgsCopy;
		allocRdArg->RDA_Source.CS_Length = strlen(ArgsCopy);
		allocRdArg->RDA_Source.CS_CurChr = 0;
		allocRdArg->RDA_Flags |= RDAF_NOPROMPT;

		rdArg = ReadArgs("EXIFONLY/S,NOFILEDATETIME/S,NOFILESIZE/S,NOJPEGPROCESS/S,NOCOMMENT/S", ArgArray, allocRdArg);

		d1(kprintf("%s/%s/%ld: rdArg=%08lx\n", __FILE__, __FUNC__, __LINE__, rdArg));
		if (NULL == rdArg)
			{
			Fault(IoErr(), NULL, FaultBuffer, sizeof(FaultBuffer)-1);
			sprintf(BufType.Buffer,  GetLocString(MSGID_ERROR_DOS, ExifPictureBase), IoErr(), FaultBuffer);

			d1(kprintf("%s/%s/%ld: ReadArgs() returned error %ld\n", __FILE__, __FUNC__, __LINE__, IoErr()));
			break;
			}

		if (ArgArray[0])
			ExifOnly = TRUE;
		if (ArgArray[1])
			ShowFileDateTime = FALSE;
		if (ArgArray[2])
			ShowFileSize = FALSE;
		if (ArgArray[3])
			ShowJpegProcess = FALSE;
		if (ArgArray[4])
			ShowJpegComment = FALSE;

		oldDir = CurrentDir(dirLock);

		fLock = Lock(ttshd->ttshd_FileName, ACCESS_READ);
		if (fLock)
			{
			if (Examine(fLock, &fib))
				FibValid = TRUE;
			UnLock(fLock);
			}

		CurrentFile = ttshd->ttshd_FileName;

		d1(kprintf("%s/%s/%ld: ResetJpgfile() \n", __FILE__, __FUNC__, __LINE__));

		ResetJpgfile();

		d1(kprintf("%s/%s/%ld: ImageInfo\n", __FILE__, __FUNC__, __LINE__));

		memset(&ImageInfo, 0, sizeof(ImageInfo));
		ImageInfo.FlashUsed = -1;
		ImageInfo.MeteringMode = -1;

		{
		struct stat st;

		 if (stat(ttshd->ttshd_FileName, &st) >= 0)
			{
			ImageInfo.FileDateTime = st.st_mtime;
			ImageInfo.FileSize = st.st_size;
			d1(kprintf("%s/%s/%ld: FileSize = %ld\n", __FILE__, __FUNC__, __LINE__, ImageInfo.FileSize));
			}
		 else
			{
			break;
			}
		}

		strncpy(ImageInfo.FileName, ttshd->ttshd_FileName, PATH_MAX);

		infile = Open(ttshd->ttshd_FileName, MODE_OLDFILE);

		if (infile == (BPTR)NULL)
        		break;

		d1(kprintf("%s/%s/%ld: ReadJpegSections(infile, ReadMde)\n", __FILE__, __FUNC__, __LINE__));

		ResultJPEG = ReadJpegSections(infile, ReadMode);

		if (!ResultJPEG)
			{
			d1(kprintf("%s/%s/%ld: ResultJPEG = [%ld]\n", __FILE__, __FUNC__, __LINE__, ResultJPEG));
			sprintf(ttshd->ttshd_Buffer, GetLocString(MSGID_ERROR_NOJPEG, ExifPictureBase), ttshd->ttshd_FileName);
			break;
			}

        	if (CheckFileSkip())
			{
			DiscardData();
			break;
			}

		d1(kprintf("%s/%s/%ld: ShowImageInfo()\n", __FILE__, __FUNC__, __LINE__, CurrentFile));

    		ttshd->ttshd_Buffer = ShowImageInfo(ExifPictureBase);

		if (BreakPos)
			strcat(ttshd->ttshd_Buffer, "\n");

/*		if ('\n' == ttshd->ttshd_Buffer[strlen(ttshd->ttshd_Buffer) - 2])
			d1(kprintf("%s/%s/%ld: BreakPos = [%ld] FOUND { n } POS - 2\n", __FILE__, __FUNC__, __LINE__, BreakPos));

		if ('\n' == ttshd->ttshd_Buffer[strlen(ttshd->ttshd_Buffer) - 1])
			d1(kprintf("%s/%s/%ld: BreakPos = [%ld] FOUND { n } POS - 1\n", __FILE__, __FUNC__, __LINE__, BreakPos));

		if ('\n' != ttshd->ttshd_Buffer[strlen(ttshd->ttshd_Buffer) - 1])
			d1(kprintf("%s/%s/%ld: BreakPos = [%ld] !!! NOT FOUND { n } POS - 1 !!!\n", __FILE__, __FUNC__, __LINE__, BreakPos));

		d1(kprintf( "\n%s/%s/%ld: TEST BreakPos = [%ld] ttshd->ttshd_Buffer: \n%s.\n", __FILE__, __FUNC__, __LINE__, BreakPos, ttshd->ttshd_Buffer));
*/

		} while (0);

	BreakPos = FALSE;

	if (infile == (BPTR)NULL)
		DiscardData();

	if (infile)
		Close(infile);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);

	if (dirLock)
		UnLock(dirLock);

	if (rdArg)
		FreeArgs(rdArg);
	if (allocRdArg)
		FreeDosObject(DOS_RDARGS, allocRdArg);

	if (ArgsCopy)
		MyFreeVecPooled(ArgsCopy);

	d1(kprintf("%s/%s/%ld: ResultString=<%s>\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_Buffer));

	return ttshd->ttshd_Buffer;
}
LIBFUNC_END

static STRPTR ScaFormatString(CONST_STRPTR formatString, STRPTR Buffer, size_t MaxLen, LONG NumArgs, ...)
{
	va_list args;

	va_start(args, NumArgs);

	if (ExifPictureLocale)
		{
		ULONG *ArgArray;

		ArgArray = MyAllocVecPooled(sizeof(ULONG) * NumArgs);
		if (ArgArray)
			{
			struct FormatDateHookData fd;
			struct Hook fmtHook;
			ULONG n;
			STATIC_PATCHFUNC(FormatDateHookFunc)

			for (n = 0; n < NumArgs; n++)
				ArgArray[n] = va_arg(args, LONG);

			fmtHook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
			fmtHook.h_Data = &fd;

			fd.fdhd_Buffer = Buffer;
			fd.fdhd_Length = MaxLen;

			FormatString(ExifPictureLocale, (STRPTR) formatString, ArgArray, &fmtHook);

			MyFreeVecPooled(ArgArray);
			}
		}
	else
		{
		vsprintf(Buffer, formatString, args);
		}

	va_end(args);

	return Buffer;
}
//----------------------------------------------------------------------------

static void ScaFormatDate(struct DateTime *dt, ULONG DayMaxLen, ULONG DateMaxLen, ULONG TimeMaxLen)
{
	if (ExifPictureLocale)
		{
		struct FormatDateHookData fd;
		struct Hook fmtHook;
		STATIC_PATCHFUNC(FormatDateHookFunc)

		d1(kprintf("%s/%s/%ld: DateBuff=%08lx Len=%ld  TimeBuff=%08lx Len=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dt->dat_StrDate, DateMaxLen, dt->dat_StrTime, DateMaxLen));

		fmtHook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
		fmtHook.h_Data = &fd;

		if (dt->dat_StrDay)
			{
			fd.fdhd_Buffer = dt->dat_StrDay;
			fd.fdhd_Length = DayMaxLen;

			FormatDate(ExifPictureLocale, "%A",
				&dt->dat_Stamp, &fmtHook);
			}

		if (dt->dat_StrDate)
			{
			fd.fdhd_Buffer = dt->dat_StrDate;
			fd.fdhd_Length = DateMaxLen;

			FormatDate(ExifPictureLocale, ExifPictureLocale->loc_ShortDateFormat,
				&dt->dat_Stamp, &fmtHook);
			}

		if (dt->dat_StrTime)
			{
			fd.fdhd_Buffer = dt->dat_StrTime;
			fd.fdhd_Length = TimeMaxLen;

			FormatDate(ExifPictureLocale, ExifPictureLocale->loc_ShortTimeFormat,
				&dt->dat_Stamp, &fmtHook);
			}
		}
	else
		{
		DateToStr(dt);		// no size checking possible here :(
		}
}

//----------------------------------------------------------------------------

static M68KFUNC_P3(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch)
{
	struct FormatDateHookData *fd = (struct FormatDateHookData *) theHook->h_Data;

	(void) locale;

	if (fd->fdhd_Length >= 1)
		{
		*(fd->fdhd_Buffer)++ = ch;
		fd->fdhd_Length--;
		}
}
M68KFUNC_END

//----------------------------------------------------------------------------

static STRPTR GetChangeDate(STRPTR BufferDate, struct DateStamp *ChangeDate)
{
	char DayBuffer[64], DateBuffer[64], TimeBuffer[64];
	struct DateTime dt;

	memset(&dt, 0, sizeof(dt));

	dt.dat_Stamp = *ChangeDate;
	dt.dat_StrDay  = DayBuffer;
	dt.dat_StrDate = DateBuffer;
	dt.dat_StrTime = TimeBuffer;

	ScaFormatDate(&dt, sizeof(DayBuffer), sizeof(DateBuffer), sizeof(TimeBuffer));

	sprintf(BufferDate, "%s, %s %s", DayBuffer, DateBuffer, TimeBuffer);

	return BufferDate;
}

/***
//----------------------------------------------------------------------------

static STRPTR GetSizeString(void)
{
	STRPTR SizeString;

	if (FibValid)
		SizeString = ScaFormatString(GetLocString(MSGID_SIZE_FORMAT), TextSize, sizeof(TextSize), 1, fib.fib_Size);
	else
		SizeString = GetLocString(MSGID_VERSION_NOT_AVAILABLE);

	return SizeString;
}
***/
//----------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Parse the marker stream until SOS or EOI is seen;
//--------------------------------------------------------------------------
static BOOL ReadJpegSections (BPTR infile, ReadMode_t ReadMode)
{
	int a;
	int HaveCom = FALSE;

	a = FGetC(infile);

	if (a != 0xff || FGetC(infile) != M_SOI)
		{
		d1(kprintf("%s/%s/%ld: return FALSE - [a != 0xff || fgetc(infile) != M_SOI]\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}

	d1(kprintf("%s/%s/%ld: a = %04lx\n", __FILE__, __FUNC__, __LINE__, a));

	for(;;)
		{
		int itemlen;
		int ll,lh, got;
		uchar * Data;
		int marker = 0;

		if (SectionsRead >= MAX_SECTIONS)
			{
			d1(kprintf("%s/%s/%ld: ResultJPEG=[%ld]\n", __FILE__, __FUNC__, __LINE__, ResultJPEG));
			ErrFatal("Too many sections in jpg file");
			}

		for (a=0; a<7; a++)
			{
			marker = FGetC(infile);
			if (marker != 0xff)
				{
				d1(kprintf("%s/%s/%ld: break - [marker != 0xff]\n", __FILE__, __FUNC__, __LINE__));
				break;
				}

			if (a >= 6)
				{
				sprintf(BufType.Buffer, "too many padding bytes\n");
				d1(kprintf("%s/%s/%ld: 'too many padding bytes'\n", __FILE__, __FUNC__, __LINE__));
				return FALSE;
				}
			}

		if (marker == 0xff)
			{
			ErrFatal("too many padding bytes!");
			d1(kprintf("%s/%s/%ld: ErrFatal(too many padding bytes!)\n", __FILE__, __FUNC__, __LINE__));
			}

		Sections[SectionsRead].Type = marker;
  
		// Read the length of the section.
		lh = FGetC(infile);
		ll = FGetC(infile);

		itemlen = (lh << 8) | ll;

		d1(kprintf("%s/%s/%ld: itemlen=[%ld]\n", __FILE__, __FUNC__, __LINE__, itemlen));

		if (itemlen < 2)
			{
			ErrFatal("invalid marker");
			d1(kprintf("%s/%s/%ld: ErrFatal(invalid marker)\n", __FILE__, __FUNC__, __LINE__));
			}

		d1(kprintf("%s/%s/%ld: ResultJPEG=[%ld] - itemlen=[%ld]\n", __FILE__, __FUNC__, __LINE__, ResultJPEG, itemlen));

		Sections[SectionsRead].Size = itemlen;

		Data = (uchar *)MyAllocVecPooled(itemlen);
		if (Data == NULL)
			{
			d1(kprintf("%s/%s/%ld: ErrFatal(Could not allocate memory)\n", __FILE__, __FUNC__, __LINE__));
			ErrFatal("Could not allocate memory");
			}

		Sections[SectionsRead].Data = Data;

		// Store first two pre-read bytes.
		Data[0] = (uchar)lh;
		Data[1] = (uchar)ll;

		got = FRead(infile, Data+2, 1, itemlen-2); // Read the whole section.
		if (got != itemlen-2)
			{
			ErrFatal("Premature end of file?");
			d1(kprintf("%s/%s/%ld: ErrFatal(Premature end of file?)\n", __FILE__, __FUNC__, __LINE__));
			}

		SectionsRead += 1;

		switch (marker)
			{
			case M_SOS:   // stop before hitting compressed data. If reading entire image is requested, read the rest of the data.
				if (ReadMode & READ_IMAGE)
					{
					int cp, ep, size;

					// Determine how much file is left.
					cp = Seek(infile, 0, SEEK_END);
					ep = Seek(infile, cp, OFFSET_BEGINNING);

					size = ep-cp;

					Data = (uchar *)MyAllocVecPooled(size);
					if (Data == NULL)
						{
						ErrFatal("could not allocate data for entire image");
						d1(kprintf("%s/%s/%ld: ErrFatal(could not allocate data for entire image)\n", __FILE__, __FUNC__, __LINE__));
						}

					got = FRead(infile, Data, 1, size);
					if (got != size)
						{
						strcat(BufType.Buffer, "Could not read the rest of the image");
						ErrFatal("could not read the rest of the image");
						d1(kprintf("%s/%s/%ld: ErrFatal(could not read the rest of the image)\n", __FILE__, __FUNC__, __LINE__));
						}

					Sections[SectionsRead].Data = Data;
					Sections[SectionsRead].Size = size;
					Sections[SectionsRead].Type = PSEUDO_IMAGE_MARKER;
					SectionsRead ++;
					HaveAll = 1;
					}
					return TRUE; // ResultJPEG = TRUE;

			case M_EOI:   // in case it's a tables-only JPEG stream
				sprintf(BufType.Buffer, "No image in jpeg!\n");
				d1(kprintf("%s/%s/%ld: ResultJPEG = [%ld] - '%s'\n", __FILE__, __FUNC__, __LINE__, ResultJPEG, BufType.Buffer));
				return FALSE;

			case M_COM: // Comment section
				if (HaveCom || ((ReadMode & READ_EXIF) == 0))
					{
					// Discard this section.
					MyFreeVecPooled(Sections[--SectionsRead].Data);
					}
				else
					{
					process_COM(Data, itemlen);
					HaveCom = TRUE;
					}
				break;

			case M_JFIF:
				// Regular jpegs always have this tag, exif images have the exif
				// marker instead, althogh ACDsee will write images with both markers.
				// this program will re-create this marker on absence of exif marker.
				// hence no need to keep the copy from the file.
				MyFreeVecPooled(Sections[--SectionsRead].Data);
				break;

			case M_EXIF:
				// Seen files from some 'U-lead' software with Vivitar scanner
				// that uses marker 31 for non exif stuff.  Thus make sure 
				// it says 'Exif' in the section before treating it as exif.

	                	if (((ReadMode & READ_EXIF) && memcmp(Data+2, "Exif", 4)) == 0)
					{
					d1(kprintf("%s/%s/%ld: ResultJPEG=[%ld] - (ReadMode & READ_EXIF) && memcmp(Data+2, 'Exif', 4)\n", __FILE__, __FUNC__, __LINE__, ResultJPEG));
					process_EXIF((unsigned char *)Data, itemlen);
					}
				else
					{
					d1(kprintf("%s/%s/%ld: MyFreeVecPooled(Sections[--SectionsRead].Data - ResultJPEG=[%ld]\n", __FILE__, __FUNC__, __LINE__, ResultJPEG));
	                    		MyFreeVecPooled(Sections[--SectionsRead].Data);
					}
				d1(kprintf("%s/%s/%ld: ResultJPEG=[%ld]\n", __FILE__, __FUNC__, __LINE__, ResultJPEG));
	                	break;

			case M_SOF0: 
			case M_SOF1: 
			case M_SOF2: 
			case M_SOF3: 
			case M_SOF5: 
			case M_SOF6: 
			case M_SOF7: 
			case M_SOF9: 
			case M_SOF10:
			case M_SOF11:
			case M_SOF13:
			case M_SOF14:
			case M_SOF15:
				d1(kprintf("%s/%s/%ld: process_SOFn(Data, marker) - ResultJPEG=[%ld]\n", __FILE__, __FUNC__, __LINE__, ResultJPEG));
			       	process_SOFn(Data, marker);
				break;

			default:
				// Skip any other sections.
				if (ShowTags)
					{
					sprintf(BufType.Buffer,"Jpeg section marker 0x%02x size %d\n",marker, itemlen);
					d1(kprintf("%s/%s/%ld: ShowTags=[%ld] - ResultJPEG=[%ld] - BufType.Buffer=%s\n", __FILE__, __FUNC__, __LINE__, ShowTags, ResultJPEG, BufType.Buffer));
					}
				break;
			}

		}

	d1(kprintf("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, BufType.Buffer));

	return TRUE;
}

//--------------------------------------------------------------------------
// Error exit handler
//--------------------------------------------------------------------------
void ErrFatal(char * msg)
{
	if (CurrentFile)
		sprintf(BufType.Buffer, "Error : %s in file '%s'\n",CurrentFile, msg);
	else
		sprintf(BufType.Buffer, "Error : %s\n", msg); 

	exit(EXIT_FAILURE);
}
#if 0
//--------------------------------------------------------------------------
// Report non fatal errors.  Now that microsoft.net modifies exif headers,
// there's corrupted ones, and there could be more in the future.
//--------------------------------------------------------------------------
void ErrNonfatal(char * msg, int a1, int a2)
{
	char Temp[100];

	if (SupressNonFatalErrors)
		return;

	sprintf(Temp, "%s %s %s\n", msg, a1, a2);

	if (CurrentFile)
		sprintf(BufType.Buffer, "Nonfatal Error : '%s'",CurrentFile);
	else
		sprintf(BufType.Buffer, "Nonfatal Error : ");

	strcat(BufType.Buffer, Temp); 	// fprintf(stderr, msg, a1, a2);
}
#endif
//--------------------------------------------------------------------------
// check if this file should be skipped based on contents.
//--------------------------------------------------------------------------
static int CheckFileSkip(void)
{
	// I sometimes add code here to only process images based on certain
	// criteria - for example, only to convert non progressive jpegs to progressives, etc..

	if (FilterModel)
		{
		// Filtering processing by camera model.
		if (strstr(ImageInfo.CameraModel, FilterModel) == NULL)
			{
			// Skip.
			return TRUE;
			}
		}

	if (ExifOnly)
		{
		// Filtering by EXIF only.  Skip all files that have no Exif.
		if (FindSection(M_EXIF) == NULL)
			return TRUE;
		}
    return FALSE;
}

#if 0
//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
static void Put16u(void * Short, unsigned short PutValue)
{
	if (MotorolaOrder)
		{
		((uchar *)Short)[0] = (uchar)(PutValue>>8);
		((uchar *)Short)[1] = (uchar)PutValue;
		}
	else
		{
		((uchar *)Short)[0] = (uchar)PutValue;
		((uchar *)Short)[1] = (uchar)(PutValue>>8);
	}
}
#endif

//--------------------------------------------------------------------------
// Convert a 16 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
static int Get16u(void * Short)
{
	if (MotorolaOrder)
		return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
	else
		return (((uchar *)Short)[1] << 8) | ((uchar *)Short)[0];
}

//--------------------------------------------------------------------------
// Convert a 32 bit signed value from file's native byte order
//--------------------------------------------------------------------------
static int Get32s(void * Long)
{
	if (MotorolaOrder)
		return  ((( char *)Long)[0] << 24) | (((uchar *)Long)[1] << 16) | (((uchar *)Long)[2] << 8 ) | (((uchar *)Long)[3] << 0 );
	else
		return  ((( char *)Long)[3] << 24) | (((uchar *)Long)[2] << 16) | (((uchar *)Long)[1] << 8 ) | (((uchar *)Long)[0] << 0 );
}

//--------------------------------------------------------------------------
// Convert a 32 bit unsigned value from file's native byte order
//--------------------------------------------------------------------------
static unsigned Get32u(void * Long)
{
	return (unsigned)Get32s(Long) & 0xffffffff;
}

//--------------------------------------------------------------------------
// Display a number as one of its many formats
//--------------------------------------------------------------------------
static void PrintFormatNumber(void * ValuePtr, int Format, int ByteCount)
{
    switch(Format)
	{
        case FMT_SBYTE:
        case FMT_BYTE:
		sprintf(BufType.Buffer, "%02x\n",*(uchar *)ValuePtr);
	        break;
        case FMT_USHORT:
		sprintf(BufType.Buffer, "%d\n",Get16u(ValuePtr));
		break;
        case FMT_ULONG:     
        case FMT_SLONG:
		sprintf(BufType.Buffer, "%d\n",Get32s(ValuePtr));
                break;
        case FMT_SSHORT:
		sprintf(BufType.Buffer, "%hd\n",(signed short)Get16u(ValuePtr));
		 break;
        case FMT_URATIONAL:
        case FMT_SRATIONAL: 
		sprintf(BufType.Buffer, "%d/%d\n",Get32s(ValuePtr), Get32s(4+(char *)ValuePtr));
		 break;
        case FMT_SINGLE:
		sprintf(BufType.Buffer, "%f\n",(double)*(float *)ValuePtr);
		break;
        case FMT_DOUBLE:
		sprintf(BufType.Buffer,"%f\n",*(double *)ValuePtr);
		break;
        default: 
		sprintf(BufType.Buffer, "Unknown format %d:\n", Format);
	}
}


//--------------------------------------------------------------------------
// Evaluate number, be it int, rational, or float from directory.
//--------------------------------------------------------------------------
static double ConvertAnyFormat(void * ValuePtr, int Format)
{
	double Value;
	Value = 0;

	switch(Format)
		{
		case FMT_SBYTE:
			Value = *(signed char *)ValuePtr;
			break;

		case FMT_BYTE:
			Value = *(uchar *)ValuePtr;
			break;

		case FMT_USHORT:
			Value = Get16u(ValuePtr);
			break;

		case FMT_ULONG:
			Value = Get32u(ValuePtr);
			break;

		case FMT_URATIONAL:
		case FMT_SRATIONAL: 
			{
			int Num,Den;
			Num = Get32s(ValuePtr);
			Den = Get32s(4+(char *)ValuePtr);
			if (Den == 0)
				Value = 0;
			else
				Value = (double)Num/Den;
			break;
			}

		case FMT_SSHORT:
			Value = (signed short)Get16u(ValuePtr);
			break;

		case FMT_SLONG:
			Value = Get32s(ValuePtr);
			break;

		case FMT_SINGLE:
			Value = (double)*(float *)ValuePtr;
			break;

		case FMT_DOUBLE:
			Value = *(double *)ValuePtr;
			break;
		}
	return Value;
}

//--------------------------------------------------------------------------
// Process one of the nested EXIF directories.
//--------------------------------------------------------------------------
static void ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength, int NestingLevel)
{
	int de;
	int a;
	int NumDirEntries;
	unsigned ThumbnailOffset = 0;
	unsigned ThumbnailSize = 0;
	char BufferTemp[256];

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NestingLevel > 4)
		{
		strcat(BufType.Buffer, "Maximum directory nesting exceeded (corrupt exif header)");
		// ErrNonfatal("Maximum directory nesting exceeded (corrupt exif header)", 0,0);
		return;
		}

	NumDirEntries = Get16u(DirStart);
	#define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))

	{
	unsigned char * DirEnd;
	DirEnd = DIR_ENTRY_ADDR(DirStart, NumDirEntries);
	if (DirEnd+4 > (OffsetBase+ExifLength))
		{
		if (DirEnd+2 == OffsetBase+ExifLength || DirEnd == OffsetBase+ExifLength)
			{
			// Version 1.3 of jhead would truncate a bit too much.
			// This also caught later on as well.
			}
		else
			{
			// Note: Files that had thumbnails trimmed with jhead 1.3 or earlier
			// might trigger this.
			// ErrNonfatal("Illegally sized directory",0,0);
			strcat(BufType.Buffer, "Illegally sized directory");
			return;
			}
		}
	if (DirEnd > LastExifRefd)
		LastExifRefd = DirEnd;
	}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (ShowTags)
		{
		sprintf(BufferTemp, "Directory with %d entries\n", NumDirEntries);
		strcat(BufType.Buffer, BufferTemp);
		// printf("Directory with %d entries\n",NumDirEntries);
		}

	for (de=0;de<NumDirEntries;de++)
		{
		int Tag, Format, Components;
		unsigned char * ValuePtr;
		int ByteCount;
		unsigned char * DirEntry;

		DirEntry = DIR_ENTRY_ADDR(DirStart, de);

		Tag = Get16u(DirEntry);
		Format = Get16u(DirEntry+2);
		Components = Get32u(DirEntry+4);

		if ((Format-1) >= NUM_FORMATS)
			{
			// (-1) catches illegal zero case as unsigned underflows to positive large.
			// ErrNonfatal("Illegal number format %d for tag %04x", Format, Tag);
			sprintf(BufferTemp, "Illegal number format %d for tag %04x", Format, Tag);
			strcat(BufType.Buffer, BufferTemp);
			continue;
			}

		ByteCount = Components * BytesPerFormat[Format];

		if (ByteCount > 4)
			{
			unsigned OffsetVal;
			OffsetVal = Get32u(DirEntry+8);

			// If its bigger than 4 bytes, the dir entry contains an offset.
			if (OffsetVal+ByteCount > ExifLength)
				{
				// Bogus pointer offset and / or bytecount value
				// ErrNonfatal("Illegal value pointer for tag %04x", Tag,0);
				sprintf(BufferTemp, "Illegal value pointer for tag %04x", Tag);
				strcat(BufType.Buffer, BufferTemp);
				continue;
				}
			ValuePtr = OffsetBase+OffsetVal;
			}
		else
			{
			// 4 bytes or less and value is in the dir entry itself
			ValuePtr = DirEntry+8;
			}

		if (LastExifRefd < ValuePtr+ByteCount)
			{
			// Keep track of last byte in the exif header that was actually referenced.
			// That way, we know where the discardable thumbnail data begins.
			LastExifRefd = ValuePtr+ByteCount;
			}

		if (ShowTags)
			{
			// Show tag name
			for (a=0;;a++)
				{
				if (TagTable[a].Tag == 0)
					{
					// printf("  Unknown Tag %04x Value = ", Tag);
					sprintf(BufferTemp, "Unknown Tag %04x Value = ", Tag);
					strcat(BufType.Buffer, BufferTemp);
					break;
					}

				if (TagTable[a].Tag == Tag)
					{
					// printf("    %s = ",TagTable[a].Desc);
					sprintf(BufferTemp, "    %s = ",TagTable[a].Desc);
					strcat(BufType.Buffer, BufferTemp);
					break;
					}
				}

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// Show tag value.
			switch(Format)
				{
				case FMT_UNDEFINED:
					// Undefined is typically an ascii string.

				case FMT_STRING:
					// String arrays printed without function call (different from int arrays)
					{
					int NoPrint = 0;
		                        // printf("\"");
					strcat(BufType.Buffer,"\"");

					for (a=0;a<ByteCount;a++)
						{
						if (ValuePtr[a] >= 32)
							{
							// putchar((ValuePtr)[a]);
							stccpy(BufferTemp, (char *)&ValuePtr[a], sizeof(BufferTemp) -1);
							strcat(BufType.Buffer, BufferTemp);
							NoPrint = 0;
							}
						else
							{
							// Avoiding indicating too many unprintable characters of proprietary
							// bits of binary information this program may not know how to parse.
							if (!NoPrint)
								{
								// putchar('?');
								strcat(BufType.Buffer, "?");
								NoPrint = 1;
								}
							}
						}
					// printf("\"\n");
					strcat(BufType.Buffer, "\n");
					}
					break;

				default:
					// Handle arrays of numbers later (will there ever be?)
					PrintFormatNumber(ValuePtr, Format, ByteCount);
				}
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// Extract useful components of tag
		switch(Tag)
			{
		case TAG_MAKE:
			strncpy(ImageInfo.CameraMake, (char *)ValuePtr, 31);
			break;

		case TAG_MODEL:
			strncpy(ImageInfo.CameraModel, (char *)ValuePtr, 39);
			break;

		case TAG_DATETIME_ORIGINAL:
			strncpy(ImageInfo.DateTime, (char *)ValuePtr, 19);
			ImageInfo.DatePointer = (char *)ValuePtr;
			break;

		case TAG_USERCOMMENT:
			// Olympus has this padded with trailing spaces.  Remove these first.
			for (a=ByteCount;;)
				{
				a--;
				if ((ValuePtr)[a] == ' ')
					{
					(ValuePtr)[a] = '\0';
					}
				else
					{
					break;
					}
				if (a == 0)
					break;
				}
	                // Copy the comment
	                if (memcmp(ValuePtr, "ASCII",5) == 0)
				{
				for (a=5;a<10;a++)
					{
					int c;
					c = (ValuePtr)[a];
					if (c != '\0' && c != ' ')
						{
						strncpy(ImageInfo.Comments, (char *)(a+ValuePtr), 199);
						break;
						}
					}
				}
			else
				{
				strncpy(ImageInfo.Comments, (char *)ValuePtr, 199);
				}
			break;

		case TAG_FNUMBER:
		        // Simplest way of expressing aperture, so I trust it the most.
		        // (overwrite previously computd value if there is one)
			ImageInfo.ApertureFNumber = (float)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_APERTURE:
		case TAG_MAXAPERTURE:
			// More relevant info always comes earlier, so only use this field if we don't 
			// have appropriate aperture information yet.
			if (ImageInfo.ApertureFNumber == 0)
				ImageInfo.ApertureFNumber = (float)exp(ConvertAnyFormat(ValuePtr, Format)*log(2.0)*0.5);
			break;

		case TAG_FOCALLENGTH:
			// Nice digital cameras actually save the focal length as a function
			// of how farthey are zoomed in.
			ImageInfo.FocalLength = (float)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_SUBJECT_DISTANCE:
			// Inidcates the distacne the autofocus camera is focused to.
			// Tends to be less accurate as distance increases.
			ImageInfo.Distance = (float)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_EXPOSURETIME:
			// Simplest way of expressing exposure time, so I trust it most.
			// (overwrite previously computd value if there is one)
			ImageInfo.ExposureTime = (float)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_SHUTTERSPEED:
			// More complicated way of expressing exposure time, so only use
			// this value if we don't already have it from somewhere else.
		if (ImageInfo.ExposureTime == 0)
			ImageInfo.ExposureTime = (float)(1/exp(ConvertAnyFormat(ValuePtr, Format)*log(2.0)));
			break;

		case TAG_FLASH:
			if ((int)ConvertAnyFormat(ValuePtr, Format) & 7)
				ImageInfo.FlashUsed = TRUE;
			else
				ImageInfo.FlashUsed = FALSE;
			break;

		case TAG_ORIENTATION:
			ImageInfo.Orientation = (int)ConvertAnyFormat(ValuePtr, Format);
			OrientationPtr = ValuePtr;
			OrientationNumFormat = Format;
			if (ImageInfo.Orientation < 1 || ImageInfo.Orientation > 8)
				{
				sprintf(BufferTemp, "Undefined rotation value %d", ImageInfo.Orientation);
				strcat(BufType.Buffer, BufferTemp);
				ImageInfo.Orientation = 0;
				}
			break;

                case TAG_EXIF_IMAGELENGTH:
                case TAG_EXIF_IMAGEWIDTH:
			// Use largest of height and width to deal with images that have been
			// rotated to portrait format.
			a = (int)ConvertAnyFormat(ValuePtr, Format);
			if (ExifImageWidth < a)
				ExifImageWidth = a;
			break;

                case TAG_FOCALPLANEXRES:
			FocalplaneXRes = ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_FOCALPLANEUNITS:
			switch((int)ConvertAnyFormat(ValuePtr, Format))
				{
			case 1:
				FocalplaneUnits = 25.4;
                                break; // inch
			case 2:
	                        // According to the information I was using, 2 means meters.
	                        // But looking at the Cannon powershot's files, inches is the only
	                        // sensible value.
	                        FocalplaneUnits = 25.4;
	                        break;

			case 3: FocalplaneUnits = 10;
				break;  // centimeter
			case 4: FocalplaneUnits = 1;
				break;  // milimeter
			case 5: FocalplaneUnits = .001;
				break;  // micrometer
	                	}
                	break;

                // Remaining cases contributed by: Volker C. Schoech (schoech@gmx.de)

		case TAG_EXPOSURE_BIAS:
			ImageInfo.ExposureBias = (float)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_WHITEBALANCE:
			ImageInfo.Whitebalance = (int)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_METERING_MODE:
			ImageInfo.MeteringMode = (int)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_EXPOSURE_PROGRAM:
			ImageInfo.ExposureProgram = (int)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_ISO_EQUIVALENT:
			ImageInfo.ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
			if ( ImageInfo.ISOequivalent < 50 )
				ImageInfo.ISOequivalent *= 200;
			break;

		case TAG_THUMBNAIL_OFFSET:
			ThumbnailOffset = (unsigned)ConvertAnyFormat(ValuePtr, Format);
			DirWithThumbnailPtrs = DirStart;
			break;

		case TAG_THUMBNAIL_LENGTH:
			ThumbnailSize = (unsigned)ConvertAnyFormat(ValuePtr, Format);
			break;

		case TAG_EXIF_OFFSET:
		case TAG_INTEROP_OFFSET:
			{
			unsigned char * SubdirStart;
			SubdirStart = OffsetBase + Get32u(ValuePtr);
			if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength)
				{
			        // ErrNonfatal("Illegal exif or interop ofset directory link",0,0);
				strcat(BufType.Buffer, "Illegal exif or interop ofset directory link\n");
			    	}
			else
				{
			        ProcessExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
			    	}
			    	continue;
			}
			break;
		default:
			d1(KPrintF("%s/%s/%ld: unknown tag:  tag=%04lx  format=%ld  Components=%ld\n", __FILE__, __FUNC__, __LINE__, Tag, Format, Components));
			break;
			}
		}


	{
        // In addition to linking to subdirectories via exif tags, 
        // there's also a potential link to another directory at the end of each
        // directory.  this has got to be the result of a comitee!
        unsigned char * SubdirStart;
        unsigned Offset;

        if (DIR_ENTRY_ADDR(DirStart, NumDirEntries) + 4 <= OffsetBase+ExifLength)
		{
            	Offset = Get32u(DirStart+2+12*NumDirEntries);
            	if (Offset)
			{
                	SubdirStart = OffsetBase + Offset;
                	if (SubdirStart > OffsetBase+ExifLength)
				{
                    		if (SubdirStart < OffsetBase+ExifLength+20)
					{
                        		// Jhead 1.3 or earlier would crop the whole directory!
                        		// As Jhead produces this form of format incorrectness, 
                        		// I'll just let it pass silently
                        		if (ShowTags)
						// printf("Thumbnail removed with Jhead 1.3 or earlier\n");
						strcat(BufType.Buffer, "Thumbnail removed with Jhead 1.3 or earlier\n");
                    			}
				else
					{
					// ErrNonfatal("Illegal subdirectory link",0,0);
					strcat(BufType.Buffer, "Illegal subdirectory link\n");
                    			}
                		}
			else
				{
                    		if (SubdirStart <= OffsetBase+ExifLength)
					{
                        		ProcessExifDir(SubdirStart, OffsetBase, ExifLength, NestingLevel+1);
                    			}
                		}
            		}
        	}
	else
		{
		// The exif header ends before the last next directory pointer.
        	}
    	}


	if (ThumbnailSize && ThumbnailOffset)
		{
        	if (ThumbnailSize + ThumbnailOffset <= ExifLength)
			{
            		// The thumbnail pointer appears to be valid.  Store it.
            		ImageInfo.ThumbnailPointer = OffsetBase + ThumbnailOffset;
            		ImageInfo.ThumbnailSize = ThumbnailSize;

            		if (ShowTags)
				{
				// printf("Thumbnail size: %d bytes\n",ThumbnailSize);
				sprintf(BufferTemp, "Thumbnail size: %d bytes\n",ThumbnailSize);
				strcat(BufType.Buffer, BufferTemp);
            			}
        		}
    		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
}

//--------------------------------------------------------------------------
// Process a EXIF marker
// Describes all the drivel that most digital cameras include...
//--------------------------------------------------------------------------
static void process_EXIF (unsigned char * ExifSection, unsigned int length)
{
	int FirstOffset;
	char Btemp[256];

	FocalplaneXRes = 0;
	FocalplaneUnits = 0;
	ExifImageWidth = 0;
	OrientationPtr = NULL;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (ShowTags)
		{
		sprintf(Btemp, "Exif header %d bytes long\n",length);
		strcat(BufType.Buffer, Btemp);
		//printf("Exif header %d bytes long\n",length);
		}

	{ // Check the EXIF header component
        	static uchar ExifHeader[] = "Exif\0\0";
        	if (memcmp(ExifSection+2, ExifHeader,6))
			{
			strcat(BufType.Buffer, "Incorrect Exif header\n");
			// ErrNonfatal("Incorrect Exif header",0,0);
			return;
			}
	}

	if (memcmp(ExifSection+8,"II",2) == 0)
		{
		if (ShowTags)
			// printf("Exif section in Intel order\n");
			strcat(BufType.Buffer, "Exif section in Intel order\n");
		MotorolaOrder = 0;
		}
	else
		{
		if (memcmp(ExifSection+8,"MM",2) == 0)
			{
			if (ShowTags)
				// printf("Exif section in Motorola order\n");
				strcat(BufType.Buffer,"Exif section in Intel order\n");
			MotorolaOrder = 1;
			}
		else
			{
			strcat(BufType.Buffer,"Invalid Exif alignment marker.");
			// ErrNonfatal("Invalid Exif alignment marker.",0,0);
			return;
			}
		}

	// Check the next value for correctness.
	if (Get16u(ExifSection+10) != 0x2a)
		{
		// ErrNonfatal("Invalid Exif start (1)",0,0);
		strcat(BufType.Buffer, "Invalid Exif start (1)");
		return;
		}

	FirstOffset = Get32u(ExifSection+12);
	if (FirstOffset < 8 || FirstOffset > 16)
		{
		// I used to ensure this was set to 8 (website I used indicated its 8)
		// but PENTAX Optio 230 has it set differently, and uses it as offset. (Sept 11 2002)
		// ErrNonfatal("Suspicious offset of first IFD value",0,0);
		strcat(BufType.Buffer, "Suspicious offset of first IFD value");
		}

	LastExifRefd = ExifSection;
	DirWithThumbnailPtrs = NULL;

	// First directory starts 16 bytes in.  All offset are relative to 8 bytes in.

	d1(kprintf("%s/%s/%ld: ProcessExifDir(ExifSection)\n", __FILE__, __FUNC__, __LINE__));

	ProcessExifDir(ExifSection+8+FirstOffset, ExifSection+8, length-6, 0);

	// Compute the CCD width, in milimeters.
	if (FocalplaneXRes != 0)
		{
		ImageInfo.CCDWidth = (float)(ExifImageWidth * FocalplaneUnits / FocalplaneXRes);
		}

	if (ShowTags)
		{
		// printf("Non settings part of Exif header: %d bytes\n",ExifSection+length-LastExifRefd);
		sprintf(Btemp, "Non settings part of Exif header: %d bytes\n",(int) (ExifSection+length-LastExifRefd));
		strcat(BufType.Buffer, Btemp);
		}

	d1(kprintf("%s/%s/%ld: ProcessExifDir END\n", __FILE__, __FUNC__, __LINE__));
}

#if 0
//--------------------------------------------------------------------------
// Convert exif time to Unix time structure
//--------------------------------------------------------------------------
static int Exif2tm(struct tm * timeptr, char * ExifTime)
{
	int a;

	timeptr->tm_wday = -1;

	// Check for format: YYYY:MM:DD HH:MM:SS format.
	a = sscanf(ExifTime, "%d:%d:%d %d:%d:%d", &timeptr->tm_year, &timeptr->tm_mon, &timeptr->tm_mday, &timeptr->tm_hour, &timeptr->tm_min, &timeptr->tm_sec);

	if (a == 6)
		{
		timeptr->tm_isdst = -1;  
		timeptr->tm_mon -= 1;      // Adjust for unix zero-based months 
		timeptr->tm_year -= 1900;  // Adjust for year starting at 1900 
		return TRUE; // worked. 
		}

	return FALSE; // Wasn't in Exif date format.
}
#endif

//--------------------------------------------------------------------------
// Show the collected image info, displaying camera F-stop and shutter speed
// in a consistent and legible fashion.
//--------------------------------------------------------------------------
static char *ShowImageInfo(struct ExifPictureBase *ExifPictureBase)
{
	int a;
	char BufFileSize[100]="";
	char BufFileDateTime[100]="";
	char BufCameraMake[300]="";
	char BufDateTime[50]="";
	char BufResolution[30]="";
	char BufOrientation[30]="";
	char BufFlashUsed[20]="";
	char BufExposureTime1[30]="";
	char BufExposureTime2[7]="";
	char BufApertureFNumber[30]="";
	char BufDistance[50]="";
	char BufISOequivalent[50]="";
	char BufExposureBias[50]="";
	char BufProcessTable[100]="";
	char BufFocalLength[50]="";
	char BufFocalLengthCCDWidth[50]=""; 
	char BufCCDWidth[10]="";
	char BufComment[3]="";
	char Temp[20]="";
	STRPTR SizeString;
	STRPTR DateString;
	char DateBuffer[128]="";
	struct tm ts;


	if (ShowFileSize)
		{
		if (FibValid)
			{
			SizeString = ScaFormatString(GetLocString(MSGID_PICTURE_FILESIZE, ExifPictureBase), BufFileSize, sizeof(BufFileSize), 1, fib.fib_Size);
			strcat(BufType.Buffer, SizeString);
			}
		else
			{
			sprintf(BufFileSize, GetLocString(MSGID_PICTURE_FILESIZE, ExifPictureBase),ImageInfo.FileSize);
			strcat(BufType.Buffer, BufFileSize);
			}
		}

	if (ShowFileDateTime)
		{
		if (FibValid)
			{
			DateString = GetChangeDate(DateBuffer, &fib.fib_Date);
			sprintf(BufFileDateTime, GetLocString(MSGID_PICTURE_FILEDATE, ExifPictureBase), DateString);
			}
		else
			{
			ts = *localtime(&ImageInfo.FileDateTime);
			strftime(Temp, 20, "%Y/%m/%d %H:%M:%S", &ts);
			sprintf(BufFileDateTime, GetLocString(MSGID_PICTURE_FILEDATE, ExifPictureBase), Temp);
			}
		strcat(BufType.Buffer, BufFileDateTime);
		}

	if (ImageInfo.CameraMake[0])
		{
		sprintf(BufCameraMake, GetLocString(MSGID_CAMERA_MAKE_MODEL, ExifPictureBase), ImageInfo.CameraMake, ImageInfo.CameraModel);
		strcat(BufType.Buffer, BufCameraMake);
		BreakPos = TRUE;
		}

	if (ImageInfo.DateTime[0])
		{
		sprintf(BufDateTime, GetLocString(MSGID_DATE_TIME, ExifPictureBase), ImageInfo.DateTime);
		strcat(BufType.Buffer, BufDateTime);
		BreakPos = TRUE;
		}

	sprintf(BufResolution, GetLocString(MSGID_PICTURE_RESOLUTION, ExifPictureBase),ImageInfo.Width, ImageInfo.Height);
	strcat(BufType.Buffer, BufResolution);


	if (ImageInfo.Orientation > 1)
		{
		// Only print orientation if one was supplied, and if its not 1 (normal orientation)
		sprintf(BufOrientation, GetLocString(MSGID_ORIENTATION, ExifPictureBase), OrientTab[ImageInfo.Orientation]);
		strcat(BufType.Buffer, BufOrientation);
		BreakPos = TRUE;
		}

	if (ImageInfo.IsColor == 0)
		{
		strcat(BufType.Buffer, GetLocString(MSGID_ISCOLOR, ExifPictureBase));
		BreakPos = TRUE;
		}

	if (ImageInfo.FlashUsed >= 0)
		{
		sprintf(BufFlashUsed, GetLocString(MSGID_FLASHUSED, ExifPictureBase), \
					ImageInfo.FlashUsed ? GetLocString(MSGID_YES, ExifPictureBase) :GetLocString(MSGID_NO, ExifPictureBase));

		strcat(BufType.Buffer, BufFlashUsed);
		BreakPos = TRUE;
		}

	if (ImageInfo.FocalLength)
		{
		sprintf(BufFocalLength, GetLocString(MSGID_FOCAL_LENGTH, ExifPictureBase), (double)ImageInfo.FocalLength); // "Focal length : %4.1fmm"
		strcat(BufType.Buffer, BufFocalLength);
		if (ImageInfo.CCDWidth)
			{
			sprintf(BufFocalLengthCCDWidth, GetLocString(MSGID_FOCAL_LENGTH_CDDWIDTH, ExifPictureBase), \
					(int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*36 + 0.5)); // "  (35mm equivalent: %dmm)"

			strcat(BufType.Buffer, BufFocalLengthCCDWidth);
			}
		strcat(BufType.Buffer, "\n");
		BreakPos = TRUE;
		}

	if (ImageInfo.CCDWidth)
		{
		sprintf(BufCCDWidth, GetLocString(MSGID_CDDWIDTH, ExifPictureBase), (double)ImageInfo.CCDWidth); // "CCD width    : %4.2fmm\n"
		strcat(BufType.Buffer, BufCCDWidth);
		BreakPos = TRUE;
		}

	if (ImageInfo.ExposureTime)
		{
		if (ImageInfo.ExposureTime < 0.010)
			{
			sprintf(BufExposureTime1, GetLocString(MSGID_EXPOSURETIME64, ExifPictureBase), (double)ImageInfo.ExposureTime); // "Exposure time: %6.4f s "
			strcat(BufType.Buffer, BufExposureTime1);
			}
		else
			{
			sprintf(BufExposureTime1, GetLocString(MSGID_EXPOSURETIME53, ExifPictureBase), (double)ImageInfo.ExposureTime); // "Exposure time: %5.3f s "
			strcat(BufType.Buffer, BufExposureTime1);
			}

		if (ImageInfo.ExposureTime <= 0.5)
			{
			sprintf(BufExposureTime2, " (1/%d)",(int)(0.5 + 1/ImageInfo.ExposureTime));
			strcat(BufType.Buffer, BufExposureTime2);
			}

		strcat(BufType.Buffer, "\n");
		BreakPos = TRUE;
		}

	if (ImageInfo.ApertureFNumber)
		{
		sprintf(BufApertureFNumber, GetLocString(MSGID_APERTUREFNUMBER, ExifPictureBase), (double)ImageInfo.ApertureFNumber); // "Aperture     : f/%3.1f\n"
		strcat(BufType.Buffer, BufApertureFNumber);
		BreakPos = TRUE;
		}

	if (ImageInfo.Distance)
		{
		if (ImageInfo.Distance < 0)
			{
			strcat(BufType.Buffer, GetLocString(MSGID_DISTANCE_INFINITE, ExifPictureBase));  // "Focus dist.  : Infinite\n"
			}
		else
			{
			sprintf(BufDistance, GetLocString(MSGID_DISTANCE, ExifPictureBase), (double)ImageInfo.Distance); // "Focus dist.  : %4.2fm\n"
			strcat(BufType.Buffer, BufDistance); 
			}
				BreakPos = TRUE;
		}

	if (ImageInfo.ISOequivalent)	// 05-jan-2001 vcs
		{
		sprintf(BufISOequivalent, GetLocString(MSGID_ISOEQUIVALENT, ExifPictureBase), (int)ImageInfo.ISOequivalent); // "ISO equiv.   : %2d\n"
		strcat(BufType.Buffer,BufISOequivalent);
				BreakPos = TRUE;
		}

	if (ImageInfo.ExposureBias)	// 05-jan-2001 vcs
		{
		sprintf(BufExposureBias, GetLocString(MSGID_EXPOSUREBIAS, ExifPictureBase), (double)ImageInfo.ExposureBias); // "Exposure bias:%4.2f\n"
		strcat(BufType.Buffer, BufExposureBias);
				BreakPos = TRUE;
		}

	if (ImageInfo.Whitebalance)	// 05-jan-2001 vcs
		{
		switch(ImageInfo.Whitebalance)
			{
			case 1:
				strcat(BufType.Buffer, GetLocString(MSGID_WHITEBALANCE1, ExifPictureBase)); // "Whitebalance : sunny\n"
				BreakPos = TRUE;
				break;
			case 2:
				strcat(BufType.Buffer, GetLocString(MSGID_WHITEBALANCE2, ExifPictureBase)); // "Whitebalance : fluorescent\n"
				BreakPos = TRUE;
				break;
			case 3:
				strcat(BufType.Buffer, GetLocString(MSGID_WHITEBALANCE3, ExifPictureBase)); // "Whitebalance : incandescent\n"
				BreakPos = TRUE;
				break;
			default:
				strcat(BufType.Buffer, GetLocString(MSGID_WHITEBALANCE4, ExifPictureBase)); // "Whitebalance : cloudy\n"
				BreakPos = TRUE;
			}
		}

	if (ImageInfo.MeteringMode)	// 05-jan-2001 vcs
		{
		switch(ImageInfo.MeteringMode)
			{
			case 2:
				strcat(BufType.Buffer,GetLocString(MSGID_METERINGMODE_CENTER, ExifPictureBase)); // "Metering Mode: center weight\n"
				BreakPos = TRUE;
				break;
			case 3:
				strcat(BufType.Buffer,GetLocString(MSGID_METERINGMODE_SPOT, ExifPictureBase)); // "Metering Mode: spot\n"
				BreakPos = TRUE;
				break;
			case 5:
				strcat(BufType.Buffer,GetLocString(MSGID_METERINGMODE_MATRIX, ExifPictureBase)); // "Metering Mode: matrix\n"
				BreakPos = TRUE;
				break;
			}
		}

	if (ImageInfo.ExposureProgram)	// 05-jan-2001 vcs
		{
		switch(ImageInfo.ExposureProgram)
			{
			case 2:
				strcat(BufType.Buffer, GetLocString(MSGID_EXPOSURE_PROGRAM1, ExifPictureBase)); // "Exposure     : program (auto)\n"
				BreakPos = TRUE;
				break;
			case 3:
				strcat(BufType.Buffer, GetLocString(MSGID_EXPOSURE_PROGRAM2, ExifPictureBase)); // "Exposure     : aperture priority (semi-auto)\n"
				BreakPos = TRUE;
				break;
			case 4:
				strcat(BufType.Buffer, GetLocString(MSGID_EXPOSURE_PROGRAM3, ExifPictureBase)); // "Exposure     : shutter priority (semi-auto)\n"
				BreakPos = TRUE;
				break;
			}
		}

	if (ShowJpegProcess)
		{
		for (a=0;;a++)
			{
			if (ProcessTable[a].Tag == ImageInfo.Process || ProcessTable[a].Tag == 0)
				{
				sprintf(BufProcessTable, GetLocString(MSGID_JPEG_PROCESS, ExifPictureBase), ProcessTable[a].Desc); // "Jpeg process : %s\n"
				strcat(BufType.Buffer, BufProcessTable);
				break;
				}
			}

		}

	if (ShowJpegComment)
		{
		// Print the comment. Print 'Comment:' for each new line of comment.
		if (ImageInfo.Comments[0])
			{
			int a,c;

			strcat(BufType.Buffer,GetLocString(MSGID_COMMENT, ExifPictureBase)); // "Comment      : "
			for (a=0; a<MAX_COMMENT; a++)
				{
	            		c = ImageInfo.Comments[a];

	            		if (c == '\0')
					break;

				if (c == '\n')
					{
					 // Do not start a new line if the string ends with a carriage return.
	                		if (ImageInfo.Comments[a+1] != '\0')
						{
						strcat(BufType.Buffer, "\n");
	                        	        strcat(BufType.Buffer, GetLocString(MSGID_COMMENT, ExifPictureBase)); // "\n Comment      : "
	                			}
					else
						{
	                        	        strcat(BufType.Buffer, "\n");
	                			}
	            			}
				else
					{
	                	        stccpy(BufComment, &ImageInfo.Comments[a], sizeof(BufComment) -1);
	                	        strcat(BufType.Buffer, BufComment);
					// strcpy(BufComment, "");
	            			}
	        		}
				if (BreakPos)
		               		strcat(BufType.Buffer, "\n");
			  }
		}

	d1(kprintf("%s/%s/%ld: \nTEST BufType.Buffer: \n%s<\n", __FILE__, __FUNC__, __LINE__, BufType.Buffer));

	return (BufType.Buffer);
}

//--------------------------------------------------------------------------
// Get 16 bits motorola order (always) for jpeg header stuff.
//--------------------------------------------------------------------------
static int Get16m(const void * Short)
{
    return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
}


//--------------------------------------------------------------------------
// Process a COM marker.
// We want to print out the marker contents as legible text;
// we must guard against random junk and varying newline representations.
//--------------------------------------------------------------------------
static void process_COM(const uchar * Data, int length)
{
	int ch;
	char Comment[MAX_COMMENT+1];
	int nch;
	int a;

	nch = 0;

	if (length > MAX_COMMENT)
		length = MAX_COMMENT; // Truncate if it won't fit in our structure.

	for (a=2; a<length; a++)
		{
		ch = Data[a];

		if (ch == '\r' && Data[a+1] == '\n')
			continue; // Remove cr followed by lf.

		if (ch >= 32 || ch == '\n' || ch == '\t')
			Comment[nch++] = (char)ch;
		else
			break;
		}

	Comment[nch] = '\0'; // Null terminate


	d1(kprintf("%s/%s/%ld: Comment =<%s>\n", __FILE__, __FUNC__, __LINE__, Comment));

/*
	if (ShowTags)
		{
		sprintf(BufType.Buffer,"COM marker comment: %s\n",Comment);
		// printf("COM marker comment: %s\n",Comment);
		}
*/
	strcpy(ImageInfo.Comments,Comment);
}

 
//--------------------------------------------------------------------------
// Process a SOFn marker.  This is useful for the image dimensions
//--------------------------------------------------------------------------
static void process_SOFn(const uchar * Data, int marker)
{
	int data_precision, num_components;

	data_precision = Data[2];
	ImageInfo.Height = Get16m(Data+3);
	ImageInfo.Width = Get16m(Data+5);
	num_components = Data[7];

	if (num_components == 3)
		{
		ImageInfo.IsColor = 1;
		}
	else
		{
		ImageInfo.IsColor = 0;
		}

	ImageInfo.Process = marker;

	if (ShowTags)
		{
		// printf("JPEG image is %uw * %uh, %d color components, %d bits per sample\n", ImageInfo.Width, ImageInfo.Height, num_components, data_precision);
		}
}

//--------------------------------------------------------------------------
// Discard read data.
//--------------------------------------------------------------------------
static void DiscardData(void)
{
	int a;
	for (a=0; a<SectionsRead; a++)
		{
		MyFreeVecPooled(Sections[a].Data);
		}
	memset(&ImageInfo, 0, sizeof(ImageInfo));
	SectionsRead = 0;
	HaveAll = 0;
}

#if 0
//--------------------------------------------------------------------------
// Discard everything but the exif and comment sections.
//--------------------------------------------------------------------------
static void DiscardAllButExif(void)
{
	Section_t ExifKeeper;
	Section_t CommentKeeper;
	int a;

	memset(&ExifKeeper, 0, sizeof(ExifKeeper));
	memset(&CommentKeeper, 0, sizeof(ExifKeeper));

	for (a=0; a<SectionsRead; a++)
		{
		if (Sections[a].Type == M_EXIF && ExifKeeper.Type == 0)
			{
			ExifKeeper = Sections[a];
			}
		else if (Sections[a].Type == M_COM && CommentKeeper.Type == 0)
			{
			CommentKeeper = Sections[a];
			}
		else
			{
			MyFreeVecPooled(Sections[a].Data);
			}
		}

	SectionsRead = 0;

	if (ExifKeeper.Type)
		{
		Sections[SectionsRead++] = ExifKeeper;
		}

	if (CommentKeeper.Type)
		{
		Sections[SectionsRead++] = CommentKeeper;
		}
}    
#endif

//--------------------------------------------------------------------------
// Check if image has exif header.
//--------------------------------------------------------------------------
Section_t * FindSection(int SectionType)
{
	int a;

	for (a=0; a<SectionsRead-1; a++)
		{
		if (Sections[a].Type == SectionType)
			{
			return &Sections[a];
			}
		}

	// Could not be found.
	return NULL;
}

#if 0
//--------------------------------------------------------------------------
// Remove a certain type of section.
//--------------------------------------------------------------------------
static int RemoveSectionType(int SectionType)
{
	int a;

	for (a=0; a<SectionsRead-1; a++)
		{
		if (Sections[a].Type == SectionType)
			{
			// Free up this section
			free (Sections[a].Data);
			// Move succeding sections back by one to close space in array.
			memmove(Sections+a, Sections+a+1, sizeof(Section_t) * (SectionsRead-a));
			SectionsRead -= 1;
			return TRUE;
			}
		}
	return FALSE;
}
#endif

//--------------------------------------------------------------------------
// Add a section (assume it doesn't already exist) - used for 
// adding comment sections.
//--------------------------------------------------------------------------
Section_t * CreateSection(int SectionType, unsigned char * Data, int Size)
{
	Section_t * NewSection;
	int a;

	// Insert it in third position - seems like a safe place to put 
	// things like comments.

	if (SectionsRead < 2)
		{
		ErrFatal("Too few sections!");
		}

	if (SectionsRead >= MAX_SECTIONS)
		{
		ErrFatal("Too many sections!");
		}

	for (a=SectionsRead; a>2; a--)
		{
		Sections[a] = Sections[a-1];          
		}

	SectionsRead += 1;

	NewSection = Sections+2;

	NewSection->Type = SectionType;
	NewSection->Size = Size;
	NewSection->Data = Data;

	return NewSection;
}


//--------------------------------------------------------------------------
// Initialisation.
//--------------------------------------------------------------------------
static void ResetJpgfile(void)
{
	memset(&Sections, 0, sizeof(Sections));
	SectionsRead = 0;
	HaveAll = 0;
}

//----------------------------------------------------------------------------

static CONST_STRPTR GetLocString(ULONG MsgId, struct ExifPictureBase *ExifPictureBase)
{
	struct ExifPicturePlugin_LocaleInfo li;

	li.li_Catalog = ExifPictureBase->pdb_Catalog;	
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return GetExifPicturePluginString(&li, MsgId);
}

//-----------------------------------------------------------------------------

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

#if !defined(__SASC) && !defined(__amigaos4__)
// Replacement for SAS/C library functions

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

APTR _WBenchMsg;

#endif /* !__SASC && !__amigaos4__ */

//-----------------------------------------------------------------------------

APTR MyAllocVecPooled(size_t Size)
{
	APTR ptr;

	if (MemPool)
		{
		ObtainSemaphore(&MemPoolSemaphore);
		ptr = AllocPooled(MemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&MemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, MemPool, Size));

	return NULL;
}


void MyFreeVecPooled(APTR mem)
{
	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, MemPool, mem));
	if (MemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&MemPoolSemaphore);
		FreePooled(MemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&MemPoolSemaphore);
		}
}

//-----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
