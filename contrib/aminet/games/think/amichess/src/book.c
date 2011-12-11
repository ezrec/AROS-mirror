#include <clib/alib_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "book.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

#define MAXMOVES 200
#define MAXMATCH 100

#define wins_off 0
#define losses_off 2
#define draws_off 4
#define key_off 6

static int bookcnt;
static HashType posshash[MAXMOVES];

static int book_allocated=0;

#define MAGIC_LENGTH 5

static const char magic_str[]="\x42\x23\x08\x15\x03";

static int check_magic(FILE *f)
{
char buf[MAGIC_LENGTH];
return fread(&buf,1,MAGIC_LENGTH,f)==MAGIC_LENGTH&&memcmp(buf,magic_str,MAGIC_LENGTH)==0;
}

static int write_magic(FILE *f)
{
return (fwrite(&magic_str,1,MAGIC_LENGTH,f)!=MAGIC_LENGTH)?BOOK_EIO:BOOK_SUCCESS;
}

static int write_size(FILE *f,unsigned int size)
{
if(fwrite(&size,sizeof(size),1,f)==1) return BOOK_SUCCESS;
return BOOK_EIO;
}

static unsigned int read_size(FILE *f)
{
unsigned int size=0;
if(fread(&size,sizeof(size),1,f)!=1) return 0;
return size;
}

#define MAX_DIGEST_BITS 20

static int digest_bits;

#define DIGEST_SIZE (1<<digest_bits)
#define DIGEST_MASK (DIGEST_SIZE-1)

static struct hashtype
{
unsigned short wins;
unsigned short losses;
unsigned short draws;
HashType key;
} *bookpos;

#ifdef __STORM__
__inline int is_empty(unsigned int index)
#else
static inline int is_empty(unsigned int index)
#endif
{
return bookpos[index].key==0&&bookpos[index].wins==0&&bookpos[index].draws==0&&bookpos[index].losses==0;
}

#ifdef __GNUC__
#define DIGEST_START(key) ((key)&DIGEST_MASK)
#define DIGEST_MATCH(i,the_key) ((the_key)==bookpos[i].key)
#define DIGEST_EMPTY(i) is_empty(i)
#define DIGEST_COLLISION(i,key) (!DIGEST_MATCH(i,key)&&!DIGEST_EMPTY(i))
#define DIGEST_NEXT(i,key) (((i)+(((key)>>digest_bits)|1))&DIGEST_MASK)
#else

static int DIGEST_START(HashType key)
{
#ifdef HAVE64BIT
return key&DIGEST_MASK;
#else
return key.lo&DIGEST_MASK;
#endif
}

static int DIGEST_MATCH(int i,HashType key)
{
return key==bookpos[i].key;
}

static int DIGEST_EMPTY(int i)
{
return is_empty(i);
}

static int DIGEST_COLLISION(int i,HashType key)
{
struct hashtype *b=&bookpos[i];
return key!=bookpos[i].key&&(b->key||b->wins||b->draws||b->losses);
}

static int DIGEST_NEXT(int i,HashType key)
{
HashType t=key>>digest_bits;
#ifdef HAVE64BIT
return (i+(((int)t)|1))&DIGEST_MASK;
#else
return (i+(t.lo|1))&DIGEST_MASK;
#endif
}

#endif

static int bookhashcollisions=0;

#define DIGEST_LIMIT ((95*DIGEST_SIZE)/100)

static int compare(const void *aa,const void *bb)
{
int ret;
const leaf *a=(const leaf *)aa;
const leaf *b=(const leaf *)bb;
if(b->score>a->score) ret=1;
else if(b->score<a->score) ret=-1;
else ret=0;
return ret;
}

static int read_book(FILE *f)
{
struct hashtype buf;
if(book_allocated)
	{
	free(bookpos);
	book_allocated=0;
	}
bookpos=(struct hashtype *)calloc(DIGEST_SIZE,sizeof(struct hashtype));
if(!bookpos) return BOOK_ENOMEM;
book_allocated=1;
bookcnt=0;
bookhashcollisions=0;
while(fread(&buf,sizeof(buf),1,f)==1)
	{
	int i;
	for(i=DIGEST_START(buf.key);DIGEST_COLLISION(i,buf.key);i=DIGEST_NEXT(i,buf.key)) bookhashcollisions++;
	if(i<DIGEST_SIZE)
		{
		bookpos[i].wins+=buf.wins;
		bookpos[i].draws+=buf.draws;
		bookpos[i].losses+=buf.losses;
		bookpos[i].key=buf.key;
		bookcnt++;
		}
	else printf("Illegal Memory Access %u\n",i);
	}
return BOOK_SUCCESS;
}

int BookBuilderOpen()
{
FILE *rfp,*wfp;
int res;
char text[100];
if((rfp=fopen(BOOKRUN,"rb")))
	{
	DoMethod(mui_app,MUIM_Chess_ShowThinking,"Opened existing book!");
	if(!check_magic(rfp))
		{
		sprintf(text,"File %s does not conform to the current format. Consider rebuilding your book.",BOOKRUN);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		return BOOK_EFORMAT;
		}
	digest_bits=MAX_DIGEST_BITS;
	read_size(rfp);
	res=read_book(rfp);
	fclose(rfp);
	if(res!=BOOK_SUCCESS) return res;
	sprintf(text,"Read %d book positions. Got %d hash collisions.",bookcnt,bookhashcollisions);
	DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
	}
else
	{
	if(!(wfp=fopen(BOOKRUN,"wb"))) /* Original src with "w+b" but didnt work with noixemul */
		{
		sprintf(text,"Could not create %s file",BOOKRUN);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		return BOOK_EIO;
		}
	if(write_magic(wfp)!=BOOK_SUCCESS)
		{
		sprintf(text,"Could not write to %s",BOOKRUN);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		return BOOK_EIO;
		}
	if(fclose(wfp))
		{
		sprintf(text,"Could not write to %s",BOOKRUN);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		return BOOK_EIO;
		}
	sprintf(text,"Created new book %s!",BOOKRUN);
	DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
	if(!(rfp=fopen(BOOKRUN,"rb")))
		{
		sprintf(text,"Could not open %s for reading",BOOKRUN);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		return BOOK_EIO;
		}
	digest_bits=MAX_DIGEST_BITS;
	if(read_book(wfp)==BOOK_ENOMEM) return BOOK_ENOMEM;
	}
return BOOK_SUCCESS;
}

int BookBuilder(int result,int side)
{
unsigned int i;
if(GameCnt>BOOKDEPTH) return BOOK_EMIDGAME;
CalcHashKey();
for(i=DIGEST_START(HashKey);;i=DIGEST_NEXT(i,HashKey))
	{
	if(HashKey==bookpos[i].key)
		{
		existpos++;
		break;
		}
	else if(DIGEST_EMPTY(i))
		{
		if(bookcnt>DIGEST_LIMIT) return BOOK_EFULL;
		bookpos[i].key=HashKey;
		newpos++;
		bookcnt++;
		break;
		}
	else bookhashcollisions++;
	}
if(side==white)
	{
	if(result==R_WHITE_WINS) bookpos[i].wins++;
	else if(result==R_BLACK_WINS) bookpos[i].losses++;
	else if(result==R_DRAW) bookpos[i].draws++;
	}
else
	{
	if(result==R_WHITE_WINS) bookpos[i].losses++;
	else if(result==R_BLACK_WINS) bookpos[i].wins++;
	else if(result==R_DRAW) bookpos[i].draws++;
	}
return BOOK_SUCCESS;
}

int BookBuilderClose()
{
FILE *wfp;
unsigned int i;
int errcode=BOOK_SUCCESS;
char text[100];
if(!(wfp=fopen(BOOKRUN,"wb")))
	{
	errcode=BOOK_EIO;
	goto bailout_noclose;
	}
if(write_magic(wfp)!=BOOK_SUCCESS)
	{
	errcode=BOOK_EIO;
	goto bailout;
	}
if(write_size(wfp,bookcnt)!=BOOK_SUCCESS)
	{
	errcode=BOOK_EIO;
	goto bailout;
	}
for(i=0;i<DIGEST_SIZE;i++)
	{
	if(!is_empty(i))
		{
		if(fwrite(&bookpos[i],sizeof(struct hashtype),1,wfp)!=1)
			{
			errcode=BOOK_EIO;
			goto bailout;
			}
		}
	}
sprintf(text,"Got %d hash collisions.",bookhashcollisions);
DoMethod(mui_app,MUIM_Chess_ShowThinking,text);

bailout:
if(fclose(wfp)) errcode=BOOK_EIO;

bailout_noclose:
free(bookpos);
book_allocated=0;
bookloaded=0;
return errcode;
}

int BookQuery()
{
int i,j,k,icnt=0,mcnt,found,maxdistribution;
int matches[MAXMATCH] ;
leaf m[MAXMOVES];
leaf pref[MAXMOVES];
struct {
unsigned short wins;
unsigned short losses;
unsigned short draws;
} r[MAXMOVES];
FILE *rfp=0;
leaf *p;
int side,xside,temp;
unsigned int booksize;
int res;
char text[100];
if(bookloaded&&!book_allocated) return BOOK_ENOBOOK;
if(!bookloaded)
	{
	bookloaded=1;
	if(!(rfp=fopen(BOOKBIN,"rb"))) return BOOK_ENOBOOK;
	sprintf(text,"Read opening book (%s)...",BOOKBIN);
	DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
	if(!check_magic(rfp))
		{
		sprintf(text,"File %s does not conform to the current format. Consider rebuilding the book.",BOOKBIN);
		DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
		return BOOK_EFORMAT;
		}
	booksize=(read_size(rfp)*106)/100;
	for(digest_bits=1;booksize;booksize>>=1) digest_bits++;
	res=read_book(rfp);
	if(res!=BOOK_SUCCESS) return res;
	sprintf(text,"%d hash collisions... ",bookhashcollisions);
	DoMethod(mui_app,MUIM_Chess_ShowThinking,text);
	}
mcnt=-1;
side=board.side;
xside=1^side;
TreePtr[2]=TreePtr[1];
GenMoves(1);
FilterIllegalMoves(1);
for(p=TreePtr[1];p<TreePtr[2];p++)
	{
	MakeMove(side,&p->move);
	m[icnt].move=p->move;
	posshash[icnt]=HashKey;
	icnt++;
	UnmakeMove(xside,&p->move);
	}
for(i=0;i<icnt;i++)
	{
	for(j=DIGEST_START(posshash[i]);!DIGEST_EMPTY(j);j=DIGEST_NEXT(j,posshash[i]))
		{
		if(DIGEST_MATCH(j,posshash[i]))
			{
			found=0;
			for(k=0;k<mcnt;k++) if(matches[k]==i)
				{
				found=1;
				break;
				}
			if(!found)
				{
				matches[++mcnt]=i;
				pref[mcnt].move=m[i].move;
				r[i].losses=bookpos[j].losses;
				r[i].wins=bookpos[j].wins;
				r[i].draws=bookpos[j].draws;
				pref[mcnt].score=m[i].score=100*(r[i].wins+(r[i].draws/2))/(MAX(r[i].wins+r[i].losses+r[i].draws,1))+r[i].wins/2;
				}
			if(mcnt>=MAXMATCH) goto fini;
			break;
			}
		}
	}

fini:  
if(mcnt==-1) return BOOK_ENOMOVES;
k=0;
if(mcnt+1)
	{
	if(bookmode==BOOKRAND)
		{
/*    		int tot; */
		k=rand();
		k=k%(mcnt+1);
		RootPV=m[matches[k]].move;
/*		printf("\n(Random picked move #%d %s%s from above list)\n",k,algbr[FROMSQ(RootPV)],algbr[TOSQ(RootPV)]);
		tot=r[matches[k]].wins+r[matches[k]].draws+r[matches[k]].losses;
		if(tot) printf("B p=%2.0f\n",100.0*(r[matches[k]].wins+r[matches[k]].draws)/tot);
		else printf("p=NO EXPERIENCES\n");
*/
		}
	else if(bookmode==BOOKBEST)
		{
		qsort(&pref,mcnt+1,sizeof(leaf),compare);
		RootPV=pref[0].move;
		}
	else if(bookmode==BOOKWORST)
		{
		qsort(&pref,mcnt+1,sizeof(leaf),compare);
		RootPV=pref[mcnt].move;
		}
	else if(bookmode==BOOKPREFER)
		{
		qsort(&pref,mcnt+1,sizeof(leaf),compare);
		for(i=0;i<=mcnt;i++) m[i].move=pref[i].move;
		temp=(bookfirstlast>mcnt+1?mcnt+1:bookfirstlast);
		maxdistribution=0;
		for(i=0;i<temp;i++) maxdistribution+=pref[i].score;
		if(!maxdistribution) return BOOK_ENOMOVES;
		k=rand()%maxdistribution;
		maxdistribution=0;
		for(i=0;i<temp;i++)
			{
			maxdistribution+=pref[i].score;
			if(k>=maxdistribution-pref[i].score&&k<maxdistribution)
				{
				k=i;
				RootPV=m[k].move;
				break;
				}
			}
		}
	}
return BOOK_SUCCESS;
}
