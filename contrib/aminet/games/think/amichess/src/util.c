#include <string.h>
#include <signal.h>
#include "common.h"

unsigned char leadz(BitBoard b)
{
int i;
#ifdef HAVE64BIT
if(i=b>>48) return lzArray[i];
if(i=b>>32) return lzArray[i]+16;
if(i=b>>16) return lzArray[i]+32;
return lzArray[(int)b]+48;
#else
if(i=b.hi>>16) return lzArray[i];
if(b.hi) return lzArray[b.hi]+16;
if(i=(b.hi<<16)|(b.lo>>16)) return lzArray[i]+32;
return lzArray[b.lo]+48;
#endif
}

unsigned char nbits(BitBoard b)
{
#ifdef HAVE64BIT
return BitCount[(int)(b>>48)]+BitCount[((int)(b>>32))&0xffff]+BitCount[((int)(b>>16))&0xffff]+BitCount[((int)b)&0xffff];
#else
return BitCount[b.hi>>16]+BitCount[b.hi&0xffff]+BitCount[b.lo>>16]+BitCount[b.lo&0xffff];
#endif
}

void UpdateFriends()
{
BitBoard *w,*b;
w=board.b[white];
b=board.b[black];
board.friends[white]=w[pawn]|w[knight]|w[bishop]|w[rook]|w[queen]|w[king];
board.friends[black]=b[pawn]|b[knight]|b[bishop]|b[rook]|b[queen]|b[king];
board.blocker=board.friends[white]|board.friends[black];
}

void UpdateCBoard()
{
BitBoard b;
int piece,sq;
memset(cboard,0,sizeof(cboard));
for(piece=pawn;piece<=king;piece++)
	{
	b=board.b[white][piece]|board.b[black][piece];
	while(b)
		{
		sq=leadz(b);
		CLEARBIT(b,sq);
		cboard[sq]=piece;
		}
	}
}

static const int OrigCboard[64]= 
{
rook,knight,bishop,queen,king,bishop,knight,rook,
pawn,pawn,pawn,pawn,pawn,pawn,pawn,pawn,
empty,empty,empty,empty,empty,empty,empty,empty,
empty,empty,empty,empty,empty,empty,empty,empty,
empty,empty,empty,empty,empty,empty,empty,empty,
empty,empty,empty,empty,empty,empty,empty,empty,
pawn,pawn,pawn,pawn,pawn,pawn,pawn,pawn,
rook,knight,bishop,queen,king,bishop,knight,rook
};

void UpdateMvboard()
{
int sq;
for(sq=0;sq<64;sq++)
	{
	if(cboard[sq]==empty||cboard[sq]==OrigCboard[sq]) Mvboard[sq]=0;
	else Mvboard[sq]=1;
	} 
}

void EndSearch(int dummy)
{
SETFLAG(flags,TIMEOUT);
#ifdef __AROS__
#warning "Disabled signal() call here"
#else
signal(SIGINT,EndSearch);
#endif
}

int ValidateBoard()
{
int side,xside,sq;
if(nbits(board.b[white][king])!=1) return false;
if(nbits(board.b[black][king])!=1) return false;
side=board.side; 
xside=1^side;
if(SqAtakd(board.king[xside],side)) return false;
if(board.ep>=0)
	{
	sq=board.ep+(xside==white?8:-8);
	if(!(BitPosArray[sq]&board.b[xside][pawn])) return false;
	}
if(board.flag&WKINGCASTLE)
	{
	if(!(BitPosArray[sE1]&board.b[white][king])) return false;
	if(!(BitPosArray[sH1]&board.b[white][rook])) return false;
	}
if(board.flag&WQUEENCASTLE)
	{
	if(!(BitPosArray[sE1]&board.b[white][king])) return false;
	if(!(BitPosArray[sA1]&board.b[white][rook])) return false;
	}
if(board.flag&BKINGCASTLE)
	{
	if(!(BitPosArray[sE8]&board.b[black][king])) return false;
	if(!(BitPosArray[sH8]&board.b[black][rook])) return false;
	}
if(board.flag&BQUEENCASTLE)
	{
	if(!(BitPosArray[sE8]&board.b[black][king])) return false;
	if(!(BitPosArray[sA8]&board.b[black][rook])) return false;
	}
return true;
}
