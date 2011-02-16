#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "eval.h"

BitBoard passed[2];
BitBoard weaked[2];

static int PawnSq[2][64]={
{
0,0,0,0,0,0,0,0,
5,5,5,-10,-10,5,5,5,
-2,-2,-2,6,6,-2,-2,-2,
0,0,0,25,25,0,0,0,
2,2,12,16,16,12,2,2,
4,8,12,16,16,12,4,4,
4,8,12,16,16,12,4,4,
0,0,0,0,0,0,0,0},
{
0,0,0,0,0,0,0,0,
4,8,12,16,16,12,4,4,
4,8,12,16,16,12,4,4,
2,2,12,16,16,12,2,2,
0,0,0,25,25,0,0,0,
-2,-2,-2,6,6,-2,-2,-2,
5,5,5,-10,-10,5,5,5,
0,0,0,0,0,0,0,0}
};

static const int Passed[2][8] ={ { 0,48,48,96,144,192,240,0},{0,240,192,144,96,48,48,0} };

static const int isolani_normal[8]={-8,-10,-12,-14,-14,-12,-10,-8};
static const int isolani_weaker[8]={-22,-24,-26,-28,-28,-26,-24,-22 };

static const int rank7[2]={ 6,1 };
static const int rank8[2]={ 7,0 };
static const BitBoard d2e2[2]={ 0x0018000000000000ULL,0x0000000000001800ULL };
static const BitBoard brank7[2]={ 0x000000000000FF00ULL,0x00FF000000000000ULL };
static const BitBoard brank8[2] ={ 0x00000000000000FFULL,0xFF00000000000000ULL };
static const BitBoard brank67[2]={ 0x0000000000FFFF00ULL,0x00FFFF0000000000ULL };
static const BitBoard brank58[2]={ 0x00000000FFFFFFFFULL,0xFFFFFFFF00000000ULL };

int ScoreKBNK(int side,int loser);

int ScoreP(int side)
{
int s,sq,i,i1,xside;
int n1,n2,backward;
int nfile[8];
int EnemyKing;
BitBoard c,t,p,blocker,*e;
PawnSlot *ptable;
if(board.b[side][pawn]==NULLBITBOARD) return 0;
xside=1^side;
EnemyKing=board.king[xside];
p=board.b[xside][pawn];
t=board.b[side][pawn];
c=t;
ptable=PawnTab[side];
ptable+=PawnHashKey&PHashMask;
TotalPawnHashCnt++;
if(ptable->phase==phase&&ptable->pkey==KEY(PawnHashKey))
	{
	GoodPawnHashCnt++;
	s=ptable->score;
	passed[side]=ptable->passed;
	weaked[side]=ptable->weaked;
	goto phase2; 
	}
s=0;
passed[side]=NULLBITBOARD;
weaked[side]=NULLBITBOARD;
memset(nfile,0,sizeof(nfile));
while(t)
	{
	sq=leadz(t);
	CLEARBIT(t,sq);
	s+=PawnSq[side][sq]; 
	if((p&PassedPawnMask[side][sq])==NULLBITBOARD)
		{
		if((side==white&&(FromToRay[sq][sq|56]&c)==0)||(side==black&&(FromToRay[sq][sq&7]&c)==0))
			{
			passed[side]|=BitPosArray[sq];
			if(side==white) s+=(Passed[side][RANK(sq)]*phase)/12;
			else if(side==black) s+=(Passed[side][RANK(sq)]*phase)/12;
			}
		}
	backward=false;
	if(side==white) i=sq+8;
	else i=sq-8;
	if(!(PassedPawnMask[xside][i]&~FileBit[ROW(sq)]&c)&&cboard[i]!=pawn)
		{
		n1=nbits(c&MoveArray[ptype[xside]][i]);
		n2=nbits(p&MoveArray[ptype[side]][i]);
		if(n1<n2) backward=true;
		}
	if(!backward&&(BitPosArray[sq]&brank7[xside]))
		{
		i1=1;
		i+=(side==white?8:-8);
		if(!(PassedPawnMask[xside][i]&~FileBit[ROW(i1)]&c))
			{
			n1=nbits(c&MoveArray[ptype[xside]][i]);
			n2=nbits(p&MoveArray[ptype[side]][i]);
			if(n1<n2) backward=true;
			}
		}
	if(backward)
		{
		weaked[side]|=BitPosArray[sq];
		s+=BACKWARDPAWN;
		}
	if((MoveArray[ptype[side]][sq]&p)&&(MoveArray[ptype[side]][sq]&c)) s+=PAWNBASEATAK;
	nfile[ROW(sq)]++;
	}
for(i=0;i<=7;i++)
	{
	if(nfile[i]>1) s+=DOUBLEDPAWN;
	if(nfile[i]&&(!(c&IsolaniMask[i])))
		{
		if(!(FileBit[i]&board.b[xside][pawn])) s+=isolani_weaker[i]*nfile[i];
		else s+=isolani_normal[i]*nfile[i];
		weaked[side]|=(c&FileBit[i]);
		}
	}
if(computerplays==side)
	{
	if(nbits(board.b[computerplays][pawn])==8) s+=EIGHT_PAWNS;
	if(nbits(stonewall[xside]&board.b[xside][pawn])==3) s+=STONEWALL;
	n=0;
	if(side==white) n=nbits((c>>8)&board.b[xside][pawn]&(rings[0]|rings[1]|rings[2]));
	else n=nbits((c<<8)&board.b[xside][pawn]&(rings[0]|rings[1]|rings[2]));
	if(n>1) s+=n*LOCKEDPAWNS;
	}
ptable->pkey=KEY(PawnHashKey);
ptable->passed=passed[side];
ptable->weaked=weaked[side];
ptable->score=s;
ptable->phase=phase;

phase2:
c=board.b[side][pawn];
sq=board.king[xside];
if(side==white&&board.b[side][queen]&&(BitPosArray[sC6]|BitPosArray[sF6])&c)
	{
	if(c&BitPosArray[sF6]&&sq>sH6&&distance[sq][sG7]==1) s+=PAWNNEARKING;
	if(c&BitPosArray[sC6]&&sq>sH6&&distance[sq][sB7]==1) s+=PAWNNEARKING;
	}
else if(side==black&&board.b[side][queen]&&(BitPosArray[sC3]|BitPosArray[sF3])&c)
	{
	if(c&BitPosArray[sF3]&&sq<sA3&&distance[sq][sG2]==1) s+=PAWNNEARKING;
	if(c&BitPosArray[sC3]&&sq<sA3&&distance[sq][sB2]==1) s+=PAWNNEARKING;
	}
t=passed[side]&brank67[side];
if(t&&(board.pmaterial[xside]==ValueR||(board.pmaterial[xside]==ValueN&&pieces[xside]==board.b[xside][knight])))
	{
	n1=ROW(board.king[xside]);
	n2=RANK(board.king[xside]);
	for(i=0;i<=6;i++)
		{
		if(t&FileBit[i]&&t&FileBit[i+1]&&(n1<i-1||n1>i+1||(side==white&&n2<4)||(side==black&&n2>3))) s+=CONNECTEDPP;
		}
	}
blocker=board.friends[side]|board.friends[xside];
if(side==white&&(((c&d2e2[white])>>8)&blocker)) s+=BLOCKDEPAWN;
if(side==black&&(((c&d2e2[black])<<8)&blocker)) s+=BLOCKDEPAWN;
if(passed[side]&&board.pmaterial[xside]==0)
	{
	e=board.b[xside];
	i1=board.king[xside];
	p=passed[side];
	while(p)
		{
		sq=leadz(p);
		CLEARBIT(p,sq);
		if(board.side==side)
			{
			if(!(SquarePawnMask[side][sq]&board.b[xside][king])) s+=ValueQ*Passed[side][RANK(sq)]/PFACTOR;
			}
		else if(!(MoveArray[king][i1]&SquarePawnMask[side][sq])) s+=ValueQ*Passed[side][RANK(sq)]/PFACTOR;
		}
	}
c=board.b[side][pawn];
if(abs(ROW(board.king[side])-ROW(board.king[xside]))>=4&&PHASE<6)
	{
	n1=ROW(board.king[xside]);
	p=(IsolaniMask[n1]|FileBit[n1])&c;
	while(p)
		{
		sq=leadz(p);
		CLEARBIT(p,sq);
		s+=10*(5-distance[sq][board.king[xside]]);
		}
	}
return s;
}

static const int Outpost[2][64]=
{
{
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,1,1,1,1,0,0,
0,1,1,1,1,1,1,0,
0,0,1,1,1,1,0,0,
0,0,0,1,1,0,0,0,
0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,
0,0,0,1,1,0,0,0,
0,0,1,1,1,1,0,0,
0,1,1,1,1,1,1,0,
0,0,1,1,1,1,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 }
};


#ifdef __STORM__
__inline int CTL(int sq,int piece,int side)
#else
static inline int CTL(int sq,int piece,int side)
#endif
{
int s,n,EnemyKing,FriendlyKing;
BitBoard controlled;
s=0;
EnemyKing=board.king[1^side];
FriendlyKing=board.king[side];
controlled=AttackXFrom(sq,side);
n=nbits(controlled&(rings[0]|rings[1]));
s+=4*n;
n=nbits(controlled&DistMap[EnemyKing][2]);
s+=n;
n=nbits(controlled&DistMap[FriendlyKing][2]);
s+=n;
n=nbits(controlled);
s+=4*n;
return s;
}

int ScoreN(int side)
{
int s,s1,sq,xside;
int EnemyKing;
BitBoard c,t;
if(board.b[side][knight]==NULLBITBOARD) return 0;
xside=side^1;
s=s1=0;
c=board.b[side][knight];
t=board.b[xside][pawn]; 
EnemyKing=board.king[xside];
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	s1=CTL(sq,knight,side);
	if((BitPosArray[sq]&(FileBit[0]|FileBit[7]|RankBit[0]|RankBit[7]))!=NULLBITBOARD) s1+=KNIGHTONRIM;
	if(Outpost[side][sq]&&!(t&IsolaniMask[ROW(sq)]&PassedPawnMask[side][sq]))
		{
		s1+=OUTPOSTKNIGHT;
		if(MoveArray[ptype[xside]][sq]&board.b[side][pawn]) s1+=OUTPOSTKNIGHT;
		}
	if(MoveArray[knight][sq]&weaked[xside]) s1+=ATAKWEAKPAWN;
	pscore[sq]=s1;
	s+=s1;
	}
return s;
}

int ScoreB(int side)
{
int s,s1,xside,n,sq,EnemyKing;
BitBoard c,t;
if(board.b[side][bishop]==NULLBITBOARD) return 0;
s=s1=0;
c=board.b[side][bishop];
xside=side^1;
EnemyKing=board.king[xside];
n=0;
t=board.b[xside][pawn];
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	n++;
	s1=CTL(sq,bishop,side);
	if(Outpost[side][sq]&&!(t&IsolaniMask[ROW(sq)]&PassedPawnMask[side][sq]))
		{
		s1+=OUTPOSTBISHOP;
		if(MoveArray[ptype[xside]][sq]&board.b[side][pawn]) s1+=OUTPOSTBISHOP;
		}
	if(side==white)
		{
		if(board.king[side]>=sF1&&board.king[side]<=sH1&&sq==sG2) s1+=FIANCHETTO;
		if(board.king[side]>=sA1&&board.king[side]<=sC1&&sq==sB2) s1+=FIANCHETTO;
		}
	else if(side==black)
		{
		if(board.king[side]>=sF8&&board.king[side]<=sH8&&sq==sG7) s1+=FIANCHETTO;
		if(board.king[side]>=sA8&&board.king[side]<=sC8&&sq==sB7) s1+=FIANCHETTO;
		}
	if(BishopAttack(sq)&weaked[xside]) s1+=ATAKWEAKPAWN;
	pscore[sq]=s1;
	s+=s1;
	}
if(n>1) s+=DOUBLEDBISHOPS;
return s;
}

int BishopTrapped(int side)
{
int s=0;
if(side==white)
	{
	if((board.b[white][bishop]&BitPosArray[sA7])&&(board.b[black][pawn]&BitPosArray[sB6])&&SwapOff(MOVE(sA7,sB6))<0) s+=BISHOPTRAPPED;
	if((board.b[white][bishop]&BitPosArray[sH7])&&(board.b[black][pawn]&BitPosArray[sG6])&&SwapOff(MOVE(sH7,sG6))<0) s+=BISHOPTRAPPED;
	}
else
	{
	if((board.b[black][bishop]&BitPosArray[sA2])&&(board.b[white][pawn]&BitPosArray[sB3])&&SwapOff(MOVE(sA2,sB3))<0) s+=BISHOPTRAPPED;
	if((board.b[black][bishop]&BitPosArray[sH2])&&(board.b[white][pawn]&BitPosArray[sG3])&&SwapOff(MOVE(sH2,sG3))<0) s+=BISHOPTRAPPED;
	}
return s;
}

int ScoreR(int side)
{
int s,s1,sq,xside,fyle,EnemyKing;
BitBoard c;
if(board.b[side][rook]==NULLBITBOARD) return 0;
s=s1=0;
c=board.b[side][rook];
xside=side^1;
EnemyKing=board.king[xside];
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	s1=CTL(sq,rook,side);
	fyle=ROW(sq);
	if(PHASE<7)
		{
		if(!(board.b[side][pawn]&FileBit[fyle]))
			{
			if(fyle==5&&ROW(board.king[xside])>=E_FILE) s1+=ROOKLIBERATED;
			s1+=ROOKHALFFILE;
			if(!(board.b[xside][pawn]&FileBit[fyle])) s1+=ROOKOPENFILE;
			}
		}
	if(phase>6&&(FileBit[fyle]&passed[white]&brank58[white]))
		{
		if(nbits(Ray[sq][7]&passed[white])==1) s1+=ROOKBEHINDPP;
		else if(Ray[sq][4]&passed[white]) s1+=ROOKINFRONTPP;
		}
	if(FileBit[fyle]&passed[black]&brank58[black])
		{
		if(nbits(Ray[sq][4]&passed[black])==1) s1+=ROOKBEHINDPP;
		else if(Ray[sq][7]&passed[black]) s1+=ROOKINFRONTPP;
		}
	if(RookAttack(sq)&weaked[xside]) s1+=ATAKWEAKPAWN;
	if(RANK(sq)==rank7[side]&&(RANK(EnemyKing)==rank8[side]||board.b[xside][pawn]&RankBit[RANK(sq)])) s1+=ROOK7RANK;
	pscore[sq]=s1;
	s+=s1;
	}
return s;
}

int DoubleQR7(int side)
{
int xside;
xside=1^side;
if(nbits((board.b[side][queen]|board.b[side][rook])&brank7[side])>1&&((board.b[xside][king]&brank8[side])||(board.b[xside][pawn]&brank7[side]))) return ROOKS7RANK;
return 0;
}

int ScoreQ(int side)
{
int s,s1,sq,xside,EnemyKing;
BitBoard c;
if(board.b[side][queen]==NULLBITBOARD) return 0;
s=s1=0;
xside=1^side;
c=board.b[side][queen];
EnemyKing=board.king[xside];
if(side==computer) if(c==NULLBITBOARD) s+=QUEEN_NOT_PRESENT;
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	s1=CTL(sq,queen,side);
	if(distance[sq][EnemyKing]<=2) s1+=QUEENNEARKING;
	if(QueenAttack(sq)&weaked[xside]) s1+=ATAKWEAKPAWN;
	pscore[sq]=s1;
	s+=s1;
	}
return s;
}

static const int KingSq[64]=
{
24,24,24,16,16,0,32,32,
24,20,16,12,12,16,20,24,
16,12,8,4,4,8,12,16,
12,8,4,0,0,4,8,12,
12,8,4,0,0,4,8,12,
16,12,8,4,4,8,12,16,
24,20,16,12,12,16,20,24,
24,24,24,16,16,0,32,32
};

static const int EndingKing[64]=
{
0,6,12,18,18,12,6,0,
6,12,18,24,24,18,12,6,
12,18,24,32,32,24,18,12,
18,24,32,48,48,32,24,18,
18,24,32,48,48,32,24,18,
12,18,24,32,32,24,18,12,
6,12,18,24,24,18,12,6,
0,6,12,18,18,12,6,0
};

static int pawncover[9]={-60,-30,0,5,30,30,30,30,30};
static const int factor[9]={7,8,8,7,6,5,4,2,0};
static const BitBoard Corner[2]={ 0xC3C3000000000000ULL,0x000000000000C3C3ULL};

int ScoreK(int side)
{
int s,sq,sq1,n,n1,n2,xside,file,fsq,rank;
BitBoard b,x;
s=0;
xside=1^side;
sq=board.king[side];
file=ROW(sq);
rank=RANK(sq);
KingSafety[side]=0;
if(!ENDING)
	{ 
	s+=((6-phase)*KingSq[sq]+phase*EndingKing[sq])/6;
	if(side==white) n=nbits(MoveArray[king][sq]&board.b[side][pawn]&RankBit[rank+1]);
	else n=nbits(MoveArray[king][sq]&board.b[side][pawn]&RankBit[rank-1]);
	s+=pawncover[n];
	if(!board.castled[side])
		{
		n=-1;
		if(side==white)
			{
			if(sq==4&&Mvboard[sq]==0)
				{
				if((board.b[side][rook]&BitPosArray[sH1])!=NULLBITBOARD&&Mvboard[sH1]==0) n=nbits(MoveArray[king][sG1]&board.b[side][pawn]&RankBit[rank+1]);
				if((board.b[side][rook]&BitPosArray[sA1])!=NULLBITBOARD&&Mvboard[sA1]==0) n=nbits(MoveArray[king][sC1]&board.b[side][pawn]&RankBit[rank+1]);
				}
			}
		else
			{
			if(sq==60&&Mvboard[sq]==0)
				{
				if((board.b[side][rook]&BitPosArray[sH8])!=NULLBITBOARD&&Mvboard[sH8]==0)
				n=nbits(MoveArray[king][sG8]&board.b[side][pawn]&RankBit[rank-1]);
				if((board.b[side][rook]&BitPosArray[sA8])!=NULLBITBOARD&&Mvboard[sA8]==0)
				n=nbits(MoveArray[king][sC8]&board.b[side][pawn]&RankBit[rank-1]);
				}
			}
		pawncover[3]=30;
		if(n!=-1) s+=pawncover[n];
		}
	if(side==computer&&file>=F_FILE&&!(FileBit[G_FILE]&board.b[side][pawn]))
		{
		if(side==white&&cboard[sF2]==pawn) s+=GOPEN;
		else if(side==black&&cboard[sF7]==pawn) s+=GOPEN;
		}
	if(!(FileBit[file]&board.b[side][pawn])) s+=KINGOPENFILE;
	if(!(FileBit[file]&board.b[xside][pawn])) s+=KINGOPENFILE1;
	switch(file)
		{
		case A_FILE:
		case E_FILE:
		case F_FILE:
		case G_FILE:
			if(!(FileBit[file+1]&board.b[side][pawn])) s+=KINGOPENFILE;
			if(!(FileBit[file+1]&board.b[xside][pawn])) s+=KINGOPENFILE1;
			break;
		case H_FILE:
		case D_FILE:
		case C_FILE:
		case B_FILE:
			if(!(FileBit[file-1]&board.b[side][pawn])) s+=KINGOPENFILE;
			if(!(FileBit[file-1]&board.b[xside][pawn])) s+=KINGOPENFILE1;
		}
	if(board.castled[side])
		{
		if(side==white)
			{
			if(file>E_FILE)
				{
				if(!(BitPosArray[sF2]&board.b[side][pawn])||!(BitPosArray[sG2]&board.b[side][pawn])||!(BitPosArray[sH2]&board.b[side][pawn])) s+=RUPTURE;
				}
			else if(file<E_FILE)
				{
				if(!(BitPosArray[sA2]&board.b[side][pawn])||!(BitPosArray[sB2]&board.b[side][pawn])||!(BitPosArray[sC2]&board.b[side][pawn])) s+=RUPTURE;
				}
			}
		else
			{
			if(file>E_FILE)
				{
				if(!(BitPosArray[sF7]&board.b[side][pawn])||!(BitPosArray[sG7]&board.b[side][pawn])||!(BitPosArray[sH7]&board.b[side][pawn])) s+=RUPTURE;
				}
			else if(file<E_FILE)
				{
				if(!(BitPosArray[sA7]&board.b[side][pawn])||!(BitPosArray[sB7]&board.b[side][pawn])||!(BitPosArray[sC7]&board.b[side][pawn])) s+=RUPTURE;
				}
			}
		}
	if(side==computer)
		{
		if(file>=E_FILE&&board.b[xside][queen]&&board.b[xside][rook]&&!((board.b[side][pawn]|board.b[xside][pawn])&FileBit[7])) s+=HOPEN;
		if(side==white)
			{
			if(file>E_FILE)
				{
				if(board.b[side][rook]&mask_kr_trapped_w[H_FILE-file]) s+=ROOKTRAPPED;
				}
			else if(file<D_FILE)
				{
				if(board.b[side][rook]&mask_qr_trapped_w[file]) s+=ROOKTRAPPED;
				}
			}
		else
			{
			if(file>E_FILE)
				{
				if(board.b[side][rook]&mask_kr_trapped_b[H_FILE-file]) s+=ROOKTRAPPED;
				}
			else if(file<D_FILE)
				{
				if(board.b[side][rook]&mask_qr_trapped_b[file]) s+=ROOKTRAPPED;
				}
			}
		}
	if(file>E_FILE&&ROW(board.king[xside])<D_FILE)
		{
		if(side==white)
		fsq=sG3; else fsq=sG6;
		if((BitPosArray[fsq]&board.b[side][pawn])!=NULLBITBOARD) if(((BitPosArray[sF4]|BitPosArray[sH4]|BitPosArray[sF5]|BitPosArray[sH5])&board.b[xside][pawn])!=NULLBITBOARD)  s+=FIANCHETTO_TARGET;	
		}
	if(file<E_FILE&&ROW(board.king[xside])>E_FILE)
		{
		if(side==white) fsq=sB3;
		else fsq=sB6;
		if((BitPosArray[fsq]&board.b[side][pawn])!=NULLBITBOARD) if(((BitPosArray[sA4]|BitPosArray[sC4]|BitPosArray[sA5]|BitPosArray[sC5])&board.b[xside][pawn])!=NULLBITBOARD) s+=FIANCHETTO_TARGET;	
		}
	x=boardhalf[side]&boardside[file<=D_FILE];
	n1=nbits(x&(board.friends[xside]));
	if(n1>0)
		{
		n2=nbits(x &(board.friends[side]&~board.b[side][pawn]&~board.b[side][king]));
		if(n1>n2) s+=(n1-n2)*KING_DEFENDER_DEFICIT;
		}
	KingSafety[side]=s;
	s=(s*factor[phase])/8;
	}
else
	{
	s+=EndingKing[sq];
	s+=CTL(sq,king,side);
	b=(board.b[white][pawn]|board.b[black][pawn]);
	while(b)
		{
		sq1=leadz(b);
		CLEARBIT(b,sq1);
		if(BitPosArray[sq1]&board.b[white][pawn]) s-=distance[sq][sq1+8]*10-5;
		else if(BitPosArray[sq1]&board.b[white][pawn]) s-=distance[sq][sq1-8]*10-5;
		else s-=distance[sq][sq1]-5;
		}
	if(MoveArray[king][sq]&weaked[xside]) s+=ATAKWEAKPAWN*2;
	}
if(phase>=4)
	{
	if(side==white)
		{
		if(sq<sA2) if(!(MoveArray[king][sq] &(~board.b[side][pawn]&RankBit[1]))) s+=KING_BACK_RANK_WEAK;
		}
	else
		{
		if(sq>sH7) if(!(MoveArray[king][sq] &(~board.b[side][pawn]&RankBit[6]))) s+=KING_BACK_RANK_WEAK;
		}
	}
pscore[sq]=s;
return s;
}

int LoneKing(int side,int loser)
{
int s,winer,sq1,sq2;
winer=1^loser;
if(board.material[winer]==ValueB+ValueN&&nbits(board.b[winer][bishop])==1&&nbits(board.b[winer][knight])==1) return(ScoreKBNK(side,loser));
sq1=board.king[winer];
sq2=board.king[loser];
s=150-6*taxicab[sq1][sq2]-EndingKing[sq2];
if(side==loser) s=-s;
s+=MATERIAL;
return s;
}

int KPK(int side)
{
int winer,loser,sq,sqw,sql;
int s;
winer=(board.b[white][pawn]?white:black);
loser=1^winer;
sq =leadz(board.b[winer][pawn]);
sqw=board.king[winer];
sql=board.king[loser];
s=ValueP+(ValueQ*Passed[winer][RANK(sq)]/PFACTOR)+4*(winer==white?RANK(sqw):7-RANK(sqw));
if(~SquarePawnMask[winer][sq]&board.b[loser][king])
	{
	if(!(MoveArray[king][sql]&SquarePawnMask[winer][sq])) return(winer==side?s:-s);
	if(winer==side) return s;
	}
if(ROW(sq)&&ROW(sq)!=7&&((IsolaniMask[ROW(sq)]|FileBit[ROW(sq)])&board.b[winer][king]))
	{
	if(winer==white)
		{
		if(RANK(sqw)==RANK(sq)+2) return winer==side?s:-s;
		if(RANK(sqw)==RANK(sq)+1)
			{
			if(RANK(sqw)==5) return winer==side?s:-s;
			if(sqw<sA6) 
				{
				if(sqw+16==sql&&winer==side) return 0;
				else return winer==side?s:-s;
				}
			}
		if(RANK(sqw)==RANK(sq))
			{
			if((RANK(sql)-RANK(sq)<2||RANK(sql)-RANK(sq)>4)&&winer==side) return s;
			if((RANK(sql)-RANK(sq)<1||RANK(sql)-RANK(sq)>5)&&loser==side) return -s;
			if(RANK(sq)==5&&sqw+16!=sql) return winer==side?s:0;
			}
		if(RANK(sq)==6&&RANK(sqw)==5)
			{
			if(sql!=sq+8) return winer==side?s:0;
			if(sql==sq+8&&sql==sqw+16) return winer==side?s:0;
			}
		}
	else
		{
		if(RANK(sqw)==RANK(sq)-2) return winer==side?s:-s;
		if(RANK(sqw)==RANK(sq)-1)
			{
			if(RANK(sqw)==2) return winer==side?s:-s;
			if(sqw>sH3)
				{
				if(sqw-16==sql&&winer==side) return 0;
				else return winer==side?s:-s;
				}
			}
		if(RANK(sqw)==RANK(sq))
			{
			if((RANK(sq)-RANK(sql)<2||RANK(sq)-RANK(sql)>4)&&winer==side) return s;
			if((RANK(sq)-RANK(sql)<1||RANK(sq)-RANK(sql)>5)&&loser==side) return -s;
			if(RANK(sq)==5&&sqw+16!=sql) return winer==side?s:0;
			}
		if(RANK(sq)==1&&RANK(sqw)==2)
			{
			if(sql!=sq-8) return winer==side?s:0;
			if(sql==sq-8&&sql==sqw-16) return winer==side?s:0;
			}
		} 
	}  
return 0;
}

int KBNK[64]=
{
0,10,20,30,40,50,60,70,
10,20,30,40,50,60,70,60,
20,30,40,50,60,70,60,50,
30,40,50,60,70,60,50,40,
40,50,60,70,60,50,40,30,
50,60,70,60,50,40,30,20,
60,70,60,50,40,30,20,10,
70,60,50,40,30,20,10,0
};

int ScoreKBNK(int side,int loser)
{
int s,winer,sq1,sq2,sqB;
winer=1^loser;
sqB=board.king[loser];
if(board.b[winer][bishop]&WHITESQUARES)
sqB=RANK(sqB)*8+7-ROW(sqB);
sq1=board.king[winer];
sq2=board.king[loser];
s=300-6*taxicab[sq1][sq2];
s-=KBNK[sqB];
s-=EndingKing[sq2];
s-=taxicab[leadz(board.b[winer][knight])][sq2];
s-=taxicab[leadz(board.b[winer][bishop])][sq2];
if(board.b[winer][king]&(BitBoard)(0x00003C3C3C3C0000ULL)) s+=20;
if(side==loser) s=-s;
s+=MATERIAL;
return s; 
}

static const BitBoard nn[2]={0x4200000000000000ULL,0x0000000000000042ULL};
static const BitBoard bb[2]={0x2400000000000000ULL,0x0000000000000024ULL};
static const BitBoard rr[2]={0x8100000000000000ULL,0x0000000000000081ULL};
static const BitBoard corner=0xC3C300000000C3C3ULL;

int ScoreDev(int side)
{
int s=0;
int sq,xside;
BitBoard c;
xside=1^side;
c=(board.b[side][knight]&nn[side])|(board.b[side][bishop]&bb[side]);
s+=nbits(c)*(-8);
if(board.castled[side]||GameCnt/2+1>=20) return s;
s+=NOTCASTLED;
if(Mvboard[board.king[side]]>0) s+=KINGMOVED;
c=board.b[side][rook];
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	if(Mvboard[sq]>0) s+=ROOKMOVED;
	}
if(board.b[side][queen])
	{
	sq=leadz(board.b[side][queen]);
	if(Mvboard[sq]>0) s+=EARLYQUEENMOVE;
	}
c=board.b[side][knight]|board.b[side][bishop];
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	if(Mvboard[sq]>1)	s+=EARLYMINORREPEAT;
	}
c=board.b[side][pawn]&(FileBit[0]|FileBit[1]|FileBit[6]|FileBit[7]);
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	if(Mvboard[sq]>0) s+=EARLYWINGPAWNMOVE;
	}
c=board.b[side][pawn]&(FileBit[2]|FileBit[3]|FileBit[4]|FileBit[5]);
while(c)
	{
	sq=leadz(c);
	CLEARBIT(c,sq);
	if(Mvboard[sq]>1) s+=EARLYCENTERPREPEAT;
	}
return s;
}

int Evaluate(int alpha,int beta)
{
int side,xside,s,s1,score;
int npiece[2];
BitBoard *b;
side=board.side;
xside=1^side;
if(alpha>MATE-255||beta<-MATE+255) return MATERIAL; 
if(board.material[white]+board.material[black]==ValueP) return KPK(side);  
if(board.material[xside]==0&&board.b[side][pawn]==NULLBITBOARD) return LoneKing(side,xside);
if(board.material[side]==0&&board.b[xside][pawn]==NULLBITBOARD) return LoneKing(side,side);
EvalCall++;
phase=PHASE;
b=board.b[white];
pieces[white]=b[knight]|b[bishop]|b[rook]|b[queen];
npiece[white]=nbits(pieces[white]);
b=board.b[black];
pieces[black]=b[knight]|b[bishop]|b[rook]|b[queen];
npiece[black]=nbits(pieces[black]);
hunged[white]=hunged[black]=0;
memset(pscore,0,sizeof(pscore));
s1=MATERIAL;
if((s1+maxposnscore[side]<alpha||s1-maxposnscore[xside]>beta)&&phase<=6)
	{
	score=s1;
	goto next;
	}
s=0;
s+=ScoreDev(side)-ScoreDev(xside);
s+=ScoreP(side)-ScoreP(xside);
s+=ScoreK(side)-ScoreK(xside);
s+=BishopTrapped(side)-BishopTrapped(xside);
s+=DoubleQR7(side)-DoubleQR7(xside);
s1=s+MATERIAL;
if(s1+lazyscore[side]<alpha||s1-lazyscore[side]>beta) score=s1;
else
	{
	EvalCnt++;
	s1=0;
	s1+=ScoreN(side)-ScoreN(xside);
	s1+=ScoreB(side)-ScoreB(xside);
	s1+=ScoreR(side)-ScoreR(xside);
	s1+=ScoreQ(side)-ScoreQ(xside);
	lazyscore[side]=MAX(s1,lazyscore[side]);
	maxposnscore[side]=MAX(maxposnscore[side],s+s1);
	score=s+s1+MATERIAL;
	}

next:
if(MATERIAL>=200)
	{
	score+=(RootPieces-nbits(pieces[white]|pieces[black]))*TRADEPIECE;
	score-=(RootPawns-nbits(board.b[white][pawn]|board.b[black][pawn]))*TRADEPAWNS;
	}
else if(MATERIAL<=-200)
	{
	score-=(RootPieces-nbits(pieces[white]|pieces[black]))*TRADEPIECE;
	score+=(RootPawns-nbits(board.b[white][pawn]|board.b[black][pawn]))*TRADEPAWNS;
	}
if(ENDING&&pieces[white]==board.b[white][bishop]&&pieces[black]==board.b[black][bishop]&&((pieces[white]&WHITESQUARES&&pieces[black]&BLACKSQUARES)||(pieces[white]&BLACKSQUARES&&pieces[black]&WHITESQUARES))) score/=2;
if(score>0&&!board.b[side][pawn]&&(board.material[side]<ValueR||pieces[side]==board.b[side][knight])) score=0;
if(score<0&&!board.b[xside][pawn]&&(board.material[xside]<ValueR||pieces[xside]==board.b[xside][knight])) score=0;
return score;
}

int EvaluateDraw()
{
BitBoard *w,*b;
int wm,bm,wn,bn;
w=board.b[white];
b=board.b[black];
if(w[pawn]||b[pawn]) return false;
wm=board.material[white];
bm=board.material[black];
wn=nbits(w[knight]);
bn=nbits(b[knight]);
if((wm<ValueR||(wm==2*ValueN&&wn==2))&&(bm<ValueR||(bm==2*ValueN&&bn==2))) return true;
if(wm<ValueR)
	{
	if(bm==2*ValueB&&(nbits(board.b[black][bishop]&WHITESQUARES)==2||nbits(board.b[black][bishop]&BLACKSQUARES)==2)) return true;
	}
if(bm<ValueR)
	{
	if(wm==2*ValueB&&(nbits(board.b[white][bishop]&WHITESQUARES)==2||nbits(board.b[white][bishop]&BLACKSQUARES)==2)) return true;
	}
return false;
}
