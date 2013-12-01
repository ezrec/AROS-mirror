// SavePngIcon.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>

#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/dos.h>
#ifdef __amigaos4__
#define Flush FFlush
#endif /* __amigaos4__ */
#include <proto/utility.h>

#include <clib/alib_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "png.h"
#include <defs.h>
#include "PNGIconDt.h"
#include "PNGIconProto.h"

//-----------------------------------------------------------------------------

static BOOL WritePng(struct InstanceData *inst, BPTR file,
	struct PngChunk *IconChunk, struct ARGBHeader *ARGBImage);
static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size);
static void PngFreeMem(png_structp png_ptr, png_voidp ptr);
static void PngWriteData(png_structp png_ptr, png_bytep data, png_size_t length);
static void PngFlushData(png_structp png_ptr);
static void PngError(png_structp png_ptr, png_const_charp error_message);
static void PngWarning(png_structp png_ptr, png_const_charp warning_message);
static BOOL GenerateIconHunk(struct InstanceData *inst, struct PngChunk *chunk);
static ULONG GetIconHunkSize(struct InstanceData *inst);
static ULONG *AddItem(ULONG *IconHunkPtr, size_t *Length,
	ULONG tag, const void *Value, size_t ValueLength);

//-----------------------------------------------------------------------------

ULONG SavePngIcon(struct InstanceData *inst, CONST_STRPTR name,
	struct ARGBHeader *NormalARGBImage, struct ARGBHeader *SelectedARGBImage)
{
	BPTR file = 0;
	STRPTR namedotinfo;
	ULONG success = FALSE;
	struct PngChunk IconChunk;

	do	{
		memset(&IconChunk, 0, sizeof(IconChunk));

		// Build "filename.info"
		namedotinfo = MyAllocVecPooled(1 + strlen(name) + 5);
		if (NULL == namedotinfo)
			{
			SetIoErr(ERROR_NO_FREE_STORE);
			break;
			}

		strcpy(namedotinfo, name);
		strcat(namedotinfo, ".info");

		file = Open(namedotinfo, MODE_NEWFILE);
		d1(KPrintF("%s/%s/%ld:  file=%08lx\n", __FILE__, __FUNC__, __LINE__, file));
		if ((BPTR)NULL == file)
			break;

		// Generate icOn hunk
		if (!GenerateIconHunk(inst, &IconChunk))
			break;

		if (!WritePng(inst, file, &IconChunk, NormalARGBImage))
			break;
		if (SelectedARGBImage->argb_ImageData)
			{
			if (!WritePng(inst, file, &IconChunk, SelectedARGBImage))
				break;
			}

		d1(KPrintF("%s/%s/%ld:  DataLength=%lu\n", __FILE__, __FUNC__, __LINE__, IconChunk.DataLength));

		success = TRUE;
		} while (0);

	if (file) 
		Close(file);
	if (IconChunk.Data)
		MyFreeVecPooled(IconChunk.Data);
	if (namedotinfo) 
		MyFreeVecPooled(namedotinfo);

	return success;
}


static BOOL WritePng(struct InstanceData *inst, BPTR file,
	struct PngChunk *IconChunk, struct ARGBHeader *ARGBImage)
{
	BOOL Success = FALSE;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytepp row_pointers = NULL;

	do	{
		struct ARGB *argb;
		ULONG y;

		/* Create and initialize the png_struct with the desired error handler
		 * functions.  If you want to use the default stderr and longjump method,
		 * you can supply NULL for the last three parameters.  We also check that
		 * the library version is compatible with the one used at compile time,
		 * in case we are using dynamically linked libraries.  REQUIRED.
		 */
		png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING,
			(png_voidp) inst, PngError, PngWarning,
			NULL, PngAllocMem, PngFreeMem);

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));
		if (png_ptr == NULL)
			break;

		/* Allocate/initialize the image information data.  REQUIRED */
		info_ptr = png_create_info_struct(png_ptr);
		d1(KPrintF("%s/%s/%ld:  info_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, info_ptr));
		if (info_ptr == NULL)
			break;

		d1(KPrintF("%s/%s/%ld:  png_jmpbuf=%08lx\n", __FILE__, __FUNC__, __LINE__, png_jmpbuf(png_ptr)));
		if (NULL == png_jmpbuf(png_ptr))
			break;

		/* Set error handling.  REQUIRED if you aren't supplying your own
		 * error handling functions in the png_create_write_struct() call.
		 */
		if (setjmp(png_jmpbuf(png_ptr)))
			{
			/* If we get here, we had a problem reading the file */
			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));
			Success = FALSE;
			break;
			}

		/* If you are using replacement read functions, instead of calling
		 * png_init_io() here you would call */
		png_set_write_fn(png_ptr, (void *) file, PngWriteData,
			PngFlushData);

		/* Set the image information here.  Width and height are up to 2^31,
		 * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
		 * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
		 * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
		 * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
		 * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
		 * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
		 */
		png_set_IHDR(png_ptr, info_ptr,
			ARGBImage->argb_Width,
			ARGBImage->argb_Height,
			8,
			PNG_COLOR_TYPE_RGB_ALPHA,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE,
			PNG_FILTER_TYPE_BASE);

		/* swap location of alpha bytes from ARGB to RGBA */
		png_set_swap_alpha(png_ptr);

		row_pointers = MyAllocVecPooled(sizeof(png_bytep) * ARGBImage->argb_Height);
		d1(KPrintF("%s/%s/%ld:  row_pointers=%08lx\n", __FILE__, __FUNC__, __LINE__, row_pointers));
		if (NULL == row_pointers)
			break;

		argb = ARGBImage->argb_ImageData;
		for (y = 0; y < ARGBImage->argb_Height; y++)
			{
			row_pointers[y] = (png_bytep) argb;
			argb += ARGBImage->argb_Width;
			}

		png_set_rows(png_ptr, info_ptr, row_pointers);

		/* Write the file header information.  REQUIRED */
		png_write_info(png_ptr, info_ptr);

		/* write out the entire image data in one call */
		png_write_image(png_ptr, row_pointers);

		/* Write a PNG chunk - size, type, (optional) data, CRC. */
		png_write_chunk(png_ptr, (png_bytep) "icOn", IconChunk->Data, IconChunk->DataLength);

		/* It is REQUIRED to call this to finish writing the rest of the file */
		png_write_end(png_ptr, info_ptr);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		Success = TRUE;
		} while (0);

	if (row_pointers)
		MyFreeVecPooled(row_pointers);

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	d1(KPrintF("%s/%s/%ld:  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size)
{
	(void) png_ptr;

	return MyAllocVecPooled(size);
}

static void PngFreeMem(png_structp png_ptr, png_voidp ptr)
{
	(void) png_ptr;
	MyFreeVecPooled(ptr);
}


static void PngWriteData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	BPTR fh = (BPTR) png_get_io_ptr(png_ptr);

	if (1 != FWrite(fh, data, length, 1))
		{
//		png_error();
		}
}

static void PngFlushData(png_structp png_ptr)
{
	BPTR fh = (BPTR) png_get_io_ptr(png_ptr);

	if (!Flush(fh))
		{
//		png_error();
		}
}


static void PngError(png_structp png_ptr, png_const_charp error_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, error_message));

	longjmp(png_jmpbuf(png_ptr), 1);
}


static void PngWarning(png_structp png_ptr, png_const_charp warning_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, warning_message));
//	png_default_warning(png_ptr, warning_message);
}


static ULONG GetIconHunkSize(struct InstanceData *inst)
{
	// Calculate allocation size for icOn hunk
	size_t Length = 0;
	ULONG tag;

	// Icon Type
	Length += sizeof(ULONG) + sizeof(ULONG);

	// Write out ToolTypes
	if (inst->id_ToolTypes)
		{
		STRPTR *tt;

		for(tt = inst->id_ToolTypes; *tt; tt++)
			{
			Length += sizeof(ULONG) + 1 + strlen(*tt);
			}
		}

	d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

	// Write out Stack Size
	if ((inst->id_Type == WBPROJECT) || (inst->id_Type == WBTOOL))
		{
		Length += sizeof(ULONG) + sizeof(inst->id_StackSize);
		}

	d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));


	// Write out Default Tool
	if (inst->id_Type == WBPROJECT)
		{
		if (inst->id_DefaultTool)
			{
			Length += sizeof(ULONG) + 1 + strlen(inst->id_DefaultTool);
			}
		else
			{
			Length += sizeof(ULONG) + 1;
			}
		}

	d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

	// Write out tool window
	if (inst->id_ToolWindow)
		{
		Length += sizeof(ULONG) + 1 + strlen(inst->id_ToolWindow);
		}

	d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

	// Write out X and Y pos

	if ((inst->id_CurrentX != NO_ICON_POSITION) && (inst->id_CurrentY != NO_ICON_POSITION))
		{
		Length += sizeof(ULONG) + sizeof(inst->id_CurrentX);
		Length += sizeof(ULONG) + sizeof(inst->id_CurrentY);
		}

	d1(kprintf("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));


	// Write out Drawer or Disk data

	if((inst->id_Type == WBDISK)||(inst->id_Type == WBDRAWER)||(inst->id_Type == WBGARBAGE))
		{
		ULONG flags = inst->id_DrawerData.dd_Flags;
		UWORD viewmodes = inst->id_DrawerData.dd_ViewModes;

		// Write out viewmode and sortmode stuff
		if(!((flags == DDFLAGS_SHOWDEFAULT) && (viewmodes == DDVM_BYDEFAULT)))
			{
			Length += sizeof(ULONG) + sizeof(tag);

			// Write out sort mode
			if (inst->id_DrawerData.dd_ViewModes >= DDVM_BYNAME)
				Length += sizeof(ULONG) + sizeof(tag);
			}

		d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

		// Write out drawer size and position
		Length += 4 * (sizeof(ULONG) + sizeof(tag));

		// Write out dd_CurrentX and dd_CurrentY
		Length += 2 * (sizeof(ULONG) + sizeof(tag));
		}

	d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

	return Length;
}


// Generate icOn hunk
static BOOL GenerateIconHunk(struct InstanceData *inst, struct PngChunk *chunk)
{
	BOOL Success = FALSE;

	do	{
		ULONG *IconHunkPtr;
		size_t Length;
		ULONG tag;

		chunk->ID = PNGICONHEADER;
		chunk->DataLength = GetIconHunkSize(inst);

		chunk->Data = MyAllocVecPooled(chunk->DataLength);
		d1(KPrintF("%s/%s/%ld:  Data=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, chunk->Data, Length));
		if (NULL == chunk->Data)
			{
			SetIoErr(ERROR_NO_FREE_STORE);
			break;
			}

		IconHunkPtr = (ULONG *) chunk->Data;
		Length = chunk->DataLength;
		d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));
		d1(KPrintF("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));

		// write icon type
		tag = SCA_LONG2BE(inst->id_Type);
		IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_TYPE, &tag, sizeof(ULONG));

		// Write out ToolTypes
		if (inst->id_ToolTypes)
			{
			STRPTR *tt;

			for(tt = inst->id_ToolTypes; *tt; tt++)
				{
				IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_TOOLTYPE, *tt, 1 + strlen(*tt));
				}
			}

		d1(kprintf("%s/%s/%ld:  Length=%lu\n", __FILE__, __FUNC__, __LINE__, Length));

		// Write out Stack Size
		if ((inst->id_Type == WBPROJECT) || (inst->id_Type == WBTOOL))
			{
			tag = SCA_LONG2BE(inst->id_StackSize);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_STACKSIZE, &tag, sizeof(inst->id_StackSize));
			}

		d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));

		// Write out Default Tool
		if (inst->id_Type == WBPROJECT)
			{
			if (inst->id_DefaultTool)
				{
				IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DEFTOOL, inst->id_DefaultTool, 1 + strlen(inst->id_DefaultTool));
				}
			else
				{
				IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DEFTOOL, "", 1);
				}
			}

		d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));

		// Write out tool window
		if (inst->id_ToolWindow)
			{
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_TOOLWINDOW, inst->id_ToolWindow, 1 + strlen(inst->id_ToolWindow));
			}

		d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));

		// Write out X and Y pos

		if ((inst->id_CurrentX != NO_ICON_POSITION) && (inst->id_CurrentY != NO_ICON_POSITION))
			{
			tag = SCA_LONG2BE(inst->id_CurrentX);
			d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_XPOS, &tag, sizeof(inst->id_CurrentX));
			d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));
			tag = SCA_LONG2BE(inst->id_CurrentY);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_YPOS, &tag, sizeof(inst->id_CurrentY));
			d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));
			}

		d1(kprintf("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));


		// Write out Drawer or Disk data

		if((inst->id_Type == WBDISK)||(inst->id_Type == WBDRAWER)||(inst->id_Type == WBGARBAGE))
			{
			ULONG flags = inst->id_DrawerData.dd_Flags;
			UWORD viewmodes = inst->id_DrawerData.dd_ViewModes;

			// Write out viewmode and sortmode stuff
			if(!((flags == DDFLAGS_SHOWDEFAULT) && (viewmodes == DDVM_BYDEFAULT)))
				{
				if (flags == DDFLAGS_SHOWDEFAULT)
					flags = DDFLAGS_SHOWICONS;
				if (viewmodes == DDVM_BYDEFAULT)
					viewmodes = DDVM_BYICON;

				tag = 0;
				if (flags & DDFLAGS_SHOWALL)
					tag |= PNGDM_SHOWALLFILES; // WB inconsistancy
				if (viewmodes == DDVM_BYICON)
					tag |= PNGDM_VIEWBYICON;
				else
					tag |=((viewmodes-DDVM_BYNAME)<<2);

				tag = SCA_LONG2BE(tag);
				IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DRAWERVIEWMODE, &tag, sizeof(tag));

				// Write out sort mode
				if (inst->id_DrawerData.dd_ViewModes >= DDVM_BYNAME)
					{
					tag = SCA_LONG2BE(inst->id_DrawerData.dd_ViewModes - DDVM_BYNAME);
					IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_SORTMODE, &tag, sizeof(tag));
					}

				d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));
				}

			d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));

			// Write out drawer size and position
			tag = SCA_LONG2BE(inst->id_DrawerData.dd_NewWindow.LeftEdge);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DRAWERXPOS, &tag, sizeof(tag));

			tag = SCA_LONG2BE(inst->id_DrawerData.dd_NewWindow.TopEdge);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DRAWERYPOS, &tag, sizeof(tag));

			tag = SCA_LONG2BE(inst->id_DrawerData.dd_NewWindow.Width);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DRAWERWIDTH, &tag, sizeof(tag));

			tag = SCA_LONG2BE(inst->id_DrawerData.dd_NewWindow.Height);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_DRAWERHEIGHT, &tag, sizeof(tag));

			d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));

			// Write out dd_CurrentX and dd_CurrentY
			tag = SCA_LONG2BE(inst->id_DrawerData.dd_CurrentX);
			IconHunkPtr = AddItem(IconHunkPtr, &Length, PNGICONA_OFFSETX, &tag, sizeof(tag));

			tag = SCA_LONG2BE(inst->id_DrawerData.dd_CurrentY);
			/* IconHunkPtr = */ AddItem(IconHunkPtr, &Length, PNGICONA_OFFSETY, &tag, sizeof(tag));
			}

		d1(KPrintF("%s/%s/%ld:  IconHunkPtr=%08lx  Length=%lu\n", __FILE__, __FUNC__, __LINE__, IconHunkPtr, Length));

		Success = TRUE;
		} while (0);

	return Success;
}


static ULONG *AddItem(ULONG *IconHunkPtr, size_t *Length,
	ULONG tag, const void *Value, size_t ValueLength)
{
	if (*Length >= sizeof(tag) + ValueLength)
		{
		*IconHunkPtr++ = SCA_LONG2BE(tag);

		if (ValueLength)
			{
			memcpy(IconHunkPtr, Value, ValueLength);
			IconHunkPtr = (ULONG *) (((UBYTE *) IconHunkPtr) + ValueLength);
			}

		*Length -= sizeof(tag) + ValueLength;
		}

	return IconHunkPtr;
}

