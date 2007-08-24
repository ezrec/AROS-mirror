#include <libraries/feelin.h>

#include <proto/exec.h>
#include <proto/feelin.h>

struct FeelinBase *FeelinBase;

/// Header
struct LocalObjectData
{
   FAreaPublic                     *AreaPublic;
   FSignalHandler                   shnode;
   uint32                            index;
};

/* Some strings to display */

static const char *Cheuveux[] =
{
   "Tes cheuveux contiennent tout un rêve.",
   "Plein de voilures et de matures.",
   "Ils contiennent de grandes mers",
   "dont les moussons me portent",
   "vers le charmant climat.",
   "Où l'espace est plus bleue",
   "et plus profond.",
   "Où l'atmosphère est parfumée par les fruits,",
   "par les feuilles",
   "et par la peau humaine.",
   "Dans l'océan de ta chevelure",
   "j'entrevois un phare",
   "fourmillant de chants mélancoliques,",
   "d'hommes vigoureux de toute nation",
   "et de navrires de toutes formes.",
   "Découpant leur architecture",
   "fine et compliquée",
   "sur un ciel immense",
   "ou se prélasse l'éternelle chaleur.",
   "Laisse-moi respirer longtemps",
   "l'odeur de tes cheveux",
   "et y plonger tout mon visage.",
   "Comme un homme althéré",
   "dans l'eau d'une source",
   "et les agiter avec ma main,",
   "comme un mouchoir odorant,",
   "pour secouer les souvenirs dans l'air.",
   "Dans les caresses de ta cheuveulure,",
   "je retrouve les langueurs des longues heures",
   "passées sur un divan",
   "dans la chambre d'un beau navire.",
   "Bbercé par les roulis imperceptibles du port,",
   "entre les pots de fleurs",
   "et les gargoulettes rafraichissantes.",
   "Dans l'ardent foyer de ta cheuveulure,",
   "je respire l'odeur du tabac,",
   "mélée à l'opimum et au sucre.",
   "Dans la nuit de ta chevelure",
   "je vois resplendir",
   "l'infini de l'azur tropical.",
   "Sur les rivages duvetés de ta chevelure",
   "je m'ennivre des odeurs combinées",
   "du gondron, du musk et de l'huile de coco.",
   "Laisse moi mordre longtemps",
   "tes tresses lourdes et noires.",
   "Quand je mordille tes cheuveux",
   "élastiques et rebelles,",
   "il me semble que je mange des souvenirs.",
   NULL
};

//+

/***************************************************************************/
/* Here is the beginning of our new class...                               */
/***************************************************************************/

enum  {  // methods

      FM_Trigger

      };

enum  {  // attributes

      FA_Delay

      };

///mNew
F_METHOD(uint32,mNew)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_AREA_SAVE_PUBLIC;

    LOD -> shnode.Object      = Obj;
    LOD -> shnode.Method      = F_IDM(FM_Trigger);
    LOD -> shnode.Flags       = FF_SignalHandler_Timer;
    LOD -> shnode.fsh_Secs    = 0;
    LOD -> shnode.fsh_Micros  = 500000;

    LOD -> index = 0;

    return F_SUPERDO();
}
//+
///mSet
F_METHOD(uint32,mSet)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Delay:
        {
            LOD -> shnode.fsh_Micros = item.ti_Data;
        }
        break;
    }

    return F_SUPERDO();
}
//+
///mSetup
F_METHOD(uint32,mSetup)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (F_SUPERDO())
    {
       F_Do(_app,FM_Application_AddSignalHandler,&LOD -> shnode);

       return TRUE;
    }
    return FALSE;
}
//+
///mCleanup
F_METHOD(uint32,mCleanup)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   F_Do(_app,FM_Application_RemSignalHandler,&LOD -> shnode);

   return F_SUPERDO();
}
//+
///mTrigger
F_METHOD(uint32,mTrigger)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Set(Obj,FA_Text,(uint32)(Cheuveux[LOD -> index]));

    if (!Cheuveux[++LOD->index])
    {
        LOD -> index = 0;
    }

    return TRUE;
}
//+

/*** Attibutes and Methods *************************************************/

STATIC F_ATTRIBUTES_ARRAY =
{
    F_ATTRIBUTES_ADD("Delay", FV_TYPE_INTEGER),

    F_ARRAY_END
};

STATIC F_METHODS_ARRAY =
{
   F_METHODS_ADD(mTrigger, "Trigger"),

   F_METHODS_ADD_STATIC(mNew, FM_New),
   F_METHODS_ADD_STATIC(mSet, FM_Set),
   F_METHODS_ADD_STATIC(mSetup, FM_Setup),
   F_METHODS_ADD_STATIC(mCleanup, FM_Cleanup),

   F_ARRAY_END
};

/*** main ******************************************************************/

int32 main()
{
    FObject app,win,spd,my;

    if (F_FEELIN_OPEN)
    {
        FClass *cc = F_CreateClass(NULL,
          
            FA_Class_Super,      FC_Text,
            FA_Class_LODSize,    sizeof (struct LocalObjectData),
            FA_Class_Attributes, F_ATTRIBUTES,
            FA_Class_Methods,    F_METHODS,
              
            TAG_DONE);
              
        if (cc)
        {
            app = ApplicationObject,
                FA_Application_Title,       "demo_SignalHandler",
                FA_Application_Version,     "$VER: demo_SignalHandler 1.0 (2004.05.19)",
                FA_Application_Copyright,   "©2004, Olivier LAVIALE <gofromiel@gofromiel.com>",
                FA_Application_Author,      "Olivier LAVIALE",
                FA_Application_Description, "Demonstrate the use of custom classes.",
                FA_Application_Base,        "DEMO_SHA1",

                Child, win = WindowObject,
                    FA_ID,            MAKE_ID('M','A','I','N'),
                    FA_Window_Title,  "SignalHandler Class",
                    FA_Window_Open,   TRUE,
                    
                    Child, VGroup,
                        Child, TextObject,
                            FA_Frame,         "$text-frame",
                            FA_Back,          "$text-back",
                            FA_ColorScheme,   "$text-scheme",
                            FA_Text,          "<align=center>Demonstration of a class that reacts on<br>events (here: timer signals) automatically.",
                            FA_SetMax,        FV_SetHeight,
                            FA_ChainToCycle,  FALSE,
                        End,

                        Child, my = F_NewObj(cc -> Name,
                            FA_Frame,         "$text-frame",
                            FA_Back,          "$text-back",
                            FA_ColorScheme,   "$text-scheme",
                            FA_Font,          "$font-big",
                            FA_ChainToCycle,  FALSE,
                            FA_Text_PreParse, "<align=center>",
                            FA_Text_VCenter,  TRUE,
                        End,

                        Child, spd = F_MakeObj(FV_MakeObj_Slider, TRUE,5000,1000000,50000,
                            FA_ID,               MAKE_ID('S','P','E','D'),
                            FA_ContextHelp,      "Adjust the number of micro seconds\nto wait between each string.",
                            FA_SetMax,           FV_SetHeight,
                           "FA_Numeric_Step",    10000,
                           "FA_Numeric_Format",  "Micros : %ld",
                        End,
                    End,
                End,
            End;

            if (app)
            {
                F_Do(win,FM_Notify,FA_Window_CloseRequest,TRUE,app,FM_Application_Shutdown,0);
                F_Do(spd,FM_Notify,"FA_Numeric_Value",FV_Notify_Always,my,FM_Set,2,"Delay",FV_Notify_Value);

                F_Do(app,FM_Application_Run);
                F_DisposeObj(app);
            }
            F_DeleteClass(cc);
        }
        
        F_FEELIN_CLOSE;
    }
    return 0;
}
