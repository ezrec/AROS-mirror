/*********************************************************************
----------------------------------------------------------------------

	MysticView
	icons

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <intuition/intuition.h>
#include <guigfx/guigfx.h>
#include <render/render.h>
#include <exec/memory.h>

#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/render.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#ifndef NO_NEWICONS
#include <proto/newicon.h>
#endif

#include "Mystic_Global.h"
#include "Mystic_Tools.h"
#include "Mystic_Icons.h"

#include <assert.h>


/*********************************************************************
----------------------------------------------------------------------

	DeleteIconThumbnail(iconthumbnail)

----------------------------------------------------------------------
*********************************************************************/

void DeleteIconThumbnail(struct IconThumbnail *icon)
{
#ifndef NO_NEWICONS
	if (icon)
	{
		if (icon->dob)
		{
			icon->dob->ndo_NormalImage = icon->oldimg1;
			icon->dob->ndo_SelectedImage = icon->oldimg2;
		}

		Free(icon->array);
		Free(icon);
	}
#endif
}

/*********************************************************************
----------------------------------------------------------------------

	iconthumbnail = CreateIconThumbnail(picture, width, height, colors, diskobject)

----------------------------------------------------------------------
*********************************************************************/

struct IconThumbnail *CreateIconThumbnail(PICTURE *picture,
	int width, int height, int maxcolors,
	struct NewDiskObject *diskobject)
{
#ifdef NO_NEWICONS
    	return 0;
#else
	struct IconThumbnail *icon = NULL;

	if (picture && NewIconBase && diskobject)
	{
		BOOL success = FALSE;

		if (icon = Malloclear(sizeof(struct IconThumbnail)))
		{
			PICTURE *thumbpic;
			ULONG w, h, ax, ay;
			float f1, f2;

			GetPictureAttrs(picture, PICATTR_Width, &w, PICATTR_Height, &h,
				PICATTR_AspectX, &ax, PICATTR_AspectY, &ay, TAG_DONE);

			f1 = (float) width / (float) w * ((float)(ay?ay:1) / (float)(ax?ax:1));
			f2 = (float) height / (float) h;

			if (f1 < f2)
			{
				height = (int)((float) h * f1);
			}
			else
			{
				width = (int)((float) w * f2);
			}

			if (icon->array = Malloc(width * height))
			{
				if (thumbpic = ClonePicture(picture, GGFX_DestWidth, width, GGFX_DestHeight, height, TAG_DONE))
				{
					if (DoPictureMethod(thumbpic, PICMTHD_RENDER, PIXFMT_0RGB_32, TAG_DONE))
					{
						APTR histo;
						char pentab[256];
						int i;

						for (i = 0; i < 255; ++i)
						{
							pentab[i] = i + 1;
						}

						icon->img1.Width = width;
						icon->img1.Height = height;
						icon->img1.Flags = 0;
						icon->img1.Palette = icon->palette;

						if (histo = CreateHistogram(NULL))
						{
							APTR palette;
							if (palette = CreatePalette(NULL))
							{
								ULONG *array;
								GetPictureAttrs(thumbpic, PICATTR_RawData, &array, TAG_DONE);
								if (AddRGBImage(histo, array, width, height, NULL) == ADDH_SUCCESS)
								{
									int numcol;
									numcol = QueryHistogram(histo, RND_NumColors);
									numcol = MIN(numcol, maxcolors - 1);
									if (ExtractPalette(histo, palette, numcol, NULL) == EXTP_SUCCESS)
									{
										if (Render(array, width, height, icon->array, palette,
											RND_PenTable, pentab, RND_DitherMode, DITHERMODE_NONE,
											TAG_DONE) == REND_SUCCESS)
										{
											ULONG rgb;
											char *pp = &icon->palette[3];
											for (i = 0; i < numcol; ++i)
											{
												ExportPalette(palette, &rgb, RND_FirstColor, i, RND_NumColors, 1, TAG_DONE);
												*pp++ = (rgb >> 16) & 0xff;
												*pp++ = (rgb >> 8) & 0xff;
												*pp++ = rgb & 0xff;
											}
											icon->img1.NumColors = numcol + 1;
											success = TRUE;
										}
									}
								}
								DeletePalette(palette);
							}
							DeleteHistogram(histo);
						}
					}
					DeletePicture(thumbpic);
				}
			}

			if (success)
			{
				icon->img1.ChunkyData = icon->array;

				icon->oldimg1 = diskobject->ndo_NormalImage;
				icon->oldimg2 = diskobject->ndo_SelectedImage;
				diskobject->ndo_NormalImage = &icon->img1;
				diskobject->ndo_SelectedImage = NULL;

				icon->dob = diskobject;
			}
			else
			{
				DeleteIconThumbnail(icon);
				icon = NULL;
			}
		}
	}

	return icon;
#endif
}


/*********************************************************************
----------------------------------------------------------------------

	success = PutNewIcon(picture, filename, default_diskobject, deftool);

----------------------------------------------------------------------
*********************************************************************/

BOOL PutNewIcon(PICTURE *picture, int width, int height, int numcolors, char *filename, struct NewDiskObject *defaultdob, char *deftool)
{
#ifdef NO_NEWICONS
    	return FALSE;
#else

	BOOL success = FALSE;

	if (picture && filename && NewIconBase)
	{
		struct IconThumbnail *it;
		struct NewDiskObject *dob;
		struct NewDiskObject *newdob = NULL;

		if ((dob = defaultdob) == NULL)
		{
			if ((dob = newdob = GetNewDiskObject(filename)) == NULL)
			{
				dob = newdob = GetDefNewDiskObject(WBPROJECT);
			}
		}

		if (dob)
		{
			if (it = CreateIconThumbnail(picture, width, height, numcolors, dob))
			{
				char *olddeftool;
				olddeftool = it->dob->ndo_StdObject->do_DefaultTool;

				if (deftool)
				{
					if (strlen(deftool))
					{
						it->dob->ndo_StdObject->do_DefaultTool = deftool;
					}
				}

				success = PutNewDiskObject(filename, it->dob);

				it->dob->ndo_StdObject->do_DefaultTool = olddeftool;

				DeleteIconThumbnail(it);
			}
		}

		if (newdob)
		{
			FreeNewDiskObject(newdob);
		}
	}

	return success;
#endif
}
