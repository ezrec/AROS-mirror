#include "icns_class.h"
#include <string.h>

int32 ParseElements (Class *cl, BPTR file, struct ElementHeader *elem, struct IconInfo *info,
	uint32 type, uint32 index, uint32 *total)
{
	struct ClassBase *libBase = (struct ClassBase *)cl->cl_UserData;
	uint32 n = 0;
	if (!IDOS->ChangeFilePosition(file, sizeof(struct ResourceHeader), OFFSET_BEGINNING)) {
		return IDOS->IoErr();
	}
	while (IDOS->Read(file, elem, sizeof(*elem)) == sizeof(*elem)) {
		if (elem->ElementSize <= sizeof(*elem)) {
			return DTERROR_INVALID_DATA;
		}
		if (GetIconInfo(elem->ElementType, info)) {
			if (total == NULL) {
				if (type != 0) {
					if (elem->ElementType == type) return OK;
				} else if (info->Image) {
					if (index == n) return OK;
				}
			}
			if (info->Image) n++;
		}
		if (!IDOS->ChangeFilePosition(file, elem->ElementSize - sizeof(*elem), OFFSET_CURRENT)) {
			return IDOS->IoErr();
		}
	}
	if (total) *total = n;
	else return DTERROR_NOT_AVAILABLE;
	return IDOS->IoErr();
}

BOOL GetIconInfo (uint32 type, struct IconInfo *info) {
	memset(info, 0, sizeof(struct IconInfo));
	info->Type = type;
	switch (type) {

		// RGB24

		case ICNS_ARGB32_512x512:
			info->Image = TRUE;
			info->Channels = 4;
			info->PixelDepth = 8;
			info->BitDepth = 32;
			info->Width = info->Height = 512;
			break;

		case ICNS_ARGB32_256x256:
			info->Image = TRUE;
			info->Channels = 4;
			info->PixelDepth = 8;
			info->BitDepth = 32;
			info->Width = info->Height = 256;
			break;

		case ICNS_RGB24_128x128:
			info->Image = TRUE;
			info->Channels = 4;
			info->PixelDepth = 8;
			info->BitDepth = 32;
			info->Width = info->Height = 128;
			break;

		case ICNS_RGB24_48x48:
			info->Image = TRUE;
			info->Channels = 4;
			info->PixelDepth = 8;
			info->BitDepth = 32;
			info->Width = info->Height = 48;
			break;

		case ICNS_RGB24_32x32:
			info->Image = TRUE;
			info->Channels = 4;
			info->PixelDepth = 8;
			info->BitDepth = 32;
			info->Width = info->Height = 32;
			break;

		case ICNS_RGB24_16x16:
			info->Image = TRUE;
			info->Channels = 4;
			info->PixelDepth = 8;
			info->BitDepth = 32;
			info->Width = info->Height = 16;
			break;

		// 8-bit image

		case ICNS_CLUT8_48x48:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 48;
			break;

		case ICNS_CLUT8_32x32:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 32;
			break;

		case ICNS_CLUT8_16x16:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 16;
			break;

		case ICNS_CLUT8_16x12:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = 16;
			info->Height = 12;
			break;

		// 4-bit image

		case ICNS_CLUT4_48x48:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 4;
			info->Width = info->Height = 48;
			break;

		case ICNS_CLUT4_32x32:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 4;
			info->Width = info->Height = 32;
			break;

		case ICNS_CLUT4_16x16:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 4;
			info->Width = info->Height = 16;
			break;

		case ICNS_CLUT4_16x12:
			info->Image = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 4;
			info->Width = 16;
			info->Height = 12;
			break;

		// 8-bit mask

		case ICNS_ALPHA8_128x128:
			info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 128;
			break;

		case ICNS_ALPHA8_48x48:
			info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 48;
			break;

		case ICNS_ALPHA8_32x32:
			info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 32;
			break;

		case ICNS_ALPHA8_16x16:
			info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 8;
			info->Width = info->Height = 16;
			break;

		// 1-bit image + 1-bit mask

		case ICNS_C1A1_48x48:
			info->Image = info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 1;
			info->Width = info->Height = 48;
			break;

		case ICNS_C1A1_32x32:
			info->Image = info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 1;
			info->Width = info->Height = 32;
			break;

		case ICNS_C1A1_16x16:
			info->Image = info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 1;
			info->Width = info->Height = 16;
			break;

		case ICNS_C1A1_16x12:
			info->Image = info->Mask = TRUE;
			info->Channels = 1;
			info->PixelDepth = info->BitDepth = 1;
			info->Width = 16;
			info->Height = 12;
			break;

		default:
			info->Type = 0;
			return FALSE;

	}
	return TRUE;
}


uint32 GetMaskType (uint32 type) {
	switch (type) {

		case ICNS_RGB24_128x128:
			return ICNS_ALPHA8_128x128;

		case ICNS_RGB24_48x48:
			return ICNS_ALPHA8_48x48;

		case ICNS_RGB24_32x32:
			return ICNS_ALPHA8_32x32;

		case ICNS_RGB24_16x16:
			return ICNS_ALPHA8_16x16;

		case ICNS_C1A1_48x48:
		case ICNS_CLUT4_48x48:
		case ICNS_CLUT8_48x48:
			return ICNS_C1A1_48x48;

		case ICNS_C1A1_32x32:
		case ICNS_CLUT4_32x32:
		case ICNS_CLUT8_32x32:
			return ICNS_C1A1_32x32;

		case ICNS_C1A1_16x16:
		case ICNS_CLUT4_16x16:
		case ICNS_CLUT8_16x16:
			return ICNS_C1A1_16x16;

		case ICNS_C1A1_16x12:
		case ICNS_CLUT4_16x12:
		case ICNS_CLUT8_16x12:
			return ICNS_C1A1_16x12;

	}
	return 0;
}
