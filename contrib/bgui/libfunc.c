/*
 * @(#) $Header$
 *
 * BGUI library
 * libfunc.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.10  1998/02/25 21:12:26  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:50  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

/*
 * Internal lock housekeeping.
 */
typedef struct {
   struct Requester      wl_IDCMPLock;       /* Requester lock.   */
   struct Window        *wl_Locked;       /* Locked window. */
   UWORD           wl_MinWidth, wl_MinHeight;   /* Current sizes. */
   UWORD           wl_MaxWidth, wl_MaxHeight;   /*     "      "         */
}  WINDOWLOCK;

/*
 * Table for class init stuff.
 */
typedef struct {
   Class                *cd_Storage;
   Class              *(*cd_InitFunc)(void);
   UWORD                 cd_ClassID;
   UBYTE                *cd_ClassFile;
   struct BGUIClassBase *cd_ClassBase;
   struct TagItem       *cd_DefTags;
}  CLASSDEF;

STATIC CLASSDEF Classes[] =
{
   { NULL, InitGroupNodeClass,   BGUI_GROUP_NODE,         NULL,                              NULL },

   { NULL, InitDGMClass,         BGUI_DGM_OBJECT,         NULL,                              NULL },
   { NULL, InitRootClass,        BGUI_ROOT_OBJECT,        NULL,                              NULL },

   /*
    * Graphic classes.
    */
   { NULL, InitTextClass,        BGUI_TEXT_GRAPHIC,       NULL,                              NULL },

   /*
    * Image classes.
    */
   { NULL, InitImageClass,       BGUI_IMAGE_OBJECT,       NULL,                              NULL },
   { NULL, InitFrameClass,       BGUI_FRAME_IMAGE,        "images/bgui_frame.image",         NULL },
   { NULL, InitLabelClass,       BGUI_LABEL_IMAGE,        "images/bgui_label.image",         NULL },
   { NULL, InitVectorClass,      BGUI_VECTOR_IMAGE,       "images/bgui_vector.image",        NULL },
   { NULL, InitSystemClass,      BGUI_SYSTEM_IMAGE,       "images/bgui_system.image",        NULL },

   /*
    * Gadget classes.
    */
   { NULL, InitGadgetClass,      BGUI_GADGET_OBJECT,      NULL,                              NULL },
   { NULL, InitBaseClass,        BGUI_BASE_GADGET,        "gadgets/bgui_base.gadget",        NULL },
   { NULL, InitButtonClass,      BGUI_BUTTON_GADGET,      "gadgets/bgui_button.gadget",      NULL },
   { NULL, InitGroupClass,       BGUI_GROUP_GADGET,       "gadgets/bgui_group.gadget",       NULL },
   { NULL, InitCycleClass,       BGUI_CYCLE_GADGET,       "gadgets/bgui_cycle.gadget",       NULL },
   { NULL, InitCheckBoxClass,    BGUI_CHECKBOX_GADGET,    "gadgets/bgui_checkbox.gadget",    NULL },
   { NULL, InitInfoClass,        BGUI_INFO_GADGET,        "gadgets/bgui_info.gadget",        NULL },
   { NULL, InitStringClass,      BGUI_STRING_GADGET,      "gadgets/bgui_string.gadget",      NULL },
   { NULL, InitPropClass,        BGUI_PROP_GADGET,        "gadgets/bgui_prop.gadget",        NULL },
   { NULL, InitIndicatorClass,   BGUI_INDICATOR_GADGET,   "gadgets/bgui_indicator.gadget",   NULL },
   { NULL, InitProgressClass,    BGUI_PROGRESS_GADGET,    "gadgets/bgui_progress.gadget",    NULL },
   { NULL, InitSliderClass,      BGUI_SLIDER_GADGET,      "gadgets/bgui_slider.gadget",      NULL },
   { NULL, InitPageClass,        BGUI_PAGE_GADGET,        "gadgets/bgui_page.gadget",        NULL },
   { NULL, InitMxClass,          BGUI_MX_GADGET,          "gadgets/bgui_mx.gadget",          NULL },
   { NULL, InitListClass,        BGUI_LISTVIEW_GADGET,    "gadgets/bgui_listview.gadget",    NULL },
   { NULL, InitExtClass,         BGUI_EXTERNAL_GADGET,    "gadgets/bgui_external.gadget",    NULL },
   { NULL, InitSepClass,         BGUI_SEPARATOR_GADGET,   "gadgets/bgui_separator.gadget",   NULL },
   { NULL, InitRadioButtonClass, BGUI_RADIOBUTTON_GADGET, "gadgets/bgui_radiobutton.gadget", NULL },
   { NULL, InitAreaClass,        BGUI_AREA_GADGET,        "gadgets/bgui_area.gadget",        NULL },
   { NULL, InitViewClass,        BGUI_VIEW_GADGET,        "gadgets/bgui_view.gadget",        NULL },

   { NULL, NULL,                 BGUI_PALETTE_GADGET,     "gadgets/bgui_palette.gadget",     NULL },
   { NULL, NULL,                 BGUI_POPBUTTON_GADGET,   "gadgets/bgui_popbutton.gadget",   NULL },

   /*
    * Misc. classes.
    */
   { NULL, InitWindowClass,      BGUI_WINDOW_OBJECT,      "bgui_window.class",               NULL },
   { NULL, InitCxClass,          BGUI_COMMODITY_OBJECT,   "bgui_commodity.class",            NULL },
   { NULL, InitAslReqClass,      BGUI_ASLREQ_OBJECT,      "bgui_aslreq.class",               NULL },
   { NULL, InitFileReqClass,     BGUI_FILEREQ_OBJECT,     "bgui_filereq.class",              NULL },
   { NULL, InitFontReqClass,     BGUI_FONTREQ_OBJECT,     "bgui_fontreq.class",              NULL },
   { NULL, InitScreenReqClass,   BGUI_SCREENREQ_OBJECT,   "bgui_screenreq.class",            NULL },

   { NULL, NULL,                 BGUI_AREXX_OBJECT,       "bgui_arexx.class",                NULL },

   { NULL, InitSpacingClass,     BGUI_SPACING_OBJECT,     NULL,                              NULL },

   { NULL, NULL,                 (UWORD)~0,               NULL }
};

/*
 * Free the classes. Returns FALSE if one of the classes
 * fail to free.
 */
makeproto BOOL FreeClasses(void)
{
   CLASSDEF       *cd;
   BOOL            rc = TRUE;
   
   for (cd = Classes; cd->cd_ClassID != (UWORD)~0; cd++)
   {
      if (cd->cd_Storage)
      {
         if (cd->cd_ClassBase)
         {
            CloseLibrary((struct Library *)cd->cd_ClassBase);
            cd->cd_ClassBase = NULL;
         }
         else
         {
            /*
             * Return FALSE if one or more fail to free.
             */
            if (!BGUI_FreeClass(cd->cd_Storage))
               rc = FALSE;
         };
         /*
          * Clear storage fields.
          */
         cd->cd_Storage = NULL;
      };
   };
   return rc;
}

/*
 * Obtain a class pointer. This routine will only fail if you pass it
 * a non-existing class ID, or the class fails to initialize.
 */
makeproto SAVEDS ASM Class *BGUI_GetClassPtr(REG(d0) ULONG classID)
{
   CLASSDEF     *cd;
   Class        *cl = NULL;


   for (cd = Classes; cd->cd_ClassID != (UWORD)~0; cd++)
   {
      if (classID == (UWORD)cd->cd_ClassID)
      {
         if (!(cl = cd->cd_Storage))
         {
            if (cd->cd_ClassFile)
            {
               if (cd->cd_ClassBase = (struct BGUIClassBase *)OpenLibrary(cd->cd_ClassFile, 0))
               {
                  /*
                   * Get the class pointer.
                   */
                  cl = cd->cd_ClassBase->bcb_Class;
               };
            };
            if (!cl && cd->cd_InitFunc)
            {
               /*
                * Call the initialization routine.
                */
               cl = (cd->cd_InitFunc)();
            };
            cd->cd_Storage = cl;
         };
         break;
      };
   };
   return cl;
}

/*
 * Var-args stub for BGUI_NewObjectA().
 */
makeproto Object *BGUI_NewObject(ULONG classID, Tag tag1, ...)
{
   return BGUI_NewObjectA(classID, (struct TagItem *)&tag1);
}

/*
 * Create an object from a class.
 */
makeproto SAVEDS ASM Object *BGUI_NewObjectA(REG(d0) ULONG classID, REG(a0) struct TagItem *attr)
{
   Class       *cl;
   Object      *obj = NULL;

   if (cl = BGUI_GetClassPtr(classID))
      obj = NewObjectA(cl, NULL, attr);

   return obj;
}

/*
 * Allocate a bitmap.
 */
makeproto SAVEDS ASM struct BitMap *BGUI_AllocBitMap(REG(d0) ULONG width, REG(d1) ULONG height, REG(d2) ULONG depth,
   REG(d3) ULONG flags, REG(a0) struct BitMap *fr)
{
   #ifdef ENHANCED
   return AllocBitMap(width, height, depth, flags | BMF_MINPLANES, fr);
   #else
   struct BitMap  *bm = NULL;
   ULONG          *b;
   int             i, rassize;

   /*
    * Use the system routine if
    * we are running OS 3.0 or better.
    */
   if (OS30)
      return AllocBitMap(width, height, depth, flags | BMF_MINPLANES, fr);

   /*
    * Not OS 3.0 or better?
    * Make the bitmap ourselves.
    */
   if (b = (ULONG *)AllocVec(sizeof(struct BitMap) + sizeof(ULONG), MEMF_PUBLIC | MEMF_CLEAR))
   {
      *b = ID_BGUI;
      bm = (struct BitMap *)(b + 1);
      InitBitMap(bm, depth, width, height);

      flags = flags & BMF_CLEAR ? MEMF_CHIP : MEMF_CHIP | MEMF_CLEAR;
      rassize = RASSIZE(width, height);
      
      /*
       * Allocate planes.
       */
      for (i = 0; i < depth; i++)
      {
         if (!(bm->Planes[i] = AllocVec(rassize, flags)))
         {
            BGUI_FreeBitMap(bm);
            return NULL;
         };
      };
   }
   return bm;
   #endif
}

/*
 * Free a bitmap.
 */
makeproto SAVEDS ASM VOID BGUI_FreeBitMap(REG(a0) struct BitMap *bm)
{
   #ifdef ENHANCED
   WaitBlit();
   FreeBitMap(bm);
   #else

   ULONG    *b = ((ULONG *)bm) - 1;
   int       i;
   UBYTE    *p;
   
   if (bm)
   {
      /*
       * Wait for the blitter.
       */
      WaitBlit();

      /*
       * Under OS 3.0 we call the system routine.
       */
      if (*b == ID_BGUI)
      {
         for (i = bm->Depth - 1; i >= 0; --i)
         {
            /*
             * Free planes.
             */
            if (p = bm->Planes[i]) FreeVec(p);
         };
         /*
          * Free the structure.
          */
         FreeVec(b);
      }
      else
      {
         FreeBitMap(bm);
      };
   };
   #endif
}

/*
 * Allocate a rastport with bitmap.
 */
makeproto SAVEDS ASM struct RastPort *BGUI_CreateRPortBitMap(REG(a0) struct RastPort *source,
   REG(d0) ULONG width, REG(d1) ULONG height, REG(d2) ULONG depth)
{
   struct RastPort   *rp;
   struct Layer_Info *li;

   /*
    * Allocate a rastport structure.
    */
   if (rp = (struct RastPort *)AllocVec(sizeof(struct RastPort), MEMF_PUBLIC | MEMF_CLEAR))
   {
      /*
       * If we have a source rastport we
       * copy it. Otherwise we initialize
       * the rastport.
       */
      if (source) *rp = *source;
      else        InitRastPort(rp);

      if (!depth && source) depth = FGetDepth(source);

      /*
       * Add a bitmap.
       */
      if (rp->BitMap = BGUI_AllocBitMap(width, height, depth, BMF_CLEAR, source ? source->BitMap : NULL))
      {
         /*
          * NO LAYER!.
          */
         // rp->Layer = NULL;

         if (li = NewLayerInfo())
         {
            if (rp->Layer = CreateUpfrontLayer(li, rp->BitMap, 0, 0, width - 1, height - 1, 0, NULL))
            {
               /*
                * Mark it as a buffered rastport.
                */
               // rp->RP_User = ID_BFRP;

               return rp;
            };
            /*
             * No layer.
             */
            DisposeLayerInfo(li);
         };
         /*
          * No layerinfo.
          */
         BGUI_FreeBitMap(rp->BitMap);
      }
      /*
       * No bitmap.
       */
      FreeVec(rp);
   }
   return NULL;
}

/*
 * Free a buffer rastport and bitmap.
 */
makeproto SAVEDS ASM VOID BGUI_FreeRPortBitMap(REG(a0) struct RastPort *rp)
{
   struct Layer      *l  = rp->Layer;
   struct Layer_Info *li = l->LayerInfo;

   /*
    * Free the layer.
    */
   InstallClipRegion(l, NULL);
   DeleteLayer(NULL, l);
   
   /*
    * Free the layerinfo.
    */
   DisposeLayerInfo(li);
   
   /*
    * Free the bitmap.
    */
   BGUI_FreeBitMap(rp->BitMap);

   /*
    * Free the rastport.
    */
   FreeVec(rp);
}

/*
 * Show AmigaGuide file.
 */
makeproto SAVEDS ASM BOOL BGUI_Help(REG(a0) struct Window *win, REG(a1) UBYTE *file, REG(a2) UBYTE *node, REG(d0) ULONG line)
{
   struct NewAmigaGuide       nag = { NULL };

   /*
    * Initialize structure.
    */
   nag.nag_Name      = ( STRPTR )file;
   nag.nag_Node      = ( STRPTR )node;
   nag.nag_Line      = line;
   nag.nag_Screen    = win ? win->WScreen : NULL;

   /*
    * Show file.
    */
   return( DisplayAGuideInfo( &nag, TAG_END ));
}


/*
 * Set or clear the busy pointer.
 */
STATIC ASM VOID Busy(REG(a0) struct Window *win, REG(d0) BOOL set)
{
   #ifdef ENHANCED

   if (set) SetWindowPointer(win, WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, TAG_END);
   else     SetWindowPointer(win, TAG_END);

   #else

   /*
    * Must be in chip memory (busy pointer on OS 2.04 machines).
    */
   static __chip UWORD BusyPointer[] =
   {
      0x0000, 0x0000, 0x0400, 0x07c0, 0x0000, 0x07c0, 0x0100, 0x0380,
      0x0000, 0x07e0, 0x07c0, 0x1ff8, 0x1FF0, 0x3FEC, 0x3FF8, 0x7FDE,
      0x3FF8, 0x7FBE, 0x7FFC, 0xFF7F, 0x7EFC, 0xFFFF, 0x7FFC, 0xFFFF,
      0x3FF8, 0x7FFE, 0x3FF8, 0x7FFE, 0x1FF0, 0x3FFC, 0x07C0, 0x1FF8,
      0x0000, 0x07E0, 0x0000, 0x0000
   };

   if (OS30)
   {
      if (set) SetWindowPointer(win, WA_BusyPointer, TRUE, WA_PointerDelay, TRUE, TAG_END);
      else     SetWindowPointer(win, TAG_END);
   }
   else
   {
      if (set) SetPointer(win, BusyPointer, 16, 16, -6, 0 );
      else     ClearPointer(win );
   }

   #endif
}

/*
 * Lock a window.
 */
makeproto SAVEDS ASM APTR BGUI_LockWindow( REG(a0) struct Window *win )
{
   WINDOWLOCK        *wl;

   /*
    * Allocate lock structure.
    */
   if ( wl = ( WINDOWLOCK * )BGUI_AllocPoolMem( sizeof( WINDOWLOCK ))) {
      /*
       * Initialize structure.
       */
      wl->wl_Locked   = win;

      /*
       * Copy current min/max sizes.
       */
      *IBOX( &wl->wl_MinWidth ) = *IBOX( &win->MinWidth );

      /*
       * Setup and open requester.
       */
      InitRequester( &wl->wl_IDCMPLock );
      Request( &wl->wl_IDCMPLock, win );

      /*
       * Set busy pointer.
       */
      Busy( win, TRUE );

      /*
       * Disable sizing.
       */
      WindowLimits( win, win->Width, win->Height, win->Width, win->Height );
   }

   return( wl );
}

/*
 * Unlock a window.
 */
makeproto SAVEDS ASM VOID BGUI_UnlockWindow( REG(a0) APTR lock )
{
   WINDOWLOCK        *wl = ( WINDOWLOCK * )lock;

   /*
    * A NULL lock is safe.
    */
   if ( wl ) {
      /*
       * Setup limits.
       */
      WindowLimits( wl->wl_Locked, wl->wl_MinWidth, wl->wl_MinHeight, wl->wl_MaxWidth, wl->wl_MaxHeight );

      /*
       * End request.
       */
      EndRequest( &wl->wl_IDCMPLock, wl->wl_Locked );

      /*
       * Clear busy pointer.
       */
      Busy( wl->wl_Locked, FALSE );

      /*
       * Free lock.
       */
      BGUI_FreePoolMem( wl );
   }
}

makeproto SAVEDS ASM STRPTR BGUI_GetLocaleStr(REG(a0) struct bguiLocale *bl, REG(d0) ULONG id)
{
   STRPTR str = NULL;
   
   struct bguiLocaleStr bls;
   
   if (bl)
   {
      if (bl->bl_LocaleStrHook)
      {
         bls.bls_ID = id;
         str = (STRPTR)CallHookPkt(bl->bl_LocaleStrHook, (void *)bl, (void *)&bls);
      }
      else
      {
         if (LocaleBase) str = GetLocaleStr(bl->bl_Locale, id);
      };
   };
   return str;
}

makeproto SAVEDS ASM STRPTR BGUI_GetCatalogStr(REG(a0) struct bguiLocale *bl, REG(d0) ULONG id, REG(a1) STRPTR str)
{
   struct bguiCatalogStr bcs;

   if (bl)
   {
      if (bl->bl_CatalogStrHook)
      {
         bcs.bcs_ID = id;
         bcs.bcs_DefaultString = str;
         str = (STRPTR)CallHookPkt(bl->bl_CatalogStrHook, (void *)bl, (void *)&bcs);
      }
      else
      {
         if (LocaleBase) str = GetCatalogStr(bl->bl_Catalog, id, str);
      };
   };
   return str;
}
