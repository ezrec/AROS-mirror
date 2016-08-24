/****************************************************************************
*                                                                           *
* project.c -- Lunapaint,                                                   *
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

#include "project.h"

void CreateProjectWindow ( struct WindowList *lst )
{

    STRPTR windowtitle = NULL;
    get ( lst->win, MUIA_Window_Title, &windowtitle );

    lst->projectWin = WindowObject,
        MUIA_Window_ScreenTitle, (IPTR) VERSION, //__(MSG_PRJ_SCR),
        MUIA_Window_Title, __(MSG_PRJ_WIN),
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','P','J'),
        MUIA_Window_CloseGadget, TRUE,
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, ( IPTR )TextObject,
                    MUIA_Text_Contents, __(MSG_PRJ_NAME),
                End,
                Child, ( IPTR )( lst->projName = StringObject,
                    MUIA_String_MaxLen, 128,
                    MUIA_Frame, MUIV_Frame_String,
                    MUIA_String_Contents, windowtitle,
                    MUIA_CycleChain, 1,
                End ),
                Child, ( IPTR )TextObject,
                    MUIA_Text_Contents, __(MSG_PRJ_AUTHOR),
                End,
                Child, ( IPTR )( lst->projAuthor = StringObject,
                    MUIA_String_MaxLen, 128,
                    MUIA_Frame, MUIV_Frame_String,
                    MUIA_String_Contents, ( IPTR )"",
                    MUIA_CycleChain, 1,
                End ),
            End,
            Child, ( IPTR )( lst->projBtnOk = SimpleButton ( _(MSG_BUTTON_OK) ) ),
        End,
    End;

    // Attach some standard methods to this gui

    DoMethod ( PaintApp, OM_ADDMEMBER, ( IPTR )lst->projectWin );

    DoMethod (
        lst->projectWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        lst->projectWin, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
    DoMethod (
        lst->projBtnOk, MUIM_Notify, MUIA_Pressed, FALSE,
        lst->win, 3, MUIM_CallHook, ( IPTR )&lst->projHook, ( APTR )lst
    );
}

void DestroyProjectWindow ( struct WindowList *lst )
{
    DoMethod ( PaintApp, OM_REMMEMBER, ( IPTR )lst->projectWin );
    MUI_DisposeObject ( lst->projectWin );
    lst->projectWin = NULL;
}

void SaveProject ( struct WindowList *lst )
{
    if ( lst->filename == NULL ) return;

    // Set up the header
    char version[ 16 ] = "Lunapaint_v1";
    STRPTR author = ( STRPTR )XGET ( lst->projAuthor, MUIA_String_Contents );
    STRPTR projname = ( STRPTR )XGET ( lst->projName, MUIA_String_Contents );

    struct LunapaintHeader header;
    memcpy ( header.version, version, 16 );
    memcpy ( header.projectName, projname, strlen ( projname ) + 1 );
    memcpy ( header.author, author, strlen ( author ) + 1 );
    header.width = ( short )lst->canvas->width;
    header.height = ( short )lst->canvas->height;
    header.layerCount = ( short )lst->canvas->totalLayers;
    header.frameCount = ( short )lst->canvas->totalFrames;
    // There's always 3 objects for the layers
    // one object for the opacity and one for the
    // layer name and one for the layer visibility:
    header.objectCount = 3 * lst->canvas->totalLayers;

    BPTR outFile;
    if ( ( outFile = Open ( lst->filename, MODE_NEWFILE ) ) == BNULL )
        return;

    // Write the header
    Write ( outFile, &header, sizeof ( struct LunapaintHeader ) );

    // Write description with size of desc first
    // TODO: make this work
    STRPTR text = " ";
    int length = 1;
    Write ( outFile, &length, 4 );
    Write ( outFile, text, length );

    // Write all the layers out!
    int buffers = lst->canvas->totalLayers * lst->canvas->totalFrames;
    int gfxsize = lst->canvas->width * lst->canvas->height * 8;
    struct gfxbuffer *buf = lst->canvas->buffer;
    int i = 0; for ( ; i < buffers; i++ )
    {
        Write ( outFile, buf->buf, gfxsize );
        buf = buf->nextbuf;
    }

    // Write all the objects -----------------------------------
    {
        buf = lst->canvas->buffer;
        struct LunaObjDesc objdesc;
        int i = 0;
        while ( buf != NULL )
        {
            int layer = i % lst->canvas->totalLayers;
            int frame = i / lst->canvas->totalLayers;
            // Write opacity
            objdesc = ( struct LunaObjDesc ){ LunaObj_LayerOpacity, layer, frame, 1 };
            Write ( outFile, &objdesc, sizeof ( struct LunaObjDesc ) );
            Write ( outFile, &buf->opacity, 1 );
            // Write layer visibility
            objdesc = ( struct LunaObjDesc ){ LunaObj_LayerVisibility, layer, frame, 1 };
            Write ( outFile, &objdesc, sizeof ( struct LunaObjDesc ) );
            char visible = buf->visible ? 1 : 0;
            Write ( outFile, &visible, 1 );
            if ( buf->name != NULL && strlen ( buf->name ) > 0 )
            {
                // Write layer name
                objdesc = ( struct LunaObjDesc ){ LunaObj_LayerName, layer, frame, strlen ( buf->name ) };
                Write ( outFile, &objdesc, sizeof ( struct LunaObjDesc ) );
                Write ( outFile, buf->name, strlen ( buf->name ) );
            }
            // Next
            buf = buf->nextbuf;
            i++;
        }
    }
    // Done with layer objects ---------------------------------

    Close ( outFile );
}

void LoadProject ( unsigned char *filename, BOOL useCurrentCanvas )
{
    if ( filename == NULL ) return;

    // Get a new filename container
    unsigned char *tmpFilename = AllocVec ( strlen ( filename ) + 1, MEMF_CLEAR );
    strcpy ( tmpFilename, filename );

    BPTR inputFile;
    if ( ( inputFile = Open ( tmpFilename, MODE_OLDFILE ) ) != BNULL )
    {
        // Get the lunapaint header
        struct LunapaintHeader *header = AllocVec ( sizeof ( struct LunapaintHeader ), MEMF_CLEAR );
        Read ( inputFile, header, sizeof ( struct LunapaintHeader ) );
        // test if the header is correct
        char verTpl[ ] = "Lunapaint_v1";
        int p = 0; for ( p = 0; p < sizeof(verTpl) - 1; p++ )
        {
            if ( verTpl[ p ] != header->version[ p ] )
            {
                FreeVec ( tmpFilename );
                FreeVec ( header );
                Close ( inputFile );
                ShowAlert(_(MSG_PRJ_ALERT_NOLUNA));
                return;
            }
        }

        // Read in the description
        int desclength;
        Read ( inputFile, &desclength, 4 );
        STRPTR text = AllocVec ( desclength + 1, MEMF_CLEAR );
        Read ( inputFile, text, desclength );

        // Load in object
        // <not implemented>

        // Load in the bitmap data
        int buffers = header->frameCount * header->layerCount;
        int framesize = header->width * header->height * 8;
        struct gfxbuffer *buf = AllocVec ( sizeof ( struct gfxbuffer ), MEMF_CLEAR );
        struct gfxbuffer *tmp = buf;
        int i = 0; for ( ; i < buffers; i++ )
        {
            tmp->buf = AllocVec ( framesize, MEMF_CLEAR );
            tmp->opacity = 100;                                 //
            tmp->visible = TRUE;                                // <- defaults
            tmp->name = AllocVec ( 2, MEMF_CLEAR );    //
            Read ( inputFile, tmp->buf, framesize );
            if ( i + 1 < buffers )
            {
                tmp->nextbuf = AllocVec ( sizeof ( struct gfxbuffer ), MEMF_CLEAR );
                tmp = tmp->nextbuf;
            }
            else
                tmp->nextbuf = NULL;
        }
        // Layer objects
        if ( header->objectCount > 0 )
        {
            for ( i = 0; i < header->objectCount; i++ )
            {
                struct LunaObjDesc desc;
                Read ( inputFile, &desc, sizeof ( struct LunaObjDesc ) );
                switch ( desc.type )
                {
                    case LunaObj_LayerOpacity:
                        {
                            char opacity;
                            Read ( inputFile, &opacity, 1 );
                            struct gfxbuffer *b = getGfxbufferFromList ( buf, desc.layer, desc.frame, header->layerCount, header->frameCount );
                            b->opacity = opacity;
                        }
                        break;
                    case LunaObj_LayerVisibility:
                        {
                            BOOL visible;
                            char vis;
                            Read ( inputFile, &vis, 1 );
                            visible = vis == 1 ? TRUE : FALSE;
                            struct gfxbuffer *b = getGfxbufferFromList ( buf, desc.layer, desc.frame, header->layerCount, header->frameCount );
                            b->visible = visible;
                        }
                        break;
                    case LunaObj_LayerName:
                        {
                            unsigned char *name = AllocVec ( desc.datalength + 1, MEMF_ANY|MEMF_CLEAR );
                            Read ( inputFile, name, desc.datalength );
                            struct gfxbuffer *b = getGfxbufferFromList ( buf, desc.layer, desc.frame, header->layerCount, header->frameCount );
                            if ( b->name ) FreeVec ( b->name ); // Free the dummy allocation
                            b->name = AllocVec ( desc.datalength + 1, MEMF_ANY|MEMF_CLEAR ); // +1 = null terminator
                            strncpy ( b->name, name, desc.datalength );
                            FreeVec ( name );
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        // We're done reading now
        Close ( inputFile );

        // If loading new
        if ( !useCurrentCanvas )
            addCanvaswindow ( header->width, header->height, header->layerCount, header->frameCount, FALSE );
        // Loading over old
        else
        {
            globalActiveCanvas->totalLayers = header->layerCount;
            globalActiveCanvas->totalFrames = header->frameCount;
            Destroy_Buffer ( globalActiveCanvas );
        }
        globalActiveCanvas->buffer = buf;
        setActiveBuffer ( globalActiveCanvas );


        if ( globalActiveWindow->filename != NULL )
            FreeVec ( globalActiveWindow->filename );
        globalActiveWindow->filename = tmpFilename;

        set ( globalActiveWindow->win, MUIA_Window_Title, ( IPTR )header->projectName );
        set ( globalActiveWindow->projName, MUIA_String_Contents, ( IPTR )header->projectName );
        set ( globalActiveWindow->projAuthor, MUIA_String_Contents, ( IPTR )header->author );

        // TODO: Do something with the project description

        if ( !useCurrentCanvas )
            set ( globalActiveWindow->win, MUIA_Window_Open, TRUE );

        // Free description
        FreeVec ( header );
        FreeVec ( text );
        return;
    }
    FreeVec ( tmpFilename );
}
