#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

/*
void ShowMoveList(int ply)
{
leaf *node;
int i=0;
for(node=TreePtr[ply];node<TreePtr[ply+1];node++)
	{
	SANMove(node->move,ply);
	printf("%5s %3d\t",SANmv,SwapOff(node->move));
	if(++i==5)
		{
		printf("\n");
		i=0;
		}
	}
printf("\n");
} 
*/

/*
void ShowBitBoard(BitBoard *b)
{
int r,c;
printf("\n");
for(r=56;r>=0;r-=8)
	{
	for(c=0;c<8;c++)
		{
		if(*b&BitPosArray[r+c]) printf("1 ");
		else printf(". ");
		}
	printf("\n");
	}
printf("\n");
}
*/

/*
void ShowGame()
{
int i;
if(GameCnt>=0)
	{
	printf("      White   Black\n");
	if((board.side==white&& GameCnt%2==1)||(board.side==black&&GameCnt%2==0))
		{
		for(i=0;i<=GameCnt;i+=2)
			{
			printf("%3d.  %-7s %-7s\n",i/2+1,Game[i].SANmv,Game[i+1].SANmv);
			}
		}
	else
		{
		printf("  1.          %-7s\n",Game[0].SANmv);
		for(i=1;i<=GameCnt;i+=2)
			{
			printf("%3d.  %-7s %-7s\n",i/2+2,Game[i].SANmv,Game[i+1].SANmv);
			}
		}
	printf("\n");
	}
}*/

