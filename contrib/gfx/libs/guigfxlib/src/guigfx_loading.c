/*********************************************************************
----------------------------------------------------------------------

	guigfx_loading

----------------------------------------------------------------------
*********************************************************************/

#include <string.h>

#include <render/render.h>
#include <render/renderhooks.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#ifndef __AROS__
#include <datatypes/pictureclassext.h>
#endif
#include <libraries/cybergraphics.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>
#include <exec/memory.h>

#include <proto/dos.h>
#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/intuition.h>


#include <guigfx/guigfx.h>

#include "guigfx_global.h"
#include "guigfx_picture.h"
#include "guigfx_picturemethod.h"

#ifdef __AROS__
#define ReadBitMapArray(bm,pic,displayID,args...) \
({ \
     IPTR __args[] = { args }; \
     ReadBitMapArrayA((bm), (pic), (displayID), (TAGLIST)__args); \
})
#endif

/*********************************************************************
----------------------------------------------------------------------

	picture = LoadPictureDT ( filename, tags )

	GGFX_ErrorCode
	GGFX_HSType

----------------------------------------------------------------------
*********************************************************************/

PIC *LoadPictureDT(STRPTR file_name, TAGLIST tags)
{
	if (!DataTypesBase)
	{
		return NULL;
	}
	else
	{
		Object *obj;
		LONG err = 0, *errptr = NULL;
		PIC pic, *p = NULL;
		UBYTE *alphaarray = NULL;

		LONG transparencycolor = -1;		/* indicate that a mask is still to be created */

	
		TurboFillMem(&pic, sizeof(PIC), 0);
	
		errptr = (LONG *) GetTagData(GGFX_ErrorCode, (ULONG) errptr, tags);

	
		if ((obj = NewDTObject(file_name, PDTA_DestMode, PMODE_V43,
				DTA_SourceType, DTST_FILE, DTA_GroupID, GID_PICTURE,
				PDTA_Remap, FALSE, TAG_DONE)))
		{
			struct BitMapHeader *bmhd;
			ULONG displayID;
	
			if (GetDTAttrs(obj, PDTA_ModeID, (IPTR)&displayID,
		                 PDTA_BitMapHeader, (IPTR)&bmhd, TAG_DONE) == 2)
			{
				BOOL success = TRUE;			


				DB(kprintf("*** GetDTAttrs durchgelaufen. Width: %ld - Height %ld\n",
					bmhd->bmh_Width, bmhd->bmh_Height));
				

				DB(kprintf("DisplayID: %x\n",displayID));
			
				if (displayID == INVALID_ID)
				{
					displayID = NULL;
				}

				#ifdef DEBUG
			
				if (displayID & HAM)
				{
					DB(kprintf("DisplayID ist HAM\n"));
				}
				
				if (displayID & EXTRA_HALFBRITE)
				{
					DB(kprintf("DisplayID ist EHB\n"));
				}

				#endif


			
				DB(kprintf("----mask---- %ld\n", GetTagData(GGFX_UseMask, FALSE, tags)));
				if (GetTagData(GGFX_UseMask, FALSE, tags))
				{
					DB(kprintf("----checking 4 mask----\n"));
					if (bmhd->bmh_Masking != mskNone)
					{
						BOOL getmask = FALSE;
						UBYTE *plane = NULL;
	
						DB(kprintf("&&& bmh masking (%ld) detected.\n", bmhd->bmh_Masking));
						
						if (GetDTAttrs(obj, PDTA_MaskPlane, (IPTR)&plane, TAG_DONE) == 1)
						{
							getmask = (plane != NULL);
						}
						
						if (bmhd->bmh_Masking == mskHasTransparentColor && bmhd->bmh_Depth <= 8 && !(displayID & HAM))
						{
							getmask = TRUE;
						}
						
						
						if (getmask)
						{
							alphaarray = AllocRenderVec(MemHandler, bmhd->bmh_Width * bmhd->bmh_Height);
						}
	
						if (alphaarray)
						{
							if (plane)
							{
								int y, x;
								UBYTE bitmask, *a = alphaarray;
								
								for (y = 0; y < bmhd->bmh_Height; ++y)
								{
									bitmask = 0x80;
								
									for (x = 0; x < bmhd->bmh_Width; ++x)
									{					
										*a++ = (plane[x>>3] & bitmask) ? 255 : 0;
								
										if (!(bitmask >>= 1))
										{
											bitmask = 0x80;
										}
									}
									plane += (((bmhd->bmh_Width + 15) >> 4) << 1);
								}
								DB(kprintf("&&& created alpha-channel from PDTA_MaskPlane.\n"));
								success = TRUE;
							}
							else
							{
								transparencycolor = bmhd->bmh_Transparent;
								DB(kprintf("&&& creating alpha-channel from transparent color %ld later...\n", transparencycolor));
							}
						}
					}
				}
			
				if (success)
				{
					pic.pixelformat = PIXFMT_CHUNKY_CLUT;
					pic.width = bmhd->bmh_Width;
					pic.height = bmhd->bmh_Height;
					pic.aspectx = bmhd->bmh_XAspect ? bmhd->bmh_XAspect : 1;
					pic.aspecty = bmhd->bmh_YAspect ? bmhd->bmh_YAspect : 1;
					pic.hstype = GetTagData(GGFX_HSType, HSTYPE_UNDEFINED, tags);
					
					pic.pixelformat = (bmhd->bmh_Depth > 8) ? PIXFMT_0RGB_32 : PIXFMT_CHUNKY_CLUT;
			
					if (pic.pixelformat == PIXFMT_CHUNKY_CLUT)
					{
						if (DoDTMethod(obj, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE))
						{
							struct BitMap *bmap;
							if (GetDTAttrs(obj, PDTA_BitMap, (IPTR)&bmap, TAG_DONE) == 1)
							{
								if (bmap)
								{
									switch (pic.pixelformat)
									{
										case PIXFMT_CHUNKY_CLUT:
										{
											ULONG *creg;
											LONG ncol;
											if (GetDTAttrs(obj, PDTA_NumColors, (IPTR)&ncol, PDTA_CRegs, (IPTR)&creg, TAG_DONE) == 2)
											{
												if (creg && ncol)
												{
													if ((pic.palette = CreatePalette(RND_RMHandler, (IPTR)MemHandler, RND_HSType, 
														pic.hstype == HSTYPE_UNDEFINED ? DEFAULT_PALETTE_HSTYPE : pic.hstype, 
														TAG_DONE)))
													{
														ImportPalette(pic.palette, creg, ncol,
															RND_PaletteFormat, PALFMT_RGB32, 
															RND_EHBPalette, displayID & EXTRA_HALFBRITE,
															TAG_DONE);
			
														if (!ReadBitMapArray(bmap, &pic, displayID, NULL))
														{
															err = ERROR_NO_FREE_STORE;
														}

														/*
														**	the palette is no longer needed when the picture
														**	is a planar HAM image! we must discard it
														*/

														if (displayID & HAM)
														{
															if (pic.palette)
															{
																DeletePalette(pic.palette);
																pic.palette = NULL;
															}
														}

		
													}
													else err = ERROR_NO_FREE_STORE;
												}
												else err = ERROR_REQUIRED_ARG_MISSING;
											}
											else err = ERROR_REQUIRED_ARG_MISSING;
											break;
										}
		
										case PIXFMT_0RGB_32:
										{
											if (!ReadBitMapArray(bmap, &pic, displayID, NULL))
											{
												err = ERROR_NO_FREE_STORE;
											}
											break;		
										}
									}
								}
								else err = ERROR_NO_FREE_STORE;
							}
							else err = ERROR_REQUIRED_ARG_MISSING;
						}
						else
						{
							if ((err = IoErr()) == 0)
							{
								err = ERROR_NO_FREE_STORE;
							}
						}
		
					}
					else
					{
						DB(kprintf("*** DTM_READPIXELARRAY\n"));
						if ((pic.array = AllocRenderVec(MemHandler, 4 * pic.width * pic.height)))
						{
							DoMethod(obj, PDTM_READPIXELARRAY, (IPTR)pic.array,
									RECTFMT_ARGB, pic.width * 4,
									0,0, pic.width, pic.height);
						}
						else err = ERROR_NO_FREE_STORE;
					}
				}
				else err = ERROR_NO_FREE_STORE;
			}
			else
			{
				if ((err = IoErr()) == 0)
				{
					err = ERROR_NO_FREE_STORE;
				}
			}
	
			DisposeDTObject(obj);
		}
		else
		{
			if ((err = IoErr()) == 0)
			{
				err = ERROR_OBJECT_WRONG_TYPE;
			}
		}
	
		if (!err)
		{		
			if (!(p = MakePicture(pic.array, pic.width, pic.height,
				GGFX_PaletteFormat, PALFMT_PALETTE,
				GGFX_HSType, pic.hstype,
				GGFX_PixelFormat, pic.pixelformat,
				GGFX_AspectX, pic.aspectx,
				GGFX_AspectY, pic.aspecty,
				GGFX_Palette, (ULONG)(pic.palette),
				GGFX_Owner, TRUE,
				TAG_DONE)))
			{
				err = ERROR_NO_FREE_STORE;
			}
			else
			{
				if (alphaarray)
				{
					if (transparencycolor > -1 && pic.pixelformat == PIXFMT_CHUNKY_CLUT)
					{
						int y, x;
						UBYTE *p = (UBYTE *) pic.array;
						UBYTE *a = alphaarray;
						
						for (y = 0; y < pic.height; ++y)
						{
							for (x = 0; x < pic.width; ++x)
							{
								*a++ = (*p++ == transparencycolor ? 0 : 255);
							}
						}
					
						DB(kprintf("&&& created alpha channel from transparency color.\n"));
					}

					if (PIC_SetAlpha(p, alphaarray, pic.width, pic.height, NULL))
					{
						DB(kprintf("&&& alpha channel set!\n"));
					}
					else
					{
						DB(kprintf("&&& error: alpha channel not set!!\n"));
					}
				}
			}
		}


		if (p == NULL)		//err
		{
			if (pic.array)
			{
				FreeRenderVec(pic.array);
			}
		
			if (pic.palette)
			{
				DeletePalette(pic.palette);
			}
		}
		

		if (alphaarray)
		{
			FreeRenderVec(alphaarray);
		}


		if (errptr)
		{
			*errptr = err;
		}
	
		return p;
	}
}




#if 0

/*--------------------------------------------------------------------

	isjpeg = DetectJPEG(filename)

--------------------------------------------------------------------*/

BOOL DetectJPEG(STRPTR filename)
{
	BOOL jpeg = FALSE;
	BPTR fh;
	APTR obj;

	if (TowerBase)
	{
		if (fh = Open(filename, MODE_OLDFILE))
		{
			if (obj = NewExtObject(JPEGCODEC, CDA_Coding, FALSE,
				CDA_StreamType, CDST_FILE, CDA_Stream, fh,
				TAG_DONE))
			{
				if (DoMethod(obj, CDM_READHEADER) == HEADER_READY)
				{
					jpeg = TRUE;
				}

				DisposeExtObject(obj);			
			}
			Close(fh);
		}		
	}

	return jpeg;
}


/*--------------------------------------------------------------------

	pic = LoadJPEG(filename, tags)

--------------------------------------------------------------------*/

PIC *LoadJPEG(STRPTR filename, TAGLIST tags)
{
	PIC *pic = NULL;

	if (TowerBase && IntuitionBase)
	{

		BPTR fh;
		APTR obj;
		ULONG colorspace;
		ULONG pixelformat = 0, width = 0, height = 0, components;
		char *linebuffer;
		LONG err;
		int x, y;
		char *buffer = NULL;

		ULONG hstype = GetTagData(GGFX_HSType, HSTYPE_UNDEFINED, tags);
		LONG *errptr = (LONG *) GetTagData(GGFX_ErrorCode, NULL, tags);

		if (fh = Open(filename, MODE_OLDFILE))
		{
			if (obj = NewExtObject(JPEGCODEC, CDA_Coding, FALSE,
				CDA_StreamType, CDST_FILE, CDA_Stream, fh,
				TAG_DONE))
			{
				if (DoMethod(obj, CDM_READHEADER) == HEADER_READY)
				{
					if (GetAttr(PCDA_ColorSpace, obj, &colorspace))
					{
						if (colorspace == PCDCS_GRAYSCALE)
						{
							pixelformat = PIXFMT_CHUNKY_CLUT;
						}
						else
						{
							SetAttrs(obj, PCDA_ColorSpace, PCDCS_RGB, TAG_DONE);
							pixelformat = PIXFMT_0RGB_32;
						}

						if (DoMethod(obj, CDM_START))
						{
							if (GetAttr(PCDA_Width, obj, &width) &&
								GetAttr(PCDA_Height, obj, &height) &&
								GetAttr(PCDA_Components, obj, &components))
							{
								if (components == 3 || components == 1)
								{
									err = 0;

									if (buffer = AllocRenderVec(MemHandler, PIXELSIZE(pixelformat) * width * height))
									{
										char *bufp = buffer;
							
										if (pixelformat == PIXFMT_CHUNKY_CLUT)
										{
											for (y = 0; y < height; ++y)
											{
												if (!DoMethod(obj, CDM_PROCESS, bufp, width))
												{
													if (!(err = IoErr())) err = ERROR_NO_FREE_STORE;
													break;
												}
												bufp += width;
											}
										}
										else
										{
											if (linebuffer = AllocRenderVec(MemHandler, width * 3))
											{
												for (y = 0; y < height; ++y)
												{
													if (!DoMethod(obj, CDM_PROCESS, linebuffer, width * 3))
													{
														if (!(err = IoErr())) err = ERROR_NO_FREE_STORE;
														break;
													}
													
													for (x = 0; x < width; ++x)
													{
														bufp[0] = 0;
														bufp[1] = linebuffer[x * 3];
														bufp[2] = linebuffer[x * 3 + 1];
														bufp[3] = linebuffer[x * 3 + 2];
														bufp += 4;
													}
												}
												FreeRenderVec(linebuffer);
											}
											else err = ERROR_NO_FREE_STORE;
										}

									}
									else err = ERROR_NO_FREE_STORE;

								}
								else err = ERROR_NOT_IMPLEMENTED;
							}
							else err = ERROR_REQUIRED_ARG_MISSING;
						}
						else err = ERROR_NOT_IMPLEMENTED;
					}
					else err = ERROR_NO_FREE_STORE;
				}
				else err = ERROR_NO_FREE_STORE;
				
				DisposeExtObject(obj);			
			}
			else if (!(err = IoErr())) err = ERROR_NO_FREE_STORE;

			Close(fh);
		}
		else if (!(err = IoErr())) err = ERROR_NO_FREE_STORE;


		if (err)
		{
			FreeRenderVec(buffer);
		}
		else
		{
			if (buffer && width && height)
			{
				if (!(pic = MakePicture(buffer, width, height,
					GGFX_HSType, hstype,
					GGFX_PixelFormat, pixelformat,
					GGFX_Owner, TRUE,
					TAG_DONE)))
				{
					err = ERROR_NO_FREE_STORE;
				}
			}
			else
			{
				FreeRenderVec(buffer);
			}
		}
		
		if (errptr)
		{
			*errptr = err;
		}

	}

	return pic;
}

#endif


/*********************************************************************
----------------------------------------------------------------------

	BOOL IsPicture ( filename, tags )

	GGFX_FormatName

----------------------------------------------------------------------
*********************************************************************/

BOOL SAVE_DS ASM IsPictureA(REG(a0) char *file_name, REG(a1) TAGLIST tags)
{
	struct DataType *dtn;
	struct DataTypeHeader *dth;
	BPTR lock;
	BOOL it_is = FALSE;


	if (DataTypesBase)
	{
	   if ((lock = Lock(file_name, ACCESS_READ)))
	   {
	      if ((dtn = ObtainDataTypeA(DTST_FILE, (APTR)lock, NULL)))
	      {
	         dth = dtn->dtn_Header;
	         if (dth->dth_GroupID == GID_PICTURE)   /* is it a picture? */
	         {
				char **formatname = (char **) GetTagData(GGFX_FormatName, NULL, tags);

	            it_is = TRUE;
	
				if (formatname)
				{
					int l = strlen(dth->dth_Name);
					if (l > 0)
					{
						*formatname = AllocVec(l + 1, MEMF_ANY);
						if (*formatname)
						{
							strcpy(*formatname, dth->dth_Name);
						}
					}
				}
	         }

	         ReleaseDataType(dtn);
	      }
	      UnLock(lock);
	   }
	}
	
	return(it_is);
}




/*********************************************************************
----------------------------------------------------------------------

	picture = LoadPicture ( filename, tags )

	GGFX_ErrorCode
	GGFX_HSType
	GGFX_License

----------------------------------------------------------------------
*********************************************************************/

PIC SAVE_DS ASM *LoadPictureA(REG(a0) STRPTR filename, REG(a1) TAGLIST tags)
{
	PIC *pic = NULL;


	#if 0
	if (GetTagData(GGFX_License, FALSE, tags))
	{
		if (DetectJPEG(filename))
		{
			DB(kprintf("using JPEG directly!\n"));
			pic = LoadJPEG(filename, tags);
		}
	}
	#endif
	

	if (!pic)
	{
		pic = LoadPictureDT(filename, tags);
	}

	return pic;
}

