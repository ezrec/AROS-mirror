/*
 * dds.datatype
 * (c) Fredrik Wikstrom
 */

#include "dds_class.h"

static const uint16 rgb_coefs[] = {
	0xff00,
	0x5500,
	0x2480,
	0x1100,
	0x0840,
	0x0410,
	0x0204,
	0x0100
};

int32 ReadRGB (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint8 *buf1, *buf2, *src, *dst;
	uint32 bpr, bpp, fmt, obpp, obpr;
	uint32 mask[4], msk, bits = 0, rgb;
	uint32 shift[4] = {0};
	uint32 coef[4] = {0};
	int32 status, error = OK;
	int y, x;
	BOOL alpha;

	alpha = (dds->PixelFormat.Flags & DDPF_ALPHAPIXELS) ? TRUE : FALSE;
	if (!alpha) {
		fmt = PBPAFMT_RGB;
		obpp = 3;
	} else {
		fmt = PBPAFMT_ARGB;
		obpp = 4;
	}
	switch (read_le32(&dds->PixelFormat.RGBBitCount)) {
		case 15:
		case 16:
			bpp = 2;
			break;
		case 24:
			bpp = 3;
			break;
		case 32:
			bpp = 4;
			break;
		default:
			return DTERROR_UNKNOWN_COMPRESSION;
	}
	bpr = (dds->Flags & DDSD_PITCH) ? read_le32(&dds->PitchOrLinearSize) : bmh->bmh_Width*bpp;
	obpr = bmh->bmh_Width * obpp;

	mask[0] = read_le32(&dds->PixelFormat.RBitMask);
	mask[1] = read_le32(&dds->PixelFormat.GBitMask);
	mask[2] = read_le32(&dds->PixelFormat.BBitMask);
	for (y = 0; y < obpp; y++) {
		if (mask[y] == 0) return DTERROR_INVALID_DATA;
		for (msk = mask[y]; (msk & 1) == 0; msk >>= 1) shift[y]++;
		for (bits = 0; (msk & 1) == 1; msk >>= 1) bits++;
		if (bits < 1 || bits > 8) return DTERROR_INVALID_DATA;
		coef[y] = rgb_coefs[bits - 1];
	}

	buf1 = IExec->AllocMem(bpr, MEMF_SHARED);
	buf2 = IExec->AllocMem(obpr, MEMF_SHARED);
	if (!buf1 || !buf2) {
		error = ERROR_NO_FREE_STORE;
		goto out;
	}

	for (y = 0; y < bmh->bmh_Height; y++) {
		status = IDOS->Read(file, buf1, bpr);
		if (status != bpr) {
			error = ReadError(status);
			goto out;
		}

		src = buf1;
		dst = buf2;
		for (x = 0; x < bmh->bmh_Width; x++) {
			if (bpp == 2) {
				rgb = read_le16(src);
				src += 2;
			} else
			if (bpp == 3) {
				rgb = ((uint32)src[0])|((uint32)src[1] << 8)|((uint32)src[2] << 16);
				src += 3;
			} else {
				rgb = read_le32(src);
				src += 4;
			}
			*dst++ = (((rgb & mask[0]) >> shift[0]) * coef[0]) >> 8;
			*dst++ = (((rgb & mask[1]) >> shift[1]) * coef[1]) >> 8;
			*dst++ = (((rgb & mask[2]) >> shift[2]) * coef[2]) >> 8;
			if (alpha) {
				*dst++ = (((rgb & mask[3]) >> shift[3]) * coef[3]) >> 8;
			}
		}

		IIntuition->IDoSuperMethod(cl, o,
			PDTM_WRITEPIXELARRAY, buf2, fmt,
			obpr, 0, y, bmh->bmh_Width, 1);
	}

out:
	IExec->FreeMem(buf1, bpr);
	IExec->FreeMem(buf2, obpr);
	return error;
}
