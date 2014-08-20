/*

	File reading/writing functions. (done)
	Image loading (via datatypes) (done).
	Chunky Image loading.		  (done)
	Texture loading (automatical scaling).
 
	Implemented better error handling.
		If picture is not found it is replaced by blank image and black palette.
		If any function fails to allocate memory then program exits automaticaly.
 
	v1.1
		Forget to free temporary ARGB buffer in 16bit loading.
	v1.2
		Improved error handling a bit.
		Texture filename is preserved (for texture caching?)
	v1.3
		Added some error handling into JPEG loading code
		Added some speed optimizations (50% in DT loader!)
		Fixed some bugs after optimizations.
	v1.4
		Removed some obsolete functions (8bit support mainly)
 
	v2.0
		Cleaned up the code. Fixed 32bit DT loader and some other loading functions.
		Also some textures haven't got the name assigned.
	v2.1
		Added PNG support.
		Moved some stuff into other files.
 
*/


#include <stdlib.h>
#include <string.h>

#include <dos/dos.h>
#include <exec/memory.h>

#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#define USE_INLINE_STDARG
#include <proto/multimedia.h>
#include <classes/multimedia/video.h>
#undef USE_INLINE_STDARG

#include <utility/tagitem.h>
#define SYSTEM_PRIVATE
#include <intuition/extensions.h>

#include <datatypes/pictureclass.h>
#include <intuition/gadgetclass.h>

#include <proto/datatypes.h>
#include <proto/dtclass.h>
#include <proto/muimaster.h>

#include <cybergraphx/cybergraphics.h>

#include <emul/emulregs.h>
#include <emul/emulinterface.h>

#include "chunky.h"
#include "file.h"
#include "data.h"
#include "functions.h"
#include "timer.h"
#include "loadsunrast.h"
#include "loadpng.h"
#include "memory.h"
#include "string.h"
#include "util.h"

static int debugmode = 0;

static char	*lastFileType = NULL;
static void	setLastFileType(char *type);

struct Library *DataTypesBase;

void dprintf(char *fmt, ...);

void SetDebugMode(int mode)
{
    debugmode = mode;
}

//#define ENABLE_16BIT

/// loadfile()

void *fileLoad(char *name)
{
	return dataLoadFile(name);
}

void fileFree(void *file)
{
	free(file);
}

////

/// fileGetFilePart()

char *fileGetFilePart(char *fname)
{
	if (fname != NULL)
		return FilePart(fname);
	else
		return NULL;
}

////

/// savefile()

int savefile(char *name, void *mem, int len)
{
	FILE *file;

	file = fopen(name, "w");
    if (file == NULL)
    {
		printf("Could not open file for writing:%s!\n", name);
		return FALSE;
    }

	fwrite(mem, len, 1, file);
    fclose(file);
	return TRUE;
}

////

/// filelen()

unsigned int fileLength(char *fname)
{
	unsigned int l = dataFileLength(fname);
	return l;
}
////

/// AllocTexture

Texture	*AllocTextureStruct(void)
{
	return (Texture*)calloc(1, sizeof(Texture));
}

////

/// FreeTexture()

void FreeTexture(Texture *txt)
{
	if (txt != NULL)
    {
		chkFree(txt->image);
		txt->image = NULL;
		free(txt->name);
		free(txt->alphaname);
		free(txt->type);
        free(txt);
    }
}

////

/// txt_CreateBlank8()

Texture	*txt_CreateBlank(int width, int height)
{
	return txtCreateBlank(width, height, CHK_PIXFMT_LUT);
}

////

/// txt_CreateBlank16()
#ifdef ENABLE_16BIT
Texture	*txt_CreateBlank16(int width, int height)
{
	return txtCreateBlank(width, height, CHK_PIXFMT_RGB565);
}
#endif
////

/// txt_CreateBlank24()

Texture	*txt_CreateBlank24(int width, int height)
{
	return txtCreateBlank(width, height, CHK_PIXFMT_RGB888);
}

////

/// txt_CreateBlank32()

Texture	*txt_CreateBlank32(int width, int height)
{
	return txtCreateBlank(width, height, CHK_PIXFMT_ARGB8888);
}
////

/// txtCreate(width, height, format)

Texture	*txtCreate(int width, int height, int pixfmt)
{
	Texture	*txt = AllocTextureStruct();

	if (txt != NULL)
	{
		txt->image = chkCreate(width, height, pixfmt, FALSE);

		if (txt->image != NULL)
		{
			/* make sure that pixel format is set correctly */

			txt->image->pixfmt = pixfmt;
		}
		else
		{
			txtFree(txt);
			txt = NULL;
		}
    }

	return txt;
}

////

/// txtCreateFromImage(ChkImage)

Texture	*txtCreateFromImage(ChkImage *image)
{
	Texture	*txt = AllocTextureStruct();

	if (txt != NULL)
		txt->image = image;

	return txt;
}

////

/// txtCreateBlank(width, height, format)

Texture	*txtCreateBlank(int width, int height, int pixfmt)
{
	Texture	*txt = txtCreate(width, height, pixfmt);

	if (txt)
	{
		memset(txt->image->data.p, 0, width * height * fmtGetBytesPerPixel(pixfmt));
    }

	return txt;
}

////

/// static LoadDTPicture()

static Texture *LoadDTPicture(char *fname, int chkpixfmt)
{
	Object	*dt_object;
    unsigned int *cregs, numcols;
	struct BitMapHeader *bmhd;
	struct BitMap *bitmap;
	struct pdtBlitPixelArray pdtmsg;
	Texture	*txt = NULL;
	int	alpha;

	//__timerGlobalStart();

	if (chkpixfmt !=CHK_PIXFMT_ARGB8888 && chkpixfmt != CHK_PIXFMT_RGB888)
		return NULL;

	if (fname != NULL)
	{
		//printf("newdtobject:%s\n", fname);
		//printf("%d\n", AvailMem(MEMF_ANY));
		dt_object = NewDTObject (fname,
								  DTA_GroupID, GID_PICTURE,
								  PDTA_DestMode, PMODE_V43,
								  PDTA_Remap, FALSE,
								  PDTA_Displayable, FALSE,
								  TAG_DONE);

		//printf("dto:%d. freemem: %d\n", dt_object, AvailMem(MEMF_ANY));
	}
	else
	{
		dt_object = NewDTObject (0,
								  DTA_GroupID, GID_PICTURE,
								  DTA_SourceType, DTST_CLIPBOARD,
								  PDTA_DestMode, PMODE_V43,
								  PDTA_Remap, FALSE,
								  PDTA_Displayable, FALSE,
								  TAG_DONE);

		if (dt_object == NULL)
			printf("Failed!\n");
	}

	if (dt_object == NULL)
	{
		//printf("not enough memory\n");
		return NULL;
	}

    {
		int result;
        struct	TagItem	tags[] =
		{
			{PDTA_CRegs, (unsigned int) &cregs},
			{PDTA_NumColors, (unsigned int) &numcols},
			{PDTA_BitMapHeader, (unsigned int) &bmhd},
			{PDTA_BitMap, (unsigned int) &bitmap},
			{PDTA_AlphaChannel, (unsigned int) &alpha},
			{0, 0}
		};

        result = GetDTAttrsA(dt_object, tags);

		if (result != 5)
		{
			printf("[FILE]: Error in datatype\n");
			DisposeDTObject(dt_object);
			return NULL;
		}
    }

	//printf("time:%f\n",__timerGlobalGet(1.0f));
	//__timerGlobalStart();


	/* get datatype */

	{
		struct DataType *dt = NULL;

		if (GetDTAttrs(dt_object, DTA_DataType, (unsigned int)&dt, TAG_DONE))
		{
			if (dt != NULL)
			{
				char *name_string = dt->dtn_Header->dth_Name;
				setLastFileType(name_string);
			}
		}
		else
		{
			setLastFileType("Unknown");
		}
	}

	txt = txtCreate(bmhd->bmh_Width, bmhd->bmh_Height, chkpixfmt);

	//printf("time2:%f\n",__timerGlobalGet(1.0f));
	//__timerGlobalStart();

	if (txt == NULL)
    {
		//printf("[FILE]: Can't allocate buffer\n");
        DisposeDTObject(dt_object);
		return NULL;
    }

	if (bmhd->bmh_Depth <= 8) 				  // remap image to 24bit
    {
        struct RastPort rastport, temprastport;
        struct BitMap *tempbitmap;
        unsigned char *target, *colormap;
        unsigned char *penarray;
        int i, x, y;

        tempbitmap = AllocBitMap(bmhd->bmh_Width, 1, bmhd->bmh_Depth, BMF_CLEAR, NULL);
		if (!tempbitmap)
        {
            printf("[FILE]: Can't allocate temporary bitmap\n");
            DisposeDTObject(dt_object);
			FreeTexture(txt);
			return NULL;
        }

		colormap = calloc(numcols, 3);
        for (i = 0; i < numcols; i++)
        {
            colormap[ i * 3 + 0 ] = cregs[ 3 * i + 0 ] >> 24;
            colormap[ i * 3 + 1 ] = cregs[ 3 * i + 1 ] >> 24;
            colormap[ i * 3 + 2 ] = cregs[ 3 * i + 2 ] >> 24;
        }

        InitRastPort(&rastport);
        rastport.BitMap = bitmap;
        InitRastPort(&temprastport);
        temprastport.BitMap = tempbitmap;
		penarray = calloc(1, ((bmhd->bmh_Width + 15) >> 4) << 4);

		target = txt->image->data.b;
        for (y = 0; y < bmhd->bmh_Height; y++)
        {
            ReadPixelLine8(&rastport, 0, y, bmhd->bmh_Width, penarray, &temprastport);

			if (chkpixfmt == CHK_PIXFMT_ARGB8888)
			{
				for (x = 0; x < bmhd->bmh_Width; x++)
				{
					*target++ = 0xff;
					*target++ = colormap[ penarray[ x ] * 3 ];
					*target++ = colormap[ penarray[ x ] * 3 + 1 ];
					*target++ = colormap[ penarray[ x ] * 3 + 2 ];
				}
			}
			else if (chkpixfmt == CHK_PIXFMT_RGB888)
			{
				for (x = 0; x < bmhd->bmh_Width; x++)
				{
					*target++ = colormap[ penarray[ x ] * 3 ];
					*target++ = colormap[ penarray[ x ] * 3 + 1 ];
					*target++ = colormap[ penarray[ x ] * 3 + 2 ];
				}
			}
        }

        free(colormap);
        free(penarray);
        FreeBitMap(tempbitmap);

    }
	else if ( bmhd->bmh_Depth == 24 && alpha == 0)
    {
		#if 1
		int	i;

		for	(i=0;i<txt->image->height;i++)
		{
			unsigned char *dst;

			if (chkpixfmt == CHK_PIXFMT_ARGB8888)
				dst = txt->image->data.b + i * bmhd->bmh_Width * 4;
			else
				dst = txt->image->data.b + i * bmhd->bmh_Width * 3;

			pdtmsg.MethodID	= PDTM_READPIXELARRAY;
			pdtmsg.pbpa_PixelData = dst;
			pdtmsg.pbpa_PixelFormat	= chkpixfmt == CHK_PIXFMT_ARGB8888 ? PBPAFMT_ARGB : PBPAFMT_RGB;
			pdtmsg.pbpa_PixelArrayMod = bmhd->bmh_Width * 3;
			pdtmsg.pbpa_Left = 0;
			pdtmsg.pbpa_Top	= i;
			pdtmsg.pbpa_Width = bmhd->bmh_Width;
			pdtmsg.pbpa_Height = 1;

			DoMethodA(dt_object, (Msg)&pdtmsg);

			/* this method for RGB images sets alpha to 0. We need to set it to 0xff */

			if (chkpixfmt == CHK_PIXFMT_ARGB8888)
				convertARGB0888_to_ARGB8888(dst, dst, txt->image->width, 1);
		}

		#else

			__timerGlobalStart();

			pdtmsg.MethodID	= PDTM_READPIXELARRAY;
			pdtmsg.pbpa_PixelData = txt->image->data.p;
			pdtmsg.pbpa_PixelFormat	= PBPAFMT_RGB;
			pdtmsg.pbpa_PixelArrayMod = bmhd->bmh_Width * 3;
			pdtmsg.pbpa_Left = 0;
			pdtmsg.pbpa_Top	= 0;
			pdtmsg.pbpa_Width	= bmhd->bmh_Width;
			pdtmsg.pbpa_Height	= bmhd->bmh_Height;

			DoDTMethodA(dt_object, (Msg) & pdtmsg);
			//printf("time3.1:%f\n",__timerGlobalGet(1.0f));
			//__timerGlobalStart();
			//expandRGB888_to_ARGB8888(txt->image->data.p, txt->image->data.p, txt->image->width, txt->image->height);
			//printf("time3.2:%f\n",__timerGlobalGet(1.0f));

		#endif

    }
	else
	{
        pdtmsg.MethodID	= PDTM_READPIXELARRAY;
		pdtmsg.pbpa_PixelData	= txt->image->data.p;
		pdtmsg.pbpa_PixelFormat	= chkpixfmt == CHK_PIXFMT_ARGB8888 ? PBPAFMT_ARGB : PBPAFMT_RGB;
		pdtmsg.pbpa_PixelArrayMod = bmhd->bmh_Width * (chkpixfmt == CHK_PIXFMT_ARGB8888 ? 4 : 3);
		pdtmsg.pbpa_Left = 0;
		pdtmsg.pbpa_Top = 0;
		pdtmsg.pbpa_Width = bmhd->bmh_Width;
		pdtmsg.pbpa_Height = bmhd->bmh_Height;

		DoMethodA(dt_object, (Msg)&pdtmsg);
    }

	//printf("time3:%f\n",__timerGlobalGet(1.0f));
	//__timerGlobalStart();
    DisposeDTObject(dt_object);
	//printf("time4:%f\n",__timerGlobalGet(1.0f));
	return	txt;
}

////

/// static LoadDTPictureTRUE24()

static Texture *LoadDTPictureTRUE24(char *fname)
{
	return LoadDTPicture(fname, CHK_PIXFMT_RGB888);
}
////

/// static LoadDTPictureTRUE32()

static Texture *LoadDTPictureTRUE32(char *fname)
{
	return LoadDTPicture(fname, CHK_PIXFMT_ARGB8888);
}
////

/// static LoadReggaePictureTRUE32()

static Texture *LoadReggaePicture(char *fname, int chkpixfmt)
{
	Texture *txt = NULL;
	APTR fileobj = MultimediaBase != NULL ? MediaNewObjectTags(
		MMA_StreamName, (IPTR)fname,
		MMA_StreamType, (IPTR)"file.stream",
		MMA_MediaType, MMT_PICTURE,
		TAG_DONE) : NULL;

	if (fileobj != NULL)
	{
		ULONG pics;

		pics = xget(fileobj, MMA_Ports);

		if (pics)
		{
			int width  = MediaGetPort(fileobj, 0, MMA_Video_Width);
			int height = MediaGetPort(fileobj, 0, MMA_Video_Height);

			if (width > 0 && height > 0)
			{
				txt = txtCreate(width, height, chkpixfmt);

				if (txt != NULL)
				{

					if (chkpixfmt == CHK_PIXFMT_ARGB8888)
						DoMethod(fileobj, MMM_Pull, 0, txt->image->data.p, width * height * 4);
					else
					{
						unsigned char *buffer = mmalloc(width * 4);
						int i;

						if (buffer != NULL)
						{
							for(i=0; i<height; i++)
							{
								DoMethod(fileobj, MMM_Pull, 0, buffer, width * 4);
								convertRGB888_to_ARGB8888(buffer, txt->image->data.b + width * 3 * i, width, 1);
							}

							mfree(buffer);
						}
					}
				}
			}
		}

		DisposeObject(fileobj);
	}

	return txt;
}

////

/// fileGetType()

void fileGetType(char *fname, char *type)
{
	/* get datatype */

	BPTR lock = Lock(fname, ACCESS_READ);

	strcpy(type, "Unknown");

	if (lock != NULL)
	{
		/* check if it's recognizable by datatypes */

		struct DataType *dt = ObtainDataTypeA(DTST_FILE, (APTR)lock, NULL);

		if (dt != NULL)
		{
			char *name_string = dt->dtn_Header->dth_Name;

			strcpy(type, name_string);

			if (!stricmp(type, "JFIF"))
				strcpy(type, "JPEG");

			ReleaseDataType(dt);
		}

		UnLock(lock);
	}
}
////

/// fileGetInfo()/fileFreeInfo()

FileInfo *fileGetInfo(char *fname, int flags)
{
	FileInfo *finfo;

	finfo = calloc(1, sizeof(FileInfo));

	if (finfo != NULL)
	{
		/* get datatype */

		BPTR lock = Lock(fname, ACCESS_READ);

		if (lock != NULL)
		{
			struct DataType *dt = NULL;
			APTR fileobj = NULL;

			/* check if it's recognizable by datatypes */

			if (flags & FINFO_DATATYPE)
				dt = ObtainDataTypeA(DTST_FILE, (APTR)lock, NULL);

			if (dt != NULL)
			{
				char *name_string = dt->dtn_Header->dth_Name;

				finfo->type = strdup(name_string);
				if (finfo->type && !stricmp(finfo->type, "JFIF"))
					strcpy(finfo->type, "JPEG");

				finfo->gid = dt->dtn_Header->dth_GroupID;

				ReleaseDataType(dt);
			}
			else if (MultimediaBase != NULL) /* check if recognized by reggae */
			{
				fileobj = MediaNewObjectTags(
					MMA_StreamName, (IPTR)fname,
					MMA_StreamType, (IPTR)"file.stream",
					MMA_MediaType, MMT_PICTURE,
					TAG_DONE);

				if (fileobj != NULL)
				{
					ULONG pics = xget(fileobj, MMA_Ports);

					if (pics)
					{
						char *type  = (char*)MediaGetPort(fileobj, 0, MMA_DataFormat);
						if (type != NULL)
							finfo->type = strdup(type);

						finfo->gid = GID_PICTURE;
					}
					else
					{
						DisposeObject(fileobj);
						fileobj = NULL;
					}
				}
			}

			if (finfo->type == NULL)
			{
				finfo->type = strdup("Unknown");
			}

			/* get dimmensions */

			if (flags & FINFO_DIMMENSIONS)
			{
				int success = pngGetDimmensions(fname, &finfo->width, &finfo->height);
				if (success == FALSE)
					success = jpegGetDimmensions(fname, &finfo->width, &finfo->height);
				if (success == FALSE)
				{
					Object *dt_object = NewDTObject (fname,
											  DTA_GroupID, GID_PICTURE,
											  PDTA_DestMode, PMODE_V43,
											  PDTA_Remap, FALSE,
											  PDTA_Displayable, FALSE,
											  TAG_DONE);

					if (dt_object != NULL)
					{
						struct BitMapHeader *bmhd;
				        struct	TagItem	tags[] =
						{
							{PDTA_BitMapHeader, (unsigned int) &bmhd},
							{TAG_DONE}
						};

						int result = GetDTAttrsA(dt_object, tags);

						if (result == 1)
						{
							finfo->width = bmhd->bmh_Width;
							finfo->height = bmhd->bmh_Height;
							success = TRUE;
						}

						DisposeDTObject(dt_object);
				    }
				}
				if (success == FALSE)
				{
					if (fileobj != NULL)
					{
						finfo->width  = MediaGetPort(fileobj, 0, MMA_Video_Width);
						finfo->height = MediaGetPort(fileobj, 0, MMA_Video_Height);
					}
				}
			}

			if (fileobj != NULL)
				DisposeObject(fileobj);

			/* get other file properties (timestamp. size) */

			{
				struct FileInfoBlock fib;

				if (Examine(lock, &fib))
				{
					finfo->datestamp = fib.fib_Date;
					finfo->filesize = fib.fib_Size;
				}
			}
		}
		else
		{
			free(finfo);
			finfo = NULL;
		}

		UnLock( lock );
	}

	return finfo;

}
void fileFreeInfo(FileInfo  *finfo)
{
	if (finfo != NULL)
	{
		free(finfo->type);
		free(finfo);
	}
}

////

/// LoadPictureHI16()
#ifdef ENABLE_16BIT
Texture *LoadPictureHI16(char *fname)
{
	Texture	*txt = NULL;

	/* try to load using JPEG loader */
    if (stricmp(fname + strlen(fname) - 4, ".jpg") == 0)
    {
        txt = LoadPictureJPEG(fname);
    }

	if (!txt)
	{
		if	(!txt)
			txt = LoadDTPictureTRUE24(fname);

		if	(txt)
		{
			/* convert image into 16bit RGB */
			Texture	*nTxt = txtCreate(txt->image->width, txt->image->height, CHK_PIXFMT_RGB565);
			if	(!nTxt)
			{
				FreeTexture(txt);
				txt = NULL;
			}
			else
			{
				convertRGB888_to_RGB565(txt->image->data.p, nTxt->image->data.p, txt->image->width, txt->image->height);
				FreeTexture(txt);
				txt = nTxt;
			}
		}
	}

	if	(!txt)
	{
		//printf("[File]: Failed to load image: %s\n", fname);
		return	NULL;
	}

	txt->name = strduplicate(fname);

	return txt;
}
#endif
////

/// LoadPictureTRUE24()

Texture	*LoadPictureTRUE24(char *fname)
{
	Texture	*txt = NULL;
	int	len;

	if (fname == NULL)
	{
		/* load from clipboard */

		txt = LoadDTPictureTRUE32(NULL);

		if	(!txt)
			return	NULL;

		txt->name = strduplicate("Clipboard");

		return txt;
	}

	len = strlen(fname);
	#if 1
	/* try to load using JPEG loader */
	if (stricmp(fname + len - 4, ".jpg") == 0)
    {
		txt = LoadPictureJPEG24(fname);
		setLastFileType("JPEG");
    }

	if ((!txt) &&  len > 4 && (stricmp(fname + len - 4, ".ras") == 0))
    {
		txt = SUNLoadPicture24(fname);
		setLastFileType("Sun Raster");
    }
	if ((!txt) && len > 6 && (stricmp(fname + len - 6, ".ras24") == 0))
    {
		txt = SUNLoadPicture24(fname);
		setLastFileType("Sun Raster");
    }
	if ((!txt) && len > 6 && (stricmp(fname + len - 6, ".ras32") == 0))
    {
		txt = SUNLoadPicture24(fname);
		setLastFileType("Sun Raster");
    }
	#endif
	if  (!txt)
	{
		txt = LoadDTPictureTRUE24(fname);
	}

	if (txt == NULL)
	{
		//printf("[File]: Failed to load image: %s\n", fname);
		return	NULL;
	}

	txt->name = strduplicate(fname);
	txt->type = strDuplicate(getLastFileType());

	return txt;
}

////

/// LoadPictureGRAY8()

Texture	*LoadPictureGRAY8(char *fname)
{
	Texture		*txt = NULL;

	/* try to load using JPEG loader */
    if (stricmp(fname + strlen(fname) - 4, ".jpg") == 0)
    {
		txt = LoadPictureJPEG8(fname);
    }
	
	if	(!txt)
	{
		/* load using DT */

		if	(!txt)
			txt = LoadDTPictureTRUE24(fname);

		if	(txt)
		{
			/* convert image into 8bit LUMINANCE */
			Texture	*nTxt = txtCreate(txt->image->width, txt->image->height, CHK_PIXFMT_LUMINANCE);
			if	(!nTxt)
			{
				FreeTexture(txt);
				txt = NULL;
			}
			else
			{
				convertRGB888_to_L8(txt->image->data.p, nTxt->image->data.p, txt->image->width, txt->image->height);
				FreeTexture(txt);
				txt = nTxt;
			}
		}
	}

	if	(!txt)
	{
		//printf("[File]: Failed to load image: %s\n", fname);
		return	NULL;
	}

	txt->name = strduplicate(fname);

	return txt;
}

////

/// LoadPictureTRUE32()

Texture	*LoadPictureTRUE32(char *fname)
{
	return	LoadPictureARGB32(fname);
}

////

/// LoadPictureARGB16()

#ifdef ENABLE_16BIT
Texture	*LoadPictureARGB16(char *fname)
{
	Texture		*txt = NULL;

	/* try to load using JPEG loader */
    if (stricmp(fname + strlen(fname) - 4, ".jpg") == 0)
    {
		txt = LoadPictureJPEG32(fname);
    }

	if	(!txt)
		txt = LoadDTPictureTRUE32(fname);

	/* convert into 16bit ARGB */

	if	(txt)
	{
		Texture	*nTxt = txtCreate(txt->image->width, txt->image->height, CHK_PIXFMT_ARGB4444);
		if	(!nTxt)
		{
			FreeTexture(txt);
			txt = NULL;
		}
		else
		{
			convertARGB8888_to_ARGB4444(txt->image->data.p, nTxt->image->data.p, txt->image->width, txt->image->height);
			FreeTexture(txt);
			txt = nTxt;
		}
	}

	if	(!txt)
	{
		//printf("[File]: Failed to load image: %s\n", fname);
		return	NULL;
	}

	txt->name = strduplicate(fname);

	return txt;
}
#endif
////

/// LoadPictureARGB32()

Texture	*LoadPictureARGB32(char *fname)
{
	Texture	*txt = NULL;
	int	len;

	if (fname == NULL)
	{
		/* load from clipboard */

		txt = LoadDTPictureTRUE32(NULL);

		if (txt == NULL)
			return	NULL;

		txt->name = strduplicate("Clipboard");
		return txt;
	}

	len = strlen(fname);

	#if 1
	/* try to load using JPEG loader */
	if (stricmp(fname + len - 4, ".jpg") == 0)
    {
		//__timerGlobalStart();
		txt = LoadPictureJPEG32(fname);
		setLastFileType("JPEG");

		//printf("time:%f\n",__timerGlobalGet(1.0f));
    }

	if ((!txt) &&  len > 4 && (stricmp(fname + len - 4, ".ras") == 0))
    {
		txt = SUNLoadPicture32(fname);
		setLastFileType("Sun Raster");
    }
	if ((!txt) && len > 6 && (stricmp(fname + len - 6, ".ras24") == 0))
    {
		txt = SUNLoadPicture32(fname);
		setLastFileType("Sun Raster");
    }
	if ((!txt) && len > 6 && (stricmp(fname + len - 6, ".ras32") == 0))
    {
		txt = SUNLoadPicture32(fname);
		setLastFileType("Sun Raster");
    }
	#endif
	if (txt == NULL)
		txt = LoadDTPictureTRUE32(fname);
	
	if (txt == NULL)
		txt = LoadReggaePicture(fname, CHK_PIXFMT_ARGB8888);

	if (txt == NULL)
	{
		//printf("[File]: Failed to load image: %s\n", fname);
		return	NULL;
	}

	txt->name = strduplicate(fname);
	txt->type = strDuplicate(getLastFileType());

	return txt;
}

////

/// isDTPicture()

#include "timer.h"

int	isDTPicture(char *fname)
{

	#if 1
	//__timerGlobalStart();

	{
		int	isPicture = 0;
		BPTR lock = Lock(fname, ACCESS_READ);
		
		if (lock)
		{
			/* check if it's recognizable by datatypes */

			struct DataType *dt = ObtainDataType(DTST_FILE, (APTR)lock, DTA_GroupID, GID_PICTURE, NULL);

			if (dt != NULL)
			{
				/* check if it's a picture (should always be, but well) */

				if (dt->dtn_Header->dth_GroupID == GID_PICTURE)
					isPicture = 1;

				ReleaseDataType(dt);
			}

			UnLock(lock);

			/* check if recognized by reggae */

			if (isPicture == FALSE)
			{
				APTR fileobj = MultimediaBase != NULL ? MediaNewObjectTags(
					MMA_StreamName, (IPTR)fname,
					MMA_StreamType, (IPTR)"file.stream",
					MMA_MediaType, MMT_PICTURE,
					TAG_DONE) : NULL;

				if (fileobj != NULL)
				{
					isPicture = 1;
					DisposeObject(fileobj);
				}
			}
		}

		/* check for builtin image formats */

		{
			static	char *suffixes[] = {
				".ras",
				".ras32",
				".ras24",
			};

			int	num = 3;
			int	len = strlen(fname);
			int	i;

			for	(i=0;i<num;i++)
			{
				int	slen = strlen(suffixes[ i ]);
				if (slen <= len)
				{
					if (0 == stricmp(suffixes[ i ], &fname[ len - slen ]))
					{
						isPicture = 1;
					}
				}
			}
		}

		//printf("Time of analysis:%f\n",__timerGlobalGet(1.0f));

		return isPicture;
	}

	#else

		/* Fast recognition method. Not too accurate though */

	{
		static char *suffixes[] = {
			".jpg",
			".png",
			".iff",
			".iff24",
			".gif",
			".pcx",
			".ppm",
			".info",
			".ras",
			".ras32",
			".ras24",
			".ilbm",
			".bmp",
			".mim",
			".mf1",
			".mf0",
			".mbr",
			".jpeg"
		};

		int	num = 18;
		int	len = strlen(fname);
		int	i;

		for	(i=0;i<num;i++)
		{
			int	slen = strlen(suffixes[ i ]);
			if (slen <= len)
			{
				if (0 == stricmp(suffixes[ i ], &fname[ len - slen ]))
				{
					return 1;
				}
			}
		}

		return	0;
	}

	#endif
}

////

/// checkExtension()

int	checkExtension(char *fname, char *extension)
{
	int	len = strlen(fname);
	int slen = strlen(extension);
	
	if (slen <= len)
	{
		if (0 == stricmp(extension, &fname[len - slen]))
			return 1;
	}

	return 0;
}

////

/// dirDelete()

void dirDelete(char *path, int recursive)
{
    /* lock the dir */

	BPTR dirLock = Lock(path, ACCESS_READ);
	struct FileInfoBlock fib;

	if (!dirLock)
		return;

	if (dirLock)
	{
		if (Examine(dirLock, &fib))
		{
			if	(isDir(&fib))
			{
				/* fine. we can start examining the entries */

				while(ExNext(dirLock, &fib))
				{
					/* build new path */

					int buffSize = strlen(path) + strlen(fib.fib_FileName) + 2;
					char *newPath = calloc(1, buffSize);

					if	(path)
					{
						strcpy(newPath, path);
						AddPart(newPath, fib.fib_FileName, buffSize);

						/* check type of an entry */

						if	(isDir(&fib) && recursive)
						{
							/* call ourself for this new directory */

							dirDelete(newPath, recursive);
						}
						else
						{
							DeleteFile(newPath);
						}

						free(newPath);
					}
					else
					{
						printf("Not enough memory for path(%d bytes)\n",buffSize);
					}
				}
			}
		}

		UnLock(dirLock);
	}

	DeleteFile(path);
	return;
}

////

/// fileDelete()

int fileDelete(char *file)
{
	return DeleteFile(file);
}

////

/// fileCopyTo()

int	fileCopyTo(char *src, char *dst)
{
	char *buf;

	if (!src || !dst)
		return	0;

	/* check if they are equal first */

	if (pathEquals(src, dst))
	{
		return 1;
	}

	/* */

	buf = (char*)malloc(65536);

	if (buf != NULL)
	{
		BPTR f1 = Open(src, MODE_OLDFILE);
		BPTR f2 = NULL;

		if (f1 == NULL)
		{
			SetIoErr(ERROR_OBJECT_NOT_FOUND);
			free(buf);
			return 0;
		}

		f2 = Open(dst, MODE_NEWFILE);

		if (f1 && f2)
		{
			int	len = fileLength(src);

			if (len != SetFileSize(f2, len, OFFSET_BEGINING))
			{
				/* not enough space on the device */

				Close(f1);
				Close(f2);
				free(buf);
				DeleteFile(dst);
				return 0;
			}

			Seek(f2, 0, OFFSET_BEGINING);

			while(len)
			{
				int bytesw = 0;
				int	bytesr = Read(f1, buf, min(len, 65536));

				if (bytesr != min(len, 65536))
				{
					/* read error */

                    Close(f1);
					Close(f2);
					DeleteFile(dst);
					free(buf);
					return 0;
				}

				bytesw = Write(f2, buf, min(len, 65536));

				if (bytesr != bytesw)
				{
					/* write error */

                    Close(f1);
					Close(f2);
					DeleteFile(dst);
					free(buf);
					return 0;
				}

				len -= min(len, 65536);
			}

			Close(f1);
			Close(f2);
			free(buf);
			return 1;
		}
		else
		{
			/* failed to open file for read or write */

			if (f1)
				Close(f1);
			if (f2)
				Close(f2);
		}
		free(buf);
	}
	return 0;
}

////

/// filesCopy()

/*
 * This one kind of doesn't fit here, but well....
 */

int filesCopy(Object *app, char *reqtitle, char **list, char *dest, int move)
{
	int	error = 0;
	int	selected = 0;

	while(list[selected] != NULL)
	{
		selected++;
	};

	/* get destination */

	{
		int num = selected;
		int i;
		int forall = 0;

		/* process each name */

		for(i=0; i<num; i++)
		{
			char *name = list[ i ];

			/* add path to dest */

			char *newdest = strAddFilePart(dest, FilePart(name));
			char *path = name;

			/* mode for file */

			int skip = 0;

			if (forall != 2 && newdest)	  /* forall == 2 -> about, but we move through all anyway */
			{
				/* check if file exists */

				if (fileLength(newdest))
				{
					if (forall == 1)      /* replace */
						skip = 0;
					else if (forall == -1)/* skip */
						skip = 1;
					else
					{
						/* file exists. display req */
						int res;

						if (selected > 1)
						{
							res	= MUI_Request(app, NULL, 0, reqtitle, "*_Replace|Replace _All|_Skip|S_kip All|Abort ",
												"Destination file \33b%s \33nalready exists", FilePart(path),
												TAG_END);

							switch (res)
							{
								case 2:	 /* replace all */
									forall = 1;
								case 1:	 /* fall through to replace */
									skip = 0;
									break;
								case 4:	 /* skip all */
									forall = -1;
								case 3:	 /* fall through to skip */
									skip = 1;
									break;
								case 0:	 /* abort */
									forall = 2;
									skip = 1;
							};
						}
						else
						{
							res	= MUI_Request(app, NULL, 0, reqtitle, "*_Replace|Abort ",
												"Destination file \33b%s \33nalready exists", FilePart(newdest),
												TAG_END);
							switch (res)
							{
								case 1:	 /* replace all */
									forall = 1;
									break;
								case 0:	 /* skip all */
									forall = 2;
									skip = 1;
							};
						}
					}
				}

				/* copy */

				if (!skip)
				{
					if (!fileCopyTo(path, newdest))
					{
						/* error */

						char msg[128];

						Fault(IoErr(), "", msg, sizeof(msg));

						MUI_Request(app, NULL, 0, "ShowGirls · Error...", "OK",
									"Failed to copy file:\n\n\033b%s\033n to\n\033b%s\033n\n\nError message %s", path, dest, msg,
									TAG_END);
						error = 1;
					}
					else
					{
						/* if "move" mode then delete file */

						if (move)
							fileDelete(path);
					}
				}
			}
			/* free singlefile paths */

			free(newdest);
		}
	}

	return error;
}

////

char *getLastFileType(void)
{
	return lastFileType ? lastFileType : "";
}

/// pathEquals()

int pathEquals(char *p1, char *p2)
{
	BPTR l1 = Lock(p1, ACCESS_READ);
	BPTR l2 = Lock(p2, ACCESS_READ);
	char path1[1024];
	char path2[1024];
	int rc = FALSE;

	if (l1 == NULL || l2 == NULL)
	{
	}
	else
	{
		if (NameFromLock(l1, path1, sizeof(path1)) && NameFromLock(l2,path2,sizeof(path2)))
		{
			if (strcmp(path1, path2) == 0)
				rc = TRUE;
		}
	}

	UnLock(l1);
	UnLock(l2);

	return rc;
}

////

static void setLastFileType(char *type)
{
	free(lastFileType);

	lastFileType = strdup(type);
	if (lastFileType && !stricmp(lastFileType, "JFIF"))
		strcpy(lastFileType, "JPEG");
}

/// fileFormatString()

void fileFormatString(char *str, int n, char *fname)
{
}
////

/// CapacityFormat() - capacity formatter

char *CapacityFormat(char *s, unsigned int size, unsigned long long n)
{
	if (n < 1024 * 10)
		snprintf(s, size, "%u bytes", (unsigned int)n);
	else if (n < 1024 * 1024)
		snprintf(s, size, "%u.%u KB", (unsigned int) (n / 1024), (unsigned int)(n % 1024 * 10 / 1024));
	else if (n < 1024 * 1024 * 1024)
		snprintf(s, size, "%u.%u MB", (unsigned int) n / (1024 * 1024), (unsigned int)(n % (1024 * 1024) * 10 / (1024 * 1024)));
	else if (n < (unsigned long long)1024 * 1024 * 1024 * 1024)
		snprintf(s, size, "%u.%u GB", (unsigned int) (n / (1024 * 1024 * 1024)), (unsigned int) (n % (1024 * 1024 * 1024) * 10 / (1024 * 1024 * 1024)));
	else
		snprintf(s, size, "%u.%u TB", (unsigned int) (n / ((unsigned long long)1024 * 1024 * 1024 * 1024)), (unsigned int)(n % ((unsigned long long)1024 * 1024 * 1024 * 1024) * 10 / ((unsigned long long)1024 * 1024 * 1024 * 1024)));

	return s;
}

////


