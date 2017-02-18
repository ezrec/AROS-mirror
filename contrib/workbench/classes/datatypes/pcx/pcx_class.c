/*
 * pcx.datatype
 * (c) Fredrik Wikstrom
 */

#include "pcx_class.h"

#define RGB8to32(RGB) (((ULONG)(RGB) << 24)|((ULONG)(RGB) << 16)|((ULONG)(RGB) << 8)|((ULONG)(RGB)))

static LONG WritePCX (Class *cl, Object *o, struct dtWrite *msg);
static LONG ConvertPCX (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
static LONG GetPCX (Class *cl, Object *o, struct TagItem *tags);

IPTR PCX__OM_NEW(Class *cl, Object *o, struct opSet *msg)
{
	IPTR ret;
	ret = DoSuperMethodA(cl, o, msg);
	if (ret) {
		LONG error;
		error = GetPCX(cl, (Object *)ret, msg->ops_AttrList);
		if (error != OK) {
			CoerceMethod(cl, (Object *)ret, OM_DISPOSE);
			ret = 0;
			SetIoErr(error);
		}
	}
	return ret;
}

IPTR PCX__DTM_WRITE(Class *cl, Object *o, struct dtWrite *msg)
{
	IPTR ret;
	/* check if dt's native format should be used */
	if (((struct dtWrite *)msg)->dtw_Mode == DTWM_RAW) {
		LONG error;
		error = WritePCX(cl, o, msg);
		if (error == OK) {
			ret = TRUE;
		} else {
			SetIoErr(error);
			ret = FALSE;
		}
	}
	return ret;
}


static LONG WritePCX (Class *cl, Object *o, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}

static LONG ReadPlanar (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
static LONG ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
static LONG ReadRGB24 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
static LONG ReadRLE (struct Library *libBase, BPTR file, UBYTE *buf, LONG len);

LONG GetCMAP16 (struct Library *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, LONG numcolors);
LONG GetCMAP256 (struct Library *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, LONG numcolors);

static LONG ConvertPCX (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	struct PCXHeader *pcx;
	struct PCXHeaderFile *pcxfile;
	LONG error = OK, status, i;
	LONG (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
	LONG (*CMAPFunc) (struct Library *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, LONG numcolors);

	pcx = AllocMem(sizeof(struct PCXHeader), /* MEMF_SHARED */ MEMF_ANY);
	if (!pcx) {
		error = ERROR_NO_FREE_STORE;
		return error;
	}

	pcxfile = AllocMem(sizeof(struct PCXHeaderFile), /* MEMF_SHARED */ MEMF_ANY);
	if (!pcxfile) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	/* read header */
	status = Read(file, pcxfile, sizeof(struct PCXHeaderFile));
	if (status != sizeof(struct PCXHeaderFile)) {
		error = ReadError(status);
		goto out;
	}
	/* skip unused data in header */
	status = Seek(file, PCX_HEADER_SIZE-sizeof(struct PCXHeaderFile), OFFSET_CURRENT);
	if (status == -1) {
		error = IoErr();
		if (error != OK) goto out;
	}

	if (pcxfile->Manufacturer != PCX_MAN_ZSOFT) {
		error = ERROR_OBJECT_WRONG_TYPE;
		goto out;
	}
	if (pcxfile->Compression != PCX_COMP_RLE) {
		error = DTERROR_UNKNOWN_COMPRESSION;
		goto out;
	}

	pcx->Manufacturer = pcxfile->Manufacturer;
	pcx->Version = pcxfile->Version;
	pcx->Compression = pcxfile->Compression;
	pcx->BitsPerPixel = pcxfile->BitsPerPixel;
	pcx->Window.XMin = AROS_LE2WORD(pcxfile->Window.XMin);
	pcx->Window.YMin = AROS_LE2WORD(pcxfile->Window.YMin);
	pcx->Window.XMax = AROS_LE2WORD(pcxfile->Window.XMax);
	pcx->Window.YMax = AROS_LE2WORD(pcxfile->Window.YMax);
	pcx->DPI.X = AROS_LE2WORD(pcxfile->DPI.X);
	pcx->DPI.Y = AROS_LE2WORD(pcxfile->DPI.Y);
	for (i = 0; i < 16 ; i++) {
		pcx->ColorMap[i].red = pcxfile->ColorMap[i].red;
		pcx->ColorMap[i].green = pcxfile->ColorMap[i].green;
		pcx->ColorMap[i].blue = pcxfile->ColorMap[i].blue;
	}
	pcx->Reserved = pcxfile->Reserved;
	pcx->NPlanes = pcxfile->NPlanes;
	pcx->BytesPerRow = AROS_LE2WORD(pcxfile->BytesPerRow);
	pcx->PaletteType = AROS_LE2WORD(pcxfile->PaletteType);

	bmh->bmh_Width = pcx->Window.XMax-pcx->Window.XMin+1;
	bmh->bmh_Height = pcx->Window.YMax-pcx->Window.YMin+1;
	bmh->bmh_Depth = pcx->BitsPerPixel*pcx->NPlanes;

	ReadFunc = NULL;
	switch (pcx->BitsPerPixel) {
		case 1:
		case 2:
		case 4:
			ReadFunc = ReadPlanar;
			break;
		case 8:
			switch (pcx->NPlanes) {
				case 1:
					ReadFunc = ReadLUT8;
					break;
				case 3:
					ReadFunc = ReadRGB24;
					break;
			}
			break;
	}

	CMAPFunc = NULL;
	if (bmh->bmh_Depth <= 4)
		CMAPFunc = GetCMAP16;
	else if (bmh->bmh_Depth <= 8)
		CMAPFunc = GetCMAP256;

	if (!ReadFunc) {
		error = ERROR_NOT_IMPLEMENTED;
		goto out;
	}

	error = ReadFunc(cl, o, file, bmh, pcx);
	if (error != OK) goto out;
	if (CMAPFunc) {
		struct ColorRegister *cmap = NULL;
		ULONG *cregs = NULL;
		LONG numcolors;

		numcolors = 1 << bmh->bmh_Depth;
		SetDTAttrs(o, NULL, NULL,
			PDTA_NumColors,	numcolors,
			TAG_END);
		if (GetDTAttrs(o, PDTA_ColorRegisters, &cmap, PDTA_CRegs, &cregs, TAG_END) == 2) {
			error = CMAPFunc(libBase, file, pcx, cmap, numcolors);
			if (error == OK) {
				int i;
				for (i = 0; i < numcolors; i++) {
					*cregs++ = RGB8to32(cmap->red);
					*cregs++ = RGB8to32(cmap->green);
					*cregs++ = RGB8to32(cmap->blue);
					cmap++;
				}
			}
		} else {
			error = 1;
		}
	}

out:
	FreeMem(pcx, sizeof(struct PCXHeader));
	FreeMem(pcxfile, sizeof(struct PCXHeaderFile));
	return error;
}

static LONG GetPCX (Class *cl, Object *o, struct TagItem *tags) {
	//struct Library *libBase = (struct ClassBase *)cl->cl_UserData;
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
		error = ConvertPCX(cl, o, file, bmh);
		if (error == OK) {
			SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	FilePart(filename),
				TAG_END);
		}
	}
	return error;
}

LONG ReadPlanar (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	UBYTE *buf, *src;
	struct BitMap *bm;
	ULONG bpr, len, buf_size, offset;
	LONG status, error = OK;
	int y, pl, x, sh_l, sh_r;
	ULONG bits;
	UBYTE mask, pixel;

	bits = pcx->BitsPerPixel;
	bpr = pcx->BytesPerRow;
	buf_size = bpr*pcx->NPlanes;
	buf = AllocMem(buf_size, MEMF_PRIVATE);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}

	len = (bmh->bmh_Width+7) >> 3;
	bm = AllocBitMap(bmh->bmh_Width, bmh->bmh_Height, bmh->bmh_Depth,
		BMF_CLEAR|BMF_DISPLAYABLE, NULL);
	if (!bm) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_BitMap,		bm,
		TAG_END);

	offset = 0;
	for (y = 0; y < bmh->bmh_Height; y++) {
		status = ReadRLE(libBase, file, buf, buf_size);
		if (status != buf_size) {
			error = IoErr();
			goto out;
		}
		src = buf;
		switch (bits) {
			case 1:
				for (pl = 0; pl < bmh->bmh_Depth; pl++) {
					CopyMem(src, &bm->Planes[pl][offset], len);
					src += bpr;
				}
				break;
			default:
				sh_r = 8 - bits;
				mask = (1 << bits) - 1;
				for (x = 0; x < bmh->bmh_Width; x++) {
					pixel = 0;
					for (pl = sh_l = 0; pl < pcx->NPlanes; pl++, sh_l+=bits) {
						pixel |= ((src[pl*bpr] >> sh_r) & mask) << sh_l;
					}

					for (pl = 0; pl < bmh->bmh_Depth; pl++) {
						if (pixel & (1 << pl))
							bm->Planes[pl][offset+(x >> 3)] |= (UBYTE)0x80 >> (x & 7);
					}

					sh_r -= bits;
					if (sh_r < 0) {
						src++;
						sh_r = 8 - bits;
					}
				}
				break;
		}
		offset += bm->BytesPerRow;
	}

out:
	FreeMem(buf, buf_size);
	return error;
}

LONG ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	ULONG bpr;
	UBYTE *buf;
	LONG status, level = 0, error = OK;
	int y;
	
	bpr = pcx->BytesPerRow;
	buf = AllocMem(bpr, MEMF_PRIVATE);
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
	if (level) {
		goto out;
	}

	for (y = 0; y < bmh->bmh_Height; y++) {
		status = ReadRLE(libBase, file, buf, bpr);
		if (status != bpr) {
			error = IoErr();
			break;
		}
		DoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf, PBPAFMT_LUT8,
			bpr, 0, y, bmh->bmh_Width, 1);
	}

out:
	FreeMem(buf, bpr);
	return error;
}

LONG ReadRGB24 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx) {
	struct Library *libBase = (struct Library *)cl->cl_UserData;
	ULONG bpr;
	UBYTE *buf1, *buf2, *src[3], *dst;
	LONG buf_size;
	LONG level = 0, error = OK;
	int y, x;

	bpr = pcx->BytesPerRow;
	//mod = bpr-bmh->bmh_Width;
	buf_size = 3*pcx->BytesPerRow;
	buf1 = AllocMem(buf_size, MEMF_PRIVATE);
	buf2 = AllocMem(buf_size, MEMF_PRIVATE);
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
	if (level) {
		goto out;
	}
	
	for (y = 0; y < bmh->bmh_Height; y++) {
		ReadRLE(libBase, file, buf1, buf_size);

		src[0] = buf1;
		src[1] = src[0]+bpr;
		src[2] = src[1]+bpr;
		dst = buf2;

		for (x = 0; x < bmh->bmh_Width; x++) {
			*dst++ = *src[0]++;
			*dst++ = *src[1]++;
			*dst++ = *src[2]++;
		}

		DoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf2, PBPAFMT_RGB,
			buf_size, 0, y, bmh->bmh_Width, 1);
	}

out:
	if (buf1) FreeMem(buf1, buf_size);
	if (buf2) FreeMem(buf2, buf_size);
	return error;
}

static LONG ReadRLE (struct Library *libBase, BPTR file, UBYTE *buf, LONG len) {
	int dec = 0;
	int cnt, val;
	while (dec < len) {
		cnt = FGetC(file);
		if (cnt == -1) break;
		if (cnt >= 0xC0) {
			val = FGetC(file);
			if (val == -1) break;
			cnt -= 0xC0;
			while (cnt-- && dec++ < len) {
				*buf++ = val;
			}
		} else {
			*buf++ = cnt;
			dec++;
		}
	}
	return dec;
}

LONG GetCMAP16 (struct Library *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, LONG numcolors) {
	if (numcolors == 2) {
		cmap[0].red = cmap[0].green = cmap[0].blue = 0;
		cmap[1].red = cmap[1].green = cmap[1].blue = 255;
	} else if (numcolors > 16) {
		CopyMem(pcx->ColorMap, cmap, 16*sizeof(struct PCXColor));
		numcolors -= 16; cmap += 16;
		memset(cmap, 0, numcolors*sizeof(struct PCXColor));
	} else {
		CopyMem(pcx->ColorMap, cmap, numcolors*sizeof(struct PCXColor));
	}
	return OK;
}

LONG GetCMAP256 (struct Library *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, LONG numcolors) {
	LONG status, error = OK;
	//int i;

	status = FGetC(file);
	if (status == -1) {
		return IoErr();
	} else if (status != 12) {
		return NOTOK;
	}

	status = FRead(file, cmap, sizeof(struct PCXColor), numcolors);
	if (status != numcolors) {
		error = ReadError(status);
	}

	return error;
}
