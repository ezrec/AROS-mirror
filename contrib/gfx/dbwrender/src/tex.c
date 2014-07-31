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

#define MODULE_TEXTURE
#include "ray.h"

vector  white   = 
{
     1.0 ,1.0 ,1.0 
}
;

void gettex(diffuse,np,p,n)
vector  diffuse,
  p,
  n;
node    *np;
{
     int         whichtexture,
       i,
       j,
       ibx,
       iby,
       ibz;
     float       texture,
       bx,
       by,
       bz;
     long        curs;

     /* keep track of maximum stack extent */
     curs = curstack();
     if (curs < stackbot)
          stackbot = curs;

     whichtexture = np->attr.tex;

     /* This is copied to a temp so that local solid texturing may be applied
     non-destructively. */

     veccopy(np->attr.dif,diffuse); /* start with the object's defined color */

     if (whichtexture != 0) 
     { /* don't bother checking if there's no texture */
          if (whichtexture >= 20 && whichtexture < 30) 
          { /* MARBLE */
               j = whichtexture - 20;  /* select the desired marble */
               texture  = p[0] * marble[j].xscale;
               texture += turbulence(p) * marble[j].turbscale;
               texture  = (sin(texture) + 1.0) / 2.0;
               for (i = 0; i < marble[j].squeeze; i++)
                    texture = texture * texture;  /* squeeze peaks thinner */
               blendcolor(diffuse,marble[j].veincolor,texture,diffuse);
          }

          else if (whichtexture >= 40 && whichtexture < 50) 
          { /* WOOD */
               /*mulvm(p,woodorient,tp);*/
               j = whichtexture - 40;  /* select the desired wood */
               texture  = sqrt(p[0]*p[0] + p[2]*p[2]);
               texture *= wood[j].ringspacing;
               texture += turbulence(p) * wood[j].turbscale;
               texture = (sin(texture) + 1.0) / 2.0;
               for (i = 0; i < wood[j].squeeze; i++)
                    texture = texture * texture;  /* squeeze peaks thinner */
               texture *= wood[j].thickscale;  /* scale 0..n */
               if (texture > 1.0)
                    texture = 1.0;  /* clip to 0..1 */

               blendcolor(diffuse,wood[j].othercolor,texture,diffuse);
          }

          else if (whichtexture >= 50 && whichtexture < 60) 
          { /* CHECKERBOARD */
               j = whichtexture - 50;  /* select the desired checker */
               ibx = 0;
               iby = 0;
               ibz = 0;

               if (checker[j].x != 0.0) 
               {
                    texture = p[0] / checker[j].x;
                    if (texture < 0.0)
                         texture = 1.0 - texture;
                    ibx = (int) texture;
               }
               if (checker[j].y != 0.0) 
               {
                    texture = p[1] / checker[j].y;
                    if (texture < 0.0)
                         texture = 1.0 - texture;
                    iby = (int) texture;
               }
               if (checker[j].z != 0.0) 
               {
                    texture = p[2] / checker[j].z;
                    if (texture < 0.0)
                         texture = 1.0 - texture;
                    ibz = (int) texture;
               }

               if ((ibx+iby+ibz) & 1) 
               {
                    veccopy(checker[j].color,diffuse);
               }
          }

          else if (whichtexture >= 70 && whichtexture < 80) 
          { /* BLEND */
               j = whichtexture - 70;  /* select the desired blend */
               texture = (p[1] - blend[j].start) / blend[j].scale;
               if (texture > 1.0)
                    texture = 1.0;
               if (texture < 0.0)
                    texture = 0.0;
               blendcolor(diffuse,blend[j].color,texture,diffuse);
          }

          else if (whichtexture >= 80 && whichtexture < 90) 
          { /* SNOW */
               j = whichtexture - 80;  /* select the desired snow */
               texture  = p[1] - snow[j].start;
               texture /= snow[j].altscale;
               texture *= snow[j].altfactor;
               texture *= DOT(n,yaxis);
               if (texture > 1.0)
                    texture = 1.0;
               if (texture < 0.0)
                    texture = 0.0;
               if (texture > snow[j].threshhold)
                    texture = 1.0;
               blendcolor(diffuse,white,texture,diffuse);
          }

          else
               switch (whichtexture) 
               { /* MISC SINGULAR TEXTURES */
               case 1  :
                    /* Y axis color gradiation */
                    hls(p[1]/20.0,0.5,1.0,diffuse);
                    break;

               case 3  :
                    /* mottled diffuse */
                    vecscale(noise(p),diffuse,diffuse);
                    break;

               case 5  :
                    /* Brick pattern texture */

                    /* First,check mortar layers in the X-Z plane */
                    by  = p[1] / brickheight;
                    iby = (int) by;
                    by -= (float) iby; /* just save fraction */
                    if (by < 0.0)
                         by += 1.0;
                    if (by <= brickmortarheight) 
                    {
                         /* This point is on the X-Z plane */
                         veccopy(mortardiffuse,diffuse);
                         break;
                    }

                    /* Check odd mortar layers in the Y-Z plane */
                    /* First calculate a common factor */
                    by  = p[1];
                    by /= brickheight;
                    by /= 2.0;
                    iby = (int) by;
                    by -= (float) iby; /* just save fraction */
                    if (by < 0.0)
                         by += 1.0;

                    bx  = p[0] / brickwidth;
                    ibx = (int) bx;
                    bx -= (float) ibx; /* just save fraction */
                    if (bx < 0.0)
                         bx += 1.0;
                    if (bx <= brickmortarwidth) 
                    {
                         /* So this point is on the Y-Z mortar plane.  We now must
                         factor in the odd staggered brick effect */
                         if (by <= 0.5) 
                         {
                              veccopy(mortardiffuse,diffuse);
                              break;
                         }
                    }
                    /* Check even mortar layers in the Y-Z plane */
                    bx  = (p[0] / brickwidth) + 0.5;
                    ibx = (int) bx;
                    bx -= (float) ibx; /* just save fraction */
                    if (bx < 0.0)
                         bx += 1.0;
                    if (bx <= brickmortarwidth) 
                    {
                         /* So this point is on the Y-Z mortar plane.  We now must
                         factor in the even staggered brick effect */
                         if (by > 0.5) 
                         {
                              veccopy(mortardiffuse,diffuse);
                              break;
                         }
                    }

                    /* Check odd mortar layers in the Y-X plane */
                    bz  = p[2] / brickdepth;
                    ibz = (int) bz;
                    bz -= (float) ibz; /* just save fraction */
                    if (bz < 0.0)
                         bz += 1.0;
                    if (bz <= brickmortardepth) 
                    {
                         /* So this point is on the Y-X mortar plane.  We now must
                         factor in the odd staggered brick effect */
                         if (by > 0.5) 
                         {
                              veccopy(mortardiffuse,diffuse);
                              break;
                         }
                    }
                    /* Check even mortar layers in the Y-X plane */
                    bz  = (p[2] / brickdepth) + 0.5;
                    ibz = (int) bz;
                    bz -= (float) ibz; /* just save fraction */
                    if (bz < 0.0)
                         bz += 1.0;
                    if (bz <= brickmortardepth) 
                    {
                         /* So this point is on the Y-X mortar plane.  We now must
                         factor in the even staggered brick effect */
                         if (by <= 0.5) 
                         {
                              veccopy(mortardiffuse,diffuse);
                              break;
                         }
                    }
                    /* So we're inside a brick */
                    break;
               }
     }
}

