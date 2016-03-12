
#include "shuffle.h"

struct EasyStruct ExitReq =
       {
        0L,0L,(UBYTE *)"Quit the Game ?",
              (UBYTE *)"Really quit ?",
              (UBYTE *)"Yes (L-AMIGA+V) | No (L-AMIGA + B)",
       };

struct EasyStruct AboutReq =
       {
        0L,0L,(UBYTE *)"About Shuffle",
              (UBYTE *)"Shuffle V1.1 is Copyright (©) 1993 CEKASOFT\nKarlheinz Klingbeil\nElzstr. 42\n7809 Gutach\nGERMANY\n\n    *ENJOY*",
              (UBYTE *)"stupid|silly|incredible",
       };

WORD bp1[] =
      {
       0,0,0,0,0,0
      };

WORD bp2[] =
      {
       0,0,0,0,0,0
      };

struct Border b2 =
       {
         0,0,1,0,JAM1,3,bp2,       
       };

struct Border b1 =
       {
        0,0,2,0,JAM1,3,bp1,&b2       
       };
        
struct Border b2r =
       {
         0,0,2,0,JAM1,3,bp2,       
       };

struct Border b1r =
       {
        0,0,1,0,JAM1,3,bp1,&b2       
       };

struct TextAttr topaz8 = {
    ( STRPTR )"topaz.font", 8, 0x00, 0x01 };

UBYTE  number[8];

struct IntuiText ntext =
       {
        1,2,JAM2,0,0,&topaz8,number,NULL
       };

struct IntuiText anykey = {1,2,JAM2,4,100,&topaz8,"--- Any key to continue ---",NULL};

struct IntuiText intro4 = {1,2,JAM2,0,50,&topaz8,"Press h for Help, Q to Quit ",&anykey};
struct IntuiText intro3 = {1,2,JAM2,0,40,&topaz8,"to their proper places !",&intro4};
struct IntuiText intro2 = {1,2,JAM2,0,30,&topaz8,"by moving the tiles around",&intro3};    
struct IntuiText intro1 = {1,2,JAM2,0,20,&topaz8,"Try  to clean up this mess",&intro2};

struct IntuiText h5 = {1,2,JAM2,0,60,&topaz8,"A for About shuffle...",&anykey};
struct IntuiText h4 = {1,2,JAM2,0,50,&topaz8,"S for Score Display",&h5};
struct IntuiText h3 = {1,2,JAM2,0,40,&topaz8,"q or ESC Key to Quit",&h4};
struct IntuiText h2 = {1,2,JAM2,0,30,&topaz8,"H or Help Key for Help",&h3};    
struct IntuiText h1 = {1,2,JAM2,0,20,&topaz8,"Move Tiles with cursor keys",&h2};

struct IntuiText s2 = {1,2,JAM2,0,50,&topaz8,"       Moves so far",&anykey};    
struct IntuiText s1 = {1,2,JAM2,0,30,&topaz8,"       You have done",&ntext};

struct IntuiText end4 = {1,2,JAM2,0,50,&topaz8,"          Moves",&anykey};
struct IntuiText end3 = {1,2,JAM2,0,30,&topaz8,"          with ",&ntext};
struct IntuiText end2 = {1,2,JAM2,0,20,&topaz8,"You  have solved the problem",&end3};    
struct IntuiText end1 = {1,2,JAM2,0,10,&topaz8,"      Congratulations !",&end2};

struct IntuiText a9 = {1,2,JAM2,0,90,&topaz8," * ENJOY * ENJOY * ENJOY *",&anykey};
struct IntuiText a8 = {1,2,JAM2,0,80,&topaz8,"    All Rights Reserved",&a9};
struct IntuiText a7 = {1,2,JAM2,0,70,&topaz8,"      Copyright © 1993",&a8};
struct IntuiText a6 = {1,2,JAM2,0,60,&topaz8,"          GERMANY",&a7};    
struct IntuiText a5 = {1,2,JAM2,0,50,&topaz8,"        7809 Gutach",&a6};
struct IntuiText a4 = {1,2,JAM2,0,40,&topaz8,"        Elzstr. 42",&a5};
struct IntuiText a3 = {1,2,JAM2,0,30,&topaz8,"    Karlheinz Klingbeil",&a4};
struct IntuiText a2 = {1,2,JAM2,0,20,&topaz8,"    to you by CEKASOFT",&a3};    
struct IntuiText a1 = {1,2,JAM2,0,10,&topaz8,"This weird Game is brought",&a2};


    
int startClicked( void )
{
	/* routine when gadget "Start" is clicked. */
 GT_SetGadgetAttrs(Project0Gadgets[GDX_start],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_tiles],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_shuffle],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_numbers],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                   GA_Disabled,TRUE,TAG_DONE);
 SetMenuStrip( Project0Wnd,NULL); 
 ModifyIDCMP(Project0Wnd,IDCMP_REFRESHWINDOW);
 Play();
 SetMenuStrip( Project0Wnd, Project0Menus );
 ModifyIDCMP(Project0Wnd,IDCMP_VANILLAKEY|IDCMP_MENUPICK | BUTTONIDCMP|CYCLEIDCMP|CHECKBOXIDCMP|IDCMP_MENUPICK|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_start],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_tiles],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_shuffle],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_numbers],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE);
 GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                   GA_Disabled,FALSE,TAG_DONE); 

 return (TRUE);
}

int tilesClicked( void )
{
	/* routine when gadget "Tiles" is clicked. */
 info.res = Project0Msg.Code;
 return (TRUE);
}

int shuffleClicked( void )
{
	/* routine when gadget "Shuffle" is clicked. */
 info.shuffle = atol(shuffle0Labels[Project0Msg.Code]); 
 info.m_shuffles = Project0Msg.Code;
 return (TRUE);
}

int numbersClicked( void )
{
	/* routine when gadget "Numbers" is clicked. */
 info.numbers = (info.numbers) ? FALSE : TRUE;
 GT_SetGadgetAttrs(Project0Gadgets[GDX_numbers],Project0Wnd,NULL,
                   GTCB_Checked,info.numbers,TAG_DONE);

 return (TRUE);
}

int modeClicked( void )
{
	/* routine when gadget "Mode" is clicked. */
 info.mode = (info.mode == MODE_TILES) ? MODE_GAP : MODE_TILES;
 GT_SetGadgetAttrs(Project0Gadgets[GDX_mode],Project0Wnd,NULL,
                   GTCY_Active,info.mode,TAG_DONE);
 return  (TRUE);
}

int Project0loadconfig( void )
{
	/* routine when (sub)item "Load configuration" is selected. */ 
 load_config(NULL);
 return (TRUE);
}

int Project0saveasconfig( void )
{
	/* routine when (sub)item "Save configuration" is selected. */
 save_config(SAVE_AS);
 return (TRUE);
}

int Project0saveconfig( void )
{
	/* routine when (sub)item "Save configuration" is selected. */
 save_config(SAVE_NAME);
 return (TRUE);
}

int Project0savedefault( void )
{
	/* routine when (sub)item "Save as default" is selected. */
 save_config(SAVE_DEF_ENVARC);
 save_config(SAVE_DEF_ENV);
 return (TRUE);
}

int Project0about( void )
{
	/* routine when (sub)item "About..." is selected. */
 EasyRequestArgs(info.wd,&AboutReq,NULL,NULL);
 return(TRUE);
}

int Project0quit( void )
{
	/* routine when (sub)item "Quit" is selected. */
 return (Project0CloseWindow());;
}

int Project0Icon()
{
 info.writeicon = (info.writeicon) ? FALSE : TRUE;
 return (TRUE);
}

int Project0CloseWindow( void )
{
	/* routine for "IDCMP_CLOSEWINDOW". */
 return(EasyRequestArgs(info.wd,&ExitReq,NULL,NULL) == 1) ? FALSE : TRUE;
}

int  MakeScWd(void)
{
 if((info.sc = CloneScreen(Scr)))
 {
  info.oldcmap = info.sc->ViewPort.ColorMap;
  info.sc->ViewPort.ColorMap = Scr->ViewPort.ColorMap;
  if((info.wd = MakeWd(info.sc)))
  {
   CopyWBGfx();
   return (TRUE);
  }
 }
 return (FALSE);
}

void  FreeScWd(void)
{
 if(info.wd)
 {
  CloseWindow(info.wd);
  info.wd = NULL;
 }
 if(info.sc)
 {
  if(info.oldcmap)info.sc->ViewPort.ColorMap = info.oldcmap;

  CloseScreen(info.sc);
  info.sc = NULL;
 }
}

struct Screen *  CloneScreen(struct Screen *wb)
{
 struct DimensionInfo dinfo;
 ULONG  mode_id;
 mode_id = GetVPModeID(&(wb->ViewPort));
 (void)GetDisplayInfoData(NULL,(APTR)&dinfo,sizeof(dinfo),DTAG_DIMS,mode_id); 
 return OpenScreenTags(NULL,
        SA_Width ,wb->Width,
        SA_Height,wb->Height,
        SA_Depth,info.dri->dri_Depth,
        SA_Pens ,info.dri->dri_Pens,
        SA_DisplayID,mode_id,
        SA_FullPalette,TRUE,
        SA_Behind,TRUE,
        SA_DClip,&dinfo.TxtOScan,
        SA_AutoScroll,TRUE,
        TAG_DONE
                      );
}

struct Window  *MakeWd(struct Screen *screen)
{
 return OpenWindowTags(NULL,
        WA_IDCMP,IDCMP_VANILLAKEY | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS,
				    WA_Flags,WFLG_ACTIVATE|WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_SMART_REFRESH,
	       WA_CustomScreen,screen,
        TAG_DONE
        );
}

void Play(void)
{
 int  end = FALSE;
 ULONG sigs,sigrcvd;
 struct IntuiMessage *msg;
 ULONG  class;
 USHORT code;
 WORD   mx,my;

 if(MakeScWd())
 {
  /* SetBPen(info.wd->RPort,1); */
  SetUpValues();
  sigs = (1L<<Project0Wnd->UserPort->mp_SigBit) | 
         (1L<<info.wd->UserPort->mp_SigBit);
  DisplayReq(&intro1);
  while(!end)
  {
   sigrcvd = Wait(sigs);
   if(sigrcvd & (1L<<Project0Wnd->UserPort->mp_SigBit))end = TRUE;
   if(sigrcvd & (1L<<info.wd->UserPort->mp_SigBit))
   {
    while((msg = (struct IntuiMessage *)GetMsg(info.wd->UserPort)))
    {
     class = msg->Class;
     code  = msg->Code;
     mx    = msg->MouseX;
     my    = msg->MouseY; 
     ReplyMsg((struct Message *)msg);
     switch(class)
     {
      case IDCMP_VANILLAKEY   :
           switch(code)
           {
            case 'a'    :
            case 'A'    :
                          DisplayReq(&a1); 
                          break;
            case 'h'    :
            case 'H'    :
                          DisplayReq(&h1);
                          break;
            case 's'    :
            case 'S'    :
                          ntext.TopEdge  = 40;
                          ntext.LeftEdge = 40;
                          ntext.NextText = &s2;
                          sprintf(number,"%3ld",(long)info.moves);
                          DisplayReq(&s1);
                          break;
            case 0x1b   :
            case 'Q'    :
            case 'q'    :
                          if( EasyRequestArgs(info.wd,&ExitReq,NULL,NULL) == 1)
                           end = TRUE;
                          break;
            default     : break;
           }
           break;
      case IDCMP_RAWKEY :
           switch(code)
           {
            case 0x5f   :
                          DisplayReq(&h1);
                          break;
            case 0x45   :
                          end = TRUE;
                          break;
            case 0x4c   :
                          if(info.mode == MODE_GAP)
                           MoveRect(DIR_UP);
                          else
                           MoveRect(DIR_DOWN);
                          break;
            case 0x4d   :
                          if(info.mode == MODE_GAP)
                           MoveRect(DIR_DOWN);
                          else
                           MoveRect(DIR_UP);
                          break;
            case 0x4f   :
                          if(info.mode == MODE_GAP)
                           MoveRect(DIR_LEFT);
                          else
                           MoveRect(DIR_RIGHT);
                          break;
            case 0x4e   :
                          if(info.mode == MODE_GAP)
                           MoveRect(DIR_RIGHT);
                          else
                           MoveRect(DIR_LEFT);
                          break;
            default     :
                          break;
           }
        break;
      case IDCMP_MOUSEBUTTONS :
           if((mx < info.x_pos[info.cx]) && (mx > info.x_pos[info.cx - 1]))
           {
            if((my > info.y_pos[info.cy]) && (my <  info.y_pos[info.cy + 1]))
            {
              MoveRect(DIR_LEFT);
            }
           }  
           if((mx > info.x_pos[info.cx + 1]) && (mx < info.x_pos[info.cx + 2]))
           {
            if((my > info.y_pos[info.cy]) && (my <  info.y_pos[info.cy + 1]))
            {
              MoveRect(DIR_RIGHT);
            }
           } 
           if((mx > info.x_pos[info.cx]) && (mx < info.x_pos[info.cx + 1]))
           {
            if((my < info.y_pos[info.cy]) && (my > info.y_pos[info.cy - 1]))
            {
              MoveRect(DIR_UP);
            }
           }        
           if((mx > info.x_pos[info.cx] ) && (mx < info.x_pos[info.cx + 1]))
           {
            if((my > info.y_pos[info.cy  + 1 ]) && (my < info.y_pos[info.cy + 2]))
            {
              MoveRect(DIR_DOWN);
            }
           }
                        break;
      default           :break;
     }
    }
               if(_solved())
           {
            CopyWBGfx();
            ntext.TopEdge  = 40;
            ntext.LeftEdge = 70;
            ntext.NextText = &end4;
            sprintf(number,"%3ld",(long)info.moves);
            DisplayReq(&end1);
            ScreenToBack(info.sc);
            end = TRUE;
           }
  
   }
  }
 }
 FreeScWd();     
}

void  CopyWBGfx(void)
{
 ClipBlit(&(Scr->RastPort),0,0,info.wd->RPort,0,0,Scr->Width,Scr->Height,0xc0);
 WaitBlit();
 ScreenToFront(info.sc);
}

void SetUpValues(void)
{
 UWORD w,h;
 UWORD i,v,n;
 setmem(&info.x_pos,sizeof(info.x_pos),'\0');
 setmem(&info.y_pos,sizeof(info.y_pos),'\0');
 setmem(&info.solved,sizeof(info.solved),'\0');
 setmem(&info.current,sizeof(info.current),'\0');
 w = (UWORD)(Scr->Width  / info.resolution[info.res].x_res);
 h = (UWORD)(Scr->Height / info.resolution[info.res].y_res);
 info.w =  w;
 info.h =  h;
 if(((w >= 64) && (h >= 64)) || (info.dri->dri_Depth > 4))
 {
  info.size_x = w; info.size_y = h; 
  info.step_x = 1; info.step_y = 1;
 }
 else
 {
  info.size_x = 1; info.size_y = 1; 
  info.step_x = w; info.step_y = h;
 }
 for(i = 0;i <= info.resolution[info.res].x_res;i++)
    info.x_pos[i] = w * i;
 for(i = 0;i <= info.resolution[info.res].y_res;i++)
    info.y_pos[i] = h * i;
 n = 1;
 bp1[0] =  0     ; bp1[1] = h - 1; 
 bp1[2] =  0     ; bp1[3] = 0; 
 bp1[4] =  w - 1 ; bp1[5] = 0; 
 bp2[0] =  w - 1 ; bp2[1] = 0; 
 bp2[2] =  w - 1 ; bp2[3] = h - 1; 
 bp2[4] =  0     ; bp2[5] = h - 1;
 for(i = 0;i < info.resolution[info.res].x_res;i++)
 {
  for(v = 0;v < info.resolution[info.res].y_res;v++,n)
  {
   info.solved[i][v]  = n;
   info.current[i][v] = n;
   n++;
  }
 }  
 SliceBoard();
 shuffle(info.shuffle);
 info.moves = 0;
}

void SliceBoard(void)
{
 int i,j,n;
 struct RastPort   *rp = info.wd->RPort;
 SetAPen(rp,1);
 SetBPen(rp,1);
 n = 1;
 ntext.LeftEdge = 0;
 ntext.TopEdge  = 0;
 ntext.NextText = NULL;
 for(j = 0;j < info.resolution[info.res].y_res;j++)
 {
  for(i = 0;i < info.resolution[info.res].x_res;i++)
  {
   DrawBorder(rp,&b1,info.x_pos[i],info.y_pos[j]);
   sprintf(number,"%3ld",(long)n);
   if(info.numbers)PrintIText(rp,&ntext,info.x_pos[i] + ( info.w / 2),
                                        info.y_pos[j] + ( info.h / 2));
   n++;
  }
 }  
 info.cx = RangeRand(info.resolution[info.res].x_res - 1);
 info.cy = RangeRand(info.resolution[info.res].y_res - 1 );
 RectFill(rp,info.x_pos[info.cx],info.y_pos[info.cy],info.x_pos[info.cx + 1] - 1,info.y_pos[info.cy + 1] - 1);
}   

void shuffle(UWORD howmuch)
{
 UWORD i,n;
 info.old_dir = DIR_UP;
 for(i = 0;i < howmuch;i++)
 {
  while((n = (UWORD)RangeRand(4)) != info.dirs[info.old_dir].opposite)
  {
   MoveRect(n);
   info.old_dir = n;
  }
 } 
}

void MoveRect(UWORD to)
{
 int i;
 struct RastPort *rp = info.wd->RPort;
 switch(to)
 {
  case DIR_UP  : if(info.cy > 0)
                {
                 swap(to);
                 info.cy -= 1;
                 for(i=0;i < info.step_y ; i++)
                 ScrollRaster(rp,0L,- info.size_y,
                              info.x_pos[info.cx],
                              info.y_pos[info.cy],
                              info.x_pos[info.cx + 1] - 1,
                              info.y_pos[info.cy + 2] - 1
                             );
                }
                break;
 case DIR_DOWN : if(info.cy < info.resolution[info.res].y_res - 1 )
                {
                 for(i=0;i < info.step_y;i++)
                 ScrollRaster(rp,0L,info.size_y,
                              info.x_pos[info.cx],
                              info.y_pos[info.cy],
                              info.x_pos[info.cx + 1] - 1,
                              info.y_pos[info.cy + 2] - 1
                             );
                swap(to);
                info.cy  += 1;
                }
                break;
  case DIR_LEFT : if(info.cx > 0)
                {
                 swap(to);
                 info.cx -= 1;
                 for(i=0;i < info.step_x;i++)
                 ScrollRaster(rp,- info.size_x,0,
                              info.x_pos[info.cx],
                              info.y_pos[info.cy],
                              info.x_pos[info.cx + 2] - 1,
                              info.y_pos[info.cy + 1] - 1
                             );
                }
                break;
  case DIR_RIGHT : if(info.cx < info.resolution[info.res].x_res - 1)
                {
                 for(i=0;i < info.step_x;i++)
                 ScrollRaster(rp,info.size_x,0,
                              info.x_pos[info.cx],
                              info.y_pos[info.cy],
                              info.x_pos[info.cx + 2] - 1,
                              info.y_pos[info.cy + 1] - 1
                             );
                 swap(to);
                 info.cx += 1;
                }
                break;  
 }
}

int  _solved(void)
{
 return(!(memcmp(info.solved,info.current,sizeof(info.solved))));
}

void swap(UWORD to)
{
 UWORD t;
 info.moves += 1;
 switch(to)
 {
  case DIR_UP     : t = info.current[info.cx][info.cy];
                    info.current[info.cx][info.cy] = info.current[info.cx][info.cy - 1];
                    info.current[info.cx][info.cy - 1] = t;
                    break;
  case DIR_DOWN   : t = info.current[info.cx][info.cy];
                    info.current[info.cx][info.cy] = info.current[info.cx][info.cy + 1];
                    info.current[info.cx][info.cy + 1] = t;
                    break;
  case DIR_LEFT   : t = info.current[info.cx][info.cy];
                    info.current[info.cx][info.cy] = info.current[info.cx - 1][info.cy];
                    info.current[info.cx - 1][info.cy] = t;
                    break;
  case DIR_RIGHT  : t = info.current[info.cx][info.cy];
                    info.current[info.cx][info.cy] = info.current[info.cx + 1][info.cy];
                    info.current[info.cx + 1][info.cy] = t;
                    break;
 }
}

int Project0VanillaKey()
{
 int rc = TRUE;
 switch(Project0Msg.Code)
 {
  case 'Q'    :
  case 'q'    : rc = Project0CloseWindow();
                break;
  case 's'    :
  case 'S'    : startClicked();
                break;
  case 'n'    :
  case 'N'    : numbersClicked();
                break;
  case 't'    :
  case 'T'    : if(info.res == NUMTILES - 1)
                   info.res = 0;
                else
                   info.res++;
                GT_SetGadgetAttrs(Project0Gadgets[GDX_tiles],Project0Wnd,NULL,
                                  GTCY_Active,info.res,TAG_DONE);
                break;
  case 'f'    :
  case 'F'    : if(info.m_shuffles == NUMSHUFFLES - 1)
                   info.m_shuffles = 0;
                else
                   info.m_shuffles++;
                info.shuffle = atol(shuffle0Labels[info.m_shuffles]); 
                GT_SetGadgetAttrs(Project0Gadgets[GDX_shuffle],Project0Wnd,NULL,
                                  GTCY_Active,info.m_shuffles,TAG_DONE);
                break;
  case 'm'    :
  case 'M'    : modeClicked();
                break;
  default     : break;
 }
 return rc;
}

void DisplayReq(struct IntuiText *text)
{
 struct Message *msg;
 struct Window *hwd;
 if((hwd = OpenWindowTags(NULL,
          WA_Left,(ULONG)(info.sc->Width  / 2 - 120),
          WA_Top ,(ULONG)(info.sc->Height / 2 - 60),
          WA_Width,240,
          WA_Height,120,
          WA_IDCMP,IDCMP_MOUSEBUTTONS | IDCMP_VANILLAKEY,
          WA_Flags,WFLG_ACTIVATE|WFLG_BORDERLESS|WFLG_SMART_REFRESH | WFLG_RMBTRAP,
          WA_CustomScreen,info.sc,
          TAG_DONE
 )))
 {
  SetRast(hwd->RPort,2);
  SetAPen(hwd->RPort,1);
  Move(hwd->RPort,2,2);
  Draw(hwd->RPort,236,2);
  Draw(hwd->RPort,236,116);
  Draw(hwd->RPort,2,116);
  Draw(hwd->RPort,2,2);
  PrintIText(hwd->RPort,text,6,6);
  WaitPort(hwd->UserPort);
  while((msg = GetMsg(hwd->UserPort)))ReplyMsg(msg);
  CloseWindow(hwd);  
 }
}
