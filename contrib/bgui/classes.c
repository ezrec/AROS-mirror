/*
 * @(#) $Header$
 *
 * BGUI library
 * classes.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 1.1  1998/02/25 17:07:51  mlemos
 * Ian sources
 *
 *
 */

#include "include/classdefs.h"
#include <dos.h>

makeproto UBYTE *RootClass   = "rootclass";
makeproto UBYTE *ImageClass  = "imageclass";
makeproto UBYTE *GadgetClass = "gadgetclass";
makeproto UBYTE *PropGClass  = "propgclass";
makeproto UBYTE *StrGClass   = "strgclass";

makeproto Class *BGUI_MakeClass(ULONG tag, ...)
{
   return BGUI_MakeClassA((struct TagItem *)&tag);
}

/*
 * Setup a class.
 */
makeproto ASM Class *BGUI_MakeClassA(REG(a0) struct TagItem *tags)
{
   ULONG  old_a4 = (ULONG)getreg(REG_A4);
   ULONG  SuperClass, SuperClass_ID, Class_ID, Flags, ClassSize, ObjectSize;
   ULONG *ClassData;
   Class *cl;

   geta4();

   if ((SuperClass = GetTagData(CLASS_SuperClassBGUI, (ULONG)~0, tags)) == (ULONG)~0)
      SuperClass = GetTagData(CLASS_SuperClass, NULL, tags);
   else
      SuperClass = (ULONG)BGUI_GetClassPtr(SuperClass);

   if (SuperClass)
      SuperClass_ID = NULL;
   else
      SuperClass_ID = GetTagData(CLASS_SuperClassID, (ULONG)"rootclass", tags);

   Class_ID   = GetTagData(CLASS_ClassID, NULL, tags);
   Flags      = GetTagData(CLASS_Flags,      0, tags);
   ClassSize  = GetTagData(CLASS_ClassSize,  0, tags);
   ObjectSize = GetTagData(CLASS_ObjectSize, 0, tags);

   if (ClassData = AllocVec(ClassSize + sizeof(ULONG) * 2, MEMF_CLEAR))
   {
      if (cl = MakeClass((UBYTE *)Class_ID, (UBYTE *)SuperClass_ID, (Class *)SuperClass, ObjectSize, Flags))
      {
         *ClassData++ = GetTagData(CLASS_DFTable, NULL, tags);
         *ClassData++ = old_a4;

         cl->cl_UserData           = (LONG)ClassData;
         cl->cl_Dispatcher.h_Entry = (HOOKFUNC)GetTagData(CLASS_Dispatcher, (ULONG)__GCD, tags);
      }
      else
      {
         FreeVec(ClassData);
         cl = NULL;
      };
   };
   putreg(REG_A4, (LONG)old_a4);
   return cl;
}

makeproto SAVEDS ASM BOOL BGUI_FreeClass(REG(a0) Class *cl)
{
   if (cl)
   {
      FreeVec(((ULONG *)cl->cl_UserData) - 2);
      return FreeClass(cl);
   };
   return FALSE;
}

makeproto ULONG ASM BGUI_GetAttrChart(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct rmAttr *ra)
{
   ULONG           flags = ra->ra_Flags;
   struct TagItem *attr  = ra->ra_Attr;
   ULONG           data;
   UBYTE          *dataspace;
   ULONG           rc;

   if (rc = AsmCoerceMethod(cl, obj, RM_GETATTRFLAGS, attr, flags))
   {
      if (!(rc & RAF_NOGET))
      {
         dataspace = (UBYTE *)INST_DATA(cl, obj) + ((rc >> 16) & 0x07FF);

         if (rc & RAF_BOOL)
         {
            data = (*dataspace >> ((rc >> 27) & 0x07)) & 1;
            if (rc & RAF_SIGNED) data = !data;
         }
         else
         {
            switch (rc & (RAF_SIGNED|RAF_BYTE|RAF_WORD|RAF_LONG|RAF_ADDR))
            {
            case RAF_BYTE:
               data = (ULONG)(*(UBYTE *)dataspace);
               break;
            case RAF_BYTE|RAF_SIGNED:
               data = (LONG)(*(BYTE *)dataspace);
               break;
            case RAF_WORD:
               data = (ULONG)(*(UWORD *)dataspace);
               break;
            case RAF_WORD|RAF_SIGNED:
               data = (LONG)(*(WORD *)dataspace);
               break;
            case RAF_LONG:
               data = (ULONG)(*(ULONG *)dataspace);
               break;
            case RAF_LONG|RAF_SIGNED:
               data = (LONG)(*(LONG *)dataspace);
               break;
            case RAF_ADDR:
               data = (ULONG)dataspace;
               break;
            case RAF_NOP:
               goto no_get;
            };
         };
         *((ULONG *)(attr->ti_Data)) = data;
      }
      no_get:
      if (rc & RAF_CUSTOM)
      {
         AsmCoerceMethod(cl, obj, RM_GETCUSTOM, attr, flags);
      };
      if (rc & RAF_SUPER)
      {
         rc |= AsmDoSuperMethod(cl, obj, RM_GET, attr, flags);
      };
      rc = (rc & 0xFFFF) | RAF_UNDERSTOOD;
   }
   else
   {
      /*
       * Let the superclass have a go at it.
       */
      rc = AsmDoSuperMethodA(cl, obj, (Msg)ra);
   };
   return rc;
}

makeproto ULONG ASM BGUI_SetAttrChart(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct rmAttr *ra)
{
   ULONG           flags = ra->ra_Flags;
   struct TagItem *attr  = ra->ra_Attr;
   ULONG           data;
   UBYTE          *dataspace, flag;
   ULONG           rc;

   if (rc = AsmCoerceMethod(cl, obj, RM_GETATTRFLAGS, attr, flags))
   {
      if ((rc & RAF_NOSET) || ((rc & RAF_NOUPDATE)  && (flags & RAF_UPDATE))
                           || ((rc & RAF_NOINTERIM) && (flags & RAF_INTERIM))
                           || ((rc & RAF_INITIAL)  && !(flags & RAF_INITIAL)))
      {
         rc &= ~RAF_SUPER;
      }
      else
      {
         if (flags & RAF_INITIAL) rc &= ~RAF_SUPER;

         dataspace = (UBYTE *)INST_DATA(cl, obj) + ((rc >> 16) & 0x07FF);
         data      = attr->ti_Data;

         if (rc & RAF_BOOL)
         {
            flag = 1 << ((rc >> 27) & 0x07);
            if (rc & RAF_SIGNED) data = !data;

            if (data) *dataspace |=  flag;
            else      *dataspace &= ~flag;
         }
         else
         {
            switch (rc & (RAF_SIGNED|RAF_BYTE|RAF_WORD|RAF_LONG|RAF_ADDR))
            {
            case RAF_BYTE:
            case RAF_BYTE|RAF_SIGNED:
               *((UBYTE *)dataspace) = (UBYTE)data;
               break;
            case RAF_WORD:
            case RAF_WORD|RAF_SIGNED:
               *((UWORD *)dataspace) = (UWORD)data;
               break;
            case RAF_LONG:
            case RAF_LONG|RAF_SIGNED:
               *((ULONG *)dataspace) = (ULONG)data;
               break;
            case RAF_NOP:
               break;
            };
         };
      };
      if (rc & RAF_CUSTOM)
      {
         AsmCoerceMethod(cl, obj, RM_SETCUSTOM, attr, flags);
      };
      if (rc & RAF_SUPER)
      {
         rc |= AsmDoSuperMethod(cl, obj, RM_SET, attr, flags);
      };
      rc = (rc & 0xFFFF) | RAF_UNDERSTOOD;
   }
   else
   {
      /*
       * Let the superclass have a go at it.
       */
      rc = AsmDoSuperMethod(cl, obj, RM_SET, attr, flags);
   };
   return rc;
}

makeproto ULONG BGUI_PackStructureTag(UBYTE *dataspace, ULONG *pt, ULONG tag, ULONG data)
{
   #ifdef ENHANCED

   struct TagItem tags[2];

   tags[0].ti_Tag  = tag;
   tags[0].ti_Data = data;
   tags[1].ti_Tag  = TAG_DONE;

   return (ULONG)PackStructureTags((APTR)dataspace, pt, tags);

   #else

   ULONG type;
   UBYTE flag;
   ULONG base = *pt++;

   while (type = *pt++)
   {
      if (type == 0xFFFFFFFF)
      {
         base = *pt++;
         continue;
      };
      if (tag == (base + ((type >> 16) & 0x03FF) ))
      {
         if (type & PKCTRL_UNPACKONLY) return 0;

         dataspace += (type & 0x1FFF);

         switch (type & 0x18000000)
         {
         case PKCTRL_BIT:
            flag = 1 << ((type >> 13) & 0x0007);
            if (type & PSTF_SIGNED) data = !data;
            if (data) *dataspace |= flag;
            else      *dataspace &= ~flag;
            break;
         case PKCTRL_UBYTE:
            *((UBYTE *)dataspace) = (UBYTE)data;
            break;
         case PKCTRL_UWORD:
            *((UWORD *)dataspace) = (UWORD)data;
            break;
         case PKCTRL_ULONG:
            *((ULONG *)dataspace) = (ULONG)data;
            break;
         };
         return 1;
      };
   };
   return 0;

   #endif
}

makeproto ULONG BGUI_UnpackStructureTag(UBYTE *dataspace, ULONG *pt, ULONG tag, ULONG *storage)
{
   #ifdef ENHANCED

   struct TagItem tags[2];

   tags[0].ti_Tag  = tag;
   tags[0].ti_Data = (ULONG)storage;
   tags[1].ti_Tag  = TAG_DONE;

   return (ULONG)UnpackStructureTags((APTR)dataspace, pt, tags);

   #else

   ULONG type, data;
   ULONG base = *pt++;

   while (type = *pt++)
   {
      if (type == 0xFFFFFFFF)
      {
         base = *pt++;
         continue;
      };
      if (tag == (base + ((type >> 16) & 0x03FF) ))
      {
         if (type & PKCTRL_PACKONLY) return 0;

         dataspace += (type & 0x1FFF);

         switch (type & 0x98000000)
         {
         case PKCTRL_UBYTE:
            data = (ULONG)(*((UBYTE *)dataspace));
            break;
         case PKCTRL_UWORD:
            data = (ULONG)(*((UWORD *)dataspace));
            break;
         case PKCTRL_ULONG:
            data = (ULONG)(*((ULONG *)dataspace));
            break;
         case PKCTRL_BYTE:
            data = (LONG)(*((BYTE *)dataspace));
            break;
         case PKCTRL_WORD:
            data = (LONG)(*((WORD *)dataspace));
            break;
         case PKCTRL_LONG:
            data = (LONG)(*((LONG *)dataspace));
            break;
         case PKCTRL_BIT:
         case PKCTRL_FLIPBIT:
            data = (*dataspace & (1 << ((type >> 13) & 0x0007))) ? ~0 : 0;
            if (type & PSTF_SIGNED) data = ~data;
            break;
         };
         *storage = data;
         return 1;
      };
   };
   return 0;

   #endif
}


makeproto SAVEDS ULONG ASM BGUI_PackStructureTags(REG(a0) APTR pack, REG(a1) ULONG *packTable, REG(a2) struct TagItem *tagList)
{
   #ifdef ENHANCED

   return PackStructureTags(pack, packTable, tagList);

   #else

   struct TagItem *tstate = tagList, *tag;
   ULONG rc = 0;

   while (tag = NextTagItem(&tstate))
   {
      rc += BGUI_PackStructureTag((UBYTE *)pack, packTable, tag->ti_Tag, tag->ti_Data);
   };
   return rc;

   #endif
}

makeproto SAVEDS ULONG ASM BGUI_UnpackStructureTags(REG(a0) APTR pack, REG(a1) ULONG *packTable, REG(a2) struct TagItem *tagList)
{
   #ifdef ENHANCED

   return UnpackStructureTags(pack, packTable, tagList);

   #else

   struct TagItem *tstate = tagList, *tag;
   ULONG rc = 0;

   while (tag = NextTagItem(&tstate))
   {
      rc += BGUI_UnpackStructureTag((UBYTE *)pack, packTable, tag->ti_Tag, (ULONG *)tag->ti_Data);
   };
   return rc;

   #endif
}

/*
 * Quick GetAttr();
 */
makeproto ASM ULONG Get_Attr(REG(a0) Object *obj, REG(d0) ULONG attr, REG(a1) void *storage)
{
   return AsmDoMethod(obj, OM_GET, attr, storage);
}
makeproto ASM ULONG Get_SuperAttr(REG(a2) Class *cl, REG(a0) Object *obj, REG(d0) ULONG attr, REG(a1) void *storage)
{
   return AsmDoSuperMethod(cl, obj, OM_GET, attr, storage);
}

makeproto ULONG NewSuperObject(Class *cl, Object *obj, struct TagItem *tags)
{
   return AsmDoSuperMethod(cl, obj, OM_NEW, (ULONG)tags, NULL);
}

/*
 * Like SetAttrs();
 */
makeproto ULONG DoSetMethodNG(Object *obj, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_SET, (struct TagItem *)&tag1, NULL);
   else     return 0;
}

/*
 * Like SetAttrs();
 */
makeproto ULONG DoSuperSetMethodNG(Class *cl, Object *obj, Tag tag1, ...)
{
   if (obj) return AsmDoSuperMethod(cl, obj, OM_SET, (struct TagItem *)&tag1, NULL);
   else     return 0;
}

/*
 * Call the OM_SET method.
 */
makeproto ULONG DoSetMethod(Object *obj, struct GadgetInfo *ginfo, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_SET, (struct TagItem *)&tag1, ginfo);
   else     return 0;
}

/*
 * Call the OM_SET method of the superclass.
 */
makeproto ULONG DoSuperSetMethod(Class *cl, Object *obj, struct GadgetInfo *ginfo, Tag tag1, ...)
{
   if (obj) return AsmDoSuperMethod(cl, obj, OM_SET, (struct TagItem *)&tag1, ginfo);
   else     return 0;
}

/*
 * Call the OM_UPDATE method.
 */
makeproto ULONG DoUpdateMethod(Object *obj, struct GadgetInfo *ginfo, ULONG flags, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_UPDATE, (struct TagItem *)&tag1, ginfo, flags);
   else     return 0;
}

/*
 * Call the OM_NOTIFY method.
 */
makeproto ULONG DoNotifyMethod(Object *obj, struct GadgetInfo *ginfo, ULONG flags, Tag tag1, ...)
{
   if (obj) return AsmDoMethod(obj, OM_NOTIFY, (struct TagItem *)&tag1, ginfo, flags);
   else     return 0;
}

/*
 * Call the GM_RENDER method.
 */
makeproto ASM ULONG DoRenderMethod(REG(a0) Object *obj, REG(a1) struct GadgetInfo *ginfo, REG(d0) ULONG redraw)
{
   struct RastPort   *rp;
   ULONG              rc = 0;

   if (obj && (rp = BGUI_ObtainGIRPort(ginfo)))
   {
      rc = AsmDoMethod(obj, GM_RENDER, ginfo, rp, redraw);
      ReleaseGIRPort(rp);
   }
   return rc;
}

/*
 * Forward certain types of messages with modifications.
 */
makeproto ASM ULONG ForwardMsg(REG(a0) Object *s, REG(a1) Object *d, REG(a2) Msg msg)
{
   WORD          *mouse = NULL;
   ULONG          rc, storage;
   struct IBox   *b1 = GADGETBOX(s);
   struct IBox   *b2 = GADGETBOX(d);
   
   /*
    * Get address of mouse coordinates in message.
    */
   switch (msg->MethodID)
   {
   case GM_HITTEST:
   case GM_HELPTEST:
      mouse = (WORD *)&((struct gpHitTest *)msg)->gpht_Mouse;
      break;
   case GM_GOACTIVE:
   case GM_HANDLEINPUT:
      mouse = (WORD *)&((struct gpInput *)msg)->gpi_Mouse;
      break;
   };
   if (!mouse) return 0;

   /*
    * Store the coordinates.
    */
   storage = *(ULONG *)mouse;

   Get_Attr(s, BT_OuterBox, (ULONG *)&b1);
   Get_Attr(d, BT_OuterBox, (ULONG *)&b2);

   /*
    * Adjust the coordinates to be relative to the destination object.
    */
   mouse[0] += b1->Left - b2->Left;
   mouse[1] += b1->Top  - b2->Top;

   /*
    * Send the message to the destination object.
    */
   rc = AsmDoMethodA(d, msg);

   /*
    * Put the coordinates back to what they were originally.
    */
   *(ULONG *)mouse = storage;

   return rc;
}

#define BI_FREE_RP  1
#define BI_FREE_DRI 2

makeproto struct BaseInfo *AllocBaseInfo(ULONG tag1, ...)
{
   return BGUI_AllocBaseInfoA((struct TagItem *)&tag1);
}

makeproto SAVEDS ASM struct BaseInfo *BGUI_AllocBaseInfoA(REG(a0) struct TagItem *tags)
{
   struct BaseInfo   *bi, *bi2;
   struct GadgetInfo *gi = NULL;
   ULONG             *flags;

   if (bi = BGUI_AllocPoolMem(sizeof(struct BaseInfo) + sizeof(ULONG)))
   {
      flags = (ULONG *)(bi + 1);

      if (bi2 = (struct BaseInfo *)GetTagData(BI_BaseInfo, NULL, tags))
      {
         *bi = *bi2;
      };

      if (gi = (struct GadgetInfo *)GetTagData(BI_GadgetInfo, (ULONG)gi, tags))
      {
         *((struct GadgetInfo *)bi) = *gi;
      };

      bi->bi_DrInfo  = (struct DrawInfo *)GetTagData(BI_DrawInfo, (ULONG)bi->bi_DrInfo, tags);
      bi->bi_RPort   = (struct RastPort *)GetTagData(BI_RastPort, (ULONG)bi->bi_RPort,  tags);
      bi->bi_IScreen = (struct Screen *)  GetTagData(BI_Screen,   (ULONG)bi->bi_Screen, tags);
      bi->bi_Pens    = (UWORD *)          GetTagData(BI_Pens,     (ULONG)bi->bi_Pens,   tags);

      if (gi && !bi->bi_RPort)
      {
         if (bi->bi_RPort = ObtainGIRPort(gi))
         {
            *flags |= BI_FREE_RP;
         };
      };

      if (bi->bi_IScreen && !bi->bi_DrInfo)
      {
         if (bi->bi_DrInfo = GetScreenDrawInfo(bi->bi_IScreen))
         {
            *flags |= BI_FREE_DRI;
         };
      };

      if (!bi->bi_Pens)
      {
         if (bi->bi_DrInfo) bi->bi_Pens = bi->bi_DrInfo->dri_Pens;
         else               bi->bi_Pens = DefDriPens;
      };
   }
   return bi;
}

makeproto void FreeBaseInfo(struct BaseInfo *bi)
{
   ULONG *flags = (ULONG *)(bi + 1);

   if (*flags & BI_FREE_RP)  ReleaseGIRPort(bi->bi_RPort);
   if (*flags & BI_FREE_DRI) FreeScreenDrawInfo(bi->bi_IScreen, bi->bi_DrInfo);

   BGUI_FreePoolMem(bi);
}

