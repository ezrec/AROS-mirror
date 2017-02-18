/*
 * targa.datatype
 * (c) Fredrik Wikstrom
 */

#include "targa_class.h"

#define RGB8to32(RGB) (((ULONG)(RGB) << 24)|((ULONG)(RGB) << 16)|((ULONG)(RGB) << 8)|((ULONG)(RGB)))

static LONG WriteTarga (Class *cl, Object *o, struct dtWrite *msg);
static LONG ConvertTarga (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
static LONG GetTarga (Class *cl, Object *o, struct TagItem *tags);

IPTR TARGA__OM_NEW(Class *cl, Object *o, struct opSet *msg)
{
	IPTR ret;
	ret = DoSuperMethodA(cl, o, msg);
	if (ret) {
		LONG error;
		error = GetTarga(cl, (Object *)ret, msg->ops_AttrList);
		if (error != OK) {
			CoerceMethod(cl, (Object *)ret, OM_DISPOSE);
			ret = 0;
			SetIoErr(error);
		}
	}
	return ret;
}

IPTR TARGA__DTM_WRITE(Class *cl, Object *o, struct dtWrite *msg)
{
	IPTR ret;
	/* check if dt's native format should be used */
	if (msg->dtw_Mode == DTWM_RAW) {
		LONG error;
		error = WriteTarga(cl, o, msg);
		if (error == OK) {
			ret = TRUE;
		} else {
			SetIoErr(error);
			ret = FALSE;
		}
	} else {
		ret = DoSuperMethodA(cl, o, msg);
	}
	return ret;
}

static LONG WriteTarga (Class *cl, Object *o, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}

static LONG ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
static LONG ReadRGB15 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
static LONG ReadRGBA16 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
static LONG ReadTruecolor (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
static LONG ReadPixels (struct Library *libBase, struct TargaHeader *tga, BPTR file, UBYTE *buf, LONG width, LONG bpp);

static LONG GetCMAP24 (struct Library *libBase, BPTR file, struct ColorRegister *cmap, LONG len);

static LONG ConvertTarga (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	LONG status, error = OK;
	struct TargaHeader tga;
	struct TargaHeaderFile tgafile;
	LONG (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
	int needs_palette;

	status = Read(file, &tgafile, sizeof(struct TargaHeaderFile));
	if (status != sizeof(struct TargaHeaderFile)) {
		return ReadError(status);
	}

	tga.idsize = tgafile.idsize;
	tga.cmaptype = tgafile.cmaptype;
	tga.imagetype = tgafile.imagetype;
	tga.cmapstart = AROS_LE2WORD(*(WORD *)tgafile.cmapstart);
	tga.cmaplength = AROS_LE2WORD(*(WORD *)tgafile.cmaplength);
	tga.cmapdepth = tgafile.cmapdepth;
	tga.xstart = AROS_LE2WORD(tgafile.xstart);
	tga.ystart = AROS_LE2WORD(tgafile.ystart);
	tga.width = AROS_LE2WORD(tgafile.width);
	tga.height = AROS_LE2WORD(tgafile.height);
	tga.depth = tgafile.depth;
	tga.descriptor = tgafile.descriptor;

	bmh->bmh_Width = tga.width;
	bmh->bmh_Height = tga.height;
	bmh->bmh_Depth = tga.depth;

	if (tga.cmaptype > 1) {
		return ERROR_NOT_IMPLEMENTED;
	}

	needs_palette = FALSE;
	ReadFunc = NULL;
	switch (tga.imagetype & ~TGA_IMAGE_RLE) {

		case TGA_IMAGE_LUT:
			if (tga.depth == 8) {
				needs_palette = TRUE;
				ReadFunc = ReadLUT8;
			}
			break;

		case TGA_IMAGE_GREY:
			if (tga.depth == 8) {
				ReadFunc = ReadLUT8;
			}
			break;

		case TGA_IMAGE_RGB:
			switch (tga.depth) {

				case 15:
					ReadFunc = ReadRGB15;
					break;

				case 16:
					ReadFunc = ReadRGBA16;
					break;

				case 24:
				case 32:
					ReadFunc = ReadTruecolor;
					break;

			}
			break;

	}

	if (!ReadFunc) {
		error = DTERROR_UNKNOWN_COMPRESSION;
		goto out;
	}

	if (tga.idsize) {
		if (Seek(file, tga.idsize, OFFSET_CURRENT) == -1 && (error = IoErr())) {
			goto out;
		}
	}

	if (tga.cmaptype > 1) {
		error = NOTOK;
		goto out;
	}
	if (needs_palette) {
		struct ColorRegister *cmap = NULL;
		ULONG *cregs = NULL;
		LONG (*CMAPFunc) (struct Library *libBase, BPTR file, struct ColorRegister *cmap,
			LONG len);

		if (tga.cmaptype == TGA_CMAP_NONE) {
			error = NOTOK;
			goto out;
		}

		CMAPFunc = NULL;
		switch (tga.cmapdepth) {
			case 24:
				CMAPFunc = GetCMAP24;
				break;
		}
		if (!CMAPFunc) {
			error = DTERROR_UNKNOWN_COMPRESSION;
			goto out;
		}

		SetDTAttrs(o, NULL, NULL,
			PDTA_NumColors, 256,
			TAG_END);

		if (GetDTAttrs(o, PDTA_ColorRegisters, &cmap, PDTA_CRegs, &cregs, TAG_END) == 2)
		{
			int start, len, i;

			start = tga.cmapstart;
			len = tga.cmaplength;

			error = CMAPFunc(libBase, file, cmap+=start, len);
			if (error) goto out;

			cregs+=start;
			for (i = 0; i < len; i++) {
				*cregs++ = RGB8to32(cmap->red);
				*cregs++ = RGB8to32(cmap->green);
				*cregs++ = RGB8to32(cmap->blue);
				cmap++;
			}
		}
	} else {
		ULONG len;
		len = tga.cmaplength * ((tga.cmapdepth+7)>>3);
		if (Seek(file, len, OFFSET_CURRENT) == -1 && (error = IoErr())) {
			goto out;
		}
	}

	error = ReadFunc(cl, o, file, bmh, &tga);

out:
	return error;
}

LONG ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	ULONG bpr, fmt;
	UBYTE *buf;
	LONG status, level = 0, error = OK;
	int y;
	
	bpr = bmh->bmh_Width;
	fmt = ((tga->imagetype & ~TGA_IMAGE_RLE) == TGA_IMAGE_GREY) ? PBPAFMT_GREY8 : PBPAFMT_LUT8;
	
	buf = AllocMem(bpr, /* MEMF_SHARED */ MEMF_ANY);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}
	
	SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_SourceMode,	PMODE_V43,
		DTA_ErrorLevel,		&level,
		DTA_ErrorNumber,	&error,
		TAG_END);
	if (level)
		goto out;
	else
		error = OK;

	for (y = bmh->bmh_Height-1; y >= 0; y--) {
		status = ReadPixels(libBase, tga, file, buf, bpr, 1);
		if (status != bmh->bmh_Width) {
			error = IoErr();
			goto out;
		}
		
		DoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf, fmt,
			bpr, 0, y, bpr, 1);
	}

out:
	FreeMem(buf, bpr);
	return error;
}

LONG ReadRGB15 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	return ERROR_NOT_IMPLEMENTED;
}

LONG ReadRGBA16 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	return ERROR_NOT_IMPLEMENTED;
}

LONG ReadTruecolor (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	ULONG bpp, bpr, fmt;
	UBYTE *buf1, *buf2, *src, *dst;
	LONG status;
	IPTR level = 0, error = OK;
	int y, x;
	
	switch (tga->depth) {
		case 24:
			bpp = 3;
			fmt = PBPAFMT_RGB;
			break;
		case 32:
			bpp = 4;
			fmt = PBPAFMT_ARGB;
			break;
		default:
			return ERROR_NOT_IMPLEMENTED;
			break;
	}
	bpr = bmh->bmh_Width * bpp;
		
	buf1 = AllocMem(bpr, /* MEMF_SHARED */ MEMF_ANY);
	buf2 = AllocMem(bpr, /* MEMF_SHARED */ MEMF_ANY);
	if (!buf1 || !buf2) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}
	
	SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_SourceMode,	PMODE_V43,
		DTA_ErrorLevel,		&level,
		DTA_ErrorNumber,	&error,
		TAG_END);
	if (level)
		goto out;
	else
		error = OK;
	
	for (y = bmh->bmh_Height-1; y >= 0; y--) {
		status = ReadPixels(libBase, tga, file, buf1, bmh->bmh_Width, bpp);
		if (status != bmh->bmh_Width) {
			error = IoErr();
			goto out;
		}

		src = buf1;
		dst = buf2;
		if (bpp == 3) {
			for (x = 0; x < bmh->bmh_Width; x++) {
				*dst++ = src[2];
				*dst++ = src[1];
				*dst++ = src[0];
				src += 3;
			}
		} else {
			for (x = 0; x < bmh->bmh_Width; x++) {
				//write_le32(dst, *(ULONG *)src);
				// FIXME: do we need endiannes conversion
				*dst++ = src[3];
				*dst++ = src[2];
				*dst++ = src[1];
				*dst++ = src[0];
				src += 4;
			}
		}

		DoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf2, fmt,
			bpr, 0, y, bmh->bmh_Width, 1);
	}
	
out:
	if (buf1) FreeMem(buf1, bpr);
	if (buf2) FreeMem(buf2, bpr);
	return error;
}

LONG ReadPixels (struct Library *libBase, struct TargaHeader *tga, BPTR file, UBYTE *buf, LONG width, LONG bpp) {
	if (tga->imagetype & TGA_IMAGE_RLE) {
		//UBYTE *ptr = buf;
		LONG x = 0;
		LONG st, cnt, rem;
		UBYTE pix[4];
		while (x < width) {
			cnt = FGetC(file);
			if (cnt == -1) break;
			rem = width-x;
			if (cnt & 0x80) {
				cnt -= 0x80;
				if (++cnt > rem) cnt = rem;
				st = FRead(file, pix, bpp, 1);
				if (st != 1) break;
				x += cnt;
				switch (bpp) {
					case 1:
						while (cnt--) {
							*buf++ = pix[0];
						}
						break;
					case 2:
						while (cnt--) {
							*(UWORD *)buf = *(UWORD *)pix;
							buf += 2;
						}
						break;
					case 3:
						while (cnt--) {
							*buf++ = pix[0];
							*buf++ = pix[1];
							*buf++ = pix[2];
						}
						break;
					case 4:
						while (cnt--) {
							*(ULONG *)buf = *(ULONG *)pix;
							buf += 4;
						}
						break;
				}
			} else {
				if (++cnt > rem) cnt = rem;
				x += st = FRead(file, buf, bpp, cnt);
				if (st != cnt) break;
				buf += (cnt*bpp);
			}
		}
		return x;
	} else {
		return FRead(file, buf, bpp, width);
	}
}

LONG GetCMAP24 (struct Library *libBase, BPTR file, struct ColorRegister *cmap, LONG len) {
	UBYTE *buf, *ptr;
	LONG status, error = OK, size = len * 3;
	LONG i;

	buf = AllocMem(size, /* MEMF_SHARED */ MEMF_ANY);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}

	status = Read(file, buf, size);
	if (status != size) {
		error = ReadError(status);
		goto out;
	}

	ptr = buf;
	for (i = 0; i < len; i++) {
		cmap->blue = *ptr++;
		cmap->green = *ptr++;
		cmap->red = *ptr++;
		cmap++;
	}

out:
	FreeMem(buf, size);
	return error;
}

static LONG GetTarga (Class *cl, Object *o, struct TagItem *tags) {
	//struct Library *libBase = (struct Library *)cl->cl_UserData;
	struct BitMapHeader *bmh = NULL;
	char *filename;
	IPTR srctype;
	LONG error = ERROR_OBJECT_NOT_FOUND;
	BPTR file = BNULL;

	filename = (char *)GetTagData(DTA_Name, (IPTR)"Untitled", tags);

	GetDTAttrs(o,
		PDTA_BitMapHeader,	&bmh,
		DTA_Handle,			&file,
		DTA_SourceType,		&srctype,
		TAG_END);

	/* Do we have everything we need? */
	if (bmh && file && srctype == DTST_FILE) {
		error = ConvertTarga(cl, o, file, bmh);
		if (error == OK) {
			SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	FilePart(filename),
				TAG_END);
		}
	}
	return error;
}
