/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <libraries/feelin.h>

#include <proto/utility.h>
#include <proto/feelin.h>

#define       SysBase               FeelinBase -> SYS
#define       DOSBase               FeelinBase -> DOS
#define       LayersBase            FeelinBase -> Layers
#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FObject                         Family;
};

enum    {

        FM_Menu_Setup,
        FM_Menu_Cleanup

        };

/*
#define FC_Menu 'Menu.fc'
#define FS_CreateMenu   feelin_Menu_Create
#define FS_SelectedNode feelin_Menu_SelectedNode
#define MenuObject   F_NewObjA(FC_Menu,[TAG_IGNORE,NIL
#define Menu(t)      F_NewObjA(FC_Menu,[FA_Item_Title,t

ENUM  FM_Menu_Create       = FR_Menu_Mthd,
      FM_Menu_HandleEvent,
      FM_Menu_Open,
      FM_Menu_Close,

      FM_Menu_CreateRootData

ENUM  FA_Menu              = FR_Menu_Attr,
      FA_Menu_Action,
      FA_Menu_Active,
      FA_Menu_ActiveItem,
      FA_Menu_Selected,
      FA_Menu_TriggerActive,
      FA_Menu_TriggerSelected,
      FA_Menu_OpenedMenu,
      FA_Menu_Owner

CONST FA_ContextMenu = FA_Menu -> General attribute

OBJECT feelin_Menu_Create ; menuowner:PTR TO feelinObject,render:PTR TO feelinRender ; ENDOBJECT
OBJECT feelin_Menu_SelectedNode OF feelinNode ; object:PTR TO feelinObject ; ENDOBJECT

OBJECT feelinMenuRender OF feelinRender
   rootdata:PTR TO feelinMenuRootData  -> Shared
ENDOBJECT

OBJECT feelinMenuRootData
   rootmenu:PTR TO feelinMenu -> True RootMenu
   mainmenu:PTR TO feelinMenu -> Current Tree Menu (may be RootMenu)
   openmenu:PTR TO feelinMenu -> Currently opened menu
   menuport:PTR TO mp
   menuowner:PTR TO feelinObject
   textobject:PTR TO feelinObject
   layerinfo:PTR TO layer_info
   inputhandler:PTR TO is
   selected:feelinList
   selectedpool:PTR TO CHAR
ENDOBJECT

OBJECT feelinMenu OF feelinItem
   family:PTR TO feelinFamily
   active:PTR TO feelinItem

   rootdata:PTR TO feelinMenuRootData  -> Only RootMenu hold this structure
   menuarea:feelinRect
   menurender:PTR TO feelinMenuRender  -> Shared with children

   bitmap:PTR TO bitmap
   saved:PTR TO bitmap
   layer:PTR TO layer
ENDOBJECT

OBJECT feelinMenuHandleEvent
   key:INT,class:INT
   mousex:INT,mousey:INT
ENDOBJECT
*/

/***************************************************************************/
/*** Attributes, Methods, Values, Flags ************************************/
/***************************************************************************/

/***************************************************************************/
/*** Preferences ***********************************************************/
/***************************************************************************/

struct p_LocalObjectData
{
};
