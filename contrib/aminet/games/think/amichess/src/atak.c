#include <stdio.h>
#include <string.h>
#include "common.h"

const int raybeg[7]={0,0,0,0,4,0,0};
const int rayend[7]={0,0,0,4,8,8,0};

int SqAtakd(int sq,int side)
{
BitBoard *a,b,*c,d,blocker;
int t;
a=board.b[side];
if(a[knight]&MoveArray[knight][sq]) return true;
if(a[king]&MoveArray[king][sq]) return true;
if(a[pawn]&MoveArray[ptype[1^side]][sq]) return true;
c=FromToRay[sq];
blocker=board.blocker;
b=(a[bishop]|a[queen])&MoveArray[bishop][sq];
d=~b&blocker;
while(b)
	{
	t=leadz(b);
	if(!(c[t]&d)) return true;
	CLEARBIT(b,t);
	}
b=(a[rook]|a[queen])&MoveArray[rook][sq];
d=~b&blocker;
while(b)
	{
	t=leadz(b);
	if(!(c[t]&d)) return true;
	CLEARBIT(b,t);
	}
return false;
}

void GenAtaks()
{
int side,sq;
BitBoard *a,b,*t,*a0;
memset(Ataks,0,sizeof(Ataks));
for(side=white;side<=black;side++)
	{
	a=board.b[side];
	t=&Ataks[side][knight];
	b=a[knight];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		*t|=MoveArray[knight][sq];
		}
	t=&Ataks[side][bishop];
	b=a[bishop];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		*t|=BishopAttack(sq);
		}
	t=&Ataks[side][rook];
	b=a[rook];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		*t|=RookAttack(sq);
		}
	t=&Ataks[side][queen];
	b=a[queen];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		*t|=QueenAttack(sq);
		}
	t=&Ataks[side][king];
	b=a[king];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		*t|=MoveArray[king][sq];
		}
	t=&Ataks[side][pawn];
	if(side==white)
		{
		b=board.b[white][pawn]&~FileBit[0];
		*t|=(b>>7);
		b=board.b[white][pawn]&~FileBit[7];
		*t|=(b>>9);
		}
	else
		{
		b=board.b[black][pawn]&~FileBit[0];
		*t|=(b<<9);
		b=board.b[black][pawn]&~FileBit[7];
		*t|=(b<<7);
		}
	a0=Ataks[side];
	a0[0]=a0[pawn]|a0[knight]|a0[bishop]|a0[rook]|a0[queen]|a0[king];
	}
}

BitBoard AttackTo(int sq,int side)
{
BitBoard *a,b,*c,e,blocker;
int t;
a=board.b[side];
e=a[knight]&MoveArray[knight][sq];	
e|=a[king]&MoveArray[king][sq];	
e|=a[pawn]&MoveArray[ptype[1^side]][sq];
c=FromToRay[sq];
blocker=board.blocker;
b=(a[bishop]|a[queen])&MoveArray[bishop][sq];
while(b)
	{
	t=leadz(b);
	CLEARBIT(b,t);
	if(!(c[t]&blocker&NotBitPosArray[t])) e|=BitPosArray[t];
	}
b=(a[rook]|a[queen])&MoveArray[rook][sq];
while(b)
	{
	t=leadz(b);
	CLEARBIT(b,t);
	if(!(c[t]&blocker&NotBitPosArray[t])) e|=BitPosArray[t];
	}
return e;
}

BitBoard AttackXTo(int sq,int side)
{
BitBoard *a,b,*c,*d,e,blocker;
int t;
a=board.b[side];
d=board.b[1^side];
e=a[knight]&MoveArray[knight][sq];	
e|=a[king]&MoveArray[king][sq];	
c=FromToRay[sq];
b=(a[pawn]&MoveArray[ptype[1^side]][sq]);
blocker=board.blocker;
blocker&=~(a[bishop]|a[queen]|d[bishop]|d[queen]|b);
b|=(a[bishop]|a[queen])&MoveArray[bishop][sq];
while(b)
	{
	t=leadz(b);
	CLEARBIT(b,t);
	if(!(c[t]&blocker&NotBitPosArray[t])) e|=BitPosArray[t];
	}
b=(a[rook]|a[queen])&MoveArray[rook][sq];
blocker=board.blocker;
blocker&=~(a[rook]|a[queen]|d[rook]|d[queen]);
while(b)
	{
	t=leadz(b);
	CLEARBIT(b,t);
	if(!(c[t]&blocker&NotBitPosArray[t])) e|=BitPosArray[t];
	}
return e;
}

BitBoard AttackFrom(int sq,int piece,int side)
{
switch(piece)
	{
	case pawn:
		return MoveArray[ptype[side]][sq];
	case knight:
		return MoveArray[knight][sq];
	case bishop:
		return BishopAttack(sq);
	case rook:
		return RookAttack(sq);
	case queen:
		return QueenAttack(sq);
	case king:
		return MoveArray[king][sq];
	}
return 0;
}

BitBoard AttackXFrom(int sq,int side)
{
BitBoard *a,b,c,blocker;
int piece,dir,blocksq;
a=board.b[side];
piece=cboard[sq];
blocker=board.blocker;
b=0;
switch(piece)
	{
	case pawn:
		b=MoveArray[ptype[side]][sq];
		break;
	case knight:
		b=MoveArray[knight][sq];
		break;
	case bishop:
	case queen:
		blocker&=~(a[bishop]|a[queen]);
		for(dir=raybeg[bishop];dir<rayend[bishop];dir++)
			{
			c=Ray[sq][dir]&blocker;
			if(c==NULLBITBOARD) c=Ray[sq][dir];
			else
				{
				blocksq=(BitPosArray[sq]>c?leadz(c):trailz(c));
				c=FromToRay[sq][blocksq];
				}
			b|=c;
			}
		if(piece==bishop) break;
		blocker=board.blocker;
	case rook:
		blocker&=~(a[rook]|a[queen]);
		for(dir=raybeg[rook];dir<rayend[rook];dir++)
			{
			c=Ray[sq][dir]&blocker;
			if(c==NULLBITBOARD) c=Ray[sq][dir];
			else
				{
				blocksq=(BitPosArray[sq]>c?leadz(c):trailz(c));
				c=FromToRay[sq][blocksq];
				}
			b|=c;
			}
		break;
	case king:
		b=MoveArray[king][sq];
	} 
return b;
}

int PinnedOnKing(int sq,int side)
{
int xside,KingSq,dir,sq1;
BitBoard b,blocker;
KingSq=board.king[side];
if((dir=directions[KingSq][sq])==-1) return false;
xside=1^side;
blocker=board.blocker;
if(FromToRay[KingSq][sq]&NotBitPosArray[sq]&blocker) return false;
b=(Ray[KingSq][dir]^FromToRay[KingSq][sq])&blocker;
if(b==NULLBITBOARD) return false;
sq1=(sq>KingSq?leadz(b):trailz(b));
if(dir<=3&&BitPosArray[sq1] &(board.b[xside][queen]|board.b[xside][bishop])) return true;
if(dir>=4&&BitPosArray[sq1] &(board.b[xside][queen]|board.b[xside][rook])) return true;
return false;
}

void FindPins(BitBoard *pin)
{
int side,xside,sq,sq1;
BitBoard b,c,e,f,t,*p;
*pin=NULLBITBOARD;
t=board.friends[white]|board.friends[black];
for(side=white;side<=black;side++)
	{
	xside=1^side;
	p=board.b[xside];
	e=p[rook]|p[queen]|p[king];
	e|=(p[bishop]|p[knight])&~Ataks[xside][0];
	b=board.b[side][bishop];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		c=MoveArray[bishop][sq]&e;
		while(c)
			{
			sq1=leadz(c);
			CLEARBIT(c,sq1);
			f=t&NotBitPosArray[sq]&FromToRay[sq1][sq];
			if((board.friends[xside]&f)&&nbits(f)==1)
			*pin|=f;
			}
		}
	e=p[queen]|p[king];
	e|=(p[rook]|p[bishop]|p[knight])&~Ataks[xside][0];
	b=board.b[side][rook];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		c=MoveArray[rook][sq]&e;
		while(c)
			{
			sq1=leadz(c);
			CLEARBIT(c,sq1);
			f=t&NotBitPosArray[sq]&FromToRay[sq1][sq];
			if((board.friends[xside]&f)&&nbits(f)==1)
			*pin|=f;
			}
		}
	e=board.b[xside][king];
	e|=(p[queen]|p[rook]|p[bishop]|p[knight])&~Ataks[xside][0];
	b=board.b[side][queen];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		c=MoveArray[queen][sq]&e;
		while(c)
			{
			sq1=leadz(c);
			CLEARBIT(c,sq1);
			f=t&NotBitPosArray[sq]&FromToRay[sq1][sq];
			if((board.friends[xside]&f)&&nbits(f)==1)
			*pin|=f;
			}
		}
	}
}

int MateScan(int side)
{
int KingSq,QueenSq,xside,sq;
BitBoard b;
xside=1^side;
if(board.b[xside][queen]==0) return 0;
KingSq=board.king[side];
QueenSq=leadz(board.b[xside][queen]);
b=QueenAttack(QueenSq)&MoveArray[king][KingSq];
if(b==0) return 0;
while(b)
	{
	sq=leadz(b);
	CLEARBIT(b,sq);
	if(AttackTo(sq,side)==board.b[side][king]&&AttackXTo(sq,xside)!=board.b[xside][queen])
	return 1;
	}
return 0;
}
