/*
 * fileio
 *
 *  the basic routines to convert 'pages' out to 'images' for the jpnm library
 *  in the future any other file io routines will be in here as well.
 *  ie:
 *	loading / saving palettes
 *	loading / saving animations
 *	calling registered callbacks if another filetype is desired
 *	perhaps we also will use the Image Magick library here...
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsui.h"
#include "fileio.h"
#include "iffio.h"

char bakbase[] = BACKUP_PIC_NAME;

#ifdef NEVER_EVER
IPAGE *
ppm_file_load_page(
	char * filename
)
{
    // eventually move this out into a plugin.

    IMAGE * newimage = NULL;
    IPAGE * newpage = NULL;
    int bpos;
    int wh;

    if (!filename)  return NULL;

    newimage = Image_Load_PNM(filename);
    if (!newimage)
    {
	return NULL;
    }

    // now, we need to convert the 'image' to a 'page'
    newpage = Page_Create(newimage->width, newimage->height, newimage->ncolors);

    if (!newpage)
    {
	Image_Destroy(newimage);
	return NULL;
    }

    wh = newpage->w * newpage->h;
    if (newimage->ncolors > 0)
    {
	// paletted / indexed color image.
	for (bpos = 0 ; bpos < wh ; bpos++)
	{
	    newpage->pimage[bpos] = newimage->data[bpos].r;
	}

	for (bpos = 0 ; bpos < newimage->ncolors ; bpos++)
	{
	    newpage->palette[bpos].r = newimage->palette[bpos].r;
	    newpage->palette[bpos].g = newimage->palette[bpos].g;
	    newpage->palette[bpos].b = newimage->palette[bpos].b;
	}

    } else {
	// truecolor image
	for (bpos = 0 ; bpos < wh ; bpos++)
	{
	    newpage->timage[bpos].r = newimage->data[bpos].r;
	    newpage->timage[bpos].g = newimage->data[bpos].g;
	    newpage->timage[bpos].b = newimage->data[bpos].b;
	}

    }

    Image_Destroy(newimage);
    return(newpage);
}


void
ppm_file_save_page(
    IPAGE * page,
    char * filename
)
{
    // eventually move this out into a plugin.
    int bpos;

    IMAGE * newimage;

    if (!page || !filename) return;

    newimage = Image_Create(page->w, page->h, 255);

    for (bpos = 0 ; bpos < page->w * page->h ; bpos++)
    {
	newimage->data[bpos].r = 
		page->palette[page->pimage[bpos]].r;

	newimage->data[bpos].g = 
		page->palette[page->pimage[bpos]].g;

	newimage->data[bpos].b = 
		page->palette[page->pimage[bpos]].b;

    }

    Image_Save_PNM(filename, newimage, SAVE_ASCII);
    Image_Destroy(newimage);
}
#endif

////////////////////////////////////////////////////////////////////////////////


IPAGE * 
file_load_page(
	char * filename
)
{
    if (file_is_iff(filename))
    {
	return( iff_file_load_page(filename) );
    }

    return NULL;

//    return( ppm_file_load_page(filename) );
}



void
file_backup(
	char * filename
)
{
    int pos, bytesread;
    char fbuf[1024];
    FILE * inf;
    FILE * otf;
    char * bakname = (char *) malloc (strlen(filename) + strlen(bakbase) +1);

    bakname = strcpy(bakname, filename);

    for (pos = strlen(bakname) ; pos > 0 && bakname[pos] != '/' ; pos--);

    if (bakname[pos] == '/')
    {
	bakname[pos+1] = '\0';
    } else {
	bakname[pos] = '\0';
    }

    bakname = strcat(bakname, bakbase);

    inf = fopen (filename, "r");
    if (inf != NULL)
    {
	otf = fopen (bakname, "w");
	do
	{
	    bytesread = fread(fbuf, 1, 1024, inf);
	    if (bytesread) fwrite(fbuf, 1, bytesread, otf);
	} while (bytesread);

	fclose(otf);
	fclose(inf);
    }

    free(bakname);
}

void
c_file_save_page(IPAGE * page, char * filename)
{
    FILE * fp;
    int linepos = 100;
    int xx;

    fp = fopen( filename, "w" );
    if (!fp) return;

    fprintf(fp, "int buffer_w = %d;\n", page->w);
    fprintf(fp, "int buffer_h = %d;\n", page->h);
    fprintf(fp, "int buffer_data[] = {");
    
    linepos = page->w + 3;

    for (xx=0 ; xx< page->h * page->w ; xx++)
    {
	if( linepos > 20)
	{
	    linepos = 0;
	    fprintf(fp, "\n    ");
	}
	fprintf(fp, "%d", page->pimage[xx]);
	if ( xx != (page->h * page->w) -1)
	    fprintf(fp, ", ");
	linepos++;
    }
    fprintf(fp, "\n};\n");
    fclose(fp);
}

void
file_save_page(
	IPAGE * page,
	char * filename
)
{
    if (!page || !filename) return;

    file_backup(filename);
    
    c_file_save_page(page, filename);
    //iff_file_save_page(page, filename);
    //ppm_file_save_page(page, filename);
}

