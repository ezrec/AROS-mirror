/****************************************************************************
*                                                                           *
* MyMath.c                                                          970319  *
*                                                                           *
* Definitions for math                                                      *
*                                                                           *
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MyMath.h"
#include "MyTypes.h"

void Vct2Mat(Vct * a, Vct * b, Vct * c, Matrix * m)
{
    m->a.x=a->x;
    m->a.y=a->y;
    m->a.z=a->z;
    m->b.x=b->x;
    m->b.y=b->y;
    m->b.z=b->z;
    m->c.x=c->x;
    m->c.y=c->y;
    m->c.z=c->z;
}

void DetMat(Matrix * m,Vct * a, Vct * b)
{
    fix d;

    d=m->a.x * m->b.y * m->c.z + m->b.x * m->c.y * m->a.z + m->c.x * m->a.y * m->b.z
     -m->a.x * m->c.y * m->b.z + m->b.x * m->a.y * m->c.z + m->c.x * m->b.y * m->a.z;

    if (d==0)
    {
        b->x=0;
        b->y=0;
        b->z=0;
    }
    else
    {
        d=1/d;

        b->x=a->x   * m->b.y * m->c.z + m->b.x * m->c.y * a->z   + m->c.x * a->y   * m->b.z
            -a->x   * m->c.y * m->b.z + m->b.x * a->y   * m->c.z + m->c.x * m->b.y * a->z;

        b->y=m->a.x * a->y   * m->c.z + a->x   * m->c.y * m->a.z + m->c.x * m->a.y * a->z
            -m->a.x * m->c.y * a->z   + a->x   * m->a.y * m->c.z + m->c.x * a->y   * m->a.z;

        b->z=m->a.x * m->b.y * a->z   + m->b.x * a->y   * m->a.z + a->x   * m->a.y * m->b.z
            -m->a.x * a->y   * m->b.z + m->b.x * m->a.y * a->z   + a->x   * m->b.y * m->a.z;

        VctMulFix(b,d,b);
    }
}

void RevMat(Matrix * m1, Matrix * m2)
{
    Matrix m3;

    m3.a.x=1;
    m3.a.y=0;
    m3.a.z=0;

    m3.b.x=0;
    m3.b.y=1;
    m3.b.z=0;

    m3.c.x=0;
    m3.c.y=0;
    m3.c.z=1;

    DetMat(m1,&m3.a,&m2->a);
    DetMat(m1,&m3.b,&m2->b);
    DetMat(m1,&m3.c,&m2->c);
}

fix VctLength(Vct * a)
{
    return (fix)sqrt(a->x*a->x+a->y*a->y+a->z*a->z);
}

void VctSub(Vct * a, Vct * b, Vct * c)
{
    c->x=a->x - b->x;
    c->y=a->y - b->y;
    c->z=a->z - b->z;
}

void VctAdd(Vct * a, Vct * b, Vct * c)
{
    c->x=a->x + b->x;
    c->y=a->y + b->y;
    c->z=a->z + b->z;
}

void VctMulFix(Vct * a, fix b, Vct * c)
{
    c->x = a->x * b;
    c->y = a->y * b;
    c->z = a->z * b;
}

fix VctDot(Vct * a, Vct * b)
{
    fix c;

    c = a->x * b->x;
    c+= a->y * b->y;
    c+= a->z * b->z;

    return c;
}

void Normal(Vct * a, Vct * b, Vct * Normal)
{
    Normal->x=(a->y) * (b->z) - (a->z) * (b->y);
    Normal->y=(a->z) * (b->x) - (a->x) * (b->z);
    Normal->z=(a->x) * (b->y) - (a->y) * (b->x);
}

void CalcNormalToSide(pSide MySide)
{
    Vct a;
    Vct b;
    Matrix m;

    VctSub(MySide->Pnts[1],MySide->Pnts[0],&a);
    VctSub(MySide->Pnts[3],MySide->Pnts[0],&b);

    Normal(&a,&b,&MySide->Normal);

    VctMulFix(&MySide->Normal,1/VctLength(&MySide->Normal),&MySide->Normal);

    VctSub(MySide->Pnts[1],MySide->Pnts[0],&a);
    VctSub(MySide->Pnts[3],MySide->Pnts[0],&b);

    VctMulFix(&a,1/VctLength(&a),&a);
    VctMulFix(&b,1/VctLength(&b),&b);

    Vct2Mat(&a,&b,&MySide->Normal,&m);

    RevMat(&m,&MySide->Rev);

    /*
    for (x=0;x<4;x++)
    {
        printf("Point %f %f %f\n",MySide->Pnts[x]->x,MySide->Pnts[x]->y,MySide->Pnts[x]->z);
    }

    printf("\nNormal %f %f %f\n",MySide->Normal.x,MySide->Normal.y,MySide->Normal.z);
    printf("\nMatrix %f %f %f\n",MySide->Rev.a.x,MySide->Rev.a.y,MySide->Rev.a.z);
    printf("       %f %f %f\n"  ,MySide->Rev.b.x,MySide->Rev.b.y,MySide->Rev.b.z);
    printf("       %f %f %f\n"  ,MySide->Rev.c.x,MySide->Rev.c.y,MySide->Rev.c.z);
    */
}

void VctMulMat(Vct * v, Matrix * M, Vct * V)
{
    V->x=VctDot(v,&M->a);
    V->y=VctDot(v,&M->b);
    V->z=VctDot(v,&M->c);
}

