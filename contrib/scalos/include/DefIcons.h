// DefIcons.h
// $Date$
// $Revision$

#ifndef	DEFICONS_H_INCLUDED
#define	DEFICONS_H_INCLUDED


#if defined(__GNUC__) && !defined(mc68000)
#pragma pack(2)
#endif /* __GNUC__ */


#define	IS_TYPENODE(icontype)   (MEMF_PUBLIC & TypeOfMem((void *) (icontype)))

#define ACT_MATCH		1	/* arg1/2 = offset arg3 = length str = string */
#define ACT_SEARCH		2	/* arg2 = length str = string */
#define ACT_SEARCHSKIPSPACES	3	/* arg2 = length str = string */
/* if length < 0, do a case unsensitive match */

#define ACT_FILESIZE		4	/* arg2 = file size */
#define ACT_NAMEPATTERN		5	/* str = filename pattern */
#define ACT_PROTECTION		6	/* arg1 = mask arg2 = protbits&mask */
#define ACT_OR			7	/* an alternative description follows */
#define ACT_ISASCII		8	/* this is used only by AsciiType */

#define ACT_DOSDEVICE		10	/* str = DOS device name like "CD0" or "DH0" */
#define	ACT_DEVICENAME		11      /* str = device name like "scsi.device" */
#define	ACT_CONTENTS		12	/* str = pattern match for device contents */
#define	ACT_MINSIZEMB		13	/* arg2 = minimum device size in MBytes */
#define	ACT_DOSTYPE		14      /* arg2 = length str = match string  for DOS type like "DOS\0" */

#define	ACT_MACROCLASS		20	/* this must be the ONLY ACT_xxx of the node. */
					/* (apart from ACT_END) */
					/* following descriptions are son of this one, but */
					/* this one will never be considered valid, i.e. */
					/* if none of the sons matches the file, we will */
					/* proceed with the following description. The parent */
					/* will be used only for icon picking purposes (e.g. */
					/* a class "picture" may contain "gif", "jpeg" and so */
					/* on, and if def_gif is missing def_picture will be used */
#define ACT_END			0



#define TYPE_DOWN_LEVEL		1	/* following description is son of previous one */
#define TYPE_UP_LEVEL		2	/* following description is brother of parent of previous one */
#define TYPE_END		0	/* end of list */

struct Magic
{
	UBYTE action;
	BYTE pad;
	LONG arg1;
	LONG arg2;
	STRPTR str;
	/* note: WORD aligned */
};


struct TypeNode
{
	struct TypeNode *tn_RightBrother;
	struct TypeNode *tn_FirstSon;
	struct TypeNode *tn_Parent;
	Object *tn_IconObject;
	STRPTR tn_Name;
	struct Magic tn_Description[1];
};

#if defined(__GNUC__) && !defined(mc68000)
#pragma pack()
#endif /* __GNUC__ */

#endif /* DEFICONS_H_INCLUDED */
