/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopuslib.h"

__regargs void fill_out_border_data(shine,shadow,w,h)
short *shine,*shadow;
int w,h;
{
    shine[0]=w-1;     /* w-1,0  -> 0,0  -> 0,h  -> 1,h-1  -> 1,0 */
    shine[5]=h;
    shine[6]=1;
    shine[7]=h-1;
    shine[8]=1;

    shadow[0]=1;      /* 1,h  -> w,h  -> w,0  -> w-1,1  -> w-1,h */
    shadow[1]=h;
    shadow[2]=w;
    shadow[3]=h;
    shadow[4]=w;
    shadow[6]=w-1;
    shadow[7]=1;
    shadow[8]=w-1;
    shadow[9]=h;
}

__regargs void fill_out_border(border,fpen,bpen,shine,shadow)
struct Border *border;
int fpen,bpen;
short *shine,*shadow;
{
    border[0].FrontPen=fpen;
    border[0].DrawMode=JAM1;
    border[0].Count=5;
    border[0].XY=shine;
    border[0].NextBorder=&border[1];
    border[1].FrontPen=bpen;
    border[1].DrawMode=JAM1;
    border[1].Count=5;
    border[1].XY=shadow;
}

void __saveds DoAddGadgetBorders(register struct DOpusRemember **key __asm("a0"),
    register struct Gadget *gadget __asm("a1"),
    register int count __asm("d0"),
    register int shine __asm("d1"),
    register int shadow __asm("d2"))
{
    struct Border *selborder,*unselborder;
    int width,height;

    width=gadget->Width;
    height=gadget->Height;

    DoCreateGadgetBorders(key,
        gadget->Width,gadget->Height,
        &selborder,&unselborder,
        0,
        shine,shadow);

    while (gadget && count) {
        if (gadget->Width==width && gadget->Height==height &&
            (gadget->GadgetType==GTYP_BOOLGADGET || gadget->GadgetType&GTYP_SYSGADGET)) {
            if (unselborder) {
                gadget->Flags|=GFLG_GADGHIMAGE;
                gadget->Flags&=~GFLG_GADGHCOMP;
                gadget->GadgetRender=unselborder;
                gadget->SelectRender=selborder;
            }
            else {
                gadget->Flags&=~GFLG_GADGHIMAGE;
                gadget->Flags|=GFLG_GADGHCOMP;
                gadget->GadgetRender=NULL;
                gadget->SelectRender=NULL;
            }
        }
        gadget=gadget->NextGadget;
        --count;
    }
}

void __saveds DoCreateGadgetBorders(register struct DOpusRemember **key __asm("a0"),
    register int w __asm("d0"),
    register int h __asm("d1"),
    register struct Border **selborder __asm("a1"),
    register struct Border **unselborder __asm("a2"),
    register int dogear __asm("d2"),
    register int shine __asm("d3"),
    register int shadow __asm("d4"))
{
    short *shine_data,*shadow_data;

    *selborder=NULL;
    *unselborder=NULL;

    if ((shine_data=DoAllocRemember(key,10*sizeof(short),MEMF_CLEAR)) &&
        (shadow_data=DoAllocRemember(key,10*sizeof(short),MEMF_CLEAR))) {

        fill_out_border_data(shine_data,shadow_data,w-1,h-1);
        if (dogear) {
            shine_data[0]=w-6;
            shadow_data[5]=4;
            shadow_data[7]=4;
        }

        if ((*selborder=DoAllocRemember(key,sizeof(struct Border)*2,MEMF_CLEAR)) &&
            (*unselborder=DoAllocRemember(key,sizeof(struct Border)*2,MEMF_CLEAR))) {

            fill_out_border(*selborder,shadow,shine,shine_data,shadow_data);
            fill_out_border(*unselborder,shine,shadow,shine_data,shadow_data);
        }
    }
}

void __saveds DoSelectGadget(register struct Window *window __asm("a0"),
    register struct Gadget *gadget __asm("a1"))
{
    if (gadget->GadgetRender && gadget->SelectRender && !(gadget->Flags&GFLG_GADGIMAGE)) {
        DrawBorder(window->RPort,gadget->SelectRender,gadget->LeftEdge,gadget->TopEdge);
        Delay(5);
        DrawBorder(window->RPort,gadget->GadgetRender,gadget->LeftEdge,gadget->TopEdge);
    }
    else {
        int old;

        old=window->RPort->DrawMode;
        SetDrMd(window->RPort,COMPLEMENT);
        HiliteGad(gadget,window->RPort);
        Delay(5);
        HiliteGad(gadget,window->RPort);
        SetDrMd(window->RPort,old);
    }
}
