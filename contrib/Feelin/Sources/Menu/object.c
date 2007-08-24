#include "Private.h"

/*** Methods ***************************************************************/

///Menu_New
F_METHOD(uint32, Menu_New)
{
    struct TagItem *Tags,item,*rtag;

    if (FamilyObject, FA_Family_Owner,Obj, TAG_MORE,Msg)
    {
        return F_SUPERDO();
    }

    Tags = Msg;

    while (rtag = F_DynamicNTI(&Tags,&item,Class))
    {
        if (item.ti_Tag == FA_Child)
        {
            F_DisposeObj((FObject)(item.ti_Data)); rtag -> ti_Tag = TAG_IGNORE; rtag -> ti_Data = NULL;
        }
    }

   return NULL;
}
//+
///Menu_Dispose
F_METHOD(void, Menu_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_DisposeObj(LOD -> Family); LOD -> Family = NULL;

    F_SUPERDO();
}
//+

/*

    FM_Menu_CreateRData  appelé  par  l'application  sur  le  menu   racine
    (FA_Application_Menu) lors de FM_Application_Setup. FM_Menu_DeleteRData
    appelé   par   l'application   sur   le    menu    racine    lors    de
    FM_Application_Cleanup.

*/

struct LocalObjectData
{
    FObject                         family;
    
    FMenuRData                     *rdata;
    FMenuItemPublic                 item_public;
    
    // preferences
    
    STRPTR                          p_menu_frame
    STRPTR                          p_item_frame
};

#define DEF_MENU_FRAME                          "<frame id='33' padding-right='1' padding-bottom='1' />"

/* Créer une classes FC_RootMenu sous classe  de  FC_Menu.  Pas  d'item  de
préférence  dans  la  structure  de  FC_Menu puisque tous partagent ceux de
FMenuRData.

*/
 
struct  FS_Menu_CreateRData                     { FObject Application; FObject Display; };

typedef struct FeelinMenuRootData
{
    FObject                         Application;
    FObject                         Display;        // valid during FM_Menu_Setup & FM_Menu_Cleanup

    FObject                         RootMenu;
 
    FObject                         MenuFrame;
    FObject                         ItemFrame;
    FObject                         IconsBack;
}
FMenuRData

/*

    La structure FMenuRData est allouée lors de la méthode FM_Connect si le
    parent  est en fait l'application. La structure est disposée lors de la
    méthode FM_Disconnect.

*/

///Menu_Connect
F_METHODM(uint32, Menu_Connect, FS_Connect)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
 
    LOD->parent = Msg->Parent;
    
    if (F_Get(LOD->Parent, FA_Application) == (uint32) LOD->Parent)
    {
        LOD->rdata = F_New(sizeof (FMenuRData));
        
        if (LOD->rdata)
        {
            LOD->rdata->Application = LOD->Parent;
            LOD->rdata->RootMenu = Obj;
            
            return TRUE;
        }
        return FALSE;
    }

    return TRUE;
}
//+
///Menu_Disconnect
F_METHOD(uint32, Menu_Disconnect)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (LOD->rdata && LOD->rdata->RootMenu == Obj)
    {
        F_Dispose(LOD->rdata);
    }

    LOD->parent = NULL;
    LOD->rdata = NULL;
    
    return TRUE;
}
//+

///Menu_CreateRData
F_METHODM(FMenuRData *, Menu_CreateRData, FS_Menu_CreateRData)
{
    uint32 data;
 
    LOD->rdata = F_New(sizeof (FMenuRData));

    if (LOD->rdata == NULL)
    {
        return NULL;
    }
 
    LOD->rdata->Application = Msg->Application;
    LOD->rdata->Display = Msg->Display;
    LOD->rdata->RootMenu = Obj;
        
    LOD->rdata->MenuFrame = FrameObject,
        
        FA_Frame, LOD->p_menu_frame,
        FA_Back, LOD->p_menu_back,
            
        End;
        
    if (LOD->rdata->MenuFrame == NULL)
    {
        return NULL;
    }

    LOD->rdata->ItemFrame = FrameObject,

        FA_Frame, LOD->p_item_frame,
        FA_Back, LOD->p_item_back,

        End;

    if (LOD->rdata->ItemFrame == NULL)
    {
        return NULL;
    }

    data = F_Do(LOD->rdata->Application, FM_Application_Resolve, LOD->p
    
    return LOD->rdata;
}
//+
 
/*

    Pour savoir qui est le menu  racine  il  suffit  d'utiliser  l'attribut
    FA_Application_Menu.
    
    Ouverture d'un menu avec  FM_Menu_Open  qui  apelle  FM_Menu_Setup  qui
    appelle   FM_Menu_RDObtain  (root  data  obtain)  sur  le  menu  racine
    (FA_Application_Menu).
    
    Fermeture avec FM_Menu_Close qui  apelle  FM_Menu_Cleanup  qui  appelle
    FM_Menu_RDReleasse   (root   data   release)   sur   le   menu   racine
    (FA_Application_Menu).
    

    Ainsi, seul l'objet racine crée la structure FMenuRData.  Tout  ce  qui
    concerne  l'affichage  doit  être  contenu dans la structure et partagé
    e.g. un seul objet FC_Frame pour tout le monde, une seule palette.

*/


/*

   FM_Menu_Setup est envoyé par l'application au menu  racine.  Il  est  le
   seul  à  recevoir  cette méthode. Lors de FM_Menu_Create, le menu racine
   crée l'objet FC_MenuRender qui sera partagé par TOUS les  enfants.  Puis
   il invoke la méthode FM_MenuItem_Setup sur ses enfants.

      struct FS_Menu_Setup
      {
         FObject                 Application;

      };

*/

///Menu_Setup
F_METHODM(uint32,Menu_Setup,FS_Menu_Setup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
}
//+
