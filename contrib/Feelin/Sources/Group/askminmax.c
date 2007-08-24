#include "Private.h"

//#define DB_ASKMINMAX_PAGE
//#define DB_ASKMINMAX_ARRAY

/*** Private ***************************************************************/

///AskMinMax_Simple
STATIC void AskMinMax_Simple(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   UWORD w=0,h=0, omw,omh, maxw=0,maxh=0;
   ULONG nObj = 0;

   _sub_declare_all;

   for _each
   {
      if (FF_Area_CanShow & _sub_flags)
      {
         nObj++;

         omw = _sub_minw;
         omh = _sub_minh;

         if (FF_Horizontal & _flags)
         {
            h = MAX(h,omh); w = (FF_Group_SameWidth  & LOD -> Flags) ? MAX(w,omw) : w + omw;
            if (maxw < 10000) maxw += _sub_maxw; maxh = MAX(maxh,_sub_maxh);
         }
         else
         {
            if (maxh < 10000) maxh += _sub_maxh; maxw = MAX(maxw,_sub_maxw);
            w = MAX(w,omw); h = (FF_Group_SameHeight & LOD -> Flags) ? MAX(h,omh) : h + omh;
         }
      }
   }

/* 3: Fix & Set values */

   if (nObj)
   {
      if (FF_Horizontal & _flags)
      {
         _maxw  = (nObj - 1) * LOD -> HSpace + _minw + maxw;
         _minw  = (nObj - 1) * LOD -> HSpace + _minw + ((FF_Group_SameWidth  & LOD -> Flags) ? w * nObj : w);
         _maxh  = _minh + maxh;
         _minh += h;
      }
      else
      {
         _maxw  = _minw + maxw;
         _minw += w;
         _maxh  = (nObj - 1) * LOD -> VSpace + _minh + maxh;
         _minh  = (nObj - 1) * LOD -> VSpace + _minh + ((FF_Group_SameHeight & LOD -> Flags) ? h * nObj : h);
      }
   }
   else
   {
      _maxw = 0; _minw = 0;
      _maxh = 0; _minh = 0;
   }

   if (_maxw > FV_MAXMAX) _maxw = FV_MAXMAX;
   if (_maxh > FV_MAXMAX) _maxh = FV_MAXMAX;
//   WriteF('[33mGroup.AskMinMax[0m - [1mSelf[0m $\h - [1mMin[0m \d, \d - [1mMax[0m \d, \d\n',Obj,_minw(Obj),_minh(Obj),_maxw(Obj),_maxh(Obj))
}
//+
///AskMinMax_Page
STATIC void AskMinMax_Page(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   UWORD minw=NULL,minh=NULL,maxw=NULL,maxh=NULL,aw,ah;
   ULONG n=NULL;

   _sub_declare_all;

   for _each
   {
      n++;

///DB_ASKMINMAX_PAGE
#ifdef DB_ASKMINMAX_PAGE
      F_Log(0,"Min %5ld x %5ld - Max %5ld x %5ld - %s{%08lx}",_sub_minw,_sub_minh,_sub_maxw,_sub_maxh,_classname(sub),sub);
#endif
//+

      if (_sub_minw > minw) minw = _sub_minw;
      if (_sub_minh > minh) minh = _sub_minh;
      if (_sub_maxw > maxw) maxw = _sub_maxw;
      if (_sub_maxh > maxh) maxh = _sub_maxh;
   }

   aw = (LOD -> PageData -> Font -> tf_XSize * 3 + 20) * n;
   ah = LOD -> PageData -> Font -> tf_YSize + 12 + 5;

   _minw += (minw + 20 > aw) ? minw + 20 : aw;
   _minh += minh + ah;
   _maxw  = maxw + 20;
   _maxh  = maxh + ah;
}
//+
///AskMinMax_Array
void AskMinMax_Array(struct FeelinClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   UWORD i,j;
   UWORD samw=0,samh=0;

   _sub_declare_all;

//   LOD -> Members = 0;
   
   for _each
   {
      if (FF_Area_CanShow & _sub_flags)
      {
         if ((FF_Group_SameWidth | FF_Group_SameHeight) & LOD -> Flags)
         {
            samw = MAX(samw,_sub_minw);
            samh = MAX(samh,_sub_minh);
         }
//         LOD -> Members++;
      }
   }

   if (!LOD -> Columns)
   {
      if (LOD -> Members % LOD -> Rows)
      {
         LOD -> Columns = 1; LOD -> Rows = LOD -> Members;
      }
      else LOD -> Columns = LOD -> Members / LOD -> Rows;
   }
   else
   {
      if (LOD -> Members % LOD -> Columns)
      {
         LOD -> Rows = 1; LOD -> Columns = LOD -> Members;
      }
      else LOD -> Rows = LOD -> Members / LOD -> Columns;
   }

   _minw = _maxw = (LOD -> Columns - 1) * LOD -> HSpace;
   _minh = _maxh = (LOD -> Rows    - 1) * LOD -> VSpace;

//   F_Log(0,"\nMIN_MAX_W %ld - MIN_MAX_H %ld",_minw,_minh);

// Compute MinW & MinH if Group has FA_Group_SameXxx

   j = 0; node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head); //sub = LOD -> ChildList.Head;

   for (i = 0 ; i < LOD -> Rows ; i++)
   {
      UWORD minh = 0;
      UWORD maxh = FV_MAXMAX;

      while (node)
      {
         if (FF_Area_CanShow & _sub_flags)
         {
            if (FF_Group_SameHeight & LOD -> Flags)
            {
               _sub_minh = MIN(samh,_sub_maxh);
            }

//            F_Log(0,"Min %5ld x %5ld - Max %5ld x %5ld - %s{%08lx}",_sub_minw,_sub_minh,_sub_maxw,_sub_maxh,_classname(sub),sub);

            minh = MAX(minh,_sub_minh);
            maxh = MIN(maxh,_sub_maxh);

            node = node -> Next;

            if ((++j % LOD -> Columns) == 0)
            {
//               F_Log(0,"ROW %ld END - MinH %ld - MaxH %ld",i,minh,maxh);

               _minh += minh;
               _maxh += MAX(minh,maxh);

               break;
            }
         }
         else
         {
            node = node -> Next;
         }
      }
   }

   j = 0;

   for (i = 0 ; i < LOD -> Columns ; i++)
   {
      UWORD minw = 0;
      UWORD maxw = FV_MAXMAX;

      for _each
      {
         if (FF_Area_CanShow & _sub_flags)
         {
            if (((++j - 1) % LOD -> Columns) == i)
            {
               if (FF_Group_SameWidth & LOD -> Flags)
               {
                  _sub_minw = MIN(samw,_sub_maxw);
               }

               minw = MAX(minw,_sub_minw);
               maxw = MIN(maxw,_sub_maxw);

//               F_Log(0,"(%ld,%ld) SUB 0x%08lx - MinW %5ld (%5ld) MaxW %5ld (%5ld)",i,j,sub,_sub_minw,minw,_sub_maxw,maxw);
            }
         }
      }
      maxw = MAX(maxw, minw); _minw += minw; _maxw += maxw;

//      F_Log(0,"COL(%2ld) - MinW %5ld - MaxW %5ld",i,minw,maxw);
   }

//   F_Log(0,">>> MIN %6ld x %6ld - MAX %6ld x %6ld\n",_minh,_minw,_maxh,_maxw);
}
//+

/*** Methods ***************************************************************/

///Group_AskMinMax
F_METHOD(void,Group_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    _sub_declare_all;

    LOD->Members = 0;
 
    /* ask min & max to all children */

    for _each
    {
        _sub_change;

        _sub_minw = 0; _sub_maxw = FV_MAXMAX;
        _sub_minh = 0; _sub_maxh = FV_MAXMAX;

        F_Do(sub,FM_AskMinMax);
        
        LOD->Members++;
    }

    /* call appropriate minmax function */

    if (LOD -> MinMaxHook)
    {
        CallHookPkt(LOD -> MinMaxHook,Obj,Msg);
    }
    else if (LOD -> PageData)
    {
        AskMinMax_Page(Class,Obj);
    }
    else if (LOD -> Rows > 1 || LOD -> Columns > 1)
    {
        AskMinMax_Array(Class,Obj);
    }
    else
    {
        AskMinMax_Simple(Class,Obj);
    }

    /* FC_Area  will  finish  everything  now,  taking  care  of  FA_MinXxx,
    FA_MaxXxx and FA_FixedXxx */

    F_SUPERDO();
}
//+

