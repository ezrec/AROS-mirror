#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Render_New
F_METHOD(FObject,Render_New)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj),
                          *friend;
   const struct TagItem   *Tags = Msg;
   struct TagItem         *item;

   if ((friend = (APTR) GetTagData(FA_Render_Friend,NULL,Tags)) != NULL)
   {
      LOD -> Application = friend -> Application;
      LOD -> Display     = friend -> Display;
      LOD -> Window      = friend -> Window;
      LOD -> RPort       = friend -> RPort;
      LOD -> Palette     = friend -> Palette;

      if (FF_Render_TrueColors & friend -> Flags)
      {
         LOD -> Flags = FF_Render_TrueColors;
      }
   }

   while  ((item = NextTagItem(&Tags)) != NULL)
   switch (item -> ti_Tag)
   {
      case FA_Render_Application:   LOD -> Application = (APTR)(item -> ti_Data); break;
      case FA_Render_Display:       LOD -> Display     = (APTR)(item -> ti_Data); break;
      case FA_Render_Window:        LOD -> Window      = (APTR)(item -> ti_Data); break;
   }

   if (!LOD -> Application)   LOD -> Application = (APTR) F_Get(LOD -> Window,FA_Application);
   if (!LOD -> Display)       LOD -> Display     = (APTR) F_Get(LOD -> Application,FA_Application_Display);

   if (LOD -> Application && LOD -> Display)
   {
      struct BitMap *bmp;

      if ((bmp = (struct BitMap *) F_Get(LOD -> Display,(ULONG) "FA_Display_BitMap")) != NULL)
      {
         if (GetBitMapAttr(bmp,BMA_DEPTH) > 8)
         {
            LOD -> Flags |= FF_Render_TrueColors;
         }
      }

      F_DoA(Obj,FM_Set,Msg);

      return Obj;
   }

   F_Log(FV_LOG_DEV,"Application 0x%08lx - Display 0x%08lx",LOD -> Application,LOD -> Display);

   return NULL;
}
//+
///Render_Get
#undef F_STORE
#define F_STORE(val)            *((ULONG *)(item -> ti_Data)) = (ULONG)(val)

F_METHOD(void,Render_Get)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   const struct TagItem   *Tags = Msg;
   struct TagItem         *item;

   while  ((item = NextTagItem(&Tags)) != NULL)
   switch (item -> ti_Tag)
   {
      case FA_Render_Application:   F_STORE(LOD -> Application); break;
      case FA_Render_Display:       F_STORE(LOD -> Display); break;
      case FA_Render_Window:        F_STORE(LOD -> Window); break;
      case FA_Render_RPort:         F_STORE(LOD -> RPort); break;
      case FA_Render_Forbid:        F_STORE(LOD -> ForbidNest > 0); break;
   }
}
//+
///Render_Set
F_METHOD(void,Render_Set)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   const struct TagItem   *Tags = Msg;
   struct TagItem         *item;

   while  ((item = NextTagItem(&Tags)) != NULL)
   switch (item -> ti_Tag)
   {
      case FA_Render_RPort:
      {
         LOD -> RPort = (struct RastPort *)(item -> ti_Data);

         /*
         LOD -> Flags &= ~FF_Render_TrueColors;

         if (LOD -> RPort = (struct RastPort *)(item -> ti_Data))
         {
            if (LOD -> RPort -> BitMap)
            {
               if (GetBitMapAttr(LOD -> RPort -> BitMap,BMA_DEPTH) > 8)
               {
                  LOD -> Flags |= FF_Render_TrueColors;
               }
            }

            else
            {
               F_Log(FV_LOG_DEV,"No BitMap attached to FA_Render_RPort 0x%08lx",LOD -> RPort);
            }
         }
         */
      }
      break;

      case FA_Render_Palette:
      {
         LOD -> Palette = (struct FeelinPalette *)(item -> ti_Data);
      }
      break;

      case FA_Render_Forbid:
      {
         if (item -> ti_Data)
         {
            LOD -> ForbidNest++;
         }
         else
         {
            LOD -> ForbidNest--;
         }

         if (LOD -> ForbidNest == 1)
         {
            LOD -> Flags |= FF_Render_Forbid;
         }
         else if (LOD -> ForbidNest == 0)
         {
            LOD -> Flags &= ~FF_Render_Forbid;
         }
      }
      break;

      case FA_Render_Window:
      {
         LOD -> Window = (FObject)(item -> ti_Data);
      }
      break;
   }
}
//+

///Render_AddClipRegion
F_METHODM(struct FeelinTraceClip *,Render_AddClipRegion,FS_Render_AddClipRegion)
{
   struct LocalObjectData  *LOD  = F_LOD(Class,Obj);
   struct FeelinTraceLayer *trace_lay  = NULL;
   struct FeelinTraceClip  *trace_clip = NULL;
   struct Window           *win;
   struct Layer            *lay;

#ifdef DB_SEMAPHORE
   F_Log(0,"RENDER_LOCK");
#endif

   F_Do(CUD -> Arbitrer,FM_Lock,FF_Lock_Exclusive);

   win = (struct Window *) F_Get(LOD -> Window,FA_Window);

   if (win)                lay = win -> WLayer;
   else if (LOD -> RPort)  lay = LOD -> RPort -> Layer;
   else                    lay = NULL;

   if (!lay)
   {
      F_Log(FV_LOG_DEV,"Unable to obtain layer"); goto __error;
   }

   for (trace_lay = CUD -> TraceLayers ; trace_lay ; trace_lay = trace_lay -> Next)
   {
      if (trace_lay -> Layer == lay)
      {
         if (trace_lay -> TraceClipList.Tail)
         {
            struct Region *region;

            if ((region = ((struct FeelinTraceClip *)(trace_lay -> TraceClipList.Tail)) -> Region) != NULL)
            {

               /*  Si  l'on  essaie  de  clipper  une  zone   totalement
               exterieure  à la zone précédente (supérieure) l'opération
               échoue  puisque  soit  le   rendue   est   totalement   à
               l'extérieur  de  la zone (non visible, ou zone protégée),
               soit l'objet est partiellement dans la zone précédente et
               dans ce cas, seule la partie commune sera modifiable. */

               if (Msg -> Region -> bounds.MinX <= region -> bounds.MinX &&
                   Msg -> Region -> bounds.MaxX >= region -> bounds.MaxX &&
                   Msg -> Region -> bounds.MinY <= region -> bounds.MinY &&
                   Msg -> Region -> bounds.MaxY >= region -> bounds.MaxY)
               {
                  goto __error;
               }
            }
         }
         break;
      }
   }

   if (!trace_lay)
   {
      if ((trace_lay = F_New(sizeof (struct FeelinTraceLayer))) != NULL)
      {
         trace_lay -> Layer = lay;

         if (CUD -> TraceLayers)
         {
            struct FeelinTraceLayer *prev = CUD -> TraceLayers;

            while (prev -> Next) prev = prev -> Next;
            prev -> Next = trace_lay;
         }
         else
         {
            CUD -> TraceLayers = trace_lay;
         }
      }
   }

   if (!trace_lay)
   {
      F_Log(FV_LOG_USER,"Unable to allocate TraceLayer"); goto __error;
   }

   trace_clip = F_New(sizeof (struct FeelinTraceClip));

   if (!trace_clip)
   {
      F_Log(FV_LOG_USER,"Unable to allocate TraceClip"); goto __error;
   }

   if ((trace_clip -> Region = NewRegion()) != NULL)
   {
      if (!OrRegionRegion(Msg -> Region,trace_clip -> Region))
      {
         DisposeRegion(trace_clip -> Region); trace_clip -> Region = NULL;
      }
   }

   if (!trace_clip -> Region)
   {
      F_Log(FV_LOG_USER,"Unable to duplicate region"); goto __error;
   }

   if (trace_lay -> TraceClipList.Tail)
   {
      AndRegionRegion(((struct FeelinTraceClip *)(trace_lay -> TraceClipList.Tail)) -> Region,trace_clip -> Region);
   }

   if (win && !(win -> Flags & WFLG_SIMPLE_REFRESH))
   {
      LockLayerInfo(&win -> WScreen -> LayerInfo);

      trace_clip -> PreviousRegion = InstallClipRegion(lay,trace_clip -> Region);

      UnlockLayerInfo(&win -> WScreen -> LayerInfo);
   }
   else
   {
      trace_clip -> PreviousRegion = InstallClipRegion(lay,trace_clip -> Region);
   }

   F_LinkTail(&trace_lay -> TraceClipList,(FNode *) trace_clip);

//   F_Log(0,"TClip 0x%08lx - TLayer 0x%08lx (0x%08lx) - Region 0x%08lx (0x%08lx)",trace_clip,trace_lay,trace_lay -> Layer,trace_clip -> Region,trace_clip -> PreviousRegion);

#ifdef DB_SEMAPHORE
   F_Log(0,"RENDER_UNLOCK");
#endif

   F_Do(CUD -> Arbitrer,FM_Unlock);

   return trace_clip;

/*** error handling ********************************************************/

__error:

   if (trace_clip)
   {
      F_Dispose(trace_clip);
   }

   if (trace_lay)
   {
      if (!trace_lay -> TraceClipList.Head)
      {
         struct FeelinTraceLayer *node,
                                 *prev=NULL;

         for (node = CUD -> TraceLayers ; node ; node = node -> Next)
         {
            if (node == trace_lay)
            {
               if (prev)   prev -> Next = node -> Next;
               else        CUD -> TraceLayers = node -> Next;

               F_Dispose(node);

               break;
            }
            prev = node;
         }
      }
   }

#ifdef DB_SEMAPHORE
   F_Log(0,"RENDER_UNLOCK");
#endif

   F_Do(CUD -> Arbitrer,FM_Unlock);

   return NULL;
}
//+
///Render_AddClip
F_METHODM(struct FeelinTraceClip *,Render_AddClip,FS_Render_AddClip)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct FeelinTraceClip *trace_clip = NULL;

   if (Msg &&
       Msg -> Rect -> x1 <= Msg -> Rect -> x2 &&
       Msg -> Rect -> y1 <= Msg -> Rect -> y2 && LOD -> RPort)
   {
      struct Region *region;

      if ((region = NewRegion()) != NULL)
      {
         OrRectRegion(region,(struct Rectangle *) Msg -> Rect);

         trace_clip = (struct FeelinTraceClip *) F_Do(Obj,FM_Render_AddClipRegion,region);

         DisposeRegion(region);
      }
   }
   return trace_clip;
}
//+
///Render_RemClip
F_METHODM(void,Render_RemClip,FS_Render_RemClip)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct Window          *win;
   struct Layer           *lay;

   win = (APTR) F_Get(LOD -> Window,FA_Window);

   if (win)                lay = win -> WLayer;
   else if (LOD -> RPort)  lay = LOD -> RPort -> Layer;
   else                    lay = NULL;

#ifdef DB_SEMAPHORE
   F_Log(0,"RENDER_LOCK");
#endif

   F_Do(CUD -> Arbitrer,FM_Lock,FF_Lock_Exclusive);

   if (Msg -> Handle)
   {
      struct FeelinTraceLayer *trace_layer;
      struct FeelinTraceClip  *trace_clip  = NULL;

      for (trace_layer = CUD -> TraceLayers ; trace_layer ; trace_layer = trace_layer -> Next)
      {
         if (!lay || trace_layer -> Layer == lay)
         {
            for (trace_clip = (struct FeelinTraceClip *) trace_layer -> TraceClipList.Tail ; trace_clip ; trace_clip = trace_clip -> Prev)
            {
               if (trace_clip == Msg -> Handle) break;
            }
         }
         if (trace_clip) break;
      }

//      F_Log(0,"trace_lay 0x%08lx - trace_clip 0x%08lx",trace_layer,trace_clip);

      if (trace_clip)
      {
         F_LinkRemove(&trace_layer -> TraceClipList,(FNode *) trace_clip);

         if (!trace_layer -> TraceClipList.Head)
         {
            struct FeelinTraceLayer *node;
            struct FeelinTraceLayer *prev=NULL;

            for (node = CUD -> TraceLayers ; node ; node = node -> Next)
            {
               if (node == trace_layer)
               {
                  if (prev)
                  {
                     prev -> Next = node -> Next;
                  }
                  else
                  {
                     CUD -> TraceLayers = node -> Next;
                  }

                  F_Dispose(node);

                  break;
               }
               prev = node;
            }
         }

         if (win && !(win -> Flags & WFLG_SIMPLE_REFRESH))
         {
            LockLayerInfo(&win -> WScreen -> LayerInfo);
            InstallClipRegion(lay,trace_clip -> PreviousRegion);
            UnlockLayerInfo(&win -> WScreen -> LayerInfo);
         }
         else
         {
            InstallClipRegion(lay,trace_clip -> PreviousRegion);
         }

         DisposeRegion(trace_clip -> Region);

//         F_Log(0,"trace_clip 0x%08lx - trace_layer 0x%08lx",trace_clip,trace_layer);

         F_Dispose(trace_clip);
      }
      else
      {
         F_Log(FV_LOG_DEV,"Unknown Handle 0x%08lx",Msg -> Handle);
      }
   }
   else if (lay)
   {
      InstallClipRegion(lay,NULL);
   }

#ifdef DB_SEMAPHORE
   F_Log(0,"RENDER_UNLOCK");
#endif

   F_Do(CUD -> Arbitrer,FM_Unlock);
}
//+

/*  CreateBuffer()  and  DeleteBuffer()  are  not  really  what  I  want...
FC_Render needs a serious update !! */

///Render_CreateBuffer
F_METHODM(FObject,Render_CreateBuffer,FS_Render_CreateBuffer)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct RastPort *rp;

   if ((rp = F_New(sizeof (struct RastPort))) != NULL)
   {
      struct BitMap *bmfrd;
      uint32 depth;

      if (LOD -> RPort)
      {
         bmfrd = LOD -> RPort -> BitMap;
         depth = GetBitMapAttr(bmfrd,BMA_DEPTH);
      }
      else
      {
         F_Do(LOD -> Display,FM_Get,

            "FA_Display_BitMap",   &bmfrd,
            "FA_Display_Depth",    &depth,

            TAG_DONE);
      }

//      F_Log(0,"fiend_bitmap 0x%08lx - depth %ld",bmfrd,depth);

      InitRastPort(rp);

      if ((rp -> BitMap = AllocBitMap(Msg -> Width,Msg -> Height,depth,BMF_INTERLEAVED,bmfrd)) != NULL)
      {
         FRender *render = RenderObject,

            FA_Render_Friend,   Obj,
            FA_Render_RPort,    rp,

            End;

         if (render)
         {
//            F_Log(0,"render 0x%08lx",render);

//            F_DisposeObj(render);

            return render;
         }
         else
         {
            FreeBitMap(rp -> BitMap);
         }
      }

#ifdef __AROS__
      DeinitRastPort(rp);
#endif
      F_Dispose(rp);
   }
   return NULL;
}
//+
///Render_DeleteBuffer
F_METHODM(void,Render_DeleteBuffer,FS_Render_DeleteBuffer)
{
//   F_Log(0,"buffer 0x%08lx",Msg -> Buffer);

   if (Msg -> Buffer)
   {
      if (F_Get(Msg -> Buffer,FA_Render_RPort))
      {
         if (Msg -> Buffer -> RPort -> BitMap)
         {
            FreeBitMap(Msg -> Buffer -> RPort -> BitMap);
         }
      #ifdef __AROS__
         DeinitRastPort(Msg -> Buffer -> RPort);
      #endif
         F_Dispose(Msg -> Buffer -> RPort);
      }

      F_DisposeObj(Msg -> Buffer);
   }
}
//+
