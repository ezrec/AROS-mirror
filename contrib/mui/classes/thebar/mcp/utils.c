/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2009 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

#include "class.h"

/***********************************************************************/

// DoSuperNew()
// Calls parent NEW method within a subclass
#if !defined(__MORPHOS__)
#if defined(__AROS__)
IPTR VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  IPTR rc;
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...)
{
  Object *rc;
#endif
  VA_LIST args;

  ENTER();

  VA_START(args, obj);
#if defined(__AROS__)
    #if defined(__ARM_ARCH__)
        #warning "TODO: fix va_arg usage for ARM"
    #else
        rc = (IPTR)DoSuperNewTagList(cl, obj, NULL, (struct TagItem *)VA_ARG(args, IPTR));
    #endif
#else
    rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
#endif
  VA_END(args);

  RETURN(rc);
  return rc;
}
#endif

/***********************************************************************/

#if !defined(__MORPHOS__) && !defined(__AROS__)
int
stch_l(const char *chr_ptr,long *u_ptr)
{
    const char *str = chr_ptr;
    ULONG      val = 0;

    if (str)
    {
        for (;;)
        {
            unsigned char c = (unsigned char)*str;

            if (!c) break;

            if (c>='0' && c<='9') c -= '0';
            else if (c>='a' && c<='f') c -= 'a'-10;
                 else if (c>='A' && c<='F') c -= 'A'-10;
                      else break;

            val <<= 4;
            val += c;

            str++;
        }
    }

    *u_ptr = (long)val;

    return str-chr_ptr;
}
#endif

/***********************************************************************/

Object *
obutton(ULONG text,ULONG help)
{
    Object *obj;

    if((obj = MUI_MakeObject(MUIO_Button,(IPTR)tr(text))))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ShortHelp,(IPTR)tr(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
ocycle(STRPTR *array,ULONG key,ULONG help)
{
    Object *obj;

    if((obj = MUI_MakeObject(MUIO_Cycle,(IPTR)tr(key),(IPTR)array)))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ShortHelp,(IPTR)tr(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
ocheck(ULONG key,ULONG help)
{
    Object *obj;

    if((obj = MUI_MakeObject(MUIO_Checkmark,(IPTR)tr(key))))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ShortHelp,(IPTR)tr(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
oslider(ULONG key,ULONG help,LONG min,LONG max)
{
    Object *obj;

    if((obj = MUI_MakeObject(MUIO_Slider,(IPTR)tr(key),min,max)))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ShortHelp,(IPTR)tr(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
opop(ULONG type,ULONG key)
{
    Object *obj;

    if (!(lib_flags & BASEFLG_MUI4) && (type==MUII_PopFont))
        type = MUII_PopUp;

    if((obj = MUI_MakeObject(MUIO_PopButton,type)))
    {
        set(obj,MUIA_CycleChain,TRUE);

        if (key)
        {
            ULONG k;

            if((k = getKeyChar(tr(key))))
                set(obj,MUIA_ControlChar,ToLower(k));
        }
    }

    return obj;
}

/***********************************************************************/

Object *
opoppen(ULONG key,ULONG title,ULONG help)
{
    #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
    return PoppenObject,
        MUIA_Window_Title, (IPTR)tr(title),
        MUIA_ControlChar,  (ULONG)getKeyChar(tr(key)),
        MUIA_Draggable,    TRUE,
        MUIA_CycleChain,   TRUE,
        MUIA_ShortHelp,    (IPTR)tr(help),
    End;
    #else
    if (lib_flags & BASEFLG_MUI20)
    {
        return PoppenObject,
            MUIA_Window_Title, (ULONG)tr(title),
            MUIA_ControlChar,  (ULONG)getKeyChar(tr(key)),
            MUIA_Draggable,    TRUE,
            MUIA_CycleChain,   TRUE,
            MUIA_ShortHelp,    (ULONG)tr(help),
        End;
    }
    else
    {
        return poppenObject,
            MUIA_Window_Title, (ULONG)tr(title),
            MUIA_ControlChar,  (ULONG)getKeyChar(tr(key)),
            MUIA_Draggable,    TRUE,
            MUIA_CycleChain,   TRUE,
            MUIA_ShortHelp,    (ULONG)tr(help),
        End;
    }
    #endif
}

/***********************************************************************/

Object *
opopfri(ULONG key,ULONG title,ULONG help)
{
    return MUI_NewObject("Popfrimage.mui",
        MUIA_Window_Title,     (IPTR)tr(title),
        MUIA_ControlChar,      (ULONG)getKeyChar(tr(key)),
        MUIA_CycleChain,       TRUE,
        MUIA_ShortHelp,        (IPTR)tr(help),
        0x80421794, 0,
        0x8042a547, 0,
        0x80426a55, 1,
        0x8042a92b, 0,
    TAG_DONE);
}

/***********************************************************************/

Object *
opopback(UNUSED ULONG gradient,ULONG key,ULONG title,ULONG help)
{
    #if defined(__MORPHOS__) || defined(__amigaos4__) || defined(__AROS__)
    return MUI_NewObject(MUIC_Popimage,
        MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
        MUIA_Window_Title,     (IPTR)tr(title),
        MUIA_ControlChar,      (ULONG)getKeyChar(tr(key)),
        MUIA_Draggable,        TRUE,
        MUIA_CycleChain,       TRUE,
        MUIA_ShortHelp,        (IPTR)tr(help),
    TAG_DONE);
    #else
    if (lib_flags & BASEFLG_MUI20)
    {
        return MUI_NewObject(MUIC_Popimage,
            MUIA_Imageadjust_Type, MUIV_Imageadjust_Type_Background,
            MUIA_Window_Title,     (ULONG)tr(title),
            MUIA_ControlChar,      (ULONG)getKeyChar(tr(key)),
            MUIA_Draggable,        TRUE,
            MUIA_CycleChain,       TRUE,
            MUIA_ShortHelp,        (ULONG)tr(help),
        TAG_DONE);
    }
    else
    {
        return popbackObject,
            MUIA_Window_Title,           (ULONG)tr(title),
            MUIA_ControlChar,            (ULONG)getKeyChar(tr(key)),
            MUIA_Draggable,              TRUE,
            MUIA_CycleChain,             TRUE,
            MUIA_ShortHelp,              (ULONG)tr(help),
            MUIA_Popbackground_Gradient, gradient,
        End;
    }
    #endif
}

/***********************************************************************/

#ifndef PopframeObject
#define PopframeObject MUI_NewObject(MUIC_Popframe
#endif

Object *
opopframe(ULONG key,ULONG title,ULONG help)
{
    return PopframeObject,
        MUIA_Window_Title, (IPTR)tr(title),
        MUIA_Draggable,    TRUE,
        MUIA_CycleChain,   1,
        MUIA_ShortHelp,    (IPTR)tr(help),
        MUIA_ControlChar,  (ULONG)getKeyChar(tr(key)),
    End;
}

/***********************************************************************/

#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
void drawGradient(Object *obj, struct MUIS_TheBar_Gradient *grad)
{
    struct RastPort *rp;
    ULONG           rs, gs, bs, horiz, from, to;
    int             i, x, d, step, rdif, gdif, bdif, r1, g1, b1, r2, g2, b2, r, g, b, rd, gd, bd, ridx, gidx, bidx;
    UWORD           l = _mleft(obj), t = _mtop(obj), w = _mwidth(obj), h = _mheight(obj);

    rd = rdif= ridx = gd = gidx = bd = bidx = 0;

    rp = _rp(obj);

    from  = grad->from;
    to    = grad->to;
    horiz = grad->flags & MUIV_TheBar_Gradient_Horiz;

    /* Compute diffs */
    r1 = (from & 0x00FF0000)>>16;
    g1 = (from & 0x0000FF00)>>8;
    b1 = (from & 0x000000FF);

    r2 = (to & 0x00FF0000)>>16;
    g2 = (to & 0x0000FF00)>>8;
    b2 = (to & 0x000000FF);

    if (r1>r2)
    {
        rdif = r1-r2;
        rs = FALSE;
    }
    else
    {
        rdif = r2-r1;
        rs = TRUE;
    }

    if (g1>g2)
    {
        gdif = g1-g2;
        gs = FALSE;
    }
    else
    {
        gdif = g2-g1;
        gs = TRUE;
    }

    if (b1>b2)
    {
        bdif = b1-b2;
        bs = FALSE;
    }
    else
    {
        bdif = b2-b1;
        bs = TRUE;
    }

    /* Find out max diff */
    step = (rdif>=gdif) ? rdif : gdif;
    if (bdif>step) step = bdif;

    /* Find out best step to minimize rest */
    if (horiz)
    {
        if (step>w) step = w;

        if (step==0) step = 1;
        else
        {
            x = w/step;
            if (w!=x*step)
            {
                step = w/(x+1);
                if (step>w) step = w;
            }
        }
    }
    else
    {
        if (step>h) step = h;

        if (step==0) step = 1;
        else
        {
            x = h/step;
            if (h!=x*step)
            {
                step = h/(x+1);
                if (step>h) step = h;
            }
        }
    }

    if (step>1)
    {
        /* Compute color components deltas */
        rd = rdif/step;
        if (!rd) rd = 1;

        gd = gdif/step;
        if (!gd) gd = 1;

        bd = bdif/step;
        if (!bd) bd = 1;

        /* Compute rests */
        ridx = (rdif>step) ? step/((rdif-step)/rd) : 0;
        gidx = (gdif>step) ? step/((gdif-step)/gd) : 0;
        bidx = (bdif>step) ? step/((bdif-step)/bd) : 0;
    }

    /* Here we go */
    d = (horiz) ? w/step : h/step;
    x = (horiz) ? l : t;
    r = r1;
    g = g1;
    b = b1;

    for (i = 0; i<step; i++)
    {
        ULONG col = (r<<16) | (g<<8) | b;

        /* Fill; if last step fill all rest */
        if (horiz)
            if (i==step-1)
            {
                FillPixelArray(rp,x,t,w-x+l,h,col);
                break;
            }
            else FillPixelArray(rp,x,t,d,h,col);
        else
            if (i==step-1)
            {
                FillPixelArray(rp,l,x,w,h-x+t,col);
                break;
            }
            else FillPixelArray(rp,l,x,w,d,col);

        x += d;

        /* Add delta to each color component */
        if (rs)
        {
            r += rd;

            /* Time to add more one more delta? */
            if (i && ridx && !(i%ridx)) r += rd;
            if (r>r2) r = r2;
        }
        else
        {
            r -= rd;

            if (i && ridx && !(i%ridx)) r -= rd;
            if (r<r2) r = r2;
        }

        if (gs)
        {
            g += gd;

            if (i && gidx && !(i%gidx)) g += gd;
            if (g>g2) g = g2;
        }
        else
        {
            g -= gd;

            if (i && gidx && !(i%gidx)) g -= gd;
            if (g<g2) g = g2;
        }

        if (bs)
        {
            b += bd;

            if (i && bidx && !(i%bidx)) b += bd;
            if (b>b2) b = b2;
        }
        else
        {
            b -= bd;

            if (i && bidx && !(i%bidx)) b -= bd;
            if (b<b2) b = b2;
        }
    }
}
#endif

/***********************************************************************/
