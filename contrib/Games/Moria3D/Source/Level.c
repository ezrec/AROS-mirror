/****************************************************************************
*									    *
* Level.C							    970319  *
*									    *
* Level loaders 							    *
*									    *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Clean.h"
#include "MyMath.h"
#include "Level.h"

/* Variables */

Missions  MyMissions;
LevelData MyLevelData;
int16	  MyPoints[8];

Color	  MyColor;
Fade	  MyFade;
pBot	  FirstUnusedBot=0;

/* Replace strtod1000() cause of bug */

fix strtod1000(char * MyStr, char ** EndStr)
{
    return ((fix)strtol(MyStr,EndStr,10))/1000.0;
}

/* Parse a string from File */
/* No error checking yet */

int16 ParseFile(FILE *MyFile,char *Buffer)
{
    int16 Error=0;
    char  *Pos;
    char  *Pos2;
    char  *Pos3;

    fscanf(MyFile,"%s",Buffer);

    Pos=Buffer;

    if (feof(MyFile))
	return 1;
    else
	{
	     if (*Pos==(char)34)                 /* Check for " and parse until " */
		 {				 /* Eliminate first " */
		     Pos2=Pos;
		     while (*(Pos2)!=(char)0)
			 {
			     *Pos2=*(Pos2+1);
			     Pos2++;
			 }
		     while (*Pos!=(char)34)      /* Parse until " */
			 {
			     if (*Pos==(char)0)
				 {
				     *Pos=(char)32;
				     Pos++;
				     fscanf(MyFile,"%s",Pos);
				 }
			     else
				 Pos++;
			 }
		     while (*(Pos)!=(char)0)     /* Eliminate second " */
			 {
			     *Pos=*(Pos+1);
			     Pos++;
			 }
		 }
	     else
		 {
		     while (*Pos!=(char)0)       /* Check for `/''*` */
			 {
			     if ((*Pos=='/') && (*(Pos+1)=='*'))
				 {
				     Pos2=Pos;	 /* Remember start of comment */
				     Pos+=2;
				     while ((*Pos!='*') || (*(Pos+1)!='/'))
					 {	 /* parse until '*''/' */
					     if (*Pos==(char)0)
						 {
						     Pos=Pos2;
						     fscanf(MyFile,"%s",Pos);
						 }
					     else
						 Pos++;
					 }
				     Pos3=Pos+2;   /* Eliminate Comment */
				     Pos=Pos2;
				     while (*Pos2!=(char)0)
					 {
					     *Pos2=*Pos3;
					     Pos2++;
					     Pos3++;
					 }
				 }
			     else
				 Pos++;
			 }
		 }

	     if ((*Buffer==(char)0)&&(Error==0))
		 return ParseFile(MyFile,Buffer);
	     else
		 return Error;
	}
}

/* Parse Missionfile and build Mission Menu */

int16 LoadMissions(char *Name)
{
    int16    Error=0;
    char     PadStr[512];
    FILE     *MissionFile = NULL;
    pMission PosMission = NULL;
    pLevel   PosLevel = NULL;

    MissionFile=fopen(Name,"r");

    while (!feof(MissionFile))
    {
	if (!ParseFile(MissionFile,PadStr))
	{
	    if (!strcmp(PadStr,"OnFile"))           /* Parse loader helpers */
		while (strcmp(PadStr,"OnFileEnd"))
		{
		    ParseFile(MissionFile,PadStr);
		}
	    else if (!strcmp(PadStr,"Missions"))    /* Parse Missions */
		while (strcmp(PadStr,"MissionsEnd"))
		{
		    ParseFile(MissionFile,PadStr);
		    if (!strcmp(PadStr,"NumMissions:"))
		    {
			ParseFile(MissionFile,PadStr);
			MyMissions.Num=strtol(PadStr,0,0);
			MyMissions.Missions=(pMission)MemAlloc((int32)(MyMissions.Num*sizeof(Mission)));
			if (MyMissions.Missions==0)
			{
			    printf("Not enough mem to alloc Missions\n");
			    /* To be considered */
			}
			PosMission=MyMissions.Missions;
		    }
		    else if (!strcmp(PadStr,"MissionsName:"))
		    {
			ParseFile(MissionFile,PadStr);
			strcpy(MyMissions.Name,PadStr);
		    }
		    else if (!strcmp(PadStr,"MissionsStorry:"))
		    {
			ParseFile(MissionFile,PadStr);
			if (*PadStr!='@')
			{
			    MyMissions.Storry="MissionsStorry";
			    /* To be considered */
			}
			else
			{
			    MyMissions.Storry="MissionsStorry";
			    /* To be considered */
			}
		    }
		}
	    else if (!strcmp(PadStr,"Mission"))     /* parse Mission */
	    {
		while (strcmp(PadStr,"MissionEnd"))
		{
		    ParseFile(MissionFile,PadStr);
		    if (!strcmp(PadStr,"MissionName:"))
		    {
			ParseFile(MissionFile,PadStr);
			strcpy(PosMission->Name,PadStr);
		    }
		    else if (!strcmp(PadStr,"MissionStorry:"))
		    {
			ParseFile(MissionFile,PadStr);
			if (*PadStr!='@')
			{
			    PosMission->Storry="MissionStorry";
			    /* To be considered */
			}
			else
			{
			    PosMission->Storry="MissionStorry";
			    /* To be considered */
			}
		    }
		    else if (!strcmp(PadStr,"NumLevels:"))
		    {
			ParseFile(MissionFile,PadStr);
			PosMission->Num=strtol(PadStr,0,0);
			PosMission->Levels=(pLevel)MemAlloc((int32)(PosMission->Num*sizeof(Level)));
			if (PosMission==0)
			{
			    printf("Could not allocate mem for LevelNames\n");
			    /* To be considered */
			}
			PosLevel=PosMission->Levels;
		    }
		    else if (!strcmp(PadStr,"Level:"))
		    {
			ParseFile(MissionFile,PadStr);
			if (*PadStr!='@')
			{
			    printf("Levelnames should have a leeding @\n");
			    /* To be considered */
			}
			else
			{
			    strcpy((char *)PosLevel,PadStr+1);
			    PosLevel++;
			}
		    }

		}
		PosMission++;
	    }
	}
    }

    fclose(MissionFile);

    return Error;
}

void ReadTextureFile(char * Name,pTexture Texture)
{
    FILE * TextFile;

    printf("Loading Texture :%s\n",Name);

    TextFile=fopen(Name,"r");

    if (TextFile != NULL)
    {
	Texture->TextureWidth  = 256;
	Texture->TextureHeight = 256;
	fread(&Texture->ColMap,256,256,TextFile);

	fclose(TextFile);
    }
    else
    {
	printf("Could not open Texture: %s\n",Name);
	exit(10);
    }
}

void LoadColors(char * Name,pColor MyColor)
{
    FILE  * ColFile;
    int     t;
    uint32 *col;
    uint8   bytes[4];

    printf("Loading Colors...\n");

    ColFile = fopen(Name,"r");

    if (ColFile != NULL)
    {
	col = &MyColor->RGB[0][0];

	for (t=0; t<256*3; t++)
	{
	    fread(bytes,1,4,ColFile);

	    col[t] = (bytes[0] << 24) | (bytes[1] << 16) |
		(bytes[2] << 8) | bytes[3];
	}

	fclose(ColFile);
    }
    else
    {
	printf("Could not open Colors:%s\n",Name);
	exit(10);
    }
}

void LoadFading(char * Name,pFade MyFade)
{
    FILE  * FadeFile;

    printf("Loading Fading...\n");

    FadeFile = fopen(Name,"r");

    if (FadeFile != NULL)
    {
	fread(&MyFade->Col,256,256,FadeFile);

	fclose(FadeFile);
    }
    else
    {
	printf("Could not open Fadeings:%s\n",Name);
	exit(10);
    }
}
int16 LoadLevel(char * Name)
{
    int16      Error=0;
    char       PadStr[512];
    FILE     * LevelFile;
    pPnt       PosPoint = NULL;
    pTexture   PosTexture = NULL;
    pSide      PosSide = NULL;
    pCube      PosCube = NULL;
    pPolygon   PosPolygon = NULL;
    pMesh      PosMesh = NULL;
    pBot       PosBot = NULL;
    int16      u,v;

    LevelFile=fopen(Name,"r");

    if (LevelFile != 0)
    {
	while (!feof(LevelFile))
	{
	    ParseFile(LevelFile,PadStr);
	    if (!strcmp(PadStr,"Header"))
	    {
		while(strcmp(PadStr,"HeaderEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    /* To be considered */
		    if (!strcmp(PadStr,"Colors:"))
		    {
			ParseFile(LevelFile,PadStr);
			if (PadStr[0] != '@')
			{
			    printf("Colors must be in a file!\n");
			    fclose(LevelFile);
			    exit(10);
			}
			else
			{
			    LoadColors(&PadStr[1],&MyColor);
			}
		    }
		    if (!strcmp(PadStr,"Fadeing:"))
		    {
			ParseFile(LevelFile,PadStr);
			if (PadStr[0] != '@')
			{
			    printf("Fading must be in a file!\n");
			    fclose(LevelFile);
			    exit(10);
			}
			else
			{
			    LoadFading(&PadStr[1],&MyFade);
			}
		    }
		    if (!strcmp(PadStr,"Rings:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumRings = strtol(PadStr,0,0);
		    }
		}
	    }
	    else if (!strcmp(PadStr,"Points"))
	    {
		while(strcmp(PadStr,"PointsEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumPoints:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumPoints=strtol(PadStr,0,0);
			MyLevelData.Points=(pPnt)MemAlloc((int32)(MyLevelData.NumPoints*sizeof(Pnt)));
			PosPoint=MyLevelData.Points;
		    }
		    else if (!strcmp(PadStr,"Point:"))
		    {
			ParseFile(LevelFile,PadStr);
			PosPoint->x=(fix)strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosPoint->y=(fix)strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosPoint->z=(fix)strtod1000(PadStr,0);
			PosPoint++;
		    }

		}
	    }
	    else if (!strcmp(PadStr,"Textures"))
	    {
		while(strcmp(PadStr,"TexturesEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumTextures:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumTextures=strtol(PadStr,0,0);
			MyLevelData.Textures=(pTexture)MemAlloc((int32)(MyLevelData.NumTextures*sizeof(Texture)));
			PosTexture=MyLevelData.Textures;
		    }
		    else if (!strcmp(PadStr,"Texture:"))
		    {
			ParseFile(LevelFile,PadStr);
			if (PadStr[0] != '@')
			{
			    printf("Textures should be in a file\n");
			    exit(10);
			}
			else
			{
			    ReadTextureFile(&PadStr[1],PosTexture);
			}
			PosTexture++;
		    }

		}
	    }
	    else if (!strcmp(PadStr,"Sides"))
	    {
		while(strcmp(PadStr,"SidesEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumSides:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumSides=strtol(PadStr,0,0);
			MyLevelData.Sides=(pSide)MemAlloc((int32)(MyLevelData.NumSides*sizeof(Side)));
			PosSide=MyLevelData.Sides;
		    }
		    else if (!strcmp(PadStr,"Side:"))
		    {
			ParseFile(LevelFile,PadStr);
			if (strtol(PadStr,0,0) != 0)
			{
			    PosSide->Back=&MyLevelData.Textures[strtol(PadStr,0,0)];
			    for (v=0;v<4;v++)
			    {
				ParseFile(LevelFile,PadStr);
				PosSide->BackPnts[v].x=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosSide->BackPnts[v].y=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosSide->BackPnts[v].z=strtod1000(PadStr,0);
			    }
			}
			else
			{
			    PosSide->Back=0;
			}
			ParseFile(LevelFile,PadStr);
			if (strtol(PadStr,0,0) != 0)
			{
			    PosSide->Front=&MyLevelData.Textures[strtol(PadStr,0,0)];
			    for (v=0;v<4;v++)
			    {
				ParseFile(LevelFile,PadStr);
				PosSide->FrontPnts[v].x=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosSide->FrontPnts[v].y=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosSide->FrontPnts[v].z=strtod1000(PadStr,0);
			    }
			}
			else
			{
			    PosSide->Front=0;
			}
			PosSide++;
		    }

		}
	    }
	    else if (!strcmp(PadStr,"Cubes"))
	    {
		while(strcmp(PadStr,"CubesEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumCubes:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumCubes=strtol(PadStr,0,0);
			MyLevelData.Cubes=(pCube)MemAlloc((int32)(MyLevelData.NumCubes*sizeof(Cube)));
			PosCube=MyLevelData.Cubes;
		    }
		    else if (!strcmp(PadStr,"Cube:"))
		    {
			PosCube->FirstBot=0;

			for (v=0;v<8;v++) /* Read 8 Points */
			{
			    ParseFile(LevelFile,PadStr);
			    MyPoints[v]=strtol(PadStr,0,0);
			}
			ParseFile(LevelFile,PadStr); /* Read Light */
			PosCube->Light=strtod1000(PadStr,0);

			for (v=0;v<6;v++) /* Read 6 Sides */
			{
			    ParseFile(LevelFile,PadStr);
			    if (strtol(PadStr,0,0) != 0)
			    {
				PosCube->Sides[v].Back=MyLevelData.Sides[strtol(PadStr,0,0)-1].Back;
				PosCube->Sides[v].BackPnts[0]=MyLevelData.Sides[strtol(PadStr,0,0)-1].BackPnts[0];
				PosCube->Sides[v].BackPnts[1]=MyLevelData.Sides[strtol(PadStr,0,0)-1].BackPnts[1];
				PosCube->Sides[v].BackPnts[2]=MyLevelData.Sides[strtol(PadStr,0,0)-1].BackPnts[2];
				PosCube->Sides[v].BackPnts[3]=MyLevelData.Sides[strtol(PadStr,0,0)-1].BackPnts[3];
				PosCube->Sides[v].Front=MyLevelData.Sides[strtol(PadStr,0,0)-1].Front;
				PosCube->Sides[v].FrontPnts[0]=MyLevelData.Sides[strtol(PadStr,0,0)-1].FrontPnts[0];
				PosCube->Sides[v].FrontPnts[1]=MyLevelData.Sides[strtol(PadStr,0,0)-1].FrontPnts[1];
				PosCube->Sides[v].FrontPnts[2]=MyLevelData.Sides[strtol(PadStr,0,0)-1].FrontPnts[2];
				PosCube->Sides[v].FrontPnts[3]=MyLevelData.Sides[strtol(PadStr,0,0)-1].FrontPnts[3];
				ParseFile(LevelFile,PadStr);
				PosCube->Sides[v].Light=strtod1000(PadStr,0);
			    }
			    else
			    {
				PosCube->Sides[v].Light=0.0;
				PosCube->Sides[v].Back=0;
				PosCube->Sides[v].Front=0;
			    }
			    switch (v) /* Insert Points depending on the side */
			    {
				case 0:
				{
				    PosCube->Sides[0].Pnts[0]=&MyLevelData.Points[MyPoints[SideFront0]-1];
				    PosCube->Sides[0].Pnts[1]=&MyLevelData.Points[MyPoints[SideFront1]-1];
				    PosCube->Sides[0].Pnts[2]=&MyLevelData.Points[MyPoints[SideFront2]-1];
				    PosCube->Sides[0].Pnts[3]=&MyLevelData.Points[MyPoints[SideFront3]-1];
				}
				case 1:
				{
				    PosCube->Sides[1].Pnts[0]=&MyLevelData.Points[MyPoints[SideBack0]-1];
				    PosCube->Sides[1].Pnts[1]=&MyLevelData.Points[MyPoints[SideBack1]-1];
				    PosCube->Sides[1].Pnts[2]=&MyLevelData.Points[MyPoints[SideBack2]-1];
				    PosCube->Sides[1].Pnts[3]=&MyLevelData.Points[MyPoints[SideBack3]-1];
				}
				case 2:
				{
				    PosCube->Sides[2].Pnts[0]=&MyLevelData.Points[MyPoints[SideLeft0]-1];
				    PosCube->Sides[2].Pnts[1]=&MyLevelData.Points[MyPoints[SideLeft1]-1];
				    PosCube->Sides[2].Pnts[2]=&MyLevelData.Points[MyPoints[SideLeft2]-1];
				    PosCube->Sides[2].Pnts[3]=&MyLevelData.Points[MyPoints[SideLeft3]-1];
				}
				case 3:
				{
				    PosCube->Sides[3].Pnts[0]=&MyLevelData.Points[MyPoints[SideRight0]-1];
				    PosCube->Sides[3].Pnts[1]=&MyLevelData.Points[MyPoints[SideRight1]-1];
				    PosCube->Sides[3].Pnts[2]=&MyLevelData.Points[MyPoints[SideRight2]-1];
				    PosCube->Sides[3].Pnts[3]=&MyLevelData.Points[MyPoints[SideRight3]-1];
				}
				case 4:
				{
				    PosCube->Sides[4].Pnts[0]=&MyLevelData.Points[MyPoints[SideTop0]-1];
				    PosCube->Sides[4].Pnts[1]=&MyLevelData.Points[MyPoints[SideTop1]-1];
				    PosCube->Sides[4].Pnts[2]=&MyLevelData.Points[MyPoints[SideTop2]-1];
				    PosCube->Sides[4].Pnts[3]=&MyLevelData.Points[MyPoints[SideTop3]-1];
				}
				case 5:
				{
				    PosCube->Sides[5].Pnts[0]=&MyLevelData.Points[MyPoints[SideBottom0]-1];
				    PosCube->Sides[5].Pnts[1]=&MyLevelData.Points[MyPoints[SideBottom1]-1];
				    PosCube->Sides[5].Pnts[2]=&MyLevelData.Points[MyPoints[SideBottom2]-1];
				    PosCube->Sides[5].Pnts[3]=&MyLevelData.Points[MyPoints[SideBottom3]-1];
				}
			    }
			    ParseFile(LevelFile,PadStr);
			    if (strtol(PadStr,0,0) != 0)
			    {
				PosCube->Cubes[v]=&MyLevelData.Cubes[strtol(PadStr,0,0)-1];
			    }
			    else
			    {
				PosCube->Cubes[v]=0;
			    }
			    CalcNormalToSide(&PosCube->Sides[v]);
			}
			PosCube++;
		    }

		}
	    }
	    else if (!strcmp(PadStr,"Polygons"))
	    {
		while(strcmp(PadStr,"PolygonsEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumPolygons:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumPolygons=strtol(PadStr,0,0);
			MyLevelData.Polygons=(pPolygon)MemAlloc((int32)(MyLevelData.NumPolygons*sizeof(Polygon)));
			PosPolygon=MyLevelData.Polygons;
		    }
		    else if (!strcmp(PadStr,"Polygon:"))
		    {
			/* Read NumPoints */

			ParseFile(LevelFile,PadStr);
			PosPolygon->NumPoints=strtol(PadStr,0,0);

			/* Read Points */

			for (u=0;u<PosPolygon->NumPoints;u++)
			{
			    ParseFile(LevelFile,PadStr);
			    PosPolygon->Pnts[u].x=strtod1000(PadStr,0);
			    ParseFile(LevelFile,PadStr);
			    PosPolygon->Pnts[u].y=strtod1000(PadStr,0);
			    ParseFile(LevelFile,PadStr);
			    PosPolygon->Pnts[u].z=strtod1000(PadStr,0);
			}

			/* Read Normal */

			ParseFile(LevelFile,PadStr);
			PosPolygon->Normal.x=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosPolygon->Normal.y=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosPolygon->Normal.z=strtod1000(PadStr,0);

			/* Read Textures */

			ParseFile(LevelFile,PadStr);

			if (strtol(PadStr,0,0) == 0)
			{
			    PosPolygon->Back = 0;
			}
			else
			{
			    PosPolygon->Back = &MyLevelData.Textures[strtol(PadStr,0,0)];

			    /* Read uvls´s */

			    for (u=0;u<4;u++)
			    {
				ParseFile(LevelFile,PadStr);
				PosPolygon->BackPnts[u].x=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosPolygon->BackPnts[u].y=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosPolygon->BackPnts[u].z=strtod1000(PadStr,0);
			    }
			}

			ParseFile(LevelFile,PadStr);

			if (strtol(PadStr,0,0) == 0)
			{
			    PosPolygon->Front = 0;
			}
			else
			{
			    PosPolygon->Front = &MyLevelData.Textures[strtol(PadStr,0,0)];

			    /* Read uvls´s */

			    for (u=0;u<4;u++)
			    {
				ParseFile(LevelFile,PadStr);
				PosPolygon->FrontPnts[u].x=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosPolygon->FrontPnts[u].y=strtod1000(PadStr,0);
				ParseFile(LevelFile,PadStr);
				PosPolygon->FrontPnts[u].z=strtod1000(PadStr,0);
			    }
			}

			PosPolygon++;
		    }
		}
	    }
	    else if (!strcmp(PadStr,"Meshes"))
	    {
		while(strcmp(PadStr,"MeshesEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumMeshes:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumMeshes=strtol(PadStr,0,0);
			MyLevelData.Meshes=(pMesh)MemAlloc((int32)(MyLevelData.NumMeshes*sizeof(Mesh)));
			PosMesh=MyLevelData.Meshes;
		    }
		    else if (!strcmp(PadStr,"Mesh:"))
		    {
			/* Read NumPolygons */

			ParseFile(LevelFile,PadStr);
			PosMesh->NumPolygons=strtol(PadStr,0,0);

			/* Read Polygons */

			for (u=0;u<PosMesh->NumPolygons;u++)
			{
			    /* Read Polygon */

			    ParseFile(LevelFile,PadStr);
			    PosMesh->Polygons[u] = &MyLevelData.Polygons[strtol(PadStr,0,0)-1];

			    ParseFile(LevelFile,PadStr);
			    PosMesh->Base[u].x=strtod1000(PadStr,0);
			    ParseFile(LevelFile,PadStr);
			    PosMesh->Base[u].y=strtod1000(PadStr,0);
			    ParseFile(LevelFile,PadStr);
			    PosMesh->Base[u].z=strtod1000(PadStr,0);
			}

			/* Read Light */

			ParseFile(LevelFile,PadStr);
			PosMesh->Light=strtod1000(PadStr,0);

			PosMesh++;
		    }
		}
	    }
	    else if (!strcmp(PadStr,"Bots"))
	    {
		while(strcmp(PadStr,"BotsEnd"))
		{
		    ParseFile(LevelFile,PadStr);
		    if (!strcmp(PadStr,"NumBots:"))
		    {
			ParseFile(LevelFile,PadStr);
			MyLevelData.NumBots=strtol(PadStr,0,0);
			MyLevelData.Bots=(pBot)MemAlloc((int32)(MyLevelData.NumBots*sizeof(Bot)));
			PosBot=MyLevelData.Bots;

			MyLevelData.Bots[0].AbsPrev=0;

			if (MyLevelData.NumBots >1)
			{
			    MyLevelData.Bots[0].AbsNext=&MyLevelData.Bots[1];
			    for (u=1;u<MyLevelData.NumBots-1;u++)
			    {
				MyLevelData.Bots[u].AbsNext = &MyLevelData.Bots[u+1];
				MyLevelData.Bots[u].AbsPrev = &MyLevelData.Bots[u-1];
			    }

			    MyLevelData.Bots[MyLevelData.NumBots-1].AbsNext = 0;
			    MyLevelData.Bots[MyLevelData.NumBots-1].AbsPrev = &MyLevelData.Bots[MyLevelData.NumBots-2];
			}
			else
			{
			    MyLevelData.Bots[0].AbsNext = 0;
			}
		    }
		    else if (!strcmp(PadStr,"Bot:"))
		    {
			PosBot->Next=0;
			PosBot->Prev=0;

			/* Read Position */

			ParseFile(LevelFile,PadStr);
			PosBot->Pos.x=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Pos.y=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Pos.z=strtod1000(PadStr,0);

			/* Read Direction */

			ParseFile(LevelFile,PadStr);
			PosBot->Direction.a.x=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Direction.a.y=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Direction.a.z=strtod1000(PadStr,0);

			ParseFile(LevelFile,PadStr);
			PosBot->Direction.b.x=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Direction.b.y=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Direction.b.z=strtod1000(PadStr,0);

			ParseFile(LevelFile,PadStr);
			PosBot->Direction.c.x=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Direction.c.y=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Direction.c.z=strtod1000(PadStr,0);

			/* Read Speed */

			ParseFile(LevelFile,PadStr);
			PosBot->Speed.x=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Speed.y=strtod1000(PadStr,0);
			ParseFile(LevelFile,PadStr);
			PosBot->Speed.z=strtod1000(PadStr,0);

			/* Read Size */

			ParseFile(LevelFile,PadStr);
			PosBot->Size=strtod1000(PadStr,0);

			/* Read Cube */

			ParseFile(LevelFile,PadStr);
			PosBot->Cube=&MyLevelData.Cubes[strtol(PadStr,0,0)-1];

			if (PosBot->Cube->FirstBot != 0)
			{
			    PosBot->Next = PosBot->Cube->FirstBot;
			    PosBot->Cube->FirstBot->Prev = PosBot;
			}

			PosBot->Cube->FirstBot = PosBot;

			/* Read Type */

			ParseFile(LevelFile,PadStr);
			PosBot->Type=strtol(PadStr,0,0);

			/* Read Light */

			ParseFile(LevelFile,PadStr);
			PosBot->Light=strtod1000(PadStr,0);

			/* Read Mesh */

			ParseFile(LevelFile,PadStr);

			if (strtol(PadStr,0,0) == 0)
			{
			    PosBot->Mesh=0;
			}
			else
			{
			    PosBot->Mesh = &MyLevelData.Meshes[strtol(PadStr,0,0)-1];
			}

			PosBot->NumRings=0;

			PosBot++;
		    }
		}
	    }
	}

	fclose(LevelFile);
    }
    else
    {
	printf ("Failed to open LevelFile\n");
	exit(10);
	Error = (0==0);
    };

    return Error;
}

