#ifndef BGUI_BGUI_H
#define BGUI_BGUI_H
/*
 * @(#) $Header$
 *
 * $VER: bgui/bgui.h 41.10 (26.4.96)
 * bgui.library structures and constants.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.3  2003/01/18 19:10:21  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.2  2000/07/08 20:14:06  stegerg
 * fixed the BITOFFSET_OF macro which did not work on little endian machines.
 *
 * Revision 42.1  2000/07/07 17:14:52  stegerg
 * STACK???? stuff in method structures.
 *
 * Revision 42.0  2000/05/09 22:23:00  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:36  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.12  2000/05/04 04:41:58  mlemos
 * Added the definition of the identifiers for the Bar and Layout group
 * classes.
 *
 * Revision 41.10.2.11  1999/08/29 20:31:34  mlemos
 * Added back the definitions for methods GRM_ADDMEMBER, GRM_REMMEMBER,
 * GRM_DIMENSIONS, GRM_ADDSPACEMEMBER, GRM_INSERTMEMBER, GRM_REPLACEMEMBER.
 * Added the definitions for the method BASE_RELAYOUT and the tag LGO_Relayout.
 * Removed the definitions of the methods that were never implemented:
 * GROUPM_ADDMEMBER, GROUPM_REMMEMBER, GROUPM_INSERTMEMBER
 * GROUPM_REPLACEMEMBER, GROUPM_ADDSPACEMEMBER.
 *
 * Revision 41.10.2.10  1999/08/13 04:36:22  mlemos
 * Changed the pre-processor define to denote that this file was included.
 *
 * Revision 41.10.2.9  1999/08/10 22:38:48  mlemos
 * Remove the definition of the ISEQ_JUSTIFY sequence that was never
 * implemented.
 *
 * Revision 41.10.2.8  1999/07/23 19:47:07  mlemos
 * Added the definition of command sequences to set the draw mode to JAM1,
 * JAM2 or COMPLEMENT.
 *
 * Revision 41.10.2.7  1999/07/17 23:01:47  mlemos
 * Corrected the documentation of the applicability of the attribute
 * PMB_MenuEntries.
 *
 * Revision 41.10.2.6  1999/05/31 00:49:20  mlemos
 * Added the definition of the ID of the TreeView gadget.
 *
 * Revision 41.10.2.5  1998/12/17 04:16:31  mlemos
 * Fixed badly formed comments.
 *
 * Revision 41.10.2.4  1998/12/09 17:34:17  mlemos
 * Added missing log description of the previous revision.
 * Added the definitions of the new text formatting sequences ISEQ_SHADOWED,
 * ISEQ_U2, ISEQ_HU, ISEQ_JUSTIFY, ISEQ_KEEP, ISEQ_WRAP .
 *
 * Revision 41.10.2.3  1998/12/09 17:29:41  mlemos
 *
 * Revision 41.10.2.2  1998/10/12 01:39:09  mlemos
 * Added the inclusion of the bgui_arexx.h include.
 * Added the definition of the tags for BGUI_MakeClass to define class methods.
 *
 * Revision 41.10.2.1  1998/02/26 18:00:27  mlemos
 * Changed C++ comments to standard C comments.
 * Added LGO_Object as publically known group node object attribute.
 *
 * Revision 41.10  1998/02/25 21:13:40  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:15:41  mlemos
 * Ian sources
 *
 *
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif /* INTUITION_CLASSES_H */

#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif /* INTUITION_CLASSUSR_H */

#ifndef INTUITION_GADGETCLASS_H
#include <intuition/gadgetclass.h>
#endif /* INTUITION_GADGETCLASS_H */

#ifndef INTUITION_CGHOOKS_H
#include <intuition/cghooks.h>
#endif /* INTUITION_CGHOOKS_H */

#ifndef LIBRARIES_GADTOOLS_H
#include <libraries/gadtools.h>
#endif /* LIBRARIES_GADTOOLS_H */

#ifndef BGUI_IMAGE_H
#include <bgui/bgui_image.h>
#endif /* BGUI_IMAGE_H */

#ifndef NO_OLD_BGUI
#include <bgui/bgui_asl.h>
#include <bgui/bgui_arexx.h>
#include <bgui/bgui_cx.h>
#include <bgui/bgui_obsolete.h>
#endif /* NO_OLD_BGUI */

/*****************************************************************************
 *
 *      The attribute definitions in this header are all followed by
 *      a small comment. This comment can contain the following things:
 *
 *      I - Attribute can be set with OM_NEW
 *      S - Attribute can be set with OM_SET
 *      G - Attribute can be read with OM_GET
 *      N - Setting this attribute triggers a notification.
 *      U - Attribute can be set with OM_UPDATE.
 */

/*****************************************************************************
 *
 *      Miscellanious library definitions.
 */
#define BGUINAME                        "bgui.library"
#define BGUI_MINIMUM                    37
#define BGUIVERSION                     41

/*****************************************************************************
 *
 *      BGUI_GetClassPtr() and BGUI_NewObjectA() class ID's.
 */
#define BGUI_LABEL_IMAGE                (0L)
#define BGUI_FRAME_IMAGE                (1L)
#define BGUI_VECTOR_IMAGE               (2L)
#define BGUI_SYSTEM_IMAGE               (3L)
/* 4 through 10 reserved. */
#define BGUI_BASE_GADGET                (11L)
#define BGUI_GROUP_GADGET               (12L)
#define BGUI_BUTTON_GADGET              (13L)
#define BGUI_CYCLE_GADGET               (14L)
#define BGUI_CHECKBOX_GADGET            (15L)
#define BGUI_INFO_GADGET                (16L)
#define BGUI_STRING_GADGET              (17L)
#define BGUI_PROP_GADGET                (18L)
#define BGUI_INDICATOR_GADGET           (19L)
#define BGUI_VIEW_GADGET                (20L)
#define BGUI_PROGRESS_GADGET            (21L)
#define BGUI_SLIDER_GADGET              (22L)
#define BGUI_LISTVIEW_GADGET            (23L)
#define BGUI_MX_GADGET                  (24L)
#define BGUI_PAGE_GADGET                (25L)
#define BGUI_EXTERNAL_GADGET            (26L)
#define BGUI_SEPARATOR_GADGET           (27L)
#define BGUI_AREA_GADGET                (28L)
#define BGUI_RADIOBUTTON_GADGET         (29L)
#define BGUI_PALETTE_GADGET             (30L)
#define BGUI_POPBUTTON_GADGET           (31L)
#define BGUI_TREEVIEW_GADGET            (32L)
#define BGUI_BAR_GADGET                 (33L)
#define BGUI_LAYOUTGROUP_GADGET         (34L)
/* 35 through 39 reserved. */
#define BGUI_WINDOW_OBJECT              (40L)
#define BGUI_FILEREQ_OBJECT             (41L)
#define BGUI_COMMODITY_OBJECT           (42L)
#define BGUI_ASLREQ_OBJECT              (43L)
#define BGUI_FONTREQ_OBJECT             (44L)
#define BGUI_SCREENREQ_OBJECT           (45L)
#define BGUI_AREXX_OBJECT               (46L)
/* 47 through 79 reserved. */
#define BGUI_TEXT_GRAPHIC               (80L)
#define BGUI_FONT_GRAPHIC               (81L)
#define BGUI_PEN_GRAPHIC                (82L)
/* 83 through 99 reserved. */
#define BGUI_ROOT_OBJECT                (100L)
#define BGUI_GADGET_OBJECT              (101L)
#define BGUI_IMAGE_OBJECT               (102L)
/* 102 through 149 reserved. */
#define BGUI_GROUP_NODE                 (150L)
#define BGUI_PAGE_NODE                  (151L)
#define BGUI_LISTVIEW_NODE              (152L)

/* Typo */
#define BGUI_SEPERATOR_GADGET           BGUI_SEPARATOR_GADGET

/*****************************************************************************
 *
 *      BGUI requester definitions.
 */
struct bguiRequest {
        ULONG            br_Flags;              /* See below.               */
        STRPTR           br_Title;              /* Requester title.         */
        STRPTR           br_GadgetFormat;       /* Gadget labels.           */
        STRPTR           br_TextFormat;         /* Body text format.        */
        UWORD            br_ReqPos;             /* Requester position.      */
        struct TextAttr *br_TextAttr;           /* Requester font.          */
        UBYTE            br_Underscore;         /* Underscore indicator.    */
        UBYTE            br_Reserved0[3];       /* Set to 0!                */
        struct Screen   *br_Screen;             /* Optional screen pointer. */
        ULONG            br_Reserved1[4];       /* Set to 0!                */
};

#define BREQF_CENTERWINDOW      (1<<0) /* Center requester on the window.   */
#define BREQF_LOCKWINDOW        (1<<1) /* Lock the parent window.           */
#define BREQF_NO_PATTERN        (1<<2) /* Don't use back-fill pattern.      */
#define BREQF_XEN_BUTTONS       (1<<3) /* Use XEN style buttons.            */
#define BREQF_AUTO_ASPECT       (1<<4) /* Aspect-ratio dependant look.      */
#define BREQF_FAST_KEYS         (1<<5) /* Return/Esc hotkeys.               */
#define BREQF_FUZZ_BUTTONS      (1<<6) /* Use fuzz style buttons.           */

/*****************************************************************************
 *
 *      BGUI localization definitions.
 */
struct bguiLocale {
        struct Locale   *bl_Locale;             /* Locale to use.               */
        struct Catalog  *bl_Catalog;            /* Catalog to use.              */
        struct Hook     *bl_LocaleStrHook;      /* Localization function.       */
        struct Hook     *bl_CatalogStrHook;     /* Localization function.       */
        APTR             bl_UserData;           /* For application use.         */
};

struct bguiLocaleStr {
        LONG             bls_ID;                /* ID of locale string.         */
};

struct bguiCatalogStr {
        LONG             bcs_ID;                /* ID of locale string.         */
        STRPTR           bcs_DefaultString;     /* Default string for this ID.  */
};








        
/*****************************************************************************
 *
 *      Tag and method bases.
 *
 *      Range 0x800F0000 - 0x800FFFFF is reserved for BGUI tags.
 *      Range 0x80020000 - 0x8002FFFF is reserved for imageclass tags.
 *      Range 0x80030000 - 0x8003FFFF is reserved for gadgetclass tags.
 *      Range 0x80040000 - 0x8004FFFF is reserved for icclass tags.
 *      Range 0x80080000 - 0x8008FFFF is reserved for gadtools and asl tags.
 *
 *      Range 0x000F0000 - 0x000FFFFF is reserved for BGUI methods.
 *      Range 0x00000001 - 0x0000FFFF is reserved for BOOPSI methods.
 *
 *      For custom classes, keep away from these ranges.  Values greater than
 *      0x80100000 for tags and 0x00100000 for methods are suggested.
 */
#define BGUI_TB                         (TAG_USER|0xF0000)
#define BGUI_MB                         (0xF0000)


/*****************************************************************************
 *
 *      BGUI graphics definitions.
 */
#define SUBSHINE   (-1)
#define SUBSHADOW  (-2)

#define NUMBGUIPENS 2

/*
 * The BaseInfo structure contains information needed for many BGUI methods.
 * It is backwards compatible with a V40 GadgetInfo.  Within a BGUI context,
 * you should only use bi_Screen, bi_RPort, bi_DrInfo, and bi_Pens.
 *
 * This structure may be passed to a method that requires a GadgetInfo, but you
 * MUST NOT pass a plain GadgetInfo to any function requiring a BaseInfo.  Do
 * not attempt to make one of these yourself; use AllocBaseInfo().
 */
struct BaseInfo
{
        struct Screen     *bi_IScreen;          /* Intuition Screen we're on.   */
        struct Window     *bi_IWindow;          /* Intuition Window we're in.   */
        struct Requester  *bi_IRequester;       /* Intuition Requester we're in.*/
        struct RastPort   *bi_RPort;            /* RastPort to render into.     */
        struct Layer      *bi_Layer;            /* Layer of the rastport.       */
        struct IBox        bi_Domain;           /* Domain of the gadget.        */
        UBYTE              bi_WindowPens[2];    /* Detail/BlockPen of window.   */
        struct DrawInfo   *bi_DrInfo;           /* DrawInfo of this screen.     */
        ULONG              bi_GIReserved[6];    /* For compatibility.           */
        /*
         * The following are extensions to GadgetInfo.
         */
        UWORD              bi_Version;          /* For expansion.               */
        Object            *bi_Screen;           /* Screen object we're in.      */
        Object            *bi_Window;           /* Window object we're in.      */
        Object            *bi_Requester;        /* Requester object we're in.   */
        UWORD             *bi_Pens;             /* Drawing pens (BGUI).         */
        ULONG              bi_RenderFlags;      /* See below.                   */
};

/*
 * The BaseInfo will grow,  Check the version to see what fields/flags are present.
 */
#define BINFO_VER_41_10    1                    /* Initial version.             */

/*
 * Render flags describe the look and feel rendering should use.
 */
#define BIRF_BUFFER        (1<<0)               /* Rendering is buffered.       */




struct bguiPattern {
        ULONG            bp_Flags;              /* Flags (see below).           */
        UWORD            bp_Left,  bp_Top;      /* Offset into bitmap.          */
        UWORD            bp_Width, bp_Height;   /* Size of cut from bitmap.     */
        struct BitMap   *bp_BitMap;             /* Pattern bitmap.              */
        Object          *bp_Object;             /* Datatype object.             */
};

#define BPF_RELATIVE_ORIGIN     (1<<0)          /* Origin relative to box.      */



#define TEXTA_Text                      (BGUI_TB+40001)    /* ISG-- */
#define TEXTA_TextID                    (BGUI_TB+40002)    /* ISG-- */
#define TEXTA_CopyText                  (BGUI_TB+40003)    /* ISG-- */
#define TEXTA_Args                      (BGUI_TB+40004)    /* ISG-- */

#define TEXTM_RENDER                    (BGUI_MB+40001)

/* Render a text graphic. */

#ifndef __AROS__

#undef STACKULONG
#define STACKULONG ULONG

#undef STACKLONG
#define STACKLONG LONG

#undef STACKUWORD
#define STACKUWORD UWORD

#undef STACKWORD
#define STACKWORD WORD
 
#endif

struct tmRender {
        STACKULONG                   MethodID;       /* TEXTM_RENDER                 */
        struct BaseInfo        *tmr_BInfo;      /* RastPort to render into.     */
        struct IBox            *tmr_Bounds;     /* Bounds to fit in.            */
};

#define TEXTM_DIMENSIONS                (BGUI_MB+40002)

/* Render a text graphic. */

struct tmDimensions {
        STACKULONG                   MethodID;       /* TEXTM_DIMENSIONS             */
        struct RastPort        *tmd_RPort;      /* RastPort for computations.   */
        struct {
                UWORD          *Width;          /* Storage width in pixels      */
                UWORD          *Height;         /* Storage height in pixels     */
        }                       tmd_Extent;
};


/*****************************************************************************
 *
 *      Class implementor information.
 */

typedef ULONG  (*FUNCPTR)();

typedef struct DispatcherFunction {
   STACKULONG       df_MethodID;
   FUNCPTR     df_Func;
}  DPFUNC;

#define DF_END (~0)

/* For use with the BGUI_MakeClass() call. */

#define CLASS_SuperClass                (BGUI_TB+10001)
#define CLASS_SuperClassID              (BGUI_TB+10002)
#define CLASS_SuperClassBGUI            (BGUI_TB+10003)
#define CLASS_ClassID                   (BGUI_TB+10004)
#define CLASS_ClassSize                 (BGUI_TB+10005)
#define CLASS_ObjectSize                (BGUI_TB+10006)
#define CLASS_Flags                     (BGUI_TB+10007)
#define CLASS_Dispatcher                (BGUI_TB+10008)
#define CLASS_DFTable                   (BGUI_TB+10009)
#define CLASS_ClassDispatcher           (BGUI_TB+10010)
#define CLASS_ClassDFTable              (BGUI_TB+10011)

struct BGUIClassBase
{
   struct Library  bcb_Library;
   Class          *bcb_Class;
};


/*****************************************************************************
 *
 *      "rootclass" - BOOPSI rootclass replacement for BGUI.
 *
 *      Tags:    negative
 *
 *      Methods: negative
 *
 *      This is a rootclass replacement that augments the rootclass with
 *      notification, more advanced attribute control, and more.
 *      It is actually derived from the intuition rootclass, so any future
 *      rootclass enhancements will work.
 */

#define RA_NewSetHighestClass           (BGUI_TB+30001)    /* --G-- */  /* V41.9 */

/* New methods */
#define RM_SET                          (BGUI_MB+30001)
#define RM_SETM                         (BGUI_MB+30002)
#define RM_SETCUSTOM                    (BGUI_MB+30003)
#define RM_GET                          (BGUI_MB+30011)
#define RM_GETM                         (BGUI_MB+30012)
#define RM_GETCUSTOM                    (BGUI_MB+30013)
#define RM_REFRESH                      (BGUI_MB+30021)
#define RM_GETATTRFLAGS                 (BGUI_MB+30022)

/* Set or get attribute(s). */
struct rmAttr {
        STACKULONG                   MethodID;
        struct TagItem         *ra_Attr;
        STACKULONG                   ra_Flags;
};

/* Set or get attribute(s). */
struct rmRefresh {
        STACKULONG                   MethodID;
        APTR                    rr_Context;
        STACKULONG                   rr_Flags;
};

/*
 * The following flags are context specific (and semi-private).
 */
#define RAF_SUPER              (1<<0)  /* Pass to the superclass too.    */
#define RAF_NOTIFY             (1<<1)  /* Send out a notification.       */
#define RAF_REDRAW             (1<<2)  /* Gadget must be redrawn.        */
#define RAF_RESIZE             (1<<3)  /* Gadget may have changed size.  */
#define RAF_CUSTOM             (1<<10) /* Custom RM_SET processing.      */
#define RAF_INITIAL            (1<<11) /* Prevent redundancy in OM_NEW.  */
#define RAF_NOGET              (1<<12) /* Get is not allowed.            */
#define RAF_NOSET              (1<<13) /* Set is not allowed.            */
#define RAF_NOUPDATE           (1<<14) /* Update is not allowed.         */
#define RAF_NOINTERIM          (1<<15) /* Interim update is not allowed. */
#define RAF_UNDERSTOOD         (1<<16) /* Tag was understood.            */
#define RAF_UPDATE             (1<<17) /* This is an update message.     */
#define RAF_INTERIM            (1<<18) /* This is an interim update.     */
#define RAF_BYTE               (0<<28) /* Size is one byte.              */
#define RAF_WORD               (1<<28) /* Size is one word.              */
#define RAF_LONG               (2<<28) /* Size is one long.              */
#define RAF_ADDR               (3<<28) /* Return address of field.       */
#define RAF_BOOL               (1<<30) /* Data is boolean.               */
#define RAF_SIGNED             (1<<31) /* Signed attribute.              */

#define RAF_ADDRESS            (RAF_ADDR|RAF_NOSET)
#define RAF_NOP                (RAF_ADDR|RAF_SIGNED)

#define P_BITNUM2(f) (f>>1?(f>>2?(f>>3?(f>>4?(f>>5?(f>>6?(f>>7?7:6):5):4):3):2):1):0)
#define P_BITNUM1(f) (f>>8?P_BITNUM2(f>>8):P_BITNUM2(f))
#define P_BITNUM0(f) (f>>16?P_BITNUM1(f>>16):P_BITNUM1(f))

#define LENGTH_OF(type,field) sizeof(((struct type *)0)->field)

#define TYPE_OF(type,field)       ((LENGTH_OF(type,field) == 2) ? RAF_WORD :\
                                  ((LENGTH_OF(type,field) == 4) ? RAF_LONG : RAF_BYTE))
#define BITOFFSET_OF(type,field,f) (LENGTH_OF(type,field) - (f>>8?(f>>16?(f>>24?4:3):2):1))

#ifdef __AROS__
 #if !AROS_BIG_ENDIAN
  #undef  BITOFFSET_OF
  #define BITOFFSET_OF(type,field,f) (f>>8?(f>>16?(f>>24?3:2):1):0)
 #endif
#endif

#define CHART_ATTR(type,field)       ((offsetof(struct type,field) << 16) | TYPE_OF(type,field))
#define CHART_FLAG(type,field,flag) (((offsetof(struct type,field) + BITOFFSET_OF(type,field,flag)) << 16) \
                                     | RAF_BOOL | (P_BITNUM0(flag) << 27))


#define RM_REMOVE                       (BGUI_MB+30030)   /* OBSOLETE */

/* Remove an object from a list. */

#define RM_ADDHEAD                      (BGUI_MB+30031)   /* OBSOLETE */
#define RM_ADDTAIL                      (BGUI_MB+30032)   /* OBSOLETE */

/* Add an object to a list. */
struct rmAdd {
        STACKULONG                   MethodID;
        struct List            *ra_List;
};

#define RM_INSERT                       (BGUI_MB+30033)   /* OBSOLETE */

/* Insert an object into a list. */
struct rmInsert {
        STACKULONG                   MethodID;
        struct List            *ri_List;
        Object                 *ri_Previous;
};

#define RM_PREV                         (BGUI_TB+30034)   /* OBSOLETE */

/* Get the previous object. */

#define RM_NEXT                         (BGUI_TB+30035)   /* OBSOLETE */

/* Get the next object. */


#define RM_ADDCOLLECTION                (BGUI_TB+30030)
#define RM_REMOVECOLLECTION             (BGUI_TB+30031)

/* Manage the collections this object is contained in. */  /* PRIVATE */
struct rmCollection {
        STACKULONG                   MethodID;
        Object                 *rc_Collection;
};







#define RM_REMNOTIFY                    (BGUI_MB+30040)

/* Remove a notification from a notification list. */
struct rmRemoveNotify {
        STACKULONG                   MethodID;
        APTR                    rrn_Notify;
        STACKULONG                   rrn_Flags;
};

#define RRF_ALL_MAPS           (1<<0)  /* Remove all maps.              */
#define RRF_ALL_ATTRS          (1<<1)  /* Remove all attributes.        */
#define RRF_ALL_METHODS        (1<<2)  /* Remove all methods.           */
#define RRF_ALL_HOOKS          (1<<3)  /* Remove all hooks.             */


#define RM_ADDMAP                       (BGUI_MB+30041)

/* Add an object to the maplist notification list. */
struct rmAddMap {
        STACKULONG                   MethodID;
        STACKLONG                    ram_Priority;
        STACKULONG                   ram_Flags;
        struct TagItem          ram_Condition;
        Object                 *ram_Object;
        struct TagItem         *ram_MapList;
};

#define RM_ADDATTR                      (BGUI_MB+30042)

/* Add an object to the conditional attribute notification list. */
struct rmAddAttr {
        STACKULONG                   MethodID;
        STACKLONG                    raa_Priority;
        STACKULONG                   raa_Flags;
        struct TagItem          raa_Condition;
        Object                 *raa_Object;
        struct TagItem          raa_Attr;
};


#define RM_ADDMETHOD                    (BGUI_MB+30043)

/* Add an object to the conditional method notification list. */
struct rmAddMethod {
        STACKULONG                   MethodID;
        STACKLONG                    ram_Priority;
        STACKULONG                   ram_Flags;
        struct TagItem          ram_Condition;
        Object                 *ram_Object;
        STACKULONG                   ram_Size;
        STACKULONG                   ram_MethodID;
};

#define RM_ADDHOOK                      (BGUI_MB+30044)

/* Add an object to the conditional method notification list. */
struct rmAddHook {
        STACKULONG                   MethodID;
        STACKLONG                    rah_Priority;
        STACKULONG                   rah_Flags;
        struct TagItem          rah_Condition;
        struct Hook            *rah_Hook;
};

#define RAF_NO_GINFO           (1<<0)  /* Do not send GadgetInfo.       */
#define RAF_NO_INTERIM         (1<<1)  /* Skip interim messages.        */
#define RAF_FALSE              (1<<2)  /* Condition must be false.      */
#define RAF_TRUE               (1<<3)  /* Condition must be true.       */
#define RAF_MIRROR_MAP         (1<<4)  /* Reverse map the other object. */






















/*****************************************************************************
 *
 *      "baseclass" - BOOPSI base gadget.
 *
 *      Tags: 201 - 300                 Methods: 41 - 80
 *
 *      This is a very important BGUI gadget class. All other gadget classes
 *      are sub-classed from this class. It will handle stuff like online
 *      help, notification, labels and frames etc. If you want to write a
 *      gadget class for BGUI be sure to subclass it from this class. That
 *      way your class will automatically inherit the same features.
 */
#define BT_TAGSTART                     (BGUI_TB+241)
#define BT_HelpFile                     (BGUI_TB+241)   /* ISG-- */
#define BT_HelpNode                     (BGUI_TB+242)   /* ISG-- */
#define BT_HelpLine                     (BGUI_TB+243)   /* ISG-- */
#define BT_Inhibit                      (BGUI_TB+244)   /* --G-- */
#define BT_HitBox                       (BGUI_TB+245)   /* --G-- */
#define BT_LabelObject                  (BGUI_TB+246)   /* ISG-- */
#define BT_FrameObject                  (BGUI_TB+247)   /* ISG-- */
#define BT_TextAttr                     (BGUI_TB+248)   /* ISG-- */
#define BT_NoRecessed                   (BGUI_TB+249)   /* ISG-- */
#define BT_LabelClick                   (BGUI_TB+250)   /* ISG-- */
#define BT_HelpText                     (BGUI_TB+251)   /* ISG-- */
#define BT_ToolTip                      (BGUI_TB+252)   /* ISG-- */  /* V40 */
#define BT_DragObject                   (BGUI_TB+253)   /* ISG-- */  /* V40 */
#define BT_DropObject                   (BGUI_TB+254)   /* ISG-- */  /* V40 */
#define BT_DragThreshold                (BGUI_TB+255)   /* ISG-- */  /* V40 */
#define BT_DragQualifier                (BGUI_TB+256)   /* ISG-- */  /* V40 */
#define BT_Key                          (BGUI_TB+257)   /* ISG-- */  /* V41.2 */
#define BT_RawKey                       (BGUI_TB+258)   /* ISG-- */
#define BT_Qualifier                    (BGUI_TB+259)   /* ISG-- */
#define BT_HelpTextID                   (BGUI_TB+260)   /* ISG-- */  /* V41.3 */
#define BT_ToolTipID                    (BGUI_TB+261)   /* ISG-- */
#define BT_MouseActivation              (BGUI_TB+262)   /* ISG-- */  /* V41.5 */
#define BT_Reserved1                    (BGUI_TB+263)   /* RESERVED */
#define BT_Reserved2                    (BGUI_TB+264)   /* RESERVED */
#define BT_Buffer                       (BGUI_TB+265)   /* ISG-- */  /* V41.6 */
#define BT_LeftOffset                   (BGUI_TB+266)   /* ISG-- */
#define BT_RightOffset                  (BGUI_TB+267)   /* ISG-- */
#define BT_TopOffset                    (BGUI_TB+268)   /* ISG-- */
#define BT_BottomOffset                 (BGUI_TB+269)   /* ISG-- */
#define BT_HelpHook                     (BGUI_TB+270)   /* ISG-- */  /* V41.7 */
#define BT_OuterBox                     (BGUI_TB+271)   /* --G-- */  /* V41.8 */
#define BT_InnerBox                     (BGUI_TB+272)   /* --G-- */
#define BT_PostRenderHighestClass       (BGUI_TB+273)   /* --G-- */
#define BT_TAGDONE                      (BGUI_TB+300)

#define MOUSEACT_RMB_ACTIVE             (1<<0)
#define MOUSEACT_RMB_REPORT             (1<<1)
#define MOUSEACT_MMB_ACTIVE             (1<<2)
#define MOUSEACT_MMB_REPORT             (1<<3)

/* New methods */

/* Methods 41 - 46 are obsolete. */

#define BASE_SHOWHELP                   (BGUI_MB+47)

/* Show attached online-help. */
struct bmShowHelp {
        STACKULONG                   MethodID;
        struct Window          *bsh_Window;
        struct Requester       *bsh_Requester;
        struct {
                STACKWORD            X;
                STACKWORD            Y;
        }                       bsh_Mouse;
};

#define BMHELP_OK               (0L)    /* OK, no problems.           */
#define BMHELP_NOT_ME           (1L)    /* Mouse not over the object. */
#define BMHELP_FAILURE          (2L)    /* Showing failed.            */

/* Methods 48 - 53 are obsolete. */

#define BASE_DRAGGING                   (BGUI_MB+54) /* V40 */

/* Return codes for the BASE_DRAGGING method. */
#define BDR_NONE                        0       /* Handle input yourself.   */
#define BDR_DRAGPREPARE                 1       /* Prepare for dragging.    */
#define BDR_DRAGGING                    2       /* Don't handle events.     */
#define BDR_DROP                        3       /* Image dropped.           */
#define BDR_CANCEL                      4       /* Drag canceled.           */

#define BASE_DRAGQUERY                  (BGUI_MB+55) /* V40 */

/* For both BASE_DRAGQUERY and BASE_DRAGUPDATE. */
struct bmDragPoint {
        STACKULONG                   MethodID;       /* BASE_DRAGQUERY   */
        struct GadgetInfo      *bmdp_GInfo;     /* GadgetInfo       */
        Object                 *bmdp_Source;    /* Object querying. */
        struct {
                STACKWORD            X;
                STACKWORD            Y;
        }                       bmdp_Mouse;     /* Mouse coords.    */
};

/* Return codes for BASE_DRAGQUERY. */
#define BQR_REJECT              0       /* Object will not accept drop. */
#define BQR_ACCEPT              1       /* Object will accept drop.     */

#define BASE_DRAGUPDATE                 (BGUI_MB+56) /* V40 */

/* Return codes for BASE_DRAGUPDATE. */
#define BUR_CONTINUE            0       /* Continue drag. */
#define BUR_ABORT               1       /* Abort drag.    */

#define BASE_DROPPED                    (BGUI_MB+57) /* V40 */

/* Source object is dropped. */
struct bmDropped {
        STACKULONG                   MethodID;
        struct GadgetInfo      *bmd_GInfo;      /* GadgetInfo structure. */
        Object                 *bmd_Source;     /* Object dropped.       */
        struct Window          *bmd_SourceWin;  /* Source obj window.    */
        struct Requester       *bmd_SourceReq;  /* Source onj requester. */
};

#define BASE_DRAGACTIVE                 (BGUI_MB+58) /* V40 */
#define BASE_DRAGINACTIVE               (BGUI_MB+59) /* V40 */

/* Used by both methods defined above. */
struct bmDragMsg {
        STACKULONG                   MethodID;
        struct GadgetInfo      *bmdm_GInfo;     /* GadgetInfo structure. */
        Object                 *bmdm_Source;    /* Object being dragged. */
};

#define BASE_GETDRAGOBJECT              (BGUI_MB+60) /* V40 */

/* Obtain BitMap image to drag. */
struct bmGetDragObject {
        STACKULONG                   MethodID;       /* BASE_GETDRAGOBJECT */
        struct GadgetInfo      *bmgo_GInfo;     /* GadgetInfo         */
        struct IBox            *bmgo_Bounds;    /* Bounds to buffer.  */
};

#define BASE_FREEDRAGOBJECT             (BGUI_MB+61) /* V40 */

/* Free BitMap image being dragged. */
struct bmFreeDragObject {
        STACKULONG                   MethodID;       /* BASE_FREEDRAGOBJECT */
        struct GadgetInfo      *bmfo_GInfo;     /* GadgetInfo          */
        struct BitMap          *bmfo_ObjBitMap; /* BitMap to free.     */
};

#define BASE_INHIBIT                    (BGUI_MB+62)

/* Inhibit/uninhibit this object.               */
struct bmInhibit {
        STACKULONG                   MethodID;       /* BASE_INHIBIT         */
        STACKULONG                   bmi_Inhibit;    /* Inhibit on/off.      */
};

#define BASE_FINDKEY                    (BGUI_MB+63)    /* V41.2 */

/* Locate object with this rawkey.      */
struct bmFindKey {
        STACKULONG                   MethodID;       /* BASE_FINDKEY         */
        struct {
        STACKUWORD                   Qual, Key;
        }                       bmfk_Key;       /* Key to find.         */

};

#define BASE_KEYLABEL                   (BGUI_MB+64)    /* V41.2 */

/* Attach key in this label to the object.      */
struct bmKeyLabel {
        STACKULONG                   MethodID;       /* BASE_KEYLABEL        */
};

#define BASE_LOCALIZE                   (BGUI_MB+65)    /* V41.3 */

/* Localize this object. */
struct bmLocalize {
        STACKULONG                   MethodID;       /* BASE_LOCALIZE        */
        struct bguiLocale      *bml_Locale;
};

#define BASE_RENDER                     (BGUI_MB+66)    /* V41.10 */

/* Render this object. */
struct bmRender {
        STACKULONG                   MethodID;       /* BASE_RENDER          */
        struct BaseInfo        *bmr_BInfo;      /* BaseInfo.            */
        STACKULONG                   bmr_Flags;      /* See below.           */
};

#define BASE_DIMENSIONS                 (BGUI_MB+67)    /* V41.10 */

/* Ask an object its dimensions information. */

struct bmDimensions {
        STACKULONG                   MethodID;       /* BASE_DIMENSIONS          */
        struct BaseInfo        *bmd_BInfo;      /* Ready for calculations.  */
        struct bguiExtent      *bmd_Extent;     /* Storage for dimensions.  */
        STACKULONG                   bmd_Flags;      /* See below.               */
};

/* Flags */
#define BDF_CUSTOM_GROUP        (1<<0)  /* Do not use groupclass layout.        */


#define BASE_LAYOUT                     (BGUI_MB+68)    /* V41.10 */

/* Layout this object. */
struct bmLayout {
        STACKULONG                   MethodID;       /* BASE_LAYOUT              */
        struct BaseInfo        *bml_BInfo;      /* BaseInfo.                */
        struct IBox            *bml_Bounds;     /* Bounds to layout into.   */
        STACKULONG                   bml_Flags;      /* See below.               */
};

#define BASE_RELAYOUT                   (BGUI_MB+69)    /* V41.10 */

struct bmRelayout {
   STACKULONG              MethodID;      /* BASE_RELAYOUT */
   struct GadgetInfo *bmr_GInfo;
   struct RastPort   *bmr_RPort;
};


/* Flags */
#define BLF_CUSTOM_GROUP        (1<<0)  /* Do not use groupclass layout.        */

/* Flags */
#define GDIMF_NO_FRAME          (1<<0)  /* Don't take frame width/height
                                           into consideration.                  */
#define GDIMF_NO_OFFSET         (1<<1)  /* No inner offset from the frame.      */


/*****************************************************************************
 *
 *      "groupclass" - BOOPSI group gadget.
 *
 *      Tags: 301 - 400                 Methods: 81 - 120
 *
 *      This class is the actual bgui.library layout engine. It will layout
 *      all members in a specific area.  Two group types are available,
 *      horizontal and vertical groups.
 */
#define GROUP_Rows                      (BGUI_TB+301)   /* ISG-- */  /* V41.9 */
#define GROUP_Columns                   (BGUI_TB+302)   /* ISG-- */
#define GROUP_HorizSpacing              (BGUI_TB+303)   /* ISG-- */
#define GROUP_VertSpacing               (BGUI_TB+304)   /* ISG-- */
#define GROUP_HorizAlign                (BGUI_TB+305)   /* ISG-- */
#define GROUP_VertAlign                 (BGUI_TB+306)   /* ISG-- */
#define GROUP_Members                   (BGUI_TB+312)   /* --G-- */
#define GROUP_NumMembers                (BGUI_TB+313)   /* --G-- */
#define GROUP_Style                     (BGUI_TB+321)   /* ISG-- */  /* V37 */
#define GROUP_Spacing                   (BGUI_TB+322)   /* ISG-- */
#define GROUP_HorizOffset               (BGUI_TB+323)   /* I---- */
#define GROUP_VertOffset                (BGUI_TB+324)   /* I---- */
#define GROUP_LeftOffset                (BGUI_TB+325)   /* I---- */
#define GROUP_TopOffset                 (BGUI_TB+326)   /* I---- */
#define GROUP_RightOffset               (BGUI_TB+327)   /* I---- */
#define GROUP_BottomOffset              (BGUI_TB+328)   /* I---- */
#define GROUP_Member                    (BGUI_TB+329)   /* I---- */
#define GROUP_SpaceObject               (BGUI_TB+330)   /* I---- */
#define GROUP_BackFill                  (BGUI_TB+331)   /* IS--- */
#define GROUP_EqualWidth                (BGUI_TB+332)   /* IS--- */
#define GROUP_EqualHeight               (BGUI_TB+333)   /* IS--- */
#define GROUP_Inverted                  (BGUI_TB+334)   /* I---- */
#define GROUP_BackPen                   (BGUI_TB+335)   /* IS--- */  /* V40 */
#define GROUP_BackDriPen                (BGUI_TB+336)   /* IS--- */  /* V40 */
#define GROUP_Offset                    (BGUI_TB+337)   /* I---- */  /* V41 */

/* Object layout attributes. */
#define LGO_TAGSTART                    (BGUI_TB+361)
#define LGO_Left                        (BGUI_TB+361)   /* -S--- */  /* V41.9 */
#define LGO_Top                         (BGUI_TB+362)   /* -S--- */
#define LGO_Width                       (BGUI_TB+363)   /* -S--- */
#define LGO_Height                      (BGUI_TB+364)   /* -S--- */
#define LGO_MinWidth                    (BGUI_TB+365)   /* --G-- */
#define LGO_MinHeight                   (BGUI_TB+366)   /* --G-- */
#define LGO_MaxWidth                    (BGUI_TB+367)   /* --G-- */
#define LGO_MaxHeight                   (BGUI_TB+368)   /* --G-- */
#define LGO_NomWidth                    (BGUI_TB+369)   /* --G-- */
#define LGO_NomHeight                   (BGUI_TB+370)   /* --G-- */
#define LGO_FixWidth                    (BGUI_TB+381)   /* I---- */  /* V37 */
#define LGO_FixHeight                   (BGUI_TB+382)   /* I---- */
#define LGO_Weight                      (BGUI_TB+383)   /* IS--- */
#define LGO_FixMinWidth                 (BGUI_TB+384)   /* I---- */
#define LGO_FixMinHeight                (BGUI_TB+385)   /* I---- */
#define LGO_Align                       (BGUI_TB+386)   /* I---- */
#define LGO_NoAlign                     (BGUI_TB+387)   /* I---- */  /* V38 */
#define LGO_FixAspect                   (BGUI_TB+388)   /* IS--- */  /* V41 */
#define LGO_Visible                     (BGUI_TB+389)   /* IS--- */  /* V41.9 */
#define LGO_HAlign                      (BGUI_TB+390)   /* IS--- */
#define LGO_VAlign                      (BGUI_TB+391)   /* IS--- */
#define LGO_Object                      (BGUI_TB+392)   /* --G-- */  /* V41.10 */
#define LGO_Relayout                    (BGUI_TB+393)   /* --G-- */  /* V41.10 */
#define LGO_TAGDONE                     (BGUI_TB+400)

/* Default object weight. */
#define DEFAULT_WEIGHT                  50

/* Group styles. */
#define GRSTYLE_HORIZONTAL              0
#define GRSTYLE_VERTICAL                1

/* Group alignments. */
#define GRALIGN_DEFAULT                 0               /* V41.9 */
#define GRALIGN_LEFT                    1               
#define GRALIGN_TOP                     1
#define GRALIGN_CENTER                  2
#define GRALIGN_RIGHT                   3
#define GRALIGN_BOTTOM                  3

/* Group spacings. */
#define GRSPACE_NARROW                  ((ULONG)~0)     /* V41 */
#define GRSPACE_NORMAL                  ((ULONG)~1)
#define GRSPACE_WIDE                    ((ULONG)~2)

struct bguiDimension {
        UWORD                   Width;
        UWORD                   Height;
};

struct bguiExtent {
        struct bguiDimension    be_Min;
        struct bguiDimension    be_Max;
        struct bguiDimension    be_Nom;
};

/* New methods. */

#define GRM_ADDMEMBER                   (BGUI_MB+81)

/* Add a member to the group. */
struct grmAddMember {
        STACKULONG                   MethodID;       /* GRM_ADDMEMBER            */
        Object                 *grma_Member;    /* Object to add.           */
        STACKULONG                   grma_Attr;      /* First of LGO attributes. */
};

#define GRM_REMMEMBER                   (BGUI_MB+82)

/* Remove a member from the group. */
struct grmRemMember {
        STACKULONG                   MethodID;       /* GRM_REMMEMBER            */
        Object                 *grmr_Member;    /* Object to remove.        */
};

#define GRM_DIMENSIONS                  (BGUI_MB+83)

/* Ask an object it's dimensions information. */
struct grmDimensions {
        STACKULONG                   MethodID;       /* GRM_DIMENSIONS           */
        struct GadgetInfo      *grmd_GInfo;     /* Can be NULL!             */
        struct RastPort        *grmd_RPort;     /* Ready for calculations.  */
        struct {
                UWORD          *Width;
                UWORD          *Height;
        }                       grmd_MinSize;   /* Storage for dimensions.  */
        ULONG                   grmd_Flags;     /* See below.               */
        struct {
                UWORD          *Width;
                UWORD          *Height;
        }                       grmd_MaxSize;   /* Storage for dimensions.  */
        struct {
                UWORD          *Width;
                UWORD          *Height;
        }                       grmd_NomSize;   /* Storage for dimensions.  */
};

/* Flags */
#define GDIMF_MAXIMUM           (1<<4)  /* The grmd_MaxSize is requested.       */
#define GDIMF_NOMINAL           (1<<3)  /* The grmd_NomSize is requested.       */


#define GRM_ADDSPACEMEMBER              (BGUI_MB+84)

/* Add a weight controlled spacing member. */
struct grmAddSpaceMember {
        STACKULONG                   MethodID;       /* GRM_ADDSPACEMEMBER       */
        STACKULONG                   grms_Weight;    /* Object weight.           */
};

#define GRM_INSERTMEMBER                (BGUI_MB+85)

/* Insert a member in the group. */
struct grmInsertMember {
        STACKULONG                   MethodID;       /* GRM_INSERTMEMBER         */
        Object                 *grmi_Member;    /* Member to insert.        */
        Object                 *grmi_Pred;      /* Insert after this member */
        STACKULONG                   grmi_Attr;      /* First of LGO attributes. */
};

#define GRM_REPLACEMEMBER               (BGUI_MB+86)    /* V40 */

/* Replace a member in the group. */
struct grmReplaceMember {
        STACKULONG                   MethodID;       /* GRM_REPLACEMEMBER        */
        Object                 *grrm_MemberA;   /* Object to replace.       */
        Object                 *grrm_MemberB;   /* Object which replaces.   */
        STACKULONG                   grrm_Attr;      /* First of LGO attributes. */
};

#define GRM_WHICHOBJECT                 (BGUI_MB+87)    /* V40 */

/* Locate object under these coords. */
struct grmWhichObject {
        STACKULONG                   MethodID;       /* GRM_WHICHOBJECT          */
        struct {
                 STACKWORD           X;
                 STACKWORD           Y;
        }                       grwo_Coords;    /* The coords.              */
};

#define GROUPM_NEWMEMBER                (BGUI_MB+88)    /* V41.10 */

/* Create a new member.  Uses opSet message. */


#define GROUPM_OBTAINMEMBERS            (BGUI_MB+90)    /* V41.9 */

/* Get the array of members, locking the list if necessary. */
struct gmObtainMembers {
        STACKULONG                   MethodID;       /* GROUPM_OBTAINMEMBERS     */
        STACKULONG                   gmom_Flags;     /* See below.               */
        UWORD                  *gmom_Number;    /* Get number of members.   */
};

#define GROMF_SHARED            (0)     /* Share the list (read-only).      */
#define GROMF_EXCLUSIVE         (1<<0)  /* Exclusive lock (modify).         */
#define GROMF_ATTEMPT           (0)     /* Return if lock fails.            */
#define GROMF_WAIT              (1<<1)  /* Wait until lock succeeds.        */
#define GROMF_MEMBERS           (0)     /* Get array of member nodes.       */
#define GROMF_OBJECTS           (1<<2)  /* Get array of actual objects.     */
#define GROMF_VISIBLE           (1<<3)  /* Visible objects only.            */

#define GROUPM_RELEASEMEMBERS           (BGUI_MB+91)    /* V41.9 */

/* Free the array of members. */
struct gmReleaseMembers {
        STACKULONG                   MethodID;       /* GROUPM_RELEASEMEMBERS    */
        Object                **gmrm_Array;     /* Array to release.        */
};


/*****************************************************************************
 *
 *      "buttonclass" - BOOPSI button gadget.
 *
 *      Tags: 401 - 480                 Methods: 121 - 160
 *
 *      GadTools style button gadget.
 *
 *      GA_Selected has been made gettable (OM_GET) for toggle-select
 *      buttons. (ISGNU)
 */
#define BUTTON_UNUSED1                  (BGUI_TB+401)   /* */
#define BUTTON_UNUSED0                  (BGUI_TB+402)   /* */
#define BUTTON_Image                    (BGUI_TB+403)   /* IS--U */
#define BUTTON_SelectedImage            (BGUI_TB+404)   /* IS--U */
#define BUTTON_EncloseImage             (BGUI_TB+405)   /* I---- */  /* V39 */
#define BUTTON_Vector                   (BGUI_TB+406)   /* IS--U */  /* V41 */
#define BUTTON_SelectedVector           (BGUI_TB+407)   /* IS--U */  /* V41 */
#define BUTTON_SelectOnly               (BGUI_TB+408)   /* I---- */  /* V41 */

/*****************************************************************************
 *
 *      "checkboxclass" - BOOPSI checkbox gadget.
 *
 *      Tags: 481 - 560                 Methods: 161 - 200
 *
 *      GadTools style checkbox gadget.
 *
 *      GA_Selected has been made gettable (OM_GET). (ISGNU)
 */


/*****************************************************************************
 *
 *      "cycleclass" - BOOPSI cycle gadget.
 *
 *      Tags: 561 - 640                 Methods: 201 - 240
 *
 *      GadTools style cycle gadget.
 */
#define CYC_Labels                      (BGUI_TB+561)   /* IS--- */
#define CYC_Active                      (BGUI_TB+562)   /* ISGNU */
#define CYC_Popup                       (BGUI_TB+563)   /* IS--- */
#define CYC_PopActive                   (BGUI_TB+564)   /* IS--- */  /* V40 */


/*****************************************************************************
 *
 *      "infoclass" - BOOPSI information gadget.
 *
 *      Tags: 641 - 720                 Methods: 241 - 280
 *
 *      Text gadget which supports different colors, text styles and
 *      text positioning.
 */
#define INFO_TextFormat                 (BGUI_TB+641)   /* IS--U */
#define INFO_Args                       (BGUI_TB+642)   /* IS--U */
#define INFO_MinLines                   (BGUI_TB+643)   /* IS--U */
#define INFO_FixTextWidth               (BGUI_TB+644)   /* IS--U */
#define INFO_HorizOffset                (BGUI_TB+645)   /* I---- */
#define INFO_VertOffset                 (BGUI_TB+646)   /* I---- */
#define INFO_FixTextHeight              (BGUI_TB+647)   /* IS--U */  /* V41.9 */

/* Command sequences. */
#define ISEQ_B                          "\33b"  /* Bold                      */
#define ISEQ_I                          "\33i"  /* Italics                   */
#define ISEQ_U                          "\33u"  /* Underlined                */
#define ISEQ_N                          "\33n"  /* Normal                    */
#define ISEQ_SHADOWED                   "\33s"  /* Shadowed                  */
#define ISEQ_U2                         "\33z"  /* Underlined offset 2       */
#define ISEQ_HU                         "\33Z"  /* Highlight, underlined     */
#define ISEQ_C                          "\33c"  /* Centered                  */
#define ISEQ_R                          "\33r"  /* Right                     */
#define ISEQ_L                          "\33l"  /* Left                      */
#define ISEQ_KEEP                       "\33k"  /* This will keep the
                                                   color/formatting changes
                                                   from not terminating at
                                                   the newline               */
#define ISEQ_WRAP                       "\33w"  /* Wrap text                 */
#define ISEQ_TEXT                       "\33d2" /* TEXTPEN                   */
#define ISEQ_SHINE                      "\33d3" /* SHINEPEN                  */
#define ISEQ_SHADOW                     "\33d4" /* SHADOWPEN                 */
#define ISEQ_FILL                       "\33d5" /* FILLPEN                   */
#define ISEQ_FILLTEXT                   "\33d6" /* FILLTEXTPEN               */
#define ISEQ_HIGHLIGHT                  "\33d8" /* HIGHLIGHTPEN              */
#define ISEQ_JAM1                       "\33""1"/* JAM1                      */
#define ISEQ_JAM2                       "\33""2"/* JAM2                      */
#define ISEQ_COMPLEMENT                 "\33C"  /* COMPLEMENT                */

/*****************************************************************************
 *
 *      "listviewclass" - BOOPSI listview gadget.
 *
 *      Tags: 721 - 800                 Methods: 281 - 320
 *
 *      GadTools style listview gadget.
 */
#define LISTV_TAGSTART                  (BGUI_TB+721)
#define LISTV_ResourceHook              (BGUI_TB+721)   /* ISG-- */
#define LISTV_DisplayHook               (BGUI_TB+722)   /* ISG-- */
#define LISTV_CompareHook               (BGUI_TB+723)   /* ISG-- */
#define LISTV_Top                       (BGUI_TB+724)   /* ISG-U */
#define LISTV_ListFont                  (BGUI_TB+725)   /* I-G-- */
#define LISTV_ReadOnly                  (BGUI_TB+726)   /* I-G-- */
#define LISTV_MultiSelect               (BGUI_TB+727)   /* ISG-U */
#define LISTV_EntryArray                (BGUI_TB+728)   /* I---- */
#define LISTV_Select                    (BGUI_TB+729)   /* -S--U */
#define LISTV_MakeVisible               (BGUI_TB+730)   /* -S--U */
#define LISTV_Entry                     (BGUI_TB+731)   /* ---N- */
#define LISTV_SortEntryArray            (BGUI_TB+732)   /* I---- */
#define LISTV_EntryNumber               (BGUI_TB+733)   /* ---N- */
#define LISTV_TitleHook                 (BGUI_TB+734)   /* ISG-- */
#define LISTV_LastClicked               (BGUI_TB+735)   /* --G-- */
#define LISTV_ThinFrames                (BGUI_TB+736)   /* ISG-- */
#define LISTV_LastClickedNum            (BGUI_TB+737)   /* --G-- */  /* V38 */
#define LISTV_NewPosition               (BGUI_TB+738)   /* ---N- */  /* V38 */
#define LISTV_NumEntries                (BGUI_TB+739)   /* --G-- */  /* V38 */
#define LISTV_MinEntriesShown           (BGUI_TB+740)   /* I---- */  /* V38 */
#define LISTV_SelectMulti               (BGUI_TB+741)   /* -S--U */  /* V39 */
#define LISTV_SelectNotVisible          (BGUI_TB+742)   /* -S--U */  /* V39 */
#define LISTV_SelectMultiNotVisible     (BGUI_TB+743)   /* -S--U */  /* V39 */
#define LISTV_MultiSelectNoShift        (BGUI_TB+744)   /* ISG-U */  /* V39 */
#define LISTV_DeSelect                  (BGUI_TB+745)   /* -S--U */  /* V39 */
#define LISTV_DropSpot                  (BGUI_TB+746)   /* --G-- */  /* V40 */
#define LISTV_ShowDropSpot              (BGUI_TB+747)   /* ISG-- */  /* V40 */
#define LISTV_ViewBounds                (BGUI_TB+748)   /* --G-- */  /* V40 */
#define LISTV_CustomDisable             (BGUI_TB+749)   /* ISG-- */  /* V40 */
#define LISTV_FilterHook                (BGUI_TB+750)   /* ISG-- */  /* V41 */
#define LISTV_Columns                   (BGUI_TB+751)   /* I-G-U */  /* V41 */
#define LISTV_ColumnWeights             (BGUI_TB+752)   /* IS--U */  /* V41 */
#define LISTV_DragColumns               (BGUI_TB+753)   /* ISG-U */  /* V41 */
#define LISTV_Title                     (BGUI_TB+754)   /* ISG-U */  /* V41 */
#define LISTV_PropObject                (BGUI_TB+755)   /* ISG-- */  /* V41 */
#define LISTV_PreClear                  (BGUI_TB+756)   /* ISG-- */  /* V41 */
#define LISTV_LastColumn                (BGUI_TB+757)   /* --G-- */  /* V41 */
#define LISTV_LayoutHook                (BGUI_TB+758)   /* IS--U */  /* V41 */

#define LISTC_TAGSTART                  (BGUI_TB+781)
#define LISTC_MinWidth                  (BGUI_TB+781)   /* -SG-- */  /* V41.9 */
#define LISTC_MaxWidth                  (BGUI_TB+762)   /* -SG-- */  /* V41.9 */
#define LISTC_Weight                    (BGUI_TB+763)   /* -SG-- */  /* V41.9 */
#define LISTC_Draggable                 (BGUI_TB+764)   /* -SG-- */  /* V41.9 */
#define LISTC_Hidden                    (BGUI_TB+765)   /* -SG-- */  /* V41.9 */
#define LISTC_NoSeparator               (BGUI_TB+766)   /* -SG-- */  /* V41.9 */
#define LISTC_PreClear                  (BGUI_TB+767)   /* -SG-- */  /* V41.9 */

/*
 *      LISTV_Select magic numbers.
 */
#define LISTV_Select_First              (-1L)                        /* V38 */
#define LISTV_Select_Last               (-2L)                        /* V38 */
#define LISTV_Select_Next               (-3L)                        /* V38 */
#define LISTV_Select_Previous           (-4L)                        /* V38 */
#define LISTV_Select_Top                (-5L)                        /* V38 */
#define LISTV_Select_Page_Up            (-6L)                        /* V38 */
#define LISTV_Select_Page_Down          (-7L)                        /* V38 */
#define LISTV_Select_All                (-8L)                        /* V39 */

/*
 *      The LISTV_ResourceHook is called as follows:
 *
 *      rc = hookFunc( REG(A0) struct Hook              *hook,
 *                     REG(A2) Object                   *lv_object,
 *                     REG(A1) struct lvResource        *message );
 */
struct lvResource {
        UWORD                   lvr_Command;
        APTR                    lvr_Entry;
};

/* LISTV_ResourceHook commands. */
#define LVRC_MAKE               1       /* Build the entry. */
#define LVRC_KILL               2       /* Kill the entry.  */

/*
 *      The LISTV_DisplayHook and the LISTV_TitleHook are called as follows:
 *
 *      rc = hookFunc( REG(A0) struct Hook             *hook,
 *                     REG(A2) Object                  *lv_object,
 *                     REG(A1) struct lvRender         *message );
 */
struct lvRender {
        struct RastPort        *lvr_RPort;      /* RastPort to render in.  */
        struct DrawInfo        *lvr_DrawInfo;   /* All you need to render. */
        struct Rectangle        lvr_Bounds;     /* Bounds to render in.    */
        APTR                    lvr_Entry;      /* Entry to render.        */
        UWORD                   lvr_State;      /* See below.              */
        UWORD                   lvr_Flags;      /* None defined yet.       */
        UWORD                   lvr_Column;     /* Column to render.       */
};

/* Rendering states. */
#define LVRS_NORMAL             0
#define LVRS_SELECTED           1
#define LVRS_NORMAL_DISABLED    2
#define LVRS_SELECTED_DISABLED  3

/*
 *      The LISTV_CompareHook is called as follows:
 *
 *      rc = hookFunc( REG(A0) struct Hook              *hook,
 *                     REG(A2) Object                   *lv_object,
 *                     REG(A1) struct lvCompare         *message );
 */
struct lvCompare {
        APTR                    lvc_EntryA;     /* First entry.  */
        APTR                    lvc_EntryB;     /* Second entry. */
};

/* New Methods. */
#define LVM_ADDENTRIES                  (BGUI_MB+281)

/* Add listview entries. */
struct lvmAddEntries {
        STACKULONG                   MethodID;       /* LVM_ADDENTRIES  */
        struct GadgetInfo      *lvma_GInfo;     /* GadgetInfo      */
        APTR                   *lvma_Entries;   /* Entries to add. */
        STACKULONG                   lvma_How;       /* How to add it.  */
};

/* Where to add the entries. */
#define LVAP_HEAD               1
#define LVAP_TAIL               2
#define LVAP_SORTED             3

#define LVM_ADDSINGLE                   (BGUI_MB+282)

/* Add a single entry. */
struct lvmAddSingle {
        STACKULONG                   MethodID;       /* LVM_ADDSINGLE */
        struct GadgetInfo      *lvma_GInfo;     /* GadgetInfo    */
        APTR                    lvma_Entry;     /* Entry to add. */
        STACKULONG                   lvma_How;       /* See above.    */
        STACKULONG                   lvma_Flags;     /* See below.    */
};

/* Flags. */
#define LVASF_MAKEVISIBLE       (1<<0)  /* Make entry visible.              */
#define LVASF_SELECT            (1<<1)  /* Select entry.                    */
#define LVASF_MULTISELECT       (1<<2)  /* Multi-select entry.          V40 */
#define LVASF_NOT_VISIBLE       (1<<3)  /* Do not make visible.         V40 */

/* Clear the entire list. ( Uses a lvmCommand structure as defined below.) */
#define LVM_CLEAR                       (BGUI_MB+283)

#define LVM_FIRSTENTRY                  (BGUI_MB+284)
#define LVM_LASTENTRY                   (BGUI_MB+285)
#define LVM_NEXTENTRY                   (BGUI_MB+286)
#define LVM_PREVENTRY                   (BGUI_MB+287)

/* Get an entry. */
struct lvmGetEntry {
        STACKULONG                   MethodID;       /* Any of the above. */
        APTR                    lvmg_Previous;  /* Previous entry.   */
        STACKULONG                   lvmg_Flags;     /* See below.        */
};

#define LVGEF_SELECTED          (1<<0)  /* Get selected entries. */

#define LVM_REMENTRY                    (BGUI_MB+288)

/* Remove an entry. */
struct lvmRemEntry {
        STACKULONG                   MethodID;       /* LVM_REMENTRY      */
        struct GadgetInfo      *lvmr_GInfo;     /* GadgetInfo        */
        APTR                    lvmr_Entry;     /* Entry to remove.  */
};

#define LVM_REFRESH                     (BGUI_MB+289)
#define LVM_SORT                        (BGUI_MB+290)
#define LVM_LOCKLIST                    (BGUI_MB+291)
#define LVM_UNLOCKLIST                  (BGUI_MB+292)

/* Refresh/Sort list. */
struct lvmCommand {
        STACKULONG                   MethodID;       /* LVM_REFRESH       */
        struct GadgetInfo      *lvmc_GInfo;     /* GadgetInfo        */
};

#define LVM_MOVE                        (BGUI_MB+293) /* V38 */

/* Move an entry in the list. */
struct lvmMove {
        STACKULONG                   MethodID;       /* LVM_MOVE          */
        struct GadgetInfo      *lvmm_GInfo;     /* GadgetInfo        */
        APTR                    lvmm_Entry;     /* Entry to move     */
        STACKULONG                   lvmm_Direction; /* See below         */
        STACKULONG                   lvmm_NewPos;    /* New position. V40 */
};

/* Move directions. */
#define LVMOVE_UP               0       /* Move entry up.            */
#define LVMOVE_DOWN             1       /* Move entry down.          */
#define LVMOVE_TOP              2       /* Move entry to the top.    */
#define LVMOVE_BOTTOM           3       /* Move entry to the bottom. */
#define LVMOVE_NEWPOS           4       /* Move to new position. V40 */

#define LVM_REPLACE                     (BGUI_MB+294) /* V39 */

/* Replace an entry by another. */
struct lvmReplace {
        STACKULONG                   MethodID;       /* LVM_REPLACE       */
        struct GadgetInfo      *lvmr_GInfo;     /* GadgetInfo        */
        APTR                    lvmr_OldEntry;  /* Entry to replace. */
        APTR                    lvmr_NewEntry;  /* New entry.        */
};

#define LVM_REDRAW                      (BGUI_MB+295) /* V40 */

#define LVM_INSERTENTRIES               (BGUI_MB+296) /* V40 */

/* Insert listview entries. */
struct lvmInsertEntries {
        STACKULONG                   MethodID;       /* LVM_INSERTENTRIES */
        struct GadgetInfo      *lvmi_GInfo;     /* GadgetInfo        */
        STACKULONG                   lvmi_Pos;       /* Position.         */
        APTR                   *lvmi_Entries;   /* Entries to insert.*/
};

#define LVM_INSERTSINGLE                (BGUI_MB+297) /* V40 */

/* Insert a single entry. */
struct lvmInsertSingle {
        STACKULONG                   MethodID;       /* LVM_INSERTSINGLE  */
        struct GadgetInfo      *lvis_GInfo;     /* GadgetInfo        */
        STACKULONG                   lvis_Pos;       /* Position.         */
        APTR                    lvis_Entry;     /* Entry to insert.  */
        STACKULONG                   lvis_Flags;     /* See LVM_ADDSINGLE */
};

#define LVM_REMSELECTED                 (BGUI_MB+298)   /* V40 */

#define LVM_REDRAWSINGLE                (BGUI_MB+299)   /* V41.7 */

/* Redraw a single entry or column. */
struct lvmRedrawSingle {
        STACKULONG                   MethodID;       /* LVM_REDRAWSINGLE  */
        struct GadgetInfo      *lvrs_GInfo;     /* GadgetInfo.       */
        APTR                    lvrs_Entry;     /* Entry to redraw.  */
        STACKULONG                   lvrs_Column;    /* Column to redraw. */
        STACKULONG                   lvrs_Flags;     /* See below.        */
};

#define LVRF_ALL_COLUMNS 1
#define LVRF_ALL_ENTRIES 2

/* Set column attributes. */
#define LVM_SETCOLUMNATTRS              (BGUI_MB+300)   /* V41.9 */

/* Get column attributes. */
#define LVM_GETCOLUMNATTRS              (BGUI_MB+301)   /* V41.9 */

struct lvmColumnAttrs {
        STACKULONG                   MethodID;       /* LVM_???COLUMNATTRS */
        struct GadgetInfo      *lvca_GInfo;     /* GadgetInfo.        */
        STACKULONG                   lvca_Column;    /* Column number.     */
        STACKULONG                   lvca_AttrList;  /* First LISTC attr.  */
};



/*****************************************************************************
 *
 *      "progressclass" - BOOPSI progression gadget.
 *
 *      Tags: 801 - 880                 Methods: 321 - 360
 *
 *      Progression indicator fuel guage.
 */
#define PROGRESS_Min                    (BGUI_TB+801)   /* IS--- */
#define PROGRESS_Max                    (BGUI_TB+802)   /* IS--- */
#define PROGRESS_Done                   (BGUI_TB+803)   /* ISGNU */
#define PROGRESS_Vertical               (BGUI_TB+804)   /* I---- */
#define PROGRESS_Divisor                (BGUI_TB+805)   /* I---- */
#define PROGRESS_FormatString           (BGUI_TB+806)   /* I---- */  /* V40 */


/*****************************************************************************
 *
 *      "propclass" - BOOPSI proportional gadget.
 *
 *      Tags: 881 - 960                 Methods: 361 - 400
 *
 *      GadTools style scroller gadget.
 */
#define PGA_Arrows                      (BGUI_TB+881)   /* I---- */
#define PGA_ArrowSize                   (BGUI_TB+882)   /* I---- */
#define PGA_Reserved1                   (BGUI_TB+883)   /* RESERVED */
#define PGA_ThinFrame                   (BGUI_TB+884)   /* I---- */
#define PGA_XenFrame                    (BGUI_TB+885)   /* I---- */
#define PGA_NoFrame                     (BGUI_TB+886)   /* I---- */  /* V40 */


/*****************************************************************************
 *
 *      "stringclass" - BOOPSI string gadget.
 *
 *      Tags: 961 - 1040                Methods: 401 - 440
 *
 *      GadTools style string/integer gadget.
 */
#define STRINGA_UNUSED1                 (BGUI_TB+961)   /* */
#define STRINGA_UNUSED2                 (BGUI_TB+962)   /* */
#define STRINGA_MinCharsVisible         (BGUI_TB+963)   /* I---- */  /* V39 */
#define STRINGA_IntegerMin              (BGUI_TB+964)   /* IS--U */  /* V39 */
#define STRINGA_IntegerMax              (BGUI_TB+965)   /* IS--U */  /* V39 */
#define STRINGA_StringInfo              (BGUI_TB+966)   /* --G-- */  /* V40 */

#define SM_FORMAT_STRING                (BGUI_MB+401)   /* V39 */

/* Format the string contents. */
struct smFormatString {
        STACKULONG              MethodID;    /* SM_FORMAT_STRING    */
        struct GadgetInfo *smfs_GInfo;  /* GadgetInfo          */
        UBYTE             *smfs_FStr;   /* Format string       */
        STACKULONG              smfs_Arg1;   /* Format arg          */
        /* STACKULONG           smfs_Arg2; */
        /* ... */
};


/*****************************************************************************
 *
 *      "viewclass" - BOOPSI view object.                            -- V41 --
 *
 *      Tags: 1041 - 1120               Methods: 441 - 480
 *
 *      Gadget to view a clipped portion of another object.
 */
#define VIEW_X                          (BGUI_TB+1041)  /* ISG-U */  /* V41.8 */
#define VIEW_Y                          (BGUI_TB+1042)  /* ISG-U */
#define VIEW_MinWidth                   (BGUI_TB+1043)  /* ISG-- */
#define VIEW_MinHeight                  (BGUI_TB+1044)  /* ISG-- */
#define VIEW_ScaleMinWidth              (BGUI_TB+1045)  /* ISG-- */
#define VIEW_ScaleMinHeight             (BGUI_TB+1046)  /* ISG-- */
#define VIEW_ScaleWidth                 (BGUI_TB+1047)  /* ISG-- */
#define VIEW_ScaleHeight                (BGUI_TB+1048)  /* ISG-- */
#define VIEW_VScroller                  (BGUI_TB+1049)  /* IS--- */
#define VIEW_HScroller                  (BGUI_TB+1050)  /* IS--- */
#define VIEW_AbsoluteX                  (BGUI_TB+1051)  /* --G-- */
#define VIEW_AbsoluteY                  (BGUI_TB+1052)  /* --G-- */
#define VIEW_Object                     (BGUI_TB+1053)  /* ISG-U */
#define VIEW_NoDisposeObject            (BGUI_TB+1054)  /* ISG-- */

/*****************************************************************************
 *
 *      "pageclass" - BOOPSI paging gadget.
 *
 *      Tags: 1121 - 1200               Methods: 481 - 520
 *
 *      Gadget to handle pages of gadgets.
 */
#define PAGE_Active                     (BGUI_TB+1121)  /* ISGNU */
#define PAGE_Member                     (BGUI_TB+1122)  /* I---- */
#define PAGE_NoBufferRP                 (BGUI_TB+1123)  /* I---- */
#define PAGE_Inverted                   (BGUI_TB+1124)  /* I---- */


/*****************************************************************************
 *
 *      "mxclass" - BOOPSI mx gadget.
 *
 *      Tags: 1201 - 1280               Methods: 521 - 560
 *
 *      GadTools style mx gadget.
 */
#define MX_Labels                       (BGUI_TB+1201)  /* I---- */
#define MX_Active                       (BGUI_TB+1202)  /* ISGNU */
#define MX_LabelPlace                   (BGUI_TB+1203)  /* I---- */
#define MX_DisableButton                (BGUI_TB+1204)  /* IS--U */
#define MX_EnableButton                 (BGUI_TB+1205)  /* IS--U */
#define MX_TabsObject                   (BGUI_TB+1206)  /* I---- */
#define MX_TabsTextAttr                 (BGUI_TB+1207)  /* I---- */
#define MX_TabsUpsideDown               (BGUI_TB+1208)  /* I---- */  /* V40 */
#define MX_TabsBackFill                 (BGUI_TB+1209)  /* I---- */  /* V40 */
#define MX_TabsBackPen                  (BGUI_TB+1210)  /* I---- */  /* V40 */
#define MX_TabsBackDriPen               (BGUI_TB+1211)  /* I---- */  /* V40 */
#define MX_LabelsID                     (BGUI_TB+1212)  /* I---- */  /* V41 */
#define MX_Spacing                      (BGUI_TB+1213)  /* I---- */  /* V41 */
#define MX_Type                         (BGUI_TB+1214)  /* I---- */  /* V41.8 */
#define MX_TabsBackFillHook             (BGUI_TB+1215)  /* I---- */
#define MX_TabsFillPattern              (BGUI_TB+1216)  /* i---- */
#define MX_RotateLeft                   (BGUI_TB+1217)  /* I---- */  /* RESERVED */
#define MX_RotateRight                  (BGUI_TB+1218)  /* I---- */  /* RESERVED */

#define MXTYPE_RADIOBUTTON              0
#define MXTYPE_TAB_TOP                  1
#define MXTYPE_TAB_BOTTOM               2
#define MXTYPE_TAB_LEFT                 3
#define MXTYPE_TAB_RIGHT                4


/*****************************************************************************
 *
 *      "sliderclass" - BOOPSI slider gadget.
 *
 *      Tags: 1281 - 1360               Methods: 561 - 600
 *
 *      GadTools style slider gadget.
 */
#define SLIDER_Min                      (BGUI_TB+1281)  /* ISG-U */
#define SLIDER_Max                      (BGUI_TB+1282)  /* ISG-U */
#define SLIDER_Level                    (BGUI_TB+1283)  /* ISGNU */
#define SLIDER_ThinFrame                (BGUI_TB+1284)  /* I---- */
#define SLIDER_XenFrame                 (BGUI_TB+1285)  /* I---- */
#define SLIDER_NoFrame                  (BGUI_TB+1286)  /* I---- */  /* V40 */


/*****************************************************************************
 *
 *      "indicatorclass" - BOOPSI indicator gadget.
 *
 *      Tags: 1361 - 1440               Methods: ??
 *
 *      Textual level indicator gadget.
 */
#define INDIC_TAGSTART                  (BGUI_TB+1361)
#define INDIC_Min                       (BGUI_TB+1361)  /* ISG-U */
#define INDIC_Max                       (BGUI_TB+1362)  /* ISG-U */
#define INDIC_Level                     (BGUI_TB+1363)  /* ISG-U */
#define INDIC_FormatString              (BGUI_TB+1364)  /* ISG-U */
#define INDIC_Justification             (BGUI_TB+1365)  /* ISG-U */

/* Justification */
#define IDJ_LEFT                (0L)
#define IDJ_CENTER              (1L)
#define IDJ_RIGHT               (2L)


/*****************************************************************************
 *
 *      "externalclass" - BGUI external class interface.
 *
 *      Tags: 1441 - 1500               Methods: ??
 */
#define EXT_Class                       (BGUI_TB+1441)  /* I---- */
#define EXT_ClassID                     (BGUI_TB+1442)  /* I---- */
#define EXT_MinWidth                    (BGUI_TB+1443)  /* I---- */
#define EXT_MinHeight                   (BGUI_TB+1444)  /* I---- */
#define EXT_TrackAttr                   (BGUI_TB+1445)  /* I---- */
#define EXT_Object                      (BGUI_TB+1446)  /* --G-- */
#define EXT_NoRebuild                   (BGUI_TB+1447)  /* I---- */


/*****************************************************************************
 *
 *      "separatorclass" - BOOPSI separator class.
 *
 *      Tags: 1501 - 1580               Methods: ??
 */
#define SEP_Horiz                       (BGUI_TB+1501)  /* IS--- */
#define SEP_Title                       (BGUI_TB+1502)  /* IS--- */
#define SEP_Thin                        (BGUI_TB+1503)  /* IS--- */
#define SEP_Highlight                   (BGUI_TB+1504)  /* IS--- */
#define SEP_CenterTitle                 (BGUI_TB+1505)  /* IS--- */
#define SEP_Recessed                    (BGUI_TB+1506)  /* IS--- */  /* V39 */
#define SEP_TitleLeft                   (BGUI_TB+1507)  /* IS--- */  /* V40 */
#define SEP_TitleRight                  (BGUI_TB+1508)  /* IS--- */  /* V40 */
#define SEP_TitleID                     (BGUI_TB+1509)  /* IS--- */  /* V41.8 */

/* BGUI_TB+1581 through BGUI_TB+1760 reserved. */

/*****************************************************************************
 *
 *      "windowclass" - BOOPSI window class.
 *
 *      Tags: 1761 - 1860               Methods: 601 - 660
 *
 *      This class creates and maintains an intuition window.
 */
#define WINDOW_TAGSTART                 (BGUI_TB+1761)
#define WINDOW_Position                 (BGUI_TB+1761)  /* IS--- */
#define WINDOW_ScaleWidth               (BGUI_TB+1762)  /* IS--- */
#define WINDOW_ScaleHeight              (BGUI_TB+1763)  /* IS--- */
#define WINDOW_LockWidth                (BGUI_TB+1764)  /* I---- */
#define WINDOW_LockHeight               (BGUI_TB+1765)  /* I---- */
#define WINDOW_PosRelBox                (BGUI_TB+1766)  /* I---- */
#define WINDOW_Bounds                   (BGUI_TB+1767)  /* ISG-- */
/* BGUI_TB+1768 through BGUI_TB+1770 reserved. */
#define WINDOW_DragBar                  (BGUI_TB+1771)  /* I---- */
#define WINDOW_SizeGadget               (BGUI_TB+1772)  /* I---- */
#define WINDOW_CloseGadget              (BGUI_TB+1773)  /* I---- */
#define WINDOW_DepthGadget              (BGUI_TB+1774)  /* I---- */
#define WINDOW_SizeBottom               (BGUI_TB+1775)  /* I---- */
#define WINDOW_SizeRight                (BGUI_TB+1776)  /* I---- */
#define WINDOW_Activate                 (BGUI_TB+1777)  /* I---- */
#define WINDOW_RMBTrap                  (BGUI_TB+1778)  /* I---- */
#define WINDOW_SmartRefresh             (BGUI_TB+1779)  /* I---- */
#define WINDOW_ReportMouse              (BGUI_TB+1780)  /* I---- */
#define WINDOW_Borderless               (BGUI_TB+1781)  /* I---- */  /* V39 */
#define WINDOW_Backdrop                 (BGUI_TB+1782)  /* I---- */  /* V39 */
#define WINDOW_ShowTitle                (BGUI_TB+1783)  /* I---- */  /* V39 */
/* BGUI_TB+1784 through BGUI_TB+1790 reserved. */
#define WINDOW_IDCMP                    (BGUI_TB+1791)  /* I---- */
#define WINDOW_SharedPort               (BGUI_TB+1792)  /* IS--- */
#define WINDOW_Title                    (BGUI_TB+1793)  /* ISGNU */
#define WINDOW_ScreenTitle              (BGUI_TB+1794)  /* ISGNU */
#define WINDOW_MenuStrip                (BGUI_TB+1795)  /* I-G-- */
#define WINDOW_MasterGroup              (BGUI_TB+1796)  /* I---- */
#define WINDOW_Screen                   (BGUI_TB+1797)  /* IS--- */
#define WINDOW_PubScreenName            (BGUI_TB+1798)  /* IS--- */
#define WINDOW_UserPort                 (BGUI_TB+1799)  /* --G-- */
#define WINDOW_SigMask                  (BGUI_TB+1800)  /* --G-- */
#define WINDOW_IDCMPHook                (BGUI_TB+1801)  /* IS--- */
#define WINDOW_VerifyHook               (BGUI_TB+1802)  /* IS--- */
#define WINDOW_IDCMPHookBits            (BGUI_TB+1803)  /* IS--- */
#define WINDOW_VerifyHookBits           (BGUI_TB+1804)  /* IS--- */
#define WINDOW_Font                     (BGUI_TB+1805)  /* ISG-- */
#define WINDOW_FallBackFont             (BGUI_TB+1806)  /* IS--- */
#define WINDOW_HelpFile                 (BGUI_TB+1807)  /* IS--- */
#define WINDOW_HelpNode                 (BGUI_TB+1808)  /* IS--- */
#define WINDOW_HelpLine                 (BGUI_TB+1809)  /* IS--- */
#define WINDOW_AppWindow                (BGUI_TB+1810)  /* I---- */
#define WINDOW_AppMask                  (BGUI_TB+1811)  /* --G-- */
#define WINDOW_UniqueID                 (BGUI_TB+1812)  /* IS--- */
#define WINDOW_Window                   (BGUI_TB+1813)  /* --G-- */
#define WINDOW_HelpText                 (BGUI_TB+1814)  /* IS--- */
#define WINDOW_NoBufferRP               (BGUI_TB+1815)  /* I---- */
#define WINDOW_AutoAspect               (BGUI_TB+1816)  /* I-G-- */
#define WINDOW_PubScreen                (BGUI_TB+1817)  /* IS--- */  /* V39 */
#define WINDOW_CloseOnEsc               (BGUI_TB+1818)  /* IS--- */  /* V39 */
#define WINDOW_ActNext                  (BGUI_TB+1819)  /* ----- */  /* V39 */
#define WINDOW_ActPrev                  (BGUI_TB+1820)  /* ----- */  /* V39 */
#define WINDOW_NoVerify                 (BGUI_TB+1821)  /* -S--- */  /* V39 */
#define WINDOW_MenuFont                 (BGUI_TB+1822)  /* IS--- */  /* V40 */
#define WINDOW_ToolTicks                (BGUI_TB+1823)  /* ISG-U */  /* V40 */
#define WINDOW_LBorderGroup             (BGUI_TB+1824)  /* I---- */  /* V40 */
#define WINDOW_TBorderGroup             (BGUI_TB+1825)  /* I---- */  /* V40 */
#define WINDOW_RBorderGroup             (BGUI_TB+1826)  /* I---- */  /* V40 */
#define WINDOW_BBorderGroup             (BGUI_TB+1827)  /* I---- */  /* V40 */
#define WINDOW_TitleZip                 (BGUI_TB+1828)  /* I---- */  /* V40 */
#define WINDOW_AutoKeyLabel             (BGUI_TB+1829)  /* I---- */  /* V41 */
#define WINDOW_TitleID                  (BGUI_TB+1830)  /* ISG-- */  /* V41 */
#define WINDOW_ScreenTitleID            (BGUI_TB+1831)  /* ISG-- */  /* V41 */
#define WINDOW_HelpTextID               (BGUI_TB+1832)  /* ISG-- */  /* V41 */
#define WINDOW_Locale                   (BGUI_TB+1833)  /* IS--- */  /* V41 */
#define WINDOW_Catalog                  (BGUI_TB+1834)  /* IS--- */  /* V41 */
#define WINDOW_PreBufferRP              (BGUI_TB+1835)  /* IS--- */  /* V41.8 */
#define WINDOW_ToolTipHook              (BGUI_TB+1836)  /* IS--- */  /* V41.9 */

/* Possible window positions. */
#define POS_CENTERSCREEN        (0L)    /* Center on the screen             */
#define POS_CENTERMOUSE         (1L)    /* Center under the mouse           */
#define POS_TOPLEFT             (2L)    /* Top-left of the screen           */

/* New methods */

#define WM_OPEN         (BGUI_MB+601)   /* Open the window                  */
#define WM_CLOSE        (BGUI_MB+602)   /* Close the window                 */
#define WM_SLEEP        (BGUI_MB+603)   /* Put the window to sleep          */
#define WM_WAKEUP       (BGUI_MB+604)   /* Wake the window up               */
#define WM_HANDLEIDCMP  (BGUI_MB+605)   /* Call the IDCMP handler           */

/* Pre-defined WM_HANDLEIDCMP return codes. */
#define WMHI_CLOSEWINDOW        (1<<16) /* The close gadget was clicked     */
#define WMHI_NOMORE             (2<<16) /* No more messages                 */
#define WMHI_INACTIVE           (3<<16) /* The window was de-activated      */
#define WMHI_ACTIVE             (4<<16) /* The window was activated         */
#define WMHI_MENUHELP           (5<<16) /* A menuhelp message was sent.     */
#define WMHI_IGNORE             (~0L)   /* Like it say's: ignore            */
#define WMHI_RMB                (1<<24) /* The object was activated by RMB  */
#define WMHI_MMB                (1<<25) /* The object was activated by MMB  */

#define WM_GADGETKEY                    (BGUI_MB+606)

/* Add a hotkey to a gadget. */
struct wmGadgetKey {
        STACKULONG              MethodID;       /* WM_GADGETKEY                  */
        struct Requester  *wmgk_Requester; /* When used in a requester      */
        Object            *wmgk_Object;    /* Object to activate            */
        STRPTR             wmgk_Key;       /* Key that triggers activ.      */
};

#define WM_KEYACTIVE                    (BGUI_MB+607)
#define WM_KEYINPUT                     (BGUI_MB+608)

/* Send with the WM_KEYACTIVE and WM_KEYINPUT methods. */
struct wmKeyInput {
        STACKULONG              MethodID;     /* WM_KEYACTIVE/WM_KEYINPUT        */
        struct GadgetInfo *wmki_GInfo;   /* GadgetInfo                      */
        struct InputEvent *wmki_IEvent;  /* Input event                     */
        ULONG             *wmki_ID;      /* Storage for the object ID       */
        STRPTR             wmki_Key;     /* Key that triggered activation.  */
};

/* Possible WM_KEYACTIVE and WM_KEYINPUT return codes. */
#define WMKF_MEACTIVE           (0L)     /* Object went active.             */
#define WMKF_CANCEL             (1<<0)   /* Key activation canceled.        */
#define WMKF_VERIFY             (1<<1)   /* Key activation confirmed        */
#define WMKF_ACTIVATE           (1<<2)   /* ActivateGadget() object         */

#define WM_KEYINACTIVE                  (BGUI_MB+609)

/* De-activate a key session. */
struct wmKeyInActive {
        STACKULONG              MethodID;    /* WM_KEYINACTIVE                   */
        struct GadgetInfo *wmkia_GInfo; /* GadgetInfo                       */
};

#define WM_DISABLEMENU                  (BGUI_MB+610)
#define WM_CHECKITEM                    (BGUI_MB+611)

/* Disable/Enable a menu or Set/Clear a checkit item. */
struct wmMenuAction {
        STACKULONG              MethodID;    /* WM_DISABLEMENU/WM_CHECKITEM      */
        STACKULONG              wmma_MenuID; /* Menu it's ID                     */
        STACKULONG              wmma_Set;    /* TRUE = set, FALSE = clear        */
};

#define WM_MENUDISABLED                 (BGUI_MB+612)
#define WM_ITEMCHECKED                  (BGUI_MB+613)

struct wmMenuQuery {
        STACKULONG              MethodID;    /* WM_MENUDISABLED/WM_ITEMCHECKED   */
        STACKULONG              wmmq_MenuID; /* Menu it's ID                     */
};

#define WM_TABCYCLE_ORDER               (BGUI_MB+614)

/* Set the tab-cycling order. */
struct wmTabCycleOrder {
        STACKULONG              MethodID;    /* WM_TABCYCLE_ORDER                */
        Object            *wtco_Object1;
        /* Object         *wtco_Object2; */
        /* ...  */
        /* NULL */
};

/* Obtain the app message. */
#define WM_GETAPPMSG                    (BGUI_MB+615)

#define WM_ADDUPDATE                    (BGUI_MB+616)

/* Add object to the update notification list. */
struct wmAddUpdate {
        STACKULONG              MethodID;            /* WM_ADDUPDATE             */
        STACKULONG              wmau_SourceID;       /* ID of source object.     */
        Object            *wmau_Target;         /* Target object.           */
        struct TagItem    *wmau_MapList;        /* Attribute map-list.      */
};

#define WM_REPORT_ID                    (BGUI_MB+617) /* V38 */

/* Report a return code from a IDCMP/Verify hook. */
struct wmReportID {
        STACKULONG              MethodID;            /* WM_REPORT_ID              */
        STACKULONG              wmri_ID;             /* ID to report.             */
        STACKULONG              wmri_Flags;          /* See below.                */
        struct Task       *wmri_SigTask;        /* Task to signal.       V40 */
};

/* Flags */
#define WMRIF_DOUBLE_CLICK      (1<<0)          /* Simulate double-click.    */
#define WMRIF_TASK              (1<<1)          /* Task to signal valid. V40 */

/* Get the window which signalled us. */
#define WM_GET_SIGNAL_WINDOW            (BGUI_MB+618) /* V39 */

#define WM_REMOVE_OBJECT                (BGUI_MB+619) /* V40 */

/* Remove an object from the window key and/or tabcycle list. */
struct wmRemoveObject {
        STACKULONG              MethodID;            /* WM_REMOVE_OBJECT         */
        Object            *wmro_Object;         /* Object to remove.        */
        STACKULONG              wmro_Flags;          /* See below.               */
};

/* Flags */
#define WMROF_KEY_LIST          (1<<0)          /* Remove from key-list.    */
#define WMROF_CYCLE_LIST        (1<<1)          /* Remove from cycle list.  */

#define WM_WHICHOBJECT                  (BGUI_MB+620) /* V40 */

/* Information for a custom tooltip hook. */          /* V41.9 */
struct ttCommand {
        ULONG              ttc_Command;         /* TT_SHOW or TT_HIDE       */
        Object            *ttc_Object;          /* Object to show tip for.  */
        APTR               ttc_UserData;        /* See description below.   */
};

#define TT_SHOW            1
/*
 * TT_SHOW should open a tooltip.  ttc_UserData is undefined.  Return 0 upon
 * failure, any other value if a tooltip is successfully displayed.  Your
 * return value will be passed as ttc_UserData to TT_HIDE; useful applications
 * may include a window pointer or object, or a custom data structure.
 */
#define TT_HIDE            2
/*
 * This command should close a tooltip previously shown with TT_SHOW.  You are
 * passed the return value from TT_SHOW as ttc_UserData, so you can close the
 * window, free resources, etc.
 */

/*****************************************************************************
 *
 *      "areaclass" - BOOPSI area gadget.
 *
 *      Tags: 2021 - 2100               Methods: 741-780
 *
 *      AREA_MinWidth and AREA_MinHeight are required attributes.
 *      Just pass the minimum area size you need here.
 */
 
#define AREA_MinWidth                   (BGUI_TB+2021)  /* I---- */  /* V41 */
#define AREA_MinHeight                  (BGUI_TB+2022)  /* I---- */  /* V41 */
#define AREA_AreaBox                    (BGUI_TB+2023)  /* --G-- */  /* V41 */

/*****************************************************************************
 *
 *      "paletteclass" - BOOPSI palette class.
 *
 *      Tags: 2101 - 2180               Methods: 781-820
 */
#define PALETTE_Depth                   (BGUI_TB+2101)  /* I---- */  /* V41.7 */
#define PALETTE_ColorOffset             (BGUI_TB+2102)  /* I---- */
#define PALETTE_PenTable                (BGUI_TB+2103)  /* I---- */
#define PALETTE_CurrentColor            (BGUI_TB+2104)  /* ISGNU */

/*****************************************************************************
 *
 *      "popbuttonclass" - BOOPSI popbutton class.
 *
 *      Tags: 2181 - 2260               Methods: 821-860
 */
#define PMB_Image                       (BGUI_TB+2181)  /* IS--- */  /* V41.7 */
#define PMB_MenuEntries                 (BGUI_TB+2182)  /* I---- */
#define PMB_MenuNumber                  (BGUI_TB+2183)  /* --GN- */
#define PMB_PopPosition                 (BGUI_TB+2184)  /* I---- */
/*
 * All labelclass attributes are usable at create time (I).
 * The vectorclass attributes are usable at create time and
 * with OM_SET (IS).
 */

/*
 * An array of these structures define
 * the menu labels.
 */
struct PopMenu {
   UBYTE      *pm_Label;               /* Menu text, NULL terminates array. */
   UWORD       pm_Flags;               /* See below.                        */
   ULONG       pm_MutualExclude;       /* Mutual-exclusion.                 */
};

/* Flags */
#define PMF_CHECKIT     (1<<0)         /* Checkable (toggle) item.          */
#define PMF_CHECKED     (1<<1)         /* The item is checked.              */
#define PMF_DISABLED    (1<<2)         /* The item is disabled. (NMC:Added) */

/*
** Special menu entry.
**/
#define PMB_BARLABEL    (UBYTE *)~0

/* New Methods */
#define PMBM_CHECK_STATUS               (BGUI_MB+821)
#define PMBM_CHECK_MENU                 (BGUI_MB+822)
#define PMBM_UNCHECK_MENU               (BGUI_MB+823)
#define PMBM_ENABLE_ITEM                (BGUI_MB+824)
#define PMBM_DISABLE_ITEM               (BGUI_MB+825)
#define PMBM_ENABLE_STATUS              (BGUI_MB+826)

struct pmbmCommand {
   STACKULONG       MethodID;
   STACKULONG       pmbm_MenuNumber;  /* Menu to perform action on. */
};

#endif /* BGUI_BGUI_H */
