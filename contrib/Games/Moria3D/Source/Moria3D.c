/****************************************************************************
*                                                                           *
* Moria3D.c                                                          970319 *
*                                                                           *
* Main Body. This binds everything to a running prog.                       *
*                                                                           *
****************************************************************************/

#include <dos/dos.h>
#include <exec/tasks.h>
#include <proto/exec.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MyTypes.h"
#include "Level.h"
#include "Init.h"
#include "Clean.h"
#include "Gfx.h"

void WaitKey(void);

int main(int arcc,char ** argv)
{
    int16     	Pad;
    int16     	x=-1;
    Mission 	* MyMission;
    char      	LevelName[512];
    LONG      	stacksize;
    struct Task * task;
    
    task = FindTask(NULL);
    stacksize = task->tc_SPUpper - task->tc_SPLower + 1;
    
    if (stacksize < 150000)
    {
        printf("Increase stack size! Minimum is 150000\n");
	exit(RETURN_FAIL);
    }
    
    /* Init Clean Up Nodes */

    InitCleanUp();

    printf("Loading Missions ...\n");
    Pad=LoadMissions("Missions");

    while((x<0)|(x>=MyMissions.Num))
    {
        printf("\n\n%s\n\n",MyMissions.Name);

        for (x=0;x<MyMissions.Num;x++)
        {
            printf("%d.) %s\n",x,MyMissions.Missions[x].Name);
        }
        printf("\nChoose Mission :");
        scanf("%d",&x);
    }

    MyMission=&MyMissions.Missions[x];
    x=-1;

    while((x<0)|(x>=MyMission->Num))
    {
        printf("\n\n%s\n\n",MyMission->Name);

        for (x=0;x<MyMission->Num;x++)
        {
            printf("%d.) %s\n",x,MyMission->Levels[x].Name);
        }
        printf("\nChoose Level :");
        scanf("%d",&x);
    }

    strcpy(LevelName,MyMission->Levels[x].Name);

    printf("Loading Level %s ...\n\n",LevelName);

    LoadLevel(LevelName);

    printf("Init Libs and Open Screen\n");

    if (!InitLibs())
    {
        if (!InitScreen())
        {
            MainLoop();

            CleanupScreen();
        }
        CleanupLibs();
    }

    CheckCleanUp();

    return 0;
}

