/* $Revision Header * Header built automatically - do not edit! *************
 *
 *	(C) Copyright 1990 by MXM
 *
 *	Name .....: RemapIcon.c
 *	Created ..: Tuesday 26-Jun-90 14:19
 *	Revision .: 1
 *
 *	Date            Author          Comment
 *	=========       ========        ====================
 *	26-Jun-90       Olsen           Created this file!
 *
 * $Revision Header ********************************************************/

	/* Included header files. */

#define __NO_PRAGMAS 1

#include <aros/oldprograms.h>

#include <workbench/workbench.h>
#include <libraries/dosextens.h>
#include <libraries/arpbase.h>
#include <functions.h>

	/* Arp cli variables. */

char *CLI_Template	= "From,To,I=Info/S";
char *CLI_Help		= "\nUsage: \33[1mIcon2C\33[0m [Source file name without \".info\"] [Destination file] [Info]\n";

	/* Argument vector offsets. */

#define ARG_FROM	1
#define ARG_TO		2
#define ARG_INFO	3

	/* Icon library base. */

struct Library	*IconBase;

	/* Image2C(struct Image *IconImage,char *Name,BYTE Number,BPTR ImageFile):
	 *
	 *	Converts an image into 'C' sourcecode.
	 */

VOID
Image2C(struct Image *IconImage,char *Name,BYTE Number,BPTR ImageFile)
{
	SHORT	 i,j,LastWord;
	USHORT	*ImageData = IconImage -> ImageData;;

		/* Remember last word to be written. */

	LastWord = ((IconImage -> Width + 15) >> 4) * IconImage -> Height;

		/* Write the header. */

	FPrintf(ImageFile,"USHORT %s%ldData[%ld] =\n{",Name,Number,LastWord * IconImage -> Depth);

		/* Write the bitmap information contained in the
		 * single bit planes.
		 */

	for(j = 0 ; j < IconImage -> Depth ; j++)
	{
		for(i = 0 ; i < LastWord ; i++)
		{
			if(!(i % 8))
				FPrintf(ImageFile,"\n\t");

			FPrintf(ImageFile,"0x%04lx",*ImageData++);

			if(i != LastWord - 1)
				FPrintf(ImageFile,",");
		}

		if(j != IconImage -> Depth - 1)
			FPrintf(ImageFile,",\n");
	}

		/* Add the image structure itself. */

	FPrintf(ImageFile,"\n};\n\n");

	FPrintf(ImageFile,"struct Image %s%ldImage =\n{\n",Name,Number);

	FPrintf(ImageFile,"\t%ld,%ld,\n",IconImage -> LeftEdge,IconImage -> TopEdge);
	FPrintf(ImageFile,"\t%ld,%ld,%ld,\n",IconImage -> Width,IconImage -> Height,IconImage -> Depth);
	FPrintf(ImageFile,"\t(USHORT *)&%s%ldData[0],\n",Name,Number);

	FPrintf(ImageFile,"\t0x%02lx,0x00,\n",(1 << IconImage -> Depth) - 1);
	FPrintf(ImageFile,"\t(struct Image *)NULL\n");

	FPrintf(ImageFile,"};\n\n");
}

	/* Icon2C(struct DiskObject *Icon,char *Name,BPTR IconFile):
	 *
	 *	Convert an icon file into 'C' sourcecode, creates
	 *	image run dumps and associated icon structures.
	 */

VOID
Icon2C(struct DiskObject *Icon,char *Name,BPTR IconFile)
{
	STATIC char *WB_Types[7] =
	{
		"WBDISK",
		"WBDRAWER",
		"WBTOOL",
		"WBPROJECT",
		"WBGARBAGE",
		"WBDEVICE",
		"WBKICK"
	};

	struct DrawerData	 *Drawer = Icon -> do_DrawerData;
	char			**ToolTypes = Icon -> do_ToolTypes;

	Puts("\n\n\tCreating icon image...");

		/* Create the header. */

	FPrintf(IconFile,"#include <workbench/workbench.h>\n\n");

		/* Create the first icon image. */

	Image2C((struct Image *)Icon -> do_Gadget . GadgetRender,Name,1,IconFile);

		/* Create alternate icon image (if present). */

	if(Icon -> do_Gadget . Flags & GADGHIMAGE)
	{
		Puts("\tCreating alternate icon image...");
		Image2C((struct Image *)Icon -> do_Gadget . SelectRender,Name,2,IconFile);
	}

		/* Create the drawer data if neccessary. */

	if(Drawer && (Icon -> do_Type == WBDISK || Icon -> do_Type == WBDRAWER || Icon -> do_Type == WBGARBAGE))
	{
		Puts("\tCreating icon drawer data...");

		FPrintf(IconFile,"struct DrawerData %sDrawer =\n{\n",Name);
		FPrintf(IconFile,"\t%ld,%ld,\n",Drawer -> dd_NewWindow . LeftEdge,Drawer -> dd_NewWindow . TopEdge);
		FPrintf(IconFile,"\t%ld,%ld,\n",Drawer -> dd_NewWindow . Width,Drawer -> dd_NewWindow . Height);
		FPrintf(IconFile,"\t%ld,%ld,\n",Drawer -> dd_NewWindow . DetailPen,Drawer -> dd_NewWindow . BlockPen);
		FPrintf(IconFile,"\t0x%08lx,\n",Drawer -> dd_NewWindow . IDCMPFlags);
		FPrintf(IconFile,"\t0x%08lx,\n",Drawer -> dd_NewWindow . Flags);
		FPrintf(IconFile,"\t(struct Gadget *)NULL,\n");
		FPrintf(IconFile,"\t(struct Image *)NULL,\n");

		if(Drawer -> dd_NewWindow . Title)
			FPrintf(IconFile,"\t(UBYTE *)\"%s\",\n",Drawer -> dd_NewWindow . Title);
		else
			FPrintf(IconFile,"\t(UBYTE *)NULL,\n");

		FPrintf(IconFile,"\t(struct Screen *)NULL,\n");
		FPrintf(IconFile,"\t(struct BitMap *)NULL,\n");
		FPrintf(IconFile,"\t%ld,%ld,\n",Drawer -> dd_NewWindow . MinWidth,Drawer -> dd_NewWindow . MinHeight);
		FPrintf(IconFile,"\t%ld,%ld,\n",Drawer -> dd_NewWindow . MaxWidth,Drawer -> dd_NewWindow . MaxHeight);
		FPrintf(IconFile,"\t%ld,\n\n",Drawer -> dd_NewWindow . Type);

		FPrintf(IconFile,"\t%ld,\n",Drawer -> dd_CurrentX);
		FPrintf(IconFile,"\t%ld,\n",Drawer -> dd_CurrentY);

		FPrintf(IconFile,"#ifdef OLDDRAWERDATAFILESIZE\n");
		FPrintf(IconFile,"\t0x%08lx,\n",Drawer -> dd_Flags);
		FPrintf(IconFile,"\t0x%04lx\n",Drawer -> dd_ViewModes);
		FPrintf(IconFile,"#endif\t/* OLDDRAWERDATAFILESIZE */\n");
		FPrintf(IconFile,"};\n\n");
	}

		/* Create icon tool types. */

	if(ToolTypes && *ToolTypes)
	{
		Puts("\tCreating icon tool types...");

		FPrintf(IconFile,"char *%sToolTypes[] =\n{\n",Name);

		while(*ToolTypes)
			FPrintf(IconFile,"\t\"%s\",\n",*ToolTypes++);

		FPrintf(IconFile,"\tNULL\n};\n\n");
	}

	Puts("\tCreating icon object...");

		/* Create the icon structure. */

	FPrintf(IconFile,"struct DiskObject %sIcon =\n{\n",Name);
	FPrintf(IconFile,"\tWB_DISKMAGIC,\n");
	FPrintf(IconFile,"\tWB_DISKVERSION,\n\n");

	FPrintf(IconFile,"\t(struct Gadget *)NULL,\n");
	FPrintf(IconFile,"\t%ld,%ld,\n",Icon -> do_Gadget . LeftEdge,Icon -> do_Gadget . TopEdge);
	FPrintf(IconFile,"\t%ld,%ld,\n",Icon -> do_Gadget . Width,Icon -> do_Gadget . Height);

	FPrintf(IconFile,"\t0x%04lx,\n",Icon -> do_Gadget . Flags);
	FPrintf(IconFile,"\t0x%04lx,\n",Icon -> do_Gadget . Activation);
	FPrintf(IconFile,"\t0x%04lx,\n",Icon -> do_Gadget . GadgetType);

	FPrintf(IconFile,"\t(APTR)&%s1Image,\n",Name);

		/* Did it have an alternate image? */

	if(Icon -> do_Gadget . Flags & GADGHIMAGE)
		FPrintf(IconFile,"\t(APTR)&%s2Image,\n",Name);
	else
		FPrintf(IconFile,"\t(APTR)NULL,\n");

	FPrintf(IconFile,"\t(struct IntuiText *)NULL,\n");
	FPrintf(IconFile,"\tNULL,\n");
	FPrintf(IconFile,"\t(APTR)NULL,\n");
	FPrintf(IconFile,"\tNULL,\n");
	FPrintf(IconFile,"\t(APTR)NULL,\n\n");

	FPrintf(IconFile,"\t%s,\n",WB_Types[Icon -> do_Type - 1]);

		/* Create default tool. */

	if(Icon -> do_DefaultTool)
		FPrintf(IconFile,"\t\"%s\",\n",Icon -> do_DefaultTool);
	else
		FPrintf(IconFile,"\t(char *)NULL,\n");

		/* Add the tool types if necessary. */

	if(Icon -> do_ToolTypes && *Icon -> do_ToolTypes)
		FPrintf(IconFile,"\t%sToolTypes,\n",Name);
	else
		FPrintf(IconFile,"\tNULL,\n");

	FPrintf(IconFile,"\tNO_ICON_POSITION,\n");
	FPrintf(IconFile,"\tNO_ICON_POSITION,\n");

		/* Add the drawerdata. */

	if(Drawer && (Icon -> do_Type == WBDISK || Icon -> do_Type == WBDRAWER || Icon -> do_Type == WBGARBAGE))
		FPrintf(IconFile,"\t(struct DrawerData *)&%sDrawer,\n",Name);
	else
		FPrintf(IconFile,"\t(struct DrawerData *)NULL,\n");

	if(Icon -> do_ToolWindow)
		FPrintf(IconFile,"\t\"%s\",\n",Icon -> do_ToolWindow);
	else
		FPrintf(IconFile,"\t(char *)NULL,\n");

	FPrintf(IconFile,"\t%ld\n};\n\n",Icon -> do_StackSize);

		/* At last, add a macro to save the icon. */

	FPrintf(IconFile,"#define Save%sIcon(Destination) PutDiskObject(Destination,&%sIcon)\n",Name,Name);
}

	/* main(int argc,char **argv):
	 *
	 *	The main program.
	 */

VOID
main(int argc,char **argv)
{
	BYTE ErrorCode = RETURN_FAIL;

	Puts("\n\33[1m\33[33mIcon2C V1.2 \33[0m\33[31mCopyright © 1990 by MXM, all rights reserved\n");

	if(argv[ARG_INFO])
	{
		Puts("This  program  converts  icon  info  files into standard 'C'");
		Puts("source code.\n");

		Puts("Author: Olaf Barthel, MXM");
		Puts("        Brabeckstrasse 35");
		Puts("        D-3000 Hannover 71\n");

		Puts("        Federal Republic of Germany\n");

		Puts("Written at Hannover, Monday 25-Jun-90.\n");

		Puts("This  program  is  Share-Ware,  if  you  like  it and use it");
		Puts("frequently  a  small  donation  will  entitle you to receive");
		Puts("updates and new programs from MXM.\n");

		exit(RETURN_OK);
	}

	if(!argv[ARG_FROM] || !argv[ARG_TO])
	{
		Puts(CLI_Help);
		exit(RETURN_WARN);
	}

	Printf("Opening icon.library... ");

	if((IconBase = (struct Library *)ArpOpenLibrary("icon.library",0)))
	{
		struct DiskObject	*IconData;
		BPTR			 IconFile;

		Printf(" Reading icon file... ");

		if((IconData = GetDiskObject(argv[ARG_FROM])))
		{
			Printf("creating output file... ");

			if(IconFile = ArpOpen(argv[ARG_TO],MODE_NEWFILE))
			{
				Icon2C(IconData,BaseName(argv[ARG_FROM]),IconFile);

				ErrorCode = RETURN_OK;

				Puts("\nIcon file created OK!\n");
			}
			else
				Printf("\33[1mERROR\33[0m (%ld): cannot create output file!\a\n",IoErr());

			FreeDiskObject(IconData);
		}
		else
			Printf("\33[1mERROR\33[0m (%ld): cannot read icon file!\a\n",IoErr());
	}
	else
		Printf("\33[1mERROR\33[0m (%ld): cannot open icon.library!\a\n",IoErr());

	exit(ErrorCode);
}
