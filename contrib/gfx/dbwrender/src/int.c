/************************************************************************
 *                                                                      *
 *                  Copyright (c) 1987, David B. Wecker                 *
 *                          All Rights Reserved                         *
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
 *      DBW - David B. Wecker                                           *
 *                                                                      *
 * Versions:                                                            *
 *      V1.0 870125 DBW - First released version                        *
 *                                                                      *
 ************************************************************************/

/* INT.C - calculations for ray intersections with primitives */

#define MODULE_INTER
#include "ray.h"

node *get_next_intersection(which,best_p,best_t)
int         which;
vector      best_p;
float       *best_t;
{
     /* This routine returns the pertinent data for the i'th intersection */

     if (which >= g_objcounter)
          return NULL;  /* No more intersections */
     else
     {
          *best_t   = g_distances[which];
          veccopy(g_points[which],best_p);
          return g_objpairs[which];
     }
}


void add_intersection(np,p,t,attenuating)
node        *np;
vector      p;
float       t;
int         attenuating;
{
     int add;

/* 
 *   This routine adds the passed intersection information to the global list
 *   of intersections.  As a performance optimization,if we know that all
 *   objects are opaque,then rather than accumulating the entire list and
 *   sorting it later,we merely keep track of the closest intersection.
 */
     add = FALSE;

     if ( allopaque    == 0 ||  /* Some transparency, all everything */
          g_objcounter == 0 ||  /* First opaque intersection         */
          t < g_distances[0])
     {
          add = TRUE;  /* Nearer opaque intersection */
     }
     
     if ( attenuating &&
         np->attr.tra[0] >= .99 &&
         np->attr.tra[1] >= .99 &&
         np->attr.tra[2] >= .99)
     {
               add = FALSE;  /* ignore invisible objects when doing shadows */
     }

     if (add) 
     {
          g_distances[g_objcounter] = t;
          veccopy(p,g_points[g_objcounter]);
          g_objpairs[g_objcounter] = np;
          if (g_objcounter < MAXOBJ)
               g_objcounter++;
     }
}

void calc_intersections(np,eye,d,attenuating)
node      *np;           /* current node                     */
vector    d;             /* direction vector for current ray */
vector    eye;           /* current ray origin (eye point)   */
int       attenuating;   /* attenuation factor for each hit  */
{

/* 
 *   This routine calculates the intersection of a ray with a given object.
 *   If an intersection is found,the appropriate parameters are added to a
 *   global list.  This routine is recursive,to handle nested extents. 
 */

     vector      p;
     float       t;
     int         hit;
     long        curs;

     /* keep track of maximum stack extent */
     curs = curstack();
     if (curs < stackbot)
          stackbot = curs;

     while (np) 
     {
          hit = FALSE;
          switch (np->kind) 
          {
          case EXTENT :
               if (hitsphere(eptr(np)->center,eptr(np)->radius,eye,d,p,&t))
                    calc_intersections(eptr(np)->sub,eye,d,attenuating);
               break;

          case SPHERE :
               hit = hitsphere(sptr(np)->center,sptr(np)->radius,eye,d,p,&t);
               break;

          case TRIANGLE :
               hit = hittriangle(np,eye,d,p,&t);
               break;

          case QUAD :
               hit = hitquad(np,eye,d,p,&t);
               break;

          case RING :
               hit = hitring(np,eye,d,p,&t);
               break;

          case CYLINDER :
               hit = hitcylinder(np,eye,d,p,&t);
               break;

          default :
               hit = FALSE;
               break;
          }

          if (hit)
               add_intersection(np,p,t,attenuating);  /* Add to global list */

          np = np->next;  /* On to the next object */
     }
}

void all_intersects(eye,d,attenuating)
vector  d;
vector  eye;
int     attenuating;
{

/* 
 *   This routine computes all of the ray-object intersections that occur for
 *   the specified ray.  The intersections are stored in global arrays, sorted
 *   closest intersection to furthest. 
 */

     g_objcounter = 0;  /* Clear the intersection list */

     calc_intersections(root,eye,d,attenuating);  /* Find them & add to list */

     if (g_objcounter > 1)
          shell(g_distances,g_points,g_objpairs,g_objcounter);  /* Sort them */

     /* Keep statistics (and decide when to GIVE UP!!) */
     if (++curr_runs >= max_runs)
          longjmp(env,-1);

     if (g_objcounter > max_intersects)
          max_intersects = g_objcounter;
}

int bouncelighting(pseudointensity,pseudolitdir,bouncenp,lit)
vector  pseudointensity,
  pseudolitdir;
node    *bouncenp;
int     lit;
{
     vector n,bouncepoint,lightdir;

     if (bouncenp->kind == SPHERE || bouncenp->attr.ref == 0.0)
          return FALSE;  /* No pseudo light source from here */
     else
     { 
          switch (bouncenp->kind) 
          {
          case TRIANGLE :
               veccopy(tptr(bouncenp)->position,bouncepoint);
               break;
          case QUAD     :
               veccopy(qptr(bouncenp)->position,bouncepoint);
               break;
          case RING     :
               veccopy(rptr(bouncenp)->position,bouncepoint);
               break;
          }
          findnormal(bouncenp,bouncepoint,n);
          veccopy(light[lit].direction,lightdir);  /* Assume directional light */
          if (light[lit].kind != 0) 
          { /* Point source */
               DIRECTION(bouncepoint,lightdir,lightdir);
          }

          vecscale(2.0 * DOT(lightdir,n),n,pseudolitdir);
          vecsub(lightdir,pseudolitdir,pseudolitdir);
          vecscale(bouncenp->attr.ref,light[lit].intensity,pseudointensity);
          return TRUE;
     }
}

void blendcolor(original,target,scale,result)
vector original;
vector target;
vector result;
float  scale;
{
     vector path;

     if (scale < 0.0)
          scale = 0.0;  /* clip off excessive blending */
     if (scale > 1.0)
          scale = 1.0;
     vecsub(target,original,path);
     vecscale(scale,path,path);
     vecsum(original,path,result);
}

void getatten(atten,p,d,lit,pointdist)
vector atten;
vector p;
vector d;
int    lit;
float  pointdist;
{
     vector  best_p;
     float   best_t;
     int     hitnext;

#ifdef MCH_AMIGA
     node    *occlude = 1L;
#else
     node    *occlude = (node *)1L;
#endif

     atten[0] = 1.0;  /* assume no attenuation */
     atten[1] = 1.0;
     atten[2] = 1.0;
     hitnext = 0;
     all_intersects(p,d,1);  /* Compute all intersections */

     while (occlude && (atten[0] > 0.01 || atten[1] > 0.01 || atten[2] > 0.01))
          if (occlude = get_next_intersection(hitnext,best_p,&best_t)) 
          {
               if (light[lit].kind == 0 || best_t <= pointdist) 
               {
                    vecmul(atten,occlude->attr.tra,atten);
               }
               hitnext++;
          }
}

