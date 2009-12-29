#include <dos/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#undef PI
#define PI 3.141592654
#define SCREENWIDTH 640
#define SCREENHEIGHT 400
#define MAPWIDTH 512
#define MAPHEIGHT 512
#define CLOUDWIDTH 640
#define CLOUDHEIGHT 400

#define MAPSHIFT 9          /* 2^9==512==MAPWIDTH */
#define voxel_dist 3000     


/* stegerg */
#define CHECKMAPRANGE 1
#define PIXELWIDTH 2
#define MAXVIEWLENGTH SCREENWIDTH
#define RESIZEABLE 1

/*************************************************************/

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *CyberGfxBase;
struct Screen *scr;
struct Window *win;
struct RastPort *rp;

ULONG cgfx_coltab[256];

int screenwidth = SCREENWIDTH;
int screenheight = SCREENHEIGHT - 1;
int resizescale = 1 << 16;

/**************************VARS*******************************/
float sintab[360],costab[360];
char *buffer,         /* Offscreen Buffer */
     *map;           /* Height Map */
unsigned char     *color,          /* Color Map */
     *clouds;         /* Cloud Bitmap */
char ende;
char scancode;
char Keys[128];
#if 0
void (__interrupt __far *old09h)();
void __interrupt __far new09h();
#endif
int hoehe,richt,xc,yc,direction,voxel_stretch=50;
int horizon,maxy,
    view_length,view_size,flyheight = 100,
    y_old,offtab[SCREENHEIGHT],
    xtab[SCREENWIDTH][MAXVIEWLENGTH],voxtab[SCREENWIDTH];
    
/************************************************************/


/**************************PCX-STRUCTS***********************/
typedef struct{
        short x,y;} Tpcxsize;


typedef struct{
        unsigned char id,version,encoding,bpp;
        short x1,y1,x2,y2,hres,vres;
        unsigned char colors[48];
        unsigned char vmode,planes;
        short bpl,palinfo;
        unsigned char fillbytes[58];
        } PCXHeader;

/************************************************************/


/*****************BUILD TABLES*******************************/

void intvars()
{    
    horizon=150,
    maxy=(screenheight-1),
    view_length=(screenwidth/5),
    view_size=(screenwidth/PIXELWIDTH)-1;
    
    resizescale = (screenheight << 16) / SCREENHEIGHT;
    
}

void make_trig()
{
    float i;
    for(i=0;i<=359;i++)sintab[(int)i]=sin(i/(180)*PI);
    for(i=0;i<=359;i++)costab[(int)i]=cos(i/(180)*PI);
}


void gentab()
{
    int i, j;
    
    for(j=1;j<=view_length;j++)voxtab[j]=horizon+voxel_dist/j;
    for(j=0;j<screenheight;j++)offtab[j]=j*SCREENWIDTH;
    for(i=0;i<=view_size;i++)
        for(j=1;j<=view_length;j++)
            xtab[i][j]=i*j /view_size-(j>>1);                    
    make_trig();
}


float sinus(int a)
{
    a+=360;
    a=a%360;
    return sintab[(int)a];
}

float cosinus(int a)
{
    a+=360;
    a=a%360;
    return costab[(int)a];
}


/************************PCX LOADER**************************/

Tpcxsize getpcxsize(char *d)
{
    Tpcxsize k;
    PCXHeader pc;
    FILE *f=fopen(d,"rb");
    memset(&pc,0,sizeof(pc));
    fread(&pc,sizeof(pc),1,f);
    fclose(f);
    k.x=pc.x2+1;
    k.y=pc.y2+1;
    return k;
}

long filesize(FILE *n)
{
    long org=ftell(n), size;
    fseek(n,-0L,SEEK_END);
    size=ftell(n);
    fseek(n,org,SEEK_SET);
    return size;
}

void loadpcx(char *name,char *po)
{
    FILE *f;
    int g,m,i,z;
    unsigned char *cimage;
    unsigned char header[128];
    
    Tpcxsize sz=getpcxsize(name);
    f=fopen(name,"rb");
    cimage=(char *)malloc(filesize(f));
    fread(&header,128,1,f);
    
    fread(&*cimage,filesize(f)-896,1,f);
    fclose(f);
    z=0;
    g=0;
    do{
        if(cimage[z]<192) po[g]=cimage[z];
        if(cimage[z]>=192){m=cimage[z]-192;
        z++;
        for(i=1;i<=m;i++) po[g+i-1]=cimage[z];
        g=g+m-1;}
        z++;
        g++;
    }while(g<sz.x*sz.y);
    free(cimage);
}



void loadpcxpal(char *name,unsigned char pal[768])
{
    FILE *f;
    int i;
    
    f=fopen(name,"rb");
    fseek(f,filesize(f)-768,SEEK_SET);
    fread(pal,768,1,f);
    for(i=0;i<768;i++) pal[i]/=4;
}

/***********************************************************/


void setpal(unsigned char palp[])
{
    unsigned int pcount;
    for(pcount=0;pcount<256;pcount++){
        cgfx_coltab[pcount] = (palp[pcount * 3] << 18) +
			      (palp[pcount * 3 + 1] << 10) +
			      (palp[pcount * 3 + 2] << 2);
			      
    }
}

void adjust(char *b) 
{
int x,y,f;
for(y=0;y<MAPHEIGHT;y++)
for(x=0;x<MAPWIDTH;x++){
        f=b[(x-1)+y*MAPWIDTH]+\
        b[(x-2)+y*MAPWIDTH]+\
        b[(x+1)+y*MAPWIDTH]+\
        b[(x+2)+y*MAPWIDTH]+\
        b[x+(y-1)*MAPWIDTH]+\
        b[x+(y-2)*MAPWIDTH]+\
        b[x+(y+1)*MAPWIDTH]+\
        b[x+(y+2)*MAPWIDTH];
        if(f!=0)b[x+y*MAPWIDTH]=f>>3;}
}



/*****************MAIN VOXEL PROCS**************************/

# if 1

static inline void innerloop(int x, int start,int col)
{
    UBYTE *dest = buffer + x + start * SCREENWIDTH;
    int count = y_old + 1 - start;

#if PIXELWIDTH==4
  #define PIXELTYPE int    
    col += (col << 24) + (col << 16) + (col << 8);
#elif PIXELWIDTH==2
  #define PIXELTYPE short
    col += (col << 8);
#elif PIXELWIDTH==1
  #define PIXELTYPE char
#else
  #error "PIXELWIDTH must be 1,2 or 4"
#endif
    
    while(--count)
    {
        *(PIXELTYPE *)dest = col;
	dest += SCREENWIDTH;
    }
}

#else

void innerloop(int x,int start,int col);    /* draws 4pix vline */
#pragma aux innerloop=   \
    "mov    edi,[buffer]"\
    "add    edi,ebx     "\
    "mov    ecx,[y_old] "\
    "inc    ecx         "\
    "sub    ecx,edx     "\
    "imul   edx,640     "\
    "add    edi,edx     "\
    "mov    ah,al       "\
    "push   ax          "\
    "shl    eax,16      "\
    "pop    ax          "\
    "l1:                "\
    "mov    [edi],eax   "\
    "add    edi,640     "\
    "loop l1            "\
     modify [edi ecx]    \
     parm[ebx][edx][eax];

#endif

void draw_voxel(int x,int e,int f,int c)
{
    int y,h;
    f-=hoehe;
    h=f*voxel_stretch /e;
#if RESIZEABLE
    y=((voxtab[e]-h) * resizescale) >> 16;
#else
    y=voxtab[e]-h;
#endif
    if(y>0&&y<y_old){
                innerloop(x,y,c);
                y_old=y;
    }        
}


void voxel_spacing(int xp,int yp,int r)   
{
    int i,j=0,offset,x,co,si,nx,ny;
    int f,c;

    r-=90;
    co=cosinus(r)*1024;
    si=sinus(r)*1024;

    hoehe=map[xp+yp*MAPWIDTH]+flyheight;

    for(i=0;i<=view_size;i++){
        y_old=screenheight-1;
        for(j=1;j<=view_length;j++){
            x=xtab[i][j];
            nx=(x*co >> 10)-(j*si >> 10);
            ny=(x*si >> 10)+(j*co >> 10);
#if CHECKMAPRANGE
            offset=((nx+xp) & (MAPWIDTH - 1)) + (((ny+yp) & (MAPHEIGHT - 1))<<MAPSHIFT); 
#else
            offset=(nx+xp)+((ny+yp)<<MAPSHIFT); 
#endif
            f=map[offset];
            c=color[offset];
            draw_voxel(i*PIXELWIDTH,j,f,c);
            }
        }
}

/***********************************************************/

#define KC_LEFT         0x4F
#define KC_RIGHT     	0x4E
#define KC_UP        	0x4C
#define KC_DOWN      	0x4D
#define KC_NUMPAD8  	0x3E
#define KC_NUMPAD2  	0x1E
#define KC_NUMPADPLUS 	0x5E
#define KC_NUMPADMINUS 	0x4A
#define KC_NUMPADMINUS2 0x5D
#define KC_ESC       	0x45
#define KC_RETURN	0x44

void control()
{
    if(Keys[KC_DOWN]){
        xc-=cosinus(richt)*4;
        yc-=sinus(richt)*4;
        }
    if(Keys[KC_UP]){
        xc+=cosinus(richt)*4;
        yc+=sinus(richt)*4;
        }
    if(Keys[KC_RIGHT]){richt-=4;direction-=4;if(richt<0)richt+=360;if(direction<0) direction+=SCREENWIDTH;}
    if(Keys[KC_LEFT]){richt+=4;direction+=4;if(richt>=360)richt-=360;if(direction>=SCREENWIDTH)direction-=SCREENWIDTH;}
    if(Keys[KC_NUMPAD2])flyheight--;
    if(Keys[KC_NUMPAD8])flyheight++;
    if(Keys[KC_RETURN])adjust(map);
    if(Keys[KC_NUMPADMINUS] || Keys[KC_NUMPADMINUS2])voxel_stretch++;
    if(Keys[KC_NUMPADPLUS])voxel_stretch--;
    if(Keys[KC_ESC]) ende=1;
}

/***********************************************************/

void cleanup(char *msg)
{
    if (msg)
    {
        printf("voxel: %s\n",msg);
    }
    
    if (win) CloseWindow(win);
    
    if (scr) UnlockPubScreen(0, scr);
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    exit(0);
}

void openlibs(void)
{
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39)))
    {
        cleanup("Can't open intuition.library V39!");
    }
    
    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39)))
    {
        cleanup("Can't open graphics.library V39!");
    }
    
    if (!(CyberGfxBase = OpenLibrary("cybergraphics.library",0)))
    {
        cleanup("Can't open cybergraphics.library!");
    }
}

void getvisual(void)
{
    if (!(scr = LockPubScreen(NULL)))
    {
        cleanup("Can't lock pub screen!");
    }
    
    if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) <= 8)
    {
        cleanup("Need hi or true color screen!");
    }
}

void makewin(void)
{
    win = OpenWindowTags(NULL, WA_CustomScreen, (IPTR)scr, 
    			       WA_InnerWidth, SCREENWIDTH,
    			       WA_InnerHeight, SCREENHEIGHT - 1,
			       WA_Title, (IPTR)"Voxel",
			       WA_DragBar, TRUE,
			       WA_DepthGadget, TRUE,
			       WA_CloseGadget, TRUE,
			       RESIZEABLE    ?
	                       WA_SizeGadget :
                               TAG_IGNORE    , TRUE,
                               RESIZEABLE    ?
			       WA_SizeBBottom:
                               TAG_IGNORE    , TRUE,
			       WA_Activate, TRUE,
			       WA_IDCMP, IDCMP_CLOSEWINDOW |
  			   (RESIZEABLE * IDCMP_NEWSIZE) |
			       		 IDCMP_RAWKEY,
			       TAG_DONE);
			       
   if (!win) cleanup("Can't open window");

#if RESIZEABLE
   WindowLimits(win, 320 + win->BorderLeft + win->BorderRight,
   		     200 + win->BorderTop + win->BorderBottom,
		     SCREENWIDTH + win->BorderLeft + win->BorderRight,
		     SCREENHEIGHT - 1 + win->BorderTop + win->BorderBottom);
			
#endif
   
   rp = win->RPort; 
}

void getevents(void)
{
    struct IntuiMessage *msg;
    
    while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
        switch(msg->Class)
	{
	    case IDCMP_CLOSEWINDOW:
	        ende = TRUE;
		break;
		
	    case IDCMP_RAWKEY:
	        {
		    WORD code = msg->Code & ~IECODE_UP_PREFIX;
		    
		    Keys[code] = (code == msg->Code) ? 1 : 0;

		}
	        break;

#if RESIZEABLE

	   case IDCMP_NEWSIZE:
	   	{
		    screenwidth = win->Width - win->BorderLeft - win->BorderRight;
		    screenheight = win->Height - win->BorderTop - win->BorderBottom;
		    intvars();
		    gentab();
		}
		break;
#endif
	}
        ReplyMsg((struct Message *)msg);
    }
}
/********************MAIN PROC******************************/

int main()
{
    int frame=0, u;
    unsigned char pal[768];

    openlibs();
    getvisual();
    
    intvars();
    gentab();
    buffer=(char*)malloc(SCREENWIDTH*(SCREENHEIGHT + 1));
    map=(char*)malloc(MAPWIDTH*MAPHEIGHT);
    color=(char*)malloc(MAPWIDTH*MAPHEIGHT);
    clouds=(char*)malloc(CLOUDWIDTH*CLOUDHEIGHT);
    if(color==0||buffer==0||map==0||clouds==0){
        printf("no mem... -sorry!\n");exit(0);
        }
    loadpcx("demoh.pcx",map);
    loadpcx("democ.pcx",color);
    loadpcx("clouds.pcx",clouds);
    loadpcxpal("democ.pcx",pal);
    for(u=0;u<7;u++)adjust(map);    
    setpal(pal);
    xc=160,yc=100,direction=0;
    
    makewin();
    
    do{
	getevents();
        control();
        memcpy(buffer,clouds + CLOUDWIDTH - direction,SCREENWIDTH*screenheight);
        voxel_spacing(xc,yc,richt);

        WriteLUTPixelArray(buffer,
			   0,
			   0,
			   SCREENWIDTH,
			   rp,
			   cgfx_coltab,
			   win->BorderLeft,
			   win->BorderTop,
			   screenwidth,
			   screenheight,
			   CTABFMT_XRGB8);
    	WaitTOF();
        frame++;
        }while(!ende);
	
    cleanup(0);
}
