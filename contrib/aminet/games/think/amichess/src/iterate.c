#include <clib/alib_protos.h>
#include <devices/timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

void gettimeofday(struct timeval *t);
void PlaySound(Object *snd,BOOL);
void MoveSound(char *);

extern Object *snd_mate;
extern Object *snd_smate;
extern Object *snd_draw;

#define WINDOW	75

static struct timeval t1,t2;

void Iterate()
{
int side;
int score,RootAlpha,RootBeta;
char text[50];
side=board.side;
computerplays=board.side;
lazyscore[white]=lazyscore[black]=100;
maxposnscore[white]=maxposnscore[black]=100;
GenCnt=0;
NodeCnt=QuiesCnt=0;
EvalCnt=EvalCall=0;
OneRepCnt=ChkExtCnt=RcpExtCnt=PawnExtCnt=HorzExtCnt=ThrtExtCnt=0;
KingExtCnt=0;
NullCutCnt=FutlCutCnt=0;
TotalGetHashCnt=GoodGetHashCnt=0;
TotalPutHashCnt=CollHashCnt=0;
TotalPawnHashCnt=GoodPawnHashCnt=0;
RootPawns=nbits(board.b[white][pawn]|board.b[black][pawn]);
RootPieces=nbits(board.friends[white]|board.friends[black])-RootPawns;
RootMaterial=MATERIAL;
RepeatCnt=0;
et=0;
memset(ChkCnt,0,sizeof(ChkCnt));
memset(ThrtCnt,0,sizeof(ThrtCnt));
memset(history,0,sizeof(history));
memset(killer1,0,sizeof(killer1));
memset(killer2,0,sizeof(killer2));
TTClear();
CLEARFLAG(flags,TIMEOUT);
if(flags&TIMECTL)
	{
	SearchTime=(TimeLimit[side]-2)/MoveLimit[side];
	SearchTime=95*SearchTime/100;
	if(suddendeath) SearchTime=92*SearchTime/100;
	if(nmovesfrombook<=3) SearchTime=1.5*SearchTime;
	if(TCinc) if(SearchTime<TCinc)
		{
		char *color[2]={"White","Black"};
/*		printf("TimeLimit[%s]=%6.2f\n",color[side],TimeLimit[side]); */
		if(TimeLimit[side]>30) SearchTime=TCinc;
		}
	}
Idepth=0;
TreePtr[2]=TreePtr[1];
GenMoves(1);
FilterIllegalMoves(1); 
SortRoot();
gettimeofday(&t1);
InChk[1]=SqAtakd(board.king[side],1^side);
if(!GenCnt)
	{
	if(InChk[1])
		{
		if(computerplays==black) DoMethod(mui_app,MUIM_Chess_ShowThinking,"1-0: Computer loses as black");
		else if(computerplays==white) DoMethod(mui_app,MUIM_Chess_ShowThinking,"0-1: Computer loses as white");
		}
	else DoMethod(mui_app,MUIM_Chess_ShowThinking,"0.5-0.5: Stalemate");
	SETFLAG(flags,TIMEOUT|ENDED);
	return;
	}
else if(GenCnt==1)
	{
	RootPV=TreePtr[1]->move;
	SETFLAG(flags,TIMEOUT);
	}
lastrootscore=score=Evaluate(-INFINITY,INFINITY);
wasbookmove=0;
if(bookmode!=BOOKOFF&&!(flags&SOLVE)&&nmovesfrombook<=3)
	{
	if(BookQuery()==BOOK_SUCCESS)
		{
		nmovesfrombook=0;
		wasbookmove=1;
		SETFLAG(flags,TIMEOUT);
		}
	else nmovesfrombook++;
	}
else nmovesfrombook++;
maxtime=4*SearchTime;
/*
if(flags&POST)
	{
	printf("Root=%d,",score);
	printf("Phase=%d ",phase);
	}
if(!SearchDepth)
	{
	if(flags&POST) printf("\nTime=%.2f,Max=%.2f,Left=%.2f,Moves=%d\n",SearchTime,maxtime,TimeLimit[side],MoveLimit[side]);
	}
*/
while(!(flags&TIMEOUT)) 
	{
	threatply=0;
	if(score>MATE-255)
		{
		RootAlpha=score-1;
		RootBeta=MATE;
		}
	else if(score<-MATE+255)
		{
		RootAlpha=-MATE;
		RootBeta=score+1;
		}
	else
		{
		RootAlpha=MAX(score-WINDOW,-MATE);
		RootBeta=MIN(score+WINDOW,MATE);
		}
	Idepth+=DEPTH;
	rootscore=-INFINITY-1;
	score=SearchRoot(Idepth,RootAlpha,RootBeta);
	if(score>=RootBeta&&score<MATE&&!(flags&TIMEOUT))
		{
		ShowLine(RootPV,score,'+');
		rootscore=-INFINITY-1;
		RootAlpha=RootBeta;
		RootBeta=INFINITY;
		score=SearchRoot(Idepth,RootAlpha,RootBeta);
		}
	else 
		{
		if(score<=RootAlpha&&!(flags&TIMEOUT))
			{
			ShowLine(RootPV,score,'-');
			rootscore=-INFINITY-1;
			RootBeta=RootAlpha;
			RootAlpha=-INFINITY;
			score=SearchRoot(Idepth,RootAlpha,RootBeta);
			}
		}
	ShowLine(RootPV,score,'.');
	lastrootscore=score;
	if(!SearchDepth&&(flags&TIMECTL)&&et>=2*SearchTime/3) SETFLAG(flags,TIMEOUT);
	if(abs(score)+Idepth>=MATE+1) SETFLAG(flags,TIMEOUT);
	if(Idepth==SearchDepth*DEPTH) break; 
	}
SANMove(RootPV,1);
strcpy(Game[GameCnt+1].SANmv,SANmv);
Game[GameCnt+1].et=et;
MakeMove(side,&RootPV);
if(flags&TIMECTL)
	{
	if(suddendeath)
		{
		if(TimeLimit[side]>0&&TimeLimit[side]<=60) MoveLimit[side]=60;
		else MoveLimit[side]=35;
/*
		printf("MoveLimit is %d\n",MoveLimit[side]);
		printf("TimeLimit is %f\n",TimeLimit[side]);
*/
		}
	else MoveLimit[side]--;
	TimeLimit[side]-=(float)et;
	if(TCinc) TimeLimit[side]+=TCinc;
	if(!MoveLimit[side]) MoveLimit[side]=TCMove;
	}
if(!wasbookmove)
	{
	DoMethod(mui_app,MUIM_Chess_Stats);
/*
	printf("\nTime=%.1f Rate=%ld Nodes=[%ld/%ld/%ld] GenCnt=%ld\n",et,et>0?(unsigned long)((NodeCnt+QuiesCnt)/et):0,NodeCnt,QuiesCnt,NodeCnt+QuiesCnt,GenCnt);
	printf("\nTime=%.1f Rate=%ld Nodes=[%ld/%ld/%ld] GenCnt=%ld\n",et,et>0?(unsigned long)((NodeCnt+QuiesCnt)/et):0,NodeCnt,QuiesCnt,NodeCnt+QuiesCnt,GenCnt);
	printf("Eval=[%ld/%ld] RptCnt=%ld NullCut=%ld FutlCut=%ld\n",EvalCnt,EvalCall,RepeatCnt,NullCutCnt,FutlCutCnt);
	printf("Ext: Chk=%ld Recap=%ld Pawn=%ld OneRep=%ld Horz=%ld Mate=%ld KThrt=%ld\n",ChkExtCnt,RcpExtCnt,PawnExtCnt,OneRepCnt,HorzExtCnt,ThrtExtCnt,KingExtCnt);
	printf("Material=[%d/%d:%d/%d] ",board.pmaterial[white],board.pmaterial[black],board.material[white],board.material[black]);
	printf("Lazy=[%d/%d] ",lazyscore[white],lazyscore[black]);
	printf("MaxPosnScore=[%d/%d]\n",maxposnscore[white],maxposnscore[black]);
	printf("Hash: Success=%ld%% Collision=%ld%% Pawn=%ld%%\n",GoodGetHashCnt*100/(TotalGetHashCnt+1),CollHashCnt*100/(TotalPutHashCnt+1),GoodPawnHashCnt*100/(TotalPawnHashCnt+1));
*/
	}
else if(wasbookmove) DoMethod(mui_app,MUIM_Chess_ShowThinking,"From book.");
DoMethod(mui_app,MUIM_Chess_MyMove,SANmv);
sprintf(text,"%d: %-5s  %2.2f",GameCnt/2+1,SANmv,et);
DoMethod(mui_app,MUIM_Chess_AddMove,side,text);
if(!(flags&SOLVE)) DoMethod(mui_app,MUIM_Chess_ShowBoard);
MoveSound(SANmv);
TreePtr[2]=TreePtr[1];
GenMoves(1);
FilterIllegalMoves(1); 
if(TreePtr[1]==TreePtr[2])
	{
	if(SqAtakd(board.king[board.side],1^board.side))
		{
		if(computerplays==black) DoMethod(mui_app,MUIM_Chess_ShowThinking,"0-1: Computer wins as black");
		else DoMethod(mui_app,MUIM_Chess_ShowThinking,"1-0: Computer wins as white");
		}
	else
		{
		DoMethod(mui_app,MUIM_Chess_ShowThinking,"0.5-0.5: Stalemate");
		PlaySound(snd_smate,0);
		}
	SETFLAG(flags,ENDED);
	}
if(EvaluateDraw()||Repeat()>=2)
	{
	DoMethod(mui_app,MUIM_Chess_ShowThinking,"0.5-0.5: Draw");
	PlaySound(snd_draw,0);
	SETFLAG(flags,ENDED);
	}
DoMethod(mui_app,MUIM_Chess_Side);
} 

void GetElapsed()
{
gettimeofday(&t2);
et=(t2.tv_secs-t1.tv_secs)+((long)t2.tv_micro-(long)t1.tv_micro)/1000000.0;
}

