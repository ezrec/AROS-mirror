/*
 * chunkyiclass.c
 * ==============
 * Implementation of chunky image class.
 *
 * Copyright © 1995 Lorens Younes (d93-hyo@nada.kth.se)
 */

#include <clib/alib_protos.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <graphics/gfxbase.h>
#include <graphics/scale.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <aros/debug.h>

#include "chunkyiclass.h"

struct rp_info {
   UWORD             width, height;
   struct RastPort  *rp;
};

/* instance data for chunkyiclass */
struct chunkyidata {
   struct Screen   *screen;
   UWORD            selected_bgpen;
   ULONG            num_pens;
   LONG            *shared_pens;
   UWORD           *def_pens;
   UBYTE           *selected_data;
   struct rp_info   normal_rpi, selected_rpi;
};


/* macros to extract components of CHUNKYIA_Palette (n >= 1) */
#define PALETTE_ENTRIES(p)    (*(p))
#define PALETTE_RED(p, n)     (*((p) + (3 * (n)) - 2))
#define PALETTE_GREEN(p, n)   (*((p) + (3 * (n)) - 1))
#define PALETTE_BLUE(p, n)    (*((p) + (3 * (n))))


/* macros to extract components of IA_Data and CHUNKYIA_SelectedData */
//#define IMAGE_WIDTH(i)    (((UWORD *)(i))[0])
//#define IMAGE_HEIGHT(i)   (((UWORD *)(i))[1])

#define IMAGE_WIDTH(i)	(i[0] << 8 | i[1])
#define IMAGE_HEIGHT(i)	(i[2] << 8 | i[3])
#define IMAGE_DATA(i)     (&(i)[4])


/* frees rastport */
static void
free_rastport (
   struct RastPort  *rp,
   UWORD             width,
   UWORD             height)
{
   if (rp && rp->BitMap)
   {
      if (GfxBase->LibNode.lib_Version >= 39L)
         FreeBitMap (rp->BitMap);
      else
      {
         register UBYTE   i;
         
         
         for (i = 0; i < rp->BitMap->Depth; ++i)
         {
            if (rp->BitMap->Planes[i] != NULL)
            {
               FreeRaster (rp->BitMap->Planes[i], width, height);
            }
         }
         FreeVec (rp->BitMap);
      }
   }
   FreeVec (rp);
}


/* creates a rastport */
static struct RastPort *
init_rastport (
   struct RastPort  *parent_rp,
   UWORD             width,
   UWORD             height)
{
   struct RastPort  *rp;
   
   
   if ((rp = AllocVec (sizeof (*rp), MEMF_PUBLIC)))
   {
      *rp = *parent_rp;
      rp->Layer = NULL;
      
      if (GfxBase->LibNode.lib_Version >= 39L)
      {
         if (!(rp->BitMap = AllocBitMap (width, height,
                                         parent_rp->BitMap->Depth, 0,
                                         parent_rp->BitMap)))
         {
            free_rastport (rp, width, height);
            rp = NULL;
         }
      }
      else
      {
         register UBYTE   i;
         
         
         if ((rp->BitMap = AllocVec (sizeof (*rp->BitMap), MEMF_PUBLIC)))
         {
            InitBitMap (rp->BitMap, parent_rp->BitMap->Depth, width, height);
            for (i = 0; i < rp->BitMap->Depth; ++i)
               rp->BitMap->Planes[i] = NULL;
            for (i = 0; i < rp->BitMap->Depth; ++i)
            {
               if (!(rp->BitMap->Planes[i] = (PLANEPTR)
                                             AllocRaster (width, height)))
               {
                  free_rastport (rp, width, height);
                  rp = NULL;
               }
            }
         }
      }
   }
   
   return rp;
}


static void
free_shared_pens (
   struct chunkyidata  *data)
{
   register ULONG   n;
   
   
   if (data->shared_pens)
   {
      for (n = 0; n < data->num_pens; ++n)
      {
         if (data->shared_pens[n] != -1)
         {
            ReleasePen (data->screen->ViewPort.ColorMap,
                        data->shared_pens[n]);
         }
      }
      FreeVec (data->shared_pens);
      data->shared_pens = NULL;
   }
}


static void
init_shared_pens (
   struct chunkyidata  *data,
   ULONG               *palette)
{
   register ULONG   n;
   
   
   free_shared_pens (data);
   
   if (palette)
   {
      data->num_pens = PALETTE_ENTRIES (palette);
      data->shared_pens = AllocVec (data->num_pens *
                                    sizeof (*data->shared_pens),
                                    MEMF_PUBLIC);
   }
   
   if (GfxBase->LibNode.lib_Version >= 39L &&
       data->shared_pens && data->screen)
   {
      for (n = 1; n <= data->num_pens; ++n)
      {
         data->shared_pens[n - 1] = ObtainBestPen (
                                             data->screen->ViewPort.ColorMap,
                                             PALETTE_RED (palette, n),
                                             PALETTE_GREEN (palette, n),
                                             PALETTE_BLUE (palette, n),
                                             OBP_Precision, PRECISION_IMAGE,
                                             TAG_DONE);
      }
   }
   else /* no pens could be obtained - IA_Pens will be used */
   {
      FreeVec (data->shared_pens);
      data->shared_pens = NULL;
   }
}


static void
free_image (
   struct rp_info  *rpi)
{
   free_rastport (rpi->rp, rpi->width, rpi->height);
   rpi->rp = NULL;
}


static void
init_image (
   struct RastPort  *parent_rp,
   struct rp_info   *rpi,
   UBYTE            *imagedata,
   LONG             *shared_pens,
   UWORD            *def_pens,
   UWORD             bgpen)
{
   register UWORD   x, y;
   
   
   free_image (rpi);
   
   if (imagedata && parent_rp)
   {
      if ((rpi->rp = init_rastport (parent_rp,
                                   rpi->width, rpi->height)))
      {
         for (y = 0; y < rpi->height; ++y)
         {
            for (x = 0; x < rpi->width; ++x)
            {
               if (imagedata[x + y * rpi->width] == 0) /* use BACKGROUNDPEN */
                  SetAPen (rpi->rp, bgpen);
               else
               {
                  if (shared_pens &&
                      shared_pens[imagedata[x + y * rpi->width] - 1] != -1)
                  {
                     SetAPen (rpi->rp,
                              shared_pens[imagedata[x + y * rpi->width] - 1]);
                  }
                  else if (def_pens)
                  {
                     SetAPen (rpi->rp,
                              def_pens[imagedata[x + y * rpi->width] - 1]);
                  }
                  else
                  {
                     SetAPen (rpi->rp,
                              imagedata[x + y * rpi->width]);
                  }
               }
               
               WritePixel (rpi->rp, x, y);
            }
         }
      }
   }
}

/* OM_NEW method for chunkyiclass */
static void
chunkyim_new (
   Class         *cl,
   Object        *obj,
   struct opSet  *msg)
{
   struct chunkyidata  *data = INST_DATA (cl, obj);
   UBYTE               *normal_data = NULL;
   struct DrawInfo     *dri = NULL;
   IPTR                 bgpen;
   
// NOTE: Had to add the following line.
data->normal_rpi.rp = NULL;
   
   /* If IA_Width or IA_Height are not explicitly given on initialization, */
   /* set them to their corresponding values in IA_Data.                   */
   GetAttr (IA_Data, obj, (IPTR *)&normal_data);
   if (normal_data != NULL)
   {
      if (!FindTagItem (IA_Width, msg->ops_AttrList))
         ((struct Image *)obj)->Width = IMAGE_WIDTH (normal_data);
      if (!FindTagItem (IA_Height, msg->ops_AttrList))
         ((struct Image *)obj)->Height = IMAGE_HEIGHT (normal_data);
   }
   
   
   /* Initialize image palette. */
   data->shared_pens = NULL;
   data->screen = (struct Screen *)
                  GetTagData (CHUNKYIA_Screen, (IPTR)NULL, msg->ops_AttrList);
   init_shared_pens (data,
                     (ULONG *)
                     GetTagData (CHUNKYIA_Palette, (IPTR)NULL, msg->ops_AttrList));
   
   if (data->screen)
      dri = GetScreenDrawInfo (data->screen);
   if (!FindTagItem (IA_BGPen, msg->ops_AttrList))
   {
      ((struct Image *)obj)->PlaneOnOff = (dri) ?
                                          dri->dri_Pens[BACKGROUNDPEN] : 0;
   }
   data->selected_bgpen = GetTagData (CHUNKYIA_SelectedBGPen,
                                      ((dri) ? dri->dri_Pens[FILLPEN] : 0),
                                      msg->ops_AttrList);
   if (dri)
      FreeScreenDrawInfo (data->screen, dri);
   
   
   /* Create images in their own rastports. */
   data->selected_data = (UBYTE *)GetTagData (CHUNKYIA_SelectedData, (IPTR)NULL,
                                              msg->ops_AttrList);
   GetAttr (IA_BGPen, obj, &bgpen);
   data->def_pens = (UWORD *)GetTagData (IA_Pens, (IPTR)NULL, msg->ops_AttrList);
   
   if (data->screen)
   {
      if (normal_data)
      {
         data->normal_rpi.width = IMAGE_WIDTH (normal_data);
         data->normal_rpi.height = IMAGE_HEIGHT (normal_data);
         init_image (&data->screen->RastPort, &data->normal_rpi,
                     IMAGE_DATA (normal_data),
                     data->shared_pens, data->def_pens, bgpen);
      }
      if (data->selected_data)
      {
         data->selected_rpi.width = IMAGE_WIDTH (data->selected_data);
         data->selected_rpi.height = IMAGE_HEIGHT (data->selected_data);
         init_image (&data->screen->RastPort, &data->selected_rpi,
                     IMAGE_DATA (data->selected_data),
                     data->shared_pens, data->def_pens, data->selected_bgpen);
      }
   }
}


/* OM_DISPOSE method for chunkyiclass */
static void
chunkyim_dispose (
   Class   *cl,
   Object  *obj)
{
   struct chunkyidata  *data = INST_DATA (cl, obj);
   
   free_shared_pens (data);
   free_image (&data->normal_rpi);
   free_image (&data->selected_rpi);
}


/* OM_GET method for chunkyiclass */
static BOOL
chunkyim_get (
   Class         *cl,
   Object        *obj,
   struct opGet  *msg)
{
   struct chunkyidata  *data = INST_DATA (cl, obj);
   BOOL                 retval = TRUE;
   
   
   switch (msg->opg_AttrID)
   {
   case IA_Pens:
      *msg->opg_Storage = (IPTR)data->def_pens;
      break;
   case CHUNKYIA_SelectedBGPen:
      *msg->opg_Storage = data->selected_bgpen;
      break;
   case CHUNKYIA_SelectedData:
      *msg->opg_Storage = (IPTR)data->selected_data;
      break;
   case CHUNKYIA_Screen:
      *msg->opg_Storage = (IPTR)data->screen;
      break;
   default:
      retval = FALSE;
      break;
   }
   
   return retval;
}


/* OM_SET method for chunkyiclass */
static void
chunkyim_set (
   Class         *cl,
   Object        *obj,
   struct opSet  *msg)
{
   struct chunkyidata  *data = INST_DATA (cl, obj);
   struct TagItem      *ti;
   struct TagItem *tstate = msg->ops_AttrList;
   BOOL                 nimg_update = FALSE, simg_update = FALSE;
   UBYTE               *normal_data = NULL;
   IPTR                 bgpen;
   
   
   while ((ti = NextTagItem (&tstate)))
   {
      switch (ti->ti_Tag)
      {
      case IA_Pens:
         data->def_pens = (UWORD *)ti->ti_Data;
         nimg_update = simg_update = TRUE;
         break;
      case IA_BGPen:
      case IA_Data:
         nimg_update = TRUE;
         break;
      case CHUNKYIA_SelectedBGPen:
         data->selected_bgpen = ti->ti_Data;
         simg_update = TRUE;
         break;
      case CHUNKYIA_SelectedData:
         if ((data->selected_data = (UBYTE *)ti->ti_Data))
            simg_update = TRUE;
         else
            free_image (&data->selected_rpi);
         break;
      case CHUNKYIA_Palette:
         init_shared_pens (data, (ULONG *)ti->ti_Data);
         nimg_update = simg_update = TRUE;
         break;
      }
   }
   
   GetAttr (IA_Data, obj, (IPTR *)&normal_data);
   if (nimg_update && data->screen && normal_data)
   {
      GetAttr (IA_BGPen, obj, &bgpen);
      data->normal_rpi.width = IMAGE_WIDTH (normal_data);
      data->normal_rpi.height = IMAGE_HEIGHT (normal_data);
      
      init_image (&data->screen->RastPort, &data->normal_rpi,
                  IMAGE_DATA (normal_data),
                  data->shared_pens, data->def_pens, bgpen);
   }
   if (simg_update && data->screen && data->selected_data)
   {
      data->selected_rpi.width = IMAGE_WIDTH (data->selected_data);
      data->selected_rpi.height = IMAGE_HEIGHT (data->selected_data);
      
      init_image (&data->screen->RastPort, &data->selected_rpi,
                  IMAGE_DATA (data->selected_data),
                  data->shared_pens, data->def_pens, data->selected_bgpen);
   }
}


/* IM_DRAW method for chunkyiclass */
static void
chunkyim_draw (
   Class           *cl,
   Object          *obj,
   struct impDraw  *msg)
{
   struct chunkyidata  *data = INST_DATA (cl, obj);
   struct rp_info      *rpi;
   SIPTR                left, top;
   IPTR                 width, height;
   
   
   switch (msg->imp_State)
   {
   case IDS_SELECTED:
   case IDS_INACTIVESELECTED:
      rpi = &data->selected_rpi;
      break;
   default:
      rpi = &data->normal_rpi;
      break;
   }
   if (!rpi->rp)
      rpi = &data->normal_rpi;
   
   if (rpi->rp)
   {
      GetAttr (IA_Left, obj, &left);
      GetAttr (IA_Top, obj, &top);
      if (msg->MethodID == IM_DRAWFRAME)
      {
         width = msg->imp_Dimensions.Width;
         height = msg->imp_Dimensions.Height;
      }
      else
      {
         GetAttr (IA_Width, obj, &width);
         GetAttr (IA_Height, obj, &height);
      }
      left += msg->imp_Offset.X;
      top += msg->imp_Offset.Y;
      
      if (width == rpi->width && height == rpi->height)
      {
         ClipBlit (rpi->rp, 0, 0, msg->imp_RPort, left, top,
                   rpi->width, rpi->height, 0xC0);
      }
      else
      {
         struct BitScaleArgs   bsa;
         struct RastPort      *temp_rp;
         
         
         if ((temp_rp = init_rastport (msg->imp_RPort, width, height)))
         {
            bsa.bsa_SrcX = 0;
            bsa.bsa_SrcY = 0;
            bsa.bsa_SrcWidth = rpi->width;
            bsa.bsa_SrcHeight = rpi->height;
            bsa.bsa_XSrcFactor = rpi->width;
            bsa.bsa_YSrcFactor = rpi->height;
            bsa.bsa_DestX = 0;
            bsa.bsa_DestY = 0;
            bsa.bsa_DestWidth = width;
            bsa.bsa_DestHeight = height;
            bsa.bsa_XDestFactor = width;
            bsa.bsa_YDestFactor = height;
            bsa.bsa_SrcBitMap = rpi->rp->BitMap;
            bsa.bsa_DestBitMap = temp_rp->BitMap;
            bsa.bsa_Flags = 0;
            
            BitMapScale (&bsa);
            ClipBlit (temp_rp, 0, 0, msg->imp_RPort, left, top,
                      width, height, 0xC0);
            
            free_rastport (temp_rp, width, height);
         }
      }
   }
}


/* IM_HITFRAME method for chunkyiclass */
static BOOL
chunkyim_hitframe (
   Class              *cl,
   Object             *obj,
   struct impHitTest  *msg)
{
   SIPTR  left, top;
   
   
   GetAttr (IA_Left, obj, &left);
   GetAttr (IA_Top, obj, &top);
   
   return (BOOL)(msg->imp_Point.X >= left && msg->imp_Point.Y >= top &&
                 msg->imp_Point.X < left + msg->imp_Dimensions.Width &&
                 msg->imp_Point.Y < left + msg->imp_Dimensions.Height);
}


/* IM_ERASEFRAME method for chunkyiclass */
static void
chunkyim_eraseframe (
   Class            *cl,
   Object           *obj,
   struct impErase  *msg)
{
   SIPTR  left, top;
   
   
   GetAttr (IA_Left, obj, &left);
   GetAttr (IA_Top, obj, &top);
   left += msg->imp_Offset.X;
   top += msg->imp_Offset.Y;
   
   EraseRect (msg->imp_RPort, left, top,
              left + msg->imp_Dimensions.Width - 1,
              top + msg->imp_Dimensions.Height - 1);
}


/* !!!dispatcher for chunkyiclass!!! */
IPTR  /*__saveds*/ 
chunkyi_dispatcher (
   Class   *cl,
   Object  *obj,
   Msg      msg)
{
   IPTR   retval = 0;
   
   switch (msg->MethodID)
   {
   case OM_NEW:
      if ((retval = DoSuperMethodA (cl, obj, msg)))
         chunkyim_new (cl, (Object *)retval, (struct opSet *)msg);
      break;
   case OM_DISPOSE:
      chunkyim_dispose (cl, obj);
      retval = DoSuperMethodA (cl, obj, msg);
      break;
   case OM_GET:
      if (!(retval = chunkyim_get (cl, obj, (struct opGet *)msg)))
         retval = DoSuperMethodA (cl, obj, msg);
      break;
   case OM_SET:
      retval = 1;
      DoSuperMethodA (cl, obj, msg);
      chunkyim_set (cl, obj, (struct opSet *)msg);
      break;
   case IM_DRAW:
   case IM_DRAWFRAME:
      chunkyim_draw (cl, obj, (struct impDraw *)msg);
      break;
   case IM_HITFRAME:
      retval = chunkyim_hitframe (cl, obj, (struct impHitTest *)msg);
      break;
   case IM_ERASEFRAME:
      chunkyim_eraseframe (cl, obj, (struct impErase *)msg);
      break;
   default: /* method not recognized by chunkyiclass */
      retval = DoSuperMethodA (cl, obj, msg);
      break;
   }
      
   return retval;
}


/* init chunkyiclass */
Class *
init_chunkyiclass (void)
{
   Class         *cl;
   extern IPTR   HookEntry();
   
   if ((cl = MakeClass (NULL, "imageclass", NULL,
                       sizeof (struct chunkyidata), 0)))
   {
      cl->cl_Dispatcher.h_Entry = HookEntry;
      cl->cl_Dispatcher.h_SubEntry = chunkyi_dispatcher;
   }
   
   return cl;
}


/* free chunkyiclass */
BOOL
free_chunkyiclass (
   Class  *cl)
{
   return FreeClass (cl);
}
