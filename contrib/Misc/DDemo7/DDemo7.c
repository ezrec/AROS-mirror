/* Dreier in C*/
/* Abgeleitet aus dem Basic-Programm Dreier */
/* P. Kittel Ffm 23.7.87, 14.6.94 */
/* Grafik-Demo-Programm mit schnellem Flächenfüllen (Dreiecke) und
   Farbzyklus-Animation */
/* Version mit 64 Farben */
/* Version für OS 3.0 und AA-Chips */
/* Version mit zusätzlichem Disable() und Enable() */
/* Version mit doppeltem TmpRas und LoadRGB4 statt SetRGB4 */
/* Version für automatische NTSC/PAL-Erkennung */
/* Bei Aufruf vom CLI aus kann eine Zahl mitangegeben werden, die
   die Laufzeit bestimmt, eine Einheit entspricht etwa 1/2 Sekunde */
/* Compiliert mit Lattice C-Compiler 6.2 */

#include <stdio.h>
#include <stdlib.h>
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <math.h>

#include <proto/exec.h>
#include <proto/dos.h>
//#include <clib/graphics_protos.h>
//#include <pragmas/graphics_pragmas.h>
//#include <clib/intuition_protos.h>
//#include <pragmas/intuition_pragmas.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <aros/oldprograms.h>

long __stack=20480;

struct IntuitionBase *IntuitionBase; /* Pointer aus exec */
struct GfxBase *GfxBase;
struct ViewPort *vp;
#define INTUITION_REV 39
#define GRAPHICS_REV 39

struct TextFont *font;
struct TextAttr MyFont =
   {
   "topaz.font",
   TOPAZ_SIXTY,
   FS_NORMAL,
   FPF_ROMFONT,
   };

struct NewScreen NewScreen =
   {
   0,
   0,
   570,   /* Nicht volle Breite, da Grafik kreisförmig.         */
          /* Aber breiter als Window, damit Vorder/Hintergrund- */
          /* Gadget und ein Stück Titelbalken des Screens frei  */
          /* sind zum Betätigen bzw. Ziehen mit der Maus.       */
   256,   /* Höhe für PAL-Format vorbelegt */
   6,
   0, 1,
   HIRES,
   CUSTOMSCREEN,
   &MyFont,
   " Commodore - AMIGA     Peter-Demos DDemo7 ",
   NULL,
   NULL,
   };

void main(argc,argv)
int argc;
char *argv[];
{
   struct Screen *Screen;
   struct NewWindow NewWindow;
   struct Window *Window;
   struct RastPort *rp;
   UWORD areabuffer[20];
   struct TmpRas mytmpras;
   struct AreaInfo myareainfo;
   PLANEPTR myplane;
   struct IntuiMessage *msg;
   ULONG  colormap[770],rl,gl,bl;
   int i,i3,x1,y1,x2,y2,x3,y3,x9,y9,nn,nnm1,nf,pf,ffi,co,fpal,z,zd,zm;
   int cc,c2,c6,cd,co2;
   double fs,ff,sif,y0,ye,z1,z2,s2,z3,a,m,fr,fg,fb,ar,ag,ab,cf,nn1,nnf;
   double pr[256],pg[256],pb[256];

   nf=6;     nn=63;   /* Anzahl Bitplanes, Farben */
   nn1=(double)(nn-1); nnm1=(nn-1)*3;
   nnf=(double)(0xffffffff);

   x9=500;   y9=184+53+2; fpal=1;   /* Vorbelegung für PAL */
             y0= 58.0;    ye=119.0; /* Anfangskoordinaten  */
   x1=250;   y1=140;      z1=0.0;
   x2= 90;   y2=y1;       z2=0.0;   z3=0.0;

   fs=0.0; ff=4000.0; sif=273.0+sin(0.15);  pf=0;  /* Farben */
   c2=15*256; c6=15*16; co=2;
   cf=1.0; cc=0;        fr=0.0; fg=0.0; fb=0.0;

   IntuitionBase = (struct IntuitionBase *)
      OpenLibrary ("intuition.library",INTUITION_REV);
   if (IntuitionBase == NULL) exit (10);

   GfxBase = (struct GfxBase *)
      OpenLibrary("graphics.library",GRAPHICS_REV);
   if(GfxBase == NULL)
      { CloseLibrary((struct Library *)IntuitionBase);
        exit (10);
      }

   /* PAL / NTSC  ? */
   if (fpal > 0)
      { if (!((GfxBase->DisplayFlags) & PAL)) fpal=0; }

   if (fpal == 0)  
      { NewScreen.Height=200;               /* NTSC! */
        y9-=55; y1=105; y0=45.0; ye=93.0;   /* NTSC-Koordinaten */
      }

   colormap[0]=2L<<16;     /* 2 Farben */
   colormap[1]=0x77777777; /* Hintergrund in grau */
   colormap[2]=0x77777777;
   colormap[3]=0x77777777;
   colormap[4]=0xFFFFFFFF; /* Rahmen      in weiß */
   colormap[5]=0xFFFFFFFF;
   colormap[6]=0xFFFFFFFF;
   colormap[7]=0;          /* Tabellenende */
   if((Screen = (struct Screen *) OpenScreenTags(&NewScreen,
         SA_Colors32   ,colormap,
         SA_Interleaved,TRUE,
         TAG_END       ,0L                      )) == NULL)
      { CloseLibrary((struct Library *)GfxBase);
        CloseLibrary((struct Library *)IntuitionBase);
        exit (10);
      }

   NewWindow.LeftEdge = 0;
   NewWindow.TopEdge  = 16;
   NewWindow.Width  = x9;   /*  9 Pixel für Randabstand */
   NewWindow.Height = y9;  /* 16 Pixel für Rand und Titelleiste */
   NewWindow.DetailPen = 0;
   NewWindow.BlockPen  = 1;
   NewWindow.Title = NULL;
   NewWindow.Flags =
     WFLG_CLOSEGADGET|WFLG_BORDERLESS|WFLG_BACKDROP;
   NewWindow.IDCMPFlags = CLOSEWINDOW;   /* Hier Abbruchmöglichkeit */
   NewWindow.Type = CUSTOMSCREEN;
   NewWindow.FirstGadget = NULL;
   NewWindow.CheckMark = NULL;
   NewWindow.Screen = Screen;
   NewWindow.BitMap = NULL;
   NewWindow.MinWidth = x9;
   NewWindow.MinHeight = y9;
   NewWindow.MaxWidth = x9;
   NewWindow.MaxHeight = y9;
   if(( Window = ( struct Window *) OpenWindow( &NewWindow )) == NULL )
     { CloseScreen (Screen);
       CloseLibrary((struct Library *)IntuitionBase);
       CloseLibrary((struct Library *)GfxBase);
       exit(10);
     }

   rp=Window->RPort;
   vp=&Screen->ViewPort;
   font=(struct TextFont *)OpenFont(&MyFont);
   SetFont(rp,font);

   /* Palette mit Graukeil vorbelegen */
   bl=0xffffffff/nn;
   for (i=2; i<=nn; i++)
     { rl=bl*i;
       pr[i]=0.0; pg[i]=0.0; pb[i]=0.0;
       colormap[3*i]=rl; colormap[3*i+1]=rl; colormap[3*i+2]=rl;
     }
   colormap[0]=(62L<<16)+2; /* 62 Farben (alle bis auf 0 und 1) */
   colormap[187]=0;          /* Tabellenende */
 
   /* Für Flächenfüllen benötigt */
   InitArea(&myareainfo,areabuffer,8);
   rp->AreaInfo=&myareainfo;
   myplane     =(PLANEPTR)AllocRaster(640,256);
   if (myplane==NULL)
     { CloseWindow (Window);
       CloseScreen (Screen);
       CloseLibrary((struct Library *)IntuitionBase);
       CloseLibrary((struct Library *)GfxBase);
       exit(10);
     }
   rp->TmpRas  =(struct TmpRas *)&mytmpras;
                  InitTmpRas(&mytmpras,myplane,RASSIZE(640,256));

   SetAPen(rp,1);      /* Reklametext in weiß */
   Move(rp,      30,    8); Text(rp,"Schnelle"     , 8);
   Move(rp,      30,   17); Text(rp,"Grafik"       , 6);
   SetAPen(rp,nn);     /* Dieser Text macht Farbzyklus mit */
   Move(rp,      30,y9-12); Text(rp,"16,8 Mio"     , 8);
   Move(rp,      30,y9- 3); Text(rp,"Farbtöne"     , 8);
   SetAPen(rp,1);      /* Wieder weiß */
   Move(rp,x9-13*10,    8); Text(rp,"Flächenfüllen",13);
   Move(rp,x9-11*10,   17); Text(rp,"mit Blitter"  ,11);
   Move(rp,x9- 5*10,y9-12); Text(rp,"Bitte"        , 5);
   Move(rp,x9-12*10,y9- 3); Text(rp,"vergleichen!" ,12);

   zm = 9;
   zd = 0;       /* Schrittweite 0 für unbegrenzte Laufzeit */
   /* Interpretation der CLI-Parameter */
   if (argc > 1) { zm=atoi(argv[argc-1]); zd=1; } /* Laufzeit begrenzt */
   z  = 0;
   co2= 0;

   while (z < zm)     /* Schleife über vorgegebene Anzahl Durchgänge */
     {
       if ((msg = (struct IntuiMessage *)GetMsg(Window->UserPort)) != NULL)
         { ReplyMsg((struct Message *)msg); z=zm+1; }      /* Abbruch! */

       /* Nächste Dreieckskoordinaten */
       z1+=0.01;           if (z1 > 6.28) z1-=6.28;
       z2+=0.03*sin(z1);   if (z2 > 6.28) z2-=6.28;
       s2 =sin(z2);        /* Modulation des Radius */
       /* Modulierter Winkelfortschritt */
       z3+=z1*s2/4;        if (z3 > 6.28) z3-=6.28;

       /* Dreieckspunkt 1 immer fest in Mitte */
       x3=x2;  /* Alter Punkt 2 wird Punkt 3 */
       y3=y2;
       /* Punkt 2 neu ermitteln */
       x2=(int)(120.0*(1.0+s2)*cos(z3)+(double)x1);
       y2=(int)(   y0*(1.0+s2)*sin(z3)+        ye);

       co++;            /* Farbzähler */
       if (co > nn)
         {
         co=2; z+=zd;   /* Nächster Durchlauf durch die Palette */

         /* Nächsten Farbton bestimmen */
         fs+=0.1; if (fs > 7.0) fs-=7.0;
         /* Linearer plus Modulationsanteil */
         ff+=sif*(1.0+cos(fs)*1.02);
         if (ff > 4095.49) ff-=4095.49;  /* Farbnummer */
         ffi=(int)ff;
         ar=fr; ag=fg; ab=fb;
         /* Einzelne Farbkomponenten */
         fr=(ffi & 15)/15.0;
         fg=(ffi & c6)/((double)c6);
         fb=(ffi & c2)/((double)c2);
         pr[nn]=fr; pg[nn]=fg; pb[nn]=fb;

         /* cf entscheidet bei Interpolation über Extremwert 1 oder 0 */
         if (cc++ > 20) { cc=0; cf=-cf; }         /* Alle 20 wechseln */

         /* Rotanteil interpolieren */
         if ((cf*(fr+ar)) > cf)
           { m=(2.0-fr-ar)/nn1; a=ar-m; /* Extremwert 1 */
             for (i=2; i<nn; i++)
               { pr[i]=a+((double)i)*m; if (pr[i] > 1.0) pr[i]=2.0-pr[i]; }
           } else
           { m=(   -fr-ar)/nn1; a=ar-m; /* Extremwert 0 */
             for (i=2; i<nn; i++)
               { pr[i]=a+((double)i)*m; if (pr[i] < 0.0) pr[i]=   -pr[i]; }
           }

         /* Grünanteil interpolieren */
         if ((cf*(fg+ag)) > cf)
           { m=(2.0-fg-ag)/nn1; a=ag-m;
             for (i=2; i<nn; i++)
               { pg[i]=a+((double)i)*m; if (pg[i] > 1.0) pg[i]=2.0-pg[i]; }
           } else
           { m=(   -fg-ag)/nn1; a=ag-m;
             for (i=2; i<nn; i++)
               { pg[i]=a+((double)i)*m; if (pg[i] < 0.0) pg[i]=   -pg[i]; }
           }

         /* Blauanteil interpolieren */
         if ((cf*(fb+ab)) > cf)
            { m=(2.0-fb-ab)/nn1; a=ab-m;
             for (i=2; i<nn; i++)
               { pb[i]=a+((double)i)*m; if (pb[i] > 1.0) pb[i]=2.0-pb[i]; }
           } else
           { m=(   -fb-ab)/nn1; a=ab-m;
             for (i=2; i<nn; i++)
               { pb[i]=a+((double)i)*m; if (pb[i] < 0.0) pb[i]=   -pb[i]; }
           }
         }

       /* Palette Farben 2 bis nn zyklisch umbelegen */
       i3=nnm1;
       rl=colormap[i3-2];  gl=colormap[i3-1];  bl=colormap[i3];
       while (i3>3)  { colormap[i3]=colormap[i3-3]; i3--; }
       colormap[1]=rl;  colormap[2]=gl;  colormap[3]=bl;

       /* Nur einen neuen Farbton zur Zeit neu anzeigen */
       cd=2*co-2; if (cd > nn) cd=cd-nn+1;
       i3=3*cd-5;
       colormap[i3  ]=(ULONG)(pr[co]*nnf);
       colormap[i3+1]=(ULONG)(pg[co]*nnf);
       colormap[i3+2]=(ULONG)(pb[co]*nnf);
       LoadRGB32(vp,colormap);

       /* Dreieck malen */
       SetAPen (rp,co);
       AreaMove(rp,x1,y1);
       AreaDraw(rp,x2,y2);
       AreaDraw(rp,x3,y3);
       WaitTOF();
       AreaEnd (rp);
     }  

   /* Sauber aufräumen */

   FreeRaster  (myplane,640,256);
   CloseWindow (Window);
   CloseScreen (Screen);
   CloseLibrary((struct Library *)IntuitionBase);
   CloseLibrary((struct Library *)GfxBase);

}
