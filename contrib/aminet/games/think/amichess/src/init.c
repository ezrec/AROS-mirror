#include <clib/alib_protos.h>
#include <dos/dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "common.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

#define NBITS 16

void Initialize()
{
InitLzArray();
InitBitPosArray();
InitMoveArray();
InitRay();
InitFromToRay();
InitRankFileBit();
InitPassedPawnMask();
InitIsolaniMask();
InitSquarePawnMask();
InitBitCount();
InitRotAtak();
InitRandomMasks();
InitDistance();
InitVars();
InitHashCode();
InitHashTable();
CalcHashKey();
}

void InitLzArray()
{
int i,j,s,n;
s=n=1;
for(i=0;i<NBITS;i++)
	{
	for(j=s;j<s+n;j++) lzArray[j]=NBITS-1-i;
	s+=n;
	n+=n;
	}
}

void InitBitPosArray()
{
BitBoard b=1;
int i;
for(i=63;i>=0;i--)
	{
	BitPosArray[i]=b;
	NotBitPosArray[i]=~b;
	b<<=1;
	}
}

static const int dir[8][8]=
{
{0,0,0,0,0,0,0,0},
{9,11,0,0,0,0,0,0},
{-21,-19,-12,-8,8,12,19,21},
{-11,-9,9,11,0,0,0,0},
{-10,-1,1,10,0,0,0,0},
{-11,-10,-9,-1,1,9,10,11},
{-11,-10,-9,-1,1,9,10,11},
{-9,-11,0,0,0,0,0,0}
};

static const int ndir[8]={0,2,8,4,4,8,8,2};

static const int map[120]=
{
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,0,1,2,3,4,5,6,7,-1,
-1,8,9,10,11,12,13,14,15,-1,
-1,16,17,18,19,20,21,22,23,-1,
-1,24,25,26,27,28,29,30,31,-1,
-1,32,33,34,35,36,37,38,39,-1,
-1,40,41,42,43,44,45,46,47,-1,
-1,48,49,50,51,52,53,54,55,-1,
-1,56,57,58,59,60,61,62,63,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 
};

void InitMoveArray()
{
int piece,fsq,tsq,f,t,n;
BitBoard *b;
for(piece=pawn;piece<=bpawn;piece++)
	{
	for(fsq=0;fsq<120;fsq++)
		{
		if((f=map[fsq])==-1) continue;
		b=&MoveArray[piece][f];
		*b=NULLBITBOARD;
		for(n=0;n<ndir[piece];n++)
			{
			tsq=fsq;
			do
				{
				tsq+=dir[piece][n];
				if((t=map[tsq])!=-1) SETBIT(*b,t);
				}
			while(range[piece]&&t!=-1);
			}
		}
	}
}

void InitRay()
{
int piece,fsq,tsq,f,t,n,ray;
BitBoard *b;
memset(directions,-1,sizeof(directions));
for(fsq=0;fsq<120;fsq++)
	{
	if((f=map[fsq])==-1) continue;
	ray=-1;
	for(piece=bishop;piece<=rook;piece++)
		{
		for(n=0;n<ndir[piece];n++)
			{
			b=&Ray[f][++ray];
			*b=NULLBITBOARD;
			tsq=fsq;
			do
				{
				tsq+=dir[piece][n];
				if((t=map[tsq])!=-1)
					{
					SETBIT(*b,t);
					directions[f][t]=ray;
					}
				}
			while(t!=-1);
			}
		}
	}
}

void InitFromToRay()
{
int piece,fsq,tsq,f,t,n;
BitBoard *b;
memset(FromToRay,0,sizeof(FromToRay));
for(piece=bishop;piece<=rook;piece++)
	{
	for(fsq=0;fsq<120;fsq++)
		{
		if((f=map[fsq])==-1) continue;
		for(n=0;n<ndir[piece];n++)
			{
			tsq=fsq;
			t=map[tsq];
			do
				{
				b=&FromToRay[f][t];
				tsq+=dir[piece][n];
				if((t=map[tsq])!=-1)
					{
					SETBIT(FromToRay[f][t],t);
					FromToRay[f][t]|=*b;
					}
				}
			while(t!=-1);
			}
		}
	}
}

void InitRankFileBit()
{
BitBoard b=255;
int i=8;
while(i--)
	{
	RankBit[i]=b;
	b<<=8;
	}
i=8;
b=0x0101010101010101ULL;
while(i--)
	{
	FileBit[i]=b;
	b<<=1;
	}
}

void InitRandomMasks()
{
mask_kr_trapped_w[0]=BitPosArray[H2];
mask_kr_trapped_w[1]=BitPosArray[H1]|BitPosArray[H2];
mask_kr_trapped_w[2]=BitPosArray[G1]|BitPosArray[H1]|BitPosArray[H2];
mask_qr_trapped_w[0]=BitPosArray[A2];
mask_qr_trapped_w[1]=BitPosArray[A1]|BitPosArray[A2];
mask_qr_trapped_w[2]=BitPosArray[A1]|BitPosArray[B1]|BitPosArray[A2];
mask_kr_trapped_b[0]=BitPosArray[H7];
mask_kr_trapped_b[1]=BitPosArray[H8]|BitPosArray[H7];
mask_kr_trapped_b[2]=BitPosArray[H8]|BitPosArray[G8]|BitPosArray[H7];
mask_qr_trapped_b[0]=BitPosArray[A7];
mask_qr_trapped_b[1]=BitPosArray[A8]|BitPosArray[A7];
mask_qr_trapped_b[2]=BitPosArray[A8]|BitPosArray[B8]|BitPosArray[A7];
}

void InitPassedPawnMask()
{
unsigned int sq;
BitBoard *b;
memset(PassedPawnMask,0,sizeof(PassedPawnMask));
for(sq=0;sq<64;sq++)
	{
	b=&PassedPawnMask[white][sq];
	*b=Ray[sq][7];
	if(ROW(sq))	*b|=Ray[sq-1][7];
	if(ROW(sq)!=7)	*b|=Ray[sq+1][7];
	}
for(sq=0;sq<64;sq++)
	{
	b=&PassedPawnMask[black][sq];
	*b=Ray[sq][4];
	if(ROW(sq)) *b|=Ray[sq-1][4];
	if(ROW(sq)!=7) *b|=Ray[sq+1][4];
	}
}

void InitIsolaniMask()
{
int i;
IsolaniMask[0]=FileBit[1];
IsolaniMask[7]=FileBit[6];
for(i=1;i<=6;i++) IsolaniMask[i]=FileBit[i-1]|FileBit[i+1];
}

void InitSquarePawnMask()
{
unsigned int sq;
int len,i,j;
memset(SquarePawnMask,0,sizeof(PassedPawnMask));
for(sq=0;sq<64;sq++)
	{
	len=7-RANK(sq);
	i=MAX(sq&56,sq-len);
	j=MIN(sq|7,sq+len);
	while(i<=j)
		{
		SquarePawnMask[white][sq]|=(BitPosArray[i]|FromToRay[i][i|56]);
		i++;
		}
	len=RANK(sq);
	i=MAX(sq&56,sq-len);
	j=MIN(sq|7,sq+len);
	while(i<=j)
		{
		SquarePawnMask[black][sq]|=(BitPosArray[i]|FromToRay[i][i&7]);
		i++;
		}
	}
for(sq=A2;sq<=H2;sq++) SquarePawnMask[white][sq]=SquarePawnMask[white][sq+8];
for(sq=A7;sq<=H7;sq++) SquarePawnMask[black][sq]=SquarePawnMask[black][sq-8];
}

void InitBitCount()
{
int i,j,n;
BitCount[0]=0;
BitCount[1]=1;
i=1;
for(n=2;n<=16;n++)
	{
	i<<=1;
	for(j=i;j<=i+(i-1);j++) BitCount[j]=1+BitCount[j-i];
	}
} 

void InitRotAtak()
{
int sq,map,sq1,sq2;
int cmap[8]={128,64,32,16,8,4,2,1};
int rot1[8]={A1,A2,A3,A4,A5,A6,A7,A8};
int rot2[8]={A1,B2,C3,D4,E5,F6,G7,H8};
int rot3[8]={A8,B7,C6,D5,E4,F3,G2,H1};
for(sq=A1;sq<=H1;sq++)
	{
	for(map=0;map<256;map++)
		{
		Rook00Atak[sq][map]=0;
		Rook90Atak[rot1[sq]][map]=0;
		Bishop45Atak[rot2[sq]][map]=0;
		Bishop315Atak[rot3[sq]][map]=0;
		sq1=sq2=sq;
		while(sq1>0)
			{
			if(cmap[--sq1]&map) break;
			}
		while(sq2<7)
			{
			if(cmap[++sq2]&map) break;
			}
		Rook00Atak[sq][map]=FromToRay[sq][sq1]|FromToRay[sq][sq2];
		Rook90Atak[rot1[sq]][map]=FromToRay[rot1[sq]][rot1[sq1]]|FromToRay[rot1[sq]][rot1[sq2]];
		Bishop45Atak[rot2[sq]][map]=FromToRay[rot2[sq]][rot2[sq1]]|FromToRay[rot2[sq]][rot2[sq2]];
		Bishop315Atak[rot3[sq]][map]=FromToRay[rot3[sq]][rot3[sq1]]|FromToRay[rot3[sq]][rot3[sq2]];
		}
	} 
for(map=0;map<256;map++)
	{
	for(sq=A2;sq<=H8;sq++)
		{
		Rook00Atak[sq][map]=Rook00Atak[sq-8][map]>>8;
		}
	for(sq1=B_FILE;sq1<=H_FILE;sq1++)
		{
		for(sq2=0;sq2<64;sq2+=8)
			{
			sq=sq2+sq1;
			Rook90Atak[sq][map]=Rook90Atak[sq-1][map]>>1;
			}
		}
	for(sq1=B1,sq2=H7;sq1<=H1;sq1++,sq2-=8)
		{
		for(sq=sq1;sq<=sq2;sq+=9)
			{
			Bishop45Atak[sq][map]=Bishop45Atak[sq+8][map]<<8;
			}
		}
	for(sq1=A2,sq2=G8;sq1<=A8;sq1+=8,sq2--)
		{
		for(sq=sq1;sq<=sq2;sq+=9)
			{
			Bishop45Atak[sq][map]=(Bishop45Atak[sq+1][map]&NotBitPosArray[sq1-8])<<1;
			}
		}
	for(sq1=H2,sq2=B8;sq1<=H8;sq1+=8,sq2++)
		{
		for(sq=sq1;sq<=sq2;sq+=7)
			{
			Bishop315Atak[sq][map]=Bishop315Atak[sq-8][map]>>8;
			}
		}
	for(sq1=G1,sq2=A7;sq1>=A1;sq1--,sq2-=8)
		{
		for(sq=sq1;sq<=sq2;sq+=7)
			{
			Bishop315Atak[sq][map]=(Bishop315Atak[sq+1][map]&NotBitPosArray[sq2+8])<<1;
			}
		}
	}
}


void InitDistance()
{
int f,t,j,d1,d2;
for(f=0;f<64;f++) for(t=0;t<8;t++) DistMap[f][t]=0;
for(f=0;f<64;f++) for(t=f;t<64;t++)
	{
	d1=(t&0x07)-(f&0x07);
	if(d1<0) d1=-d1;
	d2=(t>>3)-(f>>3);
	if(d2<0) d2=-d2;
	distance[f][t]=MAX(d1,d2);
	distance[t][f]=MAX(d1,d2);
	taxicab[f][t]=d1+d2;
	taxicab[t][f]=d1+d2;
	}
for(f=0;f<64;f++) for(t=0;t<64;t++) DistMap[f][distance[t][f]]|=BitPosArray[t];
for(f=0;f<64;f++) for(t=0;t<8;t++) for(j=0;j<t;j++) DistMap[f][t]|=DistMap[f][j];
}


void InitVars()
{
int i;
memset(&board,0,sizeof(board));
for(i=8;i<16;i++) SETBIT(board.b[white][pawn],i);
SETBIT(board.b[white][rook],0);
SETBIT(board.b[white][knight],1);
SETBIT(board.b[white][bishop],2);
SETBIT(board.b[white][queen],3);
SETBIT(board.b[white][king],4);
SETBIT(board.b[white][bishop],5);
SETBIT(board.b[white][knight],6);
SETBIT(board.b[white][rook],7);
for(i=48;i<56;i++) SETBIT(board.b[black][pawn],i);
SETBIT(board.b[black][rook],56);
SETBIT(board.b[black][knight],57);
SETBIT(board.b[black][bishop],58);
SETBIT(board.b[black][queen],59);
SETBIT(board.b[black][king],60);
SETBIT(board.b[black][bishop],61);
SETBIT(board.b[black][knight],62);
SETBIT(board.b[black][rook],63);

SETBIT(stonewall[white],D4);
SETBIT(stonewall[white],E3);
SETBIT(stonewall[white],F4);
SETBIT(stonewall[black],D5);
SETBIT(stonewall[black],E6);
SETBIT(stonewall[black],F5);

SETBIT(rings[0],D4);
SETBIT(rings[0],D5);
SETBIT(rings[0],E4);
SETBIT(rings[0],E5);
SETBIT(rings[1],C3);
SETBIT(rings[1],D3);
SETBIT(rings[1],E3);
SETBIT(rings[1],F3);
SETBIT(rings[1],C4);
SETBIT(rings[1],F4);
SETBIT(rings[1],C5);
SETBIT(rings[1],F5);
SETBIT(rings[1],C6);
SETBIT(rings[1],D6);
SETBIT(rings[1],E6);
SETBIT(rings[1],F6);
SETBIT(rings[2],B2);
SETBIT(rings[2],C2);
SETBIT(rings[2],D2);
SETBIT(rings[2],E2);
SETBIT(rings[2],F2);
SETBIT(rings[2],G2);
SETBIT(rings[2],B3);
SETBIT(rings[2],G3);
SETBIT(rings[2],B4);
SETBIT(rings[2],G4);
SETBIT(rings[2],B5);
SETBIT(rings[2],G5);
SETBIT(rings[2],B6);
SETBIT(rings[2],G6);
SETBIT(rings[2],B7);
SETBIT(rings[2],C7);
SETBIT(rings[2],D7);
SETBIT(rings[2],E7);
SETBIT(rings[2],F7);
SETBIT(rings[2],G7);
SETBIT(rings[3],A1);
SETBIT(rings[3],B1);
SETBIT(rings[3],C1);
SETBIT(rings[3],D1);
SETBIT(rings[3],E1);
SETBIT(rings[3],F1);
SETBIT(rings[3],G1);
SETBIT(rings[3],H1);
SETBIT(rings[3],A2);
SETBIT(rings[3],H2);
SETBIT(rings[3],A3);
SETBIT(rings[3],H3);
SETBIT(rings[3],A4);
SETBIT(rings[3],H4);
SETBIT(rings[3],A5);
SETBIT(rings[3],H5);
SETBIT(rings[3],A6);
SETBIT(rings[3],H6);
SETBIT(rings[3],A7);
SETBIT(rings[3],H7);
SETBIT(rings[3],A8);
SETBIT(rings[3],B8);
SETBIT(rings[3],C8);
SETBIT(rings[3],D8);
SETBIT(rings[3],E8);
SETBIT(rings[3],F8);
SETBIT(rings[3],G8);
SETBIT(rings[3],H8);

boardhalf[white]=RankBit[0]|RankBit[1]|RankBit[2]|RankBit[3];
boardhalf[black]=RankBit[4]|RankBit[5]|RankBit[6]|RankBit[7];
boardside[ks]=FileBit[4]|FileBit[5]|FileBit[6]|FileBit[7];
boardside[qs]=FileBit[0]|FileBit[1]|FileBit[2]|FileBit[3];
board.flag|=(WCASTLE|BCASTLE);
board.side=white;
DoMethod(mui_app,MUIM_Chess_Side);
board.ep=-1;
board.king[white]=E1;
board.king[black]=E8;
GameCnt=-1;
Game50=0;
computer=black;
CalcHashKey();
Game[0].hashkey=HashKey;
board.pmaterial[white]=board.pmaterial[black]=2*ValueR+2*ValueN+2*ValueB+ValueQ;
board.material[white]=board.material[black]=board.pmaterial[white]+8*ValueP;
UpdateFriends();
UpdateCBoard();
UpdateMvboard();
for(i=A1;i<=H8;i++)
	{
	if(cboard[i])
		{
		SETBIT(board.blockerr90,r90[i]);
		SETBIT(board.blockerr45,r45[i]);
		SETBIT(board.blockerr315,r315[i]);
		}
	}

TreePtr[0]=TreePtr[1]=Tree;
SET(flags,USEHASH);
// SET(flags,USENULL);
// SearchTime=5;
// SearchDepth=0;
MoveLimit[white]=MoveLimit[black]=TCMove;
TimeLimit[white]=TimeLimit[black]=TCTime*60;
board.castled[white]=board.castled[black]=false;
phase=PHASE;
i=HASHSLOTS;
TTHashMask=0;
while((i>>=1)>0)
	{
	TTHashMask<<=1;
	TTHashMask|=1;
	}
HashSize=TTHashMask+1;
i=PAWNSLOTS;
PHashMask=0;
while((i>>=1)>0)
	{
	PHashMask<<=1;
	PHashMask|=1;
	}
DebugPly=99;
DebugDepth=0;
DebugNode=999999999;
// signal(SIGINT,EndSearch);
nmovesfrombook=0;
}

void InitHashCode()
{
int color,piece,sq;
for(color=white;color<=black;color++)
	{
	for(piece=pawn;piece<=king;piece++)
		{
		for(sq=0;sq<64;sq++) hashcode[color][piece][sq]=Rand64();
		}
	}
for(sq=0;sq<64;sq++) ephash[sq]=Rand64();
WKCastlehash=Rand64();
WQCastlehash=Rand64();
BKCastlehash=Rand64();
BQCastlehash=Rand64();
Sidehash=Rand64();
}

void InitHashTable()
{
for(;;)
	{
#ifdef __cplusplus
	delete [] HashTab[0];
	delete [] HashTab[1];
	HashTab[0]=new HashSlot[HashSize];
	HashTab[1]=new HashSlot[HashSize];
#else
	free(HashTab[0]);
	free(HashTab[1]);
	HashTab[0]=malloc(HashSize*sizeof(HashSlot));
	HashTab[1]=malloc(HashSize*sizeof(HashSlot));
#endif
	if(HashTab[0]&&HashTab[1]) break;
	HashSize>>=1;
	TTHashMask>>=1;
	if(!HashSize)
#if defined(__PPC__) && !defined(__AROS__)
	FromPPCexit(RETURN_FAIL);
#else
	exit(RETURN_FAIL);
#endif
	}
#ifdef __cplusplus
delete [] PawnTab[0];
delete [] PawnTab[1];
PawnTab[0]=new PawnSlot[PAWNSLOTS];
PawnTab[1]=new PawnSlot[PAWNSLOTS];
#else
PawnTab[0]=realloc(PawnTab[0],PAWNSLOTS*sizeof(PawnSlot));
PawnTab[1]=realloc(PawnTab[1],PAWNSLOTS*sizeof(PawnSlot));
#endif
if(!PawnTab[0]||!PawnTab[1])
#if defined(__PPC__) && !defined(__AROS__)
	FromPPCexit(RETURN_FAIL);
#else
	exit(RETURN_FAIL);
#endif
}

void NewPosition()
{
CLEAR(flags,ENDED);
Game50=0;
GameCnt=-1;
Game[0].hashkey=HashKey;
TTClear();
PTClear();
nmovesfrombook=0;
ExchCnt[white]=ExchCnt[black]=0;
}

