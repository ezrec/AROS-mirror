/*
 * targa.datatype
 * (c) Fredrik Wikstrom
 */

#include "targa_class.h"
#include "endian.h"

#define RGB8to32(RGB) (((uint32)(RGB) << 24)|((uint32)(RGB) << 16)|((uint32)(RGB) << 8)|((uint32)(RGB)))

uint32 ClassDispatch (Class *cl, Object *o, Msg msg);

Class *initDTClass (struct ClassBase *libBase) {
	Class *cl;
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

static int32 WriteTarga (Class *cl, Object *o, struct dtWrite *msg);
static int32 ConvertTarga (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh);
static int32 GetTarga (Class *cl, Object *o, struct TagItem *tags);

uint32 ClassDispatch (Class *cl, Object *o, Msg msg) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 ret;

	switch (msg->MethodID) {

		case OM_NEW:
			ret = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (ret) {
				int32 error;
				error = GetTarga(cl, (Object *)ret, ((struct opSet *)msg)->ops_AttrList);
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
				error = WriteTarga(cl, o, (struct dtWrite *)msg);
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

static int32 WriteTarga (Class *cl, Object *o, struct dtWrite *msg) {
	return ERROR_NOT_IMPLEMENTED;
}

int32 ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
int32 ReadRGB15 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
int32 ReadRGBA16 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
int32 ReadTruecolor (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
int32 ReadPixels (struct ClassBase *libBase, struct TargaHeader *tga, BPTR file, uint8 *buf, int32 width, int32 bpp);

int32 GetCMAP24 (struct ClassBase *libBase, BPTR file, struct ColorRegister *cmap, int32 len);

static int32 ConvertTarga (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	int32 status, error = OK;
	struct TargaHeader tga;
	int32 (*ReadFunc) (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga);
	int needs_palette;

	status = IDOS->Read(file, &tga, sizeof(struct TargaHeader));
	if (status != sizeof(struct TargaHeader)) {
		return ReadError(status);
	}

	bmh->bmh_Width = read_le16(&tga.width);
	bmh->bmh_Height = read_le16(&tga.height);
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
		if (IDOS->Seek(file, tga.idsize, OFFSET_CURRENT) == -1 && (error = IDOS->IoErr())) {
			goto out;
		}
	}

	if (tga.cmaptype > 1) {
		error = NOTOK;
		goto out;
	}
	if (needs_palette) {
		struct ColorRegister *cmap = NULL;
		uint32 *cregs = NULL;
		int32 (*CMAPFunc) (struct ClassBase *libBase, BPTR file, struct ColorRegister *cmap,
			int32 len);

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

		IDataTypes->SetDTAttrs(o, NULL, NULL,
			PDTA_NumColors, 256,
			TAG_END);

		if (IDataTypes->GetDTAttrs(o, PDTA_ColorRegisters, &cmap, PDTA_CRegs, &cregs, TAG_END) == 2)
		{
			int start, len, i;

			start = read_le16(tga.cmapstart);
			len = read_le16(tga.cmaplength);

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
		uint32 len;
		len = read_le16(tga.cmaplength) * ((tga.cmapdepth+7)>>3);
		if (IDOS->Seek(file, len, OFFSET_CURRENT) == -1 && (error = IDOS->IoErr())) {
			goto out;
		}
	}

	error = ReadFunc(cl, o, file, bmh, &tga);

out:
	return error;
}

int32 ReadLUT8 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bpr, fmt;
	uint8 *buf;
	int32 status, level = 0, error = OK;
	int y;
	
	bpr = bmh->bmh_Width;
	fmt = ((tga->imagetype & ~TGA_IMAGE_RLE) == TGA_IMAGE_GREY) ? PBPAFMT_GREY8 : PBPAFMT_LUT8;
	
	buf = IExec->AllocMem(bpr, MEMF_SHARED);
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

	for (y = bmh->bmh_Height-1; y >= 0; y--) {
		status = ReadPixels(libBase, tga, file, buf, bpr, 1);
		if (status != bmh->bmh_Width) {
			error = IDOS->IoErr();
			goto out;
		}
		
		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf, fmt,
			bpr, 0, y, bpr, 1);
	}

out:
	IExec->FreeMem(buf, bpr);
	return error;
}

int32 ReadRGB15 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	return ERROR_NOT_IMPLEMENTED;
}

int32 ReadRGBA16 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	return ERROR_NOT_IMPLEMENTED;
}

int32 ReadTruecolor (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh, struct TargaHeader *tga) {
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 bpp, bpr, fmt;
	uint8 *buf1, *buf2, *src, *dst;
	int32 status, level = 0, error = OK;
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
		
	buf1 = IExec->AllocMem(bpr, MEMF_SHARED);
	buf2 = IExec->AllocMem(bpr, MEMF_SHARED);
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
	if (level)
		goto out;
	else
		error = OK;
	
	for (y = bmh->bmh_Height-1; y >= 0; y--) {
		status = ReadPixels(libBase, tga, file, buf1, bmh->bmh_Width, bpp);
		if (status != bmh->bmh_Width) {
			error = IDOS->IoErr();
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
				write_le32(dst, *(uint32 *)src);
				src += 4; dst += 4;
			}
		}

		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf2, fmt,
			bpr, 0, y, bmh->bmh_Width, 1);
	}
	
out:
	if (buf1) IExec->FreeMem(buf1, bpr);
	if (buf2) IExec->FreeMem(buf2, bpr);
	return error;
}

int32 ReadPixels (struct ClassBase *libBase, struct TargaHeader *tga, BPTR file, uint8 *buf, int32 width, int32 bpp) {
	if (tga->imagetype & TGA_IMAGE_RLE) {
		uint8 *ptr = buf;
		int32 x = 0;
		int32 st, cnt, rem;
		uint8 pix[4];
		while (x < width) {
			cnt = IDOS->FGetC(file);
			if (cnt == -1) break;
			rem = width-x;
			if (cnt & 0x80) {
				cnt -= 0x80;
				if (++cnt > rem) cnt = rem;
				st = IDOS->FRead(file, pix, bpp, 1);
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
							*(uint16 *)buf = *(uint16 *)pix;
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
							*(uint32 *)buf = *(uint32 *)pix;
							buf += 4;
						}
						break;
				}
			} else {
				if (++cnt > rem) cnt = rem;
				x += st = IDOS->FRead(file, buf, bpp, cnt);
				if (st != cnt) break;
				buf += (cnt*bpp);
			}
		}
		return x;
	} else {
		return IDOS->FRead(file, buf, bpp, width);
	}
}

int32 GetCMAP24 (struct ClassBase *libBase, BPTR file, struct ColorRegister *cmap, int32 len) {
	uint8 *buf, *ptr;
	int32 status, error = OK, size = len * 3;
	int32 i;

	buf = IExec->AllocMem(size, MEMF_SHARED);
	if (!buf) {
		return ERROR_NO_FREE_STORE;
	}

	status = IDOS->Read(file, buf, size);
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
	IExec->FreeMem(buf, size);
	return error;
}

static int32 GetTarga (Class *cl, Object *o, struct TagItem *tags) {
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
		error = ConvertTarga(cl, o, file, bmh);
		if (error == OK) {
			IDataTypes->SetDTAttrs(o, NULL, NULL,
				DTA_ObjName,	IDOS->FilePart(filename),
				TAG_END);
		}
	}
	return error;
}
