/****************************************************************************
*                                                                           *
* Gfx.h                                                              970319 *
*                                                                           *
* This is where all Graphics functions belong to                            *
*                                                                           *
****************************************************************************/

#ifndef GFX_H
#define GFX_H

#include "Level.h"
#include "MyMath.h"

#define MaxRecursion 10
#define Width 1152
#define Height 864
#define DefLightLess 0.975
#define DefCameraBehind 2.0
#define MaxPolSides 100
#define DefTextureSizeX 256
#define DefTextureSizeY 256
#define DefTextureU1    65536.0
#define DefTextureV1    65536.0
#define DefTextureL1    65281.0
#define DefTextureShiftX 8
#define DefTextureShiftY 8
#define DefTextureMaskX 0x000000FF
#define DefTextureMaskY 0x0000FF00

typedef struct Pol3D * pPol3D;
typedef struct Pol3D
    {
        int16 NumPoints;
        fix   MinZ;
        pCube Cube;
        Pnt   Pnts[MaxPolSides+2];
        Vct   uvl1[MaxPolSides+2];
        Vct   uvl2[MaxPolSides+2];
    } Pol3D;

typedef struct Pol2D * pPol2D;
typedef struct Pol2D
    {
        int16 NumPoints;
        Pnt   Pnts[MaxPolSides+2];
        Vct   uvl1[MaxPolSides+2];
        Vct   uvl2[MaxPolSides+2];
    } Pol2D;

typedef struct
    {
        int32 x,y,z;
    } IntVct;

typedef struct Clip2D * pClip2D;
typedef struct Clip2D
    {
        int16  MinX[Height];
        int16  MaxX[Height];
        IntVct MinUVL1[Height];
        IntVct MaxUVL1[Height];
        IntVct MinUVL2[Height];
        IntVct MaxUVL2[Height];
    } Clip2D;

extern int32 MyWidth;
extern int32 MyHeight;
extern int32 HalfWidth;
extern int32 HalfHeight;
extern int32 QuaterWidth;
extern int32 QuaterHeight;
extern int16 Finished;
extern fix   LightLess;
extern fix  CameraBehind;

extern void MainLoop(void);

#endif

