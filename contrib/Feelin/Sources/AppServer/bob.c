#include "Private.h"

#if 0
///Bob_Create
struct FeelinBob * Bob_Create(struct RastPort *RPort,struct ViewPort *VPort,ULONG Width,ULONG Height,LONG Transp)
{
   struct FeelinBob *Bob;

   if (!RPort)
   {
      F_Log(FV_LOG_DEV,"RastPort is NULL"); return NULL;
   }
   if (!VPort)
   {
      F_Log(FV_LOG_DEV,"ViewPort is NULL"); return NULL;
   }

   if ((Bob = F_New(sizeof (struct FeelinBob))) != NULL)
   {
      ULONG word_aligned_width = (Width + 15) >> 4;
      ULONG depth = RPort -> BitMap -> Depth;
 
      Bob -> MainView = VPort;
      Bob -> Width = Width;
      Bob -> Height = Height;
      Bob -> chipsize = sizeof (WORD) * word_aligned_width * Height * depth + // vsprite_imagedata
                        sizeof (WORD) * word_aligned_width + // vsprite_borderline
                        sizeof (WORD) * word_aligned_width * Height + // vsprite_collmask
                        sizeof (WORD) * word_aligned_width * Height * depth; // bob_savebuffer

      if ((Bob -> chipdata = AllocMem(Bob -> chipsize,MEMF_CHIP | MEMF_CLEAR)) != NULL)
      {
         ULONG i;
         
         /* Setup struct GelsInfo. This structure will be associated to our
         cloned rastport to handle our Bob */
 
         Bob -> gel_info.sprRsrvd   = 0x03;
         Bob -> gel_info.Flags      = 0;
         Bob -> gel_info.gelHead    = &Bob -> gel_vshead;
         Bob -> gel_info.gelTail    = &Bob -> gel_vstail;
         Bob -> gel_info.nextLine   = (WORD *)(&Bob -> gel_nextline);
         Bob -> gel_info.lastColor  = (WORD **)(&Bob -> gel_lastcolor);
         Bob -> gel_info.collHandler= &Bob -> gel_colltable;
         Bob -> gel_info.leftmost   = 1;
         Bob -> gel_info.topmost    = 1;
         Bob -> gel_info.rightmost  = (RPort -> BitMap-> BytesPerRow << 3) - 1;
         Bob -> gel_info.bottommost = RPort -> BitMap -> Rows - 1;

         InitGels(&Bob -> gel_vshead,&Bob -> gel_vstail,&Bob -> gel_info);

         /* We need to clone the RastPort to be completely  safe  (in  case
         another struct GelsInfo is already defined) */

         InitRastPort(&Bob -> MainRast);
         Bob -> MainRast.BitMap = RPort -> BitMap;
         Bob -> MainRast.GelsInfo = &Bob -> gel_info;

         /* Setup our struct VSprite */

         Bob -> vsprite.Flags      = (Transp) ? (SAVEBACK | OVERLAY) : SAVEBACK;
         Bob -> vsprite.Y          = -9999;
         Bob -> vsprite.X          = -9999;
         Bob -> vsprite.Height     = Height;
         Bob -> vsprite.Width      = word_aligned_width;
         Bob -> vsprite.Depth      = depth;
         Bob -> vsprite.MeMask     = 0;
         Bob -> vsprite.HitMask    = 0;
         Bob -> vsprite.ImageData  = (APTR)((ULONG)(Bob -> chipdata));
         Bob -> vsprite.BorderLine = (APTR)((ULONG)(Bob -> vsprite.ImageData) + sizeof (UWORD) * word_aligned_width * Height * depth);
         Bob -> vsprite.CollMask   = (APTR)((ULONG)(Bob -> vsprite.BorderLine) + sizeof (UWORD) * word_aligned_width);
         Bob -> vsprite.SprColors  = NULL;
         Bob -> vsprite.VSBob      = &Bob -> bob;
         Bob -> vsprite.PlanePick  = -1;
         Bob -> vsprite.PlaneOnOff = 0x00;

         InitMasks(&Bob -> vsprite);
         
         /* Setup our struct Bob */

         Bob -> bob.Flags          = 0;
         Bob -> bob.SaveBuffer     = (APTR)((ULONG)(Bob -> vsprite.CollMask) + sizeof (UWORD) * word_aligned_width * Height);
         Bob -> bob.ImageShadow    = Bob -> vsprite.CollMask;
         Bob -> bob.Before         = NULL;
         Bob -> bob.After          = NULL;
         Bob -> bob.BobVSprite     = &Bob -> vsprite;
         Bob -> bob.BobComp        = NULL;
         Bob -> bob.DBuffer        = NULL;

         /* Now setup the RastPort and BitMap so we can render into the Bob
         with common graphics.library functions */

         InitRastPort(&Bob -> rport);
         InitBitMap(&Bob -> bitmap,depth,Width,Height);

         Bob -> rport.BitMap = &Bob -> bitmap;

         for (i = 0 ; i < depth ; i++)
         {
            Bob -> bitmap.Planes[i] = (APTR)((ULONG)(Bob -> vsprite.ImageData) + i * sizeof (UWORD) * word_aligned_width * Height);
         }

         return Bob;
      }
      F_Dispose(Bob);
   }
   return NULL;
}
//+
///Bob_Delete
void Bob_Delete(struct FeelinBob *Bob)
{
   FreeMem(Bob -> chipdata,Bob -> chipsize);
   F_Dispose(Bob);
}
//+
///Bob_DrawGList
void Bob_DrawGList(struct FeelinBob *Bob)
{
   SortGList(&Bob -> MainRast);
   DrawGList(&Bob -> MainRast,Bob -> MainView);
}
//+
///Bob_Update
void Bob_Update(struct FeelinBob *Bob,UWORD x,UWORD y)
{
   Bob -> bob.BobVSprite -> X = x;
   Bob -> bob.BobVSprite -> Y = y;
   
   Bob_DrawGList(Bob);
}
//+
///Bob_MakeGhost
void Bob_MakeGhost(struct FeelinBob *Bob)
{
   struct RastPort *rp = &Bob -> rport;
   static uint16 pattern[] = {0xAAAA,0x5555,0xAAAA,0x5555};

   rp -> AreaPtrn = pattern;
   rp -> AreaPtSz = 2;

   _APen(1);
   _BPen(0);
   _Boxf(0,0,Bob -> Width - 1,Bob -> Height - 1);

   rp -> AreaPtSz = 0;
   rp -> AreaPtrn = NULL;

   InitMasks(&Bob -> vsprite);
}
//+
///Bob_MakeSolid
void Bob_MakeSolid(struct FeelinBob *Bob)
{
   struct RastPort *rp = &Bob -> rport;

   _APen(1);
   _Boxf(0,0,Bob -> Width - 1,Bob -> Height - 1);

   InitMasks(&Bob -> vsprite);
}
//+
///Bob_Add
void Bob_Add(struct FeelinBob *Bob)
{
   AddBob(&Bob -> bob,&Bob -> MainRast);
   Bob_DrawGList(Bob);
}
//+
///Bob_Rem
void Bob_Rem(struct FeelinBob *Bob)
{
   RemBob(&Bob -> bob);
   Bob_DrawGList(Bob);
}
//+
#endif

#define ABS(x) ({ int32 _v = (int32)(x); _v >= 0 ? _v : -_v; })
#define CLIP(x, min, max) ({ \
    int32 _v = (int32)(x), _min = (int32)(min), _max = (int32)(max); \
    _v >= _min ? (_v <= _max ? _v : _max) : _min; })

///xblit
void xblit(struct BitMap *sbm, int32 sx, int32 sy, struct BitMap *dbm, int32 x, int32 y, int32 w, int32 h)
{
    BltBitMap(sbm, sx, sy, dbm, x, y, w, h, 0xc0, -1, NULL);
}
//+
///Bob_Create
struct FeelinBob *
Bob_Create(struct RastPort *RPort, FBox * SourceBox, int32 Transp)
{
    struct FeelinBob *Bob;

    if (!RPort)
    {
        F_Log(FV_LOG_DEV, "RastPort is NULL");
        return NULL;
    }

    Bob = F_New(sizeof(struct FeelinBob));
    
    if (Bob != NULL)
    {
        uint32 depth;
        struct Rectangle rect;

        depth = GetBitMapAttr(RPort->BitMap, BMA_DEPTH);

        rect.MinX = Bob->box.x = 0; //SourceBox->x;
        rect.MinY = Bob->box.y = 0; //SourceBox->y;
        rect.MaxX = (Bob->box.w = SourceBox->w) - 1;
        rect.MaxY = (Bob->box.h = SourceBox->h) - 1;

        /* create backup & bob bitmap */
        
        Bob->backup = AllocBitMap(Bob->box.w, Bob->box.h, depth, BMF_DISPLAYABLE | BMF_MINPLANES, RPort->BitMap);
        Bob->bitmap = AllocBitMap(Bob->box.w, Bob->box.h, depth, BMF_DISPLAYABLE | BMF_MINPLANES, RPort->BitMap);

        if (!(Bob->backup && Bob->bitmap))
        {
            Bob_Delete(Bob);
            return NULL;
        }

        InitRastPort(&Bob->rp);
        InitRastPort(&Bob->brp);
        Bob->rp.BitMap = Bob->bitmap;
        Bob->brp.BitMap = Bob->backup;

        Bob->srp = &Bob->rp;
        Bob->drp = RPort;

        Bob->region = NewRegion();
        if (Bob->region == NULL)
        {
            Bob_Delete(Bob);
            return NULL;
        }

        AndRectRegion(Bob->region, &rect);
        //InstallClipRegion(Bob->srp, Bob->region);
        //InstallClipRegion(&Bob->brp.Layer, Bob->region);

        /* backup screen */
        ClipBlit(Bob->drp, Bob->box.x, Bob->box.y, &Bob->brp, 0, 0, Bob->box.w, Bob->box.h, 0xc0);

        return Bob;
    }
    return NULL;
}

//+
///Bob_Delete
void
Bob_Delete(struct FeelinBob *Bob)
{
    if (Bob->backup)
    {
        int32 cx, cy;

        /* restore screen */
        cx = MAX(Bob->box.x, 0);
        cy = MAX(Bob->box.y, 0);

        xblit(Bob->backup, 0, 0, Bob->drp->BitMap, cx, cy,
            Bob->box.w - cx + Bob->box.x,
            Bob->box.h - cy + Bob->box.y);

        FreeBitMap(Bob->backup);
    }

    if (Bob->region)
    {
        DisposeRegion(Bob->region);
    }

    if (Bob->bitmap)
    {
        FreeBitMap(Bob->bitmap);
    }

#ifdef __AROS__
    DeinitRastPort(&Bob->rp);
    DeinitRastPort(&Bob->brp);
#endif

    F_Dispose(Bob);
}
//+
///Bob_Update
void
Bob_Update(struct FeelinBob *Bob, int32 x, int32 y)
{
    int32 cx, cy, w, h;

    cx = MAX(Bob->box.x, 0);
    cy = MAX(Bob->box.y, 0);
    w = Bob->box.w - cx + Bob->box.x;
    h = Bob->box.h - cy + Bob->box.y;

    xblit(Bob->backup, 0, 0, Bob->drp->BitMap, cx, cy, w, h);

    Bob->box.x = x;
    Bob->box.y = y;

    cx = MAX(Bob->box.x, 0);
    cy = MAX(Bob->box.y, 0);
    w = Bob->box.w - cx + Bob->box.x;
    h = Bob->box.h - cy + Bob->box.y;

    xblit(Bob->drp->BitMap, cx, cy, Bob->backup, 0, 0, w, h);
    xblit(Bob->bitmap, cx - x, cy - y, Bob->drp->BitMap, cx, cy, w, h);
}
//+
///Bob_MakeGhost
void
Bob_MakeGhost(struct FeelinBob *Bob)
{
    #if 0
    struct RastPort *rp = &Bob->rp;
    static uint16 pattern[] = { 0xAAAA, 0x5555, 0xAAAA, 0x5555 };

    rp->AreaPtrn = pattern;
    rp->AreaPtSz = 2;
     
    _APen(1);
    _BPen(0);
    _Boxf(0, 0, Bob->Width - 1, Bob->Height - 1);
 
    rp->AreaPtSz = 0;
    rp->AreaPtrn = NULL;
    #endif
}
//+
///Bob_MakeSolid
void
Bob_MakeSolid(struct FeelinBob *Bob)
{
    #if 0
    struct RastPort *rp = &Bob->rport;

    _APen(1);
    _Boxf(0, 0, Bob->Width - 1, Bob->Height - 1);
    #endif
}
//+

#if 0
struct FeelinBob
{
    FBox                            box;
    struct BitMap                  *source_bm;
}
FBob;

 

 
Bob_Create(struct RastPort *SourceRPort, FBox *SourceBox)
{
    FBob *bob = F_New(sizeof (FBob));

    if (bob)
    {
        bob->box.x = SourceBox->x;
        bob->box.y = SourceBox->y;
        bob->box.w = SourceBox->w;
        bob->box.h = SourceBox->h;
 
        bob->source_bm = AllocBitMap(bob->box.w, bob->box.h, GetBitMapAttr(SourceRPort->BitMap,BMA_DEPTH), BMF_MINPLANES, SourceRPort->BitMap);
        
        if (bob->source_bm)
        {
            BltBitMap(SourceRPort->BitMap, bob->box.x, bob->box.y, bob->source_bm, 0,0, bob->box.w,bob->box.h, 0x0C0, 0xFF, NULL);
            
            FreeBitMap(bob->source_bm);
        }
        F_Dispose(bob);
    }
}
#endif
