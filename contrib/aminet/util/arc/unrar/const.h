#include "os.h"
#include "compress.h"

#define  Min(x,y) (((x)<(y)) ? (x):(y))
#define  Max(x,y) (((x)>(y)) ? (x):(y))

#define  NM  260


#define  SIZEOF_MARKHEAD         7
#define  SIZEOF_OLDMHD           7
#define  SIZEOF_NEWMHD          13
#define  SIZEOF_OLDLHD          21
#define  SIZEOF_NEWLHD          32
#define  SIZEOF_SHORTBLOCKHEAD   7
#define  SIZEOF_LONGBLOCKHEAD   11
#define  SIZEOF_COMMHEAD        13
#define  SIZEOF_PROTECTHEAD     26


#define  PACK_VER       20
#define  UNP_VER        20
#define  PROTECT_VER    20


enum { M_DENYREAD,M_DENYWRITE,M_DENYNONE,M_DENYALL };

enum { FILE_EMPTY,FILE_ADD,FILE_UPDATE,FILE_COPYOLD,FILE_COPYBLOCK };

enum { ARG_RESET,ARG_NEXT,ARG_REMOVE };

enum { ARC=1,VOL,SFX };

enum { SUCCESS,WARNING,FATAL_ERROR,CRC_ERROR,LOCK_ERROR,WRITE_ERROR,
       OPEN_ERROR,USER_ERROR,MEMORY_ERROR,USER_BREAK=255,IMM_ABORT=0x8000 };

enum { EN_LOCK=1,EN_VOL=2 };

enum { SD_MEMORY=1,SD_FILES=2 };

enum { ASK_OVERWR=0,ALL_OVERWR=1,NO_OVERWR=2 };

enum { ALARM_SOUND,ERROR_SOUND };

enum { MSG_STDOUT,MSG_STDERR,MSG_NULL };

enum { COMPARE_PATH,NOT_COMPARE_PATH };

enum { NAMES_DONTCHANGE,NAMES_UPPERCASE,NAMES_LOWERCASE };

enum { LOG_ARC=1,LOG_FILE=2 };

enum { CRC16=1,CRC32=2 };

enum { OLD_DECODE=0,OLD_ENCODE=1,NEW_CRYPT=2 };

enum { OLD_UNPACK,NEW_UNPACK };

enum { KEEP_TIME=1,LATEST_TIME };

#define  MHD_MULT_VOL       1
#define  MHD_COMMENT        2
#define  MHD_LOCK           4
#define  MHD_SOLID          8
#define  MHD_PACK_COMMENT   16
#define  MHD_NEWNUMBERING   16
#define  MHD_AV             32
#define  MHD_PROTECT        64

#define  LHD_SPLIT_BEFORE   1
#define  LHD_SPLIT_AFTER    2
#define  LHD_PASSWORD       4
#define  LHD_COMMENT        8
#define  LHD_SOLID          16

#define  LHD_WINDOWMASK     0x00e0
#define  LHD_WINDOW64       0
#define  LHD_WINDOW128      32
#define  LHD_WINDOW256      64
#define  LHD_WINDOW512      96
#define  LHD_WINDOW1024     128
#define  LHD_DIRECTORY      0x00e0

#define  SKIP_IF_UNKNOWN    0x4000
#define  LONG_BLOCK         0x8000

#define READSUBBLOCK	0x8000

enum { ALL_HEAD=0,MARK_HEAD=0x72,MAIN_HEAD=0x73,FILE_HEAD=0x74,
       COMM_HEAD=0x75,AV_HEAD=0x76,SUB_HEAD=0x77,PROTECT_HEAD=0x78};

enum { EA_HEAD=0x100 };

enum { MS_DOS=0,OS2=1,WIN_32=2,UNIX=3 };

enum { OLD=1,NEW=2};

enum { EEMPTY=-1,EBREAK=1,EWRITE,EREAD,EOPEN,ECREAT,ECLOSE,ESEEK,
       EMEMORY,EARCH };


struct MarkHeader
{
  UBYTE Mark[7];
};


struct OldMainHeader
{
  UBYTE Mark[4];
  UWORD HeadSize;
  UBYTE Flags;
};


struct NewMainArchiveHeader
{
  UWORD HeadCRC;
  UBYTE HeadType;
  UWORD Flags;
  UWORD HeadSize;
  UWORD Reserved;
  UDWORD Reserved1;
};


struct OldFileHeader
{
  UDWORD PackSize;
  UDWORD UnpSize;
  UWORD FileCRC;
  UWORD HeadSize;
  UDWORD FileTime;
  UBYTE FileAttr;
  UBYTE Flags;
  UBYTE UnpVer;
  UBYTE NameSize;
  UBYTE Method;
};


struct NewFileHeader
{
  UWORD HeadCRC;
  UBYTE HeadType;
  UWORD Flags;
  UWORD HeadSize;
  UDWORD PackSize;
  UDWORD UnpSize;
  UBYTE HostOS;
  UDWORD FileCRC;
  UDWORD FileTime;
  UBYTE UnpVer;
  UBYTE Method;
  UWORD NameSize;
  UDWORD FileAttr;
};


struct BlockHeader
{
  UWORD HeadCRC;
  UBYTE HeadType;
  UWORD Flags;
  UWORD HeadSize;
  UDWORD DataSize;
};


struct SubBlockHeader
{
  UWORD HeadCRC;
  UBYTE HeadType;
  UWORD Flags;
  UWORD HeadSize;
  UDWORD DataSize;
  UWORD SubType;
  UBYTE Level;
};


struct CommentHeader
{
  UWORD HeadCRC;
  UBYTE HeadType;
  UWORD Flags;
  UWORD HeadSize;
  UWORD UnpSize;
  UBYTE UnpVer;
  UBYTE Method;
  UWORD CommCRC;
};


struct ProtectHeader
{
  UWORD HeadCRC;
  UBYTE HeadType;
  UWORD Flags;
  UWORD HeadSize;
  UDWORD DataSize;
  UBYTE Version;
  UWORD RecSectors;
  UDWORD TotalBlocks;
  UBYTE Mark[8];
};


struct RAROptions
{
  int Overwrite;
  int Sound;
  int MsgStream;
  int DisableComment;
  int FreshFiles;
  int UpdateFiles;
  int Recurse;
  int PackVolume;
  int AllYes;
  int ConvertNames;
  int KeepBroken;
};


struct FileStat
{
  UDWORD FileAttr;
  UDWORD FileTime;
  UDWORD FileSize;
  int IsDir;
};


int ReadHeader(int BlockType);
int GetPassword(int AskCount);
void GetPswStr(char *Str);
void SplitCommandLine(int Argc,char *Argv[]);
void ExecuteCommand(void);
long GetFreeDisk(int NumDisk);
void MakeTmpPath(void);
void InitSomeVars(void);
void AddArgsFromFile(char *Name);
void Ask(char *AskStr);
void ShowUnpWrite(void);
void OutHelp(void);
void AddToArchive(void);
int CmpName(char *Wildcard,char *Name,int CmpPath);
int CmpNameEx(char *pattern,char *string);
void ListArchive(void);
void ExtrFile(void);
void ShutDown(int Mode);
void ErrExit(int ErrCode,int Code);
void ProcessSignal(int SigType);
void CreatePath(char *fpath);
int FileExist(char *FileName);
int twrite(FILE *stream,void *buf,unsigned len);
int tread(FILE *stream,void *buf,unsigned len);
int tseek(FILE *stream,long offset,int fromwhere);
long filelen(FILE *FPtr);
void SetOpenFileStat(FILE *FPtr);
void SetCloseFileStat(char *Name);
void SetOpenFileTime(FILE *FPtr,UDWORD ft);
void SetCloseFileTime(char *Name,UDWORD ft);
UDWORD GetOpenFileTime(FILE *FPtr);
FILE* topen(char *Name,char *Mode,unsigned int Sharing);
FILE* ShareOpen(char *Name,char *Mode,unsigned int Sharing);
void tclose(FILE *FPtr);
FILE* wopen(char *Name,char *Mode,unsigned int Sharing);
void NextVolumeName(int NewNumbering);
int MergeArchive(int ShowFileName);
void UnstoreFile(void);
void ConvertPath(char *OutPath, char *InPath);
void ViewComment(void);
void ViewFileComment(void);
int IsArchive(void);
int GetArchives(int Mode);
int ToPercent(long N1,long N2);
void AddExclArgs(char *Name);
int ExclCheck(char *Name);
int ReadBlock(int BlockType);
void GetArcNames(void);
void FindArchives(void);
int ReadArcName(void);
void ConvertDate(UDWORD ft);
void SetOptions(char *OptStr,int MultipleSwitches);
int GetKey(void);
int ToUpperCase(int Ch);
int IsProcessFile(int ComparePath);
char * PointToName(char *Path);
FILE* FileCreate(char *Name,int OverwriteMode,int *UserReject);
void WriteBlock(int BlockType);
unsigned int UnpRead(unsigned char *Addr,unsigned int Count);
unsigned int UnpWrite(unsigned char *Addr,unsigned int Count);
void FreeMemory(void);
void InitAddArgName(char *ArgMemAddr,long ArgMemSize);
int NextArgName(int Mode);
void AddArgName(char *Arg);
void AskNextVol(void);
int IsAllArgsUsed(void);
void SetExt(char *Name,char *NewExt);
void ShowComment(UBYTE *Addr,int Size);
int KbdAnsi(UBYTE *Addr,int Size);
int IsRemovable(int NumDrive);
void tunpack(unsigned char *Mem,int Solid,int Mode);

void Unpack(unsigned char *UnpAddr,int Solid);
void ShowAnsiComment(UBYTE *Addr,unsigned int Size);
int MakeDir(char *Name,UDWORD Attr);
void MakeSound(int MSec,int Freq);
int DateFormat(void);

void SetCryptKeys(char *Password);
void EncryptBlock(unsigned char *Buf);
void DecryptBlock(unsigned char *Buf);
void Crypt(UBYTE *Data,UDWORD Count,int Method);
void InitCRC(void);
UDWORD CRC(UDWORD StartCRC,void *Addr,UDWORD Size, int Mode);

char* strlower(char *Str);
char* strupper(char *Str);
int stricomp(char *Str1,char *Str2);
int strnicomp(char *Str1,char *Str2,int N);
int SplitPath(char *FullName,char *Path,char *Name,int RemoveDrive);
int GetPathDisk(char *Path);
int GetCurDisk(void);
void ShowAttr(void);
int IsDir(UDWORD Attr);
int IsLabel(UDWORD Attr);
void ConvertFlags(void);
DIR * DirOpen(char *Name);
int IsPathDiv(int Ch);
void ConvertUnknownHeader(void);
char* RemoveEOL(char *Str);
char* RemoveLF(char *Str);
void mprintf(char *fmt,...);
int CheckForDevice(FILE *CheckFile);

