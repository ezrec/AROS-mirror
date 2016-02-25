/****************************************************************************
*                                                                           *
* export_import.c -- Lunapaint,                                             *
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
#include "export_import.h"

Object *exportCycDT;
Object *exportBtnCancel;
Object *exportBtnExport;
Object *exportPopFilename;
Object *exportMode;

Object *exportAnimMethod;
Object *exportAnimMode;
Object *exportAnimRangeStart;
Object *exportAnimRangeEnd;
Object *exportAnimBtnExport;
Object *exportAnimBtnCancel;

Object *importCycDT;
Object *importBtnCancel;
Object *importBtnImport;
Object *importPopFilename;

Object *exportWindow;
Object *importWindow;

struct Hook export_hook;
struct Hook import_hook;
struct Hook exportanimation_hook;

static STRPTR exp_datatypes[]   = { NULL, NULL, NULL, NULL, NULL };
static STRPTR exp_mode[]        = { NULL, NULL, NULL };
static STRPTR exp_titles[]      = { NULL, NULL, NULL };
static STRPTR exp_anim_mode[]   = { NULL, NULL, NULL };
static STRPTR exp_anim_method[] = { NULL, NULL, NULL };
static STRPTR imp_datatypes[]   = { NULL, NULL };

HOOKPROTONHNO(exportanimation_func, void, int *param)
{

    int datatype = 0;
    unsigned char *filename = NULL;
    get ( exportCycDT, MUIA_Cycle_Active, &datatype );
    get ( exportPopFilename, MUIA_String_Contents, &filename );

    if ( filename == NULL )
        return;

    createImageFromAnimation ( globalActiveCanvas, datatype, filename );

}



HOOKPROTONHNO(export_func, void, int *param)
{

    int mode = 0, datatype = 0;
    unsigned char *filename = NULL;
    get ( exportMode, MUIA_Cycle_Active, &mode );
    get ( exportCycDT, MUIA_Cycle_Active, &datatype );
    get ( exportPopFilename, MUIA_String_Contents, &filename );

    if ( filename == NULL )
        return;

    unsigned int *buffer = generateExportableBuffer ( globalActiveCanvas, mode, datatype );

    // Export with the JPEG datatype
    if ( datatype == 0 )
    {
        exportDT ( globalActiveCanvas->width, globalActiveCanvas->height, buffer, filename, "jpeg" );
    }
    // Export with PNG code
    else if ( datatype == 1 )
    {
        exportPNG ( globalActiveCanvas->width, globalActiveCanvas->height, buffer, filename );
    }
    // Save in RAW (64-bit) format
    else if ( datatype == 2 )
    {
        /*BPTR myfile;
        if ( ( myfile = Open ( filename, MODE_READWRITE ) ) != NULL )
        {
            Write ( myfile, globalActiveCanvas->activebuffer, sizeof ( unsigned long long int ) * ( globalActiveCanvas->width * globalActiveCanvas->height ) );
            Close ( myfile );
        }
        else printf ( "Failed to save image raw..\n" );*/
        printf ( "64-bit export temporarily disabled.\n" );
    }
    // Save another in raw format
    else if ( datatype == 3 )
    {
        BPTR myfile;
        if ( ( myfile = Open ( filename, MODE_NEWFILE ) ) != NULL )
        {
            Write ( myfile, buffer, sizeof ( unsigned int ) * ( globalActiveCanvas->width * globalActiveCanvas->height ) );
            Close ( myfile );
        }
        else printf ( "Failed to save image raw..\n" );
    }

    FreeVec ( buffer );
}

HOOKPROTONHNO(import_func, void, int *param)
{

    int datatype = 0;
    unsigned char *filename = NULL;
    get ( importCycDT, MUIA_Cycle_Active, &datatype );
    get ( importPopFilename, MUIA_String_Contents, &filename );

    switch ( datatype )
    {
        case 0:
            // We have a filename!!
            if ( filename != NULL )
            {
                BPTR myfile;
                if ( ( myfile = Open ( filename, MODE_OLDFILE ) ) != NULL )
                {
                    Read (
                        myfile, globalActiveCanvas->activebuffer,
                        8 * globalActiveCanvas->width * globalActiveCanvas->height
                    );
                    Close ( myfile );
                    globalActiveCanvas->winHasChanged = TRUE;
                    DoMethod ( globalActiveWindow->area, MUIM_Draw, FALSE );
                    set ( importWindow, MUIA_Window_Open, FALSE );
                }
                else printf ( "Failed to load image raw..\n" );
            }
            break;
        default: break;
    }

}
unsigned int *generateExportableBuffer ( struct oCanvas *canvas, int mode, int datatype )
{
    unsigned int *buffer = NULL;

    if ( mode == 0 )
    {
        if ( ( buffer = AllocVec ( 4 * canvas->width * canvas->height, MEMF_ANY|MEMF_CLEAR ) ) == NULL )
            return 0;

        int size = canvas->width * canvas->height;
        int i = 0; for ( ; i < size; i++ )
        {
            struct rgba64 c64 = *( struct rgba64 *)&canvas->activebuffer[ i ];

            // Align the colors reverse
            c64 = ( struct rgba64 ){ c64.a, c64.b, c64.g, c64.r };

            // Set the colors in 32-but order
            // datatype 0 is JPEG
            if ( datatype == 0 )
            {
                buffer[ i ] = *( unsigned int *)&( ( struct rgba32 ){
                    ( double )c64.a / 256, ( double )c64.r / 256,
                    ( double )c64.g / 256, ( double )c64.b / 256
                } );
            }
            // If datatype is png, include all channels
            else if ( datatype == 1 )
            {
                buffer[ i ] = *( unsigned int *)&( ( struct rgba32 ){
                    ( double )c64.r / 256, ( double )c64.g / 256,
                    ( double )c64.b / 256, ( double )c64.a / 256
                } );
            }
            // Others
            else
            {
                buffer[ i ] = *( unsigned int *)&( ( struct rgba32 ){
                    ( double )c64.b / 256, ( double )c64.g / 256,
                    ( double )c64.r / 256, ( double )c64.a / 256
                } );
            }
        }
    }
    else if ( mode == 1 )
    {
        // Remove tool graphic that's overlayn
        removePrevToolPreview ( );
        ULONG tool = globalCurrentTool;
        globalCurrentTool = -1;

        // Get flattened layers without tool preview
        buffer = renderCanvas ( canvas, 0, 0, canvas->width, canvas->height, TRUE );

        // Reset tool setting
        globalCurrentTool = tool;

        // Fix color order for different storage formats
        int size = canvas->width * canvas->height;
        int i = 0; for ( ; i < size; i++ )
        {
            struct rgba32 c = *( struct rgba32 *)&buffer[ i ];

            // Set the colors in 32-bit order
            // datatype 0 is JPEG
            if ( datatype == 0 )
                c = ( struct rgba32 ){ c.a, c.r, c.g, c.b };
            else if ( datatype == 1 )
                c = ( struct rgba32 ){ c.r, c.g, c.b, c.a };
            else
                c = ( struct rgba32 ){ c.b, c.g, c.r, c.a };

            buffer[ i ] = *( unsigned int *)&c;
        }
    }
    return buffer;
}

void makeExportWindow ( )
{
    exp_datatypes[0] = _(MSG_EXPORT_JPEG);
    exp_datatypes[1] = _(MSG_EXPORT_PNG);
    exp_datatypes[2] = _(MSG_EXPORT_RAW64);
    exp_datatypes[3] = _(MSG_EXPORT_RAW32);

    exp_mode[0] = _(MSG_EXPORT_ACTIV_LAYER);
    exp_mode[1] = _(MSG_EXPORT_FLAT_LAYER);

    exp_titles[0] = _(MSG_EXPORT_ONE_FRAME);
    exp_titles[1] = _(MSG_EXPORT_FRAMES);

    exp_anim_mode[0] = _(MSG_EXPORT_ACTIV_LAYER);
    exp_anim_mode[1] = _(MSG_EXPORT_FLAT_LAYER);

    exp_anim_method[0] = _(MSG_EXPORT_FILEPERFRAME);
    exp_anim_method[1] = _(MSG_EXPORT_STRIP);

    exportWindow = WindowObject,
        MUIA_Window_ScreenTitle, __(MSG_EXPORT_SCR),
        MUIA_Window_Title, __(MSG_EXPORT_WIN),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','W','E'),
        WindowContents, VGroup,
            Child, GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, TextObject,
                    MUIA_Text_Contents, _(MSG_EXPORT_FILENAME),
                End,
                Child, HGroup,
                    Child, PopaslObject,
                        ASLFR_DoSaveMode, TRUE,
                        MUIA_Popstring_String, exportPopFilename = MUI_MakeObject (
                            MUIO_String, NULL, 200
                        ),
                        MUIA_Popstring_Button, PopButton ( MUII_PopFile ),
                        MUIA_CycleChain, 1,
                    End,
                End,
                Child, VGroup,
                    Child, TextObject,
                        MUIA_Text_Contents, _(MSG_EXPORT_DT),
                    End,
                    Child, exportCycDT = CycleObject,
                        MUIA_Cycle_Entries, exp_datatypes,
                        MUIA_CycleChain, 1,
                    End,
                End,
            End,
            Child, RegisterObject,
                MUIA_Register_Titles, exp_titles,
                MUIA_Register_Frame, MUIV_Frame_Group,
                /*
                    Image export
                */
                Child, GroupObject,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_CycleChain, 1,
                        Child, VGroup,
                            Child, exportMode = CycleObject,
                                MUIA_Cycle_Entries, exp_mode,
                                MUIA_CycleChain, 1,
                            End,
                        End,
                        Child, HGroup,
                            Child, exportBtnExport = SimpleButton ( __(MSG_EXPORT_EXPORT) ),
                            Child, exportBtnCancel = SimpleButton ( __(MSG_EXPORT_CANCEL) ),
                        End,
                    End,
                End,
                /*
                    Animation export
                */
                Child, GroupObject,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_CycleChain, 1,
                        Child, VGroup,
                            Child, exportAnimMode = CycleObject,
                                MUIA_Cycle_Entries, exp_anim_mode,
                                MUIA_CycleChain, 1,
                            End,
                            Child, exportAnimMethod = CycleObject,
                                MUIA_Cycle_Entries, exp_anim_method,
                                MUIA_CycleChain, 1,
                            End,
                        End,
                        Child, VGroup,
                            Child, HGroup,
                                Child, TextObject,
                                    MUIA_Text_Contents, __(MSG_EXPORT_FRAMERANGE),
                                End,
                                Child, exportAnimRangeStart = StringObject,
                                    MUIA_String_Accept, ( IPTR )"0123456789",
                                    MUIA_String_Format, MUIV_String_Format_Right,
                                    MUIA_String_Integer, 1,
                                    MUIA_String_MaxLen, 10,
                                    MUIA_Frame, MUIV_Frame_String,
                                    MUIA_CycleChain, 1,
                                End,
                                Child, TextObject,
                                    MUIA_Text_Contents, " - ",
                                End,
                                Child, exportAnimRangeEnd = StringObject,
                                    MUIA_String_Accept, ( IPTR )"0123456789",
                                    MUIA_String_Format, MUIV_String_Format_Right,
                                    MUIA_String_Integer, 1,
                                    MUIA_String_MaxLen, 10,
                                    MUIA_Frame, MUIV_Frame_String,
                                    MUIA_CycleChain, 1,
                                End,
                            End,
                            Child, HGroup,
                                Child, exportAnimBtnExport = SimpleButton ( __(MSG_EXPORT_FRAMES) ),
                                Child, exportAnimBtnCancel = SimpleButton ( __(MSG_EXPORT_CANCEL) ),
                            End,
                        End,
                    End,
                End,
            End,
        End,
    End;

    DoMethod (
        exportWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        ( IPTR )exportWindow, 2, MUIM_Set, MUIA_Window_Open
    );
    DoMethod (
        exportBtnCancel, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )exportWindow, 2, MUIM_Set, MUIA_Window_Open
    );
    DoMethod (
        exportAnimBtnCancel, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )exportWindow, 2, MUIM_Set, MUIA_Window_Open
    );

    // Setup hook
    export_hook.h_Entry = ( HOOKFUNC )&export_func;
    DoMethod (
        exportBtnExport, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )exportWindow, 2, MUIM_CallHook, &export_hook
    );
    exportanimation_hook.h_Entry = ( HOOKFUNC )&exportanimation_func;
    DoMethod (
        exportAnimBtnExport, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )exportWindow, 2, MUIM_CallHook, &exportanimation_hook
    );
}

void makeImportWindow ( )
{
    imp_datatypes[0] = _(MSG_EXPORT_RAW64);

    importWindow = WindowObject,
        MUIA_Window_ScreenTitle, __(MSG_IMPORT_SCR),
        MUIA_Window_Title, __(MSG_IMPORT_WIN),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','W','I'),
        WindowContents, VGroup,
            Child, GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, VGroup,
                    Child, TextObject,
                        MUIA_Text_Contents, __(MSG_IMPORT_FILENAME),
                    End,
                    Child, HGroup,
                        Child, PopaslObject,
                            ASLFR_DoSaveMode, TRUE,
                            MUIA_Popstring_String, importPopFilename = MUI_MakeObject ( MUIO_String, NULL, 200 ),
                            MUIA_CycleChain, 1,
                            MUIA_Popstring_Button, PopButton ( MUII_PopFile ),
                        End,
                    End,
                    Child, TextObject,
                        MUIA_Text_Contents, __(MSG_IMPORT_ACTIV_LAYER),
                    End,
                    Child, TextObject,
                        MUIA_Text_Contents, __(MSG_IMPORT_DT),
                    End,
                    Child, importCycDT = CycleObject,
                        MUIA_Cycle_Entries, imp_datatypes,
                        MUIA_CycleChain, 1,
                    End,
                End,
            End,
            Child, GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, HGroup,
                    Child, importBtnImport = SimpleButton ( __(MSG_IMPORT_IMPORT) ),
                    Child, importBtnCancel = SimpleButton ( __(MSG_IMPORT_CANCEL) ),
                End,
            End,
        End,
    End;

    DoMethod (
        importWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        ( IPTR )importWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
    DoMethod (
        importBtnCancel, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )importWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );

    // Setup hook
    import_hook.h_Entry = ( HOOKFUNC )&import_func;
    DoMethod (
        importBtnImport, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )importWindow, 3, MUIM_CallHook, &import_hook
    );
}

void createImageFromAnimation ( struct oCanvas *canv, int datatype, char *filename )
{
    // Get some rules
    int AnimMode = 0, AnimMethod = 0, FrameStart = 0, FrameEnd = 0;
    get ( exportAnimMode, MUIA_Cycle_Active, &AnimMode );
    get ( exportAnimMethod, MUIA_Cycle_Active, &AnimMethod );
    get ( exportAnimRangeStart, MUIA_String_Integer, &FrameStart );
    get ( exportAnimRangeEnd, MUIA_String_Integer, &FrameEnd );

    // Restrict the export frame range
    if ( FrameStart < 1 ) FrameStart = 1;
    if ( FrameEnd < 1 ) FrameEnd = 1;
    if ( FrameStart > FrameEnd )
    {
        int tmp = FrameStart;
        FrameStart = FrameEnd;
        FrameEnd = tmp;
    }
    if ( FrameEnd > canv->totalFrames )
        FrameEnd = canv->totalFrames;

    // One vertical strip
    if ( AnimMethod == 1 )
    {
        // Make room for buffer
        int framecount = 1 + FrameEnd - FrameStart;
        int framesize = canv->width * canv->height * 4;
        int framelength = canv->width * canv->height;
        int bufsize = framesize * framecount;
        int tmpFrame = canv->currentFrame;
        int offset = 0;

        unsigned int *animBuf = NULL;
        // not enough mem?
        if ( ( animBuf = AllocVec ( bufsize, MEMF_ANY|MEMF_CLEAR ) ) == NULL )
            return;

        int f = FrameStart; for ( ; f <= FrameEnd; f++ )
        {
            canv->currentFrame = f - 1;
            setActiveBuffer ( canv );

            unsigned int *buf = NULL;
            if ( ( buf = generateExportableBuffer ( canv, AnimMode, datatype ) ) == 0 )
            {
                // not enough memory...
                FreeVec ( animBuf );
                return;
            }

            // Copy pixels from buf to animbuf
            int i = 0; for ( ; i < framelength; i++ )
            {
                animBuf[ offset + i ] = buf[ i ];
            }
            FreeVec ( buf );

            offset += framelength;
        }

        // Export with the JPEG datatype
        if ( datatype == 0 )
        {
            exportDT (
                canv->width,
                canv->height * framecount, animBuf, filename, "jpeg"
            );
        }
        // Export with png export code
        else if ( datatype == 1 )
        {
            exportPNG ( canv->width, canv->height * framecount, animBuf, filename );
        }
        // Save in RAW (64-bit) format
        else if ( datatype == 2 )
        {
            // NOT YET IMPLEMENTED!
            printf ( "64-bit animation export is not yet implemented!\n" );
        }
        // Save another in raw format
        else if ( datatype == 3 )
        {
            BPTR myfile;
            if ( ( myfile = Open ( filename, MODE_NEWFILE ) ) != NULL )
            {
                Write ( myfile, animBuf, bufsize );
                Close ( myfile );
            }
            else printf ( "Failed to save image raw..\n" );
        }

        // Free memory
        FreeVec ( animBuf );

        // Reset the currentframe to where it was
        canv->currentFrame = tmpFrame;
        setActiveBuffer ( canv );
    }
    // One image pr frame
    else
    {
        // Make room for buffer
        int framesize = canv->width * canv->height * 4;
        int tmpFrame = canv->currentFrame;

        unsigned int *buf = NULL;

        int f = FrameStart;

        for ( ; f <= FrameEnd; f++ )
        {
            char *tmpFilename = AllocVec ( 255, MEMF_ANY|MEMF_CLEAR );
            sprintf ( tmpFilename, "%s%04d", filename, ( f - FrameStart + 1 ) );

            canv->currentFrame = f - 1;
            setActiveBuffer ( canv );

            buf = generateExportableBuffer ( canv, AnimMode, datatype );

            // Export with the JPEG datatype
            if ( datatype == 0 )
            {
                exportDT ( canv->width, canv->height, buf, tmpFilename, "jpeg" );
            }
            else if ( datatype == 1 )
            {
                exportPNG ( canv->width, canv->height, buf, tmpFilename );
            }
            // Save in RAW (64-bit) format
            else if ( datatype == 2 )
            {
                // NOT YET IMPLEMENTED!
                printf ( "64-bit animation export is not yet implemented!\n" );
            }
            // Save another in raw format
            else if ( datatype == 3 )
            {
                BPTR myfile;
                if ( ( myfile = Open ( tmpFilename, MODE_NEWFILE ) ) != NULL )
                {
                    Write ( myfile, buf, framesize );
                    Close ( myfile );
                }
                else printf ( "Failed to save image raw..\n" );
            }

            FreeVec ( tmpFilename );
            FreeVec ( buf );
        }

        // Reset the currentframe to where it was
        canv->currentFrame = tmpFrame;
        setActiveBuffer ( canv );
    }
}

void exportDT ( int w, int h, unsigned int *buffer, unsigned char *filename, const char *format )
{
    if ( filename != NULL )
    {
        // Generate a datatype object
        Object *DTImage = NewDTObject (
            ( APTR )NULL,
            DTA_SourceType, DTST_RAM,
            DTA_BaseName, ( IPTR )format,
            PDTA_DestMode, PMODE_V43,
            TAG_DONE
        );
        // Return if this doesn't work
        if ( !DTImage )
            return;

        // Make a header
        struct BitMapHeader *bmhdr;
        if ( !( GetDTAttrs ( DTImage, PDTA_BitMapHeader, ( IPTR )&bmhdr, TAG_DONE ) ) )
            return;

        // Write the data in memory
        struct pdtBlitPixelArray dtObj;

        dtObj.MethodID = PDTM_WRITEPIXELARRAY;
        dtObj.pbpa_PixelFormat = PBPAFMT_ARGB;
        dtObj.pbpa_PixelArrayMod = w;
        dtObj.pbpa_Left = 0;
        dtObj.pbpa_Width = w;
        dtObj.pbpa_Height = 1;

        bmhdr->bmh_Width = w;
        bmhdr->bmh_Height = h;
        bmhdr->bmh_Depth = 24;
        bmhdr->bmh_PageWidth = 320;
        bmhdr->bmh_PageHeight = 240;

        int y = 0; for ( ; y < h; y++ )
        {
            dtObj.pbpa_Top = y;
            dtObj.pbpa_PixelData = ( APTR )( buffer + ( y * w ) );
            DoMethodA ( DTImage, ( Msg )&dtObj );
        }

        // Write the data to disk
        BPTR filehandle;
        if ( ( filehandle = Open ( filename, MODE_NEWFILE ) ) )
        {
            struct dtWrite dtwObj;
            dtwObj.MethodID = DTM_WRITE;
            dtwObj.dtw_GInfo = NULL;
            dtwObj.dtw_FileHandle = filehandle;
            dtwObj.dtw_Mode = DTWM_RAW;
            dtwObj.dtw_AttrList = NULL;
            DoMethodA ( DTImage, ( Msg )&dtwObj );
            Close ( filehandle );
        }

        // Clean up
        DisposeDTObject ( DTImage );
    }
}

static void png_user_write( png_structp png_ptr, png_bytep data, png_size_t length )
{
    Write ( png_ptr->io_ptr, data, length );
}
static void png_user_flush( png_structp png_ptr )
{
    // do nothing - nothing to flush!
}
void exportPNG ( int w, int h, unsigned int *buffer, unsigned char *filename )
{
    png_structp png_ptr;
    png_infop   info_ptr;
    BPTR    		file;
    int         i;

    // Create a write struct.
    if ( !(png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ) ) )
        return;

    // Create an info struct.
    if ( !( info_ptr = png_create_info_struct( png_ptr ) ) )
    {
        // Free the unused write struct.
        png_destroy_write_struct ( &png_ptr, ( png_infopp )NULL );
        return;
    }

    // Open file for writing
    if ( ( file = Open ( filename, MODE_NEWFILE ) ) == NULL )
        return;

    // Register the write function.
    png_set_write_fn ( png_ptr, file, png_user_write, png_user_flush );

    // Create the info header.
    png_set_IHDR (
        png_ptr, info_ptr, w, h, 8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    // Write the info header.
    png_write_info ( png_ptr, info_ptr );

    // Write image rows
    for ( i = 0; i < h ; i++ )
        png_write_row( png_ptr, ( png_bytep )&buffer[ i * w ] );

    // Write file end and clean up
    png_write_end( png_ptr, info_ptr );
    png_destroy_write_struct( &png_ptr, &info_ptr );

    // Close file and done!
    Close ( file );
}




