/************************************************************************
 *                                                                      *
 *                  Copyright (c) 1987, David B. Wecker                 *
 *                         All Rights Reserved                          *
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
#                                                                       #
# Authors:                                                              #
#     DBW - David B. Wecker                                             #
#     jhl - John H. Lowery (IBM port)                                   #
#                                                                       #
# Versions:                                                             #
#     V1.0 870125 DBW     - First released version                      #
#     V2.0 880000 jhl - ported to IBM PC (MCGA/VGA display hardware)    #
#                                                                       #
 ************************************************************************/

#define MODULE_EXTENT
#include "ray.h"

node *masternp = NULL;
node **nppstack[100] = 
{
     &masternp,
}
;
int  nppsp = 0;

#define   LARGEST     ((float)1.7e+38)

#define   curnpp         (nppstack[nppsp])
#define   pushnpp(npp)   (nppstack[++nppsp] = npp)
#define   popnpp         (nppstack[nppsp--])

void sphere_rextent();
void triangle_rextent();
void quad_rextent();
void ring_rextent();
void cyl_rextent();
void cone_rextent();



void setextent(ep,re)
extent *ep;
rextent *re;
{
     int i;
     float t;

     for (ep->radius = 0.,i = 0; i < 3; i++) 
     {
          ep->center[i] = (re->min[i] + re->max[i]) / 2.0;
          t = re->max[i] - ep->center[i];
          ep->radius += t * t;
     }
     ep->radius = (float)sqrt(ep->radius);
}

/*
 *   find the bounding extent (sphere) for the current object
 */

void getextent(np,re)
node *np;
rextent *re;
{
     rextent newre;

     cv(LARGEST,LARGEST,LARGEST,re->min);     /* V1.01  */
     cv(-LARGEST,-LARGEST,-LARGEST,re->max);  /* V1.01  */

     while (np) 
     {
          switch (np->kind) 
          {
               /* user-defined extent */
          case EXTENT :

               getextent(eptr(np)->sub,&newre);
               setextent(np,&newre);
               break;

               /* planar objects */
          case TRIANGLE:

               triangle_rextent(np,&newre); 
               break;
          case QUAD:

               quad_rextent(np,&newre); 
               break;
          case RING:

               ring_rextent(np,&newre); 
               break;

               /* volume objects */
          case SPHERE:

               sphere_rextent(np,&newre); 
               break;
          case CYLINDER:
               /* in work */
               cyl_rextent(np,&newre);
               break;

          }
          unionrextent(re,&newre,re);
          np = np->next;
     }
}

/*
 *   The bounding extent for a sphere is the sphere itself.
 */

void sphere_rextent(sp,re)
sphere *sp;
rextent *re;
{
     int i;

     for (i = 0; i < 3; i++) 
     {
          re->min[i] = sp->center[i] - sp->radius;
          re->max[i] = sp->center[i] + sp->radius;
     }
}

/*
 *   The bounding extent for a triangle is
 */

void triangle_rextent(tp,re)
triangle *tp;
rextent *re;
{
     vector v1,v2;

     cv(LARGEST,LARGEST,LARGEST,re->min);     /* V1.01  */
     cv(-LARGEST,-LARGEST,-LARGEST,re->max);  /* V1.01  */

     vecsum(tp->position,tp->ve,v1);
     vecsum(tp->position,tp->vp,v2);

     unionvector(re,tp->position,re);
     unionvector(re,v1,re);
     unionvector(re,v2,re);
}

void quad_rextent(qp,re)
quad *qp;
rextent *re;
{
     vector v;

     cv(LARGEST,LARGEST,LARGEST,re->min);     /* V1.01  */
     cv(-LARGEST,-LARGEST,-LARGEST,re->max);  /* V1.01  */

     unionvector(re,qp->position,re);

     vecsum(qp->position,qp->ve,v);
     unionvector(re,v,re);

     vecsum(qp->position,qp->vp,v);
     unionvector(re,v,re);

     vecsum(v,qp->ve,v);
     unionvector(re,v,re);
}

/*
 *   The bounding extent for a ring is a sphere with
 *   the same radius as the outside radius of the ring.
 */

void ring_rextent(rp,re)
ring *rp;
rextent *re;
{
     int i;

     for (i = 0; i < 3; i++) 
     {
          re->min[i] = rp->position[i] - rp->maxrad;
          re->max[i] = rp->position[i] + rp->maxrad;
     }
}

/* 
 *   The bounding extent for a cylinder is a sphere
 *   in which the cylinder will fit
 */

void cyl_rextent(rp,re)
cylinder *rp;
rextent *re;
{
}

