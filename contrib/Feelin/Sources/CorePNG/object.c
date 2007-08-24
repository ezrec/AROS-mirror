#include "Private.h"

#ifdef __AROS__
#include <stdlib.h>

void *malloc(size_t size)
{
    return AllocVec(size, MEMF_ANY);
}

void free(void *mem)
{
    FreeVec(mem);
}

void *calloc(size_t nmemb, size_t size)
{
    return AllocVec(nmemb * size, MEMF_ANY | MEMF_CLEAR);
}
#endif

///my_error_fn
SAVEDS static void my_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
    F_Log(0,"PNG error: %s", error_msg ? error_msg : "");
    /*
    if (png_ptr && png_jmpbuf(png_ptr))
    {
        longjmp(png_jmpbuf(png_ptr), 1);
    }
    */
}
//+
///my_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
SAVEDS static void my_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
    F_Log(0,"PNG warning: %s", warning_msg ? warning_msg : "");
}
//+
///my_read_fn
SAVEDS static void my_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    BPTR file = (BPTR) png_get_io_ptr(png_ptr);
    png_uint_32 count;

    count = Read(file, data, length);
    if (count != length)
    {
        png_error(png_ptr, "Read error!");
    }
}
//+

//#define DB_LOAD

#define HEADER_CHECK_SIZE 8

#if 0
///render_pixel_array_palette

#define F_PIXEL_RED(rgba)                       ((0xFF000000 & rgba) >> 24)
#define F_PIXEL_GREEN(rgba)                     ((0x00FF0000 & rgba) >> 16)
#define F_PIXEL_BLUE(rgba)                      ((0x0000FF00 & rgba) >>  8)
#define F_PIXEL_ALPHA(rgba)                      (0x000000FF & rgba)

static uint32 render_pixel_array_palette(uint8 *Source, uint32 x, uint32 y, uint32 modulo, struct RastPort *rp, uint32 target_x, uint32 target_y, uint32 target_w, uint32 target_h, uint32 *Colors)
{
    uint32 count=0;

//    if ((FF_Render_TrueColors & LOD->Flags) && (CyberGfxBase != NULL))
    {
        uint32 target_mod = target_w * sizeof (uint32);
        uint32 *line_buffer = F_New(target_w * sizeof (uint32));

        if (line_buffer != NULL)
        {
            uint32 target_x2 = target_x + target_w;
            uint32 target_y2 = target_y + target_h;

            for ( ; target_y < target_y2 ; target_y++, y++)
            {
                uint8 *src = (APTR)((uint32)(Source) + x * sizeof (uint8) + y * modulo);
                uint32 *buf = line_buffer;
                uint32 j;
                                            
                ReadPixelArray(line_buffer, 0,0,target_mod, rp, target_x,target_y, target_w,1, RECTFMT_RGBA);

                for (j = target_x ; j < target_x2 ; j++)
                {
                    uint32 rc;
                    uint32 s_rgba = *buf;
                    uint32 d_rgba = Colors[*src++];

                    int32 a = /*255 -*/ F_PIXEL_ALPHA(d_rgba);
                    int32 s,d;

                    s = F_PIXEL_RED(s_rgba);
                    d = F_PIXEL_RED(d_rgba);

                    rc = s + (((d - s) * a) >> 8);
                    rc <<= 8;

                    s = F_PIXEL_GREEN(s_rgba);
                    d = F_PIXEL_GREEN(d_rgba);

                    rc |= s + (((d - s) * a) >> 8);
                    rc <<= 8;

                    s = F_PIXEL_BLUE(s_rgba);
                    d = F_PIXEL_BLUE(d_rgba);

                    rc |= s + (((d - s) * a) >> 8);
                    rc <<= 8;

                    *buf++ = rc;
                }

                WritePixelArray(line_buffer, 0,0,target_mod, rp, target_x,target_y, target_w,1, RECTFMT_RGBA);

                count += target_w;
            }

            F_Dispose(line_buffer);
        }
    }
    return count;
}
//+
///render_pixel_array_rgba

#define F_PIXEL_RED(rgba)                       ((0xFF000000 & rgba) >> 24)
#define F_PIXEL_GREEN(rgba)                     ((0x00FF0000 & rgba) >> 16)
#define F_PIXEL_BLUE(rgba)                      ((0x0000FF00 & rgba) >>  8)
#define F_PIXEL_ALPHA(rgba)                      (0x000000FF & rgba)

static uint32 render_pixel_array_rgba(uint32 *Source, uint32 x, uint32 y, uint32 modulo, struct RastPort *rp, uint32 target_x, uint32 target_y, uint32 target_w, uint32 target_h)
{
    uint32 count=0;

//    if ((FF_Render_TrueColors & LOD->Flags) && (CyberGfxBase != NULL))
    {
        uint32 target_mod = target_w * sizeof (uint32);
        uint32 *line_buffer = F_New(target_mod);

        if (line_buffer != NULL)
        {
            uint32 target_x2 = target_x + target_w;
            uint32 target_y2 = target_y + target_h;

            for ( ; target_y < target_y2 ; target_y++, y++)
            {
                uint32 *src = (APTR)((uint32)(Source) + x * sizeof (uint32) + y * modulo);
                uint32 *buf = line_buffer;
                uint32 j;

                ReadPixelArray(line_buffer, 0,0,target_mod, rp, target_x,target_y, target_w,1, RECTFMT_RGBA);

                for (j = target_x ; j < target_x2 ; j++)
                {
                    uint32 rc;
                    uint32 s_rgba = *buf;
                    uint32 d_rgba = *src++;

                    int32 a = /*255 -*/ F_PIXEL_ALPHA(d_rgba);
                    int32 s,d;

                    s = F_PIXEL_RED(s_rgba);
                    d = F_PIXEL_RED(d_rgba);

                    rc = s + (((d - s) * a) >> 8);
                    rc <<= 8;

                    s = F_PIXEL_GREEN(s_rgba);
                    d = F_PIXEL_GREEN(d_rgba);

                    rc |= s + (((d - s) * a) >> 8);
                    rc <<= 8;

                    s = F_PIXEL_BLUE(s_rgba);
                    d = F_PIXEL_BLUE(d_rgba);

                    rc |= s + (((d - s) * a) >> 8);
                    rc <<= 8;

                    *buf++ = rc;
                }

                WritePixelArray(line_buffer, 0,0,target_mod, rp, target_x,target_y, target_w,1, RECTFMT_RGBA);

                count += target_w;
            }

            F_Dispose(line_buffer);
        }
    }
    return count;
}
//+
#endif

///core_png_load
static int32 core_png_load(FClass *Class, FObject Obj,STRPTR Name)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    FCorePicturePublic *pub = &LOD->Public;
    
    uint8 fileheader[HEADER_CHECK_SIZE];
    
    png_structp png_ptr=NULL;
    png_infop png_info_ptr=NULL;
    png_infop png_end_info_ptr=NULL;
    png_uint_32 png_width=0;
    png_uint_32 png_height=0;
    int png_bits=0;
    int png_type=0;
    int png_lace=0;
    int png_num_lace_passes;

    LOD->handle = Open(Name, MODE_OLDFILE);

    if (LOD->handle == NULL)
    {
        return FALSE;
    }

    if (Read(LOD->handle, fileheader, sizeof(fileheader)) != sizeof(fileheader))
    {
        return FALSE;
    }

    if (png_sig_cmp(fileheader, 0, sizeof(fileheader)) != 0)
    {
        F_Log(0,"\"%s\" is not a PiNG file",Name);

        return FALSE;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,

        0,               /* error ptr */
        my_error_fn,     /* error function */
        my_warning_fn    /* warning function */

        );

    if (!png_ptr)
    {
        F_Log(0,"Can't create png read struct!");

        return NULL;
    }

    png_info_ptr = png_create_info_struct(png_ptr);

    if (!png_info_ptr)
    {
        F_Log(0,"Can't create png info struct!");
        png_destroy_read_struct(&png_ptr, NULL, NULL);

        return NULL;
    }

    png_end_info_ptr = png_create_info_struct(png_ptr);

    if (!png_end_info_ptr)
    {
        F_Log(0,"Can't create png end info struct!");

        png_destroy_read_struct(&png_ptr, &png_info_ptr, NULL);

        return NULL;
    }

    png_set_read_fn(png_ptr, (png_voidp) LOD->handle, my_read_fn);

    png_set_sig_bytes(png_ptr, HEADER_CHECK_SIZE);
/*
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        F_Log(0,"Error!"));
        png_destroy_read_struct(&png_ptr, &png_info_ptr, &png_end_info_ptr);
        if (buffer) FreeVec(buffer);
        PNG_Exit(&png, ERROR_UNKNOWN);
        return FALSE;
    }
*/
    png_read_info(png_ptr, png_info_ptr);

    png_get_IHDR(png_ptr, png_info_ptr,
        &png_width, &png_height, &png_bits, &png_type, &png_lace, NULL, NULL);
    
/*
    F_Log(0,"PNG IHDR: Size %ld x %ld  Bits %d  Type %d Lace %d",
        png_width,
        png_height,
        png_bits,
        png_type,
        png_lace);
*/
    if (png_bits == 16)
    {
//        F_Log(0,"png_set_strip_16");

        png_set_strip_16(png_ptr);
    }
    else if (png_bits < 8)
    {
//        F_Log(0,"png_set_packing");

        png_set_packing(png_ptr);

        if (png_type == PNG_COLOR_TYPE_GRAY)
        {
//            F_Log(0,"png_set_gray_1_2_4_to_8");

            png_set_gray_1_2_4_to_8(png_ptr);
        }
    }

    pub->Width = png_width;
    pub->Height = png_height;
    pub->Depth = png_bits;

    #ifdef DB_LOAD
    F_Log(0,"width %ld, height %ld, bits %ld, rowbytes %ld, num_palette %ld, num_trans %ld, color_type %ld, channels %ld",
        png_info_ptr->width, png_info_ptr->height, png_bits, png_info_ptr->rowbytes,
        png_info_ptr->num_palette, png_info_ptr->num_trans, png_info_ptr->color_type,
        png_info_ptr->channels);
    #endif
 
    #if 0
    if (png_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        F_Log(0,"png_set_strip_alpha");

        png_set_strip_alpha(png_ptr);
    }
    #endif

 
    #if 0
    {
        double png_file_gamma;
        double png_screen_gamma = 2.2;

        if (!(png_get_gAMA(png_ptr, png_info_ptr, &png_file_gamma)))
        {
            png_file_gamma = 0.45455;
        }

        png_set_gamma(png_ptr, png_file_gamma, png_screen_gamma);
    }
    #endif

    png_num_lace_passes = png_set_interlace_handling(png_ptr);

    switch(png_type)
    {
        case PNG_COLOR_TYPE_GRAY:
        {
            pub->PixelSize = sizeof (uint8);
            pub->ColorType = FV_COLOR_TYPE_GREY;
            
            #ifdef DB_LOAD
            F_Log(0,"FV_COLOR_TYPE_GREY");
            #endif
        }
        break;
         
        case PNG_COLOR_TYPE_GRAY_ALPHA:
        {
            pub->PixelSize = sizeof (uint8) * 2;
            pub->ColorType = FV_COLOR_TYPE_GREY_ALPHA;

            #ifdef DB_LOAD
            F_Log(0,"FV_COLOR_TYPE_GREY_ALPHA");
            #endif
        }
        break;

        case PNG_COLOR_TYPE_PALETTE:
        {
            pub->PixelSize = sizeof (uint8);
            pub->ColorType = FV_COLOR_TYPE_PALETTE;

            #ifdef DB_LOAD
            F_Log(0,"FV_COLOR_TYPE_PALETTE");
            #endif
        }
        break;
    
        case PNG_COLOR_TYPE_RGB:
        {
            pub->PixelSize = sizeof (uint8) * 3;
            pub->ColorType = FV_COLOR_TYPE_RGB;

            #ifdef DB_LOAD
            F_Log(0,"FV_COLOR_TYPE_RGB");
            #endif
        }
        break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
        {
            pub->PixelSize = sizeof (uint8) * 4;
            pub->ColorType = FV_COLOR_TYPE_RGB_ALPHA;

            #ifdef DB_LOAD
            F_Log(0,"FV_COLOR_TYPE_RGB_ALPHA");
            #endif
        }
        break;

        default:
        {
            F_Log(0, "Unknown PNG Color Type! (%ld)", png_type);

            goto __end;
        }
        break;
    }

    png_read_update_info(png_ptr, png_info_ptr);

    /* Palette? */

    #if 1

    if ((png_type == PNG_COLOR_TYPE_PALETTE) ||
        (png_type == PNG_COLOR_TYPE_GRAY) ||
        (png_type == PNG_COLOR_TYPE_GRAY_ALPHA))
    {
        png_colorp col = 0;
        int numcolors = 1L << pub->Depth;

        if (png_type == PNG_COLOR_TYPE_PALETTE)
        {
            if (!png_get_PLTE(png_ptr, png_info_ptr, &col, &numcolors))
            {
                F_Log(0, "PLTE chunk missing!"); numcolors = 0;
            }
        }

        if ((pub->ColorCount = numcolors) != 0)
        {
            pub->ColorArray = F_New(sizeof (uint32) * pub->ColorCount);
        }

        /* Gray images should in theory not require the following code,  as
        picture.datatype   should  handle  it  automatically  when  we  use
        PBPAFMT_GREY8. But just to be sure ... */

        if (pub->ColorArray)
        {
            uint32 i;
            uint32 rgba;
                    
            if (png_type == PNG_COLOR_TYPE_PALETTE)
            {
                for (i = 0 ; i < pub->ColorCount ; i++)
                {
                    rgba = col->red;
                    rgba <<= 8;

                    rgba |= col->green;
                    rgba <<= 8;

                    rgba |= col->blue;
                    rgba <<= 8;

                    rgba |= 0xFF;

                    pub->ColorArray[i] = rgba;

                    //F_Log(0,"COL (0x%08lx) : RGBA (0x%08lx)",col,rgba);

                    col++;
                }
            }
            else
            {
                for (i = 0 ; i < pub->ColorCount ; i++)
                {
                    rgba = i;
                    rgba <<= 8;

                    rgba |= i;
                    rgba <<= 8;

                    rgba |= i;
                    rgba <<= 8;

                    rgba |= 0xFF;

                    pub->ColorArray[i] = rgba;
                }
            }
        }
    }
    #endif
    
    #if 1 /* Palette transparency */

    if ((png_type == PNG_COLOR_TYPE_PALETTE) && (pub->ColorArray != NULL))
    {
        png_bytep trans;
        int num_trans;
    
        pub->ColorType |= FF_COLOR_TYPE_ALPHA;
 
        if (png_get_tRNS(png_ptr, png_info_ptr, &trans, &num_trans, NULL))
        {
            uint32 i;
            
            num_trans = MIN(num_trans, pub->ColorCount);
            
            for (i = 0 ; i < num_trans ; i++, trans++)
            {
                uint32 argb = pub->ColorArray[i];

                argb >>= 8;
                argb <<= 8;
                argb |= *trans;
                
                pub->ColorArray[i] = argb;
            }
        }
    }
    #endif

    pub->PixelArrayMod = png_get_rowbytes(png_ptr, png_info_ptr);
    pub->PixelArray = F_New(pub->Height * pub->PixelArrayMod);
 
    if (pub->PixelArray)
    {
        APTR buf;
        uint32 y;
        
        while (png_num_lace_passes--)
        {
            //F_Log(0,"PASS %ld",png_num_lace_passes);

            for (y = 0, buf = pub->PixelArray; y < pub->Height; y++, buf = (APTR)((uint32)(buf) + pub->PixelArrayMod))
            {
                //F_Log(0,"read row %ld - modulo (%ld)(0x%08lx)",y,modulo, buf);

                png_read_row(png_ptr, buf, NULL);
            }
        }
            
        #ifdef DB_LOAD
        F_Log(0,"PixelsArray (0x%08lx) PixelArrayMod (0x%08lx) PixelSize (%ld)",
            pub->PixelArray, pub->PixelArrayMod, pub->PixelSize);
        #endif
         
/*
        WritePixelArray(
            pub->PixelArray, 0,0, pub->PixelArrayMod, FeelinBase->Intuition->ActiveWindow->RPort,
            10,10, 100, 100, pub->PixelType);
*/
/*
        switch (pub->PixelType)
        {
            case FV_PIXEL_TYPE_PALETTE:
            {
                render_pixel_array_palette(
                    pub->PixelArray, 0,0, pub->PixelArrayMod, FeelinBase->Intuition->ActiveWindow->RPort,
                    0,0, pub->Width,pub->Height, pub->ColorArray);
            }
            break;
            
            case FV_PIXEL_TYPE_RGBA:
            {
                render_pixel_array_rgba(
                    pub->PixelArray, 0,0, pub->PixelArrayMod, FeelinBase->Intuition->ActiveWindow->RPort,
                    10,10, pub->Width, pub->Height);
            }
            break;
        }
*/
    }

__end:
 
    png_read_end(png_ptr, png_end_info_ptr);
    png_destroy_read_struct(&png_ptr, &png_info_ptr, &png_end_info_ptr);
    
    Close(LOD->handle); LOD->handle = NULL;
        
    return (pub->PixelArray) ? TRUE : FALSE;
}
//+

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///CorePNG_New
F_METHOD(uint32, CorePNG_New)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags=Msg, item;
    
    APTR source = NULL;
    uint32 source_type = 0;
//    int32 source_owner = FALSE;
    
    while  (F_DynamicNTI(&Tags, &item, Class))
    switch (item.ti_Tag)
    {
        case FA_CoreMedia_Source: source = (APTR) item.ti_Data; break;
        case FA_CoreMedia_SourceType: source_type = item.ti_Data; break;
    }
    #if 0
    if (source != NULL && source_type == FV_CoreMedia_SourceType_File)
    {
        source = (APTR) Open((STRPTR) source, MODE_OLDFILE);
        source_type = FV_CoreMedia_SourceType_Handle;
        source_owner = TRUE;
    }

    if (source != NULL && source_type == FV_CoreMedia_SourceType_Handle)
    {
        core_png_load(Class, Obj, (BPTR) source);
    }
    
    if (source_owner)
    {
        Close((BPTR) source);
    }
    #endif
    
    if ((source != NULL) && (source_type == FV_CoreMedia_SourceType_File))
    {
        core_png_load(Class, Obj, (STRPTR) source);
    }
    else
    {
        F_Log(0,"Invalid source (0x%08lx) or Type (%ld)",source, source_type);
    }

    if (LOD->Public.PixelArray)
    {
        return F_SUPERDO();
    }
    return NULL;
}
//+
///CorePNG_Dispose
F_METHOD(void, CorePNG_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    
    F_Dispose(LOD->Public.PixelArray); LOD->Public.PixelArray = NULL;
    F_Dispose(LOD->Public.ColorArray); LOD->Public.ColorArray = NULL;

    if (LOD->handle)
    {
        Close(LOD->handle); LOD->handle = NULL;
    }

    F_SUPERDO();
}
//+
///CorePNG_Get
F_METHOD(void, CorePNG_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class, Obj);
    struct TagItem *Tags = Msg, item;
    
    while  (F_DynamicNTI(&Tags, &item, Class))
    switch (item.ti_Tag)
    {
        case FA_CoreMedia_Public:
        {
            F_STORE(&LOD-> Public);
        }
        break;
    }

    F_SUPERDO();
}
//+
