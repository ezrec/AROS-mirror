/*
 * pcx.datatype
 * (c) Fredrik Wikstrom
 */

#include "pcx_class.h"
#include "endian.h"

#define RGB8to32(RGB) (((uint32)(RGB) << 24)|((uint32)(RGB) << 16)|((uint32)(RGB) << 8)|((uint32)(RGB)))

uint32 ClassDispatch (Class *cl, Object *o, Msg msg);

Class *initDTClass (struct ClassBase *libBase) {
	Class *cl = NULL;
	PictureDTLib = IExec->OpenLibrary("datatypes/picture.datatype", 51);
	if (PictureDTLib) {
		cl = IIntuition->MakeClass(libBase->libNode.lib_Node.ln_Name, PICTUREDTCLASS, NULL, 0, 0);
		if (cl) {
			cl->cl_Dispatcher.h_Entry = (HOOKFUNC)ClassDispatch;
			cl->cl_UserData = (uint32)libBase;
			IIntuition->AddClass(cl);
		} else {
			IExec->CloseLibrary(PictureDTLib);
		}
	}
	return cl;
}

BOOL freeDTClass (struct ClassBase *libBase, Class *cl) {
	BOOL res = TRUE;
	if (cl) {
		res = IIntuition->FreeClass(cl);
		if (res) {
			IExec->CloseLibrary(PictureDTLib);
		}
	}
	return res;
}

static int32 WritePCX (Class *cl, Object *o, struct dtWrite *msg);
static int32 ConvertPCX (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
static int32 GetPCX (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetPCX(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)ret, OM_DISPOSE);
					ret = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

		case DTM_WRITE:
			/* check if dt's native format should be used */
			if (((struct dtWrite *)msg)->dtw_Mode == DTWM_RAW) {
				int32 error;
				error = WritePCX(cl, o, (struct dtWrite *)msg);
				if (error == OK) {
					ret = TRUE;
				} else {
					IDOS->SetIoErr(error);
					ret = FALSE;
				}
				break;
			}
			/* fall through and let superclass deal with it */

		default:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			break;

	}

	return ret;
}

static int32 WritePCX (Class *cl, Object *o, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}

int32 ReadPlanar (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
int32 ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
int32 ReadRGB24 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
static int32 ReadRLE (struct ClassBase *libBase, BPTR file, uint8 *buf, int32 len);

int32 GetCMAP16 (struct ClassBase *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, int32 numcolors);
int32 GetCMAP256 (struct ClassBase *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, int32 numcolors);

static int32 ConvertPCX (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct PCXHeader *pcx;
	int32 error = OK, status;
	int32 (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx);
	int32 (*CMAPFunc) (struct ClassBase *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, int32 numcolors);

	pcx = IExec->AllocMem(sizeof(struct PCXHeader), MEMF_SHARED);
	if (!pcx) {
		error = ERROR_NO_FREE_STORE;
		return error;
	}

	/* read header */
	status = IDOS->Read(file, pcx, sizeof(struct PCXHeader));
	if (status != sizeof(struct PCXHeader)) {
		error = ReadError(status);
		goto out;
	}
	/* skip unused data in header */
	status = IDOS->Seek(file, PCX_HEADER_SIZE-sizeof(struct PCXHeader), OFFSET_CURRENT);
	if (status == -1) {
		error = IDOS->IoErr();
		if (error != OK) goto out;
	}

	if (pcx->Manufacturer != PCX_MAN_ZSOFT) {
		error = ERROR_OBJECT_WRONG_TYPE;
		goto out;
	}
	if (pcx->Compression != PCX_COMP_RLE) {
		error = DTERROR_UNKNOWN_COMPRESSION;
		goto out;
	}

	pcx->Window.XMin = read_le16(&pcx->Window.XMin);
	pcx->Window.YMin = read_le16(&pcx->Window.YMin);
	pcx->Window.XMax = read_le16(&pcx->Window.XMax);
	pcx->Window.YMax = read_le16(&pcx->Window.YMax);
	pcx->BytesPerRow = read_le16(&pcx->BytesPerRow);

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
		uint32 *cregs = NULL;
		int32 numcolors;

		numcolors = 1 << bmh->bmh_Depth;
		IDataTypes->SetDTAttrs(o, NULL, NULL,
			PDTA_NumColors,	numcolors,
			TAG_END);
		if (IDataTypes->GetDTAttrs(o, PDTA_ColorRegisters, &cmap, PDTA_CRegs, &cregs, TAG_END) == 2) {
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
	IExec->FreeMem(pcx, sizeof(struct PCXHeader));
	return error;
}

static int32 GetPCX (Class *cl, Object *o, struct TagItem *tags) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct BitMapHeader *bmh = NULL;
	char *filename;
	int32 srctype;
	int32 error = ERROR_OBJECT_NOT_FOUND;
	BPTR file = (BPTR)NULL;

	filename = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", tags);

	IDataTypes->GetDTAttrs(o,
		PDTA_BitMapHeader,	&bmh,
		DTA_Handle,			&file,
		DTA_SourceType,		&srctype,
		TAG_END);

	/* Do we have everything we need? */
	if (bmh && file && srctype == DTST_FILE) {
		error = ConvertPCX(cl, o, file, bmh);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	}
	return error;
}

int32 ReadPlanar (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint8 *buf, *src;
	struct BitMap *bm;
	uint32 bpr, len, buf_size, offset;
	int32 status, error = OK;
	int y, pl, x, sh_l, sh_r;
	uint32 bits;
	uint8 mask, pixel;

	bits = pcx->BitsPerPixel;
	bpr = pcx->BytesPerRow;
	buf_size = bpr*pcx->NPlanes;
	buf = IExec->AllocMem(buf_size, MEMF_PRIVATE);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}

	len = (bmh->bmh_Width+7) >> 3;
	bm = IGraphics->AllocBitMap(bmh->bmh_Width, bmh->bmh_Height, bmh->bmh_Depth,
		BMF_CLEAR|BMF_DISPLAYABLE, NULL);
	if (!bm) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_BitMap,		bm,
		TAG_END);

	offset = 0;
	for (y = 0; y < bmh->bmh_Height; y++) {
		status = ReadRLE(libBase, file, buf, buf_size);
		if (status != buf_size) {
			error = IDOS->IoErr();
			goto out;
		}
		src = buf;
		switch (bits) {
			case 1:
				for (pl = 0; pl < bmh->bmh_Depth; pl++) {
					IExec->CopyMem(src, &bm->Planes[pl][offset], len);
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
							bm->Planes[pl][offset+(x >> 3)] |= (uint8)0x80 >> (x & 7);
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
	IExec->FreeMem(buf, buf_size);
	return error;
}

int32 ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bpr;
	uint8 *buf;
	int32 status, level = 0, error = OK;
	int y;
	
	bpr = pcx->BytesPerRow;
	buf = IExec->AllocMem(bpr, MEMF_PRIVATE);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
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
			error = IDOS->IoErr();
			break;
		}
		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf, PBPAFMT_LUT8,
			bpr, 0, y, bmh->bmh_Width, 1);
	}

out:
	IExec->FreeMem(buf, bpr);
	return error;
}

int32 ReadRGB24 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct PCXHeader *pcx) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bpr, mod;
	uint8 *buf1, *buf2, *src[3], *dst;
	int32 buf_size;
	int32 status, level = 0, error = OK;
	int y, x;

	bpr = pcx->BytesPerRow;
	mod = bpr-bmh->bmh_Width;
	buf_size = 3*pcx->BytesPerRow;
	buf1 = IExec->AllocMem(buf_size, MEMF_PRIVATE);
	buf2 = IExec->AllocMem(buf_size, MEMF_PRIVATE);
	if (!buf1 || !buf2) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
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
		status = ReadRLE(libBase, file, buf1, buf_size);

		src[0] = buf1;
		src[1] = src[0]+bpr;
		src[2] = src[1]+bpr;
		dst = buf2;

		for (x = 0; x < bmh->bmh_Width; x++) {
			*dst++ = *src[0]++;
			*dst++ = *src[1]++;
			*dst++ = *src[2]++;
		}

		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf2, PBPAFMT_RGB,
			buf_size, 0, y, bmh->bmh_Width, 1);
	}

out:
	if (buf1) IExec->FreeMem(buf1, buf_size);
	if (buf2) IExec->FreeMem(buf2, buf_size);
	return error;
}

static int32 ReadRLE (struct ClassBase *libBase, BPTR file, uint8 *buf, int32 len) {
	int dec = 0;
	int cnt, val;
	while (dec < len) {
		cnt = IDOS->FGetC(file);
		if (cnt == -1) break;
		if (cnt >= 0xC0) {
			val = IDOS->FGetC(file);
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

int32 GetCMAP16 (struct ClassBase *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, int32 numcolors) {
	if (numcolors == 2) {
		cmap[0].red = cmap[0].green = cmap[0].blue = 0;
		cmap[1].red = cmap[1].green = cmap[1].blue = 255;
	} else if (numcolors > 16) {
		IExec->CopyMem(pcx->ColorMap, cmap, 16*sizeof(struct PCXColor));
		numcolors -= 16; cmap += 16;
		IUtility->SetMem(cmap, 0, numcolors*sizeof(struct PCXColor));
	} else {
		IExec->CopyMem(pcx->ColorMap, cmap, numcolors*sizeof(struct PCXColor));
	}
	return OK;
}

int32 GetCMAP256 (struct ClassBase *libBase, BPTR file, struct PCXHeader *pcx, struct ColorRegister *cmap, int32 numcolors) {
	int32 status, error = OK;
	int i;

	status = IDOS->FGetC(file);
	if (status == -1) {
		return IDOS->IoErr();
	} else if (status != 12) {
		return NOTOK;
	}

	status = IDOS->FRead(file, cmap, sizeof(struct PCXColor), numcolors);
	if (status != numcolors) {
		error = ReadError(status);
	}

	return error;
}
