/* Liniengrafik*/
/* Abgeleitet aus dem Basic-Programm Quadro und dem C-Programm Quadro4 */
/* P. Kittel Ffm 16.6.88, 14.6.94 */
/* Grafik-Demo-Programm mit schnellem Flächenfüllen (Vierecke) und
   Farbzyklus-Animation */
/* Version mit 64 Farben */
/* Version für OS 3.0 und AA-Chips */
/* Version mit voller Zeichenfläche und Borderless-Window
     daher auch ScreenToBack über neuen Menüpunkt nötig   */
/* Version mit zusätzlichem Menüpunkt für Zeichenstopp */
/* Version mit quadratischer Zeichenfläche */
/* Version mit anderen Parametern */
/* Version mit LoadRGB4 statt SetRGB4 */
/* Version für automatische NTSC/PAL-Erkennung */
/* Bei Aufruf vom CLI aus kann eine Zahl mitangegeben werden, die
   die Laufzeit bestimmt, eine Einheit entspricht etwa 1/2 Sekunde */
/* Compiliert mit Lattice C-Compiler 6.2 */

/* Prinzip: Eine Linie der Länge dx bewegt sich über den Schirm,
            dabei sind folgende Größen dynamisch:
            - Länge dx der Linie
            - Vorzeichen vfi der Linienlängenveränderung
            - Translation vx in x-Richtung
            - Translation vy in y-Richtung
            - Rotationswinkel rot
            - Änderungsgeschwindigkeit drot des Rotatonswinkels
            Je nach Symmetriemodus wird die Linie bis zu 8fach
            gezeichnet.
            Die Fläche zwischen der aktuellen Linie und ihrer
            Vorgängerin wird als allgemeines Viereck gefüllt.
            Bei Anstoßen an eine Fenstergrenze wird die Linie
            reflektiert. Dabei werden die genannten Parameter
            alle auf ihren Folgewert gesetzt (dx und rot sowieso
            bei jedem Schritt).
            Wenn die Linie nach Reflexion immer noch die Fenstergrenzen
            überschreitet, wird
            - die Länge halbiert,
            - vfi auf -1 gesetzt und
            - je nach Menüwahl auch der Symmetriemodus geändert.       */

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
#include <clib/graphics_protos.h>
#include <pragmas/graphics_pragmas.h>
#include <clib/intuition_protos.h>
#include <pragmas/intuition_pragmas.h>

#include <aros/oldprograms.h>

void xx(void);

long __stack=8192;

struct IntuitionBase *IntuitionBase; /* Pointer aus exec */
struct GfxBase *GfxBase;
struct RastPort *rp;
struct ViewPort *vp;
#define INTUITION_REV 39
#define GRAPHICS_REV 39
#define SCRW 640

int vfi,dx,xq1,xq2,xmi,xma,x1,x2,x3,x4,x9,y9,u1,u2,u3,u4,v1,v2,v3,v4;
int spiea,spieg;
double x,x0,y10,asp,sir,cor,rot;

void xx()
{
if (spiea!=spieg)
  { if (spieg==6)
      { SetAPen(rp,0);
        RectFill(rp,    0,0,xq1-1,y9);
        RectFill(rp,xq2+1,0,x9   ,y9);
        dx =(dx*(xq2-xq1))/x9;   if (dx<10)  { dx=10; vfi=1; }
        sir=(double)dx*sin(rot); cor=(double)dx*cos(rot);
        xmi=xq1; xma=xq2;
        if ( x0*asp > (double)(xq2-xq1) )
          { x0=((x0*(xq2-xq1))/(asp*x9)); x=x0; }
      }  else
      { xmi=0; xma=x9; }
  }
spiea=spieg;
}

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
   SCRW,
   256,   /* Höhe für PAL-Format vorbelegt */
   6,
   0, 0,
   HIRES,
   CUSTOMSCREEN,
   NULL,
   NULL,
   NULL,
   NULL,
   };

struct IntuiText MText[10] = {
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"8fach       ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"4fach       ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"2fach Punkt ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"2fach horiz ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"2fach vert  ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"1fach       ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"Alle        ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"Halt        ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"Nach hinten ",NULL },
  { 0,1,JAM2,CHECKWIDTH,0,NULL,"Ende        ",NULL }
  };

struct MenuItem MIt[11] = { NULL };

struct Menu Men = { NULL,0,0,136,0,MENUENABLED,"Symmetrie ",MIt };

void main(argc,argv)
int argc;
char *argv[];
{
   struct Screen *Screen;
   struct NewWindow NewWindow;
   struct Window *Window,*wi2;
   UWORD areabuffer[40];
   struct TmpRas mytmpras;
   struct AreaInfo myareainfo;
   PLANEPTR myplane;
   struct IntuiMessage *msg;
   ULONG class;
   USHORT code;
   ULONG colormap[200],rl,gl,bl;
   int i,i3,y1,y2,x3,y3,x4,y4,u3,u4,nn,nnm1,nf,pf,ffi,co,fpal,z,zd,zm;
   int cc,c2,c6,cd,co2,vx,vy,vx0,vy0,ww,w2,dspieg,frot,fzei;
   double fs,ff,sif,a,m,fr,fg,fb,ar,ag,ab,cf,nn1,nnf;
   double vf,y,pi,drot,ddrot;
   double pr[64],pg[64],pb[64];

   nf=6;     nn=63;   /* Anzahl Bitplanes, Farben */
   nn1=(double)(nn-1);  nnm1=(nn-1)*3;
   nnf=(double)(0xffffffff);

   y9=254; fpal=1; asp=1.9;  /* Vorbelegung für PAL */

   vx0=3;   vy0=2;     vx=vx0; vy=vy0;
   vf=.98;  vfi=1;
   rot=.3;  drot=-.04; ddrot=.011;  pi=3.1415926536;
   dx=10;

   fs=0.0; ff=4000.0;  sif=273.0+sin(0.15);  pf=0;  /* Farben */
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
        y9=198; asp=(asp*256)/200;          /* NTSC-Koordinaten */
      }
   x9=639;
   x=(double)(x9)/(2.0*asp);
                      x1=(int)(x-1.0); x2=(int)(x+1.0); u1=x1; u2=x2;
   y=(double)(y9/2);  y1=(int)(y-1.0); y2=(int)(y+1.0); v1=y1; v2=y2;
   xq1=(x9-y9*asp)/2; xq2=x9-xq1;
   
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
         TAG_END       ,0L                   )) == NULL)
      { CloseLibrary((struct Library *)GfxBase);
        CloseLibrary((struct Library *)IntuitionBase);
        exit (10);
      }

   /* Dieses Fenster dient nur dazu, die Screen-Gadgets rechts oben
      in der ersten Pixelzeile zu verdecken */
   NewWindow.LeftEdge    = 580;
   NewWindow.TopEdge     =   0;
   NewWindow.Width       =  60;
   NewWindow.Height      =   2;
   NewWindow.DetailPen   =   0;
   NewWindow.BlockPen    =   0;
   NewWindow.Title       = NULL;
   NewWindow.Flags       = BORDERLESS;
   NewWindow.IDCMPFlags  = 0;
   NewWindow.Type        = CUSTOMSCREEN;
   NewWindow.FirstGadget = NULL;
   NewWindow.CheckMark   = NULL;
   NewWindow.Screen      = Screen;
   NewWindow.BitMap      = NULL;
   NewWindow.MinWidth    = 60;
   NewWindow.MinHeight   =  2;
   NewWindow.MaxWidth    = 60;
   NewWindow.MaxHeight   =  2;
   if(( wi2 = ( struct Window *) OpenWindow( &NewWindow )) == NULL )
     { CloseScreen (Screen);
       CloseLibrary((struct Library *)IntuitionBase);
       CloseLibrary((struct Library *)GfxBase);
       exit(10);
     }

   /* Dieses Fenster ist um 1 Pixel nach unten verlegt, damit
      man den Screen mit der Maus zum Ziehen anpacken kann.   */
   NewWindow.LeftEdge    = 0;
   NewWindow.TopEdge     = 1;
   NewWindow.Width       = x9+1;
   NewWindow.Height      = y9+1;
   NewWindow.DetailPen   = 0;
   NewWindow.BlockPen    = 1;
   NewWindow.Title       = NULL;
   NewWindow.Flags       = WFLG_BORDERLESS|WFLG_ACTIVATE;
   NewWindow.IDCMPFlags  = MENUPICK; /* Hier im Menü Abbruchmöglichkeit */
   NewWindow.Type        = CUSTOMSCREEN;
   NewWindow.FirstGadget = NULL;
   NewWindow.CheckMark   = NULL;
   NewWindow.Screen      = Screen;
   NewWindow.BitMap      = NULL;
   NewWindow.MinWidth    = x9+1;
   NewWindow.MinHeight   = y9+1;
   NewWindow.MaxWidth    = x9+1;
   NewWindow.MaxHeight   = y9+1;
   if(( Window = ( struct Window *) OpenWindow( &NewWindow )) == NULL )
     { CloseWindow (wi2);
       CloseScreen (Screen);
       CloseLibrary((struct Library *)IntuitionBase);
       CloseLibrary((struct Library *)GfxBase);
       exit(10);
     }

   rp=Window->RPort;
   vp=&Screen->ViewPort;
   font=(struct TextFont *)OpenFont(&MyFont);
   SetFont(rp,font);

   for (i=0; i<10; i++)
     { MIt[i].NextItem=&MIt[i+1];
       MIt[i].LeftEdge=0;
       MIt[i].TopEdge =i*11;
       MIt[i].Width   =136;
       MIt[i].Height  =11;
       MIt[i].Flags   =ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT;
       MIt[i].ItemFill=(APTR)&MText[i];
     }
   spieg=6; dspieg=1; fzei=1;  /* Betriebsart Alle, anfangs 8fach */
   spiea=spieg; xmi=xq1; xma=xq2;
   MIt[6].Flags   =ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT|CHECKED;
   MIt[9].NextItem=NULL;
   SetMenuStrip(Window,&Men);

   /* Palette mit Graukeil vorbelegen */
   bl=0xffffffff/nn;
   for (i=2; i<=nn; i++)
     { rl=bl*i;
       pr[i]=0.0; pg[i]=0.0; pb[i]=0.0;
       colormap[3*i]=rl;  colormap[3*i+1]=rl;  colormap[3*i+2]=rl;
     }
   colormap[0]=(62L<<16)+2;  /* 62 Farben (alle bis auf 0 und 1) */
   colormap[187]=0;          /* Tabellenende */
 
   /* Für Flächenfüllen benötigt */
   InitArea(&myareainfo,areabuffer,8);
   rp->AreaInfo=&myareainfo;
   myplane     =(PLANEPTR)AllocRaster(SCRW,256);
   if (myplane==NULL)
     { CloseWindow (Window);
       CloseScreen (Screen);
       CloseLibrary((struct Library *)IntuitionBase);
       CloseLibrary((struct Library *)GfxBase);
       exit(10);
     }
   rp->TmpRas  =(struct TmpRas *)&mytmpras;
                  InitTmpRas(&mytmpras,myplane,RASSIZE(SCRW,256));

   zm = 9;
   zd = 0;       /* Schrittweite 0 für unbegrenzte Laufzeit */
   /* Interpretation der CLI-Parameter */
   if (argc > 1) { zm=atoi(argv[argc-1]); zd=1; } /* Laufzeit begrenzt */
   z  = 0;
   co2= 0;

   while (z < zm)     /* Schleife über vorgegebene Anzahl Durchgänge */
     {
       if (msg = (struct IntuiMessage *)GetMsg(Window->UserPort))
         { class=msg->Class;
           code =msg->Code ;
           ReplyMsg((struct Message *)msg);
           if ( (class==MENUPICK) && (code!=MENUNULL) )
             { code=ITEMNUM(code);
               if (code<8)
                 { ClearMenuStrip(Window);
                   for (i=0; i<9; i++)
                     MIt[i].Flags=ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT;
                   MIt[code].Flags |= CHECKED;
                   SetMenuStrip(Window,&Men);
                 }
               spiea=spieg;
               switch (code)
                 { case 0 :  spieg=6;  dspieg=0;   fzei=1; xx(); break;
                   case 1 :  spieg=3;  dspieg=0;   fzei=1; xx(); break;
                   case 2 :  spieg=2;  dspieg=0;   fzei=1; xx(); break;
                   case 3 :  spieg=4;  dspieg=0;   fzei=1; xx(); break;
                   case 4 :  spieg=5;  dspieg=0;   fzei=1; xx(); break;
                   case 5 :  spieg=1;  dspieg=0;   fzei=1; xx(); break;
                   case 6 :            dspieg=1;   fzei=1;       break;
                   case 7 :                        fzei=0;       break;
                   case 8 :  ScreenToBack(Screen); fzei=1;       break;
                   case 9 :  z=zm+1;                     /* Abbruch */
                 }
             }
         }

       if ( fzei )    /* Bei Halt Zeichnen stoppen, nur Farbzyklus weiter */
         {

         /* Nächste Koordinaten */
   
         x0=x; y10=y;
         dx+=vfi;
         if ( dx< 20 ) vfi= 1;
         if ( dx> 70 ) vfi=-1;

         rot+=drot;  frot=0;
         if ( rot>pi )
           { rot-=pi; frot++; }
              else
           { if ( rot<0 )
               { rot+=pi; frot++; }
           }
         w2=0; ww=1;
  
         while ( ww )
           {
           ww=0;
           sir=(double)dx*sin(rot); cor=(double)dx*cos(rot);
           x =x0+(double)vx; y=y10+(double)vy;
           x3=x1; y3=y1; x4=x2; y4=y2;
           v3=v1; u3=u1; v4=v2; u4=u2;
           x1=xmi+(int)(asp*(x+cor)); x2=xmi+(int)(asp*(x-cor));
           v1=    (int)     (x+cor) ; v2=    (int)     (x-cor) ;
           if ( x1<=xmi || x1>=xma || x2<=xmi || x2>=xma ) { ww=1; vx=-vx; }
           y1=    (int)     (y+sir) ; y2=    (int)     (y-sir) ;
           u1=xmi+(int)(asp*(y+sir)); u2=xmi+(int)(asp*(y-sir));
           if ( y1<=0   || y1>=y9  || y2<=0   || y2>=y9  ) { ww=1; vy=-vy; }
           if ( ww )
             {
/* printf("x1,y1  x2,y2  x,x0: %d,%d  %d,%d  %f,%f\n",x1,y1,x2,y2,x,x0); */
               x1=x3; y1=y3; x2=x4; y2=y4;
               v1=v3; u1=u3; v2=v4; u2=u4;

               vfi=-vfi;
               if ( dx< 80 ) vfi= 1;
               if ( dx>110 ) vfi=-1;
               drot+=ddrot*(1.0+abs(drot)*10.0);
               if (abs(drot)>.2)  { ddrot=-ddrot; drot=.9*drot; }
               w2+=1;
               if ( w2>1 )
                 { vfi=-1;         spiea=spieg;
                   dx =dx/2;       if ( dx   <1 )  dx   =1;
                   spieg+=dspieg;  if ( spieg>6 )  spieg=1;
                   if ( w2>5 )
                     { x0=(x0+0.5*x9)*0.5;
                       y10=(y10+0.5*y9)*0.5;
                       dx=5; vfi=-1; w2=1;
                     }
                   xx();
                 }
               vx0+=1; if ( vx0>6 ) vx0=1;
               vy0+=1; if ( vy0>4 ) vy0=1;
               vx  =vx0*vx/abs(vx);
               vy  =vy0*vy/abs(vy);
             }
           }
         }   else { Delay(2); }   /* Bei Zeichenstopp nicht ZU schnell */

       co++;            /* Farbzähler */
       if (co > nn)
         { co=2; z+=zd;   /* Nächster Durchlauf durch die Palette */

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
       /* Disable(); */
       LoadRGB32(vp,colormap);
       /* Enable(); */

       if ( x3<=xmi ) { x3=xmi+1; } else { if ( x3>=xma ) x3=xma-1; }
       if ( x4<=xmi ) { x4=xmi+1; } else { if ( x4>=xma ) x4=xma-1; }
       if ( u1< xmi ) { u1=xmi  ; } else { if ( u1> xma ) u1=xma  ; }
       if ( u2< xmi ) { u2=xmi  ; } else { if ( u2> xma ) u2=xma  ; }
       if ( u3<=xmi ) { u3=xmi+1; } else { if ( u3>=xma ) u3=xma-1; }
       if ( u4<=xmi ) { u4=xmi+1; } else { if ( u4>=xma ) u4=xma-1; }
       if ( y3<1    ) { y3=1;     } else { if ( y3>=y9  ) y3=y9 -1; }
       if ( y4<1    ) { y4=1;     } else { if ( y4>=y9  ) y4=y9 -1; }
       if ( v1<0    ) { v1=0;     } else { if ( v1> y9  ) v1=y9   ; }
       if ( v2<0    ) { v2=0;     } else { if ( v2> y9  ) v2=y9   ; }
       if ( v3<1    ) { v3=1;     } else { if ( v3>=y9  ) v3=y9 -1; }
       if ( v4<1    ) { v4=1;     } else { if ( v4>=y9  ) v4=y9 -1; }
       if ( fzei )    /* Zeichnen ggf. stoppen */
         {
         /* Viereck(e) malen */
         if (frot)
           { frot=x2; x2=x4; x4=frot;
             frot=y2; y2=y4; y4=frot;
             frot=v2; v2=v4; v4=frot;
             frot=u2; u2=u4; u4=frot;
             frot=1;
           }
         SetAPen (rp,co);
         AreaMove(rp,x1,y1);
         AreaDraw(rp,x2,y2);
         AreaDraw(rp,x4,y4);
         AreaDraw(rp,x3,y3);
         AreaEnd (rp);
         if (spieg==3 || spieg==5 || spieg==6)
           {
             AreaMove(rp,x9-x1,y1);
             AreaDraw(rp,x9-x2,y2);
             AreaDraw(rp,x9-x4,y4);
             AreaDraw(rp,x9-x3,y3);
             AreaEnd (rp);
           }
         if (spieg==3 || spieg==4 || spieg==6)
           {
             AreaMove(rp,x1,y9-y1);
             AreaDraw(rp,x2,y9-y2);
             AreaDraw(rp,x4,y9-y4);
             AreaDraw(rp,x3,y9-y3);
             AreaEnd (rp);
           }
         if (spieg==2 || spieg==3 || spieg==6)
           {
             AreaMove(rp,x9-x1,y9-y1);
             AreaDraw(rp,x9-x2,y9-y2);
             AreaDraw(rp,x9-x4,y9-y4);
             AreaDraw(rp,x9-x3,y9-y3);
             AreaEnd (rp);
           }
         if (spieg==6)
           {
             AreaMove(rp,u1,v1);
             AreaDraw(rp,u2,v2);
             AreaDraw(rp,u4,v4);
             AreaDraw(rp,u3,v3);
             AreaEnd (rp);
             AreaMove(rp,x9-u1,v1);
             AreaDraw(rp,x9-u2,v2);
             AreaDraw(rp,x9-u4,v4);
             AreaDraw(rp,x9-u3,v3);
             AreaEnd (rp);
             AreaMove(rp,u1,y9-v1);
             AreaDraw(rp,u2,y9-v2);
             AreaDraw(rp,u4,y9-v4);
             AreaDraw(rp,u3,y9-v3);
             AreaEnd (rp);
             AreaMove(rp,x9-u1,y9-v1);
             AreaDraw(rp,x9-u2,y9-v2);
             AreaDraw(rp,x9-u4,y9-v4);
             AreaDraw(rp,x9-u3,y9-v3);
             AreaEnd (rp);
           }
         if ( frot )  { x2=x4; y2=y4; v2=v4; u2=u4; }
       }
     }

   /* Sauber aufräumen */
   ClearMenuStrip(Window);
   FreeRaster  (myplane,SCRW,256);
   CloseWindow (wi2   );
   CloseWindow (Window);
   CloseScreen (Screen);
   CloseLibrary((struct Library *)IntuitionBase);
   CloseLibrary((struct Library *)GfxBase);
}
