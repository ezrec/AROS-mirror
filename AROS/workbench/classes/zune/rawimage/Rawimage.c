/***************************************************************************

 Magic User Interface - MUI
 Copyright (C) 1992-2004 by Stefan Stuntz <stefan@stuntz.com>
 All Rights Reserved.

 This program/documents may not in whole or in part, be copied, photocopied
 reproduced, translated or reduced to any electronic medium or machine
 readable form without prior consent, in writing, from the above authors.

 With this document the authors makes no warrenties or representations,
 either expressed or implied, with respect to MUI. The information
 presented herein is being supplied on an "AS IS" basis and is expressly
 subject to change without notice. The entire risk as to the use of this
 information is assumed by the user.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 IN NO EVENT WILL THE AUTHORS BE LIABLE FOR DIRECT, INDIRECT, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES RESULTING FROM ANY CLAIM ARISING OUT OF THE
 INFORMATION PRESENTED HEREIN, EVEN IF IT HAS BEEN ADVISED OF THE
 POSSIBILITIES OF SUCH DAMAGES.

 MUI Official Support Site:  http://www.sasg.com/

 $Id$

***************************************************************************/

#include "symbols.h"

#include "Rawimage_mcc.h"

#define CLASS       MUIC_Rawimage
#define SUPERCLASS  MUIC_Pixmap

#define INSTDATA		Data

#define USEDCLASSESP used_mcps
static const char *used_mcps[] = { NULL };

struct Data
{
	int dummy;
};

/******************************************************************************/
/* define the functions used by the startup code ahead of including mccinit.c */
/******************************************************************************/

#define COPYRIGHT "(c) 2011 Thore Böckelmann"
#include "copyright.h"

#define VERSION   20
#define REVISION 18
#define LIBDATE  "09.11.2011"
#define USERLIBID CLASS " " STR(VERSION)"."STR(REVISION) " (" LIBDATE ")" FULLVERS

#define libfunc(x) 	Rawimage_Lib ## x
#define reqfunc		muilink_use_Rawimage
#define addname(x) 	Rawimage_ ## x

#define USE_INTUITIONBASE

#include "mccinit.c"

/* ------------------------------------------------------------------------- */

#if !defined(__MORPHOS__) // MorphOS has it`s own DoSuperNew Method in amiga.lib
static Object * STDARGS VARARGS68K DoSuperNew(struct IClass *cl, Object * obj, ...)
{
	Object *rc;
	VA_LIST args;

	VA_START(args, obj);
	rc = (Object *)DoSuperMethod(cl, obj, OM_NEW, VA_ARG(args, ULONG), NULL);
	VA_END(args);

	return rc;
}
#endif

/* ------------------------------------------------------------------------- */

static BOOL setRawimage(struct IClass *cl, Object *obj, struct MUI_RawimageData *rdata)
{
	BOOL success = FALSE;
	LONG format;
	LONG compression;

	switch(rdata->ri_Format)
	{
		case RAWIMAGE_FORMAT_RAW_ARGB_ID:
			format = MUIV_Pixmap_Format_ARGB32;
			compression = MUIV_Pixmap_Compression_None;
		break;

		case RAWIMAGE_FORMAT_RAW_RGB_ID:
			format = MUIV_Pixmap_Format_RGB24;
			compression = MUIV_Pixmap_Compression_None;
		break;

		case RAWIMAGE_FORMAT_BZ2_ARGB_ID:
			format = MUIV_Pixmap_Format_ARGB32;
			compression = MUIV_Pixmap_Compression_BZip2;
		break;

		case RAWIMAGE_FORMAT_BZ2_RGB_ID:
			format = MUIV_Pixmap_Format_RGB24;
			compression = MUIV_Pixmap_Compression_BZip2;
		break;

		case RAWIMAGE_FORMAT_RLE_ARGB_ID:
			format = MUIV_Pixmap_Format_ARGB32;
			compression = MUIV_Pixmap_Compression_RLE;
		break;

		case RAWIMAGE_FORMAT_RLE_RGB_ID:
			format = MUIV_Pixmap_Format_RGB24;
			compression = MUIV_Pixmap_Compression_RLE;
		break;

		default:
			format = -1;
			compression = MUIV_Pixmap_Compression_None;
		break;
	}

	if(format != -1)
	{
		// Pixmap.mui will return failure in case the compression is not supported
		success = SetSuperAttrs(cl, obj, MUIA_FixWidth, rdata->ri_Width,
										 MUIA_FixHeight, rdata->ri_Height,
										 MUIA_Pixmap_Width, rdata->ri_Width,
										 MUIA_Pixmap_Height, rdata->ri_Height,
										 MUIA_Pixmap_Format, format,
										 MUIA_Pixmap_Data, rdata->ri_Data,
										 MUIA_Pixmap_Compression, compression,
										 MUIA_Pixmap_CompressedSize, rdata->ri_Size,
										 TAG_DONE);
	}

	return success;
}

static ULONG ASM Rawimage_New(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct opSet *msg))
{
	if((obj = (Object *)DoSuperNew(cl, obj,
		TAG_MORE, msg->ops_AttrList)) != NULL)
	{
		struct MUI_RawimageData *rdata;

		if((rdata = (struct MUI_RawimageData *)GetTagData(MUIA_Rawimage_Data, (ULONG)NULL, msg->ops_AttrList)) != NULL)
		{
			if(setRawimage(cl, obj, rdata) == FALSE)
			{
				CoerceMethod(cl, obj, OM_DISPOSE);
				obj = NULL;
			}
		}
	}

	return((ULONG)obj);
}


static ULONG ASM Rawimage_Set(REG(a0, struct IClass *cl), REG(a2, Object *obj), REG(a1, struct opSet *msg))
{
	struct MUI_RawimageData *rdata;

	if((rdata = (struct MUI_RawimageData *)GetTagData(MUIA_Rawimage_Data, (ULONG)NULL, msg->ops_AttrList)) != NULL)
	{
		setRawimage(cl, obj, rdata);
	}

	return DoSuperMethodA(cl, obj, (Msg)msg);
}


DISPATCHER(_Dispatcher)
{
	switch (msg->MethodID)
	{
		case OM_NEW             : return(Rawimage_New        (cl,obj,(APTR)msg));
		case OM_SET             : return(Rawimage_Set        (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,msg));
}

