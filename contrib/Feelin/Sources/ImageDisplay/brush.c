#include "Private.h"
#include <hardware/blit.h>

#define BIG_ENDIAN_MACHINE 1

#ifdef __AROS__
 #include <aros/macros.h>
 #if !AROS_BIG_ENDIAN
  #undef BIG_ENDIAN_MACHINE
  #define BIG_ENDIAN_MACHINE 0
 #endif
#endif

#include <aros/debug.h>

//#define DB_SHARED_OPEN
//#define DB_SHARED_CLOSE

struct FeelinBrushBitMap
{
   struct FeelinBrushBitMap       *Next;
   uint32                          UserCount;
   FPalette                       *Scheme;
   struct BitMap                  *Mapped;
};

struct FeelinSharedBrush
{
   struct FeelinSharedBrush       *Next;
   uint32                          UserCount;
   struct FileLock                *Lock;
   uint16                          Width;
   uint16                          Height;
   uint16                          AlignedW;
   uint16                          AlignedH;
   uint8                          *Pixels;
   PLANEPTR                        Mask;
   struct FeelinBrushBitMap       *BitMaps;
};

typedef struct in_Image_Brush
{
   struct FeelinSharedBrush        *Shared[2];
   struct FeelinBrushBitMap        *Map[2];
}
FImage;

///id_mask_create

/* Pixels *must* be 32 bits aligned */
 
STATIC void id_mask_create(uint8 *Pixels,uint32 *Raster,uint32 Num)
{
   for ( ; Num ; Num -= 32)
   {
      int8 i;
      bits32 bits = 0;

      for (i = 31 ; i >= 0 ; i--)
      {
         if (*Pixels++)
         {
            bits |= (1 << i);
         }
      }

   #if BIG_ENDIAN_MACHINE
      *Raster++ = bits;
   #else
      *Raster++ = AROS_LONG2BE(bits);
   #endif
   }
}
//+
///id_mask_check
STATIC int32 id_mask_check(uint8 *Pixels,uint32 Num)
{
   for ( ; Num ; Num--)
   {
      if (!*Pixels++)
      {
         return TRUE;
      }
   }
   return FALSE;
}
//+
 
/****************************************************************************
*** Blitting ****************************************************************
****************************************************************************/

///MyBltMaskBitMap
/* A BltBitMaskPort() replacement which blits masks for interleaved bitmaps correctly */
struct LayerHookMsg
{
    struct Layer *layer;
    struct Rectangle bounds;
    int32 offsetx;
    int32 offsety;
};

struct BltMaskHook
{
  struct Hook hook;
  struct BitMap maskBitMap;
  struct BitMap *srcBitMap;
  int32 srcx,srcy;
  int32 destx,desty;
};

STATIC SAVEDS void MyBltMaskBitMap(struct BitMap *srcBitMap, int32 xSrc, int32 ySrc, struct BitMap *destBitMap, int32 xDest, int32 yDest, int32 xSize, int32 ySize, struct BitMap *maskBitMap)
{
  BltBitMap(srcBitMap,xSrc,ySrc,destBitMap, xDest, yDest, xSize, ySize, 0x99,~0,NULL);
  BltBitMap(maskBitMap,xSrc,ySrc,destBitMap, xDest, yDest, xSize, ySize, 0xe2,~0,NULL);
  BltBitMap(srcBitMap,xSrc,ySrc,destBitMap, xDest, yDest, xSize, ySize, 0x99,~0,NULL);
}
//+
///HookFunc_BltMask
STATIC SAVEDS ASM(void) HookFunc_BltMask(REG(a0,struct Hook *hook),REG(a1,struct LayerHookMsg *msg),REG(a2,struct RastPort *rp))
{
  struct BltMaskHook *h = (struct BltMaskHook*)hook;

  int32 width = msg->bounds.MaxX - msg->bounds.MinX+1;
  int32 height = msg->bounds.MaxY - msg->bounds.MinY+1;
  int32 offsetx = h->srcx + msg->offsetx - h->destx;
  int32 offsety = h->srcy + msg->offsety - h->desty;

   MyBltMaskBitMap(h->srcBitMap, offsetx, offsety, rp->BitMap, msg->bounds.MinX, msg->bounds.MinY, width, height, &h->maskBitMap);
}
//+
///MyBltMaskBitMapRastPort
STATIC void MyBltMaskBitMapRastPort(struct BitMap *srcBitMap,int32 xSrc, int32 ySrc, struct RastPort *destRP, int32 xDest, int32 yDest, int32 xSize, int32 ySize, uint32 minterm, APTR bltMask )
{
   if (GetBitMapAttr(srcBitMap,BMA_FLAGS) & BMF_INTERLEAVED)
   {
      int32 src_depth = GetBitMapAttr(srcBitMap,BMA_DEPTH);
      struct Rectangle rect;
      struct BltMaskHook hook;

    /* Define the destination rectangle in the rastport */
    
      rect.MinX = xDest;
      rect.MinY = yDest;
      rect.MaxX = xDest + xSize - 1;
      rect.MaxY = yDest + ySize - 1;

    /* Initialize the hook */
    
      hook.hook.h_Entry = (HOOKFUNC)HookFunc_BltMask;
      hook.srcBitMap = srcBitMap;
      hook.srcx = xSrc;
      hook.srcy = ySrc;
      hook.destx = xDest;
      hook.desty = yDest;

      /* Initialize a bitmap where all plane pointers points to the mask */
   
      InitBitMap(&hook.maskBitMap,src_depth,GetBitMapAttr(srcBitMap,BMA_WIDTH),GetBitMapAttr(srcBitMap,BMA_HEIGHT));
    
      while (src_depth)
      {
         hook.maskBitMap.Planes[--src_depth] = bltMask;
      }

      /* Blit onto the Rastport */
      
      DoHookClipRects(&hook.hook,destRP,&rect);
   }
   else
   {
      BltMaskBitMapRastPort(srcBitMap, xSrc, ySrc, destRP, xDest, yDest, xSize, ySize, minterm, bltMask);
   }
}
//+

/****************************************************************************
*** Shared ******************************************************************
****************************************************************************/

///id_shared_open
STATIC APTR id_shared_open(STRPTR Name)
{
   struct FeelinSharedBrush *Shared = NULL;
   struct FileLock *lock;

   F_OPool(CUD -> Pool);

   if ((lock = BADDR(Lock(Name,ACCESS_READ))))
   {
      for (Shared = CUD -> SharedBrushes ; Shared ; Shared = Shared -> Next)
      {
      #ifdef __AROS__
      #warning "AROS CHECKME: lock compare"
      	 if (SameLock(Shared -> Lock, lock) == LOCK_SAME)
      #else
         if (Shared -> Lock -> fl_Volume == lock -> fl_Volume &&
             Shared -> Lock -> fl_Key    == lock -> fl_Key)
      #endif	     
         {
            Shared -> UserCount++; break;
         }
	 
      }

      if (!Shared)
      {
         APTR dt = NewDTObject(Name,

            DTA_SourceType,   DTST_FILE,
            DTA_GroupID,      GID_PICTURE,
            PDTA_Remap,       FALSE,
         
         TAG_DONE);

         if (dt)
         {
            struct BitMapHeader *bh;
            struct BitMap *bmp;

            DoDTMethod(dt, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);
            GetAttr(PDTA_BitMapHeader,dt,(uint32 *)(&bh));
            GetAttr(PDTA_BitMap,dt,(uint32 *)(&bmp));

            if (bmp)
            {
               uint16 w = (((bh -> bmh_Width + 15) >> 4) << 4);
               uint16 h = (((bh -> bmh_Height+ 15) >> 4) << 4);

               if ((Shared = F_New(sizeof (struct FeelinSharedBrush) + w * h + 16)) != NULL)
               {
                  struct RastPort *rp1;

                  Shared -> Width   = bh -> bmh_Width;  Shared -> AlignedW = w;
                  Shared -> Height  = bh -> bmh_Height; Shared -> AlignedH = h;
                  Shared -> Pixels  = (APTR)((uint32)(Shared) + sizeof (struct FeelinSharedBrush));

                  if ((rp1 = F_New(sizeof (struct RastPort) * 2)) != NULL)
                  {
                     struct RastPort *rp2 = (APTR)((uint32)(rp1) + sizeof (struct RastPort));
                     uint8           *pix = Shared -> Pixels;
                     BOOL             hasmask = FALSE;
                     uint16            x,y;

                     InitRastPort(rp1); rp1 -> BitMap = bmp;
                     InitRastPort(rp2);

                     if ((rp2 -> BitMap = AllocBitMap(w,1,bh -> bmh_Depth,0,NULL)) != NULL)
                     {
                        ReadPixelArray8(rp1,0,0,w - 1,h - 1,Shared -> Pixels,rp2);
                        FreeBitMap(rp2 -> BitMap);
                     }
                     else
                     {
                        for (y = 0 ; y < h ; y++)
                        {
                           for (x = 0 ; x < w ; x++)
                           {
                              pix[x] = ReadPixel(rp1,x,y);
                           }
                           pix += w;
                        }
                        pix = Shared -> Pixels;
                     }
		    
		  #ifdef __AROS__
		     DeinitRastPort(rp1);
		     DeinitRastPort(rp2);
		  #endif
                     F_Dispose(rp1);

// Does the brush needs a mask ?

                     for (y = 0 ; y < Shared -> Height ; y++)
                     {
                        if ((hasmask = id_mask_check(pix,Shared -> Width)) != NULL)
                        {
                           break;
                        }
                        pix += w;
                     }
///
/*
                     {
                        static char str[64];
                        CopyMem("RAM:",(STRPTR)&str,5);
                        AddPart((STRPTR)&str,FilePart(Name),64);
                        SaveMem((STRPTR)&str,Shared -> Pixels,w * h + 16);
                     }
*/
//+

                     if (hasmask)
                     {
                        if ((Shared -> Mask = AllocRaster(w,h)) != NULL)
                        {
                           id_mask_create(Shared -> Pixels,(uint32 *)(Shared -> Mask),w*h);
/*
                           {
                              BPTR out;
                              
                              if (out = Open("ram:mask",MODE_NEWFILE))
                              {
                                 Write(out,Shared -> Mask,w * h);
                                  
                                 Close(out);
                              }
                           }
*/
                        }
                     }
                  }
                  else
                  {
                     F_Dispose(Shared); Shared = NULL;
                  }
               }
            }

            DisposeDTObject(dt);

            if (Shared)
            {
               Shared -> Lock = lock; lock = NULL;
               Shared -> UserCount = 1;

               if (CUD -> SharedBrushes)
               {
                  struct FeelinSharedBrush *prev = CUD -> SharedBrushes;

                  while (prev -> Next) prev = prev -> Next;
                  prev -> Next = Shared;
               }
               else
               {
                  CUD -> SharedBrushes = Shared;
               }
            }
         }
      }
   }
   else
   {
      F_Log(FV_LOG_DEV,"Unable to lock '%s'",Name);
   }

   F_RPool(CUD -> Pool);

   if (lock)
   {
      UnLock(MKBADDR(lock));
   }

#ifdef DB_SHARED_OPEN
   if (Shared)
   {
      F_Log(FV_LOG_DEV,"Shared 0x%08lx - Mask 0x%08lx - Key 0x%08lx - Users %8ld",Shared,Shared -> Mask,Shared -> Lock -> fl_Key,Shared -> UserCount);
   }
#endif

   return Shared;
}
//+
///id_shared_close
STATIC void id_shared_close(struct FeelinSharedBrush *Shared)
{
   if (Shared)
   {
      struct FeelinSharedBrush *node,
                               *prev = NULL;
///DB_SHARED_CLOSE
#ifdef DB_SHARED_CLOSE
      F_DebugOut("ID.Shared.Close() - Shared 0x%08lx - Mask 0x%08lx - Key 0x%08lx - Users %8ld\n",Shared,Shared -> Mask,Shared -> Lock -> fl_Key,Shared -> UserCount);
#endif
//+

      F_OPool(CUD -> Pool);

      for (node = CUD -> SharedBrushes ; node ; node = node -> Next)
      {
         if (node == Shared)
         {
            if (--Shared -> UserCount == 0)
            {
               if (prev)
               {
                  prev -> Next = node -> Next;
               }
               else
               {
                  CUD -> SharedBrushes = node -> Next;
               }

               if (Shared -> Mask) FreeRaster(Shared -> Mask,Shared -> AlignedW,Shared -> AlignedH);
               UnLock(MKBADDR(Shared -> Lock));
               F_Dispose(Shared);
            }
            break;
         }
         prev = node;
      }

      if (!node)
      {
         F_Log(FV_LOG_DEV,"Unknown Shared 0x%lx",Shared);
      }

      F_RPool(CUD -> Pool);
   }
}
//+
///id_shared_setup
STATIC struct FeelinBrushBitMap * id_shared_setup(struct FeelinSharedBrush *Shared,FRender *Render)
{
   struct FeelinBrushBitMap *map;
   struct Screen *scr = (APTR) F_Get(Render -> Display,(uint32) "FA_Display_Screen");
   FPalette *scheme = (Render -> Palette) ? (Render -> Palette) : (FPalette *) F_Get(Render -> Application,FA_ColorScheme);

   if (!Shared || !scr || !scheme) return NULL;

   F_OPool(CUD -> Pool);

   for (map = Shared -> BitMaps ; map ; map = map -> Next)
   {
      if (map -> Scheme == scheme)
      {
         map -> UserCount++; break;
      }
   }

   if (!map)
   {
      if ((map = F_New(sizeof (struct FeelinBrushBitMap))) != NULL)
      {
         uint16 w = Shared -> AlignedW;
         uint16 h = Shared -> AlignedH;
         uint8 d = F_Get(Render -> Display,(uint32) "FA_Display_Depth");

         if ((map -> Mapped = AllocBitMap(w,h,d,0,scr -> RastPort.BitMap)) != NULL)
         {
            struct RastPort *rp1;

            map -> UserCount  = 1;
            map -> Scheme     = scheme;

            if ((rp1 = F_New(sizeof (struct RastPort) * 2 + w + 16)) != NULL)
            {
               struct RastPort *rp2 = (APTR)((uint32)(rp1) + sizeof (struct RastPort));
               uint8 *buf = (APTR)((uint32)(rp2) + sizeof (struct RastPort));
               uint8 *pix = Shared -> Pixels;
               uint16 x,y;

               InitRastPort(rp1); rp1 -> BitMap = map -> Mapped;
               InitRastPort(rp2); rp2 -> BitMap = AllocBitMap(w,1,d,0,NULL);

               for (y = 0 ; y < h ; y++)
               {
                  if (rp2 -> BitMap)
                  {
                     for (x = 0 ; x < w ; x++)
                     {
                        if (pix[x]) buf[x] = scheme -> Pens[pix[x]-8];
                        else if (!Shared -> Mask) buf[x] = scheme -> Pens[FV_Pen_Fill];
                     }

                     WritePixelLine8(rp1,0,y,w,buf,rp2);
                  }
                  else
                  {
                     for (x = 0 ; x < w ; x++)
                     {
                        if (pix[x])
                        {
                           SetAPen(rp1,scheme -> Pens[pix[x]-8]);
                           WritePixel(rp1,x,y);
                        }
                        else if (!Shared -> Mask)
                        {
                           SetAPen(rp1,scheme -> Pens[FV_Pen_Fill]);
                           WritePixel(rp1,x,y);
                        }
                     }
                  }
                  pix += w;
               }

               if (rp2 -> BitMap) FreeBitMap(rp2 -> BitMap);
               
	    #ifdef __AROS__
	       DeinitRastPort(rp1);
	       DeinitRastPort(rp2);
	    #endif
	    
               F_Dispose(rp1);

               if (Shared -> BitMaps)
               {
                  struct FeelinBrushBitMap *prev = Shared -> BitMaps;

                  while (prev -> Next) prev = prev -> Next;
                  prev -> Next = map;
               }
               else
               {
                  Shared -> BitMaps = map;
               }

               F_RPool(CUD -> Pool);

               return map;
            }
            FreeBitMap(map -> Mapped);
         }
         F_Dispose(map); map = NULL;
      }
   }

   F_RPool(CUD -> Pool);

   return map;
}
//+
///id_shared_cleanup
STATIC void id_shared_cleanup(struct FeelinSharedBrush *Shared,struct FeelinBrushBitMap *Map)
{
   if (Shared && Map)
   {
      struct FeelinBrushBitMap *node,
                               *prev = NULL;

      F_OPool(CUD -> Pool);

      for (node = Shared -> BitMaps ; node ; node = node -> Next)
      {
         if (node == Map)
         {
            if (--Map -> UserCount == 0)
            {
               if (prev)   prev -> Next      = node -> Next;
               else        Shared -> BitMaps = node -> Next;

               if (Map -> Mapped) FreeBitMap(Map -> Mapped);
               F_Dispose(Map);
            }
            break;
         }
         prev = node;
      }

      if (!node)
      {
         F_Log(FV_LOG_DEV,"Unknown BitMap 0x%lx - Shared 0x%lx",Map,Shared);
      }

      F_RPool(CUD -> Pool);
   }
}
//+

/****************************************************************************
*** Brush *******************************************************************
****************************************************************************/

///id_brush_create
F_CODE_CREATE_XML(id_brush_create)
{
   FImage *img = NULL;
   STRPTR buf;
   uint32 len;

   if (DataTypesBase)
   {
      STRPTR src=NULL;
      
      for ( ; Attribute ; Attribute = Attribute -> Next)
      {
         switch (Attribute -> Name -> ID)
         {
            case FV_XML_ID_SRC:  src = Attribute -> Data; break;
         }
      }
      
      if (src)
      {
         if ((buf = F_New(1024)) != NULL)
         {
            if ((img = F_NewP(CUD -> Pool,sizeof (FImage))) != NULL)
            {
               CopyMem("Feelin:Images/\0",buf,15);
               AddPart(buf,src,250);

               len = F_StrLen(buf) - 1;

               if ((img -> Shared[0] = id_shared_open(buf)) != NULL)
               {
                  if (buf[len] == '0')
                  {
                     buf[len] = '1';

                     if (!(img -> Shared[1] = id_shared_open(buf)))
                     {
                        id_shared_close(img -> Shared[0]); img -> Shared[0] = NULL;
                     }
                  }
               }
               else
               {
                  F_Dispose(img); img = NULL;
               }
            }
            F_Dispose(buf);
         }
      }
   }
   return img;
}
//+
///id_brush_delete
F_CODE_DELETE(id_brush_delete)
{
   id_shared_close(image -> Shared[0]); image -> Shared[0] = NULL;
   id_shared_close(image -> Shared[1]); image -> Shared[1] = NULL;
   
   F_Dispose(image);
}
//+
///id_brush_setup
F_CODE_SETUP(id_brush_setup)
{
   if (image -> Shared[0])
   {
      if ((image -> Map[0] = id_shared_setup(image -> Shared[0],Render)) != NULL)
      {
         if (image -> Shared[1])
         {
            if (!(image -> Map[1] = id_shared_setup(image -> Shared[1],Render)))
            {
               return FALSE;
            }
         }
         return TRUE;
      }
   }

   return FALSE;
}
//+
///id_brush_cleanup
F_CODE_CLEANUP(id_brush_cleanup)
{
   id_shared_cleanup(image -> Shared[0],image -> Map[0]);
   id_shared_cleanup(image -> Shared[1],image -> Map[1]);
}
//+
///id_brush_get
F_CODE_GET(id_brush_get)
{
   int32 rc=0;

   switch (Attribute)
   {
      case FA_ImageDisplay_Height:
      {
         if (image -> Shared[0]) rc = image -> Shared[0] -> Height;
         if (image -> Shared[1]) rc = MAX(rc,image -> Shared[1] -> Height);
      }
      break;

      case FA_ImageDisplay_Width:
      {
         if (image -> Shared[0]) rc = image -> Shared[0] -> Width;
         if (image -> Shared[1]) rc = MAX(rc,image -> Shared[1] -> Width);
      }
      break;
   
      case FA_ImageDisplay_Mask:
      {
         if (image -> Shared[0]) rc = (int32)(image -> Shared[0] -> Mask);
         if (image -> Shared[1]) rc = (int32)(image -> Shared[1] -> Mask);
      }
      break;
   }
   return rc;
}
//+
///id_brush_draw
F_CODE_DRAW(id_brush_draw)
{
   struct FeelinSharedBrush *shared;
   struct FeelinBrushBitMap *map;

   if (image -> Shared[1])
   {
      shared = image -> Shared[(FF_IMAGE_SELECTED & Msg -> Flags) ? 1 : 0];
      map    = image -> Map[(FF_IMAGE_SELECTED & Msg -> Flags) ? 1 : 0];
   }
   else
   {
      shared = image -> Shared[0];
      map    = image -> Map[0];
   }

   if (shared && map)
   {
      uint16 w = Msg -> Rect -> x2 - Msg -> Rect -> x1 + 1;
      uint16 h = Msg -> Rect -> y2 - Msg -> Rect -> y1 + 1;
      int16  x = Msg -> Rect -> x1 + ((shared -> Width  < w) ? (w - shared -> Width) / 2  : 0);
      int16  y = Msg -> Rect -> y1 + ((shared -> Height < h) ? (h - shared -> Height) / 2 : 0);
      int16  a = (shared -> Width  > w) ? (shared -> Width - w)  : 0;
      int16  b = (shared -> Height > h) ? (shared -> Height - h) : 0;

      if (shared -> Mask)
      {
      #ifdef __AROS__
         MyBltMaskBitMapRastPort(map -> Mapped,0,0, Msg -> Render -> RPort,x,y,shared -> Width-a,shared -> Height-b,(ABC|ABNC|ANBC),shared -> Mask);
      #else
         MyBltMaskBitMapRastPort(map -> Mapped,0,0, Msg -> Render -> RPort,x,y,shared -> Width-a,shared -> Height-b,0x0E2,shared -> Mask);
      #endif      
      }
      else
      {
         BltBitMapRastPort(map -> Mapped,0,0, Msg -> Render -> RPort,x,y,shared -> Width-a,shared -> Height-b,0x0C0);
      }
      return TRUE;
   }
   return FALSE;
}
//+

struct in_CodeTable id_brush_table =
{
   (in_Code_Create *)   &id_brush_create,
   (in_Code_Delete *)   &id_brush_delete,
   (in_Code_Setup *)    &id_brush_setup,
   (in_Code_Cleanup *)  &id_brush_cleanup,
   (in_Code_Get *)      &id_brush_get,
   (in_Code_Draw *)     &id_brush_draw
};
