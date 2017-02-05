/*
 * dds.datatype
 * (c) Fredrik Wikstrom
 */

#include "dds_class.h"

#define TRANS ((uint32)-1)

int32 ReadDXT1 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint8 buf1[8], buf2[48], *src, *dst, byte;
	uint32 rgb;
	uint32 c[3][4], bak[3] = {0};
	int32 status, error = OK;
	int x, y, i, j;

	for (y = 0; y < bmh->bmh_Height; y += 4) {
		for (x = 0; x < bmh->bmh_Width; x += 4) {
			status = IDOS->FRead(file, buf1, 1, 8);
			if (status != 8) {
				error = ReadError(status);
				goto out;
			}

			src = buf1;
			dst = buf2;
			for (i = 0; i < 2; i++) {
				rgb = read_le16(src); src += 2;
				c[2][i] = rgb & 31; rgb >>= 5;
				c[1][i] = rgb & 63; rgb >>= 6;
				c[0][i] = rgb & 31;
				c[2][i] = (c[2][i] << 3)|(c[2][i] >> 2);
				c[1][i] = (c[1][i] << 2)|(c[1][i] >> 4);
				c[0][i] = (c[0][i] << 3)|(c[0][i] >> 2);
			}
			for (i = 0; i < 3; i++) {
				if (c[i][0] > c[i][1]) {
					c[i][2] = (2*c[i][0] + c[i][1]) / 3;
					c[i][3] = (c[i][0] + 2*c[i][1]) / 3;
				} else {
					c[i][2] = (c[i][0] + c[i][1]) >> 1;
					c[i][3] = TRANS;
				}
			}
			for (i = 0; i < 4; i++) {
				byte = *src++;
				j = byte & 3; byte >>= 2;
				*dst++ = (c[0][j] != TRANS) ? (bak[0] = c[0][j]) : bak[0];
				*dst++ = (c[1][j] != TRANS) ? (bak[1] = c[1][j]) : bak[1];
				*dst++ = (c[2][j] != TRANS) ? (bak[2] = c[2][j]) : bak[2];
				j = byte & 3; byte >>= 2;
				*dst++ = (c[0][j] != TRANS) ? (bak[0] = c[0][j]) : bak[0];
				*dst++ = (c[1][j] != TRANS) ? (bak[1] = c[1][j]) : bak[1];
				*dst++ = (c[2][j] != TRANS) ? (bak[2] = c[2][j]) : bak[2];
				j = byte & 3; byte >>= 2;
				*dst++ = (c[0][j] != TRANS) ? (bak[0] = c[0][j]) : bak[0];
				*dst++ = (c[1][j] != TRANS) ? (bak[1] = c[1][j]) : bak[1];
				*dst++ = (c[2][j] != TRANS) ? (bak[2] = c[2][j]) : bak[2];
				j = byte & 3;
				*dst++ = (c[0][j] != TRANS) ? (bak[0] = c[0][j]) : bak[0];
				*dst++ = (c[1][j] != TRANS) ? (bak[1] = c[1][j]) : bak[1];
				*dst++ = (c[2][j] != TRANS) ? (bak[2] = c[2][j]) : bak[2];
			}

			IIntuition->IDoSuperMethod(cl, o,
				PDTM_WRITEPIXELARRAY, buf2, PBPAFMT_RGB,
				12, x, y, 4, 4);
		}
	}

out:
	return error;
}

int32 ReadDXT2_3_4_5 (Class *cl, Object *o, BPTR file, struct BitMapHeader *bmh,
	struct DDS_Header *dds)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint8 buf1[16], buf2[64], *src, *dst, byte;
	uint32 rgb, alpha;
	uint32 c[3][4];
	int32 status, error = OK;
	int x, y, i, j, pma, acomp, rxgb;
	uint8 a[8];

	pma = (dds->PixelFormat.FourCC == D3DFMT_DXT2) ||
	      (dds->PixelFormat.FourCC == D3DFMT_DXT4); /* check if premultipled alpha */
	acomp = dds->PixelFormat.FourCC >= D3DFMT_DXT4; /* compressed alpha */
	rxgb = dds->PixelFormat.FourCC == D3DFMT_RXGB;

	for (y = 0; y < bmh->bmh_Height; y += 4) {
		for (x = 0; x < bmh->bmh_Width; x += 4) {
			status = IDOS->FRead(file, buf1, 1, 16);
			if (status != 16) {
				error = ReadError(status);
				goto out;
			}

			src = buf1;
			dst = buf2;
			if (acomp) {
				a[0] = *src++;
				a[1] = *src++;
				if (a[0] > a[1]) {
					for (i = 1; i < 7; i++) a[i+1] = ((7-i)*a[0] + i*a[1]) / 7;
				} else {
					for (i = 1; i < 5; i++) a[i+1] = ((5-i)*a[0] + i*a[1]) / 5;
					a[6] = 0;
					a[7] = 255;
				}
				for (i = 0; i < 2; i++) {
					alpha = ((uint32)src[0])|((uint32)src[1] << 8)|((uint32)src[2] << 16);
					src += 3;
					for (j = 0; j < 8; j++) {
						*dst = a[alpha & 7]; alpha >>= 3;
						dst += 4;
					}
				}
			} else {
				for (i = 0; i < 8; i++) {
					byte = *src++;
					j = byte & 15; byte >>= 4;
					*dst = j * 0x11; dst += 4;
					j = byte & 15;
					*dst = j * 0x11; dst += 4;
				}
			}
			for (i = 0; i < 2; i++) {
				rgb = read_le16(src); src += 2;
				c[2][i] = rgb & 31; rgb >>= 5;
				c[1][i] = rgb & 63; rgb >>= 6;
				c[0][i] = rgb & 31;
				c[2][i] = (c[2][i] << 3)|(c[2][i] >> 2);
				c[1][i] = (c[1][i] << 2)|(c[1][i] >> 4);
				c[0][i] = (c[0][i] << 3)|(c[0][i] >> 2);
			}
			for (i = 0; i < 3; i++) {
				c[i][2] = (2*c[i][0] + c[i][1]) / 3;
				c[i][3] = (c[i][0] + 2*c[i][1]) / 3;
			}
			dst = buf2;
			for (i = 0; i < 4; i++) {
				byte = *src++;
				j = byte & 3; byte >>= 2;
				dst++;
				*dst++ = c[0][j];
				*dst++ = c[1][j];
				*dst++ = c[2][j];
				j = byte & 3; byte >>= 2;
				dst++;
				*dst++ = c[0][j];
				*dst++ = c[1][j];
				*dst++ = c[2][j];
				j = byte & 3; byte >>= 2;
				dst++;
				*dst++ = c[0][j];
				*dst++ = c[1][j];
				*dst++ = c[2][j];
				j = byte & 3;
				dst++;
				*dst++ = c[0][j];
				*dst++ = c[1][j];
				*dst++ = c[2][j];
			}
			if (pma) { /* demultiply alpha */
				dst = buf2;
				for (i = 0; i < 16; i++) {
					alpha = *dst++;
					*dst++ = (*dst * 255) / alpha;
					*dst++ = (*dst * 255) / alpha;
					*dst++ = (*dst * 255) / alpha;
				}
			} else
			if (rxgb) {
				dst = buf2;
				for (i = 0; i < 16; i++) {
					dst[1] = dst[0];
					dst[0] = 255;
					dst += 4;
				}
			}

			IIntuition->IDoSuperMethod(cl, o,
				PDTM_WRITEPIXELARRAY, buf2, PBPAFMT_ARGB,
				16, x, y, 4, 4);
		}
	}

out:
	return error;
}
