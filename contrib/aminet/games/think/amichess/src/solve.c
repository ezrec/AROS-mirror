#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

#ifdef __AROS__
#include <proto/intuition.h>
#endif

void Solve(char *file)
{
short total,correct,found;
long TotalNodes;
char *p;
total=correct=0;
TotalNodes=0;
SETFLAG(flags,SOLVE); 
while(ReadEPDFile(file,0))
	{
	NewPosition();
	total++;
	DoMethod(mui_app,MUIM_Chess_ShowBoard);
	Iterate();
	TotalNodes+=NodeCnt+QuiesCnt;
	p=solution;
	found=false;
	while(*p)
		{
		if(!strncmp(p,SANmv,strlen(SANmv)))
			{
			correct++;
			found=true;
			break;
			}
		while(*p!=' ' && *p) p++;
		while(*p==' ' && *p) p++;
		}
	printf("%s : ",id);
	printf(found?"Correct:  " : "Incorrect:  ");
	printf("%s %s\n",SANmv,solution);
	printf("Correct=%d Total=%d\n",correct,total);
	}
printf("\nTotal nodes = %ld\n",TotalNodes);
CLEARFLAG(flags,SOLVE);
}

