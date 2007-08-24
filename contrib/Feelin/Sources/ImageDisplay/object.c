#include "Private.h"

///id_image_new
APTR id_image_new(STRPTR Spec,struct in_CodeTable **TablePtr)
{
    APTR data=NULL;

    if (Spec == NULL)
    {
        return NULL;
    }
 
    if (*Spec == '<')
    {
        STATIC FDOCID xml_ids[] =
        {
            { "image",          5, FV_XML_ID_IMAGE          },
            { "type",           4, FV_XML_ID_TYPE           },

            { "id",             2, FV_XML_ID_ID             },

            { "angle",          5, FV_XML_ID_ANGLE          },
            { "start",          5, FV_XML_ID_START          },
            { "middle",         6, FV_XML_ID_MIDDLE         },
            { "end",            3, FV_XML_ID_END            },
            { "first-start",   11, FV_XML_ID_FIRST_START    },
            { "first-end",      9, FV_XML_ID_FIRST_END      },
            { "second-start",  12, FV_XML_ID_SECOND_START   },
            { "second-end",    10, FV_XML_ID_SECOND_END     },
                 
            { "src",            3, FV_XML_ID_SRC            },
            { "mode",           4, FV_XML_ID_MODE           },
            { "filter",         6, FV_XML_ID_FILTER         },

            F_ARRAY_END
        };

        FXMLMarkup *markup;

        F_Do(CUD -> XMLDocument,FM_Lock,FF_Lock_Exclusive);

        F_Do(CUD -> XMLDocument,FM_Set,

            F_ID(CUD -> IDs,FA_Document_Source), Spec,
            F_ID(CUD -> IDs,FA_Document_SourceType), FV_Document_SourceType_Memory,

            TAG_DONE);

        F_Do(CUD -> XMLDocument,F_ID(CUD -> IDs,FM_Document_AddIDs), xml_ids);

        for (markup = (FXMLMarkup *) F_Get(CUD -> XMLDocument,F_ID(CUD -> IDs,FA_XMLDocument_Markups)) ; markup ; markup = markup -> Next)
        {
            if (markup -> Name -> ID == FV_XML_ID_IMAGE)
            {
                FXMLAttribute *attribute;

                for (attribute = (FXMLAttribute *) markup -> AttributesList.Head ; attribute ; attribute = attribute -> Next)
                {
                    if (attribute -> Name -> ID == FV_XML_ID_TYPE)
                    {
                        STATIC FDOCValue image_type_values[] =
                        {
                            { "raster",    (uint32) &id_raster_table    },
                            { "gradient",  (uint32) &id_gradient_table  },
                            { "brush",     (uint32) &id_brush_table     },
                            { "picture",   (uint32) &id_picture_table   },
                            { "hook",      (uint32) &id_hook_table      },

                            F_ARRAY_END
                        };
                       
                        struct in_CodeTable *table = (struct in_CodeTable *) F_Do(CUD -> XMLDocument,F_ID(CUD -> IDs,FM_Document_Resolve),attribute -> Data,0,image_type_values,NULL);
                       
                        if (table)
                        {
                            if ((data = table -> create((STRPTR) markup -> AttributesList.Head)))
                            {
                                *TablePtr = table;
                            }
                        }
                        else
                        {
                            F_Log(0,"Unsupported type (%s) in (%s)",attribute -> Data,Spec);
                        }
                    }
                }
                break;
            }
        }
        
        F_Do(CUD -> XMLDocument,FM_Unlock);
    }
    else if (Spec[1] == ':')
    {
        F_Log(0,"(%s) is deprecated",Spec);
    }
    else
    {
        if ((data = id_color_table.create(Spec)))
        {
            *TablePtr = &id_color_table;
        }
    }

    if (!data)
    {
       *TablePtr = &id_raster_table;
       data = (APTR) FI_Fill;

  //         F_Log(FV_LOG_USER,"Unable to create image ! (0x%08lx)(%s)",Spec,Spec);
    }

    return data;
}
//+
///id_image_dispose
void id_image_dispose(struct in_CodeTable *Table,APTR Data)
{
//   F_Log(0,"id_image_delete (0x%08lx)",image);
    F_OPool(CUD -> Pool);

    if (Table && Table -> delete)
    {
        Table -> delete(Data);
    }
   
    F_RPool(CUD -> Pool);
}
//+

/*** Methods ***************************************************************/

///ID_New
F_METHOD(APTR,ID_New)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);

   if (F_SUPERDO())
   {
      if (LOD -> Methods) return Obj;
   }
   return NULL;
}
//+
///ID_Dispose
F_METHOD(void,ID_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   id_image_dispose(LOD -> Methods,LOD -> Data);
   
   LOD -> Methods = NULL;
   LOD -> Data = NULL;
}
//+
///ID_Get
F_METHOD(void,ID_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   APTR data=NULL;
   in_Code_Get *func=NULL;

   if (LOD -> Methods == &id_double_table)
   {
      data = ((struct in_Double *)(LOD -> Data)) -> Data[(FF_IMAGE_SELECTED & LOD -> Flags) ? 1 : 0];

      if (((struct in_Double *)(LOD -> Data)) -> Methods[(FF_IMAGE_SELECTED & LOD -> Flags) ? 1 : 0])
      {
         func = ((struct in_Double *)(LOD -> Data)) -> Methods[(FF_IMAGE_SELECTED & LOD -> Flags) ? 1 : 0] -> get;
      }
   }
   else if (LOD -> Methods)
   {
      data = LOD -> Data;
      func = LOD -> Methods -> get;
   }

   while  (F_DynamicNTI(&Tags,&item,NULL))
   switch (item.ti_Tag)
   {
      case FA_ImageDisplay_State:
      {
         F_STORE(LOD -> Flags);
      }
      break;

      case FA_ImageDisplay_Width:
      case FA_ImageDisplay_Height:
      {
         if (func)
         {
            F_STORE(func(data,item.ti_Tag));
         }
         else F_STORE(8);
      }
      break;
       
      case FA_ImageDisplay_Mask:
      {
         if (func)
         {
            F_STORE(func(data,item.ti_Tag));
         }
         else F_STORE(FALSE);
      }
      break;
   }
   F_SUPERDO();
}
//+
///ID_Set
F_METHOD(void,ID_Set)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,NULL))
   switch (item.ti_Tag)
   {
      case FA_ImageDisplay_State:
      {
         LOD -> Flags &= ~FF_IMAGE_SELECTED;

         if (item.ti_Data == FV_Image_Selected)
         {
            LOD -> Flags |= FF_IMAGE_SELECTED;
         }
      }
      break;

      case FA_ImageDisplay_Spec:
      {
         FRender *render = LOD -> Render;

         F_OPool(CUD -> Pool);

         if (render)
         {
            F_Do(Obj,FM_ImageDisplay_Cleanup,render); LOD -> Render = render;
         }
      
         id_image_dispose(LOD -> Methods,LOD -> Data);
         
         LOD -> Methods = NULL;
         LOD -> Data = NULL;
         
         /*** create new image ***/

         if (item.ti_Data)
         {
            if (item.ti_Data >= FI_Shine && item.ti_Data <= FI_Shine_Highlight)
            {
//               F_Log(0,"SPEC %ld",item.ti_Data);
               
               LOD -> Methods = &id_raster_table;
               LOD -> Data = (APTR) item.ti_Data;
            }
            else
            {
               STRPTR rimg = F_StrNew(NULL,"%s",item.ti_Data);
         
//               F_Log(0,"SPEC (0x%08lx)(%s)",item.ti_Data,item.ti_Data);
            
               if (rimg)
               {
                  STRPTR simg = rimg;

                  while (*simg && *simg != FV_ImageDisplay_Separator) simg++;

                  if (simg && *simg  == FV_ImageDisplay_Separator)
                  {
                     *simg++ = 0;
                  }
                  else
                  {
                     simg = NULL;
                  }
                  
                  if (rimg && simg)
                  {
//                     F_Log(0,"DUAL IMAGE:\n   >> (%s)\n   >> (%s)",rimg,simg);

                     if ((LOD -> Data = ((in_Code_CreateDouble *)(id_double_table.create))(rimg,simg)))
                     {
                        LOD -> Methods = &id_double_table;
                     }
                  }
                  else if (rimg)
                  {
                     LOD -> Data = id_image_new(rimg,&LOD -> Methods);
                  }
               
                  F_Dispose(rimg);
               }

               if (!LOD -> Methods)
               {
                  LOD -> Methods = &id_raster_table;
                  LOD -> Data = (APTR) FI_Fill;
               }
            }
         }

         if (LOD -> Methods && render)
         {
            F_Do(Obj,FM_ImageDisplay_Setup,render);
         }

         F_RPool(CUD -> Pool);
      }
      break;
   
      case FA_ImageDisplay_Origin:
      {
         LOD -> Origin = (FBox *)(item.ti_Data);
      }
      break;
   }

   F_SUPERDO();
}
//+
///ID_Setup
F_METHODM(LONG,ID_Setup,FS_ImageDisplay_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if ((LOD -> Render = Msg -> Render))
   {
      if (LOD -> Methods && LOD -> Methods -> setup)
      {
         return LOD -> Methods -> setup(LOD -> Data,Class,LOD -> Render);
      }
      return TRUE;
   }
   return FALSE;
}
//+
///ID_Cleanup
F_METHODM(void,ID_Cleanup,FS_ImageDisplay_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> Render)
   {
      if (LOD -> Methods && LOD -> Methods -> cleanup)
      {
         LOD -> Methods -> cleanup(LOD -> Data, Class, LOD -> Render);
      }
   
      LOD -> Render = NULL;
   }
}
//+
///ID_Draw
F_METHODM(void,ID_Draw,FS_ImageDisplay_Draw)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_SPool(CUD.ImagePool);

    if (Msg -> Render && Msg -> Render -> RPort && Msg -> Rect)
    {
        APTR data=NULL;
        in_Code_Draw *func=NULL;

        if (LOD -> Methods == &id_double_table)
        {
            data = ((struct in_Double *)(LOD -> Data)) -> Data[(FF_IMAGE_SELECTED & LOD -> Flags) ? 1 : 0];
            
            if (((struct in_Double *)(LOD -> Data)) -> Methods[(FF_IMAGE_SELECTED & LOD -> Flags) ? 1 : 0])
            {
                func = ((struct in_Double *)(LOD -> Data)) -> Methods[(FF_IMAGE_SELECTED & LOD -> Flags) ? 1 : 0] -> draw;
            }
        }
        else if (LOD -> Methods)
        {
            data = LOD -> Data;
            func = LOD -> Methods -> draw;
        }
     
        if (func)
        {
            struct in_DrawMessage msg;

            msg.Render  = Msg -> Render;
            msg.Origin  = LOD -> Origin;
            msg.Flags   = LOD -> Flags;
            
            if (FF_ImageDisplay_Region & Msg -> Flags)
            {
                struct RegionRectangle *rr;
                FRect r;
                msg.Rect = &r;

                for (rr = ((struct Region *)(Msg -> Rect)) -> RegionRectangle ; rr ; rr = rr -> Next)
                {
                    r.x1 = ((struct Region *)(Msg -> Rect)) -> bounds.MinX + rr -> bounds.MinX;
                    r.y1 = ((struct Region *)(Msg -> Rect)) -> bounds.MinY + rr -> bounds.MinY;
                    r.x2 = ((struct Region *)(Msg -> Rect)) -> bounds.MinX + rr -> bounds.MaxX;
                    r.y2 = ((struct Region *)(Msg -> Rect)) -> bounds.MinY + rr -> bounds.MaxY;

                    if (r.x1 <= r.x2 && r.y1 <= r.y2)
                    {
                        if (!func(data, Class, &msg))
                        {
                            SetAPen(Msg -> Render -> RPort,Msg -> Render -> Palette -> Pens[FV_Pen_Fill]);
                            RectFill(Msg -> Render -> RPort,r.x1,r.y1,r.x2,r.y2);
                        }
                    }
                }
            }
            else if (Msg -> Rect -> x1 <= Msg -> Rect -> x2 && Msg -> Rect -> y1 <= Msg -> Rect -> y2)
            {
                msg.Rect = Msg -> Rect;

                if (!func(data,Class,&msg))
                {
                    SetAPen(Msg -> Render -> RPort,Msg -> Render -> Palette -> Pens[FV_Pen_Fill]);
                    RectFill(Msg -> Render -> RPort,Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);
                }
            }
        }
        else
        {
           if (FF_ImageDisplay_Region & Msg -> Flags)
           {
              struct RegionRectangle *rr;
              FRect r;

              for (rr = ((struct Region *)(Msg -> Rect)) -> RegionRectangle ; rr ; rr = rr -> Next)
              {
                 r.x1 = ((struct Region *)(Msg -> Rect)) -> bounds.MinX + rr -> bounds.MinX;
                 r.y1 = ((struct Region *)(Msg -> Rect)) -> bounds.MinY + rr -> bounds.MinY;
                 r.x2 = ((struct Region *)(Msg -> Rect)) -> bounds.MinX + rr -> bounds.MaxX;
                 r.y2 = ((struct Region *)(Msg -> Rect)) -> bounds.MinY + rr -> bounds.MaxY;

                 if (r.x1 <= r.x2 && r.y1 <= r.y2)
                 {
                    SetAPen(Msg -> Render -> RPort,Msg -> Render -> Palette -> Pens[FV_Pen_Fill]);
                    RectFill(Msg -> Render -> RPort,r.x1,r.y1,r.x2,r.y2);
                 }
              }
           }
           else if (Msg -> Rect -> x1 <= Msg -> Rect -> x2 && Msg -> Rect -> y1 <= Msg -> Rect -> y2)
           {
              SetAPen(Msg -> Render -> RPort,Msg -> Render -> Palette -> Pens[FV_Pen_Fill]);
              RectFill(Msg -> Render -> RPort,Msg -> Rect -> x1,Msg -> Rect -> y1,Msg -> Rect -> x2,Msg -> Rect -> y2);
           }
        }
    }
//   F_RPool(CUD.ImagePool);
}
//+

