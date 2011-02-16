#include <stdio.h>
#include "common.h"

#define WEIGHT 12
#define HASHSORTSCORE INFINITY
#define KILLERSORTSCORE 1000
#define CASTLINGSCORE 500

void SortCaptures(int ply)
{
leaf *p;
int temp,f,t;
for(p=TreePtr[ply];p<TreePtr[ply+1];p++)
	{
	f=Value[cboard[FROMSQ(p->move)]];
	t=Value[cboard[TOSQ(p->move)]];
	if(f<t) p->score=t-f;
	else
		{
		temp=SwapOff(p->move);
		p->score=(temp<0?-INFINITY:temp);
		}
	}
}

void SortMoves(int ply)
{
leaf *p;
int f,t,m,tovalue;
int side,xside;
BitBoard enemyP;
side=board.side;
xside=1^side;
enemyP=board.b[xside][pawn];
for(p=TreePtr[ply];p<TreePtr[ply+1];p++)
	{
	p->score=-INFINITY;
	f=FROMSQ(p->move);
	t=TOSQ(p->move);
	m=p->move&MOVEMASK;
	if(m==Hashmv[ply]) p->score+=HASHSORTSCORE;
	else if(cboard[t]||p->move&PROMOTION)
		{
		tovalue=(Value[cboard[t]]+Value[PROMOTEPIECE(p->move)]);
		p->score+=tovalue+ValueK-Value[cboard[f]];
		}
	else if(m==killer1[ply]||m==killer2[ply])
	p->score+=KILLERSORTSCORE;
	else if(ply > 2 &&(m==killer1[ply-2]||m==killer2[ply-2]))
	p->score+=KILLERSORTSCORE;
	p->score+=history[side][(p->move&0x0FFF)]+taxicab[f][sD5]-taxicab[t][sE4];
	if(cboard[f]==pawn)
		{
		if((enemyP&PassedPawnMask[side][t])==NULLBITBOARD) p->score+=50;
		} 
	}
}

void SortRoot(void)
{
leaf *p;
int f,t ;
int side,xside;
BitBoard enemyP;
side=board.side;
xside=1^side;
enemyP=board.b[xside][pawn];
for(p=TreePtr[1];p<TreePtr[2];p++)
	{
	f=Value[cboard[FROMSQ(p->move)]];
	if(cboard[TOSQ(p->move)]||(p->move&PROMOTION))
		{
		t=Value[cboard[TOSQ(p->move)]];
		if(f<t) p->score=-1000+t-f;
		else p->score=-1000+SwapOff(p->move);
		}
	else p->score=-3000+SwapOff(p->move);
	p->score+=taxicab[FROMSQ(p->move)][sD5]-taxicab[TOSQ(p->move)][sE4];
	if(f==ValueP)
		{
		if((enemyP&PassedPawnMask[side][TOSQ(p->move)])==NULLBITBOARD) p->score+=50;
		}
	}
}

void pick(leaf *head,int ply)
{
int best;
leaf *p,*pbest,tmp;
best=head->score;
pbest=head;
for(p=head+1;p<TreePtr[ply+1];p++) 
	{
	if(p->score > best)
		{
		pbest=p;
		best=p->score;
		}
	}
tmp=*head;
*head=*pbest;
*pbest=tmp;
}

int PhasePick(leaf **p1,int ply)
{
static leaf* p[MAXPLYDEPTH];
leaf *p2;
int mv;
int side;
side=board.side;
switch(pickphase[ply])
	{
	case PICKHASH:
		TreePtr[ply+1]=TreePtr[ply];
		pickphase[ply]=PICKGEN1;
		if(Hashmv[ply]&&IsLegalMove(Hashmv[ply]))
			{
			TreePtr[ply+1]->move=Hashmv[ply];
			*p1=TreePtr[ply+1]++;
			return true;
			}
	case PICKGEN1:
		pickphase[ply]=PICKCAPT;
		p[ply]=TreePtr[ply+1];
		GenCaptures(ply);
		for(p2=p[ply];p2<TreePtr[ply+1];p2++) p2->score=SwapOff(p2->move)*WEIGHT+Value[cboard[TOSQ(p2->move)]];
	case PICKCAPT:
		while(p[ply]<TreePtr[ply+1])
			{
			pick(p[ply],ply);
			if((p[ply]->move&MOVEMASK)==Hashmv[ply])
			{
				p[ply]++;
				continue;
				} 
			*p1=p[ply]++;
			return true;
			}
	case PICKKILL1:
		pickphase[ply]=PICKKILL2;
		if(killer1[ply] && killer1[ply]!=Hashmv[ply]&&IsLegalMove(killer1[ply]))
			{
			TreePtr[ply+1]->move=killer1[ply];
			*p1=TreePtr[ply+1];
			TreePtr[ply+1]++;
			return true;
			}
	case PICKKILL2:
		pickphase[ply]=PICKGEN2;
		if(killer2[ply]&&killer2[ply]!=Hashmv[ply]&&IsLegalMove(killer2[ply]))
			{
			TreePtr[ply+1]->move=killer2[ply];
			*p1=TreePtr[ply+1];
			TreePtr[ply+1]++;
			return true;
			}
	case PICKGEN2:
		pickphase[ply]=PICKREST;
		p[ply]=TreePtr[ply+1];
		GenNonCaptures(ply);
		for(p2=p[ply];p2<TreePtr[ply+1];p2++)
			{
			p2->score=history[side][(p2->move&0x0FFF)]+taxicab[FROMSQ(p2->move)][sD5]-taxicab[TOSQ(p2->move)][sE4];
			if(p2->move&CASTLING)
			p2->score+=CASTLINGSCORE;
			}
	
	case PICKREST:
		while(p[ply]<TreePtr[ply+1])
			{
			pick(p[ply],ply);
			mv=p[ply]->move&MOVEMASK;
			if(mv==Hashmv[ply]||mv==killer1[ply]||mv==killer2[ply])
				{
				p[ply]++;
				continue;
				}
			*p1=p[ply]++;
			return true;
			}
	}
return false;
} 

int PhasePick1(leaf **p1,int ply)
{
static leaf* p[MAXPLYDEPTH];
switch(pickphase[ply])
	{
	case PICKHASH:
		pickphase[ply]=PICKREST;
		p[ply]=TreePtr[ply];
	case PICKREST:
		while(p[ply]<TreePtr[ply+1])
			{
			pick(p[ply],ply);
			*p1=p[ply]++;
			return true;
			}
	}
return false;
}
