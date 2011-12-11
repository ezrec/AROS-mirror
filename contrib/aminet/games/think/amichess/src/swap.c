#include "common.h"

static const int xray[7]={0,1,0,1,1,1,0};

int SwapOff(int move)
{
int f,t,sq,piece,side,xside,lastval;
int swaplist[MAXPLYDEPTH],n;
BitBoard b,c,*d,*e,r;
f=FROMSQ(move);
t=TOSQ(move);
side=((board.friends[white]&BitPosArray[f])?white:black);
xside=1^side;
b=AttackTo(t,side);
c=AttackTo(t,xside);
CLEARBIT(b,f);
if(xray[cboard[f]])
AddXrayPiece(t,f,side,&b,&c);
d=board.b[side];
e=board.b[xside];
if(move&PROMOTION)
	{
	swaplist[0]=Value[PROMOTEPIECE(move)]-ValueP;
	lastval=-Value[PROMOTEPIECE(move)];
	}
else
	{
	swaplist[0]=(move&ENPASSANT?ValueP:Value[cboard[t]]);
	lastval=-Value[cboard[f]];
	}
n=1;
while(1)
	{
	if(c==NULLBITBOARD) break;
	for(piece=pawn;piece<=king;piece++)
		{
		if((r=c&e[piece]))
			{
			sq=leadz(r);
			CLEARBIT(c,sq);
			if(xray[piece]) AddXrayPiece(t,sq,xside,&c,&b);
			swaplist[n]=swaplist[n-1]+lastval;
			n++;
			lastval=Value[piece];
			break;
			}
		}
	if(b==NULLBITBOARD) break;
	for(piece=pawn;piece<=king;piece++)
		{
		if((r=b&d[piece]))
			{
			sq=leadz(r);
			CLEARBIT(b,sq);
			if(xray[piece]) AddXrayPiece(t,sq,side,&b,&c);
			swaplist[n]=swaplist[n-1]+lastval;
			n++;
			lastval=-Value[piece];
			break;
			}
		}
	}

--n;
while(n)
	{
	if(n&1)
		{
		if(swaplist[n]<=swaplist[n-1]) swaplist[n-1]=swaplist[n];
		}
	else
		{
		if(swaplist[n]>=swaplist[n-1]) swaplist[n-1]=swaplist[n];
		}
	--n;
	}
return swaplist[0];
}

void AddXrayPiece(int t,int sq,int side,BitBoard *b,BitBoard *c)
{
int dir,nsq,piece;
BitBoard a;
dir=directions[t][sq];
a=Ray[sq][dir]&board.blocker;
if(a==NULLBITBOARD) return;
nsq=(t<sq?leadz(a):trailz(a));
piece=cboard[nsq];
if((piece==queen)||(piece==rook&&dir> 3)||(piece==bishop&&dir<4))
	{
	if(BitPosArray[nsq]&board.friends[side]) *b|=BitPosArray[nsq];
	else *c|=BitPosArray[nsq];
	}
}
