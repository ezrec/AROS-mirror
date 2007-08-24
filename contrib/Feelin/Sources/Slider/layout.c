#include "Private.h"

///slider_cache_update
void slider_cache_update(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

//   F_Log(0,"cache_update - kw (%ld)(%ld) kh (%ld)(%ld)",LOD->kw,_knob_w,LOD->kh,_knob_h);

    if (LOD->OwnRender && _rp)
    {
        if ((LOD->kw != _knob_w) || (LOD->kh != _knob_h) || (!LOD->OwnBitMap))
        {

            /* Setting  OwnRender  rastport  to  the  default  rastport  is  a
            fallback e.i. if bitmap allocation fails rendering will be done on
            the default rastport. OwnBitMap is checked to know if rendering is
            buffered. */

            if (LOD->OwnBitMap)
            {
                FreeBitMap(LOD->OwnBitMap); LOD->OwnBitMap = NULL;
            }
        
            LOD->OwnBitMap = AllocBitMap(LOD->kw,LOD->kh,GetBitMapAttr(_rp->BitMap,BMA_DEPTH),BMF_CLEAR,_rp->BitMap);

            if (LOD->OwnBitMap)
            {
                LOD->OwnRPort->BitMap = LOD->OwnBitMap;
            }
            else
            {
                LOD->OwnRPort->BitMap = _rp->BitMap;
            }
        }
    }

    if (LOD->OwnBitMap)
    {
        F_Layout(LOD->Knob,0,0,LOD->kw,LOD->kh,0);
    }
    else
    {
        F_Layout(LOD->Knob,LOD->kx,LOD->ky,LOD->kw,LOD->kh,0);
    }
}
//+

/***************************************************************************/
/*** Methods ***************************************************************/
/***************************************************************************/

///Slider_AskMinMax
F_METHOD(void,Slider_AskMinMax)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    uint16 maxw = 0, maxh = 0;
    uint32 num_min,num_max,num_val,num_array;

    F_Set(_render,FA_Render_Forbid,TRUE);
    F_Set(LOD->OwnRender,FA_Render_Forbid,TRUE);
        
    F_Do(Obj,FM_Get,
        
        F_IDR(FA_Numeric_Value), &num_val,
        F_IDR(FA_Numeric_Min), &num_min,
        F_IDR(FA_Numeric_Max), &num_max,
        F_IDR(FA_Numeric_StringArray), &num_array,
        
        TAG_DONE);
        
    if (num_array)
    {
        for ( ; num_min < num_max + 1; num_min++)
        {
            F_Do(LOD->Knob, FM_Set,
            
                FA_Text, F_Do(Obj, F_IDR(FM_Numeric_Stringify), num_min),
                FA_NoNotify, TRUE,
                
                TAG_DONE);
                
            _knob_minw = 0; _knob_maxw = FV_MAXMAX;
            _knob_minh = 0; _knob_maxh = FV_MAXMAX;
     
            F_Do(LOD->Knob,FM_AskMinMax);
            
            maxw = MAX(maxw, _knob_minw);
            maxh = MAX(maxh, _knob_minh);
        }
    }
    else
    {
        F_Do(LOD->Knob,FM_Set,
            
            FA_Text,     F_Do(Obj,F_IDR(FM_Numeric_Stringify),num_min),
            FA_NoNotify, TRUE,
            
            TAG_DONE);

        _knob_minw = 0; _knob_maxw = FV_MAXMAX;
        _knob_minh = 0; _knob_maxh = FV_MAXMAX;
        
        F_Do(LOD->Knob,FM_AskMinMax);
        
        maxw = MAX(maxw, _knob_minw);
        maxh = MAX(maxh, _knob_minh);

    //   F_Log(0,"Min (%16.16s) >> %ld",F_Get(LOD->Knob,FA_Text),min);

        F_Do(LOD->Knob,FM_Set,
            
            FA_Text,     F_Do(Obj,F_IDR(FM_Numeric_Stringify),num_max),
            FA_NoNotify, TRUE,
            
            TAG_DONE);
        
        _knob_minw = 0; _knob_maxw = FV_MAXMAX;
        _knob_minh = 0; _knob_maxh = FV_MAXMAX;
        
        F_Do(LOD->Knob,FM_AskMinMax);
        
        maxw = MAX(maxw, _knob_minw);
        maxh = MAX(maxh, _knob_minh);
        
    //   F_Log(0,"Max (%16.16s) >> %ld",F_Get(LOD->Knob,FA_Text),max);
    }
    
    _knob_minw = maxw;
    _knob_maxw = maxw;
    _knob_minh = maxh;
    _knob_maxh = maxh;

    _minw += maxw + ((FF_Horizontal & _flags) ? 10 :  0);
    _minh += maxh + ((FF_Horizontal & _flags) ?  0 : 10);
 
/*** set val again *********************************************************/

    F_Do(LOD->Knob, FM_Set,
    
        FA_Text, F_Do(Obj,F_IDR(FM_Numeric_Stringify),num_val),
        FA_NoNotify, TRUE,
        
        TAG_DONE);

//   F_Log(0,"MIN (%ld x %ld) MAX (%ld x %ld)",_knob_minw,_knob_minh,_knob_maxw,_knob_maxh);

    F_SUPERDO();

    F_Set(LOD->OwnRender,FA_Render_Forbid,FALSE);
    F_Set(_render,FA_Render_Forbid,FALSE);
}
//+
///Slider_Layout
F_METHOD(void,Slider_Layout)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    int16 x1 = _ix, x2 = x1 + _iw - 1;
    int16 y1 = _iy, y2 = y1 + _ih - 1;
    int32 num_val,num_min,num_max;
    uint32 v,r,p;

    F_Do(Obj,FM_Get,
        
        F_IDR(FA_Numeric_Value),   &num_val,
        F_IDR(FA_Numeric_Min),     &num_min,
        F_IDR(FA_Numeric_Max),     &num_max,
        
        TAG_DONE);

    v = num_val - num_min;
    r = num_max - num_min; LOD->LastLayoutValue = num_val;

    if (FF_Horizontal & _flags)
    {
        LOD->kw = _knob_minw ; LOD->kh = y2 - y1 + 1 ; p = (x2 - x1 + 1 - LOD->kw) * v / r + x1 ; LOD->PreviousPos = LOD->kx;
    }
    else
    {
        LOD->kh = _knob_minh ; LOD->kw = x2 - x1 + 1 ; p = (y2 - y1 + 1 - LOD->kh) * v / r + y1 ; LOD->PreviousPos = LOD->ky;
    }                            

    if (FF_Horizontal & _flags)
    {
        LOD->kx = p;  LOD->ky = y1;
    }
    else
    {
        LOD->kx = x1; LOD->ky = p;
    }

    slider_cache_update(Class,Obj);
}
//+
