#include "Private.h"

/*** Methods ***************************************************************/

///App_Setup
F_METHOD(int32,App_Setup)
{
    struct LocalObjectData    *LOD = F_LOD(Class,Obj);
    /// KeyStringTable
    STATIC STRPTR KeyStringTable[] =
    {
        "return",
        "upstroke return",
        "-repeat up",
        "-repeat down",
        "-repeat shift up",
        "-repeat shift down",
        "alt up",
        "alt down",
        "-repeat left",
        "-repeat right",
        "-repeat shift left",
        "-repeat shift right",
        "alt left",
        "alt right",
        "-repeat backspace",
        "-repeat del",
        "alt backspace",
        "alt del",
        "shift backspace",
        "shift del",
        "-repeat tab",
        "-repeat shift tab",
        "control return",
        "esc",
        
         NULL
    };
    //+
    uint32 i;

    /*** Keys ***/

    for (i = 1 ; i < FV_KEY_COUNT ; i++)
    {
        ParseIX(KeyStringTable[i - 1],&LOD -> Keys[i - 1]);
    }

    /*** Display ***/

    LOD -> DisplayClass = F_OpenClass(FC_Display);

    if (LOD -> DisplayClass)
    {
        LOD -> Display = (FObject) F_ClassDo(LOD -> DisplayClass,NULL,F_DynamicFindID("FM_Display_Create"),NULL);
        
        if (LOD -> Display)
        {
            STRPTR scheme = (STRPTR) F_Do(Obj,FM_Application_Resolve,LOD -> p_Scheme,NULL);

            LOD -> Scheme = (FPalette *) F_Do(LOD -> Display,FM_CreateColorScheme,scheme,NULL);
            
            if (LOD -> Scheme)
            {
                scheme = (STRPTR) F_Do(Obj,FM_Application_Resolve,LOD -> p_DisabledScheme,DEF_DISABLEDSCHEME);

                LOD -> DisabledScheme = (FPalette *) F_Do(LOD -> Display,FM_CreateColorScheme,scheme,LOD -> Scheme);
                
                if (LOD -> DisabledScheme)
                {
                    LOD -> Flags |= FF_Application_Setup;

                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
//+
///App_Cleanup
F_METHOD(void,App_Cleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   LOD -> Flags &= ~FF_Application_Setup;

   if (LOD -> DisplayClass)
   {
      if (LOD -> Display)
      {
         if (LOD -> DisabledScheme)
         {
            F_Do(LOD -> Display,FM_RemPalette,LOD -> DisabledScheme); LOD -> DisabledScheme = NULL;

            if (LOD -> Scheme)
            {
               F_Do(LOD -> Display,FM_RemPalette,LOD -> Scheme); LOD -> Scheme = NULL;
            }
         }

         F_ClassDo(LOD -> DisplayClass,NULL,F_DynamicFindID("FM_Display_Delete"),LOD -> Display); LOD -> Display = NULL;
      }

      F_CloseClass(LOD -> DisplayClass); LOD -> DisplayClass = NULL;
   }
}
//+
