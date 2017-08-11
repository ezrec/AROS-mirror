#ifndef COMMON_H
#define COMMON_H

#include "config.h"

#ifdef HAVE64BIT

typedef unsigned long long BitBoard;
typedef unsigned long long HashType;

#else

struct BitBoard
{
unsigned int hi,lo;
BitBoard(const BitBoard &a)
{
lo=a.lo;
hi=a.hi;
}

BitBoard(unsigned long long);
BitBoard(int h,int l)
{
lo=l;
hi=h;
}

BitBoard(int a=0)
{
lo=a;
hi=0;
}

BitBoard(unsigned int a)
{
lo=a;
hi=0;
}

BitBoard(unsigned long a)
{
lo=a;
hi=0;
}

BitBoard operator<<(int) const;
BitBoard operator>>(int) const;

BitBoard operator&(int a) const
{
return BitBoard(lo&a);
}

BitBoard operator&(unsigned long a) const
{
return BitBoard(lo&a);
}

BitBoard operator&(const BitBoard &a) const
{
return BitBoard(hi&a.hi,lo&a.lo);
}

BitBoard operator|(const BitBoard &a) const
{
return BitBoard(hi|a.hi,lo|a.lo);
}

BitBoard operator^(const BitBoard &a) const
{
return BitBoard(hi^a.hi,lo^a.lo);
}

BitBoard operator~() const
{
return BitBoard(~hi,~lo);
}

BitBoard &operator=(const BitBoard &a)
{
lo=a.lo;
hi=a.hi;
return *this;
}

BitBoard &operator&=(const BitBoard &a)
{
lo&=a.lo;
hi&=a.hi;
return *this;
}

BitBoard &operator|=(int a)
{
lo|=a;
return *this;
}

BitBoard &operator|=(const BitBoard &a)
{
lo|=a.lo;
hi|=a.hi;
return *this;
}

BitBoard &operator^=(const BitBoard &a)
{
lo^=a.lo;
hi^=a.hi;
return *this;
}

BitBoard &operator<<=(int);

int operator!=(int a) const
{
return hi!=0||lo!=a;
}

int operator!=(const BitBoard &a) const
{
return hi!=a.hi||lo!=a.lo;
}

int operator==(int a) const
{
return hi==0&&lo==a;
}

int operator==(const BitBoard &a) const
{
return hi==a.hi&&lo==a.lo;
}

operator int() const
{
return lo|hi;
}

};

typedef BitBoard HashType;

#endif

typedef unsigned long KeyType;

typedef struct 
{
BitBoard b[2][7];
BitBoard friends[2];
BitBoard blocker;
BitBoard blockerr90;
BitBoard blockerr45;
BitBoard blockerr315;
int ep;
int flag;
int side;
int material[2];
int pmaterial[2];
int castled[2];
int king[2];
} Board; 

typedef struct
{
int move;
int score; 
} leaf;

typedef struct
{
int move;
int epsq; 
int bflag;
int Game50;
int mvboard;
float et;
HashType hashkey;
HashType phashkey;
char SANmv[8];
} GameRec;

typedef struct
{
KeyType key; 
int move;
int score;
int flag;
int depth;
} HashSlot; 

typedef struct
{
KeyType pkey; 
BitBoard passed;
BitBoard weaked;
int score;
int phase;
} PawnSlot;


/* MACRO definitions */

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define SETFLAG(a,b) (a|=b)
#define CLEARFLAG(a,b) (a&=~b)
#define DRAWSCORE (computerplays==board.side?(opprating-myrating)/4:(myrating-opprating)/4)
#define MATERIAL (board.material[side]-board.material[1^side])
#define PHASE (8-(board.material[white]+board.material[black])/1150)

#ifdef HAVE64BIT
#define KEY(a) (a>>32) 
#else
#define KEY(a) (a.hi)
#endif

/* Attack MACROS */

#define BishopAttack(sq) (Bishop45Atak[sq][(board.blockerr45>>Shift45[sq])&Mask45[sq]]|Bishop315Atak[sq][(board.blockerr315>>Shift315[sq])&Mask315[sq]])
#define RookAttack(sq) (Rook00Atak[sq][(board.blocker>>Shift00[sq])&0xFF]|Rook90Atak[sq][(board.blockerr90>>Shift90[sq])&0xFF])
#define QueenAttack(sq) (BishopAttack(sq)|RookAttack(sq))

/* Some bit macros */

/*
#if defined(__GNUC__)&&defined(__PPC__)&&__GNUC__<3
#define SETBIT(b,i) ((b)|=((1ULL<<63)>>(i)))
#define CLEARBIT(b,i) ((b)&=~((1ULL<<63)>>(i)))

#define SETBIT(b,i) ((b)|=BitPosArray[i])
#define CLEARBIT(b,i) ((b)&=NotBitPosArray[i])
*/
#define SETBIT(b,i) ((b)|=BitPosArray[i])
#define CLEARBIT(b,i) ((b)&=NotBitPosArray[i])

#define RANK(i) ((i)>>3)
#define ROW(i) ((i)&7)
#define trailz(b) (leadz((b)&((~b)+1)))

#define PROMOTEPIECE(a) ((a>>12)&0x0007)
#define CAPTUREPIECE(a) ((a>>15)&0x0007)
#define TOSQ(a) ((a)&0x003F)
#define FROMSQ(a) ((a>>6)&0x003F)
#define MOVE(a,b) (((a)<<6)|(b))

#define white 0
#define black 1
#define false 0
#define true 1
#define ks 0
#define qs 1
#define INFINITY 32767
#define MATE 32767
#define MATESCORE(a)	((a)>MATE-255||(a)<-MATE+255)

/* constants for Board */
#define WKINGCASTLE 0x0001
#define WQUEENCASTLE 0x0002
#define BKINGCASTLE 0x0004
#define BQUEENCASTLE 0x0008
#define WCASTLE (WKINGCASTLE|WQUEENCASTLE)
#define BCASTLE (BKINGCASTLE|BQUEENCASTLE)

/* Material values */
#define ValueP 100	
#define ValueN 350
#define ValueB 350
#define ValueR 550
#define ValueQ 1100
#define ValueK 2000

/* constants for move description */
#define KNIGHTPRM 0x00002000
#define BISHOPPRM 0x00003000 
#define ROOKPRM 0x00004000
#define QUEENPRM 0x00005000
#define PROMOTION 0x00007000
#define PAWNCAP 0x00008000
#define KNIGHTCAP 0x00010000 
#define BISHOPCAP 0x00018000
#define ROOKCAP 0x00020000 
#define QUEENCAP 0x00028000 
#define CAPTURE 0x00038000 
#define NULLMOVE 0x00100000 
#define CASTLING 0x00200000 
#define ENPASSANT 0x00400000
#define MOVEMASK (CASTLING|ENPASSANT|PROMOTION|0x0FFF)

/* Some special BitBoards */
#define NULLBITBOARD ((BitBoard)0x0000000000000000ULL)
#define WHITESQUARES ((BitBoard)0x55AA55AA55AA55AAULL)
#define BLACKSQUARES ((BitBoard)0xAA55AA55AA55AA55ULL)
#define CENTRESQUARES ((BitBoard)0x0000001818000000ULL)
#define COMPUTERHALF ((BitBoard)0xFFFFFFFF00000000ULL)
#define OPPONENTHALF (8BitBoard)0x00000000FFFFFFFFULL)

/* Game flags */
#define QUIT 0x0001
#define TESTT 0x0002
#define THINK 0x0004
#define MANUAL 0x0008
#define TIMEOUT 0x0010
#define DEBUGG 0x0020
#define ENDED 0x0040
#define USEHASH 0x0080
#define SOLVE 0x0100
#define USENULL 0x0200
#define TIMECTL 0x0800
#define POST 0x1000
#define AUTOPLAY 0x2000
#define SUPERVISOR 0x4000
#define REVERSEBOARD 0x8000

/* Node types */ 
#define PV 0
#define ALL 1
#define CUT 2

/* Transposition table flags */
#define EXACTSCORE 1
#define LOWERBOUND 2
#define UPPERBOUND 3
#define POORDRAFT 4
#define QUIESCENT 5
#define STICKY 8

/* Book modes */
#define BOOKOFF 0
#define BOOKRAND 1
#define BOOKBEST 2
#define BOOKWORST 3
#define BOOKPREFER 4

/* The various phases during move selection */
#define PICKHASH 1
#define PICKGEN1 2
#define PICKCAPT 3
#define PICKKILL1 4
#define PICKKILL2 5
#define PICKGEN2 6
#define PICKHIST 7
#define PICKREST 8
#define PICKCOUNTER 9

#define MAXTREEDEPTH 2000
#define MAXPLYDEPTH 65
#define MAXGAMEDEPTH 600

/* 
Smaller HASHSLOT defaults 20011017 to improve blitz play
and make it easier to run on old machines
*/
#define HASHSLOTS 1024 
#define PAWNSLOTS 512

#define DEPTH	12

extern int distance[64][64];
extern int taxicab[64][64];
extern unsigned char lzArray[65536];
extern int Shift00[64];
extern int Shift90[64];
extern int Shift45[64];
extern int Shift315[64];
extern BitBoard DistMap[64][8];
extern BitBoard BitPosArray[64];
extern BitBoard NotBitPosArray[64];
extern BitBoard MoveArray[8][64];
extern BitBoard Ray[64][8];
extern BitBoard FromToRay[64][64];
extern BitBoard RankBit[8];
extern BitBoard FileBit[8];
extern BitBoard Ataks[2][7];
extern BitBoard PassedPawnMask[2][64];
extern BitBoard IsolaniMask[8];
extern BitBoard SquarePawnMask[2][64];
extern BitBoard Rook00Atak[64][256]; 
extern BitBoard Rook90Atak[64][256]; 
extern BitBoard Bishop45Atak[64][256];
extern BitBoard Bishop315Atak[64][256];
extern BitBoard pinned;
extern BitBoard rings[4];
extern BitBoard stonewall[2];
extern BitBoard pieces[2];
extern BitBoard mask_kr_trapped_w[3];
extern BitBoard mask_kr_trapped_b[3];
extern BitBoard mask_qr_trapped_w[3];
extern BitBoard mask_qr_trapped_b[3];
extern BitBoard boardhalf[2];
extern BitBoard boardside[2];
extern int directions[64][64];
extern int BitCount[65536];
extern leaf Tree[MAXTREEDEPTH];
extern leaf *TreePtr[MAXPLYDEPTH];
extern int RootPV;
extern GameRec Game[MAXGAMEDEPTH];
extern int GameCnt;
extern int computer;
extern unsigned int flags;
extern Board board;
extern int cboard[64];
extern int Mvboard[64];
extern HashType hashcode[2][7][64];
extern HashType ephash[64];
extern HashType WKCastlehash;
extern HashType WQCastlehash;
extern HashType BKCastlehash;
extern HashType BQCastlehash;
extern HashType Sidehash;
extern HashType HashKey;
extern HashType PawnHashKey;
extern HashSlot *HashTab[2];
extern PawnSlot *PawnTab[2];
extern int Idepth;
extern int SxDec;
extern int Game50;
extern int lazyscore[2];
extern int maxposnscore[2];
extern int rootscore;
extern int lastrootscore;
extern unsigned long GenCnt;
extern unsigned long NodeCnt;
extern unsigned long QuiesCnt;
extern unsigned long EvalCnt;
extern unsigned long EvalCall;
extern unsigned long ChkExtCnt;
extern unsigned long OneRepCnt;
extern unsigned long RcpExtCnt;
extern unsigned long PawnExtCnt;
extern unsigned long HorzExtCnt;
extern unsigned long ThrtExtCnt;
extern unsigned long KingExtCnt;
extern unsigned long NullCutCnt;
extern unsigned long FutlCutCnt;
extern unsigned long RazrCutCnt;
extern unsigned long TotalGetHashCnt;
extern unsigned long GoodGetHashCnt;
extern unsigned long TotalPutHashCnt;
extern unsigned long CollHashCnt;
extern unsigned long TotalPawnHashCnt;
extern unsigned long GoodPawnHashCnt;
extern unsigned long RepeatCnt;
extern unsigned HashSize;
extern unsigned long TTHashMask;
extern unsigned long PHashMask;
extern int slider[8];
extern int Value[7];
extern char SANmv[10];
extern unsigned long history[2][4096];
extern int killer1[MAXPLYDEPTH];
extern int killer2[MAXPLYDEPTH];
extern int ChkCnt[MAXPLYDEPTH];
extern int ThrtCnt[MAXPLYDEPTH];
extern char id[32];
extern char solution[64];
/*
* XXX: This variable et also exists as a local variable somewhere and
* to complete the confusion,this came up in three different
* flavours: long,float and double.
*/
extern double et;
extern float SearchTime;
extern int SearchDepth;
extern int MoveLimit[2];
extern float TimeLimit[2];
extern int TCMove;
extern int TCinc;
extern float TCTime;
extern int hunged[2];
extern int phase;
extern int Hashmv[MAXPLYDEPTH];
extern int DebugPly;
extern int DebugDepth;
extern long DebugNode;
extern int Debugmv[MAXPLYDEPTH];
extern int Debugmvl;
extern int Debugline;
extern int RootPieces;
extern int RootPawns;
extern int RootMaterial;
extern int RootAlpha;
extern int RootBeta;
extern int pickphase[MAXPLYDEPTH];
extern int InChk[MAXPLYDEPTH];
extern int KingThrt[2][MAXPLYDEPTH];
extern int threatmv;
extern int threatply;
extern int KingSafety[2];
extern int pscore[64];

extern int bookmode;
extern int bookfirstlast;

extern int range[8];
extern int ptype[2];
extern char algbr[64][3];
extern char algbrfile[9];
extern char algbrrank[9];
extern char notation[8];
extern char lnotation[8];
extern int r90[64];
extern int r45[64];
extern int r315[64];
extern int Mask45[64];
extern int Mask315[64];
extern int myrating,opprating,suddendeath;
extern char name[50];
extern int computerplays;
extern int wasbookmove;
extern int nmovesfrombook;
extern float maxtime;
extern int n; 		/* Last mobility returned by CTL */
extern int ExchCnt[2];
extern int newpos,existpos;		/* For book statistics */
extern int bookloaded;

enum Piece { empty,pawn,knight,bishop,rook,queen,king,bpawn };

enum Square {
sA1,sB1,sC1,sD1,sE1,sF1,sG1,sH1,
sA2,sB2,sC2,sD2,sE2,sF2,sG2,sH2,
sA3,sB3,sC3,sD3,sE3,sF3,sG3,sH3,
sA4,sB4,sC4,sD4,sE4,sF4,sG4,sH4,
sA5,sB5,sC5,sD5,sE5,sF5,sG5,sH5,
sA6,sB6,sC6,sD6,sE6,sF6,sG6,sH6,
sA7,sB7,sC7,sD7,sE7,sF7,sG7,sH7,
sA8,sB8,sC8,sD8,sE8,sF8,sG8,sH8
};

enum File { A_FILE,B_FILE,C_FILE,D_FILE,E_FILE,F_FILE,G_FILE,H_FILE };

/****************************************************************************
*
* The various function prototypes. They are group into the *.c files
* in which they are defined.
*
****************************************************************************/

unsigned char leadz(BitBoard);
unsigned char nbits(BitBoard);

/* The initialization routines */
void Initialize();
void InitLzArray();
void InitBitPosArray();
void InitMoveArray();
void InitRay();
void InitFromToRay();
void InitRankFileBit();
void InitBitCount();
void InitPassedPawnMask();
void InitIsolaniMask();
void InitSquarePawnMask();
void InitRandomMasks();
void InitRotAtak();
void InitDistance();
void InitVars();
void InitHashCode();
void InitHashTable();
void NewPosition();
void InitFICS();

/* The book routines */
void MakeBinBook(char *,int);
int BookQuery();
int BookBuilderOpen();
int BookBuilder(int result,int side);
int BookBuilderClose();

/*
* Return values(errorcodes) for the book routines,
* maybe one should have a global enum of errorcodes
*/
enum {
BOOK_SUCCESS,
BOOK_EFORMAT,/* Wrong format(e.g. produced by older version) */
BOOK_EMIDGAME,/* Move is past the opening book's move limit */ 
BOOK_EIO,/* I/O error,e.g. caused by wrong permissions */
BOOK_EFULL,/* Book hash is full,new position was not added. */
BOOK_ENOBOOK,/* No book present */
BOOK_ENOMOVES,/* No moves found(in BookQuery() only) */
BOOK_ENOMEM /* Memory allocation failed */
};

/* The move generation routines */
void GenMoves(int);
void GenCaptures(int);
void GenNonCaptures(int);
void GenCheckEscapes(int);
void FilterIllegalMoves(int);

/* The move routines */
void MakeMove(int,int *);
void UnmakeMove(int,int *);
void MakeNullMove(int);
void UnmakeNullMove(int);
void SANMove(int,int);
leaf *ValidateMove(char *);
leaf *IsInMoveList(int,int,int,char);
int IsLegalMove(int);
char *AlgbrMove(int);

/* The attack routines */
int SqAtakd(int,int);
void GenAtaks();
BitBoard AttackTo(int,int);
BitBoard AttackXTo(int,int);
BitBoard AttackFrom(int,int,int);
BitBoard AttackXFrom(int,int);
int PinnedOnKing(int,int);
void FindPins(BitBoard *);
int MateScan(int);

/* The swap routines */
int SwapOff(int);
void AddXrayPiece(int,int,int,BitBoard *,BitBoard *);

/* The EPD routines */
int ReadEPDFile(const char *,int);
int ParseEPD(char *);
void LoadEPD(char *);
void SaveEPD(char *);

/* Error codes for ParseEPD */
enum
{
EPD_SUCCESS,
EPD_ERROR
};            

/* The command routines */
void InputCmd(char *);
void ShowCmd(char *);
void BookCmd(char *);

/* Some utility routines */
unsigned char leadz(BitBoard);
unsigned char nbits(BitBoard);

void UpdateFriends();
void UpdateCBoard();
void UpdateMvboard();
void EndSearch(int);
int ValidateBoard();

/* PGN routines */
void PGNSaveToFile(const char *,const char *);
void PGNReadFromFile(const char *);
void BookPGNReadFromFile(const char *);

/* The hash routines */
void CalcHashKey();
void ShowHashKey(HashType);

/* The evaluation routines */
int ScoreP(int);
int ScoreN(int);
int ScoreB(int);
int ScoreR(int);
int ScoreQ(int);
int ScoreK(int);
int ScoreKBNK(int,int);
int ScoreDev(int);
int Evaluate(int,int);
int EvaluateDraw();

/* Hung routines */
int EvalHung(int);

/* The search routines */
void Iterate();
int Search(int,int,int,int,int);
int SearchRoot(int,int,int);
int Quiesce(int,int,int);
void pick(leaf *,int);
int Repeat();
void ShowLine(int,int,char);
void GetElapsed();

/* The transposition table routies */
void TTPut(int,int,int,int,int,int,int);
int TTGet(int,int,int,int *,int *);
int TTGetPV(int,int,int,int *);
void TTClear();
void PTClear();

/* Sorting routines */
void SortCaptures(int);
void SortMoves(int);
void SortRoot();
int PhasePick(leaf **,int);
int PhasePick1(leaf **,int);

/* Some output routines */
void ShowMoveList(int);
void ShowSmallBoard();
void ShowBoard();
void ShowBitBoard(BitBoard *);
void ShowCBoard();
void ShowMvboard();

void ShowGame();
void ShowTime();

/* Random numbers routines */
unsigned int Rand32();
HashType Rand64();

/* Solver routines */
void Solve(char *);

/* Miscellaneous routines */
void ShowHelp(const char *);

/* Player database */
void DBSortPlayer(const char *style);
void DBListPlayer(const char *style); 	
void DBReadPlayer();	
void DBWritePlayer();
int DBSearchPlayer(const char *player);
void DBUpdatePlayer(const char *player,const char *resultstr);
void DBTest();

#ifdef __AROS__
#include <intuition/classusr.h>
extern Object *mui_app;
#else
extern void *mui_app;
#endif

enum
{
MUIM_Chess_New=0x80660000,
MUIM_Chess_OpenEPD,
MUIM_Chess_OpenPGN,
MUIM_Chess_SaveEPD,
MUIM_Chess_SavePGN,
MUIM_Chess_Autoplay,
MUIM_Chess_SwapSides,
MUIM_Chess_SwitchSides,
MUIM_Chess_Hint,
MUIM_Chess_Undo,
MUIM_Chess_Remove,
MUIM_Chess_WinOpen,
MUIM_Chess_ShowBoard,
MUIM_Chess_Post,
MUIM_Chess_Depth,
MUIM_Chess_Time,
MUIM_Chess_ShowThinking,
MUIM_Chess_Supervisor,
MUIM_Chess_NullMove,
MUIM_Chess_BookAdd,
MUIM_Chess_BookOff,
MUIM_Chess_BookOn,
MUIM_Chess_BookBest,
MUIM_Chess_BookWorst,
MUIM_Chess_BookRandom,
MUIM_Chess_ReverseBoard,
MUIM_Chess_MyMove,
MUIM_Chess_Side,
MUIM_Chess_Board,
MUIM_Chess_Pieces,
MUIM_Chess_ClearFlag,
MUIM_Chess_AddMove,
MUIM_Chess_ClearList,
MUIM_Chess_Promote,
MUIM_Chess_Eval,
MUIM_Chess_Stats
};

struct MUIP_Chess_ShowThinking {STACKED unsigned long MethodID; STACKED char *line; };
struct MUIP_Chess_Post {STACKED unsigned long MethodID; STACKED int value; };
struct MUIP_Chess_NullMove {STACKED unsigned long MethodID; STACKED int value; };
struct MUIP_Chess_Supervisor {STACKED unsigned long MethodID; STACKED int value; };
struct MUIP_Chess_Depth {STACKED unsigned long MethodID; STACKED int depth; };
struct MUIP_Chess_Time { STACKED unsigned long MethodID; STACKED int flag; };
struct MUIP_Chess_ReverseBoard {STACKED unsigned long MethodID; STACKED int reverse; };
struct MUIP_Chess_Autoplay {STACKED unsigned long MethodID; STACKED int autoplay; };
struct MUIP_Chess_MyMove {STACKED unsigned long MethodID; STACKED char *move; };
struct MUIP_Chess_Board { STACKED unsigned long MethodID; STACKED void *menu; };
struct MUIP_Chess_Pieces {STACKED  unsigned long MethodID; STACKED void *menu; };
struct MUIP_Chess_ClearFlag { STACKED unsigned long MethodID; STACKED int flag; };
struct MUIP_Chess_AddMove { STACKED unsigned long MethodID; STACKED int side; STACKED char *move; };

#ifdef __MIXEDBINARY__

unsigned long FromPPCDoMethod(unsigned long,...);
void FromPPCexit(int);

#endif

#endif
