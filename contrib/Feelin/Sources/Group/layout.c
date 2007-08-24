#include "Private.h"

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

struct FeelinLayoutElement
{
    uint16 min;
    uint16 max;
    uint16 dim;
    uint16 weight;
};

/*** Private ***************************************************************/

///code_layout_array
F_HOOK(void,code_layout_array)
{
    struct FeelinClass     *Class = Hook -> h_Data;
    struct LocalObjectData *LOD   = F_LOD(Class,Obj);
    struct FeelinLayoutElement *rowinfo;
    struct FeelinLayoutElement *colinfo;

    ULONG hweight = 0;
    ULONG vweight = 0;
    ULONG i,j;
    WORD total_bonus_h = _ih - (LOD -> Rows - 1)    * LOD -> VSpace;
    WORD total_bonus_w = _iw - (LOD -> Columns - 1) * LOD -> HSpace;
    WORD array_y = _iy;

    _sub_declare_all;

    if (LOD -> Rows == 0 || LOD -> Columns == 0) return;
    if (LOD -> Members % LOD -> Rows)            return;
    if (LOD -> Members % LOD -> Columns)         return;

    if ((rowinfo = F_New((LOD -> Rows + LOD -> Columns) * sizeof (struct FeelinLayoutElement))) != NULL)
    {
        colinfo = (struct FeelinLayoutElement *)((ULONG)(rowinfo) + LOD -> Rows * sizeof (struct FeelinLayoutElement));

///Precalc Rows

        node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head);

        /* for each row */

        for (i = 0 ; i < LOD -> Rows ; i++)
        {
            rowinfo[i].min = 0;
            rowinfo[i].max = FV_MAXMAX;

            j = 0;

            while (node)
            {
                if (FF_Area_CanShow & _sub_flags)
                {
                    rowinfo[i].min     = MAX(rowinfo[i].min, _sub_minh);
                    rowinfo[i].max     = MIN(rowinfo[i].max, _sub_maxh);
                    rowinfo[i].weight += _sub_weight;

                    if ((++j % LOD -> Columns) == 0)
                    {
                        node = node -> Next; break;
                    }
                }
                node = node -> Next;
            }

            rowinfo[i].max = MAX(rowinfo[i].max, rowinfo[i].min);

            // process results for this row

            total_bonus_h -= rowinfo[i].min;

            if (rowinfo[i].min != rowinfo[i].max)
            {
                vweight += rowinfo[i].weight;
            }

//         F_Log(0,"L1_ROW(%3ld) : Min %5ld - Max %5ld Weight %ld",i,rowinfo[i].min,rowinfo[i].max,rowinfo[i].weight);
        }
//+
///Precalc Columns

        // for each col
        for (i = 0 ; i < LOD -> Columns ; i++)
        {
            // min and max widths

            colinfo[i].min = 0;
            colinfo[i].max = FV_MAXMAX;

            j = 0;

            for _each
            {
                if (FF_Area_CanShow & _sub_flags)
                {
                    if (((++j - 1) % LOD -> Columns) == i)
                    {
                        colinfo[i].min     = MAX(colinfo[i].min, _sub_minw);
                        colinfo[i].max     = MIN(colinfo[i].max, _sub_maxw);
                        colinfo[i].weight += _sub_weight;
                    }
                }
            }

            colinfo[i].max = MAX(colinfo[i].max, colinfo[i].min);

            // process results for this col

            total_bonus_w -= colinfo[i].min;

            if (colinfo[i].min != colinfo[i].max)
            {
                hweight += colinfo[i].weight;
            }

//         F_Log(0,"L1_COL(%3ld) : Min %5ld Max %5ld Weight %ld",i,colinfo[i].min,colinfo[i].max,colinfo[i].weight);
        }

//+

        if (vweight == 0) {/* y = total_bonus_h / 2;*/ vweight = 1; }
        if (hweight == 0) {/* x = total_bonus_w / 2;*/ hweight = 1; }

//      F_Log(0,"total_bonus_w %ld - total_bonus_h %ld",total_bonus_w,total_bonus_h);

        if (total_bonus_h < 0)
        {
            F_Log(0,"total_bonus_h %ld - probably minmax problem",total_bonus_h);
            total_bonus_h = 0;
        }

        if (total_bonus_w < 0)
        {
            F_Log(0,"total_bonus_w %ld - probably minmax problem",total_bonus_w);
            total_bonus_w = 0;
        }

///Calc Row & Columns dimensions

        // calc row heights

        for (i = 0 ; i < LOD -> Rows ; i++)
        {
            rowinfo[i].dim = rowinfo[i].min;

            if (rowinfo[i].min != rowinfo[i].max)
            {
                UWORD bonus_h   = total_bonus_h * rowinfo[i].weight / vweight;
                rowinfo[i].dim += bonus_h;
//            F_Log(0,"CLAMP %ld,%ld,%ld",rowinfo[i].dim,rowinfo[i].min,rowinfo[i].max);
                rowinfo[i].dim  = CLAMP(rowinfo[i].dim,rowinfo[i].min,rowinfo[i].max);
                vweight        -= rowinfo[i].weight;
                total_bonus_h  -= bonus_h;
            }

//         F_Log(0,"L2_ROW(%3ld) - %5ld",i,rowinfo[i].dim);
        }

        /* compute columns widths */

        for (i = 0 ; i < LOD -> Columns ; i++)
        {
            colinfo[i].dim = colinfo[i].min;

            if (colinfo[i].min != colinfo[i].max)
            {
                UWORD bonus_w   = total_bonus_w * colinfo[i].weight / hweight;
                colinfo[i].dim += bonus_w;
                colinfo[i].dim  = CLAMP(colinfo[i].dim, colinfo[i].min, colinfo[i].max);
                hweight        -= colinfo[i].weight;
                total_bonus_w  -= bonus_w;
            }

//         F_Log(0,"L2_COL(%3ld) - %5ld",i,colinfo[i].dim);
        }
//+
///Distribute space

        /*
         * pass 2 : distribute space
        */

        node = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head);

        // for each row

        for (i = 0 ; i < LOD -> Rows ; i++)
        {
            // max height for childs in this row
            WORD array_x = _ix;
            UWORD rowh = rowinfo[i].dim;
            j = 0;

            // for each column

            while (node)
            {
                _sub_change;

                if (FF_Area_CanShow & _sub_flags)
                {
                  /* max width for childs in this column */

                    uint16 colw = colinfo[j].dim;
                    uint16 cwidth;
                    uint16 cheight;
                    uint16 x,y;

                    /* center child if col width is bigger than child maxwidth */

                    cwidth = MIN(_sub_maxw, colw);
                    cwidth = MAX(cwidth, _sub_minw);

                    /* center child if row height is bigger than child maxheight */

                    cheight = MIN(_sub_maxh, rowh);
                    cheight = MAX(cheight, _sub_minh);

//               F_Log(0,"%ld : %ld - %ld x %ld - %s{%08lx}", cleft, ctop, cwidth, cheight,_classname(sub),sub);
                                
                    x = array_x + (colw - cwidth) / 2;
                    y = array_y + (rowh - cheight) / 2;
//DAMAGED
                    if ((_sub_x != x) || (_sub_y != y) || (_sub_w != cwidth) || (_sub_h != cheight))
                    {
                        _sub_x = x;
                        _sub_y = y;
                        _sub_w = cwidth;
                        _sub_h = cheight;
                        _sub_flags |= FF_Area_Damaged;
                    }
                
                    F_Do(sub,FM_Layout);
 
//DAMAGED
                    array_x += LOD -> HSpace + colw;

                    if ((++j % LOD -> Columns) == 0)
                    {
                        node = node -> Next; break;
                    }
                }
                node = node -> Next;
            }

//         F_Log(0,"ARRAY_Y %ld >>> %ld (ROWH %ld)",array_y,array_y + LOD -> VSpace + rowh,rowh);

            array_y += LOD -> VSpace + rowh;
        }
//+

        F_Dispose(rowinfo);
    }
//   F_Log(0,"DONE");
}
//+
///code_layout_page
F_HOOK(void,code_layout_page)
{
    struct FeelinClass *Class = Hook -> h_Data;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FAreaNode *node;

    WORD x = _x + 10;
    WORD y = _y + LOD -> PageData -> Font -> tf_YSize + 12;
    WORD w = _w - 20;
    WORD h = _h - LOD -> PageData -> Font -> tf_YSize - 17;

    for _each
    {
        int16 sx = (_sub_minw == _sub_maxw) ? (w - _sub_minw) / 2 + x : x;
        int16 sy = (_sub_minh == _sub_maxh) ? (h - _sub_minh) / 2 + y : y;
        uint16 sw = (_sub_minw == _sub_maxw) ? _sub_minw : w;
        uint16 sh = (_sub_minh == _sub_maxh) ? _sub_maxh : h;
                
        if ((_sub_x != sx) || (_sub_y != sy) || (_sub_w != sw) || (_sub_h != sh))
        {
            _sub_x = sx;
            _sub_y = sy;
            _sub_w = sw;
            _sub_h = sh;
            _sub_flags |= FF_Area_Damaged;
        }
        
        F_Do(node -> Object, FM_Layout);
    }
}
//+

#undef _each
#define _each                                   (node = head ; node ; node  = node -> Next)

///code_layout_horizontal
F_HOOK(void,code_layout_horizontal)
{
    struct FeelinClass *Class = Hook -> h_Data;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode *head;
    uint16 free_w = _iw;
    uint16 free_h = _ih;
    uint16 x = _ix;
    
    uint16 pad=0;
    uint32 total_weight=0;
    uint32 count=0;
    
    _sub_declare_all;

    if ((head = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head)) == NULL)
    {
        return;
    }

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            uint16 h = _sub_fixh ? _sub_minh : ((_sub_maxh < free_h) ? _sub_maxh : free_h);

            if (h != _sub_h)
            {
                _sub_h = h;
                _sub_flags |= FF_Area_Damaged;
            }
        
            count++;

            if (_sub_fixw)
            {
                if (_sub_w != _sub_minw)
                {
                    _sub_w = _sub_minw;
                    _sub_flags |= FF_Area_Damaged;
                }
                
                free_w -= _sub_w;
            }
            else
            {
                _sub_flags |= FF_COMPUTE_W;

                if (FF_Group_RelSizing & LOD -> Flags)
                {
                    _sub_weight = _sub_minw;
                }
                
                total_weight += _sub_weight;
            }
        }
    }

    free_w -= (count - 1) * LOD -> HSpace;

    if (free_w)
    {
        
/* 2.1 : D'abord on va s'assurer  que  la  taille  minimale  et  la  taille
maximale des Objets est bien respectée. */
 
        node = head;
 
        while (node)
        {
            if (FF_COMPUTE_W & _sub_flags)
            {
                uint16 size = free_w * _sub_weight / total_weight;

                if (_sub_minw > size)
                {
                    if (_sub_w != _sub_minw)
                    {
                        _sub_w = _sub_minw;
                        _sub_flags |= FF_Area_Damaged;
                    }
                    
                    _sub_flags &= ~FF_COMPUTE_W;
                }
                else if (_sub_maxw < size)
                {
                    if (_sub_w != _sub_maxw)
                    {
                        _sub_w = _sub_maxw;
                        _sub_flags |= FF_Area_Damaged;
                    }
                     
                    _sub_flags &= ~FF_COMPUTE_W;
                }
                else
                {
                    node = node->Next; continue;
                }

                free_w -= _sub_w; total_weight -= _sub_weight;

                node = head;
            }
            else
            {
                node = node->Next;
            }
        }

/* 2.2: A présent on s'occupe du reste. */

        for _each
        {
            if (FF_COMPUTE_W & _sub_flags)
            {
                uint16 w = free_w * _sub_weight / total_weight;
                
                if (_sub_w != w)
                {
                    _sub_w = w;
                    _sub_flags |= FF_Area_Damaged;
                }
                
                _sub_flags &= ~FF_COMPUTE_W;
                
                free_w -= _sub_w;
                total_weight -= _sub_weight;
            }
        }

        if (free_w)
        {
            FAreaNode *adjust_node=NULL;
            uint16 used_w=0;

            free_w = _iw - (count - 1) * LOD -> HSpace;

            for _each
            {
                if (FF_Area_CanShow & _sub_flags)
                {
                    used_w += _sub_w;

                    if (_sub_fixw == FALSE) adjust_node = node;
                }
            }

            pad = free_w - used_w;

            if (adjust_node)
            {
                adjust_node -> AreaPublic -> Box.w += pad; pad = 0;
            }
        }
    }

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            int16 y = _iy + ((_sub_h < free_h) ? (free_h - _sub_h) / 2 : 0);

            if ((_sub_x != x) || (_sub_y != y))
            {
                _sub_y = y;
                _sub_x = x;
                _sub_flags |= FF_Area_Damaged;
            }

            x += _sub_w + LOD -> HSpace + pad; pad = 0;

            F_Do(node -> Object,FM_Layout);
        }
    }
}
//+
///code_layout_vertical
F_HOOK(void,code_layout_vertical)
{
    struct FeelinClass *Class = Hook -> h_Data;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode *head;
    uint16 free_w = _iw;
    uint16 free_h = _ih;
    uint16 y = _iy;

    uint16 pad=0;
    uint32 total_weight=0;
    uint32 count=0;

    _sub_declare_all;

    if ((head = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head)) == NULL)
    {
        return;
    }

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            uint16 w = _sub_fixw ? _sub_minw : ((_sub_maxw < free_w) ? _sub_maxw : free_w);

            if (w != _sub_w)
            {
                _sub_w = w;
                _sub_flags |= FF_Area_Damaged;
            }

            count++;

            if (_sub_fixh)
            {
                if (_sub_h != _sub_minh)
                {
                    _sub_h = _sub_minh;
                    _sub_flags |= FF_Area_Damaged;
                }

                free_h -= _sub_h;
            }
            else
            {
                _sub_flags |= FF_COMPUTE_H;

                if (FF_Group_RelSizing & LOD -> Flags)
                {
                    _sub_weight = _sub_minh;
                }

                total_weight += _sub_weight;
            }
        }
    }

    free_h -= (count - 1) * LOD -> VSpace;

    if (free_h)
    {

/* 2.1 : D'abord on va s'assurer  que  la  taille  minimale  et  la  taille
maximale des Objets est bien respectée. */

        node = head;

        while (node)
        {
            if (FF_COMPUTE_H & _sub_flags)
            {
                uint16 size = free_h * _sub_weight / total_weight;

                if (_sub_minh > size)
                {
                    if (_sub_h != _sub_minh)
                    {
                        _sub_h = _sub_minh;
                        _sub_flags |= FF_Area_Damaged;
                    }
                    
                    _sub_flags &= ~FF_COMPUTE_H;
                }
                else if (_sub_maxw < size)
                {
                    if (_sub_h != _sub_maxh)
                    {
                        _sub_h = _sub_maxh;
                        _sub_flags |= FF_Area_Damaged;
                    }
                     
                    _sub_flags &= ~FF_COMPUTE_H;
                }
                else
                {
                    node = node->Next; continue;
                }

                free_h -= _sub_h; total_weight -= _sub_weight;

                node = head;
            }
            else
            {
                node = node->Next;
            }
        }

/* 2.2: A présent on s'occupe du reste. */

        for _each
        {
            if (FF_COMPUTE_H & _sub_flags)
            {
                uint16 h = free_h * _sub_weight / total_weight;
                
                if (_sub_h != h)
                {
                    _sub_h = free_h * _sub_weight / total_weight;
                    _sub_flags |= FF_Area_Damaged;
                }
                 
                _sub_flags &= ~FF_COMPUTE_H;

                free_h -= _sub_h;
                total_weight -= _sub_weight;
            }
        }

        if (free_h)
        {
            FAreaNode *adjust_node=NULL;
            uint16 used_h=0;

            free_h = _ih - (count - 1) * LOD -> VSpace;

            for _each
            {
                if (FF_Area_CanShow & _sub_flags)
                {
                    used_h += _sub_h;

                    if (_sub_fixh == FALSE) adjust_node = node;
                }
            }

            pad = free_h - used_h;

            if (adjust_node)
            {
                adjust_node -> AreaPublic -> Box.h += pad; pad = 0;
            }
        }
    }

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            int16 x = _ix + ((_sub_w < free_w) ? (free_w - _sub_w) / 2 : 0);

            if ((_sub_x != x) || (_sub_y != y))
            {
                _sub_y = y;
                _sub_x = x;
                _sub_flags |= FF_Area_Damaged;
            }

            y += _sub_h + LOD -> VSpace + pad; pad = 0;

            F_Do(node -> Object,FM_Layout);
        }
    }
}
//+
#if 0
///code_layout_vertical
F_HOOK(void,code_layout_vertical)
{
    struct FeelinClass *Class = Hook -> h_Data;
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    FAreaNode     *head;
    FAreaNode     *adjustNode=NULL;
    UWORD          grpw,grph,back_GrpH,pad;
    WORD           x=NULL,y=NULL,total_weight=0;
    ULONG          nObj=NULL,nFix=NULL;

    _sub_declare_all;

    if (!(head = (FAreaNode *) F_Get(LOD -> Family,FA_Family_Head))) return;

/* 1: D'abord, voyons ce qu'il y a dans ce groupe */

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            /*_sub_x = 1;*/ _sub_w = FV_COMPUTE_ME;
            /*_sub_y = 1;*/ _sub_h = FV_COMPUTE_ME;

            if (_sub_fixh) { nFix++; }
            else           { nObj++; total_weight += (FF_Group_RelSizing & LOD -> Flags) ? _sub_minh : _sub_weight; }
        }
    }

/* 2: On calcule l'aire utilisable par les Objets. */

    grpw = _iw;
    grph = _ih - ((nObj + nFix - 1) * LOD -> VSpace);

    back_GrpH = grph;

/* 3 *

    On traite d'abord les objets qui ont une aire fixe, et on  enlève  cette
    aire à celle du groupe.

*/

    if (nFix)
    {
        for _each
        {
//         _sub_change;

            if (FF_Area_CanShow & _sub_flags)
            {
                if (_sub_fixh)
                {
                    _sub_w = _sub_fixw ? _sub_minw : grpw;
                    _sub_h = _sub_minh ; grph = grph - _sub_h;
                }
            }
        }
    }

/* 4 *

    D'abord on va s'assurer que la taille minimale et la taille maximale des
    Objets est bien respectée.

*/

    if (nObj)
    {
        FAreaNode *next;

        for (node = head ; node ; node = next)
        {
            next = node -> Next;

            if (FF_Area_CanShow & _sub_flags)
            {
                if (_sub_h == FV_COMPUTE_ME)
                {
                    UWORD size = grph * ((FF_Group_RelSizing & LOD -> Flags) ? _sub_minh : _sub_weight) / total_weight;

                    if (_sub_minh > size)      _sub_h = _sub_minh;
                    else if (_sub_maxh < size) _sub_h = _sub_maxh;
                    else                       continue;

                    grph -= _sub_h ; total_weight -= _sub_weight ; next = head;
                }
            }
        }

/* 5: A présent on s'occupe du reste. */

        for _each
        {
//         _sub_change;

            if (FF_Area_CanShow & _sub_flags)
            {
                if (_sub_fixh == FALSE)
                {
                    _sub_w = _sub_fixw ? _sub_minw : ((_sub_maxw < grpw) ? _sub_maxw : grpw);

                    if (_sub_h == FV_COMPUTE_ME)
                    {
                        _sub_h = grph * ((FF_Group_RelSizing & LOD -> Flags) ? _sub_minh : _sub_weight) / total_weight;
                    }
                }
            }
        }
    }

/* 6: Vérification et ajustage des Objets */

    grph = back_GrpH;
    grpw = _iw;

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            y += _sub_h; x = (x > _sub_w) ? x : _sub_w; if (_sub_fixh == FALSE) adjustNode = node;
        }
    }

    pad = grph - y ;

    if (adjustNode)
    {
        adjustNode -> AreaPublic -> Box.h += pad; pad = NULL;
    }

    y = _iy;

//   F_DebugOut("Group.Layout - Obj %08.lx - Box %3.ld : %3.ld - %3.ld x %3.ld - Pad %ld\n",Obj,x,y,grpw,grph,pad);

    for _each
    {
        if (FF_Area_CanShow & _sub_flags)
        {
            _sub_x = _x + _bl + ((_sub_w < grpw) ? (grpw - _sub_w) / 2 : 0);
            _sub_y = y; y = y + _sub_h + LOD -> VSpace + pad; pad = 0;
    //      F_DebugOut("Group.Layout - Obj 0x%08.lx - Sub 0x%08.lx - Box %3.ld : %3.ld - %3.ld x %3.ld - x %3.ld - y %3.ld\n",Obj,sub,_sub_x,_sub_y,_sub_w,_sub_h,x,y);
        }
    }

    for _each
    {
        _sub_change;

        if (FF_Area_CanShow & _sub_flags)
        {
            F_Do(sub,FM_Layout);
        }
    }
}
//+
#endif

/*** Methods ***************************************************************/

///Group_Layout
F_METHOD(void,Group_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        if (LOD -> LayoutHook)
        {
            CallHookPkt(LOD -> LayoutHook,Obj,Msg);
        }

        if (LOD -> Region)
        {
            DisposeRegion(LOD -> Region); LOD -> Region = NULL;
        }

        #if 0
        {
            FAreaNode *node;
            
            for (node = (FAreaNode *) F_Get(LOD->Family, FA_Family_Head) ; node ; node = node->Next)
            {
                if ((FF_Area_Damaged & _sub_flags) == 0)
                {
                    F_Log(0,"%s{%08lx} no damage (%ld : %ld, %ld x %ld)",_classname(node->Object),node->Object,_sub_x,_sub_y,_sub_w,_sub_h);
                }
            }
        }
        #endif
 
        F_SUPERDO();
    }
}
//+

