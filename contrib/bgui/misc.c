/*
 * @(#) $Header$
 *
 * BGUI library
 * misc.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.4  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.3  2003/01/18 19:09:59  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.2  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.1  2000/05/14 23:32:48  stegerg
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
 * Revision 42.0  2000/05/09 22:09:42  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:46  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.4  1998/12/07 03:07:08  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.3  1998/11/23 14:59:16  mlemos
 * Fixed mistaken initialization of allocated buffer.
 *
 * Revision 41.10.2.2  1998/11/16 20:11:04  mlemos
 * Added missing brackets in the previous changes.
 *
 * Revision 41.10.2.1  1998/11/16 19:59:24  mlemos
 * Replaced AllocVec and FreeVec calls by BGUI_AllocPoolMem and
 * BGUI_FreePoolMem calls respectively.
 *
 * Revision 41.10  1998/02/25 21:12:40  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:07  mlemos
 * Ian sources
 *
 *
 */

#ifdef __AROS__
#include "include/bgui_macros.h"
#endif

#include "include/classdefs.h"

makeproto struct TextAttr Topaz80 =
{
   "topaz.font",
   TOPAZ_EIGHTY,
   FS_NORMAL,
   FPF_ROMFONT
};

//makeproto ASM LONG max(REG(d0) LONG a, REG(d1) LONG b)
makeproto ASM REGFUNC2(LONG, max,
	REGPARAM(D0, LONG, a),
	REGPARAM(D1, LONG, b))
{ return (a > b) ? a : b; }
REGFUNC_END

//makeproto ASM LONG min(REG(d0) LONG a, REG(d1) LONG b)
makeproto ASM REGFUNC2(LONG, min,
	REGPARAM(D0, LONG, a),
	REGPARAM(D1, LONG, b))
{ return (a < b) ? a : b; }
REGFUNC_END

#ifdef __AROS__
#warning No abs function here, since it is 'built in'.
#else
//makeproto ASM LONG abs(REG(d0) LONG a)
makeproto ASM REGFUNC1(LONG, abs,
	REGPARAM(D0, LONG, a))
{ return (a > 0) ? a :-a; }
REGFUNC_END
#endif

//makeproto ASM LONG range(REG(d0) LONG c, REG(d1) LONG a, REG(d2) LONG b)
makeproto ASM REGFUNC3(LONG, range,
	REGPARAM(D0, LONG, c),
	REGPARAM(D1, LONG, a),
	REGPARAM(D2, LONG, b))
{
   if (c < a) return a;
   if (c > b) return b;
   return c;
}
REGFUNC_END

/*
 * Count the number of labels in an array.
 */
//makeproto ASM ULONG CountLabels(REG(a0) UBYTE **labels)
makeproto ASM REGFUNC1(ULONG, CountLabels,
	REGPARAM(A0, UBYTE **, labels))
{
   int n = 0;

   while (labels[n]) n++;

   return (ULONG)(n - 1);
}
REGFUNC_END

/*
 * See if a point is in a box.
 */
//makeproto ASM BOOL PointInBox(REG(a0) struct IBox *box, REG(d0) WORD x, REG(d1) WORD y)
makeproto ASM REGFUNC3(BOOL, PointInBox,
	REGPARAM(A0, struct IBox *, box),
	REGPARAM(D0, WORD, x),
	REGPARAM(D1, WORD, y))
{
   x -= box->Left;
   y -= box->Top;

   return (BOOL)((x >= 0) && (y >= 0) && (x < box->Width) && (y < box->Height));
}
REGFUNC_END

/*
 * Scale a width and height.
 */
makeproto VOID Scale(struct RastPort *rp, UWORD *width, UWORD *height, UWORD dwidth, UWORD dheight, struct TextAttr *font)
{
   struct TextFont *tf;
   int   open = FALSE;
   ULONG fw, fh;  

   /*
    * Setup the font.
    */
   if (font)
   {
      if (tf = BGUI_OpenFont(font))
         open = TRUE;
   }
   else
   {
      tf = rp->Font;
   };

   /*
    * Get font size.
    */
   if (tf->tf_Flags & FPF_PROPORTIONAL)
   {
      fw = (tf->tf_Modulo << 3) / (tf->tf_HiChar - tf->tf_LoChar);
   }
   else
   {
      fw = tf->tf_XSize;
   };
   fh = tf->tf_YSize;

   /*
    * Close font.
    */
   if (open) BGUI_CloseFont(tf);

   /*
    * Scale size;
    */
   *height  = fh + (dheight - 8);
   *width   = fw + (dwidth - 8);
}

/*
 * Map a RAW key to an ansi character.
 */
makeproto WORD MapKey(UWORD code, UWORD qualifier, APTR *iaddress)
{
   struct InputEvent ie = { NULL };
   UBYTE       chr;

   ie.ie_Class     = IECLASS_RAWKEY;
   ie.ie_Code      = code;
   ie.ie_Qualifier     = qualifier;
   ie.ie_EventAddress  = *iaddress;

   if ( MapRawKey( &ie, &chr, 1, NULL ) != 1 )
      return( ~0 );

   return( chr );
}

/*
 * Show online-help requester.
 */
//makeproto ASM VOID ShowHelpReq( REG(a0) struct Window *win, REG(a1) UBYTE *text )
makeproto ASM REGFUNC2(VOID, ShowHelpReq,
	REGPARAM(A0, struct Window *, win),
	REGPARAM(A1, UBYTE *, text))
{
   struct bguiRequest      req = { NULL };

   InitLocale();

   req.br_Title      = LOCSTR( MSG_BGUI_HELP_TITLE );
   req.br_GadgetFormat  = LOCSTR( MSG_BGUI_HELP_CONTINUE );
   req.br_TextFormat = text;
   req.br_Flags      = BREQF_CENTERWINDOW | BREQF_LOCKWINDOW | BREQF_AUTO_ASPECT;

   BGUI_RequestA( win, &req, NULL );
}
REGFUNC_END


/*
 * Re-allocate and format a buffer.
 */
//makeproto ASM UBYTE *DoBuffer(REG(a0) UBYTE *text, REG(a1) UBYTE **buf_ptr, REG(a2) ULONG *buf_len, REG(a3) ULONG *args)
makeproto ASM REGFUNC4(UBYTE *, DoBuffer,
	REGPARAM(A0, UBYTE *, text),
	REGPARAM(A1, UBYTE **, buf_ptr),
	REGPARAM(A2, ULONG *, buf_len),
	REGPARAM(A3, ULONG *, args))
{
   ULONG       len;

   /*
    * Find out the formatted string length.
    */
   len = CompStrlenF( text, args );

   /*
    * Longer than the current buffer?
    */
   if ( len > *buf_len ) {
      /*
       * Deallocate old buffer.
       */
      if ( *buf_ptr )
         BGUI_FreePoolMem( *buf_ptr );
      /*
       * And allocate a new one.
       */
      if ( *buf_ptr = ( UBYTE * )BGUI_AllocPoolMem( len )) {
         /*
          * Setup new buffer length.
          */
         **buf_ptr = '\0';
         *buf_len = len;
      }
      else {
         /*
          * Screw up.
          */
         *buf_len = 0;
         return( text );
      }
   }
   /*
    * Format the string.
    */
   DoSPrintF( *buf_ptr, text, args );
   return( *buf_ptr );
}
REGFUNC_END

/*
 * Set on multiple object.
 */
makeproto VOID DoMultiSet(Tag tag, ULONG data, ULONG count, Object *obj1, ...)
{
   Object         **array = &obj1;
   ULONG         i;

   for (i = 0; i < count; i++)
   {
      if (array[i]) DoSetMethodNG(array[i], tag, data, TAG_END);
   }
}

/*
 * Set a gadget's bounds.
 */
//makeproto ASM VOID SetGadgetBounds(REG(a0) Object *obj, REG(a1) struct IBox *bounds)
makeproto ASM REGFUNC2(VOID, SetGadgetBounds,
	REGPARAM(A0, Object *, obj),
	REGPARAM(A1, struct IBox *, bounds))
{
   DoSetMethodNG(obj, GA_Left,   bounds->Left,    GA_Top,    bounds->Top,
                      GA_Width,  bounds->Width,   GA_Height, bounds->Height, TAG_DONE);
}
REGFUNC_END

/*
 * Set an image's bounds.
 */
//makeproto ASM VOID SetImageBounds(REG(a0) Object *obj, REG(a1) struct IBox *bounds)
makeproto ASM REGFUNC2(VOID, SetImageBounds,
	REGPARAM(A0, Object *, obj),
	REGPARAM(A1, struct IBox *, bounds))
{
   DoSetMethodNG(obj, IA_Left,   bounds->Left,    IA_Top,    bounds->Top,
                      IA_Width,  bounds->Width,   IA_Height, bounds->Height, TAG_DONE);
}
REGFUNC_END


/*
 * Un-map a mapped tag-list.
 */
//makeproto ASM VOID UnmapTags(REG(a0) struct TagItem *tags, REG(a1) struct TagItem *map)
makeproto ASM REGFUNC2(VOID, UnmapTags,
	REGPARAM(A0, struct TagItem *, tags),
	REGPARAM(A1, struct TagItem *, map))
{
   struct TagItem *tag, *tag1, *at = map;

   /*
    * Go through the mapped tag-list.
    */
   while (tag = NextTagItem(&at))
   {
      /*
       * Is this tag in the unmapped list?
       */
      if (tag1 = FindTagItem(tag->ti_Data, tags))
         /*
          * Yes. Unmap this tag.
          */
         tag1->ti_Tag = tag->ti_Tag;
   }
}
REGFUNC_END

/*
 * Create a vector image.
 */
//makeproto ASM Object *CreateVector(REG(a0) struct TagItem *attr)
makeproto ASM REGFUNC1(Object *, CreateVector,
	REGPARAM(A0, struct TagItem *, attr))
{
   struct TagItem    *tag, *tstate = attr;

   while (tag = NextTagItem(&tstate))
   {
      if (VIT_TAG(tag->ti_Tag))
         return BGUI_NewObjectA(BGUI_VECTOR_IMAGE, tag);
   }
   return NULL;
}
REGFUNC_END

/*
 * Fix the tag pointer.
 */
//makeproto ASM struct TagItem *BGUI_NextTagItem(REG(a0) struct TagItem **t)
makeproto ASM REGFUNC1(struct TagItem *, BGUI_NextTagItem,
	REGPARAM(A0, struct TagItem **, t))
{
   struct TagItem *rt, *tag, *tag2, *tstate;

   rt = NextTagItem(t);

   if (rt)
   {
      switch (rt->ti_Tag)
      {
      case GROUP_Member:
         tag2   = rt;
         tstate = rt + 1;
         while (tag = NextTagItem(&tstate))
            tag2 = tag;
         *t = tag2 + 2;
         break;
      };
   };
   return rt;
}
REGFUNC_END
