/****************************************************************************
*                                                                           *
* MyMath.h                                                           970319 *
*                                                                           *
* Definitions for math                                                      *
*                                                                           *
****************************************************************************/

#ifndef MYMATH_H
#define MYMATH_H

#include <math.h>

#include "MyTypes.h"

/* Vector */

typedef struct
    {
        fix x;
        fix y;
        fix z;
    } Vct;

typedef struct
    {
        Vct a;
        Vct b;
        Vct c;
    } Matrix;

#include "Level.h"

extern void CalcNormalToSide(pSide MySide);
extern void Normal(Vct * a, Vct * b, Vct * c);
extern void VctSub(Vct * a, Vct * b, Vct * c);
extern void VctAdd(Vct * a, Vct * b, Vct * c);
extern fix  VctDot(Vct * a, Vct * b);
extern void VctMulMat(Vct * v, Matrix * M, Vct * V);
extern void VctMulFix(Vct * a, fix b, Vct * c);
extern fix  VctLength(Vct * a);
extern void Vct2Mat(Vct * a, Vct * b, Vct * c, Matrix * m);
extern void RevMat(Matrix * m1, Matrix * m2);

/*
extern double sqrt(double);
extern double pow(double,double);
*/

#endif
