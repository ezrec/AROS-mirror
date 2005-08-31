#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

#define EPDCLOSE 1

int ReadEPDFile(const char *file,int op)
{
static FILE *fp=0;
char line[1025];
if(!fp)
	{
	if(!(fp=fopen(file,"r")))
		{
		sprintf(line,"Error opening file %s",file);
#if defined(__PPC__) && !defined(__AROS__)
		FromPPCDoMethod(MUIM_Chess_ShowThinking,line);
#else
		DoMethod(mui_app,MUIM_Chess_ShowThinking,line);
#endif
		return false;
		}
	}
if(op==EPDCLOSE)   
	{ 
	fclose(fp);
	fp=0;
	return false;
	}

next_line:
fgets(line,1024,fp);
if(!feof(fp)) 
	{
	int ret=ParseEPD(line);
	if(ret!=EPD_SUCCESS) goto next_line;
/*	if(op!=2) printf("\n%s : Best move=%s\n",id,solution); */
	return true;
	}
fclose(fp);
fp=0;
return false;
}

int ParseEPD(char *p)
{
int r,c,sq;
char *str_p;
r=56;
c=0;
memset(&board,0,sizeof(board));
while(p&&*p!=' ')
	{
	sq=r+c;
	switch(*p)
		{
		case 'P':
			SETBIT(board.b[white][pawn],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[white]+=ValueP;
			break;	
		case 'N' :
			SETBIT(board.b[white][knight],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[white]+=ValueN;
			break;	
		case 'B':
			SETBIT(board.b[white][bishop],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[white]+=ValueB;
			break;
		case 'R':
			SETBIT(board.b[white][rook],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[white]+=ValueR;
			break;	
		case 'Q' :
			SETBIT(board.b[white][queen],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[white]+=ValueQ;
			break;	
		case 'K':
			SETBIT(board.b[white][king],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			break;	
		case 'p':
			SETBIT(board.b[black][pawn],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[black]+=ValueP;
			break;	
		case 'n':
			SETBIT(board.b[black][knight],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[black]+=ValueN;
			break;	
		case 'b':
			SETBIT(board.b[black][bishop],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[black]+=ValueB;
			break;	
		case 'r':
			SETBIT(board.b[black][rook],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[black]+=ValueR;
			break;	
		case 'q':
			SETBIT(board.b[black][queen],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			board.material[black]+=ValueQ;
			break;	
		case 'k':
			SETBIT(board.b[black][king],sq);
			SETBIT(board.blockerr90,r90[sq]);
			SETBIT(board.blockerr45,r45[sq]);
			SETBIT(board.blockerr315,r315[sq]);
			break;	
		case '/':
			r-=8;
			c=-1;
		}
	if(isdigit(*p)) c+=(*p-'0');
	else c++;
	if(r<0||c>8) return EPD_ERROR;
	if(c==8&&p[1]!='/'&&p[1]!=' ') return EPD_ERROR;
	p++;
	}
board.pmaterial[white]=board.material[white]-nbits(board.b[white][pawn])*ValueP;
board.pmaterial[black]=board.material[black]-nbits(board.b[black][pawn])*ValueP;
board.king[white]=leadz(board.b[white][king]);
board.king[black]=leadz(board.b[black][king]);
UpdateFriends(); 
UpdateCBoard();
UpdateMvboard();
if(!++p) return EPD_ERROR;
if(*p=='w') board.side=white; 
else if(*p=='b') board.side=black;
else return EPD_ERROR;
if(!++p||*p!=' '||!++p) return EPD_ERROR;
while(p&&*p!=' ')
	{
	if(*p=='K') board.flag|=WKINGCASTLE;
	else if(*p=='Q') board.flag|=WQUEENCASTLE;
	else if(*p=='k') board.flag|=BKINGCASTLE;
	else if(*p=='q') board.flag|=BQUEENCASTLE;
	else if(*p=='-')
		{
		p++;
		break;
		}
	else return EPD_ERROR;
	p++;
	}
if(!p||*p!=' '||!++p) return EPD_ERROR;
if(*p!='-')
	{
	if(!p[1]||*p<'a'||*p>'h'||!(p[1]=='3'||p[1]=='6')) return EPD_ERROR;
	board.ep=(*p-'a')+(p[1]-'1')*8;
	p++;
	}
else board.ep=-1;
solution[0]=0;
id[0]=0;
if(!++p) return EPD_SUCCESS;
str_p=strstr(p,"bm");
if(str_p) sscanf(str_p,"bm %63[^;];",solution); 
str_p=strstr(p,"id");
if(str_p) sscanf(p,"id %31[^;];",id);
CalcHashKey ();
phase=PHASE;
return EPD_SUCCESS;
}

void LoadEPD(char *p)
{
char file[32];
int N=1;
sscanf(p,"%s %d ",file,&N);
if(strcmp(file,"next")==0) ReadEPDFile(file,0);
else
	{ 
	ReadEPDFile(file,1);
	while(--N)
		{
		if(ReadEPDFile(file,2)==false)
			{
#if defined(__PPC__) && !defined(__AROS__)
			FromPPCDoMethod(MUIM_Chess_ShowThinking,"File position exceeded.");
#else
			DoMethod(mui_app,MUIM_Chess_ShowThinking,"File position exceeded.");
#endif
			return; 
			}
		}
	ReadEPDFile(file,0);
	}
#if defined(__PPC__) && !defined(__AROS__)
FromPPCDoMethod(MUIM_Chess_ShowBoard);
#else
DoMethod(mui_app,MUIM_Chess_ShowBoard);
#endif
NewPosition();
}

void SaveEPD(char *file)
{
FILE *fp;
int r,c,sq,k;
char c1;
if(fp=fopen(file,"a"))
	{
	for(r=A8;r>=A1;r-=8)
		{
		k=0;
		for(c=0;c<8;c++)
			{
			sq=r+c;
			if(cboard[sq]==empty) k++;
			else
				{
				if(k) fprintf(fp,"%1d",k);
				k=0;
				c1=notation[cboard[sq]];
				if(BitPosArray[sq]&board.friends[black]) c1=tolower(c1);
				fprintf(fp,"%c",c1);
				}
			}
		if(k) fprintf(fp,"%1d",k);
		if(r>A1) fprintf(fp,"/");
		}
	fprintf(fp,(board.side==white?" w ":" b "));
	if(board.flag&WKINGCASTLE) fprintf(fp,"K");
	if(board.flag&WQUEENCASTLE) fprintf(fp,"Q");
	if(board.flag&BKINGCASTLE) fprintf(fp,"k");
	if(board.flag&BQUEENCASTLE) fprintf(fp,"q");
	if(!(board.flag&(WCASTLE|BCASTLE))) fprintf(fp,"-");
	fprintf(fp," %s",(board.ep>-1?algbr[board.ep]:"-"));
	fprintf(fp," bm 1; id 1;");
	fprintf(fp,"\n");
	fclose(fp);
	}
else
#if defined(__PPC__) && !defined(__AROS__)
FromPPCDoMethod(MUIM_Chess_ShowThinking,"Can't save file.");
#else
DoMethod(mui_app,MUIM_Chess_ShowThinking,"Can't save file.");
#endif
}
