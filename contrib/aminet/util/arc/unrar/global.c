struct MarkHeader MarkHead;
struct OldMainHeader OldMhd;
struct NewMainArchiveHeader NewMhd;
struct OldFileHeader OldLhd;
struct NewFileHeader NewLhd;
struct BlockHeader BlockHead;
struct CommentHeader CommHead;
struct ProtectHeader ProtectHead;
struct RAROptions Opt={ASK_OVERWR,0,MSG_STDOUT,0,0,0,0,0,0,NAMES_DONTCHANGE,0};

UBYTE *TempMemory=NULL;
char *CommMemory=NULL;
UBYTE *UnpMemory=NULL;
char *ArgBuf=NULL;
char *ExclPtr=NULL;
char *ArcNamePtr=NULL;

char ArgName[NM];

int NumArcDrive;
char CurExtrFile[NM],TmpArc[NM];
char ArcFileName[NM];
int SolidType,LockedType,AVType;
int MainComment,Choice;
char FindPath[NM],FindName[NM];
char ExtrPath[NM];
FILE *ArcPtr,*TmpArcPtr,*FilePtr;
char DateStr[12];
int UnpVolume;

int OverwriteAll=0;
int ArcType,SFXLen;

int SkipUnpCRC=0;

char ArcName[NM];
char MainCommand[NM];
char Password[128];
int BrokenMhd,BrokenFileHeader;

int ExclCount;
int ArcCount,TotalArcCount;
long ArcNamesSize;

int TestMode;

int ExitCode=0;
int CheckWriteSize=1;

int MainHeadSize;
long CurBlockPos,NextBlockPos;

unsigned long CurUnpRead,CurUnpWrite;
int Repack=0;
FILE *RdUnpPtr,*WrUnpPtr;

long UnpPackedSize;
long DestUnpSize;

UDWORD LatestTime;

unsigned long PackFileCRC,UnpFileCRC,PackedCRC;

UDWORD HeaderCRC;

int Encryption;

int ArcFormat;

int PackSolid,UnpSolid;

unsigned int UnpWrSize;
unsigned char *UnpWrAddr;

extern unsigned char PN1,PN2,PN3;
extern unsigned short OldKey[4];

