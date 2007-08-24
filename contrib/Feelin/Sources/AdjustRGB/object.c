#include "Private.h"

#define COMPATIBILITY

enum  {

      NOTIFY_R = 1,
      NOTIFY_G,
      NOTIFY_B,
      NOTIFY_H,
      NOTIFY_S,
      NOTIFY_V,
      
      NOTIFY_STRING_R,
      NOTIFY_STRING_G,
      NOTIFY_STRING_B,
      NOTIFY_STRING_H,
      NOTIFY_STRING_S,
      NOTIFY_STRING_V,
      
      NOTIFY_HTML

      };

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

#ifdef __AROS__
#include "../_support/rgb_to_hsv.c"
#include "../_support/hsv_to_rgb.c"
#else
#include "Feelin:Sources/_support/rgb_to_hsv.c"
#include "Feelin:Sources/_support/hsv_to_rgb.c"
#endif

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Adjust_New
F_METHOD_NEW(Adjust_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_AREA_SAVE_PUBLIC;
 
   if (F_SuperDo(Class,Obj,Method,

      "PreviewTitle", F_CAT(TITLE),
      "PreviewClass", "PreviewImage",
      "Separator",     FV_ImageDisplay_Separator,

      Child, VGroup,
         Child, RowGroup(3), GroupFrame, FA_Frame_Title,F_CAT(RGB),
            Child, HLabel(F_CAT(RED)),
            Child, LOD -> r = SliderObject, FA_ChainToCycle,FALSE, "Format",NULL, "Max",255, "Horizontal",TRUE,"SetMax","Height", FA_Back,"<image type='gradient' angle='90' start='#000000' end='#FF0000' />", End,
            Child, LOD -> string_r = StringObject, FA_Weight,20, FA_SetMax,FV_SetHeight, "Max",3, "Contents","0", "AdvanceOnCR",FALSE, "Accept",FV_String_Decimal, End,
            
            Child, HLabel(F_CAT(GREEN)),
            Child, LOD -> g = SliderObject, FA_ChainToCycle,FALSE, "Format",NULL, "Max",255, "Horizontal",TRUE,"SetMax","Height", FA_Back,"<image type='gradient' angle='90' start='#000000' end='#00FF00' />", End,
            Child, LOD -> string_g = StringObject, FA_Weight,20, FA_SetMax,FV_SetHeight, "Max",3, "Contents","0", "AdvanceOnCR",FALSE, "Accept",FV_String_Decimal, End,
            
            Child, HLabel(F_CAT(BLUE)),
            Child, LOD -> b = SliderObject, FA_ChainToCycle,FALSE, "Format",NULL, "Max",255, "Horizontal",TRUE,"SetMax","Height", FA_Back,"<image type='gradient' angle='90' start='#000000' end='#0000FF' />", End,
            Child, LOD -> string_b = StringObject, FA_Weight,20, FA_SetMax,FV_SetHeight, "Max",3, "Contents","0", "AdvanceOnCR",FALSE, "Accept",FV_String_Decimal, End,
         End,
          
         Child, RowGroup(3), GroupFrame, FA_Frame_Title,F_CAT(HSV),
            Child, HLabel(F_CAT(HUE)),
            Child, LOD -> h = SliderObject, FA_ChainToCycle,FALSE, "Format",NULL, "Max",360, "Horizontal",TRUE,"SetMax","Height", End,
            Child, LOD -> string_h = StringObject, FA_Weight,20, FA_SetMax,FV_SetHeight, "Max",3, "Contents","0", "AdvanceOnCR",FALSE, "Accept",FV_String_Decimal, End,
            
            Child, HLabel(F_CAT(SATURATION)),
            Child, LOD -> s = SliderObject, FA_ChainToCycle,FALSE, "Format",NULL, "Max",100, "Horizontal",TRUE,"SetMax","Height", End,
            Child, LOD -> string_s = StringObject, FA_Weight,20, FA_SetMax,FV_SetHeight, "Max",3, "Contents","0", "AdvanceOnCR",FALSE, "Accept",FV_String_Decimal, End,
            
            Child, HLabel(F_CAT(VALUE)),
            Child, LOD -> v = SliderObject, FA_ChainToCycle,FALSE, "Format",NULL, "Max",100, "Horizontal",TRUE,"SetMax","Height", FA_Back,"<image type='gradient' angle='90' start='#000000' end='#FFFFFF' />", End,
            Child, LOD -> string_v = StringObject, FA_Weight,20, FA_SetMax,FV_SetHeight, "Max",3, "Contents","0", "AdvanceOnCR",FALSE, "Accept",FV_String_Decimal, End,
         End,
         
         Child, HGroup, GroupFrame, FA_Frame_Title,"HTML",
            Child, LOD -> html = StringObject, FA_SetMax,FV_SetHeight, "Contents","000000", "AdvanceOnCR",FALSE, "Max",6, "Accept",FV_String_Hexadecimal, End,
            Child, LOD -> pick = F_MakeObj(FV_MakeObj_Button,F_CAT(PICK), FA_InputMode,FV_InputMode_Immediate, FA_SetMax,FV_SetBoth, FA_ContextHelp,F_CAT(PICK_HELP), TAG_DONE),
         End,
      End,

      TAG_MORE,Msg))
   {
      F_Do(LOD -> r,FM_Notify,F_IDO(FA_Numeric_Value),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_R);
      F_Do(LOD -> g,FM_Notify,F_IDO(FA_Numeric_Value),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_G);
      F_Do(LOD -> b,FM_Notify,F_IDO(FA_Numeric_Value),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_B);
      F_Do(LOD -> h,FM_Notify,F_IDO(FA_Numeric_Value),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_H);
      F_Do(LOD -> s,FM_Notify,F_IDO(FA_Numeric_Value),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_S);
      F_Do(LOD -> v,FM_Notify,F_IDO(FA_Numeric_Value),FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_V);
      
      F_Do(LOD -> string_r,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_STRING_R);
      F_Do(LOD -> string_g,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_STRING_G);
      F_Do(LOD -> string_b,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_STRING_B);
      F_Do(LOD -> string_h,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_STRING_H);
      F_Do(LOD -> string_s,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_STRING_S);
      F_Do(LOD -> string_v,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_STRING_V);
 
      F_Do(LOD -> html,FM_Notify,"Changed",FV_Notify_Always,Obj,F_IDM(FM_Adjust_ToString),3,FV_Notify_Value,TRUE,NOTIFY_HTML);
      F_Do(LOD -> pick,FM_Notify,FA_Selected,TRUE,Obj,F_IDM(FM_AdjustRGB_PickBegin),0);
 
      return Obj;
   }

   return NULL;
}
//+
///Adjust_HandleEvent
F_METHOD_HANDLEEVENT(Adjust_HandleEvent)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   switch (Msg -> Event -> Class)
   {
      case FF_EVENT_MOTION:
      {
         struct Window *win = (struct Window *) F_Get(_win,FA_Window);
         
         if (win)
         {
            uint32 rgb = F_Do(_display,(uint32) "PixelRead",Msg -> Event -> MouseX + win -> LeftEdge,Msg -> Event -> MouseY + win -> TopEdge);
            STRPTR spec = F_StrNew(NULL,"#%06lx",rgb);
             
//            F_Log(0,"motion : (%ld:%ld) >> %08lx (%s)",Msg -> Event -> MouseX + win -> LeftEdge,Msg -> Event -> MouseY + win -> TopEdge,rgb,spec);

            if (F_StrCmp(spec,LOD -> pick_spec,ALL) != 0)
            {
               F_Dispose(LOD -> pick_spec);
               LOD -> pick_spec = spec;
 
               F_SuperDo(Class,Obj,F_IDM(FM_Adjust_ToString),LOD -> pick_spec,TRUE);
            
               F_Do(LOD -> html,FM_Set, FA_NoNotify,TRUE, F_IDO(FA_String_Contents),(uint32)((LOD -> pick_spec) ? LOD -> pick_spec + 1 : (STRPTR)("000000")),TAG_DONE);
            }
            else
            {
               F_Dispose(spec);
            }
         }
      }
      break;
       
      case FF_EVENT_BUTTON:
      {
         F_Do(Obj,F_IDM(FM_AdjustRGB_PickEnd));
         
         return FF_HandleEvent_Eat;
      }
      break;
   }
   return 0;
}
//+

///Adjust_Query
F_METHODM(uint32,Adjust_Query,FS_Adjust_Query)
{
   if (Msg -> Spec)
   {
      if (*Msg -> Spec == '#')
      {
         return TRUE;
      }
#ifdef COMPATIBILITY
      else if ((Msg -> Spec[0] == 'c' && Msg -> Spec[1] == ':'))
      {
         return TRUE;
      }
#endif
   }
   return FALSE;
}
//+
///Adjust_ToString
F_METHODM(uint32,Adjust_ToString,FS_Adjust_ToString)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (Msg -> Notify)
   {
      BOOL rgb=TRUE;
      FObject dst=NULL,int_dst=NULL,src=NULL;
      uint32 rc;
      STRPTR spec;
      FRGB ar_rgb = {0, 0, 0} ;

      /* map changes */

      switch (Msg -> UserData)
      {
         case NOTIFY_R: dst = LOD -> string_r; break;
         case NOTIFY_G: dst = LOD -> string_g; break;
         case NOTIFY_B: dst = LOD -> string_b; break;
         
         case NOTIFY_H: rgb = FALSE; dst = LOD -> string_h; break;
         case NOTIFY_S: rgb = FALSE; dst = LOD -> string_s; break;
         case NOTIFY_V: rgb = FALSE; dst = LOD -> string_v; break;
      
         case NOTIFY_STRING_R: int_dst = LOD -> r; src = LOD -> string_r; break;
         case NOTIFY_STRING_G: int_dst = LOD -> g; src = LOD -> string_g; break;
         case NOTIFY_STRING_B: int_dst = LOD -> b; src = LOD -> string_b; break;
      
         case NOTIFY_STRING_H: rgb = FALSE; int_dst = LOD -> h; src = LOD -> string_h; break;
         case NOTIFY_STRING_S: rgb = FALSE; int_dst = LOD -> s; src = LOD -> string_s; break;
         case NOTIFY_STRING_V: rgb = FALSE; int_dst = LOD -> v; src = LOD -> string_v; break;
      
         case NOTIFY_HTML:
         {
            uint32 val=0;
            
            rgb = TRUE;

            if (Msg -> Spec)
            {
               stch_l(Msg -> Spec,(int32 *) &val);
            }
            
            ar_rgb.r = (0x00FF0000 & val) >> 16;
            ar_rgb.g = (0x0000FF00 & val) >>  8;
            ar_rgb.b = (0x000000FF & val) >>  0;
         }
         break;
      }
      
      if (dst)
      {
         F_Set(dst,F_IDO(FA_String_Integer),(uint32) Msg -> Spec);
      }
      else if (int_dst)
      {
         F_Do(int_dst,FM_SetAs,FV_SetAs_Decimal,F_IDO(FA_Numeric_Value),Msg -> Spec);
         
         /* clamp string */
         
         F_Set(src,F_IDO(FA_String_Integer),F_Get(int_dst,F_IDO(FA_Numeric_Value)));
      }

      if (!rgb)
      {
         hsv_to_rgb(F_Get(LOD -> h,F_IDO(FA_Numeric_Value)),
                    F_Get(LOD -> s,F_IDO(FA_Numeric_Value)),
                    F_Get(LOD -> v,F_IDO(FA_Numeric_Value)),
                    &ar_rgb);
      }
      
      if (!rgb || Msg -> UserData == NOTIFY_HTML)
      {
         F_Do(LOD -> r,FM_Set,FA_NoNotify,TRUE,F_IDO(FA_Numeric_Value),ar_rgb.r,TAG_DONE);
         F_Do(LOD -> g,FM_Set,FA_NoNotify,TRUE,F_IDO(FA_Numeric_Value),ar_rgb.g,TAG_DONE);
         F_Do(LOD -> b,FM_Set,FA_NoNotify,TRUE,F_IDO(FA_Numeric_Value),ar_rgb.b,TAG_DONE);

         F_Set(LOD -> string_r,F_IDO(FA_String_Integer),ar_rgb.r);
         F_Set(LOD -> string_g,F_IDO(FA_String_Integer),ar_rgb.g);
         F_Set(LOD -> string_b,F_IDO(FA_String_Integer),ar_rgb.b);
      }
 
      if (rgb)
      {
         FHSV hsv;
         
         ar_rgb.r = F_Get(LOD -> r,F_IDO(FA_Numeric_Value));
         ar_rgb.g = F_Get(LOD -> g,F_IDO(FA_Numeric_Value));
         ar_rgb.b = F_Get(LOD -> b,F_IDO(FA_Numeric_Value));
 
         rgb_to_hsv(ar_rgb.r,ar_rgb.g,ar_rgb.b, &hsv);

         F_Do(LOD -> h,FM_Set,FA_NoNotify,TRUE,F_IDO(FA_Numeric_Value),hsv.h,TAG_DONE);
         F_Do(LOD -> s,FM_Set,FA_NoNotify,TRUE,F_IDO(FA_Numeric_Value),hsv.s,TAG_DONE);
         F_Do(LOD -> v,FM_Set,FA_NoNotify,TRUE,F_IDO(FA_Numeric_Value),hsv.v,TAG_DONE);

         F_Set(LOD -> string_h,F_IDO(FA_String_Integer),hsv.h);
         F_Set(LOD -> string_s,F_IDO(FA_String_Integer),hsv.s);
         F_Set(LOD -> string_v,F_IDO(FA_String_Integer),hsv.v);
      }

      if (Msg -> UserData != NOTIFY_HTML)
      {
         F_Do(LOD -> html,FM_SetAs,FV_SetAs_String,F_IDO(FA_String_Contents),"%02lx%02lx%02lx",ar_rgb.r,ar_rgb.g,ar_rgb.b);
      }

      spec = F_StrNew(NULL,"#%02lx%02lx%02lx",F_Get(LOD -> r,F_IDO(FA_Numeric_Value)),F_Get(LOD -> g,F_IDO(FA_Numeric_Value)),F_Get(LOD -> b,F_IDO(FA_Numeric_Value)));

      rc = F_SuperDo(Class,Obj,Method,spec,Msg -> Notify);

//      F_Log(0,"%s",rc);
 
      F_Dispose(spec);

      return rc;
   }
   return F_SUPERDO();
}
//+
///Adjust_ToObject
F_METHODM(void,Adjust_ToObject,FS_Adjust_ToObject)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FHSV hsv;
   uint32 val=0;

   if (Msg -> Spec && *Msg -> Spec == '#')
   {
      stch_l(Msg -> Spec + 1,(int32 *)(&val));

      rgb_to_hsv((0x00FF0000 & val) >> 16,
                 (0x0000FF00 & val) >> 8,
                 (0x000000FF & val),
                 &hsv);
   }
#ifdef COMPATIBILITY
   else if (Msg -> Spec && Msg -> Spec[0] == 'c' && Msg -> Spec[1] == ':')
   {
      stch_l(Msg -> Spec + 2,(int32 *)(&val));

      rgb_to_hsv((0x00FF0000 & val) >> 16,
                 (0x0000FF00 & val) >> 8,
                 (0x000000FF & val),
                 &hsv);
   }
#endif
   else
   {
      hsv.h = 0;
      hsv.s = 0;
      hsv.v = 0;
   }

   F_Do(LOD -> r, FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Numeric_Value),(0x00FF0000 & val) >> 16, TAG_DONE);
   F_Do(LOD -> g, FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Numeric_Value),(0x0000FF00 & val) >>  8, TAG_DONE);
   F_Do(LOD -> b, FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Numeric_Value),(0x000000FF & val),       TAG_DONE);
   
   F_Set(LOD -> string_r,F_IDO(FA_String_Integer),(0x00FF0000 & val) >> 16);
   F_Set(LOD -> string_g,F_IDO(FA_String_Integer),(0x0000FF00 & val) >>  8);
   F_Set(LOD -> string_b,F_IDO(FA_String_Integer),(0x000000FF & val) >>  0);
   
   F_Do(LOD -> h, FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Numeric_Value),hsv.h,TAG_DONE);
   F_Do(LOD -> s, FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Numeric_Value),hsv.s,TAG_DONE);
   F_Do(LOD -> v, FM_Set, FA_NoNotify,TRUE, F_IDO(FA_Numeric_Value),hsv.v,TAG_DONE);

   F_Set(LOD -> string_h,F_IDO(FA_String_Integer),hsv.h);
   F_Set(LOD -> string_s,F_IDO(FA_String_Integer),hsv.s);
   F_Set(LOD -> string_v,F_IDO(FA_String_Integer),hsv.v);
   
   F_Do(LOD -> html,FM_SetAs,FV_SetAs_String,F_IDO(FA_String_Contents),"%06lx",val);
}
//+

///Adjust_PickBegin
F_METHOD(void,Adjust_PickBegin)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   
   F_Do(LOD -> pick,FM_Set, FA_NoNotify,TRUE, FA_Selected,TRUE, TAG_DONE);
   
   F_Do(Obj,FM_ModifyHandler,FF_EVENT_MOTION | FF_EVENT_BUTTON,0);
}
//+
///Adjust_PickEnd
F_METHOD(void,Adjust_PickEnd)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_MOTION | FF_EVENT_BUTTON);
   
   F_Do(LOD -> pick,FM_Set, FA_NoNotify,TRUE, FA_Selected,FALSE, TAG_DONE);

   F_Do(Obj,F_IDM(FM_Adjust_ToString),(LOD -> pick_spec) ? (LOD -> pick_spec + 1) : (STRPTR) "000000",TRUE,NOTIFY_HTML);
   F_Dispose(LOD -> pick_spec); LOD -> pick_spec = NULL;
}
//+

