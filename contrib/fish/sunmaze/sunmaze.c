/*
 *  DWR:  This file has been changed to implement shadow in the
 *  3d presentation of the maze.
 *  This is the original file from Fish 171:  Maze/3DDemo.c
 *  All modifications are marked with "DWR:".
 */

#include <aros/oldprograms.h>
#include <stdio.h> 
#include <libraries/dosextens.h>
#include <intuition/intuition.h>
#include <exec/memory.h>
#include <graphics/gfxmacros.h>

#define BITMAP

#include "maze.h"         /* This is our maze generator */

#define MENUITEMS 6

struct IntuiText item[MENUITEMS*2]=
{ {6,0,JAM2,0,0,NULL,NULL,NULL} };

struct IntuiText abouttxt5=   /* Yes, this game needs lots of credits */
{ 7,0,JAM2,32,60,NULL,"keys to move",NULL};  /* DWR: changed */
struct IntuiText abouttxt4=
{ 7,0,JAM2,12,50,NULL,"Use keypad cursor",&abouttxt5};  /* DWR: changed */
struct IntuiText abouttxt3=
{ 4,0,JAM2,0,40,NULL,"Freely distributable",&abouttxt4};  /* DWR: changed */

struct IntuiText abouttxt3b=
{ 2,0,JAM2,36,30,NULL,"Dirk Reisig",&abouttxt3};  /* DWR: inserted */
struct IntuiText abouttxt3a=
{ 1,0,JAM2,40,20,NULL,"Shadows by",&abouttxt3b};  /* DWR: inserted */

struct IntuiText abouttxt2=
{ 6,0,JAM2,24,10,NULL,"Werner Gunther",&abouttxt3a};  /* DWR: changed */
struct IntuiText abouttxt1=
{ 6,0,JAM2,40,0,NULL,"Written by",&abouttxt2};


struct MenuItem mi[MENUITEMS] =          /* main menu structures */
{
 {
 NULL,0,0,70,11,
 ITEMTEXT|ITEMENABLED|HIGHCOMP,
 NULL,NULL,NULL,NULL,NULL,NULL
 }
};
struct MenuItem sizemenu[MENUITEMS] =    /* sub menu for sizes */
{
 {
 NULL,60,0,90,11,
 ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT,
 NULL,NULL,NULL,NULL,NULL,NULL
 }
};
struct MenuItem about =                  /* about, credits & help */
{
 NULL,60,0,160,70,
 ITEMTEXT|ITEMENABLED,
 NULL,(APTR)&abouttxt1,NULL,NULL,NULL,NULL
};

struct Menu menu=
{
 NULL,0,0,70,11,
 MENUENABLED,
 "Options",
 mi
};

struct NewScreen ns=                     /* screen low-rez, 8 colors */
{
 0,0,320,200,3,
 1,0,
 NULL,
 CUSTOMSCREEN,
 NULL,NULL,NULL,NULL
};

struct NewWindow nw=                     /* window for menus & gadgets */
{
 0,1,286,199,
 1,0,
 CLOSEWINDOW|GADGETUP|MENUPICK|MENUVERIFY|VANILLAKEY,
 BORDERLESS|NOCAREREFRESH|WINDOWCLOSE|ACTIVATE,
 NULL, NULL,
 "Maze 3D Demo",
 NULL, NULL, NULL,NULL,NULL,NULL,
 CUSTOMSCREEN
};

struct Image mazeimage={0,0,0,0,1,0,4,4,0}; /* Image to display the maze */

char mtxt[MENUITEMS][9]=                    /* menu text */
{
 "Map","New Maze","Size","Demo","Quit","About"
};
char sizetxt[MENUITEMS][11]=                /* text for size menu */
{
 "Trivial","Simple","Normal","Advanced","Difficult","Impossible"
};

struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Screen *screen = NULL;
struct Window *window = NULL;
struct BitMap *bm;
struct IntuiMessage *msg;
struct AreaInfo  areainfo;
struct TmpRas tmpras;
struct RastPort *rp;

ULONG  class,code;
BOOL   WorkBench=FALSE,demomode=FALSE,forward();
void   right(),left(),demo(),newmaze(),leave(),main(),show3d(),showmaze();
void   statusline();
short  i,j,xsize,ysize,mazesize=2,x,y,wayout,moves;
char   *mymaze=NULL,areapts[7*5],*raster=NULL,buffer[20];
short  dirincs[4][2]={{0,-1},{1,0},{0,1},{-1,0}};
char   heading[][7]={"North"," East ","South"," West "};

int IsPassage(short, short);

void main(argc,argv)
short argc;
char argv[];
{
 if (!argc) WorkBench=TRUE;      /* open stuff */
 if (!(IntuitionBase = (struct IntuitionBase*)
  OpenLibrary("intuition.library",0L)))
    leave("No Intuition Library");
 if (!(GfxBase = (struct GfxBase*) OpenLibrary("graphics.library",0L)))
    leave("No Graphics Library");

 for(i=0;i<MENUITEMS;i++)        /* set menu-strip */
 {                               /* could have be done on declaration */
  sizemenu[i]=sizemenu[0];
  sizemenu[i].NextItem=&sizemenu[i+1];
  sizemenu[i].ItemFill=(APTR)&item[i+MENUITEMS];
  sizemenu[i].TopEdge=i*11;
  item[i]=item[0];
  item[i+MENUITEMS]=item[0];
  item[i+MENUITEMS].IText=sizetxt[i];
  item[i+MENUITEMS].LeftEdge=11;
  item[i].IText=mtxt[i];
  mi[i]=mi[0];
  mi[i].NextItem=&mi[i+1];
  mi[i].ItemFill=(APTR)&item[i];
  mi[i].TopEdge=i*11;
 }
 mi[MENUITEMS-1].NextItem=NULL;
 sizemenu[MENUITEMS-1].NextItem=NULL;
 mi[2].SubItem=&sizemenu[0];
 mi[5].SubItem=&about;
 sizemenu[mazesize].Flags|=CHECKED;

 if (!(screen = (struct Screen*) OpenScreen(&ns))) /* open screen & window */
    leave ("Can't open Screen");
 bm = screen->ViewPort.RasInfo->BitMap;
 rp = &screen->RastPort;

 nw.Screen=screen;
 if (!(window = (struct Window*) OpenWindow(&nw))) leave("Can't open Window");
 SetMenuStrip(window,&menu);

/*
*  DWR: Following eight statements replace the original color setting.
*/

 SetRGB4(&screen->ViewPort,0, 0, 0, 0); /* Background, black */
 SetRGB4(&screen->ViewPort,1, 1, 5, 2); /* Floor, in shadow, dark green */
 SetRGB4(&screen->ViewPort,2, 2, 7, 1); /* Floor, in sun, light green */
 SetRGB4(&screen->ViewPort,3, 8, 8, 8); /* Side wall, in shadow, dark grey */
 SetRGB4(&screen->ViewPort,4,12,12,12); /* Side wall, in sun, light grey */
 SetRGB4(&screen->ViewPort,5,10,10,10); /* Front wall, in shadow, dark white */
 SetRGB4(&screen->ViewPort,6,14,14,14); /* Front wall, in sun, light white */
 SetRGB4(&screen->ViewPort,7, 8, 8,15); /* Sky, blue  */

 /* stuff needed by FloodFill, AreaFill */
 if(!(raster=(char *)AllocRaster(192,188))) leave("Can't allocate TmpRaster");
 InitTmpRas(&tmpras,raster,4512);
 rp->TmpRas=&tmpras;
 InitArea(&areainfo,areapts,7);
 rp->AreaInfo=&areainfo;
 BNDRYOFF(rp);

 for(i=0;i<3;i++) /* Draw status-display box */
 {
  SetAPen(rp,i+3);
  AreaMove(rp,190+5*i,12+5*i);
  AreaDraw(rp,319-5*i,12+5*i);
  AreaDraw(rp,319-5*i,125-5*i);
  AreaDraw(rp,319-5*i-5,125-5*i-5);
  AreaDraw(rp,319-5*i-5,12+i*5+5);
  AreaDraw(rp,190+5*i+5,12+i*5+5);
  AreaEnd(rp);
  SetAPen(rp,i+2);
  AreaMove(rp,190+5*i,12+5*i);
  AreaDraw(rp,190+5*i,125-5*i);
  AreaDraw(rp,319-5*i,125-5*i);
  AreaDraw(rp,319-5*i-5,125-5*i-5);
  AreaDraw(rp,319-5*i-5,12+i*5+5);
  AreaDraw(rp,190+5*i+5,12+i*5+5);
  AreaEnd(rp);
 }

 newmaze();

 for(;;)           /* main loop, wait for a message if not in demo-mode */
 {
  if (!demomode) WaitPort(window->UserPort);
  else demo();
  if(msg=(struct IntuiMessage*)GetMsg(window->UserPort))
  {
   demomode=FALSE;
   class=msg->Class;
   code=msg->Code;
   ReplyMsg((struct Message *)msg);
   switch(class)
   {
    case CLOSEWINDOW:
     leave("");
    case VANILLAKEY:
     if(y!=wayout || x!=xsize-3)
      switch(code) /* we check numbers 6/4/8, these are arrows on the */
      {            /* num pad keys, so we don't need RawKeyConvert */
       case '8':
        forward();
        break;
       case '4':
        left();
        break;
       case '6':
        right();
        break;
      }
     break;
    case MENUPICK:
      switch(ITEMNUM(code))
      {
       case 0:         /* map */
        showmaze();
        moves+=30;
        break;         /* New Maze */
       case 1:
        newmaze();
        break;
       case 2:
        if(SUBNUM(code)==mazesize) break;
        sizemenu[mazesize].Flags&=~CHECKED;
        mazesize=SUBNUM(code);
        sizemenu[mazesize].Flags|=CHECKED;
        newmaze();
        break;
       case 3:         /* Demo  */
        demomode=TRUE;
        break;
       case 4:         /* Quit  */
        leave("");
       case 5:         /* About */
	   	; /* Inserted ; to make Aztec happy -- PDS(1) -- 31-jul-88 */
      }
   }
  }
 }
}

void leave(error)
char *error;
{
 BPTR file;
                   /* Free Resouces & Exit */
 if(window)        CloseWindow(window);
 if(screen)        CloseScreen(screen);
 if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
 if(GfxBase)       CloseLibrary((struct Library *)GfxBase);
 if (mymaze)       FreeMem(mymaze,(xsize>>3)*ysize);
 if (raster)       FreeRaster(raster,192,188);

 if (*error && WorkBench &&
     (file=Open("con:20/70/400/60/Maze 3D Demo",MODE_OLDFILE)))
 {
  Write(file,error,strlen(error));
  Delay(200L);
  Close(file);
 }
 else if (*error) printf("%s\n",error);
 exit(0);
}

void newmaze()
{
 if (mymaze) FreeMem(mymaze,(xsize>>3)*ysize); /* Free old maze */
 xsize=32+mazesize*16;                         /* get new dimension */
 ysize=12+mazesize*12;
 if (!(mymaze=(char*) AllocMem((xsize>>3)*ysize,MEMF_CHIP|MEMF_CLEAR)))
     leave("Out of chip memory");              /* allocate new one */
 SetAPen(rp,6);                                /* clear status display */
 RectFill(rp,205,27,304,110);
 SetAPen(rp,1);
 statusline(10,"Moves:");
 statusline(30,"Looking");
 statusline(60,"Size:");
 sprintf(buffer,"%ld*%ld",xsize,ysize);
 statusline(70,buffer);

 maze(xsize,ysize,mymaze);                     /* genarate a new maze */
 seed = FastRand(seed);
 y= ((seed & 0x7ffe) % (ysize-4)+2) & 0x7ffe;  /* random start point */
 x= 2;
 seed = FastRand(seed);
 wayout = ((seed & 0x7ffe) % (ysize-4)+2) & 0x7ffe;  /* random exit */
 MAZESET(xsize-3,wayout,xsize,mymaze);               /* open exit */
 direction=1;                                    /* heading right (east) */
 statusline(40,heading[direction]);
 moves=0;
 show3d();
 showmaze();
}

void statusline(y,text)
short y;
char *text;
{                                  /* Write text, centered, into */
 Move(rp,255-strlen(text)*4,27+y); /* status-display */
 SetAPen(rp,1);
 SetBPen(rp,6);
 Text(rp,text,strlen(text));
}

void showmaze()                   /* show the maze */
{
 mazeimage.Width=xsize;           /* we define a Image-structure on our */
 mazeimage.Height=ysize;          /* bitmapped maze */
 mazeimage.ImageData=(short*)mymaze;
 DrawImage(rp,&mazeimage,255-(xsize/2),127);
                                               /* wait for a message */
 while(! window->UserPort->mp_MsgList.lh_Head->ln_Succ)
 {
  SetAPen(rp,++i&7);                        /* exit and current position */
  WritePixel(rp,255-(xsize/2)+x,127+y);     /* are blinking */
  WritePixel(rp,253-(xsize/2)+xsize,127+wayout);
  Delay(6L);
 }
 SetAPen(rp,0);
 RectFill(rp,191,127,319,199);
}


/*
*  DWR: The original 3d function is replaced by the one
*  at the end of this file..
*/


void demo()    /* demo mode: follow the left-hand wall until we get to */
{              /* the exit */
 long delay;
               /* be fast if the exit is far away, slow if it gets nearer */
 delay=19-18*(xsize-3-x+((wayout-y)<0?-(wayout-y):(wayout-y)))/
          ((wayout<ysize/2? ysize-wayout:wayout)+xsize-3);

 if(y==wayout && x==xsize-3) /* did we reach the exit ?*/
 {
  demomode=FALSE;            /* stop if we did */
  return;
 }                           /* is it possible to turn left ? */
 if(MAZETEST(x+dirincs[(direction-1)&3][0],y+dirincs[(direction-1)&3][1],
             xsize,mymaze))
 {
  left();                    /* yes, turn left */
  Delay(delay);
  forward();                 /* and one step forward */
 }
 else if(!forward()) right(); /* if not try to move forward, if everything */
                              /* fails, turn right */
 Delay(delay);
}

BOOL forward()                /* move one step forward if possible */
{
 if(!(MAZETEST(x+dirincs[direction][0],
             y+dirincs[direction][1],xsize,mymaze))) return(FALSE);

 x+=dirincs[direction][0];
 y+=dirincs[direction][1];
 moves++;
 sprintf(buffer," %ld/%ld ",
         moves,xsize-3-x+((wayout-y)<0?-(wayout-y):(wayout-y)));
 statusline(20,buffer);
 show3d();

 if(y==wayout && x==xsize-3)  /* has the exit been reached ?*/
 {
  SetAPen(rp,1);              /* yes, write some stupid text */
  SetBPen(rp,7);
  Move(rp,58,102);  /*  DWR: changed  */
  Text(rp,"Solved in",9);
  SetAPen(rp,6);
  SetBPen(rp,2);  /*  DWR: changed  */
  sprintf(buffer,"%ld moves.",moves);
  Move(rp,94-strlen(buffer)*4,116);
  Text(rp,buffer,strlen(buffer));
 }
 return(TRUE);
}

void left()                 /* turn left */
{
 direction=(--direction) & 3;
 show3d();
 statusline(40,heading[direction]);
}

void right()                /* turn right */
{
 direction=(++direction) & 3;
 show3d();
 statusline(40,heading[direction]);
}


/*
 *  DWR: The new 3d fucntion and its support.
 *  The basic form of the display is a number of (not drawn) paralel
 *  squares.
 *  Starting at the middle of the display (roughly 200*200) the
 *  smallest square has a side of one dot. The next square two dots.
 *  This increments via 4, 8, 16, 32, 64 to 128 dots.  This way
 *  we can see 8 squares deep.
 *  By drawing diagonals we separate walls from the floor and the
 *  sky.  When there is a cross-passage, we donot draw these
 *  diagonals but extent the square in the direction of the cross
 *  path until we reach the nearer square.
 *  The shadows are based on a sun at 60 degrees above the horizon
 *  and in the south-west (3 o'clock).  This way we have shadow-patterns
 *  which can be expressed in the number system of the squares. (You
 *  remember the 30-60-90 triangle?)
 *  The order of drawing is chosen to mask away some bad drawn shadows
 *  and irregularities in conjunction with edges.
 */


#define MAXDEPTH 8	/* 8 squares deep				*/

#define SHADOW 1	/* The value you must subtract from a		*/
			/* wall or floor to get it shaded		*/
#define SKY   7
#define FLOOR 2		/* 2 -> light green, 1 -> dark green		*/
#define SIDE  4		/* 4 -> light grey,  3-> dark grey		*/
#define FRONT 6		/* 6 -> light white, 5-> dark white (grey of course) */

#define HMID 94		/* The horizontal middle of the display		*/
#define VMID 105	/* The vertical middle				*/
			/* This was 106, which was wrong		*/

#define LEFT -1		/* Factors which define the direction		*/
#define RIGHT 1		/* from the horizontal middle			*/


short Shadow[]={ 0,0,SHADOW,SHADOW }; /* Change this to reposition the sun */


void show3d()
{

  short Side;		/* Can be "LEFT" or "RIGHT"			*/
  short Wall;		/* Actual distance we can see			*/
  short Depth;		/* Level we are drawing in			*/
  short Inner;		/* Size of the wall in the square we are	*/
  short Outer;		/* Size of the wall one level nearer		*/
  short Middle;		/* Size of the wall between those two		*/
  short InMid;		/* Size of shadow far from us			*/
  short OutMid;		/* Size of shadow near to us			*/
  short Minim;		/* The minimal size we use with drawing shadows	*/
  short MazeExit;	/* Flag set if we are looking at the exit	*/

/*
  WaitBOVP(rp);
*/

 /*
  *  First we draw the pampas.  It consists of floor and
  *  the sky, separated by the horizon.
  *  All floor under the horizon is light green (in the sun),
  *  and all above is blue sky.
  */

  SetAPen(rp,SKY);
  RectFill(rp,0,VMID-HMID,2*HMID,VMID);
  SetAPen(rp,FLOOR);
  RectFill(rp,0,VMID+1,2*HMID,VMID+HMID);


 /*
  *  Find out how far we can see in the maze.
  *  That is howmuch squares is the opposite wall
  *  from us away.  If further than we can draw, this
  *  value represents the deepest square we can draw (8).
  */
  
  for (Depth=1; Depth<MAXDEPTH; Depth++){
    if (!(MAZETEST(x+Depth*dirincs[direction][0],
                   y+Depth*dirincs[direction][1],
                   xsize,mymaze)))
    break;
  }
  
 /*
  *  Keep this value for later when we draw the opposite wall.
  *  Determine the smallest value we use with drawing shades.
  *  And perhaps we are looking at the exit.
  */

  Wall=Depth-1;
  Minim=1<<((MAXDEPTH-1)-Depth);
  MazeExit= ((y==wayout)&&((x+Wall*dirincs[direction][0])==xsize-2));
    
 /*
  *  Draw first the left side of the view, followed
  *  by the right side.
  */
  
  for (Side=LEFT; Side<=RIGHT; Side+=2){

   /*
    *  Loop for drawing in all squares, beginning at
    *  the farrest level, ending at our present position.
    */
    
    for (Depth=Wall; Depth>=0; Depth--){ 

     /*
      *  Detemine some values we use in this square.
      */
      
      Inner=1<<((MAXDEPTH-2)-Depth);  /* Half size of this square          */
      Outer=Inner<<1;                 /* Half size on one square nearer    */
      Middle=(Outer+Inner)/2;         /* The same size of a square between */
      if (Outer>HMID) Outer=HMID;     /* If too near (larger than display  */
      if (Middle>HMID) Middle=HMID;   /* clip it to a useful size          */

     /*
      *  Are we dealing with a wall or a cross-passage?
      */
      
      if (!IsPassage(Side,Depth)){

       /*
        *  We draw a wall and its shadow if visable.
        *  The shadow is visable if the surface of the
        *  wall is in the shadow.
        */

        if (Shadow[(direction+Side)&3]){

         /*  The shadow is on our path, now is
          *  it directed toward us or from us away?
          */
          
          if (Shadow[direction]){          /*  To us    */
            InMid=Middle;
            OutMid=Middle<<1;
            if (OutMid>HMID) OutMid=HMID;
          } else{                          /*  From us  */
            InMid=Middle/2+1;
            if (InMid<Minim) InMid=Minim;
            OutMid=Middle;
          }

         /*
          *  Draw this shadow of the side wall on out path
          */
          
          SetAPen(rp,FLOOR-SHADOW);
          AreaMove(rp,HMID,VMID+InMid);
          AreaDraw(rp,HMID,VMID+OutMid);
          AreaDraw(rp,HMID+Side*Outer,VMID+Outer); 
          AreaDraw(rp,HMID+Side*Inner,VMID+Inner); 
          AreaEnd(rp);
        }
        /*  End of drawing shadow on our path  */

       /*
        *  Draw the side wall itself using the color of
        *  a side wall, but modify it by a possible
        *  shade correction.
        */
        
        SetAPen(rp,SIDE-Shadow[(direction+Side)&3]);
        AreaMove(rp,HMID+Side*Inner,VMID-Inner); 
        AreaDraw(rp,HMID+Side*Inner,VMID+Inner); 
        AreaDraw(rp,HMID+Side*Outer,VMID+Outer); 
        AreaDraw(rp,HMID+Side*Outer,VMID-Outer); 
        AreaEnd(rp);

       /*
        *  If we are the first time in this loop,
        *  and the opposite wall is in the shadow,
        *  and the just drawn side wall is in the sun,
        *  draw the shadow from the opposite wall on it.
        */
        
        if ((Depth==Wall)&& Shadow[direction]&& !Shadow[(direction+Side)&3]){
          SetAPen(rp,SIDE-SHADOW);
          AreaMove(rp,HMID+Side*Inner,VMID-Inner);
          AreaDraw(rp,HMID+Side*Inner,VMID+Inner);
          AreaDraw(rp,HMID+Side*Middle,VMID+Middle);
          AreaEnd(rp);
        }
      }
      /*  End of dealing with a side wall */
      
      else{
      
       /*
        *  Here we are dealing with a cross passage,
        *  We first draw some shadows on the floor,
        *  as well on our path, as on the cross passage.
        */
        
        SetAPen(rp,FLOOR-SHADOW);

       /*
        *  Is the shadow in the direction of our path?
        */
         
        if (Shadow[(direction+Side)&3]){
          
         /*
          *  The shadow is in the direction of our path,
          *  now is it directed from us away?
          */
          
          if (Shadow[direction]){
            
           /*
            *  The crosswall is in the shade.
            *  Draw the shadow on the coss path.
            *  The distiction between the sides is to meet
            *  the RectFill() form.
            */
            
            if (Side==LEFT) 
              RectFill(rp,HMID-Outer,VMID+Inner,HMID,VMID+Middle);
            else
              RectFill(rp,HMID,VMID+Inner,HMID+Outer,VMID+Middle);
          }
          else{
          
           /*
            *  The cross wall is in the sun.
            *  Draw the shadow of the other side.
            */
            
            if (Side==LEFT) 
              RectFill(rp,HMID-Outer,VMID+Middle,HMID,VMID+Outer);
            else
              RectFill(rp,HMID,VMID+Middle,HMID+Outer,VMID+Outer);
          }
        }
        /*  End of shadow directed to our path  */

        else{
          
         /*
          *  The shadow is directed from our path away.
          *  Now draw the shadow on the cross path if
          *  the cross wall is in the shade.
          */
          
          if (Shadow[direction]){
            AreaMove(rp,HMID+Side*Inner,VMID+Inner);
            AreaDraw(rp,HMID+Side*Outer,VMID+Inner);
            AreaDraw(rp,HMID+Side*Outer,VMID+(Inner+Outer)/2);
            AreaEnd(rp);
          }
        }

       /*
        *  If we are here for the first time, 
        *  and the opposite wall is visable,
        *  and it is in the shade,
        *  extent the shadow of the opposite wall to
        *  the one from the cross wall. (A tiny triangle)
        */

        if ((Depth==Wall)&&(Depth<MAXDEPTH)&&(Shadow[direction])){
          AreaMove(rp,HMID+Side*Inner,VMID+Inner);
          AreaDraw(rp,HMID+Side*Middle,VMID+Middle);
          AreaDraw(rp,HMID+Side*Outer,VMID+Middle);
          AreaDraw(rp,HMID+Side*Outer,VMID+Inner);
          AreaEnd(rp);
        }

       /*
        *  If we are not dealing with the exit's cross path
        *  draw the cross wall itself.
        */
        
        if (!(MazeExit&&(Depth>(Wall-1)))){
           SetAPen(rp,FRONT-Shadow[direction]);
          if (Side==RIGHT)
            RectFill(rp,HMID+Inner,VMID-Inner,HMID+Outer,VMID+Inner);
          else 
            RectFill(rp,HMID-Outer,VMID-Inner,HMID-Inner,VMID+Inner);
        }

      }
      /*  End of drawing a cross passage (or side wall)  */
    }
    /*  End of drawing in subsequent squares  */
  }
  /*  End of drawing the left and right side ot the picture  */
  
 /*
  *  Now we have to draw the opposite wall
  *  or the exit if not too far away
  */

  Depth=Wall;

  if (Depth<MAXDEPTH-1){
    
   /*
    *  It is not too far away.
    *  Fill in again those sizes.
    */
    
    Inner=1<<((MAXDEPTH-2)-Depth);
    Outer=Inner<<1;
    Middle=(Outer+Inner)/2;
    if (Outer>HMID) Outer=HMID;
    if (Middle>HMID) Middle=HMID;

   /*
    *  Are we not looking at the exit?
    */
    
    if (!MazeExit){
      
     /*
      *  We are looking at an opposite wall.
      *  Draw its shadow on the floor if it is in the shade.
      */
      
      if (Shadow[direction]){
        SetAPen(rp,FLOOR-SHADOW);
        AreaMove(rp,HMID+1-Inner,VMID+Inner);
        AreaDraw(rp,HMID+1-Middle,VMID+Middle);
        AreaDraw(rp,HMID-1+Middle,VMID+Middle);
        AreaDraw(rp,HMID-1+Inner,VMID+Inner);
        AreaEnd(rp);
      }
      
     /*
      *  Draw the opposite wall itself.
      */
      
      SetAPen(rp,FRONT-Shadow[direction]);
      RectFill(rp,HMID-Inner,VMID-Inner,HMID+Inner,VMID+Inner);

     /*
      *  If the opposite wall is bounded to a side wall,
      *  and this side wall is in the shadow,
      *  and the opposite wall is in the sun,
      *  draw the shadow of the side wall on the opposite wall.
      */
      
      for (Side=LEFT; Side<=RIGHT; Side+=2){
        if (
            (!IsPassage(Side,Depth))&&
            (Shadow[(direction+Side)&3])&&
            (!Shadow[direction])
           ){
          SetAPen(rp,FRONT-SHADOW);
          AreaMove(rp,HMID+Side*Inner,VMID-Inner);
          AreaDraw(rp,HMID+Side*Inner,VMID+Inner);
          AreaDraw(rp,HMID,VMID+Inner);
          AreaEnd(rp);
        }
      }
    }
  }
}



/*
 *  This was original in line, but it disturbed the view over the
 *  code.  Placing it in a function made the clearity of the show3d()
 *  function better.
 *  This function returns the presence of a cross passage at <Side>'s
 *  side and <Depth> from us away.
 */

int IsPassage(Side,Depth)
short Side,Depth;
{
  register int xx,yy;
  xx=x+dirincs[(direction+Side)&3][0]+Depth*dirincs[direction][0];
  yy=y+dirincs[(direction+Side)&3][1]+Depth*dirincs[direction][1];
  return(MAZETEST(xx,yy, xsize,mymaze));
}

