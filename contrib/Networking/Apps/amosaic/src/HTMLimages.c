// MDF - PORT FROM NCSA VERSION 2.1

/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#if defined(_AMIGA) || defined(__AROS__)
#include "includes.h"
#include <exec/types.h>
#include "globals.h" /* MUIMasterBase */
#include <stdlib.h>
#include <string.h>
#include <intuition/intuitionbase.h>
#include <clib/graphics_protos.h>
#include <graphics/gfxmacros.h>
#include "XtoI.h"
#endif

#include <stdio.h>
#include <ctype.h>
/* None of these functions can be called outside the html widget */
#include "HTMLP.h"
#include "NoImage.xbm"
#include "DelayedImage.xbm"
#include "AnchoredImage.xbm"


#define IMAGE_BORDER	1


ImageInfo no_image;
ImageInfo delayed_image;
ImageInfo anchored_image;

#if !defined(_AMIGA) && !defined(__AROS__)
static int allocation_index[256];
#endif

/* MDF - Special function to create an Amiga BitMap from some bits in
   a header file.  The header file must first be converted from XBM format
   to an Amiga compatible format! */
Pixmap CreateBitMapFromData(UBYTE *bits, int width, int height, int depth)
{
  struct BitMap *bm;
  PLANEPTR p, s;
  int i;

  bm = AllocBitMap(width, height, depth, BMF_CLEAR, NULL);

  p = bm->Planes[0];
  s = bits;

  for (i = 0; i < bm->BytesPerRow * height; i++) {
    *p++ = *s++;
  }

  return bm;
}


Pixmap CreateBitMapFromXBMData(UBYTE *bits, int width, int height, int depth)
{
  struct BitMap *bm;
  PLANEPTR d, s;
  int i, j, z1, z2;
  int bytes_per_line;
  UBYTE b1, b2;

  bm = AllocBitMap(width, height, depth, BMF_CLEAR, NULL);

  d = bm->Planes[0];
  s = bits;

  bytes_per_line = ((width + 7) / 8);

  for (i=0; i<height; i++) {
    for (j=0; j<bytes_per_line; j++) {
      b1 = *s++;
      b2 = 0;
      for (z1 = 128, z2 = 1; z2 < 250; z1 /= 2, z2 *= 2) {
	if (b1 >= z1) {
	  b1 = b1 - z1;
	  b2 = b2 + z2;
	}	
      }
      *d++ = b2;
    }
    if ((bytes_per_line) != bm->BytesPerRow) *d++ = 0;
  }
  return bm;
}


/*
 * Free all the colors in the default colormap that we have allocated so far.
 */
void
FreeColors(void *dsp, void *colormap)
{
#if !defined(_AMIGA) && !defined(__AROS__)
	int i, j;
	unsigned long pix;

	for (i=0; i<256; i++)
	{
		if (allocation_index[i])
		{
			pix = (unsigned long)i;
			/*
			 * Because X is stupid, we have to Free the color
			 * once for each time we've allocated it.
			 */
			for (j=0; j<allocation_index[i]; j++)
			{
				XFreeColors(dsp, colormap, &pix, 1, 0L);
			}
		}
		allocation_index[i] = 0;
	}
#endif /* _AMIGA */
}


/*
 * Free up all the pixmaps allocated for this document.
 */
void
FreeImages(HTMLGadClData * hw)
{
	struct ele_rec *eptr;

	eptr = hw->formatted_elements;
	while (eptr != NULL)
	{
		if ((eptr->type == E_IMAGE)&&(eptr->pic_data != NULL))
		{
			/*
			 * Don't free the no_image default image
			 */
			if ((eptr->pic_data->image != (Pixmap)NULL)&&
			    (eptr->pic_data->image != delayed_image.image)&&
			    (eptr->pic_data->image != anchored_image.image)&&
			    (eptr->pic_data->image != no_image.image))
			{
				/*
				 * Don't free internal images
				 */
				if ((eptr->edata != NULL)&&
					(strncmp(eptr->edata, INTERNAL_IMAGE,
					strlen(INTERNAL_IMAGE)) == 0))
				{
				}
				else
				{
				  WaitBlit();
				  FreeBitMap(eptr->pic_data->image);
//					XFreePixmap(XtDisplay(hw),
//						eptr->pic_data->image);
					eptr->pic_data->image = (Pixmap)NULL;
				}
			}
		}
		eptr = eptr->next;
	}
}


#if !defined(_AMIGA) && !defined(__AROS__)
/*
 * Find the closest color by allocating it, or picking an already allocated
 * color
 */
void
FindColor(dsp, colormap, colr)
	Display *dsp;
	Colormap colormap;
	XColor *colr;
{
	int i, match;
#ifdef MORE_ACCURATE
	double rd, gd, bd, dist, mindist;
#else
	int rd, gd, bd, dist, mindist;
#endif /* MORE_ACCURATE */
	int cindx;
static XColor def_colrs[256];
static int have_colors = 0;
	int NumCells;

	match = XAllocColor(dsp, colormap, colr);
	if (match == 0)
	{
		NumCells = DisplayCells(dsp, DefaultScreen(dsp));
		if (!have_colors)
		{
			for (i=0; i<NumCells; i++)
			{
				def_colrs[i].pixel = i;
			}
			XQueryColors(dsp, colormap, def_colrs, NumCells);
			have_colors = 1;
		}
#ifdef MORE_ACCURATE
		mindist = 196608.0;		/* 256.0 * 256.0 * 3.0 */
		cindx = colr->pixel;
		for (i=0; i<NumCells; i++)
		{
			rd = (def_colrs[i].red - colr->red) / 256.0;
			gd = (def_colrs[i].green - colr->green) / 256.0;
			bd = (def_colrs[i].blue - colr->blue) / 256.0;
			dist = (rd * rd) +
				(gd * gd) +
				(bd * bd);
			if (dist < mindist)
			{
				mindist = dist;
				cindx = def_colrs[i].pixel;
				if (dist == 0.0)
				{
					break;
				}
			}
		}
#else
		mindist = 196608;		/* 256 * 256 * 3 */
		cindx = colr->pixel;
		for (i=0; i<NumCells; i++)
		{
			rd = ((int)(def_colrs[i].red >> 8) -
				(int)(colr->red >> 8));
			gd = ((int)(def_colrs[i].green >> 8) -
				(int)(colr->green >> 8));
			bd = ((int)(def_colrs[i].blue >> 8) -
				(int)(colr->blue >> 8));
			dist = (rd * rd) +
				(gd * gd) +
				(bd * bd);
			if (dist < mindist)
			{
				mindist = dist;
				cindx = def_colrs[i].pixel;
				if (dist == 0)
				{
					break;
				}
			}
		}
#endif /* MORE_ACCURATE */
		colr->pixel = cindx;
		colr->red = def_colrs[cindx].red;
		colr->green = def_colrs[cindx].green;
		colr->blue = def_colrs[cindx].blue;
	}
	else
	{
		/*
		 * Keep a count of how many times we have allocated the
		 * same color, so we can properly free them later.
		 */
		allocation_index[colr->pixel]++;

		/*
		 * If this is a new color, we've actually changed the default
		 * colormap, and may have to re-query it later.
		 */
		if (allocation_index[colr->pixel] == 1)
		{
			have_colors = 0;
		}
	}
}
#endif /* _AMIGA */


#if !defined(_AMIGA) && !defined(__AROS__)
/*
 * Make am image of appropriate depth for display from image data.
 */
XImage *
MakeImage(dsp, data, width, height, depth, img_info)
	Display *dsp;
	unsigned char *data;
	int width, height;
	int depth;
	ImageInfo *img_info;
{
	int linepad, shiftnum;
	int shiftstart, shiftstop, shiftinc;
	int bytesperline;
	int temp;
	int w, h;
	XImage *newimage;
	unsigned char *bit_data, *bitp, *datap;

	switch(depth)
	{
	    case 6:
	    case 8:
		bit_data = (unsigned char *)malloc(width * height);
		bcopy(data, bit_data, (width * height));
		bytesperline = width;
		newimage = XCreateImage(dsp,
			DefaultVisual(dsp, DefaultScreen(dsp)),
			depth, ZPixmap, 0, (char *)bit_data,
			width, height, 8, bytesperline);
		break;
	    case 1:
	    case 2:
	    case 4:
		if (BitmapBitOrder(dsp) == LSBFirst)
		{
			shiftstart = 0;
			shiftstop = 8;
			shiftinc = depth;
		}
		else
		{
			shiftstart = 8 - depth;
			shiftstop = -depth;
			shiftinc = -depth;
		}
		linepad = 8 - (width % 8);
		bit_data = (unsigned char *)malloc(((width + linepad) * height)
				+ 1);
		bitp = bit_data;
		datap = data;
		*bitp = 0;
		shiftnum = shiftstart;
		for (h=0; h<height; h++)
		{
			for (w=0; w<width; w++)
			{
				temp = *datap++ << shiftnum;
				*bitp = *bitp | temp;
				shiftnum = shiftnum + shiftinc;
				if (shiftnum == shiftstop)
				{
					shiftnum = shiftstart;
					bitp++;
					*bitp = 0;
				}
			}
			for (w=0; w<linepad; w++)
			{
				shiftnum = shiftnum + shiftinc;
				if (shiftnum == shiftstop)
				{
					shiftnum = shiftstart;
					bitp++;
					*bitp = 0;
				}
			}
		}
		bytesperline = (width + linepad) * depth / 8;
		newimage = XCreateImage(dsp,
			DefaultVisual(dsp, DefaultScreen(dsp)),
			depth, ZPixmap, 0, (char *)bit_data,
			(width + linepad), height, 8, bytesperline);
		break;
	    /*
	     * WARNING:  This depth 16 code is donated code for 16 but
	     * TrueColor displays.  I have no access to such displays, so I
	     * can't really test it.
	     * Donated by - andrew@icarus.demon.co.uk
	     */
	    case 16:
		bit_data = (unsigned char *)malloc(width * height * 2);
		bitp = bit_data;
		datap = data;
		for (w = (width * height); w > 0; w--)
		{
			temp = (((img_info->reds[(int)*datap] >> 1)& 0x7c00) |
				((img_info->greens[(int)*datap] >> 6)& 0x03e0) |
				((img_info->blues[(int)*datap] >> 11)& 0x001f));

			if (BitmapBitOrder(dsp) == MSBFirst)
			{
				*bitp++ = (temp >> 8) & 0xff;
				*bitp++ = temp & 0xff;
			}
			else
			{
				*bitp++ = temp & 0xff;
				*bitp++ = (temp >> 8) & 0xff;
			}

			datap++;
		}

		newimage = XCreateImage(dsp,
			DefaultVisual(dsp, DefaultScreen(dsp)),
			depth, ZPixmap, 0, (char *)bit_data,
			width, height, 16, 0);
		break;
	    case 24:
		bit_data = (unsigned char *)malloc(width * height * 4);

		if (BitmapBitOrder(dsp) == MSBFirst)
		{
			bitp = bit_data;
			datap = data;
			for (w = (width * height); w > 0; w--)
			{
				*bitp++ = (unsigned char)0;
				*bitp++ = (unsigned char)
					((img_info->blues[(int)*datap]>>8) &
					0xff);
				*bitp++ = (unsigned char)
					((img_info->greens[(int)*datap]>>8) &
					0xff);
				*bitp++ = (unsigned char)
					((img_info->reds[(int)*datap]>>8) &
					0xff);
				datap++;
			}
		}
		else
		{
			bitp = bit_data;
			datap = data;
			for (w = (width * height); w > 0; w--)
			{
				*bitp++ = (unsigned char)
					((img_info->reds[(int)*datap]>>8) &
					0xff);
				*bitp++ = (unsigned char)
					((img_info->greens[(int)*datap]>>8) &
					0xff);
				*bitp++ = (unsigned char)
					((img_info->blues[(int)*datap]>>8) &
					0xff);
				*bitp++ = (unsigned char)0;
				datap++;
			}
		}

		newimage = XCreateImage(dsp,
			DefaultVisual(dsp, DefaultScreen(dsp)),
			depth, ZPixmap, 0, (char *)bit_data,
			width, height, 32, 0);
		break;
	    default:
		fprintf(stderr, "Don't know how to format image for display of depth %d\n", depth);
		return(NULL);
	}

	return(newimage);
}
#endif /* _AMIGA */


int
AnchoredHeight(HTMLGadClData * hw)
{
	return((int)(AnchoredImage_height + IMAGE_BORDER));
}


char *
IsMapForm(HTMLGadClData * hw)
{
	char *str;

	str = (char *)malloc(strlen("ISMAP Form") + 1);
	if (str != NULL)
	{
		strcpy(str, "ISMAP Form");
	}
	return(str);
}


int
IsIsMapForm(HTMLGadClData * hw, char *href)
{
	if ((href != NULL)&&(strcmp(href, "ISMAP Form") == 0))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}


char *
DelayedHRef(HTMLGadClData * hw)
{
	char *str;

	str = (char *)malloc(strlen("Delayed Image") + 1);
	if (str != NULL)
	{
		strcpy(str, "Delayed Image");
	}
	return(str);
}


int
IsDelayedHRef(HTMLGadClData * hw, char *href)
{
	if ((href != NULL)&&(strcmp(href, "Delayed Image") == 0))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}


/* MDF - Heavily hacked */
Pixmap
DelayedImage(HTMLGadClData * hw, Boolean anchored)
{
	if (delayed_image.image == (Pixmap)NULL)
	{
	        delayed_image.image = CreateBitMapFromData(DelayedImage_bits,
				  DelayedImage_width,
				  DelayedImage_height,
				  hw->rp->BitMap->Depth);
	}

	if ((anchored == True)&&(anchored_image.image == (Pixmap)NULL))
	{
		Pixmap pix;

	        anchored_image.image = CreateBitMapFromXBMData(AnchoredImage_bits,
				  AnchoredImage_width,
				  AnchoredImage_height,
				  hw->rp->BitMap->Depth);
		pix = AllocBitMap(DelayedImage_width,
				  DelayedImage_height + AnchoredImage_height +
				      IMAGE_BORDER,
				  hw->rp->BitMap->Depth, BMF_CLEAR, NULL);
		BltBitMap(anchored_image.image, 0, 0,
			  pix, 0, 0,
			  AnchoredImage_width, AnchoredImage_height,
			  0xc0, 0xff, NULL);
		BltBitMap(delayed_image.image, 0, 0,
			  pix, 0, AnchoredImage_height + IMAGE_BORDER,
			  DelayedImage_width, DelayedImage_height,
			  0xc0, 0xff, NULL);
		FreeBitMap(anchored_image.image);

		anchored_image.image = pix;

		return(anchored_image.image);
	}

	return(delayed_image.image);
}


ImageInfo *
DelayedImageData(HTMLGadClData * hw, Boolean anchored)
{
	delayed_image.delayed = 1;
	delayed_image.internal = 0;
	delayed_image.fetched = 0;
	delayed_image.width = DelayedImage_width;
	delayed_image.height = DelayedImage_height;
	delayed_image.num_colors = 0;
	delayed_image.reds = NULL;
	delayed_image.greens = NULL;
	delayed_image.blues = NULL;
	delayed_image.image_data = NULL;
	delayed_image.image = (Pixmap)NULL;

	if (anchored == True)
	{
		anchored_image.delayed = 0;
		anchored_image.internal = 0;
		anchored_image.fetched = 0;
		anchored_image.width = DelayedImage_width;
		anchored_image.height = DelayedImage_height +
			AnchoredImage_height + IMAGE_BORDER;
		anchored_image.num_colors = 0;
		anchored_image.reds = NULL;
		anchored_image.greens = NULL;
		anchored_image.blues = NULL;
		anchored_image.image_data = NULL;
		anchored_image.image = (Pixmap)NULL;

		return(&anchored_image);
	}

	return(&delayed_image);
}


Pixmap
NoImage(HTMLGadClData * hw)
{
	if (no_image.image == (Pixmap)NULL)
	{
	  no_image.image = CreateBitMapFromData(NoImage_bits,
				  NoImage_width,
				  NoImage_height,
				  hw->rp->BitMap->Depth);
	}
	return(no_image.image);
}


ImageInfo *
NoImageData(HTMLGadClData * hw)
{
	no_image.delayed = 0;
	no_image.internal = 0;
	no_image.fetched = 0;
	no_image.width = NoImage_width;
	no_image.height = NoImage_height;
	no_image.num_colors = 0;
	no_image.reds = NULL;
	no_image.greens = NULL;
	no_image.blues = NULL;
	no_image.image_data = NULL;
	no_image.image = (Pixmap)NULL;

	return(&no_image);
}


Pixmap
InfoToImage(HTMLGadClData * hw, ImageInfo *img_info)
{
#if !defined(_AMIGA) && !defined(__AROS__)
	int i, size;
	Pixmap Img;
	XImage *tmpimage;
	XColor tmpcolr;
	int *Mapping;
	unsigned char *tmpdata;
	unsigned char *ptr;
	unsigned char *ptr2;
	int Vclass;
	XVisualInfo vinfo, *vptr;

	/* find the visual class. */
	vinfo.visualid = XVisualIDFromVisual(DefaultVisual(XtDisplay(hw),
		DefaultScreen(XtDisplay(hw))));
	vptr = XGetVisualInfo(XtDisplay(hw), VisualIDMask, &vinfo, &i);
	Vclass = vptr->class;
	XFree((char *)vptr);

	Mapping = (int *)malloc(img_info->num_colors * sizeof(int));

	for (i=0; i < img_info->num_colors; i++)
	{
		tmpcolr.red = img_info->reds[i];
		tmpcolr.green = img_info->greens[i];
		tmpcolr.blue = img_info->blues[i];
		tmpcolr.flags = DoRed|DoGreen|DoBlue;
		if (Vclass == TrueColor)
		{
			Mapping[i] = i;
		}
		else
		{
			FindColor(XtDisplay(hw),
				DefaultColormapOfScreen(XtScreen(hw)),
				&tmpcolr);
			Mapping[i] = tmpcolr.pixel;
		}
	}

	size = img_info->width * img_info->height;
	if (size == 0)
	{
		tmpdata = NULL;
	}
	else
	{
		tmpdata = (unsigned char *)malloc(size);
	}
	if (tmpdata == NULL)
	{
		tmpimage = NULL;
		Img = (Pixmap)NULL;
	}
	else
	{
		ptr = img_info->image_data;
		ptr2 = tmpdata;
		for (i=0; i < size; i++)
		{
			*ptr2++ = (unsigned char)Mapping[(int)*ptr];
			ptr++;
		}

		tmpimage = MakeImage(XtDisplay(hw), tmpdata,
			img_info->width, img_info->height,
			DefaultDepthOfScreen(XtScreen(hw)), img_info);

                /* Caught by Purify; should be OK. */
                free (tmpdata);

		Img = XCreatePixmap(XtDisplay(hw->view),
			XtWindow(hw->view),
			img_info->width, img_info->height,
			DefaultDepthOfScreen(XtScreen(hw)));
	}

	if ((tmpimage == NULL)||(Img == (Pixmap)NULL))
	{
		if (tmpimage != NULL)
		{
			XDestroyImage(tmpimage);
		}
		if (Img != (Pixmap)NULL)
		{
			XFreePixmap(XtDisplay(hw), Img);
		}
		img_info->width = NoImage_width;
		img_info->height = NoImage_height;
		Img = NoImage(hw);
	}
	else
	{
		XPutImage(XtDisplay(hw), Img, hw->drawGC, tmpimage, 0, 0,
			0, 0, img_info->width, img_info->height);
		XDestroyImage(tmpimage);
	}

        /* Caught by Purify; should be OK. */
        free((char *)Mapping);

	return(Img);
#endif /* _AMIGA */
	return NULL;
}

