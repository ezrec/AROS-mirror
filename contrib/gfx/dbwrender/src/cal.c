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
 *     DBW - David B. Wecker                                            *
 *                                                                      *
 * Versions:                                                            *
 *     V1.0 870125 DBW  - First released version                        *
 *                                                                      *
 ************************************************************************/

#define MODULE_CALC
#include "ray.h"

void spherenormal(center,p,n)
vector   center,p,n;
{
     DIRECTION(center,p,n);
}

void planenormal(ve,vp,n)
vector ve, vp, n;
{
     cross(vp,ve,n);
     normalize(n);
}

void calcripple(point,w,ripple)
vector  point;
int     w;
vector  ripple;
{
     float riprad,temp1,temp2,temp3,damper;
     int iriprad;

     direction(point,wave[w].center,ripple); /* unit vector pointing away from wave center */

     temp1 = wave[w].center[0] - point[0];
     temp2 = wave[w].center[1] - point[1];
     temp3 = wave[w].center[2] - point[2];

     temp1 = temp1 * temp1;  /* square it */
     temp2 = temp2 * temp2;  /* square it */
     temp3 = temp3 * temp3;  /* square it */

     riprad = (float)sqrt(temp1 + temp2 + temp3);  /* distance from center to point */
     riprad += wave[w].propagate * wave[w].wavelength;  /* move it */

     riprad /= wave[w].wavelength;  /* scale half-wave crest to 0..1 */
     if (wave[w].drag == 1.0) 
          damper = 1.0;
     else 
          damper = (float)pow(wave[w].drag,riprad);  /* exponential damping */

     iriprad = (int) riprad;
     if (iriprad & 1)  
     {               /* negate it */
          ripple[0] = -ripple[0];
          ripple[1] = -ripple[1];
          ripple[2] = -ripple[2];
     }
     riprad -= (float) iriprad;  /* just get fraction 0..1 */
     riprad -= 0.5;  /* scale to -0.5 .. +0.5 */
     if (riprad < 0.0) 
          riprad = -riprad;  /* absolute value */
     riprad  = 0.5 - riprad;  /* invert */
     riprad *= damper;
     riprad *= wave[w].amplitude;
     vecscale(riprad,ripple,ripple);  /* scale bend */
}

void noise3(point,total)
vector  point,
  total;
{
     vector ripple;
     float s;
     int w;

     veczero(total);
     for (w = 0; w < numwaves; w++) 
     {
          calcripple(point,w,ripple);  /* calculate the wave perturbation */
          vecsum(ripple,total,total); /* sum the ripple bends */
     }
     s = NORM(total);
     if (s < 0.001) 
          total[0] = 1.0;
     else 
     {
          s = 1.0 / s;
          vecscale(s,total,total);  /* Make it unit vector */
     }
}


float noise(point)
vector point;
{
     vector total;
     float s;

     noise3(point,total);
     s = total[0] + total[1] + total[2];  /* sum is between -sqrt(3)..+sqrt(3) */
     s += sqrt3;  /* now between 0.0 and 2*sqrt(3) */
     s /= sqrt3times2;  /* scale to range 0..1 */

     return( s );
}

float turbulence(point)
vector point;
{
     vector temp;
     float t,scale;

     if (numwaves > 0) 
     {
          t = 0.0;
          scale = 1.0;

          while (scale < 64.0) 
          {
               vecscale(scale,point,temp);
               t += fabs(noise(temp) / scale);
               scale *= 2.0;
          }
     }
     else 
     {
          t = 1.5;
     }

     return( t );
}

void dodirection(val,eye2,d2,atten,amblits)
vector val;
vector eye2;
vector d2;
float  atten;
int    amblits;
{
     vector transparency,nextval,best_p,n,temp,q,eye,d;

#ifdef MCH_AMIGA
     node *best_obj = 1L;
#else
     node *best_obj = (node *)1L;
#endif

     float best_t,qnorm,ddotn,n1,n2;
     int hitnext;

     /* make local modifyable copies */
     veccopy(eye2,eye);
     veccopy(d2,d);
     veczero(val);
     CV(1.0,1.0,1.0,transparency);
     all_intersects(eye,d,0);

     hitnext = 0;
     while ( best_obj && 
       (transparency[0] > 0.01 ||
       transparency[1] > 0.01 || 
       transparency[2] > 0.01 ) ) 
     {
          best_obj = get_next_intersection(hitnext,best_p,&best_t);
          if (best_obj) 
          {
               getval(nextval,best_obj,best_p,d,atten,amblits);
               vecmul(transparency,nextval,nextval);
               vecsum(val,nextval,val);
               vecmul(best_obj->attr.tra,transparency,transparency);

               if (transparency[0] > 0.01 ||  /* Can some light get through? */
                 transparency[1] > 0.01 ||
                 transparency[2] > 0.01)
                    if ((best_obj->attr.idx != idxref) ||  /* Is it refractive? */
                      (best_obj->kind     == SPHERE)) 
                    {               /* Is it a volume object? */

                         /* apply refraction by changing ray direction and origin */

                         findnormal(best_obj,best_p,n);
                         ddotn = DOT(d,n);

                         if (ddotn < 0.0) 
                         {
                              n2 = best_obj->attr.idx;
                              n1 = idxref;
                         }
                         else 
                         {
                              n1 = best_obj->attr.idx;
                              n2 = idxref;
                              n[0] = -n[0];  /* Negate the normal */
                              n[1] = -n[1];
                              n[2] = -n[2];
                              ddotn = -ddotn;
                         }

                         vecscale(ddotn,n,temp);
                         vecsub(d,temp,temp);
                         vecscale(n1 / n2,temp,q);

                         qnorm = NORM(q);  /* magnitude */
                         if (qnorm >= 1.0)
                              best_obj = NULL;  /* past critical angle,no more light */
                         else 
                         {
                              vecscale(sqrt(1.0-(qnorm*qnorm)),n,temp);
                              vecsub(q,temp,d);
                              veccopy(best_p,eye);
                              hitnext = -1;  /* new direction vector,so re-depth-sort intersections */
                              all_intersects(eye,d,0);  /* Compute intersections with new ray */
                         }
                    }
               hitnext++;
          }
     }
     vecmul(backgroundval,transparency,nextval);
     vecsum(val,nextval,val);
}

