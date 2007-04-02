/*
    Copyright (C) 2006 by Michal Schulz
    $Id$

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License as 
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this program; if not, write to the
    Free Software Foundation, Inc.,
    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define DEBUG 1

#include <aros/debug.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>

#include <dos/dos.h>
#include <dos/dosextens.h>

#include <devices/inputevent.h>
#include <devices/input.h>
#include <devices/rawkeycodes.h>

#include <usb/usb.h>
#include <usb/usb_core.h>
#include <usb/hid.h>
#include "hid.h"

#include <proto/oop.h>
#include <proto/dos.h>
#include <proto/input.h>

static void kbd_process();

#define __(a) RAWKEY_##a
#define _(a) __(a)

/* 
 * Unusual convertions:
 *   PrintScreen -> Help 
 *   NumLock -> 0 (driver state change)
 */
static const uint8_t keyconv[] = {
    0, 0, 0, 0,                                                 /* 00 - 03 */
    _(A), _(B), _(C), _(D), _(E), _(F), _(G), _(H),             /* 04 - 0B */
    _(I), _(J), _(K), _(L), _(M), _(N), _(O), _(P),             /* 0C - 13 */
    _(Q), _(R), _(S), _(T), _(U), _(V), _(W), _(X),             /* 14 - 1B */
    _(Y), _(Z), _(1), _(2), _(3), _(4), _(5), _(6),             /* 1C - 23 */
    _(7), _(8), _(9), _(0), _(RETURN), _(ESCAPE), _(BACKSPACE), _(TAB), /* 24 - 2B */
    _(SPACE), _(MINUS), _(EQUAL), _(LBRACKET),                  /* 2C - 2F */
    _(RBRACKET), _(BACKSLASH), _(LESSGREATER), _(SEMICOLON),    /* 30 - 33 */
    _(QUOTE), _(TILDE), _(COMMA), _(PERIOD),                     /* 34 - 37 */
    _(SLASH), _(CAPSLOCK), _(F1), _(F2),                        /* 38 - 3B */
    _(F3), _(F4), _(F5), _(F6), _(F7), _(F8), _(F9), _(F10),    /* 3C - 43 */
    _(F11), _(F12), _(HELP), 0,                                 /* 44 - 47 */ 
    0, _(INSERT), _(HOME), _(PAGEUP),                           /* 49 - 4B */
    _(DELETE), _(END), _(PAGEDOWN), _(RIGHT),                   /* 4C - 4F */
    _(LEFT), _(DOWN), _(UP), 0,                                 /* 50 - 53 */
    0, 0, 0, _(KP_PLUS),                                        /* 54 - 57 */ // Keypad!!!!!!
    _(KP_ENTER), _(KP_1), _(KP_2), _(KP_3),                     /* 58 - 5B */
    _(KP_4), _(KP_5), _(KP_6), _(KP_7),                         /* 5C - 5F */
    _(KP_8), _(KP_9), _(KP_0), _(KP_DECIMAL),                   /* 60 - 63 */
    
};

#undef _
#undef __

void METHOD(USBKbd, Hidd_USBHID, ParseReport)
{
    KbdData *kbd = OOP_INST_DATA(cl, o);
    uint8_t *buff = msg->report;
    int i;
    
    CopyMem(kbd->code, kbd->prev_code, kbd->loc_keycnt + 1);

    for (i=0; i < kbd->loc_modcnt; i++)
    {
        if (hid_get_data(msg->report, &kbd->loc_mod[i]))
            kbd->code[0] |= 1 << i;
    }
    
    CopyMem(msg->report + kbd->loc_keycode.pos / 8, &kbd->code[1], kbd->loc_keycode.count);
    
    if (kbd->kbd_task)
        Signal(kbd->kbd_task, SIGBREAKF_CTRL_F);
}

OOP_Object *METHOD(USBKbd, Root, New)
{
    D(bug("[USBKbd] USBKeyboard::New()\n"));

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);
    if (o)
    {
        struct hid_data *d;
        struct hid_item h;
        
        KbdData *kbd = OOP_INST_DATA(cl, o);
        kbd->sd = SD(cl);
        kbd->o = o;
        kbd->hd = HIDD_USBHID_GetHidDescriptor(o);
        uint32_t flags;

        HIDD_USBHID_SetIdle(o, 500 / 4, 0);
        HIDD_USBHID_SetProtocol(o, 1);
        
        D(bug("[USBKbd::New()] Hid descriptor @ %p\n", kbd->hd));
        D(bug("[USBKbd::New()] Number of Report descriptors: %d\n", kbd->hd->bNumDescriptors));
        
        kbd->reportLength = AROS_LE2WORD(kbd->hd->descrs[0].wDescriptorLength);
        kbd->report = AllocVecPooled(SD(cl)->MemPool, kbd->reportLength);
        
        D(bug("[USBKbd::New()] Getting report descriptor of size %d\n", kbd->reportLength));
        
        HIDD_USBHID_GetReportDescriptor(o, kbd->reportLength, kbd->report);
      
        d = hid_start_parse(kbd->report, kbd->reportLength, hid_input);
        kbd->loc_modcnt = 0;
        
        while (hid_get_item(d, &h))
        {
            if (h.kind != hid_input || (h.flags & HIO_CONST) ||
                    HID_GET_USAGE_PAGE(h.usage) != HUP_KEYBOARD)
                continue;
            
            if (h.flags & HIO_VARIABLE)
            {
                kbd->loc_modcnt++;
                
                if (kbd->loc_modcnt > 8)
                {
                    bug("[USBKbd::New()] modifier code exceeds 8 bits size\n");
                    continue;
                }
                else
                    kbd->loc_mod[kbd->loc_modcnt-1] = h.loc;
            }
            else
            {
                kbd->loc_keycode = h.loc;
                kbd->loc_keycnt = h.loc.count;
            }
        }
        hid_end_parse(d);
        
        D(bug("[USBKbd::New()] %d key modifiers\n", kbd->loc_modcnt));
        D(bug("[USBKbd::New()] This keyboard reports at most %d simultanously pressed keys\n", kbd->loc_keycnt));
        
        kbd->prev_code = AllocVecPooled(SD(cl)->MemPool, kbd->loc_keycnt + 1);
        kbd->code = AllocVecPooled(SD(cl)->MemPool, kbd->loc_keycnt + 1);
        
        struct TagItem tags[] = {
                { NP_Entry,     (intptr_t)kbd_process },
                { NP_UserData,  (intptr_t)kbd },                
                { NP_Priority,  50 },
                { NP_Name,      (intptr_t)"HID Keyboard" },
                { TAG_DONE,     0UL },
        };

        kbd->kbd_task = CreateNewProc(tags);
    }
    
    return o;
}

struct pRoot_Dispose {
    OOP_MethodID        mID;
};

void METHOD(USBKbd, Root, Dispose)
{
    KbdData *kbd = OOP_INST_DATA(cl, o);
    
    Signal(kbd->kbd_task, SIGBREAKF_CTRL_C);
    
    if (kbd->report)
        FreeVecPooled(SD(cl)->MemPool, kbd->report);
    
    if (kbd->code)
        FreeVecPooled(SD(cl)->MemPool, kbd->code);

    if (kbd->prev_code)
        FreeVecPooled(SD(cl)->MemPool, kbd->prev_code);

    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

static void kbd_process()
{
    KbdData *kbd = (KbdData *)(FindTask(NULL)->tc_UserData);
    struct hid_staticdata *sd = kbd->sd;
    OOP_Object *o = kbd->o;
    OOP_Class *cl = sd->hidClass;
    uint32_t sigset;
    
    struct MsgPort *port = CreateMsgPort();
    struct IOStdReq *req = (struct IOStdReq *)CreateIORequest(port, sizeof(struct IOStdReq));
    struct InputEvent ie;
    struct Device *InputBase;
    
    if (OpenDevice("input.device", 0, (struct IORequest *)req, 0))
    {
        DeleteIORequest((struct IORequest *)req);
        DeleteMsgPort(port);
        kbd->kbd_task = NULL;
        
        bug("[Kbd] Failed to open input.device\n");
        
        return;
    }
    
    InputBase = req->io_Device;
    
    for (;;)
    {
        sigset = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F);
        
        if (sigset & SIGBREAKF_CTRL_C)
        {
            D(bug("[Kbd] USB mouse detached. Cleaning up\n"));
            
            CloseDevice((struct IORequest *)req);
            DeleteIORequest((struct IORequest *)req);
            DeleteMsgPort(port);
            return;
        }
        
        if (sigset & SIGBREAKF_CTRL_F)
        {
            int i;
            
            /* Check all new keycode buffers */
            for (i=0; i < kbd->loc_keycnt; i++)
            {
                int j;
                
                /* Code == 0? Ignore */
                if (!kbd->code[i+1])
                    continue;
                
                /* In case of failure assume that the previous report is the current one too
                 * and do nothing else. */
                if (kbd->code[i+1] == 1)
                {
                    CopyMem(kbd->prev_code, kbd->code, kbd->loc_keycnt + 1);
                    bug("[USBKbd] ERROR! Too many keys pressed at once?\n");
                    return;
                }
                
                /* Check whether this code exists in previous buffer */
                for (j=0; j < kbd->loc_keycnt; j++)
                    if (kbd->code[i+1] == kbd->prev_code[j+1])
                        break;
                
                /* Not in previous buffer. KeyDown event */
                if (j >= kbd->loc_keycnt)
                {
                    D(bug("[Kbd] KeyDown event for key %02x\n", kbd->code[i+1]));
                }
            }
            
            /* check all old keycode buffers */
            for (i=0; i < kbd->loc_keycnt; i++)
            {
                int j;
                
                /* Code == 0? Ignore */
                if (!kbd->prev_code[i+1])
                    continue;
               
                /* Check whether this code exists in previous buffer */
                for (j=0; j < kbd->loc_keycnt; j++)
                    if (kbd->prev_code[i+1] == kbd->code[j+1])
                        break;
                
                /* Not in previous buffer. KeyDown event */
                if (j >= kbd->loc_keycnt)
                {
                    D(bug("[Kbd] KeyUp event for key %02x\n", kbd->prev_code[i+1]));
                }
            }
        }
    }    
}
