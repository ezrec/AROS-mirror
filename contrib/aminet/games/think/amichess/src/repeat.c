#include "common.h"

int Repeat()
{
int i,k;
k=0;
for(i=GameCnt-3;i>=Game50;i-=2)
	{
	if(Game[i].hashkey==HashKey) k++;
	}
return k;
}
