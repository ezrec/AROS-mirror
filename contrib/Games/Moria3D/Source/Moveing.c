/****************************************************************************
*									    *
* Moveing.c							     970319 *
*									    *
* Move Player around							    *
*									    *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <intuition/intuition.h>
#include <proto/exec.h>

#include "MyTypes.h"
#include "Moveing.h"
#include "Gfx.h"
#include "Level.h"
#include "MyMath.h"

int32 XMove=0,YMove=0,ZMove=0;
int32 Button1,Button2;

struct MsgPort * WindowPort;

int32 MouseX=0,MouseY=0;
fix   SpeedUp	= DefSpeedUp;
fix   SpeedDown = DefSpeedDown;
int16 Pause = !(0==0);

void TurnPlayer(pBot MyPlayer)
{
    Matrix M;
    Vct    a,b;

    #define COS1 0.0038053
    #define SIN  0.0871557

    /*
    printf("%f %f %f , %f %f %f , %f %f %f\n",MyPlayer->Direction.a.x,MyPlayer->Direction.a.y,MyPlayer->Direction.a.z,
					      MyPlayer->Direction.b.x,MyPlayer->Direction.b.y,MyPlayer->Direction.b.z,
					      MyPlayer->Direction.c.x,MyPlayer->Direction.c.y,MyPlayer->Direction.c.z);
    */
    /* Turn sideways */

    VctMulFix(&MyPlayer->Direction.a,1.0-COS1*XMove*XMove,&a);
    VctMulFix(&MyPlayer->Direction.c,SIN*XMove,&b);
    VctAdd(&a,&b,&M.a);

    M.b.x = MyPlayer->Direction.b.x;
    M.b.y = MyPlayer->Direction.b.y;
    M.b.z = MyPlayer->Direction.b.z;

    VctMulFix(&MyPlayer->Direction.a,-SIN*XMove,&a);
    VctMulFix(&MyPlayer->Direction.c,1-COS1*XMove*XMove,&b);
    VctAdd(&a,&b,&M.c);

    /* Turn Upwards */

    MyPlayer->Direction.a.x = M.a.x;
    MyPlayer->Direction.a.y = M.a.y;
    MyPlayer->Direction.a.z = M.a.z;

    VctMulFix(&M.b,1-COS1*YMove*YMove,&a);
    VctMulFix(&M.c,SIN*YMove,&b);
    VctAdd(&a,&b,&MyPlayer->Direction.b);

    VctMulFix(&M.b,-SIN*YMove,&a);
    VctMulFix(&M.c,1-COS1*YMove*YMove,&b);
    VctAdd(&a,&b,&MyPlayer->Direction.c);


    /* Turn Around */

    VctMulFix(&MyPlayer->Direction.a,1.0-COS1*ZMove*ZMove,&a);
    VctMulFix(&MyPlayer->Direction.b,SIN*ZMove,&b);
    VctAdd(&a,&b,&M.a);

    VctMulFix(&MyPlayer->Direction.a,-SIN*ZMove,&a);
    VctMulFix(&MyPlayer->Direction.b,1-COS1*ZMove*ZMove,&b);
    VctAdd(&a,&b,&M.b);

    MyPlayer->Direction.a.x = M.a.x;
    MyPlayer->Direction.a.y = M.a.y;
    MyPlayer->Direction.a.z = M.a.z;

    MyPlayer->Direction.b.x = M.b.x;
    MyPlayer->Direction.b.y = M.b.y;
    MyPlayer->Direction.b.z = M.b.z;

    /*
    printf("%f %f %f , %f %f %f , %f %f %f\n",MyPlayer->Direction.a.x,MyPlayer->Direction.a.y,MyPlayer->Direction.a.z,
					      MyPlayer->Direction.b.x,MyPlayer->Direction.b.y,MyPlayer->Direction.b.z,
					      MyPlayer->Direction.c.x,MyPlayer->Direction.c.y,MyPlayer->Direction.c.z);
    */
}

void MoveForward(pBot MyPlayer)
{
    Vct a;

    VctMulFix(&MyPlayer->Direction.c,SpeedUp,&a);
    VctAdd(&MyPlayer->Speed,&a,&MyPlayer->Speed);
}

void MoveBackward(pBot MyPlayer)
{
    Vct a;

    VctMulFix(&MyPlayer->Direction.c,-SpeedUp,&a);
    VctAdd(&MyPlayer->Speed,&a,&MyPlayer->Speed);
}

int16 CheckCollision(pBot MyPlayer)
{
    Vct   a;
    int16 x;
    fix   d;
    pBot  MyBot,NextBot;
    int16 Death= !(0==0);

    /*
    printf("\n%d\n",MyPlayer->Cube);
    */

    /* Check Bot */

    MyBot=MyPlayer->Cube->FirstBot;

    while ((MyBot != 0) && (!Death))
    {
	NextBot = MyBot->Next;

	if (MyBot != MyPlayer)
	{
	    VctSub(&MyBot->Pos,&MyPlayer->Pos,&a);
	    d= VctLength(&a);

	    if (d == 0)
	    {
		printf("Tele Frag\n");
	    }
	    else if (d<MyPlayer->Size+MyBot->Size) /* Do we hit it ? */
	    {
		d=(MyPlayer->Size+MyBot->Size-d) / d;

		if ((MyBot->Type & BotPush) != 0)
		{
		    /* Push Player */

		    VctMulFix(&a,d,&a);
		    VctSub(&MyPlayer->Speed,&a,&MyPlayer->Speed);
		}
		if ((MyPlayer->Type & BotPickup) != 0)
		{
		    /* Pickup Bot */

		    if ((MyBot->Type & BotPlayer) != 0) /* Only Player can Pickup */
		    {
			/* Resolve Bot from Cube */

			if (MyPlayer->Next != 0)
			{
			    MyPlayer->Next->Prev = MyPlayer->Prev;
			}
			if (MyPlayer->Prev != 0)
			{
			    MyPlayer->Prev->Next = MyPlayer->Next;
			}
			else
			{
			    MyPlayer->Cube->FirstBot = MyPlayer->Next;
			}

			/* Resolve Bot from Abs Chain */

			if (MyPlayer->AbsNext != 0)
			{
			    MyPlayer->AbsNext->AbsPrev = MyPlayer->AbsPrev;
			}
			if (MyPlayer->AbsPrev != 0)
			{
			    MyPlayer->AbsPrev->AbsNext = MyPlayer->AbsNext;
			}

			/* Insert Player into unused chain */

			MyPlayer->Next = FirstUnusedBot;
			MyPlayer->Prev = 0;

			if (MyPlayer->Next != 0)
			{
			    MyPlayer->Next->Prev = MyPlayer;
			}

			/* Add Thing to Bot */

			if ((MyPlayer->Type & BotRing) != 0)
			{
			    printf("Ring collected\n");
			    MyBot->NumRings++;
			}

			Death = (0==0);
		    }
		}
	    }
	}

	MyBot = NextBot;
    }

    if (!Death)
    {
	/* Check Sides */

	for (x=0;x<6;x++)
	{
	    /*
	    printf("Pos %f %f %f\n",MyPlayer->Pos.x,MyPlayer->Pos.y,MyPlayer->Pos.z);
	    */

	    VctSub(&MyPlayer->Pos,MyPlayer->Cube->Sides[x].Pnts[0],&a);

	    /*
	    printf("Pos %f %f %f\n",a.x,a.y,a.z);
	    printf("Vct %f %f %f\n",MyPlayer->Cube->Sides[x].Rev.a.z,
				    MyPlayer->Cube->Sides[x].Rev.b.z,
				    MyPlayer->Cube->Sides[x].Rev.c.z);
	    */

	    d=a.x * MyPlayer->Cube->Sides[x].Rev.a.z
	     +a.y * MyPlayer->Cube->Sides[x].Rev.b.z
	     +a.z * MyPlayer->Cube->Sides[x].Rev.c.z;

	    /*
	    printf("%d %f\n",x,d);
	    */
	    if (MyPlayer->Cube->Cubes[x] != 0)
	    {
		if (d<0)
		{
		    /* Resolve Bot from chains */

		    if (MyPlayer->Next != 0)
		    {
			MyPlayer->Next->Prev = MyPlayer->Prev;
		    }
		    if (MyPlayer->Prev != 0)
		    {
			MyPlayer->Prev->Next = MyPlayer->Next;
		    }
		    else
		    {
			MyPlayer->Cube->FirstBot = MyPlayer->Next;
		    }

		    /* Change to new cube */

		    MyPlayer->Cube=MyPlayer->Cube->Cubes[x];

		    /* Insert into chain */

		    MyPlayer->Next = MyPlayer->Cube->FirstBot;
		    MyPlayer->Cube->FirstBot = MyPlayer;
		    MyPlayer->Prev = 0;

		    if (MyPlayer->Next != 0)
		    {
			MyPlayer->Next->Prev = MyPlayer;
		    }
		}
	    }
	    else
	    {
		if (d<0)
		{
		    printf("Side %d\n",x);

		    d=0.01;
		}
		if (d<MyPlayer->Size) /* Do we hit anything ? */
		{
		    d=MyPlayer->Size-d;

		    VctMulFix(&MyPlayer->Cube->Sides[x].Normal,d,&a);
		    VctAdd(&MyPlayer->Speed,&a,&MyPlayer->Speed);
		}
	    }
	}
    }

    return Death;
}

void MoveSpeed(pBot MyPlayer)
{
    /*
    printf("Bot %ld %f,%f,%f\n",MyPlayer,MyPlayer->Pos.x,MyPlayer->Pos.y,MyPlayer->Pos.z);
    printf("    %ld %f,%f,%f\n",MyPlayer->Type,MyPlayer->Speed.x,MyPlayer->Speed.y,MyPlayer->Speed.z);
    */
    VctAdd(&MyPlayer->Pos,&MyPlayer->Speed,&MyPlayer->Pos);
    VctMulFix(&MyPlayer->Speed,SpeedDown,&MyPlayer->Speed);
}

void MoveBot(pBot MyPlayer,pBot MyBot)
{
    Vct a;
    /*
    printf("Bot %ld\n",MyBot);
    */

    if ((MyBot->Type & BotMove) != 0)
    {
	VctSub(&MyPlayer->Pos,&MyBot->Pos,&a);

	VctMulFix(&a,0.25/MyBot->Size/VctLength(&a),&a);

	VctAdd(&a,&MyBot->Speed,&MyBot->Speed);
    }

    if (!CheckCollision(MyBot))
    {
	MoveSpeed(MyBot);
    }
}

void MoveBots(void)
{
    pBot MyBot,NextBot,MyPlayer;

    if (!Pause)
    {
	MyPlayer=&MyLevelData.Bots[0];
	/*
	printf("\nPlayer %ld\n",MyPlayer);
	printf("Bot %ld\n",MyPlayer->AbsNext);
	*/
	MyBot=MyPlayer->AbsNext;

	while (MyBot != NULL)
	{
	    NextBot = MyBot->AbsNext;

	    MoveBot(MyPlayer,MyBot);

	    MyBot = NextBot;
	}
    }
}

#define _AROS_BUG

void MovePlayer(pBot MyPlayer)
{
    struct IntuiMessage * MyMsg;

    ZMove = 0;

#ifdef _AROS_BUG
again:
    if ((MyMsg=(struct IntuiMessage *)GetMsg(WindowPort)) != NULL)
#else
    while ((MyMsg=(struct IntuiMessage *)GetMsg(WindowPort)) != NULL)
#endif
    {
	switch (MyMsg->Class)
	{
	    case IDCMP_VANILLAKEY:
	    {
		switch(MyMsg->Code)
		{
		    case 27: /* ESC quit */
		    {
			printf("\n Sucide is illegal here!\n");
			Finished = (0==0);
			break;
		    }
		    case 32: /* SPACE hit */
		    {
			printf("KILL .. KILL .. KILL .. KILL .. KILL\n");
			break;
		    }
		    case '7': /* turn anti-clockwise */
		    {
			ZMove = -1;
			break;
		    }
		    case '9': /* turn clockwise */
		    {
			ZMove = 1;
			break;
		    }
		    case 'l': /* decrease Light */
		    {
			LightLess-=0.001;

			if (LightLess<0)
			    LightLess = 0.0;

			printf("Lightless: %f\n",LightLess);

			break;
		    }
		    case 'L': /* increase Light */
		    {
			LightLess+=0.001;

			if (LightLess > 1.0)
			    LightLess = 1.0;

			printf("Lightless: %f\n",LightLess);

			break;
		    }
		    case '+': /* Decrease CameraBehind */
		    {
			CameraBehind-=0.1;

			printf("CameraBehind: %f\n",CameraBehind);

			break;
		    }
		    case '-': /* Increase CameraBehind */
		    {
			CameraBehind+=0.1;

			printf("CameraBehind: %f\n",CameraBehind);

			break;
		    }
		    case 'p': /* Pause */
		    {
			Pause = !Pause;
		    }
		}
		break;
	    }
	    case IDCMP_RAWKEY:
	    {
		printf("Do you realy wanna harm that cute little cube?\n");
		break;
	    }
	    case IDCMP_MOUSEBUTTONS:
	    {
		switch(MyMsg->Code)
		{
		    case 104:
		    {
			Button1 = (0==0);
			break;
		    }
		    case 232:
		    {
			Button1 = !(0==0);
			break;
		    }
		    case 105:
		    {
			Button2 = (0==0);
			break;
		    }
		    case 233:
		    {
			Button2 = !(0==0);
			break;
		    }
		}
	    }

	    case IDCMP_MOUSEMOVE:
	    {
		MouseX=MyMsg->MouseX;
		MouseY=MyMsg->MouseY;
	    }
	}

	ReplyMsg((struct Message *)MyMsg);
    }
#ifdef _AROS_BUG /* BUG */
    else
    {
	Wait (1L << WindowPort->mp_SigBit);
	goto again;
    }
#endif /* _AROS */

    if (MouseX<QuaterWidth)
    {
	XMove=+1;
    }
    else if (MouseX<HalfWidth+QuaterWidth)
    {
	XMove=0;
    }
    else
    {
	XMove=-1;
    }

    if (MouseY<QuaterHeight)
    {
	YMove=-1;
    }
    else if (MouseY<HalfHeight+QuaterHeight)
    {
	YMove=0;
    }
    else
    {
	YMove=1;
    }

    if (!Pause)
    {
	TurnPlayer(MyPlayer);

	if (Button1)
	{
	    MoveForward(MyPlayer);
	}
	if (Button2)
	{
	    MoveBackward(MyPlayer);
	}

	if (!CheckCollision(MyPlayer))
	{
	    MoveSpeed(MyPlayer);
	}
	else
	{
	    /* RIP */
	}
    }
}

