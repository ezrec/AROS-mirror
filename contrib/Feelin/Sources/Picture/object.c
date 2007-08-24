#include "Private.h"

//#define DB_OPEN
//#define DB_CLOSE

/*** Private ***************************************************************/

///png_sig_cmp
int32 png_sig_cmp(uint8 *sig, uint32 start, uint32 num_to_check)
{
    uint8 png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};

    if (num_to_check > 8)
    {
        num_to_check = 8;
    }
    else if (num_to_check < 1)
    {
        return (0);
    }

    if (start > 7)
    {
        return (0);
    }

    if (start + num_to_check > 8)
    {
        num_to_check = 8 - start;
    }

    return F_StrCmp(&sig[start], &png_signature[start], num_to_check);
}
//+

///picture_create_with_core_png
int32 picture_create_with_core_png(STRPTR Name, struct FeelinSharedPixels *spix)
{
    int32 rc = FALSE;
 
    FObject png = CorePNGObject,

        "Source", Name,
        "SourceType", FV_CoreMedia_SourceType_File,

        End;

    if (png)
    {
        FCorePicturePublic *pub = (FCorePicturePublic *) F_Get(png, (uint32) "Public");

        if (pub)
        {
            CopyMem(pub, &spix->core_picture_public, sizeof (FCorePicturePublic));

            /* steal arrays */

            pub->PixelArray = NULL;
            pub->ColorArray = NULL;

            rc = TRUE;
        }

        F_DisposeObj(png);
    }
    else
    {
        F_Log(0,"Unable to decore picture (%s) with CorePNG",Name);
    }
    
    return rc;
}
//+
///picture_create_with_datatypes
int32 picture_create_with_datatypes(STRPTR Name, struct FeelinSharedPixels *spix)
{
    int32 rc = FALSE;
                
    APTR dt = NewDTObject(Name,

        DTA_SourceType, DTST_FILE,
        DTA_GroupID, GID_PICTURE,
        PDTA_Remap, FALSE,
        PDTA_DestMode, PMODE_V43,

        TAG_DONE);

    if (dt)
    {
        struct BitMapHeader *header=NULL;
        struct ColorRegister *color_array=NULL;
        uint32 color_count=NULL;

        GetDTAttrs(dt,

            PDTA_BitMapHeader, &header,
            PDTA_ColorRegisters, &color_array,
            PDTA_NumColors, &color_count,

            TAG_DONE);

        if (header)
        {
            uint32 mode = PBPAFMT_RGB;
            
            spix->core_picture_public.Width = header->bmh_Width;
            spix->core_picture_public.Height = header->bmh_Height;
            spix->core_picture_public.Depth = header->bmh_Depth;
            
            if (color_array && color_count && header->bmh_Depth < 9)
            {
                mode = PBPAFMT_LUT8;
 
//                F_Log(0,"creating FV_COLOR_TYPE_PALETTE (0x%08lx)(%ld)", color_array, color_count);
            
                spix->core_picture_public.PixelSize = sizeof (uint8);
                spix->core_picture_public.PixelArrayMod = header->bmh_Width * spix->core_picture_public.PixelSize;
                
                spix->core_picture_public.ColorType = FV_COLOR_TYPE_PALETTE;
                spix->core_picture_public.ColorArray = F_New(color_count * sizeof (uint32));
                spix->core_picture_public.ColorCount = color_count;
 
                if (spix->core_picture_public.ColorArray)
                {
                    uint32 *target = spix->core_picture_public.ColorArray;
                    uint32 transp_index = (header->bmh_Masking == mskHasTransparentColor) ? header->bmh_Transparent : 0xFFFFFFFF;
                    uint32 i;
                    
//                    F_Log(0,"transp_index %ld",transp_index);
                    
                    if (transp_index != 0xFFFFFFFF)
                    {
                        spix->core_picture_public.ColorType |= FF_COLOR_TYPE_ALPHA;
                    }
                    
                    for (i = 0 ; i < color_count ; i++, color_array++)
                    {
                        uint32 rgba;
                        
                        rgba = color_array->red;
                        rgba <<= 8;
                        
                        rgba |= color_array->green;
                        rgba <<= 8;
                        
                        rgba |= color_array->blue;
                        rgba <<= 8;
                        
                        if (i == transp_index)
                        {
                            rgba |= 0x00;
                        }
                        else
                        {
                            rgba |= 0xFF;
                        }
                
                        //F_Log(0,"col(%03ld) 0x%08lx",i, rgba);
 
                        *target++ = rgba;
                    }
                
                    spix->core_picture_public.PixelArray = F_New(spix->core_picture_public.PixelArrayMod * header->bmh_Height);
                }
            }
            else
            {
                spix->core_picture_public.PixelSize = sizeof (uint8) * 3;
                spix->core_picture_public.PixelArrayMod = spix->core_picture_public.PixelSize * header->bmh_Width;
                spix->core_picture_public.PixelArray = F_New(spix->core_picture_public.PixelArrayMod * header->bmh_Height);
                spix->core_picture_public.ColorType = FV_COLOR_TYPE_RGB;
            }

            if (spix->core_picture_public.PixelArray)
            {
                DoMethod(dt, PDTM_READPIXELARRAY,
                    spix->core_picture_public.PixelArray,
                    
                    mode,
                    
                    spix->core_picture_public.PixelArrayMod,
                    0,0, spix->core_picture_public.Width, spix->core_picture_public.Height);
                
                #if 0
                {
                    uint32 *mem = spix->core_picture_public.PixelArray;
                
                    F_Log(0,"READ (%ld) 0x%08lx%08lx%08lx%08lx%08lx%08lx%08lx%08lx%08lx%08lx",
                        mem[0],mem[1],mem[2],mem[3],mem[4],mem[5],mem[6],
                        mem[7],mem[8],mem[9]);
                }
                #endif
                
                
                rc = TRUE;
            }
            else
            {
                F_Log(FV_LOG_USER,"Unable to allocate Shared Pixels (%s)",Name);
            }
        }

        DisposeDTObject(dt);
    }
    else
    {
        F_Log(FV_LOG_USER,"Unable to open \"%s\" through DataTypes",Name);
    }
    
    return rc;
}
//+
 
///picture_open
struct FeelinSharedPixels * picture_open(STRPTR Name)
{
    struct FeelinSharedPixels *spix = NULL;
    BPTR lock;

///DB_OPEN
#ifdef DB_OPEN
   F_Log(0,"picture_open >> BEGIN >> Name (%s)",Name);
#endif
//+

    F_Do(CUD->arbitrer, FM_Lock, FF_Lock_Exclusive);

/* I don't use  picture's  name  to  recognize  a  picture  because  string
comparaison would require absolute path, instead I lock the file defined by
'Name' than search for a match. */

    lock = Lock(Name, ACCESS_READ);
    
//    F_Log(0,"name (%s) lock (0x%08lx)",Name,lock);
 
    if (lock)
    {
        STRPTR buf = F_New(1024);
        
        if (buf)
        {
            NameFromLock(lock, buf, 1024);
     
///DB_OPEN
#ifdef DB_OPEN
      F_Log(0,"lock (0x%08lx) path (%s)",lock,buf);
#endif
//+

            for (spix = CUD->shared_pixels ; spix ; spix = spix->next)
            {
                if (F_StrCmp(buf, spix->path, ALL) == 0)
                {
                    spix->count++; break;
                }
            }
        
            if (!spix)
            {
                int32 use_core_png = FALSE;
                uint32 success=FALSE;
                
                BPTR file = Open(Name, MODE_OLDFILE);

                if (file)
                {
                    uint8 signature[8];
                    
                    Read(file, signature, 8);
                    
                    if (png_sig_cmp(signature, 0, 8) == 0)
                    {
                        use_core_png = TRUE;
                    }
                
                    Close(file);
                }
            
                spix = F_New(sizeof (struct FeelinSharedPixels));
                
                if (spix)
                {
                    if (use_core_png)
                    {
                        success = picture_create_with_core_png(Name, spix);
                    }
                    else
                    {
                        success = picture_create_with_datatypes(Name, spix);
                    }
                }
                
                #if 0
                #endif
            

                if (success)
                {
                    spix->count = 1;
            
                    CopyMem(buf, &spix->path,1024);

                    spix->next = CUD->shared_pixels;
                    CUD->shared_pixels = spix;
                    
                    #if 0
                    {
                        STRPTR pixel_type;
                        
                        switch (spix->core_picture_public.PixelType)
                        {
                            case FV_PIXEL_TYPE_GREY:    pixel_type = "GREY"; break;
                            case FV_PIXEL_TYPE_PALETTE: pixel_type = "PALETTE"; break;
                            case FV_PIXEL_TYPE_RGB:     pixel_type = "RGB"; break;
                            case FV_PIXEL_TYPE_RGBA:    pixel_type = "RGBA"; break;
                            default:                    pixel_type = "UNKNOWN (please report)"; break;
                        }
                    
                    F_Log(0,"PixelType (%s) PixelArray (0x%08lx)(%ld) ColorType (%ld) ColorArray (0x%08lx)(%ld)",
                        pixel_type,
                        spix->core_picture_public.PixelArray,
                        spix->core_picture_public.PixelArrayMod,
                        spix->core_picture_public.ColorType,
                        spix->core_picture_public.ColorArray,
                        spix->core_picture_public.ColorCount);
                    }
                    #endif
                }
                else
                {
                    F_Dispose(spix); spix = NULL;
                }
            
                F_Dispose(buf);
            }

///DB_OPEN
#ifdef DB_OPEN
      F_Log(0,"Shared Pixels (0x%08lx) - %ld Users",spix,spix -> count);
#endif
//+
        
        }

        UnLock(lock);
    }
    else
    {
        F_Log(0,"\"%s\" file not found",Name);
    }
    
    F_Do(CUD->arbitrer,FM_Unlock);

///DB_OPEN
#ifdef DB_OPEN
   F_Log(0,"picture_open >> DONE >> SPix (0x%08lx)",spix);
#endif
//+
    
    return spix;
}
//+
///picture_close
void picture_close(struct FeelinSharedPixels *SPix)
{
    if (SPix)
    {
        struct FeelinSharedPixels *node;
        struct FeelinSharedPixels *prev = NULL;

///DB_CLOSE
#ifdef DB_CLOSE
      F_Log(0,"picture_close >> BEGIN >> SPix (0x%08lx %ld) Path (%s)",SPix,SPix -> count,SPix->path);
#endif
//+
        
        F_Do(CUD->arbitrer,FM_Lock,FF_Lock_Exclusive);

        for (node = CUD->shared_pixels ; node ; node = node->next)
        {
            if (SPix == node)
            {
                if ((--SPix->count) == 0)
                {
                    if (prev)
                    {
                        prev->next = SPix->next;
                    }
                    else
                    {
                        CUD->shared_pixels = SPix->next;
                    }

                    #ifdef DB_CLOSE
                    F_Log(0,"PixelArray (0x%08lx) ColorArray (0x%08lx) SPix (0x%08lx)",
                        SPix->core_picture_public.PixelArray,
                        SPix->core_picture_public.ColorArray,
                        SPix);
                    #endif
                    
                    F_Dispose(SPix->core_picture_public.PixelArray);
                    F_Dispose(SPix->core_picture_public.ColorArray);
                    F_Dispose(SPix);
                }
                break;
            }
            prev = node;
        }

        if (!node)
        {
            F_Log(FV_LOG_DEV,"Unknown Pixels 0x%lx",SPix);
        }

        F_Do(CUD->arbitrer,FM_Unlock);
        
///DB_CLOSE
#ifdef DB_CLOSE
      F_Log(0,"picture_close >> DONE");
#endif
//+
 
    }
}
//+

/*** Methods ***************************************************************/

///Picture_New
F_METHOD(uint32,Picture_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    STRPTR name=NULL;
 
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Picture_Name:
        {
            name = (STRPTR) item.ti_Data;
        }
        break;
    }

//    F_Log(0,"NAME (0x%08lx)(%s)",name,name);
 
    if (name)
    {
        LOD->shared_pixels = picture_open(name);
        
        if (LOD->shared_pixels)
        {
            return F_SuperDo(Class,Obj,Method,
        
                "FA_BitMap_Width",          LOD->shared_pixels->core_picture_public.Width,
                "FA_BitMap_Height",         LOD->shared_pixels->core_picture_public.Height,
                "FA_BitMap_Depth",          LOD->shared_pixels->core_picture_public.Depth,
                "FA_BitMap_PixelSize",      LOD->shared_pixels->core_picture_public.PixelSize,
                "FA_BitMap_PixelArray",     LOD->shared_pixels->core_picture_public.PixelArray,
                "FA_BitMap_PixelArrayMod",  LOD->shared_pixels->core_picture_public.PixelArrayMod,
                "FA_BitMap_ColorType",      LOD->shared_pixels->core_picture_public.ColorType,
                "FA_BitMap_ColorArray",     LOD->shared_pixels->core_picture_public.ColorArray,
                "FA_BitMap_ColorCount",     LOD->shared_pixels->core_picture_public.ColorCount,
                
                TAG_MORE, Msg);
        }
        return NULL;
    }

    return F_SUPERDO();
}
//+
///Picture_Dispose
F_METHOD(void,Picture_Dispose)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_SUPERDO();

    if (LOD -> shared_pixels)
    {
        picture_close(LOD -> shared_pixels); LOD -> shared_pixels = NULL;
    }
}
//+
