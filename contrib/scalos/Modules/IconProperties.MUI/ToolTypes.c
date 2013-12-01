// ToolTypes.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <libraries/mui.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include "defs.h"
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "ToolTypes.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

static size_t CountToolTypeEntries(CONST_STRPTR *ToolTypeArray);
static size_t GetToolTypeLength(CONST_STRPTR *ToolTypeArray);
STRPTR *CloneToolTypeArray(CONST_STRPTR *ToolTypeArray, ULONG AdditionalEntries);
static STRPTR *scaFindToolType(STRPTR *ToolTypeArray, CONST_STRPTR typeName);

//----------------------------------------------------------------------------

LONG SetToolType(Object *iconObj, CONST_STRPTR ToolTypeName, CONST_STRPTR ToolTypeValue)
{
	LONG Result = RETURN_OK;
	STRPTR newTT = NULL;
	STRPTR *ttClone = NULL;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: iconObj=%08lx  ttName=<%s>  ttValue=<%s>\n", \
		__LINE__, iconObj, ToolTypeName, ToolTypeValue));

	do	{
		STRPTR *ToolTypeArray = NULL;
		STRPTR *ptt;
		STRPTR iconName = NULL;

		if (NULL == iconObj)
			break;

		//GetAttr(IDTA_ToolTypes, iconObj, (ULONG *) &ToolTypeArray);
		//GetAttr(DTA_Name, iconObj, (ULONG *) &iconName);
		get(iconObj, IDTA_ToolTypes, &ToolTypeArray);
		get(iconObj, DTA_Name, &iconName);

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ToolTypeArray=%08lx  iconName=<%s>\n", \
			__LINE__, ToolTypeArray, iconName));

		d1(for (ptt=ToolTypeArray; *ptt; ptt++)\
			kprintf(__FILE__ "/" __FUNC__ "/%ld: old ToolType=<%s>\n", __LINE__, *ptt));

		newTT = (STRPTR) malloc(1 + strlen(ToolTypeName) + 1 + strlen(ToolTypeValue));
		if (NULL == newTT)
			break;

		if (strlen(ToolTypeValue) > 0)
			sprintf(newTT, "%s=%s", ToolTypeName, ToolTypeValue);
		else
			strcpy(newTT, ToolTypeName);

		ttClone = CloneToolTypeArray((CONST_STRPTR *) ToolTypeArray, 1);
		if (NULL == ttClone)
			break;

		ptt = scaFindToolType(ttClone, ToolTypeName);
		if (ptt)
			{
			// overwrite old tooltype
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ToolType=<%s>\n", __LINE__, *ptt));
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
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: new ToolType=<%s>\n", __LINE__, *ptt));

		SetAttrs(iconObj, 
			IDTA_ToolTypes, ttClone,
			TAG_END);

		PutIconObjectTags(iconObj, iconName,
			ICONA_NoNewImage, TRUE,
			TAG_END);
		} while (0);

	if (newTT)
		free(newTT);
	if (ttClone)
		free(ttClone);

	return Result;
}

//----------------------------------------------------------------------------

LONG RemoveToolType(Object *iconObj, CONST_STRPTR ToolTypeName)
{
	LONG Result = RETURN_OK;
	STRPTR *ttClone = NULL;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: iconObj=%08lx  ttName=<%s>  ttValue=<%s>\n", \
		__LINE__, iconObj, ToolTypeName, ToolTypeValue));

	do	{
		STRPTR *ToolTypeArray = NULL;
		STRPTR *ptt;
		STRPTR iconName;

		if (NULL == iconObj)
			break;

		GetAttr(IDTA_ToolTypes, iconObj, (APTR) &ToolTypeArray);
		GetAttr(DTA_Name, iconObj, (APTR) &iconName);

		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ToolTypeArray=%08lx  iconName=<%s>\n", \
			__LINE__, ToolTypeArray, iconName));

		if (NULL == ToolTypeArray)
			break;

		d1(for (ptt=ToolTypeArray; ptt && *ptt; ptt++)\
			kprintf(__FILE__ "/" __FUNC__ "/%ld: old ToolType=<%s>\n", __LINE__, *ptt));

		ttClone = CloneToolTypeArray((CONST_STRPTR *) ToolTypeArray, 0);
		if (NULL == ttClone)
			break;

		ptt = scaFindToolType(ttClone, ToolTypeName);
		if (ptt)
			{
			// old tooltype found - remove it
			d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ToolType=<%s>\n", __LINE__, *ptt));
			while (*ptt)
				{
				*ptt = ptt[1];
				if (NULL == *ptt)
					break;
				ptt++;
				}

			d1(for (ptt=ttClone; *ptt; ptt++) \
				kprintf(__FILE__ "/" __FUNC__ "/%ld: new ToolType=<%s>\n", __LINE__, *ptt));

			SetAttrs(iconObj, 
				IDTA_ToolTypes, ttClone,
				TAG_END);

			PutIconObjectTags(iconObj, iconName,
				ICONA_NoNewImage, TRUE,
				TAG_END);
			}
		} while (0);

	if (ttClone)
		free(ttClone);

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

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: AdditionalEntries=%ld\n", __LINE__, AdditionalEntries));

	ttLength = GetToolTypeLength(ToolTypeArray) + AdditionalEntries * sizeof(STRPTR);
	ttCount = CountToolTypeEntries(ToolTypeArray);

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: ttLength=%ld  ttCount=%ld\n", __LINE__, ttLength, ttCount));

	newTT = newTTalloc = malloc(ttLength);
	if (NULL == newTT)
		return NULL;

	stringSpace = (STRPTR) &newTT[1 + ttCount + AdditionalEntries];

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: newTT=%08lx  stringSpace=%08lx\n", __LINE__, newTT, stringSpace));

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

		if (0 == Strnicmp(*ToolTypeArray, typeName, CompLen))
			{
			return ToolTypeArray;
			}

		ToolTypeArray++;
		}

	return NULL;
}

//----------------------------------------------------------------------------

LONG CmpToolTypeArrays(CONST_STRPTR *ToolTypeArray1, CONST_STRPTR *ToolTypeArray2)
{
	while (*ToolTypeArray1 && *ToolTypeArray2)
		{
		int rc = strcmp(*ToolTypeArray1, *ToolTypeArray2);

		if (0 != rc)
			return (LONG) rc;

		ToolTypeArray1++;
		ToolTypeArray2++;
		}

	if (*ToolTypeArray1)
		return 1;
	else if (*ToolTypeArray2)
		return -1;

	return 0;
}

//----------------------------------------------------------------------------

