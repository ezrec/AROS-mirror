#include "Private.h"
#include "parse.h"
#include "context.h"

///td_context_new
FTDContext * td_context_new(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   FTDContext *context = F_New(sizeof (FTDContext));

   if (context)
   {
      context -> line          = NULL;
      context -> align         = FV_TD_LINE_ALIGN_LEFT;
      context -> spacing       = 1;
      context -> style         = FS_NORMAL;
      context -> mode          = JAM1;
      context -> shortcut      = 0;
      context -> stop_shortcut = (0 == (FF_TD_SHORTCUT & LOD -> Flags));
      context -> stop_engine   = FALSE;
   }
   else
   {
      F_Log(0,"Unable to create FTDContext, couldn't parse lines");
   }

   return context;
}
//+
///td_context_dispose
void td_context_dispose(FTDContext *Context)
{
   if (Context)
   {
      FTDContextContainer *container;
      FTDColor *color;

      while ((container = (FTDContextContainer *) Context -> ContainerList.Tail) != NULL)
      {
         F_LinkRemove(&Context -> ContainerList,(FNode *) container);

         if (container -> Stolen == FALSE)
         {
            F_Dispose(container -> Data);
         }

         F_Dispose(container);
      }
   
      while ((color = (FTDColor *)(Context -> ColorsList.Tail)) != NULL)
      {
         F_LinkRemove(&Context -> ColorsList,(FNode *) color);
         
         if (color -> Type == FV_TD_COLOR_SPEC)
         {
            F_Dispose(color -> Spec);
         }
         F_Dispose(color);
      }

      F_Dispose(Context);
   }
}
//+
///td_context_push
APTR td_context_push(FTDContext *Context,uint32 Type)
{
   FTDContextContainer *container;

   if ((container = F_NewP(CUD -> Pool,sizeof (FTDContextContainer))) != NULL)
   {
      container -> Stolen = FALSE;

      switch (Type)
      {
         case FV_TD_CONTEXT_FONT:
         {
            container -> Type = FV_TD_CONTEXT_FONT;

            if ((container -> Data = F_NewP(CUD -> Pool,sizeof (FTDContextFont))) != NULL)
            {
               Context -> container_font = container;
            }
         }
         break;

         case FV_TD_CONTEXT_PENS:
         {
            container -> Type = FV_TD_CONTEXT_PENS;

            if ((container -> Data = F_NewP(CUD -> Pool,sizeof (FTDContextPens))) != NULL)
            {
               Context -> container_pens = container;
            }
         }
         break;
      }

//      F_Log(0,"push: container 0x%08lx, type %ld, data 0x%08lx",container,container -> Type,container -> Data);

      if (container -> Data)
      {
         F_LinkTail(&Context -> ContainerList,(FNode *) container);

         return container -> Data;
      }
      else
      {
         F_Dispose(container);
      }
   }

   return NULL;
}
//+
///td_context_pop
FTDContextContainer * td_context_pop(FTDContext *Context,uint32 Type)
{
   FTDContextContainer *container;

   for (container = (FTDContextContainer *) Context -> ContainerList.Tail ; container ; container = container -> Prev)
   {

      /* search 'Type' context */

      if (container -> Type == Type)
      {
         FTDContextContainer *previous = container -> Prev;

         switch (Type)
         {
            case FV_TD_CONTEXT_FONT:   Context -> container_font = NULL; break;
            case FV_TD_CONTEXT_PENS:   Context -> container_pens = NULL; break;
         }

         F_LinkRemove(&Context -> ContainerList,(FNode *) container);

         if (!container -> Stolen)
         {
            F_Dispose(container -> Data);
         }

         F_Dispose(container);

         /* Search for a previous context  to  restore.  If  a  context  is
         found, the corresponding FTDContext member is set */

         for (container = previous ; container ; container = container -> Prev)
         {
            if (container -> Type == Type)
            {
               switch (Type)
               {
                  case FV_TD_CONTEXT_FONT: Context -> container_font = container; break;
                  case FV_TD_CONTEXT_PENS: Context -> container_pens = container; break;
               }

               break;
            }
         }

         break;
      }
   }

   return container;
}
//+
///td_context_add_color
FTDColor * td_context_add_color(FTDContext *Context,uint32 Spec,uint32 Type)
{
   if (Type)
   {
      FTDColor *color = F_NewP(CUD -> Pool,sizeof (FTDColor));

      if (color)
      {
         color -> Spec = (STRPTR) Spec;

         switch (Type)
         {
            case FV_TD_RESULT_TYPE_DEC:    color -> Type = FV_TD_COLOR_PEN;  break;
            case FV_TD_RESULT_TYPE_HEX:    color -> Type = FV_TD_COLOR_RGB;  break;
            case FV_TD_RESULT_TYPE_STRING: color -> Type = FV_TD_COLOR_SPEC; break;
         }

         F_LinkTail(&Context -> ColorsList,(FNode *) color);
         
         return color;
      }
   }
   return NULL;
}
//+

