/*
 * dds.datatype
 * (c) Fredrik Wikstrom
 */

#include "dds_class.h"

uint32 ClassDispatch (Class *cl, Object *o, Msg msg);

Class *initDTClass (struct ClassBase *libBase) {
	Class *cl = NULL;
	SuperClassLib = IExec->OpenLibrary("datatypes/picture.datatype", 51);
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

static int32 ConvertDDS (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
static int32 GetDDS (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetDDS(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
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

static int32 ReadIndexed (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds);

static int32 ConvertDDS (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct DDS_Header dds;
	int32 status, level, error = OK;
	int32 (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
		struct DDS_Header *dds);

	status = IDOS->Read(file, &dds, sizeof(struct DDS_Header));
	if (status != sizeof(struct DDS_Header)) {
		return ReadError(status);
	}

	if (dds.Magic != DDS_MAGIC) {
		return ERROR_OBJECT_WRONG_TYPE;
	}

	if (read_le32(&dds.Size) != 124 ||
		(dds.Flags & DDSD_REQUIRED) != DDSD_REQUIRED ||
		read_le32(&dds.PixelFormat.Size) != 32)
	{
		return DTERROR_INVALID_DATA;
	}

	bmh->bmh_Width = read_le32(&dds.Width);
	bmh->bmh_Height = read_le32(&dds.Height);

	ReadFunc = NULL;
	switch (dds.PixelFormat.Flags) {

		case DDPF_INDEXED:
			bmh->bmh_Depth = 8;
			ReadFunc = ReadIndexed;
			break;

		case DDPF_RGB:
			bmh->bmh_Depth = 24;
			ReadFunc = ReadRGB;
			break;

		case DDPF_RGB|DDPF_ALPHAPIXELS:
			bmh->bmh_Depth = 32;
			bmh->bmh_Masking = mskHasAlpha;
			ReadFunc = ReadRGB;
			break;

		case DDPF_FOURCC:
		case DDPF_FOURCC|DDPF_ALPHAPIXELS:
			switch (dds.PixelFormat.FourCC) {

				case D3DFMT_DXT1:
					bmh->bmh_Depth = 24;
					ReadFunc = ReadDXT1;
					break;

				case D3DFMT_DXT2:
				case D3DFMT_DXT3:
				case D3DFMT_DXT4:
				case D3DFMT_DXT5:
				case D3DFMT_RXGB:
					bmh->bmh_Depth = 32;
					bmh->bmh_Masking = mskHasAlpha;
					ReadFunc = ReadDXT2_3_4_5;
					break;

			}
			break;

	}
	if (!ReadFunc) {
		return DTERROR_UNKNOWN_COMPRESSION;
	}

	IDataTypes->SetDTAttrs(o, NULL, NULL,
		DTA_NominalHoriz,	bmh->bmh_Width,
		DTA_NominalVert,	bmh->bmh_Height,
		PDTA_SourceMode,	PMODE_V43,
		DTA_ErrorLevel,		&level,
		DTA_ErrorNumber,	&error,
		TAG_END);
	if (level) {
		return error;
	}

	return ReadFunc(cl, o, file, bmh, &dds);
}

static int32 ReadIndexed (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds)
{
	return ERROR_NOT_IMPLEMENTED;
}

static int32 GetDDS (Class *cl, Object *o, struct TagItem *tags) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct BitMapHeader *bmh = NULL;
	char *filename;
	int32 srctype;
	int32 error = ERROR_REQUIRED_ARG_MISSING;
	BPTR file = (BPTR)NULL;

	filename = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", tags);

	IDataTypes->GetDTAttrs(o,
		PDTA_BitMapHeader,	&bmh,
		DTA_Handle,			&file,
		DTA_SourceType,		&srctype,
		TAG_END);

	/* Do we have everything we need? */
	if (bmh && file && srctype == DTST_FILE) {
		error = ConvertDDS(cl, o, file, bmh);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	} else
		if (srctype != DTST_FILE) error = ERROR_NOT_IMPLEMENTED;

	return error;
}
