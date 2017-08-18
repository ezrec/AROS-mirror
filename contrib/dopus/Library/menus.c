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

__saveds int DoFSSetMenuStrip(register struct Window *window __asm("a0"),
    register struct Menu *firstmenu __asm("a1"))
{
    struct Menu *menu;
    struct MenuItem *item;
    struct DrawInfo *drinfo;
    int fpen,bpen;
    int y,offset,xpos=0,iwidth,comwidth,checkwidth;
    struct IntuiText testtext;
    struct TextAttr testattr;

    comwidth=COMMWIDTH;
    checkwidth=CHECKWIDTH;

    AskFont(&window->WScreen->RastPort,&testattr);
    testtext.FrontPen=1;
    testtext.DrawMode=JAM1;
    testtext.NextText=NULL;

    if (//IntuitionBase->LibNode.lib_Version>36 &&
        (drinfo=GetScreenDrawInfo(window->WScreen))) {
//        if (IntuitionBase->LibNode.lib_Version>=39) {
            fpen=drinfo->dri_Pens[BARDETAILPEN];
            bpen=drinfo->dri_Pens[BARBLOCKPEN];
            if (drinfo->dri_AmigaKey) comwidth=drinfo->dri_AmigaKey->Width;
            if (drinfo->dri_CheckMark) checkwidth=drinfo->dri_CheckMark->Width;
/*
        }
        else {
            fpen=drinfo->dri_Pens[DETAILPEN];
            bpen=drinfo->dri_Pens[BLOCKPEN];
        }
*/
        FreeScreenDrawInfo(window->WScreen,drinfo);
    }
    else {
        fpen=0;
        bpen=1;
    }

    offset=window->WScreen->Font->ta_YSize+3;
    menu=firstmenu;
    while (menu) {
        if (xpos==0) xpos=menu->LeftEdge;
        else menu->LeftEdge=xpos;
        menu->Width=
            TextLength(&window->WScreen->RastPort,menu->MenuName,strlen(menu->MenuName))+
            window->WScreen->RastPort.Font->tf_XSize;
        xpos+=menu->Width+16;

        y=0;
        iwidth=0;
        item=menu->FirstItem;

        while (item) {
            item->TopEdge=y;

            if (item->Flags&ITEMTEXT) {
                struct IntuiText *text;
                int wid,mwidth=0,first=0;

                item->Height=offset-1;

                text=(struct IntuiText *)item->ItemFill;
                while (text) {
                    text->FrontPen=fpen;
                    text->BackPen=bpen;

                    if (first==0) {
                        if (LStrnCmp(text->IText,"   ",3)==0) text->IText=&text->IText[3];
                        if (item->Flags&CHECKIT) text->LeftEdge=checkwidth;
                        else text->LeftEdge=0;
                    }

                    if (text->ITextFont) testtext.ITextFont=text->ITextFont;
                    else testtext.ITextFont=&testattr;
                    testtext.IText=text->IText;

                    wid=text->LeftEdge+IntuiTextLength(&testtext)+window->WScreen->RastPort.Font->tf_XSize;
                    if (wid>mwidth) mwidth=wid;

                    if (text=text->NextText) {
                        y+=offset;
                        text->TopEdge=offset+3;
                    }
                    ++first;
                }
                if (item->Flags&COMMSEQ)
                    mwidth+=comwidth+TextLength(&window->WScreen->RastPort,&item->Command,1);
                if (mwidth>iwidth) iwidth=mwidth;

                y+=offset;
            }
            else {
                struct Image *image;
                int wid;

                ++item->TopEdge;

                image=(struct Image *)item->ItemFill;
                while (image) {
                    if (image->ImageData==NULL) {
                        image->Height=2;
                        image->PlaneOnOff=fpen;
                    }
                    else {
                        wid=image->LeftEdge+image->Width;
                        if (wid>iwidth) iwidth=wid;
                    }

                    y+=image->TopEdge+image->Height;

                    image=image->NextImage;
                }
                y+=3;
            }
            item=item->NextItem;
        }

        item=menu->FirstItem;
        while (item) {
            item->Width=iwidth;
            if (!(item->Flags&ITEMTEXT)) {
                struct Image *image;

                image=(struct Image *)item->ItemFill;
                while (image) {
                    if (image->ImageData==NULL) image->Width=iwidth;
                    image=image->NextImage;
                }
            }
            item=item->NextItem;
        }

        menu=menu->NextMenu;
    }

    return((int)SetMenuStrip(window,firstmenu));
}
