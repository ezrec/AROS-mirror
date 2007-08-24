/*

$VER: 04.00 (2005/08/10)
 
   Portability update.
   
   Add  FM_Display_PixelRead  and  FM_Display_PixelWrite   methods.   These
   methods automatically use CyberGfx whenever possible.
   
   FA_Diplay_Depth was not correct, it was 8 for HiColors screens.
   
   Color scheme format is  now  XML.  Color  scheme  attributes  have  been
   extended to support 'contrast' and 'saturation'.
   
   New colors type, "s:", "p:" and "c:" are considered deprecated.
   
    NEW COLOR TYPES
    ---------------
    
    "s:", "p:" and "c:" and now deprecated  and  replaced  with  CSS  style
    ones.
    
        The color  scheme  color  "s:3"  is  now  defined  by  "fill".  The
        following   color   scheme  references  are  available  :  "shine",
        "halfshine", "fill", "halfshadow",  "shadow",  "halfdark",  "dark",
        "text" and "highlight".
        
        The colormap pen entry "p:1" is now defined by  "1".  The  possible
        values range from -128 to 127.
        
        The RGB value "c:FF00FF" is now defined by "#FF00FF".
   
   Metaclass support
   
   ! // !
   
   <display name="$display_name">
      <modeid>
      <width>640</width>
      <height>480</height>
      <depth>32</depth>
   
   </display>
 
$VER: 03.02 (2005/03/14)
 
   The FM_Display_DecodeColor method is now public. It should  be  used  in
   HiColors environements to use direct colors instead of allocations.
   
$VER: 03.00 (2004/07/18)

   display.server has been removed. The design was  terrible,  and  it  was
   useless  anyway,  because  currently  no screen can be created. I'll see
   later for public screen management. Anyway the server must be  separated
   from the FC_Display (e.i. knowing nothing about internal management).

   FC_Display  handles  all  FC_DisplayContext  methods   :   FM_CreatePen,
   FM_DeletePen,  FM_CreateScheme  and  FM_DeleteScheme.  The  FM_CreatePen
   method has been modified a little and requires a scheme reference.  This
   scheme is used as a reference to create the pen (currently only used for
   "s:"). Displays are still shared objects but I use  different  mecanisms
   to create and delete them. **FC_DisplayContext is now obsolete**

   To create a Display the FM_Display_Create method must be invoked on  the
   class using F_ClassDoA(). If a suitable display is found, the display is
   shared. Otherwise, a new display is created (not functionnal yet).

   To delete a Display the FM_Display_Delete method must be invoked on  the
   class  using  F_ClassDoA(). If the display is no longer used, the object
   is disposed.

   Only application should create and delete displays.

   **

   Because Picture handling is quite useless AddPicture() and  RemPicture()
   have been removed. (see TODO)

   **

   AddPen() and RemPen() have been renamed as AddColor() and RemColor()

_____________________________________________________________________________

   Because FC_Display knows exactly about dispay context,  picture  mapping
   (like  pen  mapping)  should  be  handled  by  the class itself (and not
   FC_Picture).

   FPen  and  FPalette  need  further  work  and  concept  changes  to  use
   True-Colors.  The  problem  is  that  when you work with True-Colors you
   should  not  allocate  colors  but  use  them  directly.  AddPen()   and
   AddPalette()  should only be used with Low-Colors displays... This needs
   some work.


*/

#include "Project.h"

struct Library                     *CyberGfxBase;
struct ClassUserData               *CUD;

///METHODS
F_METHOD_PROTO(void,D_New);
F_METHOD_PROTO(void,D_Dispose);
F_METHOD_PROTO(void,D_Get);
F_METHOD_PROTO(void,D_Set);
F_METHOD_PROTO(void,D_Find);
F_METHOD_PROTO(void,D_Create);
F_METHOD_PROTO(void,D_Delete);
F_METHOD_PROTO(void,D_AddMember);
F_METHOD_PROTO(void,D_RemMember);
F_METHOD_PROTO(void,D_AddColor);
F_METHOD_PROTO(void,D_RemColor);
F_METHOD_PROTO(void,D_AddPalette);
F_METHOD_PROTO(void,D_RemPalette);
F_METHOD_PROTO(void,D_DecodeColor);
F_METHOD_PROTO(void,D_CreateColor);
F_METHOD_PROTO(void,D_CreateColorScheme);
F_METHOD_PROTO(void,D_PixelRead);
F_METHOD_PROTO(void,D_PixelWrite);
//+

///Class_New
F_METHOD_NEW(Class_New)
{
   CUD = F_LOD(Class,Obj);
 
   CyberGfxBase = OpenLibrary(CYBERGFXNAME,CYBERGFX_INCLUDE_VERSION);
 
   CUD -> ColorPool = F_CreatePool(sizeof (struct in_FeelinColor),

      FA_Pool_Name,"Display.Color",

      TAG_DONE);

   CUD -> PalettePool = F_CreatePool(1024,

      FA_Pool_Items,  4,
      FA_Pool_Name,   "Display.Palette",

      TAG_DONE);

   CUD -> Semaphore = F_NewObj(FC_Object,
         
      //FA_Semaphore_Name,"Display.Semaphore",
            
      End;
             
   CUD -> XMLScheme = XMLDocumentObject,
   
      End;
      
   if (CUD -> ColorPool && CUD -> PalettePool && CUD -> Semaphore && CUD -> XMLScheme)
   {
      STATIC F_RESOLVES_ARRAY =
      {
         F_RESOLVES_ADD("FA_Document_Source"),
         F_RESOLVES_ADD("FA_Document_SourceType"),
         F_RESOLVES_ADD("FM_Document_AddIDs"),
         F_RESOLVES_ADD("FM_Document_Resolve"),
         F_RESOLVES_ADD("FA_XMLDocument_Markups"),
         
         F_ARRAY_END
      };
      
      F_DynamicResolveTable(F_RESOLVES);
 
      CUD -> XMLIDs = F_RESOLVES;
 
      return (FObject) F_SUPERDO();
   }
   return NULL;
}
//+
///Class_Dispose
F_METHOD_DISPOSE(Class_Dispose)
{
   CUD = F_LOD(Class,Obj);
 
   CyberGfxBase = NULL;
   
   F_DisposeObj(CUD -> XMLScheme); CUD -> XMLScheme = NULL;
   F_DisposeObj(CUD -> Semaphore); CUD -> Semaphore = NULL;
   F_DeletePool(CUD -> PalettePool); CUD -> PalettePool = NULL;
   F_DeletePool(CUD -> ColorPool); CUD -> ColorPool = NULL;
   
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
         STATIC F_ATTRIBUTES_ARRAY =
         {
            F_ATTRIBUTES_ADD("Background",  FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("ColorMap",    FV_TYPE_POINTER),
            F_ATTRIBUTES_ADD("Depth",       FV_TYPE_INTEGER),
            F_ATTRIBUTES_ADD("BitMap",      FV_TYPE_POINTER),
            F_ATTRIBUTES_ADD("DrawInfo",    FV_TYPE_POINTER),
            F_ATTRIBUTES_ADD("Height",      FV_TYPE_INTEGER),
            F_ATTRIBUTES_ADD("Name",        FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("Screen",      FV_TYPE_POINTER),
            F_ATTRIBUTES_ADD("ScreenMode",  FV_TYPE_INTEGER),
            F_ATTRIBUTES_ADD("Spec",        FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("Title",       FV_TYPE_STRING),
            F_ATTRIBUTES_ADD("Width",       FV_TYPE_INTEGER),

            F_ARRAY_END
         };

         STATIC F_METHODS_ARRAY =
         {
            F_METHODS_ADD_STATIC(D_New,        FM_New),
            F_METHODS_ADD_STATIC(D_Dispose,    FM_Dispose),
            F_METHODS_ADD_STATIC(D_Get,        FM_Get),
            F_METHODS_ADD_STATIC(D_AddMember,  FM_AddMember),
            F_METHODS_ADD_STATIC(D_RemMember,  FM_RemMember),

            F_METHODS_ADD(D_Find,                "Find"),
            F_METHODS_ADD(D_Create,              "Create"),
            F_METHODS_ADD(D_Delete,              "Delete"),
            
            F_METHODS_ADD_BOTH(D_AddColor,            "AddColor",          FM_AddColor),
            F_METHODS_ADD_BOTH(D_RemColor,            "RemColor",          FM_RemColor),
            F_METHODS_ADD_BOTH(D_AddPalette,          "AddPalette",        FM_AddPalette),
            F_METHODS_ADD_BOTH(D_RemPalette,          "RemPalette",        FM_RemPalette),
            F_METHODS_ADD(D_DecodeColor,              "DecodeColor"),
            F_METHODS_ADD_BOTH(D_CreateColor,         "CreateColor",       FM_CreateColor),
            F_METHODS_ADD_BOTH(D_CreateColorScheme,   "CreateColorScheme", FM_CreateColorScheme),
            
            F_METHODS_ADD(D_PixelRead,    "PixelRead"),
            F_METHODS_ADD(D_PixelWrite,   "PixelWrite"),
            
            F_ARRAY_END
         };

         STATIC F_TAGS_ARRAY =
         {
            F_TAGS_ADD_SUPER(Object),
            F_TAGS_ADD_LOD,
            F_TAGS_ADD_METHODS,
            F_TAGS_ADD_ATTRIBUTES,

            F_ARRAY_END
         };

         return F_TAGS;
      }
//+
   }
   return NULL;
}
