#include "common.h"

int EvalHung(int side)
{
BitBoard c,n,b,r,q;
int xside=1^side;
hunged[side]=0;
n=(Ataks[xside][pawn]&board.b[side][knight]);
n|=(Ataks[xside][0]&board.b[side][knight]&~Ataks[side][0]);
b=(Ataks[xside][pawn]&board.b[side][bishop]);
b|=(Ataks[xside][0]&board.b[side][bishop]&~Ataks[side][0]);
r=Ataks[xside][pawn]|Ataks[xside][knight]|Ataks[xside][bishop];
r=(r&board.b[side][rook]);
r|=(Ataks[xside][0]&board.b[side][rook]&~Ataks[side][0]);
q=Ataks[xside][pawn]|Ataks[xside][knight]|Ataks[xside][bishop]|Ataks[xside][rook];
q=(q&board.b[side][queen]);
q|=(Ataks[xside][0] & board.b[side][queen]&~Ataks[side][0]);
c=n|b|r|q;
if(c) hunged[side]+=nbits(c);
if(Ataks[xside][0]&board.b[side][king]) hunged[side]++;
return hunged[side];
}
