#include <aros/oldprograms.h>
#include "cube2.h"


void feed(process)
 char *process;
{
 static int side=6,direc=0,ori=0;

   do{
      switch(*process){
	 case 'o' :ori=1;
		   break;
	 case '2' :direc=2;
		   break;
	 case 'a' :if(ori==1)
		      side=3;
		   else
		      direc=1;
		   break;
	 case 'u' :if(ori==1)
		      side=4;
		   else
		      side=0;
		   break;
	 case 'f' :side=1;
		   break;
	 case 'l' :if(ori==1)
		      side=0;
		   else
		      side=2;
		   break;
	 case 'b' :side=3;
		   break;
	 case 'r' :if(ori==1)
		     side=5;
		   else
		     side=4;
		   break;
	 case 'd' :if(ori==1)
		     side=2;
		   else
		     side=5;
		   break;
	 case 'c' :if(ori==1)
		     side=1;
		   break;
	 default  :break;
	 }
	 MyWrite(process);

	 if(ori==1 && side<6){
	     orient(side);
	     if(single && !best)
		Keywait();
	     ori=0;
	     side=6;
	     direc=0;
	 }
	 if(ori==0 && side<6){
	    rotate(side,direc);
	    side=6;
	    direc=0;
	    ori=0;
	    if(single && !best)
	      Keywait();
	    total++;
	 }
   }while(*process++);
   if(wait && !best)
      Keywait();
}

void Keywait()
{
 ULONG class,menunum,menuitem;	/* copying 20 bytes. IntuiMessage is 52  */
 USHORT code,id;		/* It looks about the same.  If you copy */
 SHORT mousex,mousey;		/* the whole message, you still have to  */
 struct Gadget *addr;		/* recopy part of the copy */

   buffer=0;
   Wait(1<<Wdw->UserPort->mp_SigBit);

   while(msg=(struct IntuiMessage*)GetMsg(Wdw->UserPort))
   {
      class=msg->Class;
      code=msg->Code;
      mousex=msg->MouseX;
      mousey=msg->MouseY;
      addr=(struct Gadget*)msg->IAddress;

      ReplyMsg((struct IntuiMessage*)msg);

	 switch(class){
	    case VANILLAKEY  :buffer++;
			      if(buffer>BUFSIZE-1)
				 buffer=BUFSIZE-1;
			      key[buffer]=code;
			      break;
	    case MENUPICK    :menunum=MENUNUM(code);
			      menuitem=ITEMNUM(code);
			      if(menunum==0)
				 DoMenu(menuitem);
			      break;
	   case MOUSEBUTTONS :ProcMouse(mousex,mousey);
			      break;
	   case GADGETDOWN   :procgads(addr);
			      break;
	   default	     :break;
	 }
   }
}

void MyWrite(letter)
 char *letter;
{
   if(best) return;

   if(*letter){
      Text(rp2,letter,1);
      if(rp2->cp_x>620){
	 yp+=9;
	 if(yp>61){
	    yp=61;
	    ScrollRaster(rp2,0,9,0,0,640,80);
	  }
	 Move(rp2,15,yp);
      }
   }
}

void rotate(side,direction)
 int side,direction;
{
 int j,i;
 char *source,*dest;

   if(direction==2){
      direction=0;
      rotate(side,direction);
   }
   if(!best)
      (*pf[side])(direction);

   for(j=0;j<4;j++){        /* move 3 pieces per side over to next side */
      source=arr1_pointer+pointer_arr[side][j];

      if(!direction){
	 if(j<3)
	    dest=arr2_pointer+pointer_arr[side][j+1];
	 else
	    dest=arr2_pointer+pointer_arr[side][j-3];
      }
      else{
	 if(j<1)
	    dest=arr2_pointer+pointer_arr[side][j+3];
	 else
	    dest=arr2_pointer+pointer_arr[side][j-1];
      }
      for(i=0;i<3;i++){
	 *dest++=*source++;
	 if(*source>15)
	    source-=8;
	 if(*dest>15)
	    dest-=8;
      }
   }
   if(!direction){            /* do surface move 8 pieces */
      for(j=1;j<3;j++)
	 arr2[side][j]=arr1[side][j+6];
      for(;j<9;j++)
	 arr2[side][j]=arr1[side][j-2];
   }
   else{
      for(j=1;j<7;j++)
	 arr2[side][j]=arr1[side][j+2];
      for(;j<9;j++)
	 arr2[side][j]=arr1[side][j-6];
   }
   update(0);
}

void update(gadget)
 int gadget;
{
 char *p1=&arr1[0][0];
 char *p2=&arr2[0][0];
 char j=0;

   if(!best && hesitate && !gadget)
      Delay(hesitate);

   while(j++<60)
      *p1++=*p2++;

   if(!best)
      DrawHome();
   if(!best && hesitate && !gadget)
      Delay(hesitate);

}

void orient(who)               /* if who is between 0 and 5 direction has */
 int who;		  /* already been selected else goto keyboard */
{
 int j,i,x;
 char *source,*dest;
 char opp,a,b,c,d,side;

      if(who>5){
	 do
	 {
	    if(buffer==0)
	       Keywait();
	    for(x=0;x<buffer;x++)
	       key[x]=key[x+1];
	    if(buffer>0)
	       buffer--;
	    switch(key[0]){
	       case'l':side=0;
		       break;
	       case'c':side=1;
		       break;
	       case'd':side=2;
		       break;
	       case'a':side=3;
		       break;
	       case'u':side=4;
		       break;
	       case'r':side=5;
		       break;
	       default:side=6;
		       break;
	    }
	 }while(side>5);
      MyWrite(key);
      }
      else
	 side=(char)who;

      if(!best)
	 (*op[side])(side);

      for(j=0;j<4;j++){               /* rotate sides all eight pieces */
       source=arr1_pointer+pointer_arr[side][j];

       if(j<3)
	 dest=arr2_pointer+pointer_arr[side][j+1];
       else
	 dest=arr2_pointer+pointer_arr[side][j-3];

       for(i=0;i<8;i++){
	  *dest++=*source++;
	  if(*source>15)
	     source-=8;
	  if(*dest>15)
	     dest-=8;
	}
      }
      for(j=1;j<3;j++)                /* rotate one side clockwise */
	 arr2[side][j]=arr1[side][j+6];
      for(;j<9;j++)
	 arr2[side][j]=arr1[side][j-2];

      opp=opposite[side];

      for(j=1;j<7;j++)                /* rotate the opposite anticlockwise */
	 arr2[opp][j]=arr1[opp][j+2];
      for(;j<9;j++)
	 arr2[opp][j]=arr1[opp][j-6];

      a=orient_ptrs[side][0];	      /* rotate center spots */
      b=orient_ptrs[side][1];
      c=orient_ptrs[side][2];
      d=orient_ptrs[side][3];

      arr2[0][a]=arr1[0][d];
      arr2[0][b]=arr1[0][a];
      arr2[0][c]=arr1[0][b];
      arr2[0][d]=arr1[0][c];

      MyWrite(" ");
      update(0);
      if(dided)
	 ColorGads();
}

StartScreens()
{
 int i;

   HBM=&hbm;
   HRP=&hrp;

   if(!(IntuitionBase=(struct IntuitionBase*)
		    OpenLibrary("intuition.library",0)))   finish();
   if(!(GfxBase=(struct GfxBase*)OpenLibrary("graphics.library",0)))
	 finish();
   if(!(NewWdw.Screen=CustScr=(struct Screen*)OpenScreen(&NewCustScr)))
	 finish();
   if(!(Wdw=(struct Window*)OpenWindow(&NewWdw)))
	 finish();
   if(!(PrintWdw.Screen=PrtScr=(struct Screen*)OpenScreen(&PrintScr)))
	 finish();
   if(!(Wdw2=(struct Window*)OpenWindow(&PrintWdw)))
	 finish();

   if(!(FontPtr=(struct TextFont*)OpenFont(&SysFont)))
	 finish();
   SetFont(rp2,FontPtr);
   SetFont(rp,FontPtr);

   SetMenuStrip(Wdw,FirstMenu);

   WVP=(struct ViewPort*)ViewPortAddress(Wdw);
   LoadRGB4(WVP,colorcode,8);
   WVP2=(struct ViewPort*)ViewPortAddress(Wdw2);
   LoadRGB4(WVP2,colormap2,4);

   InitBitMap(HBM,DEPTH,WIDTH,HEIGHT);

   InitRastPort(HRP);

   HRP->BitMap=HBM;

   for(i=0;i<DEPTH;i++){
      HBM->Planes[i]=NULL;
   }
   for(i=0;i<DEPTH;i++){
      HBM->Planes[i]=(PLANEPTR)AllocRaster(WIDTH,HEIGHT);
	  if(HBM->Planes[i]==NULL)
	     finish();
   }
   SetRast(HRP,0);
   SetAPen(rp,7);
   SetOPen(rp,7);
   SetAPen(HRP,7);
   SetOPen(HRP,7);
   SetAPen(rp2,2);
   SetOPen(rp2,2);

   Move(rp,0,0);      /* draw border */
   Draw(rp,319,0);
   Draw(rp,319,139);
   Draw(rp,0,139);
   Draw(rp,0,0);
   Move(rp,2,2);
   Draw(rp,317,2);
   Draw(rp,317,137);
   Draw(rp,2,137);
   Draw(rp,2,2);
}

DrawHome()
{
 int a,b,x,y;

   SetRast(HRP,0);
   for(x=0;x<9;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }

   DrawBord.Count=32;
   DrawBord.XY=HDraw;
   DrawBorder(HRP,&DrawBord,40,17);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

DrawR(direction)
 int direction;
{
 int x,y,a,b;

   SetRast(HRP,0);
   for(x=4;x<9;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
   }

   SetAPen(HRP,arr1[0][0]);
   a=Homeblit[0][0];
   b=Homeblit[1][0];
   BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
   SetAPen(HRP,arr1[1][0]);
   a=Homeblit[2][0];
   b=Homeblit[3][0];
   BltPattern(HRP,PaintF,a,b,a+32,b+23,4);

   if(direction==1){
      SetAPen(HRP,arr1[0][1]);
      BltPattern(HRP,PaintLRUpper,88,15,126,36,6);
      SetAPen(HRP,arr1[0][3]);
	 BltPattern(HRP,PaintLRUpper,78,61,116,82,6);
      SetAPen(HRP,arr1[0][2]);
      BltPattern(HRP,PaintLRUpper,83,38,121,59,6);
      SetAPen(HRP,arr1[1][1]);
      BltPattern(HRP,PaintLRLower,78,84,116,94,6);
      SetAPen(HRP,arr1[1][2]);
      BltPattern(HRP,PaintLRLower,84,96,122,106,6);
      SetAPen(HRP,arr1[1][3]);
      BltPattern(HRP,PaintLRLower,90,108,128,118,6);
  }
  else{
      SetAPen(HRP,arr1[1][1]);
      BltPattern(HRP,PaintLRUpper,88,15,126,36,6);
      SetAPen(HRP,arr1[1][2]);
	 BltPattern(HRP,PaintLRUpper,83,38,121,59,6);
      SetAPen(HRP,arr1[1][3]);
      BltPattern(HRP,PaintLRUpper,78,61,116,82,6);
      SetAPen(HRP,arr1[5][1]);
      BltPattern(HRP,PaintLRLower,78,84,116,94,6);
      SetAPen(HRP,arr1[5][2]);
      BltPattern(HRP,PaintLRLower,84,96,122,106,6);
      SetAPen(HRP,arr1[5][3]);
      BltPattern(HRP,PaintLRLower,90,108,128,118,6);
   }
   for(x=0;x<9;x++){             /* do rh surface */
      SetAPen(HRP,arr1[4][x]);
      if(direction==1){
	 a=RSurfaceAnti[0][x];
	 b=RSurfaceAnti[1][x];
      }
      else{
	 a=RSurfaceClock[0][x];
	 b=RSurfaceClock[1][x];
      }
      BltPattern(HRP,PaintRSurface,a,b,a+10,b+33,2);
   }
   SetAPen(HRP,7);
   x=40;
   y=17;
   Move(HRP,x,y);
   Draw(HRP,x-24,y+24);
   Draw(HRP,x-24,y+99);
   Draw(HRP,x+8,y+99);
   Draw(HRP,x+8,y+24);
   Draw(HRP,x+32,y);
   Draw(HRP,x,y);
   Move(HRP,x-8,y+8);
   Draw(HRP,x+50,y+8);
   Move(HRP,x-16,y+16);
   Draw(HRP,x+48,y+16);
   Move(HRP,x-24,y+24);
   Draw(HRP,x+40,y+24);
   Move(HRP,x+32,y);
   Draw(HRP,x+50,y);
   Move(HRP,x+47,y+17);
   Draw(HRP,x+40,y+24);
   Draw(HRP,x+40,y+50);
   Move(HRP,x+40,y+72);
   Draw(HRP,x+40,y+99);
   Draw(HRP,x+8,y+99);
   Move(HRP,x-24,y+49);
   Draw(HRP,x+40,y+49);
   Move(HRP,x-24,y+74);
   Draw(HRP,x+40,y+74);
   Move(HRP,x+40,y+99);
   Draw(HRP,x+49,y+90);
		   /* end of stationary part */
   Move(HRP,x+37,y+66);
   Draw(HRP,x+69,y+66);
   Draw(HRP,x+84,y-3);
   Draw(HRP,x+52,y-3);
   Draw(HRP,x+37,y+66);
   Draw(HRP,x+55,y+102);
   Draw(HRP,x+87,y+102);
   Draw(HRP,x+69,y+66);
   Move(HRP,x+87,y+102);
   Draw(HRP,x+102,y+33);
   Draw(HRP,x+84,y-3);
   Move(HRP,x+47,y+20);
   Draw(HRP,x+79,y+20);
   Draw(HRP,x+97,y+56);
   Move(HRP,x+42,y+43);
   Draw(HRP,x+74,y+43);
   Draw(HRP,x+92,y+79);
   Move(HRP,x+43,y+78);
   Draw(HRP,x+75,y+78);
   Draw(HRP,x+90,y+9);
   Move(HRP,x+49,y+90);
   Draw(HRP,x+81,y+90);
   Draw(HRP,x+96,y+21);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

DrawL(direction)
 int direction;
{
 int x,y,a,b;

   SetRast(HRP,0);

   if(direction==1){
      SetAPen(HRP,arr1[1][7]);
      BltPattern(HRP,PaintLRUpper,24,15,62,36,6);
      SetAPen(HRP,arr1[1][6]);
      BltPattern(HRP,PaintLRUpper,19,38,57,59,6);
      SetAPen(HRP,arr1[1][5]);
      BltPattern(HRP,PaintLRUpper,14,61,49,82,6);
      SetAPen(HRP,arr1[5][7]);
      BltPattern(HRP,PaintLRLower,14,84,52,94,6);
      SetAPen(HRP,arr1[5][6]);
      BltPattern(HRP,PaintLRLower,20,96,58,106,6);
      SetAPen(HRP,arr1[5][5]);
      BltPattern(HRP,PaintLRLower,26,108,64,118,6);
   }
   else{
      SetAPen(HRP,arr1[0][7]);
      BltPattern(HRP,PaintLRUpper,24,15,62,36,6);
      SetAPen(HRP,arr1[0][6]);
      BltPattern(HRP,PaintLRUpper,19,38,57,59,6);
      SetAPen(HRP,arr1[0][5]);
      BltPattern(HRP,PaintLRUpper,14,61,49,82,6);
      SetAPen(HRP,arr1[1][7]);
      BltPattern(HRP,PaintLRLower,14,84,52,94,6);
      SetAPen(HRP,arr1[1][6]);
      BltPattern(HRP,PaintLRLower,20,96,58,106,6);
      SetAPen(HRP,arr1[1][5]);
      BltPattern(HRP,PaintLRLower,26,108,64,118,6);
   }
   for(x=0;x<5;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
   }
   SetAPen(HRP,arr1[0][8]);
   a=Homeblit[0][8];
   b=Homeblit[1][8];
   BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
   SetAPen(HRP,arr1[1][8]);
   a=Homeblit[2][8];
   b=Homeblit[3][8];
   BltPattern(HRP,PaintF,a,b,a+32,b+23,4);

   for(x=0;x<9;x++){
      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }
   x=40;
   y=17;
   SetAPen(HRP,7);
   Move(HRP,x+8,y+99);
   Draw(HRP,x+8,y+24);
   Draw(HRP,x+32,y);
   Draw(HRP,x+96,y);
   Draw(HRP,x+72,y+24);
   Draw(HRP,x+72,y+99);
   Draw(HRP,x+8,y+99);
   Move(HRP,x+96,y);
   Draw(HRP,x+96,y+75);
   Draw(HRP,x+72,y+99);
   Move(HRP,x+64,y);
   Draw(HRP,x+40,y+24);
   Draw(HRP,x+40,y+99);
   Move(HRP,x+8,y+49);
   Draw(HRP,x+72,y+49);
   Draw(HRP,x+96,y+25);
   Move(HRP,x+8,y+74);
   Draw(HRP,x+72,y+74);
   Draw(HRP,x+96,y+50);
   Move(HRP,x+16,y+16);
   Draw(HRP,x+80,y+16);
   Draw(HRP,x+80,y+91);
   Move(HRP,x+24,y+8);
   Draw(HRP,x+88,y+8);
   Draw(HRP,x+88,y+82);
   Move(HRP,x+8,y+24);
   Draw(HRP,x+72,y+24);
   /* end of stationary part */
   Move(HRP,x-27,y+66);
   Draw(HRP,x+5,y+66);
   Draw(HRP,x+8,y+52);
   Move(HRP,x+16,y+15);
   Draw(HRP,x+20,y-3);
   Draw(HRP,x-12,y-3);
   Draw(HRP,x-27,y+66);
   Draw(HRP,x-9,y+102);
   Draw(HRP,x+23,y+102);
   Draw(HRP,x+20,y+99);
   Move(HRP,x+5,y+66);
   Draw(HRP,x+8,y+72);
   Move(HRP,x+20,y-3);
   Draw(HRP,x+25,y+6);
   Move(HRP,x-15,y+90);
   Draw(HRP,x+8,y+90);
   Move(HRP,x-21,y+78);
   Draw(HRP,x+8,y+78);
   Move(HRP,x-22,y+43);
   Draw(HRP,x+8,y+43);
   Move(HRP,x-17,y+20);
   Draw(HRP,x+11,y+20);
   Move(HRP,x+23,y+102);
   Draw(HRP,x+25,y+99);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

DrawF(direction)
 int direction;
{
 int x,y,a,b;

   SetRast(HRP,0);

   for(x=6;x<9;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }
   for(x=0;x<3;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }
   for(x=0;x<9;x++){
      SetAPen(HRP,arr1[1][x]);
      if(direction==1){
	 a=FSurfaceAnti[0][x];
	 b=FSurfaceAnti[1][x];
      }
      else{
	 a=FSurfaceClock[0][x];
	 b=FSurfaceClock[1][x];
      }
      BltPattern(HRP,PaintFSurface,a,b,a+42,b+34,6);
   }
   if(direction==1){
      SetAPen(HRP,arr1[4][5]);
      BltPattern(HRP,PaintFUpper,68,17,96,41,4);
      SetAPen(HRP,arr1[4][4]);
      BltPattern(HRP,PaintFUpper,90,35,118,59,4);
      SetAPen(HRP,arr1[4][3]);
      BltPattern(HRP,PaintFUpper,112,53,140,77,4);
   }
   else{
      SetAPen(HRP,arr1[0][5]);
      BltPattern(HRP,PaintFUpper,68,17,96,41,4);
      SetAPen(HRP,arr1[0][4]);
      BltPattern(HRP,PaintFUpper,90,35,118,59,4);
      SetAPen(HRP,arr1[0][3]);
      BltPattern(HRP,PaintFUpper,112,53,140,77,4);
   }
   x=40;
   y=17;

   SetAPen(HRP,7);
   Move(HRP,x-16,y+16);
   Draw(HRP,x,y);
   Draw(HRP,x+96,y);
   Draw(HRP,x+80,y+16);
   Draw(HRP,x+55,y+16);
   Move(HRP,x+80,y+73);
   Draw(HRP,x+80,y+91);
   Draw(HRP,x+96,y+75);
   Draw(HRP,x+96,y+59);
   Move(HRP,x+96,y+48);
   Draw(HRP,x+96,y);
   Move(HRP,x-8,y+8);
   Draw(HRP,x+25,y+8);
   Move(HRP,x+46,y+8);
   Draw(HRP,x+88,y+8);
   Draw(HRP,x+88,y+42);
   Move(HRP,x+88,y+65);
   Draw(HRP,x+88,y+82);
   Move(HRP,x+96,y+25);
   Draw(HRP,x+83,y+38);
   Move(HRP,x+64,y);
   Draw(HRP,x+52,y+12);
   Move(HRP,x+80,y+91);
   Draw(HRP,x+56,y+91);
   Move(HRP,x-2,y+91);
   Draw(HRP,x-16,y+91);
   Draw(HRP,x-16,y+80);
   Move(HRP,x-16,y+42);
   Draw(HRP,x-16,y+16);
   Draw(HRP,x+16,y+16);
   Draw(HRP,x+32,y);
   Move(HRP,x+80,y+16);
   Draw(HRP,x+80,y+35);

   Move(HRP,x-39,y+61);
   Draw(HRP,x+27,y+7);
   Draw(HRP,x+93,y+61);
   Draw(HRP,x+27,y+115);
   Draw(HRP,x-39,y+61);
   Move(HRP,x+27,y+7);
   Draw(HRP,x+35,y-1);
   Draw(HRP,x+101,y+53);
   Draw(HRP,x+93,y+61);
   Move(HRP,x-17,y+43);
   Draw(HRP,x+49,y+97);
   Move(HRP,x+5,y+25);
   Draw(HRP,x+71,y+79);
   Move(HRP,x-17,y+79);
   Draw(HRP,x+49,y+25);
   Draw(HRP,x+57,y+17);
   Move(HRP,x+5,y+97);
   Draw(HRP,x+71,y+43);
   Draw(HRP,x+79,y+35);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

DrawB(direction)
 int direction;
{
 int x,y,a,b;

   SetRast(HRP,0);

   if(direction==1){
      SetAPen(HRP,arr1[0][7]);
      BltPattern(HRP,PaintFUpper,80,3,108,27,4);
      SetAPen(HRP,arr1[0][8]);
      BltPattern(HRP,PaintFUpper,102,21,130,45,4);
      SetAPen(HRP,arr1[0][1]);
      BltPattern(HRP,PaintFUpper,124,39,152,63,4);
   }
   else{
      SetAPen(HRP,arr1[4][7]);
      BltPattern(HRP,PaintFUpper,80,3,108,27,4);
      SetAPen(HRP,arr1[4][8]);
      BltPattern(HRP,PaintFUpper,102,21,130,45,4);
      SetAPen(HRP,arr1[4][1]);
      BltPattern(HRP,PaintFUpper,124,39,152,63,4);
   }
   for(x=2;x<7;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);

      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }
   SetAPen(HRP,arr1[0][0]);
   a=Homeblit[0][0];
   b=Homeblit[1][0];
   BltPattern(HRP,PaintU,a,b,a+48,b+6,6);

   for(x=0;x<9;x++){
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
   }
   SetAPen(HRP,arr1[4][0]);
   a=Homeblit[4][0];
   b=Homeblit[5][0];
   BltPattern(HRP,PaintR,a,b,a+7,b+29,2);

   x=40;
   y=17;
   SetAPen(HRP,7);
   Move(HRP,x-8,y+8);
   Draw(HRP,x+88,y+8);
   Draw(HRP,x+88,y+83);
   Draw(HRP,x+72,y+99);
   Draw(HRP,x-24,y+99);
   Draw(HRP,x-24,y+24);
   Draw(HRP,x-8,y+8);
   Move(HRP,x-16,y+16);
   Draw(HRP,x+80,y+16);
   Draw(HRP,x+80,y+91);
   Move(HRP,x-24,y+24);
   Draw(HRP,x+72,y+24);
   Draw(HRP,x+72,y+99);
   Move(HRP,x+24,y+8);
   Draw(HRP,x+8,y+24);
   Draw(HRP,x+8,y+99);
   Move(HRP,x+56,y+8);
   Draw(HRP,x+40,y+24);
   Draw(HRP,x+40,y+99);
   Move(HRP,x-24,y+49);
   Draw(HRP,x+72,y+49);
   Draw(HRP,x+88,y+33);
   Move(HRP,x-24,y+74);
   Draw(HRP,x+72,y+74);
   Draw(HRP,x+88,y+58);
   Move(HRP,x+72,y+24);
   Draw(HRP,x+88,y+8);
      /* end of stationary */
   Move(HRP,x+21,y+8);
   Draw(HRP,x+39,y-7);
   Draw(HRP,x+57,y+8);
   Move(HRP,x+88,y+33);
   Draw(HRP,x+105,y+47);
   Draw(HRP,x+88,y+63);
   Move(HRP,x+39,y-7);
   Draw(HRP,x+47,y-15);
   Draw(HRP,x+75,y+8);
   Move(HRP,x+88,y+19);
   Draw(HRP,x+113,y+39);
   Draw(HRP,x+105,y+47);
   Move(HRP,x+64,y+8);
   Draw(HRP,x+69,y+3);
   Move(HRP,x+88,y+24);
   Draw(HRP,x+91,y+21);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

DrawU(direction)
 int direction;
{
 int x,y,a,b;

   SetRast(HRP,0);

   for(x=2;x<7;x++){
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
   }
   SetAPen(HRP,arr1[1][0]);
   a=Homeblit[2][0];
   b=Homeblit[3][0];
   BltPattern(HRP,PaintF,a,b,a+32,b+23,4);

   for(x=1;x<5;x++){
      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }
   SetAPen(HRP,arr1[4][0]);
   a=Homeblit[4][0];
   b=Homeblit[5][0];
   BltPattern(HRP,PaintR,a,b,a+7,b+29,2);

   SetAPen(HRP,arr1[4][8]);
   a=Homeblit[4][8];
   b=Homeblit[5][8];
   BltPattern(HRP,PaintR,a,b,a+7,b+29,2);

   if(direction==1){
      for(x=0;x<9;x++){
	 SetAPen(HRP,arr1[0][x]);
	 a=USurfaceAnti[0][x];
	 b=USurfaceAnti[1][x];
	 BltPattern(HRP,PaintUSurface,a,b,a+40,b+11,6);
      }
      SetAPen(HRP,arr1[2][1]);
      BltPattern(HRP,PaintULeft,13,30,28,59,2);
      SetAPen(HRP,arr1[2][2]);
      BltPattern(HRP,PaintULeft,30,36,45,65,2);
      SetAPen(HRP,arr1[2][3]);
      BltPattern(HRP,PaintULeft,47,42,62,71,2);

      SetAPen(HRP,arr1[1][7]);
      BltPattern(HRP,PaintURight,64,42,88,71,4);
      SetAPen(HRP,arr1[1][8]);
      BltPattern(HRP,PaintURight,90,36,114,65,4);
      SetAPen(HRP,arr1[1][1]);
      BltPattern(HRP,PaintURight,116,30,140,59,4);
   }
   else{
      for(x=0;x<9;x++){
	a=USurfaceClock[0][x];
	b=USurfaceClock[1][x];
	SetAPen(HRP,arr1[0][x]);
	BltPattern(HRP,PaintUSurface,a,b,a+40,b+11,6);
      }
      SetAPen(HRP,arr1[1][7]);
      BltPattern(HRP,PaintULeft,13,30,28,59,2);
      SetAPen(HRP,arr1[1][8]);
      BltPattern(HRP,PaintULeft,30,36,45,65,2);
      SetAPen(HRP,arr1[1][1]);
      BltPattern(HRP,PaintULeft,47,42,62,71,2);

      SetAPen(HRP,arr1[4][5]);
      BltPattern(HRP,PaintURight,64,42,88,71,4);
      SetAPen(HRP,arr1[4][6]);
      BltPattern(HRP,PaintURight,90,36,114,65,4);
      SetAPen(HRP,arr1[4][7]);
      BltPattern(HRP,PaintURight,116,30,140,59,4);
   }
   x=40;
   y=17;
   SetAPen(HRP,7);
   Move(HRP,x+96,y+39);
   Draw(HRP,x+96,y+75);
   Draw(HRP,x+72,y+99);
   Draw(HRP,x-24,y+99);
   Draw(HRP,x-24,y+49);
   Draw(HRP,x+8,y+49);
   Move(HRP,x+47,y+49);
   Draw(HRP,x+72,y+49);
   Draw(HRP,x+80,y+41);
   Move(HRP,x-16,y+41);
   Draw(HRP,x-24,y+49);
   Move(HRP,x-24,y+74);
   Draw(HRP,x+72,y+74);
   Draw(HRP,x+96,y+50);
   Move(HRP,x+8,y+49);
   Draw(HRP,x+8,y+99);
   Move(HRP,x+40,y+52);
   Draw(HRP,x+40,y+99);
   Move(HRP,x+72,y+49);
   Draw(HRP,x+72,y+99);
   Move(HRP,x+80,y+41);
   Draw(HRP,x+80,y+91);
   Move(HRP,x+88,y+40);
   Draw(HRP,x+88,y+83);
   /* end of stationary */
   Move(HRP,x+50,y-6);
   Draw(HRP,x+101,y+12);
   Draw(HRP,x+23,y+30);
   Draw(HRP,x-28,y+12);
   Draw(HRP,x+50,y-6);
   Move(HRP,x-28,y+12);
   Draw(HRP,x-28,y+37);
   Draw(HRP,x+23,y+55);
   Draw(HRP,x+101,y+37);
   Draw(HRP,x+101,y+12);
   Move(HRP,x+23,y+30);
   Draw(HRP,x+23,y+55);
	      /* inside lines */
   Move(HRP,x-11,y+43);
   Draw(HRP,x-11,y+18);
   Draw(HRP,x+67,y);
   Move(HRP,x+6,y+49);
   Draw(HRP,x+6,y+24);
   Draw(HRP,x+84,y+6);
   Move(HRP,x+49,y+49);
   Draw(HRP,x+49,y+24);
   Draw(HRP,x-2,y+6);
   Move(HRP,x+75,y+43);
   Draw(HRP,x+75,y+18);
   Draw(HRP,x+24,y);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

DrawD(direction)
 int direction;
{
 int a,b,x,y;

   SetRast(HRP,0);

   if(direction==1){
      SetAPen(HRP,arr1[1][5]);
      BltPattern(HRP,PaintULeft,13,80,28,109,2);
      SetAPen(HRP,arr1[1][4]);
      BltPattern(HRP,PaintULeft,30,86,45,115,2);
      SetAPen(HRP,arr1[1][3]);
      BltPattern(HRP,PaintULeft,47,92,62,121,2);

      SetAPen(HRP,arr1[4][3]);
      BltPattern(HRP,PaintURight,64,92,88,121,4);
      SetAPen(HRP,arr1[4][2]);
      BltPattern(HRP,PaintURight,90,86,114,115,4);
      SetAPen(HRP,arr1[4][1]);
      BltPattern(HRP,PaintURight,116,80,140,109,4);
   }
   else{
      SetAPen(HRP,arr1[2][7]);
      BltPattern(HRP,PaintULeft,13,80,28,110,2);
      SetAPen(HRP,arr1[2][6]);
      BltPattern(HRP,PaintULeft,30,86,45,115,2);
      SetAPen(HRP,arr1[2][5]);
      BltPattern(HRP,PaintULeft,47,92,62,121,2);

      SetAPen(HRP,arr1[1][5]);
      BltPattern(HRP,PaintURight,64,92,88,121,4);
      SetAPen(HRP,arr1[1][4]);
      BltPattern(HRP,PaintURight,90,86,114,115,4);
      SetAPen(HRP,arr1[1][3]);
      BltPattern(HRP,PaintURight,116,80,140,109,4);
   }

   for(x=0;x<9;x++){
      SetAPen(HRP,arr1[0][x]);
      a=Homeblit[0][x];
      b=Homeblit[1][x];
      BltPattern(HRP,PaintU,a,b,a+48,b+6,6);
   }
   for(x=0;x<3;x++){
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
   }
   for(x=6;x<9;x++){
      SetAPen(HRP,arr1[1][x]);
      a=Homeblit[2][x];
      b=Homeblit[3][x];
      BltPattern(HRP,PaintF,a,b,a+32,b+23,4);
   }
   for(x=4;x<9;x++){
      SetAPen(HRP,arr1[4][x]);
      a=Homeblit[4][x];
      b=Homeblit[5][x];
      BltPattern(HRP,PaintR,a,b,a+7,b+29,2);
   }
   SetAPen(HRP,arr1[4][0]);
   a=Homeblit[4][0];
   b=Homeblit[5][0];
   BltPattern(HRP,PaintR,a,b,a+7,b+29,2);

   DrawBord.Count=53;
   DrawBord.XY=DDraw;

   DrawBorder(HRP,&DrawBord,40,17);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

Larry(direction)
 int direction;
{
 int a,b,x,y;

   SetRast(HRP,0);

   if(direction==5){
      for(x=0;x<9;x++){
	 SetAPen(HRP,arr1[0][x]);
	 a=USurfaceAnti[0][x];
	 b=USurfaceAnti[1][x];
	 BltPattern(HRP,PaintUSurface,a,b,a+40,b+11,6);
	 a=LarryR[0][x];
	 b=LarryR[1][x];
	 SetAPen(HRP,arr1[2][x]);
	 BltPattern(HRP,PaintULeft,a,b,a+15,b+29,2);
	 a=LarryR[2][x];
	 b=LarryR[3][x];
	 SetAPen(HRP,arr1[1][x]);
	 BltPattern(HRP,PaintURight,a,b,a+25,b+29,4);
      }
   }
   else{
      for(x=0;x<9;x++){
	 a=USurfaceClock[0][x];
	 b=USurfaceClock[1][x];
	 SetAPen(HRP,arr1[0][x]);
	 BltPattern(HRP,PaintUSurface,a,b,a+40,b+11,6);

	 a=LarryL[0][x];
	 b=LarryL[1][x];
	 SetAPen(HRP,arr1[1][x]);
	 BltPattern(HRP,PaintULeft,a,b,a+15,b+29,2);
	 a=LarryL[2][x];
	 b=LarryL[3][x];
	 SetAPen(HRP,arr1[4][x]);
	 BltPattern(HRP,PaintURight,a,b,a+25,b+29,4);
      }
   }
   SetAPen(HRP,7);
   DrawBord.Count=31;
   DrawBord.XY=OLRDraw;
   DrawBorder(HRP,&DrawBord,40,17);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}
Dura(direction)
 int direction;
{
 int a,b,x,y;

   SetRast(HRP,0);

   if(direction==2){
      for(x=0;x<9;x++){
	 SetAPen(HRP,arr1[4][x]);
	 a=RSurfaceAnti[0][x];
	 b=RSurfaceAnti[1][x];
	 BltPattern(HRP,PaintRSurface,a,b,a+10,b+33,2);
	 a=DuraA[0][x];
	 b=DuraA[1][x];
	 SetAPen(HRP,arr1[0][x]);
	 BltPattern(HRP,PaintLRUpper,a,b,a+37,b+21,6);
	 a=DuraA[2][x];
	 b=DuraA[3][x];
	 SetAPen(HRP,arr1[1][x]);
	 BltPattern(HRP,PaintLRLower,a,b,a+38,b+10,6);
      }
   }
   else{
      for(x=0;x<9;x++){
	 a=RSurfaceClock[0][x];
	 b=RSurfaceClock[1][x];
	 SetAPen(HRP,arr1[4][x]);
	 BltPattern(HRP,PaintRSurface,a,b,a+10,b+33,2);
	 a=DuraA[0][x];
	 b=DuraA[1][x];
	 SetAPen(HRP,arr1[1][x]);
	 BltPattern(HRP,PaintLRUpper,a,b,a+38,b+21,6);
	 a=DuraA[2][x];
	 b=DuraA[3][x];
	 SetAPen(HRP,arr1[5][x]);
	 BltPattern(HRP,PaintLRLower,a,b,a+38,b+10,6);
      }
   }
   SetAPen(HRP,7);
   DrawBord.Count=17;
   DrawBord.XY=OUDDraw;
   DrawBorder(HRP,&DrawBord,40,17);

   Move(HRP,23,37);
   Draw(HRP,119,37);
   Draw(HRP,137,73);

   Move(HRP,18,60);
   Draw(HRP,114,60);
   Draw(HRP,132,96);

   Move(HRP,19,95);
   Draw(HRP,115,95);
   Draw(HRP,130,26);

   Move(HRP,25,107);
   Draw(HRP,121,107);
   Draw(HRP,136,38);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

Cathy(direction)
 int direction;
{
 int a,b,x,y;

   SetRast(HRP,0);

   if(direction==3){
      for(x=0;x<9;x++){
	 SetAPen(HRP,arr1[1][x]);
	 a=FSurfaceAnti[0][x];
	 b=FSurfaceAnti[1][x];
	 BltPattern(HRP,PaintFSurface,a,b,a+42,b+34,6);
	 a=CathyA[0][x];
	 b=CathyA[1][x];
	 SetAPen(HRP,arr1[4][x]);
	 BltPattern(HRP,PaintFUpper,a,b,a+29,b+24,4);
      }
   }
   else{
      for(x=0;x<9;x++){
	 a=FSurfaceClock[0][x];
	 b=FSurfaceClock[1][x];
	 SetAPen(HRP,arr1[1][x]);
	 BltPattern(HRP,PaintFSurface,a,b,a+42,b+34,6);
	 a=CathyA[0][x];
	 b=CathyA[1][x];
	 SetAPen(HRP,arr1[0][x]);
	 BltPattern(HRP,PaintFUpper,a,b,a+29,b+24,4);
      }
   }

   SetAPen(HRP,7);
   DrawBord.Count=25;
   DrawBord.XY=OCADraw;
   DrawBorder(HRP,&DrawBord,0,0);

   WaitTOF();
   BltBitMapRastPort(HBM,0,0,rp,82,3,WIDTH,HEIGHT,0xc0);
}

finish()
{
 int i;

   LoadRGB4(WVP2,colormap3,4);
   MoveScreen(Wdw2->WScreen,0,-144);

   for(i=0;i<DEPTH;i++){
      if(HBM->Planes[i])
	 FreeRaster(HBM->Planes[i],WIDTH,HEIGHT);
   }
   if(Wdw)
      {
       if(Wdw->MenuStrip)
	  ClearMenuStrip(Wdw);
       CloseWindow(Wdw);
      }
   if(Wdw2)          CloseWindow(Wdw2);
   if(CustScr)       CloseScreen(CustScr);
   if(PrtScr)        CloseScreen(PrtScr);
   if(GfxBase)       CloseLibrary((struct Library*)GfxBase);
   if(IntuitionBase) CloseLibrary((struct Library*)IntuitionBase);
   if(FontPtr)       CloseFont(FontPtr);

   exit();
}

InitGadgets()
{
 struct Gadget *g;
 UWORD visiblelines,totallines,overlap,topline,hidden;
 UWORD value;
 char Red,Green,Blue;

   if(done)
      return;
   if(dided)
      CloseSelect();


   SetAPen(rp,1);
   SetOPen(rp,1);
   RectFill(rp,8,40,28,50);
   SetAPen(rp,2);
   SetOPen(rp,2);
   RectFill(rp,30,40,50,50);
   SetAPen(rp,3);
   SetOPen(rp,3);
   RectFill(rp,52,40,72,50);
   SetAPen(rp,4);
   SetOPen(rp,4);
   RectFill(rp,8,55,28,65);
   SetAPen(rp,5);
   SetOPen(rp,5);
   RectFill(rp,30,55,50,65);
   SetAPen(rp,6);
   SetOPen(rp,6);
   RectFill(rp,52,55,72,65);

   SetAPen(rp,0);
   SetBPen(rp,7);
   Move(rp,11,98);
   Text(rp," Close ",7);
   Move(rp,19,85);
   SetAPen(rp,7);
   SetBPen(rp,0);
   Text(rp,"Delay",5);

	SetAPen(rp,7);
	BltPattern(rp,Box,hor[1],vert[1],hor[1]+22,vert[1]+12,4);
      col=1;

      value=GetRGB4(WVP->ColorMap,1);
      Blue=value&0xf;
      Green=(value >>4) &0xf;
      Red=(value >>8) &0xf;

      NewModifyProp(&RedGad,Wdw,NULL,AUTOKNOB|FREEHORIZ,Red<<12,
	  NULL,0x1111,NULL,1L);
      NewModifyProp(&BlueGad,Wdw,NULL,AUTOKNOB|FREEHORIZ,Blue<<12,
	  NULL,0x1111,NULL,1L);
      NewModifyProp(&GreenGad,Wdw,NULL,AUTOKNOB|FREEHORIZ,Green<<12,
	  NULL,0x1111,NULL,1L);
      NewModifyProp(&delaygad,Wdw,NULL,AUTOKNOB|FREEHORIZ,hesitate<<10,
	  NULL,0x1800,NULL,1L);

   AddGList(Wdw,&Gad1,(SHORT)~0,31,NULL);
   RefreshGList(&Gad1,Wdw,NULL,-1);
   PrintRGB(Red,Green,Blue);
   done=1;
}

void DoMenu(item)
 ULONG	item;
{
   switch(item){
      case 0   :InitMouseMen();    /*mouse select */
		break;
      case 1   :InitGadgets();     /* palette */
		break;
      case 2   :clear();
		break;
      case 3   :clear();
		solve();
		break;
      case 4   :quick=1;	   /* quicksolve */
		clear();
		solve();
		quick=0;
		break;
      case 5   :savecube();        /* best of 24 */
		best=quick=1;
		clear();
		Best();
		best=quick=0;
		break;
      case 6   :recall();
		break;
      case 7   :break;		   /*info*/
      case 8   :finish();
		break;
      default  :break;
   }
}

ClosePal()
{
   RemoveGList(Wdw,&Gad1,-1);
   SetAPen(rp,0);
   SetOPen(rp,0);
   RectFill(rp,6,10,75,105);
   done=0;
   old=0;
}

void CloseSelect()
{
   RemoveGList(Wdw,&Arrow1,-1);
   SetAPen(rp,0);
   SetOPen(rp,0);
   RectFill(rp,245,4,310,133);
   dided=0;
}

ProcMouse(mousex,mousey)
 SHORT mousex,mousey;
{
 char Red,Green,Blue;
 UWORD value;

   if(mousex>11 && mousex<68 && mousey>91 && mousey<101)  /* close palette*/
      ClosePal();
   if(mousex>245 && mousex<302 && mousey>124 && mousey<134)  /*close rotate*/
       CloseSelect();

   if(done){
      if(mousey>40 && mousey<51){     /* pick up color */
	 if(mousex>8 && mousex<28)
	    col=1;
	 if(mousex>30 && mousex<50)
	    col=2;
	 if(mousex>52 && mousex<72)
	    col=3;
	 }
      if(mousey>55 && mousey<66){
	 if(mousex>8 && mousex<28)
	    col=4;
	 if(mousex>30 && mousex<50)
	    col=5;
	 if(mousex>52 && mousex<72)
	    col=6;
	 }
     if(col!=old){
	SetAPen(rp,0);
	BltPattern(rp,Box,hor[old],vert[old],hor[old]+22,vert[old]+12,4);
	SetAPen(rp,7);
	BltPattern(rp,Box,hor[col],vert[col],hor[col]+22,vert[col]+12,4);
      }
      old=col;

      value=GetRGB4(WVP->ColorMap,col);
      Blue=value&0xf;
      Green=(value >>4) &0xf;
      Red=(value >>8) &0xf;

      NewModifyProp(&RedGad,Wdw,NULL,AUTOKNOB|FREEHORIZ,Red<<12,
	  NULL,0x1111,NULL,1L);
      NewModifyProp(&BlueGad,Wdw,NULL,AUTOKNOB|FREEHORIZ,Blue<<12,
	  NULL,0x1111,NULL,1L);
      NewModifyProp(&GreenGad,Wdw,NULL,AUTOKNOB|FREEHORIZ,Green<<12,
	  NULL,0x1111,NULL,1L);

      PrintRGB(Red,Green,Blue);
   }
}

PrintRGB(Red,Green,Blue)
 char Red,Green,Blue;
{
 char buffy[4];

   SetAPen(rp,7);
   SetBPen(rp,0);
   itoa(Red,buffy,16);
   Move(rp,68,19);
   Text(rp,buffy,1);
   itoa(Green,buffy,16);
   Move(rp,68,27);
   Text(rp,buffy,1);
   itoa(Blue,buffy,16);
   Move(rp,68,35);
   Text(rp,buffy,1);
   Move(rp,58,75);
   itoa(hesitate,buffy,10);
   Text(rp,buffy,strlen(buffy));
   Text(rp," ",1);
}

void procgads(add)
 struct Gadget *add;
{
 USHORT id;
 UBYTE,r,g,b;
 ULONG class;

   id=add->GadgetID;

      switch(id){
      case 1   :arr2[0][7]=col;
		break;
      case 2   :arr2[0][8]=col;
		break;
      case 3   :arr2[0][1]=col;
		break;
      case 4   :arr2[0][6]=col;
		break;
      case 5   :arr2[0][0]=col;
		break;
      case 6   :arr2[0][2]=col;
		break;
      case 7   :arr2[0][5]=col;
		break;
      case 8   :arr2[0][4]=col;
		break;
      case 9   :arr2[0][3]=col;
		break;
      case 10  :arr2[1][7]=col;
		break;
      case 11  :arr2[1][8]=col;
		break;
      case 12  :arr2[1][1]=col;
		break;
      case 13  :arr2[1][6]=col;
		break;
      case 14  :arr2[1][0]=col;
		break;
      case 15  :arr2[1][2]=col;
		break;
      case 16  :arr2[1][5]=col;
		break;
      case 17  :arr2[1][4]=col;
		break;
      case 18  :arr2[1][3]=col;
		break;
      case 19  :arr2[4][5]=col;
		break;
      case 20  :arr2[4][6]=col;
		break;
      case 21  :arr2[4][7]=col;
		break;
      case 22  :arr2[4][4]=col;
		break;
      case 23  :arr2[4][0]=col;
		break;
      case 24  :arr2[4][8]=col;
		break;
      case 25  :arr2[4][3]=col;
		break;
      case 26  :arr2[4][2]=col;
		break;
      case 27  :arr2[4][1]=col;
		break;
      case 28  :
      case 29  :
      case 30  :
      case 31  :do{
		    if(msg=(struct IntuiMessage*)GetMsg(Wdw->UserPort)){
		       class=msg->Class;
		       ReplyMsg((struct Message*)msg);
		    }
		   r=RedStruct.HorizPot>>12;
		   g=GreenStruct.HorizPot>>12;
		   b=BlueStruct.HorizPot>>12;
		   hesitate=delaystruct.HorizPot>>10;
		   SetRGB4CM(colorcode,col,r,g,b);
		   SetRGB4(WVP,col,r,g,b);
		   PrintRGB(r,g,b);
		  }while(class!=GADGETUP);
		break;
      case 32  :feed("f");
		break;
      case 33  :feed("af");
		break;
      case 34  :feed("2f");
		break;
      case 35  :feed("b");
		break;
      case 36  :feed("ab");
		break;
      case 37  :feed("2b");
		break;
      case 38  :feed("l");
		break;
      case 39  :feed("al");
		break;
      case 40  :feed("2l");
		break;
      case 41  :feed("r");
		break;
      case 42  :feed("ar");
		break;
      case 43  :feed("2r");
		break;
      case 44  :feed("u");
		break;
      case 45  :feed("au");
		break;
      case 46  :feed("2u");
		break;
      case 47  :feed("d");
		break;
      case 48  :feed("ad");
		break;
      case 49  :feed("2d");
		break;
      case 50  :MyWrite(" ");
		feed("oc");
		break;
      case 51  :MyWrite(" ");
		feed("oa");
		break;
      case 52  :MyWrite(" ");
		feed("ou");
		break;
      case 53  :MyWrite(" ");
		feed("od");
		break;
      case 54  :MyWrite(" ");
		feed("or");
		break;
      case 55  :MyWrite(" ");
		feed("ol");
		break;
      default  :break;
      }
   if(id<28)
      update(1);
}

InitMouseMen()
{
   if(dided)
      return;
   if(done)
      ClosePal();

      SetAPen(rp,7);
      SetBPen(rp,0);
      Move(rp,266,15);
      Text(rp,"TURN",4);
      Move(rp,259,96);
      SetAPen(rp,7);
      Text(rp,"ORIENT",6);
      SetAPen(rp,0);
      SetBPen(rp,7);
      Move(rp,255,131);
      Text(rp," Close ",7);

      AddGList(Wdw,&Arrow1,(SHORT)~0,-1,NULL);
      ColorGads();
      dided=1;
}

ColorGads()
{
      SetBPen(rp,0);
      Move(rp,246,25);
      SetAPen(rp,arr1[1][0]);
      Text(rp,"F",1);
      Move(rp,246,36);
      SetAPen(rp,arr1[3][0]);
      Text(rp,"B",1);
      Move(rp,246,47);
      SetAPen(rp,arr1[2][0]);
      Text(rp,"L",1);
      Move(rp,246,58);
      SetAPen(rp,arr1[4][0]);
      Text(rp,"R",1);
      Move(rp,246,69);
      SetAPen(rp,arr1[0][0]);
      Text(rp,"U",1);
      Move(rp,246,80);
      SetAPen(rp,arr1[5][0]);
      Text(rp,"D",1);

      DImageB.PlanePick=arr1[3][0];
      LtImageB.PlanePick=arr1[3][0];
      RtImageB.PlanePick=arr1[3][0];

      DImageF.PlanePick=arr1[1][0];
      LtImageF.PlanePick=arr1[1][0];
      RtImageF.PlanePick=arr1[1][0];

      DImageL.PlanePick=arr1[2][0];
      LtImageL.PlanePick=arr1[2][0];
      RtImageL.PlanePick=arr1[2][0];

      DImageR.PlanePick=arr1[4][0];
      LtImageR.PlanePick=arr1[4][0];
      RtImageR.PlanePick=arr1[4][0];

      DImageU.PlanePick=arr1[0][0];
      LtImageU.PlanePick=arr1[0][0];
      RtImageU.PlanePick=arr1[0][0];

      DImageD.PlanePick=arr1[5][0];
      LtImageD.PlanePick=arr1[5][0];
      RtImageD.PlanePick=arr1[5][0];

      RefreshGList(&Arrow1,Wdw,NULL,-1);
}

main()
{
 static char side;
 int direction,x;

   FirstMenu=&Menus;

   FuncItem[0].NextItem=&FuncItem[1];
   FuncItem[1].NextItem=&FuncItem[2];
   FuncItem[2].NextItem=&FuncItem[3];
   FuncItem[3].NextItem=&FuncItem[4];
   FuncItem[4].NextItem=&FuncItem[5];
   FuncItem[5].NextItem=&FuncItem[6];
   FuncItem[6].NextItem=&FuncItem[7];
   FuncItem[7].NextItem=&FuncItem[8];

   KeysItem[0].NextItem=&KeysItem[1];
   KeysItem[1].NextItem=&KeysItem[2];
   KeysItem[2].NextItem=&KeysItem[3];
   KeysItem[3].NextItem=&KeysItem[4];
   KeysItem[4].NextItem=&KeysItem[5];
   KeysItem[5].NextItem=&KeysItem[6];
   KeysItem[6].NextItem=&KeysItem[7];

   arr1_pointer=&arr1[0][0];
   arr2_pointer=&arr2[0][0];

   DrawBord.FrontPen=7;
   DrawBord.DrawMode=JAM1;

   pf[0]=(void*)DrawU;
   pf[1]=(void*)DrawF;
   pf[2]=(void*)DrawL;
   pf[3]=(void*)DrawB;
   pf[4]=(void*)DrawR;
   pf[5]=(void*)DrawD;
   op[0]=(void*)Larry;
   op[1]=(void*)Cathy;
   op[2]=(void*)Dura;
   op[3]=(void*)Cathy;
   op[4]=(void*)Dura;
   op[5]=(void*)Larry;

   StartScreens();
   DrawHome();

   Move(rp2,15,yp);
   direction=0;
   side=6;

   while(1)
   {
      Keywait();
      do
      {
       for(x=0;x<buffer;x++)
	   key[x]=key[x+1];
	 switch(key[0]){
	    case 'u':side=UP;
		     MyWrite(key);
		     break;
	    case 'd':side=DOWN;
		     MyWrite(key);
		     break;
	    case 'l':side=LEFT;
		     MyWrite(key);
		     break;
	    case 'r':side=RIGHT;
		     MyWrite(key);
		     break;
	    case 'f':side=FRONT;
		     MyWrite(key);
		     break;
	    case 'b':side=BACK;
		     MyWrite(key);
		     break;
	    case 'o':MyWrite(" ");
		     Text(rp2,key,1);
		     orient(6);
		     break;
	    case '2':direction=2;
		     MyWrite(key);
		     break;
	    case 'a':direction=1;
		     MyWrite(key);
		     break;
	    case 'w':wait=1;
		     break;
	    case '1':single=1;
		     break;
	    case 'p':find();
		     break;
	    case '5':
		     best=quick=1;
		     savecube();
		     clear();
		     Worst();
		     best=quick=0;
		     break;
	    default :break;
	    }
	 if(side!=6)
	    {
	       rotate(side,direction);
	       direction=0;
	       side=6;
	    }
      key[0]=0;
      }while(--buffer>0);
   }

}
