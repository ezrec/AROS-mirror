// LoadPngIcon.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <intuition/classes.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>

#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <clib/alib_protos.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "png.h"
#include <defs.h>
#include "PNGIconDt.h"
#include "PNGIconProto.h"

//-----------------------------------------------------------------------------

static void DetermineIconType(struct InstanceData *inst, CONST_STRPTR filename, 
	const struct WBArg *ObjectLocation, struct FileInfoBlock *fib);
static void GuessWbDrawer(struct InstanceData *inst);

//-----------------------------------------------------------------------------

BOOL LoadPngIcon(struct InstanceData *inst, CONST_STRPTR filename, BPTR IconFh, const struct WBArg *ObjectLocation)
{
	STRPTR namedotinfo = NULL;
	BPTR file = BNULL;
	struct FileInfoBlock *fib = NULL;
	struct DiskObject *DiskObj = NULL;
	BOOL success = FALSE;


	d1(kprintf("%s/%s/%ld:  inst=%08lx  ObjectLocation=%08lx\n", __FILE__, __FUNC__, __LINE__, inst, ObjectLocation));

	do	{
		UBYTE id[8];
		LONG Length;

		// mark DrawerData as invalid
		inst->id_DrawerData.dd_NewWindow.Width = inst->id_DrawerData.dd_NewWindow.Height = 0;

		if (IconFh)
			{
			d1(kprintf("%s/%s/%ld:  IconFh=%08lx\n", __FILE__, __FUNC__, __LINE__, IconFh));
			file = IconFh;
			}
		else
			{
			// Build "filename.info"

			namedotinfo = MyAllocVecPooled(1 + 5 + strlen(filename));
			d1(kprintf("%s/%s/%ld:  namedotinfo=%08lx\n", __FILE__, __FUNC__, __LINE__, namedotinfo));
			if (NULL == namedotinfo)
				break;

			strcpy(namedotinfo, filename);
			strcat(namedotinfo, ".info");

			d1(kprintf("%s/%s/%ld:  <%s>\n", __FILE__, __FUNC__, __LINE__, namedotinfo));

			// Try to open the icon file

			file = Open(namedotinfo, MODE_OLDFILE);

			d1(KPrintF("%s/%s/%ld:  <%s>\n", __FILE__, __FUNC__, __LINE__, namedotinfo));
			MyFreeVecPooled(namedotinfo);
			namedotinfo = NULL;
			}

		d1(kprintf("%s/%s/%ld:  file=%08lx\n", __FILE__, __FUNC__, __LINE__, file));
		if (BNULL == file)
			break;

		// Immediately Check if it's a PNG

		Length = FRead(file, (APTR) id, 1, sizeof(id));
		d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (sizeof(id) != Length)
			break;

		d1(kprintf("%s/%s/%ld:  id=%02lx %02lx %02lx %02lx\n", __FILE__, __FUNC__, __LINE__, id[0], id[1], id[2], id[3]));

		/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
		      break if they match */
		if (0 != png_sig_cmp((png_bytep) id, (png_size_t) 0, sizeof(id)))
			{
			d1(kprintf("%s/%s/%ld:  png_sig_cmp failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		fib = AllocDosObject(DOS_FIB, NULL);
		d1(kprintf("%s/%s/%ld:  fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));
		if (NULL == fib)
			break;

		DetermineIconType(inst, filename, ObjectLocation, fib);

		d1(KPrintF("%s/%s/%ld:  id_Type=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));

		if (!GetPngPicture(inst, file, sizeof(id), &inst->id_NormalImage))
			{
			d1(KPrintF("%s/%s/%ld:  GetPngPicture failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld:  Normal Width=%ld  Height=%ld\n",
			__FILE__, __FUNC__, __LINE__, inst->id_NormalImage.argb_Width, inst->id_NormalImage.argb_Height));

		// check if second PNG image is present
		do	{
			Length = FRead(file, (APTR) id, 1, sizeof(id));
			d1(KPrintF("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
			if (sizeof(id) != Length)
				break;

			d1(KPrintF("%s/%s/%ld:  id=%08lx %08lx\n", __FILE__, __FUNC__, __LINE__, id[0], id[1]));

			/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
			      break if they don't match */
			if (0 != png_sig_cmp((png_bytep) id, (png_size_t) 0, sizeof(id)))
				break;

			if (!GetPngPicture(inst, file, sizeof(id), &inst->id_SelectedImage))
				break;

			d1(KPrintF("%s/%s/%ld:  Selected Width=%ld  Height=%ld\n",
				__FILE__, __FUNC__, __LINE__, inst->id_SelectedImage.argb_Width, inst->id_SelectedImage.argb_Height));
			} while (0);

		switch (inst->id_Type)
			{
		case WBDISK:
		case WBDRAWER:
		case WBGARBAGE:
			if (0 == inst->id_DrawerData.dd_NewWindow.Width
				|| 0 == inst->id_DrawerData.dd_NewWindow.Height)
				{
				// if we have no valid DrawerData
				DiskObj = GetDefDiskObject(inst->id_Type);
				d1(kprintf("%s/%s/%ld:  DiskObj=%08lx\n", __FILE__, __FUNC__, __LINE__, DiskObj));
				if (DiskObj && DiskObj->do_DrawerData)
					{
					// use icon.library defaults for our DrawerData
					d1(kprintf("%s/%s/%ld:  do_DrawerData=%08lx\n", __FILE__, __FUNC__, __LINE__, DiskObj->do_DrawerData));
					inst->id_DrawerData = *DiskObj->do_DrawerData;
					}
				}
			d1(kprintf("%s/%s/%ld:   CurrentX=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_CurrentX));
			d1(kprintf("%s/%s/%ld:   CurrentY=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_CurrentY));
			d1(kprintf("%s/%s/%ld:      Flags=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_Flags));
			d1(KPrintF("%s/%s/%ld:  ViewModes=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_ViewModes));
			break;
		default:
			break;
			}

		success = TRUE;
	} while (0);

	d1(kprintf("%s/%s/%ld:  success=%ld\n", __FILE__, __FUNC__, __LINE__, success));

	if (DiskObj)
		FreeDiskObject(DiskObj);
	if (fib)
		FreeDosObject(DOS_FIB, fib);
	if (file && (BNULL == IconFh))
		Close(file);
	if (namedotinfo)
		MyFreeVecPooled(namedotinfo);

	return success;
}


void HandleIconHunk(struct InstanceData *inst, const UBYTE *Data, size_t Length)
{
	if (!inst->id_IconHunkLoaded)
		{
		ULONG ttlength, dtlength;
		ULONG ttTotalLength;
		ULONG ttCount;
		const ULONG *dataptr = (const ULONG *) Data;

		ttTotalLength = 0;
		ttCount = 0;

		// Begin scanning the tags

		inst->id_ToolTypesCount = 0;	// Set ToolType counter to zero
		d1(KPrintF("%s/%s/%ld:  dd_ViewModes=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_ViewModes));

		while (Length >= 2 * sizeof(ULONG))
			{
			ULONG tag = SCA_BE2LONG(*dataptr);

			d1(KPrintF("%s/%s/%ld:  dataptr=%08lx  Length=%ld\n", __FILE__, __FUNC__, __LINE__, dataptr, Length));
			dataptr++;
			Length -= sizeof(ULONG);

			switch (tag)
				{
			case PNGICONA_XPOS:
				inst->id_CurrentX = SCA_BE2LONG(*dataptr);
				d1(kprintf("%s/%s/%ld:  CurrentX=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_CurrentX));
				dataptr++;
				Length -= sizeof(ULONG);
				break;
			case PNGICONA_YPOS:
				inst->id_CurrentY = SCA_BE2LONG(*dataptr);
				d1(kprintf("%s/%s/%ld:  CurrentY=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_CurrentY));
				dataptr++;
				Length -= sizeof(ULONG);
				break;

			// One of the following five tags identifies a WBDISK or WBDRAWER icon
			case PNGICONA_DRAWERXPOS:
				inst->id_DrawerData.dd_NewWindow.LeftEdge = SCA_BE2WORD(*dataptr);
				d1(KPrintF("%s/%s/%ld:  LeftEdge=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_NewWindow.LeftEdge));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;
			case PNGICONA_DRAWERYPOS:
				inst->id_DrawerData.dd_NewWindow.TopEdge = SCA_BE2LONG(*dataptr);
				d1(KPrintF("%s/%s/%ld:  TopEdge=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_NewWindow.TopEdge));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;
			case PNGICONA_DRAWERWIDTH:
				inst->id_DrawerData.dd_NewWindow.Width = SCA_BE2LONG(*dataptr);
				d1(KPrintF("%s/%s/%ld:  Width=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_NewWindow.Width));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;
			case PNGICONA_DRAWERHEIGHT:
				inst->id_DrawerData.dd_NewWindow.Height = SCA_BE2LONG(*dataptr);
				d1(KPrintF("%s/%s/%ld:  Height=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_NewWindow.Height));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;
			case PNGICONA_DRAWERVIEWMODE:
				{
				ULONG vm = SCA_BE2LONG(*dataptr);

				d1(KPrintF("%s/%s/%ld: PNGICONA_DRAWERVIEWMODE  ViewModes=%08lx\n", __FILE__, __FUNC__, __LINE__, vm));

				inst->id_DrawerData.dd_Flags = (vm & PNGDM_SHOWALLFILES) ? (DDFLAGS_SHOWALL) : (DDFLAGS_SHOWICONS);
				inst->id_DrawerData.dd_ViewModes = (vm & PNGDM_VIEWBYICON) ? (DDVM_BYICON) :
								(((vm & PNGDM_SORTMODEMASK)>>2)+DDVM_BYNAME);
				d1(KPrintF("%s/%s/%ld:  dd_Flags=%08lx  dd_ViewModes=%08lx\n", __FILE__, __FUNC__,
					__LINE__, inst->id_DrawerData.dd_Flags, inst->id_DrawerData.dd_ViewModes));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				}
				break;

			case PNGICONA_STACKSIZE:
				inst->id_StackSize = SCA_BE2LONG(*dataptr);
				if (inst->id_StackSize < MINSTACKSIZE)
					inst->id_StackSize = MINSTACKSIZE;
				d1(kprintf("%s/%s/%ld:  StackSize=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_StackSize));
				dataptr++;
				Length -= sizeof(ULONG);
				break;

			case PNGICONA_DEFTOOL:
				// This identifies unequivocally a project icon - may be NULL!
				dtlength = 1 + strlen((STRPTR) dataptr);

				d1(kprintf("%s/%s/%ld:  DefaultTool=<%s>\n", __FILE__, __FUNC__, __LINE__, dataptr));

				if (dtlength > 1)
					{
					inst->id_DefaultTool = MyAllocVecPooled(1 + strlen((STRPTR) dataptr));
					if (inst->id_DefaultTool)
						strcpy(inst->id_DefaultTool, (STRPTR) dataptr);
					}

				dataptr = (ULONG *) ((UBYTE *) dataptr + dtlength);
				Length -= dtlength;
				if (!inst->id_TypeSet)
					inst->id_Type = WBPROJECT;
				break;

			case PNGICONA_TOOLWINDOW:
				dtlength = 1 + strlen((STRPTR) dataptr);

				d1(KPrintF("%s/%s/%ld:  do_ToolWindow=<%s>\n", __FILE__, __FUNC__, __LINE__, dataptr));

				if (dtlength > 1)
					{
					inst->id_ToolWindow = MyAllocVecPooled(1 + strlen((STRPTR) dataptr));
					if (inst->id_ToolWindow)
						strcpy(inst->id_ToolWindow, (STRPTR) dataptr);
					}

				dataptr = (ULONG *) ((UBYTE *) dataptr + dtlength);
				Length -= dtlength;
				if (!inst->id_TypeSet)
					inst->id_Type = WBPROJECT;
				break;

			case PNGICONA_TOOLTYPE:
				ttlength = 1 + strlen((STRPTR) dataptr);
				if (ttlength > 1)
					{
					struct TTNode *ttnode;

					d1(kprintf("%s/%s/%ld:  ToolType=<%s>\n", __FILE__, __FUNC__, __LINE__, dataptr));

					ttnode = MyAllocVecPooled(sizeof(struct TTNode) + 1 + ttlength);
					if (ttnode)
						{
						ttTotalLength += ttlength;
						ttCount++;

						AddTail(&inst->id_ToolTypesList, &ttnode->Node);
						ttnode->Length = ttlength;
						strcpy(ttnode->ToolType, (STRPTR) dataptr);
						inst->id_ToolTypesCount++;
						}
					}
				dataptr = (ULONG *) ((UBYTE *) dataptr + ttlength);
				Length -= ttlength;
				break;

			case PNGICONA_TYPE:
				inst->id_Type = SCA_BE2LONG(*dataptr);
				inst->id_TypeSet = TRUE;
				d1(KPrintF("%s/%s/%ld:  Icon type=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));
				dataptr++;
				Length -= sizeof(ULONG);
				break;

			case PNGICONA_SORTMODE:
				// only change dd_ViewMode if not set by PNGICONA_DRAWERVIEWMODE
				d1(KPrintF("%s/%s/%ld:  PNGICONA_SORTMODE=%08lx\n", __FILE__, __FUNC__, __LINE__, *dataptr));
				if (DDVM_BYDEFAULT == inst->id_DrawerData.dd_ViewModes)
					inst->id_DrawerData.dd_ViewModes = DDVM_BYNAME + SCA_BE2LONG(*dataptr);
				d1(KPrintF("%s/%s/%ld:  dd_ViewModes=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_ViewModes));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;

			case PNGICONA_OFFSETX:
				inst->id_DrawerData.dd_CurrentX = SCA_BE2LONG(*dataptr);
				d1(KPrintF("%s/%s/%ld:  dd_CurrentX=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_CurrentX));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;

			case PNGICONA_OFFSETY:
				inst->id_DrawerData.dd_CurrentY = SCA_BE2LONG(*dataptr);
				d1(KPrintF("%s/%s/%ld:  dd_CurrentY=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_CurrentY));
				dataptr++;
				Length -= sizeof(ULONG);
				GuessWbDrawer(inst);
				break;

			default:
				// Let's assume that all still unknown tags have ULONG data...
				d1(KPrintF("%s/%s/%ld:  tag=%08lx  dataptr=%08lx  Length=%ld\n", __FILE__, __FUNC__, __LINE__, tag, dataptr, Length));
				dataptr++;
				Length -= sizeof(ULONG);
				break;
				}
			}

		// Build ToolTypes array from ttlist. We'll put STRPTRS and strings in the same block.
		inst->id_ToolTypesLength = 0;
		if (ttTotalLength)
			{
			inst->id_ToolTypesLength = (ttCount + 1) * sizeof(STRPTR) + ttTotalLength;
			inst->id_ToolTypes = (STRPTR *) MyAllocVecPooled(inst->id_ToolTypesLength);
			if (inst->id_ToolTypes)
				{
				struct TTNode *ttnode;
				char *tts;

				inst->id_ToolTypes[ttCount] = NULL;
				tts = (char *) (&inst->id_ToolTypes[ttCount + 1]);

				while ((ttnode = (struct TTNode *) RemTail(&inst->id_ToolTypesList)))
					{
					inst->id_ToolTypes[--ttCount] = tts;

					strcpy(tts, ttnode->ToolType);
					tts += 1 + strlen(ttnode->ToolType);

					MyFreeVecPooled(ttnode);
					}
				}
			}

		inst->id_IconHunkLoaded = TRUE;
		}

	d1(KPrintF("%s/%s/%ld:  id_Type=%lu\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));
	d1(KPrintF("%s/%s/%ld:  dd_ViewModes=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->id_DrawerData.dd_ViewModes));
}


static void DetermineIconType(struct InstanceData *inst, CONST_STRPTR filename, 
	const struct WBArg *ObjectLocation, struct FileInfoBlock *fib)
{
	if (!inst->id_TypeSet)
		{
		BPTR fLock;

		d1(KPrintF("%s/%s/%ld:  filename=<%s>\n", __FILE__, __FUNC__, __LINE__, filename));

		inst->id_Type = WBTOOL;
		inst->id_DirEntryType = 0;

		if (ObjectLocation)
			{
			BPTR oldDir = CurrentDir(ObjectLocation->wa_Lock);

			d1(KPrintF("%s/%s/%ld:  ObjectLocation=%08lx  Lock=%08lx  Name=<%s>\n", \
				__FILE__, __FUNC__, __LINE__, ObjectLocation, ObjectLocation->wa_Lock, ObjectLocation->wa_Name));

			fLock = Lock(ObjectLocation->wa_Name, ACCESS_READ);
			CurrentDir(oldDir);
			}
		else
			{
			d1(KPrintF("%s/%s/%ld:  filename=<%s>\n", __FILE__, __FUNC__, __LINE__, filename));
			fLock = Lock(filename, ACCESS_READ);

			if ((BPTR)NULL == fLock && ERROR_OBJECT_IN_USE == IoErr())
				{
				Delay(10);
				fLock = Lock(filename, ACCESS_READ);
				}
			}


		d1(KPrintF("%s/%s/%ld:  fLock=%08lx\n", __FILE__, __FUNC__, __LINE__, fLock));
		if (fLock)
			{
			if (Examine(fLock, fib))
				{
				d1(KPrintF("%s/%s/%ld:  Examine() success <%s>   DirEntryType=%ld  Protection=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, fib->fib_FileName, \
                                        fib->fib_DirEntryType, fib->fib_Protection));

				if (fib->fib_DirEntryType > 0)
					inst->id_Type = WBDRAWER;
				else if (!(fib->fib_Protection & FIBF_EXECUTE))
					inst->id_Type = WBTOOL; // RWED are 0=on 1=off !
				else
					inst->id_Type = WBPROJECT;
				}
			UnLock(fLock);
			}
		else
			{
			// No object associated. May be a "disk.info" ?
			d1(KPrintF("%s/%s/%ld:  IoErr=%ld\n", __FILE__, __FUNC__, __LINE__, IoErr()));

			if (0 == Stricmp(filename, "disk"))
				inst->id_Type = WBDISK;
			}
		}

	d1(KPrintF("%s/%s/%ld:  id_Type=%ld\n", __FILE__, __FUNC__, __LINE__, inst->id_Type));
}


static void GuessWbDrawer(struct InstanceData *inst)
{
	if (!inst->id_TypeSet && inst->id_Type != WBDISK && inst->id_DirEntryType >= 0)
		inst->id_Type = WBDRAWER;
}



