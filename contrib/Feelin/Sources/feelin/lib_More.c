/*
**    Miscellaneous...
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************
            
$VER: 10.00 (2005/08/13)

    Updated F_Draw() to support the new 'damaged' refreshing technique.
    
    Updated F_Layout() to support the new 'damaged' technique.
 
$VER: 09.00 (2005/06/09)
 
    [OLAV]
    
    F_StrFmt() now returns a pointer to the end  of  the  formated  string,
    which may be used to add further data.
    
    F_LogA()  wasn't  displaying  developer  message   when   called   from
    "input.device" task.
 
$VER: 08.02 (2005/01/13)

    Improved F_LogA() : if the  task  is  not  a  process  the  message  is
    displayed  in  an  EZRequest  window, with the location of the error as
    window title. This time, I also check the level of  the  log,  which  I
    never did before... oops !

*/

#include "Private.h"

/****************************************************************************
*** Private (shared) ********************************************************
****************************************************************************/

///f_find_attribute_name
STRPTR f_find_attribute_name(uint32 id,struct in_FeelinBase *FeelinBase)
{
    FClass *node;
    STRPTR name=NULL;

    for (node = (FClass *) FeelinBase -> Classes.Head ; node ; node = node -> Next)
    {
        if (node -> Attributes)
        {
            FClassAttribute *e;

            for (e = node -> Attributes ; e -> Name ; e++)
            {
                if (e -> Name)
                {
                    if (id == e -> ID)
                    {
                        name = e -> Name; break;
                    }
                }
            }
        }
        if (name) break;
    }
    return name;
}
//+
///f_find_method_name
STRPTR f_find_method_name(uint32 id,struct in_FeelinBase *FeelinBase)
{
    FClass *node;
    STRPTR name=NULL;

    for (node = (FClass *) FeelinBase -> Classes.Head ; node ; node = node -> Next)
    {
        if (node -> Methods)
        {
            FClassMethod *e;

            for (e = node -> Methods ; e -> Function ; e++)
            {
                if (e -> Name)
                {
                    if (id == e -> ID)
                    {
                        name = e -> Name; break;
                    }
                }
            }
        }
        if (name) break;
    }
    return name;
}
//+

/****************************************************************************
*** string ******************************************************************
****************************************************************************/

#ifdef __AROS__
#include <aros/asmcall.h>

AROS_UFH2S(void, put_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(STRPTR *, strPtrPtr, A3))
{
    AROS_USERFUNC_INIT
    
    *(*strPtrPtr)++ = chr;
    
    AROS_USERFUNC_EXIT
}

AROS_UFH2S(void, len_func,
    AROS_UFHA(UBYTE, chr, D0),
    AROS_UFHA(ULONG *, lenPtr, A3))
{
    AROS_USERFUNC_INIT
    
    (*lenPtr)++;
    
    AROS_USERFUNC_EXIT
}
#endif

///f_str_newA
F_LIB_STR_NEW
{
#ifndef __AROS__
    STATIC const uint32 len_func = 0x52934E75; /* ADDQ.L  #1,(A3) ; RTS */
    STATIC const uint32 put_func = 0x16C04E75; /* MOVE.B d0,(a3)+ ; RTS */
#endif

    STRPTR string = NULL;
    uint32 len = 0;

#ifdef __AROS__
    RawDoFmt(Fmt,Params,(VOID_FUNC)AROS_ASMSYMNAME(len_func),&len);
#else
    RawDoFmt(Fmt,Params,(void *)(&len_func),&len);
#endif

    if (len > 1)
    {
        if ((string = F_New(len)) != NULL)
        {
	#ifdef __AROS__
      	    STRPTR stringptr = string;

            RawDoFmt(Fmt,Params,(VOID_FUNC)AROS_ASMSYMNAME(put_func),&stringptr);
	#else
            RawDoFmt(Fmt,Params,(void *)(&put_func),string);
	#endif
        }
    }

    if (LenPtr)
    {
        *LenPtr = len - 1;
    }

    return string;
}
F_LIB_END
//+
///f_str_fmtA
F_LIB_STR_FMT
{
#ifdef __AROS__
    STRPTR stringptr = Buffer;

    RawDoFmt(Fmt,Params,(VOID_FUNC)AROS_ASMSYMNAME(put_func),&stringptr);    
#else
    STATIC const uint32 put_func = 0x16C04E75; /* MOVE.B d0,(a3)+ ; RTS */

    RawDoFmt(Fmt,Params,(void *)(&put_func),Buffer);
#endif
    
    return Buffer + F_StrLen(Buffer);
}
F_LIB_END
//+
///f_str_len
F_LIB_STR_LEN
{
    if (Str)
    {
        STRPTR start = Str;

        while (*Str++);

        return ((uint32)(Str) - (uint32)(start) - 1);
    }
    return 0;
}
F_LIB_END
//+
///f_str_cmp
F_LIB_STR_CMP
{
    UBYTE c1,c2;

    if (!Length)
    {
        return 0;
    }
    else if (!Str1)
    {
        return -1;
    }
    else if (!Str2)
    {
        return 1;
    }

    do
    {
        c1 = *Str1++;
        c2 = *Str2++;
    }
    while (c1 == c2 && c1 && --Length);

    return (LONG)(c1) - (LONG)(c2);
}
F_LIB_END
//+

/****************************************************************************
*** logging *****************************************************************
****************************************************************************/

///f_alerta
F_LIB_ALERT
{
    struct EasyStruct ez;

    ez.es_StructSize   = sizeof (struct EasyStruct);
    ez.es_Flags        = 0;
    ez.es_Title        = Title;
    ez.es_TextFormat   = Body;
    ez.es_GadgetFormat = "Damned";

    return EasyRequestArgs(NULL,&ez,NULL,Params);
}
F_LIB_END
//+
///f_logA
F_LIB_LOG
{
    FClass *cl = FeelinBase -> debug_classdo_class;
    FObject Obj = FeelinBase -> debug_classdo_object;
    uint32 Method = FeelinBase -> debug_classdo_method;
    struct Task *task = FindTask(NULL);
    STRPTR str;

    if (Level > FeelinBase -> debug_log_level)
    {
        return;
    }

    /* I use AllocMem() because F_New() and F_NewP()  may  call  F_Log()  to
    complain about memory shortage. */

    str = AllocMem(1024,0); 

    if (str)
    {
        STRPTR buf = str;

        *buf++ = '[';

        if ((task -> tc_Node.ln_Type != NT_PROCESS) || !GetProgramName(buf,256))
        {
            CopyMem(task -> tc_Node.ln_Name,buf,F_StrLen(task -> tc_Node.ln_Name) + 1);
        }

        buf += F_StrLen(buf);

        *buf++ = ']';
        *buf++ = ' ';

        if (cl && Obj)
        {
            STRPTR method_name = NULL;
            FClass *node;

            for (node = (FClass *) FeelinBase -> Classes.Head ; node ; node = node -> Next)
            {
                if (node -> Methods)
                {
                    FClassMethod *me;

                    for (me = node -> Methods ; me -> Function ; me++)
                    {
                        if (me -> Name)
                        {
                            if (Method == me -> ID)
                            {
                                method_name = me -> Name; break;
                            }
                        }
                    }
                }
                if (method_name) break;
            }

            if (_class(Obj) == cl)
            {
                buf = F_StrFmt(buf,"%s{%08lx}",cl -> Name,Obj);
            }
            else
            {
                buf = F_StrFmt(buf,"%s{%08lx} @ %s",_classname(Obj),Obj,cl -> Name);
            }

            if (method_name)
            {
                buf = F_StrFmt(buf,".%s>",method_name);
            }
            else
            {
                buf = F_StrFmt(buf,".0x%08lx>",Method);
            }

            *buf++ = ' ';
        }

        *buf = '\0';

        if ((task -> tc_Node.ln_Type == NT_PROCESS) && (FeelinBase -> Public.Console))
        {
            BPTR record;
 
            VFPrintf(FeelinBase -> Public.Console,str,NULL);
            VFPrintf(FeelinBase -> Public.Console,Fmt,Msg);
            VFPrintf(FeelinBase -> Public.Console,"\n",NULL);
        
            if ((record = Open("T:feelin.log",MODE_READWRITE)))
            {
                struct DateTime dt;
                char buf_date[32];
                char buf_time[16];
                
                DateStamp(&dt.dat_Stamp);
                
                dt.dat_Format  = FORMAT_INT;
                dt.dat_Flags   = 0;
                dt.dat_StrDay  = NULL;
                dt.dat_StrDate = (STRPTR) &buf_date;
                dt.dat_StrTime = (STRPTR) &buf_time;
                
                DateToStr(&dt);

                Seek(record,0,OFFSET_END);
 
                FPrintf(record,"%s %s ",&buf_date,&buf_time);
                VFPrintf(record,str,NULL);
                VFPrintf(record,Fmt,Msg);
                VFPrintf(record,"\n",NULL);
                
                Close(record);
            }
        }
/*
        else if (F_StrCmp("input.device",task -> tc_Node.ln_Name,ALL) == 0)
        {
            STRPTR buf = AllocMem(2048,MEMF_CLEAR);
            
            if (buf)
            {
                struct RastPort *rp = IntuitionBase -> ActiveWindow -> RPort;
        
                F_StrFmtA(buf,Fmt,Msg);
                
                SetABPenDrMd(rp,2,1,JAM2);
                Move(rp,10,10);
                Text(rp,str,F_StrLen(str));
                Move(rp,10,10 + rp -> Font -> tf_YSize);
                Text(rp,buf,F_StrLen(buf));
                
                FreeMem(buf,2048);
            }
        }
        else
        {
            F_AlertA(str,Fmt,Msg);
        }
*/
        FreeMem(str,1024);
    }
}
F_LIB_END
//+

/****************************************************************************
*** area support ************************************************************
****************************************************************************/

///f_draw
F_LIB_DRAW
{
    if (Obj)
    {
        FAreaPublic *pub = (FAreaPublic *) F_Get(Obj,FA_Area_PublicData);

        if (pub)
        {
            if ((FF_Draw_Damaged & Flags) == 0)
            {
                pub->Flags |= FF_Area_Damaged;
            }
/*
            if (FF_Area_Disabled & pub->Flags)
            {
                F_Log(0,"%s{0x%08lx} is disabled : setup (%s), canshow (%s), candraw (%s)",
                    Obj,
                    _classname(Obj),
                    (pub->Flags & FF_Area_Setup) ? "yes" : "no",
                    (pub->Flags & FF_Area_CanShow) ? "yes" : "no",
                    (pub->Flags & FF_Area_CanDraw) ? "yes" : "no");
            }
*/
            if (((FF_Area_Setup | FF_Area_CanShow | FF_Area_CanDraw) & pub->Flags) == (FF_Area_Setup | FF_Area_CanShow | FF_Area_CanDraw))
            {
                FRender *render = pub->Render;

                if (render && !(FF_Render_Forbid & render -> Flags))
                {
                    if (render -> RPort)
                    {
                        if (render -> RPort -> BitMap)
                        {
                            if (pub -> Box.w > 0 && pub -> Box.w < FV_MAXMAX &&
                                pub -> Box.h > 0 && pub -> Box.h < FV_MAXMAX)
                            {
                                if (FF_Area_Bufferize & pub -> Flags)
                                {
                                    //BOOL done=FALSE;
                                    struct RastPort *rp = F_New(sizeof (struct RastPort));
 
                                    //F_Log(0,"bufferize %s{%08lx}",_classname(Obj),Obj);
                                    
                                    if (rp)
                                    {
                                        uint16 w = pub -> Box.w;
                                        uint16 h = pub -> Box.h;
 
                                        struct BitMap *bmp;
                                        
                                        if ((bmp = AllocBitMap(w,h,F_Get(render -> Display,(uint32) "FA_Display_Depth"),0,render -> RPort -> BitMap)))
                                        {
                                            FRect r;
                                            APTR clip;
                                            struct RastPort *old_rp = render -> RPort;

                                            r.x1 = pub -> Box.x; r.x2 = r.x1 + w - 1;
                                            r.y1 = pub -> Box.y; r.y2 = r.y1 + h - 1;
                                                  
                                            InitRastPort(rp);
                                            rp -> BitMap = bmp;
                                            render -> RPort = rp;
                                            
                                            clip = (APTR) F_Do(render,FM_Render_AddClip,&r);

                                            if (clip)
                                            {
                                                //F_Log(0,"rport 0x%08lx clip 0x%08lx",rp,clip);

                                                F_Layout(Obj,0,0,w,h,0);
                                                F_Do(Obj,FM_Draw,Flags);
                                                ClipBlit(rp,0,0,old_rp,r.x1,r.y1,w,h,0x0C0);
                                                F_Layout(Obj,r.x1,r.y1,w,h,0);
                                                
                                                //done = TRUE;

                                                //F_Log(0,"done");
                                                F_Do(render,FM_Render_RemClip,clip);
                                            }

                                            render -> RPort = old_rp;
                                            
                                            FreeBitMap(bmp);
                                        }
				    #ifdef __AROS__
				        DeinitRastPort(rp);
				    #endif	
                                        F_Dispose(rp);
                                    }
/*
                                    if (!done)
                                    {
                                        F_Do(Obj,FM_Draw,Flags);
                                    }
*/
                                }
                                else
                                {
                                    F_Do(Obj,FM_Draw,Flags);
                                }
                            }
/*
                            else
                            {
                                F_Log(FV_LOG_DEV,"coordinates error (%ld:%ld %ldx%ld)",pub -> Box.x,pub -> Box.y,pub -> Box.w,pub -> Box.h);
                            }
*/
                        }
                        else
                        {
                            F_Log(FV_LOG_DEV,"No BitMap attached to RPort 0x%08lx",pub -> Render -> RPort);
                        }
                    }
                    else
                    {
                        F_Log(FV_LOG_DEV,"No RastPort attached to Render 0x%08lx",pub -> Render);
                    }
                }
            }
        }
        else
        {
            F_Log(0,"F_Draw() - Object 0x%08lx is not a subclass of FC_Area",Obj);
        }
    }
}
F_LIB_END
//+
///f_erase
F_LIB_ERASE
/*

    I created the  function  because  I  was  tired  of  creating  temporary
    structures  to  store  coordinates...  Lazyness  rarely ends with clever
    choices :-)

*/
{
    FRect r;

    r.x1 = x1;
    r.y1 = y1;
    r.x2 = x2;
    r.y2 = y2;

    F_Do(Obj,FM_Erase,&r,Flags);
}
F_LIB_END
//+
///f_layout
F_LIB_LAYOUT
{
    if (Obj)
    {
        FAreaPublic *pub = (FAreaPublic *) F_Get(Obj,FA_Area_PublicData);
        
        if (pub)
        {
            if ((pub->Box.x != x) || (pub->Box.y != y) ||
                (pub->Box.w != w) || (pub->Box.h != h))
            {
                pub->Box.x = x;
                pub->Box.y = y;
                pub->Box.w = w;
                pub->Box.h = h;
                
                pub->Flags |= FF_Area_Damaged;
            }

            F_Do(Obj, FM_Layout);
        }
        else
        {
            F_Log(0,"F_Layout() Unable to get FAreaPublic of %s{%08lx}",_classname(Obj),Obj);
        }
    }
}
F_LIB_END
//+

