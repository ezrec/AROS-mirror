/****************************************************************************
*									    *
* Level.h							    970319  *
*									    *
* Defines for the Level structures					    *
*									    *
****************************************************************************/

#ifndef LEVEL_H
#define LEVEL_H

typedef struct Side * pSide;
typedef struct Cube * pCube;
typedef struct Bot * pBot;

#include "MyTypes.h"
#include "MyMath.h"
#include "Clean.h"

/* Values for Polygons and Meshes */

#define MaxPolPoints 20
#define MaxPolygons  20

/* Orderings for Point of a Side */

#define PntBotLeft  0
#define PntBotRight 1
#define PntTopRight 2
#define PntTopLeft  3

/* ordering of Sides */

#define SideFront  0
#define SideBack   1
#define SideLeft   2
#define SideRight  3
#define SideTop    4
#define SideBottom 5

/* Ordering of Points in the Cube for each side */

#define SideFront0  0
#define SideFront1  1
#define SideFront2  3
#define SideFront3  2

#define SideBack0   5
#define SideBack1   4
#define SideBack2   6
#define SideBack3   7

#define SideLeft0   4
#define SideLeft1   0
#define SideLeft2   2
#define SideLeft3   6

#define SideRight0  1
#define SideRight1  5
#define SideRight2  7
#define SideRight3  3

#define SideTop0    7
#define SideTop1    6
#define SideTop2    2
#define SideTop3    3

#define SideBottom0 1
#define SideBottom1 0
#define SideBottom2 4
#define SideBottom3 5

#define CubeFront  0
#define CubeBack   1
#define CubeLeft   2
#define CubeRight  3
#define CubeTop    4
#define CubeBottom 5

/* Bot Types */

#define BotPlayer 1
#define BotDraw   2
#define BotPickup 4
#define BotPush   8
#define BotMove  16
#define BotRing  32

/* Point */

typedef Vct * pPnt;
typedef Vct Pnt;

/* Texture */

typedef struct Texture * pTexture;
typedef struct Texture
    {
	int16	     TextureWidth;
	int16	     TextureHeight;
	uint8	     ColMap[256*256];
    } Texture;

/* Side */

/* typedef struct Side * pSide; See above */

typedef struct Side
    {
	pPnt	 Pnts[4];	/* Edges	      */
	Vct	 Normal;	/* Normal to Side     */
	Matrix	 Rev;		/* Reverse Matrix     */
	fix	 Light; 	/* Overall Light      */
	pTexture Back;		/* Background Texture */
	Vct	 BackPnts[4];	/* Coords in Texture  */
	pTexture Front; 	/* Overlay Texture    */
	Vct	 FrontPnts[4];	/* Coords in Texture  */
    } Side;


/* Cube */

typedef struct Cube
    {
	Side  Sides[6]; 	/* 6 Sides of the Cube	    */
	pCube Cubes[6]; 	/* 6 Cubes conected to this */
	fix   Light;		/* Light for the interior   */
	pBot  FirstBot; 	/* First Bot in this Cube   */
    } Cube;

/* Polygon */

typedef struct Polygon * pPolygon;
typedef struct Polygon
    {
	int32	 NumPoints;
	Vct	 Pnts[MaxPolPoints];
	Vct	 Normal;
	pTexture Back;
	Vct	 BackPnts[MaxPolPoints];
	pTexture Front;
	Vct	 FrontPnts[MaxPolPoints];
    } Polygon;

/* Mesh */

typedef struct Mesh * pMesh;
typedef struct Mesh
    {
	int32	 NumPolygons;
	pPolygon Polygons[MaxPolygons];
	Vct	 Base[MaxPolygons];
	fix	 Light;
	Matrix	 Direction;
    } Mesh;

/* Bot */

typedef struct Bot
    {
	pBot	    Next,Prev;
	pBot	    AbsNext,AbsPrev;
	Vct	    Pos;
	Matrix	    Direction;
	Vct	    Speed;
	fix	    Size;
	pCube	    Cube;
	pMesh	    Mesh;
	int32	    Type;
	fix	    Light;
	int16	    NumRings;
	int16	    HitPoints;
	int16	    MaxHitPoints;
    } Bot;

/* Level Files */

typedef struct LevelData * pLevelData;
typedef struct LevelData
    {
	int16	   NumRings;
	int32	   NumPoints;
	pPnt	   Points;
	int32	   NumTextures;
	pTexture   Textures;
	int32	   NumSides;
	pSide	   Sides;
	int32	   NumCubes;
	pCube	   Cubes;
	int32	   NumPolygons;
	pPolygon   Polygons;
	int32	   NumMeshes;
	pMesh	   Meshes;
	int32	   NumBots;
	pBot	   Bots;
    } LevelData;

typedef struct Level * pLevel;
typedef struct Level
    {
	char Name[512];
    } Level;

/* Mission Files */

typedef struct Mission * pMission;
typedef struct Mission
    {
	char   Name[512];
	char   *Storry;
	int16  Num;
	pLevel Levels;
    } Mission;

typedef struct Missions * pMissions;
typedef struct Missions
    {
	int16	 Num;
	pMission Missions;
	char	 Name[512];
	char	 *Storry;
    } Missions;

typedef struct Color * pColor;
typedef struct Color
    {
	uint32 RGB[256][3];
    } Color;

typedef struct Fade * pFade;
typedef struct Fade
    {
	uint8 Col[256][256];
    } Fade;

/* Variables */

extern Missions  MyMissions;
extern LevelData MyLevelData;
extern Fade	 MyFade;
extern Color	 MyColor;
extern pBot	 FirstUnusedBot;

/* Functions */

extern int16 LoadMissions(char * Name);
extern int16 LoadLevel(char * Name);

#endif

