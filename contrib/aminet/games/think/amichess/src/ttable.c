#include <string.h>
#include "common.h"

void TTPut(int side,int depth,int ply,int alpha,int beta,int score,int move)
{
HashSlot *t;
depth=depth/DEPTH;
t=HashTab[side]; 
t+=((HashKey&TTHashMask)&-2);
if(depth<t->depth&&!MATESCORE(score)) t++;
else if(t->flag&&t->key!=KEY(HashKey)) *(t+1)=*t;
if(t->flag) CollHashCnt++;
TotalPutHashCnt++;
t->move=move;
t->key=KEY(HashKey);
t->depth=depth;
if(t->depth==0) t->flag=QUIESCENT;
else if(score>=beta) t->flag=LOWERBOUND;         
else if(score<=alpha) t->flag=UPPERBOUND;
else t->flag=EXACTSCORE;
if(MATESCORE(score)) t->score=score+(score>0?ply:-ply);
else t->score=score;
}

int TTGet(int side,int depth,int ply,int *score,int *move)
{
HashSlot *t;
KeyType Key;
TotalGetHashCnt++;
t=HashTab[side]; 
t+=((HashKey&TTHashMask)&-2);
Key=KEY(HashKey);
if(Key!=t->key&&Key!=(++t)->key) return 0;
depth=depth/DEPTH;
GoodGetHashCnt++;
*move=t->move;
*score=t->score;
if(t->depth==0) return QUIESCENT;
if(t->depth<depth&&!MATESCORE(t->score)) return POORDRAFT;
if(MATESCORE(*score)) *score-=(*score>0?ply:-ply);
return t->flag;
}

int TTGetPV(int side,int ply,int score,int *move)
{
HashSlot *t;
KeyType Key;
int s;
t=HashTab[side]; 
t+=((HashKey&TTHashMask)&-2);
Key=KEY(HashKey);
s=t->score;
if(MATESCORE(s)) s-=(s>0?ply:-ply);
if(Key==t->key &&((ply&1&&score==s)||(!(ply&1)&&score==-s)))
	{
	*move=t->move;
	return 1;
	}
t++;
s=t->score;
if(MATESCORE(s)) s-=(s>0?ply:-ply);
if(Key==t->key &&((ply&1&&score==s)||(!(ply&1)&&score==-s)))
	{
	*move=t->move;
	return 1;
	}
return 0;
}

void TTClear()
{
memset(HashTab[white],0,HashSize*sizeof(HashSlot));
memset(HashTab[black],0,HashSize*sizeof(HashSlot));
}

void PTClear()
{
memset(PawnTab[white],0,PAWNSLOTS*sizeof(PawnSlot));
memset(PawnTab[black],0,PAWNSLOTS*sizeof(PawnSlot));
}
