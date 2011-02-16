#include "common.h"
#include <stdio.h>

static leaf *node;

/*
#define ADDMOVE(a,b,c) do { node->move=MOVE(a,b)|(c); node++; } while(0)

#define ADDPROMOTE(a,b) do { ADDMOVE(a,b,KNIGHTPRM); ADDMOVE(a,b,BISHOPPRM); ADDMOVE(a,b,ROOKPRM); ADDMOVE(a,b,QUEENPRM); } while(0)
*/

static void ADDMOVE(int a,int b,int c)
{
node->move=MOVE(a,b)|(c);
node++;
}

static void ADDPROMOTE(int a,int b)
{
ADDMOVE(a,b,KNIGHTPRM);
ADDMOVE(a,b,BISHOPPRM);
ADDMOVE(a,b,ROOKPRM);
ADDMOVE(a,b,QUEENPRM);
}

static void BitToMove(int f,BitBoard b)
{
int t;
while(b)
	{
	t=leadz(b);
	CLEARBIT(b,t);
	ADDMOVE(f,t,0);
	}
}

void GenMoves(int ply)
{
int side,piece,sq,t,ep;
BitBoard b,c,d,e,friends,notfriends,blocker,notblocker;
BitBoard *a;
side=board.side;
a=board.b[side];
friends=board.friends[side];
notfriends=~friends;
blocker=board.blocker;
notblocker=~blocker;
node=TreePtr[ply+1];
ep=board.ep;
for(piece=knight;piece<=king;piece+=4)
	{
	b=a[piece];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		BitToMove(sq,MoveArray[piece][sq]&notfriends);
		}
	}
b=a[bishop];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	d=BishopAttack(sq);
	BitToMove(sq,d&notfriends);
	}
b=a[rook];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	d=RookAttack(sq);
	BitToMove(sq,d&notfriends);
	}
b=a[queen];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	d=QueenAttack(sq);
	BitToMove(sq,d&notfriends);
	}
e=(board.friends[1^side]|(ep>-1?BitPosArray[ep]:NULLBITBOARD));
if(side==white)
	{
	c=(a[pawn]>>8)&notblocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t>=56) ADDPROMOTE(t-8,t);
		else ADDMOVE(t-8,t,0);
		}
	b=a[pawn]&RankBit[1];
	c=(b>>8)&notblocker;
	c=(c>>8)&notblocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		ADDMOVE(t-16,t,0);
		}
	b=a[pawn]&~FileBit[0];
	c=(b>>7)&e;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t>=56) ADDPROMOTE(t-7,t);
		else if(ep==t) ADDMOVE(t-7,t,ENPASSANT);
		else ADDMOVE(t-7,t,0);
		}
	b=a[pawn]&~FileBit[7];
	c=(b>>9)&e;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t>=56) ADDPROMOTE(t-9,t);
		else if(ep==t) ADDMOVE(t-9,t,ENPASSANT);
		else ADDMOVE(t-9,t,0);
		}
	}
if(side==black)
	{
	c=(a[pawn]<<8)&notblocker;		
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t<=7) ADDPROMOTE(t+8,t);
		else ADDMOVE(t+8,t,0);
		}
	b=a[pawn]&RankBit[6];
	c=(b<<8)&notblocker;
	c=(c<<8)&notblocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		ADDMOVE(t+16,t,0);
		}
	b=a[pawn]&~FileBit[7];
	c=(b<<7)&e;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t<=7) ADDPROMOTE(t+7,t);
		else if(ep==t) ADDMOVE(t+7,t,ENPASSANT);
		else ADDMOVE(t+7,t,0);
		}
	b=a[pawn]&~FileBit[0];
	c=(b<<9)&e;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t<=7) ADDPROMOTE(t+9,t);
		else if(ep==t) ADDMOVE(t+9,t,ENPASSANT);
		else ADDMOVE(t+9,t,0);
		}
	}
b=board.b[side][rook];
if(side==white&&(board.flag&WKINGCASTLE)&&(b&BitPosArray[sH1])&&!(FromToRay[sE1][sG1]&blocker)&&!SqAtakd(sE1,black)&&!SqAtakd(sF1,black)&&!SqAtakd(sG1,black)) ADDMOVE(sE1,sG1,CASTLING);
if(side==white&&(board.flag&WQUEENCASTLE)&&(b&BitPosArray[sA1])&&!(FromToRay[sE1][sB1]&blocker)&&!SqAtakd(sE1,black)&&!SqAtakd(sD1,black)&&!SqAtakd(sC1,black)) ADDMOVE(sE1,sC1,CASTLING);
if(side==black&&(board.flag&BKINGCASTLE)&&(b&BitPosArray[sH8])&&!(FromToRay[sE8][sG8]&blocker)&&!SqAtakd(sE8,white)&&!SqAtakd(sF8,white)&&!SqAtakd(sG8,white)) ADDMOVE(sE8,sG8,CASTLING);
if(side==black&&(board.flag&BQUEENCASTLE)&&(b&BitPosArray[sA8])&&!(FromToRay[sE8][sB8]&blocker)&&!SqAtakd(sE8,white)&&!SqAtakd(sD8,white)&&!SqAtakd(sC8,white)) ADDMOVE(sE8,sC8,CASTLING);
TreePtr[ply+1]=node;
GenCnt+=TreePtr[ply+1]-TreePtr[ply];
}

void GenNonCaptures(int ply)
{
int side,piece,sq,t,ep;
BitBoard b,c,d,friends,notfriends,blocker,notblocker;
BitBoard *a;
side=board.side;
a=board.b[side];
friends=board.friends[side];
notfriends=~friends;
blocker=board.blocker;
notblocker=~blocker;
node=TreePtr[ply+1];
ep=board.ep;
for(piece=knight;piece<=king;piece+=4)
	{
	b=a[piece];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		BitToMove(sq,MoveArray[piece][sq]&notblocker);
		}
	}
b=a[bishop];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	d=BishopAttack(sq);
	BitToMove(sq,d&notblocker);
	}
b=a[rook];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	d=RookAttack(sq);
	BitToMove(sq,d&notblocker);
	}
b=a[queen];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	d=QueenAttack(sq);
	BitToMove(sq,d&notblocker);
	}
if(side==white)
	{
	c=(a[pawn]>>8)&notblocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t<56)
		ADDMOVE(t-8,t,0);
		}
	b=a[pawn]&RankBit[1];
	c=(b>>8)&notblocker;
	c=(c>>8)&notblocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		ADDMOVE(t-16,t,0);
		}
	}

if(side==black)
	{
	c=(a[pawn]<<8)&notblocker;		
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t>7) ADDMOVE(t+8,t,0);
		}
	b=a[pawn]&RankBit[6];
	c=(b<<8)&notblocker;
	c=(c<<8)&notblocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		ADDMOVE(t+16,t,0);
		}
	}
b=board.b[side][rook];
if(side==white&&(board.flag&WKINGCASTLE)&&(b&BitPosArray[7])&&!(FromToRay[4][6]&blocker)&&!SqAtakd(4,black)&&!SqAtakd(5,black)&&!SqAtakd(6,black)) ADDMOVE(4,6,CASTLING);
if(side==white&&(board.flag&WQUEENCASTLE)&&(b&BitPosArray[0])&&!(FromToRay[4][1]&blocker)&&!SqAtakd(4,black)&&!SqAtakd(3,black)&&!SqAtakd(2,black)) ADDMOVE(4,2,CASTLING);
if(side==black&&(board.flag&BKINGCASTLE)&&(b&BitPosArray[63])&&!(FromToRay[60][62]&blocker)&&!SqAtakd(60,white)&&!SqAtakd(61,white)&&!SqAtakd(62,white)) ADDMOVE(60,62,CASTLING);
if(side==black&&(board.flag&BQUEENCASTLE)&&(b&BitPosArray[56])&&!(FromToRay[60][57]&blocker)&&!SqAtakd(60,white)&&!SqAtakd(59,white)&&!SqAtakd(58,white)) ADDMOVE(60,58,CASTLING);
TreePtr[ply+1]=node;
GenCnt+=TreePtr[ply+1]-TreePtr[ply];
}

void GenCaptures(int ply)
{
int side,piece,sq,t,ep;
BitBoard b,c,friends,notfriends,enemy,blocker;
BitBoard *a;
side=board.side;
a=board.b[side];
friends=board.friends[side];
notfriends=~friends;
enemy=board.friends[1^side];
blocker=board.blocker;
node=TreePtr[ply+1];
ep=board.ep;
for(piece=knight;piece<=king;piece+=4)
	{
	b=a[piece];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		BitToMove(sq,MoveArray[piece][sq]&enemy);
		}
	}
b=a[bishop];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	c=BishopAttack(sq);
	BitToMove(sq,c&enemy);
	}
b=a[rook];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	c=RookAttack(sq);
	BitToMove(sq,c&enemy);
	}
b=a[queen];
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	c=QueenAttack(sq);
	BitToMove(sq,c&enemy);
	}
if(side==white)
	{
	b=a[pawn]&RankBit[6];
	c=(b>>8)&~blocker;
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		ADDPROMOTE(t-8,t);
		}
	b=a[pawn]&~FileBit[0];
	c=(b>>7)&(board.friends[1^side]|(ep>-1?BitPosArray[ep]:0ULL));
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t>=56) ADDPROMOTE(t-7,t);
		else if(ep==t) ADDMOVE(t-7,t,ENPASSANT);
		else ADDMOVE(t-7,t,0);
		}
	b=a[pawn]&~FileBit[7];
	c=(b>>9)&(board.friends[1^side]|(ep>-1?BitPosArray[ep]:0ULL));
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t>=56) ADDPROMOTE(t-9,t);
		else if(ep==t) ADDMOVE(t-9,t,ENPASSANT);
		else ADDMOVE(t-9,t,0);
		}
	}
if(side==black)
	{
	b=a[pawn]&RankBit[1];
	c=(b<<8)&~blocker;		
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		ADDPROMOTE(t+8,t);
		}
	b=a[pawn]&~FileBit[7];
	c=(b<<7)&(board.friends[1^side]|(ep>-1?BitPosArray[ep]:0ULL));
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t<=7) ADDPROMOTE(t+7,t);
		else if(ep==t) ADDMOVE(t+7,t,ENPASSANT);
		else ADDMOVE(t+7,t,0);
		}
	b=a[pawn]&~FileBit[0];
	c=(b<<9)&(board.friends[1^side]|(ep>-1?BitPosArray[ep]:0ULL));
	while(c)
		{
		t=leadz(c);
		CLEARBIT(c,t);
		if(t<=7) ADDPROMOTE(t+9,t);
		else if(ep==t) ADDMOVE(t+9,t,ENPASSANT);
		else ADDMOVE(t+9,t,0);
		}
	}
TreePtr[ply+1]=node;
GenCnt+=TreePtr[ply+1]-TreePtr[ply];
}

void GenCheckEscapes(int ply)
{
int side,xside,kingsq,chksq,sq,sq1,epsq,dir;
BitBoard checkers,b,c,p;
side=board.side;
xside=1^side;
node=TreePtr[ply+1];
kingsq=board.king[side];
checkers=AttackTo(kingsq,xside);
p=board.b[side][pawn];
if(nbits(checkers)==1)
	{
	chksq=leadz(checkers);
	b=AttackTo(chksq,side);
	b&=~board.b[side][king];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		if(!PinnedOnKing(sq,side))
			{
			if(cboard[sq]==pawn&&(chksq<=sH1||chksq>=sA8)) ADDPROMOTE(sq,chksq);
			else ADDMOVE(sq,chksq,0);
			}
		}
	if(board.ep>=0)
		{
		epsq=board.ep;
		if(epsq+(side==white?-8:8)==chksq)
			{
			b=MoveArray[ptype[1^side]][epsq]&p;
			while(b)
				{
				sq=leadz(b);
				CLEARBIT(b,sq);
				if(!PinnedOnKing(sq,side)) ADDMOVE(sq,epsq,ENPASSANT);
				}
			}
		}
	if(slider[cboard[chksq]])
		{
		c=FromToRay[kingsq][chksq]&NotBitPosArray[chksq];
		while(c)
			{
			sq=leadz(c);
			CLEARBIT(c,sq);
			b=AttackTo(sq,side);
			b&=~(board.b[side][king]|p);
			if(side==white&&sq>sH2)
				{
				if(BitPosArray[sq-8]&p) b|=BitPosArray[sq-8];
				if(RANK(sq)==3&&cboard[sq-8]==empty&&(BitPosArray[sq-16]&p)) b|=BitPosArray[sq-16];
				}
			if(side==black&&sq<sH7)
				{
				if(BitPosArray[sq+8]&p) b|=BitPosArray[sq+8];
				if(RANK(sq)==4&&cboard[sq+8]==empty&&(BitPosArray[sq+16]&p)) b|=BitPosArray[sq+16];
				}
			while(b)
				{
				sq1=leadz(b);
				CLEARBIT(b,sq1);
				if(!PinnedOnKing(sq1,side))
					{
					if(cboard[sq1]==pawn&&(sq>sH7||sq<sA2)) ADDPROMOTE(sq1,sq);
					else ADDMOVE(sq1,sq,0);
					}
				}
			}
		}
	}
p=MoveArray[king][kingsq]&~board.friends[side];
while(checkers)
	{
	chksq=leadz(checkers);
	CLEARBIT(checkers,chksq);
	dir=directions[chksq][kingsq];
	if(slider[cboard[chksq]])
	p&=~Ray[chksq][dir];
	}
while(p)
	{
	sq=leadz(p);
	CLEARBIT(p,sq);
	if(!SqAtakd(sq,xside))
	ADDMOVE(kingsq,sq,0);
	}
TreePtr[ply+1]=node;
GenCnt+=TreePtr[ply+1]-TreePtr[ply];
}

void FilterIllegalMoves(int ply)
{
leaf *p;
int side,xside,check,sq;
side=board.side;
xside=1^side;
sq=board.king[side];
for(p=TreePtr[ply];p<TreePtr[ply+1];p++)
	{
	MakeMove(side,&p->move);
	if(cboard[TOSQ(p->move)]!=king) check=SqAtakd(sq,xside);
	else check=SqAtakd(TOSQ(p->move),xside);
	UnmakeMove(xside,&p->move);
	if(check)
		{
		--TreePtr[ply+1];
		*p=*TreePtr[ply+1];
		--p;
		GenCnt--;
		} 
	}
}
