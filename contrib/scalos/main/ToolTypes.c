// ToolTypes.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static size_t CountToolTypeEntries(CONST_STRPTR *ToolTypeArray);
static size_t GetToolTypeLength(CONST_STRPTR *ToolTypeArray);
static STRPTR *scaFindToolType(STRPTR *ToolTypeArray, CONST_STRPTR typeName);

//----------------------------------------------------------------------------

LONG SetToolType(Object *iconObj, CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeValue, BOOL SaveIcon)
{
	LONG Result = RETURN_OK;
	STRPTR newTT = NULL;
	STRPTR *ttClone = NULL;

	d1(kprintf("%s/%s/%ld: iconObj=%08lx  ttName=<%s>  ttValue=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iconObj, ToolTypeName, ToolTypeValue));

	do	{
		STRPTR *ToolTypeArray = NULL;
		STRPTR *ptt;
		STRPTR iconName;
		size_t length;

		if (NULL == iconObj)
			break;

		GetAttr(IDTA_ToolTypes, iconObj, (APTR) &ToolTypeArray);
		GetAttr(DTA_Name, iconObj, (APTR) &iconName);

		d1(kprintf("%s/%s/%ld: ToolTypeArray=%08lx  iconName=<%s>\n", \
			__FILE__, __FUNC__, __LINE__, ToolTypeArray, iconName));

		// ToolTypeArray may be NULL if there are no tooltypes attached to the icon!

		d1(for (ptt=ToolTypeArray; *ptt; ptt++)\
			kprintf("%s/%s/%ld: old ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, *ptt));

		length = 1 + strlen(ToolTypeName) + 1 + strlen(ToolTypeValue);
		newTT = (STRPTR) ScalosAlloc(length);
		if (NULL == newTT)
			break;

		if (strlen(ToolTypeValue) > 0)
			snprintf(newTT, length, "%s=%s", ToolTypeName, ToolTypeValue);
		else
			stccpy(newTT, ToolTypeName, length);

		ttClone = CloneToolTypeArray((CONST_STRPTR *) ToolTypeArray, 1);
		if (NULL == ttClone)
			break;

		ptt = scaFindToolType(ttClone, ToolTypeName);
		if (ptt)
			{
			// overwrite old tooltype
			d1(kprintf("%s/%s/%ld: ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, *ptt));
			*ptt = newTT;
			}
		else
			{
			// append new tooltype
			for (ptt=ttClone; *ptt; ptt++)
				;

			*ptt = newTT;
			}

		for (ptt=ttClone; *ptt; ptt++)
			d1(kprintf("%s/%s/%ld: new ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, *ptt));

		SetAttrs(iconObj, 
			IDTA_ToolTypes, (ULONG) ttClone,
			TAG_END);

		if (SaveIcon)
			{
			Result = PutIconObjectTags(iconObj, iconName,
				ICONA_NoNewImage, TRUE,
				TAG_END);
			}
		} while (0);

	if (newTT)
		ScalosFree(newTT);
	if (ttClone)
		ScalosFree(ttClone);

	return Result;
}

//----------------------------------------------------------------------------

LONG RemoveToolType(Object *iconObj, CONST_STRPTR ToolTypeName, BOOL SaveIcon)
{
	LONG Result = RETURN_OK;
	STRPTR *ttClone = NULL;

	d1(kprintf("%s/%s/%ld: iconObj=%08lx  ttName=<%s>  ttValue=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iconObj, ToolTypeName, ToolTypeValue));

	do	{
		STRPTR *ToolTypeArray = NULL;
		STRPTR *ptt;
		STRPTR iconName;

		if (NULL == iconObj)
			break;

		GetAttr(IDTA_ToolTypes, iconObj, (APTR) &ToolTypeArray);
		GetAttr(DTA_Name, iconObj, (APTR) &iconName);

		d1(kprintf("%s/%s/%ld: ToolTypeArray=%08lx  iconName=<%s>\n", \
			__FILE__, __FUNC__, __LINE__, ToolTypeArray, iconName));

		if (NULL == ToolTypeArray)
			break;

		for (ptt=ToolTypeArray; *ptt; ptt++)
			d1(kprintf("%s/%s/%ld: old ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, *ptt));

		ttClone = CloneToolTypeArray((CONST_STRPTR *) ToolTypeArray, 0);
		if (NULL == ttClone)
			break;

		ptt = scaFindToolType(ttClone, ToolTypeName);
		if (ptt)
			{
			// old tooltype found - remove it
			d1(kprintf("%s/%s/%ld: ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, *ptt));
			while (*ptt)
				{
				*ptt = ptt[1];
				if (NULL == *ptt)
					break;
				ptt++;
				}

			d1(for (ptt=ttClone; *ptt; ptt++) \
				kprintf("%s/%s/%ld: new ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, *ptt));

			SetAttrs(iconObj, 
				IDTA_ToolTypes, (ULONG) ttClone,
				TAG_END);

			if (SaveIcon)
				{
				Result = PutIconObjectTags(iconObj, iconName,
					ICONA_NoNewImage, TRUE,
					TAG_END);
				}
			}
		} while (0);

	if (ttClone)
		ScalosFree(ttClone);

	return Result;
}

//----------------------------------------------------------------------------

static size_t CountToolTypeEntries(CONST_STRPTR *ToolTypeArray)
{
	size_t ttCount;

	for (ttCount=0; ToolTypeArray && ToolTypeArray[ttCount]; ttCount++);

	return ttCount;
}

//----------------------------------------------------------------------------

static size_t GetToolTypeLength(CONST_STRPTR *ToolTypeArray)
{
	size_t Length = sizeof(STRPTR);

	while (ToolTypeArray && *ToolTypeArray)
		{
		Length += sizeof(STRPTR) + strlen(*ToolTypeArray) + 1;
		ToolTypeArray++;
		}

	return Length;
}

//----------------------------------------------------------------------------

STRPTR *CloneToolTypeArray(CONST_STRPTR *ToolTypeArray, ULONG AdditionalEntries)
{
	size_t ttLength;
	STRPTR *newTT, *newTTalloc;
	STRPTR stringSpace;
	ULONG ttCount;

	d1(kprintf("%s/%s/%ld: AdditionalEntries=%ld\n", __FILE__, __FUNC__, __LINE__, AdditionalEntries));

	ttLength = GetToolTypeLength(ToolTypeArray) + AdditionalEntries * sizeof(STRPTR);
	ttCount = CountToolTypeEntries(ToolTypeArray);

	d1(kprintf("%s/%s/%ld: ttLength=%ld  ttCount=%ld\n", __FILE__, __FUNC__, __LINE__, ttLength, ttCount));

	newTT = newTTalloc = ScalosAlloc(ttLength);
	if (NULL == newTT)
		return NULL;

	stringSpace = (STRPTR) &newTT[1 + ttCount + AdditionalEntries];

	d1(kprintf("%s/%s/%ld: newTT=%08lx  stringSpace=%08lx\n", __FILE__, __FUNC__, __LINE__, newTT, stringSpace));

	while (ToolTypeArray && *ToolTypeArray)
		{
		*newTT = stringSpace;

		strcpy(stringSpace, *ToolTypeArray);
		stringSpace += strlen(stringSpace) + 1;

		newTT++;
		ToolTypeArray++;
		}

	while (AdditionalEntries--)
		*newTT++ = NULL;

	*newTT = NULL;

	return newTTalloc;
}

//----------------------------------------------------------------------------

static STRPTR *scaFindToolType(STRPTR *ToolTypeArray, CONST_STRPTR typeName)
{
	if (NULL == ToolTypeArray)
		return NULL;

	while (*ToolTypeArray)
		{
		STRPTR lp = strchr(*ToolTypeArray, '=');
		STRPTR CompEnd;
		size_t CompLen;

		if (lp)
			{
			CompEnd = lp;

			if (CompEnd != *ToolTypeArray)
				CompEnd--;		// skip "="
			while (CompEnd != *ToolTypeArray && ' ' == *CompEnd)
				CompEnd--;		// skip all " " before "="

//			lp = stpblk(++lp);		// skip "=" and trailing blanks

			CompLen = CompEnd - *ToolTypeArray;
			}
		else
			{
			CompLen = strlen(*ToolTypeArray);
//			lp = (STRPTR) "";
			}

		if (0 == Strnicmp(*ToolTypeArray, (STRPTR) typeName, CompLen))
			{
			return ToolTypeArray;
			}

		ToolTypeArray++;
		}

	return NULL;
}

//----------------------------------------------------------------------------


