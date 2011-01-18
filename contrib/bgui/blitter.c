/*
 * @(#) $Header$
 *
 * BGUI library
 * blitter.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.5  2003/01/18 19:09:55  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.4  2000/07/03 21:21:00  bergers
 * Replaced stch_l & stcu_d and had to make a few changes in other places because of that.
 *
 * Revision 42.3  2000/07/02 06:08:33  bergers
 * Compiles library alright (except that I took stch_l & stcu_d out) and seems to create the right type of object. Test1 also compiles alright but crashes somewhere...
 *
 * Revision 42.2  2000/05/15 19:27:00  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:46  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:08:29  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:58  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.3  1999/08/03 05:11:03  mlemos
 * Ensured that the creation of the drag and drop screen buffer bitmap is
 * allocated and friend of the screen's bitmap.
 *
 * Revision 41.10.2.2  1999/07/26 16:36:58  mlemos
 * Prevented that a drag session starts before the previous one ends
 * completely.
 *
 * Revision 41.10.2.1  1998/03/02 23:46:18  mlemos
 * Switched vector allocation functions calls to BGUI allocation functions.
 *
 * Revision 41.10  1998/02/25 21:11:40  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:39  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"

//makeproto VOID ASM EraseBMO(REG(a0) BMO *bmo)
makeproto ASM REGFUNC1(VOID, EraseBMO,
	REGPARAM(A0, BMO *, bmo))
{
   struct Screen *s = bmo->bmo_Screen;

   ObtainSemaphore(&bmo->bmo_Lock);

   if (bmo->bmo_BMWindow)
   {
      /*
       * Signal to kill layer.
       */
      Signal((struct Task *)bmo->bmo_Process, SIGBREAKF_CTRL_E);
   };

   if (bmo->bmo_Locked)
   {
      BltBitMap(bmo->bmo_ScreenBuffer, 0, 0, s->RastPort.BitMap,
         bmo->bmo_LX, bmo->bmo_LY, bmo->bmo_CW, bmo->bmo_CH, 0xC0, 0xFF, NULL);
      UnlockLayerInfo(&s->LayerInfo);
      bmo->bmo_Locked = FALSE;
   };

   ReleaseSemaphore(&bmo->bmo_Lock);
}
REGFUNC_END

//STATIC VOID ASM SAVEDS backfill_func(REG(a0) struct Hook *hook, REG(a2) struct RastPort *rp, REG(a1) BFINFO *bf)
STATIC SAVEDS ASM REGFUNC3(VOID, backfill_func,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, struct RastPort *, rp),
	REGPARAM(A1, BFINFO *, bf))
{
   BMO *bmo = (BMO *)hook->h_Data;

   int x = bf->bf_Rect.MinX;
   int y = bf->bf_Rect.MinY;
   int w = bf->bf_Rect.MaxX - x + 1;
   int h = bf->bf_Rect.MaxY - y + 1;

   BltBitMap(bmo->bmo_ObjectBuffer, bf->bf_X, bf->bf_Y, rp->BitMap, x, y, w, h, 0xC0, 0xFF, NULL);
}
REGFUNC_END

static struct Hook bf_hook = { NULL, NULL, (FUNCPTR)backfill_func, NULL, NULL };

//makeproto VOID ASM LayerBMO(REG(a0) BMO *bmo)
makeproto ASM REGFUNC1(VOID, LayerBMO,
	REGPARAM(A0, BMO *, bmo))
{
   if (!bmo->bmo_BMWindow)
   {
      EraseBMO(bmo);

      bf_hook.h_Data = bmo;
      /*
       * Signal to open layer.
       */
      Signal((struct Task *)bmo->bmo_Process, SIGBREAKF_CTRL_F);
   };
}
REGFUNC_END

//makeproto VOID ASM DrawBMO(REG(a0) BMO *bmo)
makeproto ASM REGFUNC1(VOID, DrawBMO,
	REGPARAM(A0, BMO *, bmo))
{
   struct Screen     *s  = bmo->bmo_Screen;
   struct BitMap     *bm = bmo->bmo_ObjectBuffer;
   struct RastPort   *r  = &s->RastPort;
   
   int x, y, w, h;

   ObtainSemaphore(&bmo->bmo_Lock);

   if (!bmo->bmo_BMWindow)
   {
      w = bmo->bmo_CW;
      h = bmo->bmo_CH;

      if (bmo->bmo_Locked)
      {
         BltBitMap(bmo->bmo_ScreenBuffer, 0, 0, r->BitMap,
         bmo->bmo_LX, bmo->bmo_LY, w, h, 0xC0, 0xFF, NULL);
      }
      else
      {
         bmo->bmo_Locked = TRUE;
         LockLayerInfo(&s->LayerInfo);
      };

      x = bmo->bmo_LX = bmo->bmo_CX;
      y = bmo->bmo_LY = bmo->bmo_CY;
   
      BltBitMap(r->BitMap, x, y, bmo->bmo_ScreenBuffer, 0, 0, w, h, 0xC0, 0xFF, NULL);
      BltBitMap(bm, 0, 0, r->BitMap, x, y, w, h, 0xC0, 0xFF, NULL);
   };

   ReleaseSemaphore(&bmo->bmo_Lock);
}  
REGFUNC_END

static void KillDragObject(BMO *bmo)
{
   struct bmFreeDragObject bmfo;
   
   if (bmo)
   {
      BGUI_FreeBitMap(bmo->bmo_ScreenBuffer);
   
      bmfo.MethodID = BASE_FREEDRAGOBJECT;
   
      if (bmfo.bmfo_ObjBitMap = bmo->bmo_ObjectBuffer)
         BGUI_DoGadgetMethodA(bmo->bmo_Object, bmo->bmo_Window, NULL, (Msg)&bmfo);
      
      /*
       * Nuke the structure.
       */
      BGUI_FreePoolMem(bmo);
   };
}

static void openwin(BMO *bmo)
{
   if (bmo->bmo_BMWindow)
   {
      ChangeWindowBox(bmo->bmo_BMWindow, bmo->bmo_CX, bmo->bmo_CY, bmo->bmo_CW, bmo->bmo_CH);
   }
   else
   {
      bmo->bmo_BMWindow = OpenWindowTags(NULL,
                                         WA_Left,         bmo->bmo_CX,      WA_Top,       bmo->bmo_CY,
                                         WA_Width,        bmo->bmo_CW,      WA_Height,    bmo->bmo_CH,
                                         WA_CustomScreen, bmo->bmo_Screen,  WA_BackFill,  &bf_hook,
                                         WA_Borderless,   TRUE,             TAG_DONE);
   };
}

static void killwin(BMO *bmo)
{
   if (bmo->bmo_BMWindow)
   {
      CloseWindow(bmo->bmo_BMWindow);
      bmo->bmo_BMWindow = NULL;
   };
}

static BMO *last_bmo=NULL;

__saveds void Mover(void)
{
   BMO               *bmo;
   ULONG              sigs;
   BOOL               go;
   struct IBox       *db;
   Object            *obj, *wo, *g;
   struct Window     *w;
   struct Screen     *s;
   struct bmDragPoint bmd;

#ifdef __AROS__
   if (0 == sscanf(((struct Process *)FindTask(NULL))->pr_Arguments, "%lx", &bmo))
#else
   if (stch_l(((struct Process *)FindTask(NULL))->pr_Arguments, (long *)&bmo) == 0)
#endif
   {
      go = FALSE;
   }
   else
   {
      go  = TRUE;
      obj = bmo->bmo_Object;
      s   = bmo->bmo_Screen;

      bmd.MethodID    = BASE_DRAGQUERY;
      bmd.bmdp_GInfo  = NULL;
      bmd.bmdp_Source = obj;
   };

   while (go)
   {
      sigs = Wait(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F);
      
      if (sigs & SIGBREAKF_CTRL_F)  /* Make layer  */
      {
         openwin(bmo);
      };
      if (sigs & SIGBREAKF_CTRL_E)  /* Kill layer  */
      {
         killwin(bmo);
      };
      if (sigs & SIGBREAKF_CTRL_D)  /* Move object */
      {
         killwin(bmo);
         /*
          * When we still have an active target
          * we do not change the window.
          */
         if (!bmo->bmo_ActRec)
         {
            /*
             * Did the window change?
             */
            if ((wo = WhichWindow(s)) != bmo->bmo_ActWin)
            {
               /*
                * Unlock the previous window.
                */
               if (bmo->bmo_ActWin) AsmDoMethod(bmo->bmo_ActWin, WM_UNLOCK);

               /*
                * Setup new window object and window pointer.
                */
               bmo->bmo_ActWin = wo;
               bmo->bmo_ActPtr = NULL;
               if (wo)
               {
                  AsmDoMethod(wo, WM_LOCK);
                  Get_Attr(wo, WINDOW_Window, (ULONG *)&bmo->bmo_ActPtr);
               };
            };
         };

         /*
          * Are we located on a BGUI window?
          */
         if (wo = bmo->bmo_ActWin)
         {
            w = bmo->bmo_ActPtr;

            /*
             * Do we have a target?
             */
            if (!bmo->bmo_ActRec)
            {
               /*
                * Get the object under the mouse.
                */
               g = (Object *)AsmDoMethod(wo, WM_WHICHOBJECT);

               /*
                * Deactive any active receiver which might
                * still exist.
                */
               if (bmo->bmo_ActRec)
               {
                  if (!bmo->bmo_BMWindow) EraseBMO(bmo);
                  myDoGadgetMethod(bmo->bmo_ActRec, w, NULL, BASE_DRAGINACTIVE, NULL, obj);
                  bmo->bmo_ActRec = NULL;
               };

               /*
                * Is the new object valid?
                */
               if (g)
               {
                  /*
                   * Get the hitbox bounds of the
                   * receiver.
                   */
                  Get_Attr(g, BT_HitBox, &db);

                  /*
                   * Get mouse coords relative to the
                   * receiver hitbox.
                   */
                  bmd.bmdp_Mouse.X = s->MouseX - (w->LeftEdge + db->Left);
                  bmd.bmdp_Mouse.Y = s->MouseY - (w->TopEdge  + db->Top);

                  /*
                   * Ask if it is willing to receive us.
                   */
                  if (BGUI_DoGadgetMethodA(g, w, NULL, (Msg)&bmd) == BQR_ACCEPT)
                  {
                     /*
                      * Yes. Tell it to go active.
                      */
                     if (!bmo->bmo_BMWindow) EraseBMO(bmo);
                     bmo->bmo_ActRec = g;
                     myDoGadgetMethod(g, w, NULL, BASE_DRAGACTIVE, NULL, obj);
                  };
               };
            };
         };
         DrawBMO(bmo);
      };
      if (sigs & SIGBREAKF_CTRL_C)  /* Kill object */
      {
         killwin(bmo);
         EraseBMO(bmo);
         go = FALSE;
      };
   };
   
   /*
    * Tell the object to dispose of the
    * created object bitmap.
    */
   KillDragObject(bmo);
   last_bmo=NULL;
}

/*
 * Setup everything to move a chunk of the screen
 * around on the screen.
 */

//makeproto ASM BMO *CreateBMO(REG(a0) Object *obj, REG(a1) struct GadgetInfo * gi)
makeproto ASM REGFUNC2(BMO *, CreateBMO,
	REGPARAM(A0, Object *, obj),
	REGPARAM(A1, struct GadgetInfo *, gi))
{
   struct Screen     *scr = gi->gi_Screen;
   struct Window     *win = gi->gi_Window;
   struct RastPort   *rp = &scr->RastPort;
   BMO               *bmo;
   int                w, h, depth;
   struct IBox        bounds;
   char               args[10];

   if (last_bmo==NULL
   && (bmo=last_bmo= BGUI_AllocPoolMem(sizeof(BMO))))
   {
      if (bmo->bmo_ObjectBuffer = (struct BitMap *)AsmDoMethod(obj, BASE_GETDRAGOBJECT, gi, &bounds))
      {
         depth = FGetDepth(rp);

         bmo->bmo_CW = w = bounds.Width;
         bmo->bmo_CH = h = bounds.Height;

         /*
          * Setup mouse position.
          */
         bmo->bmo_CX = scr->MouseX;
         bmo->bmo_CY = scr->MouseY;

         bmo->bmo_IX = bmo->bmo_CX - bounds.Left - win->LeftEdge;
         bmo->bmo_IY = bmo->bmo_CY - bounds.Top  - win->TopEdge;

         /*
          * Maximum mouse positions in which a move is still performed.
          */
         bmo->bmo_MaxX = scr->Width  - w;
         bmo->bmo_MaxY = scr->Height - h;

         bmo->bmo_Screen = scr;
         bmo->bmo_Window = win;
         bmo->bmo_Object = obj;

         if (bmo->bmo_ScreenBuffer = BGUI_AllocBitMap(w, h, depth, 0, scr->RastPort.BitMap))
         {
            InitSemaphore(&bmo->bmo_Lock);
            sprintf(args, "%08lx\n", bmo);

            if (bmo->bmo_Process = CreateNewProcTags(NP_Entry, Mover, NP_Priority, 5,
               NP_Name, "BGUI Process", NP_Arguments, args, TAG_DONE))
            {
               return bmo;
            };
         };
      };
      KillDragObject(bmo);
      last_bmo=NULL;
   }
   return NULL;
}
REGFUNC_END

/*
 * Cleanup the mess we made.
 */
//makeproto ASM VOID DeleteBMO(REG(a0) BMO *bmo)
makeproto ASM REGFUNC1(VOID, DeleteBMO,
	REGPARAM(A0, BMO *, bmo))
{
   /*
    * Signal termination.
    */
   Signal((struct Task *)bmo->bmo_Process, SIGBREAKF_CTRL_C);
}
REGFUNC_END

/*
 * Move the chunk to a new location.
 */

//makeproto ASM VOID MoveBMO(REG(a0) BMO *bmo, REG(D0) WORD x, REG(D1) WORD y)
makeproto ASM REGFUNC3(VOID, MoveBMO,
	REGPARAM(A0, BMO *, bmo),
	REGPARAM(D0, WORD, x),
	REGPARAM(d1, WORD, y))
{
   /*
    * Make sure we stay inside the
    * screen range.
    */
   bmo->bmo_CX = range(x - bmo->bmo_IX, 0, bmo->bmo_MaxX);
   bmo->bmo_CY = range(y - bmo->bmo_IY, 0, bmo->bmo_MaxY);

   /*
    * Signal motion.
    */
   Signal((struct Task *)bmo->bmo_Process, SIGBREAKF_CTRL_D);
}
REGFUNC_END
