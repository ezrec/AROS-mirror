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
 *      jhl - John H. Lowery                                            *
 *                                                                      *
 * Versions:                                                            *
 *      V1.0  870125 DBW - First released version                       *
 *      V1.01 880916 jhl - IBM PC (VGA/MCGA) conversion                 *
 *            890121 jhl - MAXROW & MAXCOL become variables, add        *
 *                         'D' command for Display max <hor> <vert>     *
 *      V1.02 891031 jhl - add MAXCOL and MAXROW as first two integers  *
 *                         in output file, so multiple formats can be   *
 *                         supported.                                   * 
 *                                                                      *
 ************************************************************************/

#define MODULE_FILEIO
#include "ray.h"

static long         *fracopp;
static triangle     *scr_t;
static extent       *scr_e;
static sphere       *scr_s;
static quad         *scr_q;
static ring         *scr_r;
static cylinder     *scr_c;
static int          whichf;

void dumpnode(n)
node    *n;
{
     while (n) 
     {
          switch (n->kind) 
          {
          case EXTENT :
               printf("extent\n");
               vecdump(eptr(n)->center,"center");
               printf("radius = %f\n",eptr(n)->radius);
               dumpnode(((extent *) n)->sub);
               break;

          case SPHERE :
               printf("sphere\n");
               break;

          case TRIANGLE :

               printf("triangle\n");
               break;

          case QUAD :
               printf("quad\n");
               break;

          case RING :
               printf("ring\n");
               break;

          default :

               printf("UNKNOWN KIND %d\n",n->kind);
               break;
          }
          n = n->next;
     }
     printf("end\n");
}

void copyattr( oa, na )
attributes *oa, *na;
{
     na->ref = oa->ref;
     na->idx = oa->idx;
     na->fuz = oa->fuz;
     na->tex = oa->tex;
     veccopy( oa->tra, na->tra );
     veccopy( oa->amb, na->amb );
     veccopy( oa->dif, na->dif );
}

void read_vec(v)
vector  v;
{
     fscanf(df,"%f %f %f",&v[0],&v[1],&v[2]);
}

void read_attr(attr)
attributes *attr;
{
     fscanf(df,"%d %f %f %f",&attr->tex,&attr->fuz,&attr->ref,&attr->idx);
     read_vec(attr->tra);
     read_vec(attr->amb);
     read_vec(attr->dif);
     if (attr->tra[0] > 0.0 || attr->tra[1] > 0.0 || attr->tra[2] > 0.0)
          allopaque = 0;
     attr->fuz /= 10.0;  /* Assume more reasonable scale */
}

void dofractal(level,a,b,c,attr)
int         level;
vector      a,b,c;
attributes  *attr;
{
     float       aclen,bclen,ablen;
     vector      ab,ac,bc,abbump,acbump,bcbump,v1;
     long        *savedopp;

     if (level == 1) 
     {
          CHECK_ALLOC(scr_t,triangle);
          *fracopp = (long)scr_t;
          tptr(*fracopp)->next = NULL;
          tptr(*fracopp)->kind = TRIANGLE;
          copyattr(attr,&tptr(*fracopp)->attr);
          veccopy(a,tptr(*fracopp)->position);
          vecsub(c,a,ac);
          veccopy(ac,tptr(*fracopp)->ve);
          vecsub(b,a,ab);
          veccopy(ab,tptr(*fracopp)->vp);
          fracopp = (long *) &tptr(*fracopp)->next;
     }
     else
     {
          level--;
          /* compute four subfaces */

          /* length of edges */
          vecsub(a,c,v1);
          aclen = norm(v1);
          vecsub(a,b,v1);
          ablen = norm(v1);
          vecsub(b,c,v1);
          bclen = norm(v1);

          /* edge midpoints */
          vecsum(a,c,ac);
          vecscale(0.5,ac,ac);
          vecsum(a,b,ab);
          vecscale(0.5,ab,ab);
          vecsum(b,c,bc);
          vecscale(0.5,bc,bc);

          /* midpoint perturbations */
          noise3(ac,acbump);
          if (ac[1] == 0.0)
               acbump[1] = 0.0;
          noise3(ab,abbump);
          if (ab[1] == 0.0)
               abbump[1] = 0.0;
          noise3(bc,bcbump);
          if (bc[1] == 0.0)
               bcbump[1] = 0.0;
          acbump[0] *= fractal[whichf].xscale;
          acbump[1] *= fractal[whichf].yscale;
          acbump[2] *= fractal[whichf].zscale;

          bcbump[0] *= fractal[whichf].xscale;
          bcbump[1] *= fractal[whichf].yscale;
          bcbump[2] *= fractal[whichf].zscale;

          abbump[0] *= fractal[whichf].xscale;
          abbump[1] *= fractal[whichf].yscale;
          abbump[2] *= fractal[whichf].zscale;

          /* scale the perturbations proportional to side length */
          vecscale(aclen,acbump,acbump);
          vecscale(ablen,abbump,abbump);
          vecscale(bclen,bcbump,bcbump);

          /* new perturbed midpoints */
          vecsum(abbump,ab,ab);
          vecsum(acbump,ac,ac);
          vecsum(bcbump,bc,bc);

          CHECK_ALLOC(scr_e,extent);
          *fracopp = (long)scr_e;
          eptr(*fracopp)->next = NULL;
          eptr(*fracopp)->kind = EXTENT;
          eptr(*fracopp)->sub = NULL;
          savedopp = &*fracopp;
          fracopp = (long *) &eptr(*fracopp)->sub;

          dofractal(level,a, ab,ac,attr);
          dofractal(level,ac,bc,c, attr);
          dofractal(level,ab,b, bc,attr);
          dofractal(level,ac,ab,bc,attr);

          fracopp = (long *) &eptr(*savedopp)->next;
     }
}

void readimagefile(opp)
long *opp;
{
     triangle    temp;
     vector      a,b,c;

     while (EOF != fscanf(df,"%1s",str))
          switch(str[0]) 
          {
          case 'D' :
               fscanf(df,"%d %d",&MAXCOL,&MAXROW); 
               if (MAXCOL > MAXX)
                    MAXCOL = MAXX;
               if (MAXROW > MAXY)
                    MAXROW = MAXY;
               break;
          case 'R' :
               fscanf(df,"%f",&maxhours);
               break;
          case 'N' :
               fscanf(df,"%f",&idxref);
               break;
          case 'Z' :
               fscanf(df,"%d",&histogram);
               break;
          case 'a' :
               fscanf(df,"%f",&ambscale);
               break;
          case 'A' :
               fscanf(df,"%d %f",&antialias,&variance);
               break;
          case 'F' :
               fscanf(df,"%f %f",&aperture,&focus);
               break;
          case 'M' :
               fscanf(df,"%d %d %d",&ambientlight,&amblitnum,&amblitdenom);
               break;
          case '&' :
               fscanf(df,"%d %d",&startrow,&endrow); 
               break;
          case '*' :
               fgets(fname,255,df);     /* get, and display    */ 
               printf("%s",fname);
               break;
          case '!' :
               fgets(fname,255,df);     /* get, and throw away */
               break;
          case 'b' :
               read_vec(backgroundval);
               break;
          case 'w' :
               read_vec(wave[numwaves].center);
               fscanf(df,"%f",&wave[numwaves].wavelength);
               fscanf(df,"%f",&wave[numwaves].amplitude);
               fscanf(df,"%f",&wave[numwaves].drag);
               fscanf(df,"%f",&wave[numwaves].propagate);
               wave[numwaves].wavelength /= 2.0;  /* need half-wave */
               wave[numwaves].propagate  /= 2.0;  /* need half-wave */
               numwaves++;
               break;
          case 'g' :
               read_vec(blend[numblends].color);
               fscanf(df,"%f",&blend[numblends].start);
               fscanf(df,"%f",&blend[numblends].scale);
               numblends++;
               break;
          case 'n' :
               fscanf(df,"%f",&snow[numsnows].start);
               fscanf(df,"%f",&snow[numsnows].altscale);
               fscanf(df,"%f",&snow[numsnows].altfactor);
               fscanf(df,"%f",&snow[numsnows].threshhold);
               numsnows++;
               break;
          case 'p' :
               fscanf(df,"%f",&pebble[numpebbles].scale);
               fscanf(df,"%f",&pebble[numpebbles].zoom);
               numpebbles++;
               break;
          case 'k' :
               read_vec(checker[numcheckers].color);
               fscanf(df,"%f",&checker[numcheckers].x);
               fscanf(df,"%f",&checker[numcheckers].y);
               fscanf(df,"%f",&checker[numcheckers].z);
               fscanf(df,"%f",&checker[numcheckers].bevel);
               fscanf(df,"%f",&checker[numcheckers].angle);
               fscanf(df,"%d", &checker[numcheckers].beveltype);
               checker[numcheckers].bevel /= checker[numcheckers].x;
               numcheckers++;
               break;
          case 'H' :
               read_vec(haze[numhazes].color);
               fscanf(df,"%f",&haze[numhazes].distscale);
               numhazes++;
               break;
          case 'f' :
               fscanf(df,"%d", &fractal[numfractals].level);
               fscanf(df,"%f",&fractal[numfractals].xscale);
               fscanf(df,"%f",&fractal[numfractals].yscale);
               fscanf(df,"%f",&fractal[numfractals].zscale);
               fscanf(df,"%d", &fractal[numfractals].texture);
               numfractals++;
               break;
          case 'm' :
               read_vec(marble[nummarble].veincolor);
               fscanf(df,"%f",&marble[nummarble].xscale);
               fscanf(df,"%f",&marble[nummarble].turbscale);
               fscanf(df,"%d", &marble[nummarble].squeeze);
               nummarble++;
               break;
          case 'd' :
               read_vec(wood[numwoods].othercolor);
               fscanf(df,"%f",&wood[numwoods].thickscale);
               fscanf(df,"%f",&wood[numwoods].ringspacing);
               fscanf(df,"%f",&wood[numwoods].turbscale);
               fscanf(df,"%d", &wood[numwoods].squeeze);
               numwoods++;
               break;
          case 'e' :
               read_vec(eye);
               read_vec(vrp);
               read_vec(vu);
               vecscale((float) MAXCOL / 256.0,vrp,vrp);  /* uniform scale */
               cross(vrp,vu,vr);
               cross(vr,vrp,vu);
               normalize(vu);
               normalize(vr);
               vecsum(vrp,eye,vrp);
               break;
          case 'l' :
               light[numlits].kind = 0;
               read_vec(light[numlits].intensity);
               read_vec(light[numlits].direction);
               normalize(light[numlits++].direction);
               break;
          case 'L' :
               light[numlits].kind = 1;
               read_vec(light[numlits].intensity);
               read_vec(light[numlits].direction);
               fscanf(df,"%f",&light[numlits].distscale);
               fscanf(df,"%f",&light[numlits].radius);
               light[numlits].distscale = 
                 light[numlits].distscale * light[numlits].distscale;
               numlits++;
               break;
          case '{' :
               CHECK_ALLOC(scr_e,extent);
               *opp = (long)scr_e;
               eptr(*opp)->next = NULL;
               eptr(*opp)->kind = EXTENT;
               eptr(*opp)->sub = NULL;
               readimagefile(&eptr(*opp)->sub);
               opp = (long *) &eptr(*opp)->next;
               break;
          case '}' :
               return;
               break;
          case 's' :
               CHECK_ALLOC(scr_s,sphere);
               *opp = (long)scr_s;
               sptr(*opp)->next = NULL;
               sptr(*opp)->kind = SPHERE;
               read_attr(&sptr(*opp)->attr);
               read_vec(sptr(*opp)->center);
               fscanf(df,"%f",&sptr(*opp)->radius);
               opp = (long *) &sptr(*opp)->next;
               break;
          case 't' :
               CHECK_ALLOC(scr_t,triangle);
               *opp = (long)scr_t;
               tptr(*opp)->next = NULL;
               tptr(*opp)->kind = TRIANGLE;
               read_attr(&tptr(*opp)->attr);
               read_vec(tptr(*opp)->position);
               read_vec(tptr(*opp)->ve);
               read_vec(tptr(*opp)->vp);
               opp = (long *) &tptr(*opp)->next;
               break;
          case 'x' :
               read_attr(&temp.attr);
               read_vec(temp.position);
               read_vec(c);
               read_vec(b);
               whichf = temp.attr.tex - 60;
               temp.attr.tex = fractal[whichf].texture;
               fracopp = &*opp;
               dofractal(fractal[whichf].level,
                 temp.position,b,c,&temp.attr);
               opp = &*fracopp;
               break;
          case 'q' :
               CHECK_ALLOC(scr_q,quad);
               *opp = (long)scr_q;
               qptr(*opp)->next = NULL;
               qptr(*opp)->kind = QUAD;
               read_attr(&qptr(*opp)->attr);
               read_vec(qptr(*opp)->position);
               read_vec(qptr(*opp)->ve);
               read_vec(qptr(*opp)->vp);
               opp = (long *) &qptr(*opp)->next;
               break;
          case 'r' :
               CHECK_ALLOC(scr_r ,ring);
               *opp = (long)scr_r;
               rptr(*opp)->next = NULL;
               rptr(*opp)->kind = RING;
               read_attr(&rptr(*opp)->attr);
               read_vec(rptr(*opp)->position);
               read_vec(rptr(*opp)->ve);
               read_vec(rptr(*opp)->vp);
               fscanf(df,"%f",&rptr(*opp)->minrad);
               fscanf(df,"%f",&rptr(*opp)->maxrad);
               rptr(*opp)->minrad =
                 rptr(*opp)->minrad * rptr(*opp)->minrad;
               rptr(*opp)->maxrad =
                 rptr(*opp)->maxrad *rptr(*opp)->maxrad;
               opp = (long *) &rptr(*opp)->next;
               break;
          case 'c' :
               CHECK_ALLOC(scr_c ,cylinder);
               *opp = (long)scr_c;
               cptr(*opp)->next = NULL;
               cptr(*opp)->kind = CYLINDER;
               read_attr(&cptr(*opp)->attr);
               read_vec(cptr(*opp)->bottom);
               read_vec(cptr(*opp)->top);
               fscanf(df,"%f",&cptr(*opp)->a);
               fscanf(df,"%f",&cptr(*opp)->b);
               fscanf(df,"%f",&cptr(*opp)->c);
               opp = (long *) &cptr(*opp)->next;
               break;
          default  :
               sprintf(fname,"Bad command char '%c' encountered",str[0]);
               ERROR(fname);
               break;
          }
}

void getinput(argc,argv)
int     argc;
char    **argv;
{
     int       i;
     char      *s, *strchr();
     rextent   re;

     if (argc == 1) 
     {
          printf("Scene description file? ");
          scanf("%s",fname);
     }
     else
          sprintf(fname,"%s",argv[1]);

     /* if no filetype specified, default it */

     if (strchr( fname, '.' ) == NULL)
     {
          strcat( fname, ".dat" );
     }

     df = fopen(fname,"r");
     if (df == NULL)                               /* v1.01  */
          ERROR("Error opening input file.");       /* v1.01  */

     /* get name portion of file, append .$ST to it for statistics */

     strcpy( outname, fname );
     s = strchr( outname, '.');              /* guaranteed to be there */
     if (s == NULL)
          s = outname + strlen(outname);     /* but just to be sure... */

     *s = '\0';
     strcat( outname, ".$st" );
     fpout = fopen(outname,"w");
     if (fpout == NULL)                             /* v1.01  */
          ERROR("Error opening statistics file.");  /* v1.01  */


     readimagefile(&root);

     getextent(root,&re);

     /* dumpnode(root);   DEBUG output */

     fclose(df);
}

void write_scanline()
{
     int siz;

     fwrite(&sline,1,sizeof(int),fp);

/*- 1/21/89 ---
    fwrite(redline,1,sizeof(scanlinetype),fp);
    fwrite(greenline,1,sizeof(scanlinetype),fp);
    fwrite(blueline,1,sizeof(scanlinetype),fp);
-*/
     siz = (MAXCOL/PPW)*sizeof(int);     /* file line size */
     fwrite(redline,1,siz,fp);
     fwrite(greenline,1,siz,fp);
     fwrite(blueline,1,siz,fp);

     sline++;
}

void getoutput(argc,argv)
int     argc;
char    **argv;
{
     if (argc == 1) 
     {
          printf( "File name to save picture (.tmp)? " );
          scanf( "%s", fname );
     }
     else
          sprintf(fname,"%s.tmp",argv[1]);

     fp = fopen(fname,"wb");   /* V1.01 - IBM level 2 i/o must be binary */

     if (fp == NULL)
     {
          ERROR( "Error creating output file." );
     }
     else
     {
          fwrite(&MAXCOL,1,sizeof( MAXCOL ),fp);
          fwrite(&MAXROW,1,sizeof( MAXROW ),fp);
     }
}

