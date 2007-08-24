#include "Private.h"

///Raster_
STATIC FObject Raster_(uint32 Raster,struct FeelinBase *FeelinBase)
{
   return   AreaObject,
            FA_ChainToCycle,  FALSE,
            FA_InputMode,     FV_InputMode_Immediate,
            FA_Back,          Raster,
            FA_MinWidth,      4,
            FA_MinHeight,     4,
            End;
}
//+

#define RASTER_NUMBER 28
#define COMPATIBILITY

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New

/*  FC_Adjust  create  the  preview  and  the  reset  gadget  when   needed
(FA_Adjust_Preview = TRUE). I just have to add my own objects to the group,
and setup notifications. */

F_METHOD(FObject,Adjust_New)
{
   FObject raster[RASTER_NUMBER + 1];
 
   STATIC uint8 raster_id[] =
   {
     FI_Shine_HalfShine,FI_Shine_Fill,FI_Shine_HalfShadow,FI_Shine_Shadow,
     FI_Shine_HalfDark,FI_Shine_Dark,
     FI_HalfShine_Fill,FI_HalfShine_HalfShadow, FI_HalfShine_Shadow,FI_HalfShine_HalfDark,FI_HalfShine_Dark,
     FI_Fill_HalfShadow, FI_Fill_Shadow,FI_Fill_HalfDark,FI_Fill_Dark,
     FI_HalfShadow_Shadow, FI_HalfShadow_HalfDark,FI_HalfShadow_Dark,
     FI_Shadow_HalfDark,FI_Shadow_Dark,
     FI_HalfDark_Dark,
     FI_Shine_Highlight,FI_HalfShine_Highlight,FI_Fill_Highlight,
     FI_HalfShadow_Highlight,FI_Shadow_Highlight,FI_HalfDark_Highlight,FI_Dark_Highlight
   };
 
   FObject raster_grp = GroupObject, FA_Group_VSpacing, 0,
      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[0] = Raster_(FI_Shine_HalfShine,FeelinBase),
         Child, raster[1] = Raster_(FI_Shine_Fill,FeelinBase),
         Child, raster[2] = Raster_(FI_Shine_HalfShadow,FeelinBase),
         Child, raster[3] = Raster_(FI_Shine_Shadow,FeelinBase),
         Child, raster[4] = Raster_(FI_Shine_HalfDark,FeelinBase),
         Child, raster[5] = Raster_(FI_Shine_Dark,FeelinBase),
      End,

      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[ 6] = Raster_(FI_HalfShine_Fill,FeelinBase),
         Child, raster[ 7] = Raster_(FI_HalfShine_HalfShadow,FeelinBase),
         Child, raster[ 8] = Raster_(FI_HalfShine_Shadow,FeelinBase),
         Child, raster[ 9] = Raster_(FI_HalfShine_HalfDark,FeelinBase),
         Child, raster[10] = Raster_(FI_HalfShine_Dark,FeelinBase),
      End,

      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[11] = Raster_(FI_Fill_HalfShadow,FeelinBase),
         Child, raster[12] = Raster_(FI_Fill_Shadow,FeelinBase),
         Child, raster[13] = Raster_(FI_Fill_HalfDark,FeelinBase),
         Child, raster[14] = Raster_(FI_Fill_Dark,FeelinBase),
      End,

      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[15] = Raster_(FI_HalfShadow_Shadow,FeelinBase),
         Child, raster[16] = Raster_(FI_HalfShadow_HalfDark,FeelinBase),
         Child, raster[17] = Raster_(FI_HalfShadow_Dark,FeelinBase),
      End,

      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[18] = Raster_(FI_Shadow_HalfDark,FeelinBase),
         Child, raster[19] = Raster_(FI_Shadow_Dark,FeelinBase),
      End,

      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[20] = Raster_(FI_HalfDark_Dark,FeelinBase),
      End,

      Child, HGroup, FA_Group_HSpacing, 0,
         Child, raster[21] = Raster_(FI_Shine_Highlight,FeelinBase),
         Child, raster[22] = Raster_(FI_HalfShine_Highlight,FeelinBase),
         Child, raster[23] = Raster_(FI_Fill_Highlight,FeelinBase),
         Child, raster[24] = Raster_(FI_HalfShadow_Highlight,FeelinBase),
         Child, raster[25] = Raster_(FI_Shadow_Highlight,FeelinBase),
         Child, raster[26] = Raster_(FI_HalfDark_Highlight,FeelinBase),
         Child, raster[27] = Raster_(FI_Dark_Highlight,FeelinBase),
      End,
   End;
      
   raster[28] = NULL;

   if (!raster_grp)
   {
      return NULL;
   }
 
   if (F_SuperDo(Class,Obj,Method,
      
      "PreviewTitle", "Rasters",
      "PreviewClass", "PreviewImage",
      "Separator",     FV_ImageDisplay_Separator,

      Child, raster_grp,

      TAG_MORE,Msg))
   {
      FObject *en;
      uint32 i;
 
      for (en = raster, i=0 ; *en ; en++, i++)
      {
         F_Do(*en,FM_Notify, FA_Selected,TRUE, Obj,F_IDM(FM_Adjust_ToString),2,raster_id[i],TRUE);
      }

      return Obj;
   }

   return NULL;
}
//+

STATIC FDOCValue scheme_strings[] =
{
   { "text",        FV_Pen_Text        },
   { "shine",       FV_Pen_Shine       },
   { "halfshine",   FV_Pen_HalfShine   },
   { "fill",        FV_Pen_Fill        },
   { "halfshadow",  FV_Pen_HalfShadow  },
   { "shadow",      FV_Pen_Shadow      },
   { "halfdark",    FV_Pen_HalfDark    },
   { "dark",        FV_Pen_Dark        },
   { "highlight",   FV_Pen_Highlight   },
   
   F_ARRAY_END
};
 
///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (Msg -> Spec)
   {
      if (F_StrCmp("<image type='raster' ",Msg -> Spec,21) == 0)
      {
         return TRUE;
      }
#ifdef COMPATIBILITY
      else if (Msg -> Spec[0] == 'r' && Msg -> Spec[1] == ':')
      {
         return 0xFFFF0000 | (uint32) atol(Msg -> Spec + 2);
      }
#endif
   }
   return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
   if (Msg -> Notify)
   {
      STRPTR str = F_StrNew(NULL,"<image type='raster' id='%ld' />",Msg -> Spec);
      uint32 rc = F_SuperDo(Class,Obj,Method,str,Msg -> Notify);
         
      F_Dispose(str);

      return rc;
   }

   return F_SUPERDO();
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   /* dummy method */
}
//+
