#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#ifdef __GNUC__
#include <unistd.h>
#endif
#include "common.h"
#include "eval.h"

extern short stage,InChkDummy,terminal;

#ifdef HAVE_LIBREADLINE
static char *inputstr;
#else
static char inputstr[128];
#endif

static char cmd[128],file[128],s[128],logfile[128],gamefile[128],userinput[128];

char subcmd[128],setting[128],subsetting[128];

short InputCmd(char *muicmd)
{
short retval=1;
int i;
leaf *ptr; 
int ncmds;
char *trim;

memset(userinput,0,sizeof(userinput));
memset(cmd,0,sizeof(cmd));
strcpy(inputstr,muicmd);
strcat(inputstr,"\n");

//	  printf("%s(%d) %c ",color[board.side],(GameCnt+1)/2 + 1,prompt);
if(inputstr[0]) inputstr[strlen(inputstr)-1]='\000';
cmd[0]='\n';
strcpy(userinput,inputstr);
sscanf(inputstr,"%s %[^\n]",cmd,inputstr);
if(cmd[0]=='\n') return retval;
cmd[0]=subcmd[0]=setting[0]=subsetting[0]='\0';
ncmds=sscanf(userinput,"%s %s %s %[^\n]",
cmd,subcmd,setting,subsetting);
sprintf(inputstr,"%s %s %s",subcmd,setting,subsetting);
trim=inputstr+strlen(inputstr)-1;
while(*trim==' ' && trim>=inputstr)
*trim--='\0';
if(strcmp(cmd,"help")==0) ShowHelp(inputstr);
else if(strcmp(cmd,"show")==0) ShowCmd(inputstr);
else if(strcmp(cmd,"debug")==0)
	{
	SET(flags,DEBUGG);
	Debugmvl=0;
	if(strcmp(inputstr,"debug")==0)
		{
		while(strcmp(inputstr,s))
			{
			sscanf(inputstr,"%s %[^\n]",s,inputstr);
			ptr=ValidateMove(s);
			Debugmv[Debugmvl++]=ptr->move;
			MakeMove(board.side,&ptr->move);
			} 
		i=Debugmvl;
		while(i) UnmakeMove(board.side,&Debugmv[--i]);
		}
	}
else if(strcmp(cmd,"list")==0)
	{
	if(inputstr[0]=='?')
		{
		printf("name    - list known players alphabetically\n");
		printf("score   - list by GNU best result first \n");
		printf("reverse - list by GNU worst result first\n");
		}
	else
		{
		sscanf(inputstr,"%s %[^\n]",cmd,inputstr);
		if(inputstr=='\000') DBListPlayer("rscore");
		else DBListPlayer(inputstr);
		}
	}
else if(strcmp(cmd,"result")==0)
	{
	printf("result: %s\n",inputstr);
	printf("Save to %s\n",gamefile);
	PGNSaveToFile(gamefile,inputstr);
	DBUpdatePlayer(name,inputstr);
	}	
else if(strcmp(cmd,"rating")==0)
	{
	sscanf(inputstr,"%hd %hd",&myrating,&opprating); 
	printf("my rating=%hd,opponent rating=%hd\n",myrating,opprating); 
	if(opprating>=1700) bookfirstlast=2;
	else if(opprating>=1700) bookfirstlast=2;
	else bookfirstlast=2;
	}
else if(strcmp(cmd,"activate")==0)
	{
	CLEAR(flags,TIMEOUT);
	CLEAR(flags,ENDED);
	}
else if(strcmp(cmd,"time")==0)
	{
	sscanf(inputstr,"%s %[^\n]",s,inputstr);
	TimeLimit[1^board.side]=atoi(s) / 100.0f ;
	}
else if(strcmp(cmd,"debugply")==0) DebugPly=atoi(inputstr);
else if(strcmp(cmd,"debugdepth")==0) DebugDepth=atoi(inputstr);
else if(strcmp(cmd,"debugnode")==0) DebugNode=atoi(inputstr);
else if(strcmp(cmd,"hash")==0)
	{
	sscanf(inputstr,"%s %[^\n]",cmd,inputstr);
	if(strcmp(cmd,"off")==0) CLEAR(flags,USEHASH);
	else if(strcmp(cmd,"on")==0) SET(flags,USEHASH);
	printf("Hashing %s\n",flags & USEHASH ? "on" : "off");
	}
else if(strcmp(cmd,"hashsize")==0)
	{
	if(inputstr[0]==0)
		{
		printf("Current HashSize is %d slots\n",HashSize);
		}
	else
		{
		i=atoi(inputstr);
		TTHashMask=0;
		while((i>>=1)>0)
			{
			TTHashMask<<=1;
			TTHashMask|=1;
			}
		HashSize=TTHashMask+1;
		printf("Adjusting HashSize to %d slots\n",HashSize);
		InitHashTable(); 
		}
	}
else if(strcmp(cmd,"level")==0)
	{
	SearchDepth=0;
	sscanf(inputstr,"%hd %f %hd",&TCMove,&TCTime,&TCinc);
	if(TCMove==0)
		{
		TCMove=35 /* MIN((5*(GameCnt+1)/2)+1,60) */;
		printf("TCMove=%d\n",TCMove);
		suddendeath=1;
		}
	else suddendeath=0;
	if(TCTime==0)
		{
		SET(flags,TIMECTL);
		SearchTime=TCinc/2.0f ;
		printf("Fischer increment of %d seconds\n",TCinc);
		}
	else
		{
		SET(flags,TIMECTL);
		MoveLimit[white]=MoveLimit[black]=TCMove-(GameCnt+1)/2;
		TimeLimit[white]=TimeLimit[black]=TCTime*60;
		printf("Time Control: %d moves in %.2f secs\n",
		MoveLimit[white],TimeLimit[white]);
		printf("Fischer increment of %d seconds\n",TCinc);
		}
	}
else if(strcmp(cmd,"epd")==0)
	{
	ParseEPD(inputstr);
	NewPosition();
	ShowBoard();
	printf("\n%s : Best move=%s\n",id,solution); 
	}
else if(strcmp(cmd,"setboard")==0)
	{
	ParseEPD(inputstr);
	NewPosition();
	}
else if(strcmp(cmd,"ping")==0)
	{
	printf("pong %s\n",inputstr);
	fflush(stdout);
	}
else if(strcmp(cmd,"solve")==0||strcmp(cmd,"solveepd")==0) Solve(inputstr);
else if(strcmp(cmd,"st")==0)
	{
	sscanf(inputstr,"%hd",&TCinc);
	suddendeath=0 ;
	SearchTime=TCinc*0.90f;
	CLEAR(flags,TIMECTL);
	}
else retval=0;
return retval;
}

void ShowCmd(char *subcmd)
{
char cmd[10];
sscanf(subcmd,"%s %[^\n]",cmd,subcmd);
if(strcmp(cmd,"rating")==0)
	{
	printf("My rating=%d\n",myrating);
	printf("Opponent rating=%d\n",opprating);
	}
else if(strcmp(cmd,"time")==0) ShowTime();
else if(strcmp(cmd,"moves")==0)
	{
	GenCnt=0;
	TreePtr[2]=TreePtr[1];
	GenMoves(1);      
	ShowMoveList(1);
	printf("No. of moves generated=%ld\n",GenCnt);
	}
else if(strcmp(cmd,"escape")==0)
	{
	GenCnt=0;
	TreePtr[2]=TreePtr[1];
	GenCheckEscapes(1);      
	ShowMoveList(1);
	printf("No. of moves generated=%ld\n",GenCnt);
	}
else if(strcmp(cmd,"noncapture")==0)
	{
	GenCnt=0;
	TreePtr[2]=TreePtr[1];
	GenNonCaptures(1);      
	FilterIllegalMoves(1);
	ShowMoveList(1);
	printf("No. of moves generated=%ld\n",GenCnt);
	}
else if(strcmp(cmd,"capture")==0)
	{
	GenCnt=0;
	TreePtr[2]=TreePtr[1];
	GenCaptures(1);      
	FilterIllegalMoves(1);
	ShowMoveList(1);
	printf("No. of moves generated=%ld\n",GenCnt);
	}
else if(strcmp(cmd,"eval")==0||strcmp(cmd,"score")==0)
	{
	int s,wp,bp,wk,bk;
	short r,c,sq;
	BitBoard *b;
	phase=PHASE;
	GenAtaks();
	FindPins(&pinned);
	hunged[white]=EvalHung(white);
	hunged[black]=EvalHung(black);
	b=board.b[white];
	pieces[white]=b[knight]|b[bishop]|b[rook]|b[queen]; 
	b=board.b[black];
	pieces[black]=b[knight]|b[bishop]|b[rook]|b[queen]; 
	wp=ScoreP(white);
	bp=ScoreP(black);
	wk=ScoreK(white);
	bk=ScoreK(black);
	printf("White:  Mat:%4d/%4d  P:%d  N:%d  B:%d  R:%d  Q:%d  K:%d  Dev:%d  h:%d x:%d\n",board.pmaterial[white],board.material[white],wp,ScoreN(white),ScoreB(white),ScoreR(white),ScoreQ(white),wk,ScoreDev(white),hunged[white],ExchCnt[white]);
	printf("Black:  Mat:%4d/%4d  P:%d  N:%d  B:%d  R:%d  Q:%d  K:%d  Dev:%d  h:%d x:%d\n",board.pmaterial[black],board.material[black],bp,ScoreN(black),ScoreB(black),ScoreR(black),ScoreQ(black),bk,ScoreDev(black),hunged[black],ExchCnt[black]);
	printf("Phase: %d\t",PHASE);
	s=(EvaluateDraw()?DRAWSCORE:Evaluate(-INFINITY,INFINITY));
	printf("score=%d\n\n",s);
//	return;
	for(r=56;r>=0;r-= 8)
		{
		printf("     +---+---+---+---+---+---+---+---+\n");
		printf("   %d |",(r>>3)+1);
		for(c=0;c<8;c++)
			{
			sq=r+c;
			if(cboard[sq]==0) printf("   |");
			else printf("%3d|",pscore[sq]);
			}
		printf("\n");
		}
	printf("     +---+---+---+---+---+---+---+---+\n");
	printf("       a   b   c   d   e   f   g   h  \n");
	}
else if(strcmp(cmd,"game")==0) ShowGame();
else if(strcmp(cmd,"pin")==0)
	{
	BitBoard b;
	GenAtaks();
	FindPins(&b);
	ShowBitBoard(&b);
	}
}

void BookCmd(char *subcmd)
{
char cmd[10];
char bookfile[64];
short bookply;

sscanf(subcmd,"%s %[^\n]",cmd,subcmd);
if(strcmp(cmd,"make")==0) sscanf(subcmd,"%s %hd\n",bookfile,&bookply);
}

static const char *const helpstr[]={
"^C",
" Typically the interrupt key stops a search in progress,",
" makes the move last considered best and returns to the",
" command prompt",
"help",
" Produces a help blurb corresponding to this list of commands.",
"pgnsave FILENAME",
" saves the game so far to the file from memory",
"pgnload FILENAME",
" loads the game in the file into memory",
"name NAME",
" Lets you input your name. Also writes the log.nnn and a",
" corresponding game.nnn file. For details please see",
" auxillary file format sections.",
"activate",
" This command reactivates a game that has been terminated automatically",
" due to checkmate or no more time on the clock. However,it does not",
" alter those conditions. You would have to undo a move or two or",
" add time to the clock with level or time in that case.",
"rating COMPUTERRATING OPPONENTRATING",
" Inputs the estimated rating for computer and for its opponent",
"time",
" Inputs time left in game for computer in hundredths of a second.",
" Mostly used by Internet Chess server.",
"hashsize N",
" Sets the hash table to permit storage of N positions",
"level MOVES MINUTES INCREMENT",
" Sets time control to be MOVES in MINUTES with each move giving",
" an INCREMENT(in seconds,i.e. Fischer-style clock).",
"solve FILENAME",
"solveepd FILENAME",
" Solves the positions in FILENAME",
"show",
" board - displays the current board",
" time - displays the time settings",
" moves - shows all moves using one call to routine",
" escape - shows moves that escape from check using one call to routine",
" noncapture - shows non-capture moves",
" capture - shows capture moves",
" eval [or score] - shows the evaluation per piece and overall",
" game - shows moves in game history",
" pin - shows pinned pieces",
" evalspeed tests speed of the evaluator",
NULL,
NULL
};

void ShowHelp(const char *command)
{
const char * const *p;
int count,len;
if(strlen(command)>0)
	{
	for(p=helpstr,count=0;*p;p++)
		{
		if(strncmp(*p,inputstr,strlen(command))==0)
			{
			puts(*p);
			while(*++p&&**p!= ' ')
			for(;*p &&**p==' ';p++)
				{
				puts(*p);
				}
			return;
			}
		}
	printf("Help for command %s not found\n\n",command);
	}
printf("List of commands:(help COMMAND to get more help)\n");
for(p=helpstr,count=0;*p;p++)
	{
	len=strcspn(*p," ");
	if(len>0)
		{
		count+=printf("%.*s  ",len,*p);
		if(count>60)
			{
			count=0;
			puts("");
			}
		}
	}
puts("");
}

