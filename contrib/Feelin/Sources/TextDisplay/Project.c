/*

$VER: 05.00 (2005/08/10)
 
    Portability update.
   
    Metaclass support.
   
    Copy <image> markup as is to use it as a spec for FC_ImageDisplay.
 
$VER: 04.02 (2005/04/24)
 
    HTML parsing has been completely rewritten.

    Added the attribute 'compact' to <hr>, setting this attribute to 'true'
    disable spacings before and after <hr>.

    Any color spec decodable by  FM_Display_DecodeColor  can  be  used  for
    <pens> pens, as well as <font> ones.
 
$VER: 04.01 (2005/02/01)
 
    Remove a stupid bug in FM_Draw : The shortcut line wasn't drawn because
    'adjust.font' was NULL if the text did not requires adjustement.
 
$VER: 04.00 (2004/07/11)

    The class has been completely rewritten. Text  is  now  formated  using
    HTML markups.

    Text  can  be  formated  like  HTML  pages.  Yet,  few   commands   are
    implemented.  <i>, <b>, <u> can be used to change the style of the font
    to, respectively, italic, bold and undelined. Text can  be  aligned  to
    the  left,  to  the  right, centered or even justified with the command
    <align>. The command <font> can be used  to  change  the  font's  face,
    font's  colour  and  font's size. The command <br> can be used to break
    lines (as well as the common \n). An customizable horizontal  rule  can
    be  used  to  divided parts of a text with the command <hr>. Images can
    also be incorporated within the text with the command <img>.

    In addition to standard HTML commands, some command have been added  to
    allow  further  customization  of  the  text aspect. The command <pens>
    allows management of 4 additionnal pens used to produce effects such as
    emboss,  ghost, glow... The command <spacing> can be used to modify the
    spacing (in pixels) between two or several lines.

*/

#include "Project.h"

struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,TD_New);
F_METHOD_PROTO(void,TD_Get);
F_METHOD_PROTO(void,TD_Set);

F_METHOD_PROTO(void,TD_Setup);
F_METHOD_PROTO(void,TD_Cleanup);
F_METHOD_PROTO(void,TD_Draw);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
 
   if ((CUD -> Pool = F_CreatePool(2048,

      FA_Pool_Items,   1,
      FA_Pool_Name,   (uint32) "TextDisplay.Chunks",

   TAG_DONE)))
   {
      return (FObject) F_SUPERDO();
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   F_DeletePool(CUD -> Pool); CUD -> Pool = NULL;
   
   F_SUPERDO();
}
//+

F_QUERY()
{
   switch (Which)
   {
///Meta
      case FV_Query_MetaClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(Class_New,      FM_New),
            F_METHODS_ADD_STATIC(Class_Dispose,  FM_Dispose),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Class),
            F_TAGS_ADD(LODSize, sizeof (struct ClassUserData)),
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
///Class
      case FV_Query_ClassTags:
      {
         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC (TD_New,       FM_New),
            F_METHODS_ADD_STATIC (TD_Get,       FM_Get),
            F_METHODS_ADD_STATIC (TD_Set,       FM_Set),

            F_METHODS_ADD_BOTH   (TD_Setup,     "Setup",    FM_TextDisplay_Setup),
            F_METHODS_ADD_BOTH   (TD_Cleanup,   "Cleanup",  FM_TextDisplay_Cleanup),
            F_METHODS_ADD_BOTH   (TD_Draw,      "Draw",     FM_TextDisplay_Draw),

            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Object),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
   }
   return NULL;
};
