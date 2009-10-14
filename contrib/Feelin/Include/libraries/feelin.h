#ifndef FEELIN_H
#define FEELIN_H

/*
**    feelin.h
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

$VER: 10.00 (2005/08/08)

    FV_TYPE_BOOLEAN, FV_TYPE_INTEGER... moved to <feelin/types.h>.

    'PutChar' member removed from struct FeelinBase.

    RENAMINGS
    ---------------------------------------------------------------------

        FAreaData                               FAreaPublic
        FFrameData                              FFramePublic
        FV_VERSION                              FV_FEELIN_VERSION
        FV_REVISION                             FV_FEELIN_REVISION
        FV_BOOLEAN                              FV_TYPE_BOOLEAN
        FV_INTEGER                              FV_TYPE_INTEGER
        FV_POINTER                              FV_TYPE_POINTER
        FV_STRING                               FV_TYPE_STRING
        FV_OBJECT                               FV_TYPE_OBJECT
        F_OPEN_FEELIN                           F_FEELIN_OPEN
        F_CLOSE_FEELIN                          F_FEELIN_CLOSE
        FA_AreaData                             FA_Area_PublicData
        FA_FrameData                            FA_Frame_PublicData
        _mx                                     _ix ('i' like 'inner')
        _my                                     _iy
        _mx2                                    _ix2
        _my2                                    _ix2
        _mw                                     _iw
        _mh                                     _ih

    NEW MACROS
    --------------------------------------------------------------------

        F_AREA_SAVE_PUBLIC

            Get the FA_AreaData attribute and save  it  in  the  'AreaData'
            member of the LOD variable.

        F_SAVE_BASE

            During F_QUERY(), the macro saves the  'Feelin'  variable  into
            the FeelinBase one.

        F_METHODS_ARRAY, F_METHODS, F_METHODS_ADD, F_METHODS_ADD_STATIC,
        F_METHODS_ADD_BOTH.

        F_ATTRIBUTES_ARRAY, F_ATTRIBUTES, F_ATTRIBUTES_ADD,
        F_ATTRIBUTES_ADD_WITH_VALUES, F_ATTRIBUTES_ADD_STATIC,
        F_ATTRIBUTES_ADD_STATIC_WITH_VALUES, F_ATTRIBUTES_ADD_BOTH,
        F_ATTRIBUTES_ADD_BOTH_WITH_VALUES

        F_VALUES_ARRAY, F_VALUES_ADD.

        F_RESOLVES_ARRAY, F_RESOLVES, F_RESOLVES_ADD.

        F_AUTOS_ARRAY, F_AUTOS, F_AUTOS_ADD.

        F_TAGS_ARRAY, F_TAGS, F_TAGS_ADD_ATTRIBUTES, F_TAGS_ADD_METHODS,
        F_TAGS_ADD_RESOLVES, F_TAGS_ADD_AUTOS, F_TAGS_ADD_CATALOG,
        F_TAGS_ADD_DISPATCHER, F_TAGS_ADD_LOD, F_TAGS_ADD_SUPER.

        _palette

    DEPRECATED
    --------------------------------------------------------------------

        FV_MakeObj_Bar and  Bar  macro  are  deprecated  because  they  are
        useless, 'BarObject, End' does the same.

        FA_Class_Init, FA_Class_Exit,  F_INIT(),  F_EXIT()  are  deprecated
        since meta-classes availability.

        The FM_SetAsString method is deprecated, and  is  replaced  by  the
        FM_SetAs  method,  which  has  the same method id. You *must* check
        your sources !!

*/

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif
#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif
#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif
#ifndef FEELIN_COMPILER_H
#include <feelin/compiler.h>
#endif

#ifndef FEELIN_MACHINE_H
#include <feelin/machine.h>
#endif
#ifndef FEELIN_TYPES_H
#include <feelin/types.h>
#endif
#ifndef FEELIN_XMLAPPLICATION_H
#include <feelin/xmlapplication.h>
#endif
#ifndef FEELIN_XMLOBJECT_H
#include <feelin/xmlobject.h>
#endif
#ifndef FEELIN_XMLDOCUMENT_H
#include <feelin/xmldocument.h>
#endif

/* stegerg CHECKME: the check here was __GNUC__ and the check at the
   end (to undo prama pack) is __MORPHOS__! */

#ifndef __AROS__
#ifdef __GNUC__
#pragma pack(2)
#endif
#endif

#define ALL                                     (-1)
#define FV_FEELIN_VERSION                       10
#define FV_FEELIN_REVISION                      0

#define F_CODE_DEPRECATED                       0

/// Module information

/*
   All constants follow these rules :

   FM_<method>          - General method, understood by all classes [NUM]
   FM_<class>_<method>  - Class method                              [NUM,STR]
   FA_<attrib>          - General attribute, known by many classes. [NUM]
   FA_<class>_<attrib>  - Specific class attribute                  [NUM,STR]
   FV_<class>_<value>   - Special attribute value                   [NUM]
   FS_<name>            - Structure of arguments to a method        [SYS]

   FA_<attrib> definitions are followed by a comment [ISG] :
      I : it's possible to specify this attribute at object creation time.
      S : it's possible to change this attribute with F_Set().
      G : it's possible to get this attribute with F_Get().

   FR_<class> = Registered class id (unsigned 24 bits)
   FR_<class>_MTHD = First Method ID of <class>
   FR_<class>_ATTR = First Attribute ID of <class>

   Ranges:

   $C0000000 - $CFFFFFFF is reserved for custom classes methods.
   $8C000000 - $8CFFFFFF is reserved for custom classes attributes.

   $D0000000 - 4DFFFFFFF is reserved for Dynamic methods.
   $8D000000 - 41FFFFFFF is reserved for Dynamic attributes.

   $F0000000 - $FFFFFFFF is reserved for Feelin methods.
   $8F000000 - $8FFFFFFF is reserved for Feelin attributes.

   Since v6 of feelin.library most classes use dynamic IDs instead of static
   ones.  Dynamic  IDs  are  generated on the fly at run time, thus they are
   different at each session and future proof. You should  have  a  look  at
   Feelin_DEV.guide to learn more about this.

*/

#define FCCM_BASE               0xC0000000
#define FCCA_BASE               0x8C000000
#define DYNA_MTHD               0xD0000000
#define DYNA_ATTR               0x8D000000
#define MTHD_BASE               0xF0000000
#define ATTR_BASE               0x8F000000
//+
/// Macros
#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d)                        ((uint32) (a)<<24 | (uint32) (b)<<16 | (uint32) (c)<<8 | (uint32) (d))
#endif
#define MIN(a,b)                                (((a)<(b))?(a):(b))
#define MAX(a,b)                                (((a)>(b))?(a):(b))
#define _inside(a,a1,a2)                        ((a) >= (a1) && (a) <= (a2))
#define F_FEELIN_OPEN                           (FeelinBase = (struct FeelinBase *) OpenLibrary("feelin.library", FV_FEELIN_VERSION))
#define F_FEELIN_CLOSE                          CloseLibrary((struct Library *)(FeelinBase))
//+

struct FeelinBase
{
   struct Library               Libnode;
   BPTR                         Console;

   struct ExecBase              *SYS;
   struct DosLibrary            *DOS;
   struct GfxBase               *Graphics;
   struct IntuitionBase         *Intuition;
   struct Library               *Utility;
   struct Library               *Layers;
   struct Library               *Locale;
};

/****************************************************************************
*** feelin.library **********************************************************
****************************************************************************/

///feelin.library / F_LogA

enum    {

        FV_LOG_USER,
        FV_LOG_DEV,
        FV_LOG_CLASS,
        FV_LOG_CORE

        };

//+
///feelin.library / F_CreatePoolA

#define FA_Pool_Attributes                      (ATTR_BASE + 0)
#define FA_Pool_ItemSize                        (ATTR_BASE + 1)
#define FA_Pool_Items                           (ATTR_BASE + 2)
#define FA_Pool_Name                            (ATTR_BASE + 3)
#define FA_Pool_Public                          (ATTR_BASE + 4)

//+
///feelin.library / F_MakeObjA
enum    {

        FV_MakeObj_None,
        FV_MakeObj_Label,
        FV_MakeObj_Button,
        FV_MakeObj_BarTitle,
        FV_MakeObj_Gauge,
        FV_MakeObj_Slider,
        FV_MakeObj_Prop,
        FV_MakeObj_String,
        FV_MakeObj_Checkbox

        };

#define SimpleButton(x)                         F_MakeObj(FV_MakeObj_Button,x,TAG_DONE)
#define BarTitle(x)                             F_MakeObj(FV_MakeObj_BarTitle,x,TAG_DONE)
#define Gauge(h,mi,ma,v)                        F_MakeObj(FV_MakeObj_Gauge,h,mi,ma,v,TAG_DONE)
#define Prop(h,e,v,f)                           F_MakeObj(FV_MakeObj_Prop,h,e,v,f,TAG_DONE)
#define String(s,l)                             F_MakeObj(FV_MakeObj_String,s,l,TAG_DONE)
#define Slider(h,mi,ma,v)                       F_MakeObj(FV_MakeObj_Slider,h,mi,ma,v,TAG_DONE)
#define Checkbox(checked)                       F_MakeObj(FV_MakeObj_Checkbox,checked,TAG_DONE)
#define HLabel(label)                           F_MakeObj(FV_MakeObj_Label,TRUE,label,TAG_DONE)
#define VLabel(label)                           F_MakeObj(FV_MakeObj_Label,FALSE,label,TAG_DONE)
//+

/****************************************************************************
*** Builtin classes *********************************************************
****************************************************************************/

///Object                   0x000000
#define FR_Object                               0x000000
#define FR_Object_MTHD                          (MTHD_BASE | FR_Object)
#define FR_Object_ATTR                          (ATTR_BASE | FR_Object)
#define FC_Object                               "Object"

#define FM_New                                  (FR_Object_MTHD +  0)
#define FM_Dispose                              (FR_Object_MTHD +  1)
#define FM_Get                                  (FR_Object_MTHD +  2)
#define FM_Set                                  (FR_Object_MTHD +  3)
#define FM_Notify                               (FR_Object_MTHD +  4)
#define FM_UnNotify                             (FR_Object_MTHD +  5)
#define FM_CallHook                             (FR_Object_MTHD +  6)
#define FM_CallHookEntry                        (FR_Object_MTHD +  7)
#define FM_WriteLong                            (FR_Object_MTHD +  8)
#define FM_WriteString                          (FR_Object_MTHD +  9)
#define FM_MultiSet                             (FR_Object_MTHD + 10)
#define FM_SetAs                                (FR_Object_MTHD + 11)
#define FM_Export                               (FR_Object_MTHD + 12)
#define FM_Import                               (FR_Object_MTHD + 13)
#define FM_Connect                              (FR_Object_MTHD + 14)
#define FM_Disconnect                           (FR_Object_MTHD + 15)
#define FM_AddMember                            (FR_Object_MTHD + 16)
#define FM_RemMember                            (FR_Object_MTHD + 17)
#define FM_Lock                                 (FR_Object_MTHD + 18)
#define FM_Unlock                               (FR_Object_MTHD + 19)
struct  FS_Notify                               { uint32 Attribute; uint32 Value; FObject Target; uint32 Method; uint32 Count; /*...*/ };
struct  FS_UnNotify                             { FNotifyHandler *Handler; };
struct  FS_CallHook                             { struct Hook *Hook; /*...*/ };
struct  FS_CallHookEntry                        { FHookEntry Entry; /*...*/ };
struct  FS_WriteLong                            { uint32 Value; uint32 *Memory; };
struct  FS_WriteString                          { STRPTR Str; STRPTR Memory; };
struct  FS_MultiSet                             { uint32 Attribute; uint32 Value; /*...*/ };
struct  FS_SetAs                                { bits32 Flags; uint32 Attribute; STRPTR Data; /*...*/ };
struct  FS_Export                               { FObject Dataspace; uint32 id_Add;  };
struct  FS_Import                               { FObject Dataspace; uint32 id_Find; };
struct  FS_Connect                              { FObject Parent; };
struct  FS_AddMember                            { FObject Object; uint32 Position; FObject Previous; };
struct  FS_RemMember                            { FObject Member; };
struct  FS_Lock                                 { bits32 Flags; };

#define FA_Class                                (FR_Object_ATTR +  0)
#define FA_Revision                             (FR_Object_ATTR +  1)
#define FA_Version                              (FR_Object_ATTR +  2)
#define FA_ID                                   (FR_Object_ATTR +  3)
#define FA_UserData                             (FR_Object_ATTR +  4)
#define FA_NoNotify                             (FR_Object_ATTR +  5)
#define FA_Parent                               (FR_Object_ATTR +  6)
#define FA_Child                                (FR_Object_ATTR +  7)
#define FA_ContextHelp                          (FR_Object_ATTR +  8)
#define FA_ContextMenu                          (FR_Object_ATTR +  9)

enum    {

        FV_Notify_None,
        FV_Notify_Self,
        FV_Notify_Parent,
        FV_Notify_Window,
        FV_Notify_Application

        };

#define FV_Notify_Always                        0x49893131
#define FV_Notify_Toggle                        0x49893132
#define FV_Notify_Value                         0x49893131

enum    {

        FV_AddMember_Head = -1,
        FV_AddMember_Tail,
        FV_AddMember_Insert

        };

enum    {

        FV_SetAs_String = 1,
        FV_SetAs_Decimal,
        FV_SetAs_Hexadecimal,
        FV_SetAs_Constant

        };

#define FF_SetAs_Notify                         (1 << 31)

#define FF_Lock_Exclusive                       (1 << 0)
#define FF_Lock_Shared                          (1 << 1)
#define FF_Lock_Attempt                         (1 << 2)

#define _class(o)                               ((FClass *)(((uint32 *)(o))[-1]))
#define _classname(o)                           (_class(o) -> Name)

#define F_METHOD_NEW(name)                      F_METHODM(FObject,name,TagItem)
#define F_METHOD_DISPOSE(name)                  F_METHOD(void,name)
#define F_METHOD_GET(name)                      F_METHOD(void,name)
#define F_METHOD_SET(name)                      F_METHOD(void,name)
#define F_METHOD_EXPORT(name)                   F_METHODM(void,name,FS_Export)
#define F_METHOD_IMPORT(name)                   F_METHODM(void,name,FS_Import)
#define F_METHOD_CONNECT(name)                  F_METHODM(int32,name,FS_Connect)
#define F_METHOD_DISCONNECT(name)               F_METHODM(int32,name,FS_Disconnect)
#define F_METHOD_ADDMEMBER(name)                F_METHODM(FFamilyNode *,name,FS_AddMember)
#define F_METHOD_REMMEMBER(name)                F_METHODM(int32,name,FS_RemMember)

//+
///Class                    0x000040
#define FR_Class                                 0x000040
#define FR_Class_MTHD                           (MTHD_BASE | FR_Class)
#define FR_Class_ATTR                           (ATTR_BASE | FR_Class)
#define FC_Class                                "Class"

#define FA_Class_Name                           (FR_Class_ATTR +  0)
#define FA_Class_Super                          (FR_Class_ATTR +  1)
#define FA_Class_LODSize                        (FR_Class_ATTR +  2)
#define FA_Class_Dispatcher                     (FR_Class_ATTR +  3)
#define FA_Class_UserData                       (FR_Class_ATTR +  4)
#define FA_Class_Pool                           (FR_Class_ATTR +  5)
#define FA_Class_Methods                        (FR_Class_ATTR +  6)
#define FA_Class_Attributes                     (FR_Class_ATTR +  7)
#define FA_Class_ResolveTable                   (FR_Class_ATTR +  8)
#define FA_Class_AutoResolveTable               (FR_Class_ATTR +  9)
#define FA_Class_CatalogName                    (FR_Class_ATTR + 10)
#define FA_Class_CatalogTable                   (FR_Class_ATTR + 11)
#define FA_Class_Module                         (FR_Class_ATTR + 12)

enum    {

        FV_Query_ClassTags = 1,
        FV_Query_MetaClassTags,
        FV_Query_PrefsTags,
        FV_Query_MetaPrefsTags,
        FV_Query_DecoratorPrefsTags,
        FV_Query_MetaDecoratorPrefsTags

        };

/*** Macros ****************************************************************/

#define F_QUERY()                               struct TagItem * feelin_auto_class_query(uint32 Which)

#define F_SUPERDO()                             F_SuperDoA(Class,Obj,Method,Msg)
#define F_OBJDO(o)                              F_DoA(o,Method,Msg)
#define F_STORE(val)                            *((uint32 *)(item.ti_Data)) = (uint32)(val) /* Only for Classes using F_DynamicNTI() */
#define F_LOD(cl,o)                             ((APTR)((uint32)(o) + cl -> Offset))  /* Used by within classes to get LocalObjectData */
#define F_ID(tab,n)                             (tab[n].ID)
#define F_IDM(n)                                F_ID(Class -> Methods,n)
#define F_IDA(n)                                F_ID(Class -> Attributes,n)
#define F_IDR(n)                                F_ID(Class -> ResolvedIDs,n)
#define F_IDO(n)                                F_ID(Class -> AutoResolvedIDs,n)
#define F_CAT(n)                                FCC_CatalogTable[CAT_ ## n].String

#define F_ARRAY_END                             { 0 }

#define F_VALUES_ARRAY(name)                    FClassAttributeValue feelin_auto_values_##name[]
#define F_VALUES_ADD(name,value)                { name, value }

#define F_ATTRIBUTES_ARRAY                                          FClassAttribute feelin_auto_attributes[]
#define F_ATTRIBUTES_ADD(name,type)                                 { name, type, 0, NULL }
#define F_ATTRIBUTES_ADD_WITH_VALUES(name,type,values)              { name, type, 0, feelin_auto_values_##values }
#define F_ATTRIBUTES_ADD_STATIC(name,type,id)                       { name, type, id, NULL }
#define F_ATTRIBUTES_ADD_STATIC_WITH_VALUES(name,type,id,values)    { name, type, id, feelin_auto_values_##values }
#define F_ATTRIBUTES_ADD_BOTH(name,type,id)                         { name, type, id, NULL }
#define F_ATTRIBUTES_ADD_BOTH_WITH_VALUES(name,type,id,values)      { name, type, id, feelin_auto_values_##values }
#define F_ATTRIBUTES                                                feelin_auto_attributes

/*

GOFROMIEL: j'ai supprimé le (APTR) que tu utilisais  pour  caster  'name'
dans  la  macro F_METHODS_ADD_BOTH, parce que c'était pas une bonne idée du
tout. J'ai perdu une heure à  chercher  pourquoi  ma  méthode  n'était  pas
appelée,  j'avais  utilisé  'F_METHOD_ADD(Prefs_New,  FM_New)'. J'ai trouvé
cela relativement pénible.

YOMGUI: oula alors là je t'arrête... c'est pas normal du tout!
faut trouver pourquoi... car le cast est on ne peut plus légitime: on attend un pointeur.
Or dans ton exemple FM_New est un nombre, qui va être casté en pointeur. où est le pb?

GOFROMIEL: C'est le cast le pb ! Dans le cas que  je  te  site  FM_New  est
casté en pointer du coup pas de problème pour la structure, mais gros blème
pour moi parce que j'ai fais n'importe quoi  et  que  personne  ne  m'en  a
empéché  :-)  De toute façon le cast est inutile, ou alors un STRPTR aurait
était plus approprié, mais toujours inutile :-P

*/

#define F_METHODS_ARRAY                         FClassMethod feelin_auto_methods[]
#define F_METHODS_ADD_BOTH(func,name,id)        { (FMethod) &F_FUNCTION_GATE(func), name, id }
#define F_METHODS_ADD_STATIC(func,id)           F_METHODS_ADD_BOTH(func,NULL,id)
#define F_METHODS_ADD(func,name)                F_METHODS_ADD_BOTH(func,name,0)
#define F_METHODS                               feelin_auto_methods

/*

GOFROMIEL: j'ai renomé F_RESOLVE en F_RESOLVES

YOMGUI: oui je les avait oubliées, mais je les ai mises après ;-)

GOFROMIEL: Fais ton malin vas ;-)

*/

#define F_RESOLVES_ARRAY                        FDynamicEntry feelin_auto_resolves[]
#define F_RESOLVES_ADD(name)                    { name, 0 }
#define F_RESOLVES                              feelin_auto_resolves

#define F_AUTOS_ARRAY                           FDynamicEntry feelin_auto_autos[]
#define F_AUTOS_ADD(name)                       { name, 0 }
#define F_AUTOS                                 feelin_auto_autos

#define F_TAGS_ARRAY                            struct TagItem feelin_auto_tags[]
#define F_TAGS_ADD(tag,data)                    { FA_Class_##tag, (uint32) data }
#define F_TAGS                                  feelin_auto_tags

/* Predefined tags */

#define F_TAGS_ADD_ATTRIBUTES                   F_TAGS_ADD(Attributes, F_ATTRIBUTES)
#define F_TAGS_ADD_METHODS                      F_TAGS_ADD(Methods, F_METHODS)
#define F_TAGS_ADD_RESOLVES                     F_TAGS_ADD(ResolveTable, F_RESOLVES)
#define F_TAGS_ADD_AUTOS                        F_TAGS_ADD(AutoResolveTable, F_AUTOS)
#define F_TAGS_ADD_CATALOG                      F_TAGS_ADD(CatalogTable, FCC_CatalogTable)
#define F_TAGS_ADD_DISPATCHER(name)             F_TAGS_ADD(Dispatcher, F_FUNCTION_GATE(name))
#define F_TAGS_ADD_LOD                          F_TAGS_ADD(LODSize, sizeof (struct LocalObjectData))
#define F_TAGS_ADD_SUPER(name)                  F_TAGS_ADD(Super, FC_##name)

//+

/****************************************************************************
*** Support *****************************************************************
****************************************************************************/

///Render                   0x900000
#define FR_Render                               0x900000
#define FR_Render_MTHD                          (MTHD_BASE | FR_Render)
#define FR_Render_ATTR                          (ATTR_BASE | FR_Render)

#define FA_Render_Application                   (FR_Render_ATTR +  0)
#define FA_Render_Display                       (FR_Render_ATTR +  1)
#define FA_Render_Window                        (FR_Render_ATTR +  2)
#define FA_Render_RPort                         (FR_Render_ATTR +  3)
#define FA_Render_Palette                       (FR_Render_ATTR +  4)
#define FA_Render_Friend                        (FR_Render_ATTR +  5)
#define FA_Render_Forbid                        (FR_Render_ATTR +  6)

#define FM_Render_AddClip                       (FR_Render_MTHD +  0)
#define FM_Render_RemClip                       (FR_Render_MTHD +  1)
#define FM_Render_AddClipRegion                 (FR_Render_MTHD +  2)
#define FM_Render_CreateBuffer                  (FR_Render_MTHD +  3)
#define FM_Render_DeleteBuffer                  (FR_Render_MTHD +  4)
struct  FS_Render_AddClip                       { FRect *Rect; };
struct  FS_Render_RemClip                       { APTR Handle; };
struct  FS_Render_AddClipRegion                 { struct Region *Region; };
struct  FS_Render_CreateBuffer                  { uint32 Width; uint32 Height; };
struct  FS_Render_DeleteBuffer                  { FRender *Buffer; };

#define FF_Render_Refreshing                    (1 <<  0)
#define FF_Render_Complex                       (1 <<  1)
#define FF_Render_TrueColors                    (1 << 30)
#define FF_Render_Forbid                        (1 << 31)

#define FC_Render                               "Render"
#define RenderObject                            F_NewObj(FC_Render
//+
///Family                   0x900040
#define FR_Family                               0x900040
#define FR_Family_MTHD                          (MTHD_BASE | FR_Family)
#define FR_Family_ATTR                          (ATTR_BASE | FR_Family)

#define FA_Family                               (FR_Family_ATTR +  0)
#define FA_Family_Head                          (FR_Family_ATTR +  1)
#define FA_Family_Tail                          (FR_Family_ATTR +  2)
#define FA_Family_Owner                         (FR_Family_ATTR +  3)
#define FA_Family_CreateNodeHook                (FR_Family_ATTR +  4)
#define FA_Family_DeleteNodeHook                (FR_Family_ATTR +  5)

struct  FS_Family_CreateNode                    { FObject Object; };
struct  FS_Family_DeleteNode                    { FFamilyNode *Node; };

#define FC_Family                               "Family"
#define FamilyObject                            F_NewObj(FC_Family
#define Child                                   FA_Child
#define End                                     TAG_DONE)
//+
///ImageDisplay             0x9000C0
#define FR_ImageDisplay                         0x9000C0
#define FR_ImageDisplay_MTHD                    (MTHD_BASE | FR_ImageDisplay)
#define FR_ImageDisplay_ATTR                    (ATTR_BASE | FR_ImageDisplay)

#define FM_ImageDisplay_Setup                   (FR_ImageDisplay_MTHD + 0)
#define FM_ImageDisplay_Cleanup                 (FR_ImageDisplay_MTHD + 1)
#define FM_ImageDisplay_Draw                    (FR_ImageDisplay_MTHD + 2)
struct  FS_ImageDisplay_Setup                   { FRender *Render; };
struct  FS_ImageDisplay_Cleanup                 { FRender *Render; };
struct  FS_ImageDisplay_Draw                    { FRender *Render; FRect *Rect; bits32 Flags; };
struct  FS_ImageDisplay_HookDraw                { FRender *Render; FRect *Rect; bits32 Flags; FRect *Region; };

#define FA_ImageDisplay_Spec                    (FR_ImageDisplay_ATTR + 0)
#define FA_ImageDisplay_State                   (FR_ImageDisplay_ATTR + 1)
#define FA_ImageDisplay_Width                   (FR_ImageDisplay_ATTR + 2)
#define FA_ImageDisplay_Height                  (FR_ImageDisplay_ATTR + 3)
#define FA_ImageDisplay_Mask                    (FR_ImageDisplay_ATTR + 4)
#define FA_ImageDisplay_Origin                  (FR_ImageDisplay_ATTR + 5)

#define FV_ImageDisplay_Separator               ';'

#define FF_ImageDisplay_Region                  (1 << 0)

/* Builtin patterns */

enum    {

        FI_None,
        FI_Shine,               FI_HalfShine,               FI_Fill,            FI_HalfShadow,          FI_Shadow,          FI_HalfDark,            FI_Dark,            FI_Highlight,
        FI_Shine_HalfShine,     FI_HalfShine_Fill,          FI_Fill_HalfShadow, FI_HalfShadow_Shadow,   FI_Shadow_HalfDark, FI_HalfDark_Dark,       FI_Dark_Highlight,
        FI_Shine_Fill,          FI_HalfShine_HalfShadow,    FI_Fill_Shadow,     FI_HalfShadow_HalfDark, FI_Shadow_Dark,     FI_HalfDark_Highlight,
        FI_Shine_HalfShadow,    FI_HalfShine_Shadow,        FI_Fill_HalfDark,   FI_HalfShadow_Dark,     FI_Shadow_Highlight,
        FI_Shine_Shadow,        FI_HalfShine_HalfDark,      FI_Fill_Dark,       FI_HalfShadow_Highlight,
        FI_Shine_HalfDark,      FI_HalfShine_Dark,          FI_Fill_Highlight,
        FI_Shine_Dark,          FI_HalfShine_Highlight,
        FI_Shine_Highlight

        };

#define FC_ImageDisplay                         "ImageDisplay"
#define ImageDisplayObject                      F_NewObj(FC_ImageDisplay
//+
///TextDisplay              0x900100
#define FR_TextDisplay                          0x900100
#define FR_TextDisplay_MTHD                     (MTHD_BASE | FR_TextDisplay)
#define FR_TextDisplay_ATTR                     (ATTR_BASE | FR_TextDisplay)

#define FM_TextDisplay_Setup                    (FR_TextDisplay_MTHD + 0)
#define FM_TextDisplay_Cleanup                  (FR_TextDisplay_MTHD + 1)
#define FM_TextDisplay_Draw                     (FR_TextDisplay_MTHD + 2)
struct  FS_TextDisplay_Setup                    { FRender *Render; };
struct  FS_TextDisplay_Draw                     { FRect *Rect; bits32 Flags; };

#define FA_TextDisplay_Contents                 (FR_TextDisplay_ATTR +  0)
#define FA_TextDisplay_PreParse                 (FR_TextDisplay_ATTR +  1)
#define FA_TextDisplay_Font                     (FR_TextDisplay_ATTR +  2)
#define FA_TextDisplay_Width                    (FR_TextDisplay_ATTR +  3)
#define FA_TextDisplay_Height                   (FR_TextDisplay_ATTR +  4)
#define FA_TextDisplay_Shortcut                 (FR_TextDisplay_ATTR +  5)

#define FC_TextDisplay                          "TextDisplay"
#define TextDisplayObject                       F_NewObj(FC_TextDisplay
//+

/****************************************************************************
*** GUI System **************************************************************
****************************************************************************/

///Application              0x004000
#define FR_Application                          0x004000
#define FR_Application_MTHD                     (MTHD_BASE | FR_Application)
#define FR_Application_ATTR                     (ATTR_BASE | FR_Application)

#define FM_Application_Run                      (FR_Application_MTHD +  0)
#define FM_Application_Shutdown                 (FR_Application_MTHD +  1)
#define FM_Application_Sleep                    (FR_Application_MTHD +  2)
#define FM_Application_Awake                    (FR_Application_MTHD +  3)
#define FM_Application_PushMethod               (FR_Application_MTHD +  4)
#define FM_Application_Setup                    (FR_Application_MTHD +  5)
#define FM_Application_Cleanup                  (FR_Application_MTHD +  6)
#define FM_Application_Load                     (FR_Application_MTHD +  7)
#define FM_Application_Save                     (FR_Application_MTHD +  8)
#define FM_Application_Resolve                  (FR_Application_MTHD +  9)
#define FM_Application_ResolveInt               (FR_Application_MTHD + 10)
#define FM_Application_OpenFont                 (FR_Application_MTHD + 11)
#define FM_Application_AddSignalHandler         (FR_Application_MTHD + 12)
#define FM_Application_RemSignalHandler         (FR_Application_MTHD + 13)
#define FM_Application_Update                   (FR_Application_MTHD + 14)
#define FM_Application_ModifyEvents             (FR_Application_MTHD + 15)
struct  FS_Application_PushMethod               { FObject Target; uint32 Method ; uint32 Count; /*...*/ };
struct  FS_Application_Load                     { STRPTR Name; };
struct  FS_Application_Save                     { STRPTR Name; };
struct  FS_Application_Resolve                  { STRPTR Name; uint32 Default; };
struct  FS_Application_OpenFont                 { FObject Object; STRPTR Spec; };
struct  FS_Application_AddSignalHandler         { struct FeelinSignalHandler *Handler; };
struct  FS_Application_RemSignalHandler         { struct FeelinSignalHandler *Handler; };
struct  FS_Application_ModifyEvents             { bits32 Events; FObject Window; };

#define FA_Application                          (FR_Application_ATTR +  0)
#define FA_Application_Title                    (FR_Application_ATTR +  1)
#define FA_Application_Version                  (FR_Application_ATTR +  2)
#define FA_Application_Copyright                (FR_Application_ATTR +  3)
#define FA_Application_Author                   (FR_Application_ATTR +  4)
#define FA_Application_Description              (FR_Application_ATTR +  5)
#define FA_Application_Base                     (FR_Application_ATTR +  6)
#define FA_Application_Unique                   (FR_Application_ATTR +  7)
#define FA_Application_Signal                   (FR_Application_ATTR +  8)
#define FA_Application_UserSignals              (FR_Application_ATTR +  9)
#define FA_Application_WindowPort               (FR_Application_ATTR + 10)
#define FA_Application_BrokerPort               (FR_Application_ATTR + 11)
#define FA_Application_BrokerHook               (FR_Application_ATTR + 12)
#define FA_Application_BrokerPri                (FR_Application_ATTR + 13)
#define FA_Application_Broker                   (FR_Application_ATTR + 14)
#define FA_Application_Display                  (FR_Application_ATTR + 15)
#define FA_Application_Dataspace                (FR_Application_ATTR + 16)
#define FA_Application_Preference               (FR_Application_ATTR + 17)
#define FA_Application_Sleep                    (FR_Application_ATTR + 18)
#define FA_Application_ResolveMapping           (FR_Application_ATTR + 19)
#define FA_Application_Menu                     (FR_Application_ATTR + 20)
#define FA_Application_MenuSelection            (FR_Application_ATTR + 21)

enum    {

        FV_Application_ENV,
        FV_Application_ENVARC,
        FV_Application_BOTH

        };

enum    {

        FV_KEY_NONE,
        FV_KEY_PRESS,
        FV_KEY_RELEASE,
        FV_KEY_UP,
        FV_KEY_DOWN,
        FV_KEY_STEPUP,
        FV_KEY_STEPDOWN,
        FV_KEY_TOP,
        FV_KEY_BOTTOM,
        FV_KEY_LEFT,
        FV_KEY_RIGHT,
        FV_KEY_STEPLEFT,
        FV_KEY_STEPRIGHT,
        FV_KEY_FIRST,
        FV_KEY_LAST,
        FV_KEY_CHARBACK,
        FV_KEY_CHARDEL,
        FV_KEY_WORDBACK,
        FV_KEY_WORDDEL,
        FV_KEY_LINEBACK,
        FV_KEY_LINEDEL,
        FV_KEY_NEXTOBJ,
        FV_KEY_PREVOBJ,
        FV_KEY_NOOBJ,
        FV_KEY_CLOSEWINDOW,

        FV_KEY_COUNT

        };

#define FC_Application                          "Application"
#define ApplicationObject                       F_NewObj(FC_Application
#define AppObject                               F_NewObj(FC_Application

/*** FSignalHandler ********************************************************/

typedef struct FeelinSignalHandler
{
    struct FeelinSignalHandler     *Next;
    struct FeelinSignalHandler     *Prev;
/* end of FeelinNode header */
    bits32                          Flags;
    FObject                         Object;
    uint32                          Method;

    union
    {
        struct
        {
            bits32                  Signals;
            int32                   Reserved;
        }
        fsh_sig;

        struct
        {
            uint32                  Secs;
            uint32                  Micros;
        } fsh_timer;
    }
    fsh_union;
}
FSignalHandler;

#define fsh_Signals                     fsh_union.fsh_sig.Signals
#define fsh_Secs                        fsh_union.fsh_timer.Secs
#define fsh_Micros                      fsh_union.fsh_timer.Micros

#define FF_SignalHandler_Timer          (1 << 0)
#define FF_SignalHandler_Active         (1 << 1)

/*** FEvent ****************************************************************/

typedef struct FeelinEvent
{
    struct IntuiMessage                *IMsg;

    bits32                              Flags;

    uint32                              Class;
    uint16                              Code;
    uint16                              Qualifier;
    uint8                               Key;
    uint8                               DecodedChar;
    int16                               reserved;

    int16                               MouseX;
    int16                               MouseY;
    uint32                              Seconds;
    uint32                              Micros;

    FObject                             Window;
}
FEvent;

/*** classes ***/

#define FF_EVENT_TICK                           (1 << 0)
#define FF_EVENT_KEY                            (1 << 1)
#define FF_EVENT_BUTTON                         (1 << 2)
#define FF_EVENT_MOTION                         (1 << 3)
#define FF_EVENT_WINDOW                         (1 << 4)
#define FF_EVENT_DISK                           (1 << 5)
#define FF_EVENT_WBDROP                         (1 << 6)

/*** codes ***/

enum    {

        FV_EVENT_BUTTON_SELECT,
        FV_EVENT_BUTTON_MENU,
        FV_EVENT_BUTTON_MIDDLE,
        FV_EVENT_BUTTON_WHEEL

        };

enum    {

        FV_EVENT_WINDOW_CLOSE,
        FV_EVENT_WINDOW_ACTIVE,
        FV_EVENT_WINDOW_INACTIVE,
        FV_EVENT_WINDOW_SIZE,
        FV_EVENT_WINDOW_DEPTH,
        FV_EVENT_WINDOW_CHANGE,
        FV_EVENT_WINDOW_REFRESH

        };

enum    {

        FV_EVENT_DISK_LOAD,
        FV_EVENT_DISK_EJECT

        };

/*** flags ***/

#define FF_EVENT_KEY_UP                         (1 << 0)
#define FF_EVENT_KEY_REPEAT                     (1 << 1)
#define FF_EVENT_BUTTON_DOWN                    (1 << 0)
#define FF_EVENT_BUTTON_DOUBLE                  (1 << 1)

/*** FAppResolveMap ********************************************************/

typedef struct FeelinApplicationResolveMapping
{
   STRPTR               Name;
   APTR                 Data;
}
FAppResolveMap;

//+
///Window                   0x004040
#define FR_Window                               0x004040
#define FR_Window_MTHD                          (MTHD_BASE | FR_Window)
#define FR_Window_ATTR                          (ATTR_BASE | FR_Window)

#define FM_Window_Setup                         (FR_Window_MTHD +  0)
#define FM_Window_Cleanup                       (FR_Window_MTHD +  1)
#define FM_Window_Open                          (FR_Window_MTHD +  2)
#define FM_Window_Close                         (FR_Window_MTHD +  3)
#define FM_Window_ChainAdd                      (FR_Window_MTHD +  4)
#define FM_Window_ChainRem                      (FR_Window_MTHD +  5)
#define FM_Window_AddEventHandler               (FR_Window_MTHD +  6)
#define FM_Window_RemEventHandler               (FR_Window_MTHD +  7)
#define FM_Window_HandleEvent                   (FR_Window_MTHD +  8)
#define FM_Window_Layout                        (FR_Window_MTHD +  9)
#define FM_Window_Draw                          (FR_Window_MTHD + 10)
#define FM_Window_Zoom                          (FR_Window_MTHD + 11)
#define FM_Window_Depth                         (FR_Window_MTHD + 12)
#define FM_Window_Help                          (FR_Window_MTHD + 13)
#define FM_Window_RequestRethink                (FR_Window_MTHD + 14)
#define FM_Window_PushDraw                      (FR_Window_MTHD + 15)
struct  FS_Window_ChainAdd                      { FObject Object; };
struct  FS_Window_ChainRem                      { FObject Object; };
struct  FS_Window_AddEventHandler               { struct FeelinEventHandler *Handler; };
struct  FS_Window_RemEventHandler               { struct FeelinEventHandler *Handler; };
struct  FS_Window_Zoom                          { int32 Zoom; };
struct  FS_Window_Depth                         { int32 Depth; };
struct  FS_Window_Help                          { uint32 Action; uint32 MouseX; uint32 MouseY; };
struct  FS_Window_RequestRethink                { FObject Source; };

#define FA_Window                               (FR_Window_ATTR +  0)
#define FA_Window_Title                         (FR_Window_ATTR +  1)
#define FA_Window_ScreenTitle                   (FR_Window_ATTR +  2)
#define FA_Window_Open                          (FR_Window_ATTR +  3)
#define FA_Window_CloseRequest                  (FR_Window_ATTR +  4)
#define FA_Window_Active                        (FR_Window_ATTR +  5)
#define FA_Window_ActiveObject                  (FR_Window_ATTR +  6)
#define FA_Window_Backdrop                      (FR_Window_ATTR +  7)
#define FA_Window_Borderless                    (FR_Window_ATTR +  8)
#define FA_Window_Resizable                     (FR_Window_ATTR +  9)
#define FA_Window_GadNone                       (FR_Window_ATTR + 10)
#define FA_Window_GadDragbar                    (FR_Window_ATTR + 11)
#define FA_Window_GadClose                      (FR_Window_ATTR + 12)
#define FA_Window_GadDepth                      (FR_Window_ATTR + 13)
#define FA_Window_GadIconify                    (FR_Window_ATTR + 14)
#define FA_Window_Decorator                     (FR_Window_ATTR + 15)

enum    {

        FV_Window_ActiveObject_Prev = -2,
        FV_Window_ActiveObject_Next,
        FV_Window_ActiveObject_None

        };

enum    {

        FV_Window_Zoom_Full = - 3,
        FV_Window_Zoom_Max,
        FV_Window_Zoom_Toggle

        };

enum    {

        FV_Window_Depth_Back = -3,
        FV_Window_Depth_Front,
        FV_Window_Depth_Toggle

        };

enum    {

        FV_Window_Help_Show = 1,
        FV_Window_Help_Hide

        };

#define FC_Window                               "Window"
#define WindowObject                            F_NewObj(FC_Window

/***************************************************************************/
/*** FEventHandler *********************************************************/
/***************************************************************************/

typedef struct FeelinEventHandler
{
    struct FeelinEventHandler      *Next;
    struct FeelinEventHandler      *Prev;
/* end of FeelinNode header */
    bits16                          Flags;
    int8                            Priority;
    uint8                           reserved;
    bits32                          Events;
    FObject                         Object;
    FClass                         *Class;
}
FEventHandler;

#define FF_EventHandler_Active                  (1 << 0)

/*** ***/

#define FF_HandleEvent_Eat                      (1 << 0)
//+
///Display                  0x004080
#define FR_Display                              0x004080
#define FR_Display_MTHD                         (MTHD_BASE | FR_Display)
#define FR_Display_ATTR                         (ATTR_BASE | FR_Display)

#define FM_AddColor                             (FR_Display_MTHD +  0)
#define FM_RemColor                             (FR_Display_MTHD +  1)
#define FM_AddPalette                           (FR_Display_MTHD +  2)
#define FM_RemPalette                           (FR_Display_MTHD +  3)
#define FM_CreateColor                          (FR_Display_MTHD +  4)
#define FM_CreateColorScheme                    (FR_Display_MTHD +  5)
struct  FS_AddColor                             { uint32 ARGB; };
struct  FS_RemColor                             { FColor *Color; };
struct  FS_AddPalette                           { uint32 Count; uint32 *ARGBs; };
struct  FS_RemPalette                           { FPalette *Palette; };
struct  FS_Display_DecodeColor                  { STRPTR Spec; FPalette *Reference; };
struct  FS_CreateColor                          { STRPTR Spec; FPalette *Reference; };
struct  FS_DeleteColor                          { FColor *Color; };
struct  FS_CreateColorScheme                    { STRPTR Spec; FPalette *Reference; };
struct  FS_DeleteColorScheme                    { FPalette *Palette; };
struct  FS_Display_PixelRead                    { uint32 PixelX, PixelY; };

#define FA_ColorScheme                          (FR_Display_ATTR +  0)
#define FA_DisabledColorScheme                  (FR_Display_ATTR +  1)

enum    {

        FV_Pen_Text,         // 0
        FV_Pen_Shine,        // 1
        FV_Pen_HalfShine,    // 2
        FV_Pen_Fill,         // 3
        FV_Pen_HalfShadow,   // 4
        FV_Pen_Shadow,       // 5
        FV_Pen_HalfDark,     // 6
        FV_Pen_Dark,         // 7
        FV_Pen_Highlight,    // 8
        FV_PEN_COUNT

        };

#define FV_COLOR_SPACE                          12

enum    {                                       // FA_Display_Name

        FV_Display_Frontmost = -2,
        FV_Display_Workbench,
        FV_Display_Public

        };

typedef struct FeelinHSV
{
    uint16                                      h;
    uint8                                       s,v;
}
FHSV;

typedef struct FeelinRGB
{
    uint8                                       r,g,b;
}
FRGB;

#define FC_Display                              "Display"
#define DisplayObject                           F_NewObj(FC_Display
//+

/****************************************************************************
*** GUI Classes *************************************************************
****************************************************************************/

///Frame                    0x001000
#define FR_Frame                    0x001000
#define FR_Frame_MTHD                           (MTHD_BASE | FR_Frame)
#define FR_Frame_ATTR                           (ATTR_BASE | FR_Frame)

#define FA_Frame                                (FR_Frame_ATTR +  0)
#define FA_Frame_PublicData                     (FR_Frame_ATTR +  1)
#define FA_Frame_Font                           (FR_Frame_ATTR +  2)
#define FA_Frame_Title                          (FR_Frame_ATTR +  3)
#define FA_Frame_PreParse                       (FR_Frame_ATTR +  4)
#define FA_Frame_Position                       (FR_Frame_ATTR +  5)
#define FA_Frame_InnerLeft                      (FR_Frame_ATTR +  6)
#define FA_Frame_InnerTop                       (FR_Frame_ATTR +  7)
#define FA_Frame_InnerRight                     (FR_Frame_ATTR +  8)
#define FA_Frame_InnerBottom                    (FR_Frame_ATTR +  9)
#define FA_Back                                 (FR_Frame_ATTR + 12)

#define FM_Frame_Setup                          (FR_Frame_MTHD +  0)
#define FM_Frame_Cleanup                        (FR_Frame_MTHD +  1)
#define FM_Frame_Draw                           (FR_Frame_MTHD +  2)
struct  FS_Frame_Setup                          { FRender *Render; };
struct  FS_Frame_Cleanup                        { FRender *Render; };
struct  FS_Frame_Draw                           { FRender *Render; FBox *Box; bits32 Flags; };

#define FV_Frame_None                           0
#define FV_Frame_Count                          255//49
#define FV_Frame_Separator                      '\n'

enum    {

        FV_Frame_UpLeft,
        FV_Frame_UpRight,
        FV_Frame_UpCenter,
        FV_Frame_DownLeft,
        FV_Frame_DownRight,
        FV_Frame_DownCenter

        };

#define FF_Frame_Draw_Select                    (1 << 0)
#define FF_Frame_Draw_Erase                     (1 << 1)
#define FF_Frame_Draw_Only                      (1 << 2)

/*OLAV: J'ai renomé le membre 'Space' en 'Padding'  (pour  aller  avec  les
padding-left...).  J'ai  viré  le  membre  ID  qui n'a rien à faire dans la
structure publique, surtout que les frames vont finir par  être  bien  plus
complexes qu'actuellement */

typedef struct FeelinFramePublic
{
    FInner                          Border[2];
    FInner                          Padding[2];
    FObject                         Back;
}
FFramePublic;

#define FC_Frame                                "Frame"
#define FrameObject                             F_NewObj(FC_Frame

#define NoFrame                                 FA_Frame, FV_Frame_None
#define StringFrame                             FA_Frame,"$string-frame"
#define PropFrame                               FA_Frame,"$prop-frame"
#define SliderFrame                             FA_Frame,"$slider-frame"
#define GaugeFrame                              FA_Frame,"$gauge-frame"
#define GroupFrame                              FA_Frame,"$group-frame"
//+
///Area                     0x001040
#define FR_Area                                 0x001040
#define FR_Area_MTHD                            (MTHD_BASE | FR_Area)
#define FR_Area_ATTR                            (ATTR_BASE | FR_Area)

#define FM_Setup                                (FR_Area_MTHD +  0)
#define FM_Cleanup                              (FR_Area_MTHD +  1)
#define FM_Show                                 (FR_Area_MTHD +  2)
#define FM_Hide                                 (FR_Area_MTHD +  3)
#define FM_AskMinMax                            (FR_Area_MTHD +  4)
#define FM_Layout                               (FR_Area_MTHD +  5)
#define FM_Draw                                 (FR_Area_MTHD +  6)
#define FM_Erase                                (FR_Area_MTHD +  7)
#define FM_HandleEvent                          (FR_Area_MTHD +  8)
#define FM_ModifyHandler                        (FR_Area_MTHD +  9)
#define FM_GoActive                             (FR_Area_MTHD + 10)
#define FM_GoInactive                           (FR_Area_MTHD + 11)
#define FM_GoEnabled                            (FR_Area_MTHD + 12)
#define FM_GoDisabled                           (FR_Area_MTHD + 13)
#define FM_DnDDo                                (FR_Area_MTHD + 14)
#define FM_DnDQuery                             (FR_Area_MTHD + 15)
#define FM_DnDBegin                             (FR_Area_MTHD + 16)
#define FM_DnDFinish                            (FR_Area_MTHD + 17)
#define FM_DnDReport                            (FR_Area_MTHD + 18)
#define FM_DnDDrop                              (FR_Area_MTHD + 19)

#if F_CODE_DEPRECATED
#define FM_RethinkLayout                        (FR_Area_MTHD + 20)
#endif

#define FM_BuildContextMenu                     (FR_Area_MTHD + 21)
#define FM_BuildContextHelp                     (FR_Area_MTHD + 22)
struct  FS_Setup                                { FRender *Render; };
struct  FS_Draw                                 { bits32 Flags; };
struct  FS_Erase                                { FRect *Rect; bits32 Flags; };
struct  FS_HandleEvent                          { FEvent *Event; };
struct  FS_ModifyHandler                        { bits32 Add; bits32 Sub; };
struct  FS_DnDDo                                { int32 MouseX, MouseY; };
struct  FS_DnDQuery                             { int32 MouseX, MouseY; FObject Source; FBox *Box; };
struct  FS_DnDBegin                             { FObject Source; };
struct  FS_DnDFinish                            { FObject Source; };
struct  FS_DnDReport                            { int32 MouseX, MouseY; FObject Source; uint32 Update; FBox *DragBox; };
struct  FS_DnDDrop                              { int32 MouseX, MouseY; FObject Source; };

#if F_CODE_DEPRECATED
struct  FS_RethinkLayout                        { uint32 Operation; };
#endif

struct  FS_BuildContextHelp                     { int32 MouseX, MouseY; };
struct  FS_BuildContextMenu                     { int32 MouseX, MouseY; FObject Menu, ContextOwner; FRender *Render; };

#define FA_Area_PublicData                      (FR_Area_ATTR +  0)
#define FA_Left                                 (FR_Area_ATTR +  1)
#define FA_Top                                  (FR_Area_ATTR +  2)
#define FA_Width                                (FR_Area_ATTR +  3)
#define FA_Height                               (FR_Area_ATTR +  4)
#define FA_Right                                (FR_Area_ATTR +  5)
#define FA_Bottom                               (FR_Area_ATTR +  6)
#define FA_MinWidth                             (FR_Area_ATTR +  7)
#define FA_MinHeight                            (FR_Area_ATTR +  8)
#define FA_MaxWidth                             (FR_Area_ATTR +  9)
#define FA_MaxHeight                            (FR_Area_ATTR + 10)
#define FA_FixWidth                             (FR_Area_ATTR + 11)
#define FA_FixHeight                            (FR_Area_ATTR + 12)
#define FA_SetMin                               (FR_Area_ATTR + 13)
#define FA_SetMax                               (FR_Area_ATTR + 14)
#define FA_Weight                               (FR_Area_ATTR + 15)
#define FA_Active                               (FR_Area_ATTR + 16)
#define FA_Selected                             (FR_Area_ATTR + 17)
#define FA_Pressed                              (FR_Area_ATTR + 18)
#define FA_Hidden                               (FR_Area_ATTR + 19)
#define FA_Disabled                             (FR_Area_ATTR + 20)
#define FA_Draggable                            (FR_Area_ATTR + 21)
#define FA_Dropable                             (FR_Area_ATTR + 22)
#define FA_Horizontal                           (FR_Area_ATTR + 23)
#define FA_Timer                                (FR_Area_ATTR + 24)
#define FA_InputMode                            (FR_Area_ATTR + 25)
#define FA_ControlChar                          (FR_Area_ATTR + 26)
#define FA_ChainToCycle                         (FR_Area_ATTR + 27)
#define FA_Font                                 (FR_Area_ATTR + 28)
#define FA_NoFill                               (FR_Area_ATTR + 29)
#define FA_WindowObject                         (FR_Area_ATTR + 30)
#define FA_Bufferize                            (FR_Area_ATTR + 31)

enum    {                                       /* FM_DnDReport */

        FV_DnDReport_Timer,
        FV_DnDReport_Motion

        };

enum    {                                       /* FA_InputMode */

        FV_InputMode_None,
        FV_InputMode_Immediate,
        FV_InputMode_Release,
        FV_InputMode_Toggle

        };

enum    {                                       /* FA_SetMin & FA_SetMax */

        FV_SetNone,
        FV_SetBoth,
        FV_SetWidth,
        FV_SetHeight

        };

#if F_CODE_DEPRECATED
enum    {

        FV_RethinkLayout_Save = 1,
        FV_RethinkLayout_Compare

        };

#define FF_RethinkLayout_Modified               (1 << 0)
#endif

#define FF_Hidden_NoNesting                     (1 << 31)
#define FF_Hidden_Check                         (1 << 30)
#define FF_Disabled_NoNesting                   (1 << 31)
#define FF_Disabled_Check                       (1 << 30)

#define FV_Font_Inherit                         -1
#define FV_MAXMAX                               10000

#define FF_Horizontal                           (1 <<  0)
#define FF_Area_SetMinW                         (1 <<  1)
#define FF_Area_SetMaxW                         (1 <<  2)
#define FF_Area_SetMinH                         (1 <<  3)
#define FF_Area_SetMaxH                         (1 <<  4)
#define FF_Area_Selected                        (1 <<  5) // PRIVATE ??
#define FF_Area_Pressed                         (1 <<  6) // PRIVATE ??
#define FF_Area_Active                          (1 <<  7) // PRIVATE ??
#define FF_Area_Disabled                        (1 <<  8)
#define FF_Area_Bufferize                       (1 <<  9)
#define FF_Area_Damaged                         (1 << 10)

#define FF_Area_Group                           (1L << 12) // PRIVATE ?? This flag is set by FC_Group to recognize object as a group (only used by FM_Group_Forward)
#define FF_Area_CanDraw                         (1L << 13) // This object may be drawn
#define FF_Area_CanShow                         (1L << 14) // This object may be shown
#define FF_Area_Setup                           (1L << 15) // ?? USEFULL FM_Setup successful

/* The following flags are reserved */

#define FF_Area_Reserved1                       (1 << 16) // reserved to FC_Group
#define FF_Area_Reserved2                       (1 << 17) // reserved to FC_Group

/* FIXME: is FF_AREA_SETUP useful  ???  To  know  if  setup  succeed  check
_render. Only used by F_Draw(), which also check _render anyway... */

#define FF_Draw_Object                          (1 << 0)
#define FF_Draw_Update                          (1 << 1)
#define FF_Draw_Damaged                         (1 << 2)
#define FF_Draw_Custom_1                        (1L << 24)
#define FF_Draw_Custom_2                        (1L << 25)
#define FF_Draw_Custom_3                        (1L << 26)
#define FF_Draw_Custom_4                        (1L << 27)
#define FF_Draw_Custom_5                        (1L << 28)
#define FF_Draw_Custom_6                        (1L << 29)
#define FF_Draw_Custom_7                        (1L << 30)
#define FF_Draw_Custom_8                        (1L << 31)

#define FF_Erase_Fill                           (1L <<  0) // Force area to be filled even in refresh mode
#define FF_Erase_Region                         (1L <<  1) //
#define FF_Erase_Box                            (1L <<  2) // FRect is in fact a FBox

/*** methods proto macros **************************************************/

#define F_METHOD_SETUP(name)                    F_METHODM(uint32,name,FS_Setup)
#define F_METHOD_CLEANUP(name)                  F_METHOD(void,name)
#define F_METHOD_SHOW(name)                     F_METHOD(uint32,name)
#define F_METHOD_HIDE(name)                     F_METHOD(void,name)
#define F_METHOD_ASKMINMAX(name)                F_METHOD(void,name)
#define F_METHOD_LAYOUT(name)                   F_METHOD(void,name)
#define F_METHOD_DRAW(name)                     F_METHODM(void,name,FS_Draw)
#define F_METHOD_ERASE(name)                    F_METHODM(void,name,FS_Erase)
#define F_METHOD_HANDLEEVENT(name)              F_METHODM(bits32,name,FS_HandleEvent)
#define F_METHOD_GOACTIVE(name)                 F_METHOD(void,name)
#define F_METHOD_GOINTACTIVE(name)              F_METHOD(void,name)
#define F_METHOD_GOENABLED(name)                F_METHOD(void,name)
#define F_METHOD_GODISABLED(name)               F_METHOD(void,name)
#define F_METHOD_DNDQUERY(name)                 F_METHODM(FObject,name,FS_DnDQuery)
#define F_METHOD_DNDBEGIN(name)                 F_METHODM(void,name,FS_DnDBegin)
#define F_METHOD_DNDFINISH(name)                F_METHODM(void,name,FS_DnDFinish)
#define F_METHOD_DNDREPORT(name)                F_METHODM(void,name,FS_DnDReport)
#define F_METHOD_DNDDROP(name)                  F_METHODM(void,name,FS_DnDDrop)

#if F_CODE_DEPRECATED
#define F_METHOD_RETHINKLAYOUT(name)            F_METHODM(bits32,name,FS_RethinkLayout)
#endif

/*** FAreaData *************************************************************/

typedef struct FeelinAreaPublic
{
    FObject                                     Parent;

    FRender                                    *Render;
    bits32                                      Flags;

    FBox                                        Box;
    FInner                                      Inner;
    FMinMax                                     MinMax;

    #if F_CODE_DEPRECATED
    uint32                                     *Pens;
    #else
    FPalette                                   *Palette;
    #endif

    struct TextFont                            *Font;
    uint16                                      Weight;
}
FAreaPublic;

/* the following macros assume  the  variable  'LOD'  to  be  declared  and
holding  a  field  'AreaData'  of type (FAreaData *). If it is not the case
undef '_areadata' and redefine it to your convenience */

#define F_AREA_SAVE_PUBLIC                      LOD -> AreaPublic = (FAreaPublic *) F_Get(Obj,FA_Area_PublicData)

#define _areadata                               LOD -> AreaPublic->
#define _parent                                 (_areadata Parent)
#define _render                                 (_areadata Render)
#define _flags                                  (_areadata Flags)
#define _box                                    (_areadata Box)
#define _x                                      (_box.x)
#define _y                                      (_box.y)
#define _w                                      (_box.w)
#define _h                                      (_box.h)
#define _inner                                  (_areadata Inner)
#define _bl                                     (_inner.l)
#define _bt                                     (_inner.t)
#define _br                                     (_inner.r)
#define _bb                                     (_inner.b)
#define _minmax                                 (_areadata MinMax)
#define _minw                                   (_minmax.MinW)
#define _minh                                   (_minmax.MinH)
#define _maxw                                   (_minmax.MaxW)
#define _maxh                                   (_minmax.MaxH)

#if F_CODE_DEPRECATED
#define _pens                                   (_areadata Pens)
#else
#define _palette                                _areadata Palette
#define _pens                                   _palette->Pens
#endif

#define _font                                   (_areadata Font)
#define _weight                                 (_areadata Weight)

/* extanded coordinates macros */

#define _x2                                     (_x + _w - 1)
#define _y2                                     (_y + _h - 1)
#define _ix                                     (_x + _bl)
#define _iy                                     (_y + _bt)
#define _iw                                     (_w - _bl - _br)
#define _ih                                     (_h - _bt - _bb)
#define _ix2                                    (_x2 - _br)
#define _iy2                                    (_y2 - _bb)

/* macros to access  "_areadata.render".  !!WARNING!!  "_render"  *MUST*  be
checked before accessing flieds */

#define _display                                (_render -> Display)
#define _app                                    (_render -> Application)
#define _win                                    (_render -> Window)
#define _rp                                     (_render -> RPort)
#define _render_flags                           (_render->Flags)

/*** drawing macros ********************************************************/

#define _FPen(fp)                               SetAPen(rp,_pens[fp])
#define _APen(p)                                SetAPen(rp,p)
#define _BPen(p)                                SetBPen(rp,p)
#define _DrMd(m)                                SetDrMd(rp,m)
#define _Plot(x1,y1)                            WritePixel(rp,x1,y1)
#define _Move(x,y)                              Move(rp,x,y)
#define _Draw(x,y)                              Draw(rp,x,y)
#define _Boxf(x1,y1,x2,y2)                      RectFill(rp,x1,y1,x2,y2)
#define _Text(s,c)                              Text(rp,s,c)

/* macros that can be used while creating objects */

#define FC_Area                                 "Area"
#define AreaObject                              F_NewObj(FC_Area

#define InputImmediate                          FA_InputMode,FV_InputMode_Immediate
#define InputRelease                            FA_InputMode,FV_InputMode_Release
#define InputToggle                             FA_InputMode,FV_InputMode_Toggle
#define DontChain                               FA_ChainToCycle,FALSE
#define HorizLayout                             FA_Horizontal,TRUE
//+
///Text                     0x001080
#define FR_Text                                 0x001080
#define FR_Text_MTHD                            (MTHD_BASE | FR_Text)
#define FR_Text_ATTR                            (ATTR_BASE | FR_Text)

#define FA_Text                                 (FR_Text_ATTR +  0)
#define FA_Text_PreParse                        (FR_Text_ATTR +  1)
#define FA_Text_AltPreParse                     (FR_Text_ATTR +  2)
#define FA_Text_HCenter                         (FR_Text_ATTR +  3)
#define FA_Text_VCenter                         (FR_Text_ATTR +  4)
#define FA_Text_Shortcut                        (FR_Text_ATTR +  5)
#define FA_Text_Static                          (FR_Text_ATTR +  6)

#define FC_Text                                 "Text"
#define TextObject                              F_NewObj(FC_Text

#define ButtonBack                              FA_Back,"$button-back"
#define ButtonFrame                             FA_Frame,"$button-frame"
#define TextBack                                FA_Back,"$text-back"
#define TextFrame                               FA_Frame,"$text-frame"
//+
///Group                    0x0010C0
#define FR_Group                                0x0010C0
#define FR_Group_MTHD                           (MTHD_BASE | FR_Group)
#define FR_Group_ATTR                           (ATTR_BASE | FR_Group)

#define FM_Group_Forward                        (FR_Group_MTHD +  0)
#define FM_Group_InitChange                     (FR_Group_MTHD +  1)
#define FM_Group_ExitChange                     (FR_Group_MTHD +  2)

#define FA_Group_HSpacing                       (FR_Group_ATTR +  0)
#define FA_Group_VSpacing                       (FR_Group_ATTR +  1)
#define FA_Group_SameSize                       (FR_Group_ATTR +  2)
#define FA_Group_SameWidth                      (FR_Group_ATTR +  3)
#define FA_Group_SameHeight                     (FR_Group_ATTR +  4)
#define FA_Group_RelSizing                      (FR_Group_ATTR +  5)
#define FA_Group_PageMode                       (FR_Group_ATTR +  6)
#define FA_Group_PageFont                       (FR_Group_ATTR +  7)
#define FA_Group_PageTitle                      (FR_Group_ATTR +  8)
#define FA_Group_PageAPreParse                  (FR_Group_ATTR +  9)
#define FA_Group_PageIPreParse                  (FR_Group_ATTR + 10)
#define FA_Group_ActivePage                     (FR_Group_ATTR + 11)
#define FA_Group_Rows                           (FR_Group_ATTR + 12)
#define FA_Group_Columns                        (FR_Group_ATTR + 13)
#define FA_Group_LayoutHook                     (FR_Group_ATTR + 14)
#define FA_Group_MinMaxHook                     (FR_Group_ATTR + 15)
#define FA_Group_Forward                        (FR_Group_ATTR + 16)

#define FA_Group_BufferRegion                   (FR_Group_ATTR + 20) // PRIVATE

enum    {

        FV_Group_ActivePage_Prev = -3,
        FV_Group_ActivePage_Next,
        FV_Group_ActivePage_Last,
        FV_Group_ActivePage_First

        };

/*** types *****************************************************************/

typedef struct FeelinAreaNode
{
    struct FeelinAreaNode          *Next;
    struct FeelinAreaNode          *Prev;
    FObject                         Object;
//  end of FFamilyNode header
    FAreaPublic                    *AreaPublic;
}
FAreaNode;

/*** macros ****************************************************************/

#define FC_Group                                "Group"
#define GroupObject                             F_NewObj(FC_Group
#define VGroup                                  F_NewObj(FC_Group
#define HGroup                                  F_NewObj(FC_Group,FA_Horizontal,TRUE
#define RowGroup(x)                             F_NewObj(FC_Group,FA_Group_Rows,x
#define ColGroup(x)                             F_NewObj(FC_Group,FA_Group_Columns,x
#define Page                                    F_NewObj(FC_Group,FA_Group_PageMode,TRUE
//+

/****************************************************************************
*** Dynamic Classes *********************************************************
****************************************************************************/

#define FC_Adjust                               "Adjust"
#define FC_AdjustBrush                          "AdjustBrush"
#define FC_AdjustColor                          "AdjustColor"
#define FC_AdjustFrame                          "AdjustFrame"
#define FC_AdjustGradient                       "AdjustGradient"
#define FC_AdjustImage                          "AdjustImage"
#define FC_AdjustRaster                         "AdjustRaster"
#define FC_AdjustPen                            "AdjustPen"
#define FC_AdjustPreParse                       "AdjustPreParse"
#define FC_AdjustRGB                            "AdjustRGB"
#define FC_AdjustScheme                         "AdjustScheme"
#define FC_AdjustSchemeEntry                    "AdjustSchemeEntry"
#define FC_AdjustSpacing                        "AdjustSpacing"
#define FC_Balance                              "Balance"
#define FC_Bar                                  "Bar"
#define FC_BitMap                               "BitMap"
#define FC_Cycle                                "Cycle"
#define FC_Dataspace                            "Dataspace"
#define FC_Decorator                            "Decorator"
#define FC_Dialog                               "Dialog"
#define FC_DOSList                              "DOSList"
#define FC_DOSNotify                            "DOSNotify"
#define FC_Gauge                                "Gauge"
#define FC_FileChooser                          "FileChooser"
#define FC_FontChooser                          "FontChooser"
#define FC_FontDialog                           "FontDialog"
#define FC_Frame                                "Frame"
#define FC_Histogram                            "Histogram"
#define FC_Image                                "Image"
#define FC_List                                 "List"
#define FC_Listview                             "Listview"
#define FC_ModulesList                          "ModulesList"
#define FC_Numeric                              "Numeric"
#define FC_Palette                              "Palette"
#define FC_Picture                              "Picture"
#define FC_PopColor                             "PopColor"
#define FC_PopFile                              "PopFile"
#define FC_PopFont                              "PopFont"
#define FC_PopFrame                             "PopFrame"
#define FC_PopHelp                              "PopHelp"
#define FC_PopImage                             "PopImage"
#define FC_PopScheme                            "PopScheme"
#define FC_Preview                              "Preview"
#define FC_PreviewColor                         "PreviewColor"
#define FC_PreviewGradient                      "PreviewGradient"
#define FC_PreviewFrame                         "PreviewFrame"
#define FC_PreviewImage                         "PreviewImage"
#define FC_PreviewScheme                        "PreviewScheme"
#define FC_Prop                                 "Prop"
#define FC_Radio                                "Radio"
#define FC_Render                               "Render"
#define FC_Scrollbar                            "Scrollbar"
#define FC_Slider                               "Slider"
#define FC_String                               "String"
#define FC_TextDisplay                          "TextDisplay"
#define FC_Thread                               "Thread"

#define AdjustObject                            F_NewObj(FC_Adjust
#define AdjustBrushObject                       F_NewObj(FC_AdjustBrush
#define AdjustColorObject                       F_NewObj(FC_AdjustColor
#define AdjustFrameObject                       F_NewObj(FC_AdjustFrame
#define AdjustGradientObject                    F_NewObj(FC_AdjustGradient
#define AdjustImageObject                       F_NewObj(FC_AdjustImage
#define AdjustPenObject                         F_NewObj(FC_AdjustPen
#define AdjustPreParseObject                    F_NewObj(FC_AdjustPreParse
#define AdjustRasterObject                      F_NewObj(FC_AdjustRaster
#define AdjustRGBObject                         F_NewObj(FC_AdjustRGB
#define AdjustSchemeObject                      F_NewObj(FC_AdjustScheme
#define AdjustSchemeEntryObject                 F_NewObj(FC_AdjustSchemeEntry
#define AdjustSpacingObject                     F_NewObj(FC_AdjustSpacing
#define BalanceObject                           F_NewObj(FC_Balance
#define BalanceID(id)                           F_NewObj(FC_Balance,FA_ID,id,TAG_DONE)
#define BarObject                               F_NewObj(FC_Bar
#define BitMapObject                            F_NewObj(FC_BitMap
#define CycleObject                             F_NewObj(FC_Cycle
#define DataspaceObject                         F_NewObj(FC_Dataspace
#define DecoratorObject                         F_NewObj(FC_Decorator
#define DialogObject                            F_NewObj(FC_Dialog
#define DOSListObject                           F_NewObj(FC_DOSList
#define DOSNotifyObject                         F_NewObj(FC_DOSNotify
#define FileChooserObject                       F_NewObj(FC_FileChooser
#define FontChooserObject                       F_NewObj(FC_FontChooser
#define FontDialogObject                        F_NewObj(FC_FontDialog
#define GaugeObject                             F_NewObj(FC_Gauge
#define HistogramObject                         F_NewObj(FC_Histogram
#define ImageObject                             F_NewObj(FC_Image
#define ListObject                              F_NewObj(FC_List
#define ListviewObject                          F_NewObj(FC_Listview
#define ModulesListObject                       F_NewObj(FC_ModulesList
#define NumericObject                           F_NewObj(FC_Numeric
#define PaletteObject                           F_NewObj(FC_Palette
#define PictureObject                           F_NewObj(FC_Picture
#define PopColorObject                          F_NewObj(FC_PopColor
#define PopFileObject                           F_NewObj(FC_PopFile
#define PopFontObject                           F_NewObj(FC_PopFont
#define PopFrameObject                          F_NewObj(FC_PopFrame
#define PopHelpObject                           F_NewObj(FC_PopHelp
#define PopImageObject                          F_NewObj(FC_PopImage
#define PopSchemeObject                         F_NewObj(FC_PopScheme
#define PreviewObject                           F_NewObj(FC_Preview
#define PreviewColorObject                      F_NewObj(FC_PreviewColor
#define PreviewGradientObject                   F_NewObj(FC_PreviewGradient
#define PreviewFrameObject                      F_NewObj(FC_PreviewFrame
#define PreviewImageObject                      F_NewObj(FC_PreviewImage
#define PreviewSchemeObject                     F_NewObj(FC_PreviewScheme
#define PropObject                              F_NewObj(FC_Prop
#define RadioObject                             F_NewObj(FC_Radio
#define ScrollbarObject                         F_NewObj(FC_Scrollbar
#define SliderObject                            F_NewObj(FC_Slider
#define StringObject                            F_NewObj(FC_String
#define ThreadObject                            F_NewObj(FC_Thread

/*** Types *****************************************************************/

typedef struct FeelinListDisplay
{
    STRPTR                          String;
    STRPTR                          PreParse;
}
FListDisplay;

typedef struct FeelinModuleCreated
{
    struct FeelinModuleCreated     *Next;
    struct FeelinModuleCreated     *Prev;
    struct Library                 *Module;
    FClass                         *Meta;
    FClass                         *Class;
}
FModuleCreated;

/*** Method Messages *******************************************************/

struct  FS_Adjust_ToString                      { STRPTR Spec; int32 Notify; uint32 UserData; };
struct  FS_Adjust_ToObject                      { STRPTR Spec; FObject XMLDocument; };
struct  FS_Adjust_Query                         { STRPTR Spec; };
struct  FS_Adjust_ParseXML                      { STRPTR Source; uint32 SourceType; FDOCID *IDs; uint32 MarkupID; FObject *XMLDocument; uint32 *id_Resolve; };
struct  FS_BitMap_Blit                          { struct RastPort *rp; uint32 SourceX, SourceY; FBox *TargetBox; uint32 Tag1; /*...*/ };
struct  FS_Dataspace_Add                        { uint32 ID; APTR Data; uint32 Size; };
struct  FS_Dataspace_Remove                     { uint32 ID; };
struct  FS_Dataspace_Find                       { uint32 ID; };
struct  FS_Dataspace_Resolve                    { uint32 ID; uint32 *Save; };
struct  FS_Dataspace_WriteIFF                   { struct IFFHandle *IFF; uint32 Type; uint32 ID; };
struct  FS_Dataspace_ReadIFF                    { struct IFFHandle *IFF; };
struct  FS_List_Construct                       { APTR Entry; APTR Pool; };
struct  FS_List_Destruct                        { APTR Entry; APTR Pool; };
struct  FS_List_Display                         { APTR Entry; STRPTR *Strings; STRPTR *PreParses; };
struct  FS_List_Compare                         { APTR Entry; APTR Other; int32 Type1; int32 Type2; };
struct  FS_List_Insert                          { APTR *Entries; int32 Count; int32 Pos; };
struct  FS_List_InsertSingle                    { APTR Entry; int32 Pos; };
struct  FS_List_GetEntry                        { int32 Position; };
struct  FS_List_Remove                          { int32 Position; };
struct  FS_List_FindString                      { STRPTR String; };
struct  FS_ModulesList_Create                   { uint32 Which; };
struct  FS_ModulesList_Delete                   { uint32 Which; };
struct  FS_Numeric_Increase                     { int32 Value; };
struct  FS_Numeric_Decrease                     { int32 Value; };
struct  FS_Numeric_Stringify                    { int32 Value; };
struct  FS_Picture_Draw                         { FRender *Render; FRect *Rect; bits32 Flags; };
struct  FS_Prop_Decrease                        { int32 Value; };
struct  FS_Prop_Increase                        { int32 Value; };
struct  FS_Preview_Query                        { STRPTR Spec; };
struct  FS_Preview_ParseXML                     { STRPTR Source; uint32 SourceType; FDOCID *IDs; uint32 MarkupID; FObject *XMLDocument; uint32 *id_Resolve; };
struct  FS_Preview_ToString                     { STRPTR Spec; };
struct  FS_Preview_Adjust                       { STRPTR WindowTitle; STRPTR AdjustClass; /* additionnal attribute to create the adjust object */ };
struct  FS_Thread_Send                          { uint32 Cmd; };

/*** Values ****************************************************************/

#define FV_Adjust_Separator         '\n'

enum    {

        FV_Image_Normal = 0,
        FV_Image_Selected,
        FV_Image_Enabled,
        FV_Image_Disabled

        };

enum    {

        FV_Cycle_Active_Prev = -3,
        FV_Cycle_Active_Next,
        FV_Cycle_Active_Last,
        FV_Cycle_Active_First

        };

/*

YOMGUI: bon alors petite inversion de String et Array. Pourquoi: c'est pour
+  de  sécurité. En effet, j'ai vu que dans tes code d'exemple tu considére
Array par défaut (donc tu ne le mets pas). Soit... mais si il  advient  que
le  programmeur  se  plante et nous met une chaine de caractères à la place
d'un Tableau... bah cela te fait plein de  HIT  et  risque  de  crasher  la
machine même!

Alors qu'ici, si il y a erreur (donc un array à la place d'un  string),  le
code  de Cycle, va afficher n'importe quoi certe, mais ne plantera pas. Car
il y a forcement un '\0'  tôt  où  tard,  même  dans  un  Array.  Ainsi  le
programmeur  va voir qu'il s'est planté quand il va lancer le programme (ça
se voit de suite un cycle avec des codes de contrôle ! :)) mais  il  n'aura
pas à redemarré sa machine, car pas de hit... c'est pas plus beau :)

Le code de cycle a été modifié pour... ... reste à vérifier tous les  codes
utilisant le cyle.

GOFROMIEL: Très bonne idée ! Les  '  =  0'  c'est  inutile,  mais  tu  peux
constater que je les ai mis pour te faire plaisir :-)

*/

enum    {

        FV_Cycle_EntriesType_String = 0,
        FV_Cycle_EntriesType_Array

        };

enum    {

        FV_Cycle_Layout_Right = 0,
        FV_Cycle_Layout_Left

        };

enum    {

        FV_Dialog_Response_None = 0,
        FV_Dialog_Response_Ok,
        FV_Dialog_Response_Cancel,
        FV_Dialog_Response_Apply,
        FV_Dialog_Response_Yes,
        FV_Dialog_Response_No,
        FV_Dialog_Response_All,
        FV_Dialog_Response_Save,
        FV_Dialog_Response_Use

        };

enum    {

        FV_Dialog_Buttons_None = 0,
        FV_Dialog_Buttons_Ok,
        FV_Dialog_Buttons_Boolean,
        FV_Dialog_Buttons_Confirm,
        FV_Dialog_Buttons_Always,
        FV_Dialog_Buttons_Preference,
        FV_Dialog_Buttons_PreferenceTest

        };

#define FV_List_Hook_String         -1
#define FV_List_NotVisible          -1

enum    {

        FV_List_Active_PageDown = -7,
        FV_List_Active_PageUp,
        FV_List_Active_Down,
        FV_List_Active_Up,
        FV_List_Active_Bottom,
        FV_List_Active_Top,
        FV_List_Active_None

        };

enum    {

        FV_List_Activation_Unknown = 0,
        FV_List_Activation_Click,
        FV_List_Activation_DoubleClick,
        FV_List_Activation_Key,
        FV_List_Activation_External

        };

enum    {

        FV_List_Insert_Bottom = -3,
        FV_List_Insert_Sorted,
        FV_List_Insert_Active,
        FV_List_Insert_Top

        };

enum    {

        FV_List_Remove_First = -9,
        FV_List_Remove_Last,
        FV_List_Remove_Prev,
        FV_List_Remove_Next,
        FV_List_Remove_Active,
        FV_List_Remove_Others,
        FV_List_Remove_ToBottom,
        FV_List_Remove_FromTop,
        FV_List_Remove_All

        };

enum    {

        FV_List_GetEntry_First = -5,
        FV_List_GetEntry_Last,
        FV_List_GetEntry_Prev,
        FV_List_GetEntry_Next,
        FV_List_GetEntry_Active


        };

enum    {

        FV_List_Sort_Ascending = -1,
        FV_List_Sort_None,
        FV_List_Sort_Descending

        };

#define FV_List_FindString_NotFound             -1


enum    {

        FV_Preference_ENV = -3,
        FV_Preference_ENVARC,
        FV_Preference_BOTH

        };

enum    {

        FV_Radio_EntriesType_Strings = 0,
        FV_Radio_EntriesType_Objects

        };

enum    {

        FV_Scrollbar_Type_Bottom = 0,
        FV_Scrollbar_Type_Sym,
        FV_Scrollbar_Type_Top

        };

enum    {

        FV_String_Decimal = -2,
        FV_String_Hexadecimal,

        FV_String_Left,
        FV_String_Center,
        FV_String_Right,

        };

enum    {

        FV_Thread_Reserved1 = -4,
        FV_Thread_Reserved2,
        FV_Thread_Hello,
        FV_Thread_Bye,
        FV_Thread_Dummy = 1

        };

/*** flags *****************************************************************/

#define FF_BitMap_Blit_Tile                     (1 << 0)


#ifdef __MORPHOS__
#pragma pack()
#endif /* __MORPHOS__ */

#endif
