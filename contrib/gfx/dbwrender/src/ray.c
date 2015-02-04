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
 *      JHL - John H. Lowery, IBM-PC/Microsoft C port                   *
 *                                                                      *
 * Versions:                                                            *
 *      V1.0 870125 DBW - First released version                        *
 *      V1.1 (IBM) 890101 JHL - version 1.0 for the IBM-PC/VGA/MCGA     *
 *                                                                      *
 ************************************************************************/

#ifdef __AROS__
#include <proto/exec.h>
#include <dos/dos.h>
#endif

#define MODULE_RAY
#include "ray.h"

vector    base,
          vx,
          vy,
          d,
          p,
          sumval,
          sampleval,
          varsum,
          samplep,
          rnd_vr,
          rnd_vu,
          subvr,
          subvu,
          old_ave,
          ave_val,
          eye2,
          focalpoint,
          lens1,
          lens2,
          temp1,
          temp2;

float     rnd_wid,
          rnd_hi,
          lensx,
          lensy,
          focallength,
          totalcount,
          count,
          maxcount,
          d_alias,
          d_bestf1,
          d_bestf2;

unsigned  modulo;
int       row,
          col,
          i,j,k,
          compthresh,
          compmin,
          bunch,
          fac1,
          fac2,
          bestf1,
          bestf2,
          spatialx,
          spatialy,
          pixelcompute,
          pixelguess,
          didguess;
long      curtime,
          prvtime,
          nxttime,
          xtrtime,
          buftime,
          totaltime,
          totalguess,
          totalcompute;

unsigned char cache[9][MAXX][3];
int           computes[MAXX];


int guess2(col,sum,val,dis,xdir,ydir)
int     col,*sum,dis,xdir,ydir;
int     val[3];
{
     int i,x1,y1,x2,y2,retval,shft,isum;

     shft    = 6 - dis;
     isum    = 1 << shft;
     y1      = 4 - (dis * ydir);
     x1      = col - (dis * xdir);
     if (y1 < 0)
          y1 = 0;
     if (y1 > 8)
          y1 = 8;
     if (x1 < 0)
          x1 = 0;
     if (x1 >= MAXCOL)
          x1 = MAXCOL-1;

     retval = 0;
     y2 = y1;
     while (1) 
     {
          if (cache[y2][x1][0] != 0xFF) 
          {
               for (i = 0; i < 3; i++)
                    val[i] += ((int)cache[y2][x1][i]) << shft;
               *sum    += isum;
               retval   = 1;
          }
          if (y2 == 4)
               break;
          y2 += ydir;
     }
     x2 = x1;
     while (1) 
     {
          if (cache[y1][x2][0] != 0xFF) 
          {
               for (i = 0; i < 3; i++)
                    val[i] += ((int)cache[y1][x2][i]) << shft;
               *sum    += isum;
               retval   = 1;
          }
          if (x2 == col)
               break;
          x2 += xdir;
     }
     return(retval);
}

void guess(col)
int col;
{
     int     i,sum,dir,dis;
     int     val[3];

     sum = 0;
     dir = 0;
     for (i = 0; i < 3; i++)
          val[i] = 0;
     for (dis = 1; dir != 15 && dis < 7; dis++) 
     {
          if ((dir & 1) != 1)
               if (guess2(col,&sum,val,dis,-1,1))
                    dir |= 1;
          if ((dir & 2) != 2)
               if (guess2(col,&sum,val,dis,1,-1))
                    dir |= 2;
          if ((dir & 4) != 4)
               if (guess2(col,&sum,val,dis,1,1))
                    dir |= 4;
          if ((dir & 8) != 8)
               if (guess2(col,&sum,val,dis,-1,-1))
                    dir |= 8;
          if (dis > 2 && ((dir & 3) == 3 || (dir & 12) == 12))
               break;
     }
     if (sum == 0)
          sum = 1;
     for (i = 0; i < 3; i++) 
     {
          cache[4][col][i] = (unsigned char)(val[i] / sum);
     }
}

void do_raytrace()
{

     /* set up pixel coordinate */
     vecscale((float)col,vr,vx);
     vecsub(base,vy,p);
     vecsum(p,vx,p);

     /* Compute this next pixel.  Distribute 'n' rays to do this */
     veczero(sumval);  /* start with a black pixel */
     veczero(varsum);
     veccopy(backgroundval,ave_val);  /* have to start someplace */
     count = 0.0;
     bunch = antialias;
     spatialx = 0;
     spatialy = 0;
     curr_runs = 0;

     while (bunch > 0) 
     {

          /* Cast another sample ray for this pixel into the next subregion */
          /* Pick a new random perturbing vector for this sample for spatial
          antialiasing */
          if (count < d_alias) 
          {
               rnd_wid = 0.5;  /* force first bunch through region centers */
               rnd_hi  = 0.5;
          }
          else
          {
               rnd_wid = rnd();
               rnd_hi  = rnd();
          }

          vecscale(rnd_wid / d_bestf2,vr,rnd_vr);
          vecscale(rnd_hi  / d_bestf1,vu,rnd_vu);
          vecscale((float) spatialx / d_bestf2,vr,subvr);
          vecscale((float) spatialy / d_bestf1,vu,subvu);
          vecsum(rnd_vr,subvr,samplep);
          vecsum(samplep,p,samplep);
          vecsub(samplep,rnd_vu,samplep);
          vecsub(samplep,subvu,samplep);
          veccopy(eye,eye2);
          direction(eye2,samplep,d);  /* direction from lens center to pixel */

          if (aperture > 0.0) 
          {
               vecscale(focus,d,focalpoint);  /* relative point on focus plane */
               lensx = rnd();  /* pick random point on lens */
               lensy = rnd();
               if (rnd() < 0.5)
                    lensx = -lensx;
               if (rnd() < 0.5)
                    lensy = -lensy;
               vecscale(lensx,lens1,temp1);  /* make into vectors in lens plane */
               vecscale(lensy,lens2,temp2);
               vecsum(temp1,temp2,eye2);  /* point in the lens */
               direction(eye2,focalpoint,d);  /* from spot on lens to focal point */
               vecsum(eye,eye2,eye2);  /* calc new 3D eye point in lens */
          }

          /* allow aborting if curr_runs > max_runs */
          if (setjmp(env)) 
          {
               computes[col] = max_runs;    /* too many computes */
               cache[8][col][1] = 0;   /* make sure we don't try again */
               return;
          }

          /* perform the ray trace */
          dodirection(sampleval,eye2,d,1.0,ambientlight);

          count      += 1.0;  /* increment the ray count */
          totalcount += 1.0;
          bunch--;  /* Count down in this bunch */
          spatialx++;  /* move across to next subregion */
          if (spatialx == bestf2) 
          {
               spatialx = 0;  /* move back & down to start of next subregion scan */
               spatialy++;
               if (spatialy == bestf1)
                    spatialy = 0;  /* all done,start at beginning again */
          }

          vecsum(sampleval,sumval,sumval);    /* accumulate the samples */
          vecscale(1.0/count,sumval,ave_val); /* compute new average */
          vecsub(sampleval,ave_val,sampleval);/* compute nth variance */
          vecmul(sampleval,sampleval,sampleval); /* square it */
          vecsum(sampleval,varsum,varsum);    /* accumulate variances */

          if (bunch == 0) 
          {
               /* When bunch=0,perform the sufficiency test */
               if ((varsum[0] / count) > variance ||
                 (varsum[1] / count) > variance ||
                 (varsum[2] / count) > variance)
                    if (count < 128)  /* maximum of 127+bunch samples */
                         bunch = antialias;  /* do another bunch */
          }
     }  /* while distributing */

     if (count > maxcount)
          maxcount = count;  /* new champion pixel */

     if (histogram) 
     {
          cv(count/d_alias,count/d_alias,count/d_alias,ave_val);
     }
     else
     {
          vecscale(d_maxgray,ave_val,ave_val);
     }

     /* store the pixel away */
     for (i = 0; i < 3; i++) 
     {
          if (ave_val[i] >= d_maxgray)
               ave_val[i] = d_maxgray-1.0;
          cache[8][col][i] = (char) ave_val[i];
     }

     /* save full statistics */
     pixelcompute++;
     total_runs += curr_runs;
     computes[col] = curr_runs;
}

int main(argc,argv)
int     argc;
char    *argv;
{
     stacktop = curstack();
     stackbot = stacktop;

     setvbuf(stdout,NULL,_IONBF,0);
     printf(VERSION);

     getinput(argc,argv);
     getoutput(argc,argv);

     fprintf(fpout, VERSION);

     brickmortarwidth  = brickmortar / brickwidth;
     brickmortarheight = brickmortar / brickheight;
     brickmortardepth  = brickmortar / brickdepth;
     d_maxgray = (float) MAXGRAY;
     sqrt3 = (float)sqrt(3.0);
     sqrt3times2 = 2.0 * sqrt3;
     d_alias = (float) antialias;

     /* for simulating depth-of-field,we distribute rays scattered from an
     imaginary lens disk,out through a focal point,and into the scene.
     In order to scatter the rays over the lens,we need two vectors
     that are orthogonal to themselves and the central direction of view.
     In essence,these two vectors define the film plane.  Just such two
     vectors were calculated in FIL.C for the EYE point.  */

     veccopy(vu,lens1);
     veccopy(vr,lens2);
     vecscale(aperture,lens1,lens1);  /* Scale them accordingly */
     vecscale(aperture,lens2,lens2);


     vecscale((float) (MAXCOL / DISPWID) / (MAXROW / DISPHI),vu,vu);
     vecscale((float) (MAXROW / 2),vu,vy);
     vecscale((float) (-MAXCOL /2),vr,vx);
     vecsum(vrp,vy,base);
     vecsum(base,vx,base);

     bestf1 = 1;  /* Find the best integral factors of antialias */
     bestf2 = antialias;
     for (fac1 = 2; fac1 <= antialias / 2; fac1++) 
     {
          fac2 = antialias / fac1;
          if (fac1 * fac2 == antialias)
               if (abs(fac1 - fac2) < abs(bestf1 - bestf2)) 
               {
                    bestf1 = fac1;
                    bestf2 = fac2;
               }
     }
     d_bestf1        = (float) bestf1;
     d_bestf2        = (float) bestf2;
     totalcount      = 0.0;
     maxcount        = 0.0;
     sline           = startrow;
     buftime         = (long)(maxhours * 3600.0 / 408.0);
     totaltime       = 0L;
     curtime         = time(0L);
     xtrtime         = 0L;
     totalguess      = 0L;
     totalcompute    = 0L;
     pixelcompute    = MAXCOL;

     printf("\nApproximately %ld seconds per scan line\n",buftime);
     fprintf(fpout,"\nApproximately %ld seconds per scan line\n",buftime);

     /* start out with no guesses as to computing difficulty */
     for (i = 0; i < MAXCOL; i++)
          computes[i] = 0;

     /* define the cache scan lines as "unknown" */
     for (i = 0; i < 9; i++)
          for (j = 0; j < MAXCOL; j++)
               cache[i][j][0] = cache[i][j][1] = 0xFF;

     /* do each scan line */
     for (row = startrow-4; row < endrow+4; row++) 
     {

          /* set up stats for this line */
          compthresh      = (pixelcompute * 2) / 3;
          compmin         = (pixelcompute * 9) / 10;
          pixelguess      = 0;
          pixelcompute    = 0;

          /* Decide how much time we have */
          prvtime    = curtime;
          nxttime    = prvtime + buftime + (xtrtime >> 3);
          xtrtime   -= xtrtime >> 3;

          if ((row+4-startrow) % 60 == 0)
               printf("\nRow %3d: ",row);
          printf(".");
          fflush(stdout);

          /* premultiply out the desired row (for do_raytrace) */
          vecscale((float)row,vu,vy);

          /* get random pixels until out of time */
          curtime  = time(0L);
          max_runs = 10;
          while (curtime <= nxttime || pixelcompute <= compmin) 
          {

               /* find remaining pixel with smallest estimated computes */
               i = j = (int)(rnd() * (float)MAXCOL);
               k = 32767;
               col = -1;
               do 
               {
                    /* see if we have a possible best new pixel on the line*/
                    if (cache[8][i][0] == 0xFF) 
                    {
                         if (pixelcompute > compthresh) 
                         {
                              col      = i;
                              max_runs = 32767;
                              break;
                         }
                         if (cache[8][i][1] == 0xFF) 
                         {
                              if (col == -1 || k > computes[i]) 
                              {
                                   col = i;
                                   k   = computes[i];
                              }
                         }
                    }
                    if (++i >= MAXCOL)
                         i = 0;
               }
               while (i != j)
                    ;

               /* Couldn't find a pixel.... */
               if (col == -1) 
               {

                    /* See if there are any that need more computing */
                    j = 1;
                    for (i = 0; i < MAXCOL; i++)
                         if (cache[8][i][0] == 0xFF) 
                         {
                              cache[8][i][1] = 0xFF;
                              j = 0;
                         }
                    if (j)
                         goto DONE_COMPUTE;

                    /* see if we can TRY more computing */
                    if (max_runs == 10)
                         max_runs = 32767;
                    else
                         goto DONE_COMPUTE;
               }
               else 
               {
                    #if defined(__AROS__)
                    if(SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                    {
                         printf("\nCTRL C Received");
                         fprintf(fpout,"\nCTRL C Received");
                         goto OPERATOR_ABORT;
                    }
                    #else
                    if (kbhit())
                         if (getch() == 0x1B)
                         {
                              printf("\nESCAPE Received");
                              fprintf(fpout,"\nESCAPE Received");
                              goto OPERATOR_ABORT;
                         }
                    #endif
                    do_raytrace();
               }
               curtime = time(0L);
          }

          /* all done computing */
DONE_COMPUTE:
          totalcompute += (long)pixelcompute;

          /* ignore until we get 4 lines into the cache */
          if (row >= startrow) 
          {

               modulo = 0;
               for (col = 0; col < MAXCOL; col++) 
               {
                    if (cache[4][col][0] == 0xFF) 
                    {
                         ++pixelguess;
                         guess(col);
                         didguess = 1;
                    }
                    else
                         didguess = 0;
                    i = col / PPW;
                    if (modulo) 
                    {
                         blueline[i]  |= ((int)cache[4][col][0]) << modulo;
                         greenline[i] |= ((int)cache[4][col][1]) << modulo;
                         redline[i]   |= ((int)cache[4][col][2]) << modulo;
                    }
                    else
                    {
                         blueline[i]  = (int) cache[4][col][0];
                         greenline[i] = (int) cache[4][col][1];
                         redline[i]   = (int) cache[4][col][2];
                    }
                    modulo = (((modulo >> 3) + 1) % PPW) << 3; /* (((modulo >> 2) + 1) % PPW) << 2 */
                    if (didguess)
                         cache[4][col][0] = 0xFF;
               }

               /* dump the scan line to the file */
               write_scanline();
          }

          curtime     = time(0L);
          xtrtime    += nxttime - curtime;
          nxttime     = curtime - prvtime;
          totaltime  += nxttime;
          totalguess += (long)pixelguess;

          /* move the cache entries down */
          for (i = 1; i < 9; i++)
               for (j = 0; j < MAXCOL; j++)
                    if (cache[i][j][0] == 0xFF)
                         cache[i-1][j][0] = cache[i-1][j][1] = 0xFF;
                    else
                         for (k = 0; k < 3; k++)
                              cache[i-1][j][k] = cache[i][j][k];

          /* define the last cache scan line as "unknown" */
          for (j = 0; j < MAXCOL; j++)
               cache[8][j][0] = cache[8][j][1] = 0xFF;
     } 

OPERATOR_ABORT:

     printf("\n");
     fprintf(fpout,"\n");
     if (fclose(fp))
          ERROR( "Error closing output file." );

     printf("First Line                 = %d\n", startrow);
     printf("Last Line                  = %d\n", sline);
     printf("Average distribution count = %4.2lf, max = %1.0lf\n",
       ((float)totalcount)/((float)(endrow-startrow)*(float)MAXCOL),
       maxcount);
     printf("Total pixels computed      = %ld\n",totalcompute);
     printf("Total pixels guessed       = %ld\n",totalguess);
     printf("Total time used            = %ld seconds\n",totaltime);
     printf("Ray intersect runs         = %ld\n",total_runs);
     printf("Max intersects for 1 ray   = %ld\n",max_intersects);
     printf("Avg intersects per pixel   = %4.2lf\n",
       ((float)total_runs)/((float)(endrow-startrow)*(float)MAXCOL));
     printf("Total sorts                = %ld\n",sorts);
     if (sorts > 0)
          printf("Average sort size          = %4.2lf\n",
            ((float)sort_size) / (float)sorts);
     stacktop -= stackbot;
     stacktop += 2000L;
     printf("Maximum stack size         = %ld\n",stacktop);

     /* statistics file */
     fprintf(fpout,"First Line                 = %d\n", startrow);
     fprintf(fpout,"Last Line                  = %d\n", sline);
     fprintf(fpout,"Average distribution count = %4.2lf, max = %1.0lf\n",
       ((float)totalcount)/((float)(endrow-startrow)*(float)MAXCOL),
       maxcount);
     fprintf(fpout,"Total pixels computed      = %ld\n",totalcompute);
     fprintf(fpout,"Total pixels guessed       = %ld\n",totalguess);
     fprintf(fpout,"Total time used            = %ld seconds\n",totaltime);
     fprintf(fpout,"Ray intersect runs         = %ld\n",total_runs);
     fprintf(fpout,"Max intersects for 1 ray   = %ld\n",max_intersects);
     fprintf(fpout,"Avg intersects per pixel   = %4.2lf\n",
       ((float)total_runs)/((float)(endrow-startrow)*(float)MAXCOL));
     fprintf(fpout,"Total sorts                = %ld\n",sorts);
     if (sorts > 0)
          fprintf(fpout,"Average sort size          = %4.2lf\n",
            ((float)sort_size) / (float)sorts);
     fprintf(fpout,"Maximum stack size         = %ld\n",stacktop);

     return 0;
}

