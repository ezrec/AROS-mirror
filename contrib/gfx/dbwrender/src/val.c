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

#define MODULE_VAL
#include "ray.h"

void getval(val,np,p,d,atten,ambientlight)
vector      val,
  p,
  d;
node        *np;
float       atten;
int         ambientlight;
{
     vector      pseudodirection,
     pseudointensity,
     tp,
     lightdir,
     lightint,
     penumbra,
     transparency,
     best_p,
     nval,
     n,
     r,
     temp,
     specintensity,
     diffuse,
     v1;
     float       best_t,
       ndotdir,
       t,
       texture,
       td,
       pointdist,
       umbscale;
     float       ndotd,
       ambientscale;
     int         hitnext,
       lit,
       i,
       itd;
     node        *bouncenp,
     *occlude;
     vector      light_atten[MAXLIT];
     int         l_objcounter = 0;
     float       l_distances[MAXOBJ];
     node        *l_objpairs[MAXOBJ];
     vector      l_points[MAXOBJ];

     if (allopaque == 0) 
     { /* If not opaque,then save the ray intersection list */
          l_objcounter = g_objcounter;
          for (i = 0; i < l_objcounter; i++) 
          {
               l_distances[i] = g_distances[i];
               l_objpairs[i] = g_objpairs[i];
               veccopy(g_points[i],l_points[i]);
          }
     }

     veczero(val);  /* start with output value of zero */

     findnormal(np,p,n);
     ndotd = DOT(n,d);
     if (ndotd > 0.0) 
     {          /* We're looking at surface's back side */
          ndotd = -ndotd;
          n[0] = -n[0];  /* Flip the normal */
          n[1] = -n[1];
          n[2] = -n[2];
     }

     /* Initialize the light attenuation values with 'not-yet-computed' flag */

     for (lit = 0; lit < numlits; lit++)
          light_atten[lit][0] = -99.9;

     /* Apply procedural texture */
     gettex(diffuse,np,p,n);

     /*---------------------------------------------------------------------*/
     /* Compute diffuse reflection intensity */

     if (diffuse[0] > 0.01 || diffuse[1] > 0.01 || diffuse[2] > 0.01) 
     {

          /* First,compute light shining directly on this point */
          for (lit = 0; lit < numlits; lit++) 
          { 
               veccopy(light[lit].direction,lightdir);
               veccopy(light[lit].intensity,lightint);

               pointdist = 0.0;

               if (light[lit].kind != 0) 
               { /* If point source,compute local direction */
                    vecsub(lightdir,p,lightdir);
                    pointdist = NORM(lightdir);  /* distance to point light source */
                    if (pointdist == 0.0)
                         pointdist = 0.01;
                    umbscale  = light[lit].distscale;
                    umbscale /= pointdist * pointdist;
                    vecscale(umbscale,lightint,lightint);
                    CV(rnd(),rnd(),rnd(),penumbra);
                    if (rnd() < 0.5)
                         penumbra[0] = -penumbra[0];
                    if (rnd() < 0.5)
                         penumbra[1] = -penumbra[1];
                    if (rnd() < 0.5)
                         penumbra[2] = -penumbra[2];
                    normalize(penumbra);  /* penumbra is now random unit vector */

                    umbscale  = rnd();
                    umbscale -= 0.05;
                    umbscale *= light[lit].radius;
                    vecscale(umbscale,penumbra,penumbra);
                    vecsum(penumbra,lightdir,lightdir);
                    normalize(lightdir);
               }

               /* Now that we know where the light is shining from,compute diffuse
               (lamertian) reflection */

               ndotdir = DOT(n,lightdir);
               if (ndotdir > 0.0) 
               {
                    vecmul(lightint,diffuse,nval);
                    vecscale(ndotdir,nval,nval);
               }
               else
               {
                    veczero(nval);  /* self shadowing -- backside of spheres,etc. */
               }

               /* If the computed diffuse surface brightness is above a certain
               threshold level,then compute the shadow attenuation */

               if (nval[0] > 0.01 || nval[1] > 0.01 || nval[2] > 0.01) 
               {
                    getatten(light_atten[lit],p,lightdir,lit,pointdist);
                    vecmul(nval,light_atten[lit],nval); /* attenuate */
               }
               vecsum(nval,val,val);  /* Sum up for all light sources */
          } /* for */

          /*---------------------------------------------------------------------*/
          /* Next,compute light shining indirectly from any surfaces on this    */
          /* point.  This is done to approximate true ambient illumination.      */

          for (lit = 0; lit < ambientlight; lit++) 
          {
               ambientscale = 1.0 / (float) ambientlight;
               /* ?? not sure how to factor in distance from diffuse reflector */

               CV(rnd(),rnd(),rnd(),lightdir);
               if (rnd() < 0.5)
                    lightdir[0] = -lightdir[0];
               if (rnd() < 0.5)
                    lightdir[1] = -lightdir[1];
               if (rnd() < 0.5)
                    lightdir[2] = -lightdir[2];
               normalize(lightdir);  /* lightdir is now random unit vector */

               ndotdir = DOT(n,lightdir);
               if (ndotdir < 0.0) 
               { /* if not in upper hemisphere,invert it */
                    lightdir[0] = -lightdir[0];
                    lightdir[1] = -lightdir[1];
                    lightdir[2] = -lightdir[2];
                    ndotdir = -ndotdir;
               }

               /* okay,now the direction is known.  What is visible there? */
               dodirection(nval,p,lightdir,ambientscale,
                 ambientlight * amblitnum / amblitdenom);

               val[0] += nval[0] * ndotdir * diffuse[0];
               val[1] += nval[1] * ndotdir * diffuse[1];
               val[2] += nval[2] * ndotdir * diffuse[2];

          } /* for */


          /*---------------------------------------------------------------------*/
          /* Next,compute light shining indirectly from mirrors on this point   */

          if (dopseudo) 
          {
               for (lit = 0; lit < numlits; lit++) 
               { /* for each true light... */
                    bouncenp = root;
                    while (bouncenp) 
                    { /* for each possible mirror... */
                         if (bouncelighting(pseudointensity,pseudodirection,bouncenp,lit)
                           && bouncenp != np) 
                         { /* don't check reflections from ourselves */
                              /* Okay,this object reflects light.  Does any of it reach us? */
                              CV(1.0,1.0,1.0,transparency);
#ifdef MCH_AMIGA
                              occlude = 1L;
#else
                              occlude = (node *)1L;
#endif
                              hitnext = 0;
                              all_intersects(p,pseudodirection,0);

                              while (occlude &&  (transparency[0] > 0.01 ||
                                transparency[1] > 0.01 ||
                                transparency[2] > 0.01)) 
                              {
                                   if ((occlude = get_next_intersection(hitnext,best_p,&best_t)))
                                   {
                                        /* pseudolight intersects an object,see if any passes through */
                                        if (occlude == bouncenp)
                                             occlude = 0;  /* hit the mirror causing the pseudolight,stop */
                                        else
                                        {
                                             vecmul(transparency,occlude->attr.tra,
                                               transparency);
                                             hitnext++;
                                        }
                                   }
                                   else
                                   {
                                        /* no more objects to intersect -- we didn't intersect the mirror
                                        that is causing the pseudolight,so cancel the pseudolight */
                                        veczero(transparency);
                                   }
                              }

                              if (transparency[0] > 0.01 || transparency[1] > 0.01 ||
                                transparency[2] > 0.01) 
                              {
                                   ndotdir = DOT(n,pseudodirection);
                                   if (ndotdir > 0.0) 
                                   {
                                        for (i = 0; i < 3; i++)
                                             val[i] += transparency[i] * pseudointensity[i] * 
                                               ndotdir * diffuse[i];
                                   }
                              }
                         } /* if */
                         bouncenp = bouncenp->next;  /* next possible mirror */
                    } /* while */
               } /* for */
          } /* if dopseudo */
     } /* if any diffuse reflectivity */

     /*---------------------------------------------------------------------*/
     /* Compute mirror & specular reflection intensity */

     atten *= np->attr.ref;  /* factor in the object's relative reflectivity */

     if (atten > 0.01) 
     { /* Is object meaningfully reflective still */
          vecscale(2 * ndotd,n,r);
          vecsub(d,r,r);

          dodirection(nval,p,r,atten,ambientlight);  /* Mirror reflection */
          vecscale(atten,nval,nval);  /* attenuate by reflectivity coeff */

          /* We really want Fresnel angle factor here,but... */
          if (np->kind == SPHERE) 
          { /* Add sparkle */
               t = DOT(n,r);
               if (t >= 0.0) 
               {
                    veccopy(nval,specintensity);
                    t = 1.0 - t;
                    t = t * t * t * t * t;
                    vecscale(t,specintensity,specintensity);
                    vecsum(nval,specintensity,nval);
               }
          }

          /* Compute specular reflection intensity */

          for (lit = 0; lit < numlits; lit++) 
          {
               veccopy(light[lit].direction,lightdir);
               veccopy(light[lit].intensity,lightint);
               pointdist = 0.0;

               if (light[lit].kind != 0) 
               { /* If point source,compute local direction */
                    vecsub(lightdir,p,lightdir);
                    pointdist = NORM(lightdir);  /* distance to point light source */
                    if (pointdist == 0.0)
                         pointdist = 0.01;
                    umbscale  = light[lit].distscale;
                    umbscale /= pointdist * pointdist;
                    vecscale(umbscale,lightint,lightint);
                    normalize(lightdir);
               }

               t = DOT(lightdir,r);
               if (t > 0.0) 
               {              /* Check self-shadowing */
                    if (np->attr.fuz == 0.0)
                         td = 1000.0;  /* extremely smooth mirror */
                    else
                    {
                         td = 2.0/np->attr.fuz;  /* calc specular decay rate based on fuz */
                         if (td < 1.0)
                              td = 1.0;
                    }

                    t = pow(t,td)  /* calc reflectivity for desired gloss */
                      * atten;  /* attenuate specular reflections by reflectivity coeff */

                    if (t > 0.01) 
                    { /* specular coefficient is meaningful,add it */
                         /* first compute shadowing of light source causing specular refl. */
                         if (light_atten[lit][0] < 0.0)  /* Not already computed,compute */
                              getatten(light_atten[lit],p,lightdir,lit,pointdist);

                         nval[0] += lightint[0] * light_atten[lit][0] * t;
                         nval[1] += lightint[1] * light_atten[lit][1] * t;
                         nval[2] += lightint[2] * light_atten[lit][2] * t;
                    }
               }
          }

          vecsum(nval,val,val); /* Add mirror/specular reflections to running total */
     } /* if reflective atten is non-zero */



     /*----------------------------------------------------------------------*/
     /* Add in the object's ambient intensity */
     if (ambscale < 0.0) 
     {
          vecsum(np->attr.amb,val,val);
     }
     else
     { /* use specified computed ambient */
          val[0] += diffuse[0] * ambscale;
          val[1] += diffuse[1] * ambscale;
          val[2] += diffuse[2] * ambscale;
     }

     /*---------------------------------------------------------------------*/
     /* Apply any post-process global texture functions */

     if (numhazes > 0) 
     {
          /* Blend the final color toward the specified haze color */
          vecsub(p,eye,temp);
          texture  = NORM(temp);  /* distance to point from eye */
          texture /= haze[numhazes-1].distscale;  /* scaled according to user */
          blendcolor(val,haze[numhazes-1].color,texture,val);
     }

     if (allopaque == 0) 
     { /* If not opaque,then restore the ray intersection list */
          g_objcounter = l_objcounter;
          for (i = 0; i < g_objcounter; i++) 
          {
               g_distances[i] = l_distances[i];
               g_objpairs[i] = l_objpairs[i];
               veccopy(l_points[i],g_points[i]);
          }
     }
}

