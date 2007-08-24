#include "Private.h"

//#define DB_ADDPALETTE
//#define DB_CREATESCHEME
//#define DB_SCHEME_SEARCH

/***************************************************************************/
/*** Private ***************************************************************/
/***************************************************************************/

///D_FindPen
struct in_FeelinColor * D_FindPen(struct FeelinList *Colors,uint32 Pen)
{
   struct in_FeelinColor *col;

   for (col = (struct in_FeelinColor *)(Colors -> Head) ; col ; col = col -> Next)
   {
      if (col -> Pen == Pen) break;
   }

   return col;
}
//+
///D_FindRGB
struct in_FeelinColor * D_FindRGB(struct FeelinList *Colors,uint32 ARGB)
{
   struct in_FeelinColor *col;

   for (col = (struct in_FeelinColor *)(Colors -> Head) ; col ; col = col -> Next)
   {
      if (col -> ARGB == ARGB) break;
   }

   return col;
}
//+
///D_MatchARGB
struct in_FeelinPalette * D_MatchARGB(struct FeelinClass *Class,FObject Obj,uint32 Count,uint32 *ARGB)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct in_FeelinPalette *pal;

   for (pal = (struct in_FeelinPalette *)(LOD -> PaletteList.Head) ; pal ; pal = pal -> Next)
   {
      if (pal -> ColorCount == Count)
      {
         BOOL ok = TRUE;
         uint32 i;

         for (i = 0 ; i < Count ; i++)
         {
            if (pal -> ARGB[i] != ARGB[i])
            {
               ok = FALSE; break;
            }
         }

         if (ok)
         {
            pal -> UserCount++; return pal;
         }
      }
   }
   return NULL;
}
//+

/***************************************************************************/
/*** Methods ***************************************************************/
/***************************************************************************/

///D_AddColor
F_METHODM(struct in_FeelinColor *,D_AddColor,FS_AddColor)
{
   struct LocalObjectData *LOD  = F_LOD(Class,Obj);
   struct in_FeelinColor *col;
   uint32 c,array[3];

   F_OPool(CUD -> ColorPool);

   c = (0x00FF0000 & Msg -> ARGB) >> 16; array[0] = c | (c << 8) | (c << 16) | (c << 24);
   c = (0x0000FF00 & Msg -> ARGB) >>  8; array[1] = c | (c << 8) | (c << 16) | (c << 24);
   c = (0x000000FF & Msg -> ARGB) >>  0; array[2] = c | (c << 8) | (c << 16) | (c << 24);

//   F_Log(0,"ARGB %06lx - %08lx %08lx %08lx",Msg -> ARGB,array[0],array[1],array[2]);

   if ((col = D_FindRGB(&LOD -> ColorList,Msg -> ARGB)) != NULL)
   {
/*
      {
         uint32 rgb[3];

         F_Log(0,"RGB 0x%06lx 0x%06lx 0x%06lx - PEN %ld",array[0],array[1],array[2],col -> Pen);

         GetRGB32(LOD -> ColorMap,col -> Pen,1,(uint32 *)(&rgb));

         F_Log(0,">>> 0x%06lx 0x%06lx 0x%06lx - FOUND",rgb[0],rgb[1],rgb[2]);
      }
*/
      col -> UserCount += 1;
   }
   else
   {
      uint32 flags = FF_Color_Shared;

      uint32 pen = ObtainBestPen(LOD -> ColorMap,
         array[0],array[1],array[2],
         OBP_Precision,  PRECISION_EXACT,
         OBP_FailIfBad,  TRUE,
         TAG_DONE);

      if (pen == -1)
      {
         pen   = FindColor(LOD -> ColorMap,array[0],array[1],array[2],1 << LOD -> Depth);
         flags = 0;
      }
/*
      {
         uint32 rgb[3];

         F_Log(0,"RGB 0x%06lx 0x%06lx 0x%06lx - PEN %ld",array[0],array[1],array[2],pen);

         GetRGB32(LOD -> ColorMap,pen,1,(uint32 *)(&rgb));

         F_Log(0,">>> 0x%06lx 0x%06lx 0x%06lx",rgb[0],rgb[1],rgb[2]);
      }
*/
      if ((col = D_FindPen(&LOD -> ColorList,pen)) != NULL)
      {
         if (FF_Color_Shared & flags)
         {
            ReleasePen(LOD -> ColorMap,pen);
         }
         col -> UserCount++;
      }
      else if ((col = F_NewP(CUD -> ColorPool,sizeof (struct in_FeelinColor))) != NULL)
      {
         col -> ARGB       = Msg -> ARGB;
         col -> Pen        = pen;
         col -> Flags      = flags;
         col -> UserCount  = 1;

         F_LinkTail(&LOD -> ColorList,(FNode *) col);
      }
   }

//   F_Log(0,"Color 0x%08lx - ARGB %06lx - UserCount %4ld - Pen %ld",col,col -> ARGB,col -> UserCount,col -> Pen);

   F_RPool(CUD -> ColorPool);

   return col;
}
//+
///D_RemColor
F_METHODM(void,D_RemColor,FS_RemColor)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct in_FeelinColor * col;

   F_OPool(CUD -> ColorPool);

   for (col = (struct in_FeelinColor *)(LOD -> ColorList.Head) ; col ; col = col -> Next)
   {
      if ((struct in_FeelinColor *)(Msg -> Color) == col)
      {
         if (--col -> UserCount == 0)
         {
   //         F_DebugOut("Display.DeletePen - Pen 0x%lx - RGB 0x%06lx\n",Pen,Pen -> xrgb);

            F_LinkRemove(&LOD -> ColorList,(FNode *) col);

            if (FF_Color_Shared & col -> Flags && col -> Pen != -1)
            {
               ReleasePen(LOD -> ColorMap,col -> Pen);
            }

            F_Dispose(col);
         }
         break;
      }
   }

   if (!col)
   {
      F_Log(FV_LOG_DEV,"Unknown Color 0x%08lx",Msg -> Color);
   }

   F_RPool(CUD -> ColorPool);
}
//+
///D_AddPalette
F_METHODM(struct in_FeelinPalette *,D_AddPalette,FS_AddPalette)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct in_FeelinPalette *pal;
   uint32 count = Msg -> Count;
   uint32 *argb  = Msg -> ARGBs;

   if (count > 256)
   {
      F_Log(FV_LOG_DEV,"Count %08lx - ARGB %08lx",count,argb);

      return NULL;
   }

   F_OPool(CUD -> PalettePool);

   pal = D_MatchARGB(Class,Obj,count,argb);

   if (!pal)
   {
      if ((pal = F_NewP(CUD -> PalettePool,sizeof (struct in_FeelinPalette) + count * 12)) != NULL)
      {
         uint32 i;

         pal -> UserCount  = 1;
         pal -> ColorCount = count;

         pal -> Pens   = (APTR)((uint32)(pal) + sizeof (struct in_FeelinPalette));
         pal -> ARGB   = (APTR)((uint32)(pal) + sizeof (struct in_FeelinPalette) + count * 4);
         pal -> Colors = (APTR)((uint32)(pal) + sizeof (struct in_FeelinPalette) + count * 8);
/*
///DB_ADDPALETTE
#ifdef DB_ADDPALETTE
         F_Log(FV_LOG_DEV,"Palette 0x%08lx - Pens 0x%08lx (%ld) - ARGB 0x%08lx - Colors 0x%08lx",pal,pal -> Pens,pal -> PensCount,pal -> ARGB,pal -> Colors);
#endif
//+
*/
         for (i = 0 ; i < count ; i++)
         {
            struct in_FeelinColor *col;

            if ((col = (struct in_FeelinColor *) F_Do(Obj,FM_AddColor,argb[i])) != NULL)
            {
               pal -> Pens[i]   = col -> Pen;
               pal -> ARGB[i]   = argb[i];
               pal -> Colors[i] = col;
///DB_ADDPALETTE
#ifdef DB_ADDPALETTE
               F_Log(FV_LOG_DEV,"RGB 0x%06lx - Color 0x%08lx - Pen %4ld",pal -> ARGB[i],pal -> Colors[i],pal -> Pens[i]);
#endif
//+
            }
         }

         F_LinkTail(&LOD -> PaletteList,(FNode *) pal);
      }
   }

   F_RPool(CUD -> PalettePool);

   return pal;
}
//+
///D_RemPalette
F_METHODM(void,D_RemPalette,FS_RemPalette)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   if (Msg -> Palette)
   {
      struct in_FeelinPalette *pal;
 
      F_OPool(CUD -> PalettePool);

      for (pal = (struct in_FeelinPalette *)(LOD -> PaletteList.Head) ; pal ; pal = pal -> Next)
      {
         if ((struct in_FeelinPalette *)(Msg -> Palette) == pal)
         {
            if (--pal -> UserCount == 0)
            {
               uint32 i;

               F_LinkRemove(&LOD -> PaletteList,(FNode *) pal);

               for (i = 0 ; i < pal -> ColorCount ; i++)
               {
                  F_Do(Obj,FM_RemColor,pal -> Colors[i]);
               }

               F_Dispose(pal);
            }
            break;
         }
      }

      if (!pal)
      {
         F_Log(FV_LOG_DEV,"Unknown Palette 0x%08lx",Msg -> Palette);
      }

      F_RPool(CUD -> PalettePool);
   }
}
//+

/*** Methods ***************************************************************/

///D_DecodeColor
F_METHODM(uint32,D_DecodeColor,FS_Display_DecodeColor)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   uint32 argb = 0x00000000;

   if (Msg -> Spec)
   {
/*** deprecated color types ************************************************/
      
      if (Msg -> Spec[1] == ':')
      {
         switch (Msg -> Spec[0])
         {
            case 's':
            {
               if (Msg -> Reference)
               {
                  int32 val;
///DB_SCHEME_SEARCH
#ifdef DB_SCHEME_SEARCH
               struct in_FeelinPalette * node;

               for (node = (struct in_FeelinPalette *)(LOD -> PaletteList.Head) ; node ; node = node -> Next)
               {
                  if (node == Msg -> Reference) break;
               }

               if (!node)
               {
                  F_Log(FV_LOG_DEV,"Spec '%s' (0x%08lx) - Unknow Reference 0x%08lx",Spec,Spec,Scheme);

                  for (node = (struct in_FeelinPalette *)(LOD -> PaletteList.Head) ; node ; node = node -> Next)
                  {
                     F_Log(FV_LOG_USER,"   >>> Palette 0x%08lx - ARGB 0x%08lx - Regs 0x%08lx - Pens 0x%08lx",node,node -> ARGB,node -> Regs,node -> Colors);
                  }

                  return NULL;
               }
            }
#endif
//+
                  stcd_l(Msg -> Spec+2,&val);

                  if (val < 0) val = 0;
                  if (val > FV_PEN_COUNT) val = FV_PEN_COUNT - 1;

                  argb = Msg -> Reference -> ARGB[val];
               }
               else
               {
                  F_Log(FV_LOG_DEV,"Spec '%s' (0x%08lx) - No Scheme",Msg -> Spec,Msg -> Spec);
               }
            }
            break;

            case 'p':
            {
               if (LOD -> ColorMap)
               {
                  uint32 rgb[3];
                  int32  val;
                  uint32 depth = MIN(8,LOD -> Depth);

                  stcd_l(Msg -> Spec+2,&val);

                  if ((val = val % (1 << depth)) < 0)
                  {
                     val = (1 << depth) + val;
                  }

                  GetRGB32(LOD -> ColorMap,val,1,(uint32 *)(&rgb));
                  argb = (0x00FF0000 & rgb[0]) | (0x0000FF00 & rgb[1]) | (0x000000FF & rgb[2]);
               }
               else
               {
                  F_Log(FV_LOG_DEV,"Spec '%s' (0x%08lx) - No ColorMap",Msg -> Spec,Msg -> Spec);
               }
            }
            break;

            case 'c':
            {
               stch_l(Msg -> Spec+2,(int32 *)(&argb));
            }
            break;

            case 'i':
            {
               if (LOD -> ColorMap && LOD -> DrawInfo)
               {
                  uint32 rgb[3];
                  int32  val;

                  stcd_l(Msg -> Spec+2,&val);

                  if (val < 0)          val = 0;
                  if (val > NUMDRIPENS) val = NUMDRIPENS - 1;

                  GetRGB32(LOD -> ColorMap,LOD -> DrawInfo -> dri_Pens[val],1,(uint32 *)(&rgb));

                  argb = (0x00FF0000 & rgb[0]) | (0x0000FF00 & rgb[1]) | (0x000000FF & rgb[2]);
               }
               else
               {
                  F_Log(FV_LOG_DEV,"Spec '%s' (0x%08lx) - ColorMap 0x%08lx - DrawInfo 0x%08lx",Msg -> Spec,Msg -> Spec,LOD -> ColorMap,LOD -> DrawInfo);
               }
            }
            break;

            default:
            {
               F_Log(FV_LOG_DEV,"Bad specifications '%s' (0x%08lx)",Msg -> Spec,Msg -> Spec);
            }
            break;
         }
      }
   
/*** new color types *******************************************************/
 
      else if (*Msg -> Spec == '#')
      {
         stch_l(Msg -> Spec + 1,(int32 *)(&argb));
      }
      else if (*Msg -> Spec >= 'a' && *Msg -> Spec <= 'z')
      {
         if (Msg -> Reference)
         {
            switch (*Msg -> Spec)
            {
               case 'd':
               {
                  if (F_StrCmp("dark",Msg -> Spec,4) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_Dark];
                  }
               }
               break;

               case 'f':
               {
                  if (F_StrCmp("fill",Msg -> Spec,4) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_Fill];
                  }
               }
               break;
               
               case 'h':
               {
                  int32 cmp;
                  
                  if ((cmp = F_StrCmp("halfdark",Msg -> Spec,8)) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_HalfDark];
                  }
                  else if (cmp > 0) break;
               
                  if ((cmp = F_StrCmp("halfshadow",Msg -> Spec,10)) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_HalfShadow];
                  }
                  else if (cmp > 0) break;
                                             
                  if ((cmp = F_StrCmp("halfshine",Msg -> Spec,9)) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_HalfShine];
                  }
                  else if (cmp > 0) break;

                  if ((cmp = F_StrCmp("highlight",Msg -> Spec,9)) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_Highlight];
                  }
                  else if (cmp > 0) break;
               }
               break;

               case 's':
               {
                  int32 cmp;
               
                  if ((cmp = F_StrCmp("shadow",Msg -> Spec,6)) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_Shadow];
                  }
                  else if (cmp > 0) break;

                  if ((cmp = F_StrCmp("shine",Msg -> Spec,5)) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_Shine];
                  }
                  else if (cmp > 0) break;
               }
               break;
               
               case 't':
               {
                  if (F_StrCmp("text",Msg -> Spec,4) == 0)
                  {
                     argb = Msg -> Reference -> ARGB[FV_Pen_Text];
                  }
               }
            }
         }
         else
         {
            F_Log(0,"color scheme reference not provided");
         }
      }
      else
      {
         int32 val;
         
         if (stcd_l(Msg -> Spec,&val) && LOD -> ColorMap)
         {
            uint32 rgb[3];

            if ((val = val % (1 << LOD -> Depth)) < 0)
            {
               val = (1 << LOD -> Depth) + val;
            }

            GetRGB32(LOD -> ColorMap,val,1,(uint32 *)(&rgb));
            argb = (0x00FF0000 & rgb[0]) | (0x0000FF00 & rgb[1]) | (0x000000FF & rgb[2]);
         }
         else
         {
            F_Log(FV_LOG_DEV,"colormap is not available");
         }
      }
   }

//   F_Log(0,"RGB%06lx (%s)",argb,Spec);

   return argb;
}
//+
///D_CreateColor
F_METHODM(struct in_FeelinColor *,D_CreateColor,FS_CreateColor)
{
   return (struct in_FeelinColor *) F_Do(Obj,FM_AddColor,F_Do(Obj,F_IDM(FM_Display_DecodeColor),Msg -> Spec,Msg -> Reference));
}
//+
///D_CreateColorScheme

enum  {

      FV_XML_ID_DARK = FV_XMLDOCUMENT_ID_DUMMY,
      FV_XML_ID_FILL,
      FV_XML_ID_HIGHLIGHT,
      FV_XML_ID_SCHEME,
      FV_XML_ID_SHINE,
      FV_XML_ID_TEXT,
      FV_XML_ID_CONTRAST,
      FV_XML_ID_SATURATION
      
      };
      
#define FF_HAS_CONTRAST                         (1 << 0)
#define FF_HAS_SATURATION                       (1 << 1)
      
F_METHODM(struct in_FeelinPalette *,D_CreateColorScheme,FS_CreateColorScheme)
{
    uint32 argb[FV_PEN_COUNT];
    int32  contrast=0;
    uint32 saturation=100;
    bits32 flags=0;

    /* item equals to -1 need to be created (obtained or computed) */
    
    /* en premier on va créer les valeurs par défault */

    argb[FV_Pen_Text]       = 0x00000000;
    argb[FV_Pen_Shine]      = 0x00FFFFFF;
    argb[FV_Pen_HalfShine]  = -1;
    argb[FV_Pen_Fill]       = 0x00BEBEBE;
    argb[FV_Pen_HalfShadow] = -1;
    argb[FV_Pen_Shadow]     = -1;
    argb[FV_Pen_HalfDark]   = -1;
    argb[FV_Pen_Dark]       = 0x00000000;
    argb[FV_Pen_Highlight]  = 0x00FF0000;

    if (Msg -> Reference)
    {
       CopyMem(Msg -> Reference -> ARGB,argb,sizeof (uint32) * FV_PEN_COUNT);
    }
    else
    {
       argb[FV_Pen_Shine]     = F_Do(Obj,F_IDM(FM_Display_DecodeColor),"i:3",NULL);
       argb[FV_Pen_Fill]      = F_Do(Obj,F_IDM(FM_Display_DecodeColor),"i:7",NULL);
       argb[FV_Pen_Dark]      = F_Do(Obj,F_IDM(FM_Display_DecodeColor),"i:4",NULL);
       argb[FV_Pen_Text]      = F_Do(Obj,F_IDM(FM_Display_DecodeColor),"i:2",NULL);
       argb[FV_Pen_Highlight] = F_Do(Obj,F_IDM(FM_Display_DecodeColor),"i:8",NULL);
    }

    /* parse pens specifications */

    if (Msg -> Spec)
    {
        STATIC FDOCID xml_scheme_ids[] =
        {
            { "contrast",    8, FV_XML_ID_CONTRAST   },
            { "dark",        4, FV_XML_ID_DARK       },
            { "fill",        4, FV_XML_ID_FILL       },
            { "highlight",   9, FV_XML_ID_HIGHLIGHT  },
            { "saturation", 10, FV_XML_ID_SATURATION },
            { "scheme",      6, FV_XML_ID_SCHEME     },
            { "shine",       5, FV_XML_ID_SHINE      },
            { "text",        4, FV_XML_ID_TEXT       },
               
            F_ARRAY_END
        };
   
        FXMLMarkup *markup;
     
        F_Do(CUD -> XMLScheme,FM_Lock,FF_Lock_Exclusive);
                     
        F_Do(CUD -> XMLScheme,FM_Set,

            F_ID(CUD -> XMLIDs,FA_Document_Source),      Msg -> Spec,
            F_ID(CUD -> XMLIDs,FA_Document_SourceType),  FV_Document_SourceType_Memory,

            TAG_DONE);

        F_Do(CUD -> XMLScheme,F_ID(CUD -> XMLIDs,FM_Document_AddIDs), xml_scheme_ids);
        
        for (markup = (FXMLMarkup *) F_Get(CUD -> XMLScheme,F_ID(CUD -> XMLIDs,FA_XMLDocument_Markups)) ; markup ; markup = markup -> Next)
        {
            if (markup -> Name -> ID == FV_XML_ID_SCHEME)
            {
                FXMLAttribute *attribute;
               
                for (attribute = (FXMLAttribute *) markup -> AttributesList.Head; attribute ; attribute = attribute -> Next)
                {
                    int32 pen = -1;
     
                    switch (attribute -> Name -> ID)
                    {
                        case FV_XML_ID_DARK:       pen = FV_Pen_Dark;      break;
                        case FV_XML_ID_SHINE:      pen = FV_Pen_Shine;     break;
                        case FV_XML_ID_FILL:       pen = FV_Pen_Fill;      break;
                        case FV_XML_ID_TEXT:       pen = FV_Pen_Text;      break;
                        case FV_XML_ID_HIGHLIGHT:  pen = FV_Pen_Highlight; break;
                        case FV_XML_ID_CONTRAST:   flags |= FF_HAS_CONTRAST;   contrast = F_Do(CUD -> XMLScheme, F_ID(CUD -> XMLIDs,FM_Document_Resolve), attribute -> Data, FV_TYPE_INTEGER, NULL, NULL); break;
                        case FV_XML_ID_SATURATION: flags |= FF_HAS_SATURATION; saturation = F_Do(CUD -> XMLScheme, F_ID(CUD -> XMLIDs,FM_Document_Resolve), attribute -> Data, FV_TYPE_INTEGER, NULL, NULL); break;
                    }
                
                    if (pen != -1)
                    {
                        argb[pen] = F_Do(Obj,F_IDM(FM_Display_DecodeColor),attribute -> Data,Msg -> Reference);
                    }
                }
            }
        }

        F_Do(CUD -> XMLScheme,FM_Unlock);
    };

    if (flags & (FF_HAS_CONTRAST | FF_HAS_SATURATION))
    {
        uint8 fr = (0x00FF0000 & argb[FV_Pen_Fill]) >> 16;
        uint8 fg = (0x0000FF00 & argb[FV_Pen_Fill]) >>  8;
        uint8 fb = (0x000000FF & argb[FV_Pen_Fill]);
        uint8 fm = MAX(fr,MAX(fg,fb));

        uint8 sr = fr + ((255 - fr) * (100 + contrast) / 200);
        uint8 sg = fg + ((255 - fg) * (100 + contrast) / 200);
        uint8 sb = fb + ((255 - fb) * (100 + contrast) / 200);
        uint8 sm = MAX(sr,MAX(sb,sg));

        uint8 dr = fr - (fr * (100 + contrast) / 200);
        uint8 dg = fg - (fg * (100 + contrast) / 200);
        uint8 db = fb - (fb * (100 + contrast) / 200);
        uint8 dm = MAX(dr,MAX(db,dg));
        
        if (FF_HAS_SATURATION)
        {
            sr = sm - ((sm - sr) * saturation / 100);
            sg = sm - ((sm - sg) * saturation / 100);
            sb = sm - ((sm - sb) * saturation / 100);

            dr = dm - ((dm - dr) * saturation / 100);
            dg = dm - ((dm - dg) * saturation / 100);
            db = dm - ((dm - db) * saturation / 100);

            fr = fm - ((fm - fr) * saturation / 100);
            fg = fm - ((fm - fg) * saturation / 100);
            fb = fm - ((fm - fb) * saturation / 100);
        }
        
        argb[FV_Pen_Shine] = (sr << 16) | (sg << 8) | sb;
        argb[FV_Pen_Dark]  = (dr << 16) | (dg << 8) | db;
        argb[FV_Pen_Fill]  = (fr << 16) | (fg << 8) | fb;
    }
    
    /* create half colors */

    argb[FV_Pen_Shadow]     = MakeXGrad(argb[FV_Pen_Fill],   argb[FV_Pen_Dark]);
    argb[FV_Pen_HalfShine]  = MakeXGrad(argb[FV_Pen_Shine],  argb[FV_Pen_Fill]);
    argb[FV_Pen_HalfShadow] = MakeXGrad(argb[FV_Pen_Fill],   argb[FV_Pen_Shadow]);
    argb[FV_Pen_HalfDark]   = MakeXGrad(argb[FV_Pen_Shadow], argb[FV_Pen_Dark]);

///DB_CREATESCHEME
#ifdef DB_CREATESCHEME
   {
      uint32 i;

      FPrintf(FeelinBase -> Console,"Ref 0x%08lx - ARGB",Msg -> Reference);

      for (i = 0 ; i < FV_PEN_COUNT ; i++)
      {
         FPrintf(FeelinBase -> Console," %06lx",argb[i]);
      }
      FPrintf(FeelinBase -> Console,"\n");
      Flush(FeelinBase -> Console);
   }
#endif
//+

    return (struct in_FeelinPalette *) F_Do(Obj,FM_AddPalette,FV_PEN_COUNT,&argb);
}
//+

