// NewGroup.h

#ifndef NEWGROUP_H
#define NEWGROUP_H

#define MUIA_NewGroup_Width			(TAG_USER+1+0x8000000) //I--
#define MUIA_NewGroup_Height		(TAG_USER+2+0x8000000) //I--
#define MUIA_NewGroup_Left			(TAG_USER+3+0x8000000) //I-- -> IS-
#define MUIA_NewGroup_Top			(TAG_USER+4+0x8000000) //I-- -> IS-
#define MUIA_NewGroup_ChildWidth	(TAG_USER+5+0x8000000) //I--
#define MUIA_NewGroup_ChildHeight	(TAG_USER+6+0x8000000) //I--
#define MUIA_NewGroup_FixWidth		(TAG_USER+7+0x8000000) //I--
#define MUIA_NewGroup_FixHeight		(TAG_USER+8+0x8000000) //I--

#define MUIM_NewGroup_Layout		(TAG_USER+9+0x8000000) //I-- /* void */

struct MUIP_PlaceObject
{
	long id;
	long x,y;
	long width,height;
};

#define MUIM_PlaceObject 0x8042845B

void fail(APTR,char *);

#endif
