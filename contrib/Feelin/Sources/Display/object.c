#include "Private.h"
 
///str_copy_length
uint32 str_copy_length(STRPTR Src,STRPTR Dst,uint32 Max)
{
   uint32 len = (uint32)(Dst);

   if (Src && Dst)
   {
      for ( ; Max && *Src ; Max--)
      {
         *Dst++ = *Src++;
      }
   }

   if (Dst)
   {
      *Dst = 0;
   }

   return (uint32)(Dst) - len;
}
//+

/*

FM_New_______________________________________________________________________

   FA_Display_Screen [I..]

      Crée une interface pour un écran existant.

      FA_Display_Type [I..]

         Spécifie le type de l'écran FA_Display_Screen.

         FV_Display_Public - C'est un écran  public.  Normalement  il  doit
         déjà  être  bloqué  avec  LockPubScreen(),  pour être sûr qu'il ne
         disparaisse pas pendant la création de l'objet. L'écran public est
         bloqué  de  nouveau lors de la méthode FM_New, il sera libéré lors
         de la méthode FM_Dispose.  L'attribut  FA_Display_PublicName  doit
         être fournis avec ce type d'écran.

         FV_Display_Custom - C'est un écran privé. Vous êtes censé être  le
         propriétaire  de  l'écran.  Pas moyen d'utiliser une fonction pour
         bloquer l'écran.

*/

/****************************************************************************
**** Methods ****************************************************************
****************************************************************************/

///D_New
F_METHOD(uint32,D_New)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem *Tags = Msg,item;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Display_Screen:
      {
         LOD -> Screen = (struct Screen *)(item.ti_Data);
      }
      break;

      case FA_Display_Name:
      {
         if ((LOD -> PublicName = F_New(MAXPUBSCREENNAME + 1)) != NULL)
         {
            str_copy_length((STRPTR)(item.ti_Data),LOD -> PublicName,MAXPUBSCREENNAME);
         }
      }
      break;
   }

   if (LOD -> PublicName)
   {
      if ((LOD -> Screen = LockPubScreen(LOD -> PublicName)) != NULL)
      {
         LOD -> UserCount  = 1;
         LOD -> Type       = FV_Display_Public;
      }
   }

   if (LOD -> Screen)
   {
      LOD -> ColorMap   = LOD -> Screen -> ViewPort.ColorMap;
      LOD -> Width      = LOD -> Screen -> Width;
      LOD -> Height     = LOD -> Screen -> Height;
      LOD -> Depth      = GetBitMapAttr(LOD -> Screen -> RastPort.BitMap,BMA_DEPTH);

      if ((LOD -> DrawInfo = GetScreenDrawInfo(LOD -> Screen)) != NULL)
      {
         F_ClassDo(Class,NULL,FM_AddMember,Obj,FV_AddMember_Tail);

         return F_SUPERDO();
      }
   }
   else
   {
      F_Log(FV_LOG_USER,"Screen creation is not yet implemented");
   }

   return NULL;
}
//+
///D_Dispose
F_METHOD(void,D_Dispose)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);

   if (LOD -> DrawInfo)
   {
      F_ClassDo(Class,NULL,FM_RemMember,Obj);
   }

   F_Dispose(LOD -> PublicName); LOD -> PublicName = NULL;

   if (LOD -> Screen)
   {
      if (LOD -> DrawInfo)
      {
         struct in_FeelinColor   *col;
         struct in_FeelinPalette *pal;

         while ((pal = (struct in_FeelinPalette *)(LOD -> PaletteList.Head)) != NULL)
         {
            F_Log(FV_LOG_DEV,"Palette 0x%08lx - UserCount %ld",pal,pal -> UserCount);

            pal -> UserCount = 1;

            F_Do(Obj,FM_RemPalette,pal);
         }

         while ((col = (struct in_FeelinColor *)(LOD -> ColorList.Head)) != NULL)
         {
            F_Log(FV_LOG_DEV,"Color 0x%08lx - UserCount %ld",col,col -> UserCount);

            col -> UserCount = 1;

            F_Do(Obj,FM_RemColor,col);
         }

         FreeScreenDrawInfo(LOD -> Screen,LOD -> DrawInfo); LOD -> DrawInfo = NULL;
      }

      if (LOD -> Type == FV_Display_Public)
      {
         UnlockPubScreen(NULL,LOD -> Screen);
      }

      if (FF_Display_Created & LOD -> Flags)
      {
         CloseScreen(LOD -> Screen); 
      }

      LOD -> Screen = NULL;
   }

   F_SUPERDO();
}
//+
///D_Get
F_METHOD(void,D_Get)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   struct TagItem         *Tags = Msg;
   struct TagItem          item;
   BOOL                    up=FALSE;

   while  (F_DynamicNTI(&Tags,&item,Class))
   switch (item.ti_Tag)
   {
      case FA_Display_ColorMap:     F_STORE(LOD -> ColorMap);    break;
      case FA_Display_Depth:        F_STORE(LOD -> Depth);       break;
      case FA_Display_BitMap:       F_STORE(LOD -> Screen -> RastPort.BitMap);  break;
      case FA_Display_DrawInfo:     F_STORE(LOD -> DrawInfo);    break;
//      case FA_Display_Font:         F_STORE(LOD -> Font);        break;
      case FA_Display_Height:       F_STORE(LOD -> Height);      break;
      case FA_Display_Name:         F_STORE(LOD -> PublicName);  break;
//      case FA_Display_Pens:         F_STORE(&LOD -> ColorList);  break;
      case FA_Display_Screen:       F_STORE(LOD -> Screen);      break;
      case FA_Display_Title:        F_STORE(LOD -> ScreenName);  break;
      case FA_Display_Width:        F_STORE(LOD -> Width);       break;
      default:                      up = TRUE;
   }

   if (up) F_SUPERDO();
}
//+

/* The linking mecanism is deprecated. I should use a  FC_Family  object...
waiting for a DisplayServer */

///D_AddMember
F_METHODM(int32,D_AddMember,FS_AddMember)
{
   struct LocalObjectData *new_lod = F_LOD(Class,Msg -> Object);

   F_Do(CUD -> Semaphore,FM_Lock,FF_Lock_Exclusive);

   if (Msg -> Object)
   {
      FObject cur;
      struct LocalObjectData *cur_lod;

      for (cur = CUD -> DisplayList.Head ; cur ; cur = cur_lod -> Next)
      {
         cur_lod = F_LOD(Class,cur);

         if (cur == Msg -> Object)
         {
            F_Log(FV_LOG_DEV,"%s{%08lx} already member",_classname(Msg -> Object),Msg -> Object);

            goto __error;
         }
      }
/*
      if (!F_Do(Msg -> Object,FM_Connect,Obj))
      {
         F_Log(FV_LOG_DEV,"Unable to connect %s{%08lx}",_classname(Msg -> Object),Msg -> Object);

         return FALSE;
      }
*/
      if (CUD -> DisplayList.Tail)
      {
         struct LocalObjectData *nxt_lod;
         struct LocalObjectData *prv_lod; /* needed to be defined here because to goto */

         switch (Msg -> Position)
         {
            case FV_AddMember_Head:
            {
__add_head:
               nxt_lod = F_LOD(Class,CUD -> DisplayList.Head);

               nxt_lod -> Prev      = Msg -> Object;
               new_lod -> Next      = CUD -> DisplayList.Head;
               CUD -> DisplayList.Head = Msg -> Object;
            }
            break;

            case FV_AddMember_Tail:
            {
__add_tail:
               prv_lod = F_LOD(Class,CUD -> DisplayList.Tail);

               prv_lod -> Next      = Msg -> Object;
               new_lod -> Prev      = CUD -> DisplayList.Tail;
               CUD -> DisplayList.Tail = Msg -> Object;
            }
            break;

            case FV_AddMember_Insert:
            {
               if (Msg -> Previous)
               {
                  prv_lod = F_LOD(Class,Msg -> Previous);

                  if (prv_lod -> Next)
                  {
                     nxt_lod = F_LOD(Class,prv_lod -> Next);

                     new_lod -> Next = prv_lod -> Next;
                     new_lod -> Prev = Msg -> Previous;
                     prv_lod -> Next = Msg -> Object;
                     nxt_lod -> Prev = Msg -> Object;
                  }
                  else goto __add_tail;
               }
               else goto __add_head;
            }
            break;

            default:
            {
               F_Log(0,"Invalid Position %ld",Msg -> Position); goto __error;
            };
         }
      }
      else
      {
         CUD -> DisplayList.Head = Msg -> Object;
         CUD -> DisplayList.Tail = Msg -> Object;
      }

      F_Do(CUD -> Semaphore,FM_Unlock);

      return TRUE;
   }

__error:

   F_Do(CUD -> Semaphore,FM_Unlock);

   return FALSE;
}
//+
///D_RemMember
F_METHODM(int32,D_RemMember,FS_RemMember)
{
   struct LocalObjectData *old_lod = F_LOD(Class,Msg -> Member);

//   F_Log(0,"HEAD 0x%08lx - TAIL 0x%08lx - NEW 0x%08lx (%s)",CUD -> DisplayList.Head,CUD -> DisplayList.Tail,Msg -> Member,_classname(Msg -> Member));

   F_Do(CUD -> Semaphore,FM_Lock,FF_Lock_Exclusive);

   if (Msg -> Member)
   {
      FObject cur;
      struct LocalObjectData *cur_lod;

      for (cur = CUD -> DisplayList.Head ; cur ; cur = cur_lod -> Next)
      {
         cur_lod = F_LOD(Class,cur);

         if (cur == Msg -> Member)
         {
            break;
         }
      }

      if (!cur)
      {
         F_Log(FV_LOG_DEV,"Unknown member: %s{%08lx}",_classname(Msg -> Member),Msg -> Member);

         goto __error;
      }

//      F_Do(Msg -> Member,FM_Disconnect,Obj);

      if (CUD -> DisplayList.Head == Msg -> Member)         // [OO,??] --OO??
      {
         if (CUD -> DisplayList.Tail == Msg -> Member)      // [OO,OO] --OO--
         {
            CUD -> DisplayList.Head = NULL;
            CUD -> DisplayList.Tail = NULL;
         }
         else                                            // [OO,oo] --OOoo
         {
            struct LocalObjectData *nxt_lod = F_LOD(Class,old_lod -> Next);

            CUD -> DisplayList.Head = old_lod -> Next;
            nxt_lod -> Prev = NULL;
         }
      }
      else if (CUD -> DisplayList.Tail == Msg -> Member)    // [oo,OO] ooOO--
      {
         struct LocalObjectData *prv_lod = F_LOD(Class,old_lod -> Prev);

         CUD -> DisplayList.Tail = old_lod -> Prev;
         prv_lod -> Next = NULL;
      }
      else                                               // [oo,oo] ooOOoo
      {
         struct LocalObjectData *prv_lod = F_LOD(Class,old_lod -> Prev);
         struct LocalObjectData *nxt_lod = F_LOD(Class,old_lod -> Next);

         prv_lod -> Next = old_lod -> Next;
         nxt_lod -> Prev = old_lod -> Prev;
      }

      old_lod -> Next = NULL;
      old_lod -> Prev = NULL;

//      F_Log(0,"HEAD 0x%08lx - TAIL 0x%08lx - OBJ 0x%08lx",CUD -> DisplayList.Head,CUD -> DisplayList.Tail,Msg -> Member);

      F_Do(CUD -> Semaphore,FM_Unlock);

      return TRUE;
   }

__error:

   F_Do(CUD -> Semaphore,FM_Unlock);

   return FALSE;
}
//+

/* The following methods should  only  be  invoked  on  the  class  itself.
Displays are shared objects, but due to Feelin OOS limitation these methods
need to be called to create (or share) a FC_Display object. Thus, the 'Obj'
variable  has no use. These methods are only valid until a DisplayServer is
created. */

struct FS_Display_Find                       { STRPTR Name; };
struct FS_Display_Delete                     { FObject Display; };

///D_Find
F_METHODM(FObject,D_Find,FS_Display_Find)
{
   struct LocalObjectData *LOD;

   F_Do(CUD -> Semaphore,FM_Lock,FF_Lock_Exclusive);

   for (Obj = CUD -> DisplayList.Head ; Obj ; Obj = LOD -> Next)
   {
      LOD = F_LOD(Class,Obj);

      if (F_StrCmp(Msg -> Name,LOD -> PublicName,ALL) == 0)
      {
         break;
      }
   }

   F_Do(CUD -> Semaphore,FM_Unlock);

   return Obj;
}
//+
///D_Create
F_METHOD(FObject,D_Create)
{
    STRPTR name = F_New(MAXPUBSCREENNAME + 1);

    if (name != NULL)
    {
        GetDefaultPubScreen(name);
    }

    Obj = (FObject) F_ClassDo(Class,NULL,F_IDM(FM_Display_Find),name);

    if (Obj != NULL)
    {
        struct LocalObjectData *LOD = F_LOD(Class,Obj);

        LOD -> UserCount++;
    }
    else
    {
        struct Screen *scr = LockPubScreen(name);

        if (scr != NULL)
        {
            Obj = F_NewObj(Class -> Name,F_IDA(FA_Display_Name),name,TAG_DONE);

            UnlockPubScreen(NULL,scr);
        }
    }

    F_Dispose(name);

    return Obj;
}
//+
///D_Delete
F_METHODM(void,D_Delete,FS_Display_Delete)
{
   struct LocalObjectData *LOD;

   F_Do(CUD -> Semaphore,FM_Lock,FF_Lock_Exclusive);

//   F_DebugOut(0,"DELETE 0x%08lx - HEAD 0x%08lx\n",Msg -> Display,CUD -> DisplayList.Head);

   for (Obj = CUD -> DisplayList.Head ; Obj ; Obj = LOD -> Next)
   {
      LOD = F_LOD(Class,Obj);

//      F_DebugOut(0,"SEARCH 0x%08lx ?= 0x%08lx\n",Obj,Msg -> Display);

      if (Msg -> Display == Obj)
      {
         if (--LOD -> UserCount == 0)
         {
            F_DisposeObj(Obj);
         }

         break;
      }
   }

   if (!Obj)
   {
      F_Log(FV_LOG_DEV,"Unknown Display 0x%08lx",Obj);
   }

   F_Do(CUD -> Semaphore,FM_Unlock);
}
//+

