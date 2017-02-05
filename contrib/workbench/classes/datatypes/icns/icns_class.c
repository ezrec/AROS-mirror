/*
 * icns.datatype
 * (c) Fredrik Wikstrom
 */

#include "icns_class.h"
#include <string.h>

uint32 ClassDispatch (Class *cl, Object *o, Msg msg);

Class *initDTClass (struct ClassBase *libBase) {
	Class *cl = NULL;
	SuperClassBase = IExec->OpenLibrary("datatypes/picture.datatype", 52);
	if (SuperClassBase) {
		cl = IIntuition->MakeClass(libBase->libNode.lib_Node.ln_Name, PICTUREDTCLASS, NULL, 0, 0);
		if (cl) {
			cl->cl_Dispatcher.h_Entry = (HOOKFUNC)ClassDispatch;
			cl->cl_UserData = (uint32)libBase;
			IIntuition->AddClass(cl);
		} else {
			IExec->CloseLibrary(SuperClassBase);
		}
	}
	return cl;
}

BOOL freeDTClass (struct ClassBase *libBase, Class *cl) {
	BOOL res = TRUE;
	if (cl) {
		res = IIntuition->FreeClass(cl);
		if (res) {
			IExec->CloseLibrary(SuperClassBase);
		}
	}
	return res;
}

static int32 ConvertICNS (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	uint32 index, uint32 *total);
static int32 GetICNS (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetICNS(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
				if (error != OK) {
					IIntuition->ICoerceMethod(cl, (Object *)ret, OM_DISPOSE);
					ret = (uint32)NULL;
					IDOS->SetIoErr(error);
				}
			}
			break;

		default:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			break;

	}

	return ret;
}

static int32 ReadC1A1 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct IconInfo *info);
static int32 ReadCLUT4_8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct IconInfo *info);
static int32 ReadARGB32 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct IconInfo *info);

static int32 ConvertICNS (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	uint32 index, uint32 *total)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	int32 status, error = OK;
	struct ResourceHeader rsh;
	struct ElementHeader elem;
	struct IconInfo info;
	int32 (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
		struct IconInfo *info);

	status = IDOS->Read(file, &rsh, sizeof(struct ResourceHeader));
	if (status != sizeof(struct ResourceHeader)) {
		return ReadError(status);
	}

	if (rsh.ResourceType != RST_ICNS) {
		return ERROR_OBJECT_WRONG_TYPE;
	}

	if (total) {
		if (error = ParseElements(cl, file, &elem, &info, 0, 0, total)) return error;
		if (index >= *total) return DTERROR_NOT_AVAILABLE;
	}

	if (error = ParseElements(cl, file, &elem, &info, 0, index, NULL)) return error;

	bmh->bmh_Width = info.Width;
	bmh->bmh_Height = info.Height;
	bmh->bmh_Depth = info.BitDepth;

	info.DataSize = elem.ElementSize - sizeof(elem);
	info.Data = IExec->AllocVec(info.DataSize, MEMF_SHARED);
	if (!info.Data) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	status = IDOS->Read(file, info.Data, info.DataSize);
	if (status != info.DataSize) {
		error = ReadError(status);
		goto out;
	}

	ReadFunc = NULL;
	switch (bmh->bmh_Depth) {
		case 1:
			ReadFunc = ReadC1A1;
			break;
		case 4:
			bmh->bmh_Depth = 8;
		case 8:
			ReadFunc = ReadCLUT4_8;
			break;
		case 32:
			bmh->bmh_Masking = mskHasAlpha;
			ReadFunc = ReadARGB32;
			break;
	}
	if (!ReadFunc) {
		error = DTERROR_UNKNOWN_COMPRESSION;
		goto out;
	}

	error = ReadFunc(cl, o, file, bmh, &info);	

out:
	IExec->FreeVec(info.Data);

	return error;
}

static int32 ReadC1A1 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct IconInfo *info)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct BitMap *bm;
	uint8 *mask, *src, *dst;
	int32 error = OK;
	uint32 bpr, mod;
	int y;
	struct ColorRegister *cmap = NULL;
	uint32 *cregs = NULL;

	bpr = (bmh->bmh_Width + 7) >> 3;
	if (info->DataSize != ((bpr*bmh->bmh_Height) << 1)) {
		return DTERROR_INVALID_DATA;
	}

	bm = IGraphics->AllocBitMap(bmh->bmh_Width, bmh->bmh_Height, 1, 0, NULL);
	mask = IGraphics->AllocRaster(bmh->bmh_Width, bmh->bmh_Height);
	if (!bm && !mask) {
		IGraphics->FreeBitMap(bm);
		if (mask) IGraphics->FreeRaster(mask, bmh->bmh_Width, bmh->bmh_Height);
		return ERROR_NO_FREE_STORE;
	}

	src = info->Data;
	dst = (uint8 *)bm->Planes[0];
	mod = bm->BytesPerRow;
	for (y = 0; y < bmh->bmh_Width; y++) {
		IExec->CopyMem(src, dst, bpr);
		src += bpr;
		dst += mod;
	}
	dst = mask;
	for (y = 0; y < bmh->bmh_Width; y++) {
		IExec->CopyMem(src, dst, bpr);
		src += bpr;
		dst += bpr;
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_BitMap,		bm,
		PDTA_MaskPlane,		mask,
		TAG_END);
	bmh->bmh_Masking = mskHasMask;

	SetCMAP(cl, o, 2);

	return error;
}

static int32 ReadCLUT4_8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct IconInfo *info)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint8 *data;
	uint32 pixel_count;
	int32 level = 0, error = OK;
	uint32 mask_type;
	struct ElementHeader elem;

	pixel_count = bmh->bmh_Width * bmh->bmh_Height;
	if (info->DataSize != ((pixel_count * info->BitDepth) >> 3)) {
		return DTERROR_INVALID_DATA;
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_SourceMode,	PMODE_V43,
		DTA_ErrorLevel,		&level,
		DTA_ErrorNumber,	&error,
		TAG_END);
	if (level) return error;

	if (info->BitDepth == 4) {
		uint8 *src, *dst;
		int x, y;
		data = IExec->AllocVec(pixel_count, 0);
		if (!data) return ERROR_NO_FREE_STORE;
		src = info->Data;
		dst = data;
		for (y = 0; y < bmh->bmh_Height; y++) {
			for (x = 0; x < bmh->bmh_Width; x += 2) {
				*dst++ = *src >> 4;
				*dst++ = *src & 0xF;
				src++;
			}
		}
	} else {
		data = info->Data;
	}

	IIntuition->IDoSuperMethod(cl, o,
		PDTM_WRITEPIXELARRAY, data, PBPAFMT_LUT8,
		bmh->bmh_Width, 0, 0, bmh->bmh_Width, bmh->bmh_Height);

	if (info->BitDepth == 4) {
		IExec->FreeVec(data);
	}

	SetCMAP(cl, o, 1 << info->BitDepth);

	IExec->FreeVec(info->Data);
	info->Data = NULL;
	mask_type = GetMaskType(info->Type);
	error = mask_type ? ParseElements(cl, file, &elem, info, mask_type, 0, NULL) :
		DTERROR_NOT_AVAILABLE;
	if (error == OK) {
		uint8 *mask;
		uint32 plane_size;
		int32 status;

		plane_size = (bmh->bmh_Width * bmh->bmh_Height) >> 3;
		if (!IDOS->ChangeFilePosition(file, plane_size, OFFSET_CURRENT)) {
			return IDOS->IoErr();
		}

		mask = IGraphics->AllocRaster(bmh->bmh_Width, bmh->bmh_Height);
		if (!mask) return ERROR_NO_FREE_STORE;

		status = IDOS->Read(file, mask, plane_size);
		if (status != plane_size) {
			IGraphics->FreeRaster(mask, bmh->bmh_Width, bmh->bmh_Height);
			return ReadError(status);
		}

		IDataTypes->SetDTAttrs(o, NULL, NULL,
			PDTA_MaskPlane,		mask,
			TAG_END);
		bmh->bmh_Masking = mskHasMask;
	}
	if (error == DTERROR_NOT_AVAILABLE) error = OK;

	return OK;
}

static int32 ReadARGB32 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct IconInfo *info)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bpr;
	uint8 *buf;
	int32 status, level = 0, error = OK;
	uint32 pixel_count;
	uint32 decoded_size;
	uint8 *mask = NULL;

	bpr = bmh->bmh_Width << 2;
	pixel_count = bmh->bmh_Width * bmh->bmh_Height;
	decoded_size = pixel_count << 2;
	buf = IExec->AllocVec(decoded_size, MEMF_SHARED);
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
	if (level)
		goto out;
	else
		error = OK;

	if (bmh->bmh_Width >= 256 && bmh->bmh_Height >= 256) {
		error = icns_jp2_decode(info, buf);
	} else {
		struct ElementHeader elem;
		uint32 mask_type;
		error = rle24_decode(info->Data, info->DataSize, buf, decoded_size, pixel_count);
		if (error) goto out;

		IExec->FreeVec(info->Data);
		info->Data = NULL;
		mask = IExec->AllocVec(pixel_count, MEMF_SHARED);
		if (!mask) {
			error = ERROR_NO_FREE_STORE;
			goto out;
		}

		mask_type = GetMaskType(info->Type);
		error = mask_type ? ParseElements(cl, file, &elem, info, mask_type, 0, NULL) :
			DTERROR_NOT_AVAILABLE;
		if (error == OK) {
			uint8 *in, *out;
			uint32 pixels_left;
			status = IDOS->Read(file, mask, pixel_count);
			if (status != pixel_count) {
				error = ReadError(status);
				goto out;
			}
			in = mask;
			out = buf;
			pixels_left = pixel_count;
			while (pixels_left--) {
				*out = *in++;
				out += 4;
			}
		}
		if (error == DTERROR_NOT_AVAILABLE) error = OK;
	}
	if (error) goto out;

	IIntuition->IDoSuperMethod(cl, o,
		PDTM_WRITEPIXELARRAY, buf, PBPAFMT_ARGB,
		bpr, 0, 0, bmh->bmh_Width, bmh->bmh_Height);

out:
	IExec->FreeVec(mask);
	IExec->FreeVec(buf);
	return error;
}

static int32 GetICNS (Class *cl, Object *o, struct TagItem *tags) {
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
		error = ConvertICNS(cl, o, file, bmh, whichpic, numpics);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	} else
		if (srctype != DTST_FILE) error = ERROR_NOT_IMPLEMENTED;

	return error;
}
