#include <aros/oldprograms.h>
#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "string.h"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct RastPort *r,*r2;
struct Window *Wind,*abWind;
struct NewWindow NewWindow,NewWindow2;
struct IntuiMessage *mesg;
struct IntuiText prompt,yprompt,t0,t1;
struct Menu Menu1;
struct MenuItem m0,m1;

#define INTUITION_REV 29
#define GRAPHICS_REV  29
#define n             19

BOOL    gamewon=FALSE,firstmove=TRUE;
USHORT  mclass,mcode,msx,msy;
SHORT   d,i,j,k,l,x,y,amx,amy,start;
SHORT   lx,ly,pla,opp,x0,x1,y0,y1,max,value;
SHORT   AttackFactor;
SHORT   Board[n+1][n+1],Aline[4][n+1][n+1][2],Value[n+1][n+1][2];
int     ov,xv,len;
char    text[10];
static  SHORT Weight[]={0,0,4,20,100,500,0};


    void OpenALL(),Human(),AddUp(),UpdateValue(),MakeMove(),FindMove();
    void CreateMes(),DrawFrame(),make_window(),init_newgame();
    void setup_menu(),show_About();


VOID _main()
{

    AttackFactor=4;
    start=0;

    OpenALL();
    make_window();

    setup_menu();
    SetMenuStrip(Wind,&Menu1);

    r=Wind->RPort;

    CreateMes(&yprompt,3,3," OK ");

NewGame:

    init_newgame();
    if(start == 0) goto Human_first;

Loop:
    pla=1;
    opp=0;
    FindMove();
    MakeMove();
    Board[x][y]=2;

    if (! firstmove) {
        SetAPen(r,0);
        DrawFrame();
    }
    else firstmove=FALSE;

    x0=(x-1)*20;
    y0=(y-1)*8;
    SetAPen(r,1);
    Move(r,x0+14,y0+14);
    Draw(r,x0+26,y0+18);
    Move(r,x0+14,y0+18);
    Draw(r,x0+26,y0+14);

    amx=x0+13;
    amy=y0+13;
    SetAPen(r,3);
    DrawFrame();

    if (gamewon) {
        ov=ov+1;

        CreateMes(&prompt,50,5,"I won");
        AutoRequest(Wind,&prompt,&yprompt,&yprompt,NULL,NULL,160,50);

        goto NewGame;
    }

Human_first:

    pla=0;
    opp=1;
    Human();
    MakeMove();

    if (gamewon) {
        xv=xv+1;
        CreateMes(&prompt,37,5,"You won!");
        AutoRequest(Wind,&prompt,&yprompt,&yprompt,NULL,NULL,160,50);

        goto NewGame;
    }

    goto Loop;
}


VOID Human()

{
    /* Set Up an IDCMP Read Loop */
HumanLoop:
    Wait (1 << Wind->UserPort->mp_SigBit);

    while((mesg=(struct IntuiMessage *) GetMsg(Wind->UserPort)) != NULL) {
        mclass=mesg->Class;
        mcode=mesg->Code;
        msx=mesg->MouseX;
        msy=mesg->MouseY;
        ReplyMsg((struct Message *)mesg);
    }

    switch(mclass) {
        case MENUPICK: {
            if(ITEMNUM(mcode) == 0) {
                ClearMenuStrip(Wind);
                show_About();

                while((mesg=(struct IntuiMessage *) GetMsg((struct Message *)Wind->UserPort)) != NULL) ReplyMsg((struct Message *)mesg);

                SetMenuStrip(Wind,&Menu1);
                goto HumanLoop;
            }
            else if(ITEMNUM(mcode) == 1) {                 /* Quit */
                ClearMenuStrip(Wind);
                CloseWindow(Wind);
                exit(FALSE);
                break;
            }
            else goto HumanLoop;
        }
        case MOUSEBUTTONS: {
            if(ReadPixel(r,msx,msy) == 2) goto HumanLoop;  /* black line */
            if(msx < 10 || msx > 10+n*20) goto HumanLoop;  /* outside board */
            if(msy < 12 || msy > 12+n*8) goto HumanLoop;   /* outside board */

            y=((msy-12)/8)+1;
            x=((msx-10)/20)+1;
            if(Board[x][y] > 0) goto HumanLoop;            /* occupied square */

            Board[x][y]=1;
            i=(x-1)*20+16;
            j=y*8+6;

            SetAPen(r,1);
            Move(r,i+1,j);
            Draw(r,i+6,j);
            Move(r,i+7,j+1);
            Draw(r,i+7,j+3);
            Move(r,i+6,j+4);
            Draw(r,i+1,j+4);
            Move(r,i,j+3);
            Draw(r,i,j+1);
        }
    }
}


VOID MakeMove()

{
    gamewon=FALSE;

    d=0;
    for(k=0;k<=4;k++) {
        x1=x-k;
        y1=y;
        if(x1>=1 && x1<=n-4) {
            AddUp();
            for(l=0;l<=4;l++)
                UpdateValue();
        }
    }

    d=1;
    for(k=0;k<=4;k++) {
        x1=x-k;
        y1=y-k;
        if((x1>=1 && x1<=n-4) && (y1>=1 && y1<=n-4)) {
            AddUp();
            for(l=0;l<=4;l++)
                UpdateValue();
        }
    }

    d=3;
    for(k=0;k<=4;k++) {
        x1=x+k;
        y1=y-k;
        if((x1>=5 && x1 <= n) && (y1>=1 && y1<=n-4)) {
            AddUp();
            for(l=0;l<=4;l++)
                UpdateValue();
        }
    }

    d=2;
    for(k=0;k<=4;k++) {
        x1=x;
        y1=y-k;
        if(y1>=1 && y1<=n-4) {
            AddUp();
            for(l=0;l<=4;l++)
                UpdateValue();
        }
    }
}


VOID AddUp()

{
    Aline[d][x1][y1][pla] = Aline[d][x1][y1][pla] + 1;

    if(Aline[d][x1][y1][pla] == 5) gamewon=TRUE;
}


VOID UpdateValue()

{
    if(d==0) {
        lx=l;
        ly=0;
    }
    else if(d==1) {
        lx=l;
        ly=l;
    }
    else if(d==2) {
        lx=0;
        ly=l;
    }
    else {
        lx=-l;
        ly=l;
    }

    if(Aline[d][x1][y1][opp] == 0) Value[x1+lx][y1+ly][pla] = Value[x1+lx][y1+ly][pla] + Weight[Aline[d][x1][y1][pla]+1] - Weight[Aline[d][x1][y1][pla]];
    else if(Aline[d][x1][y1][pla] == 1) Value[x1+lx][y1+ly][opp] = Value[x1+lx][y1+ly][opp] - Weight[Aline[d][x1][y1][opp]+1];
}


VOID FindMove()

{
    max=-32767;
    x=(n+1)/2;
    y=(n+1)/2;
    if(Board[x][y] == 0) max=4;

    for(i=1;i<=n;i++)   {
        for(j=1;j<=n;j++) {
            if(Board[i][j] == 0) {
                value=Value[i][j][pla] * (16 + AttackFactor) / 16 + Value[i][j][opp];
                if(value > max) {
                    x=i;
                    y=j;
                    max=value;
                }
            }
        }
    }
}


VOID OpenALL()        /* Open required libraries */

{
    IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",INTUITION_REV);

    if(IntuitionBase == NULL) exit(FALSE);

    GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",GRAPHICS_REV);

    if(GfxBase == NULL) exit(FALSE);
}


VOID CreateMes(x,left,top,mes)

struct  IntuiText *x;
SHORT   left,top;
UBYTE   *mes;
{
    x->FrontPen=0;
    x->BackPen=1;
    x->DrawMode=JAM1;
    x->LeftEdge=left;
    x->TopEdge=top;
    x->ITextFont=NULL;
    x->IText=mes;
    x->NextText=NULL;
}


VOID DrawFrame()

{
    Move(r,amx,amy);
    Draw(r,amx+14,amy);
    Draw(r,amx+14,amy+6);
    Draw(r,amx,amy+6);
    Draw(r,amx,amy);
}


VOID make_window()        /* Open a plain window */

{
    NewWindow.LeftEdge=0;
    NewWindow.TopEdge=0;
    NewWindow.Width=480;
    NewWindow.Height=170;
    NewWindow.DetailPen=-1;
    NewWindow.BlockPen=-1;
    NewWindow.Title="Five In Line";
    NewWindow.Flags=ACTIVATE|WINDOWDRAG|WINDOWDEPTH|SMART_REFRESH;
    NewWindow.IDCMPFlags=MOUSEBUTTONS|MENUPICK;
    NewWindow.Type=WBENCHSCREEN;
    NewWindow.FirstGadget=NULL;
    NewWindow.CheckMark=NULL;
    NewWindow.Screen=NULL;
    NewWindow.BitMap=NULL;
    NewWindow.MinWidth=0;
    NewWindow.MinHeight=0;
    NewWindow.MaxWidth=640;
    NewWindow.MaxHeight=200;

    Wind=(struct Window *) OpenWindow(&NewWindow);

}


VOID init_newgame()

{
    start=1-start;                /* toggle between computer and human */

    for(x=1;x<=n;x++) {
        for(y=1;y<=n;y++)
            Board[x][y]=0;
    }

    for(d=0;d<=3;d++) {
        for(x=1;x<=n;x++) {
            for(y=1;y<=n;y++) {
                for(pla=0;pla<=1;pla++)
                    Aline[d][x][y][pla]=0;
            }
        }
    }

    for(x=1;x<=n;x++) {
        for(y=1;y<=n;y++) {
            for(pla=0;pla<=1;pla++)
                Value[x][y][pla]=0;
        }
    }

    SetAPen(r,0);
    RectFill(r,10,12,400,168);        /* blank board           */

    firstmove=TRUE;

/* draw new grid */

    SetAPen(r,2);

    for(x=10;x<=10+n*20;x=x+20) {
        Move(r,x,12);
        Draw(r,x,12+n*8);
    }

    for(y=12;y<=12+n*8;y=y+8) {
        Move(r,10,y);
        Draw(r,10+n*20,y);
    }

/* print scores */

    SetAPen(r,1);
    Move(r,420,30);
    Text(r,"You:",4);
    Move(r,450,30);
    len=sprintf(text,"%3d",xv);
    Text(r,text,len);

    Move(r,420,45);
    Text(r,"I  :",4);
    Move(r,450,45);
    len=sprintf(text,"%3d",ov);
    Text(r,text,len);
}


VOID setup_menu()

{
    CreateMes(&t0,0,0,"About");

    m0.NextItem=&m1;
    m0.LeftEdge=5;
    m0.TopEdge=0;
    m0.Width=50;
    m0.Height=10;
    m0.Flags=ITEMTEXT|HIGHCOMP|ITEMENABLED;
    m0.MutualExclude=NULL;
    m0.ItemFill=(APTR)&t0;
    m0.SelectFill=NULL;
    m0.Command=NULL;
    m0.SubItem=NULL;

    CreateMes(&t1,0,0,"Quit");

    m1.NextItem=NULL;
    m1.LeftEdge=5;
    m1.TopEdge=15;
    m1.Width=50;
    m1.Height=10;
    m1.Flags=ITEMTEXT|HIGHCOMP|ITEMENABLED;
    m1.MutualExclude=NULL;
    m1.ItemFill=(APTR)&t1;
    m1.SelectFill=NULL;
    m1.Command=NULL;
    m1.SubItem=NULL;

    Menu1.NextMenu=NULL;
    Menu1.LeftEdge=0;
    Menu1.TopEdge=0;
    Menu1.Width=60;
    Menu1.Height=100;
    Menu1.Flags=MENUENABLED;
    Menu1.MenuName="Project";
    Menu1.FirstItem=&m0;

}


VOID show_About()

{
    NewWindow2.LeftEdge=Wind->LeftEdge + 40;
    NewWindow2.TopEdge=Wind->TopEdge + 45;
    NewWindow2.Width=400;
    NewWindow2.Height=80;
    NewWindow2.DetailPen=-1;
    NewWindow2.BlockPen=-1;
    NewWindow2.Title="About Five In Line";
    NewWindow2.Flags=ACTIVATE|WINDOWCLOSE|SMART_REFRESH;
    NewWindow2.IDCMPFlags=CLOSEWINDOW;
    NewWindow2.Type=WBENCHSCREEN;
    NewWindow2.FirstGadget=NULL;
    NewWindow2.CheckMark=NULL;
    NewWindow2.Screen=NULL;
    NewWindow2.BitMap=NULL;
    NewWindow2.MinWidth=0;
    NewWindow2.MinHeight=0;
    NewWindow2.MaxWidth=640;
    NewWindow2.MaxHeight=200;

    abWind=(struct Window *) OpenWindow(&NewWindow2);
    r2=abWind->RPort;

    SetAPen(r2,1);
    Move(r2,10,20);
    Text(r2,"Placed in Public Domain 1988",28);
    Move(r2,10,30);
    Text(r2,"by Njål Fisketjøn.",18);

    Move(r2,10,50);
    Text(r2,"Algorithm from a",16);

    SetAPen(r2,3);
    Move(r2,146,50);
    Text(r2,"Borland Turbo Pascal",20);

    SetAPen(r2,1);
    Move(r2,314,50);
    Text(r2,"program.",8);

    Move(r2,10,70);
    Text(r2,"Close window to continue playing.",33);

    Wait (1 << abWind->UserPort->mp_SigBit);
    while((mesg=(struct IntuiMessage *) GetMsg(abWind->UserPort)) != NULL) ReplyMsg((struct Message *)mesg);
    CloseWindow(abWind);
}
