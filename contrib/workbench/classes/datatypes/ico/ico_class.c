/*
 * ico.datatype
 * (c) Fredrik Wikstrom
 */

#include "ico_class.h"
#include "endian.h"

#define RGB8to32(RGB) ((uint32)(RGB) * 0x01010101UL)

uint32 ClassDispatch (Class *cl, Object *o, Msg msg);

Class *initDTClass (struct ClassBase *libBase) {
	Class *cl;
	SuperClassLib = IExec->OpenLibrary("datatypes/picture.datatype", 52);
	if (SuperClassLib) {
		cl = IIntuition->MakeClass(libBase->libNode.lib_Node.ln_Name, PICTUREDTCLASS, NULL, 0, 0);
		if (cl) {
			cl->cl_Dispatcher.h_Entry = (HOOKFUNC)ClassDispatch;
			cl->cl_UserData = (uint32)libBase;
			IIntuition->AddClass(cl);
		} else {
			IExec->CloseLibrary(SuperClassLib);
		}
	}
	return cl;
}

BOOL freeDTClass (struct ClassBase *libBase, Class *cl) {
	BOOL res = TRUE;
	if (cl) {
		res = IIntuition->FreeClass(cl);
		if (res) {
			IExec->CloseLibrary(SuperClassLib);
		}
	}
	return res;
}

static int32 WriteICO (Class *cl, Object *o, struct dtWrite *msg);
static int32 ConvertICO (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, uint32 index, uint32 *total);
static int32 GetICO (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetICO(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)ret, OM_DISPOSE);
					ret = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

#if 0
		case DTM_WRITE:
			/* check if dt's native format should be used */
			if (((struct dtWrite *)msg)->dtw_Mode == DTWM_RAW) {
				int32 error;
				error = WriteICO(cl, o, (struct dtWrite *)msg);
				if (error == OK) {
					ret = TRUE;
				} else {
					IDOS->SetIoErr(error);
					ret = FALSE;
				}
				break;
			}
			/* fall through and let superclass deal with it */
#endif

		default:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			break;

	}

	return ret;
}

static int32 WriteICO (Class *cl, Object *o, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}

int32 ReadBitPacked (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
int32 ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
int32 ReadTruecolor (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);

static int32 ConvertICO (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, uint32 index, uint32 *total) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	int32 status, error = OK;
	uint32 fileoffs;
	int32 numcolors;
	int32 (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);

	{
		struct ICO_FileHeader ico;
		uint32 num;

		status = IDOS->Read(file, &ico, sizeof(struct ICO_FileHeader));

		if (status != sizeof(struct ICO_FileHeader)) {
			return ReadError(status);
		}

		if (ico.Reserved != 0000 || ico.Type != 0x0100 || ico.Count == 0x0000) {
			return ERROR_OBJECT_WRONG_TYPE;
		}

		num = read_le16(&ico.Count);

		if (total) *total = num;

		if (index >= num) {
			return DTERROR_NOT_AVAILABLE;
		}
	}

	status = IDOS->Seek(file, index*sizeof(struct ICO_Entry), OFFSET_CURRENT);
	if (status == -1 && (error = IDOS->IoErr()) != OK) {
		return error;
	}

	{
		struct ICO_Entry entry;

		status = IDOS->Read(file, &entry, sizeof(struct ICO_Entry));
		if (status != sizeof(struct ICO_Entry)) {
			return ReadError(status);
		}

		fileoffs = read_le32(&entry.FileOffset);
	}

	status = IDOS->Seek(file, fileoffs, OFFSET_BEGINNING);
	if (status == -1 && (error = IDOS->IoErr()) != OK) {
		return error;
	}

	{
		struct ICO_InfoHeader info;

		status = IDOS->Read(file, &info, sizeof(struct ICO_InfoHeader));
		if (status != sizeof(struct ICO_InfoHeader)) {
			return ReadError(status);
		}

		if (info.CompType != ICO_COMP_NONE) {
			return DTERROR_UNKNOWN_COMPRESSION;
		}
		if (read_le16(&info.Planes) != 1) {
			return ERROR_NOT_IMPLEMENTED;
		}

		bmh->bmh_Width = read_le32(&info.Width);
		bmh->bmh_Height = read_le32(&info.Height) >> 1;
		bmh->bmh_Depth = read_le16(&info.BitDepth);

		ReadFunc = NULL;
		switch (bmh->bmh_Depth) {

			case 1:
			case 4:
				ReadFunc = ReadBitPacked;
				break;

			case 8:
				ReadFunc = ReadLUT8;
				break;

			case 32:
				bmh->bmh_Masking = mskHasAlpha;
			case 24:
				ReadFunc = ReadTruecolor;
				break;

		}
		if (!ReadFunc) {
			return ERROR_NOT_IMPLEMENTED;
		}
	}

	numcolors = (bmh->bmh_Depth <= 8) ? (1 << bmh->bmh_Depth) : 0;
	if (numcolors) {
		struct ICO_Color *ico_cmap, *src;
		struct ColorRegister *cmap = NULL;
		uint32 *cregs = NULL;		

		IDataTypes->SetDTAttrs(o, NULL, NULL,
			PDTA_NumColors,	numcolors,
			TAG_END);
		if (IDataTypes->GetDTAttrs(o,
			PDTA_ColorRegisters,	&cmap,
			PDTA_CRegs,				&cregs,
			TAG_END) == 2)
		{
			int i;
			uint32 cmap_size = numcolors << 2;

			ico_cmap = src = IExec->AllocMem(cmap_size, MEMF_ANY);
			if (!ico_cmap) {
				return ERROR_NO_FREE_STORE;
			}

			status = IDOS->Read(file, ico_cmap, cmap_size);
			if (status != cmap_size) {
				IExec->FreeMem(ico_cmap, cmap_size);
				return ReadError(status);
			}

			for (i = 0; i < numcolors; i++) {
				cmap->red = src->red;
				cmap->green = src->green;
				cmap->blue = src->blue;
				*cregs++ = RGB8to32(cmap->red);
				*cregs++ = RGB8to32(cmap->green);
				*cregs++ = RGB8to32(cmap->blue);
				cmap++; src++;
			}

			IExec->FreeMem(ico_cmap, cmap_size);
		}
	}

	error =  ReadFunc(cl, o, file, bmh);

	/* read mask */
	if (error == OK && numcolors) {
		uint32 bpr, len;
		uint8 *mask, *buf;
		int y;

		mask = IGraphics->AllocRaster(bmh->bmh_Width, bmh->bmh_Height);
		if (!mask) {
			return ERROR_NO_FREE_STORE;
		}

		bpr = ((bmh->bmh_Width + 31) >> 3) & ~3;
		len = ((bmh->bmh_Width + 15) >> 3) & ~1;
		buf = IExec->AllocMem(bpr, MEMF_ANY);
		if (!buf) {
			IGraphics->FreeRaster(mask, bmh->bmh_Width, bmh->bmh_Height);
			return ERROR_NO_FREE_STORE;
		}

		mask += len*(bmh->bmh_Height-1);
		for (y = bmh->bmh_Height-1; y >= 0; y--) {
			status = IDOS->Read(file, buf, bpr);
			if (status != bpr) {
				error = ReadError(status);
				break;
			}
			IExec->CopyMem(buf, mask, len);
			mask -= len;
		}

		IDataTypes->SetDTAttrs(o, NULL, NULL,
			PDTA_MaskPlane,	mask,
			TAG_END);
		bmh->bmh_Masking = mskHasMask;
	}

	return error;
}

int32 ReadBitPacked (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bits, bpr;
	uint8 *buf;
	int32 status, error = OK;
	struct BitMap *bm;

	bits = bmh->bmh_Depth;
	bpr = (((bmh->bmh_Width * bits) + 31) >> 3) & ~3;
	buf = IExec->AllocMem(bpr, MEMF_ANY);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}

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

	if (bits == 1) {
		uint32 len;
		uint8 *dst;
		int y;

		len = (bmh->bmh_Width + 7) >> 3;
		dst = bm->Planes[0] + bm->BytesPerRow*(bm->Rows-1);
		for (y = 0; y < bmh->bmh_Height; y++) {
			status = IDOS->Read(file, buf, bpr);
			if (status != bpr) {
				error = ReadError(status);
				break;
			}
			IExec->CopyMem(buf, dst, len);
			dst -= bm->BytesPerRow;
		}
	} else {
		uint8 *src;
		uint32 offs;
		uint8 pix;
		int y, x;

		offs = bm->BytesPerRow*(bm->Rows-1);
		for (y = 0; y < bmh->bmh_Height; y++) {
			status = IDOS->Read(file, buf, bpr);
			if (status != bpr) {
				error = ReadError(status);
				break;
			}
			src = buf;
			for (x = 0; x < bmh->bmh_Width; x++) {
				if (x & 1)
					pix = *src++ & 0xF;
				else
					pix = *src >> 4;

				if (pix & 1) bm->Planes[0][offs+(x >> 3)] |= (uint8)0x80 >> (x & 7);
				if (pix & 2) bm->Planes[1][offs+(x >> 3)] |= (uint8)0x80 >> (x & 7);
				if (pix & 4) bm->Planes[2][offs+(x >> 3)] |= (uint8)0x80 >> (x & 7);
				if (pix & 8) bm->Planes[3][offs+(x >> 3)] |= (uint8)0x80 >> (x & 7);
			}
			offs -= bm->BytesPerRow;
		}
	}

out:
	IExec->FreeMem(buf, bpr);
	return error;
}

int32 ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bpr;
	uint8 *buf;
	int32 status, level = 0, error = OK;
	int y;

	bpr = (bmh->bmh_Width + 3) & ~3;
	buf = IExec->AllocMem(bpr, MEMF_ANY);
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

	for (y = bmh->bmh_Height-1; y >= 0; y--) {
		status = IDOS->Read(file, buf, bpr);
		if (status != bpr) {
			error = ReadError(status);
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

int32 ReadTruecolor (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bits, bpp, bpr;
	uint8 *buf, *ptr;
	int32 status, level = 0, error = OK;
	int y, x;

	bits = bmh->bmh_Depth;
	if (bits == 24)
		bpp = 3;
	else
		bpp = 4;
	bpr = ((bmh->bmh_Width*bpp) + 3) & ~3;
	buf = IExec->AllocMem(bpr, MEMF_ANY);
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

	if (bpp == 3) {
		uint8 tmp;
		for (y = bmh->bmh_Height-1; y >= 0; y--) {
			status = IDOS->Read(file, buf, bpr);
			if (status != bpr) {
				error = ReadError(status);
				break;
			}

			ptr = buf;
			for (x = 0; x < bmh->bmh_Width; x++) {
				tmp = ptr[0];
				ptr[0] = ptr[2];
				ptr[2] = tmp;
				ptr += 3;
			}

			IIntuition->IDoSuperMethod(cl, o,
				PDTM_WRITEPIXELARRAY, buf, PBPAFMT_RGB,
				bpr, 0, y, bmh->bmh_Width, 1);
		}
	} else {
		for (y = bmh->bmh_Height-1; y >= 0; y--) {
			status = IDOS->Read(file, buf, bpr);
			if (status != bpr) {
				error = ReadError(status);
				break;
			}

			ptr = buf;
			for (x = 0; x < bmh->bmh_Width; x++) {
				write_le32(ptr, *(uint32 *)ptr);
				ptr += 4;
			}

			IIntuition->IDoSuperMethod(cl, o,
				PDTM_WRITEPIXELARRAY, buf, PBPAFMT_ARGB,
				bpr, 0, y, bmh->bmh_Width, 1);
		}
	}

out:
	IExec->FreeMem(buf, bpr);
	return error;
}

static int32 GetICO (Class *cl, Object *o, struct TagItem *tags) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct BitMapHeader *bmh = NULL;
	char *filename;
	int32 srctype;
	int32 error = ERROR_REQUIRED_ARG_MISSING;
	BPTR file = (BPTR)NULL;
	uint32 whichpic, *numpics;

	filename = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", tags);
	whichpic = IUtility->GetTagData(PDTA_WhichPicture, 0, tags);
	numpics = (uint32 *)IUtility->GetTagData(PDTA_GetNumPictures, (uint32)NULL, tags);

	IDataTypes->GetDTAttrs(o,
		PDTA_BitMapHeader,	&bmh,
		DTA_Handle,			&file,
		DTA_SourceType,		&srctype,
		TAG_END);

	/* Do we have everything we need? */
	if (bmh && file && srctype == DTST_FILE) {
		error = ConvertICO(cl, o, file, bmh, whichpic, numpics);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	} else
		if (srctype != DTST_FILE) error = ERROR_NOT_IMPLEMENTED;

	return error;
}
