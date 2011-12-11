#include "common.h"

void CalcHashKey()
{
int sq,piece,color;
BitBoard b;
PawnHashKey=HashKey=0;
for(color=white;color<=black;color++)
	{
	for(piece=pawn;piece<=king;piece++)
		{
		b=board.b[color][piece];
		while(b)
			{
			sq=leadz(b);
			CLEARBIT(b,sq);
			HashKey^=hashcode[color][piece][sq];
			if(piece==pawn) PawnHashKey^=hashcode[color][piece][sq];
			}
		}
	}
if(board.ep>=0) HashKey^=ephash[board.ep];
if(board.flag&WKINGCASTLE) HashKey^=WKCastlehash;
if(board.flag&WQUEENCASTLE) HashKey^=WQCastlehash;
if(board.flag&BKINGCASTLE) HashKey^=BKCastlehash;
if(board.flag&BQUEENCASTLE) HashKey^=BQCastlehash;
if(board.side==black) HashKey^=Sidehash;
}

