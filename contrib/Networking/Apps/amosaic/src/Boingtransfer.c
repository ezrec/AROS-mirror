/*
** BoingTransfer - A MUI Custom Class
** © 1995, Ruediger Sopp
** displays a rotating boing ball
** $VER: BoingTransferClass 1.5 (19.03.95)
*/
#include "includes.h"
#include "globals.h"

void fail(APTR, char *);

#define min(x,y) (x<y?x:y)

#define WINKELFAKTOR (0.4)

#define VERTMOVE (16)
#define VERTMIDPOS (6)

//#define AlignValue(a) (16*(a%16?(a/16)+1:a/16))

#define AlignValue(a) 160

#define RESET_ANIM data->Position=-1;data->LeftDirection=FALSE;data->HorizPos=-10;data->VertPos=VERTMIDPOS;

#include "Boingtransfer.h"

extern struct ExecBase *SysBase;

/// Global stuff
/* These are some sin-/cos-tables to improve rendering speed */

double XSinTab[36]={0.00000000000000,-0.19509032201613,-0.38268343236509,-0.55557023301960,-0.70710678118655,-0.83146961230255,-0.92387953251129,-0.98078528040323,-1.00000000000000,-0.98078528040323,-0.92387953251129,-0.83146961230254,-0.70710678118655,-0.55557023301960,-0.38268343236509,-0.19509032201613,-0.00000000000000,0.19509032201613,0.38268343236509,0.55557023301960,0.70710678118655,0.83146961230255,0.92387953251129,0.98078528040323,1.00000000000000,0.98078528040323,0.92387953251129,0.83146961230255,0.70710678118655,0.55557023301960,0.38268343236509,0.19509032201613,0.00000000000000,-0.19509032201613,-0.38268343236509,-0.55557023301960};
double YCosTab[9]={0,0.3826834,0.7071067,0.9238795,1,0.9238795,0.7071067,0.3826834,0};
double YSinTab[9]={-1,-0.8938794,-0.7071067,-0.3826834,0,0.3826834,0.7071067,0.9238795,1};
double VertMoveTab[16]={0.000000,0.019215,0.076120,0.168530,0.292893,0.444430,0.617317,0.804910,1.000000,0.804910,0.617317,0.444430,0.292893,0.168530,0.076120,0.019215,};

struct BoingTransferMessage
{
  struct Message ExecMessage;
  ULONG Action;
  APTR App,Class;
};
///
///SendChildTaskAction
void SendChildTaskAction(struct BoingTransferClData *data,Object *obj,ULONG Action)
{
  APTR app = NULL;
  char PortName[30];      
  struct BoingTransferMessage MsgSend;
  struct MsgPort *ChildPort,*ReplyPort;

  sprintf(PortName,"BoingPort.%p",data->AutoMoveProc);
  if(ReplyPort=CreateMsgPort())
  {
    get(obj,MUIA_ApplicationObject,&app);
    ReplyPort->mp_Node.ln_Pri=0;
    ReplyPort->mp_Node.ln_Name="BoingReplyPort";
    ReplyPort->mp_SigTask=(APTR)FindTask(0);
    AddPort(ReplyPort);
    MsgSend.ExecMessage.mn_Length=sizeof(struct Message);
    MsgSend.ExecMessage.mn_Node.ln_Type=NT_MESSAGE;
    MsgSend.ExecMessage.mn_ReplyPort=ReplyPort;
    MsgSend.Action=Action;
    MsgSend.App=app;
    MsgSend.Class=obj;
    Forbid();
    if(ChildPort=(struct MsgPort *)FindPort(PortName))
    {
      PutMsg(ChildPort,(struct Message *)&MsgSend);
    }
    Permit();
    WaitPort(ReplyPort);
    RemPort(ReplyPort);
    DeletePort(ReplyPort);
  }
}       
///
///AutoMove
static void SAVEDS AutoMove(void)
{
  APTR App = NULL,Obj = NULL;
  char PortName[30];
  struct MsgPort *Port;

  sprintf(PortName,"BoingPort.%p",SysBase->ThisTask);
  if(Port=CreateMsgPort())
  {
    BOOL running=TRUE;
    struct BoingTransferMessage *Msg;
    Port->mp_Node.ln_Pri=0;
    Port->mp_Node.ln_Name=PortName;
    Port->mp_SigTask=(APTR)FindTask(0);
    AddPort(Port);
    WaitPort(Port);
    while(Msg=(struct BoingTransferMessage *)GetMsg(Port))
    {
      App=Msg->App;
      Obj=Msg->Class;
      ReplyMsg((struct Message *)Msg);
    }     
    while(running)
    {
      DoMethod(App,MUIM_Application_PushMethod,Obj,1,MUIM_BoingTransfer_Move);
      DoMethod(App,MUIM_Application_PushMethod,App,1,MUIM_Application_InputBuffered);
      Delay(6);
      while(Msg=(struct BoingTransferMessage *)GetMsg(Port))
      {
        running=FALSE;
        ReplyMsg((struct Message *)Msg);
      }
    }
    RemPort(Port);
    DeletePort(Port);
  }
}     
///
///CreateAutoMoveProc
void CreateAutoMoveProc(struct BoingTransferClData *data,Object *obj)
{
  char PortName[30];
  if(!data->AutoMoveProc)
  {
    if(data->AutoMoveProc=CreateNewProcTags(NP_Entry,AutoMove,NP_Name,"BoingMover",NP_Priority,-1,TAG_DONE))
    {
      sprintf(PortName,"BoingPort.%p",data->AutoMoveProc);
      while(!FindPort(PortName))
        Delay(1);
      SendChildTaskAction(data,obj,0);
    }
  }
}
///
///BreakAutoMoveProc
void BreakAutoMoveProc(struct BoingTransferClData *data,Object *obj)
{
  if(data->AutoMoveProc)
  {
    SendChildTaskAction(data,obj,0);
    data->AutoMoveProc=NULL;
  }
}
///
///FreeBitMaps
void FreeBitMaps(struct BoingTransferClData *data)
{
  long i;
  WaitBlit();
  for(i=0;i<FRAMES;i++)
  {
    FreeBitMap(data->BitMap[i]);
    data->BitMap[i]=NULL;
  }
  FreeBitMap(data->Mask);
  data->Mask=NULL;
  if(data->Bitplane)
  {
    FreeRaster(data->Bitplane,AlignValue(data->BitMapSize),AlignValue(data->BitMapSize));
    data->Bitplane=NULL;
  }    
}
///
///InitBitMaps
BOOL InitBitMaps(Object *obj,struct BoingTransferClData *data)
{
  ULONG i;
  FreeBitMaps(data);   
  data->BitMapSize=2*data->Radius+4;
  if(data->Bitplane=AllocRaster(AlignValue(data->BitMapSize),AlignValue(data->BitMapSize)))
  {
    InitTmpRas(&data->TmpRaster,data->Bitplane,RASSIZE(AlignValue(data->BitMapSize),AlignValue(data->BitMapSize)));
    InitArea(&data->AInfo,data->Buffer,10);
    if(data->Mask=AllocBitMap(data->BitMapSize,data->BitMapSize,1,0,NULL))
    {
      InitRastPort(&data->MaskRP);
      data->MaskRP.BitMap=data->Mask;
      data->MaskRP.TmpRas=&data->TmpRaster;
      data->MaskRP.AreaInfo=&data->AInfo;         

      InitRastPort(&data->BackRP);

      for(i=0;i<FRAMES;i++)
      {
        if(data->BitMap[i]=AllocBitMap(data->BitMapSize,data->BitMapSize,_screen(obj)->RastPort.BitMap->Depth,0,NULL))
        {
          InitRastPort(&data->RP[i]);
          data->RP[i].BitMap=data->BitMap[i];
          data->RP[i].TmpRas=&data->TmpRaster;
          data->RP[i].AreaInfo=&data->AInfo;         
        }
        else
          return(FALSE);
      }
    }
    else
      return(FALSE);
  }
  else
    return(FALSE);
  return(TRUE);
}
///
///GetXYPos: turn 3d coordinates into 2d coordinates
static void __inline __regargs GetXYPos(double Offset,long x,long y,double Radius,long *ResX,long *ResY)
{
  double rx,ry;
  double XAbstand, YAbstand;
  
#ifdef __AROS__
// FIXME: otherwise crash because of -1 coords passed to Area functions
  Radius -= 1.0;
#endif

  XAbstand=XSinTab[x]*Radius*YCosTab[y];
  YAbstand=YSinTab[y]*(double)Radius;

//  double XAbstand=sin(x*PI/16+PID2)*Radius*YCosTab[y],YAbstand=YSinTab[y]*(double)Radius;

  rx=Offset+XAbstand-YAbstand*WINKELFAKTOR;
  ry=Offset+YAbstand+XAbstand*WINKELFAKTOR;
  *ResX=rx;
  *ResY=ry;
}
///
///DrawBoingBall
static void __inline DrawBoingBall(struct RastPort *RP,double Radius,struct BoingTransferClData *data,Object *obj,long WinkelOffset,long RedPen,long WhitePen)
{
  long x,y,xw,ResX,ResY;

  for(x=0;x<16;x++)
  {
    xw=x*2+WinkelOffset;
    if(xw+2>24||xw<8)
    {
      for(y=0;y<8;y++)
      {            
        SetAPen(RP,(y+x)/2*2==(y+x)?RedPen:WhitePen);

        GetXYPos(Radius,xw,y,Radius,&ResX,&ResY);
        AreaMove(RP,ResX,ResY);

        GetXYPos(Radius,xw+2,y,Radius,&ResX,&ResY);
        AreaDraw(RP,ResX,ResY);

        GetXYPos(Radius,xw+2,y+1,Radius,&ResX,&ResY);
        AreaDraw(RP,ResX,ResY);

        GetXYPos(Radius,xw,y+1,Radius,&ResX,&ResY);
        AreaDraw(RP,ResX,ResY);

        AreaEnd(RP);
      }
    }
  }
}
///
///CalculateBoingBall: render the frames of the animation
void __regargs CalculateBoingBall(struct BoingTransferClData *data,Object *obj)
{
  long i;

  if(InitBitMaps(obj,data))
  {
    SetAPen(&data->MaskRP,0);
    RectFill(&data->MaskRP,0,0,2*(ULONG)data->Radius,2*(ULONG)data->Radius);
    SetAPen(&data->MaskRP,1);
    AreaEllipse(&data->MaskRP,(long)data->Radius,(long)data->Radius,(long)data->Radius,(long)data->Radius);
    AreaEnd(&data->MaskRP);

    for(i=0;i<FRAMES;i++)
    {
      DrawBoingBall(&data->RP[i],data->Radius,data,obj,i,data->RedPen,data->WhitePen);
    }
  }
  else
  {
    FreeBitMaps(data);
  }
}
///
///BoingTransferAskMinMax
SAVEDS static IPTR BoingTransferAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
  DoSuperMethodA(cl,obj,(Msg)msg);

  msg->MinMaxInfo->MinWidth  += 30;
  msg->MinMaxInfo->DefWidth  += 50;
  msg->MinMaxInfo->MaxWidth  += 80; // 110;

  msg->MinMaxInfo->MinHeight += 30;
  msg->MinMaxInfo->DefHeight += 50;
  msg->MinMaxInfo->MaxHeight += 64; // 110;

  return(0);
}
///
///BoingTransferSetup
SAVEDS static IPTR BoingTransferSetup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  struct BoingTransferClData *data = INST_DATA(cl,obj);
  if(!(DoSuperMethodA(cl,obj,(Msg)msg)))
    return(FALSE);

  data->RedPen=ObtainBestPen(_screen(obj)->ViewPort.ColorMap,0xff<<24,0,0,TAG_DONE);
  if(data->RedPen!=-1)
  {
    data->WhitePen=ObtainBestPen(_screen(obj)->ViewPort.ColorMap,0xff<<24,0xff<<24,0xff<<24,TAG_DONE);
    if(data->WhitePen!=-1)
    {
      RESET_ANIM;
      return(TRUE);
    }
    ReleasePen(_screen(obj)->ViewPort.ColorMap,data->RedPen);
  }
  return(FALSE);
}
///
///BoingTransferCleanup
SAVEDS static IPTR BoingTransferCleanup(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  struct BoingTransferClData *data = INST_DATA(cl,obj);

  if(data->RedPen!=-1)
    ReleasePen(_screen(obj)->ViewPort.ColorMap,data->RedPen); 
  if(data->WhitePen!=-1)
    ReleasePen(_screen(obj)->ViewPort.ColorMap,data->WhitePen);
  data->RedPen=data->WhitePen=-1;

  FreeBitMaps(data);

  data->Radius=0;

  return(DoSuperMethodA(cl,obj,(Msg)msg));
}
///
///BoingTransferShow
SAVEDS static IPTR BoingTransferShow(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  struct BoingTransferClData *data = INST_DATA(cl,obj);
  if(!(DoSuperMethodA(cl,obj,(Msg)msg)))
    return(FALSE);

  if(data->Temp=AllocBitMap(_mwidth(obj),_mheight(obj),_screen(obj)->RastPort.BitMap->Depth,BMF_INTERLEAVED,_rp(obj)->BitMap))
  {
    InitRastPort(&data->TempRP);
    data->TempRP.BitMap=data->Temp;  
    if(data->AutoMove)
    {
      CreateAutoMoveProc(data,obj);
    }

    return(TRUE);
  }       

  return(FALSE);
}
///
///BoingTransferHide
SAVEDS  ULONG BoingTransferHide(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg)
{
  struct BoingTransferClData *data = INST_DATA(cl,obj);

  BreakAutoMoveProc(data,obj);

  WaitBlit();
  FreeBitMap(data->Temp);
  data->Temp=NULL;
  FreeBitMap(data->Back);
  data->Back=NULL;
 
  return(DoSuperMethodA(cl,obj,(Msg)msg));
}
///  
/// BoingTransferSet
SAVEDS static IPTR BoingTransferSet(struct IClass *cl,Object *obj,Msg msg)
{
  struct BoingTransferClData *data = INST_DATA(cl,obj);
  const struct TagItem *tags;
  struct TagItem *tag;

  for (tags=((struct opSet *)msg)->ops_AttrList;tag=NextTagItem(&tags);)
  {
    switch (tag->ti_Tag)
    {
      case MUIA_BoingTransfer_AutoMove:
        if(tag->ti_Data)
        {
          data->AutoMove=TRUE;
          CreateAutoMoveProc(data,obj);
        }
        else
        {
          data->AutoMove=FALSE;
          BreakAutoMoveProc(data,obj);
        }
        break;
    }
  }

  return(DoSuperMethodA(cl,obj,msg));
} 
///
///BoingTransferDraw
SAVEDS static IPTR BoingTransferDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
  long LeftEdge,TopEdge,Frame;
  struct BoingTransferClData *data = INST_DATA(cl,obj);
  DoSuperMethodA(cl,obj,(Msg)msg);

  if(msg->flags&MADF_DRAWOBJECT)
  {
    if(data->Temp)
    {
      double Radius;
      Radius=min(_mwidth(obj),_mheight(obj))/2.7;
      if(data->HorizPos>_mwidth(obj)-1-2*(long)Radius)
        data->HorizPos=_mwidth(obj)-1-2*(long)Radius;
      if(data->Radius!=Radius)
      {
        data->Radius=Radius;
        CalculateBoingBall(data,obj);
      }
      if(data->BitMap[FRAMES-1])
      {
        FreeBitMap(data->Back);
        data->Back=NULL;
        if(data->Back=AllocBitMap(_mwidth(obj),_mheight(obj),_screen(obj)->RastPort.BitMap->Depth,0,NULL))
        {
          data->BackRP.BitMap=data->Back;
          ClipBlit(_rp(obj),_mleft(obj),_mtop(obj),&data->BackRP,0,0,_mwidth(obj),_mheight(obj),0xc0);
        }
      }
    }
  }

  if(data->Position<0)
  {
    LeftEdge=_mwidth(obj)/2-data->Radius;
    TopEdge=_mheight(obj)/2-data->Radius;
    data->HorizPos=_mwidth(obj)/2-data->Radius+1;
    data->Position=1;
    Frame=0;
  }
  else
  {
    LeftEdge=data->HorizPos;
    TopEdge=VertMoveTab[data->VertPos]*(_mheight(obj)-2*data->Radius);
    Frame=data->Position;
  }

  if(data->Back)
  {
    BltBitMapRastPort(data->Back,0,0,&data->TempRP,0,0,_mwidth(obj),_mheight(obj),0xc0);
    BltMaskBitMapRastPort(data->BitMap[Frame],0,0,&data->TempRP,LeftEdge,TopEdge,2*(ULONG)data->Radius+1,2*(ULONG)data->Radius+1,0xe0,data->Mask->Planes[0]);
    ClipBlit(&data->TempRP,0,0,_rp(obj),_mleft(obj),_mtop(obj),_mwidth(obj),_mheight(obj),0xc0);
  }

  return(0);
}
///
///BoingTransferDispatcher
DISPATCHER(BoingTransferDispatcher)
{
  switch(msg->MethodID)
  {
    case MUIM_AskMinMax  : return(BoingTransferAskMinMax  (cl,obj,(APTR)msg));  
    case MUIM_Setup      : return(BoingTransferSetup      (cl,obj,(APTR)msg));
    case MUIM_Cleanup    : return(BoingTransferCleanup    (cl,obj,(APTR)msg));  
    case MUIM_Show       : return(BoingTransferShow       (cl,obj,(APTR)msg));
    case MUIM_Hide       : return(BoingTransferHide       (cl,obj,(APTR)msg));
    case MUIM_Draw       : return(BoingTransferDraw       (cl,obj,(APTR)msg));
    case OM_SET          : return(BoingTransferSet        (cl,obj,(APTR)msg));
    case MUIM_BoingTransfer_Move:
    {
      struct BoingTransferClData *data = INST_DATA(cl,obj);

      MUI_Redraw(obj,MADF_DRAWUPDATE);

      if(data->LeftDirection)
      {
        data->Position--;
        if(data->Position<0)
          data->Position=FRAMES-1;
        data->HorizPos--;
        if(data->HorizPos<0)
        {
          data->HorizPos=0;
          data->LeftDirection=FALSE;
        }                                        
      }           
      else
      {
        data->Position++;
        if(data->Position>FRAMES-1)
          data->Position=0;
        data->HorizPos++;
        if(data->HorizPos>_mwidth(obj)-2*data->Radius)
        {
          data->HorizPos=_mwidth(obj)-2*data->Radius;
          data->LeftDirection=TRUE;
        }                        
      }                         

      data->VertPos++;
      if(data->VertPos>VERTMOVE-1)
        data->VertPos=0;
      return(0);
    }
    case MUIM_BoingTransfer_Reset:
    {
      struct BoingTransferClData *data = INST_DATA(cl,obj);
      RESET_ANIM;
      MUI_Redraw(obj,MADF_DRAWUPDATE);
      return(0);
    }
  }

  return(DoSuperMethodA(cl,obj,(Msg)msg));
}
///

///BoingTransferClInit
// ************************************************************
// Initilize the Boing class
// ************************************************************
Class *BoingTransferClInit(void)
{
  APTR SuperClass;
  Class *cl = NULL;

  if (!(SuperClass = MUI_GetClass(MUIC_Area)))
    fail(NULL, "Superclass for Boing class not found.");

  if (!(cl = MakeClass(NULL, NULL, SuperClass, sizeof(struct BoingTransferClData), 0)))
    {
      MUI_FreeClass(SuperClass);
      fail(NULL, "Failed to create Boing class.");
    }

  cl->cl_Dispatcher.h_Entry = ENTRY(BoingTransferDispatcher);
  cl->cl_Dispatcher.h_SubEntry = NULL;
  cl->cl_Dispatcher.h_Data = NULL;

  return cl;
}
///
///BoingTransferClFree
// *******************************************************
// Free the BoingTransfer class
// IS THERE AN ERROR HERE BECAUSE SUPERCLASS IS NEVER FREED? Yep! (sb)
// *******************************************************
BOOL BoingTransferClFree(Class *cl)
{
  FreeClass(cl);
  return 0;
} 
///

