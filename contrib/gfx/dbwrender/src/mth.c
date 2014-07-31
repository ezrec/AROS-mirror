/************************************************************************
 *                                                                      *
 *                 Copyright (c) 1987, David B. Wecker                  *
 *                       All Rights Reserved                            *
 *                                                                      *
 * This file is part of DBW_Render                                      *
 *                                                                      *
 * DBW_Render is distributed in the hope that it will be useful, but    *
 * WITHOUT ANY WARRANTY. No author or distributor accepts               *
 * responsibility to anyone for the consequences of using it or for     *
 * whether it serves any particular purpose or works at all, unless     *
 * he says so in writing. Refer to the DBW_Render General Public        *
 * License for full details.                                            *
 *                                                                      *
 * Everyone is granted permission to copy, modify and redistribute      *
 * DBW_Render, but only under the conditions described in the           *
 * DBW_Render General Public License. A copy of this license is         *
 * supposed to have been given to you along with DBW_Render so you      *
 * can know your rights and responsibilities. It should be in a file    *
 * named COPYING. Among other things, the copyright notice and this     *
 * notice must be preserved on all copies.                              *
 ************************************************************************
 *                                                                      *
 * Authors:                                                             *
 *     DBW - David B. Wecker                                            *
 *                                                                      *
 * Versions:                                                            *
 *     V1.0 870125 DBW     - First released version                     *
 *                                                                      *
 ************************************************************************/

#define MODULE_MATH
#include "ray.h"

void veczero(v)          /* return the zero vector */
vector v;
{
     v[0] = 0.0;
     v[1] = 0.0;
     v[2] = 0.0;
}

void veccopy(from,to)    /* copy vector 'from' into vector 'to' */
vector from,to;
{
     to[0] = from[0];
     to[1] = from[1];
     to[2] = from[2];
}

void vecdump(v,str)      /* diagnostic vector printout */
vector v;
char *str;
{
     int i;

     printf("%s\t",str);
     for (i = 0; i < 3; i++)
          printf("%15.8f ",v[i]);
     printf("\n");
}

float hlsvalue(n1,n2,hue)
float n1,n2,hue;
{
     while (hue >= 360)
          hue -= 360;
     while (hue < 0)
          hue += 360;
     if (hue < 60)
          return n1 + ((n2 - n1) * hue / 60);
     if (hue < 180)
          return n2;
     if (hue < 240)
          return n1 + ((n2 - n1) * (240 - hue) / 60);
     return n1;
}

/* see also: macro CV */
void cv(x,y,z,v)    /* convert cartesian position to vector */
float   x,y,z;
vector         v;
{
     v[0] = x;
     v[1] = y;
     v[2] = z;
}

void hls(h,l,s,v)
float h,l,s;
vector v;
{
     float m1,m2;

     if (s == 0) 
     {
          CV(l,l,l,v);   /* saturation 0, colorvector luminance only */
     }
     else                /* we have color saturation, compute hls    */
     {
          m2 = (l <= .5) ? l * (1 + s) : l * (1 - s) + s;
          m1 = 2 * l - m2;
          CV(  hlsvalue(m1,m2,h - 120),
            hlsvalue(m1,m2,h),
            hlsvalue(m1,m2,h + 120),
            v);
     }
}

void vecsub(v1,v2,r)          /* vector r = vector 1 - vector 2 */
vector v1,v2,r;
{
     r[0] = v1[0] - v2[0];
     r[1] = v1[1] - v2[1];
     r[2] = v1[2] - v2[2];
}

void vecsum(v1,v2,r)          /* vector r = vector 1 + vector 2 */
vector v1,v2,r;
{
     r[0] = v1[0] + v2[0];
     r[1] = v1[1] + v2[1];
     r[2] = v1[2] + v2[2];
}

void vecmul(v1,v2,r)          /* vector r = vector 1 * vector 2 */
vector v1,v2,r;
{
     r[0] = v1[0] * v2[0];
     r[1] = v1[1] * v2[1];
     r[2] = v1[2] * v2[2];
}

void vecdiv(v1,v2,r)          /* vector r = vector 1 / vector 2 */
vector v1,v2,r;
{
     r[0] = v1[0] / v2[0];
     r[1] = v1[1] / v2[1];
     r[2] = v1[2] / v2[2];
}

void vecscale(s,v,r)          /* vector r = vector 1 * scalar   */
float s;
vector v,r;
{
     r[0] = s * v[0];
     r[1] = s * v[1];
     r[2] = s * v[2];
}

/* see also: macro NORM */
float norm(v)                 /* scalar = |v| */
vector v;
{
     return (float)sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
}

void normalize(v)             /* return normalized (unit) vector */
vector v;
{
     float     n;

     n = 1.0 / NORM(v);
     vecscale(n,v,v);
}

float dot(v1,v2)
vector v1,v2;
{
     float result;

     result  = v1[0] * v2[0];
     result += v1[1] * v2[1];
     result += v1[2] * v2[2];

     return result;
}

void cross(v1,v2,r)
vector v1,v2,r;
{
     r[0] = (v1[1]*v2[2]) - (v2[1]*v1[2]);
     r[1] = (v1[2]*v2[0]) - (v1[0]*v2[2]);
     r[2] = (v1[0]*v2[1]) - (v2[0]*v1[1]);
}

void direction(f,t,d)    /* return unit direction vector f->t */
vector     f,t,d;
{
     VECSUB(t,f,d);
     normalize(d);
}

long curstack()          /* return stack position */
{
     char    dummy[5];
     return((long)(dummy));
}

