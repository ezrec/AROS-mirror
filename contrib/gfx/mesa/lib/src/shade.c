/* $Id$ */

/*
 * Mesa 3-D graphics library
 * Version:  3.0
 * Copyright (C) 1995-1998  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log$
 * Revision 1.1  2005/01/11 14:58:32  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.4  1998/07/01 02:39:14  brianp
 * added a hack to work around suspected gcc bug
 *
 * Revision 3.3  1998/04/18 05:00:28  brianp
 * now using FLOAT_COLOR_TO_UBYTE_COLOR macro
 *
 * Revision 3.2  1998/03/27 04:17:31  brianp
 * fixed G++ warnings
 *
 * Revision 3.1  1998/02/02 03:09:34  brianp
 * added GL_LIGHT_MODEL_COLOR_CONTROL (separate specular color interpolation)
 *
 * Revision 3.0  1998/01/31 21:03:42  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <math.h>
#include "macros.h"
#include "mmath.h"
#include "shade.h"
#include "types.h"
#endif



/*
 * Return x^y.
 */
static GLfloat gl_pow( GLfloat x, GLfloat y )
{
   GLdouble z = pow(x, y);
   if (z<1.0e-10)
      return 0.0F;
   else
      return (GLfloat) z;
}



/*
 * Use current lighting/material settings to compute the RGBA colors of
 * an array of vertexes.
 * Input:  side - 0=use front material, 1=use back material
 *         n - number of vertexes to process
 *         vertex - array of vertex positions in eye coordinates
 *         normal - array of surface normal vectors
 * Output:  color - array of resulting colors
 */
void gl_shade_rgba( GLcontext *ctx,
                              GLuint side,
                              GLuint n,
                              /*const*/ GLfloat vertex[][4],
                              /*const*/ GLfloat normal[][3],
                              GLubyte color[][4] )
{
   GLuint j;
   GLfloat baseR, baseG, baseB, baseA;
   GLint sumA;
   struct gl_light *light;
   struct gl_material *mat;

   mat = &ctx->Light.Material[side];

   /*** Compute color contribution from global lighting ***/
   baseR = mat->Emission[0] + ctx->Light.Model.Ambient[0] * mat->Ambient[0];
   baseG = mat->Emission[1] + ctx->Light.Model.Ambient[1] * mat->Ambient[1];
   baseB = mat->Emission[2] + ctx->Light.Model.Ambient[2] * mat->Ambient[2];
   baseA = mat->Diffuse[3];  /* Alpha is simple, same for all vertices */

   FLOAT_COLOR_TO_UBYTE_COLOR( sumA, baseA );

   for (j=0;j<n;j++) {
      GLfloat sumR, sumG, sumB;
      GLfloat nx, ny, nz;

      if (side==0) {
         /* shade frontside */
         nx = normal[j][0];
         ny = normal[j][1];
         nz = normal[j][2];
      }
      else {
         /* shade backside */
         nx = -normal[j][0];
         ny = -normal[j][1];
         nz = -normal[j][2];
      }

      sumR = baseR;
      sumG = baseG;
      sumB = baseB;

      /* Add contribution from each enabled light source */
      for (light=ctx->Light.FirstEnabled; light; light=light->NextEnabled) {
         GLfloat ambientR, ambientG, ambientB;
         GLfloat attenuation, spot;
         GLfloat VPx, VPy, VPz;  /* unit vector from vertex to light */
         GLfloat n_dot_VP;       /* n dot VP */

         /* compute VP and attenuation */
         if (light->Position[3]==0.0) {
            /* directional light */
            VPx = light->VP_inf_norm[0];
            VPy = light->VP_inf_norm[1];
            VPz = light->VP_inf_norm[2];
            attenuation = 1.0F;
         }
         else {
            /* positional light */
            GLfloat d;     /* distance from vertex to light */
            VPx = light->Position[0] - vertex[j][0];
            VPy = light->Position[1] - vertex[j][1];
            VPz = light->Position[2] - vertex[j][2];
            d = (GLfloat) GL_SQRT( VPx*VPx + VPy*VPy + VPz*VPz );
            if (d>0.001F) {
               GLfloat invd = 1.0F / d;
               VPx *= invd;
               VPy *= invd;
               VPz *= invd;
            }
            attenuation = 1.0F / (light->ConstantAttenuation
                        + d * (light->LinearAttenuation
                        + d * light->QuadraticAttenuation));
         }

         /* spotlight factor */
         if (light->SpotCutoff==180.0F) {
            /* not a spot light */
            spot = 1.0F;
         }
         else {
            GLfloat PVx, PVy, PVz, PV_dot_dir;
            PVx = -VPx;
            PVy = -VPy;
            PVz = -VPz;
            PV_dot_dir = PVx*light->NormDirection[0]
                       + PVy*light->NormDirection[1]
                       + PVz*light->NormDirection[2];
            if (PV_dot_dir<=0.0F || PV_dot_dir<light->CosCutoff) {
               /* outside of cone */
               spot = 0.0F;
            }
            else {
               double x = PV_dot_dir * (EXP_TABLE_SIZE-1);
               int k = (int) x;
               spot = light->SpotExpTable[k][0]
                    + (x-k)*light->SpotExpTable[k][1];
            }
         }

         ambientR = mat->Ambient[0] * light->Ambient[0];
         ambientG = mat->Ambient[1] * light->Ambient[1];
         ambientB = mat->Ambient[2] * light->Ambient[2];

         /* Compute dot product or normal and vector from V to light pos */
         n_dot_VP = nx * VPx + ny * VPy + nz * VPz;

         /* diffuse and specular terms */
         if (n_dot_VP<=0.0F) {
            /* surface face away from light, no diffuse or specular */
            GLfloat t = attenuation * spot;
            sumR += t * ambientR;
            sumG += t * ambientG;
            sumB += t * ambientB;
            /* done with this light */
         }
         else {
            GLfloat diffuseR, diffuseG, diffuseB;
            GLfloat specularR, specularG, specularB;
            GLfloat h_x, h_y, h_z, n_dot_h, t;
                  
            /* diffuse term */
            diffuseR = n_dot_VP * mat->Diffuse[0] * light->Diffuse[0];
            diffuseG = n_dot_VP * mat->Diffuse[1] * light->Diffuse[1];
            diffuseB = n_dot_VP * mat->Diffuse[2] * light->Diffuse[2];

            /* specular term */
            if (ctx->Light.Model.LocalViewer) {
               GLfloat vx, vy, vz, vlen;
               vx = vertex[j][0];
               vy = vertex[j][1];
               vz = vertex[j][2];
               vlen = GL_SQRT( vx*vx + vy*vy + vz*vz );
               if (vlen>0.0001F) {
                  GLfloat invlen = 1.0F / vlen;
                  vx *= invlen;
                  vy *= invlen;
                  vz *= invlen;
               }
               /* h = VP + VPe */
               h_x = VPx - vx;
               h_y = VPy - vy;
               h_z = VPz - vz;
            }
            else {
               /* h = VP + <0,0,1> */
               h_x = VPx;
               h_y = VPy;
               h_z = VPz + 1.0F;
            }

            /* attention: h is not normalized, done later if needed */
            n_dot_h = nx*h_x + ny*h_y + nz*h_z;

            if (n_dot_h<=0.0F) {
               specularR = 0.0F;
               specularG = 0.0F;
               specularB = 0.0F;
            }
            else {
               GLfloat spec_coef;
               /* now `correct' the dot product */
               n_dot_h = n_dot_h / GL_SQRT( h_x*h_x + h_y*h_y + h_z*h_z );
               if (n_dot_h>1.0F) {
                  /* only happens if normal vector length > 1.0 */
                  spec_coef = pow( n_dot_h, mat->Shininess );
               }
               else {
                  /* use table lookup approximation */
                  int k = (int) (n_dot_h * (GLfloat) (SHINE_TABLE_SIZE-1));
                  if (mat->ShineTable[k] < 0.0F)
                     mat->ShineTable[k] = gl_pow( n_dot_h, mat->Shininess );
                  spec_coef = mat->ShineTable[k];
               }
               if (spec_coef<1.0e-10) {
                  specularR = 0.0F;
                  specularG = 0.0F;
                  specularB = 0.0F;
               }
               else {
                  specularR = spec_coef * mat->Specular[0]*light->Specular[0];
                  specularG = spec_coef * mat->Specular[1]*light->Specular[1];
                  specularB = spec_coef * mat->Specular[2]*light->Specular[2];
               }
            }

            t = attenuation * spot;
            sumR += t * (ambientR + diffuseR + specularR);
            sumG += t * (ambientG + diffuseG + specularG);
            sumB += t * (ambientB + diffuseB + specularB);
         }

      } /*loop over lights*/

      /* clamp and convert to integer or fixed point */
      FLOAT_COLOR_TO_UBYTE_COLOR( color[j][0], sumR );
      FLOAT_COLOR_TO_UBYTE_COLOR( color[j][1], sumG );
      FLOAT_COLOR_TO_UBYTE_COLOR( color[j][2], sumB );
      color[j][3] = sumA;

   } /*loop over vertices*/
}



/*
 * Compute separate base and specular colors.
 * Input:  side - 0=use front material, 1=use back material
 *         n - number of vertexes to process
 *         vertex - array of vertex positions in eye coordinates
 *         normal - array of surface normal vectors
 * Output:  baseColor - array of base colors (emission, ambient, diffuse)
 *          specColor - array of specular colors
 */
void gl_shade_rgba_spec( GLcontext *ctx,
                         GLuint side,
                         GLuint n,
                         /*const*/ GLfloat vertex[][4],
                         /*const*/ GLfloat normal[][3],
                         GLubyte baseColor[][4], GLubyte specColor[][4] )
{
   GLuint j;
   GLfloat baseR, baseG, baseB, baseA;
   GLint sumBaseA;
   struct gl_light *light;
   struct gl_material *mat;

   mat = &ctx->Light.Material[side];

   /*** Compute color contribution from global lighting ***/
   baseR = mat->Emission[0] + ctx->Light.Model.Ambient[0] * mat->Ambient[0];
   baseG = mat->Emission[1] + ctx->Light.Model.Ambient[1] * mat->Ambient[1];
   baseB = mat->Emission[2] + ctx->Light.Model.Ambient[2] * mat->Ambient[2];
   baseA = mat->Diffuse[3];  /* Alpha is simple, same for all vertices */

   FLOAT_COLOR_TO_UBYTE_COLOR( sumBaseA, baseA );

   for (j=0;j<n;j++) {
      GLfloat sumBaseR, sumBaseG, sumBaseB;
      GLfloat sumSpecR, sumSpecG, sumSpecB;
      GLfloat nx, ny, nz;

      if (side==0) {
         /* shade frontside */
         nx = normal[j][0];
         ny = normal[j][1];
         nz = normal[j][2];
      }
      else {
         /* shade backside */
         nx = -normal[j][0];
         ny = -normal[j][1];
         nz = -normal[j][2];
      }

      sumBaseR = baseR;
      sumBaseG = baseG;
      sumBaseB = baseB;
      sumSpecR = sumSpecG = sumSpecB = 0.0;

      /* Add contribution from each enabled light source */
      for (light=ctx->Light.FirstEnabled; light; light=light->NextEnabled) {
         GLfloat ambientR, ambientG, ambientB;
         GLfloat attenuation, spot;
         GLfloat VPx, VPy, VPz;  /* unit vector from vertex to light */
         GLfloat n_dot_VP;       /* n dot VP */

         /* compute VP and attenuation */
         if (light->Position[3]==0.0) {
            /* directional light */
            VPx = light->VP_inf_norm[0];
            VPy = light->VP_inf_norm[1];
            VPz = light->VP_inf_norm[2];
            attenuation = 1.0F;
         }
         else {
            /* positional light */
            GLfloat d;     /* distance from vertex to light */
            VPx = light->Position[0] - vertex[j][0];
            VPy = light->Position[1] - vertex[j][1];
            VPz = light->Position[2] - vertex[j][2];
            d = (GLfloat) GL_SQRT( VPx*VPx + VPy*VPy + VPz*VPz );
            if (d>0.001F) {
               GLfloat invd = 1.0F / d;
               VPx *= invd;
               VPy *= invd;
               VPz *= invd;
            }
            attenuation = 1.0F / (light->ConstantAttenuation
                        + d * (light->LinearAttenuation
                        + d * light->QuadraticAttenuation));
         }

         /* spotlight factor */
         if (light->SpotCutoff==180.0F) {
            /* not a spot light */
            spot = 1.0F;
         }
         else {
            GLfloat PVx, PVy, PVz, PV_dot_dir;
            PVx = -VPx;
            PVy = -VPy;
            PVz = -VPz;
            PV_dot_dir = PVx*light->NormDirection[0]
                       + PVy*light->NormDirection[1]
                       + PVz*light->NormDirection[2];
            if (PV_dot_dir<=0.0F || PV_dot_dir<light->CosCutoff) {
               /* outside of cone */
               spot = 0.0F;
            }
            else {
               double x = PV_dot_dir * (EXP_TABLE_SIZE-1);
               int k = (int) x;
               spot = light->SpotExpTable[k][0]
                    + (x-k)*light->SpotExpTable[k][1];
            }
         }

         ambientR = mat->Ambient[0] * light->Ambient[0];
         ambientG = mat->Ambient[1] * light->Ambient[1];
         ambientB = mat->Ambient[2] * light->Ambient[2];

         /* Compute dot product or normal and vector from V to light pos */
         n_dot_VP = nx * VPx + ny * VPy + nz * VPz;

         /* diffuse and specular terms */
         if (n_dot_VP<=0.0F) {
            /* surface face away from light, no diffuse or specular */
            GLfloat t = attenuation * spot;
            sumBaseR += t * ambientR;
            sumBaseG += t * ambientG;
            sumBaseB += t * ambientB;
            /* done with this light */
         }
         else {
            GLfloat diffuseR, diffuseG, diffuseB;
            GLfloat specularR, specularG, specularB;
            GLfloat h_x, h_y, h_z, n_dot_h, t;
                  
            /* diffuse term */
            diffuseR = n_dot_VP * mat->Diffuse[0] * light->Diffuse[0];
            diffuseG = n_dot_VP * mat->Diffuse[1] * light->Diffuse[1];
            diffuseB = n_dot_VP * mat->Diffuse[2] * light->Diffuse[2];

            /* specular term */
            if (ctx->Light.Model.LocalViewer) {
               GLfloat vx, vy, vz, vlen;
               vx = vertex[j][0];
               vy = vertex[j][1];
               vz = vertex[j][2];
               vlen = GL_SQRT( vx*vx + vy*vy + vz*vz );
               if (vlen>0.0001F) {
                  GLfloat invlen = 1.0F / vlen;
                  vx *= invlen;
                  vy *= invlen;
                  vz *= invlen;
               }
               /* h = VP + VPe */
               h_x = VPx - vx;
               h_y = VPy - vy;
               h_z = VPz - vz;
            }
            else {
               /* h = VP + <0,0,1> */
               h_x = VPx;
               h_y = VPy;
               h_z = VPz + 1.0F;
            }

            /* attention: h is not normalized, done later if needed */
            n_dot_h = nx*h_x + ny*h_y + nz*h_z;

            if (n_dot_h<=0.0F) {
               specularR = 0.0F;
               specularG = 0.0F;
               specularB = 0.0F;
            }
            else {
               GLfloat spec_coef;
               /* now `correct' the dot product */
               n_dot_h = n_dot_h / GL_SQRT( h_x*h_x + h_y*h_y + h_z*h_z );
               if (n_dot_h>1.0F) {
                  /* only happens if normal vector length > 1.0 */
                  spec_coef = pow( n_dot_h, mat->Shininess );
               }
               else {
                  /* use table lookup approximation */
                  int k = (int) (n_dot_h * (GLfloat) (SHINE_TABLE_SIZE-1));
                  if (mat->ShineTable[k] < 0.0F)
                     mat->ShineTable[k] = gl_pow( n_dot_h, mat->Shininess );
                  spec_coef = mat->ShineTable[k];
               }
               if (spec_coef<1.0e-10) {
                  specularR = 0.0F;
                  specularG = 0.0F;
                  specularB = 0.0F;
               }
               else {
                  specularR = spec_coef * mat->Specular[0]*light->Specular[0];
                  specularG = spec_coef * mat->Specular[1]*light->Specular[1];
                  specularB = spec_coef * mat->Specular[2]*light->Specular[2];
               }
            }

            t = attenuation * spot;
            sumBaseR += t * (ambientR + diffuseR);
            sumBaseG += t * (ambientG + diffuseG);
            sumBaseB += t * (ambientB + diffuseB);
            sumSpecR += t * specularR;
            sumSpecG += t * specularG;
            sumSpecB += t * specularB;
         }

      } /*loop over lights*/

      /* clamp and convert to integer or fixed point */
      FLOAT_COLOR_TO_UBYTE_COLOR( baseColor[j][0], sumBaseR );
      FLOAT_COLOR_TO_UBYTE_COLOR( baseColor[j][1], sumBaseG );
      FLOAT_COLOR_TO_UBYTE_COLOR( baseColor[j][2], sumBaseB );
      baseColor[j][3] = sumBaseA;

      FLOAT_COLOR_TO_UBYTE_COLOR( specColor[j][0], sumSpecR );
      FLOAT_COLOR_TO_UBYTE_COLOR( specColor[j][1], sumSpecG );
      FLOAT_COLOR_TO_UBYTE_COLOR( specColor[j][2], sumSpecB );
      specColor[j][3] = 255;  /* but never used */

   } /*loop over vertices*/
}



/*
 * This is an optimized version of the above function.
 */
void gl_shade_rgba_fast( GLcontext *ctx,
                                   GLuint side,
                                   GLuint n,
                                   /*const*/ GLfloat normal[][3],
                                   GLubyte color[][4] )
{
   GLuint j;
   GLint sumA;
   GLfloat *baseColor = ctx->Light.BaseColor[side];

   /* Alpha is easy to compute, same for all vertices */
   sumA = (GLint) (baseColor[3] * 255.0F);

   /* Loop over vertices */
   for (j=0;j<n;j++) {
      GLfloat sumR, sumG, sumB;
      GLfloat nx, ny, nz;
      struct gl_light *light;

      /* the normal vector */
      if (side==0) {
         nx = normal[j][0];
         ny = normal[j][1];
         nz = normal[j][2];
      }
      else {
         nx = -normal[j][0];
         ny = -normal[j][1];
         nz = -normal[j][2];
      }

#ifdef SPEED_HACK
      if (nz<0.0F) {
         color[j][0] = 0.0F;
         color[j][1] = 0.0F;
         color[j][2] = 0.0F;
         color[j][3] = A;
         continue;
      }
#endif

      /* base color from global illumination and enabled light's ambient */
      sumR = baseColor[0];
      sumG = baseColor[1];
      sumB = baseColor[2];

      /* Add contribution from each light source */
      for (light=ctx->Light.FirstEnabled; light; light=light->NextEnabled) {
         GLfloat n_dot_VP;     /* n dot VP */

         n_dot_VP = nx * light->VP_inf_norm[0]
                  + ny * light->VP_inf_norm[1]
                  + nz * light->VP_inf_norm[2];

         /* diffuse and specular terms */
         if (n_dot_VP>0.0F) {
            GLfloat n_dot_h;
            GLfloat *lightMatDiffuse = light->MatDiffuse[side];

            /** add diffuse term **/
            sumR += n_dot_VP * lightMatDiffuse[0];
            sumG += n_dot_VP * lightMatDiffuse[1];
            sumB += n_dot_VP * lightMatDiffuse[2];

            /** specular term **/
            /* dot product of n and h_inf_norm */
            n_dot_h = nx * light->h_inf_norm[0]
                    + ny * light->h_inf_norm[1]
                    + nz * light->h_inf_norm[2];
            if (n_dot_h>0.0F) {
               if (n_dot_h>1.0F) {
                  /* only happens if Magnitude(n) > 1.0 */
                  GLfloat spec_coef = pow( n_dot_h,
                                        ctx->Light.Material[side].Shininess );
                  if (spec_coef>1.0e-10F) {
                     sumR += spec_coef * light->MatSpecular[side][0];
                     sumG += spec_coef * light->MatSpecular[side][1];
                     sumB += spec_coef * light->MatSpecular[side][2];
                  }
               }
               else {
                  /* use table lookup approximation */
                  int k = (int) (n_dot_h * (GLfloat) (SHINE_TABLE_SIZE-1));
                  struct gl_material *m = &ctx->Light.Material[side];
                  GLfloat spec_coef;
                  if (m->ShineTable[k] < 0.0F)
                     m->ShineTable[k] = gl_pow( n_dot_h, m->Shininess );
                  spec_coef = m->ShineTable[k];
                  sumR += spec_coef * light->MatSpecular[side][0];
                  sumG += spec_coef * light->MatSpecular[side][1];
                  sumB += spec_coef * light->MatSpecular[side][2];
               }
            }
         }

      } /*loop over lights*/

      /* clamp and convert to integer or fixed point */
      FLOAT_COLOR_TO_UBYTE_COLOR( color[j][0], sumR );
      FLOAT_COLOR_TO_UBYTE_COLOR( color[j][1], sumG );
      FLOAT_COLOR_TO_UBYTE_COLOR( color[j][2], sumB );
      color[j][3] = sumA;

      /* Ugh, I think there's a bug in gcc 2.7.2.3.  If the following
       * no-op code isn't here then the results of the above
       * FLOAT_COLOR_TO_UBYTE_COLOR() macro are unpredictable!
       */
      {
         GLubyte r0 = FloatToInt(CLAMP(sumR, 0, 1) * 255.0);
      }

   } /*loop over vertices*/
}



/*
 * Use current lighting/material settings to compute the color indexes
 * for an array of vertices.
 * Input:  n - number of vertices to shade
 *         side - 0=use front material, 1=use back material
 *         vertex - array of [n] vertex position in eye coordinates
 *         normal - array of [n] surface normal vector
 * Output:  indexResult - resulting array of [n] color indexes
 */
void gl_shade_ci( GLcontext *ctx,
                              GLuint side,
                              GLuint n,
                              GLfloat vertex[][4],
                              GLfloat normal[][3],
                              GLuint indexResult[] )
{
   struct gl_material *mat = &ctx->Light.Material[side];
   GLuint j;

   /* loop over vertices */
   for (j=0;j<n;j++) {
      GLfloat index;
      GLfloat diffuse, specular;  /* accumulated diffuse and specular */
      GLfloat nx, ny, nz;  /* normal vector */
      struct gl_light *light;

      if (side==0) {
         /* shade frontside */
         nx = normal[j][0];
         ny = normal[j][1];
         nz = normal[j][2];
      }
      else {
         /* shade backside */
         nx = -normal[j][0];
         ny = -normal[j][1];
         nz = -normal[j][2];
      }

      diffuse = specular = 0.0F;

      /* Accumulate diffuse and specular from each light source */
      for (light=ctx->Light.FirstEnabled; light; light=light->NextEnabled) {
         GLfloat attenuation;
         GLfloat lx, ly, lz;  /* unit vector from vertex to light */
         GLfloat l_dot_norm;  /* dot product of l and n */

         /* compute l and attenuation */
         if (light->Position[3]==0.0) {
            /* directional light */
            /* Effectively, l is a vector from the origin to the light. */
            lx = light->VP_inf_norm[0];
            ly = light->VP_inf_norm[1];
            lz = light->VP_inf_norm[2];
            attenuation = 1.0F;
         }
         else {
            /* positional light */
            GLfloat d;     /* distance from vertex to light */
            lx = light->Position[0] - vertex[j][0];
            ly = light->Position[1] - vertex[j][1];
            lz = light->Position[2] - vertex[j][2];
            d = (GLfloat) GL_SQRT( lx*lx + ly*ly + lz*lz );
            if (d>0.001F) {
               GLfloat invd = 1.0F / d;
               lx *= invd;
               ly *= invd;
               lz *= invd;
            }
            attenuation = 1.0F / (light->ConstantAttenuation
                        + d * (light->LinearAttenuation
                        + d * light->QuadraticAttenuation));
         }

         l_dot_norm = lx*nx + ly*ny + lz*nz;

         if (l_dot_norm>0.0F) {
            GLfloat spot_times_atten;

            /* spotlight factor */
            if (light->SpotCutoff==180.0F) {
               /* not a spot light */
               spot_times_atten = attenuation;
            }
            else {
               GLfloat v[3], dot;
               v[0] = -lx;  /* v points from light to vertex */
               v[1] = -ly;
               v[2] = -lz;
               dot = DOT3( v, light->NormDirection );
               if (dot<=0.0F || dot<light->CosCutoff) {
                  /* outside of cone */
                  spot_times_atten = 0.0F;
               }
               else {
                  double x = dot * (EXP_TABLE_SIZE-1);
                  int k = (int) x;
                  GLfloat spot = light->SpotExpTable[k][0]
                               + (x-k)*light->SpotExpTable[k][1];
                  spot_times_atten = spot * attenuation;
               }
            }

            /* accumulate diffuse term */
            diffuse += l_dot_norm * light->dli * spot_times_atten;

            /* accumulate specular term */
            {
               GLfloat h_x, h_y, h_z, n_dot_h, spec_coef;

               /* specular term */
               if (ctx->Light.Model.LocalViewer) {
                  GLfloat vx, vy, vz, vlen;
                  vx = vertex[j][0];
                  vy = vertex[j][1];
                  vz = vertex[j][2];
                  vlen = GL_SQRT( vx*vx + vy*vy + vz*vz );
                  if (vlen>0.0001F) {
                     GLfloat invlen = 1.0F / vlen;
                     vx *= invlen;
                     vy *= invlen;
                     vz *= invlen;
                  }
                  h_x = lx - vx;
                  h_y = ly - vy;
                  h_z = lz - vz;
               }
               else {
                  h_x = lx;
                  h_y = ly;
                  h_z = lz + 1.0F;
               }
               /* attention: s is not normalized, done later if necessary */
               n_dot_h = h_x*nx + h_y*ny + h_z*nz;

               if (n_dot_h <= 0.0F) {
                  spec_coef = 0.0F;
               }
               else {
                  /* now `correct' the dot product */
                  n_dot_h = n_dot_h / GL_SQRT(h_x*h_x + h_y*h_y + h_z*h_z);
                  if (n_dot_h>1.0F) {
                     spec_coef = pow( n_dot_h, mat->Shininess );
                  }
                  else {
                     int k = (int) (n_dot_h * (GLfloat)(SHINE_TABLE_SIZE-1));
                     if (mat->ShineTable[k] < 0.0F)
                        mat->ShineTable[k] = gl_pow( n_dot_h, mat->Shininess );
                     spec_coef = mat->ShineTable[k];
                  }
               }
               specular += spec_coef * light->sli * spot_times_atten;
            }
         }

      } /*loop over lights*/

      /* Now compute final color index */
      if (specular>1.0F) {
         index = mat->SpecularIndex;
      }
      else {
         GLfloat d_a, s_a;
         d_a = mat->DiffuseIndex - mat->AmbientIndex;
         s_a = mat->SpecularIndex - mat->AmbientIndex;

         index = mat->AmbientIndex
               + diffuse * (1.0F-specular) * d_a
               + specular * s_a;
         if (index>mat->SpecularIndex) {
            index = mat->SpecularIndex;
         }
      }
      indexResult[j] = (GLuint) (GLint) index;

   } /*for vertex*/
}

