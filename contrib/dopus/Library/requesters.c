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

/* Font-sensitive requester routines.

   These routines are for LAYOUT ONLY!!!!

   They do not add gadgets or listviews to a window, nor do they
   attempt to process gadget or listview operations. They will
   allocate most, if not all things you need to add an object
   to the requester, however */

#include "dopuslib.h"

struct PrivateData {
    struct Image *check_off_image;
    struct Image *check_on_image;
    struct Image *button_off_image;
    struct Image *button_on_image;
};

const struct TextAttr
    topazfont={(STRPTR)"topaz.font",8,0,0};

/* Open requester */

struct Window * __saveds R_OpenRequester(register struct RequesterBase *reqbase __asm("a0"))
{
    struct NewWindow newwin;
    struct Screen *screen,screendata;
    struct Window *window=NULL,*center_window=NULL;
    short width,height,attempt;

    /* Forbid to prevent changes to Screen lists */

    Forbid();

    newwin.Type=0;

    if (reqbase->rb_flags&RBF_WINDOWCENTER)
        center_window=reqbase->rb_window;
    reqbase->rb_window=NULL;

    /* rb_screen overrides rb_screenname; if this is initialised then try
       to open the window on the CUSTOMSCREEN given */

    if (reqbase->rb_screen) {
        newwin.Type=CUSTOMSCREEN;
        newwin.Screen=reqbase->rb_screen;
        screen=reqbase->rb_screen;
    }

    /* Otherwise, if rb_screenname is given, search screen lists for that
       screen */

    else if (reqbase->rb_screenname) {

        /* Look for it as a public screen first (2.0 only) */

        if (//IntuitionBase->LibNode.lib_Version>35 &&
            (screen=LockPubScreen(reqbase->rb_screenname))) {
            newwin.Type=PUBLICSCREEN;
            newwin.Screen=screen;
        }

        /* Otherwise search screen lists manually */
/*
        else {
            screen=IntuitionBase->FirstScreen;
            while (screen) {
                if (screen->Title &&
                    LStrCmpI((char *)screen->Title,reqbase->rb_screenname)==0) {
                    newwin.Type=CUSTOMSCREEN;
                    newwin.Screen=screen;
                    break;
                }
                screen=screen->NextScreen;
            }
        }
*/
    }

    /* If the screen type is still unset, just open on the Workbench screen
       (or default public screen) */

    if (!newwin.Type) {
        newwin.Type=WBENCHSCREEN;
        GetWBScreen(&screendata);
        screen=&screendata;
    }

    /* Check font pointer; if not set, use screen's font */

    if (!reqbase->rb_font)
        reqbase->rb_font=screen->RastPort.Font;

    /* If font is proportional, use system default font */

    if (reqbase->rb_font->tf_Flags&FPF_PROPORTIONAL)
        reqbase->rb_font=GfxBase->DefaultFont;

    /* Attempt this twice; first with the selected font, and if
       the window is too large, again with topaz/8 */

    for (attempt=0;attempt<2;attempt++) {

        /* Setup window size for selected font */

        width=(reqbase->rb_width*reqbase->rb_font->tf_XSize)+reqbase->rb_widthfine;
        height=(reqbase->rb_height*reqbase->rb_font->tf_YSize)+reqbase->rb_heightfine;

        if (reqbase->rb_flags&RBF_BORDERS) {
            width+=screen->WBorLeft+screen->WBorRight;
            height+=screen->WBorTop+screen->WBorBottom+screen->Font->ta_YSize+1;
        }

        /* If it's not going to fit on the screen, don't even attempt
           to open it. Otherwise, fill in NewWindow structure */

        if (width<=screen->Width && height<=screen->Height) {

            if (reqbase->rb_flags&RBF_WINDOWCENTER && center_window) {
                newwin.LeftEdge=center_window->LeftEdge+((center_window->Width-width)/2);
                newwin.TopEdge=center_window->TopEdge+((center_window->Height-height)/2);
            }
            else {
                newwin.LeftEdge=(screen->Width-width)/2;
                newwin.TopEdge=(screen->Height-height)/2;
            }

            if (newwin.LeftEdge<0) newwin.LeftEdge=0;
            else if (newwin.LeftEdge+width>screen->Width) newwin.LeftEdge=screen->Width-width;
            if (newwin.TopEdge<0) newwin.TopEdge=0;
            else if (newwin.TopEdge+height>screen->Height) newwin.TopEdge=screen->Height-height;

            newwin.Width=width;
            newwin.Height=height;
            newwin.DetailPen=255;
            newwin.BlockPen=255;

            if (reqbase->rb_idcmpflags)
                newwin.IDCMPFlags=reqbase->rb_idcmpflags;
            else newwin.IDCMPFlags=
                IDCMP_GADGETUP|
                IDCMP_GADGETDOWN|
                IDCMP_MOUSEBUTTONS|
                IDCMP_MOUSEMOVE|
                IDCMP_RAWKEY|
                IDCMP_VANILLAKEY|
                IDCMP_DISKREMOVED|
                IDCMP_DISKINSERTED;

            if (reqbase->rb_flags&RBF_BORDERS) {
                newwin.Flags=
                    WFLG_DRAGBAR|
                    WFLG_DEPTHGADGET|
                    WFLG_RMBTRAP|
                    WFLG_ACTIVATE|
                    WFLG_NEWLOOKMENUS;
                if (reqbase->rb_flags&RBF_CLOSEGAD) {
                    newwin.Flags|=WFLG_CLOSEGADGET;
                    newwin.IDCMPFlags|=IDCMP_CLOSEWINDOW;
                }
            }
            else {
                newwin.Flags=
                    WFLG_BORDERLESS|
                    WFLG_RMBTRAP|
                    WFLG_ACTIVATE|
                    WFLG_NEWLOOKMENUS;
            }

            newwin.FirstGadget=NULL;
            newwin.CheckMark=NULL;
            newwin.Title=reqbase->rb_title;

            /* Try to open window; if we succeed, then render border and fill
               in background */

            if (window=OpenWindow(&newwin)) {
                SetFont(window->RPort,reqbase->rb_font);    
                if (!(reqbase->rb_flags&RBF_BORDERS)) {
                    Do3DBox(window->RPort,2,1,window->Width-4,window->Height-2,
                        reqbase->rb_shine,reqbase->rb_shadow);
                }
                SetAPen(window->RPort,reqbase->rb_bg);
                RectFill(window->RPort,
                    window->BorderLeft+2,
                    window->BorderTop+1,
                    window->Width-3-window->BorderRight,
                    window->Height-2-window->BorderBottom);
                SetAPen(window->RPort,reqbase->rb_fg);
                SetBPen(window->RPort,reqbase->rb_bg);
                SetDrMd(window->RPort,JAM2);
                break;
            }
        }

        /* If we failed with the custom font, try again with topaz/8 */

        if (attempt==0) {
            if (!(reqbase->rb_font=OpenFont(&topazfont))) break;
            reqbase->rb_privateflags|=RBP_CLOSEFONT;
        }
    }

    /* Unlock the public screen we may have locked earlier */

    if (screen && newwin.Type==PUBLICSCREEN) UnlockPubScreen(NULL,screen);

    /* Fix StringExtend function for current font */

    if (reqbase->rb_extend && window) reqbase->rb_extend->Font=reqbase->rb_font;
        
    /* Restore multitasking */

    Permit();

    /* Initialise other data */

    reqbase->rb_objects=NULL;
    reqbase->rb_memory=NULL;
    reqbase->rb_privatedata=
        DoAllocRemember(&reqbase->rb_memory,sizeof(struct PrivateData),MEMF_CLEAR);

    /* Fill out rb_window pointer in RequesterBase structure. We also
       return the window pointer to indicate success or failure */

    reqbase->rb_window=window;

    return(window);
}

void __saveds R_CloseRequester(register struct RequesterBase *reqbase __asm("a0"))
{
    if (reqbase->rb_window) {
        CloseWindow(reqbase->rb_window);
        reqbase->rb_window=NULL;
    }
    if (reqbase->rb_privateflags&RBP_CLOSEFONT && reqbase->rb_font)
        CloseFont(reqbase->rb_font);
    DoFreeRemember(&reqbase->rb_memory);
}

APTR __saveds R_AddRequesterObject(register struct RequesterBase *reqbase __asm("a0"),
    register struct TagItem *taglist __asm("a1"))
{
    struct RequesterObject *object=NULL,*tempobject;
    struct PrivateData *private;
    IPTR data;
    int tag,size,type,xoffset,yoffset;
    struct StringInfo *stringinfo=NULL;
    struct PropInfo *propinfo;
    struct Image *propimage;
    struct Gadget *gadget=NULL;
    char **string;

    if (!taglist || !reqbase || !reqbase->rb_window) return(NULL);

    private=(struct PrivateData *)reqbase->rb_privatedata;

    if (reqbase->rb_flags&RBF_BORDERS) {
        xoffset=reqbase->rb_window->BorderLeft;
        yoffset=reqbase->rb_window->BorderTop;
    }
    else {
        xoffset=0;
        yoffset=0;
    }

    for (tag=0;tag>-1;tag++) {
        data=taglist[tag].ti_Data;
        switch (taglist[tag].ti_Tag) {
            case TAG_END:
                tag=-2;
                break;

            case TAG_SKIP:
                break;

            case TAG_MORE:
                taglist=(struct TagItem *)data;
                tag=-1;
                break;

    /* Object type */

            case RO_Type:
                if (object) break;
                if (!(object=DoAllocRemember(&reqbase->rb_memory,
                    sizeof(struct RequesterObject),MEMF_CLEAR))) return(NULL);
                switch (object->ro_type=data) {
                    case OBJECT_TEXT:
                        size=sizeof(Object_Text);   
                        break;
                    case OBJECT_BORDER:
                        size=sizeof(Object_Border);
                        break;
                    case OBJECT_GADGET:
                        size=sizeof(Object_Gadget);
                        break;
                    case OBJECT_LISTVIEW:
                        size=sizeof(Object_ListView);
                        break;
                }
                if (!(object->ro_data=DoAllocRemember(&reqbase->rb_memory,
                    size,MEMF_CLEAR))) return(NULL);

                switch (object->ro_type) {
                    case OBJECT_TEXT:
                    case OBJECT_BORDER:
                        ((Object_Text *)object->ro_data)->ot_left=
                            reqbase->rb_leftoffset+xoffset;
                        ((Object_Text *)object->ro_data)->ot_top=
                            reqbase->rb_topoffset+yoffset;
                        break;
                    case OBJECT_GADGET:
                        ((Object_Gadget *)object->ro_data)->og_gadget.LeftEdge=
                            reqbase->rb_leftoffset+xoffset;
                        ((Object_Gadget *)object->ro_data)->og_gadget.TopEdge=
                            reqbase->rb_topoffset+yoffset;
                        break;
                    case OBJECT_LISTVIEW:
                        ((Object_ListView *)object->ro_data)->ol_listview.x=
                            reqbase->rb_leftoffset+xoffset;
                        ((Object_ListView *)object->ro_data)->ol_listview.y=
                            reqbase->rb_topoffset+yoffset;
                        break;
                }

                if (tempobject=reqbase->rb_objects)
                    while (tempobject->ro_next) tempobject=tempobject->ro_next;
                if (tempobject) tempobject->ro_next=object;
                else reqbase->rb_objects=object;
                break;

    /* Gadget type (object->ro_Type==OBJECT_GADGET) */

            case RO_GadgetType:
                if (!object || object->ro_type!=OBJECT_GADGET) break;
                gadget=&((Object_Gadget *)object->ro_data)->og_gadget;
                ((Object_Gadget *)object->ro_data)->og_type=data;
                switch (data) {
                    case GADGET_CYCLE:
                        gadget->MutualExclude=GAD_CYCLE;

                    case GADGET_TINYGLASS:
                    case GADGET_BOOLEAN:
                        type=GTYP_BOOLGADGET;
                        gadget->Flags=GFLG_GADGHCOMP;
                        gadget->Activation=GACT_RELVERIFY;
                        break;

                    case GADGET_INTEGER:
                        gadget->Activation|=GACT_LONGINT;
                    case GADGET_STRING:
                        type=GTYP_STRGADGET;
                        if (!(stringinfo=(struct StringInfo *)
                            DoAllocRemember(&reqbase->rb_memory,sizeof(struct StringInfo),MEMF_CLEAR)))
                            return(NULL);
                        gadget->SpecialInfo=(APTR)stringinfo;
                        gadget->Flags=GFLG_GADGHCOMP|GFLG_TABCYCLE;
                        if (reqbase->rb_extend) {
                            gadget->Flags|=GFLG_STRINGEXTEND;
                            stringinfo->Extension=reqbase->rb_extend;
                        }
                        gadget->Activation|=GACT_RELVERIFY|GACT_TOGGLESELECT;
                        break;

                    case GADGET_PROP:
                        type=GTYP_PROPGADGET;
                        if (!(propinfo=(struct PropInfo *)
                            DoAllocRemember(&reqbase->rb_memory,sizeof(struct PropInfo),MEMF_CLEAR)))
                            return(NULL);
                        gadget->SpecialInfo=(APTR)propinfo;
                        if (!(propimage=(struct Image *)
                            DoAllocRemember(&reqbase->rb_memory,sizeof(struct Image),MEMF_CLEAR)))
                            return(NULL);
                        gadget->GadgetRender=propimage;
                        break;

                    case GADGET_CHECK:
                        type=GTYP_BOOLGADGET;
                        gadget->Width=26;
                        gadget->Height=11;
                        gadget->Flags=GFLG_GADGIMAGE|GFLG_GADGHIMAGE;
                        gadget->Activation=GACT_RELVERIFY|GACT_TOGGLESELECT;
                        if (private) {
                            if (!private->check_off_image)
                                private->check_off_image=
                                    DoGetCheckImage(reqbase->rb_shadow,reqbase->rb_bg,0,&reqbase->rb_memory);
                            if (!private->check_on_image)
                                private->check_on_image=
                                    DoGetCheckImage(reqbase->rb_shadow,reqbase->rb_bg,1,&reqbase->rb_memory);
                            gadget->GadgetRender=(APTR)private->check_off_image;
                            gadget->SelectRender=(APTR)private->check_on_image;
                        }
                        gadget->MutualExclude=GAD_CHECK;
                        break;

                    case GADGET_RADIO:
                        type=GTYP_BOOLGADGET;
                        gadget->Width=15;
                        gadget->Height=7;
                        gadget->Flags=GFLG_GADGIMAGE|GFLG_GADGHIMAGE;
                        gadget->Activation=GACT_IMMEDIATE|GACT_TOGGLESELECT;
                        if (private) {
                            if (!private->button_off_image)
                                private->button_off_image=
                                    DoGetButtonImage(15,9,reqbase->rb_shine,reqbase->rb_shadow,
                                        reqbase->rb_bg,reqbase->rb_bg,&reqbase->rb_memory);
                            if (!private->button_on_image)
                                private->button_on_image=
                                    DoGetButtonImage(15,9,reqbase->rb_shine,reqbase->rb_shadow,
                                        reqbase->rb_fg,reqbase->rb_bg,&reqbase->rb_memory);
                            gadget->GadgetRender=(APTR)private->button_off_image;
                            gadget->SelectRender=(APTR)private->button_on_image;
                        }
                        gadget->MutualExclude=GAD_RADIO;
                        break;
                }
                gadget->GadgetType=type;
                break;

    /* Gadget ID (object->ro_type==OBJECT_GADGET) */

            case RO_GadgetID:
                if (!object || object->ro_type!=OBJECT_GADGET) break;
                ((Object_Gadget *)object->ro_data)->og_gadget.GadgetID=data;
                break;

    /* Border type (object->ro_type==OBJECT_BORDER) */

            case RO_BorderType:
                if (!object || object->ro_type!=OBJECT_BORDER) break;
                ((Object_Border *)object->ro_data)->ob_type=data;
                break;

    /* ListView ID (object->ro_type==OBJECT_LISTVIEW) */

            case RO_ListViewID:
                if (!object || object->ro_type!=OBJECT_LISTVIEW) break;
                ((Object_ListView *)object->ro_data)->ol_listview.listid=data;
                break;

    /* Left edge positioning */

            case RO_Left:
                data*=reqbase->rb_font->tf_XSize;
            case RO_LeftFine:
                if (!object) break;
                switch (object->ro_type) {
                    case OBJECT_TEXT:
                    case OBJECT_BORDER:
                        ((Object_Text *)object->ro_data)->ot_left+=data;
                        break;

                    case OBJECT_GADGET:
                        ((Object_Gadget *)object->ro_data)->og_gadget.LeftEdge+=data;
                        break;

                    case OBJECT_LISTVIEW:
                        ((Object_ListView *)object->ro_data)->ol_listview.x+=data;
                        break;
                }
                break;

    /* Top edge positioning */

            case RO_Top:
                data*=reqbase->rb_font->tf_YSize;
            case RO_TopFine:
                if (!object) break;
                switch (object->ro_type) {
                    case OBJECT_TEXT:
                    case OBJECT_BORDER:
                        ((Object_Text *)object->ro_data)->ot_top+=data;
                        break;

                    case OBJECT_GADGET:
                        ((Object_Gadget *)object->ro_data)->og_gadget.TopEdge+=data;
                        break;

                    case OBJECT_LISTVIEW:
                        ((Object_ListView *)object->ro_data)->ol_listview.y+=data;
                        break;
                }
                break;

    /* Width adjustment */

            case RO_Width:
                data*=reqbase->rb_font->tf_XSize;
            case RO_WidthFine:
                if (!object) break;
                switch (object->ro_type) {
                    case OBJECT_BORDER:
                        ((Object_Border *)object->ro_data)->ob_width+=data;
                        break;

                    case OBJECT_GADGET:
                        ((Object_Gadget *)object->ro_data)->og_gadget.Width+=data;
                        break;

                    case OBJECT_LISTVIEW:
                        ((Object_ListView *)object->ro_data)->ol_listview.w+=data;
                        break;
                }
                break;

    /* Height adjustment */

            case RO_Height:
                data*=reqbase->rb_font->tf_YSize;
            case RO_HeightFine:
                if (!object) break;
                switch (object->ro_type) {
                    case OBJECT_BORDER:
                        ((Object_Border *)object->ro_data)->ob_height+=data;
                        break;

                    case OBJECT_GADGET:
                        ((Object_Gadget *)object->ro_data)->og_gadget.Height+=data;
                        break;

                    case OBJECT_LISTVIEW:
                        ((Object_ListView *)object->ro_data)->ol_listview.h+=data;
                        break;
                }
                break;

    /* Object text */

            case RO_TextNum:
                if (!reqbase->rb_flags&RBF_STRINGS || !reqbase->rb_string_table) break;
                data=(IPTR)reqbase->rb_string_table[data];

            case RO_Text:
                if (!object || !data) break;
                string=NULL;
                switch (object->ro_type) {
                    case OBJECT_TEXT:
                        string=&((Object_Text *)object->ro_data)->ot_text;
                        break;

                    case OBJECT_GADGET:
                        string=&((Object_Gadget *)object->ro_data)->og_text;
                        break;

                    case OBJECT_BORDER:
                        string=&((Object_Border *)object->ro_data)->ob_text;
                        break;

                    case OBJECT_LISTVIEW:
                        string=&((Object_ListView *)object->ro_data)->ol_listview.title;
                        break;
                }
                if (string &&
                    (*string=DoAllocRemember(&reqbase->rb_memory,strlen((char *)data)+1,0)))
                    LStrCpy(*string,(char *)data);
                break;

    /* Text positioning */

            case RO_TextPos:
                if (!object) break;
                switch (object->ro_type) {
                    case OBJECT_GADGET:
                        ((Object_Gadget *)object->ro_data)->og_textpos=data;
                        break;

                    case OBJECT_BORDER:
                        ((Object_Border *)object->ro_data)->ob_textpos=data;
                        break;

                    case OBJECT_LISTVIEW:
                        if (data==TEXTPOS_ABOVE)
                            ((Object_ListView *)object->ro_data)->ol_listview.flags|=DLVF_TTOP;
                        break;
                }
                break;

    /* String gadget settings (gadgettype==GTYP_STRGADGET) */

            case RO_StringBuf:
                if (!stringinfo) break;
                stringinfo->Buffer=(UBYTE *)data;
                break;

            case RO_StringLen:
                if (!stringinfo) break;
                stringinfo->MaxChars=data;
                break;

            case RO_StringUndo:
                if (!stringinfo) break;
                stringinfo->UndoBuffer=(UBYTE *)data;
                break;

    /* Boolean gadget setting */

            case RO_BoolOn:
                if (data && object && object->ro_type==OBJECT_GADGET)
                    ((Object_Gadget *)object->ro_data)->og_gadget.Flags|=GFLG_SELECTED;
                break;

    /* Various other tags */

            case RO_ChkCenter:
                if (data && object && object->ro_type==OBJECT_GADGET && gadget &&
                    (gadget->MutualExclude==GAD_CHECK || gadget->MutualExclude==GAD_RADIO) &&
                    reqbase->rb_font->tf_YSize>gadget->Height)
                    gadget->TopEdge+=(reqbase->rb_font->tf_YSize-gadget->Height)/2;
                break;

            case RO_HighRecess:
                if (data && object) {
                    if (object->ro_type==OBJECT_GADGET &&
                        gadget && gadget->GadgetType==GTYP_BOOLGADGET) {
                        DoAddGadgetBorders(&reqbase->rb_memory,
                            gadget,1,
                            reqbase->rb_shine,reqbase->rb_shadow);
                    }
                    else if (object->ro_type==OBJECT_LISTVIEW)
                        ((Object_ListView *)object->ro_data)->ol_listview.flags|=DLVF_HIREC;
                }
                break;
        }
    }

    if (stringinfo && !stringinfo->Buffer && stringinfo->MaxChars>0) {
        if (!(stringinfo->Buffer=
            DoAllocRemember(&reqbase->rb_memory,stringinfo->MaxChars+1,MEMF_CLEAR)))
            return(NULL);
    }

    return((object)?object->ro_data:(APTR)NULL);
}

void __saveds R_RefreshRequesterObject(register struct RequesterBase *reqbase __asm("a0"),
    register struct RequesterObject *object __asm("a1"))
{
    UWORD count;
    struct RastPort *rp;

    if (!reqbase || !reqbase->rb_window) return;
    if (object) count=1;
    else if ((object=reqbase->rb_objects)) count=65535;
    else return;

    rp=reqbase->rb_window->RPort;

    while (count-- && object) {
        switch (object->ro_type) {
            case OBJECT_TEXT:
                R_ObjectText(reqbase,
                    ((Object_Text *)object->ro_data)->ot_left,
                    ((Object_Text *)object->ro_data)->ot_top,
                    -1,-1,
                    ((Object_Text *)object->ro_data)->ot_text,
                    TEXTPOS_ABSOLUTE);
                break;

            case OBJECT_BORDER:
                switch (((Object_Border *)object->ro_data)->ob_type) {
                    case BORDER_NORMAL:
                        Do3DBox(rp,
                            ((Object_Border *)object->ro_data)->ob_left,
                            ((Object_Border *)object->ro_data)->ob_top,
                            ((Object_Border *)object->ro_data)->ob_width,
                            ((Object_Border *)object->ro_data)->ob_height,
                            reqbase->rb_shine,reqbase->rb_shadow);
                        break;

                    case BORDER_RECESSED:
                        Do3DBox(rp,
                            ((Object_Border *)object->ro_data)->ob_left,
                            ((Object_Border *)object->ro_data)->ob_top,
                            ((Object_Border *)object->ro_data)->ob_width,
                            ((Object_Border *)object->ro_data)->ob_height,
                            reqbase->rb_shadow,reqbase->rb_shine);
                        break;

                    case BORDER_GROUP:
                        DoDo3DFrame(rp,
                            ((Object_Border *)object->ro_data)->ob_left,
                            ((Object_Border *)object->ro_data)->ob_top,
                            ((Object_Border *)object->ro_data)->ob_width,
                            ((Object_Border *)object->ro_data)->ob_height,
                            ((Object_Border *)object->ro_data)->ob_text,
                            reqbase->rb_shine,reqbase->rb_shadow);
                        break;

                    case BORDER_STRING:
                        Do3DStringBox(rp,
                            ((Object_Border *)object->ro_data)->ob_left,
                            ((Object_Border *)object->ro_data)->ob_top,
                            ((Object_Border *)object->ro_data)->ob_width,
                            ((Object_Border *)object->ro_data)->ob_height,
                            reqbase->rb_shine,reqbase->rb_shadow);
                        break;
                }
                if (((Object_Border *)object->ro_data)->ob_type!=BORDER_GROUP &&
                    ((Object_Border *)object->ro_data)->ob_text) {
                    R_ObjectText(reqbase,
                        ((Object_Border *)object->ro_data)->ob_left,
                        ((Object_Border *)object->ro_data)->ob_top,
                        ((Object_Border *)object->ro_data)->ob_width,
                        ((Object_Border *)object->ro_data)->ob_height,
                        ((Object_Border *)object->ro_data)->ob_text,
                        ((Object_Border *)object->ro_data)->ob_textpos);
                }
                break;

            case OBJECT_GADGET:
                if (((Object_Gadget *)object->ro_data)->og_text) {
                    R_ObjectText(reqbase,
                        ((Object_Gadget *)object->ro_data)->og_gadget.LeftEdge,
                        ((Object_Gadget *)object->ro_data)->og_gadget.TopEdge,
                        ((Object_Gadget *)object->ro_data)->og_gadget.Width,
                        ((Object_Gadget *)object->ro_data)->og_gadget.Height,
                        ((Object_Gadget *)object->ro_data)->og_text,
                        ((Object_Gadget *)object->ro_data)->og_textpos);
                }
                if (((Object_Gadget *)object->ro_data)->og_type==GADGET_TINYGLASS) {
                    DoDoGlassImage(reqbase->rb_window->RPort,
                        &((Object_Gadget *)object->ro_data)->og_gadget,
                        reqbase->rb_shine,
                        reqbase->rb_shadow,
                        0);
                }
/*
                if (((Object_Gadget *)object->ro_data)->og_gadget.GadgetType==GTYP_STRGADGET &&
                    IntuitionBase->LibNode.lib_Version<36) {
                    SetAPen(reqbase->rb_window->RPort,0);
                    RectFill(reqbase->rb_window->RPort,
                        ((Object_Gadget *)object->ro_data)->og_gadget.LeftEdge,
                        ((Object_Gadget *)object->ro_data)->og_gadget.TopEdge,
                        ((Object_Gadget *)object->ro_data)->og_gadget.LeftEdge+
                            ((Object_Gadget *)object->ro_data)->og_gadget.Width-1,
                        ((Object_Gadget *)object->ro_data)->og_gadget.TopEdge+
                            ((Object_Gadget *)object->ro_data)->og_gadget.Height-1);
                    RefreshStrGad(&((Object_Gadget *)object->ro_data)->og_gadget,
                        reqbase->rb_window);
                }
*/
                break;
        }
        object=object->ro_next;
    }
}

void __saveds R_ObjectText(register struct RequesterBase *reqbase __asm("a0"),
    register short left __asm("d0"),
    register short top __asm("d1"),
    register short width __asm("d2"),
    register short height __asm("d3"),
    register char *text __asm("a1"),
    register unsigned short textpos __asm("d4"))
{
    struct RastPort *rp;
    short x,y,text_width,text_height,cx,cy,len,got_uscore=-1,uscoreok=1;
    char *ptr,textbuf[82];

    rp=reqbase->rb_window->RPort;

    SetAPen(rp,reqbase->rb_fg);
    SetBPen(rp,reqbase->rb_bg);

    if (textpos&TEXTPOS_F_NOUSCORE) {
        uscoreok=0;
        textpos&=~TEXTPOS_F_NOUSCORE;
    }

    for (len=0,ptr=text;*ptr && len<80;len++,ptr++) {
        if (uscoreok && *ptr=='_' && got_uscore==-1) {
            got_uscore=len;
            --len;
        }
        else textbuf[len]=*ptr;
    }
    textbuf[len]=0;

    text_width=len*rp->Font->tf_XSize;
    text_height=rp->Font->tf_YSize;

    cx=left+((width-text_width)/2);
    cy=top+((height-text_height)/2);

    switch (textpos) {
        case TEXTPOS_ABSOLUTE:
            x=left;
            y=top;
            break;

        case TEXTPOS_CENTER:
            x=cx;
            y=cy;
            break;

        case TEXTPOS_LEFT:
            x=left-text_width-rp->Font->tf_XSize;
            y=cy;
            break;

        case TEXTPOS_RIGHT:
            x=left+width+rp->Font->tf_XSize;
            y=cy;
            break;

        case TEXTPOS_ABOVE:
            x=cx;
            y=top-rp->Font->tf_YSize-2;
            break;

        case TEXTPOS_BELOW:
            x=cx;
            y=top+height+2;
            break;
    }

    DoUScoreText(rp,textbuf,x,y+rp->Font->tf_Baseline,got_uscore);
}
