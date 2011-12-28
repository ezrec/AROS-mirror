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

#include "includes.h" /* This must be included for XtoI.h */
#include "XtoI.h" /* This must be included before HTML.h */
#include "HTML.h"
#include "mosaic.h"


#define DEF_BLACK       BlackPixel(dsp, DefaultScreen(dsp))
#define DEF_WHITE       WhitePixel(dsp, DefaultScreen(dsp))
#define	MAX_LINE	81


extern unsigned char *ReadGIF();
extern unsigned char *ReadXpm3Pixmap();


unsigned char nibMask[8] = {
	1, 2, 4, 8, 16, 32, 64, 128
};



unsigned char *ReadXbmBitmap(FILE *fp, char *datafile, int *w, int *h,
			     int *colrs)
{
	char line[MAX_LINE], name_and_type[MAX_LINE];
	char *t;
	char *t2;
	unsigned char *ptr, *dataP;
	int bytes_per_line, version10p = 0, raster_length, padding;
	int i, bytes, temp, value;
	int Ncolors, charspp, xpmformat;
//        extern XColor fg_color, bg_color;
//        extern Widget view;
//        extern int Vclass;

	*w = 0;
	*h = 0;
	Ncolors = 0;
	charspp = 0;
	xpmformat = 0;
	for ( ; ; )
	{
		if (!(fgets(line, MAX_LINE, fp)))
			break;
		if (strlen(line) == (MAX_LINE - 1))
		{
			return((unsigned char *)NULL);
		}
		if (sscanf(line, "#define %s %d", name_and_type, &value) == 2)
		{
			if (!(t = strrchr(name_and_type, '_')))
				t = name_and_type;
			else
				t++;
			if (!strcmp("width", t))
				*w= value;
			if (!strcmp("height", t))
				*h= value;
			if (!strcmp("ncolors", t))
				Ncolors = value;
			if (!strcmp("pixel", t))
				charspp = value;
			continue;
		}
		if (sscanf(line, "static short %s = {", name_and_type) == 1)
		{
			version10p = 1;
			break;
		}
		else if (sscanf(line,"static char * %s = {",name_and_type) == 1)
		{
			xpmformat = 1;
			if (!(t = strrchr(name_and_type, '_')))
				t = name_and_type;
			else
				t++;
			if ((t2 = strchr(name_and_type, '[')) != NULL)
				*t2 = '\0';
			if (!strcmp("mono", t))
				continue;
			else
				break;
		}
		else if (sscanf(line, "static char %s = {", name_and_type) == 1)
		{
			version10p = 0;
			break;
		}
		else if (sscanf(line, "static unsigned char %s = {", name_and_type) == 1)
		{
			version10p = 0;
			break;
		}
		else
			continue;
	}
	if (xpmformat)
	{
#ifdef __AROS__
		(void)Ncolors;
		(void)charspp;
		return NULL;
#else
		dataP = ReadXpmPixmap(fp, datafile, w, h, colrs, Ncolors, charspp);
		return(dataP);
#endif
	}
	if (*w == 0)
	{
		/* fprintf(stderr, "Can't read image.\n"); */
		return((unsigned char *)NULL);
	}
	if (*h == 0)
	{
		/* fprintf(stderr, "Can't read image.\n"); */
		return((unsigned char *)NULL);
	}
	padding = 0;
	if (((*w % 16) >= 1)&&((*w % 16) <= 8)&&version10p)
	{
		padding = 1;
	}
	bytes_per_line = ((*w + 7) / 8) + padding;
	raster_length =  bytes_per_line * *h;
	dataP = (unsigned char *)malloc((*w) * (*h));
	if (dataP == NULL)
	{
		fprintf(stderr, "Not enough memory.\n");
		return((unsigned char *)NULL);
	}
	ptr = dataP;
	if (version10p)
	{
		int cnt = 0;
		int lim = (bytes_per_line - padding) * 8;
		for (bytes = 0; bytes < raster_length; bytes += 2)
		{
			if (fscanf(fp, " 0x%x%*[,}]%*[ \r\n]", &value) != 1)
			{
				fprintf(stderr, "Error scanning bits item.\n");
				return((unsigned char *)NULL);
			}
			temp = value;
			value = temp & 0xff;
			for (i = 0; i < 8; i++)
			{
				if (cnt < (*w))
				{
					if (value & nibMask[i])
						*ptr++ = 1;
					else
						*ptr++ = 0;
				}
				if (++cnt >= lim)
					cnt = 0;
			}
			if ((!padding)||((bytes+2) % bytes_per_line))
			{
				value = temp >> 8;
				for (i = 0; i < 8; i++)
				{
					if (cnt < (*w))
					{
						if (value & nibMask[i])
							*ptr++ = 1;
						else
							*ptr++ = 0;
					}
					if (++cnt >= lim)
						cnt = 0;
				}
			}
		}
	}
	else
	{
		for (bytes = 0; bytes < raster_length; bytes++)
		{
			if (fscanf(fp, " 0x%x%*[,}]%*[ \r\n]", &value) != 1)
			{
				fprintf(stderr, "Error scanning bits item.\n");
				return((unsigned char *)NULL);
			}
			*ptr++ = value;
		}
	}
	return(dataP);
}


