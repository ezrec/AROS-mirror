/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#if defined(__AROS__)
    #define NO_INLINE_STDARG
    // For Nlist classes
#endif

#include "system_headers.h"

struct DisassemblerWinData {
    TEXT dwd_Title[WINDOW_TITLE_LENGTH];
    APTR dwd_TitleText;
    APTR dwd_SourceList;
    APTR dwd_SourceFloattext;
    APTR dwd_DisasmSizeSlider;
#if !defined(__amigaos4__)
    struct Library *dwd_DisassemblerBase;
#endif
#if defined(__MORPHOS__)
    struct Library *dwd_PPCDissBase;
#endif
    APTR dwd_Address;
    ULONG dwd_Range;
    BOOL dwd_ForceHexDump;
    STRPTR dwd_Buffer;
    ULONG dwd_BufferSize;
};

#if !defined(__amigaos4__)
#define DisassemblerBase        dwd->dwd_DisassemblerBase
#define PPCDissBase             dwd->dwd_PPCDissBase
#endif

struct DisassembleContext {
    STRPTR dc_Index;
    LONG dc_Size;
};

STATIC IPTR mNew( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    BOOL forceHexDump;
    IPTR list, disasmsource, sizeslider;

    forceHexDump = GetTagData(MUIA_DisassemblerWin_ForceHexDump, FALSE, msg->ops_AttrList);

    if ((obj = (Object *)DoSuperNew(cl, obj,
        MUIA_Window_ID, MakeID('D','I','S','A'),
        WindowContents, VGroup,
            Child, list = (IPTR)NListviewObject,
                MUIA_CycleChain, TRUE,
                MUIA_ContextMenu, NULL,
                MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None,
                MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
                MUIA_NListview_NList, disasmsource = (IPTR)NFloattextObject,
                    ReadListFrame,
                    MUIA_NList_Input, FALSE,
                    MUIA_Font, MUIV_Font_Fixed,
                    MUIA_ContextMenu, NULL,
                End,
            End,
            Child, (IPTR)HGroup,
                Child, (IPTR)MUI_MakeObject(MUIO_Label, (forceHexDump) ? txtHexdumpRange : txtDisassemblyRange, 0),
                Child, sizeslider = (IPTR)SliderObject,
                    MUIA_CycleChain, TRUE,
                    MUIA_Numeric_Min, 64,
                    MUIA_Numeric_Max, 1024,
                    MUIA_Numeric_Format, txtRangeBytes,
                End,
            End,
        End,
        TAG_MORE, msg->ops_AttrList)) != NULL)
    {
        struct DisassemblerWinData *dwd = INST_DATA(cl, obj);
        APTR parent;

        dwd->dwd_SourceList = (APTR)list;
        dwd->dwd_SourceFloattext = (APTR)disasmsource;
        dwd->dwd_DisasmSizeSlider = (APTR)sizeslider;
        dwd->dwd_Address = (APTR)GetTagData(MUIA_DisassemblerWin_Address, (IPTR)NULL, msg->ops_AttrList);
        dwd->dwd_Range = GetTagData(MUIA_DisassemblerWin_Range, 128, msg->ops_AttrList);
        dwd->dwd_ForceHexDump = forceHexDump;
        dwd->dwd_Buffer = tbAllocVecPooled(globalPool, 128);
        dwd->dwd_BufferSize = 128;

    #if !defined(__amigaos4__)
        dwd->dwd_DisassemblerBase = OpenLibrary(DISASSEMBLER_NAME, 40);
    #endif
    #if defined(__MORPHOS__)
        dwd->dwd_PPCDissBase = OpenLibrary("ppcdiss.library", 50);
    #endif

        set(obj, MUIA_Window_DefaultObject, list);
        nnset((APTR)sizeslider, MUIA_Numeric_Value, dwd->dwd_Range);

        parent = (APTR)GetTagData(MUIA_Window_ParentWindow, (IPTR)NULL, msg->ops_AttrList);

        DoMethod(parent,     MUIM_Window_AddChildWindow, obj);
        DoMethod(obj,        MUIM_Notify,              MUIA_Window_CloseRequest, TRUE,  MUIV_Notify_Application, 5, MUIM_Application_PushMethod, parent, 2, MUIM_Window_RemChildWindow, obj);
        DoMethod((APTR)sizeslider, MUIM_Notify,              MUIA_Numeric_Value,       MUIV_EveryTime, obj,            3, MUIM_Set, MUIA_DisassemblerWin_Range, MUIV_TriggerValue);
    }

    return (IPTR)obj;
}

STATIC IPTR mDispose( struct IClass *cl,
                       Object *obj,
                       Msg msg )
{
    struct DisassemblerWinData *dwd = INST_DATA(cl, obj);

    set(obj, MUIA_Window_Open, FALSE);
    set(dwd->dwd_SourceFloattext, MUIA_NFloattext_Text, NULL);

    if (dwd->dwd_Buffer) {
        tbFreeVecPooled(globalPool, dwd->dwd_Buffer);
    }
#if !defined(__amigaos4__)
    CloseLibrary(dwd->dwd_DisassemblerBase);
#endif
#if defined(__MORPHOS__)
    CloseLibrary(dwd->dwd_PPCDissBase);
#endif

    return DoSuperMethodA(cl, obj, msg);
}

STATIC IPTR mSet( struct IClass *cl,
                   Object *obj,
                   struct opSet *msg )
{
    struct DisassemblerWinData *dwd = INST_DATA(cl, obj);
    struct TagItem *tags, *tag;
    BOOL update = FALSE;

    tags = msg->ops_AttrList;
    while ((tag = NextTagItem((APTR)&tags)) != NULL) {
        switch (tag->ti_Tag) {
            case MUIA_DisassemblerWin_Address:
                dwd->dwd_Address = (APTR)tag->ti_Data;
                update = TRUE;
                break;

            case MUIA_DisassemblerWin_Range:
                dwd->dwd_Range = tag->ti_Data;
                update = TRUE;
                break;

            case MUIA_DisassemblerWin_ForceHexDump:
                dwd->dwd_ForceHexDump = tag->ti_Data;
                break;
        }
    }

    if (update) {
        CoerceMethod(cl, obj, MUIM_DisassemblerWin_Disassemble);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

STATIC IPTR mGet( struct IClass *cl,
                   Object *obj,
                   struct opGet *msg )
{
    struct DisassemblerWinData *dwd = INST_DATA(cl, obj);
    IPTR *store = msg->opg_Storage;

    switch (msg->opg_AttrID) {
        case MUIA_DisassemblerWin_Address:
            *store = (IPTR)dwd->dwd_Address; return TRUE;
            break;

        case MUIA_DisassemblerWin_Range:
            *store = dwd->dwd_Range; return TRUE;
            break;
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

#if defined(__MORPHOS__)
STATIC void _putchar_func( VOID );
STATIC struct EmulLibEntry _putchar = { TRAP_LIB, 0, (void(*)(void))_putchar_func };
STATIC void _putchar_func( VOID )
{
    UBYTE c = (UBYTE)REG_D0;
    struct DisassembleContext *dc = (struct DisassembleContext *)REG_A3;
#else
STATIC void ASM _putchar( REG(d0, UBYTE c),
                          REG(a3, struct DisassembleContext *dc) )
{
#endif
    if (c == 0x00) {
        *dc->dc_Index = 0x00;
    } else {
        if (dc->dc_Size > 0) {
            *dc->dc_Index++ = c;
            dc->dc_Size--;

            if (dc->dc_Size == 1) {
                *dc->dc_Index = 0x00;
                dc->dc_Size = 0;
            }
        }
    }
}

STATIC IPTR mDisassemble( struct IClass *cl,
                           Object *obj,
                           UNUSED Msg msg )
{
    struct DisassemblerWinData *dwd = INST_DATA(cl, obj);
    ULONG maxSize;

    if (dwd->dwd_Address == (APTR)-1)
    {
        DoMethod(dwd->dwd_SourceFloattext, MUIM_List_Clear);
        return 0;
    }

    maxSize = dwd->dwd_Range * 128;

    set(dwd->dwd_SourceList, MUIA_NList_Quiet, TRUE);

    if (maxSize != dwd->dwd_BufferSize) {
        STRPTR buffer;

        if ((buffer = tbAllocVecPooled(globalPool, maxSize)) != NULL) {
            if (dwd->dwd_Buffer) tbFreeVecPooled(globalPool, dwd->dwd_Buffer);
            dwd->dwd_Buffer = buffer;
            dwd->dwd_BufferSize = maxSize;
        }
    }

    if (dwd->dwd_Buffer) {
        struct DisassembleContext dc;
        TEXT buffer[128 + 2];

        dc.dc_Index = dwd->dwd_Buffer;
        dc.dc_Size = dwd->dwd_BufferSize;

    #if defined(__amigaos4__)
        if (dwd->dwd_ForceHexDump) {
            LONG size;
            UWORD *addr;

            addr = (UWORD *)dwd->dwd_Address;
            size = dwd->dwd_Range;
            if (size & 1) size++;
            while (size > 0) {
                LONG s;
                LONG word;
                UWORD memory[8];

                RawDoFmt("$%08lx:", (APTR)&addr, (void(*)(void))&_putchar, &dc);
                CopyMemQuick(addr, memory, sizeof(memory));

                s = size;
                for (word = 0; word < 8; word++) {
                    ULONG data;

                    data = memory[word];

                    RawDoFmt(" %04lx", (APTR)&data, (void(*)(void))&_putchar, &dc);

                    s -= 2;
                    if (s <= 0) {
                        break;
                    }
                }

                if (s <= 0 && size < 16) {
                    for (word = 0; word < (16 - size) / 2; word++) RawDoFmt("     ", NULL, (void(*)(void))&_putchar, &dc);
                }
                RawDoFmt("    ", NULL, (void(*)(void))&_putchar, &dc);

                s = size;
                for (word = 0; word < 8; word++) {
                    UWORD data;
                    ULONG bytes[2];

                    data = memory[word];
                    bytes[0] = isprint((data >> 8) & 0xff) ? ((data >> 8) & 0xff) : '.';
                    bytes[1] = isprint(data & 0xff) ? (data & 0xff) : '.';

                    RawDoFmt("%lc%lc", (APTR)&bytes, (void(*)(void))&_putchar, &dc);

                    s -= 2;
                    if (s <= 0) {
                        break;
                    }
                }

                RawDoFmt("\n", NULL, (void(*)(void))&_putchar, &dc);

                addr += 8;
                size -= 16;
            }
        } else {
            struct DebugIFace *IDebug;

            if ((IDebug = (struct DebugIFace *)GetInterface((struct Library *)SysBase, "debug", 1, NULL)) != NULL) {
                ULONG done;
                APTR thisAddr, nextAddr;

                dwd->dwd_Buffer[0] = '\0';

                if (IsNative(dwd->dwd_Address)) {
                    done = 0;
                    thisAddr = dwd->dwd_Address;
                    do {
                        TEXT opcode[64], operand[64];
                        union {
                            UWORD int16[2];
                            ULONG int32;
                        } hexwords;

                        hexwords.int32 = *((ULONG *)thisAddr);

                        nextAddr = IDebug->DisassembleNative(thisAddr, opcode, operand);
                        _snprintf(buffer, sizeof(buffer), "$%08lx: %04lx %04lx   %-10s %s\n", thisAddr, hexwords.int16[0], hexwords.int16[1], opcode, operand);
                        _strcatn(dwd->dwd_Buffer, buffer, dwd->dwd_BufferSize);
                        done = (ULONG)nextAddr - (ULONG)dwd->dwd_Address;
                        thisAddr = nextAddr;
                    } while (done < dwd->dwd_Range);
                } else {
                    done = 0;
                    thisAddr = dwd->dwd_Address;
                    do {
                        TEXT opcode[64], operand[64];

                        nextAddr = IDebug->Disassemble68k(thisAddr, opcode, operand);
                        _snprintf(buffer, sizeof(buffer), "$%08lx: %-10s %s\n", thisAddr, opcode, operand);
                        _strcatn(dwd->dwd_Buffer, buffer, dwd->dwd_BufferSize);
                        done = (ULONG)nextAddr - (ULONG)dwd->dwd_Address;
                        thisAddr = nextAddr;
                    } while (done < dwd->dwd_Range);
                }

                DropInterface((struct Interface *)IDebug);
            }
        }
    #else
        #if defined(__MORPHOS__)
        if (dwd->dwd_PPCDissBase && !dwd->dwd_ForceHexDump && *(UBYTE *)dwd->dwd_Address == 0xff && isValidPointer(*((struct EmulLibEntry *)dwd->dwd_Address)->Func)) {
            /* PPCDiss is missing from aboxstubs */
            struct TagItem tags[] = {
                { DISASS_DEFINEPC, (IPTR)*((struct EmulLibEntry *)dwd->dwd_Address)->Func },
                { DISASS_SHOWDATA, TRUE },
                { TAG_DONE, 0 }
            };
            APTR dobj;

            dobj = PPCDissCreateObjectTagList(tags);

            if (dobj)
            {
                LONG todo = dwd->dwd_Range;

                while (todo >= 4)
                {
                    struct TagItem tags[] = { { DISASS_BUFFER, (IPTR)buffer }, { TAG_DONE, 0 } };

                    todo -= 4;
                    PPCDissTranslateTagList(dobj, tags);

                    _strcatn(dwd->dwd_Buffer, buffer, dwd->dwd_BufferSize);
                    _strcatn(dwd->dwd_Buffer, "\n", dwd->dwd_BufferSize);
                }

                if (todo > 0)
                {
                    CONST_STRPTR fmt;
                    TEXT line[16];
                    UBYTE *ptr;

                    fmt = todo == 1 ? "%02.2x" : todo == 2 ? "%02.2x%02.2x" : "%02.2x%02.2x%02.2x";
                    ptr = (UBYTE *)(((IPTR)*((struct EmulLibEntry *)dwd->dwd_Address)->Func + dwd->dwd_Range) - todo);

                    /* Must use NewRawDoFmt() to match with ppcdiss.library output */
                    NewRawDoFmt(fmt, NULL, line, ptr[0], ptr[1], ptr[2]);
                    NewRawDoFmt("$%08lx:  %s\n", NULL, buffer, ptr, line);
                    _strcatn(dwd->dwd_Buffer, buffer, dwd->dwd_BufferSize);
                }

                PPCDissDeleteObject(dobj);
            }
        } else
        #endif
        if (dwd->dwd_DisassemblerBase && !dwd->dwd_ForceHexDump) {
            struct DisData ds;

            memset(&ds, 0x00, sizeof(ds));
            ds.ds_From = dwd->dwd_Address;
            ds.ds_UpTo = (APTR)((IPTR)dwd->dwd_Address + dwd->dwd_Range);
            ds.ds_PC = NULL;
            ds.ds_PutProc = (void *)&_putchar;
            ds.ds_UserData = &dc;
            ds.ds_UserBase = NULL;
            Disassemble(&ds);
        } else {
            LONG size;
            UBYTE *addr;

            if (!dwd->dwd_ForceHexDump) {
                RawDoFmt(msgCannotOpenDisasmLibrary, NULL, (void(*)(void))&_putchar, &dc);
            }

            addr = (UBYTE *)dwd->dwd_Address;
            size = dwd->dwd_Range;
            while (size > 0) {
                LONG s;
                UWORD i;

                RawDoFmt(ADDRESS_FORMAT ":", (APTR)&addr, (void(*)(void))&_putchar, &dc);

                s = size;
                for (i = 0; i < 16; i++) {
                    ULONG data;


                    if (i % 2 == 0)
                        RawDoFmt(" ", NULL, (void(*)(void))&_putchar, &dc);
                    if (s > 0) {
                        data = addr[i];
                        RawDoFmt("%02lx", (APTR)&data, (void(*)(void))&_putchar, &dc);
                    }
                    else
                        RawDoFmt("  ", NULL, (void(*)(void))&_putchar, &dc);

                    s--;
                }

                RawDoFmt("    ", NULL, (void(*)(void))&_putchar, &dc);

                s = size;
                for (i = 0; i < 16; i++) {
                    UBYTE ch = addr[i];
                    if (!isprint(ch))
                        ch = '.';
                    RawDoFmt("%lc", (APTR)&ch, (void(*)(void))&_putchar, &dc);

                    s--;
                    if (s <= 0) {
                        break;
                    }
                }

                RawDoFmt("\n", NULL, (void(*)(void))&_putchar, &dc);

                addr += 16;
                size -= 16;
            }
        }
    #endif

        dwd->dwd_Buffer[strlen(dwd->dwd_Buffer) - 1] = 0x00;

        _snprintf(buffer, sizeof(buffer), ADDRESS_FORMAT " -> " ADDRESS_FORMAT, dwd->dwd_Address, dwd->dwd_Address + dwd->dwd_Range - 1);
        set(obj, MUIA_Window_Title, MyGetChildWindowTitle((dwd->dwd_ForceHexDump) ? txtHexdumpTitle : txtDisassemblyTitle, buffer, dwd->dwd_Title, sizeof(dwd->dwd_Title)));
    }

    set(dwd->dwd_SourceFloattext, MUIA_Floattext_Text, dwd->dwd_Buffer);
    set(dwd->dwd_SourceList, MUIA_NList_Quiet, FALSE);

    return 0;
}

DISPATCHER(DisassemblerWinDispatcher)
{
    switch (msg->MethodID) {
        case OM_NEW:                           return (mNew(cl, obj, (APTR)msg));
        case OM_DISPOSE:                       return (mDispose(cl, obj, (APTR)msg));
        case OM_SET:                           return (mSet(cl, obj, (APTR)msg));
        case OM_GET:                           return (mGet(cl, obj, (APTR)msg));
        case MUIM_DisassemblerWin_Disassemble: return (mDisassemble(cl, obj, (APTR)msg));
    }

    return (DoSuperMethodA(cl, obj, msg));
}

APTR MakeDisassemblerWinClass( void )
{
    return MUI_CreateCustomClass(NULL, NULL, ParentWinClass, sizeof(struct DisassemblerWinData), ENTRY(DisassemblerWinDispatcher));
}

