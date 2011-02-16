#include <clib/alib_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

void MakeMove(int side,int *move)
{
BitBoard *a;
int f,t,fpiece,tpiece; 
int rookf,rookt,epsq,sq;
int xside;
GameRec *g;
xside=1^side;
f=FROMSQ(*move);
t=TOSQ(*move);
fpiece=cboard[f];
tpiece=cboard[t];
a=&board.b[side][fpiece];
CLEARBIT(*a,f);
SETBIT(*a,t);
CLEARBIT(board.blockerr90,r90[f]);
SETBIT(board.blockerr90,r90[t]);
CLEARBIT(board.blockerr45,r45[f]);
SETBIT(board.blockerr45,r45[t]);
CLEARBIT(board.blockerr315,r315[f]);
SETBIT(board.blockerr315,r315[t]);
cboard[f]=empty;
cboard[t]=fpiece;
GameCnt++;
g=&Game[GameCnt];
g->epsq=board.ep; 
g->bflag=board.flag;
g->Game50=Game50;
g->hashkey=HashKey;
g->phashkey=PawnHashKey;
g->mvboard=Mvboard[t];
Mvboard[t]=Mvboard[f]+1; 
Mvboard[f]=0;
if(board.ep>-1) HashKey^=ephash[board.ep];
HashKey^=hashcode[side][fpiece][f];
HashKey^=hashcode[side][fpiece][t];
if(fpiece==king) board.king[side]=t;
if(fpiece==pawn)
	{
	PawnHashKey^=hashcode[side][pawn][f];
	PawnHashKey^=hashcode[side][pawn][t];
	}
if(tpiece)
	{
	ExchCnt[side]++;
	CLEARBIT(board.b[xside][tpiece],t);
	*move|=(tpiece<<15);
	HashKey^=hashcode[xside][tpiece][t];
	if(tpiece==pawn) PawnHashKey^=hashcode[xside][pawn][t];
	board.material[xside]-=Value[tpiece];
	if(tpiece!=pawn) board.pmaterial[xside]-=Value[tpiece];
	}
if(*move&PROMOTION)
	{
	SETBIT(board.b[side][PROMOTEPIECE(*move)],t);
	CLEARBIT(*a,t);
	cboard[t]=PROMOTEPIECE(*move);
	HashKey^=hashcode[side][pawn][t];
	HashKey^=hashcode[side][cboard[t]][t];
	PawnHashKey^=hashcode[side][pawn][t];
	board.material[side]+=(Value[cboard[t]]-ValueP);
	board.pmaterial[side]+=Value[cboard[t]];
	}
if(*move&ENPASSANT)
	{
	ExchCnt[side]++;
	epsq=board.ep+(side==white?-8:8);
	CLEARBIT(board.b[xside][pawn],epsq);
	CLEARBIT(board.blockerr90,r90[epsq]);
	CLEARBIT(board.blockerr45,r45[epsq]);
	CLEARBIT(board.blockerr315,r315[epsq]);
	cboard[epsq]=empty;
	HashKey^=hashcode[xside][pawn][epsq];
	PawnHashKey^=hashcode[xside][pawn][epsq];
	board.material[xside]-=ValueP;
	}
if(*move&(CAPTURE|CASTLING)||fpiece==pawn) Game50=GameCnt;
if(*move&CASTLING)
	{
	if(t&0x04)
		{
		rookf=t+1;
		rookt=t-1;
		}
	else
		{
		rookf=t-2;
		rookt=t+1;
		}
	a=&board.b[side][rook];
	CLEARBIT(*a,rookf);
	SETBIT(*a,rookt);
	CLEARBIT(board.blockerr90,r90[rookf]);
	SETBIT(board.blockerr90,r90[rookt]);
	CLEARBIT(board.blockerr45,r45[rookf]);
	SETBIT(board.blockerr45,r45[rookt]);
	CLEARBIT(board.blockerr315,r315[rookf]);
	SETBIT(board.blockerr315,r315[rookt]);
	cboard[rookf]=empty;
	cboard[rookt]=rook;
	Mvboard[rookf]=0;
	Mvboard[rookt]=1;
	HashKey^=hashcode[side][rook][rookf];
	HashKey^=hashcode[side][rook][rookt];
	board.castled[side]=true;
	}
if(side==white)
	{
	if(fpiece==king&&board.flag&WCASTLE)
		{
		if(board.flag&WKINGCASTLE) HashKey^=WKCastlehash;
		if(board.flag&WQUEENCASTLE) HashKey^=WQCastlehash;
		board.flag&=~WCASTLE;
		}
	else if(fpiece==rook)
		{
		if(f==sH1)
			{
			if(board.flag&WKINGCASTLE) HashKey^=WKCastlehash;
			board.flag&=~WKINGCASTLE;
			}
		else if(f==sA1) 
			{
			if(board.flag&WQUEENCASTLE) HashKey^=WQCastlehash;
			board.flag&=~WQUEENCASTLE;
			}
		}
	if(tpiece==rook)
		{
		if(t==sH8) 
			{
			if(board.flag&BKINGCASTLE) HashKey^=BKCastlehash;
			board.flag&=~BKINGCASTLE;
			}
		else if(t==sA8) 
			{
			if(board.flag&BQUEENCASTLE) HashKey^=BQCastlehash;
			board.flag&=~BQUEENCASTLE;
			}
		}
	}
else
	{
	if(fpiece==king&&board.flag&BCASTLE)
		{
		if(board.flag&BKINGCASTLE) HashKey^=BKCastlehash;
		if(board.flag&BQUEENCASTLE) HashKey^=BQCastlehash;
		board.flag&=~BCASTLE;
		}
	else if(fpiece==rook)
		{
		if(f==sH8) 
			{
			if(board.flag&BKINGCASTLE) HashKey^=BKCastlehash;
			board.flag&=~BKINGCASTLE;
			}
		else if(f==sA8) 
			{
			if(board.flag&BQUEENCASTLE) HashKey^=BQCastlehash;
			board.flag&=~BQUEENCASTLE;
			}
		}
	if(tpiece==rook)
		{
		if(t==sH1) 
			{
			if(board.flag&WKINGCASTLE) HashKey^=WKCastlehash;
			board.flag&=~WKINGCASTLE;
			}
		else if(t==sA1) 
			{
			if(board.flag&WQUEENCASTLE) HashKey^=WQCastlehash;
			board.flag&=~WQUEENCASTLE;
			}
		}
	}
if(fpiece==pawn&&abs(f-t)==16)
	{
	sq=(f+t)/2;
	board.ep=sq;
	HashKey^=ephash[sq];
	}
else board.ep=-1;
board.side=xside;
HashKey^=Sidehash;
UpdateFriends();
g->move=*move;
}

void UnmakeMove(int side,int *move)
{
BitBoard *a;
int f,t,fpiece,cpiece;   
int rookf,rookt,epsq;
int xside;
GameRec *g;
side=1^side;
xside=1^side;
f=FROMSQ(*move);
t=TOSQ(*move);
fpiece=cboard[t];
cpiece=CAPTUREPIECE(*move);
a=&board.b[side][fpiece];
CLEARBIT(*a,t);
SETBIT(*a,f);   
CLEARBIT(board.blockerr90,r90[t]); 
SETBIT(board.blockerr90,r90[f]); 
CLEARBIT(board.blockerr45,r45[t]); 
SETBIT(board.blockerr45,r45[f]); 
CLEARBIT(board.blockerr315,r315[t]); 
SETBIT(board.blockerr315,r315[f]); 
cboard[f]=cboard[t];
cboard[t]=empty;
g=&Game[GameCnt];
Mvboard[f]=Mvboard[t]-1;
Mvboard[t]=g->mvboard;
if(fpiece==king)
board.king[side]=f;
if(*move&CAPTURE)
	{
	ExchCnt[side]--;
	SETBIT(board.b[xside][cpiece],t);  
	SETBIT(board.blockerr90,r90[t]);  
	SETBIT(board.blockerr45,r45[t]);  
	SETBIT(board.blockerr315,r315[t]);  
	cboard[t]=cpiece; 
	board.material[xside]+=Value[cpiece];  
	if(cpiece!=pawn)
	board.pmaterial[xside]+=Value[cpiece];  
	}
if(*move&PROMOTION)
	{
	CLEARBIT(*a,f);
	SETBIT(board.b[side][pawn],f);  
	cboard[f]=pawn;
	board.material[side]+=(ValueP-Value[PROMOTEPIECE(*move)]); 
	board.pmaterial[side]-=Value[PROMOTEPIECE(*move)];
	}
if(*move&ENPASSANT)
	{
	ExchCnt[side]--;
	epsq =(side==white?g->epsq-8:g->epsq+8);
	SETBIT(board.b[xside][pawn],epsq); 
	SETBIT(board.blockerr90,r90[epsq]); 
	SETBIT(board.blockerr45,r45[epsq]); 
	SETBIT(board.blockerr315,r315[epsq]); 
	cboard[epsq]=pawn;
	board.material[xside]+=ValueP;  
	}   
if(*move&CASTLING)
	{
	if(t&0x04)
		{
		rookf=t+1;
		rookt=t-1;
		}
	else
		{
		rookf=t-2;
		rookt=t+1;
		}
	a=&board.b[side][rook];
	CLEARBIT(*a,rookt);
	SETBIT(*a,rookf); 
	CLEARBIT(board.blockerr90,r90[rookt]);
	SETBIT(board.blockerr90,r90[rookf]);
	CLEARBIT(board.blockerr45,r45[rookt]);
	SETBIT(board.blockerr45,r45[rookf]);
	CLEARBIT(board.blockerr315,r315[rookt]);
	SETBIT(board.blockerr315,r315[rookf]);
	cboard[rookf]=rook;
	cboard[rookt]=empty;
	Mvboard[rookf]=0;
	Mvboard[rookt]=0;	
	board.castled[side]=false;
	}
UpdateFriends();
board.side=side;
board.ep=g->epsq;
board.flag=g->bflag;  
HashKey=g->hashkey;
PawnHashKey=g->phashkey;
Game50=g->Game50;
GameCnt--;
}

void SANMove(int move,int ply)
{
int piece,side,ambiguous;
int f,t;
BitBoard b;
leaf *node1;
char *s;
side=board.side;
s=SANmv;
f=FROMSQ(move);
t=TOSQ(move);
if(move&CASTLING)
	{
	if(t==6||t==62) strcpy(s,"O-O");
	else strcpy(s,"O-O-O");
	return;
	}
piece=cboard[f];
side=board.side;
b=board.b[side][piece];
ambiguous=false;
node1=TreePtr[ply];
if(nbits(b)>1)
	{
	for(node1=TreePtr[ply];node1<TreePtr[ply+1];node1++)
		{
		if(FROMSQ(node1->move)==f) continue;
		if(TOSQ(node1->move)!=t) continue;
		if(cboard[FROMSQ(node1->move)]!=piece) continue;
		ambiguous=true;
		break;
		}
	}
if(piece==pawn)
	{
	if(cboard[t]||board.ep==t)
		{
		*s++=algbrfile[ROW(f)];
		*s++='x';
		}
	strcpy(s,algbr[t]);
	s+=2;
	if(move&PROMOTION)
		{
		*s++='=';
		*s++=notation[PROMOTEPIECE(move)];
		}
	}
else
	{
	*s++=notation[piece];
	if(ambiguous)
		{
		if(ROW(f)==ROW(FROMSQ(node1->move))) *s++=algbrrank[RANK(f)];
		else *s++=algbrfile[ROW(f)];
		}
	if(cboard[t]) *s++='x'; 
	strcpy(s,algbr[t]);
	s+=2;
	}
MakeMove(side,&move);
if(SqAtakd(board.king[1^side],side))
	{
	TreePtr[ply+2]=TreePtr[ply+1];
	GenCheckEscapes(ply+1);
	if(TreePtr[ply+1]==TreePtr[ply+2]) *s++='#';
	else *s++='+';
	GenCnt-=TreePtr[ply+2]-TreePtr[ply+1];
	}
UnmakeMove(1^side,&move); 
*s=0;
}

#define ASCIITOFILE(a) ((a)-'a')
#define ASCIITORANK(a) ((a)-'1')
#define ASCIITOSQ(a,b) (ASCIITOFILE(a))+(ASCIITORANK(b))*8
#define ATOH(a)((a)>='a'&&(a)<='h')
#define ITO8(a)((a)>='1'&&(a)<='8')

leaf *ValidateMove(char *s)
{
int f,t,side,rank,file,fileto;
int piece,kount;
char mvstr[64],*p;
char text[100];
BitBoard b;
leaf *n1,*n2;
TreePtr[2]=TreePtr[1];
GenMoves(1);   
FilterIllegalMoves(1);	
side=board.side;
p=mvstr;
do
	{
	if(*s!='x'&&*s!='+'&&*s!='=') *p++=*s; 
	}
while(*s++);
if(mvstr[strlen(mvstr)-1]=='+'||mvstr[strlen(mvstr)-1]=='#'||mvstr[strlen(mvstr)-1]=='=') mvstr[strlen(mvstr)-1]=0;
if(strcmp(mvstr,"O-O")==0||strcmp(mvstr,"o-o")==0||strcmp(mvstr,"0-0")==0)
	{
	if(side==white)
		{
		f=4;
		t=6;
		}
	else 
		{
		f=60;
		t=62;
		}
	return IsInMoveList(1,f,t,' ');
	}
if(strcmp(mvstr,"O-O-O")==0||strcmp(mvstr,"o-o-o")==0||strcmp(mvstr,"0-0-0")==0)
	{
	if(side==white)
		{
		f=4;
		t=2;
		}
	else
		{
		f=60;
		t=58;
		}
	return IsInMoveList(1,f,t,' ');
	}
if(ATOH(mvstr[0])&&ITO8(mvstr[1])&&ATOH(mvstr[2])&&ITO8(mvstr[3]))
	{
	f=ASCIITOSQ(mvstr[0],mvstr[1]);
	t=ASCIITOSQ(mvstr[2],mvstr[3]);
	piece=(strlen(mvstr)==5?mvstr[4]:' ');
	return IsInMoveList(1,f,t,piece);
	}
if(ATOH(mvstr[0]))
	{
	if(ITO8(mvstr[1]))
		{
		t=ASCIITOSQ(mvstr[0],mvstr[1]);
		f=t+(side==white?-8:8);
		if(f>0&&f<64)
			{
			if(BitPosArray[f]&board.b[side][pawn])
				{
				if(mvstr[2]) return IsInMoveList(1,f,t,mvstr[2]);
				else return IsInMoveList(1,f,t,' ');
				}
			f=t+(side==white?-16:16);
			if(f>0&&f<64)
				{
				if(BitPosArray[f]&board.b[side][pawn]) return IsInMoveList(1,f,t,' ');
				}
			}
		}
	else if(ATOH(mvstr[1])&&ITO8(mvstr[2]))
		{
		t=ASCIITOSQ(mvstr[1],mvstr[2]);
		rank=ASCIITORANK(mvstr[2])+(side==white?-1:1);
		f=rank*8+ASCIITOFILE(mvstr[0]);
		piece=(strlen(mvstr)==4?mvstr[3]:' ');
		return IsInMoveList(1,f,t,piece);
		}
	else if(ATOH(mvstr[1]))
		{
		file=ASCIITOFILE(mvstr[0]);
		fileto=ASCIITOFILE(mvstr[1]);
		b=board.b[side][pawn]&FileBit[file];
		if(side==white) b=b>>(fileto<file?7:9);
		else b=b<<(fileto<file?9:7);
		if(board.ep>-1) b=b&(board.friends[1^side]|BitPosArray[board.ep]);
		else b=b&(board.friends[1^side]);
		switch(nbits(b))
			{
			case 0:
				return 0;
			case 1:
				t=leadz(b);
				f=t-(side==white?8:-8)+(file-fileto);
				piece=(strlen(mvstr)==3?mvstr[2]:' ');
				return IsInMoveList(1,f,t,piece);
			default:
				sprintf(text,"Ambiguous move: %s %s",s,mvstr);
				DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
/*				DoMethod(mui_app,MUIM_Chess_ShowBoard); */
				return 0;
			}
		} 
	}
else if(strchr("NBRQK",mvstr[0]))
	{
	piece=empty;
	if(mvstr[0]=='N') piece=knight;
	else if(mvstr[0]=='B') piece=bishop;
	else if(mvstr[0]=='R') piece=rook;
	else if(mvstr[0]=='Q') piece=queen;
	else if(mvstr[0]=='K') piece=king;
	b=board.b[side][piece];
	t=-1;
	if(ITO8(mvstr[1]))
		{
		rank=ASCIITORANK(mvstr[1]);
		b&=RankBit[rank];
		t=ASCIITOSQ(mvstr[2],mvstr[3]);
		}
	else if(ATOH(mvstr[1])&&ATOH(mvstr[2]))
		{
		file=ASCIITOFILE(mvstr[1]);
		b&=FileBit[file];
		t=ASCIITOSQ(mvstr[2],mvstr[3]);
		}
	else if(ATOH(mvstr[1])&&ITO8(mvstr[2])) t=ASCIITOSQ(mvstr[1],mvstr[2]);
	kount=0;
	n1=0;
	n2=0;
	while(b)
		{
		f=leadz(b);
		CLEARBIT(b,f);
		if(n1=IsInMoveList(1,f,t,' '))
			{
			n2=n1;
			kount++;
			}
		}
	if(kount>1)
		{
		sprintf(text,"Ambiguous move: %s %s",s,mvstr);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
/*		DoMethod(mui_app,MUIM_Chess_ShowBoard); */
		return 0;
		}
	if(kount==0) return 0;
	return n2;
	}
return 0;
}

leaf *IsInMoveList(int ply,int f,int t,char piece)
{
leaf *node;
for(node=TreePtr[ply];node<TreePtr[ply+1];node++)
	{
	if((int)(node->move&0x0FFF)==MOVE(f,t)&&toupper(piece)==notation[PROMOTEPIECE(node->move)]) return node;
	}
return 0;
}

int IsLegalMove(int move)
{
int f,t,piece,side;
BitBoard blocker,enemy;
f=FROMSQ(move); 
t=TOSQ(move);
if(cboard[f]==empty) return false;
side=board.side;
if(!(BitPosArray[f]&board.friends[side])) return false;
if(BitPosArray[t]&board.friends[side]) return false;
piece=cboard[f];
if((move &(PROMOTION|ENPASSANT))&&piece!=pawn) return false;
if((move&ENPASSANT)&&t!=board.ep) return false;
if((move&CASTLING)&&piece!=king) return false; 
blocker=board.blocker;
if(piece==pawn)
	{
	if((move&ENPASSANT)&&board.ep>-1) enemy=board.friends[1^side]|BitPosArray[board.ep];
	else enemy=board.friends[1^side];
	if(side==white)
		{
		if(!(MoveArray[pawn][f]&BitPosArray[t]&enemy)&&!(t-f==8&&cboard[t]==empty)&&!(t-f==16&&RANK(f)==1&&!(FromToRay[f][t]&blocker))) return false;
		}
	else if(side==black)
		{
		if(!(MoveArray[bpawn][f]&BitPosArray[t]&enemy)&&!(t-f==-8&&cboard[t]==empty)&&!(t-f==-16&&RANK(f)==6&&!(FromToRay[f][t]&blocker))) return false;
		}
	}
else if(piece==king)
	{
	if(side==white)
		{
		if(!(MoveArray[piece][f]&BitPosArray[t])&&!(f==sE1&&t==sG1&&board.flag&WKINGCASTLE&&!(FromToRay[sE1][sG1]&blocker)&&!SqAtakd(sE1,black)&&!SqAtakd(sF1,black))&&!(f==sE1&&t==sC1&&board.flag&WQUEENCASTLE&&
		!(FromToRay[sE1][sB1]&blocker)&&!SqAtakd(sE1,black)&&!SqAtakd(sD1,black))) return false;
		}
	if(side==black)
		{
		if(!(MoveArray[piece][f]&BitPosArray[t])&&!(f==sE8&&t==sG8&&board.flag&BKINGCASTLE&&!(FromToRay[sE8][sG8]&blocker)&&!SqAtakd(sE8,white)&&!SqAtakd(sF8,white))&&!(f==sE8&&t==sC8&&board.flag&BQUEENCASTLE&&
		!(FromToRay[sE8][sB8]&blocker)&&!SqAtakd(sE8,white)&&!SqAtakd(sD8,white))) return false;
		}
	}
else 
	{
	if(!(MoveArray[piece][f]&BitPosArray[t])) return false;
	}
if(slider[piece])
	{
	if(FromToRay[f][t]&NotBitPosArray[t]&blocker) return false;
	}
return true;
}

char *AlgbrMove(int move)
{
static char s[6];
int f,t;
f=FROMSQ(move);
t=TOSQ(move);
strcpy(s,algbr[f]);
strcpy(s+2,algbr[t]);
if(move&PROMOTION)
	{
	/* s[4]=lnotation[PROMOTEPIECE(move)]; */
	s[4]=notation[PROMOTEPIECE(move)];
	s[5]=0;
	}
else s[4]=0;
return s;
}

