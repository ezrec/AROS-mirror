#include <clib/alib_protos.h>
#ifdef __AROS__
#include <libraries/mui.h>
#else
#include <mui/mui.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

#define R 2*DEPTH
#define TIMECHECK 1023
#define HISTSCORE(d) ((d)*(d))
#define THREATMARGIN 350

#define FUTSCORE (MATERIAL+fdel)
#define GETNEXTMOVE (InChk[ply]?PhasePick1(&p,ply):PhasePick(&p,ply))

static const int rank7[2]={6,1};
static const int rank6[2]={5,2};
static int ply1score;

int SearchRoot(int depth,int alpha,int beta)
{
int best,score,savealpha;
int side,xside,ply,nodetype;
leaf *p,*pbest;
ply=1;
side=board.side;
xside=1^side;
ChkCnt[2]=ChkCnt[1];
ThrtCnt[2]=ThrtCnt[1];
KingThrt[white][ply]=MateScan(white);
KingThrt[black][ply]=MateScan(black);
InChk[ply]=SqAtakd(board.king[side],xside);
if(InChk[ply]&&ChkCnt[ply]<3*Idepth/DEPTH)
	{
	ChkExtCnt++;
	ChkCnt[ply+1]++;
	depth+=DEPTH;
	}
best=-INFINITY;
savealpha=alpha;
nodetype=PV;
pbest=NULL;
for(p=TreePtr[1];p<TreePtr[2];p++)
	{
	pick(p,1);
	MakeMove(side,&p->move);
	NodeCnt++;
	if(p==TreePtr[1])
		{
		score=-Search(2,depth-DEPTH,-beta,-alpha,nodetype);
		if(beta==INFINITY&&score<=alpha)
			{
			alpha=-INFINITY;
			score=-Search(2,depth-DEPTH,-beta,-alpha,nodetype);
			}
		}
	else
		{
		nodetype=CUT;
		alpha=MAX(best,alpha);
		score=-Search(2,depth-DEPTH,-alpha-1,-alpha,nodetype);
		if(score>best)
			{
			if(alpha<score&&score<beta)
				{
				nodetype=PV;
				score=-Search(2,depth-DEPTH,-beta,-score,nodetype);
				}
			}
		}
	UnmakeMove(xside,&p->move);
	ply1score=p->score=score;
	if(score>best)
		{
		best=score;
		pbest=p;
		if(best>alpha)
			{
			rootscore=best;
			RootPV=p->move;
			if(best>=beta) goto done;
			ShowLine(RootPV,best,'&');
			}
		}
	if(flags&TIMEOUT)
		{
		best=ply&1?rootscore:-rootscore;
		return best;
		}
	if(SearchDepth==0&&(NodeCnt&TIMECHECK)==0)
		{
		GetElapsed();
		if((et>=SearchTime&&(rootscore==-INFINITY-1||ply1score>lastrootscore-25||flags&SOLVE))||et>=maxtime) SETFLAG(flags,TIMEOUT);
		}
	if(best==MATE) return best;
	}
if(best<=savealpha) TreePtr[1]->score=savealpha;

done:
if(best>savealpha) history[side][pbest->move&0x0FFF]+=HISTSCORE(depth/DEPTH);
rootscore=best;
return best;
}

int Search(int ply,int depth,int alpha,int beta,int nodetype)
{
int best,score,nullscore,savealpha;
int side,xside,rc,t0,t1,firstmove;
int fcut,fdel,donull,savenode,nullthreatdone,extend;
leaf *p,*pbest;
int g0,g1;
int upperbound;
if(EvaluateDraw()) return DRAWSCORE;
if(GameCnt>=Game50+3&&Repeat())
	{
	RepeatCnt++;
	return DRAWSCORE;
	}
side=board.side;
xside=1^side;
donull=true;
extend=false;
InChk[ply]=SqAtakd(board.king[side],xside);
if(InChk[ply])
	{
	TreePtr[ply+1]=TreePtr[ply];
	GenCheckEscapes(ply);
	if(TreePtr[ply]==TreePtr[ply+1]) return -MATE+ply-2;
	if(TreePtr[ply]+1==TreePtr[ply+1])
		{
		depth+=DEPTH;
		extend=true;
		OneRepCnt++;
		}
	}
if(rootscore+ply>=MATE) return MATERIAL;
g0=Game[GameCnt].move;
g1=GameCnt>0?Game[GameCnt-1].move:0;
t0=TOSQ(g0);
t1=TOSQ(g1);
ChkCnt[ply+1]=ChkCnt[ply];
ThrtCnt[ply+1]=ThrtCnt[ply];
KingThrt[white][ply]=MateScan(white);
KingThrt[black][ply]=MateScan(black);
if(InChk[ply]&&ply<=2*Idepth/DEPTH)
	{
	ChkExtCnt++;
	ChkCnt[ply+1]++;
	depth+=DEPTH;
	extend=true;
	}
else if(!KingThrt[side][ply-1]&&KingThrt[side][ply]&&ply<=2*Idepth/DEPTH)
	{
	KingExtCnt++;
	extend=true;
	depth+=DEPTH;
	extend=true;
	donull=false;
	}
else if(g0&PROMOTION)
	{
	PawnExtCnt++;
	depth+=DEPTH;
	extend=true;
	}
else if((g0&CAPTURE)&&(board.material[computer]-board.material[1^computer]==RootMaterial))
	{
	RcpExtCnt++;
	depth+=DEPTH;
	extend=true;
	}
else if(depth<=DEPTH&&cboard[t0]==pawn&&(RANK(t0)==rank7[xside]||RANK(t0)==rank6[xside]))
	{
	PawnExtCnt++;
	depth+=DEPTH;
	extend=true;
	}
if(ply>2&&InChk[ply-1]&&cboard[t0]!=king&&t0!=t1&&!SqAtakd(t0,xside))
	{
	HorzExtCnt++;
	depth+=DEPTH;
	extend=true;
	}
if(depth<=0) return Quiesce(ply,alpha,beta);
Hashmv[ply]=0;
upperbound=INFINITY;
if(flags&USEHASH)
	{
	if(rc=TTGet(side,depth,ply,&score,&g1))
		{
		Hashmv[ply]=g1&MOVEMASK;
		switch(rc)
			{
			case POORDRAFT:
				break;
			case EXACTSCORE:
				return score;
			case UPPERBOUND:
				beta=MIN(beta,score);
				upperbound=score;
				donull=false;
				break;
			case LOWERBOUND:
				alpha=score;
				break;
			case QUIESCENT:
				Hashmv[ply]=0;
			}
		if(alpha>=beta) return score;
		}
	}
if(ply>4&&InChk[ply-2]&&InChk[ply-4]) donull=false;
if(flags&USENULL&&g0!=NULLMOVE&&depth>DEPTH&&nodetype!=PV&&!InChk[ply]&&MATERIAL+ValueP>beta&&beta>-MATE+ply&&donull&&board.pmaterial[side]>ValueB&&!threatply)
	{
	TreePtr[ply+1]=TreePtr[ply];
	MakeNullMove(side);
	nullscore=-Search(ply+1,depth-DEPTH-R,-beta,-beta+1,nodetype);
	UnmakeNullMove(xside);
	if(nullscore>=beta)
		{
		NullCutCnt++;
		return nullscore;
		}
	if(depth-DEPTH-R>=1&&MATERIAL>beta&&nullscore<=-MATE+256)
		{
		depth+=DEPTH;
		extend=true;
		}
	}
if(InChk[ply]&&TreePtr[ply]+1<TreePtr[ply+1]) SortMoves(ply);
pickphase[ply]=PICKHASH;
GETNEXTMOVE;
fcut=false;
fdel=MAX(ValueQ,maxposnscore[side]);
if(!extend&&nodetype!=PV&&depth==3*DEPTH&&FUTSCORE<=alpha)
	{
	depth=2*DEPTH;
	RazrCutCnt++;
	}
fdel=MAX(ValueR,maxposnscore[side]);
fcut=(!extend&&nodetype!=PV&&depth==2*DEPTH&&FUTSCORE<=alpha);
if(!fcut)
	{
	fdel=MAX(3*ValueP,maxposnscore[side]);
	fcut =(nodetype!=PV&&depth==DEPTH&&FUTSCORE<=alpha);
	}
MakeMove(side,&p->move);
NodeCnt++;
g0=g1=0;
while((g0=SqAtakd(board.king[side],xside))>0||(fcut&&FUTSCORE<alpha&&!SqAtakd(board.king[xside],side)&&!MateScan(xside)))
	{
	if(g0==0) g1++;
	UnmakeMove(xside,&p->move);
	if(GETNEXTMOVE==false)
	return g1?Evaluate(alpha,beta):DRAWSCORE;
	MakeMove(side,&p->move);
	NodeCnt++;
	}
firstmove=true;
pbest=p;
best=-INFINITY;
savealpha=alpha;
nullthreatdone=false;
nullscore=INFINITY;
savenode=nodetype;
if(nodetype!=PV) nodetype=(nodetype==CUT)?ALL:CUT;
while(1)
	{
	if(firstmove)
		{
		firstmove=false;
		score=-Search(ply+1,depth-DEPTH,-beta,-alpha,nodetype);
		}
	else
		{
		if(GETNEXTMOVE==false) break;
		MakeMove(side,&p->move);
		NodeCnt++;
		if(SqAtakd(board.king[side],xside)) 
			{
			UnmakeMove(xside,&p->move);
			continue;
			}
		if(fcut&&FUTSCORE<=alpha&&!SqAtakd(board.king[xside],side)&&!MateScan(xside))
			{
			UnmakeMove(xside,&p->move);
			FutlCutCnt++;
			NodeCnt--;
			continue;
			}
		NodeCnt++;
		if(nodetype==PV) nodetype=CUT;
		alpha=MAX(best,alpha);
		score=-Search(ply+1,depth-DEPTH,-alpha-1,-alpha,nodetype);
		if(score>best)
			{
			if(savenode==PV) nodetype=PV;
			if(alpha<score&&score<beta) score=-Search(ply+1,depth-DEPTH,-beta,-score,nodetype);
			if(nodetype==PV&&score<=alpha&&Game[GameCnt+1].move==NULLMOVE) score=-Search(ply+1,depth-DEPTH,-alpha,INFINITY,nodetype);
			}
		}
	UnmakeMove(xside,&p->move);
	if(score>best)
		{
		best=score;
		pbest=p;
		if(best>=beta) goto done;
		}
	if(flags&TIMEOUT)
		{
		best=(ply&1?rootscore:-rootscore);
		return best;
		}
	if(SearchDepth==0&&(NodeCnt&TIMECHECK)==0)
		{
		GetElapsed();
		if((et>=SearchTime&&(rootscore==-INFINITY-1||ply1score>lastrootscore-25||flags&SOLVE))||et>=maxtime) SETFLAG(flags,TIMEOUT);
		}
	if(MATE+1==best+ply) goto done;
	}

done:

/* if(upperbound<best) printf("Inconsistencies %d %d\n",upperbound,best); */
if(flags&USEHASH) TTPut(side,depth,ply,savealpha,beta,best,pbest->move);
if(best>savealpha) history[side][pbest->move&0x0FFF]+=HISTSCORE(depth/DEPTH);
if(!(pbest->move &(CAPTURE | PROMOTION))&&best>savealpha)
	{
	if(killer1[ply]==0) killer1[ply]=pbest->move&MOVEMASK;
	else if((pbest->move&MOVEMASK)!=killer1[ply]) killer2[ply]=pbest->move&MOVEMASK;
	}
return best;
}

void ShowLine(int move,int score,char c)
{
int i;
int pvar[MAXPLYDEPTH];
char text[200];

if(!(flags&POST)) return;
if(NodeCnt<500000&&(flags&SOLVE)) return;
if(Idepth==DEPTH&&c=='&') return;
if(rootscore==-INFINITY-1) return;
GetElapsed();
text[0]=0;

if(score>MATE-255)
	{
	sprintf(text,"%d: %7.2f  \033bMate in %d\033n",Idepth/DEPTH,et,(MATE+2-abs(score))/2);
	}
else if(score<-MATE+255)
	{
	sprintf(text,"%d: %7.2f  \033bMate in %d\033n",Idepth/DEPTH,et,(MATE+2-abs(score))/2);
	}
else
	{
	sprintf(text,"%d: %7.2f \033b%d\033n",Idepth/DEPTH,et,score);
	}
if(c!='-')
	{
	strcat(text,"   ");
	if(c=='+')
		{
		SANMove(RootPV,1);
		strcat(text,board.side==white?MUIX_PH:MUIX_PT);
		strcat(text,SANmv);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		}
	else
		{
		SANMove(RootPV,1);
		strcat(text,board.side==white?MUIX_PH:MUIX_PT);
		strcat(text,SANmv);
		MakeMove(board.side,&RootPV);
		TreePtr[3]=TreePtr[2];
		GenMoves(2);
		i=2;
		pvar[1]=RootPV;
		if((flags&USEHASH))
			{
			while(TTGetPV(board.side,i,rootscore,&pvar[i]))
				{
				if((MATESCORE(score)&&abs(score)==MATE+2-i)||Repeat()) break;
				SANMove(pvar[i],i);
				strcat(text," ");
				strcat(text,board.side==white?MUIX_PH:MUIX_PT);
				strcat(text,SANmv);
				MakeMove(board.side,&pvar[i]);
				TreePtr[i+2]=TreePtr[i+1];
				GenMoves(++i);
				}
			}
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		for(--i;i;i--) UnmakeMove(board.side,&pvar[i]);
		}
	}
}
