/************************************************************************
 *                                                                      *
 *                    Copyright (c) 1987, David B. Wecker               *
 *                            All Rights Reserved                       *
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
 *        DBW - David B. Wecker                                         *
 *                                                                      *
 * Versions:                                                            *
 *        V1.0 870125 DBW  - First released version                     *
 *                                                                      *
 ************************************************************************/

#define MODULE_HIT
#include "ray.h"

void findnormal(np,p,n)
node    *np;
vector  p,
n;
{
     vector ripple,fuzzy,bevelx,bevely,bevelz,tmp;
     float t;
     int w,j,it;

     switch (np->kind) 
     {
     case SPHERE   :
          SPHERENORMAL(sptr(np)->center,p,n);
          break;
     case TRIANGLE :
          PLANENORMAL(tptr(np)->ve,tptr(np)->vp,n);
          break;
     case QUAD     :
          PLANENORMAL(qptr(np)->ve,qptr(np)->vp,n);
          break;
     case RING     :
          PLANENORMAL(rptr(np)->ve,rptr(np)->vp,n);
          break;
     }

     if (np->attr.tex != 0) 
     { /* don't bother checking if there's no texture */
          if (np->attr.tex == 4) 
          { /* fiddle with normal for all ripples */
               for (w = 0; w < numwaves; w++) 
               {
                    calcripple(p,w,ripple);  /* calculate the wave perturbation */
                    vecsum(ripple,n,n);  /* add ripple bend to normal */
               }
               normalize(n);  /* make sure it's a unit vector after all that */
          }

          /* fiddle with normal for one ripple */
          else if (np->attr.tex >= 10 && np->attr.tex <= 19) 
          {
               calcripple(p,np->attr.tex - 10,ripple);  /* calc wave perturbation */
               vecsum(ripple,n,n);  /* add ripple bend to normal */
               normalize(n);  /* make sure it's still a unit vector */
          }

          /* check for rough pebbly surface */
          else if (np->attr.tex >= 90 && np->attr.tex <= 99) 
          {
               j = np->attr.tex - 90;  /* select the desired pebble finish */
               vecscale(pebble[j].zoom,p,tmp);
               fuzzy[0] = turbulence(tmp);
               vecscale(1.5,tmp,tmp);
               fuzzy[1] = turbulence(tmp);
               vecscale(1.5,tmp,tmp);
               fuzzy[2] = turbulence(tmp);
               if (fuzzy[0] > 0.7)
                    fuzzy[0] = 0.7;
               if (fuzzy[1] > 0.7)
                    fuzzy[1] = 0.7;
               if (fuzzy[2] > 0.7)
                    fuzzy[2] = 0.7;
               if (rnd() < 0.5)
                    fuzzy[0] = -fuzzy[0];
               if (rnd() < 0.5)
                    fuzzy[1] = -fuzzy[1];
               if (rnd() < 0.5)
                    fuzzy[2] = -fuzzy[2];
               vecscale(pebble[j].scale,fuzzy,fuzzy);
               vecsum(n,fuzzy,n);
               normalize(n);
          }
     }

     /*---------------------------------------------------------------------*/
     /* Add any general normal perturbations                                */

     if (np->attr.fuz > 0.0) 
     {
          /* Perturb the normal randomly to produce fuzzy surfaces */
          fuzzy[0] = rnd();  /* 0..1 */
          fuzzy[1] = rnd();
          fuzzy[2] = rnd();
          if (rnd() < 0.5)
               fuzzy[0] = -fuzzy[0];
          if (rnd() < 0.5)
               fuzzy[1] = -fuzzy[1];
          if (rnd() < 0.5)
               fuzzy[2] = -fuzzy[2];

          /* 'fuzzy' is now approximately a random unit vector */
          vecscale(rnd() * np->attr.fuz,fuzzy,fuzzy);
          vecsum(fuzzy,n,n); /* vector addition of fuzz compunent to true normal */
          normalize(n);  /* Make sure it's still a unit vector */
     }
}

int hitcylinder(cp,eye,d,p,t)
cylinder *cp;  /* the cylinder      */
vector eye;    /* source ray origin */
vector d;      /* source ray        */
vector p;
float  *t;
{
     float aa,bb,cc,radical,a2,b2,c2,dist1,dist2;
     vector otop,oeye,p1,p2,opoint1,opoint2;

     /*
      *   translate bottom of cylinder to 0,0,0
      *   get translated eyepoint and cylinder top
      */

     VECSUB(eye,    cp->bottom, oeye);
     VECSUB(cp->top,cp->bottom, otop);  /* ? */


     a2 = cp->a*cp->a;
     b2 = cp->b*cp->b;
     c2 = cp->c*cp->c;

     if (a2 < SMALL) return( FALSE );
     if (b2 < SMALL) return( FALSE );
     if (c2 < SMALL) return( FALSE );


     aa  = d[0]*d[0] / a2;
     aa += d[2]*d[2] / b2;
     aa -= d[1]*d[1] / c2;

     bb  = d[0]*oeye[0] / a2;
     bb += d[2]*oeye[2] / b2;
     bb -= d[1]*oeye[1] / c2;

     cc  = oeye[0]*oeye[0] / a2;
     cc += oeye[2]*oeye[2] / b2;
     cc -= oeye[1]*oeye[1] / c2;

/*   
 *   descriminate < 0,  ray misses cylinder
 *   descriminate == 0, ray grazes cylinder
 */

     if ((radical = (bb*bb) - (4.0*aa*cc)) < 0.0)
          return( FALSE );

     radical = sqrt(radical);

     aa = 2.0 * aa;

     if ( aa < SMALL ) 
          return( FALSE );  

     /* the roots of the quadratic */

     dist1 = (-bb + radical) / aa;
     dist2 = (-bb - radical) / aa;


     VECSCALE(dist1,d,p1);
     VECSUM(p1,oeye,p1);
     VECSCALE(dist2,d,p2);
     VECSUM(p2,oeye,p2);
     VECSUB(oeye,p1,opoint1);
     VECSUB(oeye,p2,opoint2);
     dist1 = NORM(opoint1);
     dist2 = NORM(opoint2);

     if (dist1 < dist2) 
     { 
          VECCOPY(p1,p); 
          *t = dist1; 
     }
     else
     { 
          VECCOPY(p2,p); 
          *t = dist2; 
     }

     return( TRUE );
}

int hitsphere(center,radius,eye,d,p,t)
vector  center;     /* where it is  */
vector  eye;        /* where we are */
vector  d;
vector  p;
float   radius;     /* how big it is */
float   *t;
{
     float   r_r,d_r,t2,radical;
     vector  r;

     VECSUB(center,eye,r);
     r_r = DOT(r,r);
     d_r = DOT(d,r);

     if ((radical = (d_r*d_r) + (radius*radius) - r_r) < 0.0) 
          return( FALSE );

     radical = sqrt(radical);

     if (d_r < radical) 
     { 
          *t = d_r + radical; 
          t2 = d_r - radical; 
     }
     else
     { 
          *t = d_r - radical; 
          t2 = d_r + radical; 
     }

     if (fabs(*t) < SMALL) 
          *t = t2;

     if (*t <= 0) 
          return( FALSE );

     VECSCALE((*t),d,p);
     VECSUM(p,eye,p);
     return( TRUE );
}

int hitplane(p,ve,vp,eye,d,sfs,inter)
vector  p,
  ve,
  vp,
  eye,
  d,
  sfs,
  inter;
{
     vector h;
     float det;

     det  = ve[0] * ((vp[1] * d[2]) - (vp[2] * d[1]));
     det -= vp[0] * ((ve[1] * d[2]) - (ve[2] * d[1]));
     det +=  d[0] * ((ve[1] * vp[2])- (ve[2] * vp[1]));

     if (det == 0.0)
          return FALSE;

     vecsub(eye,p,h);

     sfs[2]  = h[0] * ((ve[1] * vp[2]) - (ve[2] * vp[1]));
     sfs[2] -= h[1] * ((ve[0] * vp[2]) - (ve[2] * vp[0]));
     sfs[2] += h[2] * ((ve[0] * vp[1]) - (ve[1] * vp[0]));
     sfs[2] /= det;
     sfs[2]  = -sfs[2];

     if (sfs[2] < SMALL)
          return FALSE;

     sfs[0]  = h[0] * ((vp[1] * d[2]) - (vp[2] * d[1]));
     sfs[0] -= h[1] * ((vp[0] * d[2]) - (vp[2] * d[0]));
     sfs[0] += h[2] * ((vp[0] * d[1]) - (vp[1] * d[0]));
     sfs[0] /= det;

     sfs[1]  = h[0] * ((ve[2] * d[1]) - (ve[1] * d[2]));
     sfs[1] += h[1] * ((ve[0] * d[2]) - (ve[2] * d[0]));
     sfs[1] -= h[2] * ((ve[0] * d[1]) - (ve[1] * d[0]));
     sfs[1] /= det;

     VECCOPY(d,inter);
     VECSCALE(sfs[2],inter,inter);
     VECSUM(inter,eye,inter);

     return TRUE;
}

int hittriangle(tp,eye,d,p,t)
triangle    *tp;
vector      eye,
d,
p;
float       *t;
{
     vector sfs;
     int hit;

     hit = hitplane(tp->position,tp->ve,tp->vp,eye,d,sfs,p) &&
       sfs[0] > 0.0  && sfs[1] > 0.0 &&  sfs[0] + sfs[1] <= 1.0;
     *t = sfs[2];

     return hit;
}

int hitquad(qp,eye,d,p,t)
quad        *qp;
vector      eye,
d,
p;
float       *t;
{
     vector sfs;
     int hit = FALSE;

     if (hitplane(qp->position,qp->ve,qp->vp,eye,d,sfs,p))
          hit = sfs[0] > 0.0 && sfs[1] > 0.0 && sfs[0] <= 1.0 && sfs[1] <= 1.0;
     *t = sfs[2];
     return hit;
}

int hitring(rp,eye,d,p,t)
ring        *rp;
vector      eye,
d,
p;
float       *t;
{
     vector  sfs;
     float   r;
     int     hit;

     hit = hitplane(rp->position,rp->ve,rp->vp,eye,d,sfs,p) &&
       (r = sfs[0] * sfs[0] + sfs[1] * sfs[1]) <= rp->maxrad &&
       r >= rp->minrad;
     *t = sfs[2];
     return( hit );
}

void shell(v,v1,v2,n)
float       v[];
vector      v1[];
node        *v2[];
int         n;
{
     int     gap,i,j;
     node    *temp2;
     vector  temp1;
     float   temp;

     sorts++;  /* Keep statistics */
     sort_size += n;

     for (gap = n/2; gap > 0; gap /=2)
          for (i = gap; i < n; i++)
               for (j = i-gap; j>=0 && v[j] > v[j+gap]; j -= gap) 
               {
                    temp = v[j];
                    v[j] = v[j+gap];
                    v[j+gap] = temp;

                    VECCOPY(v1[j],temp1);
                    VECCOPY(v1[j+gap],v1[j]);
                    VECCOPY(temp1,v1[j+gap]);

                    temp2 = v2[j];
                    v2[j] = v2[j+gap];
                    v2[j+gap] = temp2;
               }
}

