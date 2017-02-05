/*
 * info.datatype
 * (c) Fredrik Wikstrom
 */

#include "info_class.h"
#include <string.h>

#define RGB8to32(RGB) ((uint32)(RGB) * 0x01010101UL)

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

static int32 ConvertINFO (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	uint32 index, uint32 *total);
static int32 GetINFO (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetINFO(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
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

static int32 ReadPlanar (Class *cl, Object *o, struct DiskObject *icon, struct BitMapHeader *bmh,
	struct IconInfo *info);
static int32 ReadChunky (Class *cl, Object *o, struct DiskObject *icon, struct BitMapHeader *bmh,
	struct IconInfo *info);

static int32 ConvertINFO (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	uint32 index, uint32 *total)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	char *filename = NULL, *ext;
	struct DiskObject *icon = NULL;
	int32 error = OK;
	struct ColorRegister *cmap = NULL;
	uint32 *cregs = NULL;
	struct IconInfo info = {0};
	uint32 hasimage2 = FALSE;

	filename = IExec->AllocVec(512, MEMF_SHARED);
	if (!filename) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	if (!IDOS->NameFromFH(file, filename, 512)) {
		error = IDOS->IoErr();
		goto out;
	}
	ext = strrchr(filename, '.');
	if (ext && !IUtility->Stricmp(ext, ".info")) {
		*ext = 0;
	}

	icon = IIcon->GetDiskObject(filename);
	if (!icon) {
		error = IDOS->IoErr();
		goto out;
	}

	IExec->FreeVec(filename);
	filename = NULL;

	switch (index) {
		case 0:
			IIcon->IconControl(icon,
				//ICONCTRLA_HasRealImage2,		&hasimage2,
				ICONCTRLA_GetImageDataFormat,	&info.Format,
				ICONCTRLA_GetWidth,				&info.Width,
				ICONCTRLA_GetHeight,			&info.Height,
				ICONCTRLA_GetPaletteSize1,		&info.NumColors,
				ICONCTRLA_GetPalette1,			&info.CMap,
				ICONCTRLA_GetImageData1,		&info.Image,
				ICONCTRLA_GetImageData2,		&hasimage2,
				TAG_END);
			break;
		case 1:
			IIcon->IconControl(icon,
				//ICONCTRLA_HasRealImage2,		&hasimage2,
				ICONCTRLA_GetImageDataFormat,	&info.Format,
				ICONCTRLA_GetWidth,				&info.Width,
				ICONCTRLA_GetHeight,			&info.Height,
				ICONCTRLA_GetPaletteSize2,		&info.NumColors,
				ICONCTRLA_GetPalette2,			&info.CMap,
				ICONCTRLA_GetImageData2,		&info.Image,
				TAG_END);
			hasimage2 = (uint32)info.Image;
			break;
		default:
			error = DTERROR_NOT_AVAILABLE;
			goto out;
	}
	info.Which = index;
	if (info.Format == IDFMT_BITMAPPED) {
		info.NumImages = 2;
	} else {
		info.NumImages = hasimage2 ? 2 : 1;
	}
	if (index >= info.NumImages) {
		error = DTERROR_NOT_AVAILABLE;
		goto out;
	}

	bmh->bmh_Depth = 8;
	bmh->bmh_Width = info.Width;
	bmh->bmh_Height = info.Height;

	switch (info.Format) {
		case IDFMT_BITMAPPED:
			error = ReadPlanar(cl, o, icon, bmh, &info);
			break;
		case IDFMT_DIRECTMAPPED:
			bmh->bmh_Depth = 32;
			bmh->bmh_Masking = mskHasAlpha;
		case IDFMT_PALETTEMAPPED:
			if (!info.Image) {
				error = DTERROR_NOT_AVAILABLE;
				break;
			}
			if (info.Width == 0 || info.Height == 0 || info.NumColors > 256) {
				error = DTERROR_INVALID_DATA;
				break;
			}
			error = ReadChunky(cl, o, icon, bmh, &info);
			break;
		default:
			error = DTERROR_UNKNOWN_COMPRESSION;
			break;
	}
	if (total) *total = info.NumImages;

	if (error || bmh->bmh_Depth > 8) goto out;

	IDataTypes->SetDTAttrs(o, NULL, NULL,
		PDTA_NumColors,			1 << bmh->bmh_Depth,
		TAG_END);
	if (IDataTypes->GetDTAttrs(o,
		PDTA_ColorRegisters,	&cmap,
		PDTA_CRegs,				&cregs,
		TAG_END) == 2)
	{
		const struct ColorRegister *src;
		int i;
		if (src = info.CMap) {
			for (i = 0; i < info.NumColors; i++) {
				cmap->red = src->red;
				cmap->green = src->green;
				cmap->blue = src->blue;
				*cregs++ = RGB8to32(src->red);
				*cregs++ = RGB8to32(src->green);
				*cregs++ = RGB8to32(src->blue);
				cmap++; src++;
			}
		}
	}

out:
	IIcon->FreeDiskObject(icon);
	IExec->FreeVec(filename);
	return error;
}

static const struct ColorRegister planar_cmap[16] = {
	{ 153, 153, 153 },
	{   0,   0,   0 },
	{ 255, 255, 255 },
	{  51, 102, 170 },
	{ 136, 136, 136 },
	{ 204, 204, 204 },
	{ 204, 153, 102 },
	{ 221, 187,  51 },
	{ 153,  68, 170 },
	{ 221,  34,  34 },
	{  34,  34,  34 },
	{   0,  85, 153 },
	{  85,  85,  85 },
	{ 170, 170, 170 },
	{  34, 136,  34 },
	{  85, 170, 187 }
};

static void Complement (uint16 *mem, uint32 words) {
	while (words--) {
		*mem++ = ~(*mem);
	}
}

static int32 ReadPlanar (Class *cl, Object *o, struct DiskObject *icon, struct BitMapHeader *bmh,
	struct IconInfo *info)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct Image *image = NULL;
	struct BitMap *bm = NULL;
	int y, plane;
	uint8 *src, *dst;
	uint32 src_bpr, dst_bpr;
	uint32 complement = FALSE;

	if ((icon->do_Gadget.Flags & GFLG_GADGIMAGE) == 0) {
		info->NumImages = 0;
		return DTERROR_INVALID_DATA;
	}
	if ((icon->do_Gadget.Flags & GFLG_GADGHIGHBITS) == GFLG_GADGHNONE) {
		info->NumImages = 1;
	}
	switch (info->Which) {
		case 0:
			image = icon->do_Gadget.GadgetRender;
			break;
		case 1:
			image = icon->do_Gadget.GadgetRender;
			switch (icon->do_Gadget.Flags & GFLG_GADGHIGHBITS) {
				case GFLG_GADGHIMAGE:
					image = icon->do_Gadget.SelectRender;
					break;
				case GFLG_GADGHCOMP:
				case GFLG_GADGHBOX:
					complement = TRUE;
					break;
				case GFLG_GADGHNONE:
					image = NULL;
					break;
			}
			break;
	}
	if (image == NULL) {
		return DTERROR_INVALID_DATA;
	}

	bmh->bmh_Depth = image->Depth;
	bmh->bmh_Width = info->Width = image->Width;
	bmh->bmh_Height = info->Height = image->Height;
	if (image->Depth == 0 || image->PlanePick != ((1 << image->Depth)-1)) {
		return DTERROR_INVALID_DATA;
	}

	bm = IGraphics->AllocBitMap(bmh->bmh_Width, bmh->bmh_Height, bmh->bmh_Depth, 0, NULL);
	if (!bm) {
		return ERROR_NO_FREE_STORE;
	}

	src = (uint8 *)image->ImageData;
	src_bpr = ((bmh->bmh_Width + 15) >> 4) << 1;
	dst_bpr = bm->BytesPerRow;
	for (plane = 0; plane < bmh->bmh_Depth; plane++) {
		dst = (uint8 *)bm->Planes[plane];
		for (y = 0; y < bmh->bmh_Height; y++) {
			IExec->CopyMem(src, dst, src_bpr);
			if (complement) Complement((uint16 *)dst, src_bpr >> 1);
			src += src_bpr;
			dst += dst_bpr;
		}
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_SourceMode,	PMODE_V42,
		PDTA_BitMap,		bm,
		TAG_END);

	info->CMap = planar_cmap;
	info->NumColors = 1 << bmh->bmh_Depth;
	if (info->NumColors > 16) info->NumColors = 16;

	return OK;
}

static int32 ReadChunky (Class *cl, Object *o, struct DiskObject *icon, struct BitMapHeader *bmh,
	struct IconInfo *info)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	int32 level = 0, error = OK;

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

	if (info->Format == IDFMT_PALETTEMAPPED) {
		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, info->Image, PBPAFMT_LUT8,
			bmh->bmh_Width, 0, 0, bmh->bmh_Width, bmh->bmh_Height);
	} else {
		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, info->Image, PBPAFMT_ARGB,
			bmh->bmh_Width*4, 0, 0, bmh->bmh_Width, bmh->bmh_Height);
	}

out:
	return error;
}

static int32 GetINFO (Class *cl, Object *o, struct TagItem *tags) {
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
		error = ConvertINFO(cl, o, file, bmh, whichpic, numpics);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	} else
		if (srctype != DTST_FILE) error = ERROR_NOT_IMPLEMENTED;

	return error;
}
