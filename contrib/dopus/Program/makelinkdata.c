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

#include "dopus.h"

enum {
    MAKELINK_CANCEL,
    MAKELINK_OKAY,
    MAKELINK_NAME,
    MAKELINK_DESTNAME,
    MAKELINK_DESTSELECT,
    MAKELINK_TYPE};

static struct RequesterBase makelink_req={
    46,7,48,27,
    8,6,
    0,0,0,0,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,
    0,0,NULL,IDCMP_MOUSEBUTTONS|IDCMP_GADGETUP,NULL};

static struct TagItem
    makelink_type_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_CYCLE},
        {RO_GadgetID,MAKELINK_TYPE},
        {RO_Top,1},
        {RO_TopFine,2},
        {RO_Width,8},
        {RO_WidthFine,24},
        {RO_Height,1},
        {RO_HeightFine,6},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    makelink_name_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_STRING},
        {RO_GadgetID,MAKELINK_NAME},
        {RO_Top,1},
        {RO_TopFine,4},
        {RO_Height,1},
        {RO_HeightFine,2},
        {RO_Left,8},
        {RO_LeftFine,28},
        {RO_Width,38},
        {RO_StringBuf,0},
        {RO_StringLen,107},
        {RO_StringUndo,(IPTR)str_undobuffer},
        {TAG_END,0}},

    makelink_destname_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_STRING},
        {RO_GadgetID,MAKELINK_DESTNAME},
        {RO_Top,4},
        {RO_TopFine,8},
        {RO_Height,1},
        {RO_HeightFine,2},
//        {RO_Left,8},
        {RO_LeftFine,4},
        {RO_Width,46},
        {RO_WidthFine,2},
        {RO_StringBuf,0},
        {RO_StringLen,255},
        {RO_StringUndo,(IPTR)str_undobuffer},
        {TAG_END,0}},

    makelink_glass_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_TINYGLASS},
        {RO_GadgetID,MAKELINK_DESTSELECT},
        {RO_Top,4},
        {RO_TopFine,6},
        {RO_Left,47},
        {RO_LeftFine,2},
//        {RO_Width,2},
        {RO_WidthFine,24},
        {RO_Height,1},
        {RO_HeightFine,6},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    makelink_okay_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,MAKELINK_OKAY},
        {RO_Top,6},
        {RO_TopFine,11},
        {RO_Height,1},
        {RO_HeightFine,6},
        {RO_Width,10},
        {RO_TextNum,STR_OKAY},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    makelink_cancel_gadget[]={
        {RO_Type,OBJECT_GADGET},
        {RO_GadgetType,GADGET_BOOLEAN},
        {RO_GadgetID,MAKELINK_CANCEL},
        {RO_Top,6},
        {RO_TopFine,11},
        {RO_Height,1},
        {RO_HeightFine,6},
        {RO_Width,10},
        {RO_Left,36},
        {RO_LeftFine,32},
        {RO_TextNum,STR_CANCEL},
        {RO_TextPos,TEXTPOS_CENTER},
        {RO_HighRecess,TRUE},
        {TAG_END,0}},

    makelink_type_text[]={
        {RO_Type,OBJECT_BORDER},
        {RO_BorderType,BORDER_NONE},
        {RO_Width,8},
        {RO_WidthFine,24},
        {RO_TopFine,-1},
        {RO_Height,1},
        {RO_TextNum,STR_MAKELINK_TYPE},
        {RO_TextPos,TEXTPOS_CENTER},
        {TAG_END,0}},

    makelink_name_text[]={
        {RO_Type,OBJECT_BORDER},
        {RO_BorderType,BORDER_NONE},
        {RO_Left,8},
        {RO_LeftFine,28},
        {RO_Width,38},
        {RO_TopFine,-1},
        {RO_Height,1},
        {RO_TextNum,STR_MAKELINK_NAME},
        {RO_TextPos,TEXTPOS_CENTER},
        {TAG_END,0}},

    makelink_dest_text[]={
        {RO_Type,OBJECT_BORDER},
        {RO_BorderType,BORDER_NONE},
        {RO_Width,46},
        {RO_WidthFine,26},
        {RO_Top,3},
        {RO_TopFine,3},
        {RO_Height,1},
        {RO_TextNum,STR_MAKELINK_DESTINATION},
        {RO_TextPos,TEXTPOS_CENTER},
        {TAG_END,0}},

    *makelink_gadgets[]={
        makelink_name_gadget,
        makelink_type_gadget,
        makelink_destname_gadget,
        makelink_glass_gadget,
        makelink_okay_gadget,
        makelink_cancel_gadget,
        NULL};

int getmakelinkdata(char *namebuf, char *destbuf, int *type)
{
    ULONG class;
    UWORD gadgetid,__unused code;
    struct Window *swindow;
    struct Gadget *name_gad,*makelink_type_gad, *makelink_destname_gad;
    char *makelink_type_array[3] = {globstring[STR_MAKELINK_TYPE_SOFT], globstring[STR_MAKELINK_TYPE_HARD], NULL};
    char makelink_namebuf[108], makelink_destbuf[256];
    int makelink_type;
    int ret=0;

    strcpy(makelink_namebuf,namebuf);
    strcpy(makelink_destbuf,destbuf);
    makelink_type = type ? 1: 0;

    fix_requester(&makelink_req,globstring[STR_ENTER_MAKELINK_PARAMS]);

    set_reqobject(makelink_name_gadget,RO_StringBuf,(IPTR)makelink_namebuf);
    set_reqobject(makelink_destname_gadget,RO_StringBuf,(IPTR)makelink_destbuf);

    if (!(swindow=OpenRequester(&makelink_req)) ||
        !(name_gad=addreqgadgets(&makelink_req,makelink_gadgets,0,NULL)) ||
        !(AddRequesterObject(&makelink_req,makelink_type_text)) ||
        !(AddRequesterObject(&makelink_req,makelink_name_text)) ||
        !(AddRequesterObject(&makelink_req,makelink_dest_text))) {
        CloseRequester(&makelink_req);
        return(0);
    }
    RefreshRequesterObject(&makelink_req,NULL);

    makelink_type_gad=name_gad->NextGadget;
    makelink_destname_gad=makelink_type_gad->NextGadget;
    DoCycleGadget(makelink_type_gad,swindow,makelink_type_array,makelink_type);

    ActivateStrGad(name_gad,swindow);

    FOREVER {
        Wait(1<<swindow->UserPort->mp_SigBit);
        while ((IMsg=(struct IntuiMessage *)GetMsg(swindow->UserPort))) {
            class=IMsg->Class;
            code=IMsg->Code;
            switch (class)
             {
              case IDCMP_GADGETUP:
                gadgetid=((struct Gadget *) IMsg->IAddress)->GadgetID;
                break;
             }
            ReplyMsg((struct Message *) IMsg);

            switch (class) {
                case IDCMP_MOUSEBUTTONS:
                    ActivateStrGad(name_gad,swindow);
                    break;

                case IDCMP_GADGETUP:
                    switch (gadgetid) {
/*
                        case SELECT_SELECTBASE:
                            strcpy(buffer[select_base],sel_patternbuf);
                            if (++select_base>3) select_base=0;
                            DoCycleGadget(gadlist,swindow,select_base_array,select_base);
                            strcpy(sel_patternbuf,buffer[select_base]);
                            RefreshStrGad(gadlist->NextGadget,swindow);
                            ActivateStrGad(gadlist->NextGadget,swindow);
                            break;
                        case SELECT_PATTERN:
                            if (code==0) // RETURN
                              ret=select_type+1;
                            else if (code!=0xFFFF) // ESC
                              break;
*/
                        case MAKELINK_TYPE:
                            if (++makelink_type>1) makelink_type=0;
                            DoCycleGadget(makelink_type_gad,swindow,makelink_type_array,makelink_type);
                            ActivateStrGad(name_gad,swindow);
                            break;
                        case MAKELINK_DESTSELECT:
                          {
                            struct DOpusFileReq filereq;
                            char dirbuf[256],filebuf[FILEBUF_SIZE],*ptr;

                            filereq.dirbuf=dirbuf;
                            filereq.filebuf=filebuf;
                            filereq.window=swindow;
                            filereq.x=-2;
                            filereq.y=-2;
                            filereq.lines=15;
                            filereq.flags=0;
                            filereq.title=globstring[STR_FILE_REQUEST];
                            filereq.filearraykey=NULL;

                            LStrCpy(dirbuf,makelink_destbuf);
                            if ((ptr=BaseName(dirbuf))>dirbuf) {
                                LStrCpy(filebuf,ptr);
                                *ptr=0;
                            }
                            else if (CheckExist(dirbuf,NULL)<1) {
                                dirbuf[0]=0;
                                LStrCpy(filebuf,makelink_destbuf);
                            }
                            else filebuf[0]=0;
                            if (FileRequest(&filereq)) {
                                LStrCpy(makelink_destbuf,dirbuf);
                                TackOn(makelink_destbuf,filebuf,256);
                            }
                            RefreshStrGad(makelink_destname_gad,swindow);
                            ActivateStrGad(makelink_destname_gad,swindow);
                            break;
                          }
                        case MAKELINK_OKAY:
                            ret=1;
                            strcpy(namebuf,str_pathbuffer[data_active_window]);
                            TackOn(namebuf,makelink_namebuf,256);
                            strcpy(destbuf,makelink_destbuf);
                            *type=makelink_type;
                        case MAKELINK_CANCEL:
                            CloseRequester(&makelink_req);
                            return(ret);
                    }
                    break;
            }
        }
    }
}

int makelink(int rexx)
{
  char name[256], path[256];
  int mode;

  name[0]=path[0]=mode=0;

  if (rexx)
   {
    strcpy(name,rexx_args[0]);
    strcpy(path,rexx_args[1]);
    if (atoi(rexx_args[2])) mode = 1;
   }
  else if (!(getmakelinkdata(name,path,&mode))) return 0;

  if (name[0] && path[0])
   {
    BPTR lock;

    if (mode)
     {
      if ((lock = Lock(path,ACCESS_READ)))
       {
        if (MakeLink(name,(SIPTR)lock,FALSE))
         {
          dostatustext(str_okaystring);
          return 1;
         }
       }
      doerror(-1);
      return 0;
     }
    else
     {
      if (MakeLink(name,(SIPTR)path,TRUE))
       {
        dostatustext(str_okaystring);
        return 1;
       }
      doerror(-1);
      return 0;
     }
   }
  doerror(ERROR_REQUIRED_ARG_MISSING);
  return 0;
}
