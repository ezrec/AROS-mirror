/****************************************************************************
*                                                                           *
* text_to_brush.c -- Lunapaint,                                             *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#include <SDI_hook.h>

#include "text_to_brush.h"
#define TTLOAD_DEFAULT  (TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH )

Object *textToBrushWindow;
Object *ttbw_TextString;
Object *ttbw_RenderButton;
Object *ttbw_FontListview;
Object *ttbw_FontList;
Object *ttbw_FontSizeListview;
Object *ttbw_FontSizeList;

STRPTR FontList[ MAX_FONTNUM ];

struct Hook RenderTextToBrush_hook;


HOOKPROTONHNO(RenderTextToBrush_func, void, int *param)
{
    RenderTextToBrushBuffer ( );
}

void Init_TextToBrushWindow ( )
{
    static const char *fontSizes[] = {
        "6",  "8",  "9", "10", "11", "12", "13", "14", "15", "16",
        "17", "18", "19", "20", "21", "22", "23", "24", "25", "26",
        "26", "28", "29", "30", "31", "32", "33", "34", "35", "36",
        "37", "38", "39", "40", "41", "42", "43", "44", "45", "46",
        "47", "48", "49", "50", "51", "52", "53", "54", "55", "56",
        "57", "58", "59", "60", "61", "62", "63", "64", "65", "66",
        "67", "68", "69", "70", "71", "72", "73", "74", "75", "76",
        "77", "78", "79", "80", "81", "82", "83", "84", "85", "86",
        "87", "88", "89", "90", NULL
    };

    ttbw_FontSizeList = MUI_NewObject ( MUIC_List,
        MUIA_List_Title, _(MSG_TEXT_SIZE),
        MUIA_Frame, MUIV_Frame_ReadList,
        MUIA_List_SourceArray, ( APTR )fontSizes,
        TAG_END
    );

    ttbw_FontList = MUI_NewObject ( MUIC_List,
        MUIA_List_Title, _(MSG_TEXT_FONTS),
        MUIA_Frame, MUIV_Frame_ReadList,
        TAG_END
    );

    textToBrushWindow = WindowObject,
        MUIA_Window_Title, __(MSG_TEXT_WIN),
        MUIA_Window_ScreenTitle, __(MSG_TEXT_SCR),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','B','W'),
        MUIA_Window_SizeGadget, TRUE,
        WindowContents, ( IPTR )VGroup,
            Child, GroupObject,
                MUIA_FrameTitle, __(MSG_TEXT_SELECT),
                MUIA_Frame, MUIV_Frame_Group,
                Child, HGroup,
                    MUIA_Weight, 100,
                    Child, VGroup,
                        MUIA_Weight, 80,
                        Child, ( IPTR )( ttbw_FontListview = ListviewObject,
                            MUIA_Listview_List, ( IPTR )ttbw_FontList,
                        End ),
                    End,
                    Child, VGroup,
                        MUIA_Weight, 20,
                        Child, ( IPTR )( ttbw_FontSizeListview = ListviewObject,
                            MUIA_Listview_List, ( IPTR )ttbw_FontSizeList,
                        End ),
                    End,
                End,
            End,
            Child, GroupObject,
                MUIA_FrameTitle, __(MSG_TEXT_ENTER),
                MUIA_Frame, MUIV_Frame_Group,
                Child, HGroup,
                    MUIA_Weight, 100,
                    Child, VGroup,
                        MUIA_Weight, 70,
                        Child, ( IPTR )( ttbw_TextString = StringObject,
                            MUIA_String_Contents, ( IPTR )"ABCDEFGabcdefg!",
                            MUIA_Frame, MUIV_Frame_String,
                        End ),
                    End,
                    Child, VGroup,
                        MUIA_Weight, 30,
                        Child, ( IPTR )( ttbw_RenderButton = SimpleButton ( _(MSG_TEXT_RENDER) ) ),
                    End,
                End,
            End,
        End,
    End;
}

void Init_TextToBrushMethods ( )
{
    DoMethod (
        textToBrushWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        textToBrushWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );

    RenderTextToBrush_hook.h_Entry = ( HOOKFUNC )&RenderTextToBrush_func;

    DoMethod (
        ttbw_RenderButton, MUIM_Notify, MUIA_Pressed, FALSE,
        ttbw_RenderButton, 2, MUIM_CallHook, &RenderTextToBrush_hook
    );

    // Disable the keyboard when this window is activated
    DoMethod (
        textToBrushWindow, MUIM_Notify, MUIA_Window_Activate, TRUE,
        textToBrushWindow, 2, MUIM_CallHook, &DisableKeyboard_hook
    );
    DoMethod ( textToBrushWindow, MUIM_Notify, MUIA_Window_Open, TRUE,
        textToBrushWindow, 2, MUIM_CallHook, &DisableKeyboard_hook );
    // Enable the keyboard when this window is deactivated
    DoMethod (
        textToBrushWindow, MUIM_Notify, MUIA_Window_Activate, FALSE,
        textToBrushWindow, 2, MUIM_CallHook, &EnableKeyboard_hook
    );
    DoMethod ( textToBrushWindow, MUIM_Notify, MUIA_Window_Open, FALSE,
        textToBrushWindow, 2, MUIM_CallHook, &EnableKeyboard_hook );

    // TODO: Make these paths be configurable
    // Get all fonts listed out
    STRPTR SearchFolders[] = { "Ram:", "Sys:Fonts/", "Sys:Fonts/TrueType/", NULL };
    LONG fontpos = 0;
    LONG fontfolderpos = 0;

    while ( SearchFolders[ fontfolderpos ] != NULL )
    {
        BPTR lock;
        if ( ( lock = Lock ( ( CONST_STRPTR )SearchFolders[ fontfolderpos ], SHARED_LOCK ) ) != 0 )
        {
            struct FileInfoBlock *fib = AllocDosObject ( DOS_FIB, NULL );
            BOOL loop = Examine ( lock, fib );

            while ( loop )
            {
                if ( strlen ( fib->fib_FileName ) >= 5 && fib->fib_DirEntryType < 0 )
                {
                    if ( strstr ( fib->fib_FileName, ".ttf" ) )
                    {
                        STRPTR str = AllocVec ( 255, MEMF_ANY|MEMF_CLEAR );
                        STRPTR Fnt = AllocVec ( 255, MEMF_ANY|MEMF_CLEAR );
                        strcpy ( str, fib->fib_FileName );
                        sprintf ( Fnt, "%s%s", SearchFolders[ fontfolderpos ], fib->fib_FileName );
                        int i = 0, found = 0; for ( ; i < fontpos; i++ )
                        {
                            if ( strcmp ( FontList[ i ], Fnt ) == 1 )
                                found = 1;
                        }
                        if ( found == 1 )
                        {
                            FreeVec ( Fnt );
                            FreeVec ( str );
                        }
                        else
                        {
                            DoMethod ( ttbw_FontList, MUIM_List_InsertSingle, str, MUIV_List_Insert_Bottom );
                            FontList[ fontpos ] = Fnt;
                            fontpos++;
                        }
                    }
                }
                loop = ExNext ( lock, fib );
            }
            UnLock ( lock );
            FreeDosObject( DOS_FIB, fib );
        }
        fontfolderpos++;
    }
    set ( ttbw_FontList, MUIA_List_Active, 0 );
    set ( ttbw_FontSizeList, MUIA_List_Active, 3 );
    DoMethod ( ttbw_FontList, MUIM_List_Redraw, MUIV_List_Redraw_All );
}

void Exit_TextToBrushWindow ( )
{
    int i = 0;
    while ( FontList[ i ] != NULL )
    {
        FreeVec ( FontList[ i ] );
        i++;
    }
}


void RenderTextToBrushBuffer ( )
{
    STRPTR myFont = FontList[ XGET ( ttbw_FontList, MUIA_List_Active ) ];

    // Get current font size
    int fontSize = 0;
    STRPTR fontSizeStr = NULL;
    DoMethod (
        ttbw_FontSizeList, MUIM_List_GetEntry,
        XGET ( ttbw_FontSizeList, MUIA_List_Active ), &fontSizeStr
    );
    fontSize = atoi ( ( char * )fontSizeStr );
    STRPTR text = ( STRPTR )XGET ( ttbw_TextString, MUIA_String_Contents );

    unsigned int *buffer = NULL;
    struct Affrect rect = RenderTextToBuffer ( text, ( char *)myFont, fontSize, &buffer );
    brushTool.width = rect.w;
    brushTool.height = rect.h;
    globalBrushMode = 1; // <- prevent brush generation (same as when using clipbrush)

    // Convert to unsigned long long int from unsigned int
    int size = rect.w * rect.h;
    if ( size && buffer != NULL )
    {
        if ( brushTool.buffer != NULL )
            FreeVec ( brushTool.buffer );
        brushTool.buffer = AllocVec ( size * 8, MEMF_ANY );
        int i = 0; for ( ; i < size; i++ )
        {
            unsigned long long int r = ( buffer[ i ] >> 24 ) * 256;
            unsigned long long int g = ( ( buffer[ i ] << 8 ) >> 24 ) * 256;
            unsigned long long int b = ( ( buffer[ i ] << 16 ) >> 24 ) * 256;
            unsigned long long int a = ( ( buffer[ i ] << 24 ) >> 24 ) * 256;
            brushTool.buffer[ i ] = ( r << 48 ) | ( g << 32 ) | ( b << 16 ) | a;
        }
        FreeVec ( buffer );
    }
}

struct Affrect RenderTextToBuffer (
    unsigned char *text, char *font,
    int size, unsigned int **buffer
)
{
    int error;
    FT_Library library;
    FT_Face face;

    if ( ( error = FT_Init_FreeType( &library ) ) )
    {
        //printf ( "Couldn't initialize freetype library.\n" );
        goto error;
    }
    if ( ( error = FT_New_Face( library, font, 0, &face ) ) )
    {
        //printf ( "Couldn't initialize font face ( %s ).\n", font );
        goto error;
    }
    if ( ( error = FT_Set_Pixel_Sizes( face, 0, size ) ) )
    {
        //printf ( "Couldn't set pixel sizes.\n" );
        goto error;
    }

    FT_GlyphSlot slot = face->glyph; // recommended ft2 shortcut >:-D

    int bufWidth = 0, bufHeight = 0, c = 0, halfHeight = 0;

    // Calculate buffer size
    int textlen = strlen ( text );
    for ( c = 0; c < textlen; c++ )
    {
        FT_UInt glyph_index;
        glyph_index = FT_Get_Char_Index( face, text[ c ] );
        if ( ( error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER ) ) ) continue;
        bufWidth += slot->advance.x >> 6;
        if ( slot->bitmap.rows + slot->bitmap_top > bufHeight )
            bufHeight = slot->bitmap.rows + slot->bitmap_top;
    }
    halfHeight = bufHeight; bufHeight = bufHeight * 2;

    // Allocate memory for buffer and clear it, free if already allocated for
    int bufsize = bufWidth * bufHeight;
    unsigned int *buf = AllocVec ( bufsize * 4, MEMF_ANY|MEMF_CLEAR );
    *buffer = buf;

    // Make the text
    int xpos = 0;
    for ( c = 0; c < textlen; c++ )
    {
        FT_UInt glyph_index;
        glyph_index = FT_Get_Char_Index( face, text[ c ] );
        if ( ( error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER ) ) ) continue;
        // Blit over the bitmap from the glyph
        int bitmapsize = slot->bitmap.width * slot->bitmap.rows;
        struct rgbData rgb = paletteColorToRGB ( globalPalette[ currColor ] );
        int i = 0; for ( ; i < bitmapsize; i++ )
        {
            int x = i % slot->bitmap.width;
            int y = i / slot->bitmap.width;
            int pos = ( halfHeight - slot->bitmap_top + y ) * bufWidth + ( xpos + x + slot->bitmap_left );
            buf[ pos ] = ( rgb.r << 24 ) | ( rgb.g << 16 ) | ( rgb.b << 8 ) | slot->bitmap.buffer[ i ];
        }
        xpos += slot->advance.x >> 6;

    }

    // Return affected space
    struct Affrect rect;
    rect.x = 0; rect.y = 0;
    rect.w = bufWidth; rect.h = bufHeight;
    FT_Done_Face ( face );
    FT_Done_FreeType( library );
    return rect;

    // We never should end up here..
    error:
    printf ( "Error initializing freetype library or its parts.\n" );
    D(bug("Error initializing freetype library or its parts.\n"));
    FT_Done_Face ( face );
    FT_Done_FreeType( library );
    return ( struct Affrect ){ 0, 0, 0, 0 };
}
