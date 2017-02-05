/*
 * jpeg2000.datatype
 * (c) Fredrik Wikstrom
 */

#include "jp2_class.h"
#include <string.h>
#include <proto/openjpeg.h>
#include <inline4/openjpeg.h>

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

static int32 ConvertJP2 (Class *cl, Object *o, BPTR file, void *mem, uint32 size,
	struct BitMapHeader *bmh);
static int32 GetJP2 (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetJP2(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
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

static const uint8 jp2_header[12] = { 0x00, 0x00, 0x00, 0x0C, 'j', 'P', ' ', ' ', '\r', '\n', 0x87, '\n' };

static int32 ConvertJP2 (Class *cl, Object *o, BPTR file, void *mem, uint32 size,
	struct BitMapHeader *bmh)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	int32 status, level = 0, error = OK;
	opj_dinfo_t *dinfo = NULL;
	opj_dparameters_t dparams;
	opj_cio_t *cio = NULL;
	opj_image_t *image = NULL;
	uint8 *buf = NULL, *dst;
	uint32 bpr;
	int x, y;
	int *red_ptr, *green_ptr, *blue_ptr;
	int red, green, blue;

	if (!mem) {
		size = IDOS->GetFileSize(file);
		if (size == (uint32)-1 && (error = IDOS->IoErr())) {
			goto out;
		}
		mem = IExec->AllocVec(size, MEMF_SHARED);
		if (!mem) {
			error = ERROR_NO_FREE_STORE;
			goto out;
		}
		status = IDOS->Read(file, mem, size);
		if (status != size) {
			error = ReadError(status);
			goto out;
		}
	} else {
		file = ZERO;
	}

	if (!memcmp(mem, jp2_header, 12)) {
		dinfo = opj_create_decompress(CODEC_JP2);
	} else {
		dinfo = opj_create_decompress(CODEC_J2K);
	}
	if (!dinfo) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	opj_set_default_decoder_parameters(&dparams);

	opj_setup_decoder(dinfo, &dparams);

	cio = opj_cio_open((opj_common_ptr)dinfo, mem, size);
	if (!cio) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	image = opj_decode(dinfo, cio);
	if (!image) {
		error = DTERROR_INVALID_DATA;
		goto out;
	}

	opj_cio_close(cio);
	opj_destroy_decompress(dinfo);
	cio = NULL;
	dinfo = NULL;

	if (image->numcomps != 3) {
		error = DTERROR_UNKNOWN_COMPRESSION;
		goto out;
	}

	bmh->bmh_Depth = 24;
	bmh->bmh_Width = image->comps[0].w;
	bmh->bmh_Height = image->comps[0].h;

	bpr = bmh->bmh_Width*3;
	buf = IExec->AllocVec(bpr, MEMF_PRIVATE);
	if (!buf) {
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
	if (level)
		goto out;
	else
		error = OK;

	red_ptr = image->comps[0].data;
	green_ptr = image->comps[1].data;
	blue_ptr = image->comps[2].data;

	for (y = 0; y < bmh->bmh_Height; y++) {
		dst = buf;
		for (x = 0; x < bmh->bmh_Width; x++) {
			red = *red_ptr++;
			green = *green_ptr++;
			blue = *blue_ptr++;
			if (image->comps[0].sgnd) red += 1 << (image->comps[0].prec - 1);
			if (image->comps[1].sgnd) green += 1 << (image->comps[1].prec - 1);
			if (image->comps[2].sgnd) blue += 1 << (image->comps[2].prec - 1);
			*dst++ = red;
			*dst++ = green;
			*dst++ = blue;
		}
		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf, PBPAFMT_RGB,
			bpr, 0, y, bmh->bmh_Width, 1);
	}

out:
	IExec->FreeVec(buf);
	if (image) opj_image_destroy(image);
	if (cio) opj_cio_close(cio);
	if (dinfo) opj_destroy_decompress(dinfo);
	if (file) IExec->FreeVec(mem);

	return error;
}

static int32 GetJP2 (Class *cl, Object *o, struct TagItem *tags) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	struct BitMapHeader *bmh = NULL;
	char *filename;
	int32 srctype;
	int32 error = ERROR_REQUIRED_ARG_MISSING;
	BPTR file = ZERO;
	void *mem = NULL;
	uint32 memsize = 0;

	filename = (char *)IUtility->GetTagData(DTA_Name, (uint32)"Untitled", tags);

	IDataTypes->GetDTAttrs(o,
		PDTA_BitMapHeader,	&bmh,
		DTA_SourceType,		&srctype,
		DTA_Handle,			&file,
		DTA_SourceAddress,	&mem,
		DTA_SourceSize,		&memsize,
		TAG_END);

	/* Do we have everything we need? */
	if (bmh && ((srctype == DTST_FILE && file) || 
		(srctype == DTST_MEMORY && mem && memsize)))
	{
		error = ConvertJP2(cl, o, file, mem, memsize, bmh);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	} else {
		if (srctype != DTST_FILE && srctype != DTST_MEMORY)
			error = ERROR_NOT_IMPLEMENTED;
	}

	return error;
}
