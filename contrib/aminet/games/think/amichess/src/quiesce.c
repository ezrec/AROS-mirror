#include "common.h"

int Quiesce(int ply,int alpha,int beta)
{
int side,xside;
int best,delta,score,savealpha;
leaf *p,*pbest;
if(EvaluateDraw()) return DRAWSCORE;
side=board.side;
xside=1^side;
InChk[ply]=SqAtakd(board.king[side],xside);
best=Evaluate(alpha,beta);
if(best>=beta&&!InChk[ply]) return best;
TreePtr[ply+1]=TreePtr[ply];
if(InChk[ply])
	{
	GenCheckEscapes(ply);
	if(TreePtr[ply]==TreePtr[ply+1]) return -MATE+ply-2;
	if(best>=beta) return best;
	SortMoves(ply);
	}
else
	{
	GenCaptures(ply);
	if(TreePtr[ply]==TreePtr[ply+1]) return best;
	SortCaptures(ply);
	}

savealpha=alpha;
pbest=0;
alpha=MAX(best,alpha);
delta=MAX(alpha-150-best,0);
for(p=TreePtr[ply];p<TreePtr[ply+1];p++)
	{
	pick(p,ply);
	if(!InChk[ply]&&SwapOff(p->move)<delta) continue;
	if(p->score==-INFINITY) continue;
	MakeMove(side,&p->move);
	QuiesCnt++;
	if(SqAtakd(board.king[side],xside))
		{
		UnmakeMove(xside,&p->move);
		continue;
		}
	score=-Quiesce(ply+1,-beta,-alpha);
	UnmakeMove(xside,&p->move);
	if(score>best)
		{
		best=score;
		pbest=p;
		if(best>=beta) goto done;
		alpha=MAX(alpha,best);
		}
	}

done:
if(flags&USEHASH&&pbest) TTPut(side,0,ply,savealpha,beta,best,pbest->move);
return best;
}
