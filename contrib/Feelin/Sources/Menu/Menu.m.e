#define FC_Item         'Item.fc'
#define ItemObject      F_NewObjA(FC_Item,[TAG_IGNORE,NIL
#define Item(t)         F_NewObjA(FC_Item,[FA_Item_Title,t
#define ItemSeparator   F_NewObjA(FC_Item,NIL)

CONST FR_Item        = $002000
CONST FR_Item_Mthd   = MTHD_BASE OR FR_Item,
      FR_Item_Attr   = ATTR_BASE OR FR_Item


#ifndef PUB
CONST FF_Item_Check         = %0000000000000001,
      FF_Item_Toggle        = %0000000000000010,
      FF_Item_Active        = %0000000000000100,
      FF_Item_Checked       = %0000000000001000,
      FF_Item_Enabled       = %0000000000010000,
      FF_Item_Selected      = %0000000000100000,
      FF_Item_Sensitive     = %0000000001000000,
      FF_Item_CommandString = %0000000010000000

OBJECT feelinItem OF feelinObject
   flags
   render:PTR TO feelinMenuRender

   box:feelinBox
   inner:feelinInner
   minmax:feelinMinMax

   pens

   title:PTR TO CHAR
   short
ENDOBJECT
#endif
